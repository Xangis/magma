//
//  File: config.cpp     originally part of dikuEdit
//
//  Usage: functions for handling 'config' menu, containing misc options
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../vardef.h"
#include "menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

extern char madeChanges;
extern variable *varHead;

//
// displayEditConfigMenu : Displays edit config menu
//

void displayEditConfigMenu(void)
{
  char strn[2048];


  _clearscreen(7,0);

  _settextposition(1, 1);

  sprintf(strn,
"&n&+gEditing miscellaneous config options\n"
"\n"
"   &+YA&+L.&n &+wCheck all vnums and vnum input to make sure they exist?   %s&n\n"
"   &+YB&+L.&n &+wUpon loading, check all .WLD zone flags vs. .ZON numb?    %s&n\n"
"\n"
"   &+YC&+L.&n &+wShow extra info about entity being edited on menus?       %s&n\n"
"   &+YD&+L.&n &+wEnable 'create room as you walk' creation?                %s&n\n"
"   &+YE&+L.&n &+wSave vnum of current room for reentrance when reloading?  %s&n\n"
"   &+YF&+L.&n &+wConsider exits with destinations of -1 out of zone?       %s&n\n"
"   &+YG&+L.&n &+wShow shop prices adjusted for mob's sell percentage?      %s&n\n"
"   &+YH&+L.&n &+wAutomatically save zone based on 'save when' below?       %s&n\n"
"   &+YI&+L.&n &+wIf autosave is on, save every X commands ...              %u&n\n"
"\n"
"   &+YJ&+L.&n &+wScreen height                                             %u&n\n"
"   &+YK&+L.&n &+wScreen width                                              %u&n\n"
"\n"
"   &+YL&+L.&n &+wExternal desc editor - '%s'&n\n"
"   &+YM&+L.&n &+wMenu prompt - '%s'&n\n"
"   &+YN&+L.&n &+wMain prompt - '%s'&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
             getYesNoStrn(getVnumCheckVal()),
             getYesNoStrn(getCheckZoneFlagsVal()),

             getYesNoStrn(getShowMenuInfoVal()),
             getYesNoStrn(getWalkCreateVal()),
             getYesNoStrn(getStartRoomActiveVal()),
             getYesNoStrn(getNegDestOutofZoneVal()),
             getYesNoStrn(getShowPricesAdjustedVal()),
             getYesNoStrn(getSaveEveryXCommandsVal()),
             getSaveHowOftenVal(),

             getScreenHeight(),
             getScreenWidth(),

             getEditorName(),
             getMenuPromptName(),
             getMainPromptStrn(),

             getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditConfigMenu : Interprets user input for edit config menu - returns
//                        TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//

char interpEditConfigMenu(const usint ch)
{
  struct rccoord coords;
  char tempStrn[256], strn[256];


 // toggle vnum checking

  if (ch == 'A')
  {
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, !getVnumCheckVal(), FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle zone flag checking

  if (ch == 'B')
  {
    setVarBoolVal(&varHead, VAR_CHECKZONEFLAGS_NAME, !getCheckZoneFlagsVal(),
                  FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle stopping at zone 'S' line setting

/*
  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_IGNOREZONES_NAME, !getIgnoreZoneSVal(),
                  FALSE);

    displayEditConfigMenu();
  }
  else
*/

 // toggle the showing of info on menus

  if (ch == 'C')
  {
    setVarBoolVal(&varHead, VAR_SHOWMENUINFO_NAME, !getShowMenuInfoVal(),
                  FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle walk creation mode

  if (ch == 'D')
  {
    setVarBoolVal(&varHead, VAR_WALKCREATE_NAME, !getWalkCreateVal(), FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle saving of currentRoom vnum

  if (ch == 'E')
  {
    setVarBoolVal(&varHead, VAR_SROOMACTIVE_NAME, !getStartRoomActiveVal(),
                  FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle object edesc thingy

/*
  if (ch == 'F')
  {
    setVarBoolVal(&varHead, VAR_FIRSTOBJEDESCKEY_NAME,
                  !getFirstObjEdescKeyVal(), FALSE);

    displayEditConfigMenu();
  }
  else
*/

 // toggle 'consider exit dest of -1 as out-of-zone' var

  if (ch == 'F')
  {
    setVarBoolVal(&varHead, VAR_NEGDESTOUTOFZONE_NAME,
                  !getNegDestOutofZoneVal(), FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle 'show shop prices adjusted' var

  if (ch == 'G')
  {
    setVarBoolVal(&varHead, VAR_SHOWPRICESADJUSTED_NAME,
                  !getShowPricesAdjustedVal(), FALSE);

    displayEditConfigMenu();
  }
  else

 // toggle 'autosave every X commands' var

  if (ch == 'H')
  {
    setVarBoolVal(&varHead, VAR_SAVEEVERYXCOMMANDS_NAME,
                  !getSaveEveryXCommandsVal(), FALSE);

    displayEditConfigMenu();
  }
  else

 // edit 'when to autosave' variable

  if (ch == 'I')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew number of commands between autosaving: &n");

    itoa(getSaveHowOftenVal(), strn, 10);

    getStrn(tempStrn, 4, 1, 7, '°', strn, FALSE, FALSE);

    if (strnumer(tempStrn))
    {
      addVar(&varHead, VAR_SAVEHOWOFTEN_NAME, tempStrn);
    }

    displayEditConfigMenu();
  }
  else

 // edit screen height

  if (ch == 'J')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew screen height: &n");

    itoa(getScreenHeight(), strn, 10);

    getStrn(tempStrn, 4, 1, 7, '°', strn, FALSE, FALSE);

   // allowing a height of 0 would be bad

    if (strnumer(tempStrn) && atoi(tempStrn))
    {
      addVar(&varHead, VAR_SCREENHEIGHT_NAME, tempStrn);
    }

    displayEditConfigMenu();
  }
  else

 // edit screen width

  if (ch == 'K')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew screen width: &n");

    itoa(getScreenWidth(), strn, 10);

    getStrn(tempStrn, 4, 1, 7, '°', strn, FALSE, FALSE);

   // allowing a width of 0 would be very bad

    if (strnumer(tempStrn) && atoi(tempStrn))
    {
      addVar(&varHead, VAR_SCREENWIDTH_NAME, tempStrn);
    }

    displayEditConfigMenu();
  }
  else

 // edit name of external editor

  if (ch == 'L')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew external editor (full path, if necessary): &n");

    getStrn(tempStrn, MAX_VARVAL_LEN - 1, 1, 7, '°',
            getEditorName(), FALSE, FALSE);

    addVar(&varHead, VAR_TEXTEDIT_NAME, tempStrn);

    displayEditConfigMenu();
  }
  else

 // edit name of menu edit prompt

  if (ch == 'M')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew prompt for menus: &n");

    getStrn(tempStrn, MAX_VARVAL_LEN - 1, 1, 7, '°',
            getMenuPromptName(), FALSE, FALSE);

    if ((strlen(tempStrn) < (MAX_VARVAL_LEN - 1 - 2)) &&
        !strright(tempStrn, "&n") &&
        !strright(tempStrn, "&N")) strcat(tempStrn, "&n");

    addVar(&varHead, VAR_MENUPROMPT_NAME, tempStrn);

    displayEditConfigMenu();
  }
  else

 // edit name of main prompt

  if (ch == 'N')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+CNew main prompt: &n");

    getStrn(tempStrn, MAX_VARVAL_LEN - 1, 1, 7, '°',
            getMainPromptStrn(), FALSE, FALSE);

    if ((strlen(tempStrn) < (MAX_VARVAL_LEN - 1 - 2)) &&
        !strright(tempStrn, "&n") &&
        !strright(tempStrn, "&N") &&
        strcmp(tempStrn, "default")) strcat(tempStrn, "&n");

    addVar(&varHead, VAR_MAINPROMPT_NAME, tempStrn);
    createPrompt();

    displayEditConfigMenu();
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editConfig : "main" function for edit config menu
//
//  *config : config being edited
//

void editConfig(void)
{
  usint ch;
  variable *oldVar = copyVarList(varHead), *vartemp;


  displayEditConfigMenu();

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

   // if interpEditConfigMenu is TRUE, user has quit

    if (interpEditConfigMenu(ch))
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
