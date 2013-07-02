//
//  File: mobhere.cpp    originally part of dikuEdit
//
//  Usage: tons o functions for manipulating mobHere structures
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "mobhere.h"

using namespace std;

extern dikuRoom *roomHead;
extern dikuMob *mobHead;
extern editableListNode *inventory;
extern ulong numbMobs;

//
// getNumbMobHereNodes : Returns the number of nodes in a specific
//                       mobHere list
//
//    *mobHead : take a wild guess, bucko
//

ulong getNumbMobHereNodes(mobHere *mobHead)
{
  ulong i = 0;


  while (mobHead)
  {
    i++;

    mobHead = mobHead->Next;
  }

  return i;
}


//
// findMobHere : Returns the address of the mobHere node that has the
//               requested mobNumber (if any)
//
//     mobNumber : mob number to look for in all mobHere nodes everywhere
//  roomListHead : head of rooms to search through
//      roomNumb : points to variable to update with room number
// checkOnlyThisRoom
//               : self-explanatory
//

mobHere *findMobHere(const ulong mobNumber, dikuRoom *roomListHead,
                     ulong *roomNumb, const char checkOnlyThisRoom)
{
  mobHere *mobHereNode;


  while (roomListHead)
  {
    mobHereNode = roomListHead->mobHead;

    while (mobHereNode)
    {
      if (mobHereNode->mobNumb == mobNumber)
      {
        if (roomNumb) *roomNumb = roomListHead->roomNumber;
        return mobHereNode;
      }

      mobHereNode = mobHereNode->Next;
    }

    if (checkOnlyThisRoom) break;

    roomListHead = roomListHead->Next;
  }

  return NULL;
}


//
// copyMobHere : Copies just one mobHere, returning the address of the copy
//
//    *srcMobHere : take a wild guess, bucko
//

mobHere *copyMobHere(mobHere *srcMobHere, const char incLoaded)
{
  mobHere *newMobHere;
  objectHere *obj;
  char i;


  if (!srcMobHere) return NULL;

  newMobHere = new mobHere;
  if (!newMobHere)
  {
    cout << "\n\nERROR: copyMobHere() - ran out of mem alloc'ing new "
         << " mobHere node.  Aborting..\n\n";

    return NULL;
  }

  newMobHere->Next = NULL;


 // first, the easy stuff

  memcpy(newMobHere, srcMobHere, sizeof(mobHere));

 // now, the not quite as easy but still relatively simple stuff

  newMobHere->objectHead = copyObjHereList(srcMobHere->objectHead, incLoaded);
//  newMobHere->equipHead = copyObjHereList(srcMobHere->equipHead, incLoaded);

  for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
  {
    obj = srcMobHere->equipment[i];

    if (obj) newMobHere->equipment[i] = copyObjHere(obj, incLoaded);
    else newMobHere->equipment[i] = NULL;
  }

  if (incLoaded)
  {
    addEntity(ENTITY_MOB, newMobHere->mobNumb);
    numbMobs++;
    createPrompt();
  }

  return newMobHere;
}


//
// copyMobHereList : Copies a mobHere list, returning the address to the head
//                   of the copy
//
//    *srcMobHere : take a wild guess, bucko
//

mobHere *copyMobHereList(mobHere *srcMobHere, const char incLoaded)
{
  mobHere *newMobHere, *prevMobHere = NULL, *headMobHere = NULL;
  objectHere *obj;
  char i;


  if (!srcMobHere) return NULL;

 // hmm...  should just use "copyMobHere" here.  not sure why I'm not

  while (srcMobHere)
  {
    newMobHere = new mobHere;
    if (!newMobHere)
    {
      cout << "\n\nERROR: copyMobHereList() - ran out of mem alloc'ing new "
           << " mobHere node.  Aborting..\n\n";

      return NULL;
    }

    newMobHere->Next = NULL;

    if (!headMobHere) headMobHere = newMobHere;

    if (prevMobHere) prevMobHere->Next = newMobHere;
    prevMobHere = newMobHere;


   // first, the easy stuff

    memcpy(newMobHere, srcMobHere, sizeof(mobHere));

   // now, the not quite as easy but still relatively simple stuff

    newMobHere->objectHead = copyObjHereList(srcMobHere->objectHead, incLoaded);
//    newMobHere->equipHead = copyObjHereList(srcMobHere->equipHead, incLoaded);

    for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
    {
      obj = srcMobHere->equipment[i];

      if (obj) newMobHere->equipment[i] = copyObjHere(obj, incLoaded);
      else newMobHere->equipment[i] = NULL;
    }

    if (incLoaded)
    {
      addEntity(ENTITY_MOB, newMobHere->mobNumb);
      numbMobs++;
    }

    srcMobHere = srcMobHere->Next;
  }

  if (incLoaded) createPrompt();

  return headMobHere;
}


//
// deleteMobHereList : Deletes a mobHere list
//
//    *srcMobHere : take a wild guess, bucko
//

void deleteMobHereList(mobHere *srcMobHere, const char decLoaded)
{
  mobHere *nextMobHere;
  char i;


  while (srcMobHere)
  {
    nextMobHere = srcMobHere->Next;

   // delete it

    deleteObjHereList(srcMobHere->objectHead, decLoaded);
//    deleteObjHereList(srcMobHere->equipHead, decLoaded);
    for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
    {
      if (srcMobHere->equipment[i])
        deleteObjHere(srcMobHere->equipment[i], decLoaded);
    }

    if (decLoaded)
    {
      decEntity(ENTITY_MOB, srcMobHere->mobNumb);
      numbMobs--;
    }

    delete srcMobHere;

    srcMobHere = nextMobHere;
  }

  if (decLoaded) createPrompt();
}


//
// deleteMobHere : Deletes one mobHere node
//
//    mobHere : take a wild guess, bucko
//

void deleteMobHere(mobHere *mobHere, const char decLoaded)
{
  char i;

  if (!mobHere) return;

  deleteObjHereList(mobHere->objectHead, decLoaded);
//  deleteObjHereList(mobHere->equipHead, decLoaded);

  for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
  {
    if (mobHere->equipment[i])
      deleteObjHere(mobHere->equipment[i], decLoaded);
  }

  if (decLoaded)
  {
    decEntity(ENTITY_MOB, mobHere->mobNumb);
    numbMobs--;
    createPrompt();
  }

  delete mobHere;
}


//
// checkForMobHeresofType : Checks for any mobHere nodes whereever they may
//                          show up (of a certain type..)  TRUE if found..
//

char checkForMobHeresofType(const dikuMob *mobType)
{
  dikuRoom *room = roomHead;
  mobHere *mobH;
  editableListNode *edit = inventory;


  while (room)
  {
    mobH = room->mobHead;

    while (mobH)
    {
      if (mobH->mobPtr == mobType) return TRUE;

      mobH = mobH->Next;
    }

    room = room->Next;
  }

 // check inventory

  while (edit)
  {
    if ((edit->entityType == ENTITY_MOB) &&
        (((mobHere *)(edit->entityPtr))->mobPtr == mobType))
      return TRUE;

    edit = edit->Next;
  }

  return FALSE;
}


