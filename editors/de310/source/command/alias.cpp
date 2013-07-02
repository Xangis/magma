//
//  File: alias.cpp      originally part of dikuEdit
//
//  Usage: functions for implementing and interpreting alias system
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#include "alias.h"

using namespace std;

extern char madeChanges, display;
extern variable *varHead;

//
// unaliasCmd : remove an alias from the list pointed to by aliasHead
//              based on argument list in args
//
//        args : arguments to command
//   aliasHead : pointer to head of list that hopefully contains alias
//

void unaliasCmd(const char *args, alias **aliasHead)
{
  alias *node = *aliasHead, *old = NULL;
  char strn[1024];

  if (numbArgs(args) != 1)
  {
    _outtext("\nThe 'unalias' command takes one argument - the alias to delete.\n\n");
    return;
  }

  if (!node)
  {
    _outtext("\nNo aliases are defined.\n\n");
    return;
  }

  while (node)
  {
    if (!strcmp(node->aliasStrn, args))
    {
      if (node == *aliasHead) *aliasHead = (*aliasHead)->Next;
      if (old) old->Next = node->Next;

      sprintf(strn, "\nDeleted alias '&+c%s&n'\n\n", node->aliasStrn);
      displayColorString(strn);

      delete node;

      return;
    }

    old = node;
    node = node->Next;
  }

  _outtext("\nNo matching alias found.\n\n");
}


//
// addAlias : returns value based on what happened when attempting to
//            add the alias named in aliasStrn to the list pointed to by
//            *aliasHead
//
//    aliasHead : pointer to pointer to head of linked list of aliases
//    aliasStrn : name of alias being added
//         strn : contents of alias being added
//

char addAlias(alias **aliasHead, char *aliasStrn, char *strn)
{
  alias *node = *aliasHead, *adding, *old = NULL;
  char outstrn[1024];


  if (!strcmp(aliasStrn, strn))
  {
    return ALIAS_ITSELF;
  }

 // check for dupes

  while (node)
  {
    if (!strcmp(node->aliasStrn, aliasStrn))
    {
      if (!strlen(strn))  // delete the alias - correction, display what it's
                          // set to
      {
        sprintf(outstrn, "\n&+c%s&n aliases '&+Y%s&n'\n\n",
                node->aliasStrn, node->commandStrn);

        displayColorString(outstrn);

        return ALIAS_DISPLAY;
      }
      else
      {
        strcpy(node->commandStrn, strn);
        return ALIAS_REPLACED;
      }
    }

    old = node;
    node = node->Next;
  }

  if (!strlen(strn))  // no match for alias name, ignore it
    return ALIAS_BLANK;

  node = *aliasHead;

  adding = new alias;
  if (!adding)
  {
    cout << "Error: addAlias() - couldn't alloc alias.  dying..\n";
    return ALIAS_ERROR;
  }

  memset(adding, 0, sizeof(alias));

 // check string length - if it's too long, truncate it

  if (strlen(aliasStrn) >= MAX_ALIAS_LEN) aliasStrn[MAX_ALIAS_LEN - 1] = '\0';
  if (strlen(strn) >= MAX_COMMAND_LEN) strn[MAX_COMMAND_LEN - 1] = '\0';

  strcpy(adding->aliasStrn, aliasStrn);
  strcpy(adding->commandStrn, strn);

  if (!*aliasHead) *aliasHead = adding;
  else
  {
    while (node)
    {
      if (!node->Next) break;

      node = node->Next;
    }

    if (!node)
    {
      cout << "\nError in addAlias() - node is NULL.\n";
      return ALIAS_ERROR;
    }

    node->Next = adding;
  }

  return ALIAS_ADDED;
}


//
// addAliasArgs : adds an alias using addAlias() based on user input
//                contained in args
//
//           args : user input
//         addLFs : if TRUE, adds linefeeds around output
//  updateChanges : if TRUE, updates madeChanges var if a change is made
//      aliasHead : pointer to pointer to head of alias list
//

