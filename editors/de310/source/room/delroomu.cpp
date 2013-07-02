//
//  File: delroomu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions to delete rooms
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "room.h"

extern dikuRoom *currentRoom, *roomHead;
extern ulong numbRooms;
extern char madeChanges;

//
// deleteRoomUserCommon : Called by deleteRoomUser() - roomNumber is
//                        assumed to be valid or -1 (in which case number
//                        of current room is used) - returns TRUE if
//                        successful, FALSE otherwise
//
//       roomNumber : number of room to delete, or -1 for current room
//

char deleteRoomCommon(const long roomNumber)
{
  dikuRoom *roomNode;
  usint ch;


  if (roomNumber == -1) roomNode = findRoom(currentRoom->roomNumber);
  else roomNode = findRoom(roomNumber);

  do
  {
    ch = toupper(getkey());
  } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

  if (ch == 'Y')
  {
    _outtext("Yes");

    if (roomNode == roomHead)
    {
      roomHead = roomHead->Next;
      if (!roomHead)
      {
        _outtext(
"\n\nError in deleteRoomUserCommon(): roomHead set to NULL - will try to\n"
"recover, but be warned that things could also now be extremely 'wacky'..\n\n");

        roomHead = roomNode;  // at least try to recover (assuming that
                              // code didn't check if roomNode was only
                              // room in list)

        return FALSE;

//        exit(1);
      }
    }

    if (roomNode->Last)
    {
      roomNode->Last->Next = roomNode->Next;
    }

    if (roomNode->Next)
    {
      roomNode->Next->Last = roomNode->Last;
    }

    if (roomNode == currentRoom) currentRoom = roomHead;

    deleteRoomInfo(roomNode, TRUE, TRUE, TRUE);

    resetLowHighRoom();

    madeChanges = TRUE;

    displayColorString(
"\n&+cExits pointing to deleted room: &+Cs&n&+cet to -1, &+Cd&n&+celete,"
" or &+Cn&n&+ceither (&+Cs/D/n&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'S') && (ch != 'D') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'S')
    {
      _outtext("set to -1\n\n");

      resetExits(roomNumber, -1);
    }
    else
    if (ch == 'N')
    {
      _outtext("neither\n\n");
    }
    else  // d or enter
    {
      _outtext("delete\n\n");

      clearExits(roomNumber, TRUE);
    }
  }
  else _outtext("No\n\n");

  return TRUE;
}


//
// deleteRoomUser : interprets arguments passed by user to command -
//                  if no input, assumes current room, otherwise interprets
//                  args as vnum of room to delete
//
//     args : user input
//

void deleteRoomUser(const char *args)
{
  char outStrn[256], newName[256];
  dikuRoom *roomNode;
  long i;


 // delete currentRoom

  if (strlen(args) == 0)
  {
    if (!roomHead->Next)
    {
      _outtext(
"\nCannot delete this room (it is the only room in the zone).\n\n");

      return;
    }

    displayColorString("\n&+cDelete current room (&+Cy/N&n&+c)? ");

    deleteRoomCommon(-1);  // currentRoom
  }
  else
  {
    if (!strnumer(args))
    {
      _outtext("\nError in input - specify a room number as the argument.\n\n");

      return;
    }

    i = atoi(args);
    roomNode = findRoom(i);

    if (!roomNode)
    {
      sprintf(outStrn, "\nRoom #%u does not exist.\n\n", i);

      _outtext(outStrn);
      return;
    }

    if (numbRooms == 1)
    {
      _outtext(
"\nCannot delete this room (it is the only room in the zone).\n\n");

      return;
    }

    fixString(roomNode->roomName, newName, 45, !getShowColorVal());

    sprintf(outStrn, "\n&+cDelete room #%u, &+L\"&n%s&+L\"&n&+c (&+Cy/N&n&+c)? ",
            i, newName);

    displayColorString(outStrn);

    deleteRoomCommon(i);
  }
}
