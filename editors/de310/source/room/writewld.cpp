//
//  File: writewld.cpp   originally part of dikuEdit
//
//  Usage: functions for writing room info to the .WLD info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>

#include "../fh.h"
#include "../types.h"

#include "exit.h"
#include "room.h"

using namespace std;

extern char readFromSubdirs;
extern ulong numbRooms, numbExits;

//
// writeWorldExit : Writes a room exit to worldFile
//
//   *exitNode : pointer to exit record
//   *exitStrn : string that identifies direction exit leads
//  *worldFile : file to write to
//

void writeWorldExit(const roomExit *exitNode, const char *exitStrn,
                    FILE *worldFile)
{
  char strn[256];


  if (!exitNode || !worldFile || !exitStrn)
  {
    cout << "\nError: one of the parameters passed to writeWorldExit() is "
         << "NULL.  Aborting..\n";

    return;
  }

 // first, the exit identifier

  fputs(exitStrn, worldFile);

 // next, write the exit description

  writeStringNodes(worldFile, exitNode->exitDescHead);

 // terminate it

  fputs("~\n", worldFile);

 // write the keyword list

  createKeywordString(exitNode->keywordListHead, strn);
  lowstrn(strn);

  strcat(strn, "\n");

  fputs(strn, worldFile);

 // finally, write the door flag and other info

  fprintf(worldFile, "%u %d %d\n",
          getWorldDoorType(exitNode), exitNode->keyNumb, exitNode->destRoom);
}


//
// writeRoomtoFile : Writes a room to a file
//

void writeRoomtoFile(FILE *worldFile, const dikuRoom *room)
{
  char strn[256];
  extraDesc *descNode;
  int i;


 // first, the room number

  fprintf(worldFile, "#%u\n", room->roomNumber);

 // next, the name of the room

  strcpy(strn, room->roomName);
  nolf(strn);
  strcat(strn, "~\n");

  fputs(strn, worldFile);

 // next, the room description

  writeStringNodes(worldFile, room->roomDescHead);

 // terminate the description

  fputs("~\n", worldFile);

 // next, write the zone number, room flags, and sector type info

  if (getIsMapZoneVal())
    fprintf(worldFile, "%u %u %u %u\n", room->zoneNumber,
                                        room->roomFlags.longIntFlags,
                                        room->sectorType,
                                        room->resourceInfo);
  else
    fprintf(worldFile, "%u %u %u\n", room->zoneNumber,
                                     room->roomFlags.longIntFlags,
                                     room->sectorType);

 // next, the exit info

  for (i = 0; i < NUMB_EXITS; i++)
  {
    if (room->exits[i])
    {
      sprintf(strn, "D%u\n", i);
      writeWorldExit(room->exits[i], strn, worldFile);
    }
  }

 // finally, write all the extra descs

  descNode = room->extraDescHead;

  while (descNode)
  {
    fputs("E\n", worldFile);

   // write the keyword list

    createKeywordString(descNode->keywordListHead, strn);

    strcat(strn, "\n");

    fputs(strn, worldFile);

   // next, the extra description itself

    writeStringNodes(worldFile, descNode->extraDescStrnHead);

   // terminate the description

    fputs("~\n", worldFile);

    descNode = descNode->Next;
  }

 // more finally, write fall percentage, etc.

  if (room->fallChance)
  {
    fputs("F\n", worldFile);

    fprintf(worldFile, "%u\n", room->fallChance);
  }

  if (room->manaFlag || room->manaApply)
  {
    fputs("M\n", worldFile);

    fprintf(worldFile, "%u %d\n", room->manaFlag, room->manaApply);
  }

/*
  if ((room->minLevel) || (room->maxLevel))
  {
    fputs("R\n", worldFile);

    fprintf(worldFile, "%d %d\n", room->minLevel, room->maxLevel);
  }
*/

  if (room->current)
  {
    fputs("C\n", worldFile);

    fprintf(worldFile, "%d %d\n", room->current, room->currentDir);
  }

 // terminate the room info

  fputs("S\n", worldFile);
}


//
// writeWorldFile : Write the world file - contains all the rooms
//

void writeWorldFile(char *filename /*= NULL*/)
{
  FILE *worldFile;
  char worldFilename[81] = "\0";
  char strn[256];
  dikuRoom *room;

  ulong i;


 // assemble the filename of the world file

  if (readFromSubdirs) strcpy(worldFilename, "wld/");

  if (filename) strcat(worldFilename, filename);
  else strcat(worldFilename, getMainZoneNameStrn());

  strcat(worldFilename, ".wld");

 // open the world file for writing

  if ((worldFile = fopen(worldFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << worldFilename << " for writing - aborting\n";

    return;
  }

  sprintf(strn, "Writing %s - %u room%s, %u exit%s\n",
          worldFilename, numbRooms, plural(numbRooms),
                         numbExits, plural(numbExits));

  _outtext(strn);

  for (i = getLowestRoomNumber(); i <= getHighestRoomNumber(); i++)
  {
    if (room = findRoom(i)) writeRoomtoFile(worldFile, room);
  }

  fclose(worldFile);
}