void addAliasArgs(char *args, char addLFs, char updateChanges,
                  alias **aliasHead)
{
  ulong i = 0, subpos = 0;
  char alias[256], aliased[256], strn[256] = "\0", result;


  if (!strlen(args))
  {
    if (addLFs) _outtext("\n");
    _outtext("Format of alias command is 'alias <alias name> <aliased string>'.\n");
    return;
  }

  remSpacesBetweenArgs(args);
  upstrn(args);

 // get first arg (name of alias)

  while ((args[i] != ' ') && (args[i] != '\0'))
  {
    alias[i] = args[i];
    i++;
  }

  alias[i] = '\0';
  if (args[i] != '\0') i++;

  while (args[i] != '\0')
  {
    aliased[subpos++] = args[i++];
  }

  aliased[subpos] = '\0';

  result = addAlias(aliasHead, alias, aliased);

  switch (result)
  {
    case ALIAS_ADDED    : sprintf(strn, "'&+c%s&n' now aliases '&+Y%s&n'\n",
                                  alias, aliased);
                          if (updateChanges) madeChanges = TRUE;
                          break;
    case ALIAS_REPLACED : sprintf(strn, "Replaced alias '&+c%s&n' with '&+Y%s&n'\n",
                                  alias, aliased);
                          if (updateChanges) madeChanges = TRUE;
                          break;

// deleting aliases is now done with 'unalias' command

/*
    case ALIAS_DELETED  : sprintf(strn, "Deleted alias '&+c%s&n'\n", alias);
                          if (updateChanges) madeChanges = TRUE;
                          break;
*/
    case ALIAS_ITSELF   : sprintf(strn, "An alias can't alias itself.\n");
                          break;
    case ALIAS_BLANK    : sprintf(strn,
"The 'alias' command requires at least two arguments.\n"); break;
    case ALIAS_DISPLAY  :  // perhaps this function should handle this, but
                           // it's easier to let addAlias do it.  so there
    case ALIAS_ERROR    : break;

    default : sprintf(strn, "Error in addAliasArgs() (code #%d) - notify author\n",
                      result);
  }

  if (display && (result != ALIAS_DISPLAY))
  {
    if (addLFs) _outtext("\n");
    displayColorString(strn);
    if (addLFs) _outtext("\n");
  }
}


//
// aliasCmd : the first function called when user types "alias ..." -
//            displays list of aliases if there are no args, calls
//            addAliasArgs otherwise
//
//      args : user input
//    addLFs : if TRUE, adds linefeeds around output
// aliasHead : pointer to pointer to head of list
//

void aliasCmd(char *args, char addLFs, alias **aliasHead)
{
  alias *node = *aliasHead;
  char strn[256];
  ulong i = 0;


  if (!strlen(args))
  {
    if (!node) _outtext("\nThere are currently no aliases defined.\n");
    else _outtext("\n\n");

    while (node)
    {
      sprintf(strn, "  &+c%s&n aliases '&+Y%s&n'\n",
              node->aliasStrn, node->commandStrn);
      displayColorString(strn);

      i += numbLinesStringNeeds(strn);

      if ((i >= (getScreenHeight() - 3)) || (!(node->Next)))
      {
        if (i >= (getScreenHeight() - 3))
        {
          displayColorString("\n&+CPress any key to continue or Q to quit..&n");
          if (toupper(getkey()) == 'Q')
          {
            _outtext("\n\n");

            return;
          }
        }
        else break;

        _outtext("\n\n");

        i = 0;
      }

      node = node->Next;
    }

    _outtext("\n");

    return;
  }
  else addAliasArgs(args, addLFs, TRUE, aliasHead);
}


//
// aliasHasArgs : if the string specified has valid 'alias arguments'
//                (%0, %1, etc), returns TRUE
//
//    strn : alias string to check
//

char aliasHasArgs(char *strn)
{
  ulong last = strlen(strn) - 1, i;

  for (i = 0; i < last; i++)
  {
    if ((strn[i] == '%') && isdigit(strn[i + 1])) return TRUE;
  }

  return FALSE;
}


