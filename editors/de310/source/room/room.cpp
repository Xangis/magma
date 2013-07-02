//
//  File: room.cpp       originally part of dikuEdit
//
//  Usage: a plethora of functions relating to rooms
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"
#include "room.h"        // header for room stuff

using namespace std;

extern dikuRoom **roomLookup, *roomHead, *currentRoom;
extern ulong numbLookupEntries, numbRooms, lowestRoomNumber, highestRoomNumber;
extern dikuObject *objectHead;
extern dikuMob *mobHead;
extern char madeChanges;
extern _CONST char revdirs[];
extern _CONST char *exitnames[];

//
// findRoom : Returns the address of the room node that has the requested
//            roomNumber (if any)
//
//   roomNumber : room number to look for in the dikuRoom linked list
//

INLINEDEF dikuRoom *findRoom(const register ulong roomNumber)
{
  if (roomNumber >= numbLookupEntries) return NULL;

  return roomLookup[roomNumber];
}


//
// goDirection : "Goes" in a direction by changing currentRoom to the room
//               pointed to by a specific exit record.
//
//   *exitRec : exit to "go" through
//  direction : direction user is going - used for 'walkcreate' mode
//

void goDirection(const roomExit *exitRec, const ulong direction)
{
  dikuRoom *room, *roomPtr = roomHead;
  roomExit *fromExit, *toExit;
  char strn[1024];


 // exit exists

  if (exitRec)
  {
    if (exitRec->destRoom == -1)
    {
      _outtext("\n"
"Sorry, you can't go that direction (exit dest. is -1 [nowhere]).\n\n");

      return;
    }

    if (findRoom(exitRec->destRoom))
    {
      currentRoom = findRoom(exitRec->destRoom);

      displayRoom(currentRoom);
    }
    else _outtext("\n"
"Sorry, you can't go that direction (room pointed to by exit doesn't exist)."
"\n\n");
  }
  else  // no exit in this direction
  {
    if (getWalkCreateVal())
    {
      room = createRoom(TRUE, -1);
      if (!room)
      {
        sprintf(strn, "\n"
"Walkcreate mode is enabled, but couldn't create a room in that direction -\n"
"likely reason is that you've hit the max room vnum supported by the editor\n"
"(up to %u).  To remedy, use the -ll=<limit> commandline switch to increase\n"
"the limit.\n\n",
                numbLookupEntries);

        _outtext(strn);

        return;
      }

     // stick the room in the room list

      if (!roomPtr) roomHead = room;  // this shouldn't ever happen, by the way
      else
      while (roomPtr->Next)
      {
        roomPtr = roomPtr->Next;
      }

      roomPtr->Next = room;
      room->Last = roomPtr;
      room->Next = NULL;

      createExit(&fromExit, TRUE);
      fromExit->destRoom = room->roomNumber;

      createExit(&toExit, TRUE);
      toExit->destRoom = currentRoom->roomNumber;

      currentRoom->exits[direction] = fromExit;
      room->exits[revdirs[direction]] = toExit;

     // create master and editable lists

      room->masterListHead = createMasterKeywordList(room);
      room->editableListHead = createEditableList(room);

      displayColorString("\n"
"&+cNew room created through the magic of 'create as you walk' mode.&n\n");

      goDirection(fromExit, direction);
    }
    else
    {
      _outtext("\nSorry, you can't go that direction (no exit).\n\n");
    }
  }
}


//
// getSectorTypeStrn : returns a string that details what the sector type
//                     number "is"
//
//    sectorType : sector type being checked
//         *strn : string to place name into
//

