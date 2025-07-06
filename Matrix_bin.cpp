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

void matr_Thread_Adder(PVOID adder) {
  PThreadAtom slf = (PThreadAtom)adder;
  float *src = slf->pntRef->src;
  float *dst = slf->pntRef->dst;
  int32_t currentCol = slf->colIndex;
  size_t c = slf->pntRef->colWidth;
  size_t d = currentCol * c;
  for(size_t i = 0, c = slf->pntRef->colWidth; i < c; i++) {
    dst[d + i] += src[d + i];
  }
}

void matr_SetThreadNetwork(PMatrix self, PSuperThread thread) {
  if(self->threads) {
    self->threads->thr = thread;
    return ;
  }
  self->threads = (PMatrixThread)malloc(sizeof(MatrixThread));
  self->threads->thr = thread;
  self->threads->rows = (ThreadAtom *)malloc(sizeof(ThreadAtom) * self->height);
  PPntRef pntRef = (PPntRef)malloc(sizeof(PntRef));
  pntRef->colWidth = self->width;
  for(size_t i = 0, c = self->height; i < c; i++) {
    self->threads->rows[i].colIndex = i;
    self->threads->rows[i].pntRef = pntRef;
  }
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

void matr_SumLocal_Sync(PMatrix dst, PMatrix src) {
  float *bfSrc = src->buffer;
  float *bfDst = dst->buffer;
  for(size_t i = 0, c = src->height * src->width; i < c; i++) {
    bfDst[i] += bfSrc[i];
  }
}

void matr_SumLocal_Async(PMatrix dst, PMatrix src) {
  ThreadAtom currentThread = dst->threads->rows[0];
  currentThread.pntRef->dst = dst->buffer;
  currentThread.pntRef->src = src->buffer;
  PSuperThread thread = dst->threads->thr;
  thr_Execute(thread);
  for(size_t i = 0, c = dst->height; i < c; i++) {
    thr_Register(thread, matr_Thread_Adder, &dst->threads->rows[i]);
  }
  thr_Wait(thread);
}

void matr_SumLocal(PMatrix dst, PMatrix src) {
  if(!dst->height || !src->width) {
    return ;
  }
  if(!dst->threads) {
    matr_SumLocal_Sync(dst, src);
    return ;
  }
  matr_SumLocal_Async(dst, src);
}