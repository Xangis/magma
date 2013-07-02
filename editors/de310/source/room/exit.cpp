//
//  File: exit.cpp       originally part of dikuEdit
//
//  Usage: functions related to room exits
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "exit.h"

extern ulong numbExits;
extern char madeChanges;
extern dikuRoom *roomHead;

// tables are good.

_CONST char *exitnames[NUMB_EXITS] =
{
  "north",
  "east",
  "south",
  "west",
  "up",
  "down",
  "northwest",
  "southwest",
  "northeast",
  "southeast"
};

_CONST char revdirs[NUMB_EXITS] =
{
  2,
  3,
  0,
  1,
  5,
  4,
  9,
  8,
  7,
  6
};

//
// getDirfromKeyword : based on data in strn, return corresponding exit
//                     direction or NO_EXIT if no matches found
//
//    strn : exit direction name (hopefully)
//

char getDirfromKeyword(const char *strn)
{
  if (!strcmp(strn, "NORTH") || !strcmp(strn, "N")) return NORTH;
  if (!strcmp(strn, "SOUTH") || !strcmp(strn, "S")) return SOUTH;
  if (!strcmp(strn, "WEST") || !strcmp(strn, "W")) return WEST;
  if (!strcmp(strn, "EAST") || !strcmp(strn, "E")) return EAST;
  if (!strcmp(strn, "UP") || !strcmp(strn, "U")) return UP;
  if (!strcmp(strn, "DOWN") || !strcmp(strn, "D")) return DOWN;
  if (!strcmp(strn, "NORTHWEST") || !strcmp(strn, "NW")) return NORTHWEST;
  if (!strcmp(strn, "SOUTHWEST") || !strcmp(strn, "SW")) return SOUTHWEST;
  if (!strcmp(strn, "NORTHEAST") || !strcmp(strn, "NE")) return NORTHEAST;
  if (!strcmp(strn, "SOUTHEAST") || !strcmp(strn, "SE")) return SOUTHEAST;

  return NO_EXIT;
}


//
// exitNeedsKey : if the exit passed by pointer has an internal keyNumb
//                equal to keyNumb passed, return TRUE
//
//       ex : exit pointer to check
//  keyNumb : value to check for
//

INLINEDEF char exitNeedsKey(const roomExit *ex, const long keyNumb)
{
  if (!ex) return FALSE;

  return (ex->keyNumb == keyNumb);
}


//
// checkRoomExitKey : Checks the exit key object number for validity (if it
//                    isn't found in the object list, it's considered
//                    invalid).
//
//    exitNode : pointer to exit node being checked
//        room : pointer to room that contains exit
//    exitName : "name" of exit - direction it leads
//

INLINEDEF void checkRoomExitKey(const roomExit *exitNode, const dikuRoom *room,
                             const char *exitName)
{
  char outstrn[512];


  if (exitNode && (exitNode->keyNumb))
  {
    if (!findObj(exitNode->keyNumb) && (exitNode->keyNumb != -1) &&
        (exitNode->keyNumb != -2) && getVnumCheckVal())
    {
      sprintf(outstrn, "\n\n"
"Warning: Room #%u's %s exit requires a key that doesn't exist in this\n"
"         .OBJ file (#%d).\n\n"
"Hit any key to continue ...",
              room->roomNumber, exitName, exitNode->keyNumb);

/*  // shouldn't be needed anymore
#ifdef __UNIX__
  _outtext(outstrn);
#else
  cout << outstrn;
#endif
*/
  _outtext(outstrn);

      getkey();
    }
  }
}


//
// compareRoomExits : returns TRUE if they match, FALSE if they don't
//
//  *exit1 : and behind this door...
//  *exit2 : ...a beautiful red wagon!
//

char compareRoomExits(roomExit *exit1, roomExit *exit2)
{
  if (exit1 == exit2) return TRUE;
  if (!exit1 || !exit2) return FALSE;

  if (exit1->worldDoorType != exit2->worldDoorType) return FALSE;
  if (exit1->zoneDoorState != exit2->zoneDoorState) return FALSE;

  if (exit1->keyNumb != exit2->keyNumb) return FALSE;
  if (exit1->destRoom != exit2->destRoom) return FALSE;

  if (!compareStringNodes(exit1->exitDescHead,
                          exit2->exitDescHead)) return FALSE;
  if (!compareStringNodes(exit1->keywordListHead,
                          exit2->keywordListHead)) return FALSE;

  return TRUE;
}


//
// deleteRoomExit : deletes the specified exit
//
//   *srcExit : this exit is going b'bye
//

