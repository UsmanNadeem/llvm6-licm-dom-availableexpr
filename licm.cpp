// ECE 5984 S18 Assignment 2: available.cpp
// Group:
// Source: http://www.cs.cmu.edu/~15745/15745_assignment2/code/Dataflow/
////////////////////////////////////////////////////////////////////////////////

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"

#include <iostream>
#include <map>

#include "dataflow.h"
#include "dominator.cpp"
#include "available-support.h"

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ValueTracking.h"

#define FORWARD_DIRECTION 1

using namespace llvm;
using namespace std;

namespace {
  class LICM : public LoopPass {

  public:
    static char ID;
    set<Instruction*> invariantSet;
    set<Instruction*> canBeMovedSet;
    map<BasicBlock*, set<BasicBlock*>> dominatedBySet;
    map<BasicBlock*, BasicBlock*> immDomMap;

    LICM() : LoopPass(ID) { }
    
    bool isInvariant(Instruction* I) {
      if (isSafeToSpeculativelyExecute(I) && !I->mayReadFromMemory() && !isa<LandingPadInst>(I)) {
        return invariantSet.count(I);
      }

      return false;
    }

    bool instrDominatesBB(Instruction* I, BasicBlock* BB) {
      return dominatedBySet[BB].count(I->getParent());
    }

    bool instrUsedOutsideLoop(Instruction* I, Loop *loop) {
      for (User *U : I->users()) {
        if (Instruction *User = dyn_cast<Instruction>(U)) {
          // is used outside the loop
          if (!loop->getBlocksSet().count(User->getParent())) return true;
        }
      }

      return false;
    }

    bool canBeMoved(Instruction* I, Loop *loop) {
      /*
      Can be moved:
        if invariant
        if expr dominates all exits
          if does not dominate all exits then it must be dead after the loop
        if LHS has only one def inside the loop
          will be satisfied since it is SSA form
        if expr def dominates all uses inside the loop 
          my invariant checking code takes care of it if
          there are multiple reaching definitions to a use
          additionally SSA form will ensure that use is dom by def
      */

      // if this func is called then instruction is already an invariant
      // if (invariantSet.count(I)) {
      SmallVector<BasicBlock*, 5> ExitBlocks;
      loop->getUniqueExitBlocks(ExitBlocks);

      for (BasicBlock* exitBB : ExitBlocks) {
        if (!instrDominatesBB(I, exitBB)) {
          if (instrUsedOutsideLoop(I, loop)) return false;
        }
      }
      return true;
      // }
      // return false;
    }

    bool allDefsOutsideLoop (Instruction* I, Loop *loop) {
      //use-def analysis
      for (Use &U : I->operands()) {
        Value *v = U.get();
        if (Instruction* def = dyn_cast<Instruction>(v)) {
          if (loop->contains(def)) {
            return false;
          }
        }
      }
      return true;
    }

    void calculateInvariantSet(Loop *loop) {
    /*      
    Loop-Invariant:
      Operands have ALL reaching definitions outside the loop / operands are constants
      or 
      Operands are function of loop invariants / operands are constants

      If there is a reaching def of operand inside the loop then there should be EXACTLY one reaching def
          that definition must also be an invariant

      1. Reaching definitions at all entry points
          at instruction level
      For all blocks in the loop:
        check if operand has a reaching def outside/inside the loop
        add to loopInv Set
    */
    auto oldSize = invariantSet.size();

    do {
      oldSize = invariantSet.size();
      for (BasicBlock* BB : loop->getBlocks()) {
        for (Instruction& i : *BB) {
          Instruction* I = &i;
          if (!(isSafeToSpeculativelyExecute(I) && !I->mayReadFromMemory() && !isa<LandingPadInst>(I))) {
            continue;
          }

          if (PHINode* phiNode = dyn_cast<PHINode>(I)) {
            // either all values should come from outside the loop
            // or should have ONLY one value WHICH IS inside the loop
            //                                     or is a constant assignment
            if (allDefsOutsideLoop(phiNode, loop)) {
              invariantSet.insert(phiNode);
            } else if (Value* v = phiNode->hasConstantValue()) {
              if (Instruction* def = dyn_cast<Instruction>(v)) {
                if (loop->contains(def)) {
                  invariantSet.insert(phiNode);
                }
              } else if (isa<Constant>(v)) {
                  invariantSet.insert(phiNode);
              }
            }
          } else {
            // Each Operand should be 
            // either, a constant
            // or, have a def outside loop
            // or, have a def inside loop AND that def is an invariant

            bool isCandidate = true;


            for (Use &U : I->operands()) {
              Value *v = U.get();
              if (Instruction* def = dyn_cast<Instruction>(v)) {
                if (isa<Constant>(v)) continue;
                // if (!isa<Constant>(v) && loop->contains(def)) {
                  if (loop->contains(def) && !invariantSet.count(def)) {
                    isCandidate = false;
                    break;
                  }
                // }
              }
            }

            if (isCandidate) {
// if (I->getOpcode() == Instruction::SExt) outs() << "Instruction" << *I << "\n\n";
//             for (Use &U : I->operands()) {
//               Value *v = U.get();
//               if (Instruction* def = dyn_cast<Instruction>(v)) {
// if (I->getOpcode() == Instruction::SExt) outs() << "\tDef:" << *def << "\n";
// if (I->getOpcode() == Instruction::SExt) outs() << "\tisConstant Def:" << isa<Constant>(v) << "\n";
// if (I->getOpcode() == Instruction::SExt) outs() << "\tloop->contains(def):" << loop->contains(def) << "\n";
// if (I->getOpcode() == Instruction::SExt) outs() << "\tinvariantSet.count(def):" << invariantSet.count(def) << "\n";
//               }
//             }              
// if (I->getOpcode() == Instruction::SExt) outs() << "\tINSERTING " << *I << "\n";
// if (I->getOpcode() == Instruction::SExt) outs() << "Function " << I->getParent()->getParent()->getName() << "\n\n";
              invariantSet.insert(I);
            }
          }
        }
      }
    } while (oldSize != invariantSet.size());
    }

