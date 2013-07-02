//
//  File: mobu.cpp       originally part of dikuEdit
//
//  Usage: function(s) invoked by user to alter mobs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../fh.h"

#include "mob.h"

extern dikuMob *mobHead;
extern ulong numbMobTypes, numbLookupEntries;

//
// renumberMobsUser
//

void renumberMobsUser(const char *args)
{
  char strn[256];
  long usernumb, oldNumb, tempstart;

  if (strlen(args) == 0)
  {
    if (mobHead)
    {
      renumberMobs(FALSE, 0);

      sprintf(strn, "\nMobs have been renumbered (starting at %u).\n\n",
              mobHead->mobNumber);
      _outtext(strn);
    }
    else _outtext("\nThere are no mobs to renumber.\n\n");

    return;
  }

  if (!strnumer(args))
  {
    _outtext(
"\nThe 'renumbermob' command's first argument must be a positive number.\n\n");
    return;
  }

  usernumb = atoi(args);

  if (mobHead)
  {
    tempstart = getHighestMobNumber() + 1;

    if (((usernumb + numbMobTypes) >= numbLookupEntries) ||
        ((tempstart + numbMobTypes) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber mobs due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = mobHead->mobNumber;
      mobHead->mobNumber = tempstart;
      renumberMobs(TRUE, oldNumb);

      oldNumb = mobHead->mobNumber;
      mobHead->mobNumber = usernumb;
      renumberMobs(TRUE, oldNumb);
    }
  }
  else
  {
    _outtext("\nThere are no mobs to renumber.\n\n");
    return;
  }

  sprintf(strn,
"\nMobs renumbered starting at %u.\n\n", usernumb);

  _outtext(strn);
}
