//////////////////////////////////////////////////////////////////////
// mercpump.cpp
// -------------------------------------------------------------------
// This module uses a clientside glitch to make your merc appear to
// hame much better stats than it really has. This is *ONLY* client-
// side!!
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
enum { IDLE, STEP1 };
int STAGE=IDLE;
BOOL PRIVATE OnGameCommandSet(char** argv, int argc);

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
	"Merc pump",					// Short module description
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
			"set",
			OnGameCommandSet,
			"setÿc0 Pumps your merch using the next merc item you click on"
	},

	// Add your own commands here

	{NULL}	// No more commands
};


//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeSent
// -------------------------------------------------------------------
// Executes before packets are sent from the game to the server.
// 
// If you rewrite the packet, you must make sure to return the length
// of the new packet!
//
// NOTE: This event also works in single player games.
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen)
{
	// 0x61 - Merc action
	// Get item location from merc
	if ((aPacket[0] == 0x61) && (STAGE == STEP1))
	{
		STAGE=IDLE;

		// Build the packet to use
		BYTE p[]={0x61,0,0 };
		p[1]=aPacket[1];

		server->GamePrintInfo("Pumping merc, this will lock up the game for take a few seconds and your game will ÿc9lock upÿc0 while pumping...");

		// Pump the stats 100 times
		for (int x=0; x!=201; x++)
			server->GameSendPacketToServer(p,3);		// Pick up/drop the item

	}

	return aLen;
}

//////////////////////////////////////////////////////////////////////
// OnGameCommandSet
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
BOOL PRIVATE OnGameCommandSet(char** argv, int argc)
{
	if (argc != 2) return FALSE;

	server->GamePrintInfo("Click on the item on your merc that you want to pump the stats with...");
	
	STAGE=STEP1;

	return TRUE;
}