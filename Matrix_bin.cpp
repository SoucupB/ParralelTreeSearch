#include "Matrix.h"
#include <string.h>

PMatrix matr_Init(size_t height, size_t width) {
  PMatrix self = (PMatrix)malloc(sizeof(Matrix));
  self->height = height;
  self->width = width;
  self->buffer = (float *)malloc(height * width * sizeof(float));
  self->threads = NULL;
  memset(self->buffer, 0, height * width * sizeof(float));
  return self;
}

void matr_Set(PMatrix self, size_t i, size_t j, float value) {
  if(i >= self->height || j >= self->width) {
    return ;
  }
  self->buffer[i * self->width + j] = value;
}

void matr_Print(PMatrix self) {
  size_t k = 0;
  for(size_t i = 0; i < self->height; i++) {
    for(size_t j = 0; j < self->width; j++) {
      printf("%f ", self->buffer[k++]);
    }
    printf("\n");
  }
}

float matr_Value(PMatrix self, size_t i, size_t j) {
  return self->buffer[i * self->width + j];
}

void matr_SumLocal_Sync(PMatrix src, PMatrix dst) {
  float *bfSrc = src->buffer;
  float *bfDst = dst->buffer;
  for(size_t i = 0, c = src->height * src->width; i < c; i++) {
    bfDst[i] += bfSrc[i];
  }
}

void matr_SumLocal(PMatrix src, PMatrix dst) {
  if(!src->threads) {
    matr_SumLocal_Sync(src, dst);
    return ;
  }
  return ;
}