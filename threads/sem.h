#ifndef SEM_H
#define SEM_H

#include "synch.h"
#include "thread.h"

class Sem {
private:
  char name[50];
  Semaphore *sem;
public:
  Sem(char* na, int i);

  ~Sem();

  void wait();

  void signal();

  char* GetName();
};

#endif // SEM_H
