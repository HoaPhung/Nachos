#include "syscall.h"
#include "copyright.h"

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
	Write("Welcome to Echo application!\n",255,ConsoleOutput);
	Write("Type \"exit\" to exit!\n",255,ConsoleOutput);
	char str[255];
	while(1){
		int len = Read(str, 255, ConsoleInput);
		if(strequal(str,"exit")) break;
		Write(str, 255, ConsoleOutput);
		Write("\n", 255, ConsoleOutput);
	}
	Exit(0);
}
