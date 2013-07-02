//
//  File: roomu.cpp      originally part of dikuEdit
//
//  Usage: various user-interface functions relating to commands
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "room.h"
#include "roomu.h"

#include "../keys.h"

extern dikuRoom *currentRoom, *roomHead;
extern ulong numbExits, numbRooms, numbLookupEntries;
extern char madeChanges;
extern _CONST char *exitnames[];
extern _CONST char revdirs[];

//
// gotoRoomStrn : Goes to a room based on user input in *args
//
//   *args : user input
//

void gotoRoomStrn(const char *args)
{
  char tempStrn[512], numeric = strnumer(args);

  dikuRoom *room;


  if (!strlen(args))
  {
    gotoRoomPrompt();
    return;
  }

 // try to find a match

  room = getRoomKeyword(args, TRUE);

 // uh-oh, no match

  if (!room)
  {
    if (numeric)
    {
      sprintf(tempStrn, "\nCorresponding room (#%s) not found.\n\n",
              args);
    }
    else
    {
      sprintf(tempStrn, "\n"
"No objs or mobs contain that keyword (%s), and no room titles\n"
"contain that substring.\n\n",
              args);
    }

    _outtext(tempStrn);

    return;
  }
  else
  {
    currentRoom = room;

    displayRoom(currentRoom);
  }
}


//
// gotoRoomPrompt : Prompts a user to type in a room number to jump to.
//

