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
	This file contains definition of struct D2PARAM,
	which contains lots of useful client runtime information.
	This struct is not only used by the loader, but also widely
	used in game internally (EXCEPT the CUSTOM member, which is
	used only by d2loader), not all of the members is discoverd.
	(all member begin with 'u' means unknown member)
	You can find out what does a member mean directly from its
	name for most member, no much comment for them yet :P
	(some members are refered and exported to d2loader parameter 
	list, which are also described in d2loader readme)
	If you find out any useful infomation, feel free to 
	mail: onlyer@263.net.

*/
#ifndef INCLUDED_D2PARAM_H
#define INCLUDED_D2PARAM_H

#include <windows.h>

#pragma	pack(push,1)

#define	D2PARAM_STRING_LEN		24
typedef	CHAR	STRING[D2PARAM_STRING_LEN];

typedef struct
{
	BYTE		expansion;
	CHAR		u1[3];
} D2PARAM_MAIN;

typedef struct
{
	BYTE		window;
	BYTE		glide;
	BYTE		opengl;
	BYTE		rave;
	BYTE		d3d;
	BYTE		perspective;
	BYTE		lowquality;
	DWORD		gamma;
	BYTE		vsync;
	DWORD		framerate;
} D2PARAM_VIDEO;

typedef struct
{
	DWORD		gametype;	
	WORD 		joinid;
	STRING		gamename;
	STRING		serverip;
	STRING		battlenetip;
	STRING		mcpip;
	DWORD		u1;
	BYTE		nopk;
	BYTE		openc;
} D2PARAM_NETWORK;

typedef struct
{
	BYTE		ama;
	BYTE		pal;
	BYTE		sor;
	BYTE		nec;
	BYTE		bar;
	BYTE		dru;
	BYTE		asn;
	BYTE		invincible;
	STRING		bnacct;
	STRING		bnpass;
	STRING		name;
	STRING		realm;
	CHAR		u1[0x100];		/* 0x0-0x20 is character portrait */
	BYTE		ctemp;
	BYTE		charclass;		/* 0x0 - 0x06 */
	WORD		charstatus;		/* 0x20 if expansion, 0x04 for hardcore, 
						0x08 for dead */
} D2PARAM_CHARACTER;

typedef struct
{
	BYTE		nomonster;
	DWORD		monsterclass;
	BYTE		monsterinfo;
	DWORD		monsterdebug;
} D2PARAM_MONSTER;

typedef struct
{
	BYTE		rare;
	BYTE		unique;
	BYTE		u1;
	BYTE		u2;
} D2PARAM_ITEM;

typedef struct
{
	DWORD		act;
} D2PARAM_INTERFACE;

typedef struct
{
	BYTE		nopreload;
	BYTE		direct;
	BYTE		lowend;
	BYTE		nocompress;
} D2PARAM_FILEIO;	

typedef struct
{
	WORD		arena;
	DWORD		u1;			/* 0x0804, 0x04 */
	BYTE		u2;
	WORD		u3;
	BYTE		difficulty;
	FARPROC		expansion;
	BYTE		text;
} D2PARAM_GAME;

typedef struct
{
	BYTE		log;
	BYTE		msglog;
	BYTE		safemode;
	BYTE		nosave;
	DWORD		seed;
	BYTE		cheats;
	BYTE		nosound;
	BYTE		questall;
	BYTE		u1;
} D2PARAM_DEBUG;	

typedef struct
{
	DWORD		comint;	
	DWORD		token;
	STRING		realm;
	STRING		gamepass;
	STRING		s2;
	CHAR		u2[0xE8];
	BYTE		skiptobnet;
	BYTE		lasttype;		/* 0,1 for main menu, 2 for skip to bnet */
						/* 4 for skip to open */
	BYTE		shownlogo;
	BYTE		u5;
	CHAR		cchannel[0x20];		/* current channel */
	CHAR		dchannel[0x20];		/* default or previous channel */
} D2PARAM_BNET;

typedef struct
{
	BYTE		client;
	BYTE		server;
	BYTE		launch;
	BYTE		multi;
	BYTE		notitle;
	BYTE		res800;
	BYTE		res640;
	BYTE		nonotify;
	BYTE		noexit;
	BYTE		autorestart;
	BYTE		nohook;
	BYTE		nochar;
	BYTE		clientexit;
	BYTE		multiclient;
	BYTE		printarg;
	BYTE		noscript;
	BYTE		noplugin;
	BYTE		debuglog;
	BYTE		console;
	BYTE		rmconffile;
	DWORD		dllentry;
	DWORD		version;
	STRING		language;
	STRING		servername;
	CHAR		title[128];
	CHAR		conffile[MAX_PATH];
} D2PARAM_CUSTOM;

typedef struct
{
	D2PARAM_MAIN		MAIN;
	D2PARAM_VIDEO		VIDEO;
	D2PARAM_NETWORK		NETWORK;
	D2PARAM_CHARACTER	CHARACTER;
	D2PARAM_MONSTER		MONSTER;
	D2PARAM_ITEM		ITEM;
	D2PARAM_INTERFACE	INTERFACE;
	D2PARAM_FILEIO		FILEIO;
	D2PARAM_GAME		GAME;
	D2PARAM_DEBUG		DEBUGS;
	D2PARAM_BNET		BNET;
	D2PARAM_CUSTOM		CUSTOM;
} D2PARAM, * PD2PARAM, * LPD2PARAM;

#pragma	pack(pop)

#endif
