//
//  File: editshpu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shops from commandline
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "shop.h"

//
// editShopStrn : Edits a quest based on user input in *strn.
//
//   *strn : user input
//

void editShopStrn(const char *args)
{
  ulong numb;
  dikuMob *mob;
  char strn[256];
  usint ch;


 // get the number section

  if (!strlen(args))
  {
    editShopPrompt();
    return;
  }

  numb = atoi(args);
  mob = findMob(numb);

  if (!mob)
  {
    sprintf(strn, "\nCorresponding mob type (#%d) not found.\n\n", numb);
    _outtext(strn);

    return;
  }
  else
  {
    if (!mob->shopPtr)
    {
      displayColorString(
"\n&+cThis mob has no shop info - create some (&+Cy/N&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

      if (ch != 'Y')
      {
        displayColorString("No&n\n\n");
        return;
      }
      else displayColorString("Yes&n\n");

      mob->shopPtr = createShop();
      if (!mob->shopPtr) return;  // error
    }

    editShop(mob->shopPtr, mob->mobShortName, mob->mobNumber, mob, TRUE);
  }
}


//
// editShopPrompt : Prompts a user to type in a quest to edit.
//

void editShopPrompt(void)
{
  char numbStrn[256] = "\0";


  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+cEdit which mob's shop info (&+C? for list&n&+c): ");

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
    editShopStrn(numbStrn);
  }
  else _outtext("\n\n");
}
