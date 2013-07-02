//
//  File: de.cpp         originally part of dikuEdit
//
//  Usage: global variable declarations and main() function
//
//  Copyright 1995-98 (C) Michael Glosenger
//

//
// This and all other source files used by this file (except those mentioned as
// not written by me and of course standard C/C++ headers) are copyright (C) 
// 1995-98 Michael Glosenger, all rights reserved, enjoy
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __UNIX__
  char cursesInit = 0;
#endif

//
//  About graph.h - system-specific text I/O functions supplied with Watcom
//                  and used in various functions for cursor pos/color
//
//  _outtext(char *strn) - displays text in strn using colors set with
//                         _settextcolor and _setbkcolor
//
//  _settextcolor(char fg) - from 0-15 (standard IBM PC colors), sets color of
//                           all subsequent text output with _outtext.
//
//  _setbkcolor(char bg) - from 0-7 (standard IBM PC colors), sets background
//                         color of all subsequent text output with _outtext.
//
//  _gettextcolor - returns current foreground color being used
//
//  _gettextposition - returns a structure rccoord that contains two members,
//                     row and col, each short ints.  row and col are set to
//                     the current cursor position onscreen.  (upper-left
//                     corner - row = 1, col = 1)
//
//  _settextposition(struct rccoord coords) - uses row and col members in
//                                            coords to set cursor pos.
//                                            subsequent text output with
//                                            _outtext is drawn here.
//

#include "de.h"          // header for this here file
#include "system.h"      // header for system-specific stuff
#include "vardef.h"      // default values for various variables

#include "fh.h"          // header that contains function predefinitions

using namespace std;

// global variables

char madeChanges = FALSE;  // set to TRUE if user makes changes to data
char display = FALSE;      // this is lame, but then, what isn't?
char readFromSubdirs = FALSE;
                           // whether to read from wld/ mob/ etc. subdirs

char roomOnly = FALSE,     // set to TRUE if user wants to edit only rooms/
     objOnly = FALSE,      // objs/mobs
     mobOnly = FALSE;

char oldObjConv = FALSE;   // has to be done..  if an object file containing
                           // objects from pre-material/space/size days, this
                           // is set

char zoneLoadedWithQuest = FALSE;
     // used to determine if .QST file should be deleted
char zoneLoadedWithShop = FALSE;
     // used to determine if .SHP file should be deleted

char bold = FALSE, flashing = FALSE;  // ANSI crap

ulong commandsEntered = 0;   // for "save every X commands"

// pointers to linked lists and other stuff that is vital to DikuEdit

dikuRoom *roomHead = NULL, *currentRoom = NULL;      // .WLD
dikuObject *objectHead = NULL;                       // .OBJ
dikuMob *mobHead = NULL;                             // .MOB
dikuZone zoneRec;                                    // .ZON
//zoneRandomTables zoneRandTables;                     // .TAB

// default stuff

dikuRoom *defaultRoom = NULL;
roomExit *defaultExit = NULL;
dikuObject *defaultObject = NULL;
dikuMob *defaultMob = NULL;

// lookup tables

dikuRoom  **roomLookup = NULL;
dikuObject **objLookup = NULL;
dikuMob    **mobLookup = NULL;

ulong numbLookupEntries = DEFAULT_LOOKUP_ENTRIES;

ulong lowestRoomNumber = numbLookupEntries;   // handy
ulong highestRoomNumber = 0;

ulong lowestObjNumber = numbLookupEntries;   // handy
ulong highestObjNumber = 0;

ulong lowestMobNumber = numbLookupEntries;   // handy
ulong highestMobNumber = 0;


// counters that keep track of number of whatever existing

ulong numbRooms = 0, numbObjTypes = 0, numbMobTypes = 0,
      numbExits = 0, numbObjs = 0,     numbMobs = 0;


// miscellaneous strings and list heads

char promptString[256];  // input prompt - changes throughout the proggie, you
                         // see - actual prompt text is stored here, prompt
                         // "template" is in user variable

