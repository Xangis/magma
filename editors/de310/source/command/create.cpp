//
//  File: create.cpp     originally part of dikuEdit
//
//  Usage: interpreter function for handling 'create' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../types.h"
#include "../fh.h"

extern command *createCmdHead;

//
// createSumpn
//

void createSumpn(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"create\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, createCmdHead,
                "\nSpecify one of ROOM, OBJ, or MOB as the first argument.\n\n",
                createExecCommand, FALSE);
}
