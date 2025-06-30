#pragma once
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>

typedef struct MethodDecl_t {
  PVOID params;
  PVOID method;
} MethodDecl; 

typedef struct ThreadData_t {
  DWORD threadID;
  PVOID threadHandle;
} ThreadData;

typedef struct SuperThread_t {
  std::vector<MethodDecl> *atoms;
  int32_t threadsCount;
  std::vector<ThreadData> *threads;
  uint8_t started;
  uint32_t methodIndex;
  CRITICAL_SECTION cs;
} SuperThread;

typedef SuperThread *PSuperThread;

void thr_StartThreads(PSuperThread self);
PSuperThread thr_Create(int32_t threadsCount);
