#include "SuperThread.h"
#include <windows.h>

void _threadAtom(PVOID selfBuffer);

PSuperThread thr_Create(int32_t threadsCount) {
  PSuperThread self = (PSuperThread)malloc(sizeof(SuperThread));
  self->atoms = new std::vector<MethodDecl>();
  self->threads = new std::vector<ThreadData>();
  self->threadsCount = threadsCount;
  return self;
}

void thr_StartThreads(PSuperThread self) {
  DWORD threadId;
  HANDLE hThread;

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

void _threadAtom(PVOID selfBuffer) {
  PSuperThread self = (PSuperThread)selfBuffer;
  while(1) {

  }
}

