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
//	HANDLE  hFile,//�ļ����
//	LPCVOID lpBuffer,//���ݻ�����ָ��
//	DWORD   nNumberOfBytesToWrite,//Ҫд���ֽ���
//	LPDWORD lpNumberOfBytesWritten,//���ڱ���ʵ��д���ֽ����Ĵ洢�����ָ��
//	LPOVERLAPPED lpOverlapped//OVERLAPPED�ṹ��ָ��
//);

int __stdcall Fake_WriteFile(HANDLE hFile, __in_opt LPCVOID lpBuffer, __in_opt DWORD nNumberOfBytesToWrite, __in LPDWORD lpNumberOfBytesWritten, __in LPOVERLAPPED lpOverlapped)
{
	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);  //ԭ����ָ��
	wchar_t *s = lpBuffer;
	wchar_t re[100];
	for(int i=0;i<strlen(s);i++)
		re[i] = s[i];
	for (int i = 0; i < strlen(s); i++)
	{
		wchar_t * t = L"��";
		wchar_t * m = L"��";
		if (re[i]== t[0])
			re[i] = m[0];
	}//��wchar_t*ת����char*
	char *end = (char *)malloc(nNumberOfBytesToWrite);  
	WideCharToMultiByte(CP_ACP, NULL, re, -1, end, nNumberOfBytesToWrite, NULL, FALSE);
	return fnOrigin(hFile, end, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

int __cdecl wmain()
{
	do
	{
		IATHook(
			GetModuleHandleW(NULL),//��ǰ���̵ľ�� ����ַ
			"kernel32.dll",        //Ŀ��dll��
			"WriteFile",           //Ŀ�꺯����
			Fake_WriteFile,        //�滻�ĺ���ָ��
			&g_hHook_WriteFile     //�����޸ĺ��IATHOOK_BLOCK PE����
		);
		DWORD RSize=0;
		HANDLE m_hFile = (HANDLE)CreateFile("C:\\Users\\PC\\Desktop\\fakewrite.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		DWORD fileSize = GetFileSize(m_hFile, NULL);
		char* pBuffer = (char*)malloc(fileSize);
		ZeroMemory(pBuffer, fileSize);
		ReadFile(m_hFile, pBuffer, fileSize, &RSize, NULL);
		//��char*ת����wchar_t*
		wchar_t *pwstr = (wchar_t *)malloc(fileSize);
		MultiByteToWideChar(CP_ACP, 0, pBuffer, -1, pwstr, fileSize);
		CloseHandle(m_hFile);

		//�޸Ĳ�д��
		m_hFile = (HANDLE)CreateFile("C:\\Users\\PC\\Desktop\\fakewrite.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
		WriteFile(m_hFile, pwstr, fileSize, &RSize, NULL);
		CloseHandle(m_hFile);    //ʹ����Ҫ�ر��ļ����
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
