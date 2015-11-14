//////////////////////////////////////////////////////////////////////
// MemorySearchFunctions.cpp
// -------------------------------------------------------------------
// Pattern search algorithm and other memory related issues.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// GetMemoryAddressFromPattern
// -------------------------------------------------------------------
// Returns the address of szSearchPattern+offset if found in szDllName
//
// The search criteria are determined by the first character of
// szSearchPattern:
//
// !				An ordinal inside	(ex. !NameOfFuntion or !10005)
// #				An actual hexadecimal adress (ex. #6fba80b4)
// other			Fingerprint pattern (see below)
//
// Patterns are interpreted as a string of bytes. The value 00 to ff
// represents an actual value. A byte represented as 'xx' is not
// important to the fingerprint and are masked out. Example of masked
// bytes are absolute addresses/offsets inside the code that are
// likely to change location on blizzard patches.
//
// Once the address of the ordinal, actual adress or fingerprint has
// been found, the offset is added to the result and passed back to
// the calling function.
//
// If the address is not found the function returns 0
//
// <thohell>
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetMemoryAddressFromPattern(LPSTR szDllName, LPCSTR szSearchPattern, DWORD offset)
{
	DWORD lResult = 0;
	
	// Check for actual address
	if (szSearchPattern[0] == '#')
	{
		LPSTR t="";
		lResult=strtoul(&szSearchPattern[1], &t, 0x10);
		return lResult+=(lResult?offset:0);
	} 
	
	// Check for ordinal
	if (szSearchPattern[0] == '!')
	{
		HMODULE hModule = GetModuleHandle(szDllName);

		// First let's try to find ordinal by name
		if (hModule)
			lResult = (DWORD)GetProcAddress(hModule, &szSearchPattern[1]);

		// No luck, lets try by ordinal number instead
		if (!lResult)
		{
			LPSTR x="";
			lResult = (DWORD)GetProcAddress(hModule, (LPCSTR)MAKELONG(strtoul(&szSearchPattern[1], &x, 10),0));
		}

		return lResult+=(lResult?offset:0);
	} 
	
	// Parse fingerprint
	DWORD len=(strlen(szSearchPattern))/2;
	WORD *pPattern=new WORD[len];
	DWORD SearchAddress=GetBaseAddress(szDllName);
	DWORD SearchSize=GetImageSize(szDllName);
	MakeSearchPattern(szSearchPattern,pPattern);
	if (lResult = (DWORD)PatternSearch((BYTE*)SearchAddress, SearchSize, pPattern, len))
		lResult+=offset;
	delete pPattern;
	return lResult;
}

//////////////////////////////////////////////////////////////////////
// Pattern search algorithm written by Druttis.
//
// Patterns string is in the form of
//
//	0xMMVV, 0xMMVV, 0xMMVV
//
//	Where MM = Mask & VV = Value
//
//	Pattern Equals is doing the following match
//
//	(BB[p] & MM[p]) == VV[p]
//
//	Where BB = buffer data
//
//	That means :
//
//	a0, b0, c0, d0, e0 is equal to
//
//	1)	0xffa0, 0xffb0, 0x0000, 0x0000, 0xffe0
//	2)	0x0000, 0x0000, 0x0000, 0x0000, 0x0000
//	3)	0x8080, 0x3030, 0x0000, 0xffdd, 0xffee
//
//	I think you got the idea of it...BOOL _fastcall PatternEquals(LPBYTE buf, LPWORD pat, DWORD plen)
//////////////////////////////////////////////////////////////////////
BOOL	PRIVATE PatternEquals(LPBYTE buf, LPWORD pat, DWORD plen)
{
	//
	//	Just a counter
	DWORD i;
	//
	//	Offset
	DWORD ofs = 0;
	//
	//	Loop
	for (i = 0; plen > 0; i++) {
		//
		//	Compare mask buf and compare result
		//  <thohell>Swapped mask/data. Old code was buggy.</thohell>
		if ((buf[ofs] & ((pat[ofs] & 0xff00)>>8)) != (pat[ofs] & 0xff))
			return FALSE;
		// 
		//	Move ofs in zigzag direction
		plen--;
		if ((i & 1) == 0)
			ofs += plen;
		else
			ofs -= plen;
	}
	//
	//	Yep, we found
	return TRUE;
}

