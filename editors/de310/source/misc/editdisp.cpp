//
//  File: editdisp.cpp   originally part of dikuEdit
//
//  Usage: functions for handling 'displayconfig' menu
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
// displayEditDisplayMenu : Displays edit config menu
//

void displayEditDisplayMenu(void)
{
  char strn[4096];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  sprintf(strn,
"&+gEditing display options\n"
"\n"
"   &+YA&+L.&n &+wInterpret Diku color codes?                               %s&n\n"
"   &+YB&+L.&n &+wDisplay Diku color codes?                                 %s&n\n"
"\n"
"   &+YC&+L.&n &+wShow room 'extra info' (sector type and flags)?           %s&n\n"
"   &+YD&+L.&n &+wShow room vnum after room name?                           %s&n\n"
"   &+YE&+L.&n &+wShow exit flags after exit name?                          %s&n\n"
"   &+YF&+L.&n &+wShow exit room vnum dest after exit name?                 %s&n\n"
"\n"
"   &+YG&+L.&n &+wShow object flags info before name?                       %s&n\n"
"   &+YH&+L.&n &+wShow object vnum after object name?                       %s&n\n"
"\n"
"   &+YI&+L.&n &+wShow mob flags info before name?                          %s&n\n"
"   &+YJ&+L.&n &+wShow mob default pos after name?                          %s&n\n"
"   &+YK&+L.&n &+wShow mob vnum after mob name?                             %s&n\n"
"   &+YL&+L.&n &+wShow mobs following/riding/ridden by others?              %s&n\n"
"\n"
" &+YY/Z&+L.&n &+wTurn all vnum/extra info display options off/on\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getInterpColorVal()),
             getYesNoStrn(getShowColorVal()),

             getYesNoStrn(getShowRoomExtraVal()),
             getYesNoStrn(getShowRoomVnumVal()),
             getYesNoStrn(getShowExitFlagsVal()),
             getYesNoStrn(getShowExitDestVal()),

             getYesNoStrn(getShowObjFlagsVal()),
             getYesNoStrn(getShowObjVnumVal()),

             getYesNoStrn(getShowMobFlagsVal()),
             getYesNoStrn(getShowMobPosVal()),
             getYesNoStrn(getShowMobVnumVal()),
             getYesNoStrn(getShowMobRideFollowVal()),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditDisplayMenu : Interprets user input for edit config menu - returns
//                         TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditDisplayMenu(usint ch)
{

 // toggle interpretation of diku color codes

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_INTERPCOLOR_NAME, !getInterpColorVal(), FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle display of diku color codes

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_SHOWCOLOR_NAME, !getShowColorVal(), FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle display of room 'extra stuff'

  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_SHOWROOMEXTRA_NAME, !getShowRoomExtraVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle display of room vnum after name

  if (ch == 'D')
  {
    setVarBoolVal(&varHead, VAR_SHOWROOMVNUM_NAME, !getShowRoomVnumVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle display of exit flags after name

  if (ch == 'E')
  {
    setVarBoolVal(&varHead, VAR_SHOWEXITFLAGS_NAME, !getShowExitFlagsVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle display of exit dest room vnum after name

  if (ch == 'F')
  {
    setVarBoolVal(&varHead, VAR_SHOWEXITDEST_NAME, !getShowExitDestVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show obj flags thingy

  if (ch == 'G')
  {
    setVarBoolVal(&varHead, VAR_SHOWOBJFLAGS_NAME, !getShowObjFlagsVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show obj vnum var

  if (ch == 'H')
  {
    setVarBoolVal(&varHead, VAR_SHOWOBJVNUM_NAME, !getShowObjVnumVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show mob flags thingy

  if (ch == 'I')
  {
    setVarBoolVal(&varHead, VAR_SHOWMOBFLAGS_NAME, !getShowMobFlagsVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show mob pos thingy

  if (ch == 'J')
  {
    setVarBoolVal(&varHead, VAR_SHOWMOBPOS_NAME, !getShowMobPosVal(), FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show mob vnum var

  if (ch == 'K')
  {
    setVarBoolVal(&varHead, VAR_SHOWMOBVNUM_NAME, !getShowMobVnumVal(),
                  FALSE);

    displayEditDisplayMenu();
  }
  else

 // toggle show mob ride/follow var

  if (ch == 'L')
  {
    setVarBoolVal(&varHead, VAR_SHOWMOBRIDEFOLLOW_NAME,
                  !getShowMobRideFollowVal(), FALSE);

    displayEditDisplayMenu();
  }
  else

 // turn all display options off

  if (ch == 'Y')
  {
    setVarBoolVal(&varHead, VAR_SHOWROOMEXTRA_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWROOMVNUM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWEXITFLAGS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWEXITDEST_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWOBJFLAGS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWOBJVNUM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBFLAGS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBPOS_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBVNUM_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBRIDEFOLLOW_NAME, FALSE, FALSE);

    displayEditDisplayMenu();
  }
  else

 // turn all display options on

  if ((ch == 'Z') || (ch == '1'))
  {
    setVarBoolVal(&varHead, VAR_SHOWROOMEXTRA_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWROOMVNUM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWEXITFLAGS_NAME, TRUE, FALSE);
    if (ch != '1') setVarBoolVal(&varHead, VAR_SHOWEXITDEST_NAME, TRUE, FALSE);
    else setVarBoolVal(&varHead, VAR_SHOWEXITDEST_NAME, FALSE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWOBJFLAGS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWOBJVNUM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBFLAGS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBPOS_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBVNUM_NAME, TRUE, FALSE);
    setVarBoolVal(&varHead, VAR_SHOWMOBRIDEFOLLOW_NAME, TRUE, FALSE);

    displayEditDisplayMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editDisplay : "main" function for edit config menu
//
//  *config : config being edited
//

void editDisplay(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditDisplayMenu();

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

   // if interpEditDisplayMenu is TRUE, user has quit

    if (interpEditDisplayMenu(ch))
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