void gotoRoomPrompt(void)
{
  ulong i;
  char numbStrn[256], strn[256];
  dikuRoom *room;


//  _outtext("\n");

  while (TRUE)
  {
    displayColorString("\n&+cEnter room number to go to (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', "\0", FALSE, FALSE);

    if (!strcmp(numbStrn, "?")) displayRoomList();
    else break;
  }

  _setbkcolor(0);
  i = atoi(numbStrn);

  room = findRoom(i);

  if (!room)
  {
    sprintf(strn, "\nCorresponding room (#%s) not found.\n\n",
            numbStrn);

    _outtext(strn);

    return;
  }
  else
  {
    currentRoom = room;

    _outtext("\n");

    displayRoom(currentRoom);
  }
}


//
// createGrid : given x, y, and z sizes, creates a grid of rooms connected by
//              north, south, west, and east exits - returns TRUE if
//              successful, FALSE otherwise
//
//   sizex : number of columns in grid
//   sizey : number of rows in grid
//   sizez : number of levels in grid
//

char createGrid(const long sizex, const long sizey, const long sizez,
                const char exitCreation)
{
  ulong gridsize, roomNumb, x, y, z, arrPos;
  register long i;
  ulong *roomArr, startExit = numbExits, startRoom = numbRooms;
  register dikuRoom *room = roomHead, *prevRoom = NULL;
//  clock_t start = clock(), end;
//  float dummy;
  char strn[256];


  if ((sizex <= 0) || (sizey <= 0) || (sizez <= 0))
  {
    _outtext("\nThe x-size, y-size, or z-size of grid is 0 or less - aborting.\n\n");
    return FALSE;
  }

  gridsize = sizex * sizey * sizez;

  if (getNumbFreeRooms() < gridsize)
  {
    sprintf(strn, "\n"
"Creating a grid that large would exceed the current vnum limit of %u.\n"
"To remedy, load dikuEdit with the -ll=<limit> parameter to increase the max\n"
"vnum limit.\n\n", numbLookupEntries);

    _outtext(strn);
    return FALSE;
  }

  roomArr = new ulong[gridsize];
  if (!roomArr)
  {
    _outtext("\nError alloc'ing roomArr in createGrid()\n\n");
    return FALSE;
  }

  while (room)
  {
    prevRoom = room;
    room = room->Next;
  }

 // create rooms for grid

  for (i = gridsize - 1; i >= 0; i--)
  {
    room = createRoom(TRUE, -1);

    prevRoom->Next = room;
    room->Last = prevRoom;
    prevRoom = room;

    roomArr[i] = room->roomNumber;

//    _outtext("*");
  }

 // link rooms

  arrPos = 0;

  for (z = 0; z < sizez; z++)
  {
    for (y = 0; y < sizey; y++)
    {
      for (x = 0; x < sizex; x++)
      {
        roomNumb = roomArr[arrPos];

        if (z != 0) createExit(roomNumb, roomArr[arrPos - (sizex * sizey)], DOWN);
        if (z != (sizez - 1))
                    createExit(roomNumb, roomArr[arrPos + (sizex * sizey)], UP);

        if ((exitCreation == GRID_STANDARD) || (exitCreation == GRID_BOTH))
        {
          if (y != 0) createExit(roomNumb, roomArr[arrPos - sizex], SOUTH);
          if (y != (sizey - 1))
                      createExit(roomNumb, roomArr[arrPos + sizex], NORTH);
          if (x != 0) createExit(roomNumb, roomArr[arrPos - 1], EAST);
          if (x != (sizex - 1))
                      createExit(roomNumb, roomArr[arrPos + 1], WEST);
        }

        if ((exitCreation == GRID_DIAGONAL) || (exitCreation == GRID_BOTH))
        {
          if ((x != (sizex - 1)) && (y != (sizey - 1)))
            createExit(roomNumb, roomArr[arrPos + sizex + 1], NORTHWEST);
          if ((x != 0) && (y != (sizey - 1)))
            createExit(roomNumb, roomArr[(arrPos + sizex) - 1], NORTHEAST);
          if ((x != (sizex - 1)) && (y != 0))
            createExit(roomNumb, roomArr[(arrPos - sizex) + 1], SOUTHWEST);
          if ((x != 0) && (y != 0))
            createExit(roomNumb, roomArr[arrPos - sizex - 1], SOUTHEAST);
        }

  //      _outtext(".");

        arrPos++;
      }
    }
  }

  sprintf(strn, "\n"
"%ux%ux%u grid created, starting at room #%u (upper northwest corner)\n"
"and ending at room #%u (lower southeast corner) (%u exits, %u rooms\n"
"created).\n\n",
          sizex, sizey, sizez, roomArr[gridsize - 1], roomArr[0],
          numbExits - startExit, numbRooms - startRoom);

  _outtext(strn);

  madeChanges = TRUE;

  delete[] roomArr;

  return TRUE;
/*
  end = clock();

  dummy = (end - start) / CLOCKS_PER_SEC;
  dummy *= 1000;

  sprintf(strn, "\nTotal time: %f milliseconds\n\n", dummy);
  _outtext(strn);
*/
}


//
// createGridInterp : interprets user input, and if valid, passes it to
//                    createGrid()
//
//    args : user arguments to command
//

void createGridInterp(const char *args)
{
  ulong x, y, z;
  char arg1[256], arg2[256], arg3[256];
  usint ch;


  if (!strlen(args))
  {
    _outtext("\n"
"Specify an x, y, and z size (optional) for the grid as the first, second, and\n"
"and third arguments.\n\n");
    return;
  }

  getArg(args, 1, arg1);
  getArg(args, 2, arg2);
  getArg(args, 3, arg3);

  if (arg3[0] == '\0')  // z-size - assume 1
  {
    strcpy(arg3, "1");
  }

  if (arg2[0] == '\0')  // no second arg - DOH!
  {
    _outtext("\nSpecify a y-size as the second argument.\n\n");
    return;
  }
  else
  {
    if (!strnumer(arg1) || !strnumer(arg2) || !strnumer(arg3))
    {
      _outtext("\n"
"The first, second, and third arguments (third argument optional) should specify\n"
"the x, y, and z sizes of the grid.\n\n");
      return;
    }
  }

  displayColorString("\n"
"&+cLink the rooms with &+Cs&n&+ctandard compass directions, &+Cd&n&+ciagonal directions, or\n"
"&+cboth (&+Cs/d/B&n&+c)? ");

  do
  {
    ch = toupper(getkey());
  } while ((ch != K_Enter) && (ch != 'S') && (ch != 'D') && (ch != 'B'));

  x = atoi(arg1);
  y = atoi(arg2);
  z = atoi(arg3);

  if ((ch == K_Enter) || (ch == 'B'))
  {
    displayColorString("both&n\n");
    createGrid(x, y, z, GRID_BOTH);
  }
  else if (ch == 'D')
  {
    displayColorString("diagonal&n\n");
    createGrid(x, y, z, GRID_DIAGONAL);
  }
  else
  {
    displayColorString("standard&n\n");
    createGrid(x, y, z, GRID_STANDARD);
  }
}


//
// linkRooms : assumes from room and to room exist and dir is valid - creates
//             exits between the two rooms specified, leading to each other -
//             returns TRUE if successful, FALSE is failed (can only fail if
//             user aborts)
//
//     fromnumb : source room
//       tonumb : destination room
//    direction : direction of exit from source room
//

char linkRooms(const ulong fromnumb, const ulong tonumb, const char direction)
{
  dikuRoom *fromr = findRoom(fromnumb), *tor = findRoom(tonumb);
  char rev = revdirs[direction], strn[256], addedboth = TRUE;
  usint ch;


  _outtext("\n");

  if (fromr->exits[direction])
  {
    displayColorString(
"&+cSource room already has an exit in that direction - continue (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'Y')
    {
      displayColorString("yes&n\n");
    }
    else
    {
      displayColorString("no&n\n\n");
      return FALSE;
    }

    addedboth = FALSE;
  }

  if (tor->exits[rev])
  {
    displayColorString(
"&+cDestination room already has an exit in the opposite direction -\n"
"continue (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'Y')
    {
      displayColorString("yes&n\n");
    }
    else
    {
      displayColorString("no&n\n\n");
      return FALSE;
    }

    addedboth = FALSE;
  }

/*
  if (fromr->exits[direction] && tor->exits[rev])
  {
    _outtext(
"Silly goose, there's nothing to do (both exits already exist - though they may\n"
"lead to rooms other than the ones you want).\n\n");

    return FALSE;
  }
*/

  if (!fromr->exits[direction])
  {
    createExit(fromnumb, tonumb, direction);
  }

  if (!tor->exits[rev])
  {
    createExit(tonumb, fromnumb, rev);
  }

  sprintf(strn, "Exit%s created between rooms %u and %u (%s-%s).\n\n",
          addedboth ? "s" : "", fromnumb, tonumb,
          exitnames[direction], exitnames[rev]);
  _outtext(strn);

  return TRUE;
}


//
// linkRoomsInterp : interprets user input, massages it, checks it, and passes
//                   it to linkRooms if it's valid
//
//      args : user arguments to command
//

void linkRoomsInterp(const char *args)
{
  char arg1[256], arg2[256], arg3[256], dir, isnum1, isnum2 = TRUE;
  long room1, room2;

  if (numbArgs(args) == 3)
  {
    getArg(args, 1, arg1);
    getArg(args, 2, arg2);
    getArg(args, 3, arg3);

    room1 = atoi(arg1);
    room2 = atoi(arg2);
    dir = getDirfromKeyword(arg3);

    isnum1 = strnumer(arg1);
    isnum2 = strnumer(arg2);
  }
  else if (numbArgs(args) == 2)
  {
    getArg(args, 1, arg1);
    getArg(args, 2, arg2);

    room1 = currentRoom->roomNumber;
    room2 = atoi(arg1);
    dir = getDirfromKeyword(arg2);

    isnum1 = strnumer(arg1);
  }
  else  // in order to prevent redundancy, just set some variable to invalid
  {
    isnum1 = FALSE;
  }

  if (!isnum1 || !findRoom(room1) || !isnum2 || !findRoom(room2) ||
      (dir == NO_EXIT))
  {
    _outtext("\n"
"The 'link' command can take two or three arguments: either 'link <fromroom>\n"
"<toroom> <direction>' or 'link <toroom> <direction>' (current room is assumed\n"
"as source room in latter case).\n\n");

    return;
  }

  linkRooms(room1, room2, dir);
}

//
// renumberRoomsUser
//

void renumberRoomsUser(const char *args)
{
  char strn[256];
  long usernumb, oldNumb, tempstart;

  if (strlen(args) == 0)
  {
    renumberRooms(FALSE, 0);

    sprintf(strn, "\nRooms have been renumbered (starting at %u).\n\n",
            roomHead->roomNumber);
    _outtext(strn);

    return;
  }

  if (!strnumer(args))
  {
    _outtext(
"\nThe 'renumberroom' command's first argument must be a positive number.\n\n");
    return;
  }

  usernumb = atoi(args);

  if (roomHead)
  {
    tempstart = getHighestRoomNumber() + 1;

    if (((usernumb + numbRooms) >= numbLookupEntries) ||
        ((tempstart + numbRooms) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber rooms due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = roomHead->roomNumber;
      roomHead->roomNumber = tempstart;
      renumberRooms(TRUE, oldNumb);

      oldNumb = roomHead->roomNumber;
      roomHead->roomNumber = usernumb;
      renumberRooms(TRUE, oldNumb);
    }
  }
  else
  {
    _outtext("\nThere are no rooms to renumber.\n\n");
    return;
  }

  sprintf(strn,
"\nRooms renumbered starting at %u.\n\n", usernumb);

  _outtext(strn);
}
