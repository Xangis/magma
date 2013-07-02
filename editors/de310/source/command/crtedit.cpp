//
//  File: crtedit.cpp    originally part of dikuEdit
//
//  Usage: interpreter functions for handling 'createedit' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

extern command *createEditCmdHead;
extern dikuRoom *currentRoom;
extern _CONST char *exitnames[];

//
// createEdit
//

void createEdit(const char *args)
{
  if (strlen(args) == 0)
  {
    _outtext("\nThe \"createedit\" command requires at least one argument.\n\n");
    return;
  }

  checkCommands(args, createEditCmdHead, "\n"
"Specify one of <room|obj|mob|exit> as the first argument.\n\n",
                createEditExecCommand, FALSE);
}


//
// createEditExit
//

void createEditExit(const char *args)
{
  long returnVal, val;


  if (!strlen(args))
  {
    returnVal = createRoomExit(currentRoom, USER_CHOICE);
  }
  else
  {
    val = getDirfromKeyword(args);
    if (val == NO_EXIT)
    {
      _outtext("\nCreate and edit which exit?\n\n");
      return;
    }

    returnVal = createRoomExit(currentRoom, val);
  }

  if (returnVal == NO_EXIT)
  {
    return;
  }

  if ((returnVal < 0) || (returnVal >= NUMB_EXITS))
  {
    _outtext("\nBig time error in createEditExit() - notify the author\n\n");
    return;
  }

  editExit(currentRoom, &currentRoom->exits[returnVal], exitnames[returnVal],
           TRUE);
}
