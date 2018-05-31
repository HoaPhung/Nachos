#ifndef PTABLE_H
#define PTABLE_H

#include "synch.h"
#include "thread.h"
#include "pcb.h"
#include "bitmap.h"

#define MAX_PROCESS 10

class PTable {
private:
  BitMap *bm;
  PCB* pcb[MAX_PROCESS];
  int psize;
  Semaphore* bmsem;
  int getIndexOfThread(Thread *thread);
public:
  PTable();
  ~PTable();
  int ExecUpdate(char* name);
  int ExitUpdate(int ec);
  int JoinUpdate(int id);
  int GetFreeSlot();
  bool IsExist(int pid);
  void Remove(int pid);
  char* GetFileName(int id);
};

#endif // PTABLE_H
