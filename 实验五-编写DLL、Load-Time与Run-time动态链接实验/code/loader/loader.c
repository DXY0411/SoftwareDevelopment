#include <windows.h> 
#include <stdio.h> 

typedef int(WINAPI *MYPROC)(char *);

int main(void)
{
	HMODULE hBaselib = LoadLibrary("baselib.dll");

	if (hBaselib == NULL)
		return 0;

	MYPROC func = (MYPROC)GetProcAddress(hBaselib, "lib_function");
	func("run time load");
	system("pause");
}

