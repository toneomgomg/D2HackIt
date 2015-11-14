//////////////////////////////////////////////////////////////////////
// GamePrintFunctions.cpp
// -------------------------------------------------------------------
// Functions for sending text to the game.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// GamePrintString
// -------------------------------------------------------------------
// Helper function to print free-formatted text in game without having
// to convert to Unicode first.
// 
// If there is no color information at the start of the string, we 
// force the color to white. Not doing this may give unexpected
// results.
//
// This is the actual interface with the game. The other functions in
// this file are just wrappers for text formatting.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GamePrintString(LPCSTR buf)
{
	// Make sure we *CAN* write to screen...
	if ((!psi->GamePrintStringLocation) || (!thisgame->player))
		return FALSE;

	// Make a string big enough to fit header+text
	char t[512];
	char *pt=t;
	DWORD Entrypoint=psi->GamePrintStringLocation;

	// Format the text so game will take it.
	sprintf (t, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%s%c",
		0xFF,0x01,0xFF,0xFF,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		buf, 0x00);

	// The call
	__asm {
		mov ecx, pt
		mov eax, Entrypoint // Function to print text
		call eax
	};

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GamePrintInfo
// -------------------------------------------------------------------
// Standard function to output general info to the client.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GamePrintInfo(LPCSTR buf)
{
	LPSTR newBuf=new char[strlen(psi->InfoPrompt)+strlen(buf)+5];
	sprintf(newBuf,"%sÿc0 %s", psi->InfoPrompt, buf);
	GamePrintString(newBuf);
	delete newBuf;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GamePrintError
// -------------------------------------------------------------------
// Standard function to print error messages to the client.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GamePrintError(LPCSTR buf)
{
	LPSTR newBuf=new char[strlen(psi->ErrorPrompt)+strlen(buf)+5];
	sprintf(newBuf,"%sÿc0 %s", psi->ErrorPrompt, buf);
	GamePrintString(newBuf);
	delete newBuf;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GamePrintInfo
// -------------------------------------------------------------------
// Standard function to output general info to the client.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GamePrintVerbose(LPCSTR buf)
{
	if (!psi->Verbose)
		return TRUE;
	LPSTR newBuf=new char[strlen(psi->VerbosePrompt)+strlen(buf)+5];
	sprintf(newBuf,"%sÿc0 %s", psi->VerbosePrompt, buf);
	GamePrintString(newBuf);
	delete newBuf;
	return TRUE;
}
