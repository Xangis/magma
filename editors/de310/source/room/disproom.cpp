//
//  File: disproom.cpp   originally part of dikuEdit
//
//  Usage: functions that display information about rooms
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "room.h"

using namespace std;

extern dikuRoom *roomHead, *currentRoom;
extern _CONST char *exitnames[];

//
// displayRoomList : Displays the list of rooms starting at roomHead
//

void displayRoomList(void)
{
  dikuRoom *room = roomHead;
  ulong i = 0;
  char strn[256];


  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    sprintf(strn, "&n%s&n (#%u)\n", room->roomName, room->roomNumber);

    displayColorString(strn);
    i += numbLinesStringNeeds(strn);

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      i = 0;
    }

    room = room->Next;
  }

  _outtext("\n");
}


//
// displayRoomList : Displays the list of rooms starting at roomHead
//

void displayRoomList(const char *strn)
{
  dikuRoom *room = roomHead;
  ulong i = 0, numb;
  char outStrn[512], tempStrn[512], foundRoom = FALSE, vnum;


  if (strlen(strn) == 0)
  {
    displayRoomList();
    return;
  }

  _setbkcolor(0);

  _outtext("\n\n");

  vnum = strnumer(strn);
  numb = atoi(strn);

  while (room)
  {
    strcpy(tempStrn, room->roomName);
    upstrn(tempStrn);

    if ((vnum && (numb == room->roomNumber)) || strstr(tempStrn, strn))
    {
      sprintf(outStrn, "&n%s&n (#%u)\n", room->roomName, room->roomNumber);

      foundRoom = TRUE;

      displayColorString(outStrn);
      i += numbLinesStringNeeds(outStrn);
    }

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");
      i = 0;
    }

    room = room->Next;
  }

  if (!foundRoom) _outtext("No matching rooms found.\n");

  _outtext("\n");
}


//
// displayRoomFlagsandSector : Does just that...  Displays the room flags
//                             and sector info for the room pointed to by
//                             room.  Nice, hmm?
//
//   *room : room to display info on
//

void displayRoomFlagsandSector(const dikuRoom *room)
{
  roomBitFlags roomBits = room->roomFlags.roomBits;
  char strn[256];


 // first, display the room sector info

  displayColorString(getRoomSectorStrn(room->sectorType, TRUE, TRUE, TRUE, strn));


 // display room flags

 // light source needed

  if (roomBits.dark) displayColorString("&+L(Dark)&n  ");

 // automatic death upon entering

  if (roomBits.limit_scan) displayColorString("&+L(Not Used)&n  ");

 // wandering mobs can't walk into this room

  if (roomBits.noMob) displayColorString("&+r(No mob)&n  ");

 // room is inside

  if (roomBits.indoors) displayColorString("&+c(Indoors)&n  ");

 // room is silent - no speaking, spells

  if (roomBits.silent) displayColorString("&+L(Silent)&n  ");

 // room is underwater - water breathing required

  if (roomBits.underwater) displayColorString("&+b(Underwater)&n  ");

 // cannot recall from this room

  if (roomBits.noRecall) displayColorString("&+g(No recall)&n  ");

 // cannot cast magic in this room

  if (roomBits.noMagic) displayColorString("&+g(No magic)&n  ");

 // room is a tunnel - can't fly into a tunnel..  I think that's about it

  if (roomBits.tunnel) displayColorString("&+y(Tunnel)&n  ");

 // only two people allowed in room

  if (roomBits.privateRm) displayColorString("&+L(Private)&n  ");

 // arena room flag

  if (roomBits.arena) displayColorString("&+r(Arena)&n  ");

 // no fighting or stealing allowed

  if (roomBits.safe) displayColorString("&+W(Safe)&n  ");

 // no precipitation - room is outside but is covered

  if (roomBits.noPrecip) displayColorString("&+w(No precip)&n  ");

 // single file room - people bump into each other

  if (roomBits.singleFile) displayColorString("&+g(Single file)&n  ");

 // room is a jail

  if (roomBits.jail) displayColorString("&+c(Jail)&n  ");

 // random teleport ignores a noTeleport room

  if (roomBits.noTeleport) displayColorString("&+g(No teleport)&n  ");

 // private zone flag - do not use

  if (roomBits.privZone) displayColorString("&+R(Private zone)&n  ");

 // regen of hits is faster

  if (roomBits.heal) displayColorString("&+W(Heal)&n  ");

 // regen of hits is slower

  if (roomBits.noHeal) displayColorString("&+L(No heal)&n  ");

 // unused - used to be hasTrap, which, by the way, was never implemented

  if (roomBits.unused) displayColorString("(Unused)  ");

 // ships can dock here

  if (roomBits.dockable) displayColorString("&+y(Dockable)&n  ");

 // room has a magical darkness spell cast on it

  if (roomBits.magicDark) displayColorString("&+L(Magical dark)&n  ");

 // room has a magical light spell cast on it

  if (roomBits.magicLight) displayColorString("&+W(Magical light)&n  ");

 // cannot summon into or be summoned out of this room

  if (roomBits.noSummon) displayColorString("&+g(No summon)&n  ");

 // guildroom

  if (roomBits.guildRoom) displayColorString("&+C(Guildroom)&n  ");

 // twilight

  if (roomBits.twilight) displayColorString("&+c(Twilight)&n  ");

 // readd dark - shouldn't really be used

  if (roomBits.readdDark) displayColorString("&+R(Readd dark)&n  ");

 // no gate - can't gate/planeshift out of room

  if (roomBits.noGate) displayColorString("&+g(No gate/planeshift)&n  ");

 // house

  if (roomBits.house) displayColorString("&+C(House)&n  ");

 // atrium

  if (roomBits.atrium) displayColorString("&+g(Atrium)&n  ");

 // blocks sight (farsee, scan)

  if (roomBits.blocksSight) displayColorString("&+L(Blocks sight)&n  ");

 // BFS mark (so internal, it's scary)

  if (roomBits.bfsMark) displayColorString("&+R(BFS mark)&n  ");

  _outtext("\n");
}


