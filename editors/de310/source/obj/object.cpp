//
//  File: object.cpp     originally part of dikuEdit
//
//  Usage: multitudes of functions for use with objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "object.h"
#include "objhere.h"

using namespace std;

extern dikuRoom *roomHead;
extern dikuObject **objLookup, *objectHead;
extern dikuMob *mobHead;
extern ulong lowestObjNumber, highestObjNumber, numbLookupEntries, numbObjTypes;
extern editableListNode *inventory;
extern char madeChanges;


//
// findObj : Returns the address of the object node that has the requested
//           objNumber (if any)
//
//   objNumber : object number to look for in the dikuObject linked list
//

INLINEDEF dikuObject *findObj(const register ulong objNumber)
{
  if (objNumber >= numbLookupEntries) return NULL;

  return objLookup[objNumber];
}


//
// fixObjName : Trims *oldName to MAX_OBJSHORT_LEN, setting *newName with
//              the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixObjName(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_OBJSHORT_LEN, !getShowColorVal());
}


//
// copyObjectType : copies all the info from an object type record into a new
//                  record and returns the address of the new record
//
//        *srcObj : source object type record
// incNumbObjects : if TRUE, increments the global numbObjTypes var
//

dikuObject *copyObjectType(dikuObject *srcObj, const char incNumbObjects)
{
  dikuObject *newObj;


 // make sure src exists

  if (!srcObj) return NULL;

 // alloc mem for new rec

  newObj = new dikuObject;
  if (!newObj)
  {
    cout << "\n\nError creating new dikuObject in copyObjectType().  "
            "Out of memory.\n\n";

    return NULL;
  }

//  newObj->Next = NULL;

 // first, copy the simple stuff

  memcpy(newObj, srcObj, sizeof(dikuObject));

 // next, the not-so-simple stuff

  newObj->keywordListHead = copyStringNodes(srcObj->keywordListHead);

 // extra desc linked list

  newObj->extraDescHead = copyExtraDescs(srcObj->extraDescHead);

  if (incNumbObjects)
  {
    numbObjTypes++;
    createPrompt();
  }

 // return the address of the new object

  return newObj;
}


//
// compareObjectApply : FALSE no match TRUE match
//
//   *app1 : first record to compare
//   *app2 : second record
//

INLINEDEF char compareObjectApply(const objApplyRec *app1, const objApplyRec *app2)
{
  if (!memcmp(app1, app2, sizeof(objApplyRec))) return TRUE;
  else return FALSE;
}


//
// compareObjectType : compares it all, baby - returns FALSE if they don't
//                     match, TRUE if they do
//

