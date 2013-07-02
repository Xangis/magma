//
//  File: crtroom.cpp    originally part of dikuEdit
//
//  Usage: functions for creating a room structure
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>

#include "../types.h"
#include "../fh.h"

using namespace std;

extern dikuRoom *defaultRoom;
extern dikuZone zoneRec;
extern ulong numbLookupEntries, numbRooms, highestRoomNumber, lowestRoomNumber;
extern dikuRoom **roomLookup;

//
// createRoom : Function to create a new room - returns pointer to the new
//              room
//

INLINEDEF dikuRoom *createRoom(const char incLoaded, const long roomNumb)
{
  dikuRoom *newRoom;
  ulong numb;


 // create a new room

  newRoom = new dikuRoom;
  if (!newRoom)
  {
    cout << "\n\nError creating new dikuRoom in createRoom().  "
            "(memory problem)  Aborting.\n\n";

    return NULL;
  }

  if (defaultRoom)
  {
    newRoom = copyRoomInfo(defaultRoom, FALSE, TRUE);
  }
  else
  {
    memset(newRoom, 0, sizeof(dikuRoom));

    strcpy(newRoom->roomName, "Unnamed");
  }

  newRoom->zoneNumber = zoneRec.zoneNumber;

  if (roomNumb >= 0)
    newRoom->roomNumber = numb = roomNumb;
  else
    newRoom->roomNumber = numb = getFirstFreeRoomNumber();

  if (incLoaded)
  {
   // check if we have a dupe or if the vnum is too high

    if (findRoom(numb) || (numb >= numbLookupEntries))
    {
      deleteRoomInfo(newRoom, FALSE, FALSE, FALSE);
      return NULL;
    }

    numbRooms++;

    roomLookup[numb] = newRoom;

    if (numb > getHighestRoomNumber()) highestRoomNumber = numb;
    if (numb < getLowestRoomNumber())  lowestRoomNumber = numb;

    createPrompt();
  }

  return newRoom;
}
