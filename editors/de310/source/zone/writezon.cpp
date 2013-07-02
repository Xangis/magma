//
//  File: writezon.cpp   originally part of dikuEdit
//
//  Usage: functions for writing zone info to the .ZON file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"
#include "../de.h"

#include "writezon.h"

using namespace std;

extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern dikuMob *mobHead;
extern dikuZone zoneRec;
extern char readFromSubdirs;
extern ulong numbObjs, numbMobs;
extern _CONST char *exitnames[];


//
// addExitWrittentoList : Adds an exitWritten node onto the end of a linked
//                        list of exitWrittens
//
//    **exitHead : pointer to pointer to head of list
//    *exittoAdd : pointer to node to add
//

void addExitWrittentoList(exitWritten **exitHead, exitWritten *exittoAdd)
{
  exitWritten *exitNode;


  if (!exittoAdd) return;

  if (!*exitHead) *exitHead = exittoAdd;
  else
  {
    exitNode = *exitHead;

    while (exitNode)
    {
      if (!exitNode->Next) break;

      exitNode = exitNode->Next;
    }

    if (!exitNode)
    {
      _outtext("\nError in addExitWrittentoList() - exitNode is NULL.\n");
      return;
    }

    exitNode->Next = exittoAdd;
  }

//  cout << "\nadded exitWrit with roomNumb " << exitNode->roomNumber << " and " <<
//          "exit direction " << (usint)(exitNode->exitNumber) << ".\n";
}


//
// deleteExitWrittenList : Deletes a linked list of exitWrittens
//
//    *exitHead : pointer to head of list
//

void deleteExitWrittenList(exitWritten *exitHead)
{
  exitWritten *nextExit;


  while (exitHead)
  {
    nextExit = exitHead->Next;
    delete exitHead;

    exitHead = nextExit;
  }
}


//
// checkExitWrittenforExit : Checks a list of exitWrittens for an exit
//                           that matches the room number and exit direction
//                           specified - returns the address of the first
//                           matching node
//
//    *exitHead : head of list to check
//
//     roomNumb : src room numb to match
//     exitNumb : exit dir to match
//

exitWritten *checkExitWrittenListforExit(exitWritten *exitHead,
                                         const long roomNumb,
                                         const char exitNumb)
{
  exitWritten *exitNode = exitHead;

  while (exitNode)
  {
    if ((exitNode->roomNumber == roomNumb) &&
        (exitNode->exitNumber == exitNumb)) return exitNode;

    exitNode = exitNode->Next;
  }

  return NULL;
}


//
// checkForPairedExit : Tries to find the "paired exit" for the exit specified
//
//    *exitNode : pointer to exit node to find "pair" of
//  srcRoomNumb : room number exit is in
//     *exitDir : direction exit leads in (set by findCorrespondingExit)
//

roomExit *checkForPairedExit(const roomExit *exitNode, const long srcRoomNumb,
                             char *exitDir)
{
 // look for an exit that is the "opposite" of this one

  return findCorrespondingExit(exitNode->destRoom, srcRoomNumb, exitDir);
}


//
// writeAllExitsPairedRedundant :
//      Redundant stuff used for each exit in writeAllExitsPaired - returns
//      TRUE if a door state has been changed, not currently used, may be
//      useful someday
//
//   *zoneFile : file to write exit pair commands to
//       *room : room that source exit is in
//
// **exitWHead : pointer to pointer to start of exitWritten list
//   *exitNode : source exit being written
//
//   *exitName : "name" of source exit - north, south, etc
//    exitChar : key that should be hit to set door state for this exit
//               (north = N, etc.)
//
//     origDir : direction of *exitNode
//

