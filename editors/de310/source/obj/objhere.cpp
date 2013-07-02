//
//  File: objhere.cpp    originally part of dikuEdit
//
//  Usage: functions related to objectHere structures in general
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>

#include "../fh.h"
#include "../types.h"

#include "objhere.h"
#include "../room/room.h"
#include "object.h"

using namespace std;

extern ulong numbObjs;
extern dikuRoom *roomHead;
extern dikuMob *mobHead;
extern editableListNode *inventory;

//
// checkAllObjInsideForVnum : checks object pointed to by objHere as well
//                            as objects inside for matching vnum
//

char checkAllObjInsideForVnum(objectHere *objHere, const long vnum)
{
  objectHere *obj = objHere;

  while (obj)
  {
    if (obj->objectNumb == vnum) return TRUE;

    if (checkAllObjInsideForVnum(obj->objInside, vnum)) return TRUE;

    obj = obj->Next;
  }

  return FALSE;
}


//
// findObjHere : Returns the address of the objectHere node that has the
//               requested objNumber (if any) - scans through every objHere
//               in the room list
//
//         objNumber : object number to look for in all objectHere nodes
//                     everywhere
//     *roomListHead : head of rooms to search through
//         *roomNumb : pointer to ulong to set to roomNumb
// checkOnlyThisRoom : if TRUE, checks only room pointer passed
//  checkOnlyVisible : if TRUE, doesn't check objs in containers or on mobs
//

objectHere *findObjHere(const ulong objNumber, dikuRoom *roomNode,
                        ulong *roomNumb,
                        const char checkOnlyThisRoom /* = FALSE*/,
                        const char checkOnlyVisible /* = FALSE*/)
{
  objectHere *obj;
  mobHere *mob;
  long i;


  while (roomNode)
  {
    obj = roomNode->objectHead;

    while (obj)
    {
      if (obj->objectNumb == objNumber)
      {
        *roomNumb = roomNode->roomNumber;
        return obj;
      }

      if (obj->objInside && !checkOnlyVisible)
      {
        if (checkAllObjInsideForVnum(obj->objInside, objNumber))
        {
          *roomNumb = roomNode->roomNumber;
          return obj;
        }
      }

      obj = obj->Next;
    }

    mob = roomNode->mobHead;

    while (mob && !checkOnlyVisible)
    {
      if (mob->objectHead)
      {
        obj = mob->objectHead;

        while (obj)
        {
          if (obj->objectNumb == objNumber)
          {
            *roomNumb = roomNode->roomNumber;
            return obj;
          }

          if ((obj->objInside) &&
              (checkAllObjInsideForVnum(obj->objInside, objNumber)))
          {
            *roomNumb = roomNode->roomNumber;
            return obj;
          }

          obj = obj->Next;
        }
      }

      for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
      {
        obj = mob->equipment[i];

        if (obj)
        {
          if (obj->objectNumb == objNumber)
          {
            *roomNumb = roomNode->roomNumber;
            return obj;
          }

          if ((obj->objInside) &&
              (checkAllObjInsideForVnum(obj->objInside, objNumber)))
          {
            *roomNumb = roomNode->roomNumber;
            return obj;
          }
        }
      }

      mob = mob->Next;
    }

    if (!checkOnlyThisRoom) roomNode = roomNode->Next;
    else return NULL;
  }

  return NULL;
}


//
// getNumbObjectHereNodes : Returns the number of nodes in a specific
//                          objectHere list
//
//    *objNode : take a wild guess, bucko
//

ulong getNumbObjectHereNodes(objectHere *objNode)
{
  ulong i = 0;


  while (objNode)
  {
    i++;

    objNode = objNode->Next;
  }

  return i;
}


//
// copyObjHere : Copies just one objectHere
//
//    *srcObjHere : take a wild guess, bucko
//      incLoaded : if true, increments appropriate vars (in funcs called)
//

objectHere *copyObjHere(objectHere *srcObjHere, const char incLoaded)
{
  objectHere *newObjHere;


  if (!srcObjHere) return NULL;

  newObjHere = new objectHere;
  if (!newObjHere)
  {
    cout << "\n\nERROR: copyObjHere() - ran out of mem alloc'ing new "
         << " objectHere node.  Aborting..\n\n";

    return NULL;
  }


 // first, the easy stuff

  memcpy(newObjHere, srcObjHere, sizeof(objectHere));
  newObjHere->Next = NULL;

 // now, the not quite as easy but still relatively simple stuff

  newObjHere->objInside = copyObjHereList(srcObjHere->objInside, incLoaded);


  return newObjHere;
}


