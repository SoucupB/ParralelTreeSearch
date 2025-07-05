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
  self->threads = new std::vector<ThreadData>();
  self->threadsCount = threadsCount;
  self->currentThreads = 0;
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
  uint32_t totalThreadCount = self->threadsCount;
  uint32_t currentThreadCount = self->currentThreads;
  uint8_t cnt = currentThreadCount < totalThreadCount;
  uint8_t totalThreadsRun = self->totalThreadsRun;
  LeaveCriticalSection(&self->cs);
  return totalThreadsRun < totalThreadCount && isStarted && cnt;
}

void thr_Register(PSuperThread self, void (*method)(PVOID), PVOID buffer) {
  EnterCriticalSection(&self->cs);
  self->atoms->push_back((MethodDecl) {
    .params = buffer,
    .method = (PVOID)method
  });
  LeaveCriticalSection(&self->cs);
}

uint8_t shouldThreadCloseMethod(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  if(self->shouldThreadClose) {
    self->closeThreadsClosed++;
    LeaveCriticalSection(&self->cs);
    return 1;
  }
  LeaveCriticalSection(&self->cs);
  return 0;
}

// void _threadAtom(PVOID selfBuffer) {
//   PSuperThread self = (PSuperThread)selfBuffer;
//   while(1) {
//     if(shouldThreadCloseMethod(self)) {
//       return ;
//     }
//     if(!shouldRun(self)) {
//       continue;
//     }
//     EnterCriticalSection(&self->cs);
//     uint32_t currentIndex = self->methodIndex;
//     self->methodIndex++;
//     self->currentThreads++;
//     if(currentIndex >= self->atoms->size()) {
//       LeaveCriticalSection(&self->cs);
//       continue;
//     }
//     LeaveCriticalSection(&self->cs);
//     MethodDecl currentA = (*self->atoms)[currentIndex];
//     void (*cMethod)(PVOID) = (void (*)(PVOID))currentA.method;
//     cMethod(currentA.params);
//     EnterCriticalSection(&self->cs);
//     if(self->methodIndex >= self->atoms->size()) {
//       self->totalThreadsRun++;
//     }
//     self->currentThreads--;
//     LeaveCriticalSection(&self->cs);
//   }
// }
void _threadAtom(PVOID selfBuffer) {
  PSuperThread self = (PSuperThread)selfBuffer;
  while(1) {
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
  }
}

void thr_Terminate(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  self->shouldThreadClose = 1;
  LeaveCriticalSection(&self->cs);
  while(1) {
    EnterCriticalSection(&self->cs);
    uint8_t currentCloseCount = self->closeThreadsClosed;
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
  while(self->atoms->size());
}

void thr_Execute(PSuperThread self) {
  EnterCriticalSection(&self->cs);
  self->started = 1;
  LeaveCriticalSection(&self->cs);
}