/*
char *getSectorTypeStrn(uchar sectorType, char *strn)
{
  if (sectorType > SECT_HIGHEST)
  {
    strcpy(strn, "\0");

    return strn;
  }

  switch (sectorType)
  {
    case SECT_INSIDE : strcpy(strn, "Inside");  return strn;
    case SECT_CITY : strcpy(strn, "City");  return strn;
    case SECT_FIELD : strcpy(strn, "Field");  return strn;
    case SECT_FOREST : strcpy(strn, "Forest");  return strn;
    case SECT_HILLS : strcpy(strn, "Hills");  return strn;
    case SECT_MOUNTAIN : strcpy(strn, "Mountain");  return strn;
    case SECT_WATER_SWIM : strcpy(strn, "Water - swimmable");  return strn;
    case SECT_WATER_NOSWIM : strcpy(strn, "Water - need boat");  return strn;
    case SECT_NO_GROUND : strcpy(strn, "No ground");  return strn;
    case SECT_UNDERWATER : strcpy(strn, "Underwater");  return strn;
    case SECT_UNDERWATER_GR : strcpy(strn, "Underwater - ground");  return strn;
    case SECT_FIREPLANE : strcpy(strn, "Fireplane");  return strn;
    case SECT_OCEAN : strcpy(strn, "Ocean");  return strn;
    case SECT_UNDRWLD_WILD : strcpy(strn, "Underworld - wild");  return strn;
    case SECT_UNDRWLD_CITY : strcpy(strn, "Underworld - city");  return strn;
    case SECT_UNDRWLD_INSIDE : strcpy(strn, "Underworld - inside");  return strn;
    case SECT_UNDRWLD_WATER : strcpy(strn, "Underworld - water");  return strn;
    case SECT_UNDRWLD_NOSWIM : strcpy(strn, "Underworld - need boat");  return strn;
    case SECT_UNDRWLD_NOGROUND : strcpy(strn, "Underworld - no ground");  return strn;
  }

  strcpy(strn, "\0");
  return strn;
}
*/


//
// copyRoomInfo : copies all the info from a room record into a new record and
//                returns the address of the new record
//
//     *srcRoom : source room record
//    incLoaded : if TRUE, increments internal counter
//  copyMobInfo : if FALSE, doesn't create a copy mob list, instead using
//                old room's mob list pointer (used somewhere because it
//                made life much easier..  aren't hacks fun?)
//

dikuRoom *copyRoomInfo(const dikuRoom *srcRoom, const char incLoaded,
                       const char copyMobInfo)
{
  dikuRoom *newRoom;
  int i;

 // make sure src exists

  if (!srcRoom) return NULL;

 // alloc mem for new rec

  newRoom = new dikuRoom;
  if (!newRoom)
  {
    cout << "\n\nError creating new dikuRoom in copyRoomInfo().  "
            "Out of memory.\n\n";

    return NULL;
  }

//  newRoom->Next = NULL;

 // first, copy the simple stuff

  memcpy(newRoom, srcRoom, sizeof(dikuRoom));

 // next, the not-so-simple stuff

  newRoom->roomDescHead = copyStringNodes(srcRoom->roomDescHead);

 // room exits..

  for (i = 0; i < NUMB_EXITS; i++)
  {
    newRoom->exits[i] = copyRoomExit(srcRoom->exits[i], incLoaded);
  }

 // extra desc linked list

  newRoom->extraDescHead = copyExtraDescs(srcRoom->extraDescHead);

 // object list

  newRoom->objectHead = copyObjHereList(srcRoom->objectHead, incLoaded);

 // mob list

  if (copyMobInfo) newRoom->mobHead = copyMobHereList(srcRoom->mobHead, incLoaded);
  else newRoom->mobHead = srcRoom->mobHead;
      // this should increment loaded shit

 // master list

//  newRoom->masterListHead = copyMasterKeywordList(srcRoom->masterListHead);
  newRoom->masterListHead = createMasterKeywordList(newRoom);

 // editable list

//  newRoom->editableListHead = copyEditableList(srcRoom->editableListHead);
  newRoom->editableListHead = createEditableList(newRoom);


  if (incLoaded)
  {
    numbRooms++;
    createPrompt();
  }

 // return the address of the new room

  return newRoom;
}


//
// compareRoomInfo : compares it all, baby - returns FALSE if they don't
//                   match, TRUE if they do
//
//      in theory, this function should return different results based on
//      what matched - save that for later
//

