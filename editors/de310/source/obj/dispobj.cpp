//
//  File: dispobj.cpp    originally part of dikuEdit
//
//  Usage: functions for displaying information on objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "object.h"

extern dikuRoom *roomHead;
extern dikuObject *objectHead;

extern _CONST flagDef objExtraFlagDef[], objExtra2FlagDef[], objWearFlagDef[], objAntiFlagDef[],
               objAnti2FlagDef[], aff1FlagDef[], aff2FlagDef[], aff3FlagDef[], aff4FlagDef[];

//
// displayObjectTypeList : Displays the list of object types loaded into game,
//                         starting at objectHead
//

void displayObjectTypeList(void)
{
  dikuObject *obj = objectHead;
  ulong i = 0;
  char strn[256];


  _setbkcolor(0);

  if (!obj)
  {
    _outtext("\nThere are currently no object types.\n\n");

    return;
  }

  _outtext("\n\n");

  while (obj)
  {
    sprintf(strn, "%s&n (#%u)\n", obj->objShortName, obj->objNumber);
    displayColorString(strn);

    i += numbLinesStringNeeds(strn);

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      i = 0;
    }

    obj = obj->Next;
  }

  _outtext("\n");
}


//
// displayObjectTypeList : Displays the list of object types loaded into
//                         game, starting at objectHead
//

void displayObjectTypeList(const char *strn, const char scanTitle)
{
  dikuObject *obj = objectHead;
  ulong i = 0, numb;
  char outStrn[512], uptitle[512], upperstrn[512], foundObj = FALSE, vnum;


  if (strlen(strn) == 0)
  {
    displayObjectTypeList();
    return;
  }

  _setbkcolor(0);

  if (!obj)
  {
    _outtext("\nThere are currently no object types.\n\n");

    return;
  }

  vnum = strnumer(strn);
  numb = atoi(strn);

  if (scanTitle)
  {
    strcpy(upperstrn, strn);
    upstrn(upperstrn);

    strcpy(uptitle, obj->objShortName);
    upstrn(uptitle);
  }

  _outtext("\n\n");

  while (obj)
  {
    if ((vnum && (numb == obj->objNumber)) ||
        (!scanTitle && scanKeyword(strn, obj->keywordListHead)) ||
        (scanTitle && strstr(uptitle, upperstrn)))
    {
      sprintf(outStrn, "%s&n (#%u)\n", obj->objShortName, obj->objNumber);

      foundObj = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

      if (i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return;
        }

        _outtext("\n\n");
        i = 0;
      }

    }

    obj = obj->Next;
  }

  if (!foundObj) _outtext("No matching object types found.\n");
  _outtext("\n");
}


//
// displayObjectHereList
//

void displayObjectHereList(void)
{
  dikuRoom *room = roomHead;
  mobHere *mob;
  char strn[512], mobName[512];
  ulong i = 0, j;


  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    sprintf(strn, " in %s&n (#%u)",
            room->roomName, room->roomNumber);

    if (displayEntireObjectHereList(room->objectHead, strn, &i)) return;

    mob = room->mobHead;
    while (mob)
    {
      if (mob->mobPtr) strcpy(mobName, mob->mobPtr->mobShortName);
      else strcpy(mobName, "(mob type not in this .MOB file)");

      sprintf(strn, " equipped by %s&n (#%u) [#%u]",
              mobName, mob->mobNumb, room->roomNumber);

//      if (displayEntireObjectHereList(mob->equipHead, strn, &i)) return;
     // this should work.  shrug

      for (j = WEAR_LOW; j <= WEAR_TRUEHIGH; j++)
      {
        if (displayEntireObjectHereList(mob->equipment[j], strn, &i)) return;
      }

      sprintf(strn, " carried by %s&n (#%u) [#%u]",
              mobName, mob->mobNumb, room->roomNumber);

      if (displayEntireObjectHereList(mob->objectHead, strn, &i)) return;

      mob = mob->Next;
    }

    room = room->Next;
  }

  _outtext("\n");
}


//
// displayEntireObjectHereList - this un shows a list of stuff in an objectHere
//                               list, and all the stuff inside the objHeres -
//                               returns TRUE if user hits Q
//

char displayEntireObjectHereList(objectHere *objHead, const char *postStrn,
                                 ulong *i)
{
  objectHere *objHere = objHead;
  char strn[512], objName[512];


  _setbkcolor(0);

  while (objHere)
  {
    if (objHere->objectPtr) strcpy(objName, objHere->objectPtr->objShortName);
    else strcpy(objName, "(object type not in this .OBJ file)");

    sprintf(strn, "%s&n (#%u)%s\n",
            objName, objHere->objectNumb, postStrn);
    displayColorString(strn);

    (*i) += numbLinesStringNeeds(strn);

    if (*i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return TRUE;
      }

      _outtext("\n\n");

      *i = 0;
    }

    if (objHere->objInside)
    {
      sprintf(strn, " inside %s&n (#%u)",
              objName, objHere->objectNumb);

      if (displayEntireObjectHereList(objHere->objInside, strn, i)) return TRUE;
    }

    objHere = objHere->Next;
  }

  return FALSE;
}