    virtual bool runOnLoop(Loop *loop, LPPassManager &LPM) {
      if (loop->getLoopPreheader() == NULL) {
        return false;
      }

      // reset them so that state is not caried over to the next loop
      invariantSet.clear();
      canBeMovedSet.clear();

      Function& F = * loop->getLoopPreheader()->getParent();
      DominatorAnalysis* DA = getAnalysisIfAvailable<DominatorAnalysis>();
      DA->runOnFunction(F);
      dominatedBySet = DA->getDominatedBySet();
      immDomMap = DA->getImmDomMap();

      outs() << "==================================================\n";
      outs() << "LICM::runOnLoop(); Function: " << F.getName() << "\n";
      outs() << "==================================================\n";
      for (BasicBlock* BB : loop->getBlocks()) {
        // errs() << *BB <<"\n";
        outs() << "BB: \"" << BB->getName() << "\"\nImmediate Dominator: \"";
        if (immDomMap.find(BB) != immDomMap.end()) {
          outs() << immDomMap[BB]->getName() << "\"\n\n";
        } else {
          outs() << "NO IMM DOM\"\n\n";
        }
      }
      outs() << "\n";

      calculateInvariantSet(loop);

      for (Instruction* I : invariantSet) {
        if (canBeMoved(I, loop)) {
          canBeMovedSet.insert(I);
        }
      }

      vector<pair<unsigned int, Instruction*>> sortedInstructions;

      for (Instruction* I : canBeMovedSet) {
        unsigned int numUses = 0;
        for (User *U : I->users()) {
          if (Instruction *User = dyn_cast<Instruction>(U)) {
            // is used by another invariant in the loop
            if (canBeMovedSet.count(User)) ++numUses;
          }
        }

        if (sortedInstructions.size() == 0) {
          sortedInstructions.push_back(make_pair(numUses, I));
        } else {
          for (auto i = sortedInstructions.begin(); i != sortedInstructions.end(); ++i) {
            if (numUses <= i->first) {
              sortedInstructions.insert(i, make_pair(numUses, I));
              break;
            } else if ((i+1) == sortedInstructions.end()) {
              sortedInstructions.push_back(make_pair(numUses, I));
              break;
            }
          }
        }
      }
      //   outs() << "\npreHeaderBranch: " << *loop->getLoopPreheader()->getTerminator() <<"\n";
      //   outs() << "\n";
      // for (Instruction* I : invariantSet)
      //   outs() << "inv: " << *I <<"\n";
      //   outs() << "\n";

      // for (auto& pair : sortedInstructions) {
      //   Instruction* I = pair.second;
      //   outs() << "moving: " << *I <<"\n";
      // }
      //   outs() << "\n\n";

      for (auto i = sortedInstructions.rbegin(); i != sortedInstructions.rend(); ++i) {
        Instruction* I = (*i).second;
        Instruction* preHeaderBranch = loop->getLoopPreheader()->getTerminator();
        I->moveBefore(preHeaderBranch);
      }

      return (sortedInstructions.size() > 0);
    }
    
    
    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      // AU.setPreservesAll();
      AU.addRequired<DominatorAnalysis>();
    }
    
  private:
  };
  
  char LICM::ID = 0;
  RegisterPass<LICM> X("loop-invariant-code-motion",
				       "LICM");
}