uchar compareRoomInfo(dikuRoom *room1, dikuRoom *room2)
{
  int i;


  if (room1 == room2) return TRUE;
  if (!room1 || !room2) return FALSE;

 // check room names etc blah blah

//  _outtext("\nComparing room names and other sundry crap..\n");

  if (strcmp(room1->roomName, room2->roomName)) return FALSE;
  if (room1->roomNumber != room2->roomNumber) return FALSE;
  if (room1->zoneNumber != room2->zoneNumber) return FALSE;
  if (room1->roomFlags.longIntFlags
                        != room2->roomFlags.longIntFlags) return FALSE;
  if (room1->sectorType != room2->sectorType) return FALSE;
  if (room1->fallChance != room2->fallChance) return FALSE;
  if (room1->manaFlag   != room2->manaFlag) return FALSE;
  if (room1->manaApply  != room2->manaApply) return FALSE;
//  if (room1->minLevel != room2->minLevel) return FALSE;
//  if (room1->maxLevel != room2->maxLevel) return FALSE;
  if (room1->current    != room2->current) return FALSE;
  if (room1->currentDir != room2->currentDir) return FALSE;

 // description and extra descs

  if (!compareStringNodes(room1->roomDescHead, room2->roomDescHead))
    return FALSE;

  if (!compareExtraDescs(room1->extraDescHead, room2->extraDescHead))
    return FALSE;

 // compare exits

  for (i = 0; i < NUMB_EXITS; i++)
  {
    if (!compareRoomExits(room1->exits[i], room2->exits[i])) return FALSE;
  }

  return TRUE;
}


//
// deleteRoomInfo : deletes all the associated baggage with a room rec
//
//     *srcRoom : record to delete
//

void deleteRoomInfo(dikuRoom *srcRoom, const char decNumbRooms,
                    const char decLoaded, const char deleteMobs)
{
  ulong numb, i;

 // make sure src exists

  if (!srcRoom) return;

  numb = srcRoom->roomNumber;

 // delete room desc

  deleteStringNodes(srcRoom->roomDescHead);

 // room exits..

  for (i = 0; i < NUMB_EXITS; i++)
  {
    deleteRoomExit(srcRoom->exits[i], decLoaded);
  }

 // extra desc linked list

  deleteExtraDescs(srcRoom->extraDescHead);

 // object list

  deleteObjHereList(srcRoom->objectHead, decLoaded);

 // mob list

  if (deleteMobs) deleteMobHereList(srcRoom->mobHead, decLoaded);

 // master list

  deleteMasterKeywordList(srcRoom->masterListHead);

 // editable list

  deleteEditableList(srcRoom->editableListHead);

 // finally, delete the record itself

  delete srcRoom;

  if (decNumbRooms)
  {
    numbRooms--;
    roomLookup[numb] = NULL;
    createPrompt();
  }
}


//
// deleteRoomAssocLists : deletes all the associated baggage with a room rec,
//                        but not the record itself
//
//     *srcRoom : record to alter
//

void deleteRoomAssocLists(dikuRoom *srcRoom, const char decLoaded,
                          const char deleteMobs)
{

 // make sure src exists

  if (!srcRoom) return;

 // delete room desc

  deleteStringNodes(srcRoom->roomDescHead);
  srcRoom->roomDescHead = NULL;

 // room exits.. - not yet - if ever

//  deleteRoomExit(srcRoom->north);
//  deleteRoomExit(srcRoom->south);
//  deleteRoomExit(srcRoom->west);
//  deleteRoomExit(srcRoom->east);
//  deleteRoomExit(srcRoom->up);
//  deleteRoomExit(srcRoom->down);

 // extra desc linked list

  deleteExtraDescs(srcRoom->extraDescHead);
  srcRoom->extraDescHead = NULL;

 // object list

  deleteObjHereList(srcRoom->objectHead, decLoaded);
  srcRoom->objectHead = NULL;

 // mob list

  if (deleteMobs)
  {
    deleteMobHereList(srcRoom->mobHead, decLoaded);
    srcRoom->mobHead = NULL;
  }

 // master list

  deleteMasterKeywordList(srcRoom->masterListHead);
  srcRoom->masterListHead = NULL;

 // editable list

  deleteEditableList(srcRoom->editableListHead);
  srcRoom->editableListHead = NULL;
}


//
// updateAllRoomExits : Looks for exits that lead to oldRoom and changes them
//                      to newRoom.  Woohoo!
//

/*
void updateAllRoomExits(dikuRoom *roomHead, dikuRoom *oldRoom,
                        dikuRoom *newRoom)
{
  if ((!roomHead) || (!oldRoom) || (!newRoom))
  {
    cout << "\n\nOne of the nodes passed to updateAllRoomExits() is NULL."
            "  Aborting..\n\n";

    return;
  }

  while (roomHead)
  {
    updateRoomExit(roomHead->north, oldRoom, newRoom);
    updateRoomExit(roomHead->south, oldRoom, newRoom);
    updateRoomExit(roomHead->west, oldRoom, newRoom);
    updateRoomExit(roomHead->east, oldRoom, newRoom);
    updateRoomExit(roomHead->up, oldRoom, newRoom);
    updateRoomExit(roomHead->down, oldRoom, newRoom);

    roomHead = roomHead->Next;
  }
}
*/


