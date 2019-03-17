// ECE 5984 S18 Assignment 2: dataflow.h
// Group:
// http://www.cs.cmu.edu/~15745/15745_assignment2/code/Dataflow/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/CFG.h"

#include <ostream>
#include <list>

using namespace llvm;
using namespace std;

namespace llvm {
    typedef list<BasicBlock*> BBList;
    typedef ValueMap<const BasicBlock*, BitVector*> DomainMapping;
    
    class Dataflow {
        int direction;
        public:
            DomainMapping *in;
            DomainMapping *out;
            DomainMapping *neighbors;
            ValueMap<BasicBlock*, bool> *checked;

            Dataflow(int direction);                
            void forwardPass(BBList &w);
            void backwardPass(BBList &w);
            void runOnFunction(Function &F);

        protected:
            virtual void boundaryCond(BitVector*) = 0;
            virtual void meetOp(BitVector* lhs, const BitVector* rhs) = 0;
            virtual void initBitVectors(BasicBlock* b) = 0;
            virtual BitVector* transferFunction(BasicBlock& b) = 0;
    };
}

#endif
