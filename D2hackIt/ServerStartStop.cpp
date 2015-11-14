//////////////////////////////////////////////////////////////////////
// ServerStartStop.cpp
// -------------------------------------------------------------------
// Initialize/destroy server.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

// These are the dll's we want to force-load to get them in memory.
//char* NeededDlls[] = { "D2Common.dll", "D2Game.dll", "D2Multi.dll", "D2Client.dll", NULL };
char* NeededDlls[] = { "D2Common.dll", "D2Game.dll", "D2Client.dll", NULL };
//////////////////////////////////////////////////////////////////////
// ServerStart()
// -------------------------------------------------------------------
// Responsible for setting up the server.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE ServerStart(HANDLE hModule)
{
	// Temporary string
	LPSTR t=NULL; 

	//////////////////////////////////////////////////////////////////
	// Before anything else, create the global structures we use in 
	// the hack. Make sure we delete these in ServerStop.
	//////////////////////////////////////////////////////////////////
	si =	new SERVERINFO;
	psi =	new PRIVATESERVERINFO;
	fep =	new FUNCTIONENTRYPOINTS;
	pfep=	new PRIVATEFUNCTIONENTRYPOINTS;

	thisgame=new THISGAMESTRUCT;
	thisgame->player=NULL;

	//////////////////////////////////////////////////////////////////
	// Force-load needed dll's so we can patch thei'r memory space.
	// We should unload these is ServerStop.
	//////////////////////////////////////////////////////////////////
	for (int i=0; NeededDlls[i] != NULL; i++) LoadLibrary(NeededDlls[i]);

	//////////////////////////////////////////////////////////////////
	// Build initial data of the SERVERINFO structure
	//////////////////////////////////////////////////////////////////
	si->Version=__SERVERVERSION__;

	// Get plugin path
	t=new char[_MAX_PATH];
	if (!GetModuleFileName((HINSTANCE)hModule, t, _MAX_PATH))
		{ MessageBox(NULL, "Unable to get PluginPath!", "D2Hackit Error!", MB_ICONERROR); return FALSE; }
	int p=strlen(t);
	while (p) 
	{
		if (t[p] == '\\')
			{ t[p] = 0; p=0;}
		else
			p--;
	}
	si->PluginDirectory=new char[strlen(t)+1];
	strcpy((LPSTR)si->PluginDirectory, t);

	psi->DontShowErrors=FALSE;
	//////////////////////////////////////////////////////////////////
	// Build initial data of the PRIVATESERVERINFO structure
	//////////////////////////////////////////////////////////////////
	sprintf(t, "%s\\D2HackIt.ini", t);
	psi->IniFile=new char[strlen(t)+1];
	strcpy((LPSTR)psi->IniFile, t);
	delete t;
	

	/* This block of code is replaced by the single GetCurrentProcessId()
	   call below.  This works because DLL initialization is performed
	   within the context of the process to which the DLL is attaching -
	   i.e. under that process's PID

	// Get Diablo II's hwnd
	psi->hwnd = FindWindow("Diablo II", "Diablo II");	// Get hwnd
		if (!psi->hwnd) { MessageBox(NULL, "Can't get Diablo II's window handle.", "D2Hackit Error!", MB_ICONERROR); return FALSE; }
	
	// Get Diablo II's pid & Process handle


	GetWindowThreadProcessId(psi->hwnd, &psi->pid);
	*/

    // Get the process ID and the process handle
	psi->pid = GetCurrentProcessId();
	psi->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, psi->pid);
		if (!psi->hProcess) { MessageBox(NULL, "Can't get Diablo II's process handle.", "D2Hackit Error!", MB_ICONERROR); return FALSE;}


	// Get build date/time
	strcpy(psi->BuildDate, __DATE__);
	strcpy(psi->BuildTime, __TIME__);

	//////////////////////////////////////////////////////////////////
	// Build initial callbacks in the FUNCTIONENTRYPOINTS structure.
	//////////////////////////////////////////////////////////////////
