g++ -c *_bin.cpp -o binaryes_bin.o -O0 -static
g++ test_v1.cpp binaryes_bin.o -o program -O0 -static
program