//
//  File: editobju.cpp   originally part of dikuEdit
//
//  Usage: user-interface side of editing objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "object.h"

//
// editObjectTypeStrn : Edits an object type based on user input in *strn.
//
//   *strn : user input
//     pos : pos where id number starts
//

void editObjectTypeStrn(const char *args)
{
  ulong numb;
  dikuObject *obj;
  char strn[256];


  if (!strlen(args))
  {
    editObjectTypePrompt();
    return;
  }

  numb = atoi(args);
  obj = findObj(numb);

  if (!obj)
  {
    sprintf(strn, "\nCorresponding object type (#%d) not found.\n\n", numb);
    _outtext(strn);

    return;
  }
  else editObjType(obj, TRUE);
}


//
// editObjectTypePrompt : Prompts a user to type in a object type id to edit.
//

void editObjectTypePrompt(void)
{
  ulong i;
  char numbStrn[256] = "\0", strn[256];
  dikuObject *obj;


  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+cEnter object number to edit (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayObjectTypeList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);
  i = atoi(numbStrn);

  obj = findObj(i);

  if (!obj)
  {
    sprintf(strn, "\nCorresponding object type (#%d) not found.\n\n", i);
    _outtext(strn);

    return;
  }
  else editObjType(obj, TRUE);
}
