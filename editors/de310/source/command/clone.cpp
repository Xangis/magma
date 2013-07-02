//
//  File: clone.cpp      originally part of dikuEdit
//
//  Usage: functions related to cloning stuff
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

extern char madeChanges;
extern dikuRoom **roomLookup, *roomHead;
extern dikuObject **objLookup, *objectHead;
extern dikuMob **mobLookup, *mobHead;
extern command *cloneCmdHead;
extern ulong highestRoomNumber, highestObjNumber, highestMobNumber, numbLookupEntries;

//
// cloneSumpn
//

void cloneSumpn(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"clone\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, cloneCmdHead,
                "\nSpecify one of ROOM, OBJ, or MOB as the first argument.\n\n",
                cloneExecCommand, FALSE);
}


//
// cloneRoom : clone a specific room a specific number of times
//
//      vnum : vnum of room to clone
//   toclone : number of times to clone room
//

void cloneRoom(const ulong vnum, const ulong toclone)
{
  dikuRoom *room, *endRoom = roomHead, *lastRoom = NULL;
  char strn[256], clonetonumb = FALSE, copyExits;
  ulong i, startclone = getFirstFreeRoomNumber(), j;
  usint ch;


  if (!findRoom(vnum))
  {
    _outtext("\nRoom vnum specified not found.\n\n");
    return;
  }

  if (toclone == 0)
  {
    _outtext("\nTrying to clone a room zero times, eh?  I pity you.\n\n");
    return;
  }

  room = findRoom(vnum);

  if (!room)
  {
    _outtext("\nCouldn't locate room vnum specified in second arg.\n\n");
    return;
  }

  displayColorString("\n&+cCopy room exits along with rooms (&+Cy/N&n&+c)? ");

  do
  {
    ch = toupper(getkey());
  } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

  switch (ch)
  {
    case K_Enter :
    case 'N' : displayColorString("No&n");  copyExits = FALSE; break;
    case 'Y' : displayColorString("Yes&n"); copyExits = TRUE; break;
    default : _outtext("\nerror in input processing for cloneRoom().\n");
              return;
  }

  while (endRoom->Next) endRoom = endRoom->Next;

  for (i = 0; i < toclone; i++)
  {
    if (getFirstFreeRoomNumber() >= numbLookupEntries)
    {
      sprintf(strn, "\n"
"&+cYou have reached the maximum vnum limit - this limit is currently set at\n"
"&+c%u.  To remedy, load dikuEdit with the -ll=<loaded> parameter.\n",
             numbLookupEntries);

      displayColorString(strn);
      endRoom->Next = NULL;
      if (i) madeChanges = TRUE;
      return;
    }

   // tack room info onto end of list

    endRoom->Next = copyRoomInfo(room, TRUE, TRUE);
    endRoom->Last = lastRoom;
    lastRoom = endRoom;

   // set endRoom to new room

    endRoom = endRoom->Next;
    endRoom->Next = NULL;

    if (!copyExits)
    {
      for (j = 0; j < NUMB_EXITS; j++)
      {
        if (endRoom->exits[j])
        {
          deleteRoomExit(endRoom->exits[j], TRUE);
          endRoom->exits[j] = NULL;
        }
      }
    }

    endRoom->roomNumber = getFirstFreeRoomNumber();

    if (endRoom->roomNumber > highestRoomNumber)
      highestRoomNumber = endRoom->roomNumber;

    roomLookup[endRoom->roomNumber] = endRoom;
  }

 // let's be extra-extra safe - though why I'm not sure

  endRoom->Next = NULL;
  endRoom->Last = lastRoom;

  madeChanges = TRUE;

  if (toclone != 1)
  {
    sprintf(strn, "\nRoom #%u cloned %u times (starting at %u).\n\n",
            vnum, toclone, startclone);
  }
  else sprintf(strn, "\nRoom #%u cloned once into %u.\n\n",
               vnum, startclone);

  _outtext(strn);
}


//
// cloneObjectType : clone a specific object a specific number of times
//
//      vnum : vnum of object to clone
//   toclone : number of times to clone object
//

