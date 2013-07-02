//
//  File: readwld.cpp    originally part of dikuEdit
//
//  Usage: functions for reading room info from the .WLD file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "room.h"
#include "exit.h"
#include "../edesc/edesc.h"
#include "../vardef.h"

using namespace std;

extern ulong numbExits, numbLookupEntries, numbRooms, lowestRoomNumber, highestRoomNumber;
extern dikuRoom **roomLookup, *roomHead;
extern char madeChanges, readFromSubdirs;
extern variable *varHead;

//
// readRoomExtraDesc : Reads an extra description for a specified room -
//                     returns the address of the new extra description node
//
//    *worldFile        : world file info
//    *roomNode         : pointer to current room node
//    *extraDescOldNode : pointer to previous extraDesc node, if any
//

extraDesc *readRoomExtraDesc(FILE *worldFile, dikuRoom *roomNode,
                             extraDesc *extraDescOldNode)
{
  extraDesc *extraDescNode;
  char strn[256];


 // create new extra description node

  extraDescNode = new extraDesc;
  if (!extraDescNode)
  {
    cout << "Error allocating memory for extraDescNode - aborting\n";

    exit(1);
  }

 // set stuff

  memset(extraDescNode, 0, sizeof(extraDesc));

  if (extraDescOldNode) extraDescOldNode->Next = extraDescNode;
  extraDescNode->Last = extraDescOldNode;

  if (!roomNode->extraDescHead)
  {
    roomNode->extraDescHead = extraDescNode;
  }

 // read the extra description keyword list - all on one line

  if (!fgets(strn, MAX_EDESCKEY_LEN, worldFile))
  {
    cout << "Unexpected EOF while looking for extra desc. keyword list "
         << "for room #" << roomNode->roomNumber << "\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire keyword string for extra desc for room\n" <<
"        #" << roomNode->roomNumber << " - max length that can be read is " << MAX_EDESCKEY_LEN << ".\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

    exit(1);
  }

 // remove linefeeds and check for tilde at the end of the keyword strn

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strn[strlen(strn) - 1] != '~')
  {
    cout << "\n" <<
"Error: Keyword list for one of room #" << roomNode->roomNumber << "'s extra descs doesn't\n" <<
"       end in a ~ (string read is '" << strn << "').\n";

    exit(1);
  }

 // generate a keyword list with this string

  extraDescNode->keywordListHead = createKeywordList(strn);

 // now, read the actual description

  extraDescNode->extraDescStrnHead = readStringNodes(worldFile, TILDE_LINE, FALSE);


  return extraDescNode;
}


//
// readRoomExit : Reads a dikuRoom exit, basically
//
//  *worldFile : pointer to world file
//   *roomNode : pointer to room that exit belongs to
//   *exitStrn : string as read from worldFile that contains exit info
//

