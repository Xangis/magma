//
//  File: editqstu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing quests by commandline
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"
#include "../misc/menu.h"

#include "quest.h"

//
// editQuestStrn : Edits a quest based on user input in *strn.
//
//   *strn : user input
//

void editQuestStrn(const char *strn)
{
  ulong numb;
  dikuMob *mob;
  usint ch;
  char outstrn[512];


 // get the number section

  if (!strlen(strn))
  {
    editQuestPrompt();
    return;
  }

  numb = atoi(strn);
  mob = findMob(numb);

  if (!mob)
  {
    sprintf(outstrn, "\nCorresponding mob type (#%d) not found.\n\n", numb);
    _outtext(outstrn);

    return;
  }
  else
  {
    if (!mob->questPtr)
    {
      displayColorString(
"\n&+cThis mob has no quest info - create some (&+Cy/N&n&+c)? ");

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

      mob->questPtr = createQuest();
      if (!mob->questPtr) return;  // error

//      mob->questPtr->mobNumb = mob->mobNumber;
    }

    editQuest(mob->questPtr, mob->mobShortName, mob->mobNumber, mob, TRUE);
  }
}


//
// editQuestPrompt : Prompts a user to type in a quest to edit.
//

void editQuestPrompt(void)
{
  char numbStrn[256] = "\0";


  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+cEdit which mob's quest info (&+C? for list&n&+c): ");

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
    editQuestStrn(numbStrn);
  }
  else _outtext("\n\n");
}
