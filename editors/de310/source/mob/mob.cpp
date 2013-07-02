//
//  File: mob.cpp        originally part of dikuEdit
//
//  Usage: multitudinous functions related to handling mob structures
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <ctype.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "mob.h"
#include "mobhere.h"

using namespace std;

extern ulong numbLookupEntries, numbMobTypes, lowestMobNumber, highestMobNumber;
extern dikuRoom *roomHead;
extern dikuMob **mobLookup, *mobHead;
extern char madeChanges;
extern editableListNode *inventory;

//
// findMob : Returns the address of the mob node that has the requested
//           mobNumber (if any)
//
//   mobNumber : mob number to look for in the dikuMob linked list
//

INLINEDEF dikuMob *findMob(const register ulong mobNumber)
{
  if (mobNumber >= numbLookupEntries) return NULL;

  return mobLookup[mobNumber];
}


//
// copyMobType : copies all the info from an mob type record into a new
//               record and returns the address of the new record
//
//   srcMob : source mob type record
//

dikuMob *copyMobType(const dikuMob *srcMob, const char incLoaded)
{
  dikuMob *newMob;


 // make sure src exists

  if (!srcMob) return NULL;

 // alloc mem for new rec

  newMob = new dikuMob;
  if (!newMob)
  {
    cout << "\n\nError creating new dikuMob in copyMobType().  "
            "Out of memory.\n\n";

    return NULL;
  }

//  newMob->Next = NULL;

 // first, copy the simple stuff

  memcpy(newMob, srcMob, sizeof(dikuMob));

 // next, the not-so-simple stuff

  newMob->keywordListHead = copyStringNodes(srcMob->keywordListHead);
  newMob->mobDescHead = copyStringNodes(srcMob->mobDescHead);
  newMob->questPtr = copyQuest(srcMob->questPtr);
  newMob->shopPtr = copyShop(srcMob->shopPtr);

  if (incLoaded)
  {
    numbMobTypes++;
    createPrompt();
  }

 // return the address of the new mob

  return newMob;
}


//
// compareMobInfo : compares it all, baby - returns FALSE if they don't
//                  match, TRUE if they do
//
//      in theory, this function should return different results based on
//      what matched - save that for later
//

uchar compareMobType(dikuMob *mob1, dikuMob *mob2)
{
  if (mob1 == mob2) return TRUE;
  if (!mob1 || !mob2) return FALSE;

 // check mob names etc blah blah

  if (strcmp(mob1->mobShortName, mob2->mobShortName)) return FALSE;
  if (strcmp(mob1->mobLongName, mob2->mobLongName)) return FALSE;
  if (mob1->mobNumber != mob2->mobNumber) return FALSE;

  if (mob1->actionBits.longIntFlags != mob2->actionBits.longIntFlags)
    return FALSE;

  if (mob1->affect1Bits.longIntFlags != mob2->affect1Bits.longIntFlags)
    return FALSE;

  if (mob1->affect2Bits.longIntFlags != mob2->affect2Bits.longIntFlags)
    return FALSE;

  if (mob1->alignment != mob2->alignment) return FALSE;

  if (strcmp(mob1->mobSpecies, mob2->mobSpecies)) return FALSE;
  if (mob1->mobHometown != mob2->mobHometown) return FALSE;
  if (mob1->mobClass != mob2->mobClass) return FALSE;

  if (mob1->level != mob2->level) return FALSE;
  if (mob1->thac0 != mob2->thac0) return FALSE;
  if (mob1->ac != mob2->ac) return FALSE;

  if (strcmp(mob1->hitPoints, mob2->hitPoints)) return FALSE;
  if (strcmp(mob1->mobDamage, mob2->mobDamage)) return FALSE;

  if (mob1->copper != mob2->copper) return FALSE;
  if (mob1->silver != mob2->silver) return FALSE;
  if (mob1->gold != mob2->gold) return FALSE;
  if (mob1->platinum != mob2->platinum) return FALSE;
  if (mob1->exp != mob2->exp) return FALSE;

  if (mob1->position != mob2->position) return FALSE;
  if (mob1->defaultPos != mob2->defaultPos) return FALSE;
  if (mob1->sex != mob2->sex) return FALSE;

 // quest and shop

  if (!compareQuestInfo(mob1->questPtr, mob2->questPtr)) return FALSE;
  if (!compareShopInfo(mob1->shopPtr, mob2->shopPtr)) return FALSE;

 // description and extra descs

  if (!compareStringNodes(mob1->keywordListHead, mob2->keywordListHead))
    return FALSE;

  if (!compareStringNodes(mob1->mobDescHead, mob2->mobDescHead))
    return FALSE;

  if (!compareStringNodes(mob1->mobProgHead, mob2->mobProgHead))
    return FALSE;

  return TRUE;
}


