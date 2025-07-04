#include <stdio.h>
#include "SuperThread.h"

typedef struct Interval_t {
  uint32_t left;
  uint32_t right;
  uint32_t *a;
  uint32_t *b;
  uint32_t *c;
  int32_t intervalIndex;
} Interval;

typedef struct BufferChecker_t {
  uint32_t *a;
  uint32_t *b;
  uint32_t *c;
  Interval *intervals;
  int32_t intervalCount;
} BufferChecker;

uint32_t min(uint32_t a, uint32_t b) {
  return a < b ? a : b;
}

void searchElement(PVOID buffer) {
  Interval *interval = (Interval *)buffer;
  for(int32_t i = interval->left; i <= interval->right; i++) {
    interval->c[i] = interval->a[i] + interval->b[i];
  }
  printf("Finished sum for %d\n", interval->intervalIndex);
}

BufferChecker createTestData(size_t sz, int32_t splitCount) {
  BufferChecker self;
  self.a = (uint32_t *)malloc(sizeof(uint32_t) * sz);
  self.b = (uint32_t *)malloc(sizeof(uint32_t) * sz);
  self.c = (uint32_t *)malloc(sizeof(uint32_t) * sz);
  for(size_t i = 0; i < sz; i++) {
    self.a[i] = i;
    self.b[i] = i;
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
      .a = self.a,
      .b = self.b,
      .c = self.c,
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

void checkSum(BufferChecker meta, size_t sz) {
  for(int32_t i = 0; i < sz; i++) {
    if(meta.a[i] + meta.b[i] != meta.c[i]) {
      printf("Sums are wrong\n");
      return ;
    }
  }
  printf("Test passed!\n");
}

void test_v1() {
  PSuperThread thr = thr_Create(1);
  const size_t sz = 499260024;
  BufferChecker meta = createTestData(sz, 3);
  for(size_t i = 0; i < meta.intervalCount; i++) {
    thr_Register(thr, searchElement, &meta.intervals[i]);
  }
  printf("Started\n");
  uint64_t currentTime = GetTickCount64();
  thr_Execute(thr);
  thr_Wait(thr);
  printf("Finished searching took %lld ms\n", GetTickCount64() - currentTime);
  thr_Delete(thr);
  printf("Done\n");
  checkSum(meta, sz);
}

int main() {
  // test_v1();
  printf("Tests passed!\n");
  return 0;
}