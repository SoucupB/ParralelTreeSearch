#include "SuperThread.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

void _spawnWorkers(PSuperThread self);

PSuperThread thr_Create(int32_t totalThreadsSpawn) {
  PSuperThread self = (PSuperThread)malloc(sizeof(SuperThread));
  self->atoms = new Vects();
  self->threadIDs = new VectINT();
  self->totalThreadsSpawn = totalThreadsSpawn;
  _spawnWorkers(self);
  return self;
}

void _threadChildPointer(PVOID buffer) {
}

void _spawnWorkers(PSuperThread self) {
  
  HANDLE hThread;
  DWORD threadId;
  hThread = CreateThread(
    NULL,
    0,
    (LPTHREAD_START_ROUTINE)_threadChildPointer,
    (PVOID)self,
    0,
    &threadId
  );
}