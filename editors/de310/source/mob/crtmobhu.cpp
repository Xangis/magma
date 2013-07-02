//
//  File: crtmobhu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for creating mobHeres
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

extern dikuRoom *currentRoom;

//
// createMobHereUser : Userish crap for creating an mobHere in currentRoom
//

void createMobHereUser(void)
{
  char strn[256] = "\0";
  ulong numb;
  dikuMob *mob;


  while (TRUE)
  {
    displayColorString(
      "\n&+cEnter number of mob to create here (&+C? for list&n&+c): ");

    getStrn(strn, 6, 1, 7, '°', strn, FALSE, FALSE);

    if (!strcmp(strn, "?")) displayMobTypeList();
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
  mob = findMob(numb);

  _setbkcolor(0);

  if (!mob && getVnumCheckVal())
  {
    displayColorString("&nCouldn't find that mob.\n\n");

    return;
  }

  if (!createMobHereinRoom(currentRoom, mob, numb)) return;

  sprintf(strn, "\nMob of type #%u created in current room.\n\n", numb);

  _outtext(strn);
}


//
// createMobHereStrn : Creates an mob type with id number specified in
//                     strn
//
//   *strn : user-entered string
//     pos : position where id number starts in strn
//

void createMobHereStrn(const char *args)
{
  ulong numb;
  dikuMob *mob;
  char strn[256];


  if (!strlen(args))
  {
    createMobHereUser();
    return;
  }

  if (!strnumer(args))
  {
    displayColorString("&n\nError in input.\n\n");
    return;
  }

  numb = atoi(args);
  mob = findMob(numb);

  if (!mob && getVnumCheckVal())
  {
    _outtext("\nCouldn't find that mob.\n\n");

    return;
  }
  else
  {
    if (!createMobHereinRoom(currentRoom, mob, numb)) return;

    sprintf(strn, "\nMob of type #%u created in current room.\n\n", numb);
    _outtext(strn);
  }
}