//
// fixRoomName : Trims *oldName to MAX_ROOMSHORT_LEN, setting *newName with
//               the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixRoomName(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_ROOMSHORT_LEN, !getShowColorVal());
}


//
// fixRoomNameShort : Trims *oldName to MAX_ROOMSHORT_LEN, setting *newName with
//               the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixRoomNameShort(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_ROOMSHORTSHORT_LEN, !getShowColorVal());
}


//
// getHighestRoomNumber : Gets the highest room number currently in the
//                        roomHead list, returning it..
//

INLINEDEF ulong getHighestRoomNumber(void)
{
  return highestRoomNumber;
}


//
// getLowestRoomNumber : Gets the lowest room number currently in the
//                       roomHead list, returning it..
//

INLINEDEF ulong getLowestRoomNumber(void)
{
  return lowestRoomNumber;
}


//
// getFirstFreeRoomNumber : Gets the first free room number currently in the
//                          roomHead list, returning it..
//

INLINEDEF ulong getFirstFreeRoomNumber(void)
{
  register ulong i;

  for (i = lowestRoomNumber + 1; i <= highestRoomNumber - 1; i++)
  {
    if (!roomLookup[i]) return i;
  }

  return highestRoomNumber + 1;
}


//
//
//

/*
char checkAllObjInsideForKeyword(objectHere *objHere, char *strn, usint pos)
{
  objectHere *obj = objHere;

  while (obj)
  {
    if (scanKeyword(strn, obj->keywordListHead, pos)) return TRUE;

    if (obj->objInside)
    {
      if (checkAllObjInsideForKeyword(obj->objInside, strn, pos)) return TRUE;
    }

    obj = obj->Next;
  }
}
*/


//
// checkAndFixRefstoRoom : scans through the object list, looking for
//                         the few object types that specify a room vnum
//                         in one of their values and changing it to
//                         new number if necessary
//
//      oldNumb : old vnum
//      newNumb : new vnum
//

void checkAndFixRefstoRoom(const ulong oldNumb, const ulong newNumb)
{
  ulong i;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;
  questQuest *qst;
  questItem *item;

 // scan through object types

  while (obj)
  {
    for (i = 0; i < NUMB_OBJ_VALS; i++)
    {
      if ((fieldRefsRoomNumb(obj->objType, i) &&
          (obj->objValues[i] == oldNumb)))
      {
        obj->objValues[i] = newNumb;

        madeChanges = TRUE;
      }
    }

    obj = obj->Next;
  }

 // fix quest refs

  while (mob)
  {
    if (mob->questPtr)
    {
      qst = mob->questPtr->questHead;

      while (qst)
      {
        item = qst->questPlayRecvHead;
        while (item)
        {
          if ((item->itemType == QUEST_RITEM_TRANSFERMOBS ||
               item->itemType == QUEST_RITEM_TRANSFERPLR ||
               item->itemType == QUEST_RITEM_TRANSFERGRP ) &&
              (item->itemVal == oldNumb))
          {
            item->itemVal = newNumb;
          }

          item = item->Next;
        }

        qst = qst->Next;
      }
    }

    mob = mob->Next;
  }

}


//
// rebuildRoomLookup : resets the entire roomLookup table to NULL, then
//                     rebuilds it by simply running through the global
//                     linked list of rooms
//

void rebuildRoomLookup(void)
{
  register dikuRoom *room = roomHead;
  register ulong i;

  if (!roomLookup) return;  // whoops

  memset(roomLookup, 0, sizeof(dikuRoom *) * numbLookupEntries);

  lowestRoomNumber = numbLookupEntries;
  highestRoomNumber = 0;

  while (room)
  {
    i = room->roomNumber;

    roomLookup[i] = room;
    if (i < lowestRoomNumber) lowestRoomNumber = i;
    if (i > highestRoomNumber) highestRoomNumber = i;

    room = room->Next;
  }
}


//
// renumberRooms : Renumbers the rooms so that there are no "gaps" - starts
//                 at the first room and simply renumbers like mad from there
//
//   renumberHead : needed to reset where shit is renumbered from (boolean)
//    oldHeadNumb : ditto
//

