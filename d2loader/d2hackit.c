#include <windows.h>



#include "..\global.h"


#include "plugin.h"


PLUGIN_INTERFACE 	Interface;
LPD2PARAM			g_pD2Param;

DWORD	__stdcall 	PluginEntry(DWORD dwReason, LPVOID lpData);

LPPLUGIN_INTERFACE __declspec( dllexport ) QueryInterface()
{
	Interface.dwMagicword=PLUGIN_MAGICWORD;
	Interface.dwVersion=PLUGIN_VERSION;
	Interface.szDescription="thohell's D2HackIt! Mk2, Version 0.58";
	Interface.fpEntry=PluginEntry;

	//MessageBox( NULL, "QueryInterface", "Debug", MB_OK );

	return &Interface;
}

DWORD __stdcall PluginEntry(DWORD dwReason, LPVOID lpData)
{
	LPPLUGIN_DATA_INIT	lpDataInit;

	switch (dwReason)
	{
		case REASON_INIT:
			lpDataInit=(LPPLUGIN_DATA_INIT)lpData;
			break;

		case REASON_CLEANUP:
			g_pD2Param=(LPD2PARAM)lpData;
			break;
			
		default:
		/* for other reasons, you should use & to test if 
		   certain flag is set.
		   e.g: if (dwReason & REASON_ENTER_GAME) {} */

		case REASON_ENTER_GAME:
		case REASON_ENTER_CHANNEL:
		case REASON_ENTER_MAIN_MENU:
		case REASON_LEAVE_GAME:
		case REASON_LEAVE_CHANNEL:
		case REASON_LEAVE_MAIN_MENU:
			g_pD2Param=(LPD2PARAM)lpData;
			break;

			break;
	}
	return TRUE;

}

/*
BOOL WINAPI DllMain(HINSTANCE hDll,DWORD dwReason,LPVOID lpReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		default:
			break;
	}
	return TRUE;
}
*/