//
// getHighestMobNumber : Gets the highest mob number currently in the
//                       mobHead list, returning it..
//

INLINEDEF ulong getHighestMobNumber(void)
{
  return highestMobNumber;
}


//
// getLowestMobNumber : Gets the lowest mob number currently in the
//                      mobHead list, returning it..
//

INLINEDEF ulong getLowestMobNumber(void)
{
  return lowestMobNumber;
}


//
// getFirstFreeMobNumber : Gets the first "free" mob number currently in the
//                         mobHead list, returning it
//

INLINEDEF ulong getFirstFreeMobNumber(void)
{
  register ulong i;

  for (i = lowestMobNumber + 1; i <= highestMobNumber; i++)
  {
    if (!mobLookup[i]) return i;
  }

  return highestMobNumber + 1;
}


//
// fixMobName : Trims *oldName to MAX_MOBSHORT_LEN, setting *newName with
//              the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixMobName(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_MOBSHORT_LEN, !getShowColorVal());
}


//
// fixMobNameShort : Trims *oldName to MAX_MOBSHORTSHORT_LEN, setting *newName
//                   with the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixMobNameShort(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_MOBSHORTSHORT_LEN, !getShowColorVal());
}

//
// checkAndFixRefstoMob :  scans through the quest list, looking for
//                         the few quest types that specify a mob vnum
//                         in one of their values and changing it to
//                         new number if necessary
//
//      oldNumb : old vnum
//      newNumb : new vnum
//

