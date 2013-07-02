//
//  File: editroom.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing rooms
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../fh.h"
#include "../types.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "room.h"
#include "../misc/menu.h"
#include "../keys.h"

extern dikuRoom **roomLookup, *currentRoom;
extern char madeChanges;
extern ulong lowestRoomNumber, highestRoomNumber, numbRooms, numbLookupEntries;

//
// editRoomDesc : Edit room description function - this function is very
//                complex, by the way
//
//   *room : room to edit description of
//

void editRoomDesc(dikuRoom *room, const char updateChanges)
{
  stringNode *stringCopy;

  if (updateChanges) stringCopy = copyStringNodes(room->roomDescHead);

  room->roomDescHead = editStringNodes(room->roomDescHead, TRUE);

  if (updateChanges)
  {
    if (!compareStringNodes(room->roomDescHead, stringCopy))
    {
      madeChanges = TRUE;
    }

    deleteStringNodes(stringCopy);
  }
}


//
// displayEditRoomMenu : Displays edit room menu
//
//   *room : room being edited
//

void displayEditRoomMenu(const dikuRoom *room)
{
  char strn[256], newName[256], *outStrn = new char[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomName(room->roomName, newName);

  sprintf(strn, "&n&+gEditing room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          room->roomNumber, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit room name&n\n"
  "   &+YB&+L.&n &+wEdit room description &+c(exists: %s)&n\n"
  "   &+YC&+L.&n &+wEdit room flags &+c(%u)&n\n"
  "   &+YD&+L.&n &+wEdit room sector type &+c(\"%s\" [%u])&n\n"
  "   &+YE&+L.&n &+wEdit room extra descriptions &+c(%u)&n\n"
  "   &+YF&+L.&n &+wEdit room exits &+c(%s)&n\n"
  "   &+YG&+L.&n &+wEdit fall chance &+c(%u)&n, mana &+c(%s %d)&n, and current &+c(%s %u)&n\n"
  "\n"
  "   &+YV&+L.&n &+wChange room's vnum&n\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getYesNoStrn((void *)(room->roomDescHead)),

        room->roomFlags.longIntFlags,

        getRoomSectorStrn(room->sectorType, FALSE, FALSE, FALSE, newName),
        room->sectorType,

        getNumbExtraDescs(room->extraDescHead),

        getRoomExitsShortStrn(room, strn),

        room->fallChance, getManaFlagStrn(room->manaFlag), room->manaApply,
        getExitStrn(room->currentDir), room->current,

        getMenuPromptName());
  }
  else
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit room name&n\n"
  "   &+YB&+L.&n &+wEdit room description&n\n"
  "   &+YC&+L.&n &+wEdit room flags&n\n"
  "   &+YD&+L.&n &+wEdit room sector type&n\n"
  "   &+YE&+L.&n &+wEdit room extra descriptions&n\n"
  "   &+YF&+L.&n &+wEdit room exits&n\n"
  "   &+YG&+L.&n &+wEdit fall chance, mana, and current&n\n"
  "\n"
  "   &+YV&+L.&n &+wChange room's vnum&n\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getMenuPromptName());
  }

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditRoomMenu : Interprets user input for edit room menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//  *room : room to edit
//

