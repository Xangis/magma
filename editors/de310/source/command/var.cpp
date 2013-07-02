//
//  File: var.cpp        originally part of dikuEdit
//
//  Usage: functions for handling variable-related functions and
//         internal structures
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "var.h"
#include "../vardef.h"

using namespace std;

extern variable *varHead;
extern char madeChanges, display;

//
// unvarCmd : removes a variable from the list pointed to by varHead
//            based on the user input in args
//
//      args : arguments entered by user
//   varHead : pointer to pointer to head of list of variables
//

void unvarCmd(char *args, variable **varHead)
{
  variable *node = *varHead, *old = NULL;
  char strn[1024];

  if (numbArgs(args) != 1)
  {
    _outtext("\nThe 'unset' command takes one argument - the variable to delete.\n\n");
    return;
  }

  if (!node)
  {
    _outtext("\nNo variables are defined.\n\n");
    return;
  }

  while (node)
  {
    if (!strcmp(node->varName, args))
    {
      if (node == *varHead) *varHead = (*varHead)->Next;
      if (old) old->Next = node->Next;

      sprintf(strn, "\nDeleted variable '&+c%s&n'\n\n",
              node->varName);
      displayColorString(strn);

      delete node;

      return;
    }

    old = node;
    node = node->Next;
  }

  _outtext("\nNo matching variable found.\n\n");
}


//
// addVar : returns value based on what happened - adds a variable to a
//          variable list with specified var name and value
//
//   varHead : pointer to pointer to head of linked list of vars
//   varStrn : name of variable
//      strn : value of variable
//

char addVar(variable **varHead, char *varStrn, const char *instrn)
{
  variable *node = *varHead, *adding, *old = NULL;
  char outstrn[1024], strn[512];


  strcpy(strn, instrn);

 // check for dupes

  while (node)
  {
    if ((!strcmp(node->varName, varStrn)))
    {
      if (!strlen(strn))  // display var contents
      {
        sprintf(outstrn, "\n&+c%s&n is set to '&+Y%s&n'\n\n",
                node->varName, node->varValue);

        displayColorString(outstrn);

        return VAR_DISPLAY;
      }
      else
      {
        if (!strcmp(node->varValue, strn)) return VAR_SAMEVAL;

        if ((!strcmp(varStrn, VAR_SCREENWIDTH_NAME) ||
             !strcmp(varStrn, VAR_SCREENHEIGHT_NAME)) &&
            !strcmp(strn, "0"))
        {
          return VAR_CANNOT_BE_ZERO;
        }

        strcpy(node->varValue, strn);
        return VAR_REPLACED;
      }
    }

    old = node;
    node = node->Next;
  }

  if (!strlen(strn))  // no match for var name, ignore it
    return VAR_BLANK;

  node = *varHead;

  adding = new variable;
  if (!adding)
  {
    cout << "\n\nError: addVar() - couldn't alloc variable rec.  dying..\n\n";
    return VAR_ERROR;
  }

  memset(adding, 0, sizeof(variable));

 // check string length - if it's too long, truncate it

  if (strlen(varStrn) >= MAX_VARNAME_LEN) varStrn[MAX_VARNAME_LEN - 1] = '\0';
  if (strlen(strn) >= MAX_VARVAL_LEN) strn[MAX_VARVAL_LEN - 1] = '\0';

  if ((!strcmp(varStrn, VAR_SCREENWIDTH_NAME) ||
       !strcmp(varStrn, VAR_SCREENHEIGHT_NAME)) &&
      !strcmp(strn, "0"))
  {
    delete adding;

    return VAR_CANNOT_BE_ZERO;
  }

  strcpy(adding->varName, varStrn);
  upstrn(adding->varName);

  strcpy(adding->varValue, strn);
//  upstrn(adding->varValue);

  if (!*varHead) *varHead = adding;
  else
  {
    while (node)
    {
      if (!node->Next) break;

      node = node->Next;
    }

    if (!node)
    {
      cout << "\nError in addVar() - node is NULL.\n";
      return VAR_ERROR;
    }

    node->Next = adding;
  }

  return VAR_ADDED;
}


