//
//  File: writeshp.cpp   originally part of dikuEdit
//
//  Usage: functions for writing shop info to the .SHP file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <iostream>

#include "../types.h"
#include "../fh.h"

#include "shop.h"

using namespace std;

extern dikuRoom *roomHead;
extern dikuMob *mobHead;
extern char readFromSubdirs;

//
// writeShoptoFile
//

void writeShoptoFile(FILE *shopFile, shop *shopRec, const ulong mobNumb)
{
  ulong i;
  char strn[256];

  if (!shopRec || !shopFile)
  {
    _outtext("\nerror in writeShoptoFile() - NULL pointer passed\n\n");
    return;
  }

 // write shop numb (same as mob numb) and N line

  fprintf(shopFile, "#%u~\nN\n", mobNumb);

 // write list of items produced by shop

  for (i = 0; shopRec->producedItemList[i]; i++)
  {
    fprintf(shopFile, "%u\n", shopRec->producedItemList[i]);
  }

  fputs("0\n", shopFile);

 // write buy/sell multipliers

  fprintf(shopFile, "%.2f\n%.2f\n", shopRec->buyMult, shopRec->sellMult);

 // write list of item types bought by shop

  for (i = 0; shopRec->tradedItemList[i]; i++)
  {
    fprintf(shopFile, "%u\n", shopRec->tradedItemList[i]);
  }

  fputs("0\n", shopFile);

 // write shop messages and two zeroes

  fprintf(shopFile, "%s~\n" "%s~\n" "%s~\n" "%s~\n" "%s~\n" "%s~\n" "%s~\n"
                    "0\n" "0\n",
          shopRec->notSellingItem,  shopRec->playerNoItem,
          shopRec->shopNoTradeItem, shopRec->shopNoMoney,
          shopRec->playerNoMoney,   shopRec->sellMessage, shopRec->buyMessage);

 // write shop's mob numb and yet another zero

  fprintf(shopFile, "%u\n" "0\n", mobNumb);

 // write shop's shop number (room number)

  if (shopRec->roaming) fputs("0\n", shopFile);
  else
  if (!findMobHere(mobNumb, roomHead, &i, FALSE))  // shouldn't happen, but hey
  {
    sprintf(strn, "\n"
"Warning: Mob #%u has a shop, but is not loaded.  Writing shop room number\n"
"         as #0 and setting to roaming, but this may cause problems..\n", mobNumb);

    _outtext(strn);
    getkey();
    _outtext("\n");

    shopRec->roaming = TRUE;

    fputs("0\n", shopFile);
  }
  else fprintf(shopFile, "%u\n", i);

 // write shop's closing/opening times

  fprintf(shopFile, "%d\n" "%d\n" "%d\n" "%d\n",
          shopRec->firstOpen,  shopRec->firstClose,
          shopRec->secondOpen, shopRec->secondClose);

 // write shop roam, no magic, killable flags

  fprintf(shopFile, "%s\n" "%s\n" "%s\n",
          getYesNoBool(shopRec->roaming),
          getYesNoBool(shopRec->noMagic),
          getYesNoBool(shopRec->killable));

 // write shop opening and closing messages

  fprintf(shopFile, "%s~\n" "%s~\n",
          shopRec->openMessage, shopRec->closeMessage);

 // write shop hometown and social action flags

  fprintf(shopFile, "%d\n" "%d\n",
          shopRec->hometownFlag, shopRec->socialActionTypes);

 // write shop racist value, race, and racist message, terminate with X

  fprintf(shopFile, "%s\n" "%d\n" "%s~\n" "X\n",
          getYesNoBool(shopRec->racist),
          shopRec->shopkeeperRace,
          shopRec->racistMessage);
}


//
// writeShopFile : Write the shop file
//

void writeShopFile(const char *filename /*= NULL*/)
{
  FILE *shopFile;
  char shopFilename[81] = "\0";
  char strn[256];
  dikuMob *mob = mobHead;

  ulong highest = getHighestMobNumber(), numb = getLowestMobNumber(), i;


 // assemble the filename of the shop file

  if (readFromSubdirs) strcpy(shopFilename, "shp/");

  if (filename) strcat(shopFilename, filename);
  else strcat(shopFilename, getMainZoneNameStrn());

  strcat(shopFilename, ".shp");

 // open the world file for writing

  if ((shopFile = fopen(shopFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << shopFilename << " for writing - aborting\n";

    return;
  }

  i = getNumbShopMobs();

  sprintf(strn, "Writing %s - %u shop%s\n", shopFilename, i, plural(i));

  _outtext(strn);

  while (numb <= highest)
  {
    mob = findMob(numb);

    if (mob && mob->shopPtr)
      writeShoptoFile(shopFile, mob->shopPtr, mob->mobNumber);

    numb++;
  }

  fclose(shopFile);
}
