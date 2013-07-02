//
//  File: command.cpp    originally part of dikuEdit
//
//  Usage: functions for manipulating command structures (used for the
//         command-parsing functions)
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"
#include "command.h"

using namespace std;

//
// addCommand : adds a command (with string and number) to the list of
//              commands pointed to by *commHead
//
//      commHead : pointer to pointer to head of command list
//          strn : string that makes up command (shouldn't be any spaces)
//          numb : command number
//

void addCommand(command **commHead, const char *instrn, const uchar numb)
{
  command *node = *commHead, *adding;
  char strn[MAX_COMMAND_LEN];


  strncpy(strn, instrn, MAX_COMMAND_LEN);
  strn[MAX_COMMAND_LEN - 1] = 0;

 // check for dupes

  while (node)
  {
    if (!strcmp(node->commandStrn, strn))
    {
      cout << "\n" <<
"Warning: Adding duplicate command name to command list - skipping..\n\n";

      getkey();

      return;
    }

    node = node->Next;
  }

  adding = new command;
  if (!adding)
  {
    cout << "addCommand(): couldn't alloc command - memory must be very low\n";
    exit(1);
  }

  memset(adding, 0, sizeof(command));

 // check string length - if it's too long, truncate it
 // (no need - done instead with strncpy above)

//  if (strlen(strn) >= MAX_COMMAND_LEN) strn[MAX_COMMAND_LEN - 1] = '\0';

  strcpy(adding->commandStrn, strn);
  adding->command = numb;

  if (!*commHead) *commHead = adding;
  else
  {
    node = *commHead;

    while (node && node->Next)
    {
      node = node->Next;
    }

/*  // shouldn't ever happen
    if (!node)
    {
      cout << "\nError in addCommand() - node is NULL.\n";
      return;
    }
*/

    node->Next = adding;
  }
}


//
// checkCommands : runs through a linked list of command structs pointed to
//                 by cmd, checking first for an exact match with strn and
//                 then for a lefthand match
//
//         strn : user input
//          cmd : head of linked list of commands
// notfoundStrn : text displayed if no match is found
//  execCommand : function executed to process command (based on number)
//  noTrimTrail : if TRUE, trailing spaces are not trimmed from strn
//

void checkCommands(const char *strn, command *cmd, const char *notfoundStrn,
                   const void (*execCommand)(const uchar, const char *),
                   const char noTrimTrail)
{
  char strn2[256], commandStrn[256], args[256] = "\0";
  usint i, len, rightptr = 0;
  command *cmdorig = cmd;


  strcpy(strn2, strn);

  remLeadingSpaces(strn2);
  if (!noTrimTrail) remTrailingSpaces(strn2);

  len = strlen(strn2);

  if (!len)
  {
    displayColorString(notfoundStrn);
    return;
  }

 // get left bit - part before spaces (if any)

  for (i = 0; i < len; i++)
  {
    if (strn2[i] == ' ') break;

    commandStrn[i] = strn2[i];
  }

  commandStrn[i] = '\0';

 // get right bit - part after spaces (again, if any)

  for (; i < len; i++)
  {
    args[rightptr++] = strn2[i];
  }

  args[rightptr] = '\0';
//  remTrailingSpaces(args);
  remLeadingSpaces(args);

 // first, check for exact match - then, check for left-hand match

  while (cmd)
  {
    if (!strcmp(cmd->commandStrn, commandStrn))
    {
      execCommand(cmd->command, args);
      return;
    }

    cmd = cmd->Next;
  }

  cmd = cmdorig;

  while (cmd)
  {
    if (strleft(cmd->commandStrn, commandStrn))
    {
      execCommand(cmd->command, args);
      return;
    }

    cmd = cmd->Next;
  }

  displayColorString(notfoundStrn);
}
