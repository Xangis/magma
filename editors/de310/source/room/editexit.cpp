//
//  File: editexit.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing exits
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "../misc/menu.h"
#include "../misc/flags.h"
#include "../dispflag.h"
#include "editexit.h"
#include "room.h"
#include "exit.h"

extern char madeChanges;
extern _CONST char *exitnames[];


//
// displayEditExitStateMenu : Displays the menu that allows you to edit exit
//                            state.
//
//      *room : room that contains the exit
//  *exitNode : exit node being edited
//  *exitName : "name" of exit - direction it leads
//

void displayEditExitStateMenu(const dikuRoom *room, const roomExit *exitNode,
                              const char *exitName)
{
  char strn[256], newName[256];
  struct rccoord coords;
  long i;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomNameShort(room->roomName, newName);

  sprintf(strn, "&+gEditing exit state for %s exit in &+L\"&n%s&+L\""
                "\n\n",
          exitName, newName);

  displayColorString(strn);

 // only allow user to make door closed and locked if

  displayColorString(
  "   &+YA&+L.&n &+c(.WLD) &+wSet door type to \"no door\"&n\n"
  "   &+YB&+L.&n &+c(.WLD) &+wSet door type to \"door (no key)\"&n\n"
  "   &+YC&+L.&n &+c(.WLD) &+wSet door type to \"door (req key)\"&n\n"
  "   &+YD&+L.&n &+c(.WLD) &+wSet door type to \"unpickable door (req key)\"&n\n"
  "\n"
  "   &+YE&+L.&n &+c(.WLD) &+wToggle secret door bit&n\n"
  "   &+YF&+L.&n &+c(.WLD) &+wToggle blocked door bit&n\n"
  "\n"
  "   &+YG&+L.&n &+c(.ZON) &+wSet door state to \"open\"&n\n"
  "   &+YH&+L.&n &+c(.ZON) &+wSet door state to \"closed\"&n\n"
  "   &+YI&+L.&n &+c(.ZON) &+wSet door state to \"closed and locked\"&n\n"
  "\n"
  "   &+YJ&+L.&n &+c(.ZON) &+wToggle secret state bit&n\n"
  "   &+YK&+L.&n &+c(.ZON) &+wToggle blocked state bit&n\n"
  "\n"
  MENU_COMMON
  "\n");

  displayColorString(getMenuPromptName());

  coords = _gettextposition();

 // check world door type

  for (i = 3; i >= 0; i--)
  {
    if ((exitNode->worldDoorType & i) == i)
    {
      _settextposition(WORLD_FLAG_STARTY + i, WORLD_FLAGX);

      displayColorString(FLAG_ACTIVE_STR);

      break;
    }
  }

 // check zone door state

  for (i = 2; i >= 0; i--)
  {
    if ((exitNode->zoneDoorState & i) == i)
    {
      _settextposition(ZONE_FLAG_STARTY + i, ZONE_FLAGX);

      displayColorString(FLAG_ACTIVE_STR);

      break;
    }
  }

 // next, the wacky-ass bits for the world door type

  if (exitNode->worldDoorType & 4)  // secret state
  {
    _settextposition(WORLD_FLAG_STARTY + 5, WORLD_FLAGX);

    displayColorString(FLAG_ACTIVE_STR);
  }

  if (exitNode->worldDoorType & 8)  // blocked state
  {
    _settextposition(WORLD_FLAG_STARTY + 6, WORLD_FLAGX);

    displayColorString(FLAG_ACTIVE_STR);
  }

 // next, the wacky-ass bits for the zone door state

  if (exitNode->zoneDoorState & 4)  // secret state
  {
    _settextposition(ZONE_FLAG_STARTY + 4, ZONE_FLAGX);

    displayColorString(FLAG_ACTIVE_STR);
  }

  if (exitNode->zoneDoorState & 8)  // blocked state
  {
    _settextposition(ZONE_FLAG_STARTY + 5, ZONE_FLAGX);

    displayColorString(FLAG_ACTIVE_STR);
  }

  _settextposition(coords.row, coords.col);
}