entityLoaded *numbLoadedHead = NULL;  // used to store how many of whatever
                                      // have been loaded

editableListNode *inventory = NULL;  // stores inventory

// heads of millions of command lists, one big, most small

command *cmdHead = NULL, *lookupCmdHead = NULL, *createCmdHead = NULL,
        *loadCmdHead = NULL, *editCmdHead = NULL, *listCmdHead = NULL,
        *createEditCmdHead = NULL, *setCmdHead = NULL, *deleteCmdHead = NULL,
        *statCmdHead = NULL, *setLimitCmdHead = NULL, *cloneCmdHead = NULL,
        *copyDescCmdHead = NULL, /**flagFieldNameHead = NULL,*/
        *copyDefaultCmdHead = NULL, *copyCmdHead = NULL;

alias *aliasHead = NULL;   // head of alias list

variable *varHead = NULL;  // head of variable list

// pointer to head of command history list

stringNode *commandHistory = NULL;

extern ulong roomFlagTemplates[], objExtraFlagTemplates[], objWearFlagTemplates[],
             objExtra2FlagTemplates[], objAntiFlagTemplates[], objAnti2FlagTemplates[],
             objAff1FlagTemplates[], objAff2FlagTemplates[], 
             objAff3FlagTemplates[], objAff4FlagTemplates[], mobActionFlagTemplates[],
             mobAff1FlagTemplates[], mobAff2FlagTemplates[],
             mobAff3FlagTemplates[], mobAff4FlagTemplates[];

// main