roomExit *readRoomExit(FILE *worldFile, dikuRoom *roomNode,
                       const char *exitStrn, const char incNumbExits)
{
  roomExit *roomExitRec;
  char currentExit, strn[256];
  char dtype;


 // make sure that the string is at least the right length

  if ((strlen(exitStrn) != 2) || (exitStrn[0] != 'D'))
  {
    cout << "\n" <<
"Error: No or incorrect exit type specified in exit data for room number\n" <<
"       " << roomNode->roomNumber << ".\n";

    exit(1);
  }

 // allocate memory for the roomExitRec

  roomExitRec = new roomExit;
  if (!roomExitRec)
  {
    cout << "Error allocating memory for roomExit record - aborting..\n";

    exit(1);
  }

 // clear it

  memset(roomExitRec, 0, sizeof(roomExit));

 // set currentExit

  currentExit = exitStrn[1];
  if ((currentExit < EXIT_FIRST_CH) || (currentExit > EXIT_LAST_CH))
  {
     cout << "\n" <<
"Error: Invalid exit direction for room #" << roomNode->roomNumber << " specified ('" << currentExit << "')\n" <<
"       Aborting..\n";
     exit(1);
  }

 // read the exit description

  roomExitRec->exitDescHead = readStringNodes(worldFile, TILDE_LINE, TRUE);

 // read keyword string for exit

  if (!fgets(strn, MAX_EXITKEY_LEN, worldFile))
  {
    cout << "\n"
"Error: Hit EOF trying to read keyword list for exit - this is room\n" <<
"       #" << roomNode->roomNumber << ", by the way.  Aborting.\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire keyword string for exit #" << currentExit << " of room\n" <<
"        #" << roomNode->roomNumber << " - max length that can be read is " << MAX_EXITKEY_LEN << ".\n" <<
"        aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strn[strlen(strn) - 1] != '~')
  {
    cout << "\n" <<
"Error: Exit keyword list for room #" << roomNode->roomNumber << ", exit #" << currentExit << "\n"
"       doesn't terminate with a '~' (string read was '" << strn << "').\n" <<
"       Aborting..\n";

    exit(1);
  }

 // create a keyword list

  roomExitRec->keywordListHead = createKeywordList(strn);

 // read the rest of the exit info

  fgets(strn, 256, worldFile);  // no need to check for errors

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire misc info string for exit #" << currentExit << " of room\n" <<
"        #" << roomNode->roomNumber << " - max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

    exit(1);
  }

  if (numbArgs(strn) != 3)
  {
    cout << "\n"
"Error: Number of fields in line specifying exit info for room #" << roomNode->roomNumber << "\n" <<
"       exit #" << currentExit << " incorrect (" << numbArgs(strn) << " instead of 3).\n\n";

    exit(1);
  }

  sscanf(strn, "%d%d%d\n",
         &(roomExitRec->worldDoorType), &(roomExitRec->keyNumb),
         &(roomExitRec->destRoom));

 // check door type..  ya never know

  dtype = roomExitRec->worldDoorType;

  if ((dtype < 0) || (dtype > 15))
  {
    cout << "\n" <<
"Error: door type specified for exit #" << currentExit << " in room #" << roomNode->roomNumber << "\n" <<
"       invalid because the door type specified is an impossible value (" << (int)(dtype) << ").\n" <<
"       Aborting..\n";

    exit(1);
  }

 // Done reading exit description and info, now put the address of the
 // "temp" record in the corresponding record member thingy

  if (roomNode)
  {
    currentExit -= '0';
    roomNode->exits[currentExit] = roomExitRec;
  }

  if (incNumbExits) numbExits++;

  return roomExitRec;
}


//
// readRoomFromFile : Returns pointer to new roomOldNode
//

