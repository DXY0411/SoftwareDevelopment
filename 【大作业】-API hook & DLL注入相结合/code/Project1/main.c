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
	LPFN_MessageBoxA fnOrigin = (LPFN_MessageBoxA)GetIATHookOrign(g_hHook_MessageBoxA);//ԭ����ָ��

	return fnOrigin(hWnd , "hook" , lpCaption , uType);
}

int __cdecl wmain(int nArgc, WCHAR** Argv)
{
	do 
	{
		UNREFERENCED_PARAMETER(nArgc);
		UNREFERENCED_PARAMETER(Argv);

		IATHook(
			GetModuleHandleW(NULL) ,//��ǰ���̵ľ�� ����ַ
			"user32.dll" ,          //Ŀ��dll��
			"MessageBoxA" ,         //Ŀ�꺯����
			Fake_MessageBoxA ,      //�滻�ĺ���ָ��
			&g_hHook_MessageBoxA    //�����޸ĺ��IATHOOK_BLOCK PE����
		);
		
		MessageBoxA(NULL , "test" , "caption" , 0);

		UnIATHook( g_hHook_MessageBoxA);

		MessageBoxA(NULL , "test" , "caption" , 0);
	
	} while (FALSE);
	
	return 0;
}


