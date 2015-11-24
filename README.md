# D2HackIt! v0.60

This is the full source code for D2HackIt! 0.60 together with a few select original modules under the modules directory.

I'm putting this up here solely for archiving purposes. None of this stuff works anymore (I assume). My old e-mail at ~~thohell@home.se~~ is long gone and most (if not all) of the links found in this repository are dead.

This code is released in public domain, see UNLICENSE for details.

```
D2HackIt! v0.60 README!

--------------------------------------------------------------------------------

What is D2HackIt?
-----------------

  D2HackIt! consists of a server dll and a loader which tricks the game into 
  loading the dll as if it was its own. Once the server dll is in memory, it 
  intercepts some locations of the games code. It also exposes some functions 
  from the game as 'clean' c functions that can be called like any other c 
  functions. Once this is done, the server dll is an actual part of the game 
  code, and gets a chance to snoop and/or change information passed in the game 
  and perform actions as if it was the game itself. 

  The server dll is also in charge of loading/unloading plug-ins that provide 
  the actual functionality of the hack. Note: Without hack plugins, D2HackIt! 
  is useless. By the time 1.0 is released, there will be a few modules 
  available, written by me and others. A very easy to modify 'skeleton plugin' 
  will also be available as c source to get you started. 

  List of features (implemented and/or planned) 

  * Version independent patching by the use of 'fingerprints' and memory 
    search-and-report. 
  * In-game input and feedback, using standard functions in the game. 
  * Loading of plug-ins that extends the functionality of the game in various 
    ways. 
  * Fully extendible command-line interface for server and clients. 
  * The use of a bind client to bind keystrokes to actions/commandlines to 
    perform many tasks with one keystroke. 
  * Packet sending/listening in each direction with the option to rewrite 
    packages on the fly. 
  * Central event dispatcher for common client events. (OnGamePacketSent, 
    OnGamePacketReceived,OnJoinGame etc.etc.) 
  * Exposing common functions from the game. (GamePrintString, 
    GameSendPacketToServer, GameInterceptCode etc.etc.) 
  * Exposing common structures from the game, such as playerinfo, gameinfo etc. 
  * Intercept dispatcher to let multiple clients intercept the same 
    instruction of code in memory. 
  * Hookable WinProc to allow clients to receive/modify WM_ messages sent to 
    the game. 
  * Ini-file handling exposed to clients to save/restore settings. 
  * Fingerprint functions exposed to clients. 
  * ...and much more. 

  The goal is to provide a stable and simple environment to write hack plugins 
  that runs in the context of the game and provides extra functionality. 
  Essentially, the plugins are an extension of the game itself. These plugins 
  use a simple API and can be written in any language that can produce 
  standard window dlls. The list of possible plugins is endless; packet 
  sniffers/senders, item grabbers, memory search/replace, in-game 
  irc-clients, in-game ICQ-notifier, in-game mail-notifier, different maphacks, 
  trainers (for open/single), auto go-to-town when a PK is close, spam-filter, 
  color-hack, CD-Key changer etc.etc.


A Note from Herzog_Zwei
-----------------------

 I integrated the changes I made a while back into the latest version.  This version
 contains some bug fixes and changes necessary to allow an easier implementation to guard
 against most forms of the drop item hack without affecting normal game play.  It contains
 a change that may break some older modules (which is easy to fix as long a the source code
 for them is available).  You will need to update your d2hackit.ini file to include extra
 fingerprints.  The new fingerprints are:

 GamePacketReceivedIntercept2=D2Client.dll,7,18,85C9894C24xx0F8FxxxxxxxxFF05xxxxxxxx5F5E5D
 GameSendMessageToChat=bnclient.dll,0,0,8BD15785D274xx8BFA83C9FF

A Note from D'Flame
-------------------

 Man, took me 3 solid days to write the recursive directory search routine =P
 I don't want to do THAT again. The Load command will search all subdirectories
 where the d2hackit is located. Some of the modules that use custom .INI file
 routines will probably still only look in the d2hackit directory, so for those
 put the .INI files for that module in there, until the writter updates for this
 version.

 Report Bugs to: http://www.gamer-networks.com/forums/
                 in the D2HackIt Module Develop section

A Note from bootyjuice
----------------------

 I made these modifications to thohell's core module because the parser was bothering me 
 (it executed multiple commands in reverse order) and because I thought a command to 
 emulate a mouseclick would be cool.  I highly encourage thohell or anyone else to
 use this code.

 You can contact me at the following locations with any issues/questions:

 Cheatlist: bootyjuice
 GOD forum: bootyjuice
 http://people.atl.attbroadband.com/bootyjuice

Changes since 0.59
------------------
 0.60 2002-10-10 Merged D'Flame's changes from 0.581 -> 0.582 into 0.59: -hz
			     GetHackProfileSectionNames didn't return a proper string.
				 Fixed GetHackProfileString for when SectionName or KeyName is NULL,
				 Use GetHackProfileStringEx, you need the size - D'Flame

Changes since 0.58
------------------
 0.59 2002-10-07 Added GetServerInfo (retrieve D2HackIt server info),
				  GameSendMessageToChat (send message to B.Net chat room),
				  GameInsertPacketToGame (insert packet into front of receive queue => there is
				  now a receive queue).
				 Added.disconnectchat command to disconnect from B.Net chat room.
				 Added critical section to prevent potential race conditions from
				  loading/unloading modules and TickThread execution.
				 Fixed OnGamePacketBeforeSent and OnGamePacketBeforeReceived to honor returned
				  value as new length (as opposed to treating it as a BOOL and using the original
				  length) (this breaks some modules that incorrectly return BOOL instead of the
				  packet length).
				 Fixed server start/stop to handle TickThread better.  Exported OnGameTimer in
				  D2Client.h template.

				 -hz

Changes since 0.57
------------------

 0.58 2002-08-21 Users:
                   D2Loader can now load it. Modules can now be in their own directories
                   as long as it's somewhere in the D2HackIt directory.

				 Module Makers:
				   The GetHackProfile functions will automaticlly delete the returned
                   strings on exit. Added GetHackProfileSectionNames and D2FindFile

				   LPSTR EXPORT GetHackProfileSectionNames( LPSTR lpHackName )
				     Virtually functions the same as the similarly named Win32 one

				   bool D2FindFile( char* szDest, const char* szFileName )
				     Searches for files in the D2HackIt Directory and Subdirectories

                  - D'Flame

Changes since 0.56
------------------

 0.57 2002-02-04 Added click() command.  Rewrote parser to execute multiple commands
                 in the proper order and to support strings ("") - bootyjuice

Changes since 0.52
------------------

 0.53 2001-12-09 Fixed a memory leak with |
 0.54 2001-12-12 D2HackIt is now -multiclient compatible with D2Loader (Thanks to onlyer)
 0.55 2001-12-17 Fixed a problem with fingerprints & multiclient
 0.56 2001-12-24 Added some code sent by sonata + a new loader for multiclient
 0.57 2002-02-04 click() '|' string support
 0.58 2002-08-21 D2Loader loaded  Module in different dirs  other stuff
http://www.gamer-networks.com/forums/forumdisplay.php?s=&forumid=21

How do I start/stop it?
-----------------------

  To start:
  1. Start D2/LoD and join a game
  2. Doubleclick loader.exe
  3. Alt+Tab back to game

  To stop:
  1. Doubleclick loader.exe
  2. Alt+Tab back to game

  If you use D2Loader, make sure you read D2Loader.txt for more instructions!
  Put D2HackIt files in <Diablo Dir>\plugin
  (ie. C:\Games\Diablo II\plugin)

What are the built in commands?
-------------------------------

  .load <module>                   - Loads <module>
  .unload <module>                 - Removes <module>
  .list                            - Lists all currently loaded modules
  .send <packet>                   - Sends a packet to the game
  .receive <packet>                - Fakes a server packet to the game
  .say <text>                      - Say something in the game
  .overhead <text>                 - Put text over your head in the game
  .set <verbose on|off>            - Shows/hides extra debug-info
  .version                         - Shows your D2HackIt version.
  .credits                         - Shows credits
  .repeat <count> <command>        - Repeats <command>, <count> # of times

Multiple commands
-----------------
  
  You can execute multiple commands on one comman line by separating them with
  a pipe "|". Remember to omit the dot "." on all commands except the first!

  examples
    ".say Clicking OK|send 4f040000000000" - Says "Clicking OK" and clicks ok
	                                         in trade.

Colors
------

  You can use colors in games by using $$ and a color code. This works for
  any command, anything you say and any text you put over your head with "!"

  examples:
    "$$1This is red"               - You say "This is red" in red
	"bind 70 0 overhead $$2Green"  - Puts "Green" over your head when hitting F1
   
D2HackIt.ini
------------
  
  In d2hackit.ini you can define various aspects of the behaviour of the program
  that you can not set from inside the game:

  Autoload=                       - List modules that will autoload when 
                                    starting D2HackIt
  InfoPrompt=                     - Custom info prompt
  ErrorPrompt=                    - Custom error prompt
  VerbosePrompt=                  - Custom verbose prompt

Official websites
-----------------
  http://thohell.d2network.com/ - its been down for a long time now =/

Official forum
--------------
  D2HackIt
  http://www.gamer-networks.com/forums/forumdisplay.php?s=&forumid=21

  D2HackIt Module Developement
  http://www.gamer-networks.com/forums/forumdisplay.php?s=&forumid=28

/thohell
--------------------------------------------------------------------------------
```
