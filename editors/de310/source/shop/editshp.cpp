//
//  File: editshp.cpp    originally part of dikuEdit
//
//  Usage: user-interface functions for edit shops
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "editshp.h"

using namespace std;

extern char madeChanges;
extern ulong numbLookupEntries;

//
// displayEditShopMenu : Displays edit shop menu
//
//   *shp : shop being edited
//

void displayEditShopMenu(shop *shp, const char *mobName, const long mobNumb)
{
  char newName[256], *strn;


  strn = new char[4096];
  if (!strn)
  {
    _outtext("\n\nout of memory - cannot alloc strn in displayEditShopMenu\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&n&+gEditing shop for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wEdit list of items shop sells &+c(%u)\n"
  " &+YB/C&+L.&n &+wEdit buy/sell multipliers &+c(%.2f/%.2f)\n"
  "   &+YD&+L.&n &+wEdit list of item types shop buys &+c(%u)\n"
  "   &+YE&+L.&n &+wEdit shop messages\n"
  "   &+YF&+L.&n &+wEdit shop opening/closing times &+c(%u/%u|%u/%u)\n"
  "   &+YG&+L.&n &+wEdit shop roam/no magic/killable values &+c(%s/%s/%s)\n"
  "   &+YH&+L.&n &+wEdit shop racist info &+c(%s/%s)\n"
  "   &+YY&+L.&n &+wDelete shop\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getNumbShopSold(shp->producedItemList),
        shp->buyMult, shp->sellMult,
        getNumbShopBought(shp->tradedItemList),
        shp->firstOpen, shp->firstClose, shp->secondOpen, shp->secondClose,
        getYesNoStrn(shp->roaming), getYesNoStrn(shp->noMagic),
         getYesNoStrn(shp->killable),
        getYesNoStrn(shp->racist), getShopRaceStrn(shp->shopkeeperRace),

        getMenuPromptName());
  }
  else
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wEdit list of items shop sells\n"
  " &+YB/C&+L.&n &+wEdit buy/sell multipliers\n"
  "   &+YD&+L.&n &+wEdit list of item types shop buys\n"
  "   &+YE&+L.&n &+wEdit shop messages\n"
  "   &+YF&+L.&n &+wEdit shop opening/closing times\n"
  "   &+YG&+L.&n &+wEdit shop roam/no magic/killable values\n"
  "   &+YH&+L.&n &+wEdit shop racist info\n"
  "   &+YY&+L.&n &+wDelete shop\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getMenuPromptName());
  }

  displayColorString(strn);

  delete[] strn;
}


//
// interpEditShopMenu : Interprets user input for edit shop menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//    ch : user input
//  *shp : shop to edit
//

