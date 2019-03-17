// ECE 5984 S18 Assignment 2: aataflow.cpp
// Group:
// Source: http://www.cs.cmu.edu/~15745/15745_assignment2/code/Dataflow/
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"
#include <iostream>

namespace llvm {

	Dataflow::Dataflow(int dir) {
	    this->direction = dir;  
	    in  = new DomainMapping();
	    out = new DomainMapping();
	    gen = new DomainMapping();
	    kill = new DomainMapping();
	    neighbors = new DomainMapping();
	    checked = new ValueMap<BasicBlock*, bool>();
	}

	// /// Get the number of predecessors of \p BB. This is a linear time operation.
	// /// Use \ref BasicBlock::hasNPredecessors() or hasNPredecessorsOrMore if able.
	// code taken from newer version of llvm
	inline unsigned pred_size(const BasicBlock *BB) {
		return std::distance(pred_begin(BB), pred_end(BB));
	}

	bool Dataflow::forwardPass(BBList &bbsWorkList){
	    BasicBlock *currBB = bbsWorkList.front();
	    bbsWorkList.pop_front();
	    bool changesMade = false;
	    (*checked)[currBB] = true;

	    if (pred_size(currBB) == 0) {
	    	// entry block
	    	boundaryCond((*in)[currBB]);
	    } else {
		    for (BasicBlock* pred : predecessors(currBB)) {
		    	// meet over every pred
	    		meetOp((*in)[currBB], (*out)[pred]);
			}
	    }

	    BitVector newOutSet = transferFunction(currBB);
	    if (newOutSet != *((*out)[currBB])) {
	    	*((*out)[currBB]) = newOutSet;
	    	changesMade = true;
	    }
	     

	    for (BasicBlock* successor : successors(currBB)) {
	    	if ((*checked)[successor] == false) {
	    		bbsWorkList.push_back(successor);
	    	}
	    }
	    return changesMade;
	}

	void Dataflow::backwardPass(BBList &bbs){
	    BasicBlock *curr = *bbs.begin();
	    bbs.pop_front();
	    (*checked)[curr] = true;
	    int successors = 0;

	    succ_iterator start_s = succ_begin(curr), end_s = succ_end(curr);
	    while (start_s != end_s) {
	    	successors++;
	    	if (start_s != succ_begin(curr)) {
	    		meetOp((*out)[curr], (*in)[*start_s]);
	    	} else {
	    		*(*out)[curr] = *(*in)[*start_s];
	    	}
	    	start_s++;
	    }

	    if ((*neighbors).find(curr) != (*neighbors).end()) {
	        meetOp((*out)[curr], (*neighbors)[curr]);
	    }

	    if (successors == 0) {
	    	boundaryCond((*out)[curr]); 
	    }

	    BitVector updatedInSet = transferFunction(curr);

	    if ((updatedInSet != (*(*in)[curr]))) {
	        *(*in)[curr] = updatedInSet;
	    }
        
        pred_iterator start_p = pred_begin(curr), end_p = pred_end(curr);
        while (start_p != end_p) {
        	if ((*checked)[*start_p] == false) {                            
                bbs.push_back(*start_p); 
            }
        	start_p++;
        }
	}

	void Dataflow::runOnFunction(Function &F) {
	    checked = new ValueMap<BasicBlock*, bool>();
	    Function::iterator i = F.begin();
		while (i != F.end()) {
			BasicBlock *b = &(*i);
			initBitVectors(b);
			// (*in)[b] = initBitVectors(*i);
			// (*out)[b] = initBitVectors(*i);
			i++;
		}

	    BBList *bbs = new BBList();

	    if (direction == 0) { // 0 = Backward
	    	Function::iterator start_f = F.begin(), end_f = F.end();
	    	while (start_f != end_f) {
	    		int successors = 0;
	    		succ_iterator start_s = succ_begin(&*start_f), end_s = succ_end(&*start_f);
	    		while (start_s != end_s) {
	    			successors++;
	    			start_s++;
	    		}

	    		if (successors == 0) {
	    			bbs->push_back(&*start_f);
	    		}
	    		start_f++;
	    	}
	    } else if (direction == 1) { // 1 = Forward

	    	// iterate until the algo has converged
		    bool changesMade = false;
		    do {
		    	// reset the visited marker
		    	for (BasicBlock& b : F) {
					(*checked)[&b] = false;
		    	}
			    changesMade = false;
		    	BasicBlock &entryBlock = F.getEntryBlock();
		    	bbs->push_back(&entryBlock);

		    	// forwardPass for all BBs in CFG
			    while (!bbs->empty()) {
		            changesMade = forwardPass(*bbs) ? true : changesMade;
			    }
		    } while (changesMade == true);

		    return; // dont know what the code below is for!!
		    // /////*********FORWARD PASS ENDS HERE*************
	    }

	    while (!bbs->empty()) {
	        if (direction == 0) {
	            backwardPass(*bbs);
	        }
	    }

	    i = F.begin();
	    while (i != F.end()) {
	    	BasicBlock *curr = &*i;
	    	if ((*neighbors).find(curr) != (*neighbors).end()) {
	    		succ_iterator iter2 = succ_begin(&*i);
	    		while (iter2 != succ_end(&*i)) {
	    			meetOp((*in)[*iter2], (*neighbors)[curr]);
	    			iter2++;
	    		}
	    	}
	    	i++;
	    }
	}
}