//
// setVarArgs : sets a var with an argument string - called from varCmd -
//              should probably stick this in varCmd but ah well
//
//           args : arguments entered by user
//         addLFs : if TRUE, linefeeds are added to output
//  updateChanges : if TRUE, madeChanges is updated if changes are made
//

void setVarArgs(const char *args, const char addLFs, const char updateChanges)
{
  ulong i, numb;
  char var[256], varVal[256] = "\0", strn[256], result;


  if (!strlen(args))
  {
    if (addLFs) _outtext("\n");
    _outtext("Format of set command is 'set <var> <value>'.\n");
    return;
  }

  getArg(args, 1, var);

 // this method is probably not the fastest, but then, neither are linked
 // lists, so I guess I'm just damned to hell

  numb = numbArgs(args);
  if (numb >= 2)
  {
    for (i = 2; i <= numb; i++)
    {
      strcat(varVal, getArg(args, i, strn));
      if (i != numb) strcat(varVal, " ");
    }
  }

  upstrn(var);

  result = addVar(&varHead, var, varVal);

  switch (result)
  {
    case VAR_ADDED    : sprintf(strn, "Variable '&+c%s&n' set to '&+Y%s&n'\n",
                                var, varVal);
                        if (updateChanges) madeChanges = TRUE;
                        break;
    case VAR_REPLACED : sprintf(strn, "Variable '&+c%s&n' set to '&+Y%s&n'\n",
                                var, varVal);
                        if (updateChanges) madeChanges = TRUE;
                        break;

// 'unvar' command now deletes variables

/*
    case VAR_DELETED  : sprintf(strn, "Deleted variable '&+c%s&n'\n", var);
                        if (updateChanges) madeChanges = TRUE;
                        break;
*/
    case VAR_BLANK    : sprintf(strn,
"The 'set' command requires at least two arguments.\n"); break;

    case VAR_CANNOT_BE_ZERO : sprintf(strn,
"The value of that variable cannot be set to zero.\n"); break;

    case VAR_SAMEVAL  :
    case VAR_DISPLAY  :
    case VAR_ERROR    : break;

    default : sprintf(strn, "Error in addVar() - notify author (value was %d)\n",
                      result);
  }

  if (display && (result != VAR_DISPLAY))
  {
    if (addLFs) _outtext("\n");
    displayColorString(strn);
    if (addLFs) _outtext("\n");
  }
}


//
// varCmd : interprets user args, displaying list if none and calling
//          setVarArgs otherwise
//
//    args : user input
//  addLFs : if TRUE, linefeeds are added to output
// varHead : pointer to pointer of head of linked list of variables
//

