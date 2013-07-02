//
//  File: editomsc.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing misc. object info
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

#include "object.h"

//
// displayEditObjMiscMenu : Displays the edit obj miscellany menu
//
//    *obj : obj being edited
//

void displayEditObjMiscMenu(const dikuObject *obj)
{
  char strn[256], newName[256], *outStrn = new char[2048],
       vstrn1[256], vstrn2[256], vstrn3[256], vstrn4[256],
       vstrn5[256], vstrn6[256], vstrn7[256], vstrn8[256],
       v2strn1[256], v2strn2[256], v2strn3[256], v2strn4[256],
       v2strn5[256], v2strn6[256], v2strn7[256], v2strn8[256];


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
"   &+YA&+L.&n &+wObject type                       %u (%s)&n\n"
"\n"
"   &+YB&+L.&n &+wObject value #1                   %d (%s)&n\n"
"   &+YC&+L.&n &+wObject value #2                   %d (%s)&n\n"
"   &+YD&+L.&n &+wObject value #3                   %d (%s)&n\n"
"   &+YE&+L.&n &+wObject value #4                   %d (%s)&n\n"
"   &+YF&+L.&n &+wObject value #5                   %d (%s)&n\n"
"   &+YG&+L.&n &+wObject value #6                   %d (%s)&n\n"
"   &+YH&+L.&n &+wObject value #7                   %d (%s)&n\n"
"   &+YI&+L.&n &+wObject value #8                   %d (%s)&n\n"
"\n"
"   &+YJ&+L.&n &+wObject apply #1 - what            %u (%s)&n\n"
"   &+YK&+L.&n &+wObject apply #1 - value           %d&n\n"
"\n"
"   &+YL&+L.&n &+wObject apply #2 - what            %u (%s)&n\n"
"   &+YM&+L.&n &+wObject apply #2 - value           %d&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
                   obj->objType, getObjTypeStrn(obj->objType),

                   obj->objValues[0],
                    getObjValueStrn(obj->objType, 0, obj->objValues[0],
                                    vstrn1, v2strn1, TRUE, obj),
                   obj->objValues[1],
                    getObjValueStrn(obj->objType, 1, obj->objValues[1],
                                    vstrn2, v2strn2, TRUE, obj),
                   obj->objValues[2],
                    getObjValueStrn(obj->objType, 2, obj->objValues[2],
                                    vstrn3, v2strn3, TRUE, obj),
                   obj->objValues[3],
                    getObjValueStrn(obj->objType, 3, obj->objValues[3],
                                    vstrn4, v2strn4, TRUE, obj),
                   obj->objValues[4],
                    getObjValueStrn(obj->objType, 4, obj->objValues[4],
                                    vstrn5, v2strn5, TRUE, obj),
                   obj->objValues[5],
                    getObjValueStrn(obj->objType, 5, obj->objValues[5],
                                    vstrn6, v2strn6, TRUE, obj),
                   obj->objValues[6],
                    getObjValueStrn(obj->objType, 6, obj->objValues[6],
                                    vstrn7, v2strn7, TRUE, obj),
                   obj->objValues[7],
                    getObjValueStrn(obj->objType, 7, obj->objValues[7],
                                    vstrn8, v2strn8, TRUE, obj),

                   obj->objApply[0].applyWhere,
                    getObjApplyStrn(obj->objApply[0].applyWhere),
                   obj->objApply[0].applyModifier,

                   obj->objApply[1].applyWhere,
                    getObjApplyStrn(obj->objApply[1].applyWhere),
                   obj->objApply[1].applyModifier,

                   getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// editObjValueField
//

void editObjValueField(dikuObject *obj, uchar valueField)
{
  struct rccoord coords;
  char strn[256],
       helpAvail = checkForValueList(obj->objType, valueField),
       verboseAvail = checkForVerboseAvail(obj->objType, valueField),
       searchAvail =
             (checkForSearchAvail(obj->objType, valueField) ? TRUE : FALSE);


  coords = _gettextposition();

  _settextposition(coords.row, 1);
  clrline(coords.row);

  if (!specialObjValEdit(obj, valueField, FALSE))
  {
    while (TRUE)
    {
      if (!(verboseAvail && searchAvail))
        sprintf(strn, "&+CNew value for object field #%u", valueField + 1);
      else
        sprintf(strn, "&+CNew val for obj field #%u", valueField + 1);

     // assume verbose/search are only available if help is available

      if (helpAvail)
      {
        strcat(strn, " (? for list");

//        if (verboseAvail) strcat(strn, ", ?? for full list");

        if (searchAvail) strcat(strn, ", $ to search");

        strcat(strn, "): &n");
      }
      else strcat(strn, ": &n");

      displayColorString(strn);

      itoa(obj->objValues[valueField], strn, 10);

      getStrn(strn, 8, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?") && helpAvail)
      {
        displayObjValueHelp(obj->objType, valueField, FALSE);
      }
      else

     // below assumes that verbose is only available when help is available

      if (!strcmp(strn, "??") && verboseAvail)
      {
        displayObjValueHelp(obj->objType, valueField, TRUE);
      }
      else

     // ditto for search

      if (!strcmp(strn, "$") && searchAvail)
      {
        searchObjValue(obj->objType, valueField);
      }
      else

      break;  // later add some sorta range checking or sump'n
    }

    obj->objValues[valueField] = atoi(strn);
  }
  else specialObjValEdit(obj, valueField, TRUE);
}


//
// editObjApplyType
//

void editObjApplyType(dikuObject *obj, uchar applyField)
{
  struct rccoord coords;
  char strn[128];


  coords = _gettextposition();

  _settextposition(coords.row, 1);
  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn, "&+CNew type for object apply #%u (? for list): &n",
            applyField + 1);

    displayColorString(strn);

    itoa(obj->objApply[applyField].applyWhere, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    if (!strcmp(strn, "?")) displayObjApplyTypeList();
    else break;  // later add some sorta range checking or sump'n
  }

  obj->objApply[applyField].applyWhere = atoi(strn);
}


//
// editObjApplyValue
//

void editObjApplyValue(dikuObject *obj, uchar applyField)
{
  struct rccoord coords;
  char strn[128];


  coords = _gettextposition();

  _settextposition(coords.row, 1);
  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn, "&+CNew value for object apply #%u: &n", applyField + 1);
    displayColorString(strn);

    itoa(obj->objApply[applyField].applyModifier, strn, 10);

    getStrn(strn, 5, 1, 7, '°', strn, FALSE, FALSE);

    break;  // later add some sorta range checking or sump'n
  }

  obj->objApply[applyField].applyModifier = atoi(strn);
}


