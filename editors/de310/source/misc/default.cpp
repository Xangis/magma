//
//  File: default.cpp    originally part of dikuEdit
//
//  Usage: functions related to default rooms, exits, etc
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../system.h"
#include "../keys.h"

#include "../room/room.h"
#include "../obj/object.h"
#include "../mob/mob.h"

using namespace std;

extern dikuRoom *defaultRoom;
extern dikuObject *defaultObject;
extern dikuMob *defaultMob;
extern roomExit *defaultExit;
extern char madeChanges;

//
// readDefaultsFromFiles
//

void readDefaultsFromFiles(const char *filePrefix)
{
  FILE *defaultFile;
  char strn[256] = "\0", fileName[256];
     // dummy strn for reading obj


  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_WLD_EXT);

 // read default room first

  defaultFile = fopen(fileName, "rt");

  if (defaultFile)
  {
    cout << "\nReading room default in " << fileName << "...\n";

    defaultRoom = readRoomFromFile(defaultFile, FALSE, FALSE);

/*
    deleteRoomExit(defaultRoom->north, FALSE);
    deleteRoomExit(defaultRoom->south, FALSE);
    deleteRoomExit(defaultRoom->west, FALSE);
    deleteRoomExit(defaultRoom->east, FALSE);
    deleteRoomExit(defaultRoom->up, FALSE);
    deleteRoomExit(defaultRoom->down, FALSE);

    defaultRoom->north = defaultRoom->south = defaultRoom->west =
    defaultRoom->east  = defaultRoom->up    = defaultRoom->down = NULL;
*/

    defaultRoom->roomNumber = 0;
    defaultRoom->defaultRoom = TRUE;

    fclose(defaultFile);
  }

 // read default object

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_OBJ_EXT);

  defaultFile = fopen(fileName, "rt");

  if (defaultFile)
  {
    cout << "Reading object default in " << fileName << "...\n";

    defaultObject = readObjectFromFile(defaultFile, strn, FALSE, FALSE);

    defaultObject->objNumber = 0;
    defaultObject->defaultObj = TRUE;

    fclose(defaultFile);
  }

 // read default mob

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_MOB_EXT);

  defaultFile = fopen(fileName, "rt");

  if (defaultFile)
  {
    cout << "Reading mob default in " << fileName << "...\n";

    defaultMob = readMobFromFile(defaultFile, FALSE, FALSE);

    defaultMob->mobNumber = 0;
    defaultMob->defaultMob = TRUE;

    fclose(defaultFile);
  }

 // read default exit

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_EXIT_EXT);

  defaultFile = fopen(fileName, "rt");

  if (defaultFile)
  {
    cout << "Reading exit default in " << fileName << "...\n";

    fgets(strn, 256, defaultFile);
    nolf(strn);
    defaultExit = readRoomExit(defaultFile, NULL, strn, FALSE);

    fclose(defaultFile);
  }
}


//
// writeDefaultFiles
//

void writeDefaultFiles(const char *filePrefix)
{
  FILE *defaultFile;
  char fileName[256], strn[256];


  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_WLD_EXT);

  if (defaultRoom)
  {
    sprintf(strn, "Writing room default to %s...\n", fileName);
    _outtext(strn);

    defaultFile = fopen(fileName, "wt");

    writeRoomtoFile(defaultFile, defaultRoom);

    fclose(defaultFile);
  }
  else  // delete old default, if any
  {
    remove(fileName);
  }

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_OBJ_EXT);

  if (defaultObject)
  {
    sprintf(strn, "Writing object default to %s...\n", fileName);
    _outtext(strn);

    defaultFile = fopen(fileName, "wt");

    writeObjecttoFile(defaultFile, defaultObject);

    fclose(defaultFile);
  }
  else  // delete old default, if any
  {
    remove(fileName);
  }

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_MOB_EXT);

  if (defaultMob)
  {
    sprintf(strn, "Writing mob default to %s...\n", fileName);
    _outtext(strn);

    defaultFile = fopen(fileName, "wt");

    writeMobtoFile(defaultFile, defaultMob);

    fclose(defaultFile);
  }
  else  // delete old default, if any
  {
    remove(fileName);
  }

  strcpy(fileName, filePrefix);
  strcat(fileName, DEF_EXIT_EXT);

  if (defaultExit)
  {
    sprintf(strn, "Writing exit default to %s...\n", fileName);
    _outtext(strn);

    defaultFile = fopen(fileName, "wt");

    writeWorldExit(defaultExit, "D0\n", defaultFile);

    fclose(defaultFile);
  }
  else  // delete old default, if any
  {
    remove(fileName);
  }

  _outtext("\n");
}


