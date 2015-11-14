//////////////////////////////////////////////////////////////////////
// Structs.h
// -------------------------------------------------------------------
// This include file contains all structs used by only the server.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>

//////////////////////////////////////////////////////////////////////
// FINGERPRINTSTRUCT
// -------------------------------------------------------------------
// Structure used when fingerprinting functions.
//////////////////////////////////////////////////////////////////////
#define MAX_FPS_NAME_LEN		0x40
#define MAX_FPS_MODULENAME_LEN	0x10
#define MAX_FPS_FINGERPRINT_LEN	0x80
typedef struct fingerprintstruct_t
{
	char		Name[MAX_FPS_NAME_LEN];
	char		ModuleName[MAX_FPS_MODULENAME_LEN];
	char		FingerPrint[MAX_FPS_FINGERPRINT_LEN];
	DWORD		Offset;
	DWORD		PatchSize;
	DWORD		AddressFound;
} FINGERPRINTSTRUCT;

//////////////////////////////////////////////////////////////////////
// FINGERPRINTINFO
// -------------------------------------------------------------------
// Contains information about fingerprints.
//////////////////////////////////////////////////////////////////////
typedef struct fingerprintinfo_t
{
	LPSTR		FingerprintName;
	LPSTR		DllName;
	LPSTR		Fingerprint;
	DWORD		PatchSize;
	DWORD		Offset;
	PBYTE		OriginalCode;
	DWORD		MemoryLocation;
} FINGERPRINTINFO;

//////////////////////////////////////////////////////////////////////
// LOADERDATA *psi;
// -------------------------------------------------------------------
// Contains information passed from the loader.
//////////////////////////////////////////////////////////////////////
typedef struct loaderdata_t {
	DWORD	LoaderMagic;
	DWORD	LoaderVersion;
	DWORD	CodeLocation;
	HWND	hwnd;
	DWORD	pid;
	HANDLE	hProcess;
	DWORD	pLoadLibraryA;
	DWORD	pFreeLibrary;
	DWORD	pGetModuleHandleA;
	char	ModuleName[0x10];
	DWORD	WinProcLocation;
	DWORD	WinProcPatchsize;
} LOADERDATA;

//////////////////////////////////////////////////////////////////////
// PRIVATESERVERINFO *psi;
// -------------------------------------------------------------------
// Contains private information about the server.
//////////////////////////////////////////////////////////////////////
#define MAXINSERTRECVBUFFERS	20
#define MAXRECVBUFFERLEN		0x80
typedef struct privateserverinfo_t
{
	DWORD			pid;
	HWND			hwnd;
	HANDLE			hProcess;
	CRITICAL_SECTION csData;
	LOADERDATA*		loader;
	char			BuildDate[16];
	char			BuildTime[16];
	LPCSTR			IniFile;
	char			ErrorPrompt[MAXPROMPTLENGTH];
	char			InfoPrompt[MAXPROMPTLENGTH];
	char			VerbosePrompt[MAXPROMPTLENGTH];
	BOOL			Verbose;
	// Tick thread
	volatile DWORD	TickShutDown;
	BOOL			TickThreadActive;
	HANDLE			TickThreadHandle;
	// Quick hack
	BOOL			DontShowErrors;
	// Insert receive buffers
	BYTE	InsertRecvBuffer[MAXINSERTRECVBUFFERS][MAXRECVBUFFERLEN];
	volatile DWORD	InsertRecvLen[MAXINSERTRECVBUFFERS];
	volatile DWORD	nRecvBufferPos;
	// Fingerprinted information
	DWORD			GameSocketLocation;
	DWORD			GamePrintStringLocation;
	DWORD			GameKeyDownIntercept;
	DWORD			pPlayerInfoStruct;
	DWORD			GameSendPacketToGameLocation;
	DWORD			GameSendMessageToChatLocation;
	struct {
		FINGERPRINTSTRUCT	GamePacketReceivedIntercept;
		FINGERPRINTSTRUCT	GamePacketReceivedIntercept2;
		FINGERPRINTSTRUCT	GamePacketSentIntercept;
		FINGERPRINTSTRUCT	GamePlayerInfoIntercept;
	} fps;
} PRIVATESERVERINFO;
extern PRIVATESERVERINFO			*psi;

//////////////////////////////////////////////////////////////////////
// PRIVATEFUNCTIONENTRYPOINTS *pfep;
// -------------------------------------------------------------------
// Contains information about entrypoints only used by the server.
//////////////////////////////////////////////////////////////////////
// Typedefs for toolhelp32
typedef BOOL	(WINAPI *fnModule32First)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL	(WINAPI *fnModule32Next)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef HANDLE	(WINAPI *fnCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);  
// Typedefs for psapi
typedef struct _MODULEINFO {LPVOID lpBaseOfDll;DWORD SizeOfImage;LPVOID EntryPoint;} MODULEINFO, *LPMODULEINFO;
typedef BOOL	(*fnEnumProcessModules)(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD	(*fnGetModuleBaseName)(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);
typedef BOOL	(*fnGetModuleInformation)(HANDLE hProcess,HMODULE hModule,LPMODULEINFO lpmodinfo,DWORD cb);

// Common typedefs
typedef DWORD (PRIVATE *fnGetBaseAddress)(LPSTR ModuleName);
typedef DWORD (PRIVATE *fnGetImageSize)(LPSTR ModuleName);

typedef struct privatefunctionentrypoints_t
{
	fnGetBaseAddress				GetBaseAddress;
	fnGetImageSize					GetImageSize;
	struct {
		fnCreateToolhelp32Snapshot		CreateToolhelp32Snapshot;
		fnModule32First					Module32First;
		fnModule32Next					Module32Next;
	} toolhelp;
	struct {	
		fnEnumProcessModules			EnumProcessModules;
		fnGetModuleBaseName				GetModuleBaseName;
		fnGetModuleInformation			GetModuleInformation;
	} psapi;
} PRIVATEFUNCTIONENTRYPOINTS;
extern PRIVATEFUNCTIONENTRYPOINTS	*pfep;
