//
//  File: editrmsc.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing miscellaneous room info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "room.h"
#include "../misc/menu.h"

//
// displayEditRoomMiscMenu : Displays the edit room miscellany menu
//
//    *room : room being edited
//

void displayEditRoomMiscMenu(const dikuRoom *room)
{
  char strn[256], newName[256], *outStrn = new char[2048];


  _clearscreen(7, 0);

  if (!outStrn)
  {
    _outtext("\n\nError allocating outStrn in displayEditRoomMiscMenu()...\n\n");

    return;
  }

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomName(room->roomName, newName);

  sprintf(strn, "&+gEditing miscellany for room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          room->roomNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wFall percentage chance            %u%%&n\n\n"
"   &+YB&+L.&n &+wMana flag                         %d (%s)&n\n"
"   &+YC&+L.&n &+wMana value                        %d&n\n\n"
"   &+YD&+L.&n &+wCurrent strength                  %d&n\n"
"   &+YE&+L.&n &+wCurrent direction                 %d (%s)&n\n"
"\n"
MENU_COMMON
"\n"
"%s",              room->fallChance,
                   room->manaFlag, getManaFlagStrn(room->manaFlag),
                   room->manaApply,
                   room->current, room->currentDir,
                   getExitStrn(room->currentDir),
                   getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditRoomMiscMenu : Interprets user input for edit room flags menu -
//                         returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *room : room to edit
//

char interpEditRoomMiscMenu(const usint ch, dikuRoom *room)
{
  char strn[256] = "\0";
  struct rccoord coords;


 // edit room fall chance

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew chance of falling: &n");

      itoa(room->fallChance, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    room->fallChance = atoi(strn);

    displayEditRoomMiscMenu(room);
  }
  else

 // edit room mana flag

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew mana flag (? for list): &n");

      itoa(room->manaFlag, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayRoomManaTypeList();
      else break;  // later add some sorta range checking or sump'n
    }

    room->manaFlag = atoi(strn);

    displayEditRoomMiscMenu(room);
  }
  else

 // edit room mana apply flag

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew mana apply value: &n");

      itoa(room->manaApply, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    room->manaApply = atoi(strn);

    displayEditRoomMiscMenu(room);
  }
  else

 // edit room current value

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew current value: &n");

      itoa(room->current, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    room->current = atoi(strn);

    displayEditRoomMiscMenu(room);
  }
  else

 // edit room current direction

  if (ch == 'E')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew current direction (? for list): &n");

      itoa(room->currentDir, strn, 10);

      getStrn(strn, 1, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayDirectionList();
      else break;  // later add some sorta range checking or sump'n
    }

    room->currentDir = atoi(strn);

    displayEditRoomMiscMenu(room);
  }
  else
  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;


  return FALSE;
}


//
// editRoomMisc : Edit room misc - "main" function
//
//   *room : room to edit
//

void editRoomMisc(dikuRoom *room)
{
  usint ch;
  uchar oldFallChance = room->fallChance;
  long oldManaApply = room->manaApply;
  uchar oldCurrent = room->current, oldCurrDir = room->currentDir;


  displayEditRoomMiscMenu(room);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      room->fallChance = oldFallChance;
      room->manaApply = oldManaApply;
      room->current = oldCurrent;
      room->currentDir = oldCurrDir;

      _outtext("\n\n");

      return;
    }

    if (interpEditRoomMiscMenu(ch, room)) return;
  }
}
