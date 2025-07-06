#pragma once
#include <stdlib.h>
#include "SuperThread.h"

typedef struct PntRef_t {
  float *dst;
  float *src;
  float *secondDst;
  size_t colWidth;
} PntRef;

typedef PntRef *PPntRef;

typedef struct ThreadAtom_t {
  int32_t colIndex;
  PPntRef pntRef;
} ThreadAtom;

typedef ThreadAtom *PThreadAtom;

typedef struct MatrixThread_t {
  PSuperThread thr;
  ThreadAtom *rows;
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
void matr_SumLocal(PMatrix dst, PMatrix src);
float matr_Value(PMatrix self, size_t i, size_t j);
void matr_SumGlobal(PMatrix a, PMatrix b, PMatrix result);
void matr_SetThreadNetwork(PMatrix self, PSuperThread thread);
void matr_MatMul(PMatrix src, PMatrix adjucant, PMatrix dst);
void matr_Delete(PMatrix self);