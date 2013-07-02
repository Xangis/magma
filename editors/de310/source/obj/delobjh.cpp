//
//  File: delobjh.cpp    originally part of dikuEdit
//
//  Usage: functions for deleting objectHere structures
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../types.h"
#include "../fh.h"

#include "objhere.h"
#include "../zone/zone.h"

extern dikuRoom *roomHead;
extern ulong numbObjs;

//
// deleteObjHereList : Deletes an entire objectHere list
//
//    srcObjHere : take a wild guess, bucko
//     decLoaded : if TRUE, decrements stuff
//

void deleteObjHereList(objectHere *srcObjHere, const char decLoaded)
{
  objectHere *nextObjHere;


  while (srcObjHere)
  {
    nextObjHere = srcObjHere->Next;

    deleteObjHereList(srcObjHere->objInside, decLoaded);

    if (decLoaded)
    {
      decEntity(ENTITY_OBJECT, srcObjHere->objectNumb);
      numbObjs--;
    }

    delete srcObjHere;

    srcObjHere = nextObjHere;
  }

  if (decLoaded) createPrompt();  // bit more efficient..
}


//
// deleteObjHere : Deletes one objectHere node
//
//    objHere : take a wild guess, bucko
//

void deleteObjHere(objectHere *objHere, const char decLoaded)
{
  if (!objHere) return;

  deleteObjHereList(objHere->objInside, decLoaded);
                                          // takes care of all objectHere
                                          // nodes in objInside

  if (decLoaded)
  {
    decEntity(ENTITY_OBJECT, objHere->objectNumb);
    numbObjs--;
    createPrompt();
  }

  delete objHere;
}


//
// deleteObjHereinList : Deletes a specific objectHere in a list that starts
//                       at *objHead, updating whatever needs to be updated for
//                       the list to be valid.
//

void deleteObjHereinList(objectHere **objHead, objectHere *objHere,
                         const char decLoaded)
{
  objectHere *prevObj;


  if (!objHere)
  {
    _outtext("\n\nERROR: deleteObjHereinList() - objHere is NULL\n\n");

    return;
  }

  if (objHere == (*objHead))
  {
    *objHead = (*objHead)->Next;

    deleteObjHere(objHere, decLoaded);
  }
  else
  {
    prevObj = *objHead;

    while (prevObj && (prevObj->Next != objHere))
    {
      prevObj = prevObj->Next;
    }

   // couldn't find an object that points to object being deleted - error

    if (!prevObj)
    {
      _outtext("\n\nERROR: deleteObjHereinList() - no previous object found\n\n");

      return;
    }

    prevObj->Next = objHere->Next;

    deleteObjHere(objHere, decLoaded);
  }
}


//
// removeObjHerefromList : exactly like deleteObjHereinList, except it
//                         only removes the references to the record, not
//                         the record itself
//

void removeObjHerefromList(objectHere **objHead, objectHere *objHere)
{
  objectHere *prevObj;


  if (!objHere)
  {
    _outtext("\n\nERROR: deleteObjHereinList() - objHere is NULL\n\n");

    return;
  }

  if (objHere == (*objHead))
  {
    *objHead = (*objHead)->Next;
  }
  else
  {
    prevObj = *objHead;

    while (prevObj && (prevObj->Next != objHere))
    {
      prevObj = prevObj->Next;
    }

   // couldn't find an object that points to object being deleted - error

    if (!prevObj)
    {
      _outtext("\n\nERROR: removeObjHerefromList() - no previous object found\n\n");

      return;
    }

    prevObj->Next = objHere->Next;
  }

  objHere->Next = NULL;
}


//
// deleteAllObjHereofTypeInList :
//        Runs through a list pointed to by objHereHead, deleting all
//        objectHeres that match objType.  Fun
//

void deleteAllObjHereofTypeInList(objectHere **objHereHead,
                                  const dikuObject *objType,
                                  const char decLoaded)
{
  objectHere *objHere = *objHereHead, *prevObj, *nextObj;


  while (objHere)
  {
    nextObj = objHere->Next;

    if (objHere->objInside)
    {
      deleteAllObjHereofTypeInList(&(objHere->objInside), objType, decLoaded);
    }

    if (objHere->objectPtr == objType)
    {
      if (objHere == *objHereHead) *objHereHead = objHere->Next;
      else
      {
        prevObj = *objHereHead;
        while (prevObj && (prevObj->Next != objHere))
        {
          prevObj = prevObj->Next;
        }

        if (!prevObj)
        {
          _outtext(
"\n\nERROR: deleteAllObjHereofTypeInList() - notify author\n\n");

          return;
        }

        prevObj->Next = objHere->Next;
      }

      deleteObjHere(objHere, decLoaded);
    }

    objHere = nextObj;
  }
}


//
// deleteAllObjHeresofType : Deletes all objectHere nodes that match objType -
//                           all nodes in the editor, that is
//
//   currently only deletes objHeres in rooms, will be changed soon
//

void deleteAllObjHeresofType(const dikuObject *objType, const char decLoaded)
{
  dikuRoom *room = roomHead;
  mobHere *mob;
  long i;


 // delete objects laying about and shit that mobs are carrying and equipping
 // from every room

  while (room)
  {
    deleteAllObjHereofTypeInList(&(room->objectHead), objType, decLoaded);

    mob = room->mobHead;

    while (mob)
    {
      deleteAllObjHereofTypeInList(&(mob->objectHead), objType, decLoaded);

      for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        deleteAllObjHereofTypeInList(&(mob->equipment[i]), objType, decLoaded);

      mob = mob->Next;
    }

    room = room->Next;
  }
}
