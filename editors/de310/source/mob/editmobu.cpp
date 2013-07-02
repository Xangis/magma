//
//  File: editmobu.cpp   originally part of dikuEdit
//
//  Usage: calls editMobType() functions based on user input from
//         command-line
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "mob.h"

//
// editMobTypeStrn : Edits a mob type based on user input in strn
//
//   *strn : user input
//

void editMobTypeStrn(const char *args)
{
  dikuMob *mob;
  char strn[256];
  ulong numb;


  if (!strlen(args))
  {
    editMobTypePrompt();
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
  else editMobType(mob, TRUE);
}


//
// editMobTypePrompt : Prompts a user to type in a mob type id to edit.
//

void editMobTypePrompt(void)
{
  ulong i;
  char numbStrn[256] = "\0", strn[256];
  dikuMob *mob;


  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+cEnter mob number to edit (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayMobTypeList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);
  i = atoi(numbStrn);

  mob = findMob(i);

  if (!mob)
  {
    sprintf(strn, "\nCorresponding mob type (#%s) not found.\n\n",
            numbStrn);
    _outtext(strn);

    return;
  }
  else editMobType(mob, TRUE);
}
