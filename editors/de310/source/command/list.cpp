//
//  File: list.cpp       originally part of dikuEdit
//
//  Usage: interpreter for 'list' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"

extern command *listCmdHead;
extern dikuRoom *currentRoom;

//
// list
//

void list(const char *args)
{
  if (strlen(args) == 0)
  {
   // check for shop in the same room

    if (!listShopSold(currentRoom))
      _outtext("\n"
"The \"list\" command requires at least one argument (or a shop in the room).\n\n");

    return;
  }

  checkCommands(args, listCmdHead, "\n"
"Specify one of <room|obj|mob|objhere/oh|mobhere/mh|quest|shop|loaded|lookup> as\n"
"the first argument.\n\n", listExecCommand, FALSE);
}
