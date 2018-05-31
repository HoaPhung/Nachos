#include "syscall.h"

int strequal(char *a, char *b){
	int i=0;
	while(1){
		if(a[i]!=b[i]) return 0;
		if(a[i]==0) return 1;
		++i;
	}
}

int main(int argnum, char **arg)
{
  SpaceId newProc;
	Write("Welcome to Shell application!\n", 255, ConsoleOutput);
	Write("Type filename of the program to run,\n", 255, ConsoleOutput);
	Write("or type \"exit\" to exit!\n", 255, ConsoleOutput);
	char str[255];
	while(1){
		Write("$ ", 255, ConsoleOutput);
		int len = Read(str, 255, ConsoleInput);
		if (strequal(str,"exit")) break;
    OpenFileId file = Open(str, 1);
    if (file == -1) {
      Write("ERROR: Find not found!\n", 255, ConsoleOutput);
      continue;
    } else {
      Write("Executing app: ", 255, ConsoleOutput);
	    Write(str, 255, ConsoleOutput);
	    Write("\n================================================\n", 255, ConsoleOutput);
      newProc = Exec(str, 0, 0, 1);
      Join(newProc);
	    Write("\n=====================DONE=======================\n", 255, ConsoleOutput);
    }
	}
	Exit(0);
}