//
// addMobHeretoList
//

void addMobHeretoList(mobHere **mobListHead, mobHere *mobToAdd)
{
  mobHere *mob;

  if (!*mobListHead) *mobListHead = mobToAdd;
  else
  {
    mob = *mobListHead;

    while (mob && mob->Next)
    {
      mob = mob->Next;
    }

    mob->Next = mobToAdd;
  }

  mobToAdd->Next = NULL;
}


//
// deleteAllMobHereofTypeInList :
//        Runs through a list pointed to by mobHereHead, deleting all
//        mobHeres that match mobType.  Fun
//

void deleteAllMobHereofTypeInList(mobHere **mobHereHead,
                                  const dikuMob *mobType, const char decLoaded)
{
  mobHere *mobHere = *mobHereHead, *prevMob, *nextMob;


  while (mobHere)
  {
    nextMob = mobHere->Next;

    if (mobHere->mobPtr == mobType)
    {
      if (mobHere == *mobHereHead) *mobHereHead = mobHere->Next;
      else
      {
        prevMob = *mobHereHead;
        while (prevMob && (prevMob->Next != mobHere))
        {
          prevMob = prevMob->Next;
        }

        if (!prevMob)
        {
          _outtext(
"\n\nERROR: deleteAllMobHereofTypeInList() - notify author\n\n");

          return;
        }

        prevMob->Next = mobHere->Next;
      }

      deleteMobHere(mobHere, decLoaded);
    }

    mobHere = nextMob;
  }
}


//
// deleteAllMobHeresofType : Deletes all mobHere nodes that match mobType -
//                           all nodes in the editor, that is
//
//   currently only deletes mobHeres in rooms, will be changed soon - should
//   also check inventory
//

void deleteAllMobHeresofType(const dikuMob *mobType, const char decLoaded)
{
  dikuRoom *room = roomHead;


 // delete mobs laying about and shit that mobs are carrying and equipping
 // from every room

  while (room)
  {
    deleteAllMobHereofTypeInList(&(room->mobHead), mobType, decLoaded);

    room = room->Next;
  }
}


//
// deleteMobHereinList : Deletes a specific mobHere in a list that starts
//                       at *mobHead, updating whatever needs to be updated for
//                       the list to be valid.
//

void deleteMobHereinList(mobHere **mobHead, mobHere *mob, const char decLoaded)
{
  mobHere *prevMob;


  if (mob == (*mobHead))
  {
    *mobHead = (*mobHead)->Next;

    deleteMobHere(mob, decLoaded);
  }
  else
  {
    prevMob = *mobHead;

    while (prevMob && (prevMob->Next != mob))
    {
      prevMob = prevMob->Next;
    }

   // couldn't find an mob that points to mob being deleted - error

    if (!prevMob)
    {
      _outtext("\n\nERROR: deleteMobHereinList() - notify author\n\n");

      return;
    }

    prevMob->Next = mob->Next;

    deleteMobHere(mob, decLoaded);
  }
}


//
// resetMobHere : "Resets" mobHere records by setting mob->mobNumb to
//                newNumb if it equals oldNumb.  *room points to the head of
//                the room list to run through.
//
//   oldNumb : see above
//   newNumb : ditto
//     *room : yep
//

void resetMobHere(const ulong oldNumb, const ulong newNumb, dikuRoom *room)
{
  mobHere *mob;


  if (oldNumb == newNumb) return;

  while (room)
  {
    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == oldNumb) mob->mobNumb = newNumb;

      mob = mob->Next;
    }

    room = room->Next;
  }
}


//
// eqSlotFull : kinda useless now that there's an array instead of a linked
//              list, but hey, here it is
//

char eqSlotFull(const mobHere *mob, const char slot)
{
  if (!mob)
  {
    cout << "\n\neqSlotFull(): NULL mobHere pointer passed\n\n";

    return TRUE;  // damned if we do, damned if we don't
  }

  if ((slot < WEAR_LOW) || (slot > WEAR_TRUEHIGH))
  {
    cout << "\n\neqSlotFull(): slot val too low or high\n\n";

    return TRUE;
  }

  if (mob->equipment[slot]) return TRUE;
  else return FALSE;
}


//
// eqRestrict
//