char writeAllExitsPairedRedundant(
       FILE *zoneFile, const dikuRoom *room, exitWritten **exitWHead,
       roomExit *exitNode, const char *exitName, const char exitChar,
       const char origDir)
{
  roomExit *pairedExit;
  exitWritten *exitWrit;
  char exitDir, upExitName[64];
  char strn[2048], changedDoorState = FALSE;
  char zoneType, worldType, zoneBits, worldBits;
  char pzoneType, pworldType, pzoneBits, pworldBits;
  usint ch;


  if (!room || !exitNode) return FALSE;

  strcpy(upExitName, exitName);
  upExitName[0] = toupper(upExitName[0]);

  zoneType = exitNode->zoneDoorState & 3;
  zoneBits = exitNode->zoneDoorState & 12;

  worldType = exitNode->worldDoorType & 3;
  worldBits = exitNode->worldDoorType & 12;

 // if exit has a door state, do stuff to it, but only if it hasn't already
 // been written

  if (((zoneType || worldType) || ((zoneBits & 8) || (worldBits & 8))) &&
       (!checkExitWrittenListforExit(*exitWHead, room->roomNumber, origDir)))
  {
    pairedExit = checkForPairedExit(exitNode, room->roomNumber, &exitDir);

    if (!pairedExit)
    {
      sprintf(strn, "\n\n"
"Warning: %s exit in room #%u has a door type/state set, but has no\n"
"         corresponding exit on the opposite side.  Thus, this exit's door\n"
"         state and type must be set to 0.\n\n"
"Press any key to continue..", upExitName, room->roomNumber);

      _outtext(strn);

//      exitNode->zoneDoorState = exitNode->worldDoorType = 0;
      exitNode->zoneDoorState = 0 | zoneBits;
      if (exitNode->zoneDoorState & 8) exitNode->zoneDoorState ^= 8;

      exitNode->worldDoorType = 0 | worldBits;
      if (exitNode->worldDoorType & 8) exitNode->worldDoorType ^= 8;

      getkey();
      _outtext("\n\n");

      return TRUE;
    }

    zoneType = exitNode->zoneDoorState & 3;
    zoneBits = exitNode->zoneDoorState & 12;

    worldType = exitNode->worldDoorType & 3;
    worldBits = exitNode->worldDoorType & 12;

    pzoneType = pairedExit->zoneDoorState & 3;
    pzoneBits = pairedExit->zoneDoorState & 12;

    pworldType = pairedExit->worldDoorType & 3;
    pworldBits = pairedExit->worldDoorType & 12;

   // add blocked bit checking to this - what a massive pain in the ass

    if (!((zoneBits & 8) || (pzoneBits & 8)))
    {
      if ((pzoneType && !zoneType) || (!pzoneType && zoneType))
      {
        sprintf(strn, "\n"
  "Warning: %s exit in room #%u has a non-zero door type set in the\n"
  "         .zon (%u) but the exit on the opposite side has a door type of\n"
  "         %u (.zon).\n\n"
  "Either the opposite exit must be set to the same door type as the %s\n"
  "exit, or the %s exit must be set to that of the opposite exit.\n\n"
  "Set opposite equal to %s (O), or set %s equal to opposite (%c)? ",

  upExitName, room->roomNumber, zoneType, pzoneType,
  exitName, exitName, exitName, exitName, exitChar);

        _outtext(strn);

        do
        {
          ch = toupper(getkey());
        } while ((ch != 'O') && (ch != exitChar));

        sprintf(strn, "%c\n\n", (char)ch);
        _outtext(strn);

        if (ch == 'O') pairedExit->zoneDoorState = zoneType | pzoneBits;
        else exitNode->zoneDoorState = pzoneType | zoneBits;

        changedDoorState = TRUE;
      }

      if ((pworldType && !worldType) || (!pworldType && worldType))
      {
        sprintf(strn, "\n"
  "Warning: %s exit in room #%u has a non-zero door type set in the\n"
  "         .wld (%u) but the exit on the opposite side has a door type of\n"
  "         %u (.wld).\n\n"
  "Either the opposite exit must be set to the same door type as the %s\n"
  "exit, or the %s exit must be set to that of the opposite exit.\n\n"
  "Set opposite equal to %s (O), or set %s equal to opposite (%c)? ",

  upExitName, room->roomNumber, worldType, pworldType,
  exitName, exitName, exitName, exitName, exitChar);

        _outtext(strn);

        do
        {
          ch = toupper(getkey());
        } while ((ch != 'O') && (ch != exitChar));

        sprintf(strn, "%c\n\n", (char)ch);
        _outtext(strn);

        if (ch == 'O') pairedExit->worldDoorType = worldType | pworldBits;
        else exitNode->worldDoorType = pworldType | worldBits;

        changedDoorState = TRUE;
      }
    }

   // write paired exits

    // first, write "original" exit

    fprintf(zoneFile, "D 0 %u %u %u 100 0 0 0\n",
            room->roomNumber, origDir, getZoneDoorState(exitNode));

    exitWrit = new exitWritten;
    if (!exitWrit)
    {
      cout << "\n\nError alloc'ing exitWritten node in writeAllExitsPairedRedundant\n" <<
              "Aborting..\n";

      exit(1);
    }

    exitWrit->roomNumber = room->roomNumber;
    exitWrit->exitNumber = origDir;
    exitWrit->Next = NULL;

    addExitWrittentoList(exitWHead, exitWrit);

    getDescExitStrnforZonefile(room, exitNode, strn, exitName);
    fputs(strn, zoneFile);

    // now, "paired" exit

    fprintf(zoneFile, "D 0 %u %u %u 100 0 0 0\n",
            exitNode->destRoom, exitDir, getZoneDoorState(pairedExit));

    exitWrit = new exitWritten;
    if (!exitWrit)
    {
      cout << "\n\nError alloc'ing exitWritten node in writeAllExitsPairedRedundant\n" <<
              "Aborting..\n";

      exit(1);
    }

    exitWrit->roomNumber = exitNode->destRoom;
    exitWrit->exitNumber = exitDir;
    exitWrit->Next = NULL;

    addExitWrittentoList(exitWHead, exitWrit);

    getDescExitStrnforZonefile(findRoom(exitNode->destRoom),
                    getExitNode(exitNode->destRoom, exitDir), strn,
                    getExitStrn(exitDir));

    fputs(strn, zoneFile);

    return changedDoorState;
  }

  return FALSE;
}


