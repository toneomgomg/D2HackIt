/*	
	D2Loader SDK
  	Copyright (C) 2000, 2001  Onlyer(onlyer@263.net)

	This SDK is based on original Diablo2 game library,
	Diablo2 is a trademark of Blizzard Entertainment.
	This library is done by volunteers and is neither 
	supported by nor otherwise affiliated with Blizzard Entertainment.
	You should NEVER use the library on communicate use.

	It is distributed WITHOUT ANY WARRANTY, use it 
	at your own risk.
*/
/*
	How does D2loader Plugin works:
 * 1.	The Loader will search *.dll files under plugin\ directory and
	try to load it.
 * 2.	If the dll is loaded sucessfully, then the loader will try
 	to find if the dll have an exported entry named "QueryInterface"
	(See QueryInterfaceFunc)
 * 3.	After got the function "QueryInterface" entry address, That function
 	will be called, and the plugin dll should return a pointer to
	a PLUGIN_INTERFACE struct, this struct contains:
	1).a DWORD magicword. (should equal to PLUGIN_MAGICWORD, used for verify)
	2).a DWORD version number. (should equal to PLUGIN_VERSION)
	3).a LPCSTR description. (description of your plugin, which will be
		logged into file "d2loader.log" if the loader is started
		with option -l or -log)
	4).a pointer to function fpEntry, as described below
 * 4.	Then the loader will continue to load the game and the entry function
 	fpEntry will be called at certain condition.
	1).Just after step 3, the plugin dll is loaded, the loader
	   will call fpEntry with dwReason = REASON_INIT and 
	   lpData = a pointer to PLUGIN_DATA_INIT struct
	2).For all other reasons, the lpData parameter will be
	   a pointer to D2PARAM struct (see d2param.h), you may
	   get useful client information from this struct. do NOT
	   modify data in this struct if you are not sure.
	   (see chatlog plugin source code for a sample use of 
	   D2PARAM struct)
	3).If the loader is going to unload a plugin, fpEntry will
	   be called with REASON_CLEANUP.
	4).You can easily find out what does each reason mean from
	   their name. just notes here: dwReason maybe a a combination
	   of more than one reason only except REASON_INIT and
	   REASON_CLEANUP will not. also, two notices here:
	   a. ENTER and CLEANUP of the same set will never be combined
	   (e.g: if REASON_ENTER_GAME is in dwReason, then 
	   REASON_LEAVE_GAME will surely not)
	   b. there will never be more than one ENTER or LEAVE reasons
	   (e.g: if REASON_ENTER_GAME is in dwReason, then
	   other ENTER reason like REASON_ENTER_CHANNEL and
	   REASON_ENTER_MAIN_MENU will surely not)
	5).Here is an sample log of how dwReason is called.
	   (You may use option -l to check it)
	   a. Start the game, REASON_INIT will be sent
	   b. Enter main menu, REASON_ENTER_MAIN_MENU is sent
	   c. Connect to battle.net and logged in,
	      REASON_LEAVE_MAIN_MENU |REASON_ENTER_CHANNEL is sent
	      after the player select a character
	   d. create a join a game in battle.net
	      REASON_LEAVE_CHANNEL |REASON_ENTER_GAME is sent
	   e. quit game,REASON_LEAVE_GAME |REASON_ENTER_CHANNEL is sent
	   f. quit channel, REASON_LEAVE_CHANNEL |REASON_ENTER_MAIN_MENU
	   g. exit, REASON_CLEANUP is sent.
	6).The function fpEntry should return TRUE if it executed successfully.
	   return a FALSE value will cause the loader to unload it.
*/
#ifndef INCLUDED_PLUGIN_H
#define INCLUDED_PLUGIN_H


#include <windows.h>
#include "d2param.h"

#define DLLExport	__declspec( dllexport )

typedef DWORD ( __stdcall * PluginEntryFunc)(DWORD dwReason, LPVOID lpData);

typedef struct
{
	DWORD			dwMagicword;	/* should be "x0\x02D" */
	DWORD			dwVersion;	/* plugin version */
	LPCSTR			szDescription;
	PluginEntryFunc		fpEntry;
} PLUGIN_INTERFACE, * LPPLUGIN_INTERFACE;
#define PLUGIN_MAGICWORD		0x44320000
#define PLUGIN_VERSION			0x01000911

#define REASON_NONE			0x00
#define REASON_INIT			0x01
typedef struct
{
	DWORD			dwVersion;
	HANDLE			hEventUnload;
} PLUGIN_DATA_INIT, * LPPLUGIN_DATA_INIT;

#define REASON_CLEANUP			0x02
#define REASON_ENTER_GAME		0x04
#define REASON_LEAVE_GAME		0x08
#define REASON_ENTER_CHANNEL		0x10
#define REASON_LEAVE_CHANNEL		0x20
#define REASON_ENTER_MAIN_MENU		0x40
#define REASON_LEAVE_MAIN_MENU		0x80

typedef LPPLUGIN_INTERFACE ( __stdcall * QueryInterfaceFunc) (VOID);

#endif
