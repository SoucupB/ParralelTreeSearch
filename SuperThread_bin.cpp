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
  InitializeCriticalSection(&self->cs);
  thr_StartThreads(self);
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
    self->threads->push_back((ThreadData) {
      .threadID = threadId,
      .threadHandle = hThread
    });
  }
}

uint8_t shouldRun(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  uint8_t isStarted = self->started;
  LeaveCriticalSection(&self->cs);
  return isStarted;
}

void thr_Register(PSuperThread self, void (*method)(PVOID), PVOID buffer) {
  EnterCriticalSection(&self->cs);
  self->atoms->push_back((MethodDecl) {
    .params = buffer,
    .method = (PVOID)method
  });
  self->atomsFinished++;
  LeaveCriticalSection(&self->cs);
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
    EnterCriticalSection(&self->cs);
    self->atomsFinished--;
    LeaveCriticalSection(&self->cs);
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
  free(self);
}

void thr_Wait(PSuperThread self) {
  while(self->atomsFinished);
}

void thr_Execute(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  self->started = 1;
  LeaveCriticalSection(&self->cs);
}