char eqRestrict(const dikuMob *mob, const objExtraBitFlags objExtra,
                const objAntiBitFlags objAnti, const objAnti2BitFlags objAnti2)
{
  char mobcl;
  char species[MAX_SPECIES_LEN];
  long align;

//  if (!mob) return EQ_ERROR;
  if (!mob) return EQ_WEARABLE;  // bleh..  have to do it

  mobcl = mob->mobClass;
  align = mob->alignment;

  if (getCheckMobClassEqVal() && !objAnti.allowAll)
  {
    switch (mobcl)
    {
      case CLASS_WARRIOR : if (!objAnti.allowWar) return EQ_RESCLASS;
                           break;
      case CLASS_RANGER  : if (!objAnti.allowRan) return EQ_RESCLASS;
                           break;
      case CLASS_PSIONICIST:
      case CLASS_ENSLAVER:
                           if (!objAnti.allowPsi) return EQ_RESCLASS;
                           break;
      case CLASS_PALADIN : if (!objAnti.allowPal) return EQ_RESCLASS;
                           break;
      case CLASS_ANTIPALADIN: if (!objAnti.allowAnP) return EQ_RESCLASS;
                           break;
      case CLASS_CLERIC  : if (!objAnti.allowCle) return EQ_RESCLASS;
                           break;
      case CLASS_MONK    : if (!objAnti.allowMon) return EQ_RESCLASS;
                           break;
      case CLASS_DRUID   : if (!objAnti.allowDru) return EQ_RESCLASS;
                           break;
      case CLASS_SHAMAN  : if (!objAnti.allowSha) return EQ_RESCLASS;
                           break;
      case CLASS_SORCERER:
      case CLASS_CHRONOMANCER:
      case CLASS_ILLUSIONIST:
                           if (!objAnti.allowSor) return EQ_RESCLASS;
                           break;
      case CLASS_NECROMANCER: if (!objAnti.allowNec) return EQ_RESCLASS;
                           break;
      case CLASS_CONJURER:
      case CLASS_ELEM_AIR:
      case CLASS_ELEM_FIRE:
      case CLASS_ELEM_WATER:
      case CLASS_ELEM_EARTH:
                           if (!objAnti.allowCon) return EQ_RESCLASS;
                           break;
      case CLASS_THIEF   : if (!objAnti.allowThi) return EQ_RESCLASS;
                           break;
      case CLASS_ASSASSIN: if (!objAnti.allowAss) return EQ_RESCLASS;
                           break;
      case CLASS_MERCENARY: if (!objAnti.allowMer) return EQ_RESCLASS;
                           break;
      case CLASS_BARD:
      case CLASS_WARLOCK:
                           if (!objAnti.allowBar) return EQ_RESCLASS;
                           break;
    }
  }

  if (getCheckMobRaceEqVal() && !objAnti.allowAll)
  {
    strcpy(species, mob->mobSpecies);

    if (objAnti.antiHuman && !strcmp(species, MOB_HUMAN)) return EQ_RESRACE;
    if (objAnti.antiGrey && !strcmp(species, MOB_GREY_ELF)) return EQ_RESRACE;
    if (objAnti.antiHalfE && !strcmp(species, MOB_HALF_ELF)) return EQ_RESRACE;
    if (objAnti.antiDwarf && !strcmp(species, MOB_MOUNT_DWARF)) return EQ_RESRACE;
    if (objAnti.antiHalfl && !strcmp(species, MOB_HALFLING)) return EQ_RESRACE;
    if (objAnti.antiGnome && !strcmp(species, MOB_GNOME)) return EQ_RESRACE;
    if (objAnti.antiBarb && !strcmp(species, MOB_BARBARIAN)) return EQ_RESRACE;
    if (objAnti.antiDuerg && !strcmp(species, MOB_DUERGAR)) return EQ_RESRACE;
    if (objAnti.antiDrow && !strcmp(species, MOB_DROW_ELF)) return EQ_RESRACE;
    if (objAnti.antiTroll && !strcmp(species, MOB_TROLL)) return EQ_RESRACE;
    if (objAnti.antiOgre && !strcmp(species, MOB_OGRE)) return EQ_RESRACE;
    if (objAnti.antiIllit && !strcmp(species, MOB_ILLITHID)) return EQ_RESRACE;
    if (objAnti.antiOrc && !strcmp(species, MOB_ORC)) return EQ_RESRACE;
    if (objAnti2.antiThri && !strcmp(species, MOB_THRIKREEN)) return EQ_RESRACE;
    if (objAnti2.antiCentaur && !strcmp(species, MOB_CENTAUR)) return EQ_RESRACE;
    if (objAnti2.antiMinotaur && !strcmp(species, MOB_MINOTAUR)) return EQ_RESRACE;
// enable when new races are in ...
/*
    if (objAnti2.antiAquaelf && !strcmp(species, MOB_AQUAELF)) return EQ_RESRACE;
    if (objAnti2.antiSahaugin && !strcmp(species, MOB_SAHAUGIN)) return EQ_RESRACE;
*/

    if (objAnti.antiEvilR &&
        (!strcmp(species, MOB_DUERGAR) || !strcmp(species, MOB_DROW_ELF) ||
         !strcmp(species, MOB_TROLL)   || !strcmp(species, MOB_OGRE) ||
         !strcmp(species, MOB_ILLITHID)|| !strcmp(species, MOB_ORC)))
      return EQ_RESRACE;

    if (objAnti.antiGoodR &&
        (!strcmp(species, MOB_HUMAN)    || !strcmp(species, MOB_GREY_ELF) ||
         !strcmp(species, MOB_HALF_ELF) || !strcmp(species, MOB_MOUNT_DWARF) ||
         !strcmp(species, MOB_HALFLING) || !strcmp(species, MOB_GNOME) ||
         !strcmp(species, MOB_BARBARIAN)|| !strcmp(species, MOB_CENTAUR)))
      return EQ_RESRACE;
  }

  if (getCheckMobSexEqVal() && !objAnti.allowAll)
  {
    if (objAnti2.antiMale && (mob->sex == SEX_MALE)) return EQ_RESSEX;
    if (objAnti2.antiFemale && (mob->sex == SEX_FEMALE)) return EQ_RESSEX;
    if (objAnti2.antiNeuter && (mob->sex == SEX_NEUTER)) return EQ_RESSEX;
  }

  if (objExtra.antiEvil && (align <= -350)) return EQ_RESALIGN;

  if (objExtra.antiNeutral && ((align > -350) && (align < 350)))
    return EQ_RESALIGN;

  if (objExtra.antiGood && (align >= 350)) return EQ_RESALIGN;

  if (objExtra.good && (align < 350)) return EQ_RESALIGN;

  return EQ_WEARABLE;
}


//
// handsFree
//

char handsFree(const mobHere *mob)
{
  char hands;

  if (!mob) return 0;

  if (mob->mobPtr && !strcmp(mob->mobPtr->mobSpecies, MOB_THRIKREEN)) hands = 4;
  else hands = 2;

  if (mob->equipment[HOLD]) hands--;
  if (mob->equipment[HOLD2]) hands--;
  if (mob->equipment[WIELD_PRIMARY]) hands--;
  if (mob->equipment[WIELD_SECOND]) hands--;
  if (mob->equipment[WIELD_THIRD]) hands--;
  if (mob->equipment[WIELD_FOURTH]) hands--;
  if (mob->equipment[WEAR_SHIELD]) hands--;
  if (mob->equipment[WIELD_TWOHANDS]) hands -= 2;
  if (mob->equipment[WIELD_TWOHANDS2]) hands -= 2;

  if (hands < 0) hands = 0;  // could happen

  return hands;
}


//
// getMobHereEquipSlot : if mobHere can wear obj, return slot.  if not,
//                       return error
//
//                       if where is not a valid slot and obj is non-NULL,
//                       find the first slot that the item fits into and
//                       return the pos
//

