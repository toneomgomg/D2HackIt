//////////////////////////////////////////////////////////////////////
// IniFileHandlers.cpp
// -------------------------------------------------------------------
// Functions used for manipulating .ini files.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"
#include <sys\types.h>
#include <sys\stat.h>

//////////////////////////////////////////////////////////////////////
// GetHackProfileString
// -------------------------------------------------------------------
// Reads a whole profile section using GetPrivateProfileString().
// Remember that this function uses 'new char[]' to create a return
// value the application. This means it should be unitialized when 
// calling this functions. You also need to manually delete it when 
// you are done with the data if you plan on using the variable again.
//
// Failing to do this *WILL* result in memory leaks!
//
// Update: The string will be deleted on ServerStop() if you relie on
// this then you must check for the server version number v0.58 and up
//
//////////////////////////////////////////////////////////////////////
LPSTR EXPORT GetHackProfileString(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName)
{
	DWORD	dwDummy;
	return fep->GetHackProfileStringEx( lpHackName, lpSectionName, lpKeyName, dwDummy );
}

LPSTR EXPORT GetHackProfileStringEx(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName, DWORD& cSize)
{
	bool		bFileFound = false;
	LPSTR		lpFileName = new char[MAX_PATH];
	LPCSTR		lpDefault="";
	LPSTR		lpReturnedString = NULL;

	LinkedItem*	li;
	CLIENTINFOSTRUCT* cds;

	sprintf( lpFileName, "%s\\%s.ini", si->PluginDirectory, lpHackName );

	// Check if the file exists
	if( _access(lpFileName, 0) )
	{
		// Check in the dir of the loaded modules
		li=ClientList.GetFirstItem();
		for( int i=0; i<ClientList.GetItemCount(); i++ )
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			sprintf( lpFileName, "%s\\%s.ini", cds->pDir, lpHackName );
			if ( !_access(lpFileName, 0) )
			{
				bFileFound = true;
				break;
			}
			li=ClientList.GetNextItem(li);
		}

		if( !bFileFound )
		{
			char t[1024];
			sprintf(t, "Unable to open ini-file: ÿc4%s", lpFileName);
			fep->GamePrintError(t);
			delete lpFileName;
			return NULL;
		}
	}

	// Try getting the data in 1024-byte increments
	DWORD	alloc=0;
	DWORD	allocStep = 1024;
	int		SizeOffset;

	cSize = 0;

	if( !lpSectionName || !lpKeyName )
		SizeOffset = 2;
	else
		SizeOffset = 1;

	while (TRUE)
	{
		alloc+=allocStep;
		lpReturnedString = new char[alloc];
		if ( (cSize = GetPrivateProfileString(lpSectionName, lpKeyName, lpDefault, lpReturnedString, alloc, lpFileName)) < (alloc - SizeOffset) )
			break;
		delete lpReturnedString;
	}
	delete lpFileName;


	szBufferList.AddItem( lpReturnedString );		// Buffer used to delete this string
												// when Server exits

	return lpReturnedString;
}

//////////////////////////////////////////////////////////////////////
// GetHackProfileSectionNames()
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
LPSTR EXPORT GetHackProfileSectionNames( LPCSTR lpHackName, DWORD& cSize )
{
	bool		bFileFound = false;
	LPSTR		lpFileName = new char[MAX_PATH];
	//LPCSTR		lpDefault="";
	LPSTR		lpReturnedString;

	LinkedItem*	li;
	CLIENTINFOSTRUCT* cds;

	sprintf( lpFileName, "%s\\%s.ini", si->PluginDirectory, lpHackName );
	// Check if the file exists
	if( _access(lpFileName, 0) )
	{
		// Check in the dir of the loaded modules
		li=ClientList.GetFirstItem();
		for( int i=0; i<ClientList.GetItemCount(); i++ )
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			sprintf( lpFileName, "%s\\%s.ini", cds->pDir, lpHackName );
			if ( !_access(lpFileName, 0) )
			{
				bFileFound = true;
				break;
			}
			li=ClientList.GetNextItem(li);
		}

		if( !bFileFound )
		{
			char t[1024];
			sprintf(t, "Unable to open ini-file: ÿc4%s", lpFileName);
			fep->GamePrintError(t);
			delete lpFileName;
			cSize = 0;
			return NULL;
		}
	}

	// Try getting the data in starting at Half the file size
	// then going a quarter bigger

	DWORD	alloc=0;
	DWORD	allocStep = 1024;
	struct _stat	stFileData;
	lpReturnedString = NULL;
	cSize = 0;


	if( _stat(lpFileName, &stFileData) )
	{
		if( allocStep < stFileData.st_size )
		{
			allocStep = stFileData.st_size / 4;
			alloc = allocStep;
		}

		while (TRUE)
		{
			alloc+=allocStep;
			lpReturnedString = new char[alloc];
			if ( (cSize = GetPrivateProfileSectionNames(lpReturnedString, alloc, lpFileName)) < (alloc-2) )
				break;
			delete lpReturnedString;
		}
		delete lpFileName;

		szBufferList.AddItem( lpReturnedString );		// Buffer used to delete this string
														// when Server exits
	}

	return lpReturnedString;
}


