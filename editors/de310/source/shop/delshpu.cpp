//
//  File: delshpu.cpp    originally part of dikuEdit
//
//  Usage: user-interface functions for deleting shops
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "../mob/mob.h"

extern char madeChanges;

//
// deleteShopUser : Deletes a shop based on info specified by user in strn
//
//   *strn : user-entered string
//

void deleteShopUser(const char *args)
{
  long numb;
  dikuMob *mob;
  char strn[256];


  if (!strlen(args))
  {
    deleteShopUserPrompt();
    return;
  }

  if (!strnumer(args))
  {
    displayColorString(
"&n\nError in input - specify vnum of mob to delete shop from.\n\n");

    return;
  }

  numb = atoi(args);
  mob = findMob(numb);

  if (!mob)
  {
    sprintf(strn, "\nMob type #%d not found.\n\n", numb);
    _outtext(strn);

    return;
  }

  if (!mob->shopPtr)
  {
    sprintf(strn, "\nMob type #%d has no shop info.\n\n", numb);
    _outtext(strn);

    return;
  }

  deleteShop(mob->shopPtr);
  mob->shopPtr = NULL;

  sprintf(strn, "\nMob type #%d's shop info deleted.\n\n", numb);
  _outtext(strn);

  madeChanges = TRUE;
}


//
// deleteShopUserPrompt
//

void deleteShopUserPrompt(void)
{
  char numbStrn[32] = "\0";


  while (TRUE)
  {
    displayColorString("\n"
"&+cEnter number of mob type to delete shop info from (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayShopList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);

  if (strlen(numbStrn))
  {
    deleteShopUser(numbStrn);
  }
  else _outtext("\n\n");
}
