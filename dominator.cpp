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
#include "available-support.h"

#define FORWARD_DIRECTION 1

using namespace llvm;
using namespace std;

namespace {
  class DominatorAnalysis : public FunctionPass, public Dataflow {


  public:
    static char ID;
    map<BasicBlock*, unsigned int> BBToPosMap;
    map<unsigned int, BasicBlock*> posToBBMap;
    map<BasicBlock*, set<BasicBlock*>> dominatedBySet;
    map<BasicBlock*, BasicBlock*> immDom;
    
    DominatorAnalysis() : Dataflow(FORWARD_DIRECTION), FunctionPass(ID) { }
    
    map<BasicBlock*, set<BasicBlock*>> getDominatedBySet() {
      return dominatedBySet;
    }

    map<BasicBlock*, BasicBlock*> getImmDomMap() {
      return immDom;
    }

    virtual bool runOnFunction(Function& F) {
      
      unsigned int i = 0;
      for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) {
        BasicBlock* block = &*FI;
        BBToPosMap[block] = i;
        posToBBMap.insert(pair<unsigned int, BasicBlock*>(i, block));
        ++i;
      }

      Dataflow::runOnFunction(F);
      
      // outs() << "***DominatorAnalysis Called for Function: "<< F.getName() <<"\n";
      // outs() << "==================================================\n";
      // outs() << "DominatorAnalysis Result for Function: "<< F.getName() <<"\n";
      // outs() << "==================================================\n\n";

      // Prepare the dominatedBySet which is used in LICM
      for (BasicBlock& bb : F) {
        BasicBlock* BB = &bb;
        BitVector& outVec = *((*out)[&bb]);

        // bb.printAsOperand(outs(), false);
        // outs() << " BB: #" << BBToPosMap[BB] << " \"" << bb.getName() << "\"\nDominated by " << outVec.count() << " block(s)\n";

        for (auto i : outVec.set_bits()) {
          // outs() << "\t#" << i << " \"" << posToBBMap[i]->getName()<< "\"\n";
          dominatedBySet[BB].insert(posToBBMap[i]);
        }

        // outs() << "\n";
      }

      // Prepare the isImmDom map which is required for Assignment output specs
      for (BasicBlock& bb : F) {
        BasicBlock* currBB = &bb;

        auto strictDomSet = dominatedBySet[currBB];
        strictDomSet.erase(currBB);

        for (auto i = strictDomSet.begin(); i != strictDomSet.end(); ++i) {
          BasicBlock* currDom = *i;
          bool isImmDom = true;
          auto j = i;
          ++j;
        // outs() << "BB: ";
        // currBB->printAsOperand(outs(), false);
        // outs() << " currDom: ";
        // currDom->printAsOperand(outs(), false);

          for (; j != strictDomSet.end(); ++j) {
        // outs() << " comparing with: ";
        // (*j)->printAsOperand(outs(), false);
        // outs() << "\n ";
            if (dominatedBySet[*j].count(currDom)) {
              isImmDom = false;
              break;
            }
          }
          // outs() << "\n";

          if (isImmDom) {
        // outs() << "BB: ";
        // currBB->printAsOperand(outs(), false);
        // outs() << "  " << currBB << "\n";
        // outs() << "Dom: ";
        // currDom->printAsOperand(outs(), false);
        // outs() << "  " << currDom << "\n";
            immDom[currBB] = currDom;
            break;
          }
        }

      }
      // Did not modify the incoming Function.
      return false;
    }
    
    virtual void initBitVectors(BasicBlock* bb) {
      (*in)[bb] = new BitVector(BBToPosMap.size(), true);
      (*out)[bb] = new BitVector(BBToPosMap.size(), true);
    }

    virtual void boundaryCond(BitVector* bitVec) {
      bitVec->reset();
      bitVec->set(0);
    }

    virtual void meetOp(BitVector* lhs, const BitVector* rhs) {
      (*lhs) &= (*rhs);
    }

    virtual BitVector transferFunction(BasicBlock* bb) {
      // current BB union bitvector

      BitVector newVec(BBToPosMap.size(), false);

      BitVector& inVec = *((*in)[bb]);
      newVec |= inVec;
      newVec.set(BBToPosMap[bb]);

      return newVec;      
    }

    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
    }
    
  private:
  };
  
  char DominatorAnalysis::ID = 0;
  RegisterPass<DominatorAnalysis> Xx("dominators",
				       "DominatorAnalysis");
}