//////////////////////////////////////////////////////////////////////
// SetHackProfileString
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
BOOL EXPORT SetHackProfileString(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName, LPCSTR lpValue)
{
	bool		bFileFound;
	LPSTR		lpFileName = new char[MAX_PATH];
	LPCSTR		lpDefault="";

	LinkedItem*	li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;

	sprintf( lpFileName, "%s\\%s.ini", si->PluginDirectory, lpHackName );
	// Check if the file exists
	if( _access(lpFileName, 0) )
	{
		// Check in the dir of the loaded modules
		for( int i=0; i<ClientList.GetItemCount(); i++ )
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			sprintf( lpFileName, "%s\\%s.ini", cds->pDir, lpHackName );
			if ( !_access(lpFileName, 0) )
			{
				bFileFound = true;
				break;
			}
			li=ClientList.GetNextItem(li);
		}

		if( !bFileFound )
		{
			char t[1024];
			sprintf(t, "Unable to open ini-file: ÿc4%s", lpFileName);
			fep->GamePrintError(t);
			delete lpFileName;
			return FALSE;
		}
	}

	WritePrivateProfileString(lpSectionName, lpKeyName, lpValue, lpFileName);
	delete lpFileName;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GetFingerPrint
// -------------------------------------------------------------------
// Reads szFingerPrintName from the [FingerprintData] section of the
// ini-file asociated with szHackName.
// 
// It breaks up the components of the string, does some basic sanity-
// check and calls GetMemoryAddressFromPattern() to store the address
// of the fingerprint, if found.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GetFingerprint(LPCSTR szHackName, LPCSTR szFingerprintName, FINGERPRINTSTRUCT &fps)
{
	LPSTR szReturnString;
	sprintf(fps.Name, szFingerprintName);
	fps.AddressFound=0;

	int i;
	int nFields=0;
	char *t;
	szReturnString=fep->GetHackProfileString(szHackName, "FingerprintData", szFingerprintName);
	

	// No such fingerprint!
	if (!strlen(szReturnString))
	{ 
		t=new char[256];
		sprintf(t, "Can't find fingerprint for '%s' in '%s.ini'",
			szFingerprintName, szHackName);
		fep->GamePrintError(t);
		delete t, szReturnString; 
		return FALSE;
	}


	// Make sure we have 4 fields
	for (i=0; szReturnString[i]; i++)
		if (szReturnString[i] == ',')
			nFields++;

	if (nFields != 3)
	{ 
		t=new char[256];
		sprintf(t, "Fingerprint for '%s' in '%s.ini' is corrupt.",
			szFingerprintName, szHackName);
		fep->GamePrintError(t);
		delete t, szReturnString; 
		return FALSE;
	}


	// Loop backwards to get fingerprint info
	for (;i!=0;i--)
	{
		if (szReturnString[i] == ',')
		{
			szReturnString[i] = 0;
			nFields--;
			switch (nFields)
			{
			case 2:
				strcpy(fps.FingerPrint, &szReturnString[i+1]);
				break;
			case 1:
				fps.Offset=atoi(&szReturnString[i+1]);
				break;
			case 0:
				fps.PatchSize=atoi(&szReturnString[i+1]);
				break;
			}
		}
	}
	strcpy(fps.ModuleName, szReturnString);
	delete szReturnString;


	if ((fps.AddressFound=GetMemoryAddressFromPattern(fps.ModuleName, fps.FingerPrint, fps.Offset)) < 0x100)
	{
		if (psi->DontShowErrors)
			return FALSE;

		t=new char[256];
		sprintf(t, "Unable fo find location for '%s'.",
			szFingerprintName, szHackName);
		fep->GamePrintError(t);
		delete t;
		return FALSE;
	} else {
		t=new char[256];
		sprintf(t, "Found '%s' at %.8x",
			szFingerprintName, fps.AddressFound);
		fep->GamePrintVerbose(t);
		delete t;
		return TRUE;
	}
}
