//
//  File: editrext.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing room exits
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "room.h"
#include "exit.h"

#include "../misc/menu.h"

using namespace std;

extern char madeChanges;

//
// displayEditRoomExitsMenu : displays the edit exits menu for rooms
//
//       *room : pointer to room being edited
//  *exitFlags : "exit flags" - specify which exits are editable
//

void displayEditRoomExitsMenu(const dikuRoom *room, ulong *exitFlags)
{
  ulong exitFlag = 0;
  char strn[256], newName[256];


 // clear the screen, set colors..

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomName(room->roomName, newName);

 // display title

  sprintf(strn,
          "&+gEditing exits for room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          room->roomNumber, newName);

  displayColorString(strn);

 // set exitFlags and numbExits

 // should update for new exit stuff to make more modular/easily updatable -
 // exit fix search hook

  if (room->exits[NORTH])
  {
    exitFlag |= EXIT_NORTH_FLAG;

    displayColorString("   &+YN&+L.&n &+wEdit north exit\n");
  }

  if (room->exits[NORTHWEST])
  {
    exitFlag |= EXIT_NORTHWEST_FLAG;

    displayColorString("   &+Y1&+L.&n &+wEdit northwest exit\n");
  }

  if (room->exits[NORTHEAST])
  {
    exitFlag |= EXIT_NORTHEAST_FLAG;

    displayColorString("   &+Y2&+L.&n &+wEdit northeast exit\n");
  }

  if (room->exits[SOUTH])
  {
    exitFlag |= EXIT_SOUTH_FLAG;

    displayColorString("   &+YS&+L.&n &+wEdit south exit\n");
  }

  if (room->exits[SOUTHWEST])
  {
    exitFlag |= EXIT_SOUTHWEST_FLAG;

    displayColorString("   &+Y3&+L.&n &+wEdit southwest exit\n");
  }

  if (room->exits[SOUTHEAST])
  {
    exitFlag |= EXIT_SOUTHEAST_FLAG;

    displayColorString("   &+Y4&+L.&n &+wEdit southeast exit\n");
  }

  if (room->exits[WEST])
  {
    exitFlag |= EXIT_WEST_FLAG;

    displayColorString("   &+YW&+L.&n &+wEdit west exit\n");
  }

  if (room->exits[EAST])
  {
    exitFlag |= EXIT_EAST_FLAG;

    displayColorString("   &+YE&+L.&n &+wEdit east exit\n");
  }

  if (room->exits[UP])
  {
    exitFlag |= EXIT_UP_FLAG;

    displayColorString("   &+YU&+L.&n &+wEdit up exit\n");
  }

  if (room->exits[DOWN])
  {
    exitFlag |= EXIT_DOWN_FLAG;

    displayColorString("   &+YD&+L.&n &+wEdit down exit\n");
  }

  if (exitFlag == 0)
  {
    displayColorString("   &nThis room has no exits.\n");
  }

  *exitFlags = exitFlag;


 // display the rest of the stuff

  displayColorString(
"\n"
"   &+YY&+L.&n &+wDelete exit\n"
"   &+YZ&+L.&n &+wCreate exit\n"
"\n"
MENU_COMMON
"\n");

  displayColorString(getMenuPromptName());
}


//
// interpEditRoomExitsMenu : Interprets user input - returns TRUE if user
//                           hit 'Q', FALSE otherwise
//
//          ch : user input
//       *room : room record associated with extra descs being edited
//  *exitFlags : "exit flags" - specifies which exits are editable
//

char interpEditRoomExitsMenu(usint ch, dikuRoom *room, ulong *exitFlags)
{
  struct rccoord coords;


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  if ((ch == 'Y') && (*exitFlags))
  {
    coords = _gettextposition();

    _settextposition(coords.row - 1, 1);

    deleteExitPromptPrompt(room, getExitAvailFlags(room));

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

  if ((ch == 'Z') && (*exitFlags != EXIT_ALL_EXITS_FLAG))
  {
    coords = _gettextposition();

    _settextposition(coords.row - 1, 1);

    createRoomExitPrompt(room, getExitAvailFlags(room));

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit north exit

  if ((ch == 'N') && ((*exitFlags) & EXIT_NORTH_FLAG))
  {
    editExit(room, &(room->exits[NORTH]), "north", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit northwest exit

  if ((ch == '1') && ((*exitFlags) & EXIT_NORTHWEST_FLAG))
  {
    editExit(room, &(room->exits[NORTHWEST]), "northwest", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit northeast exit

  if ((ch == '2') && ((*exitFlags) & EXIT_NORTHEAST_FLAG))
  {
    editExit(room, &(room->exits[NORTHEAST]), "northeast", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit south exit

  if ((ch == 'S') && ((*exitFlags) & EXIT_SOUTH_FLAG))
  {
    editExit(room, &(room->exits[SOUTH]), "south", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit southwest exit

  if ((ch == '3') && ((*exitFlags) & EXIT_SOUTHWEST_FLAG))
  {
    editExit(room, &(room->exits[SOUTHWEST]), "southwest", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit southeast exit

  if ((ch == '4') && ((*exitFlags) & EXIT_SOUTHEAST_FLAG))
  {
    editExit(room, &(room->exits[SOUTHEAST]), "southeast", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit west exit

  if ((ch == 'W') && ((*exitFlags) & EXIT_WEST_FLAG))
  {
    editExit(room, &(room->exits[WEST]), "west", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit east exit

  if ((ch == 'E') && ((*exitFlags) & EXIT_EAST_FLAG))
  {
    editExit(room, &(room->exits[EAST]), "east", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit up exit

  if ((ch == 'U') && ((*exitFlags) & EXIT_UP_FLAG))
  {
    editExit(room, &(room->exits[UP]), "up", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }
  else

 // edit down exit

  if ((ch == 'D') && ((*exitFlags) & EXIT_DOWN_FLAG))
  {
    editExit(room, &(room->exits[DOWN]), "down", FALSE);

    displayEditRoomExitsMenu(room, exitFlags);
  }


  return FALSE;
}


//
// editRoomExits : The "main" function for the edit room exits menu -
//                 lists all the exits, etc.
//
//   *room : room being edited
//

void editRoomExits(dikuRoom *room)
{
  usint ch;
  ulong exitFlags;

  dikuRoom *newRoom;


  newRoom = copyRoomInfo(room, FALSE, FALSE);
  if (!newRoom)
  {
    cout << "\n\nError: editRoomExits() ran out of memory allocating memory "
            "for new room info.  Aborting..\n\n";

    return;
  }

  displayEditRoomExitsMenu(newRoom, &exitFlags);


  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
//    if (ch == 'X')  // user chose to abort changes
    {
      deleteRoomInfo(newRoom, FALSE, FALSE, FALSE);

      _outtext("\n\n");

      break;
    }

    if (interpEditRoomExitsMenu(ch, newRoom, &exitFlags))
    {
      if (!compareRoomInfo(room, newRoom))
      {
       // since only the exits can be modified, this code does the job (though
       // it is a bit inefficient)

        deleteRoomAssocLists(room, FALSE, FALSE);

        memcpy(room, newRoom, sizeof(dikuRoom));

        delete newRoom;

        madeChanges = TRUE;
      }
      else deleteRoomInfo(newRoom, FALSE, FALSE, FALSE);

      _outtext("\n\n");

      return;
    }
  }
}
