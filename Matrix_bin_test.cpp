#include <stdio.h>
#include "SuperThread.h"
#include "Matrix.h"
#include <assert.h>

void test_v1() {
  PMatrix a = matr_Init(3, 3);
  PMatrix b = matr_Init(3, 3);
  for(size_t i = 0; i < 3; i++) {
    for(size_t j = 0; j < 3; j++) {
      matr_Set(a, i, j, (float)(j + i * 3));
      matr_Set(b, i, j, (float)(j + i * 3));
    }
  }
  matr_SumLocal(a, b);
  for(size_t i = 0; i < 3; i++) {
    for(size_t j = 0; j < 3; j++) {
      assert(matr_Value(b, i, j) == 2.0f * (float)(j + i * 3));
    }
  }
}

int main() {
  test_v1();
  printf("Tests passed!\n");
  return 0;
}