//
// displayExitList : Displays the list of exits
//

void displayExitList(const char *strn)
{
  dikuRoom *room = roomHead;
  roomExit *ex;
  ulong i = 0, j, numb;
  char outStrn[512], foundExit = FALSE, vnum;


  if (strlen(strn) == 0)
  {
    vnum = FALSE;
  }
  else
  {
    if (strnumer(strn))
    {
      numb = atoi(strn);
      vnum = TRUE;
    }
    else
    {
      _outtext("\nSpecify a numeric argument (or no argument).\n\n");
      return;
    }
  }

  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    for (j = 0; j < NUMB_EXITS; j++)
    {
      if (!room->exits[j]) continue;
      ex = room->exits[j];

      if (!vnum || (vnum && ((numb == ex->destRoom) || (numb == room->roomNumber))))
      {
        sprintf(outStrn, "  &+croom #&+g%u &+L(&n%s&+L) &+R%s&n to &+Croom &+G#%d&n\n",
                room->roomNumber, room->roomName, getExitStrn(j), ex->destRoom);

        foundExit = TRUE;

        displayColorString(outStrn);
        i += numbLinesStringNeeds(outStrn);
      }

      if (i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return;
        }

        _outtext("\n\n");
        i = 0;
      }
    }

    room = room->Next;
  }

  if (!foundExit) _outtext("No matching exits found.\n");

  _outtext("\n");
}


//
// displayDoorState : Displays the "door state" for an exit - each applicable
//                    flag has a letter, you see
//
//   *exitNode : pointer to exit to display info on
//

void displayDoorState(const roomExit *exitNode)
{
  char doorState, doorType;


 // check to make sure exitNode exists - somewhat handy

  if (!exitNode) return;

  doorType = exitNode->worldDoorType;
  doorState = exitNode->zoneDoorState;

  if ((doorState) || (doorType))
  {
    displayColorString("&n[");

   // world flags

    if ((doorType & 3) == 3) displayColorString("&+RU&n");
    else if (doorType & 1) displayColorString("&+cC&n");
    else if (doorType & 2) displayColorString("&+CL&n");

    if (doorType & 4) displayColorString("&+LS&n");
    if (doorType & 8) displayColorString("&+rB&n");

    displayColorString("|");

   // zone flags

    if (doorState & 1) displayColorString("&+cC&n");
    else if (doorState & 2) displayColorString("&+CL&n");
    else if (!(doorState & 8)) displayColorString("&+WO&n");  // door is open

    if (doorState & 4) displayColorString("&+LS&n");
    if (doorState & 8) displayColorString("&+rB&n");

    displayColorString("] ");
  }
}


