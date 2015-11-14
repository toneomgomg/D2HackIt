//////////////////////////////////////////////////////////////////////
// dclient.cpp
// -------------------------------------------------------------------
// Used to initiate new trades automatically when cancelled. 
// Just click on a char to start, .unload dclient to stop.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\ClientCore.cpp"	  // Include the backend of the module


BYTE p[]={0,0,0,0,0,0,0,0,0};
BOOL GOTPACKET=false;

//////////////////////////////////////////////////////////////////////
// CLIENTINFO
//////////////////////////////////////////////////////////////////////
CLIENTINFO
(		
	1,0,							// Module version (Major, Minor)
	thohell,						// Author
	thohell.d2network.com,			// url (http:// is appended)
	dclient,						// Short module description
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
	{NULL}	// No more commands
};

//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeReceived
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeReceived(BYTE* aPacket, DWORD aLen)
{
	// When tradescreen closes, wait a while and reopen it
	if ((aPacket[0]==0x77) && (aPacket[1]==0x0c) && GOTPACKET)
	{
		Sleep(6000);
		server->GameSendPacketToServer(p, 9);
	}
	return aLen;
}


//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeSend
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen)
{
	// Get packet to initiate trade with char
	if (aPacket[0]==0x13)
	{
		memcpy(p, aPacket, 9);
		GOTPACKET=true;
	}
	return aLen;
}
