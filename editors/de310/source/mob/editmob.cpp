//
//  File: editmob.cpp    originally part of dikuEdit
//
//  Usage: user-interface functions for editing mobs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "mob.h"

extern char madeChanges;
extern ulong numbLookupEntries, lowestMobNumber, highestMobNumber, numbMobTypes;
extern dikuRoom *roomHead;
extern dikuMob **mobLookup;

//
// editMobTypeDesc : Edit mob type description function - this function is very
//                   complex, by the way
//
//   *mob : mob to edit description of
//

void editMobTypeDesc(dikuMob *mob, const char updateChanges)
{
  stringNode *stringCopy;

  if (updateChanges) stringCopy = copyStringNodes(mob->mobDescHead);

  mob->mobDescHead = editStringNodes(mob->mobDescHead, TRUE);

  if (updateChanges)
  {
    if (!compareStringNodes(mob->mobDescHead, stringCopy))
    {
      madeChanges = TRUE;
    }

    deleteStringNodes(stringCopy);
  }
}

void editMobTypeProg(dikuMob *mob, const char updateChanges)
{
  stringNode *stringCopy;

  if (updateChanges) stringCopy = copyStringNodes(mob->mobProgHead);

  mob->mobProgHead = editStringNodes(mob->mobProgHead, TRUE);

  if (updateChanges)
  {
    if (!compareStringNodes(mob->mobProgHead, stringCopy))
    {
      madeChanges = TRUE;
    }

    deleteStringNodes(stringCopy);
  }
}

//
// displayEditMobTypeMenu : Displays edit mob type menu
//
//   *mob : mob being edited
//

void displayEditMobTypeMenu(const dikuMob *mob)
{
  char strn[256], newName[256], *outStrn = new char[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&n&+gEditing mob type #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit mob short name&n\n"
  "   &+YB&+L.&n &+wEdit mob long name&n\n"
  "   &+YC&+L.&n &+wEdit mob keywords &+c(%s)&n\n"
  "   &+YD&+L.&n &+wEdit mob description &+c(exists: %s)&n\n"
  "   &+YE&+L.&n &+wEdit mob action flags &+c(%u)&n\n"
  " &+YF/G&+L.&n &+wEdit mob affect1/affect2 flags &+c(%u/%u)&n\n"
  " &+YH/I&+L.&n &+wEdit mob affect3/affect4 flags &+c(%u/%u)&n\n"
  "   &+YJ&+L.&n &+wEdit species &+c(%s)&n, alignment &+c(%d)&n, and class &+c(%s)&n\n"
  "   &+YK&+L.&n &+wEdit mob level &+c(%u)&n, position &+c(%s)&n, default pos &+c(%s)&n,\n"
  "      and sex &+c(%s)\n"
  "\n"
  " &+YQ/S&+L.&n &+wEdit mob quest/shop info &+c(exists: %s/%s)&n\n"
  "   &+YL&+L.&n &+wSet limit override value for all mobs of type &+c(%u/%u)&n\n"
  "   &+YV&+L.&n &+wChange mob's vnum&n\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s",
    getReadableKeywordStrn(mob->keywordListHead, strn),

    getYesNoStrn((void *)(mob->mobDescHead)),

    mob->actionBits.longIntFlags,
    mob->affect1Bits.longIntFlags,
    mob->affect2Bits.longIntFlags,
    mob->affect3Bits.longIntFlags,
    mob->affect4Bits.longIntFlags,

    getMobSpeciesStrn(mob->mobSpecies), 
     mob->alignment, getMobClassStrn(mob->mobClass),

    mob->level, getMobPosStrn(mob->position), getMobPosStrn(mob->defaultPos),
     getMobSexStrn(mob->sex),

    getYesNoStrn((void *)(mob->questPtr)),
    getYesNoStrn((void *)(mob->shopPtr)),

    getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE),
    getNumbEntities(ENTITY_MOB, mob->mobNumber, TRUE),

    getMenuPromptName());
  }
  else  // no extra info
  {
    strcpy(outStrn,
  "   &+YA&+L.&n &+wEdit mob short name&n\n"
  "   &+YB&+L.&n &+wEdit mob long name&n\n"
  "   &+YC&+L.&n &+wEdit mob keywords&n\n"
  "   &+YD&+L.&n &+wEdit mob description&n\n"
  "   &+YE&+L.&n &+wEdit mob action flags&n\n"
  "   &+YF&+L.&n &+wEdit mob affect1 flags&n\n"
  "   &+YG&+L.&n &+wEdit mob affect2 flags&n\n"
  "   &+YH&+L.&n &+wEdit mob affect3 flags&n\n"
  "   &+YI&+L.&n &+wEdit mob affect4 flags&n\n"
  "   &+YJ&+L.&n &+wEdit mob species, alignment, and class&n\n"
  "   &+YK&+L.&n &+wEdit mob level, position, default pos, and sex\n"
/*  "   &+YM&+L.&n &+wEdit mobprogs&n\n"*/
  " &+YQ/S&+L.&n &+wEdit mob quest/shop info\n"
  "   &+YL&+L.&n &+wSet limit override value for all mobs of type&n\n"
  "   &+YV&+L.&n &+wChange mob's vnum&n\n"
  "\n"
  MENU_COMMON
  "\n");

    strcat(outStrn, getMenuPromptName());
  }

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditMobTypeMenu : Interprets user input for edit mob type menu -
//                         returns TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *mob : mob to edit
//

