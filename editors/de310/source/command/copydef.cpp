//
//  File: copydef.cpp    originally part of dikuEdit
//
//  Usage: function for copying stuff into defaults
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <string.h>

#include "../fh.h"
#include "../types.h"

extern command *copyDefaultCmdHead;

//
// copyDefault
//

void copyDefault(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"copydefault\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, copyDefaultCmdHead,
                "\nSpecify one of ROOM, EXIT, OBJECT, or MOB as the first argument.\n\n",
                copyDefaultExecCommand, FALSE);
}
