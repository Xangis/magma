//
//  File: editobj.cpp    originally part of dikuEdit
//
//  Usage: functions for editing object types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


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

extern dikuRoom *roomHead;
extern dikuObject **objLookup;
extern char madeChanges;
extern ulong lowestObjNumber, highestObjNumber, numbObjTypes, numbLookupEntries;

//
// displayEditObjTypeMenu : Displays edit object type menu
//
//   *obj : obj being edited
//

void displayEditObjTypeMenu(dikuObject *obj)
{
  char strn[512], newName[256], *outStrn = new char[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&n&+gEditing object type #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  " &+YA/B&+L.&n &+wEdit object short/long name&n\n"
  "   &+YC&+L.&n &+wEdit object keywords &+c(%s)&n\n"
  " &+YD/E&+L.&n &+wEdit object extra/extra2 flags &+c(%u/%u)&n\n"
  "   &+YF&+L.&n &+wEdit object wear flags &+c(%u)&n\n"
  " &+YG/H&+L.&n &+wEdit object anti/anti2 flags &+c(%u/%u)&n\n"
  "   &+YI&+L.&n &+wEdit type &+c(%s)&n, values &+c(%d %d %d %d %d %d %d %d)&n,\n"
  "      and applies &+c(%s %d, %s %d)&n\n"
  "   &+YJ&+L.&n &+wEdit material &+c(%s&+c)&n, weight &+c(%d)&n, craftsmanship &+c(%d)&n\n"
  "   &+YK&+L.&n &+wEdit object extra descs &+c(%u)&n\n"
  "   &+YM&+L.&n &+wEdit object trap info &+c(%s&+c %s&+c [%d charge%s, level %u])&n\n",

    getReadableKeywordStrn(obj->keywordListHead, strn),

    obj->extraBits.longIntFlags,
    obj->extra2Bits.longIntFlags,
    obj->wearBits.longIntFlags,
    obj->antiBits.longIntFlags,
    obj->anti2Bits.longIntFlags,

    getObjTypeStrnShort(obj->objType),
    obj->objValues[0], obj->objValues[1], obj->objValues[2],
    obj->objValues[3], obj->objValues[4], obj->objValues[5],
    obj->objValues[6], obj->objValues[7],
      getObjApplyStrnShort(obj->objApply[0].applyWhere),
      obj->objApply[0].applyModifier,
      getObjApplyStrnShort(obj->objApply[1].applyWhere),
      obj->objApply[1].applyModifier,

    getMaterialStrn(obj->material),
    obj->weight,
    obj->craftsmanship,

    getNumbExtraDescs(obj->extraDescHead),

    getObjTrapAffStrn(obj->trapAff, newName), getObjTrapDamStrn(obj->trapDam),
      obj->trapCharge, plural(obj->trapCharge), obj->trapLevel);


    if (getObjAffectVal())
    {
      sprintf(strn,
  " &+YN/O&+L.&n &+wEdit object affect1/affect2 flags &+c(%u/%u)&n\n",
            obj->affect1Bits.longIntFlags,
            obj->affect2Bits.longIntFlags);

      strcat(outStrn, strn);

      sprintf(strn,
  " &+YP/Q&+L.&n &+wEdit object affect3/affect4 flags &+c(%u/%u)&n\n",
            obj->affect3Bits.longIntFlags,
            obj->affect4Bits.longIntFlags);

      strcat(outStrn, strn);
    }

    sprintf(strn,
  "\n"
  "   &+YL&+L.&n &+wSet limit of loads for all objs of type &+c(%u/%u)&n\n"
  "   &+YR&+L.&n &+wSet mud-wide object load limit &+c(%u) (0=no limit)&n\n"
  "   &+YV&+L.&n &+wChange object's vnum&n\n"
  "\n"
  MENU_COMMON
//  "\n"
  "\n", getNumbEntities(ENTITY_OBJECT, obj->objNumber, FALSE),
        obj->maxload, obj->globalmaxload );

    strcat(outStrn, strn);

    strcat(outStrn, getMenuPromptName());
  }
  else  // no extra info
  {
    strcpy(outStrn,
  " &+YA/B&+L.&n &+wEdit object short/long name&n\n"
  "   &+YC&+L.&n &+wEdit object keywords&n\n"
  "   &+YD&+L.&n &+wEdit object extra flags&n\n"
  "   &+YE&+L.&n &+wEdit object extra2 flags&n\n"
  "   &+YF&+L.&n &+wEdit object wear flags&n\n"
  "   &+YG&+L.&n &+wEdit object anti flags&n\n"
  "   &+YH&+L.&n &+wEdit object anti2 flags&n\n"
  "   &+YI&+L.&n &+wEdit object type, values, and applies\n"
  "   &+YJ&+L.&n &+wEdit object material, weight, craftsmanship&n\n"
  "   &+YK&+L.&n &+wEdit object extra descs\n"
  "   &+YM&+L.&n &+wEdit object trap info\n");

    if (getObjAffectVal())
    {
    strcat(outStrn,
  "   &+YN&+L.&n &+wEdit object affect1 flags&n\n"
  "   &+YO&+L.&n &+wEdit object affect2 flags&n\n");
    strcat(outStrn,
  "   &+YP&+L.&n &+wEdit object affect3 flags&n\n"
  "   &+YQ&+L.&n &+wEdit object affect4 flags&n\n");
    }

    strcat(outStrn,
  "\n"
  "   &+YL&+L.&n &+wSet limit of loads for all objs of type&n\n"
  "   &+YV&+L.&n &+wChange object's vnum&n\n"
  "\n"
  MENU_COMMON
//  "\n"
  "\n");

    strcat(outStrn, getMenuPromptName());
  }

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditObjTypeMenu : Interprets user input for edit object type menu -
//                         returns TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *obj : object to edit
//