char getMobHereEquipSlot(const mobHere *mob, const dikuObject *obj,
                         const char where)
{
  objWearBitFlags objWear;
  objExtraBitFlags objExtra;
  char ch, noEq = FALSE, mobSpecies[MAX_SPECIES_LEN];


  if (!mob) return EQ_ERROR;

 // no flags available, so simply check for objects already equipped in
 // this slot

  if (!obj)
  {
    if ((where < WEAR_LOW) || (where > WEAR_TRUEHIGH)) return EQ_ERROR;

    if (mob->equipment[where]) return EQ_SLOT_FILLED;

    return where;
  }

  if (mob->mobPtr) strcpy(mobSpecies, mob->mobPtr->mobSpecies);
  else memset(mobSpecies, 0, sizeof(char) * MAX_SPECIES_LEN);

  objWear = obj->wearBits.objWearBits;
  objExtra = obj->extraBits.objExtraBits;

  if ((!objWear.wearFinger)  && (!objWear.wearNeck)  &&
      (!objWear.wearBody)    && (!objWear.wearHead)  &&
      (!objWear.wearLegs)    && (!objWear.wearFeet)  &&
      (!objWear.wearHands)   && (!objWear.wearArms)  &&
      (!objWear.wearShield)  && (!objWear.wearAbout) &&
      (!objWear.wearWaist)   && (!objWear.wearWrist) &&
      (!objWear.wield)       && (!objWear.hold)      &&
      (!objWear.wearEyes)    && (!objWear.wearFace)  &&
      (!objWear.wearEarring) && (!objWear.wearQuiver) &&
      (!objWear.wearBadge)   && (!objWear.wearBack)  &&
      (!objWear.attachBelt)  && (!objWear.horseBody) &&
      (!objWear.wearTail)    && (!objWear.wearNose)  &&
      (!objWear.wearHorns)   && (!objExtra.wholeBody) &&
      (!objExtra.wholeHead)  && (!objExtra.twoHands))
  {
    return EQ_NO_WEARBITS;
  }

  ch = eqRestrict(mob->mobPtr, objExtra, obj->antiBits.objAntiBits,
                  obj->anti2Bits.objAnti2Bits);
  if (ch) return ch;

 // if where var is valid, check if the location specified matches the wear
 // bits available on the object

  if ((where >= WEAR_LOW) && (where <= WEAR_TRUEHIGH))
  {
   // let's check all the thri-kreen/centaur/minotaur crap here..
   // better way to check this crap is by slot

    switch (where)
    {
      case WIELD_THIRD     :
      case WIELD_FOURTH    :
      case WEAR_ARMS_2     :
      case WEAR_HANDS_2    :
      case WEAR_WRIST_LR   :
      case WEAR_WRIST_LL   : if (strcmp(mobSpecies, MOB_THRIKREEN))
                               return EQ_WRONGRACE;
                             break;

      case WEAR_FINGER_L   :
      case WEAR_FINGER_R   :
      case WEAR_EARRING_L  :
      case WEAR_EARRING_R  : if (!strcmp(mobSpecies, MOB_THRIKREEN))
                               return EQ_RACECANTUSE;
                             break;

      case WEAR_FEET       : if (!strcmp(mobSpecies, MOB_THRIKREEN) ||
                                 !strcmp(mobSpecies, MOB_MINOTAUR) ||
                                 (!strcmp(mobSpecies, MOB_CENTAUR) &&
                            !scanKeyword("horseshoe", obj->keywordListHead)))
                              return EQ_RACECANTUSE;
                             break;

      case WEAR_WHOLEHEAD  :
      case WEAR_HEAD       : if (!strcmp(mobSpecies, MOB_MINOTAUR))
                               return EQ_RACECANTUSE;
                             break;

      case WEAR_BODY       : if (!strcmp(mobSpecies, MOB_THRIKREEN))
                               return EQ_RACECANTUSE;

                             if (objExtra.wholeBody &&
                                 (!strcmp(mobSpecies, MOB_CENTAUR) ||
                                  !strcmp(mobSpecies, MOB_MINOTAUR)))
                               return EQ_RACECANTUSE;
                             break;

      case WEAR_WHOLEBODY  : if (!strcmp(mobSpecies, MOB_THRIKREEN) ||
                                 !strcmp(mobSpecies, MOB_CENTAUR) ||
                                 !strcmp(mobSpecies, MOB_MINOTAUR))
                               return EQ_RACECANTUSE;
                             break;

      case WEAR_HORSE_BODY : if (strcmp(mobSpecies, MOB_CENTAUR))
                               return EQ_WRONGRACE;
                             break;

      case WEAR_TAIL       : if (strcmp(mobSpecies, MOB_MINOTAUR) &&
                                 strcmp(mobSpecies, MOB_CENTAUR))
                               return EQ_WRONGRACE;
                             break;

      case WEAR_HORN       :
      case WEAR_NOSE       : if (strcmp(mobSpecies, MOB_MINOTAUR))
                               return EQ_WRONGRACE;
                             break;

      case WEAR_LEGS       : if (!strcmp(mobSpecies, MOB_CENTAUR))
                               return EQ_RACECANTUSE;
                             break;
    }

    if ((where == WEAR_FEET_REAR) || (where == WEAR_LEGS_REAR))
      return EQ_WRONGRACE;  // not implemented, apparently

   // in this first chunk, we just check if the object has the proper
   // wear bits set for the particular slot requested (if not, set noEq)

    switch (where)
    {
      case WEAR_FINGER_L :
      case WEAR_FINGER_R : if (!objWear.wearFinger) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_NECK_1   :
      case WEAR_NECK_2   : if (!objWear.wearNeck) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_BODY     : if (!objWear.wearBody && !objExtra.wholeBody)
                             noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_HEAD     : if ((!objWear.wearHead) && (!objExtra.wholeHead))
                             noEq = TRUE;
                           else noEq = FALSE;
                           break;
/*      case WEAR_LEGS_REAR:*/
      case WEAR_LEGS     : if (!objWear.wearLegs) noEq = TRUE;
                           else noEq = FALSE;
                           break;
/*      case WEAR_FEET_REAR:*/
      case WEAR_FEET     : if (!objWear.wearFeet) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_HANDS_2   :
      case WEAR_HANDS    : if (!objWear.wearHands) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_ARMS_2    :
      case WEAR_ARMS     : if (!objWear.wearArms) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_SHIELD   : if (!objWear.wearShield) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_ABOUT    : if (!objWear.wearAbout) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_WAIST    : if (!objWear.wearWaist) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_WRIST_LL :
      case WEAR_WRIST_LR :
      case WEAR_WRIST_R  :
      case WEAR_WRIST_L  : if (!objWear.wearWrist) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WIELD_FOURTH  :
      case WIELD_THIRD   :
      case WIELD_SECOND  :
      case WIELD_PRIMARY : if ((!objWear.wield) && (!objExtra.twoHands))
                             noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case HOLD2         :
      case HOLD          : if (!objWear.hold) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_EYES     : if (!objWear.wearEyes) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_FACE     : if (!objWear.wearFace) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_EARRING_R:
      case WEAR_EARRING_L: if (!objWear.wearEarring) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_QUIVER   : if (!objWear.wearQuiver) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_BADGE    : if (!objWear.wearBadge) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_WHOLEBODY: if (!objExtra.wholeBody) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_WHOLEHEAD: if (!objExtra.wholeHead) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WIELD_TWOHANDS2:
      case WIELD_TWOHANDS: if (!objExtra.twoHands) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_BACK     : if (!objWear.wearBack) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_ATTACH_BELT_3 :
      case WEAR_ATTACH_BELT_2 :
      case WEAR_ATTACH_BELT_1 :
                           if (!objWear.attachBelt) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_HORSE_BODY :
                           if (!objWear.horseBody) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_TAIL     : if (!objWear.wearTail) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_NOSE     : if (!objWear.wearNose) noEq = TRUE;
                           else noEq = FALSE;
                           break;
      case WEAR_HORN     : if (!objWear.wearHorns) noEq = TRUE;
                           else noEq = FALSE;
                           break;

      default : _outtext("\nERROR: 'where' value not recognized\n\n");
                getkey();
    }

    if (noEq) return EQ_WEAR_NOTSET;
  }

 // need to check for whole body/head shit on non-whole body/head objs too -
 // simplest way is to set equip pos to WEAR_WHOLEBODY and HEAD internally
 // and write them out as WEAR_BODY and HEAD externally

  if (objExtra.wholeBody &&
      !eqSlotFull(mob, WEAR_BODY) && !eqSlotFull(mob, WEAR_LEGS) &&
      !eqSlotFull(mob, WEAR_ARMS) && !eqSlotFull(mob, WEAR_WHOLEBODY) &&
      strcmp(mobSpecies, MOB_THRIKREEN) && strcmp(mobSpecies, MOB_MINOTAUR) &&
      strcmp(mobSpecies, MOB_CENTAUR))
  {
    return WEAR_WHOLEBODY;
  }
  else
  if (objExtra.wholeHead &&
      !eqSlotFull(mob, WEAR_HEAD) && !eqSlotFull(mob, WEAR_EYES) &&
      !eqSlotFull(mob, WEAR_FACE) && /*!eqSlotFull(mob, WEAR_EARRING_R) &&
      !eqSlotFull(mob, WEAR_EARRING_L) &&*/ !eqSlotFull(mob, WEAR_WHOLEHEAD) &&
      strcmp(mobSpecies, MOB_MINOTAUR))
  {
    return WEAR_WHOLEHEAD;
  }
  else
  if (objExtra.twoHands)
  {
    if (handsFree(mob) >= 2)
    {
     // assume that they're thri-kreen, or hell, whatever, who cares,
     // handsFree() func handles that crap

      if (!eqSlotFull(mob, WIELD_TWOHANDS)) return WIELD_TWOHANDS;
      else if (!eqSlotFull(mob, WIELD_TWOHANDS2)) return WIELD_TWOHANDS2;
    }
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wearFinger && (!eqSlotFull(mob, WEAR_FINGER_R) ||
      !eqSlotFull(mob, WEAR_FINGER_L)) && strcmp(mobSpecies, MOB_THRIKREEN))
  {
    if (!eqSlotFull(mob, WEAR_FINGER_R)) return WEAR_FINGER_R;
    else return WEAR_FINGER_L;
  }
  else
  if (objWear.wearWrist && (!eqSlotFull(mob, WEAR_WRIST_R) ||
      !eqSlotFull(mob, WEAR_WRIST_L)) && strcmp(mobSpecies, MOB_THRIKREEN))
  {
    if (!eqSlotFull(mob, WEAR_WRIST_R)) return WEAR_WRIST_R;
    else return WEAR_WRIST_L;
  }
  else
  if (objWear.wearWrist && (!eqSlotFull(mob, WEAR_WRIST_R) ||
      !eqSlotFull(mob, WEAR_WRIST_L) || !eqSlotFull(mob, WEAR_WRIST_LR) ||
      !eqSlotFull(mob, WEAR_WRIST_LL)) && !strcmp(mobSpecies, MOB_THRIKREEN))
  {
    if (!eqSlotFull(mob, WEAR_WRIST_R)) return WEAR_WRIST_R;
    else if (!eqSlotFull(mob, WEAR_WRIST_L)) return WEAR_WRIST_L;
    else if (!eqSlotFull(mob, WEAR_WRIST_LR)) return WEAR_WRIST_LR;
    else return WEAR_WRIST_LL;
  }
  else
  if (objWear.wearNeck && (!eqSlotFull(mob, WEAR_NECK_1) ||
      !eqSlotFull(mob, WEAR_NECK_2) /*&& !eqSlotFull(mob, WEAR_WHOLEHEAD)*/))
  {
    if (!eqSlotFull(mob, WEAR_NECK_1)) return WEAR_NECK_1;
    else return WEAR_NECK_2;
  }
  else
  if (objWear.wearEarring && (!eqSlotFull(mob, WEAR_EARRING_R) ||
      !eqSlotFull(mob, WEAR_EARRING_L)) && strcmp(mobSpecies, MOB_THRIKREEN))
  {
    if (!eqSlotFull(mob, WEAR_EARRING_R)) return WEAR_EARRING_R;
    else return WEAR_EARRING_L;
  }
  else
  if (objWear.wearBody && (!eqSlotFull(mob, WEAR_BODY) &&
      !eqSlotFull(mob, WEAR_WHOLEBODY)) && strcmp(mobSpecies, MOB_THRIKREEN))
  {
    if (objExtra.wholeBody &&
        (strcmp(mobSpecies, MOB_MINOTAUR) || strcmp(mobSpecies, MOB_CENTAUR)))
      return EQ_RACECANTUSE;

    return WEAR_BODY;
  }
  else
  if (objWear.wearHead && (!eqSlotFull(mob, WEAR_HEAD) &&
      !eqSlotFull(mob, WEAR_WHOLEHEAD)) && strcmp(mobSpecies, MOB_MINOTAUR))
  {
    return WEAR_HEAD;
  }
  else
  if (objWear.wearLegs && (!eqSlotFull(mob, WEAR_LEGS) &&
      !eqSlotFull(mob, WEAR_WHOLEBODY)) && strcmp(mobSpecies, MOB_MINOTAUR) &&
      strcmp(mobSpecies, MOB_CENTAUR))
  {
    return WEAR_LEGS;
  }
  else
  if (objWear.wearFeet && !eqSlotFull(mob, WEAR_FEET) &&
      strcmp(mobSpecies, MOB_MINOTAUR) &&
      strcmp(mobSpecies, MOB_THRIKREEN) &&
      (strcmp(mobSpecies, MOB_CENTAUR) ||
       scanKeyword("horseshoe", obj->keywordListHead)))
  {
    return WEAR_FEET;
  }
  else
  if (objWear.wearHands && !eqSlotFull(mob, WEAR_HANDS))
  {
    return WEAR_HANDS;
  }
  else
  if (objWear.wearArms && (!eqSlotFull(mob, WEAR_ARMS) &&
      !eqSlotFull(mob, WEAR_WHOLEBODY)))
  {
    return WEAR_ARMS;
  }
  else
  if (objWear.wearArms && !strcmp(mobSpecies, MOB_THRIKREEN) &&
      !eqSlotFull(mob, WEAR_ARMS_2))
  {
    return WEAR_ARMS_2;
  }
  else
  if (objWear.wearShield && !eqSlotFull(mob, WEAR_SHIELD))
  {
    if (handsFree(mob)) return WEAR_SHIELD;
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wearBack && !eqSlotFull(mob, WEAR_BACK))
  {
    return WEAR_BACK;
  }
  else
  if (objWear.wearAbout && (!eqSlotFull(mob, WEAR_ABOUT) /*&&
      !eqSlotFull(mob, WEAR_WHOLEBODY)*/))
  {
    return WEAR_ABOUT;
  }
  else
  if (objWear.wearWaist && (!eqSlotFull(mob, WEAR_WAIST) &&
      !eqSlotFull(mob, WEAR_WHOLEBODY)))
  {
    return WEAR_WAIST;
  }
  else
  if (objWear.wield && !eqSlotFull(mob, WIELD_PRIMARY))
  {
    if (handsFree(mob)) return WIELD_PRIMARY;
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wield && !eqSlotFull(mob, WIELD_SECOND))
  {
    if (handsFree(mob)) return WIELD_SECOND;
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wield && !eqSlotFull(mob, WIELD_THIRD))
  {
    if (handsFree(mob)) return WIELD_THIRD;
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wield && !eqSlotFull(mob, WIELD_FOURTH))
  {
    if (handsFree(mob)) return WIELD_FOURTH;
    else return EQ_NOHANDS;
  }
  else
  if (objWear.wearEyes && (!eqSlotFull(mob, WEAR_EYES) &&
      !eqSlotFull(mob, WEAR_WHOLEHEAD)))
  {
    return WEAR_EYES;
  }
  else
  if (objWear.wearFace && (!eqSlotFull(mob, WEAR_FACE) &&
      !eqSlotFull(mob, WEAR_WHOLEHEAD)))
  {
    return WEAR_FACE;
  }
  else
  if (objWear.wearBadge && !eqSlotFull(mob, WEAR_BADGE))
  {
    return WEAR_BADGE;
  }
  else
  if (objWear.wearQuiver && !eqSlotFull(mob, WEAR_QUIVER))
  {
    return WEAR_QUIVER;
  }
  else
  if (objWear.horseBody && !eqSlotFull(mob, WEAR_HORSE_BODY) &&
      !strcmp(mobSpecies, MOB_CENTAUR))
  {
    return WEAR_HORSE_BODY;
  }
  else
  if (objWear.wearTail && !eqSlotFull(mob, WEAR_TAIL) &&
      (!strcmp(mobSpecies, MOB_CENTAUR) || !strcmp(mobSpecies, MOB_MINOTAUR)))
  {
    return WEAR_TAIL;
  }
  else
  if (objWear.wearNose && !eqSlotFull(mob, WEAR_NOSE) &&
      !strcmp(mobSpecies, MOB_MINOTAUR))
  {
    return WEAR_NOSE;
  }
  else
  if (objWear.wearHorns && !eqSlotFull(mob, WEAR_HORN) &&
      !strcmp(mobSpecies, MOB_MINOTAUR))
  {
    return WEAR_HORN;
  }
  else
  if (objWear.attachBelt && (!eqSlotFull(mob, WEAR_ATTACH_BELT_1) ||
      !eqSlotFull(mob, WEAR_ATTACH_BELT_2) ||
      !eqSlotFull(mob, WEAR_ATTACH_BELT_3)))
  {
    if (!eqSlotFull(mob, WEAR_WAIST)) return EQ_NOBELTATTCH;
    else if (!eqSlotFull(mob, WEAR_ATTACH_BELT_1)) return WEAR_ATTACH_BELT_1;
    else if (!eqSlotFull(mob, WEAR_ATTACH_BELT_2)) return WEAR_ATTACH_BELT_2;
    else return WEAR_ATTACH_BELT_3;
  }
  else
  if (objWear.hold)
  {
    if (handsFree(mob))
    {
      if (!eqSlotFull(mob, HOLD)) return HOLD;
      else if (!eqSlotFull(mob, HOLD2)) return HOLD2;
    }
    else return EQ_NOHANDS;
  }

  return EQ_SLOT_FILLED;
}


