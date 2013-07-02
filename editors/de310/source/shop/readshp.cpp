//
//  File: readshp.cpp    originally part of dikuEdit
//
//  Usage: functions for reading shop info from the .SHP file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "shop.h"

using namespace std;

extern char madeChanges, readFromSubdirs;

//
// readShopFromFile : read a mob's shop info from a file - returns pointer
//                    to shop info
//
//   *shopFile : file to read info from
//

shop *readShopFromFile(FILE *shopFile)
{
  char strn[256];
  long i, shopNumb;
  dikuMob *mob;
  shop *shopNode;


 // Read the shop number - read past blank lines

  while (TRUE)
  {
    if (!fgets(strn, 256, shopFile)) return NULL;  // reached end of shop file

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    if (strlen(strn)) break;
  }

 // allocate memory for shop

  shopNode = new shop;
  if (!shopNode)
  {
    cout <<
"Uh-oh, ran out of memory allocating memory for a new shop.  Aborting..\n";

    exit(1);
  }

 // set everything in shop record to 0/NULL

  memset(shopNode, 0, sizeof(shop));

 // do stuff to string read above

  if (strn[0] != '#')
  {
    cout <<
"String that should be vnum of shop doesn't start with a '#'.  String read was\n" <<
"'" << strn << "'.  Aborting..\n";

    exit(1);
  }

  i = strlen(strn) - 1;

  if (strn[i] != '~')
  {
    cout <<
"String that should be vnum of shop doesn't end with a '~'.  String read was\n"
"'" << strn << "'.  Aborting..\n";

    exit(1);
  }

  deleteChar(strn, i);
  deleteChar(strn, 0);

  if (!strnumer(strn))
  {
    cout <<
"Error in string that should be shop vnum - after removing first and last\n" <<
"characters ('#' at start and '~' at end), string is '" << strn << "'.\n\n" <<
"Aborting..\n";

    exit(1);
  }

  shopNumb = atoi(strn);
  mob = findMob(shopNumb);

  if (!mob)
  {
    cout <<
"Couldn't find mob #" << shopNumb << " specified in .SHP file - aborting..\n";

    exit(1);
  }

  if (mob->shopPtr)
  {
    cout <<
"Error - mob #" << shopNumb << " specified in .SHP file already has a shop - aborting..\n";

    exit(1);
  }

  mob->shopPtr = shopNode;

 // Next string should be 'N' for new format

  if (!fgets(strn, 256, shopFile))
  {
    cout << "hit EOF reading line after shop number for shop #" << shopNumb << "..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);

  if (strcmp(strn, "N"))
  {
    cout <<
"error: string after shop number (for shop #" << shopNumb << ") that should be 'N'\n" <<
"       is '" << strn << "'.  aborting..\n";

    exit(1);
  }

 // read item list

  i = 0;
  while (TRUE)
  {
    if (!fgets(strn, 256, shopFile))
    {
      cout <<
"error: hit EOF while reading produced item list for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);

    if (numbArgs(strn) != 1)
    {
      cout <<
"error: line in produced item list for shop #" << shopNumb << " has more/less than one\n" <<
"       field specified.  string read was '" << strn << "'.  aborting..\n";

      exit(1);
    }

    if (!strnumer(strn))
    {
      cout <<
"error: line in item produced list for shop #" << shopNumb << " not numeric - string\n" <<
"       read was '" << strn << "'.  aborting..\n";

      exit(1);
    }

    if (atoi(strn) == 0)
    {
      break;  // end of list (array already set to 0 above)
    }

    if (i < MAX_NUMBSHOPITEMS) shopNode->producedItemList[i] = atoi(strn);
    i++;
  }

  if (i >= MAX_NUMBSHOPITEMS)
  {
    cout <<
"warning: produced object list for shop #" << shopNumb << " more than " << (long)MAX_NUMBSHOPITEMS << "\n" <<
"         objects (" << (long)i << " objects read total) - list truncated.\n";

    madeChanges = TRUE;

    getkey();
  }

 // read buy multiplier (0.0 - 1.0)

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading buy multiplier for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 1)
  {
    cout <<
"error: buy multiplier for shop #" << shopNumb << " has more/less than one\n" <<
"       field specified.  string read was '" << strn << "'.  aborting..\n";

    exit(1);
  }

  shopNode->buyMult = atof(strn);
  if ((shopNode->buyMult > 1.0) || (shopNode->buyMult <= 0.0))
  {
    cout <<
"error: buy multiplier for shop #" << shopNumb << " is an invalid value\n" <<
"       (less than or equal to 0.0 or greater than 1.0).  value read\n" <<
"       was " << shopNode->buyMult << ".  aborting..\n";

    exit(1);
  }

 // read sell multiplier (>1.0)

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading sell multiplier for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 1)
  {
    cout <<
"error: sell multiplier for shop #" << shopNumb << " has more/less than one\n" <<
"       field specified.  string read was '" << strn << "'.  aborting..\n";

    exit(1);
  }

  shopNode->sellMult = atof(strn);
  if (shopNode->sellMult < 1.0)
  {
    cout <<
"error: sell multiplier for shop #" << shopNumb << " is an invalid value\n" <<
"       (less than 1.0).  value read was " << shopNode->sellMult << ".\n" <<
"       aborting..\n";

    exit(1);
  }

 // read item types bought list

  i = 0;
  while (TRUE)
  {
    if (!fgets(strn, 256, shopFile))
    {
      cout <<
"error: hit EOF while reading item type list for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);

    if (numbArgs(strn) != 1)
    {
      cout <<
"error: line in item type list for shop #" << shopNumb << " has more/less than one\n" <<
"       field specified.  string read was '" << strn << "'.  aborting..\n";

      exit(1);
    }

    if (!strnumer(strn))
    {
      cout <<
"error: line in item type list for shop #" << shopNumb << " not numeric - string\n" <<
"       read was '" << strn << "'.  aborting..\n";

      exit(1);
    }

    if (atoi(strn) == 0)
    {
      break;  // end of list (array already set to 0 above)
    }

    if (i < MAX_NUMBSHOPITEMTYPES) shopNode->tradedItemList[i] = atoi(strn);
    i++;
  }

  if (i >= MAX_NUMBSHOPITEMTYPES)
  {
    cout <<
"warning: object type list for shop #" << shopNumb << " more than " << (long)MAX_NUMBSHOPITEMTYPES << "\n" <<
"         objects (" << (long)i << " items read total) - list truncated.\n";

    madeChanges = TRUE;

    getkey();
  }

 // read "not selling item" message

  if (!fgets(shopNode->notSellingItem, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'not selling item' message for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(shopNode->notSellingItem);
  remTrailingSpaces(shopNode->notSellingItem);

 // check string for tilde and delete it

  if (shopNode->notSellingItem[strlen(shopNode->notSellingItem) - 1] != '~')
  {
    cout <<
"error: 'not selling item' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->notSellingItem << ".  aborting..\n";

    exit(1);
  }

  shopNode->notSellingItem[strlen(shopNode->notSellingItem) - 1] = '\0';

 // read "player does not have item trying to sell" message

  if (!fgets(shopNode->playerNoItem, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'player doesn't have item' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->playerNoItem);
  remTrailingSpaces(shopNode->playerNoItem);

 // check string for tilde and delete it

  if (shopNode->playerNoItem[strlen(shopNode->playerNoItem) - 1] != '~')
  {
    cout <<
"error: 'player doesn't have item' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->playerNoItem << ".  aborting..\n";

    exit(1);
  }

  shopNode->playerNoItem[strlen(shopNode->playerNoItem) - 1] = '\0';

 // read "shop doesn't trade in item trying to sell" message

  if (!fgets(shopNode->shopNoTradeItem, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'shop doesn't trade in item type' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->shopNoTradeItem);
  remTrailingSpaces(shopNode->shopNoTradeItem);

 // check string for tilde and delete it

  if (shopNode->shopNoTradeItem[strlen(shopNode->shopNoTradeItem) - 1] != '~')
  {
    cout <<
"error: 'shop doesn't trade in item' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->shopNoTradeItem << ".  aborting..\n";

    exit(1);
  }

  shopNode->shopNoTradeItem[strlen(shopNode->shopNoTradeItem) - 1] = '\0';

 // read "shop doesn't have the money" message

  if (!fgets(shopNode->shopNoMoney, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'shop has no money' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->shopNoMoney);
  remTrailingSpaces(shopNode->shopNoMoney);

 // check string for tilde and delete it

  if (shopNode->shopNoMoney[strlen(shopNode->shopNoMoney) - 1] != '~')
  {
    cout <<
"error: 'shop doesn't have the money' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->shopNoMoney << ".  aborting..\n";

    exit(1);
  }

  shopNode->shopNoMoney[strlen(shopNode->shopNoMoney) - 1] = '\0';

 // read "player doesn't have the money" message

  if (!fgets(shopNode->playerNoMoney, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'player has no money' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->playerNoMoney);
  remTrailingSpaces(shopNode->playerNoMoney);

 // check string for tilde and delete it

  if (shopNode->playerNoMoney[strlen(shopNode->playerNoMoney) - 1] != '~')
  {
    cout <<
"error: 'player doesn't have the money' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->playerNoMoney << ".  aborting..\n";

    exit(1);
  }

  shopNode->playerNoMoney[strlen(shopNode->playerNoMoney) - 1] = '\0';

 // read "sell message" message (shop sells to player)

  if (!fgets(shopNode->sellMessage, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'shop sells to player' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->sellMessage);
  remTrailingSpaces(shopNode->sellMessage);

 // check string for tilde and delete it

  if (shopNode->sellMessage[strlen(shopNode->sellMessage) - 1] != '~')
  {
    cout <<
"error: 'shop sells to player' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->sellMessage << ".  aborting..\n";

    exit(1);
  }

  shopNode->sellMessage[strlen(shopNode->sellMessage) - 1] = '\0';

 // read "buy message" message (shop buys from player)

  if (!fgets(shopNode->buyMessage, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading 'shop buys from player' message for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(shopNode->buyMessage);
  remTrailingSpaces(shopNode->buyMessage);

 // check string for tilde and delete it

  if (shopNode->buyMessage[strlen(shopNode->buyMessage) - 1] != '~')
  {
    cout <<
"error: 'shop buys from player' string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->buyMessage << ".  aborting..\n";

    exit(1);
  }

  shopNode->buyMessage[strlen(shopNode->buyMessage) - 1] = '\0';

 // next two lines should simply be zeroes

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading 0s after first block of shop messages after\n" <<
"       shop #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(strn);

  if (strcmp(strn, "0"))
  {
    cout <<
"error: first line after first block of shop messages should be '0', but instead\n" <<
"       is '" << strn << "'  (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading 0s after first block of shop messages for shop\n" <<
"       #" << shopNumb << ".  aborting..\n";

    exit(1);
  }

  nolf(strn);

  if (strcmp(strn, "0"))
  {
    cout <<
"error: second line after first block of shop messages should be '0', but instead\n" <<
"       is '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

 // read shopkeeper's mob numb - same as shop numb, hopefully

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's mob number (shop #" << shopNumb << ".)\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);

  if (atoi(strn) != shopNumb)
  {
    cout <<
"error: shop's mob number should be same as the shop number, but string read\n" <<
"       was '" << strn << "'.  (shop number is " << shopNumb << ".)\n" <<
"       aborting..\n";

    exit(1);
  }

 // yet another 0

  if (!fgets(strn, 256, shopFile))
  {
    cout << "error: hit EOF while reading 0 line after shop mob numb.  aborting..\n";

    exit(1);
  }

  nolf(strn);

  if (strcmp(strn, "0"))
  {
    cout <<
"error: line after shop's mob number should be '0', but instead was\n" <<
"       '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

 // shopkeeper's room numb - for right now, just read it and trust the user..

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's room numb (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout << "\n" <<
"error: line that should be shop's room numb isn't numeric.  string read was\n" <<
"       '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->roomNumb = atoi(strn);

  if ((shopNode->roomNumb) && (!findRoom(shopNode->roomNumb)))
  {
    cout << "\n" <<
"error: shop's room numb line for shop #" << shopNumb << " specifies a room that\n" <<
"       doesn't exist in this .WLD (room #" << shopNode->roomNumb << ").  aborting..\n";

    exit(1);
  }

 // read first opening and closing times

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's opening time (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's opening time isn't numeric.  string read was\n" <<
"       '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->firstOpen = atoi(strn);

 // read closing time

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's closing time (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's closing time isn't numeric.  string read was\n" <<
"       '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->firstClose = atoi(strn);

 // read second opening and closing times

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's second opening time (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's second opening time isn't numeric.  string\n" <<
"       read was '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->secondOpen = atoi(strn);

 // read second closing time

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's second closing time (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's second closing time isn't numeric.  string\n" <<
"       read was '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->secondClose = atoi(strn);

 // read shop roaming value - Y/N

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's roam value (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  shopNode->roaming = getBoolYesNo(strn);
  if (shopNode->roaming == -1)
  {
    cout <<
"error: roaming value for shop #" << shopNumb << " is not 'Y' or 'N' - string read was\n" <<
"       '" << strn << "'.  aborting..\n";

    exit(1);
  }

  if (shopNode->roaming)
  {
    if (shopNode->roomNumb)
    {
      cout << "\n" <<
"error: shop #" << shopNumb << " is a roaming shop, but has a room number specified\n" <<
"       (#" << shopNode->roomNumb << ").  room number should be 0.  aborting..\n";

      exit(1);
    }
  }
  else  // not roaming
  {
    if (!shopNode->roomNumb)
    {
      cout << "\n" <<
"error: shop #" << shopNumb << " is not a roaming shop, but has no room number specified\n" <<
"       (was specified as 0).  aborting..\n";

      exit(1);
    }
  }

 // read shop no magic value - Y/N

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's no magic value (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  shopNode->noMagic = getBoolYesNo(strn);
  if (shopNode->noMagic == -1)
  {
    cout <<
"error: no magic value for shop #" << shopNumb << " is not 'Y' or 'N' - string read was\n" <<
"       '" << strn << "'.  aborting..\n";

    exit(1);
  }

 // read shop killable value - Y/N

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's killable value (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  shopNode->killable = getBoolYesNo(strn);
  if (shopNode->killable == -1)
  {
    cout <<
"error: killable value for shop #" << shopNumb << " is not 'Y' or 'N' - string read was\n" <<
"       '" << strn << "'.  aborting..\n";

    exit(1);
  }

 // read opening message

  if (!fgets(shopNode->openMessage, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading opening message for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(shopNode->openMessage);
  remTrailingSpaces(shopNode->openMessage);

 // check string for tilde and delete it

  if (shopNode->openMessage[strlen(shopNode->openMessage) - 1] != '~')
  {
    cout <<
"error: opening message string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->openMessage << ".  aborting..\n";

    exit(1);
  }

  shopNode->openMessage[strlen(shopNode->openMessage) - 1] = '\0';

 // read closing message

  if (!fgets(shopNode->closeMessage, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading closing message for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(shopNode->closeMessage);
  remTrailingSpaces(shopNode->closeMessage);

 // check string for tilde and delete it

  if (shopNode->closeMessage[strlen(shopNode->closeMessage) - 1] != '~')
  {
    cout <<
"error: closing message string for shop #" << shopNumb << " doesn't end in\n"
"       a tilde.  string read was '" << shopNode->closeMessage << ".  aborting..\n";

    exit(1);
  }

  shopNode->closeMessage[strlen(shopNode->closeMessage) - 1] = '\0';

 // read hometown flag

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's hometown flag (shop #" << shopNumb << ").\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's hometown flag isn't numeric.  string\n" <<
"       read was '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->hometownFlag = atoi(strn);

 // read social action types (oooh, neat-sounding, isn't it?  bleh)

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's social action flag (shop #" << shopNumb << ").\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's social action flag isn't numeric.  string\n" <<
"       read was '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->socialActionTypes = atoi(strn);

 // read shop racism value - Y/N

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's racism value (shop #" << shopNumb << ").\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  shopNode->racist = getBoolYesNo(strn);
  if (shopNode->racist == -1)
  {
    cout <<
"error: racist value for shop #" << shopNumb << " is not 'Y' or 'N' - string read was\n" <<
"       '" << strn << "'.  aborting..\n";

    exit(1);
  }

 // read shopkeeper's race

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF while reading shop's race flag (shop #" << shopNumb << ").\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strnumer(strn))
  {
    cout <<
"error: line that should be shop's race flag isn't numeric.  string\n" <<
"       read was '" << strn << "' (shop #" << shopNumb << ").  aborting..\n";

    exit(1);
  }

  shopNode->shopkeeperRace = atoi(strn);

 // read racism message

  if (!fgets(shopNode->racistMessage, MAX_SHOPSTRING_LEN, shopFile))
  {
    cout <<
"error: hit EOF while reading racist message for shop #" << shopNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(shopNode->racistMessage);
  remTrailingSpaces(shopNode->racistMessage);

 // check string for tilde and delete it

  if (shopNode->racistMessage[strlen(shopNode->racistMessage) - 1] != '~')
  {
    cout <<
"error: racist message for shop #" << shopNumb << " doesn't end in\n" <<
"       a tilde.  string read was '" << shopNode->racistMessage << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  shopNode->racistMessage[strlen(shopNode->racistMessage) - 1] = '\0';

 // finally, last line should be an 'X'

  if (!fgets(strn, 256, shopFile))
  {
    cout <<
"error: hit EOF on last line in shop #" << shopNumb << " - should be an 'X'.\n"
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strcmp(strn, "X"))
  {
    cout <<
"error: last line for shop #" << shopNumb << " should be an 'X', but\n"
"       '" << strn << "' was read instead.  aborting..\n";

    exit(1);
  }

  return shopNode;  // not too useful, but hey
}


//
// readShopFile : Reads shops from the shop file - returns TRUE if file
//                was found, FALSE otherwise
//
//   *filename : pointer to filename - if NULL, checks the MAINZONENAME var
//

char readShopFile(const char *filename /*= NULL*/)
{
  FILE *shopFile;
  char shopFilename[81] = "\0";


 // assemble the filename of the shop file

  if (readFromSubdirs) strcpy(shopFilename, "shp/");

  if (filename) strcat(shopFilename, filename);
  else strcat(shopFilename, getMainZoneNameStrn());

  strcat(shopFilename, ".shp");

 // open the shop file for reading

  if ((shopFile = fopen(shopFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << shopFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << shopFilename << "...\n";

 // this while loop reads shop by shop, one shop per iteration

  while (!feof(shopFile))
  {
    if (!readShopFromFile(shopFile)) break;  // eof
  }

  fclose(shopFile);
  return TRUE;
}