char interpEditObjTypeMenu(usint ch, dikuObject *obj, dikuObject *origObj,
                           char *addedIdentKeyword)
{
  struct rccoord coords;
  char strn[256], numbStrn[32], newName[256];
  stringNode *strnN;
  ulong i;


 // edit obj short name

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+CNew object short name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(obj->objShortName, MAX_OBJSNAME_LEN - 1 - 1 - 1, 1, 7, '°',
            obj->objShortName, FALSE, FALSE);

    remTrailingSpaces(obj->objShortName);
    remLeadingSpaces(obj->objShortName);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj long name

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+CNew object long name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(obj->objLongName, MAX_OBJLNAME_LEN - 1 - 1 - 1, 1, 7, '°',
            obj->objLongName, FALSE, FALSE);

    remTrailingSpaces(obj->objLongName);
    remLeadingSpaces(obj->objLongName);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj keywords

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+CNew object keywords: &n");

    createKeywordString(obj->keywordListHead, strn);
    strn[strlen(strn) - 1] = '\0';  // get rid of tilde

    getStrn(strn, MAX_OBJKEY_LEN - 1, 1, 7, '°', strn, FALSE, FALSE);

    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    strcat(strn, "~");

    deleteStringNodes(obj->keywordListHead);
    obj->keywordListHead = createKeywordList(strn);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj extra flags

  if (ch == 'D')
  {
    editObjExtraFlags(obj, origObj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj extra2 flags

  if (ch == 'E')
  {
    editObjExtra2Flags(obj, origObj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj wear flags

  if (ch == 'F')
  {
    editObjWearFlags(obj, origObj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj anti flags

  if (ch == 'G')
  {
    editObjAntiFlags(obj, origObj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj anti2 flags

  if (ch == 'H')
  {
    editObjAnti2Flags(obj, origObj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj type, applies, values

  if (ch == 'I')
  {
    editObjMisc(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj weight, dam bonus, etc

  if (ch == 'J')
  {
    editObjMisc2(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj extra descs

  if (ch == 'K')
  {
    if (editObjExtraDesc(obj, addedIdentKeyword))
    {
     // user aborted, check for _id_ keyword and remove it

      if (*addedIdentKeyword)
      {
        strnN = obj->keywordListHead;
        while (strnN)
        {
          if (!strcmp(strnN->string, "_ID_"))
          {
            if (!strnN->Last) // head of list
            {
              obj->keywordListHead = obj->keywordListHead->Next;
              obj->keywordListHead->Last = NULL;

              delete strnN;
            }
            else
            {
              strnN->Last->Next = strnN->Next;
              strnN->Next->Last = strnN->Last;

              delete strnN;
            }
          }

          strnN = strnN->Next;
        }
      }
    }

    displayEditObjTypeMenu(obj);
  }
  else

 // edit obj trap info

  if (ch == 'M')
  {
    editObjTrapInfo(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // if allowed, edit aff1 flag

  if (getObjAffectVal() && (ch == 'N'))
  {
    editObjAffect1Flags(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // ditto for aff2 flag

  if (getObjAffectVal() && (ch == 'O'))
  {
    editObjAffect2Flags(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // ditto for aff3 flag

  if (getObjAffectVal() && (ch == 'P'))
  {
    editObjAffect3Flags(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // ditto for aff4 flag

  if (getObjAffectVal() && (ch == 'Q'))
  {
    editObjAffect4Flags(obj);

    displayEditObjTypeMenu(obj);
  }
  else

 // change limit

  if (ch == 'L')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString(
"&+CNew limit on loads for this object type (0 = no limit): &n");

    itoa(getNumbEntities(ENTITY_OBJECT, obj->objNumber, TRUE), numbStrn, 10);
//    strn[0] = '\0';

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strnumer(strn))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);
    if ((i <= getNumbEntities(ENTITY_OBJECT, obj->objNumber, FALSE)) && (i != 0))
    {
      _settextposition(coords.row, 1);
      clrline(coords.row, 7, 0);
      _setbkcolor(0);

      displayColorString(
"&+CError: Limit being set lower than or equal to the number loaded - press any key&n");

      getkey();

      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    setEntityOverride(ENTITY_OBJECT, obj->objNumber, i, obj->globalmaxload);
    obj->maxload = i;

    madeChanges = TRUE;

    displayEditObjTypeMenu(obj);
  }
  else
  if (ch == 'R')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString(
"&+CNew MUD-wide limit on loads for this object type (0 = no limit): &n");

    itoa(getNumbEntities(ENTITY_OBJECT, obj->objNumber, TRUE), numbStrn, 10);
//    strn[0] = '\0';

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strnumer(strn))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);
    if ((i <= getNumbEntities(ENTITY_OBJECT, obj->objNumber, FALSE)) && (i != 0))
    {
      _settextposition(coords.row, 1);
      clrline(coords.row, 7, 0);
      _setbkcolor(0);

      displayColorString(
"&+CError: Limit being set lower than or equal to the number loaded - press any key&n");

      getkey();

      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    setEntityOverride(ENTITY_OBJECT, obj->objNumber, obj->maxload, i);
    obj->globalmaxload = i;

    madeChanges = TRUE;

    displayEditObjTypeMenu(obj);
  }

  else

 // change vnum

  if ((ch == 'V') && !obj->defaultObj)
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    sprintf(strn, "&+YNew object vnum (highest allowed %u): &n",
            numbLookupEntries - 1);
    displayColorString(strn);

    itoa(obj->objNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if ((!strnumer(strn)) || (atoi(strn) >= numbLookupEntries) ||
        (findObj(atoi(strn))))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);

    resetAllObjHere(obj->objNumber, i, roomHead);
    resetNumbLoaded(ENTITY_OBJECT, obj->objNumber, i);

    checkAndFixRefstoObj(obj->objNumber, i);

    objLookup[i] = origObj;
    objLookup[obj->objNumber] = NULL;

    obj->objNumber = i;

    resetEntityPointersByNumb(TRUE, FALSE);

    _settextposition(1, 1);

    clrline(1, 0, 0);
    fixObjName(obj->objShortName, newName);

    _setbkcolor(0);
    sprintf(strn, "&n&+gEditing object type #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            obj->objNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editObjType : "main" function for edit object type menu
//
//  *obj : obj being edited
//

dikuObject *realEditObjType(dikuObject *obj, const char allowJump /*= TRUE*/)
{
  usint ch;
  dikuObject *newObj;
  char done = FALSE, addedIdentKeyword = FALSE;
  ulong numb, oldVnum;


  if (!obj) return NULL;  // ya never know

  oldVnum = obj->objNumber;

 // copy obj into newObj and display the menu

  newObj = copyObjectType(obj, FALSE);

  displayEditObjTypeMenu(newObj);

  while (TRUE)
  {
    ch = toupper(getkey());

#ifndef __UNIX__
    if (ch == '$')
    {
      displayFreeStack(1, 1, FALSE);
    }
#endif

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
     // if user changed vnum and aborted, set it back

      if (newObj->objNumber != oldVnum)
      {
        resetAllObjHere(newObj->objNumber, oldVnum, roomHead);
        resetNumbLoaded(ENTITY_OBJECT, newObj->objNumber, oldVnum);

        checkAndFixRefstoObj(newObj->objNumber, oldVnum);

        objLookup[oldVnum] = obj;
        objLookup[newObj->objNumber] = NULL;
      }

      deleteObjectType(newObj, FALSE);

      _outtext("\n\n");

      return NULL;
    }

    if (allowJump)
    {
      if ((checkMenuKey(ch, FALSE) == MENUKEY_NEXT) &&
          (obj->objNumber != getHighestObjNumber()))
      {
        done = TRUE;
      }

      if ((checkMenuKey(ch, FALSE) == MENUKEY_PREV) &&
          (obj->objNumber != getLowestObjNumber()))
      {
        done = TRUE;
      }

      if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP) done = TRUE;
    }

   // if interpEditObjTypeMenu is TRUE, user has quit

    if (!done)
      done = interpEditObjTypeMenu(ch, newObj, obj, &addedIdentKeyword);

    if (done)
    {
      if (!compareObjectType(obj, newObj))   // changes have been made
      {
        deleteObjectTypeAssocLists(obj);

        memcpy(obj, newObj, sizeof(dikuObject));

        delete newObj;

        numb = obj->objNumber;

        if (!obj->defaultObj)
        {
          if (numb < lowestObjNumber) lowestObjNumber = numb;
          if (numb > highestObjNumber) highestObjNumber = numb;

          if (numbObjTypes == 1) lowestObjNumber = highestObjNumber = numb;
        }

        madeChanges = TRUE;

       // recreate all editable and master keyword lists.  no choice..

        updateAllObjMandElists();  // update if room has objs

       // recreate inventory keywords for appropriate objs

        updateInvKeywordsObj(obj);
      }
      else deleteObjectType(newObj, FALSE);

      if (checkMenuKey(ch, FALSE) != MENUKEY_JUMP) _outtext("\n\n");

      if (allowJump)
      {
        if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP)
        {
          switch (jumpObj(obj, &numb))
          {
            case MENU_JUMP_ERROR : return obj;
            case MENU_JUMP_VALID : return findObj(numb);
            default : _outtext("ERR in editObjType()\n"); getkey(); return NULL;
          }
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_NEXT)
        {
          return getNextObj(obj->objNumber);
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_PREV)
        {
          return getPrevObj(obj->objNumber);
        }
      }

      return NULL;
    }
  }
}


void editObjType(dikuObject *obj, const char allowJump /*= TRUE*/)
{
  do
  {
    obj = realEditObjType(obj, allowJump);
  } while (obj);
}


char jumpObj(const dikuObject *obj, ulong *numb)
{
  struct rccoord coords;
  char strn[128], numbStrn[64];
  ulong numb2;


  coords = _gettextposition();
//  coords.row -= 2;

  _settextposition(coords.row, 1);
  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn, "&+CJump to which object (%u-%u, ? for list, $ to search): &n",
            getLowestObjNumber(), getHighestObjNumber());
    displayColorString(strn);

    itoa(obj->objNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strcmp(strn, "?"))
    {
      displayObjectTypeList();
    }
    else
    if (!strcmp(strn, "$"))
    {
      strn[0] = 0;

      _setbkcolor(0);
      displayColorString("\n&+CEnter substring to search object list for: &n");
      getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
      upstrn(strn);

      displayObjectTypeList(strn, FALSE);
    }
    else
    {
      numb2 = strtoul(strn, NULL, 10);

      if ((!strnumer(strn)) || (numb2 >= numbLookupEntries) ||
          (!findObj(numb2)))
      {
        clrline(coords.row, 7, 0);
        _settextposition(coords.row, 1);
        _setbkcolor(0);

        displayColorString(getMenuPromptName());

        return MENU_JUMP_ERROR;
      }
      else
      {
        *numb = numb2;
        return MENU_JUMP_VALID;
      }
    }
  }
}
