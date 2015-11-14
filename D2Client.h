// Non-exported functions
extern FUNCTIONENTRYPOINTS		*server;
extern char ModuleWebsite[];
extern char ModuleAuthor[];
extern DWORD ModuleVersion;
extern char	ModuleDescription[];
extern char	ModuleEmail[];

// Exported functions
extern "C" {
BOOL EXPORT OnClientStart();
BOOL EXPORT OnClientStop();
DWORD EXPORT OnGameTimerTick();
BOOL EXPORT OnGameCommandLine(char** argv, int argc);
DWORD EXPORT OnGamePacketBeforeSent(BYTE* aPacket, DWORD aLen);
DWORD EXPORT OnGamePacketBeforeReceived(BYTE* aPacket, DWORD aLen);
LPCSTR EXPORT GetModuleAuthor();
LPCSTR EXPORT GetModuleWebsite();
LPCSTR EXPORT GetModuleEmail();
LPCSTR EXPORT GetModuleDescription();
DWORD EXPORT GetModuleVersion();
VOID EXPORT OnGameJoin(THISGAMESTRUCT* thisgame);
VOID EXPORT OnGameLeave(THISGAMESTRUCT* thisgame);
}

// Commandline functions
typedef struct modulecommandstruct_t
{
	char*	szName;
	BOOL	(PRIVATE *pFunc)(char** argv, int argc);
	char*	szUsage;
} MODULECOMMANDSTRUCT;
extern MODULECOMMANDSTRUCT ModuleCommands[];
BOOL PRIVATE OnGameCommandHelp(char** argv, int argc);

//
//
// Nice little enum for character classes
enum {
	CharClassAmazon = 0,
	CharClassSorceress,
	CharClassNecromancer,
	CharClassPaladin,
	CharClassBarbarian,
	CharClassDruid,
	CharClassAssassin
};


// Helper stuff to make the code less messy :)
#define BIND_TO_SERVER(x)		server->##x##=(fn##x##)GetProcAddress(hModule,#x ##);
#define CLIENTINFO(_vMaj, _vMin, _Author, _url, _desc, _email)	\
	DWORD	ModuleVersion=		MAKELONG(_vMaj,_vMin);			\
	char	ModuleAuthor[]=		#_Author;						\
	char	ModuleWebsite[]=	#_url;							\
	char	ModuleDescription[]=#_desc;							\
	char	ModuleEmail[]=		#_email;

