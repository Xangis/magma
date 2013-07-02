//
//  File: load.cpp       originally part of dikuEdit
//
//  Usage: interpreter for handling 'load' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../types.h"
#include "../fh.h"

#include "command.h"

extern command *loadCmdHead;

//
// loadSumpn
//

void loadSumpn(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"load\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, loadCmdHead,
                "\nSpecify one of OBJ or MOB as the first argument.\n\n",
                loadExecCommand, FALSE);
}
