/////////////////////////////////////////////////////////////////////////////
// D2HackItHelperFunctions.cpp
// --------------------------------------------------------------------------
//
// Helper functions for D2HackIt.dll
// 
// <thohell@home.se>
/////////////////////////////////////////////////////////////////////////////
#define THIS_IS_SERVER
#include "..\D2HackIt.h"

/////////////////////////////////////////////////////////////////////////////
// overloaded memcpy()
// --------------------------------------------------------------------------
// Takes care of giving the regions of memory the righ access rights needed
// when copying blocks of data.
/////////////////////////////////////////////////////////////////////////////
VOID* PRIVATE d2memcpy(DWORD lpDest, DWORD lpSource, int len)
{
	DWORD oldSourceProt,oldDestProt=0;
	 VirtualProtect((void*)lpSource,len,PAGE_EXECUTE_READWRITE,&oldSourceProt);
 	  VirtualProtect((void*)lpDest,len,PAGE_EXECUTE_READWRITE,&oldDestProt);
	   memcpy((void*)lpDest,(void*)lpSource,len);
	  VirtualProtect((void*)lpDest,len,oldDestProt,&oldDestProt);
	 VirtualProtect((void*)lpSource,len,oldSourceProt,&oldSourceProt);
	return (void*)lpDest;
};

/////////////////////////////////////////////////////////////////////////////
// Intercept()
// --------------------------------------------------------------------------
// Intercepts a code location to route it somewhere else.
// Also inserts original code at the destination.
/////////////////////////////////////////////////////////////////////////////
BOOL EXPORT Intercept(int instruction, DWORD lpSource, DWORD lpDest, int len)
{
	char t[1024];
	sprintf(t,"Code at %.8x intercepted and routed to %.8x",lpSource,lpDest);
	fep->GamePrintVerbose(t);

	BYTE* buffer = new BYTE[len];
	buffer[0] = instruction;
	*(DWORD*) (buffer + 1) = lpDest - (lpSource + 5);
	memset(buffer + 5, 0x90, len - 5);		// nops
	d2memcpy(lpDest, lpSource, len);		// Patch in old code to new location
	d2memcpy(lpSource, (DWORD) buffer, len);// Route old location to new
	delete buffer;
	return TRUE;
}

// Used by FindFile()
typedef struct _finddir_t
{
	LinkedItem*	pParent;
	char*		pName;
} finddir, *LPfinddir;


//
// Deletes the DirList from FindFile
void DeleteDirList( LinkedList* pDirList )
{
	LinkedItem* pItem;

#ifdef SHOWFINDMODULEMESSAGEBOX

	MessageBox( NULL, "Deleting Dir List", "Debug", MB_OK );
#endif

	while ( (pItem = pDirList->GetFirstItem()) != NULL )
	{
		delete ( ((LPfinddir)pItem->lpData)->pName );
		delete ( pItem->lpData );
		pDirList->RemoveItem(pItem);
	}

}

