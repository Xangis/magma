//
//  File: stat.cpp       originally part of dikuEdit
//
//  Usage: functions for handling 'stat' command
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../types.h"
#include "../fh.h"

#include "var.h"

extern command *statCmdHead;

//
// statSumpn
//

void statSumpn(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"stat\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, statCmdHead,
                "\nSpecify one of ROOM, OBJ, or MOB as the first argument.\n\n",
                statExecCommand, FALSE);
}
