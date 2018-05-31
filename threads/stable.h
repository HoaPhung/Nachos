#ifndef STABLE_H
#define STABLE_H

#include "synch.h"
#include "thread.h"
#include "sem.h"
#include "bitmap.h"

#define MAX_SEMAPHORE 10

class STable {
private:
  BitMap* bm;
  Sem* semTab[MAX_SEMAPHORE];
  int getIndexWithName(char *name);
public:
  STable();
  ~STable();
  int Create(char* name, int init);
  int Wait(char* name);
  int Signal(char* name);
  int FindFreeSlot();
};

#endif // STABLE_H
