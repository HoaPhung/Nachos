// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------


//variables
SynchConsole *synchConsole;

void InitExceptions(){
	synchConsole = new SynchConsole();
}

// increment the PC
void IncrementPC()
{
	//in machine.h...
	//PCReg for current program counter
	//PrevPCReg for previous program counter
	//NextPCReg for next program counter

	//current pc set to prev
	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);

	//next pc set to current
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);

	// plus 4 for next pc
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}

char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit+1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);
	return i;
}

const int maxFileCnt = 10;
OpenFile* fileTable[maxFileCnt];
int fileTableID[maxFileCnt], filePos[maxFileCnt], fileCnt = 0;
bool readOnly[maxFileCnt];

int putFile(OpenFile* f, int type){
	for(int i=0; i<maxFileCnt; ++i) if(!fileTableID[i]){
		fileTableID[i] = 1;
		fileTable[i] = f;
		filePos[i] = 0;
		readOnly[i] = type;
		++fileCnt;
		return i;
	}
	return -1;
}

void removeFile(int fileID){
	if(fileID<0 || fileID>=maxFileCnt) return;
	if(fileTableID[fileID]) --fileCnt;
	fileTable[fileID] = 0;
	fileTableID[fileID] = 0;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
	if(synchConsole==NULL) synchConsole = new SynchConsole();
	if(fileCnt == 0) //init fileTable
	{
		for(int i=0; i<10; ++i) fileTable[i] = 0;
		fileTableID[0] = 1;
		fileTableID[1] = 1;
		fileCnt = 2;
	}
	int MaxFileLength = 50;
	int MaxNumLength = 20;
	int MaxStringLength = 255;

	switch (which) {
		case NoException:
			return;
		case PageFaultException:
			printf("\n Pauge Fault Exception");
			break;
		case ReadOnlyException:
			printf("ReadOnly Exception");
			break;
		case BusErrorException:
			printf("Bus Error Exception");
			break;
		case AddressErrorException:
			printf("Address Error Exception");
			break;
		case OverflowException:
			printf("Overflow Exception");
			break;
		case IllegalInstrException:
			printf("Illegal Instruction Exception");
			break;
		case NumExceptionTypes:
			printf("Number Exception Type");
			break;
		case SyscallException:
		{
			switch (type){
			case SC_Halt:
			{
				DEBUG('a', "\n Shutdown, initiated by user program.");
				printf ("\n\n Shutdown, initiated by user program.");
				interrupt->Halt();
				break;
			}
			case SC_CreateFile:
			{
				IncrementPC();
				int virtAddr;
				char* filename;
				DEBUG('a',"\n Reading virtual address of filename");
				virtAddr = machine->ReadRegister(4);
				DEBUG ('a',"\n Reading filename.");
				filename = User2System(virtAddr,MaxFileLength+1);
				if (filename == NULL)
				{
					printf("\n Not enough memory in system");
					DEBUG('a',"\n Not enough memory in system");
					machine->WriteRegister(2,-1);
					delete filename;
					break;
				}
				DEBUG('a',"\n Finish reading filename.");
				if (!fileSystem->Create(filename,0))
				{
					printf("\n Error create file '%s'",filename);
					machine->WriteRegister(2,-1);
					delete filename;
					break;
				}
				machine->WriteRegister(2,0);
				delete filename;
				break;
			}
			case SC_Open:
			{
				//increase the PC
				IncrementPC();

				int virtAddr, ftype;
				char* filename;

				//read filename and type variables
				virtAddr = machine->ReadRegister(4);
				filename = User2System(virtAddr,MaxFileLength+1);
				if (filename == NULL)
				{
					machine->WriteRegister(2,-1);
					delete filename;
					break;
				}
				ftype = machine->ReadRegister(5);

				//put to filetable
				OpenFile* file = fileSystem->Open(filename);
				if(file==NULL){
					//if error then exit
					machine->WriteRegister(2,-1);
					delete filename;
					break;
				}

				int fileId = putFile(file, ftype);
				if(fileId == -1){
					//if error then exit
					machine->WriteRegister(2,-1);
					delete filename;
					break;
				}

				machine->WriteRegister(2,fileId);

				delete filename;
				break;
			}
			case SC_Close:
			{
				//increase the PC
				IncrementPC();
				//get file Id
				int fileId = machine->ReadRegister(4);
				if(fileId < 0 || fileId >= maxFileCnt || !fileTableID[fileId]){
					//error in fileId input
					printf("\n fileID not found!");
					DEBUG('a',"\n fileID not found!");
					machine->WriteRegister(2,-1);
					break;
				}
				removeFile(fileId);
				break;
			}
			case SC_Read:
			{
				//increase the PC
				IncrementPC();

				//get the address and the max length of the string for reading
				int addr = machine->ReadRegister(4);
				int maxLength = machine->ReadRegister(5);
				int fileId = machine->ReadRegister(6);

				if(addr == 0){
					printf("\n Address not found!");
					DEBUG('a',"\n Address not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				if(fileId < 0 || fileId >= maxFileCnt || !fileTableID[fileId]){
					//error in fileId input
					printf("\n fileID not found!");
					DEBUG('a',"\n fileID not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				char *str = new char[maxLength+1];
				if(fileId < 2){
					//get the string from console

					int len = synchConsole->Read(str, maxLength);
					if(len==-1){
						printf("\n Reading error");
						DEBUG('a', "\n Reading error");
						machine->WriteRegister(2,-1);
						delete str;
						break;
					}
					str[len++]='\0';

					//put it to the address of user's variable
					int result = System2User(addr, len, str);

					//return the length of the input string
					machine->WriteRegister(2,result);
				}else{
					//get the string from file
					OpenFile *file = fileTable[fileId];

					if(filePos[fileId] >= file->Length()){
						DEBUG('a', "\n End of file");
						machine->WriteRegister(2,-2);
						delete str;
						break;
					}

					int len = file->ReadAt(str, maxLength, filePos[fileId]);
					if(len==-1){
						printf("\n Reading error");
						DEBUG('a', "\n Reading error");
						machine->WriteRegister(2,-1);
						delete str;
						break;
					}
					filePos[fileId] += len;

					//put it to the address of user's variable
					int result = System2User(addr, len, str);

					//return the length of the input string
					machine->WriteRegister(2,result);
				}

				delete str;

				break;
			}
			case SC_Write:
			{
				//increase the PC
				IncrementPC();

				//get address of the output string
				int addr = machine->ReadRegister(4);
				int maxLength = machine->ReadRegister(5);
				int fileId = machine->ReadRegister(6);

				if(addr == 0){
					printf("\n Address not found!");
					DEBUG('a',"\n Address not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				if(fileId < 0 || fileId >= maxFileCnt || !fileTableID[fileId]){
					//error in fileId input
					printf("\n fileID not found!");
					DEBUG('a',"\n fileID not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				char *str;

				//read the string
				str = User2System(addr, maxLength);
				if (str == NULL)
				{
					printf("\n Not enough memory in system");
					DEBUG('a',"\n Not enough memory in system");
					machine->WriteRegister(2,-1);
					break;
				}

				//count the length of the string
				int len = 0;
				while(str[len]!=0) ++len;

				if(fileId<2)
				{
					//write to console
					len = synchConsole->Write(str, len);
				}else{

					if(readOnly[fileId])
					{
						//not for writing
						printf("\n can not write on readonly file");
						DEBUG('a',"\n can not write on readonly file");
						machine->WriteRegister(2,-1);
						delete str;
						break;
					}

					//write to file
					OpenFile *file = fileTable[fileId];
					if(file==NULL){
						printf("\n FileID Not found");
						DEBUG('a', "\n FileID Not found");
						machine->WriteRegister(2,-1);
						delete str;
						break;
					}
					len = file->WriteAt(str, len, filePos[fileId]);
					filePos[fileId] += len;
				}

				machine->WriteRegister(2,len);

				delete str;
				break;
			}
			case SC_Seek:
			{
				//increase the PC
				IncrementPC();

				int pos = machine->ReadRegister(4);
				int fileId = machine->ReadRegister(5);

				if(fileId < 0 || fileId >= maxFileCnt || !fileTableID[fileId]){
					//error in fileId input
					printf("\n fileID not found!");
					DEBUG('a',"\n fileID not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				OpenFile *file = fileTable[fileId];
				if(file==NULL){
					printf("\n FileID Not found");
					DEBUG('a', "\n FileID Not found");
					machine->WriteRegister(2,-1);
					break;
				}


				if(pos == -1){
					//seek to EOF

					filePos[fileId] = file->Length();
					machine->WriteRegister(2,filePos[fileId]);

				}else{

					if(pos < 0 || pos>file->Length()){
						printf("\n Seek error!");
						DEBUG('a',"\n Seek error!");
						machine->WriteRegister(2,-1);
						break;
					}
					//seek to desired position
					filePos[fileId] = pos;
					machine->WriteRegister(2,filePos[fileId]);

				}

				break;
			}

			case SC_Exit:
			{
				//increase the PC
				IncrementPC();

				int val = machine->ReadRegister(4);
				int res = pTab->ExitUpdate(val);
			 	machine->WriteRegister(2, res);

				break;
			}

			case SC_Exec:
			{
				//increase the PC
				IncrementPC();

				int virtAddr = machine->ReadRegister(4);
				if (virtAddr == 0) {
					machine->WriteRegister(2, -1);
					return;
				}
				char* filename = User2System(virtAddr, MaxFileLength+1);
				int res = pTab->ExecUpdate(filename);
			 	machine->WriteRegister(2, res);

				break;
			}

			case SC_Join:
			{
				//increase the PC
				IncrementPC();

				int val = machine->ReadRegister(4);
				int res = pTab->JoinUpdate(val);
				machine->WriteRegister(2, res);

				break;
			}

			case SC_CreateSemaphore:
			{
				//increase the PC
				IncrementPC();

				int virtAddr = machine->ReadRegister(4);
				char* name = User2System(virtAddr, MaxFileLength+1);
				int semval = machine->ReadRegister(5);

				int res = semTab->Create(name, semval);
				machine->WriteRegister(2, res);
				break;
			}

			case SC_Up:
			{
				//increase the PC
				IncrementPC();

				int virtAddr = machine->ReadRegister(4);
				char* name = User2System(virtAddr, MaxFileLength+1);
				int res = semTab->Signal(name);
				machine->WriteRegister(2, res);
				break;
			}

			case SC_Down:
			{
				//increase the PC
				IncrementPC();

				int virtAddr = machine->ReadRegister(4);
				char* name = User2System(virtAddr, MaxFileLength+1);
				int res = semTab->Wait(name);
				machine->WriteRegister(2, res);
				break;
			}

			case SC_Fork:
				break;
			case SC_Yield:
				break;

			case SC_ReadInt:
			{
				//increase the PC
				IncrementPC();
				char* charnum = new char[MaxNumLength];

				//read from the console
				int len = synchConsole->Read(charnum, MaxNumLength);
				if(len==-1){
					printf("\n Reading error");
					DEBUG('a', "\n Reading error");
					machine->WriteRegister(2,-1);
					delete charnum;
					break;
				}

				int val = 0, sign = 1;
				//convert char array to number, add value to 'val'
				for(int i=0; i<len; ++i){
					char ch = charnum[i];
					if('0' <= ch && ch <= '9'){
						val = val*10 + ch-'0';
					}else if(ch=='-' && i==0){
						sign = -1;
					}else{
						//Not a number, return 0
						machine->WriteRegister(2,0);
						delete charnum;
						break;
					}
				}

				//return the value
				val *= sign;
				machine->WriteRegister(2,val);
				delete charnum;

				break;

			}

			case SC_PrintInt:
			{
				//increase the PC
				IncrementPC();
				int val = machine->ReadRegister(4);

				char *charNum = new char[MaxNumLength];
				int startIdx = 0;

				//for negative number
				if(val < 0){
					charNum[startIdx++] = '-';
					val *= -1;
				}

				//convert number to array of numbers
				int *arr = new int[MaxNumLength];
				int len=0;
				while(val || len==0){
					arr[len++] = val%10;
					val/=10;
				}

				//put it to the array of char
				for(int i=0; i<len; ++i) charNum[startIdx++] = arr[len-i-1] + '0';

				//print out to the console
				synchConsole->Write(charNum, startIdx);

				//return 0
				machine->WriteRegister(2,0);

				delete charNum;
				break;
			}

			case SC_ReadChar:
			{
				//increase the PC
				IncrementPC();
				char* arr = new char[2];

				//read from the console
				int len = synchConsole->Read(arr, 2);
				if(len==-1){
					printf("\n Reading error");
					DEBUG('a', "\n Reading error");
					machine->WriteRegister(2,-1);
					break;
				}

				//return the character
				machine->WriteRegister(2,arr[0]);


				break;
			}

			case SC_PrintChar:
			{
				//increase the PC
				IncrementPC();

				//get the character for printing out
				int val = machine->ReadRegister(4);
				char *arr = new char[1];
				if(arr==NULL){
					printf("\n Not enough memory!");
					DEBUG('a',"\n Not enough memory!");
					machine->WriteRegister(2,-1);
					break;
				}

				//put it to the char array
				arr[0] = val;

				//print out to the console
				synchConsole->Write(arr, 1);

				//return 0
				machine->WriteRegister(2,0);

				delete arr;
				break;
			}

			case SC_ReadString:
			{
				//increase the PC
				IncrementPC();

				//get the address and the max length of the string for reading
				int addr = machine->ReadRegister(4);
				int maxLength = machine->ReadRegister(5);

				if(addr == 0){
					printf("\n Address not found!");
					DEBUG('a',"\n Address not found!");
					machine->WriteRegister(2,-1);
					break;
				}

				char *str = new char[maxLength+1];
				//get the string from console
				int len = synchConsole->Read(str, maxLength);
				if(len==-1){
					printf("\n Reading error");
					DEBUG('a', "\n Reading error");
					machine->WriteRegister(2,-1);
					delete str;
					break;
				}
				str[len++]='\0';
				//put it to the address of user's variable
				int result = System2User(addr, len, str);

				//return the length of the input string
				machine->WriteRegister(2,result);
				delete str;

				break;
			}

			case SC_PrintString:
			{
				//increase the PC
				IncrementPC();

				//get address of the output string
				int addr = machine->ReadRegister(4);
				char *str;

				//read the string
				str = User2System(addr, MaxStringLength+1);
				if (str == NULL)
				{
					printf("\n Not enough memory in system");
					DEBUG('a',"\n Not enough memory in system");
					machine->WriteRegister(2,-1);
					break;
				}

				//count the length of the string
				int len = 0;
				while(str[len]!=0) ++len;
				synchConsole->Write(str, len);

				//return the length of the string
				machine->WriteRegister(2,len);

				delete str;
				break;
			}
			default:
				printf("\n Unexpected user modeexception (%d %d)", which, type);
				interrupt->Halt();
			}
		}
	}


}
