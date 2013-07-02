//
//  File: editrflg.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing the room flags bitvector
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "room.h"

extern _CONST flagDef roomFlagDef[];
extern ulong roomFlagTemplates[];

//
// editRoomFlags : Edit room flags - "main" function
//
//   *room : room to edit
//

void editRoomFlags(dikuRoom *room)
{
  char strn[256], newName[256];

  fixRoomName(room->roomName, newName);

  sprintf(strn, "&+gEditing flags for room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          room->roomNumber, newName);

  editFlags(roomFlagDef, &(room->roomFlags.longIntFlags), strn, "room flags",
            roomFlagTemplates, 3);
}
