//
//  File: dispzone.cpp   originally part of dikuEdit
//
//  Usage: functions for displaying zone info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#include "dispzone.h"

#include "../misc/loaded.h"

extern dikuZone zoneRec;
extern ulong numbRooms, numbExits, numbObjTypes, numbObjs, numbMobTypes, numbMobs;

//
// displayZoneResetModeList
//

void displayZoneResetModeList(void)
{
  ulong i;
  char strn[256];


  _setbkcolor(0);

  _outtext("\n\n");

  for (i = ZONE_RESET_LOWEST; i <= ZONE_RESET_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getZoneResetStrn(i));

    _outtext(strn);
  }

  _outtext("\n");
}


//
// addZoneExittoList
//

void addZoneExittoList(zoneExit **head, roomExit *ex, char dir, dikuRoom *room)
{
  zoneExit *node, *prev;

  node = new zoneExit;
  if (!node)
  {
    _outtext("\n\nerror allocating zoneExit node in addZoneExittoList\n\n");
    return;
  }

  memset(node, 0, sizeof(zoneExit));

  if (!*head) *head = node;
  else
  {
    prev = *head;
    while (prev->Next) prev = prev->Next;
    prev->Next = node;
  }

  node->roomIn = room;
  node->outRoom = ex->destRoom;
  node->exitDir = dir;
}


//
// displayZoneExits - display exits that lead to rooms not in zone
//

void displayZoneExits(void)
{
  ulong topNumb = getHighestRoomNumber(), i, lines = 0;
  zoneExit *zoneExitHead = NULL, *node;
  dikuRoom *room;
  char strn[1024], j;


 // create zone exit list..

  for (i = getLowestRoomNumber(); i <= topNumb; i++)
  {
    room = findRoom(i);
    if (!room) continue;

    for (j = 0; j < NUMB_EXITS; j++)
    {
      if (roomExitOutsideZone(room->exits[j]))
        addZoneExittoList(&zoneExitHead, room->exits[j], j, room);
    }
  }


 // run through list, displaying as we go..

  node = zoneExitHead;
  if (!node)
  {
    _outtext("\nNo out-of-zone exit destinations found.\n\n");
    return;
  }

  _outtext("\n\n");

  while (node)
  {
    sprintf(strn, "  &+croom #&+g%u &+L(&n%s&+L) &+R%s&n to &+Croom &+G#%d&n\n",
            node->roomIn->roomNumber, node->roomIn->roomName,
            getExitStrn(node->exitDir), node->outRoom);

    displayColorString(strn);

    lines += numbLinesStringNeeds(strn);

    if (lines >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");
        return;
      }

      _outtext("\n\n");

      lines = 0;
    }

    node = node->Next;
  }

 // delete the list..

  while (zoneExitHead)
  {
    node = zoneExitHead->Next;
    delete zoneExitHead;
    zoneExitHead = node;
  }

  _outtext("\n");
}


//
// displayZoneInfo
//

void displayZoneInfo(void)
{
  char *strn = new char[4096];


  if (!strn)
  {
    _outtext("\n\nError allocating string buffer in displayZoneInfo() - "
             "aborting..\n");

    return;  // no need to die completely, after all
  }

  sprintf(strn,
"\n"
"&+YZone number:&n %d  &+YZone name:&n %s&n\n"
"\n"
"&+YTop room number:&n %u        &+YZone lifespan  :&n %d-%d\n"
"&+YZone reset mode:&n %d (%s)\n"
"\n"
"&+YNumber of rooms/exits     :&n %u/%u\n"
"&+YNumber of obj types/loaded:&n %u/%u\n"
"&+YNumber of mob types/loaded:&n %u/%u\n"
"\n",

zoneRec.zoneNumber, zoneRec.zoneName,

getHighestRoomNumber(),
zoneRec.lifeLow, zoneRec.lifeHigh,
zoneRec.resetMode, getZoneResetStrn(zoneRec.resetMode),

numbRooms, numbExits, numbObjTypes, numbObjs, numbMobTypes, numbMobs);


  displayColorString(strn);

  delete[] strn;

  displayColorString("\n&+C"
"Press any key to display the out-of-zone exits list or Q to quit..&n");

  if (toupper(getkey()) == 'Q') return;
  displayZoneExits();
}
