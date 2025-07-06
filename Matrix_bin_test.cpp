#include <stdio.h>
#include "SuperThread.h"
#include "Matrix.h"
#include <assert.h>

void helper_Test_Sum(PMatrix a, PMatrix b, PMatrix result) {
  for(size_t i = 0; i < a->height; i++) {
    for(size_t j = 0; j < a->width; j++) {
      assert(matr_Value(a, i, j) + matr_Value(b, i, j) == matr_Value(result, i, j));
    }
  }
}

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
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  matr_SumLocal(a, b);
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 500; j++) {
      assert(matr_Value(a, i, j) == 2.0f * (float)(j + i * 3));
    }
  }
  thr_Delete(thr);
}

void test_v3() {
  PMatrix a = matr_Init(500, 5000);
  PMatrix b = matr_Init(500, 5000);
  PMatrix c = matr_Init(500, 5000);
  PSuperThread thr = thr_Create(7);
  for(size_t i = 0; i < 500; i++) {
    for(size_t j = 0; j < 5000; j++) {
      matr_Set(a, i, j, (float)(j + i * 3));
      matr_Set(b, i, j, (float)(j + i * 3));
    }
  }
  matr_SetThreadNetwork(a, thr);
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0; i < 1000; i++) {
    matr_SumGlobal(a, b, c);
  }
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  helper_Test_Sum(a, b, c);
  thr_Delete(thr);
}

int main() {
  // test_v1();
  // test_v2();
  test_v3();
  printf("Tests passed!\n");
  return 0;
}