uchar compareObjectType(dikuObject *obj1, dikuObject *obj2)
{
  if (obj1 == obj2) return TRUE;
  if (!obj1 || !obj2) return FALSE;

 // check object names etc blah blah

  if (strcmp(obj1->objShortName, obj2->objShortName)) return FALSE;
  if (strcmp(obj1->objLongName, obj2->objLongName)) return FALSE;

  if (obj1->objNumber != obj2->objNumber) return FALSE;

  if (obj1->extraBits.longIntFlags != obj2->extraBits.longIntFlags)
    return FALSE;
  if (obj1->wearBits.longIntFlags != obj2->wearBits.longIntFlags)
    return FALSE;
  if (obj1->extra2Bits.longIntFlags != obj2->extra2Bits.longIntFlags)
    return FALSE;
  if (obj1->antiBits.longIntFlags != obj2->antiBits.longIntFlags)
    return FALSE;
  if (obj1->anti2Bits.longIntFlags != obj2->anti2Bits.longIntFlags)
    return FALSE;

  if (obj1->affect1Bits.longIntFlags != obj2->affect1Bits.longIntFlags)
    return FALSE;
  if (obj1->affect2Bits.longIntFlags != obj2->affect2Bits.longIntFlags)
    return FALSE;

  if (obj1->objType != obj2->objType) return FALSE;
  if (obj1->material != obj2->material) return FALSE;
  if (obj1->size != obj2->size) return FALSE;
  if (obj1->space != obj2->space) return FALSE;
  if (obj1->craftsmanship != obj2->craftsmanship) return FALSE;
//  if (obj1->maxSP != obj2->maxSP) return FALSE;
  if (obj1->damResistBonus != obj2->damResistBonus) return FALSE;
  if (obj1->weight != obj2->weight) return FALSE;
  if (obj1->value != obj2->value) return FALSE;
  if (obj1->condition != obj2->condition) return FALSE;
  if (!compareObjectApply(&(obj1->objApply[0]), &(obj2->objApply[0])))
    return FALSE;
  if (!compareObjectApply(&(obj1->objApply[1]), &(obj2->objApply[1])))
    return FALSE;
  if (memcmp(obj1->objValues, obj2->objValues, sizeof(long) * NUMB_OBJ_VALS))
    return FALSE;

  if (obj1->trapAff != obj2->trapAff) return FALSE;
  if (obj1->trapDam != obj2->trapDam) return FALSE;
  if (obj1->trapCharge != obj2->trapCharge) return FALSE;
  if (obj1->trapLevel != obj2->trapLevel) return FALSE;

 // description and extra descs

  if (!compareStringNodes(obj1->keywordListHead, obj2->keywordListHead))
    return FALSE;

  if (!compareExtraDescs(obj1->extraDescHead, obj2->extraDescHead))
    return FALSE;

 // limits

  if(obj1->maxload != obj2->maxload) return FALSE;
  if(obj1->globalmaxload != obj2->globalmaxload) return FALSE;

  return TRUE;
}


//
// getHighestObjNumber : Gets the highest obj number currently in the
//                       objHead list, returning it..
//

INLINEDEF ulong getHighestObjNumber(void)
{
  return highestObjNumber;
}


//
// getLowestObjNumber : Gets the lowest obj number currently in the
//                      objHead list, returning it..
//

INLINEDEF ulong getLowestObjNumber(void)
{
  return lowestObjNumber;
}


//
// getFirstFreeObjNumber : Starts at lowest obj number, loops up to highest,
//                         returning the first number with no object type
//                         (highest + 1 if no gaps are found)
//

INLINEDEF ulong getFirstFreeObjNumber(void)
{
  register ulong i;

  for (i = lowestObjNumber + 1; i <= highestObjNumber; i++)
  {
    if (!objLookup[i]) return i;
  }

  return highestObjNumber + 1;
}


//
// checkAndFixRefstoObj
//

void checkAndFixRefstoObj(const ulong oldNumb, const ulong newNumb)
{
  ulong i;
  dikuObject *obj = objectHead;
  dikuRoom *room = roomHead;
  dikuMob *mob = mobHead;
  questQuest *qst;
  questItem *item;


 // scan through object types and room exits

 // fix object field refs

  while (obj)
  {
    for (i = 0; i < NUMB_OBJ_VALS; i++)
    {
     // fix renumber key numb of 0 bug with containers

      if ((i == 2) && (obj->objValues[i] == 0) &&
          (obj->objType == ITEM_CONTAINER))
      {
       continue;
      }
      else
      {
        if ((fieldRefsObjNumb(obj->objType, i) && (obj->objValues[i] == oldNumb)))
        {
          obj->objValues[i] = newNumb;

          madeChanges = TRUE;
        }
      }
    }

    obj = obj->Next;
  }

 // fix keynumb refs

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (room->exits[i] && (room->exits[i]->keyNumb) &&
          (room->exits[i]->keyNumb == oldNumb))
      {
        room->exits[i]->keyNumb = newNumb;

        madeChanges = TRUE;
      }
    }

    room = room->Next;
  }

 // fix quest refs

  while (mob)
  {
    if (mob->questPtr)
    {
      qst = mob->questPtr->questHead;

      while (qst)
      {
        item = qst->questPlayRecvHead;
        while (item)
        {
          if ((item->itemType == QUEST_RITEM_OBJ) &&
              (item->itemVal == oldNumb))
          {
            item->itemVal = newNumb;
          }

          item = item->Next;
        }

        item = qst->questPlayGiveHead;
        while (item)
        {
          if ((item->itemType == QUEST_GITEM_OBJ) &&
              (item->itemVal == oldNumb))
          {
            item->itemVal = newNumb;
          }

          item = item->Next;
        }

        qst = qst->Next;
      }
    }

    mob = mob->Next;
  }

 // fix shop refs

  mob = mobHead;

  while (mob)
  {
    if (mob->shopPtr)
    {
      for (i = 0; mob->shopPtr->producedItemList[i]; i++)
      {
        if (mob->shopPtr->producedItemList[i] == oldNumb)
        {
          mob->shopPtr->producedItemList[i] = newNumb;
        }
      }
    }

    mob = mob->Next;
  }
}


