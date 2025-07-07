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

void matr_SumGlobal_Sync(PMatrix a, PMatrix b, PMatrix result) {
  float *resultBuffer = result->buffer;
  float *aBuffer = a->buffer;
  float *bBuffer = b->buffer;
  for(size_t i = 0, c = a->height * a->width; i < c; i++) {
    resultBuffer[i] = aBuffer[i] + bBuffer[i];
  }
}

void matr_Thread_GlobalAdder(PVOID adder) {
  PThreadAtom slf = (PThreadAtom)adder;
  float *a = slf->pntRef->src;
  float *b = slf->pntRef->dst;
  float *result = slf->pntRef->secondDst;
  int32_t currentCol = slf->colIndex;
  size_t c = slf->pntRef->colWidth;
  size_t d = currentCol * c;
  for(size_t i = 0, c = slf->pntRef->colWidth; i < c; i++) {
    result[d + i] = a[d + i] + b[d + i];
  }
}

void matr_SumGlobal_Async(PMatrix a, PMatrix b, PMatrix result) {
  ThreadAtom currentThread = a->threads->rows[0];
  currentThread.pntRef->dst = a->buffer;
  currentThread.pntRef->src = b->buffer;
  currentThread.pntRef->secondDst = result->buffer;
  PSuperThread thread = a->threads->thr;
  thr_Execute(thread);
  for(size_t i = 0, c = a->height; i < c; i++) {
    thr_Register(thread, matr_Thread_GlobalAdder, &a->threads->rows[i]);
  }
  thr_Wait(thread);
}

void matr_SumGlobal(PMatrix a, PMatrix b, PMatrix result) {
  if(!a->height || !b->width || !result->width) {
    return ;
  }
  if(!a->threads) {
    matr_SumGlobal_Sync(a, b, result);
    return ;
  }
  matr_SumGlobal_Async(a, b, result);
}

void matr_MatMul_Sync(PMatrix src, PMatrix adj, PMatrix dst) {
  float *dstBufffer = dst->buffer;
  float *srcBufffer = src->buffer;
  float *adjBufffer = adj->buffer;
  size_t k = 0;
  for(size_t i = 0, srH = src->height, srW = src->width, adW = adj->width, adH = adj->height; i < srH; i++) {
    for(size_t j = 0, z = i * srW; j < adW; j++) {
      float rowResult = 0.0f;
      for(size_t k = 0; k < adH; k++) {
        rowResult += srcBufffer[k + z] * adjBufffer[k * adW + j];  
      }
      dstBufffer[k++] = rowResult;
    }
  }
}

void matr_RemoveThreadData(PMatrix self) {
  if(self->width) {
    free(self->threads->rows[0].pntRef);
  }
  free(self->threads->rows);
}

void matr_Delete(PMatrix self) {
  if(self->threads) {
    matr_RemoveThreadData(self);
  }
  free(self->buffer);
  free(self);
}

void matr_Thread_GlobalMul(PVOID adder) {
  MulThreadAtom *slf = (MulThreadAtom *)adder;
  float *a = slf->pntRef->src;
  float *b = slf->pntRef->dst;
  float *result = slf->pntRef->secondDst;
  size_t aHeight = slf->pntRef->aHeight;
  size_t aWidth = slf->pntRef->aWidth; 
  size_t bHeight = slf->pntRef->bHeight;
  size_t bWidth = slf->pntRef->bWidth;
  int32_t aLine = slf->aLine;
  int32_t bCol = slf->bCol;
  float lValue = 0.0f;
  for(size_t i = 0; i < aWidth; i++) {
    lValue += a[i + aLine * aWidth] * b[i * bWidth + bCol];
  }
  result[slf->currentIndex] = lValue; 
}

void matr_MatMul_Async(PMatrix src, PMatrix adjucant, PMatrix dst) {
  const size_t sz = src->height * adjucant->width;
  MulThreadAtom currentBuffer[sz + 1];
  MatMulRef currentThreadData;
  currentThreadData.src = src->buffer;
  currentThreadData.dst = adjucant->buffer;
  currentThreadData.secondDst = dst->buffer;
  currentThreadData.aHeight = src->height;
  currentThreadData.aWidth = src->width;
  currentThreadData.bHeight = adjucant->height;
  currentThreadData.bWidth = adjucant->width;
  PSuperThread thread = src->threads->thr;
  thr_Execute(thread);
  int32_t k = 0;
  for(int32_t i = 0, c = src->height, v = adjucant->width; i < c; i++) {
    for(int32_t j = 0; j < v; j++) {
      currentBuffer[k] = (MulThreadAtom) {
        .aLine = i,
        .bCol = j,
        .currentIndex = k++,
        .pntRef = &currentThreadData
      };
      thr_Register(thread, matr_Thread_GlobalMul, &currentBuffer[k]);
    }
  }
  thr_Wait(thread);
}

void matr_MatMul(PMatrix src, PMatrix adjucant, PMatrix dst) {
  if(!src->threads) {
    matr_MatMul_Sync(src, adjucant, dst);
    return ;
  }
  matr_MatMul_Async(src, adjucant, dst);
}