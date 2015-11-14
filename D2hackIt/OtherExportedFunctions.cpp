//////////////////////////////////////////////////////////////////////
// OtherExportedFunctions.cpp
// -------------------------------------------------------------------
// Misc. stuff that doesn't really go anywhere else.
//
// <thohell@home.se>
//////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

PTHISGAMESTRUCT EXPORT GetThisgameStruct(void)
{
	return thisgame;
}

PSERVERINFO EXPORT GetServerInfo(void)
{
	return si;
}

//Doesn't work =/
/*
////////////////////////////////////////////////////////////////////
// GetInfoStruct()
// ----------------------------------------------------------------
// dwGetStruct =
//		GS_ClientList - Returns a pointer to the modules ClientInfoStruct
//						Uses szClientName to find the module
//
//		GS_D2Param - Returns a pointer to a D2PARAM struct
//						szClient is ignored
///////////////////////////////////////////////////////////////////
LPVOID EXPORT GetInfoStruct( DWORD dwGetStruct, const char* szClientName )
{
	LinkedItem*	li;
	CLIENTINFOSTRUCT* cds;
	int i;
	char	szTemp[MAX_PATH];

	fep->GamePrintInfo( "GetInfoStruct Called" );

	switch( dwGetStruct )
	{
	case 1:
		sprintf( szTemp, "Getting ClientInfo for: %s", szClientName );
		fep->GamePrintInfo( szTemp );

		li=ClientList.GetFirstItem();
		for( i=0; i<ClientList.GetItemCount(); i++ )
		{	
			cds=(CLIENTINFOSTRUCT*)li->lpData;
			if( !stricmp(cds->Name, szClientName) )
			{
				fep->GamePrintInfo( "Match found returning ClientInfo pointer" );
				return cds;
				break;
			}
			li=ClientList.GetNextItem(li);
		}
		break;

	case 2:
		fep->GamePrintInfo( "Returning D2Param" );
		return g_pD2Param;
		break;

	default:
		fep->GamePrintInfo( "Switch NULL" );
		return NULL;
	}
	fep->GamePrintInfo( "Exit Switch NULL" );
	return NULL;
}

*/