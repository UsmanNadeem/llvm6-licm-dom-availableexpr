Loop Invariant Code Motion, Dominator Analysis and Available expressions pass for LLVM 6
==========================================

Run `make clean && make` to compile the dominator analysis pass and the licm pass.
Do the same in the `availableExpressions/` folder. Both passes are independent. 

### Compile and instrument the first test case

* `clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S -emit-llvm tests/licm1.c -o tests/licm1.ll;`
* `opt -mem2reg tests/licm1.ll -S -o tests/licm1.ll`
* `opt -loop-simplify tests/licm1.ll -S -o tests/licm1.ll`
* `opt -load ./licm.so -loop-invariant-code-motion tests/licm1.ll -S -o tests/licm1Opt.ll`


### Compile and instrument the second test case

* `clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S -emit-llvm tests/licm2.c -o tests/licm2.ll;`
* `opt -mem2reg tests/licm2.ll -S -o tests/licm2.ll`
* `opt -loop-simplify tests/licm2.ll -S -o tests/licm2.ll`
* `opt -load ./licm.so -loop-invariant-code-motion tests/licm2.ll -S -o tests/licm2Opt.ll`


### Compile and instrument the third test case

* `clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S -emit-llvm tests/licm3.c -o tests/licm3.ll;`
* `opt -mem2reg tests/licm3.ll -S -o tests/licm3.ll`
* `opt -loop-simplify tests/licm3.ll -S -o tests/licm3.ll`
* `opt -load ./licm.so -loop-invariant-code-motion tests/licm3.ll -S -o tests/licm3Opt.ll`

