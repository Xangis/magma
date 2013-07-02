//
//  File: crtobjtu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for creating object types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "object.h"

extern dikuObject *objectHead;

//
// createObjectTypeUser : user interface stuff for creating an object type -
//                        returns vnum of new obj on success, -1 on error
//
//   args : user input
//

long createObjectTypeUser(const char *args)
{
  char strn[256];
  dikuObject *lastNode = objectHead, *newNode;
  long numb = -1;


  if (strlen(args))
  {
    if (!strnumer(args))
    {
      _outtext("\nError in vnum argument - non-numerics in input.\n\n");
      return -1;
    }

    numb = atoi(args);
    if (findObj(numb))
    {
      sprintf(strn,
"\nCannot create an object type with the vnum %d - this vnum is already taken.\n\n",
              numb);
      _outtext(strn);

      return -1;
    }
  }

 // get to end of list..

  if (lastNode)
  {
    while (lastNode->Next)
    {
      lastNode = lastNode->Next;
    }
  }

  newNode = createObjectType(TRUE, numb);
  if (!newNode)
  {
    _outtext("\n"
"Couldn't create an object type with that vnum - most likely reason is that\n"
"you've hit the vnum limit.  To increase, load dikuEdit with the -ll=<limit>\n"
"parameter.\n\n");

    return -1;
  }

  if (!objectHead) objectHead = newNode;
  else lastNode->Next = newNode;

  resetEntityPointersByNumb(TRUE, FALSE);

  sprintf(strn, "\nObject type #%u created.\n\n", newNode->objNumber);
  _outtext(strn);

  return newNode->objNumber;
}
