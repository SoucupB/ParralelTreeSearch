#include "SuperThread.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

void _threadAtom(PVOID selfBuffer);
void thr_StartThreads(PSuperThread self);

PSuperThread thr_Create(int32_t threadsCount) {
  PSuperThread self = (PSuperThread)malloc(sizeof(SuperThread));
  memset(self, 0, sizeof(SuperThread));
  self->atoms = new std::vector<MethodDecl>();
  self->atomsFinished = 0;
  self->threads = new std::vector<ThreadData>();
  self->threadsCount = threadsCount;
  self->startedThreads = 0;
  self->counter = 0;
  InitializeCriticalSection(&self->cs);
  thr_StartThreads(self);
  return self;
}

void thr_WaitForThreadsStart(PSuperThread self) {
  while(1) {
    if(self->startedThreads == self->threads->size()) {
      return ;
    }
  }
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
    self->threads->push_back((ThreadData) {
      .threadID = threadId,
      .threadHandle = hThread
    });
  }
  thr_WaitForThreadsStart(self);
}

static inline uint8_t shouldRun(PSuperThread self) {
  // EnterCriticalSection(&self->cs);
  // uint8_t isStarted = self->started;
  // LeaveCriticalSection(&self->cs);
  // return isStarted;
  return InterlockedCompareExchange(&self->started, 0, 0);
}

void thr_Register(PSuperThread self, void (*method)(PVOID), PVOID buffer) {
  EnterCriticalSection(&self->cs);
  self->atoms->push_back((MethodDecl) {
    .params = buffer,
    .method = (PVOID)method
  });
  LeaveCriticalSection(&self->cs);
  InterlockedIncrement(&self->atomsFinished);
}

uint8_t shouldThreadCloseMethod(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  if(self->shouldThreadClose) {
    self->closedThreadsClosed++;
    LeaveCriticalSection(&self->cs);
    return 1;
  }
  LeaveCriticalSection(&self->cs);
  return 0;
}

void _threadAtom(PVOID selfBuffer) {
  PSuperThread self = (PSuperThread)selfBuffer;
  EnterCriticalSection(&self->cs);
  self->startedThreads++;
  LeaveCriticalSection(&self->cs);
  while(1) {
    if(!shouldRun(self)) {
      continue;
    }
    if(shouldThreadCloseMethod(self)) {
      return ;
    }
    EnterCriticalSection(&self->cs);
    if(!self->atoms->size()) {
      LeaveCriticalSection(&self->cs);
      continue;
    }
    MethodDecl currentA = (*self->atoms)[self->atoms->size() - 1];
    self->atoms->pop_back();
    LeaveCriticalSection(&self->cs);
    void (*cMethod)(PVOID) = (void (*)(PVOID))currentA.method;
    cMethod(currentA.params);
    InterlockedDecrement(&self->atomsFinished);
  }
}

void thr_Terminate(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  self->shouldThreadClose = 1;
  LeaveCriticalSection(&self->cs);
  while(1) {
    EnterCriticalSection(&self->cs);
    uint8_t currentCloseCount = self->closedThreadsClosed;
    LeaveCriticalSection(&self->cs);
    if(currentCloseCount >= self->threadsCount) {
      return ;
    }
  }
}

void thr_Delete(PSuperThread self) {
  thr_Terminate(self);
  for(size_t i = 0, c = self->threads->size(); i < c; i++) {
    CloseHandle((*self->threads)[i].threadHandle);
  }
  free(self);
}

void thr_Wait(PSuperThread self) {
  while(InterlockedCompareExchange(&self->atomsFinished, 0, 0));
}

void thr_Execute(PSuperThread self) {
  InterlockedExchange(&self->started, 1);
}

