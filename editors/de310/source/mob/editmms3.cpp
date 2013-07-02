//
//  File: editmms3.cpp   originally part of dikuEdit
//
//  Usage: functions for user-editing of misc mob info (thac0, money, hp, dam)
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "mob.h"

//
// displayEditMobMisc3Menu : Displays the edit mob miscellany menu
//
//    *mob : mob being edited
//

void displayEditMobMisc3Menu(const dikuMob *mob)
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
"&+WNOTE:&n All of these values are automatically set when the mob loads into\n"
"      the mud.  Thus, be left alone.\n"
"\n"
"   &+YA&+L.&n &+wMob copper                     %u&n\n"
"   &+YB&+L.&n &+wMob silver                     %u&n\n"
"   &+YC&+L.&n &+wMob gold                       %u&n\n"
"   &+YD&+L.&n &+wMob platinum                   %u&n\n"
"\n"
"   &+YE&+L.&n &+wMob experience                 %u&n\n"
"   &+YF&+L.&n &+wMob THAC0                      %d&n\n"
"   &+YG&+L.&n &+wMob armor class                %d&n\n"
"\n"
"   &+YH&+L.&n &+wMob hit points                 %s&n\n"
"   &+YI&+L.&n &+wMob damage                     %s&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  mob->copper, mob->silver, mob->gold, mob->platinum,
                  mob->exp, mob->thac0, mob->ac,
                  mob->hitPoints, mob->mobDamage, getMenuPromptName());

  displayColorString(outStrn);
}


//
// interpEditMobMisc3Menu : Interprets user input for edit mob flags menu -
//                          returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *mob : mob to edit
//

char interpEditMobMisc3Menu(usint ch, dikuMob *mob)
{
  struct rccoord coords;
  char strn[256];



 // mob copper

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew amount of copper on mob: &n");

      utoa(mob->copper, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->copper = strtoul(strn, NULL, 10);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob silver

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew amount of silver on mob: &n");

      utoa(mob->silver, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->silver = strtoul(strn, NULL, 10);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob gold

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew amount of gold on mob: &n");

      utoa(mob->gold, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->gold = strtoul(strn, NULL, 10);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob platinum

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew amount of platinum on mob: &n");

      utoa(mob->platinum, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->platinum = strtoul(strn, NULL, 10);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob experience

  if (ch == 'E')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob experience: &n");

      utoa(mob->exp, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->exp = strtoul(strn, NULL, 10);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob THAC0

  if (ch == 'F')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob THAC0: &n");

      itoa(mob->thac0, strn, 10);

      getStrn(strn, 4, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->thac0 = atoi(strn);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob armor class

  if (ch == 'G')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew mob armor class: &n");

      itoa(mob->ac, strn, 10);

      getStrn(strn, 4, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    mob->ac = atoi(strn);

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob hitpoints

  if (ch == 'H')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      clrline(coords.row);

      displayColorString("&+CNew value for mob hitpoints: &n");

      getStrn(strn, MAX_MOBHP_LEN - 1, 1, 7, '°', mob->hitPoints, FALSE, FALSE);
      remTrailingSpaces(strn);
      remLeadingSpaces(strn);

      if (checkDieStrnValidityShort(strn)) break;
    }

    strcpy(mob->hitPoints, lowstrn(strn));  // gots ta have lowercase 'd's

    displayEditMobMisc3Menu(mob);
  }
  else

 // mob damage

  if (ch == 'I')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      clrline(coords.row);

      displayColorString("&+CNew value for mob damage: &n");
      remTrailingSpaces(strn);
      remLeadingSpaces(strn);

      getStrn(strn, MAX_MOBDAM_LEN - 1, 1, 7, '°', mob->mobDamage, FALSE, FALSE);

      if (checkDieStrnValidityShort(strn)) break;
    }

    strcpy(mob->mobDamage, lowstrn(strn));

    displayEditMobMisc3Menu(mob);
  }
  else

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;


  return FALSE;
}


//
// editMobMisc3 : Edit mob misc #3 - "main" function
//
//   *mob : mob to edit
//

void editMobMisc3(dikuMob *mob)
{
  usint ch;
  ulong oldC = mob->copper, oldS = mob->silver, oldG = mob->gold,
        oldP = mob->platinum;

  long oldTHAC0 = mob->thac0;
  long oldAC = mob->ac;

  char oldHP[MAX_MOBHP_LEN];
  char oldDmg[MAX_MOBDAM_LEN];



  strcpy(oldHP, mob->hitPoints);
  strcpy(oldDmg, mob->mobDamage);

  displayEditMobMisc3Menu(mob);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      mob->copper = oldC;
      mob->silver = oldS;
      mob->gold = oldG;
      mob->platinum = oldP;

      mob->thac0 = oldTHAC0;
      mob->ac = oldAC;

      strcpy(mob->hitPoints, oldHP);
      strcpy(mob->mobDamage, oldDmg);

      _outtext("\n\n");

      return;
    }

    if (interpEditMobMisc3Menu(ch, mob)) return;
  }
}