//
// displayExitDest : Displays the exit dest for exit
//
//   *exitNode : pointer to exit to display info on
//

void displayExitDest(const roomExit *exitNode)
{
  char strn[256];

 // check to make sure exitNode exists - somewhat handy

  if (!exitNode) return;

  sprintf(strn, "&n(&+Y%d&n) ", exitNode->destRoom);
  displayColorString(strn);
}


//
// displayRoom : Display the room title, flags, description, and exits
//
//   *room : Pointer to room to display
//

void displayRoom(const dikuRoom *room)
{
  objectHere *objNode;
  mobHere *mobNode;
  char strn[512], strn2[512], strn3[512], foundExit = FALSE;
  objExtraBitFlags extraFlags;
  affect1BitFlags affect1Flags;
  mobActionBitFlags actionFlags;
  char map;
  int i;


  if (!room)
  {
    cout << "\n\nError - room is NULL - this is bad.  Aborting..\n\n";
    exit(1);
  }

  map = getIsMapZoneVal();

  if (map)
  {
    displayMap(currentRoom);
  }

 // display room title with color

  if (!map) _outtext("\n");

  if (getShowRoomVnumVal())
  {
    sprintf(strn, "%s&n (#%u)\n", room->roomName, room->roomNumber);
  }
  else
  {
    sprintf(strn, "%s&n\n", room->roomName);
  }

  displayColorString(strn);


 // display room flags and sector info

  if (getShowRoomExtraVal()) displayRoomFlagsandSector(room);


 // display room description

  if (!(room->roomDescHead))
  {
    if (!map) displayColorString("&n[[[ This room has no description ]]]\n");
  }
  else displayStringNodes(room->roomDescHead);


 // now, display exits

  displayColorString("&n&+gObvious exits: ");

  for (i = 0; i < NUMB_EXITS; i++)
  {
    if (room->exits[i])
    {
      strcpy(strn2, exitnames[i]);
      strn2[0] = toupper(strn2[0]);

      sprintf(strn, "&+c-%s ", strn2);
      displayColorString(strn);

      if (getShowExitFlagsVal()) displayDoorState(room->exits[i]);
      if (getShowExitDestVal()) displayExitDest(room->exits[i]);

      foundExit = TRUE;
    }
  }

  if (!foundExit)
  {
    displayColorString("&+cNone!");
  }

  displayColorString("&n\n");

 // check for objects in room

  if (room->objectHead)
  {
    objNode = room->objectHead;

    while (objNode)
    {
      if (objNode->objectPtr)
      {
        strn2[0] = '\0';

        if (getShowObjFlagsVal())
        {
          extraFlags = objNode->objectPtr->extraBits.objExtraBits;

          if (extraFlags.glow)       strcat(strn2, "&+MG&n");
          if (extraFlags.hum)        strcat(strn2, "&+rH&n");
          if (extraFlags.noshow)     strcat(strn2, "&+cN&n");
          if (extraFlags.buried)     strcat(strn2, "&+LB&n");
          if (extraFlags.invisible)  strcat(strn2, "&+LI&n");
          if (extraFlags.transient)  strcat(strn2, "&+RT&n");
          if (extraFlags.magic)      strcat(strn2, "&+bM&n");
          if (extraFlags.secret)     strcat(strn2, "&+gS&n");
          if (extraFlags.lit)        strcat(strn2, "&+WL&n");
          if (extraFlags.itemFloat)  strcat(strn2, "&+yF&n");
          if (strlen(strn2)) strcat(strn2, "]");
        }

        sprintf(strn, "%s%s&n", strn2, objNode->objectPtr->objLongName);

        if (getShowObjVnumVal())
        {
          sprintf(strn2, " (#%u)", objNode->objectNumb);
          strcat(strn, strn2);
        }

        sprintf(strn2, " &+c(%u%%)&n", objNode->randomChance);
        strcat(strn, strn2);

        strcat(strn, "\n");
      }
      else
      {
        sprintf(strn,
"An object of a type not in this .OBJ file is here. (#%u) &+c(%u%%)\n",
                objNode->objectNumb, objNode->randomChance);
      }

      displayColorString(strn);

      objNode = objNode->Next;
    }
  }

 // check for mobs in room

  if (room->mobHead)
  {
    mobNode = room->mobHead;

    while (mobNode)
    {
      if (mobNode->mobPtr)
      {
        strn[0] = '\0';

        if (getShowMobFlagsVal())
        {
          affect1Flags = mobNode->mobPtr->affect1Bits.affect1Bits;
          actionFlags = mobNode->mobPtr->actionBits.mobActionBits;

          if (mobNode->mobPtr->questPtr) strcat(strn, "&+GQ&n");
          if (mobNode->mobPtr->shopPtr)  strcat(strn, "&+MSh&n");

          if (affect1Flags.invisible)    strcat(strn, "&+LI&n");
          if (affect1Flags.hide)         strcat(strn, "&+rH&n");
          if (actionFlags.memory)        strcat(strn, "&+CM&n");
          if (actionFlags.sentinel)      strcat(strn, "&+WS&n");
          if (actionFlags.stayZone)      strcat(strn, "&+YZ&n");
          if (actionFlags.protector)     strcat(strn, "&+mP&n");
          if (actionFlags.hunter)        strcat(strn, "&+RHu&n");
          if (actionFlags.scavenger)     strcat(strn, "&+ySc&n");
          if (actionFlags.teacher)       strcat(strn, "&+YT&n");
          if (actionFlags.wimpy)         strcat(strn, "&+gW&n");
          if (actionFlags.aggressive)    strcat(strn, "&+RA&n");
          if (actionFlags.aggresEvil)    strcat(strn, "&+RaE&n");
          if (actionFlags.aggresGood)    strcat(strn, "&+RaG&n");
          if (actionFlags.aggresNeut)    strcat(strn, "&+RaN&n");
          if (actionFlags.aggrEvilR)     strcat(strn, "&+RaEr&n");
          if (actionFlags.aggrGoodR)     strcat(strn, "&+RaGr&n");
          if (actionFlags.aggrOutcast)   strcat(strn, "&+RaO&n");

          if (strlen(strn)) strcat(strn, "]");
        }

        strcat(strn, mobNode->mobPtr->mobLongName);

        if (getShowMobPosVal())
        {
          strcat(strn, "&n [&+c");

          switch (mobNode->mobPtr->defaultPos)
          {
            case POSITION_SLEEPING  : strcat(strn, "Sl");  break;
            case POSITION_RESTING   : strcat(strn, "R");  break;
            case POSITION_SITTING   : strcat(strn, "Si");  break;
            case POSITION_STANDING  : strcat(strn, "St");  break;
            default : strcat(strn, "XX");  break;
          }

          strcat(strn, "&n]");
        }

        strcpy(strn2, strn);
        if (getShowMobVnumVal())
        {
          sprintf(strn, " (#%u)", mobNode->mobNumb);
          strcat(strn2, strn);
        }
      }
      else
      {
        sprintf(strn2,
"A mob of a type not in this .MOB file is here. (#%u)",
                mobNode->mobNumb);
      }

      sprintf(strn, " &+c(%u%%)&n", mobNode->randomChance);
      strcat(strn2, strn);

      if ((mobNode->riding) && getShowMobRideFollowVal())
      {
        if (mobNode->riding->mobPtr)
        {
          strcpy(strn3, mobNode->riding->mobPtr->mobShortName);
        }
        else strcpy(strn3, "(mob not in this .MOB file)");

        sprintf(strn, " (riding %s&n (#%d))", strn3, mobNode->riding->mobNumb);
        strcat(strn2, strn);
      }

      if ((mobNode->riddenBy) && getShowMobRideFollowVal())
      {
        if (mobNode->riddenBy->mobPtr)
        {
          strcpy(strn3, mobNode->riddenBy->mobPtr->mobShortName);
        }
        else strcpy(strn3, "(mob not in this .MOB file)");

        sprintf(strn, " (ridden by %s&n (#%d))", strn3, mobNode->riddenBy->mobNumb);
        strcat(strn2, strn);
      }

      if (getShowMobRideFollowVal())
      {
/*   // ehh this kinda sucks, maybe readd it as a toggleable option
        i = getNumbFollowers(mobNode);
        if (i)
        {
          sprintf(strn, " (followed by %u mob%s)",
                  i, plural(i));
          strcat(strn2, strn);
        }
*/

        if (mobNode->following)
        {
          if (mobNode->following->mobPtr)
          {
            strcpy(strn3, mobNode->following->mobPtr->mobShortName);
          }
          else strcpy(strn3, "(mob not in this .MOB file)");

          sprintf(strn, " (following %s&n (#%d))", strn3, mobNode->following->mobNumb);
          strcat(strn2, strn);
        }
      }

      strcat(strn2, "\n");
      displayColorString(strn2);

      mobNode = mobNode->Next;
    }
  }

  _outtext("\n");
}


