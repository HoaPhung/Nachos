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
		Halt();
		return 0;
	}
	
	Write("Output file: ",255,ConsoleOutput);
	Read(path, 255, ConsoleInput);
	if(CreateFile(path)==-1){
		Write("Can not create file!\n", 255, ConsoleOutput);
		Halt();
		return 0;
	}
	fileOut = Open(path,0);
	if(fileOut==-1){
		Write("Can not create file!\n", 255, ConsoleOutput);
		Halt();
		return 0;
	}
	
	char str[255];
	int len = Read(str, 255, fileIn);
	--len;
	int i;
	for(i=0; i<len/2; ++i){
		char t=str[i];
		str[i]=str[len-i-1];
		str[len-i-1]=t;
	}
	Write(str,len+1,fileOut);
	Write("File reverse created!", 255, ConsoleOutput);
	
	Halt();
	return 0;
} 