void deleteRoomExit(roomExit *srcExit, const char decNumbExits)
{
  if (!srcExit) return;


  deleteStringNodes(srcExit->exitDescHead);
  deleteStringNodes(srcExit->keywordListHead);

  delete srcExit;

  if (decNumbExits)
  {
    numbExits--;
    createPrompt();
  }
}


//
// updateRoomExit : hmm..  interesting idea.  not too sure I need this anymore.
//
//   *srcExit : exit being modified
//   *oldRoom : old room
//   *newRoom : new room
//

#if 0
void updateRoomExit(roomExit *srcExit, dikuRoom *oldRoom, dikuRoom *newRoom)
{
  dikuRoom *destRoom;


  if (!srcExit) return;


  destRoom = findRoom(srcExit->destRoom);

  if (destRoom)
  {
    if (destRoom == oldRoom)
    {
      srcExit->destRoom = newRoom->roomNumber;
    }
  }
/*
  else
  {
    cout << "\n\nERROR: updateRoomExit() - srcExit has an invalid roomNumb.  "
            "Things are probably unstable.  Aborting..\n\n";

    return;
  }
*/
}
#endif


//
// copyRoomExit : copies a room exit into a new exit record, returning the
//                address to that new exit
//
//   srcExit : exit to copy from
//

roomExit *copyRoomExit(roomExit *srcExit, const char incNumbLoaded)
{
  roomExit *newExit;


 // check to make sure the source exit exists - kinda helpful

  if (!srcExit) return NULL;

 // alloc mem for new exit rec

  newExit = new roomExit;
  if (!newExit)
  {
    _outtext("\n\nError creating new roomExit in copyRoomExit().  "
            "Out of memory.\n\n");

    return NULL;
  }

 // copy all the easy stuff

  memcpy(newExit, srcExit, sizeof(roomExit));

 // copy all the linked lists

  newExit->exitDescHead = copyStringNodes(srcExit->exitDescHead);
  newExit->keywordListHead = copyStringNodes(srcExit->keywordListHead);

  if (incNumbLoaded)
  {
    numbExits++;
    createPrompt();
  }

 // return the address of the new rec

  return newExit;
}


//
// getExitAvailFlags : Sets an unsigned long's bits based on which exits
//                     are "available" - more accurately, a corresponding bit
//                     is set if a room's particular exit pointer is non-NULL.
//                     The value in the unsigned long is returned.
//
//    room : room to check
//

INLINEDEF ulong getExitAvailFlags(const dikuRoom *room)
{
  ulong exitFlag = 0;

  if (!room) return 0;

  if (room->exits[NORTH])     exitFlag |= EXIT_NORTH_FLAG;
  if (room->exits[SOUTH])     exitFlag |= EXIT_SOUTH_FLAG;
  if (room->exits[WEST])      exitFlag |= EXIT_WEST_FLAG;
  if (room->exits[EAST])      exitFlag |= EXIT_EAST_FLAG;
  if (room->exits[UP])        exitFlag |= EXIT_UP_FLAG;
  if (room->exits[DOWN])      exitFlag |= EXIT_DOWN_FLAG;
  if (room->exits[NORTHWEST]) exitFlag |= EXIT_NORTHWEST_FLAG;
  if (room->exits[SOUTHWEST]) exitFlag |= EXIT_SOUTHWEST_FLAG;
  if (room->exits[NORTHEAST]) exitFlag |= EXIT_NORTHEAST_FLAG;
  if (room->exits[SOUTHEAST]) exitFlag |= EXIT_SOUTHEAST_FLAG;

  return exitFlag;
}


//
// getRoomExitsShortStrn : returns a readable string that shows which
//                         exits exist in the specified room.  string is
//                         copied into strn
//
//     room : room to base string on
//     strn : string to copy stuff into
//

INLINEDEF char *getRoomExitsShortStrn(const dikuRoom *room, char *strn)
{
  ulong exitFlag;


  strn[0] = '\0';

  if (room) exitFlag = getExitAvailFlags(room);
  else return strn;

  if (exitFlag & EXIT_NORTH_FLAG) strcat(strn, "N ");
  if (exitFlag & EXIT_NORTHWEST_FLAG) strcat(strn, "NW ");
  if (exitFlag & EXIT_NORTHEAST_FLAG) strcat(strn, "NE ");
  if (exitFlag & EXIT_SOUTH_FLAG) strcat(strn, "S ");
  if (exitFlag & EXIT_SOUTHWEST_FLAG) strcat(strn, "SW ");
  if (exitFlag & EXIT_SOUTHEAST_FLAG) strcat(strn, "SE ");
  if (exitFlag & EXIT_WEST_FLAG) strcat(strn, "W ");
  if (exitFlag & EXIT_EAST_FLAG) strcat(strn, "E ");
  if (exitFlag & EXIT_UP_FLAG) strcat(strn, "U ");
  if (exitFlag & EXIT_DOWN_FLAG) strcat(strn, "D ");

  if (strlen(strn)) strn[strlen(strn) - 1] = '\0';
  return strn;
}


