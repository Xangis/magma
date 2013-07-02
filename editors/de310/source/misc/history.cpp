//
//  File: history.cpp    originally part of dikuEdit
//
//  Usage: functions for manipulating command-line history
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <string.h>

#include "../types.h"
#include "../fh.h"
#include "strnnode.h"

using namespace std;

//
// addCommand : Adds a "command" to the history list pointed to by
//              *commandHistory
//
//   *strn : command to add
//

void addCommand(const char *strn, stringNode **commandHistory)
{
  stringNode *strnNode, *oldStrnNode = *commandHistory;


 // don't add empty strings

  if (strn[0] == '\0') return;

  strnNode = new stringNode;
  if (!strnNode)
  {
    cout << "\n\nError: addCommand() ran out of mem alloc'ing stringNode."
            "  Aborting..\n\n";

    return;
  }

  memset(strnNode, 0, sizeof(stringNode));

  strcpy(strnNode->string, strn);

  if (!*commandHistory) *commandHistory = strnNode;
  else
  {
    while (oldStrnNode)
    {
      if (!oldStrnNode->Next) break;

      oldStrnNode = oldStrnNode->Next;
    }

    if (oldStrnNode)
    {
      oldStrnNode->Next = strnNode;
      strnNode->Last = oldStrnNode;
    }
    else
    {
      _outtext("addCommand(): couldn't find oldStrnNode\n\n");
      return;
    }
  }
}
