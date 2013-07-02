//
//  File: crtmobtu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for creating mob types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

extern dikuMob *mobHead;

//
// createMobTypeUser : Userish crap for creating an mob type - returns
//                     vnum if created, -1 if not
//

long createMobTypeUser(const char *args)
{
  char strn[256];
  dikuMob *lastNode = mobHead, *newNode;
  long numb = -1;


  if (strlen(args))
  {
    if (!strnumer(args))
    {
      _outtext("\nError in vnum argument - non-numerics in input.\n\n");
      return -1;
    }

    numb = atoi(args);
    if (findMob(numb))
    {
      sprintf(strn,
"\nCannot create a mob type with the vnum %d - this vnum is already taken.\n\n",
              numb);
      _outtext(strn);

      return -1;
    }
  }

  if (lastNode)
  {
    while (lastNode->Next)
    {
      lastNode = lastNode->Next;
    }
  }

  newNode = createMobType(TRUE, numb);

  if (!newNode)
  {
    _outtext("\n"
"Couldn't create a mob type with that vnum - most likely reason is that you've\n"
"hit the vnum limit.  To increase, load dikuEdit with the -ll=<limit> parameter.\n\n");

    return -1;
  }


  if (!mobHead) mobHead = newNode;
  else lastNode->Next = newNode;

  resetEntityPointersByNumb(FALSE, TRUE);

  sprintf(strn, "\nMob type #%u created.\n\n", newNode->mobNumber);
  _outtext(strn);

  return newNode->mobNumber;
}
