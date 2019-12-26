#include <windows.h>
LONG IATHook
(
	__in void* pImageBase,
	__in_opt char* pszImportDllName,
	__in char* pszRoutineName,
	__in void* pFakeRoutine,
	__out HANDLE* Param_phHook
);

void* GetIATHookOrign(__in HANDLE hHook);
typedef int(__stdcall *LPFN_WriteFile)(__in_opt HANDLE hFile, __in_opt LPCVOID lpBuffer, __in_opt DWORD nNumberOfBytesToWrite, __in LPDWORD lpNumberOfBytesWritten, __in LPOVERLAPPED lpOverlapped);
HANDLE g_hHook_WriteFile = NULL;
//////////////////////////////////////////////////////////////////////////
//BOOL WriteFile(
//	HANDLE  hFile,//文件句柄
//	LPCVOID lpBuffer,//数据缓存区指针
//	DWORD   nNumberOfBytesToWrite,//要写的字节数
//	LPDWORD lpNumberOfBytesWritten,//用于保存实际写入字节数的存储区域的指针
//	LPOVERLAPPED lpOverlapped//OVERLAPPED结构体指针
//);

int __stdcall Fake_WriteFile(HANDLE hFile, __in_opt LPCVOID lpBuffer, __in_opt DWORD nNumberOfBytesToWrite, __in LPDWORD lpNumberOfBytesWritten, __in LPOVERLAPPED lpOverlapped)
{
	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);  //原函数指针
	wchar_t *s = lpBuffer;
	wchar_t re[100];
	for(int i=0;i<strlen(s);i++)
		re[i] = s[i];
	for (int i = 0; i < strlen(s); i++)
	{
		wchar_t * t = L"呵";
		wchar_t * m = L"哈";
		if (re[i]== t[0])
			re[i] = m[0];
	}//将wchar_t*转化成char*
	char *end = (char *)malloc(nNumberOfBytesToWrite);  
	WideCharToMultiByte(CP_ACP, NULL, re, -1, end, nNumberOfBytesToWrite, NULL, FALSE);
	return fnOrigin(hFile, end, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

int __cdecl wmain()
{
	do
	{
		IATHook(
			GetModuleHandleW(NULL),//当前进程的句柄 基地址
			"kernel32.dll",        //目标dll名
			"WriteFile",           //目标函数名
			Fake_WriteFile,        //替换的函数指针
			&g_hHook_WriteFile     //返回修改后的IATHOOK_BLOCK PE对象
		);
		DWORD RSize=0;
		HANDLE m_hFile = (HANDLE)CreateFile("C:\\Users\\PC\\Desktop\\fakewrite.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		DWORD fileSize = GetFileSize(m_hFile, NULL);
		char* pBuffer = (char*)malloc(fileSize);
		ZeroMemory(pBuffer, fileSize);
		ReadFile(m_hFile, pBuffer, fileSize, &RSize, NULL);
		//将char*转化成wchar_t*
		wchar_t *pwstr = (wchar_t *)malloc(fileSize);
		MultiByteToWideChar(CP_ACP, 0, pBuffer, -1, pwstr, fileSize);
		CloseHandle(m_hFile);

		//修改并写入
		m_hFile = (HANDLE)CreateFile("C:\\Users\\PC\\Desktop\\fakewrite.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
		WriteFile(m_hFile, pwstr, fileSize, &RSize, NULL);
		CloseHandle(m_hFile);    //使用完要关闭文件句柄
	} while (FALSE);
	return 0;
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)   // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		wmain();
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
