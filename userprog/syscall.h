/* syscalls.h
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel.
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Fork		9
#define SC_Yield	10

//our defined values of system calls
#define SC_ReadInt		11
#define SC_PrintInt 	12
#define SC_ReadChar		13
#define SC_PrintChar 	14
#define SC_ReadString	15
#define SC_PrintString	16
#define SC_CreateSemaphore	18
#define SC_Up	        19
#define SC_Down	      20

#define ConsoleInput	0
#define ConsoleOutput	1

//Ass2
#define SC_CreateFile	4
#define SC_Open			5
#define SC_Read			6
#define SC_Write		7
#define SC_Close		8
#define SC_Seek			17

#ifndef IN_ASM

typedef int OpenFileId;

// Define for custom system calls
int ReadInt();
void PrintInt(int number);
char ReadChar();
void PrintChar(char character);
int ReadString(char* buffer, int length);
int PrintString(char* buffer);

// 2
int CreateFile(char *name);
OpenFileId Open(char *name, int type);
int Write(char *buffer, int size, OpenFileId id);
int Read(char *buffer, int size, OpenFileId id);
void Close(OpenFileId id);
int Seek(int pos, OpenFileId id);

// 3
int CreateSemaphore(char *name, int semval);
int Up(char *name);
int Down(char *name);


/* Stop Nachos, and print out performance stats */
void Halt();


/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the
 * address space identifier
 */
SpaceId Exec(char *name, int argc, char **argv, int opt);

/* Only return once the the user program "id" has finished.
 * Return the exit status.
 */
int Join(SpaceId id);









/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program.
 */

/* Fork a thread to run a procedure ("func") in the *same* address space
 * as the current thread.
 */
void Fork(void (*func)());

/* Yield the CPU to another runnable thread, whether in this address space
 * or not.
 */
void Yield();

#endif /* IN_ASM */

#endif /* SYSCALL_H */