//
// displayRoomInfo : Displays various info on the room record passed to it
//
//   *room : room record to display info on
//

//void displayRoomInfo(dikuRoom *room)
void displayRoomInfo(const char *args)
{
  char *strn = new char[2048], *strn2 = new char[2048];

/*
  long nroom = -1, sroom = -1, wroom = -1, eroom = -1, uroom = -1, droom = -1;

  char nname[MAX_ROOMNAME_LEN] = "Exit does not exist",
       sname[MAX_ROOMNAME_LEN] = "Exit does not exist",
       wname[MAX_ROOMNAME_LEN] = "Exit does not exist",
       ename[MAX_ROOMNAME_LEN] = "Exit does not exist",
       uname[MAX_ROOMNAME_LEN] = "Exit does not exist",
       dname[MAX_ROOMNAME_LEN] = "Exit does not exist";
*/

  dikuRoom *room;
  extraDesc *extraDescNode;


  if (!strn || !strn2)
  {
    _outtext("\n\nError allocating string buffer in displayRoomInfo() - "
             "aborting..\n");

    if (strn) delete[] strn;
    if (strn2) delete[] strn2;

    return;  // no need to die completely, after all
  }

  if (!strlen(args)) room = currentRoom;
  else
  {
    if (!strnumer(args))
    {
      displayColorString("&n\nError in input - specify a valid room vnum.\n\n");
      return;
    }

    room = findRoom(atoi(args));
    if (!room)
    {
      displayColorString("&n\nNo room exists with that vnum.\n\n");
      return;
    }
  }

/*
  if (room->north)
  {
    nroom = room->north->destRoom;

    roomAddr = findRoom(nroom);

    if (roomAddr)
    {
      strcpy(nname, roomAddr->roomName);
    }
    else
    {
      if (nroom != -1) strcpy(nname, "Room info not in this .WLD file");
      else strcpy(nname, "Exit leads nowhere");
    }
  }

  if (room->south)
  {
    sroom = room->south->destRoom;

    roomAddr = findRoom(sroom);

    if (roomAddr)
    {
      strcpy(sname, roomAddr->roomName);
    }
    else
    {
      if (sroom != -1) strcpy(sname, "Room info not in this .WLD file");
      else strcpy(sname, "Exit leads nowhere");
    }
  }

  if (room->west)
  {
    wroom = room->west->destRoom;

    roomAddr = findRoom(wroom);

    if (roomAddr)
    {
      strcpy(wname, roomAddr->roomName);
    }
    else
    {
      if (wroom != -1) strcpy(wname, "Room info not in this .WLD file");
      else strcpy(wname, "Exit leads nowhere");
    }
  }

  if (room->east)
  {
    eroom = room->east->destRoom;

    roomAddr = findRoom(eroom);

    if (roomAddr)
    {
      strcpy(ename, roomAddr->roomName);
    }
    else
    {
      if (eroom != -1) strcpy(ename, "Room info not in this .WLD file");
      else strcpy(ename, "Exit leads nowhere");
    }
  }

  if (room->up)
  {
    uroom = room->up->destRoom;

    roomAddr = findRoom(uroom);

    if (roomAddr)
    {
      strcpy(uname, roomAddr->roomName);
    }
    else
    {
      if (uroom != -1) strcpy(uname, "Room info not in this .WLD file");
      else strcpy(uname, "Exit leads nowhere");
    }
  }

  if (room->down)
  {
    droom = room->down->destRoom;

    roomAddr = findRoom(droom);

    if (roomAddr)
    {
      strcpy(dname, roomAddr->roomName);
    }
    else
    {
      if (droom != -1) strcpy(dname, "Room info not in this .WLD file");
      else strcpy(dname, "Exit leads nowhere");
    }
  }
*/

  sprintf(strn, "\n\n"
                "roomNumber     : %u\n"
                "roomName       : ",
                room->roomNumber);

  _outtext(strn);

  displayColorString(room->roomName);
  displayColorString("&n");
//  _settextcolor(7);


 // show keywords for all extra descs in room - each desc separated by a slash

  extraDescNode = room->extraDescHead;

  strcpy(strn2, "\0");

  while (extraDescNode)
  {
   // second argument strn because it requires a second arg, and hell, why not?
    strcat(strn2, getReadableKeywordStrn(extraDescNode->keywordListHead, strn));

    extraDescNode = extraDescNode->Next;
    if (extraDescNode) strcat(strn2, " / ");
  }

 // compose string for object keywords - ehh, do this later

  sprintf(strn, "\n"
                "zoneNumber     : %u\n"
                "roomFlags      : %u\n"
                "sectorType     : %u\n\n"
                "extra descs    : %u (%s)\n"
                "objs in room   : %u\n"
                "mobs in room   : %u\n\n",
//                "north leads to : %d (",
                room->zoneNumber,
                room->roomFlags.longIntFlags,
                room->sectorType,
                getNumbExtraDescs(room->extraDescHead), strn2,
                getNumbObjectHereNodes(room->objectHead),
                getNumbMobHereNodes(room->mobHead));
//                nroom);

  _outtext(strn);

/*
  displayColorString(nname);
  displayColorString("&n");
//  _settextcolor(7);


  sprintf(strn, ")\n"
                "south ...      : %d (",
                sroom);

  _outtext(strn);

  displayColorString(sname);
  displayColorString("&n");
//  _settextcolor(7);


  sprintf(strn, ")\n"
                "west ...       : %d (",
                wroom);

  _outtext(strn);

  displayColorString(wname);
  displayColorString("&n");
//  _settextcolor(7);


  sprintf(strn, ")\n"
                "east ...       : %d (",
                eroom);

  _outtext(strn);

  displayColorString(ename);
  displayColorString("&n");
//  _settextcolor(7);


  sprintf(strn, ")\n"
                "up ...         : %d (",
                uroom);

  _outtext(strn);

  displayColorString(uname);
  displayColorString("&n");
//  _settextcolor(7);


  sprintf(strn, ")\n"
                "down ...       : %d (",
                droom);

  _outtext(strn);

  displayColorString(dname);
  displayColorString("&n");
//  _settextcolor(7);
*/

  sprintf(strn, //")\n\n"
                "current dir    : %u (%s)\n"
                "current str    : %u\n\n"
                "fall percentage: %u\n\n",

          room->currentDir, getExitStrn(room->currentDir),
          room->current,

          room->fallChance);

  _outtext(strn);

  delete[] strn;
  delete[] strn2;
}


