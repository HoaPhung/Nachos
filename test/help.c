#include "syscall.h"
#include "copyright.h"

int main(){
	PrintString("Welcome to Nachos!\n");
	PrintString("I am Phung Thi Hoa - Student ID: 1512186 \n");
	PrintString("I have written 2 programs that are requested in the assignment's description:\n");
	PrintString(" - ascii: Print the ascii character map.\n");
	PrintString("   To Run: + Go to the folder \"./nachos/code/\".\n");
	PrintString("           + Type: \"./userprog/nachos -rs 1023 -x ./test/ascii\"\n");
	PrintString(" - sort: Input n number and it will sort those numbers in increasing order\n");

	PrintString("   To Run: + Go to the folder \"./nachos/code/\".\n");
	PrintString("           + Type: \"./userprog/nachos -rs 1023 -x ./test/sort\"\n");
	PrintString("           + Input number n, then following by n numbers\n");
	PrintString("----------------------------------------\n");
	PrintString("Have a nice day!\n");

	Exit(0);
	return 0;

}
