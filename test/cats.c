#include "syscall.h"
#include "copyright.h"

int main(int argnum, char **arg)
{
	OpenFileId file=-1;
	if(argnum==0){
		char path[255];
		Write("Input filename: ",255,ConsoleOutput);
		Read(path, 255, ConsoleInput);
		file = Open(path, 1);
	}else{
		file = Open(arg[0], 1);
	}
	if(file==-1){
		Write("File not found!\n",255,ConsoleOutput);
		Exit(0);
		return 0;
	}

	int skip = 0;
	Write("Number of skip characters: ", 255, ConsoleOutput);
	skip = ReadInt();
	Seek(skip, file);
	Write("==========File content==========\n",255,ConsoleOutput);

	char str[255];
	while(Read(str, 255, file)!=-2){
		Write(str,255,ConsoleOutput);
	}
	Write("================================\n",255,ConsoleOutput);

	Exit(0);
	return 0;
}