char interpEditShopMenu(usint ch, shop *shp, const char *mobName,
                        const long mobNumb, dikuMob *mob)
{
  struct rccoord coords;
  char strn[256];


 // edit list of sold items

  if (ch == 'A')
  {
    editShopSold(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit buy multipliers

  if (ch == 'B')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      clrline(coords.row);

      displayColorString("&+YNew buy multiplier: &n");

      sprintf(strn, "%.2f", shp->buyMult);

      getStrn(strn, 7, 1, 7, '°', strn, FALSE, FALSE);

      if ((atof(strn) <= 0.0) || (atof(strn) > 1.0))
      {
        _settextposition(coords.row, 1);
        _setbkcolor(0);
        displayColorString(
"&+YError - buy multiplier must be greater than 0.0 and less than or equal to 1.0");
        getkey();
      }
      else
      {
        shp->buyMult = atof(strn);
        break;
      }
    }

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit sell multiplier

  if (ch == 'C')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);

      clrline(coords.row);
      displayColorString("&+YNew sell multiplier: &n");

      sprintf(strn, "%.2f", shp->sellMult);

      getStrn(strn, 7, 1, 7, '°', strn, FALSE, FALSE);

      if (atof(strn) < 1.0)
      {
        _settextposition(coords.row, 1);
        _setbkcolor(0);
        displayColorString(
"&+YError - sell multiplier must be greater than or equal to 1.0");
        getkey();
      }
      else
      {
        shp->sellMult = atof(strn);
        break;
      }
    }

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit list of item types bought

  if (ch == 'D')
  {
    editShopBought(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop messages

  if (ch == 'E')
  {
    editShopMessages(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop opening/closing times

  if (ch == 'F')
  {
    editShopTimes(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop roam/no magic/killable crap

  if (ch == 'G')
  {
    editShopBooleans(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop racist info

  if (ch == 'H')
  {
    editShopRacist(shp, mobName, mobNumb);

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // delete entire shop

  if (ch == 'Y')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+cDelete this mob's shop info (&+Cy/N&n&+c)? ");

    while (TRUE)
    {
      ch = toupper(getkey());

      if ((ch == 'Y') || (ch == 'N') || (ch == K_Enter)) break;
    }

   // g'night, gracie

    if (ch == 'Y')
    {
      _outtext("\n\n");
      deleteShop(mob->shopPtr);
      mob->shopPtr = NULL;
      return SHOP_DELETED;
    }

    displayEditShopMenu(shp, mobName, mobNumb);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// realEditShop : edit a shop
//

dikuMob *realEditShop(shop *shp, const char *mobName, const long mobNumb,
                      dikuMob *mob, const char allowJump)
{
  usint ch;
  shop *newShop;
  char done = FALSE;
  ulong numb;


  if (!shp) return NULL;  // ya never know

 // copy shp into newShop and display the menu

  newShop = copyShop(shp);
  if (!newShop)
  {
    cout << "\n\nError allocating newShop in editShop(): aborting.\n\n";
    return NULL;
  }

  displayEditShopMenu(newShop, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteShop(newShop);

      _outtext("\n\n");

      return NULL;
    }

    if (allowJump)
    {
      if ((checkMenuKey(ch, FALSE) == MENUKEY_NEXT) &&
          (mobNumb != getHighestShopMobNumber()))
      {
        done = TRUE;
      }

      if ((checkMenuKey(ch, FALSE) == MENUKEY_PREV) &&
          (mobNumb != getLowestShopMobNumber()))
      {
        done = TRUE;
      }

      if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP) done = TRUE;
    }

   // if interpEditShopMenu is TRUE, user has quit

    if (!done) done = interpEditShopMenu(ch, newShop, mobName, mobNumb, mob);

    if (done == SHOP_DELETED)
    {
      return NULL;
    }

    if (done)
    {
      if (!compareShopInfo(shp, newShop))   // changes have been made
      {
        memcpy(shp, newShop, sizeof(shop));

        delete newShop;

        madeChanges = TRUE;
      }
      else deleteShop(newShop);

      _outtext("\n\n");

      if (allowJump)
      {
        if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP)
        {
          switch (jumpShop(&numb, mobNumb))
          {
            case MENU_JUMP_ERROR : return mob;
            case MENU_JUMP_VALID : return findMob(numb);
            default : _outtext("ERR in editShop()\n");
                      getkey();
                      return NULL;
          }
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_NEXT)
        {
          return getNextShopMob(mobNumb);
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_PREV)
        {
          return getPrevShopMob(mobNumb);
        }
      }

      return NULL;
    }
  }
}

void editShop(shop *shp, char *mobName, long mobNumb,
              dikuMob *mob, const char allowJump)
{
  dikuMob *mob2;

  do
  {
    mob2 = realEditShop(shp, mobName, mobNumb, mob, allowJump);

    if (!mob2) shp = NULL;
    else
    {
      mob = mob2;
      mobName = mob->mobShortName;
      mobNumb = mob->mobNumber;
      shp = mob->shopPtr;
    }
  } while (shp);
}


char jumpShop(ulong *numb, ulong mobNumb)
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
    sprintf(strn, "&+CJump to which shop mob (%u-%u, ? for list, $ to search): &n",
            getLowestShopMobNumber(), getHighestShopMobNumber());
    displayColorString(strn);

    utoa(mobNumb, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strcmp(strn, "?"))
    {
      displayShopList();
    }
    else
    if (!strcmp(strn, "$"))
    {
      strn[0] = 0;

      _setbkcolor(0);
      displayColorString("\n&+CEnter substring to search mob shop list for: &n");
      getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
      upstrn(strn);

      displayShopList(strn);
    }
    else
    {
      numb2 = strtoul(strn, NULL, 10);

      if (!strnumer(strn) || (numb2 >= numbLookupEntries) || !findShop(numb2))
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
