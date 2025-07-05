g++ -c SuperThread_bin.cpp -o SuperThread_bin.o -O0 -static
g++ -c Matrix_bin.cpp -o Matrix_bin.o -O0 -static
g++ test_v1.cpp Matrix_bin.o SuperThread_bin.o -o program -O0 -static
program