/////////////////////////////////////////////////////////////////////////////
// D2Findfile()
// --------------------------------------------------------------------------
// Searchs the nested directories where D2Hackir is at for the szFileName
// szDest is the first path matching szFileName
// Return Value is True if Successful or False if not
// If the file isn't found szDest is the Error Message
// -D'Flame 08-20-2002
/////////////////////////////////////////////////////////////////////////////
bool EXPORT D2FindFile( char* szDest, const char* szFileName )
{
	_finddata_t			fdPlugin;
	LPfinddir			pDir;
	LinkedList			LDirList;
	long				hFile;
	char				szTemp[MAX_PATH];
	char				szTemp2[MAX_PATH];
	bool				bFoundDir = false;

	sprintf( szTemp, "%s\\%s", si->PluginDirectory, szFileName );

	if( -1 == (hFile = _findfirst(szTemp, &fdPlugin)) )
	{		// File not in D2HackIt Directory
		_findclose( hFile );

		pDir = new finddir;
		pDir->pName = new char[ strlen(si->PluginDirectory) ];
		pDir->pParent = NULL;

		sprintf( pDir->pName, si->PluginDirectory );
		LDirList.AddItem( pDir );

		sprintf(szTemp, "%s\\*.*", si->PluginDirectory);
		if( -1 == (hFile = _findfirst(szTemp, &fdPlugin)) )	// Start gathering Dirs
		{
			_findclose( hFile );
			sprintf( szDest, "The search dir '%s' seems to be empty", szTemp );
			DeleteDirList( &LDirList );
			return false; // <---!!! Delete the link list
		} // if

		if( fdPlugin.attrib & _A_SUBDIR )
		{
			if( stricmp(fdPlugin.name, ".") && stricmp(fdPlugin.name, "..") )
			{
				pDir = new finddir;
				pDir->pName = new char[ strlen(fdPlugin.name)+2 ];
				pDir->pParent = LDirList.GetFirstItem();

				sprintf( pDir->pName, fdPlugin.name );
				LDirList.AddItem( pDir );
				bFoundDir = true;
			} // if
		} // if

		while( 0 == _findnext(hFile, &fdPlugin) )
		{
			if( fdPlugin.attrib & _A_SUBDIR )
			{
				if( stricmp(fdPlugin.name, ".") && stricmp(fdPlugin.name, "..") )
				{
					pDir = new finddir;
					pDir->pName = new char[ strlen(fdPlugin.name)+2 ];
					pDir->pParent = LDirList.GetFirstItem();

					sprintf( pDir->pName, fdPlugin.name );
					LDirList.AddItem( pDir );
					bFoundDir = true;
				} // if
			} // if
		} // while

		_findclose( hFile );

		if( !bFoundDir )
		{
			sprintf( szDest, "'%s' did not contain the File nor any subdirectories.",
				si->PluginDirectory );
			DeleteDirList( &LDirList );
			return false; // <---!!! Delete the linklist
		} // if

		LinkedItem*			pSearchBase;
		LinkedItem*			pSearchPos;
		LinkedItem*			pTempItem;
		bool				bDoneSearching = false;
		bool				bRecurse = false;

		pSearchBase = LDirList.GetFirstItem();
		pSearchPos = pSearchBase->pNext;
		bFoundDir = false;

		//sprintf( szTemp2, "Find File Before Loop Start - szTemp = %s", ((LPfinddir)pSearchPos->lpData)->pName );
		//if( MessageBox( NULL, szTemp2, "Debug", MB_OKCANCEL ) == IDCANCEL )
		//{
		//	DeleteDirList( &LDirList );
		//	return true;
		//}

		while( !bDoneSearching ) // <----!!!
		{
			bRecurse = false;

			sprintf( szTemp, szFileName );
			for( pTempItem = pSearchPos; pTempItem != NULL; pTempItem = ((LPfinddir)pTempItem->lpData)->pParent )
			{
				sprintf( szTemp2, "%s\\%s", ((LPfinddir)pTempItem->lpData)->pName, szTemp );
				sprintf( szTemp, szTemp2 );
			} // for

			if( -1 != (hFile = _findfirst(szTemp, &fdPlugin)) )
			{
				bDoneSearching = true;
				_findclose( hFile );
				sprintf( szDest, szTemp );
				DeleteDirList( &LDirList );
				return true;	// <---!!! Hmmm - Found It
			} // if

			_findclose( hFile );

//			if( psi->Verbose )	// Debug
//			{
//				sprintf( szTemp2, "File '%s' not found in : %s", szFileName, szTemp );
//				GamePrintString( szTemp2 );
//			}

			sprintf( szTemp, "*.*" );
			for( pTempItem = pSearchPos; pTempItem != NULL; pTempItem = ((LPfinddir)pTempItem->lpData)->pParent )
			{
				sprintf( szTemp2, "%s\\%s", ((LPfinddir)pTempItem->lpData)->pName, szTemp );
				sprintf( szTemp, szTemp2 );
			} // for

//			if( psi->Verbose )
//			{
//				sprintf( szTemp2, "Starting search of '%s'", szTemp );
//				GamePrintString( szTemp2 );
//			}

#ifdef SHOWFINDMODULEMESSAGEBOX

			sprintf( szTemp2, "Searching Dir: %s", szTemp );
			if( MessageBox(NULL, szTemp2, "Debug", MB_OKCANCEL) == IDCANCEL )
			{
				DeleteDirList( &LDirList );
				return false;
			}
#endif

			if( -1 == (hFile = _findfirst(szTemp, &fdPlugin)) )
			{		//Empty
				_findclose( hFile );

#ifdef SHOWFINDMODULEMESSAGEBOX

				sprintf( szTemp2, "FindFirst Empty: %s", szTemp );
				if( MessageBox(NULL, szTemp2, "Debug", MB_OKCANCEL) == IDCANCEL )
				{
					DeleteDirList( &LDirList );
					return false;
				}
#endif

				bRecurse = false;

				if( pSearchPos->pNext )	// Checking and Recurse
				{
					if( ((LPfinddir)pSearchPos->lpData)->pParent ==
						((LPfinddir)pSearchPos->pNext->lpData)->pParent )
						pSearchPos = pSearchPos->pNext;
					else
						bRecurse = true;
				}
				else
					bRecurse = true;

				while( bRecurse )
				{
					if( ((LPfinddir)pSearchPos->lpData)->pParent )
					{
						if( ((LPfinddir)pSearchPos->lpData)->pParent == pSearchBase )
						{
							sprintf( szDest, "Search exited with File '%s' not found - find first recurse loop - parent = base", szFileName );
							bDoneSearching = true;
							DeleteDirList( &LDirList );
							return false;
							// End Search
						} // if = base

						pSearchPos = ((LPfinddir)pSearchPos->lpData)->pParent;

						if( pSearchPos->pNext )
						{
							if( ((LPfinddir)pSearchPos->lpData)->pParent ==
								((LPfinddir)pSearchPos->pNext->lpData)->pParent )
							{
								pSearchPos = pSearchPos->pNext;
								bRecurse = false;
							}

						} // if next

					} // if parent != NULL
					else
					{
						sprintf( szDest, "Search exited with File '%s' not found - find first recurse loop - Parent = null", szFileName );
						bDoneSearching = true;
						DeleteDirList( &LDirList );
						return false;
						// End Search
					} // parent = NULL

				} // while recursing

			} // if findfirst empty
			else
			{
				bFoundDir = false;
				pTempItem = NULL;

				if( fdPlugin.attrib & _A_SUBDIR )
				{
					if( stricmp(fdPlugin.name, ".") && stricmp(fdPlugin.name, "..") )
					{
						pDir = new finddir;
						pDir->pName = new char[ strlen(fdPlugin.name)+2 ];
						sprintf( pDir->pName, fdPlugin.name );
						pDir->pParent = pSearchPos;
						pTempItem = LDirList.AddItem( pDir );
						bFoundDir = true;

#ifdef SHOWFINDMODULEMESSAGEBOX

						sprintf( szTemp2, "Dir found: %s", fdPlugin.name );
						if( MessageBox(NULL, szTemp2, "Debug", MB_OKCANCEL) == IDCANCEL )
						{
							DeleteDirList( &LDirList );
							return false;
						}
#endif
					} // if not dots
				} // if is dir

				while( 0 == _findnext(hFile, &fdPlugin) )
				{
					if( fdPlugin.attrib & _A_SUBDIR )
					{
						if( stricmp(fdPlugin.name, ".") && stricmp(fdPlugin.name, "..") )
						{
							pDir = new finddir;
							pDir->pName = new char[ strlen(fdPlugin.name)+2 ];
							sprintf( pDir->pName, fdPlugin.name );
							pDir->pParent = pSearchPos;

							if( !bFoundDir )
							{
								pTempItem = LDirList.AddItem( pDir );
								bFoundDir = true;

#ifdef SHOWFINDMODULEMESSAGEBOX

								sprintf( szTemp2, "Dir found: %s", fdPlugin.name );
								if( MessageBox(NULL, szTemp2, "Debug", MB_OKCANCEL) == IDCANCEL )
								{
									DeleteDirList( &LDirList );
									return false;
								}
#endif
							} // if !bFoundDir
							else
								LDirList.AddItem( pDir );
						} // if not dots
					} // if is dir

				} // while findnext
				_findclose( hFile );

				if( !bFoundDir )	// No Directories found
				{
#ifdef SHOWFINDMODULEMESSAGEBOX

					sprintf( szTemp2, "!bFoundDir: %s", szTemp );
					if( MessageBox(NULL, szTemp2, "Debug", MB_OKCANCEL) == IDCANCEL )
					{
						DeleteDirList( &LDirList );
						return false;
					}
#endif					
					bRecurse = false;

					if( pSearchPos->pNext )	// Checking and Recurse
					{
						if( ((LPfinddir)pSearchPos->lpData)->pParent ==
							((LPfinddir)pSearchPos->pNext->lpData)->pParent )
							pSearchPos = pSearchPos->pNext;
						else
							bRecurse = true;
					}
					else
						bRecurse = true;

					while( bRecurse )
					{
						if( ((LPfinddir)pSearchPos->lpData)->pParent )
						{
							if( ((LPfinddir)pSearchPos->lpData)->pParent == pSearchBase )
							{
								sprintf( szDest, "Search exited with File '%s' not found - Recursion !bFoundDir, parent = base", szFileName );
								bDoneSearching = true;
								DeleteDirList( &LDirList );
								return false;
								// End Search
							} // if = base

							pSearchPos = ((LPfinddir)pSearchPos->lpData)->pParent;

							if( pSearchPos->pNext )
							{
								if( ((LPfinddir)pSearchPos->lpData)->pParent ==
									((LPfinddir)pSearchPos->pNext->lpData)->pParent )
								{
									pSearchPos = pSearchPos->pNext;
									bRecurse = false;
								} // if this and next has same parent

							} // if next != NULL

						} // if parent != NULL
						else
						{
							sprintf( szDest, "Search exited with File '%s' not found - Recursion Parent = null, Search = %s", szFileName, ((LPfinddir)pSearchPos->lpData)->pName );
							bDoneSearching = true;
							DeleteDirList( &LDirList );
							return false;
							// End Search
						} // parent = NULL

					} // while recursing

				} // if no dir found
				else
					pSearchPos = pTempItem;		// Moving to First found Dir

			} // else findfind not empty
		} // while not done searching
		// Should not get here
		sprintf( szDest, "Search exited 'while' with File '%s' not found", szFileName );
		DeleteDirList( &LDirList );
		return false;
	} // if - findfirst
	// findfirst found the file
	strcpy( szDest, szTemp );
	return true;
} // D2FindFile