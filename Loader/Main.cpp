//////////////////////////////////////////////////////////////////////
// Main.cpp
// -------------------------------------------------------------------
// Main file for D2Hackit loader.
//
// If a single D2 window is found, it will be loaded/unloaded without
// any user interaction.  If multiple windows are found, the user will
// be prompted as to whether the program should load/unload each 
// particular window.
//
// This code is version independent in the sense that it finds the
// location of game.exe and it's WinProc which is what we need to put
// our code inside the game and execute it.
//
// We also get exported kernel functions dynamically instead of 
// calling them via vectors inside the game. The *only* criteria for
// this loader to work is that the instruction at WinProc is 5 bytes
// long. 
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include "../D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// Version history.
// -------------------------------------------------------------------
// 1.00 2001-09-24 Original release
// 1.01 2001-09-24 Rewrote GetPatchAddress to generic GetBaseAddress
// 1.02 2001-09-28 Major rewrite to include support for NT 4.0
// 1.03 2001-10-16 Removed PSAPI code, it's buggy
// 1.04 2001-10-17 Rewrite to give back WinProc after load
// 1.05 2001-11-20 Added support for D2Loader
// 1.06 2001-12-21 Added logic for multiple sessions (D2Loader -multiclient)
//////////////////////////////////////////////////////////////////////
#define LOADERVERSION		MAKELONG(1,06)


//////////////////////////////////////////////////////////////////////
// Macros to make our life easier
//////////////////////////////////////////////////////////////////////
#define INST_CALL 0xe8
#define make_space __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
#define FUNCTLEN(func) ((DWORD)func##_END-(DWORD)func)


//////////////////////////////////////////////////////////////////////
// Data definitions for c code and assembler
//////////////////////////////////////////////////////////////////////
#define DATA				0x200			// Offset between data+code
#define LOADERMAGIC			0xD1AB101D
#define LOADERMAGICID		0x00
#define LOADERVERSIONID		0x04
#define CODELOCATION		0x08
#define GAME_HWND			0x0c
#define GAME_PID			0x10
#define GAME_HANDLE			0x14
#define PLOADLIBRARYA		0x18			
#define PFREELIBRARY		0x1c			
#define PGETMODULEHANDLEA	0x20			
#define MODULENAME			0x24			// char[0x10]
#define FILENAME			0x3c			// char[...]



//////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////
void WriteProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len);
void ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len);
DWORD ReadProcessDWORD(HANDLE hProcess, DWORD lpAddress);
void Intercept(HANDLE hProcess, int inst, DWORD pOldCode, DWORD pNewCode, int nBytes);
DWORD GetBaseAddress(DWORD pid, char* ModuleName);
void AsmCode(void);
void AsmCode_END(void);
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);

// hd is a structure for the data required by the DLL load routine
loaderdata_t hd;

// windowlist will hold a list of Diablo II Window handles
HWND windowlist[16];

// NumWindows holds the number of D2 windows found by EnumWindows()
int  NumWindows = 0;

