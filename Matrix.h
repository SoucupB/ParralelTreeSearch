#pragma once
#include <stdlib.h>
#include "SuperThread.h"

typedef struct MatrixThread_t {
  PSuperThread thr;
  int32_t *columns;
} MatrixThread;

typedef MatrixThread *PMatrixThread;

typedef struct Matrix_t {
  float *buffer;
  size_t height;
  size_t width;
  PMatrixThread threads;
} Matrix;

typedef Matrix *PMatrix;

PMatrix matr_Init(size_t height, size_t width);
void matr_Set(PMatrix self, size_t i, size_t j, float value);
void matr_SumLocal(PMatrix src, PMatrix dst);
float matr_Value(PMatrix self, size_t i, size_t j);