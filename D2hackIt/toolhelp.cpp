//////////////////////////////////////////////////////////////////////
// toolhelp.cpp
// -------------------------------------------------------------------
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// GetImageSize_toolhelp
// -------------------------------------------------------------------
// Used to get the image size of a dll/exe.
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetImageSize_toolhelp(LPSTR ModuleName)
{
	MODULEENTRY32 lpme;
	if (FindImage_toolhelp(ModuleName, &lpme))
		return lpme.modBaseSize;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////
// GetBaseAddress_toolhelp
// -------------------------------------------------------------------
// Used to get the base address of a dll/exe.
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetBaseAddress_toolhelp(LPSTR ModuleName)
{
	MODULEENTRY32 lpme;
	if (FindImage_toolhelp(ModuleName, &lpme))
		return (DWORD)lpme.modBaseAddr;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////
// FindImage_toolhelp
// -------------------------------------------------------------------
// Loop through loaded images to get the MODULEINFO32 you need.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE FindImage_toolhelp(LPSTR ModuleName, MODULEENTRY32* lpme)
{
	// Get a snapshot
	HANDLE hSnapshot = pfep->toolhelp.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, psi->pid);
	if ((int)hSnapshot == -1) return FALSE;

	lpme->dwSize=sizeof(MODULEENTRY32);

	// Get first module, this is needed for win9x/ME
	if (!pfep->toolhelp.Module32First(hSnapshot, lpme)) { CloseHandle(hSnapshot); return FALSE; };

	// Loop through all other modules
	while (TRUE)
	{
		if (!strcmpi(lpme->szModule, ModuleName)) { CloseHandle(hSnapshot); return TRUE; }
		if (!pfep->toolhelp.Module32Next(hSnapshot, lpme)) { CloseHandle(hSnapshot); return FALSE; };
	}
}

