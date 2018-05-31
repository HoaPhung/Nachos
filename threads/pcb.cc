#include "pcb.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"

PCB::PCB() {
  joinsem = new Semaphore("joinsem", 0);
  exitsem = new Semaphore("exitsem", 0);
  mutex = new Semaphore("mutex", 1);
  parentId = processId = -1;
  thread = NULL;
  memset(filename, 0, FILENAME_LEN);
  numwait = exitcode = 0;
}

PCB::PCB(int id) {
  PCB();
  processId = id;
}

PCB::~PCB() {
  delete joinsem;
  delete exitsem;
  delete mutex;
}

void exec_func(int) {
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->Run();
}

int PCB::Exec(char *_filename, int _processId, int _parentId) {
  mutex->P();
  thread = new Thread(_filename);
  if (thread == NULL) {
    printf("Not enought memory!\n");
    mutex->V();
    return -1;
  }
  OpenFile *executable = fileSystem->Open(_filename);
  if (executable == NULL) {
    printf("Excutable not found!\n");
    mutex->V();
    return -1;
  }
  thread->space = new AddrSpace(executable);
  this->processId = _processId;
  this->parentId = _parentId;
  strcpy(this->filename, _filename);

  // If this is the first thread to run
  if (parentId == -1) {
    Thread *tmpthread = currentThread;
    currentThread = thread;
    currentThread->setStatus(RUNNING);
    delete tmpthread;
    thread->space->InitRegisters();
    thread->space->RestoreState();
  } else {
    thread->Fork(exec_func, 0);
  }
  mutex->V();
  return this->processId;
}

int PCB::GetID() {
  return processId;
}

int PCB::GetParentID() {
  return parentId;
}

void PCB::SetFileName(char* fn) {
  strcpy(this->filename, fn);
}

char* PCB::GetFileName() {
  return this->filename;
}

void PCB::SetExitCode(int ec) {
  exitcode = ec;
}

int PCB::GetExitCode() {
  return exitcode;
}

int PCB::GetNumWait() {
  return numwait;
}

void PCB::IncNumWait() {
  ++numwait;
}

void PCB::DecNumWait() {
  if (numwait > 0) --numwait;
}

void PCB::JoinWait() {
  joinsem->P();
}

void PCB::ExitWait() {
  exitsem->P();
}

void PCB::JoinRelease() {
  joinsem->V();
}

void PCB::ExitRelease() {
  exitsem->V();
}


Thread* PCB::GetThread() {
  return thread;
}