//////////////////////////////////////////////////////////////////////
// WinMain
// -------------------------------------------------------------------
// The meat of the application.
//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DWORD check;
	DWORD PatchAddress;
	DWORD DataAddress;
	DWORD WinProcAddress;
	char message[128];
    char windowname[128];
    char dllFullName[256];
	int x;
    int mode;
	
	// Sets up NumWindows, and the windolist[] array of HWND's
    // See EnumWindowsProc() for deatils.....
	EnumWindows(EnumWindowsProc, 0);
	
	if(NumWindows == 0)
		MessageBox(NULL, "No D2 Windows found!", "Loader Error", MB_ICONERROR);

	// Loop through all the D2 windows found
	for(x=0; x<NumWindows; x++) {

		// Set the HWND for the current window
		hd.hwnd = windowlist[x];
		
		// Get Diablo II's Process handle
		GetWindowThreadProcessId(hd.hwnd, &hd.pid);
		hd.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hd.pid);
		if (!hd.hProcess) { MessageBox(NULL, "Can't get Diablo II's process handle.", "Loader Error!", MB_ICONERROR); return -1;}
		
		// Find a good spot to put our code.  Look first for the original game.
		PatchAddress=GetBaseAddress(hd.pid, "Game.exe");
		if (!PatchAddress) { 
			PatchAddress=GetBaseAddress(hd.pid, "Diablo II.exe");
			if (!PatchAddress) { 
				PatchAddress=GetBaseAddress(hd.pid, "D2Loader.exe");
				if (!PatchAddress) { MessageBox(NULL, "Unable to find a patch location.", "Loader Error!", MB_ICONERROR); return -1;}
			}
		}
		PatchAddress += 0x400;
		DataAddress   = PatchAddress+DATA;

		// Get location of the game's WinProc
		WinProcAddress = GetClassLong(hd.hwnd,GCL_WNDPROC);
		if (!WinProcAddress) { MessageBox(NULL, "Unable to find WinProc entrypoint.", "Loader Error!", MB_ICONERROR); return -1;}
		
		// Initialize our data structure
		sprintf(hd.ModuleName, "%s", "D2HackIt");

		hd.LoaderMagic      = LOADERMAGIC;
		hd.CodeLocation     = PatchAddress;
		hd.LoaderVersion    = LOADERVERSION;
		hd.WinProcLocation  = WinProcAddress;
		hd.WinProcPatchsize = 5;

		HMODULE hModule = GetModuleHandle("Kernel32.dll");
		if (!hModule) { MessageBox(NULL, "Unable to get handle of KERNEL32.DLL.", "Loader Error!", MB_ICONERROR); return -1;}
		
		// We're getting offsets to kernel functions from this userspace.
		// They will be the same inside the game, so we won't need to use
		// functions imported from the game.

		hd.pLoadLibraryA = (DWORD)GetProcAddress(hModule, "LoadLibraryA");
		if (!hd.pLoadLibraryA ) { MessageBox(NULL, "Unable to get address of LoadLibraryA in KERNEL32.DLL.", "Loader Error!", MB_ICONERROR); return -1;}
		
		hd.pFreeLibrary = (DWORD)GetProcAddress(hModule, "FreeLibrary");
		if (!hd.pFreeLibrary ) { MessageBox(NULL, "Unable to get address of FreeLibrary in KERNEL32.DLL.", "Loader Error!", MB_ICONERROR); return -1;}
		
		hd.pGetModuleHandleA = (DWORD)GetProcAddress(hModule, "GetModuleHandleA");
		if (!hd.pGetModuleHandleA ) { MessageBox(NULL, "Unable to get address of GetModuleHandleA in KERNEL32.DLL.", "Loader Error!", MB_ICONERROR); return -1;}

		// Get the first DWORD from DataAddress to see if we're loading or
		// unloading.  Set up the text for the message box
		ReadProcessBYTES(hd.hProcess, DataAddress, &check, sizeof(DWORD));
		if (check != LOADERMAGIC) { 
			mode = 1;		// We are loading
			strcpy(message, "LOAD ");
		} else {
			mode = 0;		// We are unloading
			strcpy(message, "UNLOAD ");
		}

		GetWindowText(hd.hwnd, windowname, 127);
		strcat(message, windowname);
		strcat(message, "?");

		// If a single window was found, or if the user chooses to do so, 
		// run the load/unload procedure against the current window
		if((NumWindows == 1) || (MessageBox(NULL, message, "Loader Question", MB_YESNO)) == IDYES) {

			// Patch in our code
			WriteProcessBYTES(hd.hProcess, PatchAddress, &AsmCode, FUNCTLEN(AsmCode));
		
			// Patch in our data
			WriteProcessBYTES(hd.hProcess, DataAddress, &hd, sizeof(LOADERDATA));
		
			// Patch in full path+filename to server dll
			GetCurrentDirectory(255, dllFullName);
			strcat(dllFullName, "\\D2HackIt.dll");
			WriteProcessBYTES(hd.hProcess, DataAddress+sizeof(LOADERDATA), dllFullName, strlen(dllFullName)+1);
		
			// Now hi-jack WinProc of the game to our code
			Intercept(hd.hProcess, INST_CALL, WinProcAddress, PatchAddress, 5);
		
			// Send WM_APP to toggle load/unload of the dll
			SendMessage(hd.hwnd, WM_APP,0,0);
		
			// Give back WinProc, we don't need it anymore
			Intercept(hd.hProcess, INST_CALL, PatchAddress, WinProcAddress, 5);

			// If we're unloading, then clear LOADERMAGIC from the data area
			if(mode == 0) {
				check = 0;
				WriteProcessBYTES(hd.hProcess, DataAddress, &check, sizeof(DWORD));
			}
		}
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////
// GetBaseAddress
// -------------------------------------------------------------------
// Find a good location to patch in our code. The code is relocateable
// so any location is good. Thanks to Skywing for pointing me to 
// toolhelp32 instead if PSAPI.DLL.
//
// Update:
// 1.02 - Added code for both PSAPI and toolhelp32 to include support
//        for NT 4.0.
// 1.03 - Removed PSAPI code. It's buggy!
//////////////////////////////////////////////////////////////////////
DWORD GetBaseAddress(DWORD pid, char* ModuleName)
{
	// Typedefs for toolhelp32
	typedef BOOL (WINAPI *fnModule32First)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	typedef BOOL (WINAPI *fnModule32Next)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	typedef HANDLE (WINAPI *fnCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);  
	
	// Find out if the toolhelp API exists in kernel32
	HMODULE k32=GetModuleHandle("kernel32.dll");
	if (!k32) { MessageBox(NULL, "Unable to get handle of KERNEL32.DLL.", "Loader Error!", MB_ICONERROR); return -1;}
	fnModule32First Module32First = (fnModule32First)GetProcAddress(k32, "Module32First");
	fnModule32Next Module32Next = (fnModule32Next)GetProcAddress(k32, "Module32Next");
	fnCreateToolhelp32Snapshot CreateToolhelp32Snapshot=(fnCreateToolhelp32Snapshot)GetProcAddress(k32,"CreateToolhelp32Snapshot");
	
	
	// Verify that the ToolHelp32 API is available
	if (!(Module32First) || !(Module32Next) || !(CreateToolhelp32Snapshot))	{
		MessageBox(NULL, "Your operating system does not support the TOOLHELP32 API.\nCheck back for updates that use PSAPI instead.", "Loader Error!", NULL);
		return 0;
	} else {
		// toolhelp code
		
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if ((int)hSnapshot == -1) { MessageBox(NULL, "Can't create toolhelp32 snapshot of game.", "Loader Error!", MB_ICONERROR); return 0; }
		
		MODULEENTRY32 lpme; lpme.dwSize=sizeof(MODULEENTRY32);
		
		// Get first module, this is needed for win9x/ME
		if (!Module32First(hSnapshot, &lpme)) { CloseHandle(hSnapshot); MessageBox(NULL, "Can't get first module of game.", "Loader Error!", MB_ICONERROR); return 0; };
		
		// Loop through all other modules
		while (TRUE)
		{
			if (!stricmp(lpme.szModule, ModuleName)) { CloseHandle(hSnapshot); return (DWORD)lpme.modBaseAddr; }
			if (!Module32Next(hSnapshot, &lpme)) { CloseHandle(hSnapshot); return 0; };
		}
		return 0;
	}
}