//
// rebuildObjLookup
//

void rebuildObjLookup(void)
{
  dikuObject *obj = objectHead;
  ulong i;

  if (!objLookup) return;  // whoops

  memset(objLookup, 0, sizeof(dikuObject *) * numbLookupEntries);

  lowestObjNumber = numbLookupEntries;
  highestObjNumber = 0;

  while (obj)
  {
    i = obj->objNumber;

    objLookup[i] = obj;
    if (i < lowestObjNumber) lowestObjNumber = i;
    if (i > highestObjNumber) highestObjNumber = i;

    obj = obj->Next;
  }
}


//
// renumberObjs : Renumbers the objs so that there are no "gaps" - starts
//                at the first obj and simply renumbers like mad from there
//

void renumberObjs(const char renumberHead, const long oldHeadNumb)
{
  dikuObject *objNode = objectHead;
  long lastNumb = objNode->objNumber - 1, oldNumb;


  if (renumberHead)
  {
    resetAllObjHere(oldHeadNumb, objNode->objNumber, roomHead);
    resetNumbLoaded(ENTITY_OBJECT, oldHeadNumb, objNode->objNumber);

    checkAndFixRefstoObj(oldHeadNumb, objNode->objNumber);

    madeChanges = TRUE;
  }

  while (objNode)
  {
   // first, renumber object types and objHeres of these types

    if ((lastNumb + 1) != objNode->objNumber)
    {
      oldNumb = objNode->objNumber;
      objNode->objNumber = lastNumb + 1;

      resetAllObjHere(oldNumb, objNode->objNumber, roomHead);
      resetNumbLoaded(ENTITY_OBJECT, oldNumb, objNode->objNumber);

      checkAndFixRefstoObj(oldNumb, objNode->objNumber);

      madeChanges = TRUE;
    }

    lastNumb = objNode->objNumber;

    objNode = objNode->Next;
  }

  rebuildObjLookup();
//  resetEntityPointersByNumb(TRUE, FALSE);
}


//
// noObjNext
//

/*
char noObjNext(dikuObject *obj, char *substrn, usint pos)
{
  if (!obj) return TRUE;
  obj = obj->Next;

  while (obj)
  {
    if (scanKeyword(substrn, obj->keywordListHead, pos)) return FALSE;

    obj = obj->Next;
  }

  return TRUE;
}
*/


//
// getPrevObj : find object right before objNumb, numerically
//

INLINEDEF dikuObject *getPrevObj(const ulong objNumb)
{
  ulong i = objNumb - 1;

  if (objNumb <= getLowestObjNumber()) return NULL;

  while (!objLookup[i]) i--;

  return objLookup[i];
}


//
// getNextObj : find object right after objNumb, numerically
//

