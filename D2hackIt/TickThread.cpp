//////////////////////////////////////////////////////////////////////
// TickThread.cpp
// -------------------------------------------------------------------
// Tick that calls modules approximatly every 1/10th of a second. 
// Great for implementing timers in modules.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#include "..\D2HackIt.h"


DWORD WINAPI TickThread(LPVOID lpParameter)
{
	LinkedList *cl=(LinkedList*)lpParameter;

	for (; !psi->TickShutDown; )
	{
		EnterCriticalSection(&psi->csData);

		LinkedItem *li=cl->GetFirstItem();
		for(int i=0; i < cl->GetItemCount(); i++)
		{
			CLIENTINFOSTRUCT *cds=(CLIENTINFOSTRUCT*)li->lpData;
			if (cds->OnGameTimerTick)
				cds->OnGameTimerTick();
			li = ClientList.GetNextItem(li);
		}

		LeaveCriticalSection(&psi->csData);
		Sleep(100);
	}

	++psi->TickShutDown;
	// Dummy!
	return 0;
}

