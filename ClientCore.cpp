//////////////////////////////////////////////////////////////////////
// ClientCore.cpp
// -------------------------------------------------------------------
// Default Client Dll entrypoint.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4530)	// Disable warnings for exceptions
#include "D2Hackit.h"
#include "D2Client.h"
#pragma warning( default : 4530)	// Enable warnings for exceptions


//////////////////////////////////////////////////////////////////////
// Global server struct holding pointers to exported functions
//////////////////////////////////////////////////////////////////////
FUNCTIONENTRYPOINTS		*server;
HMODULE					g_hModule;					// New Global
char					g_szModuleDir[MAX_PATH];	// New Global


//////////////////////////////////////////////////////////////////////
// Dll entry/exit
//////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	
	char	szTemp[MAX_PATH];
	int		i;


	BOOL hResult = TRUE;
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:

			g_hModule = (HMODULE)hModule;
			GetModuleFileName( g_hModule, szTemp, sizeof(szTemp) );

			for( i=strlen(szTemp); i != 0; i-- )
			{
				if( szTemp[ i ] == '\\' )
				{
					szTemp[ i ] = '\0';
					break;
				}
			}
			sprintf( g_szModuleDir, szTemp );

			// Create server struct			
			server = new FUNCTIONENTRYPOINTS; 

			// Bind exported functions from server
			HMODULE hModule;
			hModule = (HMODULE)GetModuleHandle("D2HackIt");
			
			// Macros make this look a lot nicer :)
			BIND_TO_SERVER(GamePrintInfo);
			BIND_TO_SERVER(GamePrintError);
			BIND_TO_SERVER(GamePrintVerbose);
			BIND_TO_SERVER(GamePrintString);
			BIND_TO_SERVER(GameCommandLine);
			BIND_TO_SERVER(GameSendPacketToServer);
			BIND_TO_SERVER(GameSendPacketToGame);
			BIND_TO_SERVER(GetFingerprint);
			BIND_TO_SERVER(Intercept);
			BIND_TO_SERVER(GetHackProfileString);
			BIND_TO_SERVER(GetHackProfileStringEx);
			BIND_TO_SERVER(GetHackProfileSectionNames);	// v0.58
			BIND_TO_SERVER(SetHackProfileString);
			BIND_TO_SERVER(GetThisgameStruct);
			BIND_TO_SERVER(D2FindFile);					// v0.58

			// 0.59 exports
			BIND_TO_SERVER(GetServerInfo);
			BIND_TO_SERVER(GameInsertPacketToGame);

			// initiate client
			// hResult = OnClientStart();			// D2HackIt calls it already
			break;

		case DLL_PROCESS_DETACH:
			// kill client

			// hResult = OnClientStop();			// D2HackIt calls it already

			delete server;
			break;
    } 
    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Stubfunctions for 'property get' functions.
//////////////////////////////////////////////////////////////////////
LPCSTR	EXPORT GetModuleAuthor()		{return ModuleAuthor;}
LPCSTR	EXPORT GetModuleWebsite()		{return ModuleWebsite;}
DWORD	EXPORT GetModuleVersion()		{return ModuleVersion;}
LPCSTR	EXPORT GetModuleEmail()			{return ModuleEmail;}
LPCSTR	EXPORT GetModuleDescription()	{return ModuleDescription;}

//////////////////////////////////////////////////////////////////////
// OnClientCommandLine
// -------------------------------------------------------------------
// The modules own extension of the command line interface. Any custom
// commands you add are parsed here.
//
// Return value should be TRUE, but it is not used at this 
// time.
//
// Arguments when we get here:
// argv[0]			Name of module
// argv[1]			Name of command (If supplied)
// argv[2 ... n]	The rest
//
// Syntax in the game: .<module> <arguments>
//////////////////////////////////////////////////////////////////////
BOOL EXPORT OnGameCommandLine(char* argv[], int argc)
{
	// Check if user supplied anything at all, if not assume help...
	if (argc==1)
		argv[argc++]="help";


	MODULECOMMANDSTRUCT* mcs=ModuleCommands;

	while (mcs->szName) {
		if (!stricmp(mcs->szName, argv[1]))
			break;
		mcs++;
	}

	char *p,*t,*msg,*fMsg;
	fMsg=new char[256];
	//
	// Is this a built-in function ?
	if (mcs->szName) {
		//
		// If functions returns false, show usage help
		if (!mcs->pFunc(argv, argc)) {
			t=new char[strlen(mcs->szUsage)+1];
			server->GamePrintInfo("Usage:");
			sprintf((char*)t, "%s", mcs->szUsage);
			if (strlen((char*)t))
			{
				msg=p=t;
				while (*p != 0) {
					if (*p == '\n') 
					{
						*(p++) = 0;
						sprintf(fMsg, "ÿc4.%s %s", argv[0], msg);
						server->GamePrintInfo((char*)fMsg);
					if (*p != 0)
						msg = p;
					} else
						p++;
					}
				sprintf(fMsg, "ÿc4.%s %s", argv[0], msg);
				server->GamePrintInfo((char*)fMsg);
			}
			delete t;
		}
	} else {
	// Unknown command, show catch-all help phraze.
	t=new char[128];
	sprintf(t, "Unknown command ÿc4'%s %s'ÿc0 - try ÿc4'.%s help'ÿc0 to get help.",
		argv[0], argv[1], argv[0]);
	server->GamePrintError(t);
	delete t;
	}
	delete fMsg;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// OnClientCommandHelp
// -------------------------------------------------------------------
// Our default help function.
//
// Syntax in the game: .<module> <arguments>
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandHelp(char** argv, int argc)
{
	// If command line is longer than 2, show usage for 'help'
	if (argc>2) return FALSE;

	char t[1024];
	sprintf(t, "Available commands for %s:", argv[0]);
	server->GamePrintInfo(t);

	// Loop through ModuleCommands[] and print their names
	for (int i=0; ModuleCommands[i].szName != NULL; i++)
	{
		sprintf(t, "ÿc4.%s %s", argv[0], ModuleCommands[i].szName);
		server->GamePrintInfo(t);
	}

	sprintf(t, "For help on a specific command, type ÿc4.%s <command> help", argv[0]);
	server->GamePrintInfo(t);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// assert()
// -------------------------------------------------------------------
// Thanks to ClckWcher for contributing assert code
//
// Use assert(expression) or assert(expression, "Message") in your 
// module.
//
//////////////////////////////////////////////////////////////////////
#define assert(exp) (void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )
#define msg_assert(exp,msg) (void)( (exp) || (_assert(msg, __FILE__, __LINE__), 0) )
void _assert(char * str,char * file,int line)
{
	char t[1024];
	sprintf(t, "Assertion failed: ÿc4%sÿc0, file ÿc4%sÿc0, line ÿc4%d", 		str, file, line);
	server->GamePrintError(t);
}