char interpEditMobTypeMenu(usint ch, dikuMob *mob, dikuMob *origMob)
{
  struct rccoord coords;
  char strn[256], newName[256], numbStrn[64];
  ulong i;


 // edit mob short name

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew mob short name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(mob->mobShortName, MAX_MOBSNAME_LEN - 1 - 1 - 1, 1, 7, '°',
            mob->mobShortName, FALSE, FALSE);

    remTrailingSpaces(mob->mobShortName);
    remLeadingSpaces(mob->mobShortName);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob long name

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew mob long name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(mob->mobLongName, MAX_MOBLNAME_LEN - 1 - 1 - 1, 1, 7, '°',
            mob->mobLongName, FALSE, FALSE);

    remTrailingSpaces(mob->mobLongName);
    remLeadingSpaces(mob->mobLongName);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob keywords

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew mob keywords: &n");

    createKeywordString(mob->keywordListHead, strn);
    strn[strlen(strn) - 1] = '\0';  // get rid of tilde

    getStrn(strn, MAX_MOBKEY_LEN - 1, 1, 7, '°', strn, FALSE, FALSE);

    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    strcat(strn, "~");

    deleteStringNodes(mob->keywordListHead);
    mob->keywordListHead = createKeywordList(strn);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob description

  if (ch == 'D')
  {
    editMobTypeDesc(mob, FALSE);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob extra flags

  if (ch == 'E')
  {
    editMobActionFlags(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob affect1 flags

  if (ch == 'F')
  {
    editMobAffect1Flags(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob affect2 flags

  if (ch == 'G')
  {
    editMobAffect2Flags(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob affect3 flags

  if (ch == 'H')
  {
    editMobAffect3Flags(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob affect4 flags

  if (ch == 'I')
  {
    editMobAffect4Flags(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob species, hometown, alignment, class

  if (ch == 'J')
  {
    editMobMisc(mob, origMob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mob level, position, default pos, sex

  if (ch == 'K')
  {
    editMobMisc2(mob);

    displayEditMobTypeMenu(mob);
  }
  else

 // edit mobprogs

/*  if (ch == 'M')
  {
    editMobTypeProg(mob, FALSE);

    displayEditMobTypeMenu(mob);
  }
*/

 // edit quest

  if (ch == 'Q')
  {
    if (!mob->questPtr)
    {
      coords = _gettextposition();

      _settextposition(coords.row, 1);

      clrline(coords.row);
      displayColorString(
"&n&+cThis mob currently has no quest information - create some (&+CY/n&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

      if (ch == 'N')
      {
        displayEditMobTypeMenu(mob);  // fix this later..
        return FALSE;
      }

      mob->questPtr = createQuest();
    }

    editQuest(mob->questPtr, mob->mobShortName, mob->mobNumber, mob, TRUE);

    displayEditMobTypeMenu(mob);
  }
  else

  if (ch == 'S')
  {
    if (!mob->shopPtr)
    {
      coords = _gettextposition();

      _settextposition(coords.row, 1);

      clrline(coords.row);
      displayColorString(
"&n&+cThis mob currently has no shop information - create some (&+CY/n&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

      if (ch == 'N')
      {
        displayEditMobTypeMenu(mob);  // fix this later..
        return FALSE;
      }

      mob->shopPtr = createShop();
    }

    editShop(mob->shopPtr, mob->mobShortName, mob->mobNumber, mob, TRUE);

    displayEditMobTypeMenu(mob);
  }
  else

 // change limit

  if (ch == 'L')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    displayColorString(
"&+CNew limit on loads for this mob type (0 = number loaded): &n");

    itoa(getNumbEntities(ENTITY_MOB, mob->mobNumber, TRUE), numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strnumer(strn))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);
    if ((i <= getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE)) && (i != 0))
    {
      _settextposition(coords.row, 1);
      clrline(coords.row, 7, 0);
      _setbkcolor(0);

      displayColorString(
"&+CError: Limit being set lower than or equal to the number loaded - press any key&n");

      getkey();

      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    setEntityOverride(ENTITY_MOB, mob->mobNumber, i, 0);
    madeChanges = TRUE;

    displayEditMobTypeMenu(mob);
  }
  else

 // change vnum

  if ((ch == 'V') && !mob->defaultMob)
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    sprintf(strn, "&+YNew mob vnum (highest allowed %u): &n",
            numbLookupEntries - 1);
    displayColorString(strn);

    itoa(mob->mobNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if ((!strnumer(strn)) || (atoi(strn) >= numbLookupEntries) ||
        (findMob(atoi(strn))))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);

    resetMobHere(mob->mobNumber, i, roomHead);
    resetNumbLoaded(ENTITY_MOB, mob->mobNumber, i);

    mobLookup[i] = origMob;
    mobLookup[mob->mobNumber] = NULL;

    mob->mobNumber = i;

    resetEntityPointersByNumb(FALSE, TRUE);

    _settextposition(1, 1);

    clrline(1, 0, 0);
    fixMobName(mob->mobShortName, newName);

    _setbkcolor(0);
    sprintf(strn, "&n&+gEditing mob type #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            mob->mobNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editMobType : "main" function for edit mob type menu
//
//  *mob : mob being edited
//

dikuMob *realEditMobType(dikuMob *mob, const char allowJump /*= TRUE*/)
{
  usint ch;
  dikuMob *newMob;
  char done = FALSE;
  ulong numb, oldVnum;


  if (!mob) return NULL;  // ya never know

  oldVnum = mob->mobNumber;

 // copy mob into newMob and display the menu

  newMob = copyMobType(mob, FALSE);

  displayEditMobTypeMenu(newMob);

  while (TRUE)
  {
    ch = toupper(getkey());

#ifndef __UNIX__
    if (ch == '$')
    {
      displayFreeStack(1, 1, FALSE);
    }
#endif

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {

     // if user changed vnum and aborted, set it back

      if (newMob->mobNumber != oldVnum)
      {
        resetMobHere(newMob->mobNumber, oldVnum, roomHead);
        resetNumbLoaded(ENTITY_MOB, newMob->mobNumber, oldVnum);

        mobLookup[oldVnum] = mob;
        mobLookup[newMob->mobNumber] = NULL;
      }

      deleteMobType(newMob, FALSE);

      _outtext("\n\n");

      return NULL;
    }

    if (allowJump)
    {
      if ((checkMenuKey(ch, FALSE) == MENUKEY_NEXT) &&
          (mob->mobNumber != getHighestMobNumber()))
      {
        done = TRUE;
      }

      if ((checkMenuKey(ch, FALSE) == MENUKEY_PREV) &&
          (mob->mobNumber != getLowestMobNumber()))
      {
        done = TRUE;
      }

      if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP) done = TRUE;
    }

   // if interpEditMobTypeMenu is TRUE, user has quit

    if (!done) done = interpEditMobTypeMenu(ch, newMob, mob);

    if (done)
    {
      if (!compareMobType(mob, newMob))   // changes have been made
      {
        deleteMobTypeAssocLists(mob);

        memcpy(mob, newMob, sizeof(dikuMob));

        delete newMob;

        numb = mob->mobNumber;

        if (!mob->defaultMob)
        {
          if (numb < lowestMobNumber) lowestMobNumber = numb;
          if (numb > highestMobNumber) highestMobNumber = numb;

          if (numbMobTypes == 1) lowestMobNumber = highestMobNumber = numb;
        }

        madeChanges = TRUE;

       // recreate all editable and master keyword lists.  no choice..

        updateAllMobMandElists();  // update rooms with mobs

       // recreate inventory keywords for appropriate mobs

        updateInvKeywordsMob(mob);
      }
      else deleteMobType(newMob, FALSE);

      if (checkMenuKey(ch, FALSE) != MENUKEY_JUMP) _outtext("\n\n");

      if (allowJump)
      {
        if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP)
        {
          switch (jumpMob(mob, &numb))
          {
            case MENU_JUMP_ERROR : return mob;
            case MENU_JUMP_VALID : return findMob(numb);
            default : _outtext("ERR in editMobType()\n"); getkey(); return NULL;
          }
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_NEXT)
        {
          return getNextMob(mob->mobNumber);
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_PREV)
        {
          return getPrevMob(mob->mobNumber);
        }
      }

      return NULL;
    }
  }
}


void editMobType(dikuMob *mob, const char allowJump /*= TRUE*/)
{
  do
  {
    mob = realEditMobType(mob, allowJump);
  } while (mob);
}


char jumpMob(const dikuMob *mob, ulong *numb)
{
  struct rccoord coords;
  char strn[128], numbStrn[64];
  ulong numb2;


  coords = _gettextposition();
//  coords.row -= 2;

  _settextposition(coords.row, 1);
  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn, "&+CJump to which mob (%u-%u, ? for list, $ to search): &n",
            getLowestMobNumber(), getHighestMobNumber());
    displayColorString(strn);

    itoa(mob->mobNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strcmp(strn, "?"))
    {
      displayMobTypeList();
    }
    else
    if (!strcmp(strn, "$"))
    {
      strn[0] = 0;

      _setbkcolor(0);
      displayColorString("\n&+CEnter substring to search mob list for: &n");
      getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
      upstrn(strn);

      displayMobTypeList(strn);
    }
    else
    {
      numb2 = strtoul(strn, NULL, 10);

      if ((!strnumer(strn)) || (numb2 >= numbLookupEntries) ||
          (!findMob(numb2)))
      {
        clrline(coords.row, 7, 0);
        _settextposition(coords.row, 1);
        _setbkcolor(0);

        displayColorString(getMenuPromptName());

        return MENU_JUMP_ERROR;
      }
      else
      {
        *numb = numb2;
        return MENU_JUMP_VALID;
      }
    }
  }
}