//
// writeAllExitsPaired : Runs through the room list, writing all exits
//                       with door states and pairing them appropriately.
//
//   *zoneFile : file to write set door state commands to
//

void writeAllExitsPaired(FILE *zoneFile)
{
  exitWritten *exitWHead = NULL;
  dikuRoom *room;
  int i, numb, high = getHighestRoomNumber();


  for (numb = getLowestRoomNumber(); numb <= high; numb++)
  {
    if (!(room = findRoom(numb))) continue;

    for (i = 0; i < NUMB_EXITS; i++)
    {
      writeAllExitsPairedRedundant(zoneFile, room, &exitWHead, room->exits[i],
                                   exitnames[i], toupper(exitnames[i][0]), i);
    }
  }

  deleteExitWrittenList(exitWHead);
}


//
// writeAllObjInside
//

void writeAllObjInside(FILE *zoneFile, objectHere *insidePtr,
                       const ulong containerNumb, const char extraSpaces)
{
  objectHere *objInsidePtr = insidePtr;
  char preComment[256], fullLine[MAX_OBJSNAME_LEN + 512],
       objName[MAX_OBJSNAME_LEN];
  ulong i;


  if (!insidePtr) return;

 // write objects inside original object

  while (objInsidePtr)
  {
    sprintf(preComment, "P 1 %u %u %u %u %u 0 0",
            objInsidePtr->objectNumb,
            getNumbEntities(ENTITY_OBJECT, objInsidePtr->objectNumb, TRUE),
            containerNumb, objInsidePtr->randomChance, objInsidePtr->objectPtr->globalmaxload);

    for (i = strlen(preComment); i < COMMENT_POS; i++)
    {
      preComment[i] = ' ';
    }

    preComment[i] = '\0';

    if (objInsidePtr->objectPtr)
    {
      strcpy(objName, objInsidePtr->objectPtr->objShortName);
      remColorCodes(objName);
    }
    else strcpy(objName, "(object not in this .OBJ file)");

    sprintf(fullLine, "%s*   ", preComment);
    for (i = 0; i < extraSpaces; i++) strcat(fullLine, " ");

    strcat(fullLine, objName);
    strcat(fullLine, "\n");

    fputs(fullLine, zoneFile);

    objInsidePtr = objInsidePtr->Next;
  }

 // reset objInsidePtr and write obj inside lists for shit inside this object

  objInsidePtr = insidePtr;

  while (objInsidePtr)
  {
    if (objInsidePtr->objInside)
      writeAllObjInside(zoneFile, objInsidePtr->objInside,
                        objInsidePtr->objectNumb, extraSpaces + 2);

    objInsidePtr = objInsidePtr->Next;
  }
}


