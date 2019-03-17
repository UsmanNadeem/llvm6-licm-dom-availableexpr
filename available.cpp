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
  class AvailableExpressions : public FunctionPass, public Dataflow {

    map<Expression, unsigned int> expToPosMap;
    map<unsigned int, Expression> posToExpMap;

  public:
    static char ID;
    
    AvailableExpressions() : Dataflow(FORWARD_DIRECTION), FunctionPass(ID) { }
    
    virtual bool runOnFunction(Function& F) {
      
      // Here's some code to familarize you with the Expression
      // class and pretty printing code we've provided:
      
      set<Expression> expressions;
      for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) {
      	BasicBlock* block = &*FI;
      	for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i) {
        
      	  Instruction* I = &*i;
      	  // We only care about available expressions for BinaryOperators
      	  if (BinaryOperator *BI = dyn_cast<BinaryOperator>(I)) {
      	    // Create a new Expression to capture the RHS of the BinaryOperator
      	    expressions.insert(Expression(BI));
      	  }
      	}
      }

      unsigned int i = 0;
      for (Expression e : expressions) {
        expToPosMap[e] = i;
        posToExpMap.insert(pair<unsigned int, Expression>(i, e));
        ++i;
      }

      Dataflow::runOnFunction(F);

      // Print out the expressions used in the function
      outs() << "Expressions used by this function:\n";
      printSet(&expressions);

      for (BasicBlock& bb : F) {
        outs() << "BB:\t" << bb.getName().str()<< "\n";

        // GEN
        BitVector& genVec = *((*gen)[&bb]);
        set<Expression> generatedExpressions;
        for (auto i : genVec.set_bits()) {
          generatedExpressions.insert(posToExpMap[i]);
        }
        outs() << "GEN["<<bb.getName().str()<<"]: " << "\t";
        printSet(&generatedExpressions);
 
        // KILL
        BitVector& killVec = *((*kill)[&bb]);
        set<Expression> killedExpressions;
        for (auto i : killVec.set_bits()) {
          killedExpressions.insert(posToExpMap[i]);
        }
        outs() << "KILL["<<bb.getName().str()<<"]: " << "\t";
        printSet(&killedExpressions);

        // IN
        BitVector& inVec = *((*in)[&bb]);
        set<Expression> inExpressions;
        for (auto i : inVec.set_bits()) {
          inExpressions.insert(posToExpMap[i]);
        }
        outs() << "IN["<<bb.getName().str()<<"]: " << "\t";
        printSet(&inExpressions);

        // OUT
        BitVector& outVec = *((*out)[&bb]);
        set<Expression> outExpressions;
        for (auto i : outVec.set_bits()) {
          outExpressions.insert(posToExpMap[i]);
        }
        outs() << "OUT["<<bb.getName().str()<<"]: " << "\t";
        printSet(&outExpressions);        
        
        outs() << "\n\n";
      }
      
      // Did not modify the incoming Function.
      return false;
    }
    
    virtual void initBitVectors(BasicBlock* bb) {
      // initialize all to true 
      // cant have false because of INTERSECTION
      (*in)[bb] = new BitVector(expToPosMap.size(), true);
      (*out)[bb] = new BitVector(expToPosMap.size(), true);

      (*gen)[bb] = new BitVector(expToPosMap.size(), false);
      (*kill)[bb] = new BitVector(expToPosMap.size(), false);

      BitVector& killVec = *((*kill)[bb]);
      BitVector& genVec = *((*gen)[bb]);

      for (Instruction& I : *bb) {
        if (BinaryOperator *BI = dyn_cast<BinaryOperator>(&I)) {

          // initialize the gen set
          // this Expression is generated
          genVec.set(expToPosMap[Expression(BI)]);

          // initialize the kill set
          for (auto& globalExpr : expToPosMap) {
            if (globalExpr.first.uses(BI)) {
              // if any expression uses the LHS of this instruction kill the expression
              killVec.set(expToPosMap[globalExpr.first]);

              // a local experession could also be killed
              // in that case remove it from the genset
              genVec.reset(expToPosMap[globalExpr.first]);
            }
          }
        }
      }
    }

    virtual void boundaryCond(BitVector* bitVec) {
      // out entry = null 
      // we dont have a special entry block
      // so <in first BB> = <out entry> = null 

      bitVec->reset();

    }

    virtual void meetOp(BitVector* lhs, const BitVector* rhs) {
      (*lhs) &= (*rhs);
    }

    virtual BitVector transferFunction(BasicBlock* bb) {
      // gen U (x-Kill)
      BitVector newVec(expToPosMap.size(), false);

      BitVector& killVec = *((*kill)[bb]);
      BitVector& genVec = *((*gen)[bb]);
      BitVector& inVec = *((*in)[bb]);

      newVec &= inVec;
      newVec.reset(killVec);
      newVec |= genVec;

      return newVec;      
    }

    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
    }
    
  private:
  };
  
  char AvailableExpressions::ID = 0;
  RegisterPass<AvailableExpressions> X("available",
				       "15745 Available Expressions");
}
