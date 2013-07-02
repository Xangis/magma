//
//  File: check.cpp      originally part of dikuEdit
//
//  Usage: functions galore for checking validity of everything
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#ifdef __UNIX__
#  include <unistd.h>
#else
#  include <io.h>
#endif

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "../misc/mudcomm.h"
#include "check.h"

FILE *tmpCheck;  // unneeded global variables are bad..  so sue me

extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern dikuMob *mobHead;
extern _CONST flagDef roomFlagDef[], objExtraFlagDef[], objExtra2FlagDef[],
               objWearFlagDef[], aff1FlagDef[], aff2FlagDef[],
               aff3FlagDef[], aff4FlagDef[],
               objAntiFlagDef[], objAnti2FlagDef[], mobActFlagDef[];
extern _CONST char *exitnames[];

//
// outCheckError
//

void outCheckError(const char *error, FILE *file)
{
  if (file) fputs(error, file);

  if (!getPauseCheckScreenfulVal()) _outtext((char *)error);
  else if (tmpCheck) fputs(error, tmpCheck);
}


//
// checkFlags
//

ulong checkFlags(FILE *file, const ulong flagVal, const flagDef *flagArr,
                 const char *flagName, const char *entityName,
                 const ulong entityNumb, const char outputErrors)
{
  ulong errors = 0, numbFlags = flagArr[0].editable, i;
  char strn[256];


  for (i = 1; i <= numbFlags; i++)
  {
    if (!flagArr[i].editable &&
        (((flagVal & (1 << (i - 1))) ? 1 : 0) != flagArr[i].defVal))
    {
      if (outputErrors)
      {
        sprintf(strn,
"error in %s #%u - %s flag's %s bit is set to %u instead of %u\n",
                entityName, entityNumb, flagName, flagArr[i].flagShort,
                (flagVal & (1 << (i - 1))) ? 1 : 0, flagArr[i].defVal);

        outCheckError(strn, file);
      }

      errors++;
    }
  }

  return errors;
}


//
// checkAllFlags
//

ulong checkAllFlags(FILE *file)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;
//  char strn[256];
  ulong errors = 0;


  while (room)
  {
    errors += checkFlags(file, room->roomFlags.longIntFlags, roomFlagDef,
                         "roomflag", "room", room->roomNumber, TRUE);

    room = room->Next;
  }

  while (obj)
  {
    errors += checkFlags(file, obj->extraBits.longIntFlags, objExtraFlagDef,
                         "extra", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->extra2Bits.longIntFlags, objExtra2FlagDef,
                         "extra2", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->wearBits.longIntFlags, objWearFlagDef,
                         "wear", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->affect1Bits.longIntFlags, aff1FlagDef,
                         "aff1", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->affect2Bits.longIntFlags, aff2FlagDef,
                         "aff2", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->affect3Bits.longIntFlags, aff3FlagDef,
                         "aff3", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->affect4Bits.longIntFlags, aff4FlagDef,
                         "aff4", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->antiBits.longIntFlags, objAntiFlagDef,
                         "anti", "object", obj->objNumber, TRUE);

    errors += checkFlags(file, obj->anti2Bits.longIntFlags, objAnti2FlagDef,
                         "anti2", "object", obj->objNumber, TRUE);

    obj = obj->Next;
  }

  while (mob)
  {
    errors += checkFlags(file, mob->actionBits.longIntFlags, mobActFlagDef,
                         "action", "mob", mob->mobNumber, TRUE);

    errors += checkFlags(file, mob->affect1Bits.longIntFlags, aff1FlagDef,
                         "aff1", "mob", mob->mobNumber, TRUE);

    errors += checkFlags(file, mob->affect2Bits.longIntFlags, aff2FlagDef,
                         "aff2", "mob", mob->mobNumber, TRUE);

    errors += checkFlags(file, mob->affect3Bits.longIntFlags, aff3FlagDef,
                         "aff3", "mob", mob->mobNumber, TRUE);

    errors += checkFlags(file, mob->affect4Bits.longIntFlags, aff4FlagDef,
                         "aff4", "mob", mob->mobNumber, TRUE);

    mob = mob->Next;
  }

  return errors;
}


//
// checkRooms
//

