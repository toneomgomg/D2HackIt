//////////////////////////////////////////////////////////////////////
// GameSendPacketToGame.cpp
// -------------------------------------------------------------------
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"


//////////////////////////////////////////////////////////////////////
// GameSendPacketToGame
// -------------------------------------------------------------------
// Fakes a packet sent from the server to the game.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GameSendPacketToGame(LPBYTE buf, DWORD len)
{
	DWORD SendPacket = psi->GameSendPacketToGameLocation;
	
_asm {
	call FakeEntrypoint
	jmp  EndOfFunction

FakeEntrypoint:
	// This code is an approximate copy of the code
	// at 0x6fc015e0 (1.09). Thanks to Techwarrior 
	// for the info. We use this to stub the 
	// function and pass our own packet to the game.
	sub esp, 0x10c
	push ebx
	mov ebx, buf
	push ebp
	push esi
	mov esi, len

	// Jump to original code
	mov eax, dword ptr [SendPacket]
	push eax
	ret					
EndOfFunction:
	}
	return len;
}

//////////////////////////////////////////////////////////////////////
// GameInsertPacketToGame
// -------------------------------------------------------------------
// Inserts a packet to beginning of queue.
//////////////////////////////////////////////////////////////////////
BOOL EXPORT GameInsertPacketToGame(LPBYTE buf, DWORD len)
{
	EnterCriticalSection(&psi->csData);

	if(psi->nRecvBufferPos >= MAXINSERTRECVBUFFERS || len > MAXRECVBUFFERLEN) {
		// Can't add
		LeaveCriticalSection(&psi->csData);
		return FALSE;
	}

	memcpy(psi->InsertRecvBuffer[psi->nRecvBufferPos], buf, len);
	psi->InsertRecvLen[psi->nRecvBufferPos] = len;
	++psi->nRecvBufferPos;

	LeaveCriticalSection(&psi->csData);
	return TRUE;
}