char interpEditRoomMenu(const usint ch, dikuRoom *room, dikuRoom *origRoom)
{
  struct rccoord coords;
  char strn[256], newName[256], numbStrn[32];
  ulong i;


 // edit room name

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew room name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(room->roomName, MAX_ROOMNAME_LEN - 1 - 1 - 1, 1, 7, '°',
            room->roomName, FALSE, FALSE);

    remTrailingSpaces(room->roomName);

    _settextposition(1, 1);

    clrline(1, 0, 0);
    fixRoomName(room->roomName, newName);

    _setbkcolor(0);
    sprintf(strn, "&+gEditing room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            room->roomNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // edit room description

  if (ch == 'B')
  {
    editRoomDesc(room, FALSE);

    displayEditRoomMenu(room);
  }
  else

 // edit room flags

  if (ch == 'C')
  {
    editRoomFlags(room);

    displayEditRoomMenu(room);
  }
  else

 // edit room sector

  if (ch == 'D')
  {
    editRoomSector(room);

    displayEditRoomMenu(room);
  }
  else

 // edit room extra descs

  if (ch == 'E')
  {
    editRoomExtraDesc(room);

    displayEditRoomMenu(room);
  }
  else

 // edit room exits

  if (ch == 'F')
  {
    editRoomExits(room);

    displayEditRoomMenu(room);
  }
  else

 // edit room misc. stuff

  if (ch == 'G')
  {
    editRoomMisc(room);

    displayEditRoomMenu(room);
  }
  else

 // change vnum (don't allow them to try to change vnum if this is default
 // room)

  if ((ch == 'V') && !room->defaultRoom)
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    sprintf(strn, "&+YNew room vnum (highest allowed %u): &n",
            numbLookupEntries - 1);
    displayColorString(strn);

    itoa(room->roomNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if ((!strnumer(strn)) || (atoi(strn) >= numbLookupEntries) ||
        (findRoom(atoi(strn))))
    {
      clrline(coords.row, 7, 0);
      _settextposition(coords.row, 1);
      _setbkcolor(0);

      displayColorString(getMenuPromptName());

      return FALSE;
    }

    i = atoi(strn);

    resetExits(room->roomNumber, i);
    checkAndFixRefstoRoom(room->roomNumber, i);

    roomLookup[i] = origRoom;
    roomLookup[room->roomNumber] = NULL;

    room->roomNumber = i;

    _settextposition(1, 1);

    clrline(1, 0, 0);
    fixRoomName(room->roomName, newName);

    _setbkcolor(0);
    sprintf(strn, "&n&+gEditing room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            room->roomNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editRoom : "main" function for edit room menu
//
//  *room : room being edited
//

dikuRoom *editRoom(dikuRoom *room, const char allowJump)
{
  usint ch;
  dikuRoom *newRoom;
  char done = FALSE;
  ulong numb, oldVnum;


  if (!room) return NULL;  // ya never know

  oldVnum = room->roomNumber;

 // copy room into newRoom and display the menu

  newRoom = copyRoomInfo(room, FALSE, FALSE);

  displayEditRoomMenu(newRoom);

  while (TRUE)
  {
    ch = toupper(getkey());

#ifndef __UNIX__
    if (ch == '$')
    {
      displayFreeStack(1, 1, FALSE);
    }
#endif

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {

     // if user changed vnum and aborted, set it back

      if (newRoom->roomNumber != oldVnum)
      {
        resetExits(newRoom->roomNumber, oldVnum);
        checkAndFixRefstoRoom(newRoom->roomNumber, oldVnum);

        roomLookup[oldVnum] = room;
        roomLookup[newRoom->roomNumber] = NULL;
      }

      deleteRoomInfo(newRoom, FALSE, FALSE, FALSE);

      _outtext("\n\n");

      return NULL;
    }

    if (allowJump)
    {
      if ((checkMenuKey(ch, FALSE) == MENUKEY_NEXT) &&
          (room->roomNumber != getHighestRoomNumber()))
      {
        done = TRUE;
      }

      if ((checkMenuKey(ch, FALSE) == MENUKEY_PREV) &&
          (room->roomNumber != getLowestRoomNumber()))
      {
        done = TRUE;
      }

      if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP) done = TRUE;
    }

   // if interpEditRoomMenu is TRUE, user has quit

    if (!done) done = interpEditRoomMenu(ch, newRoom, room);

    if (done)
    {
      if (!compareRoomInfo(room, newRoom))   // changes have been made
      {
        deleteRoomAssocLists(room, FALSE, FALSE);

        memcpy(room, newRoom, sizeof(dikuRoom));

        delete newRoom;

        numb = room->roomNumber;

       // don't reset crap if this is the default room

        if (!room->defaultRoom)
        {
          if (numb < getLowestRoomNumber()) lowestRoomNumber = numb;
          if (numb > getHighestRoomNumber()) highestRoomNumber = numb;

          if (numbRooms == 1) lowestRoomNumber = highestRoomNumber = numb;
        }

        deleteMasterKeywordList(room->masterListHead);
        room->masterListHead = createMasterKeywordList(room);

        deleteEditableList(room->editableListHead);
        room->editableListHead = createEditableList(room);

        madeChanges = TRUE;
      }
      else deleteRoomInfo(newRoom, FALSE, FALSE, FALSE);

      if (checkMenuKey(ch, FALSE) != MENUKEY_JUMP) _outtext("\n\n");

      if (allowJump)
      {
        if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP)
        {
          switch (jumpRoom(room, &numb))
          {
            case MENU_JUMP_ERROR : return room;
            case MENU_JUMP_VALID : return findRoom(numb);
            default : _outtext("ERR in editRoom()\n");
                      getkey();
                      return NULL;
          }
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_NEXT)
        {
          return getNextRoom(room->roomNumber);
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_PREV)
        {
          return getPrevRoom(room->roomNumber);
        }
      }

      return NULL;
    }
  }
}


//
// preEditRoom
//

void preEditRoom(const char *args)
{
  long i;
  char strn[256];
  dikuRoom *room;

  if (!strlen(args)) room = currentRoom;
  else
  {
    i = atoi(args);
    room = findRoom(i);

    if (!room)
    {
      sprintf(strn, "\nCouldn't find room number %d.\n\n", i);
      _outtext(strn);

      return;
    }
  }

  do
  {
    room = editRoom(room, TRUE);
  } while (room);
}


char jumpRoom(const dikuRoom *room, ulong *numb)
{
  struct rccoord coords;
  char strn[128], numbStrn[64];
  ulong numb2;


  coords = _gettextposition();
//  coords.row -= 2;

  _settextposition(coords.row, 1);

  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn, "&+CJump to which room (%u-%u, ? for list, $ to search): &n",
            getLowestRoomNumber(), getHighestRoomNumber());
    displayColorString(strn);

    itoa(room->roomNumber, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strcmp(strn, "?"))
    {
      displayRoomList();
    }
    else
    if (!strcmp(strn, "$"))
    {
      strn[0] = 0;

      _setbkcolor(0);
      displayColorString("\n&+CEnter substring to search room list for: &n");
      getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
      upstrn(strn);

      displayRoomList(strn);
    }
    else
    {
      numb2 = strtoul(strn, NULL, 10);

      if (!strnumer(strn) || (numb2 >= numbLookupEntries) ||
          !findRoom(numb2))
      {
        clrline(coords.row, 7, 0);
        _settextposition(coords.row, 1);
        _setbkcolor(0);

        displayColorString(getMenuPromptName());

        return MENU_JUMP_ERROR;
      }
      else
      {
        *numb = numb2;
        return MENU_JUMP_VALID;
      }
    }
  }
}