void checkAndFixRefstoMob(const ulong oldNumb, const ulong newNumb)
{
  ulong i;
  dikuMob *mob = mobHead;
  questQuest *qst;
  questItem *item;

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
          if ((item->itemType == QUEST_RITEM_CREATEMOB ||
               item->itemType == QUEST_RITEM_CREATEPET ) &&
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

}



//
// rebuildMobLookup
//

void rebuildMobLookup(void)
{
  dikuMob *mob = mobHead;
  ulong i;

  if (!mobLookup) return;  // whoops

  memset(mobLookup, 0, sizeof(dikuMob *) * numbLookupEntries);

  lowestMobNumber = numbLookupEntries;
  highestMobNumber = 0;

  while (mob)
  {
    i = mob->mobNumber;

    mobLookup[i] = mob;
    if (i < lowestMobNumber) lowestMobNumber = i;
    if (i > highestMobNumber) highestMobNumber = i;

    mob = mob->Next;
  }
}


//
// renumberMobs : Renumbers the mobs so that there are no "gaps" - starts
//                at the first mob and simply renumbers like mad from there
//

void renumberMobs(const char renumberHead, const long oldHeadNumb)
{
  dikuMob *mobNode = mobHead;
  long lastNumb = mobNode->mobNumber - 1, oldNumb;


  if (renumberHead)
  {
    resetMobHere(oldHeadNumb, mobNode->mobNumber, roomHead);
    resetNumbLoaded(ENTITY_MOB, oldHeadNumb, mobNode->mobNumber);

    madeChanges = TRUE;
  }

  while (mobNode)
  {
   // first, renumber mob types and mobHeres of these types

    if ((lastNumb + 1) != mobNode->mobNumber)
    {
      oldNumb = mobNode->mobNumber;
      mobNode->mobNumber = lastNumb + 1;

      resetMobHere(oldNumb, mobNode->mobNumber, roomHead);
      resetNumbLoaded(ENTITY_MOB, oldNumb, mobNode->mobNumber);
      checkAndFixRefstoMob(oldNumb,mobNode->mobNumber);

      madeChanges = TRUE;
    }

    lastNumb = mobNode->mobNumber;

    mobNode = mobNode->Next;
  }

  rebuildMobLookup();
//  resetEntityPointersByNumb(FALSE, TRUE);
}


//
// noMobNext
//

/*
char noMobNext(dikuMob *mob, char *substrn, usint pos)
{
  if (!mob) return TRUE;
  mob = mob->Next;

  while (mob)
  {
    if (scanKeyword(substrn, mob->keywordListHead, pos)) return FALSE;

    mob = mob->Next;
  }

  return TRUE;
}
*/


//
// getPrevMob
//

INLINEDEF dikuMob *getPrevMob(const ulong mobNumb)
{
  ulong i = mobNumb - 1;

  if (mobNumb <= lowestMobNumber) return NULL;

  while (!mobLookup[i]) i--;

  return mobLookup[i];
}


//
// getNextMob : find object right after objNumb, numerically
//

INLINEDEF dikuMob *getNextMob(const ulong mobNumb)
{
  ulong i = mobNumb + 1;

  if (mobNumb >= getHighestMobNumber()) return NULL;

  while (!mobLookup[i]) i++;

  return mobLookup[i];
}


//
// checkDieStrnValidityShort - no output, no dying, just a boolean returned
//

char checkDieStrnValidityShort(const char *strn)
{
  ulong i;
  char foundDigit = FALSE;


 // remove spaces throughout the string

/*
  for (i = 0; strn[i] != '\0'; i++)
  {
    if (strn[i] == ' ') deleteChar(strn, i);
    if (strn[i] == '\0') break;
  }
*/

  i = 0;

  while ((strn[i] != '\0') && (toupper(strn[i]) != 'D'))
  {
    if (!isdigit(strn[i])) return FALSE;
    else foundDigit = TRUE;

    i++;
  }

  if (strn[i] == '\0') return FALSE;
  if (!foundDigit) return FALSE;

  foundDigit = FALSE;

  i++;

  while ((strn[i] != '\0') && (strn[i] != '+'))
  {
    if (!isdigit(strn[i])) return FALSE;
    else foundDigit = TRUE;

    i++;
  }

  if (strn[i] == '\0') return FALSE;
  if (!foundDigit) return FALSE;

  i++;

  if (!isdigit(strn[i])) return FALSE;

  while (strn[i] != '\0')
  {
    if (!isdigit(strn[i])) return FALSE;

    i++;
  }

  return TRUE;
}


//
// checkDieStrnValidity - checks an XdY+Z format string - returns TRUE if
//                        valid, FALSE if not, though right now this is
//                        useless, cuz it exits anyway.  hmm
//

char checkDieStrnValidity(const char *strn, const ulong mobNumb,
                          const char *fieldName)
{
  ulong i;
  char foundDigit = FALSE;


 // remove spaces throughout the string

/*
  for (i = 0; strn[i] != '\0'; i++)
  {
    if (strn[i] == ' ') deleteChar(strn, i);
    if (strn[i] == '\0') break;
  }
*/

  i = 0;

  while ((strn[i] != '\0') && (toupper(strn[i]) != 'D'))
  {
    if (!isdigit(strn[i]))
    {
      cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

      exit(1);
    }
    else foundDigit = TRUE;

    i++;
  }

  if ((strn[i] == '\0') || !foundDigit) // error
  {
    cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

    exit(1);
  }

  foundDigit = FALSE;
  i++;

  while ((strn[i] != '\0') && (strn[i] != '+'))
  {
    if (!isdigit(strn[i])) // error
    {
      cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

      exit(1);
    }
    else foundDigit = TRUE;

    i++;
  }

  if ((strn[i] == '\0') || !foundDigit) // error
  {
    cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

    exit(1);
  }

  i++;

  if (!isdigit(strn[i]))
  {
    cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

    exit(1);
  }

  while (strn[i] != '\0')
  {
    if (!isdigit(strn[i])) // error
    {
      cout << "\n"
"Error in " << fieldName << " string for mob #" << mobNumb << "...\n"
"String read is '" << strn << "'.  Aborting..\n";

      exit(1);
    }

    i++;
  }

  return TRUE;
}


//
// deleteMobsinInv
//

void deleteMobsinInv(const dikuMob *mobPtr)
{
  editableListNode *edit = inventory;

  if (!mobPtr) return;

  while (edit)
  {
    if ((edit->entityType == ENTITY_MOB) &&
        (((mobHere *)(edit->entityPtr))->mobPtr == mobPtr))
      deleteEditableinList(&inventory, edit);

    edit = edit->Next;
  }
}


//
// updateInvKeywordsMob
//

void updateInvKeywordsMob(const dikuMob *mobPtr)
{
  editableListNode *edit = inventory;

  if (!mobPtr) return;

  while (edit)
  {
    if ((edit->entityType == ENTITY_MOB) &&
        (((mobHere *)(edit->entityPtr))->mobPtr == mobPtr))
    {
      deleteStringNodes(edit->keywordListHead);
      edit->keywordListHead = copyStringNodes(mobPtr->keywordListHead);
    }

    edit = edit->Next;
  }
}


//
// resetLowHighMob
//

INLINEDEF void resetLowHighMob(void)
{
  ulong i, high = 0, low = numbLookupEntries;

  for (i = 0; i < numbLookupEntries; i++)
  {
    if (mobLookup[i])
    {
      if (i > high) high = i;
      if (i < low) low = i;
    }
  }

  lowestMobNumber = low;
  highestMobNumber = high;
}
