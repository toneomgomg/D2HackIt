//////////////////////////////////////////////////////////////////////
// psapi.cpp
// -------------------------------------------------------------------
// PSAPI specific memory functions.
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// GetImageSize_psapi
// -------------------------------------------------------------------
// Used to get the image size of a dll/exe.
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetImageSize_psapi(LPSTR ModuleName)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////
// GetBaseAddress_psapi
// -------------------------------------------------------------------
// Used to get the base address of a dll/exe.
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetBaseAddress_psapi(LPSTR ModuleName)
{
	// For now, just display a dummy error.
	MessageBox(NULL,
		"Your OS does not have toolhelp32 funtcions needed for D2HackIt!\n"
		"I need to add support for PSAPI functions to support you platform.\n"
		"Please check http://thohell.d2network.com/ for updates!\n\n"
		"/T",
		"D2HackIt! Error!",NULL);
	return 0;
}