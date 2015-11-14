//////////////////////////////////////////////////////////////////////
// InterceptDispatcher.cpp
// -------------------------------------------------------------------
// Functions for safely intercepting gamecode without conflicts.
//
// Far from done!
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
/*
#include "..\D2HackIt.h"


typedef struct interceptdispatchertrampoline_t
{
	PBYTE		code1;				// jmp to user code
	PBYTE		code2;				// jmp to next intercept OR original code + jmp back to game
	DWORD		HostileTakeover;	// TRUE = No more intercepts allowed after this
	LPVOID		prev;
	LPVOID		next;
} INTERCEPTDISPATCHERTRAMPOLINE;

typedef struct interceptdispatcherlocation_t
{
	DWORD		InterceptLocation;			// Address of intercept
	DWORD		InstructionSize;			// Size of instruction at intercept
	INTERCEPTDISPATCHERTRAMPOLINE	*idt;	// ptr to first trampoline
	LPVOID		prev;
	LPVOID		next;
} INTERCEPTDISPATCHERLOCATION;

INTERCEPTDISPATCHERLOCATION *idl=NULL;
*/

/*

 Principle of operation:
 -----------------------
 
In action:
 
 1. Jump from PATCHLOCATION -> &(patchlocation)icl->(first)idt->code1
 2. Jump from icl->idt->code1 to &InsertedCode
 3. Jump from &InsertedCode to icl->idt->code2
 4. If (icl->idt->next) icl->idt = icl->idt->next. Go to 2
*/ 
/*
#define JUMPSIZE		5
#define INTERCEPTSIZE	9
#define RETURN_FROM_INTERCEPT(x)	\
		__asm sub esp,4							\
		__asm push eax							\
		__asm add esp,8							\
		__asm mov eax, [##x##Trampoline]		\
		__asm push eax							\
		__asm sub esp,4							\
		__asm pop eax							\
		__asm add esp,4							\
		__asm ret
#define PREPARE_FOR_INTERCEPT(x)	DWORD x ## Trampoline=0; DWORD x ## ErrorCode=0
#define INTERCEPT(x, InterceptLocation, InterceptSize)	\
		x##Trampoline = InterceptCode((DWORD)##&##x, InterceptLocation, InterceptSize);\
		if ( x##Trampoline < 10) { x##ErrorCode=x##Trampoline; x##Trampoline=0; }
#define INTERCEPTRETURNVALUE(InterceptProcName)	InterceptProcName ## Trampolin
#define INTERCEPT_ERRORCODE(x)		x ## ErrorCode

DWORD InterceptCode(DWORD pFunction, DWORD Location, DWORD Size)
{
	return 1;
}
//
// Sample intercept source
//

PREPARE_FOR_INTERCEPT(MyProc);

//
// This is the code we want to pass through.
// Declare this function the same as the function 
// you are intercepting using the same calling
// convention to get parameters right.
//
void MyProc(LPSTR szParam, DWORD nParam)
{
	// Do something!
	MessageBox(NULL, szParam, "Inside MyProc!", NULL);
	
	// Resume original code
	RETURN_FROM_INTERCEPT(MyProc);
}

void main(void)
{
	// Route the code at 0x1234568 throgh MyProc
	INTERCEPT(MyProc, 0x1234568, 5);
	
	// Check errorcode
	if (INTERCEPT_ERRORCODE(MyProc))
	{
		// If we get here, something went wrong!
		// The errorcode is in INTERCEPT_ERRORCODE(MyProc)
	}
}

*/