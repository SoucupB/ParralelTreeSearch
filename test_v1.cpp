#include <stdio.h>
#include "SuperThread.h"

typedef struct Interval_t {
  uint32_t left;
  uint32_t right;
  uint32_t *bufferChecker;
  int32_t intervalIndex;
} Interval;

typedef struct BufferChecker_t {
  uint32_t *bufferChecker;
  Interval *intervals;
  int32_t intervalCount;
} BufferChecker;

int32_t checker = 837222200;

uint32_t min(uint32_t a, uint32_t b) {
  return a < b ? a : b;
}

void searchElement(PVOID buffer) {
  Interval *interval = (Interval *)buffer;
  for(int32_t i = interval->left; i <= interval->right; i++) {
    if(interval->bufferChecker[i] == checker) {
      printf("FOUND at interval %d\n", interval->intervalIndex);
      return ;
    }
  }
  printf("NOT found! %d\n", interval->intervalIndex);
}

BufferChecker createTestData(size_t sz, int32_t splitCount) {
  BufferChecker self;
  self.bufferChecker = (uint32_t *)malloc(sizeof(uint32_t) * sz);
  for(size_t i = 0; i < sz; i++) {
    self.bufferChecker[i] = i;
  }
  int32_t intervalSize = sz / splitCount;
  int32_t startingInterval = 0;
  int32_t index = 0;
  self.intervals = (Interval *)malloc(sizeof(Interval) * (intervalSize + 1));
  for(;;) {
    uint32_t left = startingInterval;
    uint32_t right = startingInterval + intervalSize - 1;
    self.intervals[index] = (Interval) {
      .left = left,
      .right = min(sz - 1, right),
      .bufferChecker = self.bufferChecker,
      .intervalIndex = index
    };
    index++;
    if(right >= sz - 1) {
      break;
    }
    startingInterval += intervalSize;
  }
  self.intervalCount = index;
  return self;
}

void test_v1() {
  PSuperThread thr = thr_Create(3);
  const size_t sz = 911222200;
  BufferChecker meta = createTestData(sz, 15);
  printf("%d\n", meta.intervalCount);
  for(size_t i = 0; i < meta.intervalCount; i++) {
    thr_Register(thr, searchElement, &meta.intervals[i]);
    // printf("%d %d\n", meta.intervals[i].left, meta.intervals[i].right);
  }
  thr_Execute(thr);
  thr_Wait(thr);
  printf("YOLO\n");

}

int main() {
  test_v1();
  return 0;
}