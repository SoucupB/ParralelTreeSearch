#include <stdio.h>
#include "SuperThread.h"
#include "Matrix.h"
#include <assert.h>
#include <math.h>

typedef struct GraphInput_t {
  int32_t matrixHeight;
  int32_t matrixWidth;
  int32_t threadCount;
  int32_t repeats;
} GraphInput;

typedef struct GraphOutput_t {
  ssize_t averageTimeSync;
  ssize_t averageTimeParralel;
} GraphOutput;

GraphOutput graphElementSum(GraphInput input) {
  GraphOutput response;
  response.averageTimeParralel = 0;
  response.averageTimeSync = 0;
  PSuperThread thr = thr_Create(input.threadCount);
  PMatrix a = matr_Init(input.matrixHeight, input.matrixWidth);
  PMatrix b = matr_Init(input.matrixHeight, input.matrixWidth);
  PMatrix result = matr_Init(input.matrixHeight, input.matrixWidth);

  uint64_t currentTime = GetTickCount64();
  for(size_t i = 0, c = input.repeats; i < c; i++) {
    matr_SumGlobal(a, b, result);
  }
  response.averageTimeSync = GetTickCount64() - currentTime;

  matr_SetThread(a, thr);
  currentTime = GetTickCount64();
  for(size_t i = 0, c = input.repeats; i < c; i++) {
    matr_SumGlobal(a, b, result);
  }
  response.averageTimeParralel = GetTickCount64() - currentTime;

  thr_Delete(thr);
  matr_Delete(a);
  matr_Delete(b);
  matr_Delete(result);
  return response;
}

void printData(GraphOutput inp) {
  
}

int main() {
  return 0;
}