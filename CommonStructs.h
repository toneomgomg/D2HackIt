//////////////////////////////////////////////////////////////////////
// CommonStructs.h
// -------------------------------------------------------------------
// This include file contains all structs and typedefs used by both 
// the server and the clients.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#pragma once

#include "global.h"

//////////////////////////////////////////////////////////////////////
// PLAYERINFOSTUCT
// -------------------------------------------------------------------
// Contains info about the current player.
//////////////////////////////////////////////////////////////////////
typedef struct playerinfostruct_t
{
	BYTE		UnitType;		// +0x00
	DWORD		CharacterClass;	// +0x04
	DWORD		PlayerID;		// +0x08
	DWORD		PlayerLocation;	// +0x0c (defines location somehow in or out of town and maybe other locations (0x05=in town, 0x01=out of town)
	DWORD		unknown1;		// +0x10
	BYTE		Act;			// +0x14
	BYTE		unknown2[0x5b];	// +0x15
	LPCSTR		PlayerName;		// +0x70 pointer to LPSZ player name
	BYTE		unknown3[0x3c];	// +0x74
	DWORD		PlayerPositionX;// +0xb0
	DWORD		PlayerPositionY;// +0xb4
} PLAYERINFOSTRUCT, *PPLAYERINFOSTRUCT;


//////////////////////////////////////////////////////////////////////
// GAMESTRUCT
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
typedef struct gamestruct_t
{
	char	Unknown1[0x1a];
	char	GameName[0x10];
	char	ServerIP[0x10];
	char	Unknown2[0x76];
	char	CharacterName[0x10];
	char	Unknown3[0x08];
	char	Realm[0x10];
	char	Unknown4[0x15d];
	char	GamePassword[0x10];
} GAMESTRUCT, *PGAMESTRUCT;



//////////////////////////////////////////////////////////////////////
// GAMEINFOSTRUCT
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
typedef struct thisgamestruct_t
{
	PPLAYERINFOSTRUCT	player;
	PGAMESTRUCT			CurrentGame;
} THISGAMESTRUCT, *PTHISGAMESTRUCT;

extern THISGAMESTRUCT*	thisgame;


//////////////////////////////////////////////////////////////////////
// SERVERINFO *si;
// -------------------------------------------------------------------
// Contains public information about the server.
//////////////////////////////////////////////////////////////////////
typedef struct serverinfo_t
{
	DWORD	Version;
	DWORD	PlayerSlot;
	LPSTR	PluginDirectory;
} SERVERINFO, *PSERVERINFO;
extern SERVERINFO			*si;


//////////////////////////////////////////////////////////////////////
// Constant for GetInfoStruct
// -------------------------------------------------------------------
//
//////////////////////////////////////////////////////////////////////
#define	GS_ClientList	0x00000001
#define	GS_D2Param		0x00000002


//////////////////////////////////////////////////////////////////////
// FUNCTIONENTRYPOINTS *fep;
// -------------------------------------------------------------------
// Pointers to functions inside the game code and helper functions
// inside D2HackIt. This is exposed to the clients.
//////////////////////////////////////////////////////////////////////
typedef BOOL	(__cdecl *fnGamePrintString)(LPCSTR buf);
typedef BOOL	(__cdecl *fnGamePrintInfo)(LPCSTR buf);
typedef BOOL	(__cdecl *fnGamePrintError)(LPCSTR buf);
typedef BOOL	(__cdecl *fnGamePrintVerbose)(LPCSTR buf);
typedef LPSTR   (__cdecl *fnGetHackProfileString)(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName);
typedef LPSTR   (__cdecl *fnGetHackProfileStringEx)(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName, DWORD& cSize);
typedef LPSTR	(__cdecl *fnGetHackProfileSectionNames)(LPCSTR lpHackName, DWORD& cSize);
typedef	BOOL	(__cdecl *fnGameSendPacketToServer)(LPBYTE buf, DWORD len);
typedef BOOL	(__cdecl *fnGameCommandLine)(char* buf);
typedef BOOL	(__cdecl *fnIntercept)(int instruction, DWORD lpSource, DWORD lpDest, int len);
typedef	BOOL	(__cdecl *fnGetFingerprint)(LPCSTR szHackName, LPCSTR szFingerprintName, FINGERPRINTSTRUCT &fps);
typedef BOOL	(__cdecl *fnSetHackProfileString)(LPCSTR lpHackName, LPCSTR lpSectionName, LPCSTR lpKeyName, LPCSTR lpValue);
typedef	BOOL	(__cdecl *fnGameSendPacketToGame)(LPBYTE buf, DWORD len);
typedef	BOOL	(__cdecl *fnGameInsertPacketToGame)(LPBYTE buf, DWORD len);
typedef	BOOL	(__cdecl *fnGameSendMessageToChat)(LPSTR msg);
typedef BOOL	(__cdecl *fnD2FindFile)(char* szDest, const char* szFileName);
//typedef LPVOID	(__cdecl *fnGetInfoStruct)(DWORD dwGetStruct, const char* szClientName);
typedef PTHISGAMESTRUCT (__cdecl *fnGetThisgameStruct)(void);
typedef PSERVERINFO (__cdecl *fnGetServerInfo)(void);


