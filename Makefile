INC=-I/usr/lib/llvm-6.0/include/
all: licm.so dominator.so

CXX = clang
CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) $(INC) -fPIC -g -O0

dataflow.o: dataflow.cpp dataflow.h

available-support.o: available-support.cpp available-support.h	

%.so: %.o dataflow.o available-support.o
	$(CXX) -dylib -shared $^ -o $@

clean:
	rm -f *.o *~ *.so

.PHONY: clean all
