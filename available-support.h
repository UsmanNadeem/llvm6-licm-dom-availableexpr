// ECE 5984 S18 Assignment 2: available-support.h
// Group:
// Source: http://www.cs.cmu.edu/~15745/15745_assignment2/code/Dataflow/
////////////////////////////////////////////////////////////////////////////////

#ifndef __AVAILABLE_SUPPORT_H__
#define __AVAILABLE_SUPPORT_H__

#include <string>
#include <vector>
#include <set>
#include <map>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
  std::string getShortValueName(const Value * v);

  class Expression {
  public:
    Value * v1;
    Value * v2;
    Instruction::BinaryOps op;
    Expression () {}
    Expression (Instruction * I);
    Expression (const Expression& e);
    Expression& operator = (const Expression &e); 
    bool operator== (const Expression &e2) const;
    bool operator< (const Expression &e2) const;
    std::string toString() const;
    bool uses(const Value* v) const;
  };

  void printSet(std::set<Expression> * x);
}

#endif
