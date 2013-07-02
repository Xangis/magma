//
//  File: crtmob.cpp     originally part of dikuEdit
//
//  Usage: non-user functions for creating mobs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "mob.h"
#include "mobhere.h"

using namespace std;

extern ulong numbMobs, numbMobTypes, lowestMobNumber, highestMobNumber, numbLookupEntries;
extern char madeChanges;
extern dikuRoom *roomHead;
extern dikuMob *defaultMob, *mobHead, **mobLookup;

//
// createMobType : Function to create a new mob type - returns pointer to
//                 the new mob type
//

dikuMob *createMobType(const char incLoaded, const long mobNumb)
{
  dikuMob *newMob;
  ulong numb;


 // create a new mob type

  newMob = new dikuMob;
  if (!newMob)
  {
    cout << "\n\nError creating new dikuMob in createMobType().  "
            "(memory problem)  Aborting.\n\n";

    return NULL;
  }

  if (defaultMob)
  {
    newMob = copyMobType(defaultMob, FALSE);
  }
  else
  {
    memset(newMob, 0, sizeof(dikuMob));

   // default mob names

    strcpy(newMob->mobShortName, "an unnamed mob");
    strcpy(newMob->mobLongName,
           "An unnamed mob stands here, looking lonely and forelorn.");

    newMob->keywordListHead = createKeywordList("mob unnamed~");

   // level, position, sex, and species

    newMob->level = 1;
    newMob->position = newMob->defaultPos = POSITION_STANDING;
    newMob->sex = SEX_NEUTER;
    strcpy(newMob->mobSpecies, "PH");  // human

    newMob->actionBits.mobActionBits.isnpc = TRUE;  // all mobs are NPCs

   // default hit points and damage strings

    strcpy(newMob->hitPoints, "1d1+1");
    strcpy(newMob->mobDamage, "1d1+1");
  }

  if (mobNumb >= 0)
  {
    newMob->mobNumber = numb = mobNumb;

    if (findMob(numb))
    {
      deleteMobType(newMob, FALSE);
      return NULL;
    }
  }
  else
  {
    if (!mobHead) newMob->mobNumber = numb = roomHead->roomNumber;
    else newMob->mobNumber = numb = getFirstFreeMobNumber();
  }

  if ((numb >= numbLookupEntries) && incLoaded)
  {
    deleteMobType(newMob, FALSE);
    return NULL;
  }

  if (incLoaded)
  {
    numbMobTypes++;

    mobLookup[numb] = newMob;

    if (numb > highestMobNumber) highestMobNumber = numb;
    if (numb < lowestMobNumber)  lowestMobNumber = numb;

    createPrompt();
  }

  madeChanges = TRUE;

  return newMob;
}


//
// createMobHere : Function to create a new mobHere - returns pointer to
//                 the new mobHere - returns FALSE if not created
//
//   *room : room to create mobHere node in
//    *mob : mob type of mobHere
//

char createMobHereinRoom(dikuRoom *room, dikuMob *mob, const ulong numb)
{
  mobHere *newMobHere;


 // don't load more than one of the same type of shop

  if (mob && mob->shopPtr &&
      getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE))
  {
    _outtext("\n"
"Error: already one of this mob loaded - since it has a shop, no more than\n"
"       one can be loaded.  aborting..\n\n");

    return FALSE;
  }

 // create a new mob type

  newMobHere = new mobHere;
  if (!newMobHere)
  {
    _outtext("\n\nError creating new mobHere in createMobHere().  "
            "(memory problem)  Aborting.\n\n");

    return FALSE;
  }

  memset(newMobHere, 0, sizeof(mobHere));

 // tack new node onto end of list

  addMobHeretoList(&room->mobHead, newMobHere);

  newMobHere->mobNumb = numb;
  newMobHere->mobPtr = mob;

  newMobHere->randomChance = 100;

  deleteMasterKeywordList(room->masterListHead);
  room->masterListHead = createMasterKeywordList(room);

  deleteEditableList(room->editableListHead);
  room->editableListHead = createEditableList(room);


  addEntity(ENTITY_MOB, numb);
  numbMobs++;
  createPrompt();

  madeChanges = TRUE;

  return TRUE;
}
