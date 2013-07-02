//
//  File: editoms2.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing misc object info
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

//
// displayEditObjMisc2Menu : Displays the edit obj miscellany menu
//
//    *obj : obj being edited
//

void displayEditObjMisc2Menu(const dikuObject *obj)
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

  sprintf(strn, "&+gEditing miscellany for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wObject material                   %u (%s)&n\n"
"   &+YB&+L.&n &+wObject weight                     %d&n\n"
"   &+YC&+L.&n &+wObject craftsmanship              %d (%s)&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
//
//                   obj->size, getObjSizeStrnShort(obj->size),
//                   obj->material, getMaterialStrn(obj->material),
//                   obj->craftsmanship,
//                   getObjCraftsmanshipStrn(obj->craftsmanship),
//                   obj->damResistBonus,
//
//                   obj->weight, obj->space, obj->value,
//                   getMoneyStrn(obj->value, newName), obj->condition,

                  obj->material, getMaterialStrn(obj->material), obj->weight,
                  obj->craftsmanship,
                  getObjCraftsmanshipStrn(obj->craftsmanship),

                  getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditObjMisc2Menu : Interprets user input for edit obj flags menu -
//                          returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *obj : obj to edit
//

char interpEditObjMisc2Menu(usint ch, dikuObject *obj)
{
  char strn[256] = "\0";
  struct rccoord coords;

// edit object size

//  if (ch == 'A')
//  {
//    coords = _gettextposition();
//
//    _settextposition(coords.row, 1);
//    clrline(coords.row);
//
//    while (TRUE)
//    {
//      displayColorString("&+CNew object size (? for list): &n");
//
//      itoa(obj->size, strn, 10);
//
//      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);
//
//      if (!strcmp(strn, "?")) displayObjSizeList();
//      else break;
//    }
//
//    obj->size = atoi(strn);
//
//    displayEditObjMisc2Menu(obj);
//  }
//  else

// edit object material

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew object material (? for list): &n");

      itoa(obj->material, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayMaterialList();
      else break;
    }

    obj->material = atoi(strn);

    displayEditObjMisc2Menu(obj);
  }
  else

// edit object craftsmanship

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew object craftsmanship (? for list): &n");

      itoa(obj->craftsmanship, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayCraftsmanshipList();
      else break;
    }

    obj->craftsmanship = atoi(strn);

    displayEditObjMisc2Menu(obj);
  }
  else

// edit object enchantment level
//
//  if (ch == 'D')
//  {
//    coords = _gettextposition();
//
//    _settextposition(coords.row, 1);
//    clrline(coords.row);
//
//    while (TRUE)
//    {
//      displayColorString("&+CNew object damage resistance bonus (? for help): &n");
//
//      itoa(obj->damResistBonus, strn, 10);
//
//      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);
//
//      if (!strcmp(strn, "?"))
//      {
//        _setbkcolor(0);
//        _outtext("\n\n"
//"This value is used to keep track of the resistance that the 'resist damage'\n"
//"spell imbues upon an object.  Levels no higher than 100 should be used.\n\n");
//      }
//      else break;
//    }
//
//    obj->damResistBonus = atoi(strn);
//
//    displayEditObjMisc2Menu(obj);
//  }

// object weight

//  if (ch == 'E')

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew object weight: &n");

      itoa(obj->weight, strn, 10);

      getStrn(strn, 5, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    obj->weight = atoi(strn);

    displayEditObjMisc2Menu(obj);
  }

// object volume (space)
//
//  if (ch == 'F')
//  {
//    coords = _gettextposition();
//
//    _settextposition(coords.row, 1);
//    clrline(coords.row);
//
//    while (TRUE)
//    {
//      displayColorString("&+CNew object volume (? for help): &n");
//
//      itoa(obj->space, strn, 10);
//
//      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);
//
//      if (!strcmp(strn, "?"))
//      {
//        _setbkcolor(0);
//        _outtext("\n\n"
//"   This value is not currently used, but it will specify the amount of\n"
//"   space a particular object takes up.  For now, it should be left at 0.\n\n");
//      }
//      else break;  // range checking, blah blah
//    }
//
//    obj->space = atoi(strn);
//
//    displayEditObjMisc2Menu(obj);
//  }
//  else

// object worth

//  if (ch == 'B')
//  {
//    coords = _gettextposition();
//
//    _settextposition(coords.row, 1);
//    clrline(coords.row);
//
//    while (TRUE)
//    {
//      displayColorString("&+CNew object value (in copper): &n");
//
//      itoa(obj->value, strn, 10);
//
//      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);
//
//      break;  // range checking, blah blah
//    }
//
//    obj->value = atoi(strn);
//
//    displayEditObjMisc2Menu(obj);
//  }
//
// object condition
//  if (ch == 'H')
//  {
//    coords = _gettextposition();
//
//    _settextposition(coords.row, 1);
//    clrline(coords.row);
//
//    while (TRUE)
//    {
//      displayColorString("&+CNew value for object condition (1-100%, ? for help): &n");
//
//      itoa(obj->condition, strn, 10);
//
//      getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);
//
//      if (!strcmp(strn, "?"))
//      {
//        _setbkcolor(0);
//        _outtext("\n\n"
//"   This value is used to determine how 'tough' an object is when it first\n"
//"   loads into the mud.  This value is set automatically by the mud when the\n"
//"   object loads, and should generally be left at default - however, if you\n"
//"   want the object to load damaged, then set the condition to some percentage\n"
//"   from 1-99%.  The object's SP will be set upon loading into the mud\n"
//"   automatically.\n\n");
//      }
//      else break;
//    }
//
//    if (strlen(strn) &&
//        (strn[strlen(strn) - 1] == '%')) strn[strlen(strn) - 1] = '\0';
//
//    obj->condition = atoi(strn);
//
//    displayEditObjMisc2Menu(obj);
//  }

  else if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editObjMisc2 : Edit obj misc - "main" function
//
//   *obj : obj to edit
//

void editObjMisc2(dikuObject *obj)
{
  usint ch;
  ulong oldMat = obj->material, oldSize = obj->size,
        oldCraft = obj->craftsmanship, oldDam = obj->damResistBonus,
        oldValue = obj->value, oldCond = obj->condition, oldVol = obj->space;
  long oldWeight = obj->weight;

 // back up the old stuff

  displayEditObjMisc2Menu(obj);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      obj->material = oldMat;
      obj->size = oldSize;
      obj->craftsmanship = oldCraft;
      obj->damResistBonus = oldDam;
      obj->weight = oldWeight;
      obj->value = oldValue;
      obj->condition = oldCond;
      obj->space = oldVol;

      _outtext("\n\n");

      return;
    }

    if (interpEditObjMisc2Menu(ch, obj)) return;
  }
}