//
//	Search for the pattern, returns the pointer to buf+ofset matching
//	the pattern or null.
LPVOID PRIVATE PatternSearch(LPBYTE buf, DWORD blen, LPWORD pat, DWORD plen)
{

	//
	//	Offset and End of search
	DWORD ofs;
	DWORD end;
	//
	//	Buffer length and Pattern length may not be 0
	if ((blen == 0) || (plen == 0))
		return NULL;
	//
	//	Calculate End of search
	end = blen - plen;
	//
	//	Do the booring loop
	for (ofs = 0; ofs < end; ofs++) {
		//
		//	Return offset to first byte of buf matching width the pattern
		if (PatternEquals(&buf[ofs], pat, plen))
			return &buf[ofs];
	}
	//
	//	Me no find, me return 0, NULL, nil
	return NULL;
}


//////////////////////////////////////////////////////////////////////
// MakeSearchPattern
// -------------------------------------------------------------------
// Convert a pattern-string into a pattern array for use with pattern 
// search.
//
// <thohell>
//////////////////////////////////////////////////////////////////////
VOID  PRIVATE MakeSearchPattern(LPCSTR pString, LPWORD pat)
{
	char *tmp=new char[strlen(pString)+1];
	strcpy(tmp, pString);

	for (int i=(strlen(tmp)/2)-1; strlen(tmp) > 0; i--)
	{
		char *x="";
		BYTE value=(BYTE)strtoul(&tmp[i*2], &x, 0x10);
		if (strlen(x))
			pat[i]=0;
		 else
			pat[i]=MAKEWORD(value, 0xff);

		tmp[i*2]=0;
	}
	delete tmp;
}

	
//////////////////////////////////////////////////////////////////////
// GetBaseAddress
// -------------------------------------------------------------------
// Stub function
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetBaseAddress(LPSTR ModuleName)
{
	SetMemToolType();
	return pfep->GetBaseAddress(ModuleName);
}

//////////////////////////////////////////////////////////////////////
// GetImageSize
// -------------------------------------------------------------------
// Stub function
//////////////////////////////////////////////////////////////////////
DWORD PRIVATE GetImageSize(LPSTR ModuleName)
{
	SetMemToolType();
	return pfep->GetImageSize(ModuleName);
}

//////////////////////////////////////////////////////////////////////
// SetMemToolType
// -------------------------------------------------------------------
// Decides if we need to use psapi or toolhelp for memory functions.
//////////////////////////////////////////////////////////////////////
void PRIVATE SetMemToolType(void)
{
	// Find out if the toolhelp API exists in kernel32
	HMODULE k32=GetModuleHandle("kernel32.dll");
	pfep->toolhelp.Module32First = (fnModule32First)GetProcAddress(k32, "Module32First");
	pfep->toolhelp.Module32Next = (fnModule32Next)GetProcAddress(k32, "Module32Next");
	pfep->toolhelp.CreateToolhelp32Snapshot=(fnCreateToolhelp32Snapshot)GetProcAddress(k32,"CreateToolhelp32Snapshot");
	
	
	// If any of these addresses are 0, we'll assume psapi
	if (!(pfep->toolhelp.Module32First) || !(pfep->toolhelp.Module32Next) || !(pfep->toolhelp.CreateToolhelp32Snapshot))
	{		
		MessageBox(NULL, "PSAPI code is not yet implemented!\nExpect game crash!", "D2HackIt! Error!", NULL);
		/*
		HMODULE psapi=GetModuleHandle("psapi.dll");
		if (!psapi) psapi=LoadLibrary("psapi.dll");
		if (!psapi) { MessageBox(NULL, "Unable to get handle of PSAPI.DLL.", "D2HackIt! Error!", MB_ICONERROR);}

		pfep->psapi.EnumProcessModules=(fnEnumProcessModules)GetProcAddress(psapi, "EnumProcessModules");
		pfep->psapi.GetModuleBaseName=(fnGetModuleBaseName)GetProcAddress(psapi, "GetModuleBaseNameA");
		pfep->psapi.GetModuleInformation=(fnGetModuleInformation)GetProcAddress(psapi, "GetModuleInformation");

		pfep->GetBaseAddress=&GetBaseAddress_psapi;
		pfep->GetImageSize=&GetImageSize_psapi;
		*/
	} else {

		pfep->GetBaseAddress=&GetBaseAddress_toolhelp;
		pfep->GetImageSize=&GetImageSize_toolhelp;
	}
		
	return;
	
}


