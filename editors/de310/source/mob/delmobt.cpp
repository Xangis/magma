//
//  File: delmobt.cpp    originally part of dikuEdit
//
//  Usage: functions for deleting mob types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../fh.h"
#include "../types.h"

extern dikuMob **mobLookup;
extern ulong numbMobTypes, lowestMobNumber, highestMobNumber, numbLookupEntries;

//
// deleteMobType : deletes all the associated baggage with a mobType rec
//
//      *srcMob : record to delete
//  decNumbMobs : if TRUE, decrements numbMobTypes variable
//

void deleteMobType(dikuMob *srcMob, const char decNumbMobs)
{
  ulong numb;

 // make sure src exists

  if (!srcMob) return;

  numb = srcMob->mobNumber;

 // delete mob keywords

  deleteStringNodes(srcMob->keywordListHead);

 // delete mob desc

  deleteStringNodes(srcMob->mobDescHead);

 // delete mobprog

  deleteStringNodes(srcMob->mobProgHead);

 // delete quest

  deleteQuest(srcMob->questPtr);

 // delete shop

  deleteShop(srcMob->shopPtr);

 // finally, delete the record itself

  delete srcMob;

  if (decNumbMobs)
  {
    numbMobTypes--;
    if (!numbMobTypes)
    {
      lowestMobNumber = numbLookupEntries;
      highestMobNumber = 0;
    }

    mobLookup[numb] = NULL;
    createPrompt();
  }
}


//
// deleteMobTypeinList : Deletes a specific dikuMob in a list that starts
//                       at *mobHead, updating whatever needs to be updated
//                       for the list to be valid.
//

void deleteMobTypeinList(dikuMob **mobHead, dikuMob *mob, const char decLoaded)
{
  dikuMob *prevMob;


  if (mob == (*mobHead))
  {
    *mobHead = (*mobHead)->Next;

    deleteMobType(mob, decLoaded);
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
      _outtext("\n\nERROR: deleteMobTypeinList() - notify author\n\n");

      return;
    }

    prevMob->Next = mob->Next;

    deleteMobType(mob, decLoaded);
  }
}


//
// deleteMobTypeAssocLists : deletes all the associated baggage with a
//                           mob type, but not the record itself
//
//     *srcMob : record to alter
//

void deleteMobTypeAssocLists(dikuMob *srcMob)
{

 // make sure src exists

  if (!srcMob) return;

 // delete mob keywords

  deleteStringNodes(srcMob->keywordListHead);

 // delete mob desc

  deleteStringNodes(srcMob->mobDescHead);

 // delete mob quest

  deleteQuest(srcMob->questPtr);

 // delete mob shop

  deleteShop(srcMob->shopPtr);
}
