#include "SuperThread.h"
#include <windows.h>

void _threadAtom(PVOID selfBuffer);

PSuperThread thr_Create(int32_t threadsCount) {
  PSuperThread self = (PSuperThread)malloc(sizeof(SuperThread));
  self->atoms = new std::vector<MethodDecl>();
  self->threads = new std::vector<ThreadData>();
  self->threadsCount = threadsCount;
  InitializeCriticalSection(&self->cs);
  return self;
}

void thr_StartThreads(PSuperThread self) {
  DWORD threadId;
  HANDLE hThread;

  for(size_t i = 0; i < self->threadsCount; i++) {
    hThread = CreateThread(
          NULL,
          0,
          (LPTHREAD_START_ROUTINE)_threadAtom,
          (PVOID)self,
          0,
          &threadId);
  }

  self->threads->push_back((ThreadData) {
    .threadID = threadId,
    .threadHandle = hThread
  });
}

uint8_t shouldRun(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  uint8_t isStarted = self->started;
  LeaveCriticalSection(&self->cs);
  return isStarted;
}

void executeIndexesMethod(PSuperThread self) {
  
}

void _threadAtom(PVOID selfBuffer) {
  PSuperThread self = (PSuperThread)selfBuffer;
  while(1) {
    if(!shouldRun(self)) {
      continue;
    }

  }
}

void thr_Execute(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  self->started = 1;
  LeaveCriticalSection(&self->cs);
}

