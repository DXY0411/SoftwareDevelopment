#include <windows.h>
LONG IATHook
(
	__in void* pImageBase,
	__in_opt char* pszImportDllName,
	__in char* pszRoutineName,
	__in void* pFakeRoutine,
	__out HANDLE* Param_phHook
);

LONG UnIATHook( __in HANDLE hHook );

void* GetIATHookOrign( __in HANDLE hHook );

typedef int (__stdcall *LPFN_MessageBoxA)( __in_opt HWND hWnd , __in_opt char* lpText , __in_opt char* lpCaption , __in UINT uType);

HANDLE g_hHook_MessageBoxA = NULL;
//////////////////////////////////////////////////////////////////////////

int __stdcall Fake_MessageBoxA( __in_opt HWND hWnd , __in_opt char* lpText , __in_opt char* lpCaption , __in UINT uType)
{
	LPFN_MessageBoxA fnOrigin = (LPFN_MessageBoxA)GetIATHookOrign(g_hHook_MessageBoxA);//原函数指针

	return fnOrigin(hWnd , "hook" , lpCaption , uType);
}

int __cdecl wmain(int nArgc, WCHAR** Argv)
{
	do 
	{
		UNREFERENCED_PARAMETER(nArgc);
		UNREFERENCED_PARAMETER(Argv);

		IATHook(
			GetModuleHandleW(NULL) ,//当前进程的句柄 基地址
			"user32.dll" ,          //目标dll名
			"MessageBoxA" ,         //目标函数名
			Fake_MessageBoxA ,      //替换的函数指针
			&g_hHook_MessageBoxA    //返回修改后的IATHOOK_BLOCK PE对象
		);
		
		MessageBoxA(NULL , "test" , "caption" , 0);

		UnIATHook( g_hHook_MessageBoxA);

		MessageBoxA(NULL , "test" , "caption" , 0);
	
	} while (FALSE);
	
	return 0;
}