//
// canMobTypeEquip : just check align/class (gonna have to check position
//                   too thanks to new race crap)
//

char canMobTypeEquip(const dikuMob *mob, const dikuObject *obj,
                     const char where)
{
  objWearBitFlags objWear;
  objExtraBitFlags objExtra;
  char ch, mobSpecies[MAX_SPECIES_LEN];


  if (!mob || !obj) return EQ_ERROR;

  objWear = obj->wearBits.objWearBits;
  objExtra = obj->extraBits.objExtraBits;
  strcpy(mobSpecies, mob->mobSpecies);

  if ((!objWear.wearFinger)  && (!objWear.wearNeck)  &&
      (!objWear.wearBody)    && (!objWear.wearHead)  &&
      (!objWear.wearLegs)    && (!objWear.wearFeet)  &&
      (!objWear.wearHands)   && (!objWear.wearArms)  &&
      (!objWear.wearShield)  && (!objWear.wearAbout) &&
      (!objWear.wearWaist)   && (!objWear.wearWrist) &&
      (!objWear.wield)       && (!objWear.hold)      &&
      (!objWear.wearEyes)    && (!objWear.wearFace)  &&
      (!objWear.wearEarring) && (!objWear.wearQuiver) &&
      (!objWear.wearBadge)   && (!objWear.wearBack)  &&
      (!objWear.attachBelt)  && (!objWear.horseBody) &&
      (!objWear.wearTail)    && (!objWear.wearNose)  &&
      (!objWear.wearHorns)   && (!objExtra.wholeBody) &&
      (!objExtra.wholeHead)  && (!objExtra.twoHands))
  {
    return EQ_NO_WEARBITS;
  }

  switch (where)
  {
    case WIELD_THIRD     :
    case WIELD_FOURTH    :
    case WEAR_ARMS_2     :
    case WEAR_HANDS_2    :
    case WEAR_WRIST_LR   :
    case WEAR_WRIST_LL   : if (strcmp(mobSpecies, MOB_THRIKREEN))
                             return EQ_WRONGRACE;
                           break;

    case WEAR_FINGER_L   :
    case WEAR_FINGER_R   :
    case WEAR_EARRING_L  :
    case WEAR_EARRING_R  : if (!strcmp(mobSpecies, MOB_THRIKREEN))
                             return EQ_RACECANTUSE;
                           break;

    case WEAR_BODY       : if (!strcmp(mobSpecies, MOB_THRIKREEN))
                             return EQ_RACECANTUSE;

                           if (objExtra.wholeBody &&
                               (!strcmp(mobSpecies, MOB_CENTAUR) ||
                                !strcmp(mobSpecies, MOB_MINOTAUR)))
                             return EQ_RACECANTUSE;
                           break;

    case WEAR_FEET       : if (!strcmp(mobSpecies, MOB_THRIKREEN) ||
                               !strcmp(mobSpecies, MOB_MINOTAUR) ||
                               (!strcmp(mobSpecies, MOB_CENTAUR) &&
                          !scanKeyword("horseshoe", obj->keywordListHead)))
                            return EQ_RACECANTUSE;
                           break;

    case WEAR_WHOLEHEAD  :
    case WEAR_HEAD       : if (!strcmp(mobSpecies, MOB_MINOTAUR))
                             return EQ_RACECANTUSE;
                           break;

    case WEAR_WHOLEBODY  : if (!strcmp(mobSpecies, MOB_THRIKREEN) ||
                               !strcmp(mobSpecies, MOB_CENTAUR) ||
                               !strcmp(mobSpecies, MOB_MINOTAUR))
                             return EQ_RACECANTUSE;
                           break;

    case WEAR_HORSE_BODY : if (strcmp(mobSpecies, MOB_CENTAUR))
                             return EQ_WRONGRACE;
                           break;

    case WEAR_TAIL       : if (strcmp(mobSpecies, MOB_MINOTAUR) &&
                               strcmp(mobSpecies, MOB_CENTAUR))
                             return EQ_WRONGRACE;
                           break;

    case WEAR_HORN       :
    case WEAR_NOSE       : if (strcmp(mobSpecies, MOB_MINOTAUR))
                             return EQ_WRONGRACE;
                           break;

    case WEAR_LEGS       : if (!strcmp(mobSpecies, MOB_CENTAUR))
                             return EQ_RACECANTUSE;
                           break;
  }

  ch = eqRestrict(mob, objExtra, obj->antiBits.objAntiBits,
                  obj->anti2Bits.objAnti2Bits);
  if (ch) return ch;

  return EQ_WEARABLE;
}