//	fep->GetMemoryAddressFromPattern=&GetMemoryAddressFromPattern;
	fep->GamePrintString=&GamePrintString;
	fep->GamePrintInfo=&GamePrintInfo;
	fep->GamePrintVerbose=&GamePrintVerbose;
	fep->GamePrintError=&GamePrintError;
	fep->GetHackProfileString=&GetHackProfileString;
	fep->GetHackProfileStringEx=&GetHackProfileStringEx;
	fep->GetHackProfileSectionNames=&GetHackProfileSectionNames;
	fep->GameSendMessageToChat=&GameSendMessageToChat;

	//////////////////////////////////////////////////////////////////
	// Build initial callbacks in PRIVATEFUNCTIONENTRYPOINTS 
	//////////////////////////////////////////////////////////////////
	pfep->GetBaseAddress=&GetBaseAddress;
	pfep->GetImageSize=&GetImageSize;

	//////////////////////////////////////////////////////////////////
	// Check if D2HackIi.ini exisits
	//////////////////////////////////////////////////////////////////
	if (_access(psi->IniFile, 0))
	{ 
		LPSTR t=new char[strlen(psi->IniFile)+50];
		sprintf(t, "Unable to open ini-file:\n%s", psi->IniFile);
		MessageBox(NULL, t, "D2Hackit Error!", MB_ICONERROR); 
		delete t;
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////
	// Set default prompts
	//////////////////////////////////////////////////////////////////
	t=fep->GetHackProfileString("D2HackIt", "Misc", "InfoPrompt");
	lstrcpyn(psi->InfoPrompt, (strlen(t)?t:DEFAULTINFOPROMPT),MAXPROMPTLENGTH-1);
	delete t;

	t=fep->GetHackProfileString("D2HackIt", "Misc", "ErrorPrompt");
	lstrcpyn(psi->ErrorPrompt, (strlen(t)?t:DEFAULTERRORPROMPT),MAXPROMPTLENGTH-1);
	delete t;

	t=fep->GetHackProfileString("D2HackIt", "Misc", "VerbosePrompt");
	lstrcpyn(psi->VerbosePrompt, (strlen(t)?t:DEFAULTVERBOSEPROMPT),MAXPROMPTLENGTH-1);
	
	t=fep->GetHackProfileString("D2HackIt", "Misc", "Verbose");
	if (!stricmp(t, "on"))
		psi->Verbose = TRUE;
	else
		psi->Verbose = FALSE;

	delete t;

	//////////////////////////////////////////////////////////////////
	// Start by binding a way to print to screen. This is vital, so if
	// we are unable to do this, exit with an error message!
	//////////////////////////////////////////////////////////////////
	FINGERPRINTSTRUCT fps;
	if(!GetFingerprint("D2HackIt", "GamePrintStringLocation", fps))
		{ MessageBox(NULL, "Fingerprint information for 'GamePrintStringLocation'\nmissing or corrupt!", "D2Hackit Error!", MB_ICONERROR); return FALSE; }
	
	psi->GamePrintStringLocation=fps.AddressFound;
	if (!psi->GamePrintStringLocation)
		{ MessageBox(NULL, "Unable to find entrypoint for 'GamePrintStringLocation'!", "D2Hackit Error!", MB_ICONERROR); return FALSE; }


	// Get playerinfo struct
	if (!GetFingerprint("D2HackIt", "pPlayerInfoStruct", fps)) 
		{ fep->GamePrintError("Fatal error! Exiting!"); return FALSE; }

	// Messy pointers :)
	thisgame->player=(PLAYERINFOSTRUCT*)*(DWORD*)(*(DWORD*)fps.AddressFound);


	// Get gameinfo struct
	if (!GetFingerprint("D2HackIt", "pPlayerInfoStruct", fps)) 
		{ fep->GamePrintError("Fatal error! Exiting!"); return FALSE; }

	// Get GameSendMessageToChat
	psi->GameSendMessageToChatLocation = NULL;
	if(GetFingerprint("D2HackIt", "GameSendMessageToChat", fps))
		psi->GameSendMessageToChatLocation = fps.AddressFound;

	// Messy pointers :)
	//thisgame->CurrentGame=(GAMESTRUCT*)*(DWORD*)(*(DWORD*)fps.AddressFound);
	
	// Initialize critical section
	InitializeCriticalSection(&psi->csData);

	// Initialize insert receive buffers
	psi->nRecvBufferPos = 0;
	
	//////////////////////////////////////////////////////////////////
	// Print startup banner. 
	//////////////////////////////////////////////////////////////////
	t=new char[128];
	sprintf(t, "Starting D2HackIt! Mk2 version %d.%.2d (%s@%s)",
		LOWORD(si->Version), HIWORD(si->Version), psi->BuildDate, psi->BuildTime
		);
		fep->GamePrintInfo(t);

	//////////////////////////////////////////////////////////////////
	// Get loader data
	//////////////////////////////////////////////////////////////////
	BOOL UsingD2Loader=FALSE;
	psi->DontShowErrors=TRUE;
	if (!GetFingerprint("D2HackIt", "LoaderStruct", fps))
	{
		psi->DontShowErrors=FALSE;
		sprintf(fps.ModuleName, "Diablo II.exe");
		UsingD2Loader=TRUE;
		if ((fps.AddressFound=GetMemoryAddressFromPattern(fps.ModuleName, fps.FingerPrint, fps.Offset)) < 0x100)
		{
			sprintf(fps.ModuleName, "D2Loader.exe");
			if ((fps.AddressFound=GetMemoryAddressFromPattern(fps.ModuleName, fps.FingerPrint, fps.Offset)) < 0x100)
			{
				//fep->GamePrintError("Unable to find loader data in 'Game.exe', 'Diablo II.exe' or 'D2Loader.exe'!");
				//fep->GamePrintError("Fatal error! Exiting!"); return FALSE; 
				fps.AddressFound=0;
			}	
		}
	}
	
	psi->loader = (LOADERDATA*)fps.AddressFound;

	if (psi->loader) 
	{
		sprintf(t, "Loader version ÿc4%d.%.2d. ÿc0Game is %sstarted with D2Loader.",
			LOWORD(psi->loader->LoaderVersion), HIWORD(psi->loader->LoaderVersion),
			(UsingD2Loader?"":"ÿc4not ÿc0"));
	} else {
		sprintf(t, "D2Hackit was loaded without loader");
	}
	fep->GamePrintInfo(t);

		
	//////////////////////////////////////////////////////////////////
	// Continue binding entrypoints and intercepts
	//////////////////////////////////////////////////////////////////
	sprintf(t, "Found 'GamePrintStringLocation' at %.8x", psi->GamePrintStringLocation);
	fep->GamePrintVerbose(t);

	/* 
	 * No need for this
	 *
	// Get socket location
	if (!GetFingerprint("D2HackIt", "pGameSocketLocation", fps)) 
		{ fep->GamePrintError("Fatal error! Exiting!"); return FALSE; }
	psi->GameSocketLocation=*(DWORD*)fps.AddressFound;
	*/

	// Get GamePacketReceivedIntercept
	if (!GetFingerprint("D2HackIt", "GamePacketReceivedIntercept", psi->fps.GamePacketReceivedIntercept)) 
		{ fep->GamePrintError("Fatal error! Exiting!"); return FALSE; }

	Intercept(INST_CALL, psi->fps.GamePacketReceivedIntercept.AddressFound, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.PatchSize);

	// Get GamePacketReceivedIntercept2
	if (!GetFingerprint("D2HackIt", "GamePacketReceivedIntercept2", psi->fps.GamePacketReceivedIntercept2)) 
	{ 
		fep->GamePrintError("Fatal error! Exiting!"); 
		Intercept(INST_CALL, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept.PatchSize);
		return FALSE; 
	}
	Intercept(INST_JMP, psi->fps.GamePacketReceivedIntercept2.AddressFound, (DWORD)&GamePacketReceivedIntercept2STUB, psi->fps.GamePacketReceivedIntercept2.PatchSize);

	// Get GamePacketSentIntercept
	if (!GetFingerprint("D2HackIt", "GamePacketSentIntercept", psi->fps.GamePacketSentIntercept)) 
	{ 
		fep->GamePrintError("Fatal error! Exiting!"); 
		Intercept(INST_JMP, (DWORD)&GamePacketReceivedIntercept2STUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept2.PatchSize);
		Intercept(INST_CALL, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept.PatchSize);
		return FALSE; 
	}
	Intercept(INST_CALL, psi->fps.GamePacketSentIntercept.AddressFound, (DWORD)&GamePacketSentInterceptSTUB, psi->fps.GamePacketSentIntercept.PatchSize);
	

	// Get GamePlayerInfoIntercept
	if (!GetFingerprint("D2HackIt", "GamePlayerInfoIntercept", psi->fps.GamePlayerInfoIntercept)) 
	{ 
		fep->GamePrintError("Fatal error! Exiting!"); 
		Intercept(INST_CALL, (DWORD)&GamePacketSentInterceptSTUB, psi->fps.GamePacketSentIntercept.AddressFound, psi->fps.GamePacketSentIntercept.PatchSize);
		Intercept(INST_JMP, (DWORD)&GamePacketReceivedIntercept2STUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept2.PatchSize);
		Intercept(INST_CALL, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept.PatchSize);
		return FALSE; 
	}
	Intercept(INST_CALL, psi->fps.GamePlayerInfoIntercept.AddressFound, (DWORD)&GamePlayerInfoInterceptSTUB, psi->fps.GamePlayerInfoIntercept.PatchSize);


	// Get GameSendPacketToGameLocation
	// Thanks to TechWarrior
	if (!GetFingerprint("D2HackIt", "GameSendPacketToGameLocation", fps)) 
	{ 
		fep->GamePrintError("Fatal error! Exiting!"); 
		Intercept(INST_CALL, (DWORD)&GamePacketSentInterceptSTUB, psi->fps.GamePacketSentIntercept.AddressFound, psi->fps.GamePacketSentIntercept.PatchSize);
		Intercept(INST_JMP, (DWORD)&GamePacketReceivedIntercept2STUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept2.PatchSize);
		Intercept(INST_CALL, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept.PatchSize);
		Intercept(INST_CALL, (DWORD)&GamePlayerInfoInterceptSTUB, psi->fps.GamePlayerInfoIntercept.AddressFound, psi->fps.GamePlayerInfoIntercept.PatchSize);
		return FALSE; 
	}
	psi->GameSendPacketToGameLocation=fps.AddressFound;

	// Start TickThread, We dont care about closing it later.
	// It will be destroyed when unloading the dll.
	DWORD dummy=0;
	psi->TickShutDown = 0;
	psi->TickThreadHandle = CreateThread(NULL,0,TickThread,(void*)&ClientList,0,&dummy);
	psi->TickThreadActive = psi->TickThreadHandle!=NULL;

	// Load any clients listed in Autorun
	t=new char[1024];
	t=fep->GetHackProfileString("D2HackIt", "Misc", "Autoload");

	if (strlen(t))
	{
		char* command[2];
		command[0]=".load";
		char *p;
		p=t;

		command[1] = p;
		while (*p != 0) {
			if (*p == ',') {
				*(p++) = 0;
				GameCommandLineLoad(command,2);
			while (*p == ' ')
						p++;
				if (*p != 0)
					command[1] = p;
			} else
				p++;
		}
		GameCommandLineLoad(command,2);
	}

	delete t;

	fep->GamePrintInfo("D2HackIt! Mk2 Loaded! Type ÿc4.helpÿc0 for help on commands.");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// ServerStop()
// -------------------------------------------------------------------
// Responsible for stopping the server.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE ServerStop(void)
{
	if(psi->TickThreadActive) {
		int i;
		// Give TickThread 2.5 seconds max to clean up before killing it
		for(psi->TickShutDown=1,i=1; i<25 && 1==psi->TickShutDown; ++i) {
			Sleep(100);
		}
		if(1==psi->TickShutDown) {
			TerminateThread(psi->TickThreadHandle, 1234);
		}
		CloseHandle(psi->TickThreadHandle);
	}

	// Unload any loaded clients
	while (ClientList.GetItemCount())
	{
		LinkedItem *li=ClientList.GetLastItem();
		CLIENTINFOSTRUCT *cds=(CLIENTINFOSTRUCT*)li->lpData;
		char t[32];
		sprintf(t, "unload %s", cds->Name);
		GameCommandLine(t);
	}


	// Un-patch intercept locations
	Intercept(INST_CALL, (DWORD)&GamePlayerInfoInterceptSTUB, psi->fps.GamePlayerInfoIntercept.AddressFound, psi->fps.GamePlayerInfoIntercept.PatchSize);
	Intercept(INST_CALL, (DWORD)&GamePacketSentInterceptSTUB, psi->fps.GamePacketSentIntercept.AddressFound, psi->fps.GamePacketSentIntercept.PatchSize);
	Intercept(INST_JMP, (DWORD)&GamePacketReceivedIntercept2STUB, psi->fps.GamePacketReceivedIntercept2.AddressFound, psi->fps.GamePacketReceivedIntercept2.PatchSize);
	Intercept(INST_CALL, (DWORD)&GamePacketReceivedInterceptSTUB, psi->fps.GamePacketReceivedIntercept.AddressFound, psi->fps.GamePacketReceivedIntercept.PatchSize);

	// Delete critical section
	DeleteCriticalSection(&psi->csData);

	// Release dll's that we loaded upon entry.
	for (int i=0; NeededDlls[i] != NULL; i++) FreeLibrary (GetModuleHandle(NeededDlls[i]));

	fep->GamePrintInfo("D2HackIt! Mk2 Unloaded.");

	delete (LPSTR)si->PluginDirectory, (LPSTR)psi->IniFile, thisgame, pfep, fep, psi, si;

	LinkedItem*	pItem;		// Delete the GetHackProfileStrings
	while ( (pItem = szBufferList.GetFirstItem()) != NULL )
	{
		delete ( pItem->lpData );
		szBufferList.RemoveItem(pItem);
	}
	return TRUE;
}

