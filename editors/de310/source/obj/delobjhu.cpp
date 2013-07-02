//
//  File: delobjhu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for deleting objectHeres
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "objhere.h"

extern char madeChanges;

//
// deleteObjectHereUser : Deletes an objectHere based on info specified by user
//                        in strn.
//
//   *strn : user-entered string
//   *room : room to delete objectHere from
//

void deleteObjectHereUser(const char *args, dikuRoom *room)
{
  char outStrn[256];
  long numb;
  objectHere *objHere = room->objectHead;


  if (!objHere)
  {
    _outtext("\nThere are no objects in this room.\n\n");
    return;
  }

  if (!strlen(args) || !strnumer(args))
  {
    displayColorString("&n\nError in input - specify vnum of obj in this room.\n\n");
    return;
  }

  numb = atoi(args);

  while (objHere && (objHere->objectNumb != numb))
  {
    objHere = objHere->Next;
  }

  if (!objHere)
  {
    sprintf(outStrn, "\nObject #%d not found in this room.\n\n", numb);
    _outtext(outStrn);

    return;
  }

  deleteObjHereinList(&room->objectHead, objHere, TRUE);

  sprintf(outStrn, "\nObject #%d deleted from this room.\n\n", numb);
  _outtext(outStrn);

  deleteMasterKeywordList(room->masterListHead);
  room->masterListHead = createMasterKeywordList(room);

  deleteEditableList(room->editableListHead);
  room->editableListHead = createEditableList(room);


  madeChanges = TRUE;
}