//
// copyObjHereList : Copies an objectHere list, returning the address of the
//                   new list
//
//    *srcObjHere : take a wild guess, bucko
//      incLoaded : specifies what you might expect
//

objectHere *copyObjHereList(objectHere *srcObjHere, const char incLoaded)
{
  objectHere *newObjHere, *prevObjHere = NULL, *headObjHere = NULL;


  if (!srcObjHere) return NULL;

  while (srcObjHere)
  {
    newObjHere = new objectHere;
    if (!newObjHere)
    {
      cout << "\n\nERROR: copyObjHereList() - ran out of mem alloc'ing new "
           << " objectHere node.  Aborting..\n\n";

      return NULL;
    }

    newObjHere->Next = NULL;

    if (!headObjHere) headObjHere = newObjHere;

    if (prevObjHere) prevObjHere->Next = newObjHere;
    prevObjHere = newObjHere;


   // first, the easy stuff

    memcpy(newObjHere, srcObjHere, sizeof(objectHere));

   // now, the not quite as easy but still relatively simple stuff

    newObjHere->objInside = copyObjHereList(srcObjHere->objInside, incLoaded);

    if (incLoaded)
    {
      addEntity(ENTITY_OBJECT, newObjHere->objectNumb);
      numbObjs++;
    }

    srcObjHere = srcObjHere->Next;
  }

  if (incLoaded) createPrompt();  // more efficient..

  return headObjHere;
}


//
// checkEntireObjHereListForType :
//      checks entire objectHere list for an objHere of type *objType
//

char checkEntireObjHereListForType(objectHere *objHereHead,
                                   const dikuObject *objType)
{
  objectHere *objHere = objHereHead;

  while (objHere)
  {
    if (objHere->objectPtr == objType) return TRUE;

    if (objHere->objInside)
    {
      if (checkEntireObjHereListForType(objHere->objInside, objType))
      {
        return TRUE;
      }
    }

    objHere = objHere->Next;
  }

  return FALSE;
}


//
// checkForObjHeresofType : Checks for any objectHere nodes whereever they may
//                          show up (of a certain type..)  TRUE if found..
//

char checkForObjHeresofType(const dikuObject *objType)
{
  dikuRoom *room = roomHead;
  editableListNode *edit = inventory;
  objectHere *objHere;//, *objHere2;
  mobHere *mobHere;
  long i;


  while (room)
  {
    objHere = room->objectHead;

    while (objHere)
    {
      if (objHere->objectPtr == objType) return TRUE;

      if (checkEntireObjHereListForType(objHere->objInside, objType))
        return TRUE;

      objHere = objHere->Next;
    }

    mobHere = room->mobHead;

    while (mobHere)
    {
      if (checkEntireObjHereListForType(mobHere->objectHead, objType))
        return TRUE;

/*
      if (checkEntireObjHereListForType(mobHere->equipHead, objType))
        return TRUE;
*/
      for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        if (checkEntireObjHereListForType(mobHere->equipment[i], objType))
          return TRUE;

      mobHere = mobHere->Next;
    }

    room = room->Next;
  }

 // check inventory

  while (edit)
  {
    if ((edit->entityType == ENTITY_OBJECT) &&
        (((objectHere *)(edit->entityPtr))->objectPtr == objType))
      return TRUE;

    edit = edit->Next;
  }

  return FALSE;
}


//
// scanObjHereListForLoadedContainer :
//       Scans through a list of objectHeres and their objInside pointers
//       for "loaded" containers (shit in them) of a certain vnum
//

char scanObjHereListForLoadedContainer(objectHere *objHere,
                                       const ulong containNumb)
{
  objectHere *obj = objHere;


  while (obj)
  {
    if ((obj->objectNumb == containNumb) && obj->objInside) return TRUE;

    if (scanObjHereListForLoadedContainer(obj->objInside, containNumb))
      return TRUE;

    obj = obj->Next;
  }

  return FALSE;
}


//
// checkForObjHeresWithLoadedContainer :
//            Checks for any objectHere nodes with "loaded" containers of a
//            certain object number - that is, containers with shit in them -
//            returns TRUE if any are found
//
//   containNumb : vnum of container to check for
//