//
// swapExits : Swaps two exits.  Makes sense, right?
//
//             This function returns TRUE if the parser recognized the
//             keywords, FALSE otherwise
//
//   *strn : user input taken from prompt
//   *room : room record of exits being swapped
//

char swapExits(const char *args, dikuRoom *room)
{
  char strn[256], arg1[256], arg2[256], val;
  roomExit *exit1, *exit2, *oldExit1, *oldExit2;//, *tmpExit;
  ulong exitType1, exitType2;


  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if ((arg1[0] == '\0') || (arg2[0] == '\0'))
  {
    _outtext(
"\nInvalid arguments specified - the format is 'swap <exit1 dir> <exit2 dir>'.\n\n");
    return FALSE;
  }

  val = getDirfromKeyword(arg1);
  if (val == NO_EXIT)
  {
    _outtext(
"\nInvalid arguments specified - the format is 'swap <exit1 dir> <exit2 dir>'.\n\n");
    return FALSE;
  }

  exit1 = oldExit1 = room->exits[val];
  exitType1 = val;

  val = getDirfromKeyword(arg2);
  if (val == NO_EXIT)
  {
    _outtext(
"\nInvalid arguments specified - the format is 'swap <exit1 dir> <exit2 dir>'.\n\n");
    return FALSE;
  }

  exit2 = oldExit2 = room->exits[val];
  exitType2 = val;

  if (exitType1 == exitType2)
  {
    _outtext("\nYou can't swap an exit with itself.\n\n");
    return FALSE;
  }

  room->exits[exitType1] = exit2;
  room->exits[exitType2] = exit1;

  sprintf(strn, "\nSwapped %s exit with %s exit.\n\n",
          exitnames[exitType1], exitnames[exitType2]);
  _outtext(strn);

//  if ((exit1 != oldExit1) || (exit2 != oldExit2)) madeChanges = TRUE;
  madeChanges = TRUE;  // bleh

  return TRUE;
}


//
// swapExitsNorthSouth : Swaps all north/south exits, "reversing" map
//

void swapExitsNorthSouth(void)
{
  roomExit *tmpExit;
  dikuRoom *room = roomHead;

  while (room)
  {
    tmpExit = room->exits[SOUTH];
    room->exits[SOUTH] = room->exits[NORTH];
    room->exits[NORTH] = tmpExit;

    room = room->Next;
  }

  madeChanges = TRUE;

  displayColorString("\n&+cAll north/south exits in all rooms swapped.&n\n\n");
}


//
// swapExitsWestEast : Swaps all west/east exits, "reversing" map
//

void swapExitsWestEast(void)
{
  roomExit *tmpExit;
  dikuRoom *room = roomHead;

  while (room)
  {
    tmpExit = room->exits[EAST];
    room->exits[EAST] = room->exits[WEST];
    room->exits[WEST] = tmpExit;

    room = room->Next;
  }

  madeChanges = TRUE;

  displayColorString("\n&+cAll west/east exits in all rooms swapped.&n\n\n");
}


//
// swapExitsUpDown : Swaps all up/down exits, "reversing" map
//

void swapExitsUpDown(void)
{
  roomExit *tmpExit;
  dikuRoom *room = roomHead;

  while (room)
  {
    tmpExit = room->exits[DOWN];
    room->exits[DOWN] = room->exits[UP];
    room->exits[UP] = tmpExit;

    room = room->Next;
  }

  madeChanges = TRUE;

  displayColorString("\n&+cAll up/down exits in all rooms swapped.&n\n\n");
}


//
// getWorldDoorType : Returns the door type of a specific exit record
//
//   exitRec : exit record to return info on
//

INLINEDEF char getWorldDoorType(const roomExit *exitRec)
{
  if (exitRec) return exitRec->worldDoorType;
  else return 0;
}


//
// getZoneDoorState : Returns the door state of a specific exit record
//
//   exitRec : exit record to return info on
//

