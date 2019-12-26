#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int hacked()
{
	printf("hacked!!");
}

int sub(char* x)
{
	char y[10];
	strcpy(y, x);
	return 0;
}
//hack函数的printf操作的地址为：213115C9
char * ov = "1234123512361237\xc9\x15\x31\x21";
int main(int argc, char** argv)
{
	if (argc > 1)
		sub(ov);
	printf("exit");
}