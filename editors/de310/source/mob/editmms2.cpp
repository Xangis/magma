//
//  File: editmms2.cpp   originally part of dikuEdit
//
//  Usage: functions for editing misc mob info (level, pos, sex)
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "mob/editmmsc.h"
#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "mob.h"

//
// displayEditMobMisc2Menu : Displays the edit mob miscellany menu
//
//    *mob : mob being edited
//

void displayEditMobMisc2Menu(const dikuMob *mob)
{
  char strn[256], newName[256], outStrn[800];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing miscellany for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wMob level                      %u\n"
"   &+YB&+L.&n &+wMob position                   %d (%s)\n"
"   &+YC&+L.&n &+wMob default position           %d (%s)\n"
"\n"
"   &+YD&+L.&n &+wMob sex                        %d (%s)\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  mob->level,
                  mob->position, getMobPosStrn(mob->position),
                  mob->defaultPos, getMobPosStrn(mob->defaultPos),
                  mob->sex, getMobSexStrn(mob->sex), getMenuPromptName());

  displayColorString(outStrn);
}


//
// interpEditMobMisc2Menu : Interprets user input for edit mob flags menu -
//                          returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *mob : mob to edit
//

char interpEditMobMisc2Menu(usint ch, dikuMob *mob)
{
  struct rccoord coords;
  char strn[256];


 // mob level

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob level (1-60): &n");

      itoa(mob->level, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->level = atoi(strn);

    displayEditMobMisc2Menu(mob);
  }
  else

 // mob position

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob position (? for help): &n");

      itoa(mob->position, strn, 10);
      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayMobPositionList();
      else break;  // later add some sorta range checking or sump'n
    }

    mob->position = atoi(strn);

    displayEditMobMisc2Menu(mob);
  }
  else

 // mob default position

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob default position (? for help): &n");

      itoa(mob->defaultPos, strn, 10);
      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayMobPositionList();
      else break;  // later add some sorta range checking or sump'n
    }

    mob->defaultPos = atoi(strn);

    displayEditMobMisc2Menu(mob);
  }
  else

 // mob sex

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob sex (? for help): &n");

      itoa(mob->sex, strn, 10);

      getStrn(strn, 1, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayMobSexList();
      else break;  // range checking, blah blah
    }

    mob->sex = atoi(strn);

    displayEditMobMisc2Menu(mob);
  }
  else

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;


  return FALSE;
}


//
// editMobMisc2 : Edit mob misc #2 - "main" function
//
//   *mob : mob to edit
//

void editMobMisc2(dikuMob *mob)
{
  usint ch;
  char oldPos = mob->position, oldDefPos = mob->defaultPos,
       oldSex = mob->sex;

  uchar oldLevel = mob->level;



  displayEditMobMisc2Menu(mob);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      mob->level = oldLevel;
      mob->position = oldPos;
      mob->defaultPos = oldDefPos;
      mob->sex = oldSex;

      _outtext("\n\n");

      return;
    }

    if (interpEditMobMisc2Menu(ch, mob)) return;
  }
}