//
// getCanEquipErrStrn : returns error strings for equipping errors, but not
//                      valid equip codes
//

char *getCanEquipErrStrn(const char err, const char where, char *strn)
{
  switch (err)
  {
    case EQ_SLOT_FILLED : sprintf(strn, "slot is already full (slot #%d)",
                                  where);
                          return strn;
    case EQ_RESALIGN    : return "mob cannot use equipment due to align";
    case EQ_RESCLASS    : return "mob cannot use equipment due to class";
    case EQ_RESRACE     : return "mob cannot use equipment due to race (anti bits)";
    case EQ_RESSEX      : return "mob cannot use equipment due to sex (anti2 bits)";
    case EQ_NO_WEARBITS : return "object has no valid wear bits";
    case EQ_NOHANDS     : return "mob has no free hands available";
    case EQ_ERROR       : return "error checking mob or object";
    case EQ_WEAR_NOTSET : return "correct wear bit not set for position";
    case EQ_WRONGRACE   : return "wrong race for position";
    case EQ_RACECANTUSE : return "race can't use equipment in that slot";
    case EQ_NOBELTATTCH : return "trying to attach something to belt, but no belt";

    default : sprintf(strn, "unrecognized error code (#%d)", err);
              return strn;
  }
}


//
// getVerboseCanEquipStrn
//

