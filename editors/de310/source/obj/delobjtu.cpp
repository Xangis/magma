//
//  File: delobjtu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for deleting object types
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../keys.h"

#include "object.h"

extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern char madeChanges;

//
// checkDeleteObjectsofType : returns TRUE if user doesn't abort - this
//                            function prompts user if any of these object
//                            types exist
//

char checkDeleteObjectsofType(const dikuObject *objType)
{
  usint ch;


  if (checkForObjHeresofType(objType))
  {
    displayColorString(
"\n&+cObjects exist of this type and will have to be deleted - continue (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");

      return FALSE;
    }
    else
    {
      displayColorString("Yes&n");

      deleteAllObjHeresofType(objType, TRUE);
      deleteObjsinInv(objType);

      return TRUE;
    }
  }

  return TRUE;
}


//
// deleteObjectTypeUser : Deletes an objectType based on info specified by user
//                        in strn.
//
//   *strn : user-entered string
//     pos : pos in string where id number starts
//

void deleteObjectTypeUser(const char *args)
{
  char outStrn[256];
  dikuObject *obj;
  dikuRoom *room = roomHead;
  ulong numb;


  if (!strlen(args))
  {
    deleteObjectTypeUserPrompt();
    return;
  }

  if (!strlen(args) || !strnumer(args))
  {
    displayColorString("&n\nError in input - specify vnum of object type.\n\n");
    return;
  }

  numb = atoi(args);

  obj = findObj(numb);
  if (!obj)
  {
    sprintf(outStrn, "\nObject type #%d not found.\n\n", numb);
    _outtext(outStrn);

    return;
  }

  if (!checkDeleteObjectsofType(obj)) return;

  deleteObjectTypeinList(&objectHead, obj, TRUE);

  sprintf(outStrn, "\nObject type #%d deleted.\n\n", numb);
  _outtext(outStrn);

  madeChanges = TRUE;
}


//
// deleteObjectTypeUserPrompt
//

void deleteObjectTypeUserPrompt(void)
{
  char numbStrn[32] = "\0";


  while (TRUE)
  {
    displayColorString("\n&+cEnter number of object type to delete (&+C? for list&n&+c): ");

    getStrn(numbStrn, 8, 1, 7, '°', numbStrn, FALSE, FALSE);

    if (!strcmp(numbStrn, "?"))
    {
      displayObjectTypeList();

      strcpy(numbStrn, "\0");
    }
    else break;
  }

  _setbkcolor(0);

  if (strlen(numbStrn)) deleteObjectTypeUser(numbStrn);
  else _outtext("\n\n");
}
