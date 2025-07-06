#include <stdio.h>
#include "SuperThread.h"
#include "Matrix.h"
#include <assert.h>

void test_v1() {
  PMatrix a = matr_Init(500, 500);
  PMatrix b = matr_Init(500, 500);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 500; j++) {
      matr_Set(a, i, j, (float)(j + i * 3));
      matr_Set(b, i, j, (float)(j + i * 3));
    }
  }
  matr_SumLocal(a, b);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 500; j++) {
      assert(matr_Value(a, i, j) == 2.0f * (float)(j + i * 3));
    }
  }
}

void test_v2() {
  PMatrix a = matr_Init(500, 500);
  PMatrix b = matr_Init(500, 500);
  PSuperThread thr = thr_Create(1);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 500; j++) {
      matr_Set(a, i, j, (float)(j + i * 3));
      matr_Set(b, i, j, (float)(j + i * 3));
    }
  }
  matr_SetThreadNetwork(a, thr);
  matr_SumLocal(a, b);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 500; j++) {
      assert(matr_Value(a, i, j) == 2.0f * (float)(j + i * 3));
    }
  }
}

int main() {
  // test_v1();
  test_v2();
  printf("Tests passed!\n");
  return 0;
}