//
// displayEntireObjectHereList - this un shows a list of stuff in an objectHere
//                               list, and all the stuff inside the objHeres
//

char displayEntireObjectHereList(objectHere *objHead, const char *postStrn,
                                 ulong *i, const char *keyStrn,
                                 const long roomNumber, char *foundMatch)
{
  objectHere *objHere = objHead;
  char strn[512], objName[512];


  _setbkcolor(0);

  while (objHere)
  {
    if (objHere->objectPtr)
    {
      strcpy(objName, objHere->objectPtr->objShortName);
    }
    else strcpy(objName, "(object type not in this .OBJ file)");

    if (objHere->objectPtr &&
        scanKeyword(keyStrn, objHere->objectPtr->keywordListHead))
    {
      *foundMatch = TRUE;

      sprintf(strn, "%s&n (#%u)%s\n",
              objName, objHere->objectNumb, postStrn);
      displayColorString(strn);

      (*i) += numbLinesStringNeeds(strn);

      if (*i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return TRUE;
        }

        _outtext("\n\n");

        *i = 0;
      }
    }

    if (objHere->objInside)
    {
      sprintf(strn, " inside %s&n (#%u)  [#%u]",
              objName, objHere->objectNumb, roomNumber);

      if (displayEntireObjectHereList(objHere->objInside, strn, i,
                                      keyStrn, roomNumber, foundMatch))
        return TRUE;
    }

    objHere = objHere->Next;
  }

  return FALSE;
}


//
// displayEntireObjectHereList - this un shows a list of stuff in an objectHere
//                               list, and all the stuff inside the objHeres
//

char displayEntireObjectHereList(objectHere *objHead, char *postStrn, ulong *i,
                                 long objNumber, long roomNumber,
                                 char *foundMatch)
{
  objectHere *objHere = objHead;
  char strn[512], objName[512];


  _setbkcolor(0);

  while (objHere)
  {
    if (objHere->objectPtr)
    {
      strcpy(objName, objHere->objectPtr->objShortName);
    }
    else strcpy(objName, "(object type not in this .OBJ file)");

    if (objHere->objectNumb == objNumber)
    {
      *foundMatch = TRUE;

      sprintf(strn, "%s&n (#%u)%s\n",
              objName, objHere->objectNumb, postStrn);
      displayColorString(strn);

      (*i) += numbLinesStringNeeds(strn);

      if (*i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return TRUE;
        }

        _outtext("\n\n");

        *i = 0;
      }
    }

    if (objHere->objInside)
    {
      sprintf(strn, " inside %s&n (#%u)  [#%u]",
              objName, objHere->objectNumb, roomNumber);

      if (displayEntireObjectHereList(objHere->objInside, strn, i,
                                      objNumber, roomNumber, foundMatch))
        return TRUE;
    }

    objHere = objHere->Next;
  }

  return FALSE;
}


//
// displayObjTypeList : Displays list of object types
//

void displayObjTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = ITEM_LOWEST; i <= ITEM_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getObjTypeStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayObjApplyTypeList : Displays list of object apply types
//

void displayObjApplyTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = APPLY_LOWEST; i <= APPLY_HIGHEST; i++)
  {
    if (i == 17) continue; // old AC apply

    sprintf(strn, "  %u - %s\n", i, getObjApplyStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayObjTrapDamList : Displays list of object trap damage types
//

void displayObjTrapDamList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = TRAP_DAM_LOWEST; i <= TRAP_DAM_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getObjTrapDamStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayObjectInfo : Displays various info on the room record passed to it
//
//   *room : room record to display info on
//

void displayObjectInfo(const char *args, dikuRoom *room)
{
  char *strn = new char[4096], *strn2 = new char[4096], extraStrn[256],
       fstrn[2048], fstrn2[2048], fstrn3[2048], fstrn4[2048],
        fstrn5[2048], fstrn6[2048], fstrn7[2048], fstrn8[2048], fstrn9[2048],
       vstrn[512], vstrn2[512], vstrn3[512], vstrn4[512],
       v2strn[512], v2strn2[512], v2strn3[512], v2strn4[512];

  dikuObject *obj;
  extraDesc *extraDescNode;


  if (!strn || !strn2)
  {
    _outtext("\n\nError allocating string buffer in displayObjectInfo() - "
             "aborting..\n");

    if (strn) delete[] strn;
    if (strn2) delete[] strn2;

    return;  // no need to die completely, after all
  }

  if (!room)
  {
    _outtext("\n\nerror in displayObjectInfo() - room ptr is NULL\n");

    delete[] strn;
    delete[] strn2;

    return;
  }

 // input-checking galore

  if (!strlen(args))
  {
    displayColorString("&n\nSpecify an object vnum/keyword to stat.\n\n");
    return;
  }

  obj = getMatchingObj(args, room);
  if (!obj)
  {
    if (strnumer(args))
    {
      displayColorString("&n\nNo object type exists with that vnum.\n\n");
      return;
    }
    else
    {
      displayColorString("&n\nNo object type exists with that keyword.\n\n");
      return;
    }
  }

  extraDescNode = obj->extraDescHead;

  strcpy(extraStrn, "\0");

  while (extraDescNode)
  {
   // second argument strn because it requires a second arg, and hell, why not?
    strcat(extraStrn,
           getReadableKeywordStrn(extraDescNode->keywordListHead, strn));

    extraDescNode = extraDescNode->Next;
    if (extraDescNode) strcat(extraStrn, " / ");
  }

  sprintf(strn,
"\n"
"&+YVnum:&n %d  &+YShort name:&n %s&n\n"
"&+YLong name:&n %s&n\n"
"&+YKeywords:&n %s\n"
"&+YExtra descs:&n %s\n"
"\n"
"&+YObject extra bits :&n %u (%s)\n"
"&+YObject extra2 bits:&n %u (%s)\n"
"&+YObject wear bits  :&n %u (%s)\n"
"&+YObject anti bits  :&n %u (%s)\n"
"&+YObject anti2 bits :&n %u (%s)\n"
"&+YObject aff1 bits  :&n %u (%s)\n"
"&+YObject aff2 bits  :&n %u (%s)\n"
"&+YObject aff3 bits  :&n %u (%s)\n"
"&+YObject aff4 bits  :&n %u (%s)\n"
"\n"
"&+YObject type:&n %d (%s)\n"
"&+YValue #1   :&n %d (%s)\n"
"&+YValue #2   :&n %d (%s)\n"
"&+YValue #3   :&n %d (%s)\n"
"&+YValue #4   :&n %d (%s)\n"
"\n"
"&+YObj apply #1:&n %d (%s) by %d\n"
"&+YObj apply #2:&n %d (%s) by %d\n"
"\n"
"&+YWeight:&n %d    &+YValue:&n %u    &+YCondition:&n %d\n"
"\n",

obj->objNumber, obj->objShortName,
obj->objLongName,
getReadableKeywordStrn(obj->keywordListHead, strn2),
extraStrn,

obj->extraBits.longIntFlags,
  getFlagStrn(obj->extraBits.longIntFlags, objExtraFlagDef, fstrn),
obj->extra2Bits.longIntFlags,
  getFlagStrn(obj->extra2Bits.longIntFlags, objExtra2FlagDef, fstrn2),
obj->wearBits.longIntFlags,
  getFlagStrn(obj->wearBits.longIntFlags, objWearFlagDef, fstrn3),
obj->antiBits.longIntFlags,
  getFlagStrn(obj->antiBits.longIntFlags, objAntiFlagDef, fstrn4),
obj->anti2Bits.longIntFlags,
  getFlagStrn(obj->anti2Bits.longIntFlags, objAnti2FlagDef, fstrn5),
obj->affect1Bits.longIntFlags,
  getFlagStrn(obj->affect1Bits.longIntFlags, aff1FlagDef, fstrn6),
obj->affect2Bits.longIntFlags,
  getFlagStrn(obj->affect2Bits.longIntFlags, aff2FlagDef, fstrn7),
obj->affect3Bits.longIntFlags,
  getFlagStrn(obj->affect3Bits.longIntFlags, aff3FlagDef, fstrn8),
obj->affect4Bits.longIntFlags,
  getFlagStrn(obj->affect4Bits.longIntFlags, aff4FlagDef, fstrn9),

obj->objType, getObjTypeStrn(obj->objType),
obj->objValues[0], getObjValueStrn(obj->objType, 0, obj->objValues[0],
                                   vstrn, v2strn, TRUE, obj),
obj->objValues[1], getObjValueStrn(obj->objType, 1, obj->objValues[1],
                                   vstrn2, v2strn2, TRUE, obj),
obj->objValues[2], getObjValueStrn(obj->objType, 2, obj->objValues[2],
                                   vstrn3, v2strn3, TRUE, obj),
obj->objValues[3], getObjValueStrn(obj->objType, 3, obj->objValues[3],
                                   vstrn4, v2strn4, TRUE, obj),

obj->objApply[0].applyWhere, getObjApplyStrn(obj->objApply[0].applyWhere),
  obj->objApply[0].applyModifier,
obj->objApply[1].applyWhere, getObjApplyStrn(obj->objApply[1].applyWhere),
  obj->objApply[1].applyModifier,

obj->weight, obj->value, obj->condition);

  displayColorString(strn);

  if (strn) delete[] strn;
  if (strn2) delete[] strn2;
}
