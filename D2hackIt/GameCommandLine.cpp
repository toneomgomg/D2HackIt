//////////////////////////////////////////////////////////////////////
// toolhelp.cpp
// -------------------------------------------------------------------
//
//	0.01 thohell
//	0.02 bootyjuice	Cleaned up tokenizer to support multiple commands
//					properly.  Also, can send multiple commands in
//					module parameters now. Also added click() 
//					function.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2Hackit.h"

/*
typedef struct _finddir_t
{
	LinkedItem*	pParent;
	char*		pName;
} finddir, *LPfinddir;
*/

const char cCommandSeparator = '|';

// This click function by Trojan is so simple I see no reason to modify it :)
void click( int x, int y, char* sWindowName) 
{  
	HWND handle;

	// If a window handle was supplied then use it otherwise just use
	// the currently active window.
	if( sWindowName )
	{
		handle=FindWindow("Diablo II",sWindowName);
	}
	else
	{
		handle=FindWindow("Diablo II",NULL);
	}

	int pos=((short)y<< 16)+(short)x;  
	SendMessage(handle,WM_MOUSEMOVE,NULL,pos);  
	SendMessage(handle,WM_LBUTTONDOWN,MK_LBUTTON,pos);  
	SendMessage(handle,WM_LBUTTONUP,MK_LBUTTON,pos);
}


//
//	Initialize commands
GAMECOMMANDSTRUCT GameCommands[] = {
	{
		"help",
		GameCommandLineHelp,
		"ÿc4.helpÿc0 List all commands available in the server dll\n"
		"ÿc4.<command> helpÿc0 Shows detailed help for specified command.\n"
		"ÿc4.<module> helpÿc0 Shows help for specified module."
	},
	{
		"load",
		GameCommandLineLoad,
		"ÿc4.load <module>ÿc0 Loads the specified module."
	},
	{
		"unload",
		GameCommandLineUnload,
		"ÿc4.unload <module>ÿc0 Unloads the specified module."
	},
	{
		"list",
		GameCommandLineList,
		"ÿc4.listÿc0 Lists all currently loaded modules."
	},
	{
		"version",
		GameCommandLineVersion,
		"ÿc4.versionÿc0 Displays detailed version information on the D2Hackit server dll."
	},
	{
		"credits",
		GameCommandLineCredits,
		"ÿc4.credits"
	},
	{
		"set",
		GameCommandLineSet,
		"ÿc4.set verbose <on|off>ÿc0 Verbose output (Can also be set in D2HackIt.ini)"
	},
	{
		"send",
		GameCommandLineSend,
		"ÿc4.send <packet>ÿc0 Sends a packet to the server. Great for testing stuff :)\n"
		"The packet is not validated so please; no spaces, only hexadecimal number and even number of chars!\n"
		"Try ÿc4'.send 500100000001000000'ÿc0 in a game you created."
	},
	{
		"receive",
		GameCommandLineReceive,
		"ÿc4.receive <packet>ÿc0 Fakes a packet from the server to the game. Great for testing stuff :)\n"
		"The packet is not validated so please; no spaces, only hexadecimal number and even number of chars!"
	},
	/*	
	{
		"sendkey",
		GameCommandLineSendkey,
		"ÿc4.sendkey <keycode>ÿc0 Sends a keystroke to the game"
	},
	*/
	{
		"say",
		GameCommandLineSay,
		"ÿc4.say <text>ÿc0 Sends a string of text to the game as if your character said it."
	},
	{
		"overhead",
		GameCommandLineOverhead,
		"ÿc4.overhead <text>ÿc0 Puts a box of text over your head."
	},
	{
		"repeat",
		GameCommandLineRepeat,
		"ÿc4.repeat <count> <command>ÿc0 Repeats <command>, <count> number of times"
	},
	/*
	{
		"test",
		GameCommandLineTest,
		"ÿc4.testÿc0 This does nothing..."
	},
	*/
	{
		"crash",
		GameCommandLineCrash,
		"ÿc4.crashÿc0 This crashes the game in those rare cases when you *can't* exit a game!"
	},
	{
		"click",
		GameCommandLineClick,
		"ÿc4.click <x-coords> <y-coords> [window_name]ÿc0 Simulates a mouse click at x,y coordinates in window window_name."
	},
	{
		"disconnectchat",
		GameCommandLineDisconnectChat,
		"ÿc4.disconnectchatÿc0 Disconnect from chat server."
	},
	{
		NULL,
	}
};

