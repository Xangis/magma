//
//  File: editchck.cpp   originally part of dikuEdit
//
//  Usage: functions for handling main 'checkconfig' menu
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "../command/var.h"
#include "../vardef.h"

extern variable *varHead;
extern char madeChanges;

//
// displayEditCheckMenu : Displays edit config menu
//

void displayEditCheckMenu(void)
{
  char strn[4096];


  _clearscreen(7,0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing check options\n"
"\n"
"   &+YA&+L.&n &+wEdit room/exit checking options&n\n"
"   &+YB&+L.&n &+wEdit object checking options&n\n"
"   &+YC&+L.&n &+wEdit mob checking options&n\n"
"   &+YD&+L.&n &+wEdit miscellaneous checking options&n\n"
"\n"
"   &+YE&+L.&n &+wAutomatically run check every save?                       %s&n\n"
"   &+YF&+L.&n &+wAbort saving if any errors found?                         %s&n\n"
"   &+YG&+L.&n &+wWrite errors to CHECK.LOG as well as screen?              %s&n\n"
"   &+YH&+L.&n &+wPause every screenful of check errors?                    %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn &+Wall&n&+w check options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getCheckSaveVal()),
             getYesNoStrn(getNoSaveonCheckErrVal()),
             getYesNoStrn(getSaveCheckLogVal()),
             getYesNoStrn(getPauseCheckScreenfulVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditCheckMenu : Interprets user input for edit check menu - returns
//                       TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditCheckMenu(usint ch)
{

 // room/exit check options

  if (ch == 'A')
  {
    editRoomCheck();

    displayEditCheckMenu();
  }
  else

 // obj check options

  if (ch == 'B')
  {
    editObjCheck();

    displayEditCheckMenu();
  }
  else

 // mob check options

  if (ch == 'C')
  {
    editMobCheck();

    displayEditCheckMenu();
  }
  else

 // misc check options

  if (ch == 'D')
  {
    editMiscCheck();

    displayEditCheckMenu();
  }
  else

 // toggle check while saving

  if (ch == 'E')
  {
    setVarBoolVal(&varHead, VAR_CHECKSAVE_NAME, !getCheckSaveVal(),
                  FALSE);

    displayEditCheckMenu();
  }
  else

 // toggle abort saving on check err

  if (ch == 'F')
  {
    setVarBoolVal(&varHead, VAR_NOSAVEONCHECKERR_NAME,
                  !getNoSaveonCheckErrVal(), FALSE);

    displayEditCheckMenu();
  }
  else

 // toggle check log

  if (ch == 'G')
  {
    setVarBoolVal(&varHead, VAR_SAVECHECKLOG_NAME,
                  !getSaveCheckLogVal(), FALSE);

    displayEditCheckMenu();
  }
  else

 // toggle check screenful pausing

  if (ch == 'H')
  {
    setVarBoolVal(&varHead, VAR_PAUSECHECKSCREENFUL_NAME,
                  !getPauseCheckScreenfulVal(), FALSE);

    displayEditCheckMenu();
  }
  else

 // turn all check options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_CHECKSAVE_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_NOSAVEONCHECKERR_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SAVECHECKLOG_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_PAUSECHECKSCREENFUL_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLONEROOM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKROOM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXIT_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXITDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJ_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOB_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOBDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEDESC_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJMATERIAL_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKFLAGS_NAME, FALSE, FALSE);

    displayEditCheckMenu();
  }
  else

 // turn all check options on

  if (ch == 'Z')
  {
    setVarBoolVal(&varHead, VAR_CHECKSAVE_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_NOSAVEONCHECKERR_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SAVECHECKLOG_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_PAUSECHECKSCREENFUL_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLONEROOM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMISSINGKEYS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKLOADED_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKROOM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXIT_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEXITDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJ_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOB_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKMOBDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKEDESC_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKOBJMATERIAL_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_CHECKFLAGS_NAME, TRUE, FALSE);

    displayEditCheckMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editCheck : "main" function for edit config menu
//
//  *config : config being edited
//

void editCheck(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditCheckMenu();

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

   // if interpEditCheckMenu is TRUE, user has quit

    if (interpEditCheckMenu(ch))
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
