//
//  File: crtobj.cpp     originally part of dikuEdit
//
//  Usage: functions related to creating object types and objHeres
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>

#include "../types.h"
#include "../fh.h"

#include "object.h"
#include "objhere.h"

using namespace std;

extern dikuRoom *roomHead;
extern dikuObject *defaultObject, *objectHead, **objLookup;
extern ulong numbObjs, numbLookupEntries, numbObjTypes, lowestObjNumber, highestObjNumber;
extern char madeChanges;

//
// createObjectType : Function to create a new object type - returns pointer to
//                    the new object type
//
//    incLoaded : if TRUE, assumes object is being added to global list
//      objNumb : number of obj type being created (if -1, assigns first
//                free vnum)
//

dikuObject *createObjectType(const char incLoaded, const long objNumb)
{
  dikuObject *newObj;
  ulong numb;


 // create a new object type

  newObj = new dikuObject;
  if (!newObj)
  {
    cout << "\n\nError creating new dikuObject in createObjectType().  "
            "(memory problem)  Aborting.\n\n";

    return NULL;
  }

  if (defaultObject)
  {
    newObj = copyObjectType(defaultObject, FALSE);
  }
  else
  {
    memset(newObj, 0, sizeof(dikuObject));

    strcpy(newObj->objShortName, "an unnamed object");
    strcpy(newObj->objLongName,
           "An unnamed object lies here, looking lonely and forelorn.");

    newObj->keywordListHead = createKeywordList("object unnamed~");

    newObj->objType = ITEM_TRASH;
    newObj->size = OBJSIZE_MEDIUM;
    newObj->material = MAT_IRON;
    newObj->condition = 100;
    newObj->craftsmanship = OBJCRAFT_AVERAGE;

    newObj->antiBits.objAntiBits.allowAll = 1;  // this is good
  }

  if (objNumb >= 0)
  {
    newObj->objNumber = numb = objNumb;

    if (findObj(numb))  // whoops, a dupe
    {
      deleteObjectType(newObj, FALSE);
      return NULL;
    }
  }
  else
  {
    if (!objectHead) newObj->objNumber = numb = roomHead->roomNumber;
    else newObj->objNumber = numb = getFirstFreeObjNumber();
  }


  if (incLoaded)
  {

   // are we over the max?

    if (numb >= numbLookupEntries)
    {
      deleteObjectType(newObj, FALSE);
      return NULL;
    }

   // set everything appropriately

    numbObjTypes++;

    objLookup[numb] = newObj;

    if (numb > highestObjNumber) highestObjNumber = numb;
    if (numb < lowestObjNumber)  lowestObjNumber = numb;

    createPrompt();

    madeChanges = TRUE;
  }

  return newObj;
}


//
// createObjectHereinRoom : Function to create a new objectHere - adds it to a
//                          room's objectHere list
//
//   *room : room to create objectHere node in
//    *obj : object type of objectHere
//    numb : vnum of objectHere being created
//

void createObjectHereinRoom(dikuRoom *room, dikuObject *obj, const ulong numb)
{
  objectHere *newObjHere;


 // create a new objectHere

  newObjHere = new objectHere;
  if (!newObjHere)
  {
    cout << "\n\nError creating new objectHere in createObjectHere().  "
            "(memory problem)  Aborting.\n\n";

    return;
  }

  addObjHeretoList(&room->objectHead, newObjHere);

  memset(newObjHere, 0, sizeof(objectHere));

  newObjHere->objectNumb = numb;
  newObjHere->objectPtr = obj;

  newObjHere->randomChance = 100;

  deleteMasterKeywordList(room->masterListHead);
  room->masterListHead = createMasterKeywordList(room);

  deleteEditableList(room->editableListHead);
  room->editableListHead = createEditableList(room);

  addEntity(ENTITY_OBJECT, numb);

  numbObjs++;
  createPrompt();

  madeChanges = TRUE;
}


//
// createObjHere : Function to create a new objectHere
//
//           *obj : object type of objectHere
//  incLoadedList : if TRUE, increments stuff appropriately
//

objectHere *createObjHere(const ulong numb, const char incLoadedList)
{
  objectHere *newObjHere;


 // create a new objectHere

  newObjHere = new objectHere;
  if (!newObjHere)
  {
    cout << "\n\nError creating new objectHere in createObjHere().  "
            "(memory problem)  Aborting.\n\n";

    return NULL;
  }

  memset(newObjHere, 0, sizeof(objectHere));

  newObjHere->objectNumb = numb;
  newObjHere->objectPtr = findObj(numb);

  newObjHere->randomChance = 100;

  if (incLoadedList)
  {
    addEntity(ENTITY_OBJECT, numb);

    numbObjs++;
    createPrompt();

    madeChanges = TRUE;
  }

  return newObjHere;
}