//////////////////////////////////////////////////////////////////////
// FindGameCommand
// -------------------------------------------------------------------
// Returns pointer to a COMMANDSTRUCT
//////////////////////////////////////////////////////////////////////
GAMECOMMANDSTRUCT* PRIVATE FindGameCommand(char* name)
{
	GAMECOMMANDSTRUCT* gcs = GameCommands;
	while (gcs->szName) {
		if (!stricmp(gcs->szName, name))
			return gcs;
		gcs++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLine
// -------------------------------------------------------------------
// This is where we take a command line, tokenize it, and send it to
// the right destination :)
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GameCommandLine(char* raw)
{
	char*     p;  
	char buf[1024];
	char	*t,*msg;
	char*	argv[1024];
	int		argc = 0;
	bool	keep_going = true;
	GAMECOMMANDSTRUCT*	gcs;
	 
	// Parse through the command line for tokens, string characters, and the 
	// command separator.

	// Copy the command line to a temporary buffer so we can mangle it.
	sprintf(buf, "%s", raw);

	// Initialize a few pointers to it.
	argv[0]=p=buf;

	// This is druttis' tokenizer 
	// that has been prettified by bootyjuice :)
	while ( keep_going == true )
	{
		// Parse through any initial spaces.
		while( *p == ' ' && *p != 0 )
		{
			p++;
		}
		
		// Did we reach the end of the command line already?
		if( p == 0 )
		{
			return FALSE;
		}
		
		// Make sure we're starting with argc count properly set.
		// This is important if we've encountered multiple commands via the
		// command separator.
		argc = 0;
		argv[argc++] = p;
		
		// Reset the loop flag.  This will only get set to true
		// when we find another command to execute i.e. via the
		// command separator.
		keep_going = false;
		
		//	Tokenize cmdline.
		while (*p != 0) 
		{

			// Spaces are our token separator.
			if (*p == ' ') 
			{

				// Set the first space encountered to null.
				*(p++) = 0;

				// Parse past the rest until we get to the start of the next
				// token.
				while (*p == ' ')
				{
					p++;
				}
				
				// If we're not at the end of cmdline then
				// this must be the start of the next token.
				if (*p != 0)
				{
					argv[argc++] = p;
				}
			} 
			else if (*p == '"') 
			{
				
				// We've encountered a " character so this must be the
				// start of a string token.
				// Remove the " character and adjust the token accordingly.
				*(p++) = 0;
				argv[argc - 1] = p;
				
				// Parse until we find the closing " character or the end of
				// cmdline.
				while ((*p != '"') && (*p != 0))
				{
					p++;
				}
				
				// If we're not at the end of cmdline then we must've 
				// found the closing " character.  Replace it with null and
				// parse ahead to the start of the next token.
				if (*p != 0) 
				{
					*(p++) = 0;
					while (*p == ' ')
					{
						p++;
					}
				}

				// If we're STILL not at the end of cmdline then
				// we must be at the start of the next token. Set
				// argv to point here and increment argc.
				if (*p != 0)
				{
					argv[argc++] =  p;
				}
			} 
			else if (*p == cCommandSeparator) 
			{
				// We've encountered the command separator.

				// Replace command separator with null character and move one
				// character forward.
				*(p++) = 0;

				// If there's another command to execute then flag it as such.
				if( *p != 0)
				{
					keep_going = true;

					// Parse ahead through whitespace.
					while (*p == ' ')
					{
						p++;
					}
				}
				else
				{
					// We've encountered the end of the command line 
					// so stop looking for additional commands.
					keep_going = false;
				}

				// Break out of the innermost loop.
				break;

			}
			else
			{
				// Parse through the token.
				p++;
			}

		}

		gcs = FindGameCommand(argv[0]);
		//
		// Is this a built-in function ?
		if (gcs) 
		{
			//
			// If functions returns false, show usage help
			if (!gcs->pFunc(argv, argc)) 
			{
				t=new char[strlen(gcs->szUsage)+1];
				fep->GamePrintInfo("Usage:");
				sprintf(t, "%s", gcs->szUsage);
				if (strlen(t))
				{
					msg=p=t;
					while (*p != 0) 
					{
						if (*p == '\n') 
						{
							*(p++) = 0;
							fep->GamePrintInfo(msg);
							if (*p != 0)
							{
								msg = p;
							}
						} 
						else
						{
							p++;
						}
					}
					fep->GamePrintInfo(msg);
				}
				delete t;
			}
		} 
		else 
		{	
			//
			// This is not a built in function
			// Do we have a module loaded with that name ?
			if (!ClientList.GetItemCount())
			{
				// We have no modules...
				fep->GamePrintError("ÿc0Unknown command. Type .help to get help.");
				return FALSE;
			}

			LinkedItem *li=ClientList.GetFirstItem();
			CLIENTINFOSTRUCT* cds;
			for(int i=0; i<ClientList.GetItemCount(); i++)
			{	
				cds=(CLIENTINFOSTRUCT*)li->lpData;
				if (!stricmp(argv[0], cds->Name))
				{
					// If module supports CLI, route the arguments to it
					if (cds->OnGameCommandLine)
						return cds->OnGameCommandLine(argv, argc);
					else
					{
						//
						// If not, just let the user know
						t=new char[128];
						sprintf(t, "Module ÿc4'%s'ÿc0 does not support the command line interface.", argv[0]);
						fep->GamePrintInfo(t);
						delete t;
						return FALSE;
					}
				}
				li=ClientList.GetNextItem(li);
			}
			fep->GamePrintError("ÿc0Unknown command. Type .help to get help.");
		}

	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineHelp
// -------------------------------------------------------------------
// Handles command line help
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineHelp(char** argv, int argc)
{
	if (argc>1) return FALSE;
	//
	// Header
	fep->GamePrintInfo("Available D2HackIt commands:");
	//
	// Loop through commands and write them to screen.
	char t[128];
	for (int i=0; GameCommands[i].szName != NULL; i++)
	{
		sprintf(t, "ÿc4.%s",GameCommands[i].szName);
		fep->GamePrintInfo(t);
	}
	//
	// The two extended modes of '.help'
	fep->GamePrintInfo("ÿc4.<command> help");
	fep->GamePrintInfo("ÿc4.<module> help");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineVersion
// -------------------------------------------------------------------
// 
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineVersion(char** argv, int argc)
{
	//
	// Show usage if we send too many arguments
	if (argc>1) return FALSE;
	//
	// Just print it :)
	char t[256];
	sprintf(t, "ÿc4D2HackIt! Mk2ÿc0 version %d.%.2d (%s@%s)",LOWORD(si->Version), HIWORD(si->Version), psi->BuildDate, psi->BuildTime);
	fep->GamePrintInfo(t);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineCredits
// -------------------------------------------------------------------
// 
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineCredits(char** argv, int argc)
{
	//
	// Show usage if we send too many arguments
	if (argc>1) return FALSE;
	//
	// Just print it :)
	fep->GamePrintInfo("CREDITS:");
	fep->GamePrintInfo("These people have in one way or another made this project possible:");
	fep->GamePrintInfo("ÿc4Druttis, mousepad, TechWarrior, Nyttzy99, BlackBird, hello2k1, Arsenic,");
	fep->GamePrintInfo("ÿc4onlyer, icky, RazorBladeEnema, Skull, proff, ClckWtchr");
	fep->GamePrintInfo("BIG thanks to all the beta testers, too many to fit here! :)");
	fep->GamePrintInfo("Also, thanks to d2network for hosting D2HackIt! ÿc4(http://thohell.d2network.com)");
	fep->GamePrintInfo("ÿc4`Socÿc0 wanted to be mentioned here, so I will mention him as well :)");
	fep->GamePrintInfo("Latest updates done by:");
	fep->GamePrintInfo("v0.57 - BootJuice - click, fixed '|', and added string support? \"\"");
	fep->GamePrintInfo("v0.58 - D'Flame - Plugin recursive directory search, improved GetHackProfileString");

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineLoad
// -------------------------------------------------------------------
// Loads a client
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineLoad(char** argv, int argc)
{
	if (argc != 2) return FALSE;
	if (!stricmp(argv[1], "help")) return FALSE;

	char t[1024];


	// Check if module is loaded
	LinkedItem *li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;
	for(int i=0; i<ClientList.GetItemCount(); i++)
	{	
		cds=(CLIENTINFOSTRUCT*)li->lpData;
		if (!stricmp(argv[1], cds->Name))
		{
			sprintf(t, "Module '%s' is already loaded!", argv[1]);
			fep->GamePrintError(t);
			return TRUE;
		}
		li=ClientList.GetNextItem(li);
	}

	// Try to load .d2h		-	Added directory searching code - DFlame

	char	szTemp[MAX_PATH];

	sprintf( szTemp, "%s.d2h", argv[1] );
	if( !D2FindFile(t, szTemp) )
	{
		fep->GamePrintError(t);
		return TRUE;
	}

	HMODULE hModule = (HMODULE)LoadLibrary(t);
	if (!hModule)
	{
		// Try to load dll
		//sprintf(t, "%s\\%s.d2h", si->PluginDirectory, argv[1]); - D'Flame looks like just try again
		hModule = (HMODULE)LoadLibrary(t);
		if (!hModule)
		{
			// Didn't find any
			sprintf(t, "Unable to load module '%s'!", argv[1]);
			fep->GamePrintError(t);
			return TRUE;
		}
	}


	cds=new CLIENTINFOSTRUCT;
	
	cds->hModule=hModule;
	cds->Name=new char[strlen(argv[1])+1];
	sprintf(cds->Name, "%s", argv[1]);

	int p=strlen(t);
	while (p) 
	{
		if (t[p] == '\\')
			{ t[p] = 0; p=0;}
		else
			p--;
	}
	cds->pDir=new char[strlen(t)+1];
	strcpy(cds->pDir, t);
	
	// Bind event handlers
	BIND(OnGamePacketBeforeSent);
	BIND(OnGamePacketBeforeReceived);
	BIND(OnGameCommandLine);
	BIND(OnGameJoin);
	BIND(OnGameLeave);
	BIND(OnGameTimerTick);
	BIND(OnClientStart);
	BIND(OnClientStop);
	
	// Bind property gets
	BIND(GetModuleAuthor);
	BIND(GetModuleWebsite);
	BIND(GetModuleEmail);
	BIND(GetModuleDescription);
	BIND(GetModuleVersion);

	EnterCriticalSection(&psi->csData);
	ClientList.AddItem(cds);

	// Initiate client
	if ((cds->OnClientStart))
	{
		// If initialization fails, unload module
		if (!cds->OnClientStart())
		{
			LeaveCriticalSection(&psi->csData);
			char* oargv = argv[0];
			argv[0]=new char[10];
			sprintf(argv[0], "unload");
			GameCommandLineUnload(argv, argc);
			delete argv[0];
			argv[0] = oargv;
			return TRUE;
		}
	}
	if ((cds->OnGameJoin) && (thisgame->player))
		cds->OnGameJoin(thisgame);

	LeaveCriticalSection(&psi->csData);

	sprintf(t, "Module ÿc4'%s'ÿc0%s%s loaded!",
		cds->Name,
		(cds->GetModuleAuthor?" by ":""),
		(cds->GetModuleAuthor?cds->GetModuleAuthor():"")
		);
	fep->GamePrintInfo(t);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineUnload
// -------------------------------------------------------------------
// Unloads a client
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineUnload(char** argv, int argc)
{
	if (argc != 2) return FALSE;
	if (!stricmp(argv[1], "help")) return FALSE;

	if (!ClientList.GetItemCount())
	{
		fep->GamePrintInfo("No modules loaded!");
		return TRUE;
	}

	char t[128];

	EnterCriticalSection(&psi->csData);

	LinkedItem *li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;
	for(int i=0; i<ClientList.GetItemCount(); i++)
	{	
		cds=(CLIENTINFOSTRUCT*)li->lpData;
		if (!stricmp(argv[1], cds->Name))
		{
			// Tell module that we leave game
			if ((cds->OnGameLeave) && (thisgame->player))
					cds->OnGameLeave(thisgame);

			// Prepare module to be unloaded
			if ((cds->OnClientStop))
			{
				// If this fails, let the user know
				if (!cds->OnClientStop())
				{
					fep->GamePrintInfo("Module not unloaded cleanly! This could ba a problem!");
				}
			}

			FreeLibrary((HMODULE)cds->hModule);
			sprintf(t, "Module ÿc4'%s'ÿc0 unloaded!", argv[1]);
			fep->GamePrintInfo(t);
			ClientList.RemoveItem(li);
			LeaveCriticalSection(&psi->csData);
			delete cds->Name;
			delete cds->pDir;
			delete cds;
			return TRUE;
		}
		li=ClientList.GetNextItem(li);
	}
	LeaveCriticalSection(&psi->csData);

	sprintf(t, "Could not unload module '%s'. It's not loaded!",argv[1]);
	fep->GamePrintError(t);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineList
// -------------------------------------------------------------------
// Lists loaded clients
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineList(char** argv, int argc)
{
	//
	// Show usage if we send too many arguments
	if (argc>1) return FALSE;

	//
	// Are any modules loaded ?
	if (!ClientList.GetItemCount())
	{
		fep->GamePrintInfo("No modules loaded!");
		return TRUE;
	}
	//
	// Header
	fep->GamePrintInfo("Loaded modules:");
	//
	// Loop through modules and print them to screen
	LinkedItem *li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;
	for(int i=0; i<ClientList.GetItemCount(); i++)
	{	
		cds=(CLIENTINFOSTRUCT*)li->lpData;
		char t[256];
		sprintf(t, "ÿc4%sÿc0", cds->Name);
		if (cds->GetModuleVersion)
			sprintf(t, "%s version %d.%d",t, LOWORD(cds->GetModuleVersion()), HIWORD(cds->GetModuleVersion()));
		if (cds->GetModuleAuthor)
			sprintf(t, "%s by %s", t, cds->GetModuleAuthor());
		if (cds->GetModuleEmail)
			sprintf(t, "%s (%s)", t, cds->GetModuleEmail());
		if (cds->GetModuleWebsite)
			sprintf(t, "%s ÿc4http://%s", t, cds->GetModuleWebsite());

		fep->GamePrintInfo(t);
		li=ClientList.GetNextItem(li);
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineSet
// -------------------------------------------------------------------
// Set server options
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineSet(char** argv, int argc)
{
	// Too few arguments, show usage
	if (argc<3) return FALSE;
	//
	// Check for 'verbose' options
	if (!stricmp(argv[1], "verbose"))
	{
		if (!stricmp(argv[2], "on"))
		{
			psi->Verbose = TRUE;
			fep->GamePrintInfo("Verbose operation is ÿc4ON");
			SetHackProfileString("D2HackIt", "Misc", "Verbose", "on");
			return TRUE;
		}
		if (!stricmp(argv[2], "off"))
		{
			psi->Verbose = FALSE;
			fep->GamePrintInfo("Verbose operation is ÿc4OFF");
			SetHackProfileString("D2HackIt", "Misc", "Verbose", "off");
			return TRUE;
		}
	}
	//
	// No valid options found, show usage
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineSend
// -------------------------------------------------------------------
// QnD packet sender for testing stuff
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineSend(char** argv, int argc)
{
	//
	// Some sanity checks
	if (argc != 2) return FALSE;
	if (strlen(argv[1])%2) return FALSE;
	if (!stricmp(argv[1], "help")) return FALSE;
	//
	// Make a buffer
	int len=strlen(argv[1])/2;
	PBYTE buf=new BYTE[len];
	//
	// Build the packet
	char *x="";
	for (int i=len-1; strlen(argv[1]) > 0; i--)
	{
		buf[i]=(BYTE)strtoul(&argv[1][i*2], &x, 0x10);
		argv[1][i*2]=0;
	}
	//
	// Send the packet
	GameSendPacketToServer(buf, len);
	//
	// Done
	delete buf;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineReceive
// -------------------------------------------------------------------
// QnD incoming packet sender for testing stuff
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineReceive(char** argv, int argc)
{
	//
	// Some sanity checks
	if (argc != 2) return FALSE;
	if (strlen(argv[1])%2) return FALSE;
	if (!stricmp(argv[1], "help")) return FALSE;
	//
	// Make a buffer
	int len=strlen(argv[1])/2;
	PBYTE buf=new BYTE[len];
	//
	// Build the packet
	char *x="";
	for (int i=len-1; strlen(argv[1]) > 0; i--)
	{
		buf[i]=(BYTE)strtoul(&argv[1][i*2], &x, 0x10);
		argv[1][i*2]=0;
	}
	//
	// Send the packet
	GameSendPacketToGame(buf, len);
	//
	// Done
	delete buf;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineReceive
// -------------------------------------------------------------------
// QnD incoming packet sender for testing stuff
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineSendkey(char** argv, int argc)
{
	// Sanity checks
	if (argc != 2) return FALSE;
	if (strlen(argv[1]) != 2) return FALSE;
	char* x="";
	DWORD Keycode=strtoul(argv[1], &x, 0x10);
	DWORD p = psi->GameKeyDownIntercept;

	fep->GamePrintError(".sendkey is not working yet!");

	/*
	__asm {
		mov esi,0x20
		call p
	}
	*/

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineSay
// -------------------------------------------------------------------
// Makes your character say something in game
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineSay(char** argv, int argc)
{
	// Sanity check
	if (argc<2) return FALSE;
		
	BYTE aPacket[1024];
	char aString[1024];
	aString[0]=0;
	sprintf(aString, "%s", argv[1]);

	for (int i=2; i!=argc; i++)
	{
		sprintf(aString, "%s %s", aString, argv[i]);
	}

	sprintf((char*)aPacket, "%c%c%c%s%c%c%c",
			0x15, 0x01, 0x00, aString, 0,0,0);
	GameSendPacketToServer(aPacket, strlen(aString)+6);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineOverhead
// -------------------------------------------------------------------
// Makes your character say something in game over his head.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineOverhead(char** argv, int argc)
{
	// Sanity check
	if (argc<2) return FALSE;
		
	BYTE aPacket[1024];
	char aString[1024];
	aString[0]=0;
	sprintf(aString, "%s", argv[1]);

	for (int i=2; i!=argc; i++)
	{
		sprintf(aString, "%s %s", aString, argv[i]);
	}

	sprintf((char*)aPacket, "%c%c%c%s%c%c%c",
			0x14, 0x00, 0x00, aString, 0,0,0);
	GameSendPacketToServer(aPacket, strlen(aString)+6);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineRepeat
// -------------------------------------------------------------------
// Repeats <command>, <count> number of times
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineRepeat(char** argv, int argc)
{
	if (argc < 3)
		return FALSE;

	int count=atoi(argv[1]);
	
	// Build commandline
	char t[256];
	t[0]=0;
	for(int i=2;i!=argc;i++)
		sprintf(t, "%s %s", t, argv[i]);
	
	fep->GamePrintInfo(t);

	// Send command
	for (; count; count--)
		GameCommandLine(t+1);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GameCommandLineCrash
// -------------------------------------------------------------------
// Crashes game, not recommended for normal use :)
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineCrash(char** argv, int argc)
{
	// Jump to nowhere should do it :)
	__asm
	{
		push 0
		ret
	}
	return TRUE;
}

	

//////////////////////////////////////////////////////////////////////
// GameCommandLineTest
// -------------------------------------------------------------------
// Test stuff goes here
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineTest(char** argv, int argc)
{
	fep->GamePrintInfo("ÿc1Test does nothing!");
	return TRUE;
}

	
//////////////////////////////////////////////////////////////////////
// GameCommandLineClick
// -------------------------------------------------------------------
// Simulates a mouse click at the coordinates you specify.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineClick(char** argv, int argc)
{
	// Sanity check.
	if (argc < 3)
		return FALSE;

	// Convert x and y coordinates to numbers
	int x=atoi(argv[1]);
	int y=atoi(argv[2]);
	
	// Did we specify a window?
	if( argc >= 4 )
	{
		click(x,y,argv[3]);
	}
	else
	{
		click(x,y,NULL);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// GameCommandLineDisconnectChat
// -------------------------------------------------------------------
// Disconnects from chat server
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE GameCommandLineDisconnectChat(char** argv, int argc)
{
	// Sanity check.
	if (argc != 1)
		return FALSE;

	for(int i=0; i<5; ++i) {
		fep->GameSendMessageToChat("/whoami");
	}

	return TRUE;
}
