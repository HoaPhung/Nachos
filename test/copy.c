#include "syscall.h"
#include "copyright.h"

int main(int argnum, char **arg)
{
	OpenFileId fileIn=-1, fileOut=-1;
	char path[255];
	Write("Input file: ",255,ConsoleOutput);
	Read(path, 255, ConsoleInput);
	fileIn = Open(path, 1);
	if(fileIn==-1){
		Write("File not found!\n",255,ConsoleOutput);
		Exit(0);
		return 0;
	}

	Write("Output file: ",255,ConsoleOutput);
	Read(path, 255, ConsoleInput);
	if(CreateFile(path)==-1){
		Write("Can not create file!\n", 255, ConsoleOutput);
		Exit(0);
		return 0;
	}
	fileOut = Open(path,0);
	if(fileOut==-1){
		Write("Can not create file!\n", 255, ConsoleOutput);
		Exit(0);
		return 0;
	}

	char str[255];
	while(Read(str, 255, fileIn)!=-2){
		Write(str,255,fileOut);
	}
	Write("File copied!", 255, ConsoleOutput);

	Exit(0);
	return 0;
}