char *getVerboseCanEquipStrn(const char err, const char *objName, char *strn)
{
  switch (err)
  {
    case EQ_SLOT_FILLED : return
"Cannot equip that obj on this mob - no slots available.\n";

    case EQ_RESALIGN    : return
"Cannot equip that obj on this mob - cannot use due to alignment.\n";

    case EQ_RESCLASS    : return
"Cannot equip that obj on this mob - cannot use due to class.\n";

    case EQ_RESRACE     : return
"Cannot equip that obj on this mob - cannot use due to race (anti bits).\n";

    case EQ_RESSEX      : return
"Cannot equip that obj on this mob - cannot use due to sex (anti2 bits).\n";

    case EQ_NO_WEARBITS : return
"Cannot equip that obj on this mob - the object has no valid wear bits.\n";

    case EQ_NOHANDS     : return
"Cannot equip that obj on this mob - mob has no free hands.\n";

    case EQ_WEAR_NOTSET : return
"Cannot equip that obj on this mob - obj doesn't have correct wear bit set.\n";

    case EQ_ERROR       : return
"Cannot equip that obj on this mob - error checking mob or object.\n";

    case EQ_WRONGRACE   : return
"Cannot equip that obj on this mob - wrong race for equipment position.\n";

    case EQ_RACECANTUSE : return
"Cannot equip that obj on this mob - race can't use equipment in that slot.\n";

    case EQ_NOBELTATTCH : return
"Cannot equip that obj on this mob - mob has no belt to attach it to.\n";

    case WEAR_FINGER_R  :
      sprintf(strn, "'%s&n' equipped on mob's right finger slot.\n",
              objName);
      return strn;

    case WEAR_FINGER_L  :
      sprintf(strn, "'%s&n' equipped on mob's left finger slot.\n",
              objName);
      return strn;

    case WEAR_NECK_1    :
      sprintf(strn, "'%s&n' equipped on mob's first neck slot.\n",
              objName);
      return strn;

    case WEAR_NECK_2    :
      sprintf(strn, "'%s&n' equipped on mob's second neck slot.\n",
              objName);
      return strn;

    case WEAR_BODY      :
      sprintf(strn, "'%s&n' equipped on mob's body slot.\n",
              objName);
      return strn;

    case WEAR_WHOLEBODY      :
      sprintf(strn, "'%s&n' equipped on mob's entire body.\n",
              objName);
      return strn;

    case WEAR_HEAD      :
      sprintf(strn, "'%s&n' equipped on mob's head slot.\n",
              objName);
      return strn;

    case WEAR_WHOLEHEAD :
      sprintf(strn, "'%s&n' equipped on mob's entire head.\n",
              objName);
      return strn;

    case WEAR_LEGS      :
      sprintf(strn, "'%s&n' equipped on mob's legs slot.\n",
              objName);
      return strn;

    case WEAR_FEET      :
      sprintf(strn, "'%s&n' equipped on mob's feet slot.\n",
              objName);
      return strn;

    case WEAR_HANDS     :
      sprintf(strn, "'%s&n' equipped on mob's hands slot.\n",
              objName);
      return strn;

    case WEAR_ARMS      :
      sprintf(strn, "'%s&n' equipped on mob's arms slot.\n",
              objName);
      return strn;

    case WEAR_SHIELD    :
      sprintf(strn, "'%s&n' equipped on mob's shield slot.\n",
              objName);
      return strn;

    case WEAR_ABOUT     :
      sprintf(strn, "'%s&n' equipped on mob's about body slot.\n",
              objName);
      return strn;

    case WEAR_WAIST     :
      sprintf(strn, "'%s&n' equipped on mob's waist slot.\n",
              objName);
      return strn;

    case WEAR_WRIST_R   :
      sprintf(strn, "'%s&n' equipped on mob's right wrist slot.\n",
              objName);
      return strn;

    case WEAR_WRIST_L   :
      sprintf(strn, "'%s&n' equipped on mob's left wrist slot.\n",
              objName);
      return strn;

    case WIELD_PRIMARY  :
      sprintf(strn, "'%s&n' equipped on mob's primary weapon slot.\n",
              objName);
      return strn;

    case WIELD_SECOND   :
      sprintf(strn, "'%s&n' equipped on mob's secondary weapon slot.\n",
              objName);
      return strn;

    case WIELD_TWOHANDS :
      sprintf(strn, "'%s&n' equipped in both of mob's hands.\n",
              objName);
      return strn;

    case WIELD_TWOHANDS2 :
      sprintf(strn, "'%s&n' equipped in both of mob's hands.\n",
              objName);
      return strn;

    case HOLD           :
    case HOLD2          :
      sprintf(strn, "'%s&n' equipped on mob's hold slot.\n",
              objName);
      return strn;

    case WEAR_EYES      :
      sprintf(strn, "'%s&n' equipped on mob's eyes slot.\n",
              objName);
      return strn;

    case WEAR_FACE      :
      sprintf(strn, "'%s&n' equipped on mob's face slot.\n",
              objName);
      return strn;

    case WEAR_EARRING_R :
      sprintf(strn, "'%s&n' equipped on mob's right earring slot.\n",
              objName);
      return strn;

    case WEAR_EARRING_L :
      sprintf(strn, "'%s&n' equipped on mob's left earring slot.\n",
              objName);
      return strn;

    case WEAR_BADGE     :
      sprintf(strn, "'%s&n' equipped on mob's badge slot.\n",
              objName);
      return strn;

    case WEAR_QUIVER    :
      sprintf(strn, "'%s&n' equipped on mob's quiver slot.\n",
              objName);
      return strn;

    case WIELD_THIRD     :
      sprintf(strn, "'%s&n' equipped on mob's tertiary weapon slot.\n",
              objName);
      return strn;

    case WIELD_FOURTH    :
      sprintf(strn, "'%s&n' equipped on mob's fourth weapon slot.\n",
              objName);
      return strn;

    case WEAR_BACK      :
      sprintf(strn, "'%s&n' equipped on mob's back slot.\n",
              objName);
      return strn;

    case WEAR_ATTACH_BELT_1      :
      sprintf(strn, "'%s&n' equipped on mob's first belt attach slot.\n",
              objName);
      return strn;

    case WEAR_ATTACH_BELT_2      :
      sprintf(strn, "'%s&n' equipped on mob's second belt attach slot.\n",
              objName);
      return strn;

    case WEAR_ATTACH_BELT_3      :
      sprintf(strn, "'%s&n' equipped on mob's third belt attach slot.\n",
              objName);
      return strn;

    case WEAR_ARMS_2      :
      sprintf(strn, "'%s&n' equipped on mob's lower arms slot.\n",
              objName);
      return strn;

    case WEAR_HANDS_2      :
      sprintf(strn, "'%s&n' equipped on mob's lower hands slot.\n",
              objName);
      return strn;

    case WEAR_WRIST_LR      :
      sprintf(strn, "'%s&n' equipped on mob's lower right wrist slot.\n",
              objName);
      return strn;

    case WEAR_WRIST_LL      :
      sprintf(strn, "'%s&n' equipped on mob's lower left wrist slot.\n",
              objName);
      return strn;

    case WEAR_HORSE_BODY      :
      sprintf(strn, "'%s&n' equipped on mob's horse body slot.\n",
              objName);
      return strn;

    case WEAR_LEGS_REAR      :
      sprintf(strn, "'%s&n' equipped on mob's rear legs slot.\n",
              objName);
      return strn;

    case WEAR_TAIL      :
      sprintf(strn, "'%s&n' equipped on mob's tail slot.\n",
              objName);
      return strn;

    case WEAR_FEET_REAR      :
      sprintf(strn, "'%s&n' equipped on mob's rear feet slot.\n",
              objName);
      return strn;

    case WEAR_NOSE      :
      sprintf(strn, "'%s&n' equipped on mob's nose slot.\n",
              objName);
      return strn;

    case WEAR_HORN      :
      sprintf(strn, "'%s&n' equipped on mob's horn slot.\n",
              objName);
      return strn;

    default : sprintf(strn,
"Unrecognized error code passed to getVerboseCanEquipStrn() [err = %d].\n",
                      err);
              return strn;
  }
}