void cloneObjectType(const ulong vnum, const ulong toclone)
{
  dikuObject *obj, *endObject = objectHead;
  char strn[256];
  ulong i, startclone = getFirstFreeObjNumber();


  if (!findObj(vnum))
  {
    _outtext("\nObject vnum specified not found.\n\n");
    return;
  }

  if (toclone == 0)
  {
    _outtext("\nTrying to clone an object zero times, eh?  I pity you.\n\n");
    return;
  }

  obj = findObj(vnum);

  if (!obj)
  {
    _outtext("\nCouldn't locate object vnum specified in second arg.\n\n");
    return;
  }

  while (endObject->Next) endObject = endObject->Next;

  for (i = 0; i < toclone; i++)
  {
    if (getFirstFreeObjNumber() >= numbLookupEntries)
    {
      sprintf(strn, "\n"
"&+cYou have reached the maximum vnum limit - this limit is currently set at\n"
"&+c%u.  To remedy, load dikuEdit with the -ll=<loaded> parameter.\n",
             numbLookupEntries);

      displayColorString(strn);
      endObject->Next = NULL;
      if (i) madeChanges = TRUE;
      return;
    }

    endObject->Next = copyObjectType(obj, TRUE);
    endObject = endObject->Next;
    endObject->Next = NULL;

    endObject->objNumber = getFirstFreeObjNumber();

    if (endObject->objNumber > highestObjNumber)
      highestObjNumber = endObject->objNumber;

    objLookup[endObject->objNumber] = endObject;
  }

//  endObject->Next = NULL;

  madeChanges = TRUE;

  if (toclone != 1)
  {
    sprintf(strn, "\nObject type #%u cloned %u times (starting at %u).\n\n",
            vnum, toclone, startclone);
  }
  else sprintf(strn, "\nObject type #%u cloned once into %u.\n\n",
               vnum, startclone);

  _outtext(strn);
}


//
// cloneMobType : clone a specific mob a specific number of times
//
//      vnum : vnum of mob to clone
//   toclone : number of times to clone mob
//

void cloneMobType(const ulong vnum, const ulong toclone)
{
  dikuMob *mob, *endMob = mobHead;
  char strn[256];
  ulong i, startclone = getFirstFreeMobNumber();


  if (!findMob(vnum))
  {
    _outtext("\nMob vnum specified not found.\n\n");
    return;
  }

  if (toclone == 0)
  {
    _outtext("\nTrying to clone a mob zero times, eh?  I pity you.\n\n");
    return;
  }

  mob = findMob(vnum);

  if (!mob)
  {
    _outtext("\nCouldn't locate mob vnum specified in second arg.\n\n");
    return;
  }

  while (endMob->Next)
  {
    endMob = endMob->Next;
  }

  for (i = 0; i < toclone; i++)
  {
    if (getFirstFreeMobNumber() >= numbLookupEntries)
    {
      sprintf(strn, "\n"
"&+cYou have reached the maximum vnum limit - this limit is currently set at\n"
"&+c%u.  To remedy, load dikuEdit with the -ll=<loaded> parameter.\n",
             numbLookupEntries);

      displayColorString(strn);
      endMob->Next = NULL;
      if (i) madeChanges = TRUE;
      return;
    }

    endMob->Next = copyMobType(mob, TRUE);
    endMob = endMob->Next;
    endMob->Next = NULL;

    endMob->mobNumber = getFirstFreeMobNumber();

    if (endMob->mobNumber > highestMobNumber)
      highestMobNumber = endMob->mobNumber;

    mobLookup[endMob->mobNumber] = endMob;
  }

  endMob->Next = NULL;

  madeChanges = TRUE;

  if (toclone != 1)
  {
    sprintf(strn, "\nMob type #%u cloned %u times (starting at %u).\n\n",
            vnum, toclone, startclone);
  }
  else sprintf(strn, "\nMob type #%u cloned once into %u.\n\n",
               vnum, startclone);

  _outtext(strn);
}
