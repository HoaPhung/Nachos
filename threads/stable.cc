#include "stable.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"

STable::STable() {
  bm = new BitMap(MAX_SEMAPHORE);
  for(int i=0; i<MAX_SEMAPHORE; ++i) semTab[i]=NULL;
}

STable::~STable() {
  delete bm;
  for(int i=0; i<MAX_SEMAPHORE; ++i) if (semTab[i]!=NULL) delete semTab[i];
}

int STable::getIndexWithName(char *name) {
  for(int i=0; i<MAX_SEMAPHORE; ++i)
    if (semTab[i]!=NULL && strcmp(name,semTab[i]->GetName()) == 0) return i;
  return -1;
}

int STable::Create(char* name, int init) {
  if (getIndexWithName(name) != -1) {
    printf("Duplicated name!\n");
    return -1;
  }
  int id = FindFreeSlot();
  if (id == -1) {
    printf("No free slots!\n");
    return -1;
  }
  semTab[id] = new Sem(name, init);
  return 0;
}

int STable::Wait(char* name) {
  int id = getIndexWithName(name);
  if (id==-1) {
    printf("Name not found!\n");
    return -1;
  }
  semTab[id]->wait();
  return 0;
}

int STable::Signal(char* name) {
  int id = getIndexWithName(name);
  if (id==-1) {
    printf("Name not found!\n");
    return -1;
  }
  semTab[id]->signal();
  return 0;
}

int STable::FindFreeSlot() {
  return bm->Find();
}
