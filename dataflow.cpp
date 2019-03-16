// ECE 5984 S18 Assignment 2: aataflow.cpp
// Group:
// Source: http://www.cs.cmu.edu/~15745/15745_assignment2/code/Dataflow/
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"

namespace llvm {

	Dataflow::Dataflow(int dir) {
	    this->direction = dir;  
	    in  = new DomainMapping();
	    out = new DomainMapping();
	    neighbors = new DomainMapping();
	    checked = new ValueMap<BasicBlock*, bool>();
	}

	void Dataflow::forwardPass(BBList &bbs){
	    BasicBlock *curr = *bbs.begin();
	    bbs.pop_front();
	    (*checked)[curr] = true;
	    int predecessors = 0;

	    pred_iterator start_p = pred_begin(curr), end_p = pred_end(curr);
	    while (start_p != end_p) {
	    	predecessors++;
	    	if (start_p != pred_begin(curr)) {
	    		meetOp((*in)[curr], (*out)[*start_p]);
	    	} else {
	    		*(*in)[curr] = *(*out)[*start_p];
	    	}
	    }

	    if (predecessors == 0) {
	    	boundaryCond((*in)[curr]);
	    }


	    BitVector *updatedSet = transferFunction(*curr);

	    if (*updatedSet != *(*out)[curr]) {
	    	*(*out)[curr] = *updatedSet;
	    }

	    succ_iterator start_s = succ_begin(curr), end_s = succ_end(curr);
	    while (start_s != end_s) {
	    	if ((*checked)[*start_s]) {
	    		bbs.push_back(*start_s);
	    	}
	    	start_s++;
	    }
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

	    BitVector *updatedInSet = transferFunction(*curr);

	    if (((*updatedInSet) != (*(*in)[curr]))) {
	        *(*in)[curr] = *updatedInSet;
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
			(*in)[b] = initBitVectors(*i);
			(*out)[b] = initBitVectors(*i);
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
	    	BasicBlock &entryBlock = F.getEntryBlock();
	    	bbs->push_back(&entryBlock);
	    }

	    while (!bbs->empty()) {
	        if (direction == 0) {
	            backwardPass(*bbs);
	        } else if (direction == 1) {
	            forwardPass(*bbs);
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