ulong checkRooms(FILE *file)
{
  dikuRoom *room = roomHead;
  char strn[256];
  ulong errors = 0;


  while (room)
  {
   // check room name

    if (!room->roomName[0])
    {
      sprintf(strn, "error - room #%u has no name\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for rooms with no descriptions

    if (!room->roomDescHead)
    {
      sprintf(strn,
"error - room #%u has no description\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check fall chance

    if ((room->fallChance < 0) || (room->fallChance > 100))
    {
      sprintf(strn,
"error - room #%u's fall chance is out of range (0-100)\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check current chance/direction

    if ((room->current < 0) || (room->current > 100))
    {
      sprintf(strn,
"error - room #%u's current chance is out of range (0-100)\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    if ((room->currentDir < EXIT_LOWEST) || (room->currentDir > EXIT_HIGHEST))
    {
      sprintf(strn,
"error - room #%u's current direction is out of range\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mana flag type

    if (/*(room->manaFlag < MANA_LOWEST) ||*/ (room->manaFlag > MANA_HIGHEST))
    {
      sprintf(strn,
"error - room #%u's mana type is out of range\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check sector type

    if (/*(room->sectorType < SECT_LOWEST) ||*/ (room->sectorType > SECT_HIGHEST))
    {
      sprintf(strn,
"error - room #%u's sector type is out of range\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for single-file rooms with incorrect number of exits

    if (room->roomFlags.roomBits.singleFile && (getNumbExits(room) != 2))
    {
      sprintf(strn,
"error - room #%u has SINGLE_FILE bit set, but has %u exit%s instead of 2\n",
              room->roomNumber, getNumbExits(room), plural(getNumbExits(room)));
      outCheckError(strn, file);

      errors++;
    }

   // check for underwater rooms improperly set one way or the other

    if (((room->sectorType == SECT_UNDERWATER) ||
         (room->sectorType == SECT_UNDERWATER_GR)) &&
         !room->roomFlags.roomBits.underwater)
    {
      sprintf(strn,
"error - room #%u has sector type of UNDERWATER or UNDERWATER_GROUND, but\n"
"        doesn't have UNDERWATER flag set\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    if ((!((room->sectorType == SECT_UNDERWATER) ||
         (room->sectorType == SECT_UNDERWATER_GR))) &&
         room->roomFlags.roomBits.underwater)
    {
      sprintf(strn,
"error - room #%u has UNDERWATER bit set, but a sector type other than\n"
"        UNDERWATER or UNDERWATER_GROUND\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for no-ground/fall rooms with no valid down exit

    if (((room->sectorType == SECT_NO_GROUND) ||
         (room->sectorType == SECT_UNDRWLD_NOGROUND)) &&
         (!room->exits[DOWN] || (room->exits[DOWN]->destRoom < 0)))
    {
      sprintf(strn,
"error - room #%u is NO_GROUND or UNDRWLD_NOGROUND but has no valid down exit\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    if (room->fallChance &&
        (!room->exits[DOWN] || (room->exits[DOWN]->destRoom < 0)))
    {
      sprintf(strn,
"error - room #%u has a fall percentage but no valid down exit\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for rooms with more than one light flag set

    if ((room->roomFlags.roomBits.dark &&
        (room->roomFlags.roomBits.twilight || room->roomFlags.roomBits.magicLight ||
         room->roomFlags.roomBits.magicDark)) ||
        (room->roomFlags.roomBits.twilight &&
        (room->roomFlags.roomBits.magicLight || room->roomFlags.roomBits.magicDark)) ||
        (room->roomFlags.roomBits.magicLight && room->roomFlags.roomBits.magicDark))
    {
      sprintf(strn,
"error - room #%u has some combination of nonsensical light flags set\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for rooms with a nonsensical combination of heal flags set

    if (room->roomFlags.roomBits.heal && room->roomFlags.roomBits.noHeal)
    {
      sprintf(strn,
"error - room #%u is both HEAL and NOHEAL - makes no sense\n",
              room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    room = room->Next;
  }

  return errors;
}


//
// checkLoneRooms
//

ulong checkLoneRooms(FILE *file)
{
  dikuRoom *room = roomHead;
  char strn[256];
  ulong errors = 0;


  while (room)
  {
    if (noExitsLeadtoRoom(room->roomNumber))
    {
      sprintf(strn, "error - no exits lead to room #%u\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    if (noExitOut(room))
    {
      sprintf(strn, "error - no exits lead out of room #%u\n", room->roomNumber);
      outCheckError(strn, file);

      errors++;
    }

    room = room->Next;
  }

  return errors;
}


//
// checkExitKeyLoaded - true if error
//

INLINEDEF char checkExitKeyLoaded(const roomExit *exitNode)
{
  if (!exitNode) return FALSE;

 // exit with key vnum specified, but no key loaded

  if ((exitNode->keyNumb > 0) && !isObjTypeUsed(exitNode->keyNumb))
    return TRUE;
  else
    return FALSE;
}


//
// checkMissingKeys : returns number of errors
//

ulong checkMissingKeys(FILE *file)
{
  ulong errors = 0;
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead, *obj2;
  char strn[256], found, i;

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (checkExitKeyLoaded(room->exits[i]))
      {
        sprintf(strn,
                "error in %s exit of room #%u - requires key #%u, but it isn't loaded\n",
                exitnames[i], room->roomNumber, room->exits[i]->keyNumb);
        outCheckError(strn, file);

        errors++;
      }
    }

    room = room->Next;
  }

  while (obj)
  {
    if ((obj->objType == ITEM_CONTAINER) && (obj->objValues[2] > 0) &&
        !isObjTypeUsed(obj->objValues[2]))
    {
      sprintf(strn,
              "error in object #%u - requires key #%u, but it isn't loaded\n",
              obj->objNumber, obj->objValues[2]);
      outCheckError(strn, file);

      errors++;
    }

    obj = obj->Next;
  }

 // check for extraneous keys

  obj = objectHead;

  while (obj)
  {
    found = FALSE;

    if (obj->objType == ITEM_KEY)
    {
      room = roomHead;

      while (room)
      {
        for (i = 0; i < NUMB_EXITS; i++)
        {
          if (room->exits[i] && (room->exits[i]->keyNumb == obj->objNumber))
          {
            found = TRUE;
            break;
          }
        }

        if (found) break;

        room = room->Next;
      }

      if (!found)
      {
        obj2 = objectHead;

        while (obj2)
        {
          if ((obj2->objType == ITEM_CONTAINER) &&
              (obj2->objValues[2] == obj->objNumber))
          {
            found = TRUE;
            break;
          }

          obj2 = obj2->Next;
        }
      }

      if (!found)
      {
        errors++;
        sprintf(strn, "error in key #%u - never used in this zone\n", obj->objNumber);
        outCheckError(strn, file);
      }
    }

    obj = obj->Next;
  }

  return errors;
}


//
// checkLoaded
//

ulong checkLoaded(FILE *file)
{
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;
  char strn[256];
  ulong errors = 0;


  while (obj)
  {
    if (!isObjTypeUsed(obj->objNumber))
    {
      sprintf(strn,
              "error - object type #%u is never loaded/used\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

    obj = obj->Next;
  }

  while (mob)
  {
    if (!getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE))
    {
      sprintf(strn,
              "error - mob type #%u is never loaded\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

    mob = mob->Next;
  }

  return errors;
}


//
// checkExit - true if error
//

INLINEDEF char checkExit(const roomExit *exitNode)
{
  if (!exitNode) return FALSE;

 // exit with door type set and no keywords

  if ((exitNode->worldDoorType & 3) && (!exitNode->keywordListHead))
    return TRUE;
  else
    return FALSE;
}


//
// checkExits - check for door state/type w/ no keywords - returns number
//              of errors found
//

ulong checkExits(FILE *file)
{
  dikuRoom *room = roomHead;
  char strn[256], i;
  ulong errors = 0;

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (checkExit(room->exits[i]))
      {
        sprintf(strn,
                "error in %s exit of room #%u - door type but no keywords\n",
                exitnames[i], room->roomNumber);
        outCheckError(strn, file);

        errors++;
      }
    }

    room = room->Next;
  }

  return errors;
}


//
// checkExitDesc - true if error
//

INLINEDEF char checkExitDesc(const roomExit *exitNode)
{
  if (!exitNode) return FALSE;

 // exit with no exit desc

  if (!exitNode->exitDescHead)
    return TRUE;
  else
    return FALSE;
}


//
// checkExitDescs - check for exit w/ no desc
//

ulong checkExitDescs(FILE *file)
{
  dikuRoom *room = roomHead;
  char strn[256], i;
  ulong errors = 0;

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (checkExitDesc(room->exits[i]))
      {
        sprintf(strn,
                "error in %s exit of room #%u - no exit desc\n",
                exitnames[i], room->roomNumber);
        outCheckError(strn, file);

        errors++;
      }
    }

    room = room->Next;
  }

  return errors;
}


//
// checkObjects
//

ulong checkObjects(FILE *file)
{
  ulong errors = 0, high, i;
  dikuObject *obj = objectHead;
  objectHere *objHere = NULL;
  extraDesc *edesc;
//  stringNode *strnNode;
  dikuRoom *room;
  objWearBitFlags objFlags;
  objExtraBitFlags objExtra;
  char strn[256];

  while (obj)
  {
   // check for keyword existance

    if (!obj->keywordListHead)
    {
      sprintf(strn,
"error in object #%u - object has no keywords\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for short and long names

    if (!obj->objShortName[0])
    {
      sprintf(strn,
"error in object #%u - object has no short name\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

    if (!obj->objLongName[0])
    {
      sprintf(strn,
"error in object #%u - object has no long name\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for no _id_ keyword with ident extra descs

    if ((getEdescinList(obj->extraDescHead, "_ID_NAME_") ||
         getEdescinList(obj->extraDescHead, "_ID_SHORT_") ||
         getEdescinList(obj->extraDescHead, "_ID_DESC_")) &&
        !scanKeyword("_ID_", obj->keywordListHead))
    {
      sprintf(strn,
"error in object #%u - object has 'ident' extra descs but no _ID_ keyword\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for _id_ keyword without ident extra descs

    if (scanKeyword("_ID_", obj->keywordListHead) &&
        !getEdescinList(obj->extraDescHead, "_ID_NAME_") &&
        !getEdescinList(obj->extraDescHead, "_ID_SHORT_") &&
        !getEdescinList(obj->extraDescHead, "_ID_DESC_"))
    {
      sprintf(strn,
"error in object #%u - object has _ID_ keyword but no 'ident' extra descs\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for ident extra descs with wrong amount of text in em

    if (edesc = getEdescinList(obj->extraDescHead, "_ID_NAME_"))
    {
      if ((i = getNumbStringNodes(edesc->extraDescStrnHead)) != 1)
      {
        sprintf(strn,
"error in object #%u - 'ident' extra desc '_ID_NAME_' has the wrong number\n"
"                      of lines of info - %u instead of one\n",
                obj->objNumber, i);
        outCheckError(strn, file);

        errors++;
      }
    }

    if (edesc = getEdescinList(obj->extraDescHead, "_ID_SHORT_"))
    {
      if ((i = getNumbStringNodes(edesc->extraDescStrnHead)) != 1)
      {
        sprintf(strn,
"error in object #%u - 'ident' extra desc '_ID_SHORT_' has the wrong number\n"
"                      of lines of info - %u instead of one\n",
                obj->objNumber, i);
        outCheckError(strn, file);

        errors++;
      }
    }

    if (edesc = getEdescinList(obj->extraDescHead, "_ID_DESC_"))
    {
      if ((i = getNumbStringNodes(edesc->extraDescStrnHead)) != 1)
      {
        sprintf(strn,
"error in object #%u - 'ident' extra desc '_ID_DESC_' has the wrong number\n"
"                      of lines of info - %u instead of one\n",
                obj->objNumber, i);
        outCheckError(strn, file);

        errors++;
      }
    }

   // check object type for validity

    if ((obj->objType < ITEM_LOWEST) || (obj->objType > ITEM_HIGHEST))
    {
      sprintf(strn,
"error in object #%u - object type is out of range\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check object size

    if (/*(obj->size < OBJSIZE_LOWEST) || */(obj->size > OBJSIZE_HIGHEST))
    {
      sprintf(strn,
"error in object #%u - object size is out of range\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check object craftsmanship

    if (/*(obj->craftsmanship < OBJCRAFT_LOWEST) ||*/
        (obj->craftsmanship > OBJCRAFT_HIGHEST))
    {
      sprintf(strn,
"error in object #%u - object craftmanship is out of range\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check object condition percentage

    if ((obj->condition == 0) || (obj->condition > 100))
    {
      sprintf(strn,
"error in object #%u - object's condition percentage is not between 1-100%%\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check object craftsmanship

    if (/*(obj->craftsmanship < OBJCRAFT_LOWEST) ||*/
        (obj->craftsmanship > OBJCRAFT_HIGHEST))
    {
      sprintf(strn,
"error in object #%u - object craftmanship value is out of range\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // all takeable objects should be worth something

    if (!obj->value && obj->wearBits.objWearBits.take)
    {
      sprintf(strn,
"error in object #%u - takeable object has a value of 0\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check object applies

    for (i = 0; i < NUMB_OBJ_APPLIES; i++)
    {
      if (!obj->objApply[i].applyWhere && obj->objApply[i].applyModifier)
      {
        sprintf(strn,
"error in object #%u apply #%u - 'where' of NOWHERE but non-zero modifier\n",
                obj->objNumber, i + 1);
        outCheckError(strn, file);

        errors++;
      }

      if ((obj->objApply[i].applyWhere == APPLY_CURSE) &&
          (obj->objApply[i].applyModifier <= 0))
      {
        sprintf(strn,
"error in object #%u apply #%u - APPLY_CURSE modifier must be greater than 0\n",
                obj->objNumber, i + 1);
        outCheckError(strn, file);

        errors++;
      }

      if (obj->objApply[i].applyWhere && !obj->objApply[i].applyModifier &&
          (obj->objApply[i].applyWhere != APPLY_FIRE_PROT))
      {
        sprintf(strn,
"error in object #%u apply #%u - valid 'where' location, but modifier is zero\n",
                obj->objNumber, i + 1);
        outCheckError(strn, file);

        errors++;
      }

      if (obj->objApply[i].applyWhere > APPLY_HIGHEST)
      {
        sprintf(strn,
"error in object #%u apply #%u - 'where' location is invalid (too high)\n",
                obj->objNumber, i + 1);
        outCheckError(strn, file);

        errors++;
      }

      if (obj->objApply[i].applyWhere == 17)
      {
        sprintf(strn,
"error in object #%u apply #%u - location is APPLY_AC, which is now outdated\n",
                obj->objNumber, i + 1);
        outCheckError(strn, file);

        errors++;
      }
    }

   // check trap stuff if object has one

    if (obj->trapAff)
    {
      if ((obj->trapDam < TRAP_DAM_LOWEST) || (obj->trapDam > TRAP_DAM_HIGHEST))
      {
        sprintf(strn,
  "error in object #%u - has a trap, but trap damage value is out of range\n",
                obj->objNumber);
        outCheckError(strn, file);

        errors++;
      }

      if ((obj->trapLevel == 0) || (obj->trapLevel > 100))
      {
        sprintf(strn,
  "error in object #%u - has a trap, but trap level is out of range (1-100)\n",
                obj->objNumber);
        outCheckError(strn, file);

        errors++;
      }

      if (obj->trapCharge == 0)
      {
        sprintf(strn,
  "error in object #%u - has a trap, but no charges\n",
                obj->objNumber);
        outCheckError(strn, file);

        errors++;
      }
    }

   // check for objects that are wearable but not takeable

    objFlags = obj->wearBits.objWearBits;
    objExtra = obj->extraBits.objExtraBits;

    if (((objFlags.wearFinger)  || (objFlags.wearNeck)  ||
         (objFlags.wearBody)    || (objFlags.wearHead)  ||
         (objFlags.wearLegs)    || (objFlags.wearFeet)  ||
         (objFlags.wearHands)   || (objFlags.wearArms)  ||
         (objFlags.wearShield)  || (objFlags.wearAbout) ||
         (objFlags.wearWaist)   || (objFlags.wearWrist) ||
         (objFlags.wield)       || (objFlags.hold)      ||
         (objFlags.wearEyes)    || (objFlags.wearFace)  ||
         (objFlags.wearEarring) || (objFlags.wearBadge) ||
         (objFlags.wearBack)    || (objFlags.attachBelt) ||
         (objFlags.horseBody)   || (objFlags.wearTail)  ||
         (objFlags.wearNose)    || (objFlags.wearHorns) ||
         (objExtra.wholeBody)   || (objExtra.wholeHead) ||
         (objExtra.twoHands))   && (!objFlags.take))
    {
      sprintf(strn,
"error in object #%u - object is wearable, but not takeable\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check for proper use of "allow all" anti flag

    if (obj->antiBits.objAntiBits.allowAll)
    {
      obj->antiBits.objAntiBits.allowAll = FALSE;
      if (obj->antiBits.longIntFlags || obj->anti2Bits.longIntFlags)
      {
        sprintf(strn,
"error in obj #%u - object has ALLOW_ALL anti flag set, but has other\n"
"                   anti/anti2 flags set - ALLOW_ALL overrides all flags in\n"
"                   anti/anti2 bitvectors\n",
                obj->objNumber);
        outCheckError(strn, file);

        errors++;
      }

      obj->antiBits.objAntiBits.allowAll = TRUE;
    }

   // if none of the "usable" flags in the first anti bitvector are set, the
   // object is not usable by anyone (anti2 contains only anti flags)

    if (obj->wearBits.objWearBits.take &&
        !obj->antiBits.objAntiBits.allowAll &&
        !obj->antiBits.objAntiBits.allowWar &&
        !obj->antiBits.objAntiBits.allowRan &&
        !obj->antiBits.objAntiBits.allowPal &&
        !obj->antiBits.objAntiBits.allowAnP &&
        !obj->antiBits.objAntiBits.allowCle &&
        !obj->antiBits.objAntiBits.allowMon &&
        !obj->antiBits.objAntiBits.allowDru &&
        !obj->antiBits.objAntiBits.allowSha &&
        !obj->antiBits.objAntiBits.allowSor &&
        !obj->antiBits.objAntiBits.allowNec &&
        !obj->antiBits.objAntiBits.allowCon &&
        !obj->antiBits.objAntiBits.allowPsi &&
        !obj->antiBits.objAntiBits.allowThi &&
        !obj->antiBits.objAntiBits.allowAss &&
        !obj->antiBits.objAntiBits.allowMer &&
        !obj->antiBits.objAntiBits.allowBar)
    {
      sprintf(strn,
"error in obj #%u - no 'use' flags in the first 'anti' bitvector are set -\n"
"                   no one will be able to wear/hold this takeable object\n",
                obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check a bunch of guild crap?

    if (getCheckGuildStuffVal())
    {
      if (obj->extraBits.objExtraBits.noshow)
      {
        sprintf(strn,
"error in object #%u - object in guild is NOSHOW\n",
                obj->objNumber);
        outCheckError(strn, file);

        errors++;
      }

      if (obj->objType == ITEM_TELEPORT)
      {
        if ((obj->objValues[1] != MUDCOMM_ENTER) &&
            (obj->objValues[1] != MUDCOMM_TOUCH))
        {
          sprintf(strn,
"error in object #%u - teleporter in guild with keyword other than 'enter'\n"
"                      or 'touch'\n",
                  obj->objNumber);
          outCheckError(strn, file);

          errors++;
        }
      }
    }

    obj = obj->Next;
  }

 // now, check for rooms that are water rooms and yet have non-floating
 // objects in them - hey, it would be hard to track down..

 // let's check for objects in no_ground rooms while we're at it

 // and hey let's check for transient objects on the ground too

  high = getHighestRoomNumber();

  for (i = getLowestRoomNumber(); i <= high; i++)
  {
    if (room = findRoom(i))
    {
      objHere = room->objectHead;
      while (objHere)
      {
        if (objHere->objectPtr &&
            objHere->objectPtr->extraBits.objExtraBits.transient)
        {
          sprintf(strn,
"error in object #%u - transient object loads on ground in room #%u -\n"
"            object will decay as soon as it loads\n",
                  objHere->objectNumb, i);
          outCheckError(strn, file);

          errors++;
        }

        objHere = objHere->Next;
      }

      if ((room->sectorType == SECT_NO_GROUND) ||
          (room->sectorType == SECT_UNDRWLD_NOGROUND))
      {
        objHere = room->objectHead;
        while (objHere)
        {
          if (!objHere->objectPtr ||
              (objHere->objectPtr &&
               !objHere->objectPtr->extraBits.objExtraBits.levitates))
          {
            sprintf(strn,
"error in object #%u - non-levitating object loads in a room with\n"
"            no ground (#%u)\n",
                    objHere->objectNumb, i);
            outCheckError(strn, file);

            errors++;
          }

          objHere = objHere->Next;
        }
      }


      if ((room->sectorType == SECT_WATER_SWIM) ||
          (room->sectorType == SECT_WATER_NOSWIM) ||
          (room->sectorType == SECT_UNDERWATER) ||
          (room->sectorType == SECT_OCEAN) ||
          (room->sectorType == SECT_UNDRWLD_WATER) ||
          (room->sectorType == SECT_UNDRWLD_NOSWIM) ||
          (room->sectorType == SECT_PLANE_OF_WATER))
      {
        objHere = room->objectHead;
        while (objHere)
        {
          if (!objHere->objectPtr || (objHere->objectPtr &&
              !objHere->objectPtr->extraBits.objExtraBits.itemFloat))
          {
            sprintf(strn,
"error in object #%u - non-floating object loads in a water room (#%u)\n",
                objHere->objectNumb, i);
            outCheckError(strn, file);

            errors++;
          }

          objHere = objHere->Next;
        }
      }
    }
  }

  return errors;
}


//
// checkObjectValueRange : generic function to check value based on various
//                         parameters
//

ulong checkObjectValueRange(const dikuObject *obj, const uchar valNumb,
                            const long lowRange, const long highRange,
                            const long exclude, FILE *file)
{
  ulong errors = 0;
  long val;
  char strn[256], valstrn2[256], valstrn3[256];
  char *valstrn;


 // check tons of crap just cuz we can

  if (!obj || (valNumb >= NUMB_OBJ_VALS))
  {
    _outtext("\n\ncheckObjectValueRange(): error in args\n\n");
    return 0;
  }

  val = obj->objValues[valNumb];
  if ((val == exclude) && (exclude != NO_EXCLUSION)) return 0;

  valstrn = getObjValueStrn(obj->objType, valNumb, val, valstrn2, valstrn3, FALSE, obj);

 // if lowRange == highRange, this value numb can't == val stored in
 // low/highRange

  if ((lowRange == highRange) && (lowRange != NO_EXCLUSION) &&
      (lowRange != NO_LIMIT))
  {
    if (val == lowRange)
    {
      sprintf(strn,
"error in object #%u - val #%u (%s) is invalid (value is %d)\n",
                obj->objNumber, valNumb + 1, valstrn, lowRange);
      outCheckError(strn, file);

      errors++;
    }

    return errors;
  }

 // another special case

  if (highRange == MUST_BE_LOWEST)
  {
    if (val != lowRange)
    {
      sprintf(strn,
"error in object #%u - val #%u (%s) is not equal to %d (required)\n",
                obj->objNumber, valNumb + 1, valstrn, lowRange);
      outCheckError(strn, file);

      errors++;
    }

    return errors;
  }

  if ((lowRange != NO_LIMIT) && (val < lowRange))
  {
    sprintf(strn,
"error in object #%u - val #%u (%s) is too low (below %d)\n",
            obj->objNumber, valNumb + 1, valstrn, lowRange);
    outCheckError(strn, file);

    errors++;
  }

  if ((highRange != NO_LIMIT) && (val > highRange))
  {
    sprintf(strn,
"error in object #%u - val #%u (%s) is too high (above %d)\n",
            obj->objNumber, valNumb + 1, valstrn, highRange);
    outCheckError(strn, file);

    errors++;
  }

  return errors;
}


//
// checkObjValValidLevel
//

ulong checkObjValValidLevel(const dikuObject *obj, const char valNumb,
                            FILE *file)
{
  return checkObjectValueRange(obj, valNumb, 1, 60, NO_EXCLUSION, file);
}


//
// checkObjValValidSpellType
//

ulong checkObjValValidSpellType(const dikuObject *obj, const char valNumb,
                                FILE *file)
{
  return checkObjectValueRange(obj, valNumb, SPELL_LOWEST, SPELL_HIGHEST, -1,
                               file);
}


//
// checkOAWLvWBredund
//

void checkOAWLvWBredund(const dikuObject *obj, const char *pos, FILE *file)
{
  char strn[256];

  sprintf(strn,
"error in object #%u - armor/worn item has 'worn %s' set in\n"
"          val2, but not in 'wear' flags info\n",
          obj->objNumber, pos);
  outCheckError(strn, file);
}


//
// checkOAWLvWBredund2
//

void checkOAWLvWBredund2(const dikuObject *obj, const char *pos, FILE *file)
{
  char strn[256];

  sprintf(strn,
"error in object #%u - armor/worn item has 'worn %s' set in\n"
"          wear flags, but not in val2 (main wear position)\n",
          obj->objNumber, pos);
  outCheckError(strn, file);
}


//
// checkObjArmorWearLocvsWearBits
//

ulong checkObjArmorWearLocvsWearBits(const dikuObject *obj, FILE *file)
{
  objWearBitFlags objWear;
  int val, errors = 0;

  if (!obj || ((obj->objType != ITEM_ARMOR) &&
      (obj->objType != ITEM_WORN)))
  {
    _outtext("\n\nerror in checkObjArmorWear ...\n\n");
    return 0;
  }

  objWear = obj->wearBits.objWearBits;
  val = obj->objValues[1];

 // check wear bits vs loc first

  switch (val)
  {
    case ARMOR_WEAR_FINGER          :
      if (!objWear.wearFinger)
      {
        checkOAWLvWBredund(obj, "on finger", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_NECK            :
      if (!objWear.wearNeck)
      {
        checkOAWLvWBredund(obj, "on neck", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_BODY            :
      if (!objWear.wearBody)
      {
        checkOAWLvWBredund(obj, "on body", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_HEAD            :
      if (!objWear.wearHead)
      {
        checkOAWLvWBredund(obj, "on head", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_LEGS            :
      if (!objWear.wearLegs)
      {
        checkOAWLvWBredund(obj, "on legs", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_FEET            :
      if (!objWear.wearFeet)
      {
        checkOAWLvWBredund(obj, "on feet", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_HANDS           :
      if (!objWear.wearHands)
      {
        checkOAWLvWBredund(obj, "on hands", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_ARMS            :
      if (!objWear.wearArms)
      {
        checkOAWLvWBredund(obj, "on arms", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_ABOUT           :
      if (!objWear.wearAbout)
      {
        checkOAWLvWBredund(obj, "about-body", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_WAIST           :
      if (!objWear.wearWaist)
      {
        checkOAWLvWBredund(obj, "around waist", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_WRIST           :
      if (!objWear.wearWrist)
      {
        checkOAWLvWBredund(obj, "on wrist", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_EYES            :
      if (!objWear.wearEyes)
      {
        checkOAWLvWBredund(obj, "over eyes", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_FACE            :
      if (!objWear.wearFace)
      {
        checkOAWLvWBredund(obj, "on face", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_EAR             :
      if (!objWear.wearEarring)
      {
        checkOAWLvWBredund(obj, "in ear", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_QUIVER          :
      if (!objWear.wearQuiver)
      {
        checkOAWLvWBredund(obj, "as quiver", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_BADGE           :
      if (!objWear.wearBadge)
      {
        checkOAWLvWBredund(obj, "as badge", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_ATTACHBELT      :
      if (!objWear.attachBelt)
      {
        checkOAWLvWBredund(obj, "attached to belt", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_HORSEBODY       :
      if (!objWear.horseBody)
      {
        checkOAWLvWBredund(obj, "on horse's body", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_TAIL            :
      if (!objWear.wearTail)
      {
        checkOAWLvWBredund(obj, "on tail", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_NOSE            :
      if (!objWear.wearNose)
      {
        checkOAWLvWBredund(obj, "in nose", file);

        errors++;
      }

      break;

    case ARMOR_WEAR_HORNS           :
      if (!objWear.wearHorns)
      {
        checkOAWLvWBredund(obj, "on horns", file);

        errors++;
      }

      break;
  }

 // check locs vs wear bits next - each item should only be wearable in
 // one position

  if (objWear.wearFinger && (val != ARMOR_WEAR_FINGER))
  {
    checkOAWLvWBredund2(obj, "on finger", file);
    errors++;
  }

  if (objWear.wearNeck && (val != ARMOR_WEAR_NECK))
  {
    checkOAWLvWBredund2(obj, "on neck", file);
    errors++;
  }

  if (objWear.wearBody && (val != ARMOR_WEAR_BODY))
  {
    checkOAWLvWBredund2(obj, "on body", file);
    errors++;
  }

  if (objWear.wearHead && (val != ARMOR_WEAR_HEAD))
  {
    checkOAWLvWBredund2(obj, "on head", file);
    errors++;
  }

  if (objWear.wearLegs && (val != ARMOR_WEAR_LEGS))
  {
    checkOAWLvWBredund2(obj, "on legs", file);
    errors++;
  }

  if (objWear.wearFeet && (val != ARMOR_WEAR_FEET))
  {
    checkOAWLvWBredund2(obj, "on feet", file);
    errors++;
  }

  if (objWear.wearHands && (val != ARMOR_WEAR_HANDS))
  {
    checkOAWLvWBredund2(obj, "on hands", file);
    errors++;
  }

  if (objWear.wearArms && (val != ARMOR_WEAR_ARMS))
  {
    checkOAWLvWBredund2(obj, "on arms", file);
    errors++;
  }

  if (objWear.wearAbout && (val != ARMOR_WEAR_ABOUT))
  {
    checkOAWLvWBredund2(obj, "about-body", file);
    errors++;
  }

  if (objWear.wearWaist && (val != ARMOR_WEAR_WAIST))
  {
    checkOAWLvWBredund2(obj, "around waist", file);
    errors++;
  }

  if (objWear.wearWrist && (val != ARMOR_WEAR_WRIST))
  {
    checkOAWLvWBredund2(obj, "on wrist", file);
    errors++;
  }

  if (objWear.wearEyes && (val != ARMOR_WEAR_EYES))
  {
    checkOAWLvWBredund2(obj, "on eyes", file);
    errors++;
  }

  if (objWear.wearFace && (val != ARMOR_WEAR_FACE))
  {
    checkOAWLvWBredund2(obj, "on face", file);
    errors++;
  }

  if (objWear.wearEarring && (val != ARMOR_WEAR_EAR))
  {
    checkOAWLvWBredund2(obj, "in ear", file);
    errors++;
  }

  if (objWear.wearQuiver && (val != ARMOR_WEAR_QUIVER))
  {
    checkOAWLvWBredund2(obj, "as quiver", file);
    errors++;
  }

  if (objWear.wearBadge && (val != ARMOR_WEAR_BADGE))
  {
    checkOAWLvWBredund2(obj, "as badge", file);
    errors++;
  }

  if (objWear.wearBack && (val != ARMOR_WEAR_BACK))
  {
    checkOAWLvWBredund2(obj, "on back", file);
    errors++;
  }

  if (objWear.attachBelt && (val != ARMOR_WEAR_ATTACHBELT))
  {
    checkOAWLvWBredund2(obj, "attached to belt", file);
    errors++;
  }

  if (objWear.horseBody && (val != ARMOR_WEAR_HORSEBODY))
  {
    checkOAWLvWBredund2(obj, "on horse's body", file);
    errors++;
  }

  if (objWear.wearTail && (val != ARMOR_WEAR_TAIL))
  {
    checkOAWLvWBredund2(obj, "on tail", file);
    errors++;
  }

  if (objWear.wearNose && (val != ARMOR_WEAR_NOSE))
  {
    checkOAWLvWBredund2(obj, "in nose", file);
    errors++;
  }

  if (objWear.wearHorns && (val != ARMOR_WEAR_HORNS))
  {
    checkOAWLvWBredund2(obj, "on horns", file);
    errors++;
  }

  return errors;
}


//
// checkObjectValues
//

ulong checkObjectValues(FILE *file)
{
  ulong errors = 0;
  dikuObject *obj = objectHead;
  char strn[256];
  uchar otype;

  while (obj)
  {
    otype = obj->objType;

    switch (otype)
    {
      case ITEM_LIGHT      :
       // lights should have at least an hour of light (or -1 for eternal)

        errors += checkObjectValueRange(obj, 2, 1, NO_LIMIT, -1, file);
        break;

      case ITEM_SCROLL     :
      case ITEM_WAND       :
      case ITEM_STAFF      :
      case ITEM_POTION     :

       // check spell level range (common to all 4 types)

        errors += checkObjValValidLevel(obj, 0, file);

       // further break em down..

        switch (otype)
        {
         // check spell type range for scrolls and potions

          case ITEM_SCROLL :
          case ITEM_POTION :
            errors += checkObjValValidSpellType(obj, 1, file);
            errors += checkObjValValidSpellType(obj, 2, file);
            errors += checkObjValValidSpellType(obj, 3, file);
            break;

         // check max charges (has to be at least 1), current charges
         // (has to be at least 0), and spell type for wands and staves

          case ITEM_WAND :
          case ITEM_STAFF :
            errors += checkObjectValueRange(obj, 1, 1, NO_LIMIT, NO_EXCLUSION, file);
            errors += checkObjectValueRange(obj, 2, 0, NO_LIMIT, NO_EXCLUSION, file);
            errors += checkObjValValidSpellType(obj, 3, file);
            break;
        }

        break;

      case ITEM_WEAPON     :
       // weapon type should be within range

        errors += checkObjectValueRange(obj, 0, WEAPON_LOWEST, WEAPON_HIGHEST,
                                        NO_EXCLUSION, file);

       // check damage dice and damage type

        errors += checkObjectValueRange(obj, 1, 1, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, 1, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, WEAP_LOWEST, WEAP_HIGHEST,
                                        NO_EXCLUSION, file);

       // a few values in between are considered invalid..

        errors += checkObjectValueRange(obj, 3, 4, 4, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 5, 5, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 8, 8, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 9, 9, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 10, 10, NO_EXCLUSION, file);

        break;

      case ITEM_FIREWEAPON :
       // range must be at least 1 (or maybe not..  shrug, oh well)

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);

       // rate of fire should be 0 or higher (maybe 1 or higher..)

        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, NO_EXCLUSION, file);

       // check for valid missile type range

        errors += checkObjectValueRange(obj, 3, MISSILE_LOWEST, MISSILE_HIGHEST,
                                        NO_EXCLUSION, file);

        break;

      case ITEM_MISSILE    :
       // check damage dice and missile type

        errors += checkObjectValueRange(obj, 1, 1, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, 1, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, MISSILE_LOWEST, MISSILE_HIGHEST,
                                        NO_EXCLUSION, file);

        break;

//      case ITEM_TREASURE   :
      case ITEM_ARMOR      :
//      case ITEM_WORN       :
       // armor type should be within bounds (temporarily still ac)

/*
        errors += checkObjectValueRange(obj, 0, ARMOR_TYPE_LOWEST,
                                        ARMOR_TYPE_HIGHEST, NO_EXCLUSION, file);
*/
        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);

       // wear location should be within bounds

/*
        errors += checkObjectValueRange(obj, 1, ARMOR_WEAR_LOWEST,
                                        ARMOR_WEAR_HIGHEST, NO_EXCLUSION, file);

       // wear location should match with wear bits (and vice versa) (val2)

        errors += checkObjArmorWearLocvsWearBits(obj, file);
*/

       // armor thickness should be within bounds

        errors += checkObjectValueRange(obj, 3, ARMOR_THICKNESS_LOWEST,
                                        ARMOR_THICKNESS_HIGHEST, NO_EXCLUSION,
                                        file);

       // misc flags should be at least 0

        errors += checkObjectValueRange(obj, 4, 0, NO_LIMIT, NO_EXCLUSION, file);

        break;

//      case ITEM_OTHER      :
//      case ITEM_TRASH      :
//      case ITEM_TRAP       :

      case ITEM_CONTAINER  :
       // weight container can hold should be at least 1

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);

       // container flags should be at least 0

        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, NO_EXCLUSION, file);

       // check for closed container that isn't closeable

        if ((obj->objValues[1] & 4) && (!(obj->objValues[1] & 1)))
        {
          sprintf(strn,
  "error in object #%u - container is closed, but not closeable (val #2)\n",
                  obj->objNumber);
          outCheckError(strn, file);

          errors++;
        }

       // value 3 should be 0 just in case - not anymore

//        errors += checkObjectValueRange(obj, 3, 0, MUST_BE_LOWEST, NO_EXCLUSION, file);

        break;

//      case ITEM_NOTE       :

      case ITEM_DRINKCON   :
       // should be at least 0 current and 1 max drink units and a
       // valid liquid type, poison value should be at least 0 (there is
       // a high value for poisons, too..  dunno what it is)

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, -1, file);
        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, -1, file);
        errors += checkObjectValueRange(obj, 2, LIQ_LOWEST, LIQ_HIGHEST,
                                        NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 0, NO_LIMIT, NO_EXCLUSION, file);

        break;

      case ITEM_KEY        :
       // percentage of breaking should be between 0-100%

        errors += checkObjectValueRange(obj, 1, 0, 100, NO_EXCLUSION, file);
        break;

      case ITEM_FOOD       :
       // should fill for at least 1 hour, poison value should be at least 0
       // (there is a high value on poisons as well..  shrug)

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 0, NO_LIMIT, NO_EXCLUSION, file);
        break;

      case ITEM_MONEY      :
       // money item should have at least 1 of something and no negative
       // values

        errors += checkObjectValueRange(obj, 0, 0, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, 0, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 0, NO_LIMIT, NO_EXCLUSION, file);

        if (!obj->objValues[0] && !obj->objValues[1] && !obj->objValues[2] &&
            !obj->objValues[3])
        {
          sprintf(strn,
  "error in object #%u - money object has no actual money in it\n",
                  obj->objNumber);
          outCheckError(strn, file);

          errors++;
        }

        break;

//      case ITEM_PEN        :
//      case ITEM_BOAT       :
//      case ITEM_BOOK       :
//      case ITEM_BATTERY    :

      case ITEM_TELEPORT   :
       // target room should be at least 0, command should be valid, number
       // of charges should be >0 or -1

        errors += checkObjectValueRange(obj, 0, 0, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 1, MUDCOMM_LOWEST, MUDCOMM_HIGHEST,
                                        NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, 1, NO_LIMIT, -1, file);

        break;

//      case ITEM_TIMER      :
//      case ITEM_VEHICLE    :
//      case ITEM_SHIP       :

      case ITEM_SWITCH     :
       // command should be valid, room # should be at least 0, direction
       // of exit should be valid, val 3 should be 0 or 1

        errors += checkObjectValueRange(obj, 0, MUDCOMM_LOWEST, MUDCOMM_HIGHEST,
                                        NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, EXIT_LOWEST, EXIT_HIGHEST,
                                        NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 0, 1, NO_EXCLUSION, file);

        break;

      case ITEM_QUIVER     :
       // quivers should have a capacity of at least one, no closed and
       // not closeable container flag, missile type should be valid,
       // current amount of missiles should be at least 0

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);

       // quiver flags should be at least 0

        errors += checkObjectValueRange(obj, 1, 0, NO_LIMIT, NO_EXCLUSION, file);

        if ((obj->objValues[1] & 4) && (!(obj->objValues[1] & 1)))
        {
          sprintf(strn,
  "error in object #%u - quiver is closed, but not closeable (val #2)\n",
                  obj->objNumber);
          outCheckError(strn, file);

          errors++;
        }

        errors += checkObjectValueRange(obj, 2, MISSILE_LOWEST, MISSILE_HIGHEST,
                                        NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 3, 0, NO_LIMIT, NO_EXCLUSION, file);

        break;

      case ITEM_PICK       :
       // percentages should be between 0-100%

        errors += checkObjectValueRange(obj, 0, 0, 100, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 1, 0, 100, NO_EXCLUSION, file);

        break;

      case ITEM_INSTRUMENT :
       // instrument type should be valid, level should be valid, break
       // chance should be at least 0 and no more than 1000, min level to
       // use must be valid

        errors += checkObjectValueRange(obj, 0, INSTRUMENT_LOWEST,
                                        INSTRUMENT_HIGHEST, NO_EXCLUSION, file);
        errors += checkObjValValidLevel(obj, 1, file);
        errors += checkObjectValueRange(obj, 2, 0, 1000, NO_EXCLUSION, file);
        errors += checkObjValValidLevel(obj, 3, file);

        break;

      case ITEM_SPELLBOOK  :
       // number of pages must be at least 1

        errors += checkObjectValueRange(obj, 2, 1, NO_LIMIT, NO_EXCLUSION, file);

        break;

      case ITEM_TOTEM      :
       // spheres value must be at least 1

        errors += checkObjectValueRange(obj, 0, 1, NO_LIMIT, NO_EXCLUSION, file);

        break;

      case ITEM_SHIELD     :
       // shield type, shape, size should be within bounds

        errors += checkObjectValueRange(obj, 0, SHIELDTYPE_LOWEST,
                                        SHIELDTYPE_HIGHEST, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 1, SHIELDSHAPE_LOWEST,
                                        SHIELDSHAPE_HIGHEST, NO_EXCLUSION, file);
        errors += checkObjectValueRange(obj, 2, SHIELDSIZE_LOWEST,
                                        SHIELDSIZE_HIGHEST, NO_EXCLUSION, file);
       // val3 is temporarily ac, so should be at least 1
        errors += checkObjectValueRange(obj, 3, 1, NO_LIMIT, NO_EXCLUSION, file);

       // shield thickness should be within bounds

        errors += checkObjectValueRange(obj, 4, ARMOR_THICKNESS_LOWEST,
                                        ARMOR_THICKNESS_HIGHEST, NO_EXCLUSION,
                                        file);

       // shield misc flags should be at least 0

        errors += checkObjectValueRange(obj, 5, 0, NO_LIMIT, NO_EXCLUSION, file);

        break;
    }

    obj = obj->Next;
  }

  return errors;
}


//
// checkObjMaterial
//

ulong checkObjMaterial(FILE *file)
{
  ulong errors = 0;
  dikuObject *obj = objectHead;
  char strn[256];

  while (obj)
  {
    if ((obj->material > MAT_HIGHEST) ||
        (obj->material < MAT_LOWEST))
    {
      sprintf(strn,
"error in object #%u - invalid material type set\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

    obj = obj->Next;
  }

  return errors;
}


//
// checkMobs
//

ulong checkMobs(FILE *file)
{
  ulong errors = 0, i;
  dikuMob *mob = mobHead;
  dikuRoom *room = roomHead;
  questMessage *qstm;
  questQuest *qstq;
  mobHere *mobH;
  char strn[256], strn2[256];
  mobActionBitFlags ma;

  while (mob)
  {
    if (!mob->mobShortName[0])
    {
      sprintf(strn, "error in mob #%u - mob has no short name\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

    if (!mob->keywordListHead)
    {
      sprintf(strn, "error in mob #%u - mob has no keywords\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob level

    if (!scanKeyword("_ignore_", mob->keywordListHead) &&
        ((mob->level < 1) || (mob->level > 60)))
    {
      sprintf(strn, "error in mob #%u - mob's level is out of range (1-60)\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob class

    if ((mob->mobClass < CLASS_LOWEST) || (mob->mobClass > CLASS_HIGHEST))
    {
      sprintf(strn, "error in mob #%u - mob's class is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob alignment

    if ((mob->alignment < -1000) || (mob->alignment > 1000))
    {
      sprintf(strn, "error in mob #%u - mob's alignment is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob species

    if (!strcmp(getMobSpeciesStrn(mob->mobSpecies), "unrecog. species"))
    {
      sprintf(strn, "error in mob #%u - mob's species is invalid\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob hometown

    if ((mob->mobHometown < HOME_LOWEST) || (mob->mobHometown > HOME_HIGHEST))
    {
      sprintf(strn, "error in mob #%u - mob's hometown is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob position and default pos

    if ((mob->position < POSITION_LOWEST_LEGAL) ||
        (mob->position > POSITION_HIGHEST))
    {
      sprintf(strn, "error in mob #%u - mob's position is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

    if ((mob->defaultPos < POSITION_LOWEST_LEGAL) ||
        (mob->defaultPos > POSITION_HIGHEST))
    {
      sprintf(strn, "error in mob #%u - mob's default pos is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob's sex

    if ((mob->sex < SEX_LOWEST) || (mob->sex > SEX_HIGHEST))
    {
      sprintf(strn, "error in mob #%u - mob's sex is out of range\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check if mob is set aggro, but has a non-standing pos

    ma = mob->actionBits.mobActionBits;

    if (((mob->defaultPos < POSITION_STANDING) || (mob->position < POSITION_STANDING)) &&
         isAggro(mob))
    {
      sprintf(strn, "error in mob #%u - mob is aggro, but has a position/default position less\n"
                    "     than standing (non-standing mobs don't attack anyone)\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // for the hell of it, check class as well..  a non-standing caster is gonna be pretty
   // ill-prepared

    if (((mob->defaultPos < POSITION_STANDING) || (mob->position < POSITION_STANDING)) &&
         castingClass(mob->mobClass))
    {
      sprintf(strn, "error in mob #%u - mob is a caster, but has a position/default position less\n"
                    "     than standing (non-standing mobs can't spell up)\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

   // check mob quest info

    if (mob->questPtr)
    {
      if (!mob->questPtr->messageHead && !mob->questPtr->questHead)
      {
        sprintf(strn,
  "error in mob #%u - mob has a quest, but no messages or actual quests\n",
                mob->mobNumber);
        outCheckError(strn, file);

        errors++;
      }
      else
      {
        qstm = mob->questPtr->messageHead;
        while (qstm)
        {
          if (!qstm->keywordListHead)
          {
            sprintf(strn,
"error in mob #%u - a quest message has no keywords\n",
                    mob->mobNumber);
            outCheckError(strn, file);

            errors++;
          }

          if (!qstm->questMessageHead)
          {
            sprintf(strn,
"error in mob #%u - a quest message (%s) has no message\n",
                    mob->mobNumber,
                    qstm->keywordListHead ?
                      getReadableKeywordStrn(qstm->keywordListHead, strn2) :
                      "no keywords");
            outCheckError(strn, file);

            errors++;
          }

          qstm = qstm->Next;
        }

        qstq = mob->questPtr->questHead;
        while (qstq)
        {
          if (!qstq->questReplyHead)
          {
            sprintf(strn,
"error in mob #%u - a quest's quest info has no response when finishing\n",
                    mob->mobNumber);
            outCheckError(strn, file);

            errors++;
          }

         // all quests require the mob to get something from the players..
         // but the players won't always get something from the mob

          if (!qstq->questPlayGiveHead)
          {
            sprintf(strn,
"error in mob #%u - a quest's quest info requires nothing from players\n"
"                   (quest is impossible to complete)\n",
                    mob->mobNumber);
            outCheckError(strn, file);

            errors++;
          }

          qstq = qstq->Next;
        }
      }
    }

   // check mob shop info

    if (mob->shopPtr)
    {
      if (!mob->shopPtr->producedItemList[0] && !mob->shopPtr->tradedItemList[0])
      {
        sprintf(strn,
"error in mob #%u - mob has a shop, but doesn't buy or sell anything\n",
                mob->mobNumber);
        outCheckError(strn, file);

        errors++;
      }
      else
      {
        for (i = 0;
             (i < MAX_NUMBSHOPITEMTYPES) && mob->shopPtr->tradedItemList[i];
             i++)
        {
          if ((mob->shopPtr->tradedItemList[i] < ITEM_LOWEST) ||
              (mob->shopPtr->tradedItemList[i] > ITEM_HIGHEST))
          {
            sprintf(strn,
"error in mob #%u - shop has one or more invalid item types in bought list\n",
                    mob->mobNumber);
            outCheckError(strn, file);

            errors++;
            break;  // no need to spam ceaselessly
          }
        }
      }
    }

   // check guild stuff?

    if (getCheckGuildStuffVal())
    {
      if (mob->actionBits.mobActionBits.aggressive ||
          mob->actionBits.mobActionBits.aggresEvil ||
          mob->actionBits.mobActionBits.aggresGood ||
          mob->actionBits.mobActionBits.aggresNeut ||
          mob->actionBits.mobActionBits.aggrEvilR ||
          mob->actionBits.mobActionBits.aggrGoodR ||
          mob->actionBits.mobActionBits.aggrOutcast ||
          mob->actionBits.mobActionBits.hunter)
      {
        sprintf(strn,
"error in mob #%u - guild mob has an aggro/hunter bit set\n",
                mob->mobNumber);
        outCheckError(strn, file);

        errors++;
      }

      if (mob->alignment != 0)
      {
        sprintf(strn,
"error in mob #%u - guild mob has non-neutral alignment\n",
                mob->mobNumber);
        outCheckError(strn, file);

        errors++;
      }
    }

    mob = mob->Next;
  }

  while (room)
  {
    mobH = room->mobHead;
    while (mobH)
    {
      if (mobH->following && mobH->mobPtr &&
          !mobH->mobPtr->actionBits.mobActionBits.sentinel)
      {
        sprintf(strn,
"error with mob #%u in room #%u - mob is following another, but doesn't\n"
"   have its SENTINEL action flag set (follower will wander)\n",
                mobH->mobNumb, room->roomNumber);
        outCheckError(strn, file);

        errors++;
      }

      mobH = mobH->Next;
    }

    room = room->Next;
  }

  return errors;
}


//
// checkObjectDescs
//

ulong checkObjectDescs(FILE *file)
{
  ulong errors = 0;
  dikuObject *obj = objectHead;
  char strn[256];

  while (obj)
  {
    if (!obj->extraDescHead)
    {
      sprintf(strn,
"error in object #%u - object has no extra descs\n",
              obj->objNumber);
      outCheckError(strn, file);

      errors++;
    }

    obj = obj->Next;
  }

  return errors;
}


//
// checkMobDescs
//

ulong checkMobDescs(FILE *file)
{
  ulong errors = 0;
  dikuMob *mob = mobHead;
  char strn[256];

  while (mob)
  {
    if (!mob->mobDescHead)
    {
      sprintf(strn,
"error in mob #%u - mob has no description\n",
              mob->mobNumber);
      outCheckError(strn, file);

      errors++;
    }

    mob = mob->Next;
  }

  return errors;
}


//
// checkEdesc
//

INLINEDEF char checkEdesc(const extraDesc *descNode)
{
  if (!descNode) return FALSE;

  if (!descNode->keywordListHead) return EDESC_NO_KEYWORDS;
  if (!descNode->extraDescStrnHead) return EDESC_NO_DESC;

  return 0;
}


//
// checkAllEdescs
//

ulong checkAllEdescs(FILE *file)
{
  ulong errors = 0;
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  extraDesc *descNode;
  char strn[256];


  while (room)
  {
    descNode = room->extraDescHead;
    while (descNode)
    {
      switch (checkEdesc(descNode))
      {
        case EDESC_NO_KEYWORDS :
                 sprintf(strn,
"error in extra desc for room #%u - no keywords\n", room->roomNumber);
                 outCheckError(strn, file);
                 errors++;
                 break;

        case EDESC_NO_DESC :
                 sprintf(strn,
"error in extra desc for room #%u - no description\n",
                         room->roomNumber);
                 outCheckError(strn, file);
                 errors++;
                 break;
      }

      descNode = descNode->Next;
    }

    room = room->Next;
  }

  while (obj)
  {
    descNode = obj->extraDescHead;
    while (descNode)
    {
      switch (checkEdesc(descNode))
      {
        case EDESC_NO_KEYWORDS :
                 sprintf(strn,
"error in extra desc for obj #%u - no keywords\n", obj->objNumber);
                 outCheckError(strn, file);
                 errors++;
                 break;

        case EDESC_NO_DESC :
                 sprintf(strn,
"error in extra desc for obj #%u - no description\n",
                         obj->objNumber);
                 outCheckError(strn, file);
                 errors++;
                 break;
      }

      descNode = descNode->Next;
    }

    obj = obj->Next;
  }

  return errors;
}


//
// checkAll : returns TRUE if any errors found
//

char checkAll(void)
{
  ulong errors = 0;
  char strn[256];
  FILE *outFile = NULL;

  tmpCheck = NULL;

  if (getSaveCheckLogVal()) outFile = fopen("check.log", "wt");
  if (getPauseCheckScreenfulVal()) tmpCheck = fopen("checklog.tmp", "wt");

  _outtext("\n");

  if (getCheckExitDescVal()) errors += checkExitDescs(outFile);
  if (getCheckObjDescVal()) errors += checkObjectDescs(outFile);
  if (getCheckMobDescVal()) errors += checkMobDescs(outFile);
  if (getCheckEdescVal()) errors += checkAllEdescs(outFile);
  if (getCheckFlagsVal()) errors += checkAllFlags(outFile);
  if (getCheckLoneRoomVal()) errors += checkLoneRooms(outFile);
  if (getCheckRoomVal()) errors += checkRooms(outFile);
  if (getCheckMissingKeysVal()) errors += checkMissingKeys(outFile);
  if (getCheckLoadedVal()) errors += checkLoaded(outFile);
  if (getCheckExitVal()) errors += checkExits(outFile);
  if (getCheckObjVal()) errors += checkObjects(outFile);
  if (getCheckObjValuesVal()) errors += checkObjectValues(outFile);
  if (getCheckObjMaterialVal()) errors += checkObjMaterial(outFile);
  if (getCheckMobVal()) errors += checkMobs(outFile);

  if (errors)
  {
    if (errors == 1) strcpy(strn, "\nOne error found while checking entities.\n\n");
    else sprintf(strn, "\n%u errors found while checking entities.\n\n", errors);

    outCheckError(strn, outFile);

    if (outFile) fclose(outFile);
    if (tmpCheck)
    {
      fclose(tmpCheck);
      if (getPauseCheckScreenfulVal())
        dumpTextFile("checklog.tmp", FALSE);
    }

   // delete temp check file (unlink returns TRUE if it fails)

   // Commented out by Veygoth - Unlink does not exist in DJGPP.
   // if (unlink("checklog.tmp")) remove("checklog.tmp");

    return TRUE;
  }
  else /*outCheckError*/_outtext("No errors found in entities.\n\n");//, outFile);

  if (outFile) fclose(outFile);
  if (tmpCheck) fclose(tmpCheck);

  // Commented out by Veygoth - unlink does not exist.
  // if (unlink("checklog.tmp")) remove("checklog.tmp");

  return FALSE;
}
