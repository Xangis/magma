//
//  File: interp.cpp     originally part of dikuEdit
//
//  Usage: contains heart of command interpreter
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <string.h>

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  ifdef _WIN32
#    include <mapiutil.h>
#    include <mapiwin.h>
#    include <wincon.h>
#  endif

#  include "graphcon.h"
#endif

#include "fh.h"
#include "types.h"

#include "vardef.h"
#include "de.h"
#include "command/alias.h"

extern dikuRoom *currentRoom;
extern variable *varHead;
extern command *cmdHead;
extern alias *aliasHead;
extern ulong commandsEntered;

//
// interpCommands : Grab a string from the user and interpret it, taking
//                  action based on what the user typed
//

void interpCommands(const char *inputStrn, const char dispPrompt)
{
  char strn[MAX_PROMPTINPUT_LEN + 1], parseStrn[MAX_PROMPTINPUT_LEN + 1],
       parseOn = FALSE;
  ulong parsepos, oldpos;


 // grab the string, woowoo

  if (!currentRoom) _outtext("\n\nERROR - currentRoom is NULL\n\n");

  if (!inputStrn)
    getStrn(strn, MAX_PROMPTINPUT_LEN, 0, 7, ' ', "\0", TRUE, TRUE);
  else
    strncpy(strn, inputStrn, MAX_PROMPTINPUT_LEN);

  nolf(strn);
  remLeadingSpaces(strn);

 // if line has a semicolon in it, parse each section individually

  if (strstr(strn, ";"))
  {
    parseOn = TRUE;
//    strcpy(parseStrn, strn);

    parsepos = 0;
    while (strn[parsepos] && (strn[parsepos] != ';')) parsepos++;

    if (parsepos) strnSect(strn, parseStrn, 0, parsepos - 1);
    else parseStrn[0] = '\0';

    remTrailingSpaces(parseStrn);
/*
    _outtext(parseStrn);
    _outtext("\n");
*/
//    parsepos++;
    if (strn[parsepos]) parsepos++;
  }
  else strcpy(parseStrn, strn);

  do
  {
   // check for '!' (repeat last command)

    if (!strcmp(strn, "!")) strcpy(parseStrn, getLastCommandStrn());
    else if (strn[0]) addVar(&varHead, VAR_LASTCOMMAND_NAME, parseStrn);

    upfirstarg(parseStrn);

    checkAliases(parseStrn, aliasHead);
    checkCommands(parseStrn, cmdHead, MAIN_PROMPT_COMM_NOT_FOUND,
                  mainExecCommand, TRUE);

    if (parseOn)
    {
      oldpos = parsepos;

      while (strn[parsepos] && (strn[parsepos] != ';')) parsepos++;

      if (parsepos == oldpos) parseOn = FALSE;
      else
      {
        strnSect(strn, parseStrn, oldpos, parsepos - 1);
        remTrailingSpaces(parseStrn);
        remLeadingSpaces(parseStrn);
/*
        _outtext(parseStrn);
        _outtext("\n");
*/
        if (strn[parsepos]) parsepos++;
      }
    }
  } while (parseOn);

  if (getSaveEveryXCommandsVal())
  {
    commandsEntered++;

    if (commandsEntered >= getSaveHowOftenVal())
    {
      commandsEntered = 0;

      displayColorString("&nAutosaving...");

      writeFiles();
    }
  }

  if (dispPrompt) displayPrompt();
}
