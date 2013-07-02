//
//  File: delmobhu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for deleting mobHeres
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "../room/room.h"

extern char madeChanges;

//
// deleteMobHereUser : Deletes a mobHere based on info specified by user
//                     in args.
//
//   args : user-entered string
//   room : room to delete mobHere from
//

void deleteMobHereUser(const char *args, dikuRoom *room)
{
  char outStrn[256];
  mobHere *mobHere = room->mobHead;
  long numb;


  if (!mobHere)
  {
    _outtext("\nThere are no mobs in this room.\n\n");
    return;
  }

  if (!strlen(args) || !strnumer(args))
  {
    displayColorString("&n\nError in input - specify vnum of mob in this room.\n\n");
    return;
  }

  numb = atoi(args);

  while (mobHere && (mobHere->mobNumb != numb))
  {
    mobHere = mobHere->Next;
  }

  if (!mobHere)
  {
    sprintf(outStrn, "\nMob #%d not found in this room.\n\n", numb);
    _outtext(outStrn);

    return;
  }

  deleteMobHereinList(&room->mobHead, mobHere, TRUE);

  sprintf(outStrn, "\nMob #%d deleted from this room.\n\n", numb);
  _outtext(outStrn);

  deleteMasterKeywordList(room->masterListHead);
  room->masterListHead = createMasterKeywordList(room);

  deleteEditableList(room->editableListHead);
  room->editableListHead = createEditableList(room);


  madeChanges = TRUE;
}
