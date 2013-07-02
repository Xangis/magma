//
//  File: editmchk.cpp   originally part of dikuEdit
//
//  Usage: functions handling 'mob check options' menu
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../vardef.h"
#include "../misc/menu.h"

extern variable *varHead;
extern char madeChanges;

//
// displayEditMobCheckMenu : Displays edit config menu
//

void displayEditMobCheckMenu(void)
{
  char strn[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing mob check options\n"
"\n"
"   &+YA&+L.&n &+wCheck for object/mob types that aren't loaded?            %s&n\n"
"   &+YB&+L.&n &+wCheck for mobs with illegal values?                       %s&n\n"
"   &+YC&+L.&n &+wCheck for mobs with no descriptions?                      %s&n\n"
"   &+YD&+L.&n &+wCheck objects/mobs for guild-restricted stuff?            %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn all mob check options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getCheckLoadedVal()),
             getYesNoStrn(getCheckMobVal()),
             getYesNoStrn(getCheckMobDescVal()),
             getYesNoStrn(getCheckGuildStuffVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditMobCheckMenu : Interprets user input for edit check menu - returns
//                       TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditMobCheckMenu(usint ch)
{

 // toggle obj/mob type not loaded checking

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, !getCheckLoadedVal(), FALSE);

    displayEditMobCheckMenu();
  }
  else

 // toggle mob checking

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_CHECKMOB_NAME, !getCheckMobVal(),
                  FALSE);

    displayEditMobCheckMenu();
  }
  else

 // toggle mob desc checking

  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_CHECKMOBDESC_NAME, !getCheckMobDescVal(),
                  FALSE);

    displayEditMobCheckMenu();
  }
  else

 // toggle guild checking

  if (ch == 'D')
  {
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, !getCheckGuildStuffVal(),
                  FALSE);

    displayEditMobCheckMenu();
  }
  else

 // turn all check options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOB_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOBDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, FALSE, FALSE);

    displayEditMobCheckMenu();
  }
  else

 // turn all check options on

  if (ch == 'Z')
  {
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOB_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOBDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, TRUE, FALSE);

    displayEditMobCheckMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editMobCheck : "main" function for edit config menu
//
//  *config : config being edited
//

void editMobCheck(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditMobCheckMenu();

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

   // if interpEditMobCheckMenu is TRUE, user has quit

    if (interpEditMobCheckMenu(ch))
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
