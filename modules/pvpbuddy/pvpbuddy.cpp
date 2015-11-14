//////////////////////////////////////////////////////////////////////
// pvpbuddy.cpp
// -------------------------------------------------------------------
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\ClientCore.cpp"	  // Include the backend of the module


//////////////////////////////////////////////////////////////////////
// Global stuff.
// -------------------------------------------------------------------
// This is a very convenient place to your function declarations and
// global variables.
//////////////////////////////////////////////////////////////////////
DWORD GetBitField(BYTE* data, DWORD pos, DWORD len)
{
 return (DWORD)(*(unsigned __int64 *)(data+pos/8)<<(64-len-(pos&7))>>(64-len));
}
struct BitFields
{
 BYTE *data;
 DWORD pos;
 BitFields(BYTE *d) {data = d; pos = 0;}
 DWORD GetField(DWORD len) {return GetBitField(data, (pos+=len)-len, len);}
};
DWORD bits(BYTE *data, DWORD pos, DWORD len)
{
	DWORD aValue=0;
	for (int i=0;i<len;i++)
		aValue=(aValue<<1)+(((data[((pos+i)/8)])&(1<<((pos+i)&7)))==0?0:1);
	return aValue;
}


DWORD showEarLevel;
DWORD showGoldAmount;
BOOL PRIVATE OnGameCommandEars(char** argv, int argc);
BOOL PRIVATE OnGameCommandGold(char** argv, int argc);
BOOL PRIVATE OnGameCommandSettings(char** argv, int argc);
//////////////////////////////////////////////////////////////////////
// CLIENTINFO
// -------------------------------------------------------------------
// Tell us a little about this module.
//////////////////////////////////////////////////////////////////////
CLIENTINFO
(		
	1,0,							// Module version (Major, Minor)
	"thohell",						// Author
	"thohell.d2network.com",		// url (http:// is appended)
	"PvPBuddy module",				// Short module description
	"thohell@home.se"				// Author eMail
)

//////////////////////////////////////////////////////////////////////
// MODULECOMMANDSTRUCT ModuleCommands[]
// -------------------------------------------------------------------
// To add your own commands, just add them to this list like:
//
//	{
//		"name"			// Name of the command
//		pFunction		// The function that handles command
//		"usage"			// Help text for the command
//	}
// 
// ...where pFunction is the name of the function that handles the
// command. The function must be declared as...
//
// BOOL PRIVATE OnGameCommandName(char** argv, int argc)
//
// ...and return TRUE on success or FALSE to show usage of the command.
//
// The "usage" line can contain "\n" as linebreaks and should be in 
// the following format for consistency:
//
// "commandname <required parameter> [optional parameter]ÿc0 Description"
//
//
// To use your commands in the game, just type 
// .modulename <command> [arguments]
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
		"ears",
		OnGameCommandEars,
		"ears <level>ÿc0 Hides ears from character below <level>."
	},
	{
		"gold",
		OnGameCommandGold,
		"gold <amount>ÿc0 Hides gold below <amount>."
	},
	{
		"settings",
		OnGameCommandSettings,
		"settingsÿc0 Show current PvPBuddy settings."
	},

	// Add your own commands here

	{NULL}	// No more commands
};

//////////////////////////////////////////////////////////////////////
// OnClientStart
// -------------------------------------------------------------------
// Runs *once* when the module loads. 
// This function cannot be removed.
//
// Put any initialization code here.
//
// Return FALSE to prevent module from loading.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnClientStart()
{
	char *retval;

	retval=server->GetHackProfileString("PvPBuddy", "Default", "ShowEarLevel");
	showEarLevel=atoi(retval);
	delete retval;

	retval=server->GetHackProfileString("PvPBuddy", "Default", "ShowGoldAmount");
	showGoldAmount=atoi(retval);
	delete retval;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// OnClientStop
// -------------------------------------------------------------------
// Runs *once* before client is unloaded. 
// This function cannot be removed.
// 
// Put any cleanup code here.
//
// Return value should be TRUE.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnClientStop()
{
	return TRUE;
}




//////////////////////////////////////////////////////////////////////
// EVENTS
// -------------------------------------------------------------------
// The rest of this file are event functions, and they can safely be
// removed if the module makes no use of them.
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeReceived
// -------------------------------------------------------------------
// Executes before packets are received to the game from the server.
// 
// Returning 0 hides the packet from the game. Rewriting the packet
// changes what the game sees.
//
// NOTE: This event also works in single player games.
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeReceived(BYTE* aPacket, DWORD aLen)
{
	// Only react to items on ground
	if ((aPacket[0]==0x9c) &&
		((aPacket[1]==0x00) ||
		 (aPacket[1]==0x02) ||
		 (aPacket[1]==0x03)))
	{

		BitFields item(aPacket);
		
		// Remove junk
		item.GetField(80);
		
		// Get "ear" flag
		DWORD isEar=item.GetField(1);
		if (isEar)
		{
			item.GetField(63);
			DWORD earLevel = item.GetField(7);
			// Show or hide ear ?
			return (earLevel < showEarLevel) ? 0 : aLen;
		}

		
		// Get item code
		item.GetField(60);
		DWORD ItemCode=item.GetField(32);	// Get item code

		// Gold ?
		if (ItemCode == 0x20646C67) 
		{

			DWORD goldsize = item.GetField(1);
			DWORD goldAmount = item.GetField(goldsize?32:12);
			// Show or hide gold?
			return (goldAmount < showGoldAmount) ? 0 : aLen;
		}
	}
	return aLen;
}

//////////////////////////////////////////////////////////////////////
// OnGameCommandEars
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandEars(char** argv, int argc)
{
	if (argc!=3) return FALSE;
	showEarLevel = atoi(argv[2]);
	if (showEarLevel>100)
		showEarLevel=100;

	char t[128];
	sprintf(t, "%d", showEarLevel);
	server->SetHackProfileString("PvPBuddy", "Default", "ShowEarLevel", t);

	sprintf(t, "Hide ears below level ÿc4%dÿc0.", showEarLevel);
	server->GamePrintInfo(t);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// OnGameCommandGold
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandGold(char** argv, int argc)
{
	if (argc!=3) return FALSE;
	showGoldAmount = atoi(argv[2]);

	char t[32];
	sprintf(t, "%d", showGoldAmount);
	server->SetHackProfileString("PvPBuddy", "Default", "ShowGoldAmount", t);

	sprintf(t, "Hide gold below ÿc4%dÿc0.", showGoldAmount);
	server->GamePrintInfo(t);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// OnGameCommandSettings
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandSettings(char** argv, int argc)
{
	if (argc!=2) return FALSE;
	char t[128];
	sprintf(t, "PvPBuddy Settings: Hide ears below level ÿc4%dÿc0. Hide gold below ÿc4%dÿc0.",
		showEarLevel, showGoldAmount);
	server->GamePrintInfo(t);
	return TRUE;
}
