#include <stdio.h>
#include "SuperThread.h"

typedef struct Interval_t {
  int32_t left;
  int32_t right;
  int32_t *bufferChecker;
} Interval;

Interval intervals[64];
int32_t checker = 64;
int32_t buffer[] = {1, 2, 3, 4, 5};

void searchElement(PVOID buffer) {
  Interval *interval = (Interval *)buffer;
  printf("PENIS\n");
  for(int32_t i = interval->left; i <= interval->right; i++) {
    if(interval->bufferChecker[i] == checker) {
      printf("FOUND\n");
      return ;
    }
  }
  printf("NOT found!\n");
}

int main() {
  PSuperThread thr = thr_Create(1);
  intervals[0] = (Interval) {
    .left = 0,
    .right = sizeof(buffer) / sizeof(int32_t) - 1,
    .bufferChecker = buffer
  };
  thr_Register(thr, searchElement, &intervals[0]);
  thr_Execute(thr);
  thr_Wait(thr);
  printf("YOLO\n");
  return 0;
}