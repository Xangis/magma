//
//  File: editochk.cpp   originally part of dikuEdit
//
//  Usage: functions for handling 'object check options' menu
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"

#include "../misc/menu.h"
#include "../vardef.h"

extern variable *varHead;
extern char madeChanges;

//
// displayEditObjCheckMenu : Displays edit config menu
//

void displayEditObjCheckMenu(void)
{
  char strn[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing object check options\n"
"\n"
"   &+YA&+L.&n &+wCheck for object/mob types that aren't loaded?            %s&n\n"
"   &+YB&+L.&n &+wCheck for objects with illegal values?                    %s&n\n"
"   &+YC&+L.&n &+wCheck 'object value' values based on object type?         %s&n\n"
"   &+YD&+L.&n &+wCheck for objects with no extra descriptions?             %s&n\n"
"   &+YE&+L.&n &+wCheck for missing/extraneous keys?                        %s&n\n"
"   &+YF&+L.&n &+wCheck for invalid material settings on armor/worn objs?   %s&n\n"
"   &+YG&+L.&n &+wCheck objects/mobs for guild-restricted stuff?            %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn all object check options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getCheckLoadedVal()),
             getYesNoStrn(getCheckObjVal()),
             getYesNoStrn(getCheckObjValuesVal()),
             getYesNoStrn(getCheckObjDescVal()),
             getYesNoStrn(getCheckMissingKeysVal()),
             getYesNoStrn(getCheckObjMaterialVal()),
             getYesNoStrn(getCheckGuildStuffVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditObjCheckMenu : Interprets user input for edit check menu - returns
//                          TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditObjCheckMenu(usint ch)
{

 // toggle obj/mob type not loaded checking

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, !getCheckLoadedVal(), FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle object checking

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_CHECKOBJ_NAME, !getCheckObjVal(),
                  FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle object value checking

  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_CHECKOBJVAL_NAME, !getCheckObjValuesVal(),
                  FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle object desc checking

  if (ch == 'D')
  {
    setVarBoolVal(&varHead, VAR_CHECKOBJDESC_NAME, !getCheckObjDescVal(),
                  FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle missing key checking

  if (ch == 'E')
  {
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME,
                  !getCheckMissingKeysVal(), FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle material checking

  if (ch == 'F')
  {
    setVarBoolVal(&varHead, VAR_CHECKOBJMATERIAL_NAME,
                  !getCheckObjMaterialVal(), FALSE);

    displayEditObjCheckMenu();
  }
  else

 // toggle guild checking

  if (ch == 'G')
  {
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, !getCheckGuildStuffVal(),
                  FALSE);

    displayEditObjCheckMenu();
  }
  else

 // turn all check options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJ_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJVAL_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJMATERIAL_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, FALSE, FALSE);

    displayEditObjCheckMenu();
  }
  else

 // turn all check options on

  if (ch == 'Z')
  {
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJ_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJVAL_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJMATERIAL_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, TRUE, FALSE);

    displayEditObjCheckMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editObjCheck : "main" function for edit config menu
//
//  *config : config being edited
//

void editObjCheck(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditObjCheckMenu();

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

   // if interpEditObjCheckMenu is TRUE, user has quit

    if (interpEditObjCheckMenu(ch))
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