//////////////////////////////////////////////////////////////////////
// WriteProcessBYTES()
// -------------------------------------------------------------------
// Originally mousepads code
//////////////////////////////////////////////////////////////////////
void WriteProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len)
{
	DWORD oldprot,dummy = 0;
	VirtualProtectEx(hProcess, (void*) lpAddress, len, PAGE_READWRITE, &oldprot);
	WriteProcessMemory(hProcess, (void*) lpAddress, buf, len, 0);
	VirtualProtectEx(hProcess, (void*) lpAddress, len, oldprot, &dummy);
}

//////////////////////////////////////////////////////////////////////
// ReadProcessBYTES()
// -------------------------------------------------------------------
// Originally mousepads code
//////////////////////////////////////////////////////////////////////
void ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len)
{
	DWORD oldprot, dummy = 0;
	VirtualProtectEx(hProcess, (void*) lpAddress, len, PAGE_READWRITE, &oldprot);
	ReadProcessMemory(hProcess, (void*) lpAddress, buf, len, 0);
	VirtualProtectEx(hProcess, (void*) lpAddress, len, oldprot, &dummy);
}


//////////////////////////////////////////////////////////////////////
// Intercept()
// -------------------------------------------------------------------
// Originally mousepads code but modified to also copy original code
// to the entrypoint of our code.
//////////////////////////////////////////////////////////////////////
void Intercept(HANDLE hProcess, int instruction, DWORD lpSource, DWORD lpDest, int len)
{
	BYTE* buffer = new BYTE[len];
	ReadProcessBYTES(hProcess, lpSource, buffer, len);
	WriteProcessBYTES(hProcess, lpDest, buffer, len);
	
	buffer[0] = instruction;
	*(DWORD*) (buffer + 1) = lpDest - (lpSource + 5);
	memset(buffer + 5, 0x90, len - 5);		// nops
	
	WriteProcessBYTES(hProcess, lpSource, buffer, len);
	
	delete buffer;
}

