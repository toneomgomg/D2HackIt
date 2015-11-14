//////////////////////////////////////////////////////////////////////
// DllMain.cpp
// -------------------------------------------------------------------
// Default Dll entrypoint.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

// Global structures
SERVERINFO					*si;		
PRIVATESERVERINFO			*psi;
FUNCTIONENTRYPOINTS			*fep;
PRIVATEFUNCTIONENTRYPOINTS	*pfep;
THISGAMESTRUCT				*thisgame;

LinkedList	szBufferList;	// List of GetHackProfile created strings
LinkedList	ClientList;		// List of Loaded Modules

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	BOOL hResult = TRUE;
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			// initiate server
			hResult = ServerStart(hModule);
			break;

		case DLL_PROCESS_DETACH:
			hResult = ServerStop();
			break;
    } 
    return hResult;
}