//
// writeLoadObjectCmd : Writes the "load object" command for the specified
//                      objHere node
//
//       *room : room that contains the object
//    *objHere : objHere node that "is" the object
//   *zoneFile : file to write to
//

void writeLoadObjectCmd(const dikuRoom *room, const objectHere *objHere,
                        FILE *zoneFile)
{
  ulong i;
  char objName[MAX_OBJSNAME_LEN], preComment[MAX_OBJSNAME_LEN + 256],
       comment[MAX_OBJSNAME_LEN + 128];


  if (objHere->objectPtr)
  {
    strcpy(objName, objHere->objectPtr->objShortName);
    remColorCodes(objName);
  }
  else strcpy(objName, "(object not in this .OBJ file)");

  sprintf(preComment, "O 0 %u %u %u %u %u 0 0",
          objHere->objectNumb,
          getNumbEntities(ENTITY_OBJECT, objHere->objectNumb, TRUE),
          room->roomNumber, objHere->randomChance, objHere->objectPtr->globalmaxload);

  for (i = strlen(preComment); i < COMMENT_POS; i++)
  {
    preComment[i] = ' ';
  }

  preComment[i] = '\0';

  sprintf(comment, "* %s\n", objName);

  strcat(preComment, comment);

  fputs(preComment, zoneFile);
}


//
// writeLoadMobCommand : Writes a "load mob" command to the zone file
//
//      *room : room that mob is in
//  *mobHNode : mob to load
//  *zoneFile : file to write info to
//

void writeLoadMobCommand(const dikuRoom *room, const mobHere *mobHNode,
                         FILE *zoneFile)
{
  ulong i;
  char mobName[MAX_MOBSNAME_LEN], preComment[MAX_MOBSNAME_LEN + 256],
       comment[MAX_MOBSNAME_LEN + 128];


  if (mobHNode->mobPtr)
  {
    strcpy(mobName, mobHNode->mobPtr->mobShortName);
    remColorCodes(mobName);
  }
  else strcpy(mobName, "(mob not in this .MOB file)");

  sprintf(preComment, "M 0 %u %u %u %u 0 0 0",
          mobHNode->mobNumb,
          getNumbEntities(ENTITY_MOB, mobHNode->mobNumb, TRUE),
          room->roomNumber, mobHNode->randomChance);

  for (i = strlen(preComment); i < COMMENT_POS; i++)
  {
    preComment[i] = ' ';
  }

  preComment[i] = '\0';

  sprintf(comment, "* %s\n", mobName);

  strcat(preComment, comment);

  fputs(preComment, zoneFile);
}


//
// writeEquipMobCommand : Write a "equip mob with obj" command to the zone file
//
//   *mobEquip : object to equip mob with
//   *zoneFile : file to write command to
//

