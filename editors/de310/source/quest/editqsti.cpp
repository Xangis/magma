//
//  File: editqsti.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing 'items' in mob quests
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "quest.h"

//
// displayEditQuestItemMenu : Displays the edit quest item menu
//
//    *item : item being edited
//

void displayEditQuestItemMenu(questItem *item, const char *mobName,
                              const long mobNumb, const char giveRecv)
{
  char strn[256], newName[256];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobNameShort(mobName, newName);

  if (giveRecv == QUEST_GIVEITEM)
  {
    sprintf(strn,
            "&+gEditing quest item given to mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
            mobNumb, newName);
  }
  else
  {
    sprintf(strn,
            "&+gEditing quest item given to PCs by mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
            mobNumb, newName);
  }

  displayColorString(strn);

if(item->itemType == 4 || item->itemType == 5)
{
  sprintf(strn,
"   &+YA&+L.&n &+wEdit item type                       %d (%s)\n"
"   &+YB&+L.&n &+wEdit item vnum/value                 %d (%s)\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  item->itemType,
                   getQuestItemTypeStrn(item->itemType, giveRecv),

                  item->itemVal,
                   getSpellTypeStrn(item->itemVal),

                  getMenuPromptName());
}
else
{
  sprintf(strn,
"   &+YA&+L.&n &+wEdit item type                       %d (%s)\n"
"   &+YB&+L.&n &+wEdit item vnum/value                 %d\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  item->itemType,
                   getQuestItemTypeStrn(item->itemType, giveRecv),

                  item->itemVal,

                  getMenuPromptName());
}

  displayColorString(strn);
}


//
// interpEditQuestItemMenu : Interprets user input for edit quest item menu -
//                           returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *item : item to edit
//

char interpEditQuestItemMenu(const usint ch, questItem *item,
                             const char *mobName, const long mobNumb,
                             const char giveRecv)
{
  char strn[256] = "\0",
       helpAvail = checkForQuestItemHelp(item->itemType, giveRecv);
  struct rccoord coords;


 // edit item type

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+CNew item type (? for list): &n");

      itoa(item->itemType, strn, 10);

      getStrn(strn, 4, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?"))
      {
        displayQuestItemTypeList(giveRecv);
//        displayEditQuestItemMenu(item, mobName, mobNumb, giveRecv);
      }
      else if (strnumer(strn)) break;
      else
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);
      }

      _setbkcolor(0);
    }

    item->itemType = atoi(strn);

    displayEditQuestItemMenu(item, mobName, mobNumb, giveRecv);
  }
  else

 // edit item value

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      if (helpAvail) displayColorString("&+CNew item vnum/value (? for list): &n");
      else displayColorString("&+CNew item value: &n");

      itoa(item->itemVal, strn, 10);

      getStrn(strn, 6, 1, 7, '°', strn, FALSE, FALSE);

      if ((!strcmp(strn, "?")) && helpAvail)
      {
        displayQuestItemHelp(item->itemType, giveRecv);
//        displayEditQuestItemMenu(item, mobName, mobNumb, giveRecv);
      }
      else if (strnumer(strn)) break;
      else
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);
      }


      _setbkcolor(0);
    }

    item->itemVal = atoi(strn);

    displayEditQuestItemMenu(item, mobName, mobNumb, giveRecv);
  }
  else
  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editQuestItem : Edit item misc - "main" function
//
//   *item : item to edit
//

void editQuestItem(questItem *item, const char *mobName, const long mobNumb,
                   const char giveRecv)
{
  usint ch;
  questItem oldItem;


  if (!item) return;

 // back up the old stuff

  memcpy(&oldItem, item, sizeof(questItem));


  displayEditQuestItemMenu(item, mobName, mobNumb, giveRecv);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      memcpy(item, &oldItem, sizeof(questItem));

      _outtext("\n\n");

      return;
    }

    if (interpEditQuestItemMenu(ch, item, mobName, mobNumb, giveRecv)) return;
  }
}