dikuRoom *readRoomFromFile(FILE *worldFile, const uchar checkDupes,
                           const uchar incNumbRooms)
{
  char strn[256];
  dikuRoom *roomNode;
  extraDesc *extraDescOldNode = NULL;
  long i;


 // Read the room number - read past blank lines

  while (TRUE)
  {
    if (!fgets(strn, 256, worldFile)) return NULL;  // hit EOF

    if (!lastCharLF(strn))  // holy shit - that's one long vnum
    {
      cout << "\n" <<
"error: line in .WLD file is too long - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    if (strlen(strn)) break;
  }

 // allocate memory for dikuRoom

  roomNode = new dikuRoom;
  if (!roomNode)
  {
    cout << "Uh-oh, ran out of memory allocating memory for a new dikuRoom."
            "  Aborting..\n";

    exit(1);
  }

 // set everything in room record to 0/NULL

  memset(roomNode, 0, sizeof(dikuRoom));

 // get room vnum

  if (strn[0] != '#')
  {
    cout << "\n" <<
"Line for room that should have '#' and vnum doesn't - string read was\n"
"'" << strn << "'.  Aborting..\n";

    exit(1);
  }

  deleteChar(strn, 0);

  i = atoi(strn);

 // if incNumbRooms is TRUE, assume room is being read into main room list
 // and not a default room or other neato crap

  if ((i < 0) && incNumbRooms)
  {
    cout << "\n" <<
"Error: room has vnum of " << i << " - negative vnums are not allowed.  aborting..\n";

    exit(1);
  }

  if (checkDupes && findRoom(i))
  {
    cout << "\n" <<
"Error: Room #" << i << " has more than one entry in the .WLD file.\n" <<
"       Aborting..\n";

    exit(1);
  }

  if (i >= numbLookupEntries)
  {
    cout << "\n" <<
"Error: room #" << i << "'s vnum is too high - only vnums up to " << (numbLookupEntries - 1) << " are allowed.\n" <<
"       To remedy, load dikuEdit with the -ll=<vnum limit> command-line switch.\n\n";

    exit(1);
  }

  roomNode->roomNumber = i;


 // Now, read the room name - this should be one line, but yet has a tilde
 // at the end anyway.  Go figure.

  if (!fgets(strn, MAX_ROOMNAME_LEN, worldFile))
  {
    cout << "Data for room #" << roomNode->roomNumber << " not complete - "
         << "hit EOF while reading room name.  Aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);

  i = strlen(strn) - 1;  // it's handy

  if (strn[i] != '~')
  {
    cout << "\n" <<
"Error: Data for room #" << roomNode->roomNumber << " not complete -\n" <<
"       room name doesn't end in ~.  Aborting..\n";

    exit(1);
  }

  strn[i] = '\0';  // get rid of the tilde
  remTrailingSpaces(strn);

  strcpy(roomNode->roomName, strn);

 // read the room description

  roomNode->roomDescHead = readStringNodes(worldFile, TILDE_LINE, TRUE);

 // read miscellaneous room info

 // get rid of blank lines - some zone writers like to stick blank lines
 // after the room desc (shrug)

  do
  {
    if (!fgets(strn, 256, worldFile))
    {
      cout << "\n" <<
"Error - hit EOF while removing blank lines after description for room #"
           << roomNode->roomNumber << " -\n" <<
"        aborting...\n";

      exit(1);
    }

    remLeadingSpaces(strn);
  } while (!strcmp(strn, "\n"));

//  fgets(strn, 256, worldFile);

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire misc info string after room desc for room #" << roomNode->roomNumber << " -\n" <<
"        max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

    exit(1);
  }

  if ((numbArgs(strn) != 3) && (numbArgs(strn) != 4))
  {
    cout << "\n"
"Error: Number of fields in line specifying misc info for room #" << roomNode->roomNumber << "\n" <<
"       incorrect (" << (usint)(numbArgs(strn)) << " instead of 3 or 4).\n\n";

    exit(1);
  }

  if (numbArgs(strn) == 3)
  {
    sscanf(strn, "%u%u%u\n",
           &(roomNode->zoneNumber), &(roomNode->roomFlags.longIntFlags),
           &(roomNode->sectorType));
  }
  else
  {
    sscanf(strn, "%u%u%u%u\n",
           &(roomNode->zoneNumber), &(roomNode->roomFlags.longIntFlags),
           &(roomNode->sectorType), &(roomNode->resourceInfo));

   // assume that 4-arg line means this is a map zone

    setVarBoolVal(&varHead, VAR_ISMAPZONE_NAME, TRUE, TRUE); 
  }

 // Now, check for either a D, E, or S - D is exit, E is extra description, S
 // signifies end of room data - keep checking for Ds and Es until we hit an
 // S of death

  while (TRUE)
  {
    if (!fgets(strn, 256, worldFile))
    {
      cout << "\nUnexpected EOF while looking for D, E, S, F, M, or C for room #"
           << roomNode->roomNumber << "\n";

      exit(1);
    }

    if (!lastCharLF(strn))  // couldn't read whole string
    {
      cout << "\n" <<
"Error - couldn't read entire string specifying D, E, S, F, M, or C for\n" <<
"        room #" << roomNode->roomNumber << " - max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);

    if (!strcmp(strn, "\0")) // empty string
    {
      continue;
    }
    else
    if (!strcmp(strn, "S"))  // end of room info
    {
      break;
    }
    else
    if (!strcmp(strn, "E"))  // extra description
    {
      extraDescOldNode = readRoomExtraDesc(worldFile, roomNode,
                                           extraDescOldNode);
    }
    else
    if (strn[0] == 'D')      // exit
    {
      readRoomExit(worldFile, roomNode, strn, TRUE);
    }
    else
    if (!strcmp(strn, "F"))  // fall percentage
    {
      fgets(strn, 256, worldFile);

      if (!lastCharLF(strn))  // couldn't read whole string
      {
        cout << "\n" <<
"Error - couldn't read entire string specifying fall percentage info for\n" <<
"        room #" << roomNode->roomNumber << " - max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

        exit(1);
      }

      if (numbArgs(strn) != 1)
      {
        cout << "\n"
"Error: Number of fields in line specifying fall percentage for room #" << roomNode->roomNumber << "\n" <<
"       incorrect (" << numbArgs(strn) << " instead of 1).\n\n";

        exit(1);
      }

      sscanf(strn, "%d\n", &(roomNode->fallChance));
    }
    else
    if (!strcmp(strn, "M"))  // mana apply stuff
    {
      fgets(strn, 256, worldFile);

      if (!lastCharLF(strn))  // couldn't read whole string
      {
        cout << "\n" <<
"Error - couldn't read entire string specifying mana info for room #" << roomNode->roomNumber << " -\n" <<
"        max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

        exit(1);
      }

      if (numbArgs(strn) != 2)
      {
        cout << "\n"
"Error: Number of fields in line specifying mana info for room #" << roomNode->roomNumber << "\n" <<
"       incorrect (" << numbArgs(strn) << " instead of 2).\n\n";

        exit(1);
      }

      sscanf(strn, "%u%d\n", &(roomNode->manaFlag), &(roomNode->manaApply));
    }
    else
    if (!strcmp(strn, "R"))  // min level max level stuff - lose it forever
    {
      fgets(strn, 256, worldFile);

      if (!lastCharLF(strn))  // couldn't read whole string
      {
        cout << "\n" <<
"Error - couldn't read entire string specifying min/max level info for\n" <<
"        room #" << roomNode->roomNumber << " - max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

        exit(1);
      }

      if (numbArgs(strn) != 2)
      {
        cout << "\n"
"Error: Number of fields in line specifying level info for room #" << roomNode->roomNumber << "\n" <<
"       incorrect (" << (usint)(numbArgs(strn)) << " instead of 2).\n\n";

        exit(1);
      }

//      sscanf(strn, "%d%d\n", &i, &i);  // why bother
    }
    else
    if (!strcmp(strn, "C"))  // current
    {
      fgets(strn, 256, worldFile);

      if (!lastCharLF(strn))  // couldn't read whole string
      {
        cout << "\n" <<
"Error - couldn't read entire string specifying current info for\n" <<
"        room #" << roomNode->roomNumber << " - max length that can be read is 256.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

        exit(1);
      }

      if (numbArgs(strn) != 2)
      {
        cout << "\n"
"Error: Number of fields in line specifying current info for room #" << roomNode->roomNumber << "\n" <<
"       incorrect (" << (usint)(numbArgs(strn)) << " instead of 2).\n\n";

        exit(1);
      }

      sscanf(strn, "%d%d\n", &(roomNode->current), &(roomNode->currentDir));
    }
    else
    {
      cout << "\n" <<
"Unrecognized extra stuff - \"" << strn << "\" found in\n" <<
"data for room #" << roomNode->roomNumber << " - aborting..\n";

      exit(1);
    }
  }

  if (incNumbRooms)  // assume we're adding to global list
  {
    numbRooms++;

    i = roomNode->roomNumber;

    roomLookup[i] = roomNode;

    if (i > highestRoomNumber) highestRoomNumber = i;
    if (i < lowestRoomNumber)  lowestRoomNumber = i;
  }

  return roomNode;
}


//
// readWorldFile : Reads rooms from the world file
//

char readWorldFile(const char *filename /* = NULL in fh.h */)
{
  FILE *worldFile;
  char worldFilename[81] = "\0";

  dikuRoom *roomOldNode = NULL, *room;

  long lastRoom = 0;


 // assemble the filename of the world file

  if (readFromSubdirs) strcpy(worldFilename, "wld/");

  if (filename) strcat(worldFilename, filename);
  else strcat(worldFilename, getMainZoneNameStrn());

  strcat(worldFilename, ".wld");

 // open the world file for reading

  if ((worldFile = fopen(worldFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << worldFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << worldFilename << "...\n";

 // this while loop reads room by room, one room per iteration

  while (!feof(worldFile))
  {
    room = readRoomFromFile(worldFile, TRUE, TRUE);
    if (!room) break;  // eof

    if (!roomHead) roomHead = room;

    room->Last = roomOldNode;
    if (roomOldNode) roomOldNode->Next = room;
    roomOldNode = room;

    if (room->roomNumber < lastRoom)
    {
      cout << "\n" <<
"Warning: room numbers out of order - rooms #" << lastRoom <<
" and #" << room->roomNumber << ".\n";

      madeChanges = TRUE;

      getkey();
    }
    else lastRoom = room->roomNumber;
  }

  fclose(worldFile);
  return TRUE;
}
