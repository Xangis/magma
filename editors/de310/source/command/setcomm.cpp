//
//  File: setcomm.cpp    originally part of dikuEdit
//
//  Usage: functions for handling reading of .set files
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "setcomm.h"
#include "../vardef.h"
#include "alias.h"

extern alias *aliasHead;

//
// setExecCommandFile
//

const void setExecCommandFile(const uchar command, const char *args)
{
  char args2[256], check[256];


  strcpy(args2, args);
  strcpy(check, args);

  if (command != SETCMD_SET) remTrailingSpaces(args2);
  else
  {
   // don't process SET MAINZONENAME command, cuz it screws up paths while
   // loading

    upstrn(check);
    if (strleft(check, VAR_MAINZONENAME_NAME)) return;
  }

  switch (command)
  {
    case SETCMD_SET     : setVarArgs(args2, FALSE, FALSE); break;
    case SETCMD_ALIAS   : addAliasArgs(args2, FALSE, FALSE, &aliasHead); break;
    case SETCMD_LIMIT   : setLimitArgsStartup(args2); break;
    case SETCMD_SETTEMP : setTemplateArgs(args2, FALSE, FALSE); break;
//    case SETCMD_RANDOM : setRandomArgs(args2); break;

    default : _outtext(
"\nsetExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
//
//

/*
void setExecCommandPrompt(uchar command, char *args)
{
  switch (command)
  {
//    case SETCMD_SET    : setVarArgs(args); break;
    case SETCMD_ALIAS  : addAliasArgs(args, TRUE); break;
//    case SETCMD_LIMIT  : setLimitArgs(args); break;

    default : _outtext(
"\nsetExecCommand() - internal error (no match found for command in list)\n\n");
  }
}
*/
