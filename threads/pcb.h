#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

#define FILENAME_LEN 100

class PCB {
private:
  Semaphore* joinsem;
  Semaphore* exitsem;
  Semaphore* mutex;
  int exitcode;
  int numwait;
  int parentId, processId;
  char filename[FILENAME_LEN];
  Thread *thread;
public:
  int parentID;
  PCB();
  PCB(int id);
  ~PCB();
  int Exec(char *_filename, int _processId, int _parentId);
  int GetID();
  int GetParentID();
  void SetFileName(char* fn);
  char* GetFileName();
  void SetExitCode(int ec);
  int GetExitCode();
  int GetNumWait();
  void IncNumWait();
  void DecNumWait();
  void JoinWait();
  void ExitWait();
  void JoinRelease();
  void ExitRelease();
  Thread* GetThread();

};

#endif // PCB_H
