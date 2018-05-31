#include "ptable.h"
#include "system.h"

PTable::PTable() {
  bm = new BitMap(MAX_PROCESS);
  bmsem = new Semaphore("bmsem", 1);
  for (int i=0; i<MAX_PROCESS; ++i) pcb[i]=NULL;
}

PTable::~PTable() {
  delete bm;
  delete bmsem;
  for (int i=0; i<MAX_PROCESS; ++i)
    if (pcb[i]!=NULL) delete pcb[i];
}

int PTable::ExecUpdate(char* name) {
  bmsem->P();
  // Check valid file name
  if (name==NULL || (currentThread != NULL && strcmp(currentThread->getName(),name) == 0)) {
    bmsem->V();
    return -1;
  }
  // Check valid execute file
  OpenFile *executable = fileSystem->Open(name);
  if (executable == NULL) {
    bmsem->V();
    return -1;
  }
  delete executable;
  int processId = 0, parentId = -1;
  if (bm->NumClear() == MAX_PROCESS) {
    bm->Mark(processId);
  } else {
    parentId = getIndexOfThread(currentThread);
    if (parentId ==-1) {
      bmsem->V();
      return -1;
    }
    processId = bm->Find();
  }
  if(processId == -1) {
    bmsem->V();
    return -1;
  }
  pcb[processId] = new PCB();
  int res = pcb[processId]->Exec(name, processId, parentId);
  bmsem->V();
  return res;
}

int PTable::JoinUpdate(int id) {
  if (id < 0 || id >= MAX_PROCESS || !bm->Test(id)) return -1;
  int parentId = getIndexOfThread(currentThread);
  if (parentId != pcb[id]->GetParentID()) return -1;
  pcb[parentId]->IncNumWait();
  pcb[parentId]->JoinWait();
  int ret = pcb[parentId]->GetExitCode();
  pcb[id]->ExitRelease();
  return ret;
}

int PTable::ExitUpdate(int ec) {
  int processId = getIndexOfThread(currentThread);
  if (processId == -1) return -1;
  int parentId = pcb[processId]->GetParentID();
  if (parentId == -1) {
    interrupt->Halt();
    return 0;
  }
  pcb[parentId]->SetExitCode(ec);
  pcb[parentId]->JoinRelease();
  pcb[parentId]->DecNumWait();
  pcb[processId]->ExitWait();
  delete pcb[processId];
  pcb[processId] = NULL;
  bm->Clear(processId);
  delete currentThread->space;
  currentThread->Finish();
  return 0;
}

int PTable::getIndexOfThread(Thread *thread) {
  if (thread == NULL) return -1;
  for (int i = 0; i < MAX_PROCESS; ++i) {
      if (pcb[i] != NULL && pcb[i]->GetThread() == thread) return i;
  }
  return -1;
}
