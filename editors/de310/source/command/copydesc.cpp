//
//  File: copydesc.cpp   originally part of dikuEdit
//
//  Usage: functions for copying room/mob descriptions
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../types.h"
#include "../fh.h"

#include "../room/room.h"
#include "../mob/mob.h"

extern char madeChanges;

//
// copyDesc
//

/*
void copyDesc(char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"copydesc\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, copyDescCmdHead,
                "\nSpecify one of ROOM or MOB as the second argument.\n\n",
                copyDescExecCommand);
}
*/


//
// copyRoomDesc : copies a room desc from one room to another
//
//   tocopy : vnum of room to copy from
//   copyto : vnum of room to copy to
//

void copyRoomDesc(const ulong tocopy, const ulong copyto)
{
  dikuRoom *copyFrom, *copyTo;
  char strn[128];


  copyFrom = findRoom(tocopy);
  copyTo = findRoom(copyto);

  if (copyFrom == copyTo)
  {
    _outtext("\n"
"Why would you want to copy to the same room you copy from?  Silly little user..\n\n");

    return;
  }

  if (!copyFrom)
  {
    _outtext("\nThe room vnum specified in the third argument does not exist.\n\n");
    return;
  }

  if (!copyTo)
  {
    _outtext("\nThe room vnum specified in the fourth argument does not exist.\n\n");
    return;
  }

  deleteStringNodes(copyTo->roomDescHead);
  copyTo->roomDescHead = copyStringNodes(copyFrom->roomDescHead);

  sprintf(strn, "\nRoom desc of room #%u copied to room #%u.\n\n",
          tocopy, copyto);

  _outtext(strn);

  madeChanges = TRUE;
}


//
// copyMobDesc : copies a mob desc from one mob to another
//
//   tocopy : the mob to copy from
//   copyto : the mob to copy to
//

void copyMobDesc(const ulong tocopy, const ulong copyto)
{
  dikuMob *copyFrom, *copyTo;
  char strn[128];


  copyFrom = findMob(tocopy);
  copyTo = findMob(copyto);

  if (copyFrom == copyTo)
  {
    _outtext("\n"
"Why would you want to copy to the same mob you copy from?  Silly little user..\n\n");

    return;
  }

  if (!copyFrom)
  {
    _outtext("\nThe mob type vnum specified in the third argument does not exist.\n\n");
    return;
  }

  if (!copyTo)
  {
    _outtext("\nThe mob type vnum specified in the fourth argument does not exist.\n\n");
    return;
  }

  deleteStringNodes(copyTo->mobDescHead);
  copyTo->mobDescHead = copyStringNodes(copyFrom->mobDescHead);

  sprintf(strn, "\nMob desc of mob type #%u copied to mob type #%u.\n\n",
          tocopy, copyto);

  _outtext(strn);

  madeChanges = TRUE;
}