void writeEquipMobCommand(const objectHere *mobEquip, FILE *zoneFile,
                          char eqpos)
{
  ulong i;
  char equipName[MAX_OBJSNAME_LEN], preComment[MAX_OBJSNAME_LEN + 256],
       comment[MAX_OBJSNAME_LEN + 128];


  if (!mobEquip) return;

  if (mobEquip->objectPtr)
  {
    strcpy(equipName, mobEquip->objectPtr->objShortName);
    remColorCodes(equipName);
  }
  else strcpy(equipName, "(object not in this .OBJ file)");

  if (eqpos == WEAR_WHOLEBODY) eqpos = WEAR_BODY;
  else if (eqpos == WEAR_WHOLEHEAD) eqpos = WEAR_HEAD;
  else if (eqpos == WIELD_TWOHANDS) eqpos = WIELD_PRIMARY;
  else if (eqpos == WIELD_TWOHANDS2) eqpos = WIELD_SECOND;
  else if (eqpos == HOLD2) eqpos = HOLD;

  sprintf(preComment, "E 1 %u %u %u %u %u 0 0",
          mobEquip->objectNumb,
          getNumbEntities(ENTITY_OBJECT, mobEquip->objectNumb, TRUE), eqpos,
          mobEquip->randomChance, mobEquip->objectPtr->globalmaxload );

  for (i = strlen(preComment); i < COMMENT_POS; i++)
  {
    preComment[i] = ' ';
  }

  preComment[i] = '\0';

  sprintf(comment, "*   %s\n", equipName);

  strcat(preComment, comment);

  fputs(preComment, zoneFile);
}


//
// writeGiveMobCommand : Write a "give obj to mob" command to the zone file
//
//     *mobObj : object to give
//   *zoneFile : file to write command to
//

void writeGiveMobCommand(const objectHere *mobObj, FILE *zoneFile)
{
  ulong i;
  char objName[MAX_OBJSNAME_LEN], preComment[MAX_OBJSNAME_LEN + 256],
       comment[MAX_OBJSNAME_LEN + 128];


  if (mobObj->objectPtr)
  {
    strcpy(objName, mobObj->objectPtr->objShortName);
    remColorCodes(objName);
  }
  else strcpy(objName, "(object not in this .OBJ file)");

  sprintf(preComment, "G 1 %u %u 0 %u %u 0 0",
          mobObj->objectNumb,
          getNumbEntities(ENTITY_OBJECT, mobObj->objectNumb, TRUE),
          mobObj->randomChance, mobObj->objectPtr->globalmaxload );

  for (i = strlen(preComment); i < COMMENT_POS; i++)
  {
    preComment[i] = ' ';
  }

  preComment[i] = '\0';

  sprintf(comment, "*    %s\n", objName);

  strcat(preComment, comment);

  fputs(preComment, zoneFile);
}


//
// writeRideMobCommand : Writes a "ride mob" command to the zone file along
//                       with all eq loads
//
//  *mobHNode : mob riding something
//  *zoneFile : file to write info to
//

void writeRideMobCommand(const dikuRoom *room, const mobHere *mobHNode,
                         FILE *zoneFile)
{
  ulong i;
  char mobName[MAX_MOBSNAME_LEN], preComment[MAX_MOBSNAME_LEN + 256],
       comment[MAX_MOBSNAME_LEN + 128];
  mobHere *mount;
  objectHere *mobEquip, *mobObj;


  if (!mobHNode || !mobHNode->riding) return;

  mount = mobHNode->riding;

  if (mount->mobPtr)
  {
    strcpy(mobName, mount->mobPtr->mobShortName);
    remColorCodes(mobName);
  }
  else strcpy(mobName, "(mob not in this .MOB file)");

  sprintf(preComment, "R 1 %u %u %u %u 0 0 0",
          mount->mobNumb,
          getNumbEntities(ENTITY_MOB, mount->mobNumb, TRUE),
          room->roomNumber, mount->randomChance);

  for (i = strlen(preComment); i < COMMENT_POS; i++)
  {
    preComment[i] = ' ';
  }

  preComment[i] = '\0';

  sprintf(comment, "* %s (being ridden)\n", mobName);

  strcat(preComment, comment);

  fputs(preComment, zoneFile);

  for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
  {
    mobEquip = mount->equipment[i];

    if (mobEquip)
    {
      writeEquipMobCommand(mobEquip, zoneFile, i);
      writeAllObjInside(zoneFile, mobEquip->objInside, mobEquip->objectNumb, 0);
    }
  }

  mobObj = mount->objectHead;
  while (mobObj)
  {
    writeGiveMobCommand(mobObj, zoneFile);

    writeAllObjInside(zoneFile, mobObj->objInside, mobObj->objectNumb, 0);

    mobObj = mobObj->Next;
  }
}