INLINEDEF char getZoneDoorState(const roomExit *exitRec)
{
  if (exitRec) return exitRec->zoneDoorState;
  else return 0;
}


//
// resetExits : Searches through the global list of rooms, looking for exits
//              that lead to destRoom and setting to newDest.
//
//     destRoom : original destination
//      newDest : new destination
//

INLINEDEF void resetExits(const long destRoom, const long newDest)
{
  dikuRoom *room = roomHead;
  int i;


  if (destRoom == newDest) return;

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (room->exits[i] && (room->exits[i]->destRoom == destRoom))
        room->exits[i]->destRoom = newDest;
    }

    room = room->Next;
  }
}


//
// clearExits : Searches through a list of dikuRooms, deleting all exits
//              that lead to destRoom
//
//       destRoom : destination room
//   decNumbExits : if TRUE, decrements internal number of exits

INLINEDEF void clearExits(const long destRoom, const char decNumbExits)
{
  dikuRoom *room = roomHead;
  int i;


  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (room->exits[i] && (room->exits[i]->destRoom == destRoom))
      {
        deleteRoomExit(room->exits[i], decNumbExits);
        room->exits[i] = NULL;
      }
    }

    room = room->Next;
  }
}


//
// findCorrespondingExit : Finds an exit that is in srcRoom and leads to
//                         destRoom - tries to, anyway.  exitDir is set
//                         to the direction of the matching exit
//

INLINEDEF roomExit *findCorrespondingExit(const long srcRoom, const long destRoom,
                                       char *exitDir)
{
  dikuRoom *room;
  long i;


  room = findRoom(srcRoom);
  if (!room) return NULL;

  for (i = 0; i < NUMB_EXITS; i++)
  {
    if (room->exits[i] && (room->exits[i]->destRoom == destRoom))
    {
      *exitDir = i;
      return room->exits[i];
    }
  }

// code below is batty..

/*
  while (room)
  {
    currRoomNumb = room->roomNumber;

    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (room->exits[i] && (room->exits[i]->destRoom == destRoom) &&
          (currRoomNumb == srcRoom))
      {
        *exitDir = i;
        return room->exits[i];
      }
    }

    room = room->Next;
  }
*/

  return NULL;
}


//
// getExitStrn : Returns a descriptive string based on the exit direction
//               value
//

INLINEDEF const char *getExitStrn(const char exitDir)
{
/*
  switch (exitDir)
  {
    case NORTH      : return "north";
    case SOUTH      : return "south";
    case WEST       : return "west";
    case EAST       : return "east";
    case UP         : return "up";
    case DOWN       : return "down";
    case NORTHWEST  : return "northwest";
    case SOUTHWEST  : return "southwest";
    case NORTHEAST  : return "northeast";
    case SOUTHEAST  : return "southeast";

    default : return "unrecognized";
  }
*/
  if ((exitDir >= 0) && (exitDir < NUMB_EXITS)) return exitnames[exitDir];
  else return "unrecognized";
}


//
// getExitNode : Based on a room number and an exit direction, returns the
//               corresponding exit node
//

INLINEDEF roomExit *getExitNode(const ulong roomNumb, const char exitDir)
{
  dikuRoom *room = findRoom(roomNumb);

  if (!room || (exitDir < 0) || (exitDir >= NUMB_EXITS)) return NULL;

  return room->exits[exitDir];
}


//
// getExitNode : Based on a room address and an exit direction, returns the
//               corresponding exit node
//

INLINEDEF roomExit *getExitNode(const dikuRoom *room, const char exitDir)
{
  if (!room || (exitDir < 0) || (exitDir >= NUMB_EXITS)) return NULL;

  return room->exits[exitDir];
}


//
// checkExit : checks an exit to make sure it has a corresponding exit on
//             the opposite side with valid type and state set based on the
//             type and state for this exit - prompts user to fix (and fixes)
//             if any errors are found
//
//    exitNode : node to check
//    exitName : name of exit (north, south, etc)
//    roomNumb : number of room exitNode is in
//

