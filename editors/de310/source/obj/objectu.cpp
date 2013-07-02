//
//  File: objectu.cpp    originally part of dikuEdit
//
//  Usage: miscellaneous functions invoked by the user, object-related
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

extern dikuObject *objectHead;
extern ulong numbObjTypes, numbLookupEntries;

//
// renumberObjectsUser
//

void renumberObjectsUser(const char *args)
{
  char strn[256];
  long usernumb, oldNumb, tempstart;

  if (strlen(args) == 0)
  {
    if (objectHead)
    {
      renumberObjs(FALSE, 0);

      sprintf(strn, "\nObjects have been renumbered (starting at %u).\n\n",
              objectHead->objNumber);
      _outtext(strn);
    }
    else _outtext("\nThere are no objects to renumber.\n\n");

    return;
  }

  if (!strnumer(args))
  {
    _outtext(
"\nThe 'renumberobject' command's first argument must be a positive number.\n\n");
    return;
  }

  usernumb = atoi(args);

  if (objectHead)
  {
    tempstart = getHighestObjNumber() + 1;

    if (((usernumb + numbObjTypes) >= numbLookupEntries) ||
        ((tempstart + numbObjTypes) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber objects due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = objectHead->objNumber;
      objectHead->objNumber = tempstart;
      renumberObjs(TRUE, oldNumb);

      oldNumb = objectHead->objNumber;
      objectHead->objNumber = usernumb;
      renumberObjs(TRUE, oldNumb);
    }
  }
  else
  {
    _outtext("\nThere are no objects to renumber.\n\n");
    return;
  }

  sprintf(strn,
"\nObjects renumbered starting at %u.\n\n", usernumb);

  _outtext(strn);
}
