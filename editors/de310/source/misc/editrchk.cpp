//
//  File: editrchk.cpp   originally part of dikuEdit
//
//  Usage: functions for handling 'room check options' menu
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../boolean.h"

#include "../misc/menu.h"
#include "../vardef.h"

extern variable *varHead;
extern char madeChanges;

//
// displayEditRoomCheckMenu : Displays edit config menu
//

void displayEditRoomCheckMenu(void)
{
  char strn[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing room checking options\n"
"\n"
"   &+YA&+L.&n &+wCheck for 'stranded' rooms/rooms with no exits in/out?    %s&n\n"
"   &+YB&+L.&n &+wCheck for missing/extraneous keys?                        %s&n\n"
"   &+YC&+L.&n &+wCheck for rooms with illegal values/no descs?             %s&n\n"
"   &+YD&+L.&n &+wCheck for exits with illegal values?                      %s&n\n"
"   &+YE&+L.&n &+wCheck for exits with no descriptions?                     %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn all room check options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getCheckLoneRoomVal()),
             getYesNoStrn(getCheckMissingKeysVal()),
             getYesNoStrn(getCheckRoomVal()),
             getYesNoStrn(getCheckExitVal()),
             getYesNoStrn(getCheckExitDescVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditRoomCheckMenu : Interprets user input for edit check menu - returns
//                           TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditRoomCheckMenu(const usint ch)
{

 // toggle lone room checking

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_CHECKLONEROOM_NAME, !getCheckLoneRoomVal(),
                  FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // toggle missing key checking

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME,
                  !getCheckMissingKeysVal(), FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // toggle room checking

  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_CHECKROOM_NAME, !getCheckRoomVal(), FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // toggle exit checking

  if (ch == 'D')
  {
    setVarBoolVal(&varHead, VAR_CHECKEXIT_NAME, !getCheckExitVal(), FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // toggle exit desc checking

  if (ch == 'E')
  {
    setVarBoolVal(&varHead, VAR_CHECKEXITDESC_NAME, !getCheckExitDescVal(),
                  FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // turn all check options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_CHECKLONEROOM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKROOM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXIT_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXITDESC_NAME, FALSE, FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // turn all check options on

  if (ch == 'Z')
  {
    setVarBoolVal(&varHead, VAR_CHECKLONEROOM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKROOM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXIT_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXITDESC_NAME, TRUE, FALSE);

    displayEditRoomCheckMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editRoomCheck : "main" function for edit room config menu
//
//  *config : config being edited
//

void editRoomCheck(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditRoomCheckMenu();

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

   // if interpEditRoomCheckMenu is TRUE, user has quit

    if (interpEditRoomCheckMenu(ch))
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