//
// writeMobFollowerCommands : writes "F" lines for all mobs following a
//                            certain leader
//
//  *mobLeader : leader
//   *zoneFile : zone file
//

void writeMobFollowerCommands(const mobHere *mobLeader, FILE *zoneFile)
{
  char mobName[MAX_MOBSNAME_LEN], preComment[MAX_MOBSNAME_LEN + 256],
       comment[MAX_MOBSNAME_LEN + 128];
  dikuRoom *room = roomHead;
  mobHere *follower;
  objectHere *mobEquip, *mobObj;
  ulong i;


  if (!mobLeader) return;

  while (room)
  {
    follower = room->mobHead;
    while (follower)
    {
      if (follower->following == mobLeader)
      {
        if (follower->mobPtr)
        {
          strcpy(mobName, follower->mobPtr->mobShortName);
          remColorCodes(mobName);
        }
        else strcpy(mobName, "(mob not in this .MOB file)");

        sprintf(preComment, "F 1 %u %u %u %u 0 0 0",
                follower->mobNumb,
                getNumbEntities(ENTITY_MOB, follower->mobNumb, TRUE),
                room->roomNumber, follower->randomChance);

        for (i = strlen(preComment); i < COMMENT_POS; i++)
        {
          preComment[i] = ' ';
        }

        preComment[i] = '\0';

        sprintf(comment, "* %s (following)\n", mobName);

        strcat(preComment, comment);

        fputs(preComment, zoneFile);

        for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        {
          mobEquip = follower->equipment[i];

          if (mobEquip)
          {
            writeEquipMobCommand(mobEquip, zoneFile, i);
            writeAllObjInside(zoneFile, mobEquip->objInside,
                              mobEquip->objectNumb, 0);
          }
        }

        mobObj = follower->objectHead;
        while (mobObj)
        {
          writeGiveMobCommand(mobObj, zoneFile);

          writeAllObjInside(zoneFile, mobObj->objInside, mobObj->objectNumb, 0);

          mobObj = mobObj->Next;
        }
      }

      follower = follower->Next;
    }

    room = room->Next;
  }
}


//
// writeZoneFile : Write the zone file - controls what is loaded where
//

