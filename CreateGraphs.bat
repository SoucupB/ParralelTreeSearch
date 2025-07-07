g++ -c SuperThread_bin.cpp -o SuperThread_bin.o -O0 -static
g++ -c Matrix_bin.cpp -o Matrix_bin.o -O0 -static
g++ Graph.cpp Matrix_bin.o SuperThread_bin.o -o GraphExe -O0 -static
GraphExe