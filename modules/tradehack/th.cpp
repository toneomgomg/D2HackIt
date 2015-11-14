//////////////////////////////////////////////////////////////////////
// th.cpp
//////////////////////////////////////////////////////////////////////
#include "..\ClientCore.cpp"	  // Include the backend of the module

//////////////////////////////////////////////////////////////////////
// Global stuff.
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandSet(char** argv, int argc);
BOOL PRIVATE OnGameCommandUnSet(char** argv, int argc);

// This is our trigger
bool SET=false;

// Comment this to hide warning message
#define CRIPPLED


//////////////////////////////////////////////////////////////////////
// CLIENTINFO
//////////////////////////////////////////////////////////////////////
CLIENTINFO
(		
	1,0,							// Module version (Major, Minor)
	"thohell",						// Author
	"thohell.d2network.com",		// url (http:// is appended)
	"Trade Hack",					// Short module description
	"thohell@home.se"				// Author eMail
)

//////////////////////////////////////////////////////////////////////
// MODULECOMMANDSTRUCT ModuleCommands[]
//////////////////////////////////////////////////////////////////////
MODULECOMMANDSTRUCT ModuleCommands[]=
{
	{	
		"help",
		OnGameCommandHelp,
		"helpÿc0 List commands available in this module.\n"
		"<command> helpÿc0 Shows detailed help for <command> in this module."
	},
	{
		// Set
		"set",
		OnGameCommandSet,
		"setÿc0 Prepares item for trade hack."
	},
	{
		// Unset
		"unset",
		OnGameCommandUnSet,
		"unsetÿc0"
	},
	{NULL}	// No more commands
};

//////////////////////////////////////////////////////////////////////
// OnClientStart
//////////////////////////////////////////////////////////////////////
BOOL EXPORT OnClientStart()
{
	server->GamePrintInfo("To use tradehack, type ÿc4.th setÿc0 and click on an item in the trade window.");
	server->GamePrintInfo("This will prepare the item for tradehack. Then, open a tradewindow with the same");
	server->GamePrintInfo("person to tradehack him/her.");
	server->GamePrintInfo("Use ÿc4.th unsetÿc0 if you wish to cancel item preparation");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeSent
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen)
{
	if ((aPacket[0]==0x19) && (SET))
	{
		SET=false;

		BYTE drop[17]={0x18,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0};
		BYTE close[7]={0x4F,3,0,0,0,0,0};

		// Paste item ID
		memcpy(drop+1,aPacket+1,4);

		server->GameSendPacketToServer(aPacket, aLen);	// Pick up item
		server->GameSendPacketToServer(drop, 17);		// Drop item
		server->GameSendPacketToServer(close, 7);		// Close tradescreen

		server->GamePrintInfo("Tradehack prepared, item in buffer!");
		server->GamePrintInfo("Now trade with the same person and he will se a fake item!");

		return 0;
	}
	return aLen;
}

//////////////////////////////////////////////////////////////////////
// OnGamePacketSet
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandSet(char** argv, int argc)
{
	if (argc!=2) 
		return FALSE;
	server->GamePrintInfo("Please lift up the item you wish to put in trade-buffer");
	SET=true;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// OnGamePacketUnSet
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandUnSet(char** argv, int argc)
{
	if (argc>2) 
		return FALSE;
	server->GamePrintInfo("Tradehack preparation cancelled.");
	SET=false;
	return TRUE;
}