char checkForObjHeresWithLoadedContainer(const ulong containNumb)
{
  dikuRoom *room = roomHead;
  objectHere *objHere;
  mobHere *mobHere;
  long i;


  while (room)
  {
    objHere = room->objectHead;

    while (objHere)
    {
      if (scanObjHereListForLoadedContainer(objHere, containNumb))
        return TRUE;

      objHere = objHere->Next;
    }

    mobHere = room->mobHead;

    while (mobHere)
    {
      if (scanObjHereListForLoadedContainer(mobHere->objectHead, containNumb))
        return TRUE;

      for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        if (scanObjHereListForLoadedContainer(mobHere->equipment[i],
                                              containNumb))
          return TRUE;

      mobHere = mobHere->Next;
    }

    room = room->Next;
  }

  return FALSE;
}


//
// resetObjHereList : resets (renumbers) all obj heres in a list
//

void resetObjHereList(const long oldNumb, const long newNumb,
                      objectHere *objHead)
{
  objectHere *objHere = objHead;


  while (objHere)
  {
    if (objHere->objectNumb == oldNumb) objHere->objectNumb = newNumb;

    if (objHere->objInside)
      resetObjHereList(oldNumb, newNumb, objHere->objInside);

    objHere = objHere->Next;
  }
}


//
// resetAllObjHere : "Resets" (renumbers) all objectHere nodes in all dikuRoom nodes
//                   nodes in the list pointed to by *room
//
//   oldNumb : see above
//   newNumb : ditto
//     *room : yep
//

void resetAllObjHere(const long oldNumb, const long newNumb, dikuRoom *room)
{
  mobHere *mob;
  long i;


  if (oldNumb == newNumb) return;

  while (room)
  {
    resetObjHereList(oldNumb, newNumb, room->objectHead);

    mob = room->mobHead;

    while (mob)
    {
      resetObjHereList(oldNumb, newNumb, mob->objectHead);

      for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        resetObjHereList(oldNumb, newNumb, mob->equipment[i]);

      mob = mob->Next;
    }

    room = room->Next;
  }
}


//
// addObjHeretoList
//

void addObjHeretoList(objectHere **objListHead, objectHere *objToAdd)
{
  objectHere *obj;


  if (!objToAdd)
  {
    _outtext("\nError in addObjHeretoList() - objToAdd is NULL.\n");
    return;
  }

  if (!*objListHead) *objListHead = objToAdd;
  else
  {
    obj = *objListHead;

    while (obj && obj->Next)
    {
      obj = obj->Next;
    }

    obj->Next = objToAdd;
  }

  objToAdd->Next = NULL;
}


//
// objinInv : checks mob's carried list for a particular object type (by ptr)
//

objectHere *objinInv(mobHere *mob, const dikuObject *obj)
{
  objectHere *objH;

  if (!mob || !obj) return NULL;

  objH = mob->objectHead;
  while (objH)
  {
    if (objH->objectPtr == obj) return objH;
    objH = objH->Next;
  }

  return NULL;
}


//
// objinInv : check's mob's carried list for a particular object type (by vnum)
//

objectHere *objinInv(mobHere *mob, const ulong objNumb)
{
  objectHere *objH;

  if (!mob) return NULL;

  objH = mob->objectHead;
  while (objH)
  {
    if (objH->objectNumb == objNumb) return objH;
    objH = objH->Next;
  }

  return NULL;
}


//
// isObjTypeUsed : is a particular object type used?  (in other words,
//                 is it loaded or used by a shop or quest)
//

char isObjTypeUsed(ulong numb)
{
  dikuMob *mob = mobHead;
  questQuest *qstq;
  questItem *qsti;
  ulong i;


  if (getNumbEntities(ENTITY_OBJECT, numb, FALSE)) return TRUE;

 // check quests and shops

  while (mob)
  {
    if (mob->questPtr)
    {
      qstq = mob->questPtr->questHead;
      while (qstq)
      {
        qsti = qstq->questPlayRecvHead;
        while (qsti)
        {
          if ((qsti->itemType == QUEST_RITEM_OBJ) && (qsti->itemVal == numb))
          {
            return TRUE;
          }

          qsti = qsti->Next;
        }

        qstq = qstq->Next;
      }
    }

    if (mob->shopPtr)  // this code is semi-pointless, since it sets mobs' inv
    {                  // appropriately anyway when saving improperly set shops
      for (i = 0; mob->shopPtr->producedItemList[i]; i++)
      {
        if (mob->shopPtr->producedItemList[i] == numb) return TRUE;
      }
    }

    mob = mob->Next;
  }

  return FALSE;
}


//
// resetObjHEntityPointersByNumb
//

void resetObjHEntityPointersByNumb(objectHere *objH)
{
  while (objH)
  {
    resetObjHEntityPointersByNumb(objH->objInside);

    objH->objectPtr = findObj(objH->objectNumb);

    objH = objH->Next;
  }
}
