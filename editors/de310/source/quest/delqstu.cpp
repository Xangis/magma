//
//  File: delqstu.cpp    originally part of dikuEdit
//
//  Usage: user-interface functions for deleting quest info from mobs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../fh.h"

#include "quest.h"

extern char madeChanges;

//
// deleteQuestUser : Deletes a quest based on info specified by user
//                   in strn.
//
//   *args : user-entered string
//

void deleteQuestUser(const char *args)
{
  dikuMob *mob;
  long numb;
  char strn[256];


  if (!strlen(args))
  {
    deleteQuestUserPrompt();
    return;
  }

  if (!strnumer(args))
  {
    displayColorString(
"&n\nError in input - specify vnum of mob to delete quest from.\n\n");
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

  if (!mob->questPtr)
  {
    sprintf(strn, "\nMob type #%d has no quest info.\n\n", numb);
    _outtext(strn);
    return;
  }

  deleteQuest(mob->questPtr);
  mob->questPtr = NULL;

  sprintf(strn, "\nMob type #%d's quest info deleted.\n\n", numb);
  _outtext(strn);

  madeChanges = TRUE;
}


//
// deleteQuestUserPrompt
//

void deleteQuestUserPrompt(void)
{
  char numbStrn[32] = "\0";


  while (TRUE)
  {
    displayColorString("\n"
"&+cEnter number of mob type to delete quest info from (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayQuestList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);

  if (strlen(numbStrn))
  {
    deleteQuestUser(numbStrn);
  }
  else _outtext("\n\n");
}