void varCmd(char *args, const char addLFs, variable **varHead)
{
  variable *node = *varHead;
  char strn[256];
  long i = 0;


  if (!strlen(args))
  {
    if (!node) _outtext("\nThere are currently no variables defined.\n");
    else _outtext("\n\n");

    while (node)
    {
      sprintf(strn, "  &+c%s&n is set to '&+Y%s&n'\n",
              node->varName, node->varValue);
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
  else setVarArgs(args, addLFs, TRUE);
}


//
// getVarNumb : converts the value in a variable to a long int, if possible
//
//    varHead : head of variable list to scan through
//    varName : name of variable to get value of
//   notFound : if non-NULL, set to TRUE or FALSE based on whether the
//              variable was found
//     defVal : default value returned if var not found
//

long getVarNumb(variable *varHead, const char *varName, char *notFound,
                const long defVal)
{
  variable *node = varHead;
  long i;

  while (node)
  {
    if (!strcmp(varName, node->varName))
    {
      if (strnumer(node->varValue)) i = atoi(node->varValue);
      else i = defVal;

      if (notFound) *notFound = FALSE;
      return i;
    }

    node = node->Next;
  }

  if (notFound) *notFound = TRUE;
  return defVal;
}


//
// getVarStrn : returns string value of a specific variable
//
//    varHead : head of list to scan through
//    varName : name of variable to return value of
//   notFound : if non-NULL set to TRUE/FALSE based on success
//       strn : value of variable is copied into this puppy if non-NULL
//     defVal : returned/copied if variable not found
//

const char *getVarStrn(variable *varHead, const char *varName, char *notFound,
                       char *strn, const char *defVal)
{
  variable *node = varHead;

  while (node)
  {
    if (!strcmp(varName, node->varName))
    {
      if (notFound) *notFound = FALSE;
      if (strn) strcpy(strn, node->varValue);
      return node->varValue;
    }

    node = node->Next;
  }

 // no match found

  if (notFound) *notFound = TRUE;

  if (strn) strcpy(strn, defVal);
  return defVal;
}


//
// setVarBoolVal : sets a variable to a specified boolean val
//
//        varHead : pointer to pointer to head of linked list of vars
//        varName : name of variable to set boolean value of
//        boolVal : value to set
//  updateChanges : if TRUE, updates madeChanges appropriately
//

void setVarBoolVal(variable **varHead, char *varName, const char boolVal,
                   const char updateChanges)
{
  long i;  // stores result

  if (boolVal) i = addVar(varHead, varName, "true");
  else i = addVar(varHead, varName, "false");

  if (updateChanges && (i != VAR_BLANK) && (i != VAR_SAMEVAL) &&
      (i != VAR_ERROR))
    madeChanges = TRUE;
}


//
// getVarBoolVal : returns the boolean equivalent of a variable, if possible
//
//     varHead : pointer to head of linked list of variables
//     varName : name of variable to get value of
//    notFound : if non-NULL, set to TRUE/FALSE based on success
//      defVal : default value to return if var not found
//

char getVarBoolVal(variable *varHead, const char *varName, char *notFound,
                   const char defVal)
{
  variable *node = varHead;
  char tempStrn[MAX_VARVAL_LEN];

  while (node)
  {
    if (!strcmp(node->varName, varName))
    {
      strcpy(tempStrn, node->varValue);
      upstrn(tempStrn);

      if ((!strcmp(tempStrn, "TRUE")) || (!strcmp(tempStrn, "1")) ||
          (!strcmp(tempStrn, "ON")))
      {
        if (notFound) *notFound = FALSE;
        return TRUE;
      }

      if ((!strcmp(tempStrn, "FALSE")) || (!strcmp(tempStrn, "0")) ||
          (!strcmp(tempStrn, "OFF")))
      {
        if (notFound) *notFound = FALSE;
        return FALSE;
      }
    }

    node = node->Next;
  }

  if (notFound) *notFound = TRUE;
  return defVal;
}


//
// varExists : checks to see if specified variable exists
//
//    varHead : head of variable list
//    varName : name of variable to check for
//

char varExists(variable *varHead, const char *varName)
{
  variable *node = varHead;

  while (node)
  {
    if (!strcmp(node->varName, varName)) return TRUE;

    node = node->Next;
  }

  return FALSE;
}


//
// isVarBoolean : checks a variable to see if it contains a boolean value
//
//   varHead : head of list of vars to check
//   varName : name of variable to check for booleanism
//

char isVarBoolean(variable *varHead, const char *varName)
{
  char strn[MAX_VARVAL_LEN], matched, strnup[MAX_VARVAL_LEN];


  if (!varExists(varHead, varName)) return FALSE;

  getVarStrn(varHead, varName, &matched, strn, "\0");
  strcpy(strnup, strn);
  upstrn(strnup);

  if (!strcmp(strnup, "TRUE") || !strcmp(strnup, "FALSE") ||
      !strcmp(strnup, "ON")   || !strcmp(strnup, "OFF") ||
      !strcmp(strnup, "1")    || !strcmp(strnup, "0"))
  {
    return TRUE;
  }
  else return FALSE;
}


//
// toggleVar : toggles a boolean variable based on args
//
//    varHead : pointer to pointer to head of linked list of vars
//       args : arguments as entered by user
//

void toggleVar(variable **varHead, char *args)
{
  usint ch;
  int i = 0;
  char notFound, strn[256], foundBool = FALSE;
  variable *node = *varHead;


  if (!strlen(args))
  {
    if (!node)
    {
      _outtext("\nThere are currently no variables defined.\n\n");
      return;
    }
    else
    {
      while (node)
      {
        if (isVarBoolean(*varHead, node->varName))
        {
          foundBool = TRUE;
          break;
        }

        node = node->Next;
      }
    }

    if (!foundBool)
    {
      _outtext("\nThere are currently no boolean variables defined.\n\n");
      return;
    }

    _outtext("\n\n");

    while (node)
    {
      if (isVarBoolean(*varHead, node->varName))
      {
        sprintf(strn, "  &+c%s&n is set to '&+Y%s&n'\n",
                node->varName, node->varValue);
        displayColorString(strn);

        i += numbLinesStringNeeds(strn);
      }

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
  else
  {
    while (args[i])  // cut out any extra crap after the space, if any
    {
      if (args[i] == ' ')
      {
        args[i] = '\0';
        break;
      }

      i++;
    }

    if (!varExists(*varHead, args))
    {
      _outtext("\nThat variable does not exist.\n\n"
               "Set to true (T), false (F), or neither (Q) (t/f/Q)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'T') && (ch != 'F') && (ch != 'Q') && (ch != K_Enter));

      switch (ch)
      {
        case 'T' : _outtext("true\n\n");
                    setVarBoolVal(varHead, args, TRUE, TRUE); return;
        case 'F' : _outtext("false\n\n");
                   setVarBoolVal(varHead, args, FALSE, TRUE); return;
        default : _outtext("neither\n\n"); return;
      }
    }

    if (!isVarBoolean(*varHead, args))
    {
      _outtext("\nThat variable is not a boolean.\n\n");
      return;
    }

    setVarBoolVal(varHead, args,
                  !getVarBoolVal(*varHead, args, &notFound, FALSE), TRUE);

    if (getVarBoolVal(*varHead, args, &notFound, FALSE))
    {
      sprintf(strn, "\nVariable '&+c%s&n' toggled to '&+Ytrue&n'.\n\n",
              args);
    }
    else
    {
      sprintf(strn, "\nVariable '&+c%s&n' toggled to '&+Yfalse&n'.\n\n",
              args);
    }

    displayColorString(strn);
  }
}


//
// copyVarList : copies the list of variables pointed to by varHead,
//               returning the address of the head of the new list
//
//   varHead : head of list to copy
//

variable *copyVarList(const variable *varHead)
{
  variable *newVar, *prevVar = NULL, *headVar = NULL;


  if (!varHead) return NULL;

  while (varHead)
  {
    newVar = new variable;
    if (!newVar)
    {
      cout << "\n\nERROR: copyVarList() - ran out of mem alloc'ing new "
           << " variable node.  Aborting..\n\n";

      return NULL;
    }

    if (!headVar) headVar = newVar;

    if (prevVar) prevVar->Next = newVar;
    prevVar = newVar;

    memcpy(newVar, varHead, sizeof(variable));
    newVar->Next = NULL;

    varHead = varHead->Next;
  }

  return headVar;
}


//
// deleteVarList : Deletes an entire variable list
//
//    *varHead : take a wild guess, bucko
//

void deleteVarList(variable *varHead)
{
  variable *nextVar;

  while (varHead)
  {
    nextVar = varHead->Next;

    delete varHead;

    varHead = nextVar;
  }
}


//
// compareVarLists : compare two variable lists - FALSE, no match, TRUE,
//                   match
//
//   varH1 : head of first list
//   varH2 : head of second list
//

char compareVarLists(variable *varH1, variable *varH2)
{
  variable *v1 = varH1, *v2 = varH2;
//  char notFound;

  // first, make sure both lists contain the same variables

  while (v1)
  {
    if (!varExists(varH2, v1->varName)) return FALSE;

    v1 = v1->Next;
  }

  while (v2)
  {
    if (!varExists(varH1, v2->varName)) return FALSE;

    v2 = v2->Next;
  }

 // now, make sure that the values in both lists are the same

  v1 = varH1;
  while (v1)
  {
    if (strcmp(getVarStrn(varH2, v1->varName, /*&notFound*/NULL, NULL, "\0"),
               v1->varValue))
    {
      return FALSE;
    }

    v1 = v1->Next;
  }

  return TRUE;
}
