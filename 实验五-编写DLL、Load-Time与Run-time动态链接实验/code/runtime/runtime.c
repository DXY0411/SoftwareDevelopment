// A simple program that uses LoadLibrary and 
// GetProcAddress to access myPuts from Myputs.dll. 

#include <windows.h> 
#include <stdio.h> 
typedef int(__cdecl *MYPROC)(LPWSTR);
int main(void)
{
	HINSTANCE hinstLib;
	MYPROC ProcAdd;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// Get a handle to the DLL module.
	hinstLib = LoadLibrary(TEXT("User32.dll"));

	// If the handle is valid, try to get the function address.
	if (hinstLib != NULL)
	{
		ProcAdd = (MYPROC)GetProcAddress(hinstLib, "MessageBoxA");
		// If the function address is valid, call the function.
		if (NULL != ProcAdd)
		{
			fRunTimeLinkSuccess = TRUE;
			(ProcAdd)(0, "msg", 0, 0);
		}
		// Free the DLL module.
		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("Message printed from executable\n");
	system("pause");
	return 0;
}