////////////////////////////////////////////////////////////////////
// EnumWindowsProc()
// ------------------------------------------------------------------
// This function is called by EnumWindows().  EnumWindows() will loop
// through all top-level windows in the system, calling this function
// for each one, until it runs out of windows or this function returns
// a FALSE (0).  It's being used to build a list of all D2 windows
// currently running on the system.
/////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam) {
	char classname[128];

	GetClassName(hwnd, classname, 127);
	if(!strcmp(classname, "Diablo II")) {
		windowlist[NumWindows] = hwnd;
		NumWindows++;
	}
	if (NumWindows > 15)
		return(0);

	return(1);
}

//////////////////////////////////////////////////////////////////////
// Asm_Code()
// -------------------------------------------------------------------
// This is our assembler routine that we patch in to the game. Since
// this code is relocateable, you can patch in anywhere in the game's
// address-space. We chose to put it at game.exe+0x400 bytes.
//
// This code hi-jacks WinProc and loads/unloads our dll when it 
// receives WM_APP.
//////////////////////////////////////////////////////////////////////
void _declspec(naked) AsmCode(void)
{
	__asm {
		make_space						// Make room for original code
		pushad
		call getaddress					// Get our EIP
getaddress:
		pop esi
		sub esi, 0x0c					// We now have base address in esi
		add esi, DATA					// Add offset to data
		mov eax, [esp+0x2c]				// esp+0x2c = uMsg
		cmp eax, WM_APP					// Check for our signal
		je do_our_stuff
hack_exit:
		popad
		ret
do_our_stuff:
		mov eax, esi
		add eax, MODULENAME
		push eax
		mov eax, [esi+PGETMODULEHANDLEA]
		call eax						// Is our dll loaded ?
		cmp eax, 0
		je load_dll						// No, load it
		push eax						// Yes, unload it
		mov eax, [esi+PFREELIBRARY]
		call eax
		jmp hack_exit
load_dll:							
		mov eax, esi
		add eax, FILENAME
		push eax
		mov eax, [esi+PLOADLIBRARYA]
		call eax
		jmp hack_exit
	}
}
void _declspec(naked) AsmCode_END(void){};