INLINEDEF dikuObject *getNextObj(const ulong objNumb)
{
  ulong i = objNumb + 1;

  if (objNumb >= getHighestObjNumber()) return NULL;

  while (!objLookup[i]) i++;

  return objLookup[i];
}


//
// deleteObjsinInv
//

void deleteObjsinInv(const dikuObject *objPtr)
{
  editableListNode *edit = inventory;

  if (!objPtr) return;

  while (edit)
  {
    if ((edit->entityType == ENTITY_OBJECT) &&
        (((objectHere *)(edit->entityPtr))->objectPtr == objPtr))
      deleteEditableinList(&inventory, edit);

    edit = edit->Next;
  }
}


//
// updateInvKeywordsObj
//

void updateInvKeywordsObj(const dikuObject *objPtr)
{
  editableListNode *edit = inventory;

  if (!objPtr) return;

  while (edit)
  {
    if ((edit->entityType == ENTITY_OBJECT) &&
        (((objectHere *)(edit->entityPtr))->objectPtr == objPtr))
    {
      deleteStringNodes(edit->keywordListHead);
      edit->keywordListHead = copyStringNodes(objPtr->keywordListHead);
    }

    edit = edit->Next;
  }
}


//
// resetLowHighObj
//

/*INLINEDEF*/ void resetLowHighObj(void)
{
  ulong i, high = 0, low = numbLookupEntries;

  for (i = 0; i < numbLookupEntries; i++)
  {
    if (objLookup[i])
    {
      if (i > high) high = i;
      if (i < low) low = i;
    }
  }

  lowestObjNumber = low;
  highestObjNumber = high;
}


//
// getMatchingObj : snags the first matching loaded object based on character
//                  string - if numeric, looks for vnum, if non-numeric, scans
//                  keyword list.  starts at *room if non-null, roomHead
//                  otherwise.  ditto for *startObj and *startObjH (startObjH
//                  specifies node in room's object list [no error-checking])
//

dikuObject *getMatchingObj(const char *strn, dikuRoom *room /*= NULL*/,
                           dikuObject *startObj /*= NULL*/,
                           objectHere *startObjH /*= NULL*/)
{
  ulong vnum;
  char numb;
  objectHere *objHere;
  dikuObject *obj;

  if (strnumer(strn))
  {
    numb = TRUE;
    vnum = atoi(strn);
  }
  else numb = FALSE;

  if (room)
  {
    if (startObjH) objHere = startObjH;
    else objHere = room->objectHead;

    while (objHere)
    {
      if (!numb)
      {
        if (objHere->objectPtr &&
            scanKeyword(strn, objHere->objectPtr->keywordListHead))
        {
          return objHere->objectPtr;
        }
      }
      else
      {
        if (objHere->objectPtr && (objHere->objectNumb == vnum))
          return objHere->objectPtr;
      }

      objHere = objHere->Next;
    }
  }

  if (startObj) obj = startObj;
  else obj = objectHead;

  while (obj)
  {
    if (!numb)
    {
      if (scanKeyword(strn, obj->keywordListHead)) return obj;
    }
    else if (obj->objNumber == vnum) return obj;

    obj = obj->Next;
  }

  return NULL;
}


//
// armorPosTypeMatters
//

char armorPosTypeMatters(const long pos)
{
  switch (pos)
  {
    case ARMOR_WEAR_NECK :
    case ARMOR_WEAR_BODY :
    case ARMOR_WEAR_HEAD :
    case ARMOR_WEAR_LEGS :
    case ARMOR_WEAR_FEET :
    case ARMOR_WEAR_HANDS :
    case ARMOR_WEAR_ARMS :
    case ARMOR_WEAR_ABOUT :
    case ARMOR_WEAR_WRIST :
    case ARMOR_WEAR_EYES :
    case ARMOR_WEAR_FACE :
    case ARMOR_WEAR_HORSEBODY : return TRUE;
  }

  return FALSE;
}
