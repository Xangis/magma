//
//  File: shop.cpp       originally part of dikuEdit
//
//  Usage: various functions for manipulating shop records
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "shop.h"
#include "../mob/mob.h"

using namespace std;

extern ulong numbLookupEntries;
extern dikuRoom *currentRoom, *roomHead;
extern dikuMob **mobLookup, *mobHead;

//
// findShop : shrug
//

INLINEDEF shop *findShop(ulong numb)
{
  dikuMob *mob = findMob(numb);

  if (!mob) return NULL;

  return mob->shopPtr;
}


//
// getLowestShopMobNumber : making sure that a mob does indeed have a
//                          shop somewhere in the zone should be done
//                          before calling this function
//

INLINEDEF ulong getLowestShopMobNumber(void)
{
  long i, highest = getHighestMobNumber();

  for (i = getLowestMobNumber(); i <= highest; i++)
  {
    if (findMob(i) && findMob(i)->shopPtr) return i;
  }

  return numbLookupEntries;
}


//
// getHighestShopMobNumber : making sure that a mob does indeed have a
//                           shop somewhere in the zone should be done
//                           before calling this function
//

INLINEDEF ulong getHighestShopMobNumber(void)
{
  long i, lowest = getLowestMobNumber();

  for (i = getHighestMobNumber(); i >= lowest; i--)
  {
    if (findMob(i) && findMob(i)->shopPtr) return i;
  }

  return 0;
}


//
// getPrevShopMob : find shop mob right before mobNumb, numerically
//

dikuMob *getPrevShopMob(const ulong mobNumb)
{
  ulong i = mobNumb - 1;

  if (mobNumb <= getLowestShopMobNumber()) return NULL;

  while (!mobLookup[i] || !mobLookup[i]->shopPtr) i--;

  return mobLookup[i];
}


//
// getNextShopMob : find shop mob right after mobNumb, numerically
//

dikuMob *getNextShopMob(const ulong mobNumb)
{
  ulong i = mobNumb + 1;

  if (mobNumb >= getHighestShopMobNumber()) return NULL;

  while (!mobLookup[i] || !mobLookup[i]->shopPtr) i++;

  return mobLookup[i];
}


//
// createShop
//

shop *createShop(void)
{
  shop *shp = new shop;

  if (!shp)
  {
    cout << "\n\nerror alloc'ing in createShop(): aborting\n\n";
    return NULL;
  }

  memset(shp, 0, sizeof(shop));

  shp->buyMult = shp->sellMult = 1.0;

  strcpy(shp->notSellingItem, "I'm not selling that, $N!");
  strcpy(shp->playerNoItem, "You don't seem to have that item, $N.");
  strcpy(shp->shopNoTradeItem, "I won't buy that, $N.");
  strcpy(shp->shopNoMoney, "Sorry there $N, but I don't have the money.");
  strcpy(shp->playerNoMoney, "You don't have the money, $N!");
  strcpy(shp->sellMessage, "Thank you, $N, that will be $t.");
  strcpy(shp->buyMessage, "$N, I will give you $t for that.");

  shp->firstOpen = shp->secondOpen = 0;
  shp->firstClose = shp->secondClose = 28;

  shp->noMagic = TRUE;

  strcpy(shp->openMessage, "I am now open!");
  strcpy(shp->closeMessage, "My shop is now closed.");

  shp->shopkeeperRace = SHOP_RACE_HUMAN;

  strcpy(shp->racistMessage, "I do not sell to your people!");

  return shp;
}


//
// deleteShop
//

void deleteShop(shop *shp)
{
  if (!shp) return;

  delete shp;
}


//
// copyShop
//

shop *copyShop(const shop *srcShp)
{
  shop *newShp;


  if (!srcShp) return NULL;

  newShp = new shop;
  if (!newShp)
  {
    cout << "\n\ncopyShop(): error allocating memory.  aborting..\n\n";

    return NULL;
  }

  memcpy(newShp, srcShp, sizeof(shop));

  return newShp;
}


//
// compareShopInfo
//

char compareShopInfo(const shop *shp1, const shop *shp2)
{
  if (shp1 == shp2) return TRUE;
  if (!shp1 || !shp2) return FALSE;

  if (memcmp(shp1, shp2, sizeof(shop))) return FALSE;

  return TRUE;
}


//
// getNumbShopMobs
//

ulong getNumbShopMobs(void)
{
  dikuMob *mob = mobHead;
  ulong numbShops = 0;


  while (mob)
  {
    if (mob->shopPtr) numbShops++;

    mob = mob->Next;
  }

  return numbShops;
}


//
// checkForMobWithShop
//

char checkForMobWithShop(void)
{
  dikuMob *mob = mobHead;

  while (mob)
  {
    if (mob->shopPtr) return TRUE;

    mob = mob->Next;
  }

  return FALSE;
}


