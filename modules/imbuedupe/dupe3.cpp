//////////////////////////////////////////////////////////////////////
// dupe3.cpp
// -------------------------------------------------------------------
// Lag-aware dupe method using imbue quest. This will spit out between
// 1 to 3 rings per second. Click on akara to get her NPC id, then 
// stand in rogue encapment and let other char initiate trade with 
// you.
//
// The module will drop 88 rings (full stash + inventory) and then 
// cancel the trade. Used with dclient on other computer will make 
// this fully automatic. Initiate trade from the computer with dclient
// and go watch tv :)
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\ClientCore.cpp"	  // Include the backend of the module


char npc[]={0,0,0,0};
bool GOTNPC=false;

// This click function by Trojan is so simple I see no reason to modify it :)
void click( int x, int y) {  
  HWND handle=FindWindow("Diablo II","Diablo II");  
  int pos=((short)y<< 16)+(short)x;  
  SendMessage(handle,WM_MOUSEMOVE,NULL,pos);  
  SendMessage(handle,WM_LBUTTONDOWN,MK_LBUTTON,pos);  
  SendMessage(handle,WM_LBUTTONUP,MK_LBUTTON,pos);  
}

//////////////////////////////////////////////////////////////////////
// CLIENTINFO
//////////////////////////////////////////////////////////////////////
CLIENTINFO
(		
	1,0,							// Module version (Major, Minor)
	thohell,						// Author
	thohell.d2network.com,			// url (http:// is appended)
	Dupe3,							// Short module description
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
	// Local variables
	static BOOL IN_TRADE=false;
	static char lastID[]={0,0,0,0};
	static int ringsleft=88;
	static int scol=0;
	static int srow=0;
	static int icol=0;
	static int irow=0;

	// Set up oneside trade and open stash.
	if (aPacket[0]==0x77)
	{
		BYTE p[]={0x4f,0x03,0,0,0,0,0};
		BYTE p2[]={0x77, 0x10};
		DWORD dummy=0;
		switch (aPacket[1])
		{
		case 0x01:					// 77 01 - Trade ok/cancel
			// Got NPC id of akara ?
			if (!GOTNPC)
			{
				server->GamePrintInfo("ÿc1Unable to dupe before talking to akara!");
				p[1]=0x02;
				server->GameSendPacketToServer(p,7);
				return 0;
			}

			// Yes, go on and open oneside trade and open stash
			server->GameSendPacketToServer(p,7);
			server->GameSendPacketToGame(p2,2);
			server->GamePrintInfo("ÿc1Ready to dupe! Start dropping items!");

			// Restore defaults
			ringsleft=88;
			scol=0;
			srow=0;
			icol=0;
			irow=0;
			return 0;
			break;

			
		case 0x06:				// 77 06 - Ready to trade
			IN_TRADE=true;
			click(170,160);		// Click first ring in stash to initiate drop
			break;

		case 0x0c:				// 77 0c - Trade closed
			IN_TRADE=false;
			return 0;
		}
	}

	
	// This is the magic: 
	// When we have the ring on cursor, try to imbue it.
	// It will fail and drop to the ground.
	if ((aPacket[0]==0x9d) && (IN_TRADE))
	{
		// Make imbue packet
		BYTE p[]={0x38,0,0,0,0,0,0,0,0,0,0,0,0};
		memcpy(p+9,aPacket+4,4);					// Item ID
		memcpy(p+5, npc, 4);						// NPC ID
		memcpy(lastID, aPacket+4,4);				// Save ITEM ID
		server->GameSendPacketToServer(p, 13);
		return 0;
	}

	// When a ring drops, pick up the next one
	if ((aPacket[0]==0x9c) && (IN_TRADE))
	{
		// Filter out junk
		if (memcmp(lastID,aPacket+4,4))
			return aLen;
		
		// Count rings left & stop when 88 is done
		if (!--ringsleft)
		{
			// Restore defaults
			ringsleft=88;
			scol=0;
			srow=0;
			icol=0;
			irow=0;
			// Cancel trade
			BYTE p[]={0x4f,2,0,0,0,0,0};
			server->GameSendPacketToServer(p,7);
			return 0;
		}

		// Do the clicking of 87 more rings :)
		if (ringsleft > 40)
		{
			// Drop from stash
			if (++scol > 5)
				{ scol=0; srow++; }
			Sleep(400);
			click(170+29*scol, 160+29*srow);
		} else {
			// Drop from inventory
			Sleep(400);
			click(420+29*icol, 325+29*irow);
			if (++icol > 9)
				{ icol=0; irow++; }
		}
		return 0;		// Hide the ring to avoid lag
	}
	return aLen;
}


//////////////////////////////////////////////////////////////////////
// OnGamePacketBeforeSend
//////////////////////////////////////////////////////////////////////
DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen)
{
	// We need to get the NPC of akara by clicking on her
	if (aPacket[0] == 0x2f)
	{
		GOTNPC=true;
		server->GamePrintInfo("ÿc1Got NPC ID, ready to dupe");
		memcpy(npc,aPacket+5,4);
		return 0;
	}
	return aLen;
}