//
// getMobEquipPos - this call is a tad useless now, though it does
//                  serve to provide a small measure of data protection/
//                  encapsulation
//

objectHere *getMobEquipPos(const mobHere *mob, const char pos)
{
  if (!mob || (pos < WEAR_LOW) || (pos > WEAR_TRUEHIGH)) return NULL;

  return mob->equipment[pos];
}


//
// mobsCanEquipEquipped
//

char mobsCanEquipEquipped(const char fixEq)
{
  ulong numb = getLowestRoomNumber(), high = getHighestRoomNumber();
  dikuRoom *room;
  mobHere *mob;
  objectHere *tmpObj;
  long val, i;


 // run through all valid rooms, looking for loaded mobs

  while (numb <= high)
  {
    room = findRoom(numb);
    if (room)
    {
     // run through mobs, mob by mob

      mob = room->mobHead;
      while (mob)
      {
       // run through objects, obj by obj

        for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        {
          if (mob->equipment[i] && mob->equipment[i]->objectPtr)
          {
            tmpObj = mob->equipment[i];
            mob->equipment[i] = NULL;

            val = canMobTypeEquip(mob->mobPtr, tmpObj->objectPtr, i);

            mob->equipment[i] = tmpObj;
          }
          else continue;

          if (val > WEAR_TRUEHIGH)
          {
            if (!fixEq) return FALSE;
            else
            {
              addObjHeretoList(&(mob->objectHead), mob->equipment[i]);
              mob->equipment[i] = NULL;
            }
          }
        }

        mob = mob->Next;
      }
    }

    numb++;
  }

  return TRUE;
}


//
// mobEqinCorrectSlot
//

char mobsEqinCorrectSlot(const char fixEq)
{
  ulong numb = getLowestRoomNumber(), high = getHighestRoomNumber();
  dikuRoom *room;
  mobHere *mob;
  objectHere *tmpObj;
  long val, i;

  while (numb <= high)
  {
    room = findRoom(numb);
    if (room)
    {
      mob = room->mobHead;
      while (mob)
      {
        for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        {
          if (mob->equipment[i])
          {
            tmpObj = mob->equipment[i];
            mob->equipment[i] = NULL;

//            val = canEquipinSlot(mob, tmpObj->objectPtr, i);
            val = getMobHereEquipSlot(mob, tmpObj->objectPtr, i);

            mob->equipment[i] = tmpObj;
          }
          else continue;

          if (val > WEAR_TRUEHIGH)
          {
            if (!fixEq) return FALSE;
            else
            {
              addObjHeretoList(&(mob->objectHead), mob->equipment[i]);
              mob->equipment[i] = NULL;
            }
          }
        }

        mob = mob->Next;
      }
    }

    numb++;
  }

  return TRUE;
}


//
// mobEquippingSomething
//

char mobEquippingSomething(const mobHere *mob)
{
  long i;

  if (!mob) return FALSE;

  for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
    if (mob->equipment[i]) return TRUE;

  return FALSE;
}


//
// getNumbFollowers
//

ulong getNumbFollowers(const mobHere *mob)
{
  dikuRoom *room;
  mobHere *mobN;
  ulong i, high = getHighestRoomNumber(), numb = 0;

  if (!mob) return 0;

  for (i = getLowestRoomNumber(); i <= high; i++)
  {
    if (room = findRoom(i))
    {
      mobN = room->mobHead;
      while (mobN)
      {
        if (mobN->following == mob) numb++;

        mobN = mobN->Next;
      }
    }
  }

  return numb;
}


//
// getMatchingMob
//

dikuMob *getMatchingMob(const char *strn, const dikuRoom *room /*= NULL*/,
                        dikuMob *startMob /*= NULL*/,
                        mobHere *startMobH /*= NULL*/)
{
  ulong vnum;
  char numb;
  mobHere *mobHere;
  dikuMob *mob;

  if (strnumer(strn))
  {
    numb = TRUE;
    vnum = atoi(strn);
  }
  else numb = FALSE;

  if (room)
  {
    if (startMobH) mobHere = startMobH;
    else mobHere = room->mobHead;

    while (mobHere)
    {
      if (!numb)
      {
        if (mobHere->mobPtr &&
            scanKeyword(strn, mobHere->mobPtr->keywordListHead))
        {
          return mobHere->mobPtr;
        }
      }
      else
      {
        if (mobHere->mobPtr && (mobHere->mobNumb == vnum))
          return mobHere->mobPtr;
      }

      mobHere = mobHere->Next;
    }
  }

  if (startMob) mob = startMob;
  else mob = mobHead;

  while (mob)
  {
    if (!numb)
    {
      if (scanKeyword(strn, mob->keywordListHead)) return mob;
    }
    else if (mob->mobNumber == vnum) return mob;

    mob = mob->Next;
  }

  return NULL;
}