int main(const int argc, const char *argv[])
{
  char strn[256];
  long i;
 // init flag template arrays

  textmode(C80);
  memset(roomFlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);

  memset(objExtraFlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objWearFlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objExtra2FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAntiFlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAnti2FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAff1FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAff2FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAff3FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(objAff4FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);

  memset(mobActionFlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(mobAff1FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(mobAff2FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(mobAff3FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);
  memset(mobAff4FlagTemplates, 0, sizeof(ulong) * NUMB_FLAG_TEMPLATES);

 // first, clear the screen to fg 7, bg 0

#ifndef _WIN32
  _clearscreen(7,0);
#endif

 // check for command-line arguments or lack thereof

  if (argc < 2)
  {
    cout <<
"Specify the \"main\" name of the zone as the first command-line argument -\n"
"i.e. 'DE FOREST'.  The .WLD, .MOB, etc. files will have this prefix.\n";

    exit(1);
  }

  if (strlen(argv[1]) >= MAX_MAINNAME_LEN)
  {
    cout <<
"That \"main\" zone name is too long (greater than " << (usint)(MAX_MAINNAME_LEN - 1) << " characters).\n";

    exit(1);
  }

 // set mainName var

  addVar(&varHead, VAR_MAINZONENAME_NAME, argv[1]);

 // init the command list for the .set file

  initSetCommands();
  initSetLimitCommands();

 // read the data files

  display = FALSE;

  readSettingsFile(MAIN_SETFILE_NAME);

  strcpy(strn, getMainZoneNameStrn());
  strcat(strn, ".set");

  checkPreArgs(argc, argv);   // certain args need to be checked for first

  readSettingsFile(strn);     // read zone-specific settings file

  display = TRUE;

 // check command-line args (after reading set file..)

  checkArgs(argc, argv);

 // if user wants to go into edit-only mode, oblige willingly

  if (roomOnly)
  {
    editRoomsOnly();
    return 0;
  }
  else
  if (objOnly)
  {
    editObjsOnly();
    return 0;
  }
  else
  if (mobOnly)
  {
    editMobsOnly();
    return 0;
  }

 // init reference tables

  roomLookup = new   dikuRoom*[numbLookupEntries];
  objLookup  = new dikuObject*[numbLookupEntries];
  mobLookup  = new    dikuMob*[numbLookupEntries];

  if (!roomLookup || !objLookup || !mobLookup)
  {
    cout << "\n" <<
"Error allocating memory for room, object, and/or mob lookup tables - the\n"
"current size of " << numbLookupEntries << " entries will take up " <<
(numbLookupEntries * (sizeof(dikuRoom*) + sizeof(dikuObject*) +
                      sizeof(dikuMob*))) << " bytes - try reducing\n"
"the size with the -ll=<max vnum> command-line switch, or buy more RAM.\n";

    return 1;
  }

  memset(roomLookup, 0, numbLookupEntries * sizeof(dikuRoom*));
  memset(objLookup, 0, numbLookupEntries * sizeof(dikuObject*));
  memset(mobLookup, 0, numbLookupEntries * sizeof(dikuMob*));

#ifdef _WIN32
  if (checkWindowsVer()) return 1;

  if (setupWindowsConsole()) return 1;

  SetConsoleTitle(DE_WIN32_CONSOLEAPP_NAME);
#endif

 // read the main data files

  readWorldFile();
  readObjectFile();
  readMobFile();
  readZoneFile();
  readQuestFile();
  readShopFile();

  checkForDupeShopLoaded();

  readDefaultsFromFiles(MAIN_DEFAULT_NAME); // first, read "default defaults"...
  readDefaultsFromFiles(getMainZoneNameStrn()); // then, zone name...

 // quest/shop vars used to determine if we need to delete .qst/.shp file
 // when user saves - otherwise we'd have remnants if some existed before
 // and user deleted all quest/shops

  zoneLoadedWithQuest = checkForMobWithQuest();
  zoneLoadedWithShop  = checkForMobWithShop();

 // init stuff (under Unix, ncurses needs extra-special initializing)

#ifdef __UNIX__
  initUnixCursesScreen();
#endif
#ifdef _WIN32
  initWindowsScreen();
  _clearscreen(7,0);
#endif
  startInit();

 // check startRoom var - if set and room is valid, stick user there

  if (getStartRoomActiveVal() && varExists(varHead, VAR_STARTROOM_NAME))
  {
    i = getRoomStartVal();

    if (findRoom(i)) currentRoom = findRoom(i);
  }

 // set check guild val based on some semi-intelligent crap

  if (!getCheckGuildStuffVal())
  {
    strcpy(strn, zoneRec.zoneName);
    lowstrn(strn);

   // yep, this is as good as it gets..  shrug

    if (strstr(strn, "player_guilds") || findRoom(35000))
      setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, TRUE, TRUE);
  }

 // display the initial room and prompt


  displayRoom(currentRoom);
  displayPrompt();

 // cycle through interpCommands like crazy, my man

  while (TRUE) interpCommands(NULL, TRUE);  // prompt user from now until
                                            // eternity
                                                
  return FALSE;  // shouldn't get here..
}

int gback, gfore;

struct rccoord _gettextposition (){
   struct rccoord temp;
   temp.row = wherey();
   temp.col = wherex();
   return temp;
}

void _outtext(char *textstring) {
  // Needs to replace /n with /r/n
   int len = strlen(textstring), pos = 0, i;
   char *outie;
   outie = new char[len+64];
   for (i = 0; i < len; i++) {
      if (textstring[i] == '\n') {
        outie[pos] = '\n';
        pos ++;
        outie[pos] = '\r';
        pos++;
      } else {
        outie[pos] = textstring[i];
        pos ++;
      }
   }
   outie[pos] = '\0';
   cprintf(outie);
}                                                
void _settextcolor(int which) {
 gfore = which;
 textcolor(which);
}
int _gettextcolor() {
  return gfore;
}
int _getbkcolor() {
  return gback;
}
void _settextposition(int row, int col ){
  gotoxy(col, row);
}
void _setbkcolor(int which) {
  gback = which;
  textbackground(which);
}

void _clearscreen(int f, int b) {
  textcolor(f);
  textbackground(b);
  clrscr();
}


