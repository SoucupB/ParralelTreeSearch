#pragma once
#include <stdlib.h>
#include "SuperThread.h"

typedef struct Matrix_t {
  float *buffer;
  size_t height;
  size_t width;
} Matrix;

typedef Matrix *PMatrix;

PMatrix matr_Init(size_t height, size_t width);
void matr_Set(PMatrix self, size_t i, size_t j, float value);