//////////////////////////////////////////////////////////////////////
// FormatString.cpp
// -------------------------------------------------------------------
// This function scans a string for controlcodes and replaces them
// with the right value. Return value is the length of the string.
//
// $$0-$$9		Colors
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\D2HackIt.h"

DWORD PRIVATE FormatString(LPSTR aString)
{
	DWORD aLength=strlen(aString);
	char* p;

	// Replace all occurances of $$ with ÿc
	while (strstr(aString, "$$"))
	{
		p=strstr(aString, "$$");
		p[0]='ÿ';
		p[1]='c';
	}

	return aLength;
}