//
// interpEditExitStateMenu : Interprets user input - returns TRUE if user hit
//                           'Q', FALSE otherwise
//
//         ch : user input
//      *room : room that contains the exit
//  *exitNode : exit node being edited
//  *exitName : "name" of exit
//

char interpEditExitStateMenu(const char ch, roomExit *exitNode)
{
  struct rccoord coords = _gettextposition();

  uchar oldWorldDoorBits = (exitNode->worldDoorType & 12),
        oldWorldDoorType = (exitNode->worldDoorType & 3),
        oldZoneDoorBits = (exitNode->zoneDoorState & 12),
        oldZoneDoorType = (exitNode->zoneDoorState & 3);


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // world state crap

 // set exit state to open

  if (ch == 'A')
  {
    exitNode->worldDoorType = 0 | oldWorldDoorBits;

    _settextposition(WORLD_FLAG_STARTY + oldWorldDoorType, WORLD_FLAGX);
    _outtext(" ");

    _settextposition(WORLD_FLAG_STARTY + 0, WORLD_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // set exit state to closed

  if (ch == 'B')
  {
    exitNode->worldDoorType = 1 | oldWorldDoorBits;

    _settextposition(WORLD_FLAG_STARTY + oldWorldDoorType, WORLD_FLAGX);
    _outtext(" ");

    _settextposition(WORLD_FLAG_STARTY + 1, WORLD_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // set exit state to closed and locked

  if (ch == 'C')
  {
    exitNode->worldDoorType = 2 | oldWorldDoorBits;

    _settextposition(WORLD_FLAG_STARTY + oldWorldDoorType, WORLD_FLAGX);
    _outtext(" ");

    _settextposition(WORLD_FLAG_STARTY + 2, WORLD_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // set exit state to closed and locked, unpickable

  if (ch == 'D')
  {
    exitNode->worldDoorType = 3 | oldWorldDoorBits;

    _settextposition(WORLD_FLAG_STARTY + oldWorldDoorType, WORLD_FLAGX);
    _outtext(" ");

    _settextposition(WORLD_FLAG_STARTY + 3, WORLD_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // toggle secret bit

  if (ch == 'E')
  {
    exitNode->worldDoorType ^= 4;

    _settextposition(WORLD_FLAG_STARTY + 5, WORLD_FLAGX);

    if (exitNode->worldDoorType & 4)
    {
      displayColorString(FLAG_ACTIVE_STR);
    }
    else
    {
      _outtext(" ");
    }

    _settextposition(coords.row, coords.col);
  }
  else

 // toggle blocked bit

  if (ch == 'F')
  {
    exitNode->worldDoorType ^= 8;

    _settextposition(WORLD_FLAG_STARTY + 6, WORLD_FLAGX);

    if (exitNode->worldDoorType & 8)
    {
      displayColorString(FLAG_ACTIVE_STR);
    }
    else
    {
      _outtext(" ");
    }

    _settextposition(coords.row, coords.col);
  }
  else

 // zone state crap

 // set exit state to open

  if (ch == 'G')
  {
    exitNode->zoneDoorState = 0 | oldZoneDoorBits;

    _settextposition(ZONE_FLAG_STARTY + oldZoneDoorType, ZONE_FLAGX);
    _outtext(" ");

    _settextposition(ZONE_FLAG_STARTY + 0, ZONE_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // set exit state to closed

  if (ch == 'H')
  {
    exitNode->zoneDoorState = 1 | oldZoneDoorBits;

    _settextposition(ZONE_FLAG_STARTY + oldZoneDoorType, ZONE_FLAGX);
    _outtext(" ");

    _settextposition(ZONE_FLAG_STARTY + 1, ZONE_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // set exit state to closed and locked

  if (ch == 'I')
  {
    exitNode->zoneDoorState = 2 | oldZoneDoorBits;

    _settextposition(ZONE_FLAG_STARTY + oldZoneDoorType, ZONE_FLAGX);
    _outtext(" ");

    _settextposition(ZONE_FLAG_STARTY + 2, ZONE_FLAGX);
    displayColorString(FLAG_ACTIVE_STR);

    _settextposition(coords.row, coords.col);
  }
  else

 // toggle secret bit

  if (ch == 'J')
  {
    exitNode->zoneDoorState ^= 4;

    _settextposition(ZONE_FLAG_STARTY + 4, ZONE_FLAGX);

    if (exitNode->zoneDoorState & 4)
    {
      displayColorString(FLAG_ACTIVE_STR);
    }
    else
    {
      _outtext(" ");
    }

    _settextposition(coords.row, coords.col);
  }
  else

 // toggle blocked bit

  if (ch == 'K')
  {
    exitNode->zoneDoorState ^= 8;

    _settextposition(ZONE_FLAG_STARTY + 5, ZONE_FLAGX);

    if (exitNode->zoneDoorState & 8)
    {
      displayColorString(FLAG_ACTIVE_STR);
    }
    else
    {
      _outtext(" ");
    }

    _settextposition(coords.row, coords.col);
  }


  return FALSE;
}


//
// editExitState : "Main" function for the "edit exit state" menu.
//
//      *room : room that contains exit being edited
//  *exitNode : exit being edited
//  *exitName : exit "name" - direction exit leads
//

void editExitState(const dikuRoom *room, roomExit *exitNode,
                   const char *exitName)
{
  usint ch;
  uchar oldWorldDoorType = exitNode->worldDoorType,
        oldZoneDoorState = exitNode->zoneDoorState;


  displayEditExitStateMenu(room, exitNode, exitName);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      exitNode->worldDoorType = oldWorldDoorType;
      exitNode->zoneDoorState = oldZoneDoorState;

      _outtext("\n\n");

      return;
    }

    if (interpEditExitStateMenu(ch, exitNode))
    {
      return;
    }
  }
}


//
// displayEditExitMenu : Displays the edit exit menu
//
//       *room : pointer to room that contains the exit
//   *exitName : direction exit is heading in a nice ASCII string
//

void displayEditExitMenu(const dikuRoom *room, const char *exitName,
                         const roomExit *exitNode)
{
  char strn[256], newName[256], *outStrn = new char[4096];


 // clear the screen, set colors..

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomNameShort(room->roomName, newName);

 // display title

  sprintf(strn,
          "&+gEditing %s exit for room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          exitName, room->roomNumber, newName);

  displayColorString(strn);


 // display the rest of the stuff

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit exit keywords &+c(%s)&n\n"
  "   &+YB&+L.&n &+wEdit exit description &+c(exists: %s)&n\n"
  "   &+YC&+L.&n &+wEdit door type and state &+c(%u %u)&n\n"
  "   &+YD&+L.&n &+wEdit key object number &+c(%d)&n\n"
  "   &+YE&+L.&n &+wEdit destination room &+c(%d)&n\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getReadableKeywordStrn(exitNode->keywordListHead, strn),
        getYesNoStrn((void *)(exitNode->exitDescHead)),
        exitNode->worldDoorType, exitNode->zoneDoorState,
        exitNode->keyNumb,
        exitNode->destRoom,
        getMenuPromptName());
  }
  else
  {
    strcpy(outStrn,
  "   &+YA&+L.&n &+wEdit exit keywords\n"
  "   &+YB&+L.&n &+wEdit exit description\n"
  "   &+YC&+L.&n &+wEdit door type and state\n"
  "   &+YD&+L.&n &+wEdit key object number\n"
  "   &+YE&+L.&n &+wEdit destination room\n"
  "\n"
  MENU_COMMON
  "\n");

    strcat(outStrn, getMenuPromptName());
  }

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditExitMenu : Interprets user input - returns TRUE if user hit 'Q',
//                      FALSE otherwise
//
//         ch : user input
//      *room : room that contains the exit
//  *exitNode : exit node being edited
//  *exitName : "name" of exit
//

char interpEditExitMenu(const char ch, const dikuRoom *room, roomExit *exitNode,
                        const char *exitName)
{
  struct rccoord coords;
  char strn[256] = "\0";


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // edit exit keywords

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    displayColorString("&+YNew exit keywords: &n");

    createKeywordString(exitNode->keywordListHead, strn);
    strn[strlen(strn) - 1] = '\0';  // get rid of tilde

    getStrn(strn, MAX_EXITKEY_LEN - 1, 1, 7, '°', strn, FALSE, FALSE);

    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    strcat(strn, "~");

    deleteStringNodes(exitNode->keywordListHead);
    exitNode->keywordListHead = createKeywordList(strn);

    displayEditExitMenu(room, exitName, exitNode);
  }
  else

 // edit exit description

  if (ch == 'B')
  {
    exitNode->exitDescHead = editStringNodes(exitNode->exitDescHead, TRUE);

    displayEditExitMenu(room, exitName, exitNode);
  }
  else

 // edit exit door state

  if (ch == 'C')
  {
    editExitState(room, exitNode, exitName);

    displayEditExitMenu(room, exitName, exitNode);
  }
  else

 // edit exit key object number

  if (ch == 'D')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+CEnter new key object number for exit (? for list):&n ");

      itoa(exitNode->keyNumb, strn, 10);
      getStrn(strn, 12, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?"))
      {
        displayObjectTypeList();
        coords = _gettextposition();
      }
      else
      {
        if ((findObj(atoi(strn)) && getVnumCheckVal()) || (atoi(strn) == -1) ||
            (atoi(strn) == -2) || (atoi(strn) == 0) || (!getVnumCheckVal()))
        {
          exitNode->keyNumb = atoi(strn);
          displayEditExitMenu(room, exitName, exitNode);
          break;
        }
      }
    }
  }
  else

 // edit exit destination room

  if (ch == 'E')
  {
    coords = _gettextposition();

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+CEnter new destination room number for exit (? for list):&n ");

      itoa(exitNode->destRoom, strn, 10);
      getStrn(strn, 12, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?"))
      {
        displayRoomList();
        coords = _gettextposition();
      }
      else
      {
        if (getVnumCheckVal())
        {
          if ((findRoom(atoi(strn))) || (atoi(strn) == -1))
          {
            exitNode->destRoom = atoi(strn);
            displayEditExitMenu(room, exitName, exitNode);

            break;
          }
        }
        else
        {
          exitNode->destRoom = atoi(strn);
          displayEditExitMenu(room, exitName, exitNode);

          break;
        }
      }
    }
  }


  return FALSE;
}


//
// editExit : The "main" function for the edit exit menu
//
//       *room : room that contains exit being edited
//  **exitNode : pointer to a pointer to the exit being edited
//   *exitName : exit "name" - one of the cardinal directions
//

void editExit(dikuRoom *room, roomExit **exitNode, const char *exitName,
              const uchar updateMadeChanges)
{
  uchar ch;
  roomExit *newExit;


  if (!(*exitNode))
  {
    _outtext("\nNo exit to edit!\n\n");

    return;
  }

  newExit = copyRoomExit(*exitNode, FALSE);

  displayEditExitMenu(room, exitName, *exitNode);


  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteRoomExit(newExit, FALSE);

      _outtext("\n\n");

      return;
    }

    if (interpEditExitMenu(ch, room, newExit, exitName))
    {
      if (!compareRoomExits(*exitNode, newExit))
      {
        deleteRoomExit(*exitNode, FALSE);
        *exitNode = newExit;

        if (updateMadeChanges) madeChanges = TRUE;
      }
      else deleteRoomExit(newExit, FALSE);

      _outtext("\n\n");

      return;
    }
  }
}


//
// preEditExit
//

void preEditExit(const char *args, dikuRoom *currentRoom)
{
  int val;

  if (!strlen(args)) editRoomExits(currentRoom);
  else
  {
    val = getDirfromKeyword(args);
    if (val == NO_EXIT)
    {
      _outtext("\nEdit which exit?\n\n");
      return;
    }

    editExit(currentRoom, &currentRoom->exits[val], exitnames[val], TRUE);
  }
}
