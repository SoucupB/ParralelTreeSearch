#pragma once
#include <windows.h>
#include <stdio.h>
#include <vector>

typedef struct MethodDecl_t {
  PVOID params;
  PVOID method;
} MethodDecl;

#define Vects std::vector<MethodDecl> 

typedef struct SuperThread_t {
  Vects atoms;
} SuperThread;

typedef SuperThread *PSuperThread;

PSuperThread thr_Create();