//////////////////////////////////////////////////////////////////////
// GamePacketReceived.cpp
// -------------------------------------------------------------------
// All hooks have a 'stub' and a 'clean' c version. I did this to be
// able to update the stub when new patches come out without having to
// change anything in the clean code. Once in the clean code, we don't
// need to worry about registers/stack etc.etc.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"


//////////////////////////////////////////////////////////////////////
// GamePacketReceived()
// -------------------------------------------------------------------
// Our 'clean' GamePacketReceived()
//////////////////////////////////////////////////////////////////////
DWORD __fastcall GamePacketReceivedIntercept(BYTE* aPacket, DWORD aLength)
{
	// Pass packet to all clients who wants to snoop
	LinkedItem *li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;
	for(int i=0; i<ClientList.GetItemCount(); i++)
	{	
		cds=(CLIENTINFOSTRUCT*)li->lpData;
		if (cds->OnGamePacketBeforeReceived)
		{
			aLength=cds->OnGamePacketBeforeReceived(aPacket, aLength);
			if(!aLength)
				break;
		}
		li=ClientList.GetNextItem(li);
	}
	return aLength;
}

//////////////////////////////////////////////////////////////////////
// GamePacketReceivedSTUB()
// -------------------------------------------------------------------
// ebx = packet number
// ebp = pointer to packet		(Needed)
// edi = length
// eax = length
// ecx = length					(Needed)
// edx = ebx * 3				(Needed)
//////////////////////////////////////////////////////////////////////
void __declspec(naked) GamePacketReceivedInterceptSTUB()
{
	static DWORD len;
	__asm {
		nop									// Make room for original code
		nop
		nop
		nop
		nop
		nop
		nop

		pushad

		// Call our clean c function
		mov edx, ecx
		mov ecx, ebp
		call GamePacketReceivedIntercept
		mov [len], eax

		// Return 0?, if so send dummy packet.
		test eax, eax
		jz  dummy

		// Return to game
		popad

		mov	bl, [ebp]
		lea edx, [ebx+2*ebx]
		mov eax, [len]
		mov ecx, eax

		ret
		
dummy:	// Send dummy packet (0x77, 0x06);
		popad
		mov word ptr [ebp], 0x0677		// 7706 packet
		mov bl, 0x77
		mov edx, 0x165					// 0x77 * 3 = 0165
		mov eax, 2						// 2 bytes long
		mov ecx, eax
		ret
	}
}

//////////////////////////////////////////////////////////////////////
// GameInsertReceived()
// -------------------------------------------------------------------
// Our 'clean' GameInsertReceived()
//////////////////////////////////////////////////////////////////////
static DWORD RecvBuf;
static DWORD RecvLen;

BOOL __fastcall GamePacketReceivedIntercept2()
{
	if(!psi->nRecvBufferPos)
		return FALSE;

	EnterCriticalSection(&psi->csData);
	// recheck in case it's been updated
	if(!psi->nRecvBufferPos) {
		LeaveCriticalSection(&psi->csData);
		return FALSE;
	}

	// remove from LIFO
	--psi->nRecvBufferPos;
	RecvBuf = (DWORD)&psi->InsertRecvBuffer[psi->nRecvBufferPos];
	RecvLen = psi->InsertRecvLen[psi->nRecvBufferPos];

	LeaveCriticalSection(&psi->csData);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// GamePacketReceivedSTUB()
// -------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
void __declspec(naked) GamePacketReceivedIntercept2STUB()
{
	__asm {
		nop									// Make room for original code
		nop
		nop
		nop
		nop
		nop
		nop

		pushad

		// Call our clean c function
		call GamePacketReceivedIntercept2
		test	eax,eax
		popad
		jz		done

		mov		ecx, [RecvBuf]
		mov		edx, [RecvLen]

		// Get function starting address
		mov		eax, [esp]
		add		eax, [eax - 4]
		jmp		eax

done:
		ret
	}
}