void renumberRooms(const char renumberHead, const long oldHeadNumb)
{
  dikuRoom *roomNode = roomHead;
  dikuMob *mob = mobHead;
  long lastNumb = roomHead->roomNumber - 1, oldNumb;


  if (renumberHead)
  {
    resetExits(oldHeadNumb, roomNode->roomNumber);
    checkAndFixRefstoRoom(oldHeadNumb, roomNode->roomNumber);

    madeChanges = TRUE;
  }

 /* remove gaps */

  while (roomNode)
  {
    if ((lastNumb + 1) != roomNode->roomNumber)
    {
      oldNumb = roomNode->roomNumber;
      roomNode->roomNumber = lastNumb + 1;

      resetExits(oldNumb, roomNode->roomNumber);
      checkAndFixRefstoRoom(oldNumb, roomNode->roomNumber);

      madeChanges = TRUE;
    }

    lastNumb = roomNode->roomNumber;

    roomNode = roomNode->Next;
  }

  rebuildRoomLookup();
}


//
// updateAllMandElists : updates all rooms' masterKeyword and editable lists
//

/*
void updateAllMandElists(void)
{
  register dikuRoom *room;
  register ulong i = getLowestRoomNumber(), high = getHighestRoomNumber();


  while (i <= high)
  {
    room = roomLookup[i];
    i++;

    if (!room) continue;

    if (room->objectHead || room->mobHead || room->extraDescHead)
    {
      deleteMasterKeywordList(room->masterListHead);
      room->masterListHead = createMasterKeywordList(room);

      deleteEditableList(room->editableListHead);
      room->editableListHead = createEditableList(room);
    }
  }
}
*/


//
// updateAllObjMandElists : updates all rooms' masterKeyword and editable lists
//

void updateAllObjMandElists(void)
{
  register dikuRoom *room;
  register ulong i = getLowestRoomNumber(), high = getHighestRoomNumber();


  while (i <= high)
  {
    room = roomLookup[i];
    i++;

    if (!room) continue;

    if (room->objectHead)
    {
      deleteMasterKeywordList(room->masterListHead);
      room->masterListHead = createMasterKeywordList(room);

      deleteEditableList(room->editableListHead);
      room->editableListHead = createEditableList(room);
    }
  }
}


//
// updateAllMobMandElists : updates all rooms' masterKeyword and editable lists
//

void updateAllMobMandElists(void)
{
  register dikuRoom *room;
  register ulong i = getLowestRoomNumber(), high = getHighestRoomNumber();


  while (i <= high)
  {
    room = roomLookup[i];
    i++;

    if (!room) continue;

    if (room->mobHead)
    {
      deleteMasterKeywordList(room->masterListHead);
      room->masterListHead = createMasterKeywordList(room);

      deleteEditableList(room->editableListHead);
      room->editableListHead = createEditableList(room);
    }
  }
}


//
// noRoomNext
//

/*
char noRoomNext(dikuRoom *room, char *substrn)
{
  char strn[256];

  if (!room) return TRUE;
  room = room->Next;

  while (room)
  {
    strcpy(strn, room->roomName);
    upstrn(strn);

    if (strstr(strn, substrn)) return FALSE;

    room = room->Next;
  }

  return TRUE;
}
*/


//
// noExitsLeadtoRoom
//

INLINEDEF char noExitsLeadtoRoom(const ulong roomNumb)
{
  dikuRoom *room = roomHead;
  int i;

  while (room)
  {
    for (i = 0; i < NUMB_EXITS; i++)
    {
      if (room->exits[i] && (room->exits[i]->destRoom == roomNumb)) return FALSE;
    }

    room = room->Next;
  }

  return TRUE;
}


//
// noExitOut
//

INLINEDEF char noExitOut(const dikuRoom *room)
{
  int i;

  if (!room) return FALSE;

  for (i = 0; i < NUMB_EXITS; i++)
  {
    if (isExitOut(room->exits[i])) return FALSE;
  }

  return TRUE;
}


//
// roomHasAllFollowers : checks all rooms to see if any are filled with
//                       mobs that are all following somebody - no way
//                       to write this out to the zone file, you see -
//                       returns -1 if no room found, room number otherwise
//

long roomHasAllFollowers(void)
{
  dikuRoom *room = roomHead;
  mobHere *mob;
  char allFollow = FALSE;

  while (room)
  {
    if (!room->mobHead)
    {
      room = room->Next;
      continue;
    }

    allFollow = TRUE;

    mob = room->mobHead;
    while (mob)
    {
      if (!mob->following)
      {
        allFollow = FALSE;
        break;
      }

      mob = mob->Next;
    }

    if (allFollow) return room->roomNumber;

    room = room->Next;
  }

  return -1;
}