void checkExit(roomExit *exitNode, const char *exitName, const long roomNumb)
{
  char zoneType, worldType, zoneBits, worldBits, strn[512];
  usint ch;


  if (!exitNode) return;

 // make sure that exit doesn't have a zone state of 1 or 2 and no door
 // state of 1 2 or 3

  zoneType = exitNode->zoneDoorState & 3;
  zoneBits = exitNode->zoneDoorState & 12;

  worldType = exitNode->worldDoorType & 3;
  worldBits = exitNode->worldDoorType & 12;

  if (zoneType && !worldType)
  {
    sprintf(strn, "\n\n"
"Warning: %s exit in room #%u has a door state of %u set in the .zon, but\n"
"         a door type of 'no door' in the .wld.\n\n"
"Set type/bits in .zon equal to %u (Z), or set door type in .wld to %u (W)? ",

   exitName, roomNumb, exitNode->zoneDoorState,
   worldType | zoneBits, zoneType | worldBits);

    _outtext(strn);

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Z') && (ch != 'W'));

    sprintf(strn, "%c\n\n", (char)ch);
    _outtext(strn);

    if (ch == 'Z') exitNode->zoneDoorState = worldType | zoneBits;
    else exitNode->worldDoorType = zoneType | worldBits;

    madeChanges = TRUE;
  }
}


//
// checkAllExits : checks all exits in the zone, calling checkExit() for each
//

INLINEDEF void checkAllExits(void)
{
  dikuRoom *room = roomHead;
  char strn[64];
  int i;


  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      strcpy(strn, exitnames[i]);
      strn[0] = toupper(strn[0]);

      checkExit(room->exits[i], strn, room->roomNumber);
    }

    room = room->Next;
  }
}


//
// getDescExitStrnforZonefile : Creates a descriptive string about an exit -
//                              used for comments in zones
//
//       room : room exit is in
//    exitRec : specific exit
//    endStrn : string is put in here
//   exitName : "name" of exit - north, south, etc
//

char *getDescExitStrnforZonefile(const dikuRoom *room, const roomExit *exitRec,
                                 char *endStrn, const char *exitName)
{
  char fromRoom[MAX_ROOMNAME_LEN], toRoom[MAX_ROOMNAME_LEN],
       exitInfo[256], endString[1024], doorState;
  dikuRoom *tmpRoom;


  strcpy(fromRoom, room->roomName);
  remColorCodes(fromRoom);

  tmpRoom = findRoom(exitRec->destRoom);
  if (tmpRoom)
  {
    strcpy(toRoom, tmpRoom->roomName);
    remColorCodes(toRoom);
  }
  else
  {
    strcpy(toRoom, "[destination room not found]");
  }

 // now, fill up exitInfo string

  doorState = getZoneDoorState(exitRec);

//  if (doorState)
//  {
    strcpy(exitInfo, " - ");

    if ((doorState & 3) == 3)
    {
      strcat(exitInfo, "closed/locked/unpickable/");
    }
    else
    if (doorState & 1)
    {
      strcat(exitInfo, "closed/");
    }
    else
    if (doorState & 2)
    {
      strcat(exitInfo, "closed/locked/");
    }
    else strcat(exitInfo, "open/");

    if (doorState & 4)
    {
      strcat(exitInfo, "secret/");
    }

    if (doorState & 8)
    {
      strcat(exitInfo, "blocked/");
    }

   // cut out the final trailing slash

    exitInfo[strlen(exitInfo) - 1] = '\0';
//  }

 // compose string to be suitable for zone file

  sprintf(endString, "*     %s exit from \"%s\" to\n"
                     "*     \"%s\"%s\n*\n",
                         exitName, fromRoom, toRoom, exitInfo);

  strcpy(endStrn, endString);

  return endStrn;
}


//
// getNumbExits : returns number of exits in room passed
//
//    room : room to check
//

INLINEDEF char getNumbExits(const dikuRoom *room)
{
  char numb = 0, i;

  if (!room) return 0;

  for (i = 0; i < NUMB_EXITS; i++) if (room->exits[i]) numb++;

  return numb;
}


//
// isExitOut : returns TRUE if exit is considered to lead to somewhere
//             (exit destination room does not have to exist, a destination
//             of less than 0 is considered to be nowhere)
//
//     ex : exit to check
//

INLINEDEF char isExitOut(const roomExit *ex)
{
  return (ex && (ex->destRoom >= 0));
}


//
// roomExitOutsideZone : returns TRUE if exit is considered to lead outside
//                       the zone
//
//   ex : exit to check
//

INLINEDEF char roomExitOutsideZone(const roomExit *ex)
{
/*
  if (!ex) return FALSE;

  if (((ex->destRoom < 0) && getNegDestOutofZoneVal()) ||
      ((ex->destRoom >= 0) && (!findRoom(ex->destRoom)))) return TRUE;

  return FALSE;
*/
  return (ex && (((ex->destRoom < 0) && getNegDestOutofZoneVal()) ||
                 ((ex->destRoom >= 0) && (!findRoom(ex->destRoom)))));
}
