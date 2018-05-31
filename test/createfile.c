#include "syscall.h"
#include "copyright.h"

int main(int argnum, char **arg)
{
	Write("Input the file name: ",50,ConsoleOutput);
	char filename[50];
	Read(filename, 50, ConsoleInput);
	CreateFile(filename);
	
	PrintString("File \""); PrintString(filename); PrintString("\" created\n");
	Halt();
} 
