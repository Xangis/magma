//
//  File: delmobtu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for deleting mob types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "mob.h"

extern dikuMob *mobHead;
extern char madeChanges;

//
// checkDeleteMobsofType : returns TRUE if user doesn't abort - deletes
//                         specified mob type
//

char checkDeleteMobsofType(const dikuMob *mobType)
{
  usint ch;


  if (checkForMobHeresofType(mobType))
  {
    displayColorString(
"\n&+cMobs exist of this type and will have to be deleted - continue (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");

      return FALSE;
    }
    else
    {
      displayColorString("Yes&n");

      return TRUE;
    }
  }
  else return TRUE;
}


//
// deleteMobTypeUser : Deletes an mobType based on info specified by user
//                     in args.
//
//   args : user-entered string
//

void deleteMobTypeUser(const char *args)
{
  char outStrn[256];
  dikuMob *mob = mobHead;
  long numb;


  if (!strlen(args))
  {
    deleteMobTypeUserPrompt();
    return;
  }

  if (!strnumer(args))
  {
    displayColorString("&n\nError in input - specify vnum of mob type.\n\n");
    return;
  }

  numb = atoi(args);
  mob = findMob(numb);

  if (!mob)
  {
    sprintf(outStrn, "\nMob type #%d not found.\n\n", numb);
    _outtext(outStrn);

    return;
  }

  if (!checkDeleteMobsofType(mob)) return;

  deleteAllMobHeresofType(mob, TRUE);
  deleteMobsinInv(mob);

  deleteMobTypeinList(&mobHead, mob, TRUE);

  sprintf(outStrn, "\nMob type #%d deleted.\n\n", numb);
  _outtext(outStrn);

  madeChanges = TRUE;
}


//
// deleteMobTypeUserPrompt
//

void deleteMobTypeUserPrompt(void)
{
  char numbStrn[32] = "\0";


  while (TRUE)
  {
    displayColorString("\n&+cEnter number of mob type to delete (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayMobTypeList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);

  if (strlen(numbStrn)) deleteMobTypeUser(numbStrn);
  else _outtext("\n\n");
}