//
// displayShopList : Displays a list of mob types starting at mobHead
//                   that have shops
//

void displayShopList(void)
{
  dikuMob *mob = mobHead;
  ulong i = 0;
  char strn[256];


  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");

    return;
  }

  if (!checkForMobWithShop())
  {
    _outtext("\nNone of the current mob types have a shop.\n\n");
    return;
  }

  _outtext("\n\n");

  while (mob)
  {
    if (mob->shopPtr)
    {
      sprintf(strn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);
      displayColorString(strn);

      i += numbLinesStringNeeds(strn);
    }

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      i = 0;
    }

    mob = mob->Next;
  }

  _outtext("\n");
}


//
// displayShopList : Displays the list of mob types loaded into
//                   game with shops, starting at mobHead
//

void displayShopList(const char *args)
{
  dikuMob *mob = mobHead;
  ulong i = 0;
  char outStrn[512], searchStrn[512], foundMob = FALSE;


  strcpy(searchStrn, args);

  remLeadingSpaces(searchStrn);
  upstrn(searchStrn);  // just in case..  (interp.c should do it, but hey)

  if (strlen(searchStrn) == 0)
  {
    displayShopList();
    return;
  }

  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");
    return;
  }

  if (!checkForMobWithShop())
  {
    _outtext("\nNone of the current mob types have a shop.\n\n");
    return;
  }

  _outtext("\n\n");

  while (mob)
  {
    if (mob->shopPtr && (scanKeyword(searchStrn, mob->keywordListHead)))
    {
      sprintf(outStrn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);

      foundMob = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

      if (i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return;
        }

        i = 0;
      }
    }

    mob = mob->Next;
  }

  if (!foundMob) _outtext("No matching mob types found.\n");
  _outtext("\n");
}


//
// getShopRaceStrn
//

char *getShopRaceStrn(const char race)
{
  if ((race > SHOP_RACE_HIGHEST) || (race < SHOP_RACE_LOWEST))
  {
    return "out of range";
  }

  switch (race)
  {
    case SHOP_RACE_HUMAN : return "human";
    case SHOP_RACE_BARB  : return "barbarian";
    case SHOP_RACE_DROW  : return "drow elf";
    case SHOP_RACE_GREY  : return "grey elf";
    case SHOP_RACE_DWARF : return "mountain dwarf";
    case SHOP_RACE_DUERGAR : return "duergar dwarf";
    case SHOP_RACE_HALFLING : return "halfling";
    case SHOP_RACE_GNOME : return "gnome";
    case SHOP_RACE_OGRE  : return "ogre";
    case SHOP_RACE_TROLL : return "troll";
    case SHOP_RACE_HALFELF : return "half-elf";
    case SHOP_RACE_RAKSHASA : return "rakshasa";
    case SHOP_RACE_FLIND : return "flind";
    case SHOP_RACE_CENTAUR : return "centaur";
    case SHOP_RACE_GITHYANKI : return "githyanki";
    case SHOP_RACE_GOBLIN : return "goblin";
    case SHOP_RACE_MINOTAUR : return "minotaur";
    case SHOP_RACE_GITHZERAI : return "githzerai";

    default : return "unrecognized";
  }
}


//
// displayShopRaceList : Displays list of shopkeeper races
//

void displayShopRaceList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SHOP_RACE_LOWEST; i <= SHOP_RACE_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getShopRaceStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n\n");
}


//
// getNumbShopSold
//

ulong getNumbShopSold(const ulong *soldArr)
{
  ulong numb = 0, pos = 0;

  while (soldArr[pos])
  {
    numb++;
    pos++;
  }

  return numb;
}


//
// getNumbShopBought
//

ulong getNumbShopBought(const ulong *soldArr)
{
  ulong numb = 0, pos = 0;

  while (soldArr[pos])
  {
    numb++;
    pos++;
  }

  return numb;
}


//
// checkShopCarried : returns TRUE if shop is carrying everything it should
//                    be
//

/*
char checkShopCarried(shop *shp)
{
  return 0;
}
*/


//
// getShopinRoom
//

shop *getShopinRoom(dikuRoom *room)
{
  mobHere *mob;

  if (!room) return NULL;

  mob = room->mobHead;

 // should theoretically only be one shop mob in each room

  while (mob)
  {
    if (mob->mobPtr && (mob->mobPtr->shopPtr)) return mob->mobPtr->shopPtr;

    mob = mob->Next;
  }

  return NULL;
}


//
// getShopOwner
//

dikuMob *getShopOwner(shop *shp)
{
  dikuMob *mob = mobHead;


  if (!shp) return NULL;

  while (mob)
  {
    if (mob->shopPtr == shp) return mob;

    mob = mob->Next;
  }

  return NULL;
}


//
// listShopSold : returns TRUE if shop found, FALSE otherwise
//

