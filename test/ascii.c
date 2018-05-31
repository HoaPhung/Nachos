#include "syscall.h"
#include "copyright.h"

int main(){
	int i;
	PrintString("ASCII Character map:\n");
	//just print out printable character from 32 to 126
	for(i=32; i<127; ++i){
		PrintString(" - ASCII Code: "); PrintInt(i); PrintString(" as character: "); PrintChar(i); PrintString("\n");
	}
	PrintString("-------------------------\n");
	Exit(0);
}
