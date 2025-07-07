#include <stdio.h>
#include "SuperThread.h"
#include "Matrix.h"
#include <assert.h>
#include <math.h>

#define EPSILON 1e-5

#define FLOAT_EQL(x, y) (fabs(x - y) < EPSILON)

void helper_Test_Sum(PMatrix a, PMatrix b, PMatrix result) {
  for(size_t i = 0; i < a->height; i++) {
    for(size_t j = 0; j < a->width; j++) {
      assert(FLOAT_EQL(matr_Value(a, i, j) + matr_Value(b, i, j), matr_Value(result, i, j)));
    }
  }
}

void helper_Test_Sync(PMatrix a, PMatrix b, PMatrix result) {
  PMatrix secReview = matr_Init(a->height, b->width);
  if(a->threads) {
    matr_ClearThreadData(a);
  }
  matr_MatMul(a, b, secReview);
  for(size_t i = 0; i < secReview->height * secReview->width; i++) {
    assert(FLOAT_EQL(secReview->buffer[i], result->buffer[i]));
  }
  matr_Delete(secReview);
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
  matr_SetThread(a, thr);
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
  PMatrix a = matr_Init(50, 50);
  PMatrix b = matr_Init(50, 50);
  PMatrix c = matr_Init(50, 50);
  PSuperThread thr = thr_Create(3);
  for(size_t i = 0; i < 50; i++) {
    for(size_t j = 0; j < 50; j++) {
      matr_Set(a, i, j, (float)(j + i * 3));
      matr_Set(b, i, j, (float)(j + i * 3));
    }
  }
  matr_SetThread(a, thr);
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0; i < 1000; i++) {
    matr_SumGlobal(a, b, c);
  }
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  helper_Test_Sum(a, b, c);
  thr_Delete(thr);
  matr_Delete(a);
  matr_Delete(b);
  matr_Delete(c);
}

void test_v4() {
  PMatrix a = matr_Init(3, 3);
  PMatrix b = matr_Init(3, 3);
  PMatrix c = matr_Init(3, 3);
  PSuperThread thr = thr_Create(1);
  size_t z = 1;
  for(size_t i = 0; i < 3; i++) {
    for(size_t j = 0; j < 3; j++) {
      matr_Set(a, i, j, (float)z);
      matr_Set(b, i, j, (float)z);
      z++;
    }
  }
  // matr_SetThread(a, thr);
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0; i < 1; i++) {
    matr_MatMul(a, b, c);
  }
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  assert(matr_Value(c, 0, 0) == 30.0f);
  assert(matr_Value(c, 0, 1) == 36.0f);
  assert(matr_Value(c, 0, 2) == 42.0f);

  assert(matr_Value(c, 1, 0) == 66.0f);
  assert(matr_Value(c, 1, 1) == 81.0f);
  assert(matr_Value(c, 1, 2) == 96.0f);

  assert(matr_Value(c, 2, 0) == 102.0f);
  assert(matr_Value(c, 2, 1) == 126.0f);
  assert(matr_Value(c, 2, 2) == 150.0f);
  thr_Delete(thr);
  matr_Delete(a);
  matr_Delete(b);
  matr_Delete(c);
}

void test_v5() {
  PMatrix a = matr_Init(3, 3);
  PMatrix b = matr_Init(3, 3);
  PMatrix c = matr_Init(3, 3);
  PSuperThread thr = thr_Create(1);
  size_t z = 1;
  for(size_t i = 0; i < 3; i++) {
    for(size_t j = 0; j < 3; j++) {
      matr_Set(a, i, j, (float)z);
      matr_Set(b, i, j, (float)z);
      z++;
    }
  }
  // matr_SetThread(a, thr);
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0; i < 8000; i++) {
    matr_MatMul(a, b, c);
  }
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  assert(matr_Value(c, 0, 0) == 30.0f);
  assert(matr_Value(c, 0, 1) == 36.0f);
  assert(matr_Value(c, 0, 2) == 42.0f);

  assert(matr_Value(c, 1, 0) == 66.0f);
  assert(matr_Value(c, 1, 1) == 81.0f);
  assert(matr_Value(c, 1, 2) == 96.0f);

  assert(matr_Value(c, 2, 0) == 102.0f);
  assert(matr_Value(c, 2, 1) == 126.0f);
  assert(matr_Value(c, 2, 2) == 150.0f);
  thr_Delete(thr);
  matr_Delete(a);
  matr_Delete(b);
  matr_Delete(c);
}

void test_v6() {
  PMatrix a = matr_Init(50, 3200);
  PMatrix b = matr_Init(3200, 50);
  PMatrix c = matr_Init(50, 50);
  PSuperThread thr = thr_Create(4);
  size_t z = 1;
  for(size_t i = 0; i < 50; i++) {
    for(size_t j = 0; j < 3200; j++) {
      matr_Set(a, i, j, (float)z);
      z++;
    }
  }
  z = 1;
  for(size_t i = 0; i < 3200; i++) {
    for(size_t j = 0; j < 50; j++) {
      matr_Set(b, i, j, (float)z);
      z++;
    }
  }
  matr_SetThread(a, thr);
  printf("Start checker\n");
  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0; i < 400; i++) {
    matr_MatMul(a, b, c);
  }
  printf("Finished in %lld ms\n", GetTickCount64() - currentTime);
  helper_Test_Sync(a, b, c);
  thr_Delete(thr);
  matr_Delete(a);
  matr_Delete(b);
  matr_Delete(c);
}

int main() {
  // test_v1();
  // test_v2();
  test_v3();
  // test_v4();
  // test_v5();
  test_v6();
  printf("Tests passed!\n");
  return 0;
}