//
//  File: delobjt.cpp    originally part of dikuEdit
//
//  Usage: functions for deleting object types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../types.h"
#include "../fh.h"

#include "object.h"

extern dikuObject **objLookup;
extern ulong numbObjTypes;
extern char madeChanges;

//
// deleteObjectType : deletes all the associated baggage with an objectType rec
//
//       srcObject : record to delete
//    updateGlobal : if TRUE, decrements numbObjTypes variable
//

void deleteObjectType(dikuObject *srcObject, const char updateGlobal)
{
  ulong numb;

 // make sure src exists

  if (!srcObject) return;

  numb = srcObject->objNumber;

 // delete object keywords

  deleteStringNodes(srcObject->keywordListHead);

 // extra desc linked list

  deleteExtraDescs(srcObject->extraDescHead);

 // update "globally" - a bit ambiguous, but life's a bitch

  if (updateGlobal)
  {
    if (isObjTypeUsed(numb)) updateAllObjMandElists();

    objLookup[numb] = NULL;

    numbObjTypes--;
    createPrompt();

    resetLowHighObj();

    deleteEntityinLoaded(ENTITY_OBJECT, numb);

    madeChanges = TRUE;
  }

 // finally, delete the object itself

  delete srcObject;
}


//
// deleteObjectTypeinList : Deletes a specific dikuObject in a list that starts
//                          at *objHead, updating whatever needs to be updated
//                          for the list to be valid.
//

void deleteObjectTypeinList(dikuObject **objHead, dikuObject *obj,
                            const char decLoaded)
{
  dikuObject *prevObj;


  if (obj == (*objHead))
  {
    *objHead = (*objHead)->Next;

    deleteObjectType(obj, decLoaded);
  }
  else
  {
    prevObj = *objHead;

    while (prevObj && (prevObj->Next != obj))
    {
      prevObj = prevObj->Next;
    }

   // couldn't find an object that points to object being deleted - error

    if (!prevObj)
    {
      _outtext("\n\nERROR: deleteObjectTypeinList() - notify author\n\n");

      return;
    }

    prevObj->Next = obj->Next;

    deleteObjectType(obj, decLoaded);
  }
}


//
// deleteObjectTypeAssocLists : deletes all the associated baggage with an
//                              object type, but not the record itself
//
//     *srcObject : record to alter
//

void deleteObjectTypeAssocLists(dikuObject *srcObject)
{
 // make sure src exists

  if (!srcObject) return;

 // delete object keywords

  deleteStringNodes(srcObject->keywordListHead);

 // extra desc linked list

  deleteExtraDescs(srcObject->extraDescHead);
}