//
// interpEditObjMiscMenu : Interprets user input for edit obj flags menu -
//                         returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *obj : obj to edit
//

char interpEditObjMiscMenu(usint ch, dikuObject *obj)
{
  char strn[256] = "\0";
  struct rccoord coords;


 // edit object type

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      if (checkForObjHeresWithLoadedContainer(obj->objNumber))
      {
        displayColorString(
"&+MCannot change type - objects exist of this type with items inside - press a key&n");

        getkey();

        displayEditObjMiscMenu(obj);

        return FALSE;
      }

      displayColorString("&+CNew object type (? for list): &n");

      itoa(obj->objType, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayObjTypeList();
      else break;
    }

    obj->objType = atoi(strn);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 0

  if (ch == 'B')
  {
    editObjValueField(obj, 0);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 1

  if (ch == 'C')
  {
    editObjValueField(obj, 1);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 2

  if (ch == 'D')
  {
    editObjValueField(obj, 2);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 3

  if (ch == 'E')
  {
    editObjValueField(obj, 3);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 4

  if (ch == 'F')
  {
    editObjValueField(obj, 4);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 5

  if (ch == 'G')
  {
    editObjValueField(obj, 5);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 6

  if (ch == 'H')
  {
    editObjValueField(obj, 6);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit object value 7

  if (ch == 'I')
  {
    editObjValueField(obj, 7);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit apply "type" 0

  if (ch == 'J')
  {
    editObjApplyType(obj, 0);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit apply modifier 0

  if (ch == 'K')
  {
    editObjApplyValue(obj, 0);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit apply "type" 1

  if (ch == 'L')
  {
    editObjApplyType(obj, 1);

    displayEditObjMiscMenu(obj);
  }
  else

 // edit apply modifier 1

  if (ch == 'M')
  {
    editObjApplyValue(obj, 1);

    displayEditObjMiscMenu(obj);
  }
  else if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editObjMisc : Edit obj misc - "main" function
//
//   *obj : obj to edit
//

void editObjMisc(dikuObject *obj)
{
  usint ch;
  uchar oldType = obj->objType;
  long oldValues[NUMB_OBJ_VALS];
  objApplyRec oldApplies[NUMB_OBJ_APPLIES];

 // back up the old stuff

  memcpy(oldValues, obj->objValues, sizeof(long) * NUMB_OBJ_VALS);
  memcpy(oldApplies, obj->objApply, sizeof(objApplyRec) * NUMB_OBJ_APPLIES);

  displayEditObjMiscMenu(obj);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      obj->objType = oldType;
      memcpy(obj->objValues, oldValues, sizeof(long) * NUMB_OBJ_VALS);
      memcpy(obj->objApply, oldApplies, sizeof(objApplyRec) * NUMB_OBJ_APPLIES);

      _outtext("\n\n");

      return;
    }

    if (interpEditObjMiscMenu(ch, obj)) return;
  }
}
