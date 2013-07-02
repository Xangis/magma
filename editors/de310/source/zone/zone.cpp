//
//  File: zone.cpp       originally part of dikuEdit
//
//  Usage: functions used for various zone-related stuff
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "zone.h"

extern dikuRoom *roomHead;
extern dikuZone zoneRec;
extern char madeChanges;

//
// fixZoneName : Trims *oldName to MAX_ZONESHORT_LEN, setting *newName with
//               the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixZoneName(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_ZONESHORT_LEN, !getShowColorVal());
}


//
// setZoneNumb
//

void setZoneNumb(const ulong numb, const char dispText)
{
  dikuRoom *room = roomHead;
  char strn[128];


  if (numb == 0)
  {
    _outtext("\nZone number unchanged.\n\n");

    return;
  }

  while (room)
  {
    room->zoneNumber = numb;

    room = room->Next;
  }

  zoneRec.zoneNumber = numb;

  if (dispText)
  {
    sprintf(strn, "\nAll rooms (and zone itself) set to zone number of %u.\n\n",
            numb);

    _outtext(strn);
  }

  madeChanges = TRUE;
}


//
// setZoneNumbStrn : Sets zone number of all room recs and of zone rec.
//
//   *strn : user input
//

void setZoneNumbStrn(const char *args)
{
  ulong i;

  if (!strlen(args))
  {
    setZoneNumbPrompt();
    return;
  }

  if (!strnumer(args))
  {
    _outtext("\nThe 'set zone number' command requires a numeric argument.\n\n");
    return;
  }

  i = atoi(args);

  setZoneNumb(i, TRUE);
}


//
// setZoneNumbPrompt : Prompts a user to type in a room number to jump to.
//

void setZoneNumbPrompt(void)
{
  ulong i;
  char numbStrn[256] = "\0";


  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+CEnter new zone number value:&n ");

    getStrn(numbStrn, 4, 1, 7, '°', numbStrn, FALSE, FALSE);

    break;
  }

  _setbkcolor(0);
  i = atoi(numbStrn);

  setZoneNumb(i, TRUE);
}
