//////////////////////////////////////////////////////////////////////
// anim8.cpp
// -------------------------------------------------------------------
// Plays animated .overhead messages
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\ClientCore.cpp"	  // Include the backend of the module


//////////////////////////////////////////////////////////////////////
// Global stuff.
//////////////////////////////////////////////////////////////////////
char* iniFile;
BOOL PRIVATE OnGameCommandPlay(char** argv, int argc);
DWORD WINAPI PlayThread(LPVOID lpParameter);

//////////////////////////////////////////////////////////////////////
// CLIENTINFO
//////////////////////////////////////////////////////////////////////
CLIENTINFO
(		
	1,0,							// Module version (Major, Minor)
	thohell,						// Author
	thohell.d2network.com,			// url (http:// is appended)
	Anim8 plugin,					// Short module description
	thohell@home.se					// Author eMail
)

//////////////////////////////////////////////////////////////////////
// MODULECOMMANDSTRUCT ModuleCommands[]
//////////////////////////////////////////////////////////////////////
MODULECOMMANDSTRUCT ModuleCommands[]=
{
	{	// The 'help' command is implemented in ClientCore.cpp and 
		// should not need to be changed.
		"help",
		OnGameCommandHelp,
		"helpÿc0 List commands available in this module.\n"
		"<command> helpÿc0 Shows detailed help for <command> in this module."
	},
	{
		"play",
		OnGameCommandPlay,
		"play <section>ÿc0 Plays animation from [section] in anim8.ini"
	},
	// Add your own commands here

	{NULL}	// No more commands
};

//////////////////////////////////////////////////////////////////////
// OnClientStart
// -------------------------------------------------------------------
// Runs *once* when the module loads. 
// This function can be removed.
//
// Put any initialization code here.
//
// Return FALSE to prevent module from loading.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT OnClientStart()
{
	// Get path to inifile

	// Find Diablo II's hwnd
	DWORD pid=0;
	HWND hwnd = FindWindow("Diablo II", "Diablo II");	// Get hwnd
	GetWindowThreadProcessId(hwnd, &pid);

	// Typedefs for toolhelp32
	typedef BOOL (WINAPI *fnModule32First)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	typedef BOOL (WINAPI *fnModule32Next)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	typedef HANDLE (WINAPI *fnCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);  

	// Get entrypoints
	HMODULE k32=GetModuleHandle("kernel32.dll");
	fnModule32First Module32First = (fnModule32First)GetProcAddress(k32, "Module32First");
	fnModule32Next Module32Next = (fnModule32Next)GetProcAddress(k32, "Module32Next");
	fnCreateToolhelp32Snapshot CreateToolhelp32Snapshot=(fnCreateToolhelp32Snapshot)GetProcAddress(k32,"CreateToolhelp32Snapshot");
	
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	MODULEENTRY32 lpme; lpme.dwSize=sizeof(MODULEENTRY32);
	// Get first module, this is needed for win9x/ME
	Module32First(hSnapshot, &lpme);

	// Loop through all modules
	while (TRUE)
	{
		if (!stricmp(lpme.szModule, "anim8.d2h")) 
		{
			// Found it, make path to ini file!
			int i=strlen(lpme.szExePath);
			iniFile=new char[i+1];
			strcpy(iniFile, lpme.szExePath);
			iniFile[--i]='i';
			iniFile[--i]='n';
			iniFile[--i]='i';
			
			CloseHandle(hSnapshot);

			return TRUE; 
		}
		if (!Module32Next(hSnapshot, &lpme))
			return FALSE;	// Get next
	}
}

//////////////////////////////////////////////////////////////////////
// OnClientStop
//////////////////////////////////////////////////////////////////////
BOOL EXPORT OnClientStop()
{
	delete iniFile;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// OnGameCommandPlay
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandPlay(char** argv, int argc)
{
	if (argc!=3)
		return FALSE;

	// Start PlayThread
	DWORD dummy=0;
	CreateThread(NULL,0,PlayThread,(void*)argv[2],0,&dummy);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// PlayThread
//////////////////////////////////////////////////////////////////////
DWORD WINAPI PlayThread(LPVOID lpParameter)
{
	char* section=new char[strlen((char*)lpParameter)+4];
	sprintf(section, "[%s]", strlwr((char*)lpParameter));

	char t[1024];
	BYTE aPacket[1024];
	int sleep=0;

	FILE *f;

	// Try to open file
	if(!(f=fopen(iniFile, "r")))
	{
		sprintf(t, "ÿc4Anim8ÿc0: Unable to open file %s", iniFile);
		server->GamePrintError(t);
		return 0;
	}

	// Find [section]
	for (;;)
	{
		fgets(t,1023,f);
		if(feof(f))
		{
			// eof == error
			sprintf(t, "ÿc4Anim8ÿc0: Unable to find section %s", section);
			server->GamePrintError(t);
			return 0;
		}
		// Remove trailing space
		if (t[0])
			t[strlen(t)-1]=0;
		if (!stricmp(t,section))
			break;
	} 

	// We now have the right section, get speed
	fgets(t,1023,f);

	char *x="";
	sleep = strtoul(t, &x, 10);

	// Set default = 100ms
	if (!sleep)
		sleep=100;


	while (!feof(f))
	{
		fgets(t,1023,f);
		
		if (t[0]=='[')
			break;

		sprintf((char*)aPacket, "%c%c%c%s%c%c%c",
			0x14, 0x00, 0x00, t, 0,0,0);
		server->GameSendPacketToServer(aPacket, strlen(t)+6);

		Sleep(sleep);
	}

	fclose(f);

	return 0;
}

