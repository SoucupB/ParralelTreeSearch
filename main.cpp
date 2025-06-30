#include <stdio.h>
#include "SuperThread.h"

int main() {
  PSuperThread self = thr_Create(3);
  printf("YOLO\n");
  return 0;
}