//
// displayRoomManaTypeList
//

void displayRoomManaTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = MANA_LOWEST; i <= MANA_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getManaFlagStrn(i));

    _outtext(strn);
  }

  _outtext("\n");
}


//
// getRoomExitInfo
//

char *getRoomExitInfo(const roomExit *exitNode, const char *direction,
                      char *strn)
{
  dikuRoom *roomAddr;
  char roomName[MAX_ROOMNAME_LEN];


  if (!exitNode)
  {
    strn[0] = '\0';
    return NULL;
  }

  roomAddr = findRoom(exitNode->destRoom);

  if (roomAddr)
  {
    strcpy(roomName, roomAddr->roomName);
  }
  else
  {
    if (exitNode->destRoom != -1) strcpy(roomName, "Room info not in this .WLD file");
    else strcpy(roomName, "Exit leads nowhere");
  }

  sprintf(strn, "&+c%s&n - %s&n&+c (#%d)&n\n", direction, roomName, exitNode->destRoom);

  return strn;
}


//
// displayRoomExitInfo : similar to "exits" command in diku
//

void displayRoomExitInfo(const dikuRoom *room)
{
  char strn[512], foundExit = FALSE, i, strn2[128];


  displayColorString("\n&+gExits:&n\n");

  for (i = 0; i < NUMB_EXITS; i++)
  {
    sprintf(strn2, "%-10s", exitnames[i]);
    strn2[0] = toupper(strn2[0]);

    if (getRoomExitInfo(room->exits[i], strn2, strn))
    {
      displayColorString(strn);
      foundExit = TRUE;
    }
  }

  if (!foundExit)
  {
    displayColorString("&+RNone!&n\n");
  }

  _outtext("\n");
}
