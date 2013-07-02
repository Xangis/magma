//
//  File: init.cpp       originally part of dikuEdit 
//                                                                          
//  Usage: various functions used when initializing at startup
//
//  Copyright 1995-98 (C) Michael Glosenger                                  
//

#include <iostream>
#include <stdlib.h>

#include "fh.h"
#include "types.h"
#include "keys.h"
#include "vardef.h"
#include "const.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "graphcon.h"
#endif

using namespace std;

extern dikuRoom *currentRoom, *roomHead, **roomLookup;
extern dikuZone zoneRec;
extern ulong numbLookupEntries, highestRoomNumber, lowestRoomNumber, numbRooms;
extern char madeChanges;
extern variable *varHead;
extern _CONST char *exitnames[];


//
// startInit : Initializes various thingies after reading the data files
//

void startInit(void)
{
  dikuRoom *room = currentRoom = roomHead;
  char strn[64] = "\0";//, bigstrn[512];
  struct rccoord coords;
  usint ch;
  ulong i;


 // init command lists

  initMainCommands();
  initLookupCommands();
  initCreateCommands();
  initLoadCommands();
  initEditCommands();
  initListCommands();
  initCreateEditCommands();
  initDeleteCommands();
  initStatCommands();
  initCloneCommands();
  initCopyDescCommands();
  initCopyDefaultCommands();
  initCopyCommands();
//  initFlagFieldNames();

 // make sure that key vnums specified for exits are valid (only checks
 // if the "check vnums to make sure they're valid" var is true)

  if (getVnumCheckVal())
  {
    while (room)
    {
      for (i = 0; i < NUMB_EXITS; i++)
      {
        checkRoomExitKey(room->exits[i], room, exitnames[i]);
      }

      room = room->Next;
    }
  }

 // clear the screen to fg 7, bg 0

  _clearscreen(7,0);
  _settextcolor(7);
  _setbkcolor(0);

  room = roomHead;

 // check for non-existence of roomHead - if it ain't there, create a default
 // room (user has started new zone)

  if (!roomHead)
  {
    roomHead = currentRoom = room = new dikuRoom;

    if (!roomHead)
    {
      cout << "\n\n"
              "ERROR: startInit() ran out of memory allocating roomHead.\n"
              "       You must be supah-low on memory, free some up.\n\n";

      exit(1);
    }

    memset(roomHead, 0, sizeof(dikuRoom));
    memset(&zoneRec, 0, sizeof(dikuZone));

    displayColorString("\n&+C"
"Editing new zone - specify a starting vnum for rooms, objs, and mobs:&n ");

    coords = _gettextposition();

    while (TRUE)
    {
      getStrn(strn, 6, 1, 7, '°', "1", FALSE, FALSE);

      if ((atoi(strn) != 0) && strnumer(strn) &&
          (atoi(strn) < numbLookupEntries)) break;
      else _settextposition(coords.row, coords.col);
    }

    i = atoi(strn);

    roomHead->roomNumber = highestRoomNumber = lowestRoomNumber = i;
    roomLookup[i] = roomHead;

    _settextcolor(7);
    _setbkcolor(0);

   // get name for zone straight off

    displayColorString("\n&+C"
"Enter a new name for the zone:&n ");

    getStrn(zoneRec.zoneName, MAX_ZONENAME_LEN, 1, 7, '°',
            "The Horrible Tracts of the Unnamed", FALSE, FALSE);

    strcpy(roomHead->roomName, "Unnamed");

   // if user hit [Enter], roomNumb and topRoom set to 0 by memset()s above

//    roomHead->zoneNumber = zoneRec.zoneNumber = 0;

   // set zone defaults - whee

/*    strcpy(zoneRec.zoneName, "The Horrible Tracts of the Unnamed");*/
    zoneRec.lifeLow = 40;
    zoneRec.lifeHigh = 50;
    zoneRec.resetMode = ZONE_RESET_ALWAYS;

/*
    _setbkcolor(0);
    displayColorString("\n&+cIs this zone a guildhall (&+Cy/N&n&+c)? ");
    do
    {
      ch = toupper(getkey());
    } while ((ch != K_Enter) && (ch != 'Y') && (ch != 'N'));

    if (ch == 'Y')
    {
      displayColorString("yes\n\n");
      setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, TRUE, TRUE);
    }
    else
    {
      displayColorString("no\n\n");
      setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, FALSE, TRUE);
    }
*/

    madeChanges = TRUE;

    numbRooms = 1;
  }
  else

 // create a master keyword list and editable list for each room (only
 // needs to be done if new zone wasn't just created)

  while (room)
  {
    room->masterListHead = createMasterKeywordList(room);
    room->editableListHead = createEditableList(room);

    room = room->Next;
  }

 // check that all rooms have the same zone value as the zone itself
 // (mud doesn't actually use the room value, but hey, let's do it anyway)

  verifyZoneFlags();

 // read the default room, object, mob info

//  readDefaultsFromFiles();

 // if any room vnum is >=100000, we have a map zone, so set variable
 // appropriately

  room = roomHead;

  while (room)
  {
    if (room->roomNumber >= 100000)
    {
     // default val of variable is FALSE, so no need to set it to FALSE
     // (besides, maybe the user wants to override for some reason)

      setVarBoolVal(&varHead, VAR_ISMAPZONE_NAME, TRUE, TRUE);
      break;
    }

    room = room->Next;
  }

 // create the initial promptString

  createPrompt();

 // clear the screen

  _clearscreen(7,0);
}
