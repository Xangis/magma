//
//  File: editoflg.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing object bitvect flags
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"
#include "../const.h"

#include "object.h"

extern _CONST flagDef objExtraFlagDef[], objExtra2FlagDef[], objWearFlagDef[],
               objAntiFlagDef[], objAnti2FlagDef[];

extern ulong objExtraFlagTemplates[], objWearFlagTemplates[];

//
// editObjExtraFlags : Edit object extra flags - "main" function
//
//   *obj : obj to edit
//

void editObjExtraFlags(dikuObject *obj, dikuObject *origObj)
{
  char strn[256], newName[256];
  ulong oldFlags = obj->extraBits.longIntFlags,
        oldOrigEFlags = origObj->extraBits.longIntFlags,
        oldOrigWFlags = origObj->wearBits.longIntFlags;
  usint ch;

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing extra flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(objExtraFlagDef, &(obj->extraBits.longIntFlags), strn,
            "object extra flags", objExtraFlagTemplates, 3);

 // temporarily set flags in origObj to new value so that below call works

  origObj->extraBits.longIntFlags = obj->extraBits.longIntFlags;
  origObj->wearBits.longIntFlags = obj->wearBits.longIntFlags;

  if (!mobsCanEquipEquipped(FALSE))
  {
    displayColorString("\n\n&+c"
"Error - mobs that were equipping this object no longer can due to changes in\n&+c"
"        object flags - revert flags back to &+CP&n&+crevious, or stick all affected\n&+c"
"        eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

    if (ch != 'P')
    {
      mobsCanEquipEquipped(TRUE);
    }
    else obj->extraBits.longIntFlags = oldFlags;

    _outtext("\n\n");
  }

  origObj->extraBits.longIntFlags = oldOrigEFlags;
  origObj->wearBits.longIntFlags = oldOrigWFlags;
}


//
// editObjExtra2Flags : Edit object extra2 flags - "main" function
//
//   *obj : obj to edit
//

void editObjExtra2Flags(dikuObject *obj, dikuObject *origObj)
{
  char strn[256], newName[256];
  ulong oldFlags = obj->extra2Bits.longIntFlags,
        oldOrigEFlags = origObj->extraBits.longIntFlags,
        oldOrigE2Flags = origObj->extra2Bits.longIntFlags,
        oldOrigWFlags = origObj->wearBits.longIntFlags;
  usint ch;

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing extra2 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(objExtra2FlagDef, &(obj->extra2Bits.longIntFlags), strn,
            "object extra2 flags", /*objExtra2FlagTemplates*/NULL, 0);

 // temporarily set flags in origObj to new value so that below call works

  origObj->extraBits.longIntFlags = obj->extraBits.longIntFlags;
  origObj->extra2Bits.longIntFlags = obj->extra2Bits.longIntFlags;
  origObj->wearBits.longIntFlags = obj->wearBits.longIntFlags;

  if (!mobsCanEquipEquipped(FALSE))
  {
    displayColorString("\n\n&+c"
"Error - mobs that were equipping this object no longer can due to changes in\n&+c"
"        object flags - revert flags back to &+CP&n&+crevious, or stick all affected\n&+c"
"        eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

    if (ch != 'P')
    {
      mobsCanEquipEquipped(TRUE);
    }
    else obj->extra2Bits.longIntFlags = oldFlags;

    _outtext("\n\n");
  }

  origObj->extraBits.longIntFlags = oldOrigEFlags;
  origObj->extra2Bits.longIntFlags = oldOrigE2Flags;
  origObj->wearBits.longIntFlags = oldOrigWFlags;
}


//
// editObjWearFlags : Edit object wear flags - "main" function
//
//   *obj : obj to edit
//

void editObjWearFlags(dikuObject *obj, dikuObject *origObj)
{
  char strn[256], newName[256];
  ulong oldFlags = obj->wearBits.longIntFlags,
        oldOrigWFlags = origObj->wearBits.longIntFlags,
        oldOrigEFlags = origObj->extraBits.longIntFlags;
  usint ch;

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing wear flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(objWearFlagDef, &(obj->wearBits.longIntFlags), strn,
            "object wear flags", objWearFlagTemplates, 3);

 // temporarily set flag in origObj to new value so that below call works

  origObj->wearBits.longIntFlags = obj->wearBits.longIntFlags;
  origObj->extraBits.longIntFlags = obj->extraBits.longIntFlags;

 // no anti flags in wear bits - check only that position equipment is
 // currently in for mobs is valid for the set bits

  if (!mobsEqinCorrectSlot(FALSE))
  {
    displayColorString("\n\n&+c"
"Error - mobs that were equipping this object no longer can due to changes in\n&+c"
"        object flags - revert flags back to &+CP&n&+crevious, or stick all affected\n&+c"
"        eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

    if (ch != 'P')
    {
      mobsEqinCorrectSlot(TRUE);
    }
    else obj->wearBits.longIntFlags = oldFlags;

    _outtext("\n\n");
  }

  origObj->wearBits.longIntFlags = oldOrigWFlags;
  origObj->extraBits.longIntFlags = oldOrigEFlags;
}


//
// editObjAntiFlags : Edit object anti flags - "main" function
//
//   *obj : obj to edit
//

void editObjAntiFlags(dikuObject *obj, dikuObject *origObj)
{
  char strn[256], newName[256];
  ulong oldFlags = obj->antiBits.longIntFlags,
        oldOrigEFlags = origObj->extraBits.longIntFlags,
        oldOrigE2Flags = origObj->extra2Bits.longIntFlags,
        oldOrigWFlags = origObj->wearBits.longIntFlags,
        oldOrigAFlags = origObj->antiBits.longIntFlags,
        oldOrigA2Flags = origObj->anti2Bits.longIntFlags;
  usint ch;

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing anti flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(objAntiFlagDef, &(obj->antiBits.longIntFlags), strn,
            "object anti flags", /*objAntiFlagTemplates*/NULL, 3);

 // temporarily set flags in origObj to new value so that below call works

  origObj->extraBits.longIntFlags = obj->extraBits.longIntFlags;
  origObj->extra2Bits.longIntFlags = obj->extra2Bits.longIntFlags;
  origObj->wearBits.longIntFlags = obj->wearBits.longIntFlags;
  origObj->antiBits.longIntFlags = obj->antiBits.longIntFlags;
  origObj->anti2Bits.longIntFlags = obj->anti2Bits.longIntFlags;

  if (!mobsCanEquipEquipped(FALSE))
  {
    displayColorString("\n\n&+c"
"Error - mobs that were equipping this object no longer can due to changes in\n&+c"
"        object flags - revert flags back to &+CP&n&+crevious, or stick all affected\n&+c"
"        eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

    if (ch != 'P')
    {
      mobsCanEquipEquipped(TRUE);
    }
    else obj->antiBits.longIntFlags = oldFlags;

    _outtext("\n\n");
  }

  origObj->extraBits.longIntFlags = oldOrigEFlags;
  origObj->extra2Bits.longIntFlags = oldOrigE2Flags;
  origObj->wearBits.longIntFlags = oldOrigWFlags;
  origObj->antiBits.longIntFlags = oldOrigAFlags;
  origObj->anti2Bits.longIntFlags = oldOrigA2Flags;
}


//
// editObjAnti2Flags : Edit object anti flags - "main" function
//
//   *obj : obj to edit
//

void editObjAnti2Flags(dikuObject *obj, dikuObject *origObj)
{
  char strn[256], newName[256];
  ulong oldFlags = obj->anti2Bits.longIntFlags,
        oldOrigEFlags = origObj->extraBits.longIntFlags,
        oldOrigE2Flags = origObj->extra2Bits.longIntFlags,
        oldOrigWFlags = origObj->wearBits.longIntFlags,
        oldOrigAFlags = origObj->antiBits.longIntFlags,
        oldOrigA2Flags = origObj->anti2Bits.longIntFlags;
  usint ch;

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing anti2 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(objAnti2FlagDef, &(obj->anti2Bits.longIntFlags), strn,
            "object anti2 flags", /*objAntiFlagTemplates*/NULL, 3);

 // temporarily set flags in origObj to new value so that below call works

  origObj->extraBits.longIntFlags = obj->extraBits.longIntFlags;
  origObj->extra2Bits.longIntFlags = obj->extra2Bits.longIntFlags;
  origObj->wearBits.longIntFlags = obj->wearBits.longIntFlags;
  origObj->antiBits.longIntFlags = obj->antiBits.longIntFlags;
  origObj->anti2Bits.longIntFlags = obj->anti2Bits.longIntFlags;

  if (!mobsCanEquipEquipped(FALSE))
  {
    displayColorString("\n\n&+c"
"Error - mobs that were equipping this object no longer can due to changes in\n&+c"
"        object flags - revert flags back to &+CP&n&+crevious, or stick all affected\n&+c"
"        eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

    if (ch != 'P')
    {
      mobsCanEquipEquipped(TRUE);
    }
    else obj->anti2Bits.longIntFlags = oldFlags;

    _outtext("\n\n");
  }

  origObj->extraBits.longIntFlags = oldOrigEFlags;
  origObj->extra2Bits.longIntFlags = oldOrigE2Flags;
  origObj->wearBits.longIntFlags = oldOrigWFlags;
  origObj->antiBits.longIntFlags = oldOrigAFlags;
  origObj->anti2Bits.longIntFlags = oldOrigA2Flags;
}
