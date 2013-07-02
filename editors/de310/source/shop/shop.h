#ifndef _SHOP_H_

#include "../types.h"

// shopkeeper race crap

#define SHOP_RACE_LOWEST         1
#define SHOP_RACE_HUMAN          1
#define SHOP_RACE_BARB           2
#define SHOP_RACE_DROW           3
#define SHOP_RACE_GREY           4
#define SHOP_RACE_DWARF          5
#define SHOP_RACE_DUERGAR        6
#define SHOP_RACE_HALFLING       7
#define SHOP_RACE_GNOME          8
#define SHOP_RACE_OGRE           9
#define SHOP_RACE_TROLL         10
#define SHOP_RACE_HALFELF       11
#define SHOP_RACE_RAKSHASA      12
#define SHOP_RACE_FLIND         13
#define SHOP_RACE_CENTAUR       14
#define SHOP_RACE_GITHYANKI     15
#define SHOP_RACE_GOBLIN        16
#define SHOP_RACE_MINOTAUR      17
#define SHOP_RACE_GITHZERAI     18
#define SHOP_RACE_HIGHEST       18

#define MAX_SHOPSTRING_LEN     (usint)256
#define MAX_NUMBSHOPITEMTYPES   (usint)50
#define MAX_NUMBSHOPITEMS       (usint)50

typedef struct _shop
{
  ulong producedItemList[MAX_NUMBSHOPITEMS];  // 0-terminated
  float buyMult;
  float sellMult;

  ulong tradedItemList[MAX_NUMBSHOPITEMTYPES];  // 0-terminated

  char notSellingItem[MAX_SHOPSTRING_LEN];
  char playerNoItem[MAX_SHOPSTRING_LEN];
  char shopNoTradeItem[MAX_SHOPSTRING_LEN];
  char shopNoMoney[MAX_SHOPSTRING_LEN];
  char playerNoMoney[MAX_SHOPSTRING_LEN];
  char sellMessage[MAX_SHOPSTRING_LEN];
  char buyMessage[MAX_SHOPSTRING_LEN];

//  ulong mobNumb;  // shouldn't be needed
  ulong roomNumb;  // room number mob's shop is in

  uchar firstOpen;
  uchar firstClose;
  uchar secondOpen;
  uchar secondClose;

  char roaming;
  char noMagic;
  char killable;

  char openMessage[MAX_SHOPSTRING_LEN];
  char closeMessage[MAX_SHOPSTRING_LEN];

  long hometownFlag;  // currently unused
  long socialActionTypes;  // unused

  char racist;
  char shopkeeperRace;

  char racistMessage[MAX_SHOPSTRING_LEN];
} shop;

#define _SHOP_H_
#endif