//
// editDefaultRoom
//

void editDefaultRoom(void)
{
  usint ch;


  if (defaultRoom) editRoom(defaultRoom, FALSE);
  else
  {
    displayColorString("\n&+cNo default room exists - create one (&+CY/n&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'N') && (ch != 'Y') && (ch != K_Enter));

    if (ch != 'N')
    {
      displayColorString("Yes&n\n\n");

      defaultRoom = createRoom(FALSE, 0);
      defaultRoom->defaultRoom = TRUE;

      editRoom(defaultRoom, FALSE);

      madeChanges = TRUE;
    }
    else displayColorString("No&n\n\n");
  }
}


//
// editDefaultObject
//

void editDefaultObject(void)
{
  usint ch;


  if (defaultObject) editObjType(defaultObject, FALSE);
  else
  {
    displayColorString("\n&+cNo default object type exists - create one (&+CY/n&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'N') && (ch != 'Y') && (ch != K_Enter));

    if (ch != 'N')
    {
      displayColorString("Yes&n\n\n");

      defaultObject = createObjectType(FALSE, 0);
      defaultObject->defaultObj = TRUE;

      editObjType(defaultObject, FALSE);

      madeChanges = TRUE;
    }
    else displayColorString("No&n\n\n");
  }
}


//
// editDefaultMob
//

void editDefaultMob(void)
{
  usint ch;


  if (defaultMob) editMobType(defaultMob, FALSE);
  else
  {
    displayColorString("\n&+cNo default mob type exists - create one (&+CY/n&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'N') && (ch != 'Y') && (ch != K_Enter));

    if (ch != 'N')
    {
      displayColorString("Yes&n\n\n");

      defaultMob = createMobType(FALSE, 0);
      defaultMob->defaultMob = TRUE;

      editMobType(defaultMob, FALSE);

      madeChanges = TRUE;
    }
    else displayColorString("No&n\n\n");
  }
}


//
// editDefaultExit
//

void editDefaultExit(void)
{
  usint ch;
  dikuRoom *room;


  room = new dikuRoom;
  if (!room) return;

  memset(room, 0, sizeof(dikuRoom));
  strcpy(room->roomName, "(def exit - there is no room)");

  if (defaultExit) editExit(room, &defaultExit, "default", TRUE);
  else
  {
    displayColorString("\n&+cNo default exit exists - create one (&+CY/n&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'N') && (ch != 'Y') && (ch != K_Enter));

    if (ch != 'N')
    {
      displayColorString("Yes&n\n\n");

      createExit(&defaultExit, FALSE);
      editExit(room, &defaultExit, "(default)", TRUE);

//      numbExits--;

      madeChanges = TRUE;
    }
    else displayColorString("No&n\n\n");
  }

  delete room;
}


//
// deleteDefaultRoom
//

void deleteDefaultRoom(void)
{
  if (defaultRoom)
  {
    deleteRoomInfo(defaultRoom, FALSE, FALSE, TRUE);

    defaultRoom = NULL;

    madeChanges = TRUE;

    displayColorString("\n&+cDefault room deleted.&n\n\n");
  }
  else
  {
    displayColorString("\n&+cNo default room info to delete.&n\n\n");
  }
}


//
// deleteDefaultObject
//

void deleteDefaultObject(void)
{
  if (defaultObject)
  {
    deleteObjectType(defaultObject, FALSE);

    defaultObject = NULL;

    madeChanges = TRUE;

    displayColorString("\n&+cDefault object deleted.&n\n\n");
  }
  else
  {
    displayColorString("\n&+cNo default object info to delete.&n\n\n");
  }
}


//
// deleteDefaultMob
//

void deleteDefaultMob(void)
{
  if (defaultMob)
  {
    deleteMobType(defaultMob, FALSE);

    defaultMob = NULL;

    madeChanges = TRUE;

    displayColorString("\n&+cDefault mob deleted.&n\n\n");
  }
  else
  {
    displayColorString("\n&+cNo default mob info to delete.&n\n\n");
  }
}


//
// deleteDefaultExit
//

void deleteDefaultExit(void)
{
  if (defaultExit)
  {
    deleteRoomExit(defaultExit, FALSE);

    defaultExit = NULL;

    madeChanges = TRUE;

    displayColorString("\n&+cDefault exit deleted.&n\n\n");
  }
  else
  {
    displayColorString("\n&+cNo default exit info to delete.&n\n\n");
  }
}