//
// getRoomKeyword : returns a pointer to the first room that contains
//                  an obj or mob with the specified keyword.  if
//                  checkRooms is set, checks room titles as well.
//

dikuRoom *getRoomKeyword(const char *key, const char checkRooms)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;
  char tempStrn[MAX_ROOMNAME_LEN];
  ulong i;


  if (strnumer(key)) return findRoom(atoi(key));

  while (obj)
  {
    if (scanKeyword(key, obj->keywordListHead))
    {
      if (findObjHere(obj->objNumber, roomHead, &i)) return findRoom(i);
    }

    obj = obj->Next;
  }

  while (mob)
  {
    if (scanKeyword(key, mob->keywordListHead))
    {
      if (findMobHere(mob->mobNumber, roomHead, &i, FALSE)) return findRoom(i);
    }

    mob = mob->Next;
  }

  if (checkRooms)
  {
    while (room)
    {
      strcpy(tempStrn, room->roomName);
      upstrn(tempStrn);
      remColorCodes(tempStrn);

      if (strstr(tempStrn, key)) return room;

      room = room->Next;
    }
  }

  return NULL;
}


//
// getNumbFreeRooms
//

INLINEDEF ulong getNumbFreeRooms(void)
{
  ulong i, numb = 0;

  for (i = lowestRoomNumber; i < numbLookupEntries; i++)
  {
    if (!roomLookup[i]) numb++;
  }

  return numb;
}


//
// resetLowHighRoom
//

INLINEDEF void resetLowHighRoom(void)
{
  register ulong i, high = 0, low = numbLookupEntries;

  for (i = 0; i < numbLookupEntries; i++)
  {
    if (roomLookup[i])
    {
      if (i > high) high = i;
      if (i < low) low = i;
    }
  }

  lowestRoomNumber = low;
  highestRoomNumber = high;
}


//
// getPrevRoom : find room right before roomNumb, numerically
//

INLINEDEF dikuRoom *getPrevRoom(const ulong roomNumb)
{
  ulong i = roomNumb - 1;

  if (roomNumb <= getLowestRoomNumber()) return NULL;

  while (!roomLookup[i]) i--;

  return roomLookup[i];
}


//
// getNextRoom : find room right after roomNumb, numerically
//

INLINEDEF dikuRoom *getNextRoom(const ulong roomNumb)
{
  ulong i = roomNumb + 1;

  if (roomNumb >= getHighestRoomNumber()) return NULL;

  while (!roomLookup[i]) i++;

  return roomLookup[i];
}


//
// checkMapTruenessRedund : redundant checking for checkMapTrueness
//

int checkMapTruenessRedund(const dikuRoom *room, const int exitNumb, const int dest, int &lines)
{
  char strn[1024];

  if (room->exits[exitNumb] && (room->exits[exitNumb]->destRoom != dest))
  {
    sprintf(strn, "  &+croom #%&+g%u&n's %s exit leads to &+R%u&n &+ginstead of &+R%u&n\n",
            room->roomNumber, exitnames[exitNumb], room->exits[exitNumb]->destRoom, dest);

    displayColorString(strn);

    lines += numbLinesStringNeeds(strn);

    if (lines >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");
        return TRUE;
      }

      _outtext("\n");

      lines = 0;
    }
  }

  return FALSE;
}


//
// checkMapTrueness : checks map 'trueness' - a truly perfect map will
//                    have rooms that all have north exits leading to room
//                    numb - 100, east exits to room numb + 1, and the
//                    opposite for south/west
//

void checkMapTrueness(void)
{
  dikuRoom *room = roomHead;
  int numb, lines = 2;

  _outtext("\n\n");

  while (room)
  {
    numb = room->roomNumber;

    if (checkMapTruenessRedund(room, NORTH, numb - 100, lines) ||
        checkMapTruenessRedund(room, SOUTH, numb + 100, lines) ||
        checkMapTruenessRedund(room, WEST,  numb - 1, lines) ||
        checkMapTruenessRedund(room, EAST,  numb + 1, lines))
    {
      _outtext("\n");
      return;
    }

    room = room->Next;
  }

  _outtext("\n");
}
