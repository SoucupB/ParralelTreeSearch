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

void printData(GraphInput *inp, GraphOutput *out, size_t sz) {
  FILE *fd = fopen("GraphData.out", "w+");
  for(size_t i = 0; i < sz; i++) {
    fprintf(fd, "%d %d %d %d %d\n", inp[i].matrixHeight, inp[i].matrixWidth, inp[i].threadCount, out[i].averageTimeParralel, out[i].averageTimeSync);
  }
  fclose(fd);
}

int main() {
  GraphInput input[] = {
    {
      .matrixHeight = 10,
      .matrixWidth = 10,
      .threadCount = 2,
      .repeats = 10000
    }
  };
  GraphOutput output[1024];
  size_t sz = sizeof(input) / sizeof(GraphInput);
  for(size_t i = 0; i < sz; i++) {
    output[i] = graphElementSum(input[i]);
  }
  printData(input, output, sz);
  printf("Done\n");
  return 0;
}