void writeZoneFile(const char *filename /*= NULL*/)
{
  FILE *zoneFile;
  char zoneFilename[81] = "\0";
  char strn[256], i;

  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;

  objectHere *objHere, *mobEquip, *mobObj;
  mobHere *mobHNode, *mobLeader = NULL;


 // assemble the filename of the zone file

  if (readFromSubdirs) strcpy(zoneFilename, "zon/");

  if (filename) strcat(zoneFilename, filename);
  else strcat(zoneFilename, getMainZoneNameStrn());

  strcat(zoneFilename, ".zon");


 // open the zone file for writing

  if ((zoneFile = fopen(zoneFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << zoneFilename << " for writing - aborting\n";

    return;
  }

  sprintf(strn, "Writing %s - %u object%s, %u mob%s\n",
          zoneFilename, numbObjs, plural(numbObjs),
                        numbMobs, plural(numbMobs));

  _outtext(strn);

 // write the zone number and name

  fprintf(zoneFile, "#%u\n", zoneRec.zoneNumber);
  fprintf(zoneFile, "%s~\n", zoneRec.zoneName);

 // now write the highest room numb, reset mode, zone bits, lifespan info,
 // and a 0

  fprintf(zoneFile, "%u %u %u %u %u 0\n",
          getHighestRoomNumber(), zoneRec.resetMode,
          zoneRec.miscBits.longIntFlags, zoneRec.lifeLow, zoneRec.lifeHigh);

 // write comment

  fputs(
"*\n"
"* Zone edited with dikuEdit v" DE_VERSION "\n"
"*\n", zoneFile);

 // write door states

//  fputs("*\n*\n* door states\n*\n", zoneFile);
  fputs("*\n", zoneFile);

  checkAllExits();  // check shit

  writeAllExitsPaired(zoneFile);

 // object loading

  fputs("*\n", zoneFile);
//  fputs("*\n*\n* object loading\n*\n", zoneFile);

  room = roomHead;

  while (room)
  {
    objHere = room->objectHead;

    while (objHere)
    {
      writeLoadObjectCmd(room, objHere, zoneFile);

      writeAllObjInside(zoneFile, objHere->objInside, objHere->objectNumb, 0);

      objHere = objHere->Next;
    }

    room = room->Next;
  }

 // mob loading

  fputs("*\n", zoneFile);

  room = roomHead;

  while (room)
  {
   // make sure to write non-following mob first, to prevent the shit getting
   // fucked up

    mobHNode = room->mobHead;

    while (mobHNode)
    {
     // prevent dupe load commands

      if (!mobHNode->following)
      {
        if (!mobHNode->riddenBy)
        {
          writeLoadMobCommand(room, mobHNode, zoneFile);

          for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
          {
            mobEquip = mobHNode->equipment[i];

            writeEquipMobCommand(mobEquip, zoneFile, i);

            if (mobEquip)
              writeAllObjInside(zoneFile, mobEquip->objInside,
                                mobEquip->objectNumb, 0);
          }

          mobObj = mobHNode->objectHead;
          while (mobObj)
          {
            writeGiveMobCommand(mobObj, zoneFile);

            writeAllObjInside(zoneFile, mobObj->objInside, mobObj->objectNumb, 0);

            mobObj = mobObj->Next;
          }
        }

        if (mobHNode->riding)
        {
          writeRideMobCommand(room, mobHNode, zoneFile);  // also writes eq loads
        }

        writeMobFollowerCommands(mobHNode, zoneFile);  // also writes eq loads

        mobLeader = mobHNode;
        break;
      }

      mobHNode = mobHNode->Next;
    }

   // write rest of mobs

    mobHNode = room->mobHead;

    while (mobHNode)
    {
     // prevent dupe load commands

      if (mobHNode != mobLeader)
      {
        if (!mobHNode->following && !mobHNode->riddenBy)
        {
          writeLoadMobCommand(room, mobHNode, zoneFile);

          for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
          {
            mobEquip = mobHNode->equipment[i];

            writeEquipMobCommand(mobEquip, zoneFile, i);

            if (mobEquip)
              writeAllObjInside(zoneFile, mobEquip->objInside,
                                mobEquip->objectNumb, 0);
          }

          mobObj = mobHNode->objectHead;

          while (mobObj)
          {
            writeGiveMobCommand(mobObj, zoneFile);

            writeAllObjInside(zoneFile, mobObj->objInside, mobObj->objectNumb, 0);

            mobObj = mobObj->Next;
          }
        }

        if (mobHNode->riding)
        {
          writeRideMobCommand(room, mobHNode, zoneFile);  // also writes eq loads
        }

        writeMobFollowerCommands(mobHNode, zoneFile);  // also writes eq loads
      }

      mobHNode = mobHNode->Next;
    }

    room = room->Next;
  }

 // write the end token thingy and close the file

  fputs("*\nS\n", zoneFile);

  fclose(zoneFile);
}
