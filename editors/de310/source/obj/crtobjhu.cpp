//
//  File: crtobjhu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for creating objectHeres
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#include "object.h"
#include "objhere.h"

extern dikuRoom *currentRoom;

//
// createObjectHereUser : Userish crap for creating an objectHere in
//                        currentRoom
//

void createObjectHereUser(void)
{
  char strn[256] = "\0";
  long numb;
  dikuObject *obj;


  while (TRUE)
  {
    displayColorString(
      "\n&+cEnter number of object to create here (&+C? for list&n&+c): ");

    getStrn(strn, 6, 1, 7, '°', strn, FALSE, FALSE);

    if (!strcmp(strn, "?")) displayObjectTypeList();
    else break;
  }

  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (!strlen(strn) || !strnumer(strn))
  {
    displayColorString("&n\nError in input.\n\n");
    return;
  }

  numb = atoi(strn);
  obj = findObj(numb);

  _setbkcolor(0);

  if (!obj && getVnumCheckVal())
  {
    sprintf(strn,
"&n\nObject of type #%d does not exist in this zone.\n\n", numb);
    displayColorString(strn);

    return;
  }

  createObjectHereinRoom(currentRoom, obj, numb);

  sprintf(strn, "\nObject of type #%u created in current room.\n\n", numb);

  _outtext(strn);
}


//
// createObjectHereStrn : Creates an object type with id number specified in
//                        args.
//
//   *args : user-entered string
//     pos : position where id number starts in strn
//

void createObjectHereStrn(const char *args)
{
  dikuObject *obj;
  char strn[256];
  long numb;


  if (!strlen(args))
  {
    createObjectHereUser();
    return;
  }

  if (!strnumer(args))
  {
    displayColorString("&n\nError in input (not numeric).\n\n");
    return;
  }

  numb = atoi(args);
  obj = findObj(numb);

  if (!obj && getVnumCheckVal())
  {
    sprintf(strn,
"&n\nObject of type #%d does not exist in this zone.\n\n", numb);
    displayColorString(strn);

    return;
  }
  else
  {
    createObjectHereinRoom(currentRoom, obj, numb);

    sprintf(strn, "\nObject of type #%u created in current room.\n\n", numb);
    _outtext(strn);
  }
}
