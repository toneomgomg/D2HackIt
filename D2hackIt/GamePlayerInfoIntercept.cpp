//////////////////////////////////////////////////////////////////////
// GamePlayerInfoIntercept.cpp
// -------------------------------------------------------------------
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\D2HackIt.h"

//////////////////////////////////////////////////////////////////////
// GamePlayerInfoIntercept
// -------------------------------------------------------------------
// Our clean function. Updates the *thisgame struct and tells the
// loaded modules that we just entered/left a game
//////////////////////////////////////////////////////////////////////
VOID __fastcall GamePlayerInfoIntercept(PLAYERINFOSTRUCT* pis)
{
	LinkedItem *li=ClientList.GetFirstItem();
	CLIENTINFOSTRUCT* cds;

	// We're joining a game
	if ((pis) && (!thisgame->player))
	{
		SHOW_REGS("GamePlayerInfoIntercept");
		thisgame->player=pis;
		for(int i=0; i<ClientList.GetItemCount(); i++)
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			if (cds->OnGameJoin)
			{
				cds->OnGameJoin(thisgame);
			}
			li=ClientList.GetNextItem(li);
		}
	}

	// We're leaving a game
	if ((!pis) && (thisgame->player))
	{
		thisgame->player=pis;
		for(int i=0; i<ClientList.GetItemCount(); i++)
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			if (cds->OnGameLeave)
			{
				cds->OnGameLeave(thisgame);
			}
			li=ClientList.GetNextItem(li);
		}
	}
}


//////////////////////////////////////////////////////////////////////
// GamePlayerInfoInterceptSTUB
// -------------------------------------------------------------------
// esi = pointer to PLAYERINFOSTRUCT or NULL
//////////////////////////////////////////////////////////////////////
VOID __declspec(naked) GamePlayerInfoInterceptSTUB()
{
	__asm {
		nop
		nop
		nop
		nop
		nop
		nop
		pushad
		mov ecx, esi
		call GamePlayerInfoIntercept
		popad
		ret
	}
}