typedef struct functionentrypoints_t
{
	fnGamePrintString				GamePrintString;
	fnGamePrintInfo					GamePrintError;
	fnGamePrintError				GamePrintInfo;
	fnGamePrintVerbose				GamePrintVerbose;
	fnGetHackProfileString			GetHackProfileString;
	fnGetHackProfileStringEx		GetHackProfileStringEx;
	fnGetHackProfileSectionNames	GetHackProfileSectionNames;
	fnSetHackProfileString			SetHackProfileString;
	fnGameSendPacketToServer		GameSendPacketToServer;
	fnGameCommandLine				GameCommandLine;
	fnIntercept						Intercept;
	fnGetFingerprint				GetFingerprint;
	fnGetThisgameStruct				GetThisgameStruct;
	fnGetServerInfo					GetServerInfo;			// 0.59
	fnGameSendPacketToGame			GameSendPacketToGame;
	fnGameInsertPacketToGame		GameInsertPacketToGame;	// 0.59
	fnGameSendMessageToChat			GameSendMessageToChat;	// 0.59
	fnD2FindFile					D2FindFile;
//	fnGetInfoStruct					GetInfoStruct;
} FUNCTIONENTRYPOINTS;
extern FUNCTIONENTRYPOINTS		*fep;

extern LinkedList	ClientList;
extern LinkedList	szBufferList;
extern LPD2PARAM	g_pD2Param;



//////////////////////////////////////////////////////////////////////
// CLIENTINFOSTRUCT
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
typedef DWORD	(__cdecl *fnOnGamePacketBeforeSent)(BYTE* aPacket, DWORD aLen);
typedef DWORD	(__cdecl *fnOnGamePacketBeforeReceived)(BYTE* aPacket, DWORD aLen);
typedef	BOOL	(__cdecl *fnOnGameCommandLine)(char** argv, int argc);
typedef	VOID	(__cdecl *fnOnGameJoin)(THISGAMESTRUCT* thisgame);
typedef	VOID	(__cdecl *fnOnGameLeave)(THISGAMESTRUCT* thisgame);
typedef LPCSTR	(__cdecl *fnGetModuleAuthor)();
typedef	LPCSTR	(__cdecl *fnGetModuleWebsite)();
typedef LPCSTR	(__cdecl *fnGetModuleEmail)();
typedef LPCSTR	(__cdecl *fnGetModuleDescription)();
typedef	DWORD	(__cdecl *fnGetModuleVersion)();
typedef	DWORD	(__cdecl *fnOnGameTimerTick)();
typedef BOOL	(__cdecl *fnOnClientStart)();
typedef BOOL	(__cdecl *fnOnClientStop)();

typedef struct clientinfostruct_t
{
	HANDLE	hModule;			// Handle to DLL
	char*	Name;

	// Event handlers
	fnOnGamePacketBeforeSent		OnGamePacketBeforeSent;
	fnOnGamePacketBeforeReceived	OnGamePacketBeforeReceived;
	fnOnGameCommandLine				OnGameCommandLine;
	fnOnGameLeave					OnGameLeave;
	fnOnGameJoin					OnGameJoin;
	// Property get
	fnGetModuleAuthor				GetModuleAuthor;
	fnGetModuleWebsite				GetModuleWebsite;
	fnGetModuleEmail				GetModuleEmail;
	fnGetModuleDescription			GetModuleDescription;
	fnGetModuleVersion				GetModuleVersion;

	// Added v0.42
	fnOnGameTimerTick				OnGameTimerTick;

	// Added v0.44
	fnOnClientStart					OnClientStart;
	fnOnClientStop					OnClientStop;

	// Added v0.58
	char*	pDir;	// For GetHackProfileString
} CLIENTINFOSTRUCT;


#define BIND(x)		cds->##x##=(fn##x##)GetProcAddress((HMODULE)hModule,#x ##);