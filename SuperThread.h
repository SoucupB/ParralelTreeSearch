#pragma once
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>

typedef struct MethodDecl_t {
  PVOID params;
  PVOID method;
} MethodDecl;

#define Vects std::vector<MethodDecl> 
#define VectINT std::vector<DWORD> 

typedef struct SuperThread_t {
  Vects *atoms;
  VectINT *threadIDs;
  int32_t totalThreadsSpawn;
} SuperThread;

typedef SuperThread *PSuperThread;

PSuperThread thr_Create(int32_t totalThreadsSpawn);