//
//  File: editrsec.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing a room's sector type
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "editrsec.h"
#include "room.h"

#include "../misc/menu.h"

//
// displayEditRoomSectorMenu : Displays the menu for the edit room sector menu
//
//     *room : room being edited
//

// I was gonna someday hack the generic menu flag-editing/display stuff to
// work for an arbitrary enumerated list, but it never happened..  bummer
// eh

void displayEditRoomSectorMenu(const dikuRoom *room)
{
  char strn[256], newName[256];
  struct rccoord xy;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixString(room->roomName, newName, MAX_ROOMSHORT_LEN - 5, !getShowColorVal());

  sprintf(strn, "&+gEditing sector type for room #&+c%u&+w, &+L\"&n%s&+L\"&n"
                "\n\n",
          room->roomNumber, newName);

  displayColorString(strn);


  displayColorString(
"   &+YA&+L.&n &+wInside                  "
"&+YO&+L.&n &+wUnderworld - city&n\n"

"   &+YB&+L.&n &+wCity                    "
"&+YP&+L.&n &+wUnderworld - inside&n\n"

"   &+YC&+L.&n &+wField                   "
"&+YQ&+L.&n &+wUnderworld - water&n\n"

"   &+YD&+L.&n &+wForest                  "
"&+YR&+L.&n &+wUnderworld - no swim&n\n"

"   &+YE&+L.&n &+wHills                   "
"&+YS&+L.&n &+wUnderworld - no ground&n\n"

"   &+YF&+L.&n &+wMountain                "
"&+YT&+L.&n &+wPlane of air&n\n"

"   &+YG&+L.&n &+wWater (swimmable)       "
"&+YU&+L.&n &+wPlane of water&n\n"

"   &+YH&+L.&n &+wWater (req boat)        "
"&+YV&+L.&n &+wPlane of earth&n\n"

"   &+YI&+L.&n &+wNo ground (midair)      "
"&+YW&+L.&n &+wEthereal plane&n\n"

"   &+YJ&+L.&n &+wUnderwater              "
"&+YX&+L.&n &+wAstral plane&n\n"

"   &+YK&+L.&n &+wUnderwater - ground     "
"&+YY&+L.&n &+wDesert&n\n"

"   &+YL&+L.&n &+wPlane of fire           "
"&+YZ&+L.&n &+wArctic&n\n"

"   &+YM&+L.&n &+wOcean                   "
"&+Y1&+L.&n &+wSwamp&n\n"

"   &+YN&+L.&n &+wUnderworld - wild&n\n"

"\n"
MENU_COMMON
"\n");

  displayColorString(getMenuPromptName());

  xy = _gettextposition();

  if ((room->sectorType >= EDITR_SECT_COL2_START) &&
      (room->sectorType <= SECT_HIGHEST))
  {
    _settextposition(EDITR_SECT_HEADER_OFFSETY +
                     (room->sectorType - EDITR_SECT_COL2_START),
                     EDITR_SECT_COL2_FLAGX);

    displayColorString(SECT_ACTIVE_STR);
  }
  else
  {
    _settextposition(EDITR_SECT_HEADER_OFFSETY + room->sectorType,
                     EDITR_SECT_COL1_FLAGX);

    displayColorString(SECT_ACTIVE_STR);
  }


  _settextposition(xy.row, xy.col);
}


//
// interpEditRoomSectorMenu : Interprets user input for edit room sector menu -
//                            returns TRUE if the user hit 'Q'
//
//       ch : user input
//    *room : room being edited
//

char interpEditRoomSectorMenu(usint ch, dikuRoom *room)
{
  struct rccoord xy = _gettextposition();
  uchar oldSectType = room->sectorType,
        oldX, oldY;


  if (checkMenuKey(ch, TRUE) == MENUKEY_SAVE) return TRUE;

 // if character is '1' or higher (but less than 'A') add enough to push
 // it just past 'Z' (handy eh)

  if ((ch >= '1') && (ch < 'A')) ch += 42;  // ('Z' - '1') + 1

  if ((ch >= 'A') && (ch <= ('A' + SECT_HIGHEST)))
  {
   // run through some hoops to make figuring out the new sector type quick
   // n easy

    ch -= 'A';

    room->sectorType = ch;

   // update the display

    if (oldSectType >= EDITR_SECT_COL2_START)
    {
      oldX = EDITR_SECT_COL2_FLAGX;
      oldY = oldSectType + EDITR_SECT_HEADER_OFFSETY - EDITR_SECT_COL2_START;
    }
    else
    {
      oldX = EDITR_SECT_COL1_FLAGX;
      oldY = oldSectType + EDITR_SECT_HEADER_OFFSETY - EDITR_SECT_COL1_START;
    }

    if (ch >= EDITR_SECT_COL2_START)
    {
     // erase old type

      _settextposition(oldY, oldX);

      _outtext(" ");

     // draw new type

      _settextposition(ch + EDITR_SECT_HEADER_OFFSETY -
                       EDITR_SECT_COL2_START,
                       EDITR_SECT_COL2_FLAGX);

      displayColorString(SECT_ACTIVE_STR);

      _settextposition(xy.row, xy.col);
    }
    else
    {
     // erase old type

      _settextposition(oldY, oldX);

      _outtext(" ");

     // draw new type

      _settextposition(ch + EDITR_SECT_HEADER_OFFSETY -
                       EDITR_SECT_COL1_START,
                       EDITR_SECT_COL1_FLAGX);

      displayColorString(SECT_ACTIVE_STR);

      _settextposition(xy.row, xy.col);
    }
  }

  return FALSE;
}


//
// editRoomSector : "main" function for editing room sector type
//
//     *room : room record to edit
//

void editRoomSector(dikuRoom *room)
{
  usint ch;
  ulong oldSectorType = room->sectorType;


 // display the menu

  displayEditRoomSectorMenu(room);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, TRUE) == MENUKEY_ABORT)
    {
      room->sectorType = oldSectorType;

      _outtext("\n\n");

      return;
    }

    if (interpEditRoomSectorMenu(ch, room)) return;
  }
}
