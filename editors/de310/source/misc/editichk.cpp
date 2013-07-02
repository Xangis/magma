//
//  File: editichk.cpp   originally part of dikuEdit
//
//  Usage: functions for handling 'misc check options' menu
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../vardef.h"
#include "../misc/menu.h"

extern variable *varHead;
extern char madeChanges;

//
// displayEditMiscCheckMenu : Displays edit config menu
//

void displayEditMiscCheckMenu(void)
{
  char strn[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing miscellaneous check options\n"
"\n"
"   &+YA&+L.&n &+wCheck for extra descs with no keywords/desc?              %s&n\n"
"   &+YB&+L.&n &+wCheck all room/object/mob flags for misset bits?          %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn all miscellaneous check options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getCheckEdescVal()),
             getYesNoStrn(getCheckFlagsVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditMiscCheckMenu : Interprets user input for edit check menu - returns
//                       TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditMiscCheckMenu(usint ch)
{

 // toggle edesc checking

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_CHECKEDESC_NAME, !getCheckEdescVal(),
                  FALSE);

    displayEditMiscCheckMenu();
  }
  else

 // toggle bit checking val

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_CHECKFLAGS_NAME, !getCheckFlagsVal(),
                  FALSE);

    displayEditMiscCheckMenu();
  }
  else

 // turn all check options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_CHECKEDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKFLAGS_NAME, FALSE, FALSE);

    displayEditMiscCheckMenu();
  }
  else

 // turn all check options on

  if (ch == 'Z')
  {
    setVarBoolVal(&varHead, VAR_CHECKEDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKFLAGS_NAME, TRUE, FALSE);

    displayEditMiscCheckMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editMiscCheck : "main" function for edit config menu
//
//  *config : config being edited
//

void editMiscCheck(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditMiscCheckMenu();

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
     // only fragment heap if necessary

      if (!compareVarLists(varHead, oldVar))
      {
        vartemp = varHead;
        varHead = oldVar;

        deleteVarList(vartemp);
      } else deleteVarList(oldVar);

      _outtext("\n\n");

      return;
    }

   // if interpEditMiscCheckMenu is TRUE, user has quit

    if (interpEditMiscCheckMenu(ch))
    {
      if (!compareVarLists(varHead, oldVar))
      {
        madeChanges = TRUE;
      }

      deleteVarList(oldVar);

      _outtext("\n\n");
      return;
    }
  }
}
