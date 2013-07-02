//
//  File: editotrp.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing object trap info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"
#include "../const.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "object.h"

extern _CONST flagDef trapEffFlagDef[];

//
// displayEditObjTrapInfoMenu : Displays the edit obj trap menu
//
//    *obj : obj being edited
//

void displayEditObjTrapInfoMenu(const dikuObject *obj)
{
  char strn[256], newName[256], *outStrn = new char[2048];


  _clearscreen(7, 0);

  if (!outStrn)
  {
    _outtext("\n\nError allocating outStrn in displayblah()...\n\n");

    return;
  }

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing trap info for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wTrap affect flag (0 = no trap)    %u (%s)&n\n"
"\n"
"   &+YB&+L.&n &+wTrap damage type/effect           %d (%s)&n\n"
"   &+YC&+L.&n &+wNumber of charges (-1 = infinite) %d&n\n"
"\n"
"   &+YD&+L.&n &+wTrap level                        %u&n\n"
"   &+YE&+L.&n &+wPercentage chance of setting off  %d&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
                   obj->trapAff, getObjTrapAffStrn(obj->trapAff, newName),
                   obj->trapDam, getObjTrapDamStrn(obj->trapDam),
                   obj->trapCharge,
                   obj->trapLevel,
                   obj->trapPercent,

                   getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditObjTrapInfoMenu : Interprets user input for edit obj trap menu -
//                             returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *obj : obj to edit
//

char interpEditObjTrapInfoMenu(usint ch, dikuObject *obj)
{
  char strn[256] = "\0", newName[256];
  struct rccoord coords;

 // edit trap affect flag

  if (ch == 'A')
  {
    fixObjName(obj->objShortName, newName);

    sprintf(strn, "&+gEditing trap effect flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            obj->objNumber, newName);

    editFlags(trapEffFlagDef, &(obj->trapAff), strn,
              "object trap effect flags", NULL, 1);

    displayEditObjTrapInfoMenu(obj);
  }
  else

 // edit trap damage type

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew trap damage type (? for list):&n ");

      itoa(obj->trapDam, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayObjTrapDamList();
      else break;  // later add some sorta range checking or sump'n
    }

    obj->trapDam = atoi(strn);

    displayEditObjTrapInfoMenu(obj);
  }
  else

 // edit number of trap uses

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew number of trap uses:&n ");

      itoa(obj->trapCharge, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    obj->trapCharge = atoi(strn);

    displayEditObjTrapInfoMenu(obj);
  }
  else

 // edit trap level

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew trap level (1-100):&n ");

      itoa(obj->trapLevel, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    obj->trapLevel = atoi(strn);

    displayEditObjTrapInfoMenu(obj);
  }
  else
  if (ch == 'E')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew trap percent (1-100):&n ");

      itoa(obj->trapPercent, strn, 10);

      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

      break;
    }

    obj->trapPercent = atoi(strn);

    displayEditObjTrapInfoMenu(obj);
  }
  else

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editObjTrapInfo : Edit obj misc - "main" function
//
//   *obj : obj to edit
//

void editObjTrapInfo(dikuObject *obj)
{
  usint ch;
  ulong oldAff = obj->trapAff;
  long oldDam = obj->trapDam, oldCharge = obj->trapCharge;


  displayEditObjTrapInfoMenu(obj);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      obj->trapAff = oldAff;
      obj->trapDam = oldDam;
      obj->trapCharge = oldCharge;

      _outtext("\n\n");

      return;
    }

    if (interpEditObjTrapInfoMenu(ch, obj)) return;
  }
}