char listShopSold(dikuRoom *room)
{
  shop *shp;
  ulong i, numbItems = 0;
  char objName[256], strn[256], mobName[256], tempstrn[256];
  dikuObject *obj;
  dikuMob *mob;
  mobHere *mobH;


 // check for shop in the same room

  shp = getShopinRoom(room);
  if (!shp) return FALSE;

 // run through shop list - go crazy, my man

  if (shp->producedItemList[0] == 0)
  {
    _outtext("\nThe shop in this room isn't selling anything.\n\n");
    return TRUE;
  }

  mob = getShopOwner(shp);
  if (!mob)
  {
    _outtext("\n_big_ error in shop listing code - notify author\n\n");
    return TRUE;
  }
  else fixMobName(mob->mobShortName, mobName);

  mobH = findMobHere(mob->mobNumber, currentRoom, NULL, TRUE);
  if (!mobH)
  {
    _outtext("\n_big_ error #2 in shop listing code - notify author\n\n");
    return TRUE;
  }

  sprintf(strn, "\n%s&n (#%u) is selling -\n\n", mobName, mob->mobNumber);
  displayColorString(strn);

  for (i = 0; shp->producedItemList[i]; i++)
  {
    obj = findObj(shp->producedItemList[i]);

    if (obj)
    {
      fixObjName(obj->objShortName, objName);

      if (getShowPricesAdjustedVal())
        getMoneyStrn((int)((float)obj->value * shp->sellMult), tempstrn);
      else
        getMoneyStrn(obj->value, tempstrn);
    }
    else
    {
      strcpy(objName, "(object not in this .OBJ file)");
      strcpy(tempstrn, "(price unknown)");
    }

    sprintf(strn, "  %s &n(#%u) - %s",
            objName, shp->producedItemList[i], tempstrn);

   // let user know if obj is in shop list but not inventory - handy

    if (!objinInv(mobH, shp->producedItemList[i]))
      strcat(strn, " &+c(not in inv)&n");

    strcat(strn, "\n");

    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return TRUE;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");

  return TRUE;
}


//
// getMobHereShop
//

mobHere *getMobHereShop(const shop *shp)
{
  dikuRoom *room = roomHead;
  mobHere *mob;

  if (!shp) return NULL;

  while (room)
  {
    mob = room->mobHead;
    while (mob)
    {
      if (mob->mobPtr && (mob->mobPtr->shopPtr == shp)) return mob;

      mob = mob->Next;
    }

    room = room->Next;
  }

  return NULL;
}


//
// checkAllShops : returns TRUE if fatal error
//

char checkAllShops(void)
{
  dikuMob *mob = mobHead;
  mobHere *mobH;
  objectHere *objH;
  char strn[1024];
  usint ch;
  ulong i;

  while (mob)
  {
    if (mob->shopPtr)
    {
      mobH = getMobHereShop(mob->shopPtr);
      if (!mobH)
      {
        sprintf(strn, "\n"
"error: mob #%u has a shop defined, but no instance of this mob is loaded.\n"
"       this is bad.  fix it.  skipping saving .SHP file..\n\n", mob->mobNumber);

        _outtext(strn);
        return TRUE;
      }

      for (i = 0; mob->shopPtr->producedItemList[i]; i++)
      {
        if (!objinInv(mobH, mob->shopPtr->producedItemList[i]))
        {
          sprintf(strn, "\n"
"warning: mob #%u's shop sells object #%u, but it isn't in the mob's\n"
"         carried list.  add it (Y/n)? ",
                  mob->mobNumber, mob->shopPtr->producedItemList[i]);

          _outtext(strn);

          do
          {
            ch = toupper(getkey());
          } while ((ch != 'N') && (ch != 'Y') && (ch != K_Enter));

          if (ch != 'N')
          {
            _outtext("yes\n\n");

            objH = createObjHere(mob->shopPtr->producedItemList[i], TRUE);
            if (!objH)
            {
              _outtext("\nbah, error allocating objectHere.  aborting..\n\n");
              return TRUE;
            }

            addObjHeretoList(&(mobH->objectHead), objH);

           // set override so the object reloads properly

            setEntityOverride(ENTITY_OBJECT, mob->shopPtr->producedItemList[i],
                              999, objH->objectPtr->globalmaxload);
          }
          else _outtext("no\n\n");
        }
      }
    }

    mob = mob->Next;
  }

  return FALSE;
}


//
// checkForDupeShopLoaded
//

void checkForDupeShopLoaded(void)
{
  dikuMob *mob = mobHead;

  while (mob)
  {
    if (mob->shopPtr &&
        (getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE) > 1))
    {
      cout << "\n" <<
"Error: mob #" << mob->mobNumber << " has a shop, and is loaded more than once.  you can't\n" <<
"       load a particular shop mob more than once, Paco.  aborting..\n";

      exit(1);
    }

    mob = mob->Next;
  }
}