//
// expandAlias : takes an alias string and expands it, replacing argument
//               tokens with whatever should be there
//
//     endStrn : expanded string is put here
// commandStrn : alias command string
//        args : args to alias
//

char *expandAlias(char *endStrn, char *commandStrn, char *args)
{
  char argList[MAX_ALIAS_ARGS][512], outStrn[1024] = "\0", strn[512], ch;
  ulong i, len = strlen(commandStrn);


  for (i = 0; i < MAX_ALIAS_ARGS; i++)
  {
    getArg(args, i + 1, argList[i]);
  }

  for (i = 0; i < len; i++)
  {
    if ((commandStrn[i] == '%') && (commandStrn[i + 1]))
    {
      ch = commandStrn[i + 1] - '0';

      if ((ch < 0) || (ch > MAX_ALIAS_ARGS)) continue;

     /* if %0, stick the entire arg list in there */

      if (ch == 0) strcat(outStrn, args);
      else strcat(outStrn, argList[ch - 1]);

      i++;
    }
    else
    {
      strn[0] = commandStrn[i];
      strn[1] = '\0';

      strcat(outStrn, strn);
    }
  }

  strcpy(endStrn, outStrn);

/*
  _outtext("\n");
  _outtext(endStrn);
  _outtext("\n");
*/

  return endStrn;
}


//
// replaceAliasVars : takes an alias string and replaces all '$askdf' stuff
//                    with variable contents
//
//     strn : alias string
//  endStrn : new string is copied here
//

char *replaceAliasVars(char *endStrn, char *strn)
{
  ulong len = strlen(strn), i, j, old;
  char tempstrn[256];


  if (!endStrn) return strn;

  endStrn[0] = 0;

  for (i = 0; i < len; i++)
  {
    if ((strn[i] == '$') && strn[i + 1])
    {
      j = 0;
      old = i;
      i++;

      do
      {
        tempstrn[j] = strn[i];
        i++;
        j++;
      } while (strn[i] && (strn[i] != ' '));

      tempstrn[j] = '\0';

      upstrn(tempstrn);

      if (varExists(varHead, tempstrn))
      {
        strcat(endStrn, getVarStrn(varHead, tempstrn, NULL, NULL, "error"));
        strcat(endStrn, " ");
      }
      else
      {
        i = old;
        strcat(endStrn, "$");
      }
    }
    else
    {
      tempstrn[0] = strn[i];
      tempstrn[1] = '\0';

      strcat(endStrn, tempstrn);
    }
  }

  return endStrn;
}


//
// checkAliases : user input is in strn..  if an alias is called in strn,
//                check for special alias stuff and modify strn accordingly
//
//        strn : contains user input
//   aliasHead : pointer to head of linked list of aliases
//

void checkAliases(char *strn, alias *aliasHead)
{
  alias *node = aliasHead;
  char first[256], args[256];//, aliasHadArgs = FALSE;
  ulong i = 0, subpos = 0;


  if (!aliasHead) return;  // why bother

  remLeadingSpaces(strn);
  while (strn[i] && (strn[i] != ' '))
  {
    first[i] = strn[i];
    i++;
  }

  first[i] = '\0';
  upstrn(first);

  while (strn[i])
  {
    args[subpos++] = strn[i++];
  }

  args[subpos] = '\0';
  remLeadingSpaces(args);

  while (node)
  {
    if (!strcmp(first, node->aliasStrn))
    {
      if (aliasHasArgs(node->commandStrn))
      {
        expandAlias(strn, node->commandStrn, args);
//        aliasHadArgs = TRUE;
      }
      else
      {
        strcpy(strn, node->commandStrn);
        strcat(strn, " ");  // not needed  // testing temporarily
        strcat(strn, args);
      }

 /* add later */

/*      while (aliasHasVars(strn))
      {
        if (aliasHasVars(strn))*/ replaceAliasVars(first, strn);
//      }

/*
      _outtext("\n");
      _outtext(strn);
      _outtext("\n");
*/

      strcpy(strn, first);
      upstrn(strn);  // maybe, maybe not

      return;
    }

    node = node->Next;
  }
}
