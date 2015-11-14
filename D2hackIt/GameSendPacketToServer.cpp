//////////////////////////////////////////////////////////////////////
// GameSendPacketToServer.cpp
// -------------------------------------------------------------------
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"


//////////////////////////////////////////////////////////////////////
// GameSendPacketToServer
// -------------------------------------------------------------------
// Helper function to send a packet of data to the server.
//
// 0.21: This now works perfectly ok even in single player as we're 
//       no longer sending to a socket, but to the game itself.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GameSendPacketToServer(LPBYTE buf, DWORD len)
{
	DWORD p = psi->fps.GamePacketSentIntercept.AddressFound;
	__asm {
		push len
		push buf
		push 0
		call p
	}
	return len;
}

//////////////////////////////////////////////////////////////////////
// GameSendMessageToChat
// -------------------------------------------------------------------
// Sends text message to chat channel (temporary until raw packet
// sender implemented)
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GameSendMessageToChat(LPSTR msg)
{
	DWORD p = psi->GameSendMessageToChatLocation;
	DWORD s = (DWORD)&msg;
	if(p) {
		_asm {
			mov  ecx, s
			call p
		}
	}
	return !!p;
}
