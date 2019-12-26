#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#define MAX_THREADS 10
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);

typedef struct MyData {
	int val1;
	int val2;
} MYDATA, *PMYDATA;

int _tmain()
{
	PMYDATA pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];

	DWORD start = GetTickCount();

	for (int i = 0; i<MAX_THREADS; i++)
	{
		pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(MYDATA));

		if (pDataArray[i] == NULL)
		{
			ExitProcess(2);
		}

		pDataArray[i]->val1 = i;
		pDataArray[i]->val2 = i + 100;
		MyThreadFunction(pDataArray[i]);

	} // End of main thread creation loop.
	DWORD end = GetTickCount();
	printf("tick count:%d\n", end - start);
	system("pause");
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	PMYDATA pDataArray;
	pDataArray = (PMYDATA)lpParam;
	Sleep(1000);
	printf("Parameters=%d,%d\n", pDataArray->val1, pDataArray->val2);
	return 0;
}

void ErrorHandler(LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

}