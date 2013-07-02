//
//  File: crtroomu.cpp   originally part of dikuEdit
//
//  Usage: user-interface end functions for creating rooms from prompt
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"
#include "room.h"

extern dikuRoom *roomHead, *currentRoom;
extern ulong numbLookupEntries;
extern char madeChanges;

//
// createRoomPrompt : This function is poorly named, so sue me.
//
//                    At any rate, it creates a new room record, tacks it onto
//                    the end of the list pointed to by roomHead, creates an
//                    exit leading to the new room from currentRoom, and
//                    optionally creates an exit loading from the new room to
//                    currentRoom.
//
//                    returns FALSE if didn't create, TRUE if did
//
//   args : user input
//

char createRoomPrompt(const char *args)
{
  usint ch, oldch;
  ulong exitAvail;
  char strn[256] = "\0";
  roomExit **exitPtr;
  dikuRoom *room, *roomPtr = roomHead;
  long i = -1;


  if (getFirstFreeRoomNumber() >= numbLookupEntries)
  {
    displayColorString("\n"
"Cannot create any more rooms - the limit has been reached.  To increase,\n"
"load dikuEdit with the -ll=<limit> parameter.\n\n");

    return FALSE;
  }

  if (strlen(args))
  {
    if (!strnumer(args))
    {
      _outtext("\nError in vnum argument - non-numerics in input.\n\n");
      return FALSE;
    }

    i = atoi(args);
    if (findRoom(i))
    {
      sprintf(strn, "\n"
"Cannot create a room with the vnum %u - a room with this vnum already exists.\n\n",
              i);
      _outtext(strn);

      return FALSE;
    }

    if (i >= numbLookupEntries)
    {
      sprintf(strn, "\n"
"Cannot create a room with that vnum - the limit of %u would be exceeded.\n"
"To remedy, load dikuEdit with the -ll=<limit> parameter.\n\n",
              numbLookupEntries - 1);

      return FALSE;
    }
  }

  exitAvail = getExitAvailFlags(currentRoom);

 // based on value of exitAvail, do stuff

  displayColorString("\n"
"&+cWhich direction should the new room be in from this one:\n");

 // set colors of each choice based on which are available

  if (!(exitAvail & EXIT_NORTH_FLAG))
  {
    strcat(strn, "[&+CN&n&+c]orth, ");
  }
  else strcat(strn, "&+L[N]orth,&n&+c ");

  if (!(exitAvail & EXIT_NORTHWEST_FLAG))
  {
    strcat(strn, "[&+C1&n&+c]Northwest, ");
  }
  else strcat(strn, "&+L[1]Northwest,&n&+c ");

  if (!(exitAvail & EXIT_NORTHEAST_FLAG))
  {
    strcat(strn, "[&+C2&n&+c]Northeast, ");
  }
  else strcat(strn, "&+L[2]Northeast,&n&+c ");

  if (!(exitAvail & EXIT_SOUTH_FLAG))
  {
    strcat(strn, "[&+CS&n&+c]outh, ");
  }
  else strcat(strn, "&+L[S]outh,&n&+c ");

  if (!(exitAvail & EXIT_SOUTHWEST_FLAG))
  {
    strcat(strn, "[&+C3&n&+c]Southwest, ");
  }
  else strcat(strn, "&+L[3]Southwest,&n&+c ");

  if (!(exitAvail & EXIT_SOUTHEAST_FLAG))
  {
    strcat(strn, "[&+C4&n&+c]Southeast, ");
  }
  else strcat(strn, "&+L[4]Southeast,&n&+c ");

  if (!(exitAvail & EXIT_WEST_FLAG))
  {
    strcat(strn, "[&+CW&n&+c]est, ");
  }
  else strcat(strn, "&+L[W]est,&n&+c ");

  if (!(exitAvail & EXIT_EAST_FLAG))
  {
    strcat(strn, "[&+CE&n&+c]ast, ");
  }
  else strcat(strn, "&+L[E]ast,&n&+c ");

  if (!(exitAvail & EXIT_UP_FLAG))
  {
    strcat(strn, "[&+CU&n&+c]p, ");
  }
  else strcat(strn, "&+L[U]p,&n&+c ");

  if (!(exitAvail & EXIT_DOWN_FLAG))
  {
    strcat(strn, "or [&+CD&n&+c]own (&+CQ to quit, X for none&n&+c)? &n");
  }
  else strcat(strn, "&+L[D]own &n&+c(&+CQ to quit, X for none&n&+c)? &n");

  displayColorString(strn);


  do
  {
    ch = toupper(getkey());
  } while (((ch != 'N') || ((exitAvail & EXIT_NORTH_FLAG))) &&
           ((ch != '1') || ((exitAvail & EXIT_NORTHWEST_FLAG))) &&
           ((ch != '2') || ((exitAvail & EXIT_NORTHEAST_FLAG))) &&
           ((ch != 'S') || ((exitAvail & EXIT_SOUTH_FLAG))) &&
           ((ch != '3') || ((exitAvail & EXIT_SOUTHWEST_FLAG))) &&
           ((ch != '4') || ((exitAvail & EXIT_SOUTHEAST_FLAG))) &&
           ((ch != 'W') || ((exitAvail & EXIT_WEST_FLAG))) &&
           ((ch != 'E') || ((exitAvail & EXIT_EAST_FLAG))) &&
           ((ch != 'U') || ((exitAvail & EXIT_UP_FLAG))) &&
           ((ch != 'D') || ((exitAvail & EXIT_DOWN_FLAG))) &&
            (ch != 'Q') && (ch != 'X'));

  switch (ch)
  {
    case 'N' : exitPtr = &(currentRoom->exits[NORTH]);  break;
    case '1' : exitPtr = &(currentRoom->exits[NORTHWEST]);  break;
    case '2' : exitPtr = &(currentRoom->exits[NORTHEAST]);  break;
    case 'S' : exitPtr = &(currentRoom->exits[SOUTH]);  break;
    case '3' : exitPtr = &(currentRoom->exits[SOUTHWEST]);  break;
    case '4' : exitPtr = &(currentRoom->exits[SOUTHEAST]);  break;
    case 'W' : exitPtr = &(currentRoom->exits[WEST]);  break;
    case 'E' : exitPtr = &(currentRoom->exits[EAST]);  break;
    case 'U' : exitPtr = &(currentRoom->exits[UP]);  break;
    case 'D' : exitPtr = &(currentRoom->exits[DOWN]);  break;
    case 'Q' : _outtext("\n\n");  return FALSE;
    case 'X' : exitPtr = NULL;  break;

    default : _outtext("\n\nERROR: createRoomPrompt() - user input checking"
                       " incorrect - notify the author or sump'n\n\n");

              return FALSE;
  }

 // create the room

  room = createRoom(TRUE, i);
/*
  if (i != -1)
  {
    room->roomNumber = i;
  }
*/

 // stick the room in the room list

  if (!roomPtr) roomHead = room;  // this shouldn't ever happen, by the way
  else
  while (roomPtr)
  {
    if (roomPtr->Next) roomPtr = roomPtr->Next;
    else break;
  }

  roomPtr->Next = room;
  room->Last = roomPtr;

 // create the exit and set stuff - must be done here

  if (exitPtr)
  {
    createExit(exitPtr, TRUE);
    (*exitPtr)->destRoom = room->roomNumber;
  }

 // now do opposite exit - don't allow them to hit Enter for opposite
 // exit if no exit entered in first direction

  if (ch != 'X')
  {
    displayColorString(
  "\n\n"
  "&+cWhich direction should lead to this room from the new room:\n"
  "&+c[&+CN&n&+c]orth, [&+C1&n&+c]Northwest, [&+C2&n&+c]Northeast, "
  "[&+CS&n&+c]outh, [&+C3&n&+c]Southwest, [&+C4&n&+c]Southeast, [&+CW&n&+c]est,"
  " [&+CE&n&+c]ast, [&+CU&n&+c]p, [&+CD&n&+c]own "
  "(&+CEnter=opposite/X=none/Q=quit&n&+c)? ");
  }
  else
  {
    displayColorString(
  "\n\n"
  "&+cWhich direction should lead to this room from the new room:\n"
  "&+c[&+CN&n&+c]orth, [&+C1&n&+c]Northwest, [&+C2&n&+c]Northeast, "
  "[&+CS&n&+c]outh, [&+C3&n&+c]Southwest, [&+C4&n&+c]Southeast, [&+CW&n&+c]est,"
  " [&+CE&n&+c]ast, [&+CU&n&+c]p, [&+CD&n&+c]own "
  "(&+LEnter=opposite&+C/X=none/Q=quit&n&+c)? ");
  }

  oldch = ch;  // save the previous exit choice

  while (TRUE)
  {
    ch = toupper(getkey());

    if ((ch == 'N') || (ch == 'S') || (ch == 'W') || (ch == 'E') ||
        (ch == 'U') || (ch == 'D') || (ch == '1') || (ch == '2') ||
        (ch == '3') || (ch == '4') || ((oldch != 'X') && (ch == K_Enter)) ||
        (ch == 'X') || (ch == 'Q'))
      break;
  }

  switch (ch)
  {
    case 'N' : exitPtr = &(room->exits[NORTH]);  break;
    case '1' : exitPtr = &(room->exits[NORTHWEST]);  break;
    case '2' : exitPtr = &(room->exits[NORTHEAST]);  break;
    case 'S' : exitPtr = &(room->exits[SOUTH]);  break;
    case '3' : exitPtr = &(room->exits[SOUTHWEST]);  break;
    case '4' : exitPtr = &(room->exits[SOUTHEAST]);  break;
    case 'W' : exitPtr = &(room->exits[WEST]);  break;
    case 'E' : exitPtr = &(room->exits[EAST]);  break;
    case 'U' : exitPtr = &(room->exits[UP]);  break;
    case 'D' : exitPtr = &(room->exits[DOWN]);  break;
    case 'X' : exitPtr = NULL;  break;
    case 'Q' : roomPtr->Next = NULL;
               deleteRoomInfo(room, TRUE, TRUE, TRUE);
//               if (*exitPtr) deleteRoomExit(*exitPtr, TRUE);
//               *exitPtr = NULL;

               rebuildRoomLookup();

               _outtext("\n\n");
               return FALSE;

    case K_Enter : switch (oldch)
                   {
                     case 'N' : exitPtr = &(room->exits[SOUTH]);  break;
                     case 'S' : exitPtr = &(room->exits[NORTH]);  break;
                     case 'W' : exitPtr = &(room->exits[EAST]);  break;
                     case 'E' : exitPtr = &(room->exits[WEST]);  break;
                     case 'U' : exitPtr = &(room->exits[DOWN]);  break;
                     case 'D' : exitPtr = &(room->exits[UP]);  break;
                     case '1' : exitPtr = &(room->exits[SOUTHEAST]); break;
                     case '2' : exitPtr = &(room->exits[SOUTHWEST]); break;
                     case '3' : exitPtr = &(room->exits[NORTHEAST]); break;
                     case '4' : exitPtr = &(room->exits[NORTHWEST]); break;

                     default  : _outtext(
"\nerror: createRoomPrompt() - user input checking incorrect (#3) - notify the\n"
"author or sump'n\n\n");
                                return FALSE;
                   }
                   break;

    default : _outtext("\n\nERROR: createRoomPrompt() - user input checking"
                       " incorrect (#2) - notify the author or sump'n\n\n");

              return FALSE;
  }

 // create the exit

  if (exitPtr)
  {
    createExit(exitPtr, TRUE);
    (*exitPtr)->destRoom = currentRoom->roomNumber;
  }

  madeChanges = TRUE;

  createPrompt();  // update the prompt

  _outtext("\n\n");
  return TRUE;
}
