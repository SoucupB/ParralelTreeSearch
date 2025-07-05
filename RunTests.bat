g++ -c SuperThread_bin.cpp -o SuperThread_bin.o -O0 -static
g++ -c Matrix_bin.cpp -o Matrix_bin.o -O0 -static
g++ Matrix_bin_test.cpp Matrix_bin.o SuperThread_bin.o -o Matrix_bin_test -O0 -static
Matrix_bin_test