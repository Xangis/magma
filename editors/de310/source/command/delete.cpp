//
//  File: delete.cpp     originally part of dikuEdit
//
//  Usage: interpreter for handling 'delete' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"

extern command *deleteCmdHead;

//
// deleteSumpn
//

void deleteSumpn(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"delete\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, deleteCmdHead, "\n"
"Specify one of ROOM, OBJ, MOB, or a whole bunch of other shit as the first\n"
"argument.\n\n", deleteExecCommand, FALSE);
}
