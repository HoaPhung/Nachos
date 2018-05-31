#include "syscall.h"

int main(){
	int n, i, j;
	int a[100];

	//input number n
	PrintString("Input n: ");
	n = ReadInt();

	//check for value n
	while(n < 1 || n > 100){
		PrintString("ERROR: n must between 1 and 100\n");
		PrintString("Input n: ");
		n = ReadInt();
	}

	//input value of elements
	for(i = 0; i<n; ++i){
		PrintString(" - Input number "); PrintInt(i+1); PrintString("-th: ");
		a[i] = ReadInt();
	}

	//sort with bubble algorithm
	for(i = 0; i < n; ++i)
		for(j = n-1; j>i; --j)
			if(a[j-1] > a[j]){
				int t = a[j-1];
				a[j-1] = a[j];
				a[j] = t;
			}

	//print out the result
	PrintString("Sorted array: ");
	for(i = 0; i<n; ++i){
		PrintInt(a[i]);
		PrintChar(' ');
	}
	PrintChar('\n');

	Exit(0);
}
