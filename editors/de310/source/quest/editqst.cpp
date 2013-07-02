//
//  File: editqst.cpp    originally part of dikuEdit
//
//  Usage: user-interface functions for editing quest info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "editqst.h"

using namespace std;

extern char madeChanges;
extern ulong numbLookupEntries;

//
// displayEditQuestMenu : Displays edit room menu
//
//   *room : room being edited
//

void displayEditQuestMenu(quest *qst, const char *mobName, const long mobNumb)
{
  char strn[1024], newName[256], ch;
  questMessage *msg = qst->messageHead;
  questQuest *qstqst = qst->questHead;
  ulong numbShown = 0;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&n&+gEditing quest info for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (!msg && !qstqst)
  {
    displayColorString("&+w  This quest contains no information.\n");
  }

  while (msg)
  {
    if ((numbShown + 'A') > LAST_QUEST_CH)
      ch = FIRST_POSTALPHA_CH + (numbShown - (LAST_QUEST_CH - 'A')) - 1;
    else ch = numbShown + 'A';

    sprintf(strn, "   &+Y%c&+L.&n &+wResponse to \"%s\"&n\n",
            ch, getReadableKeywordStrn(msg->keywordListHead, newName));

    displayColorString(strn);

    numbShown++;

    if ((numbShown % MAX_ENTRIES) == 0)
    {
      displayColorString("\n&+CPress any key to continue..&n");
      getkey();
      displayColorString("\n\n");
    }

    msg = msg->Next;
  }

  while (qstqst)
  {
    if ((numbShown + 'A') > LAST_QUEST_CH)
      ch = FIRST_POSTALPHA_CH + (numbShown - (LAST_QUEST_CH - 'A')) - 1;
    else ch = numbShown + 'A';

    sprintf(strn, "   &+Y%c&+L.&n &+wQuest for \"%s\"&n\n",
            ch, getQuestRecvString(qstqst->questPlayRecvHead, newName));

    displayColorString(strn);

    numbShown++;

    if ((numbShown % MAX_ENTRIES) == 0)
    {
      displayColorString("\n&+CPress any key to continue..&n");
      getkey();
      displayColorString("\n\n");
    }

    qstqst = qstqst->Next;
  }

  sprintf(strn,
"\n"
"   &+YY&+L.&n &+wDelete an entry&n\n"
"   &+YZ&+L.&n &+wCreate an entry&n\n"
"\n"
MENU_COMMON
"\n"
"%s", getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditQuestMenu : Interprets user input for edit room menu - returns
//                       TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//  *room : room to edit
//

char interpEditQuestMenu(usint ch, quest *qst, const char *mobName,
                         const long mobNumb, dikuMob *mob)
{
  struct rccoord coords;
  ulong numbMsgs = getNumbMessageNodes(qst->messageHead),
        numbQsts = getNumbQuestNodes(qst->questHead);

  questMessage *msg;
  questQuest *qstQst;


 // delete stuff

  if (ch == 'Y')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString(
"&+cEnter letter of entry to delete (&+CY to delete entire quest&n&+c): ");

    do
    {
      ch = toupper(getkey());

      if (ch == 'Y')
      {
        deleteQuest(qst);
        mob->questPtr = NULL;
        _outtext("\n\n");
        return QUEST_DELETED;
      }

      if ((ch == 'X') || (ch == K_Escape) || (ch == K_Enter))
      {
        displayEditQuestMenu(qst, mobName, mobNumb);
        return FALSE;
      }

      if ((ch >= FIRST_POSTALPHA_CH) && (ch < 'A'))
         ch += (LAST_QUEST_CH - FIRST_POSTALPHA_CH) + 1;  // it's magic
    } while (!((numbMsgs + numbQsts) > (ch - 'A')));

    if ((numbMsgs + numbQsts) > (ch - 'A'))
    {
      if (((ch - 'A') <= (numbMsgs - 1)) && numbMsgs) // messages
      {
        deleteQuestMessageinList(&(qst->messageHead),
           getMessageNodeNumb(ch - 'A', qst->messageHead));
      }
      else  // quests
      {
        deleteQuestQuestinList(&(qst->questHead),
           getQuestNodeNumb(ch - 'A' - numbMsgs, qst->questHead));
      }
    }

    displayEditQuestMenu(qst, mobName, mobNumb);
  }
  else

 // create something

  if (ch == 'Z')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString(
"&+cCreate new &+Cm&n&+cessage, new &+Cq&n&+cuest entry, or e&+CX&n&+cit (&+Cm/q/X&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    }
    while ((ch != 'M') && (ch != 'Q') && (ch != 'X') && (ch != K_Enter) &&
           (ch != K_Escape));

    switch (ch)
    {
      case 'M' :
        msg = createQuestMessage();
        addQuestMessagetoList(&(qst->messageHead), msg);

        break;

      case 'Q' :
        qstQst = createQuestQuest();
        addQuestQuesttoList(&(qst->questHead), qstQst);

        break;

      case 'X' :
      case K_Escape:
      case K_Enter :
        break;

      default :
        coords = _gettextposition();

        _settextposition(coords.row, 1);
        clrline(coords.row);

        displayColorString(
    "&+CError in input checking - hit a key&n");

        getkey();

        break;
    }

    displayEditQuestMenu(qst, mobName, mobNumb);
  }
  else

 // get input, and love it, cherish it, and best of all, sex it up a little

  if (((ch >= 'A') || (ch >= FIRST_POSTALPHA_CH)) &&
      (numbMsgs || numbQsts))
  {
    if ((ch >= FIRST_POSTALPHA_CH) && (ch < 'A'))
      ch += (LAST_QUEST_CH - FIRST_POSTALPHA_CH) + 1;  // it's magic

    if ((numbMsgs + numbQsts) > (ch - 'A'))
    {
      // edit one or the other

      if (((ch - 'A') <= (numbMsgs - 1)) && numbMsgs)  // edit messages
      {
        editQuestMessage(getMessageNodeNumb(ch - 'A', qst->messageHead));
        displayEditQuestMenu(qst, mobName, mobNumb);
      }
      else  // edit quests
      {
        editQuestQuest(getQuestNodeNumb(ch - 'A' - numbMsgs, qst->questHead),
                       mobName, mobNumb);
        displayEditQuestMenu(qst, mobName, mobNumb);
      }
    }
  }
  else

 // quit - including nice little hack to prevent 'Q' from registering

  if ((checkMenuKey(ch, FALSE) == MENUKEY_SAVE) && (ch != 'Q')) return TRUE;

  return FALSE;
}


//
// realEditQuest : edit a quest
//

dikuMob *realEditQuest(quest *qst, const char *mobName, const long mobNumb,
                       dikuMob *mob, const char allowJump)
{
  usint ch;
  quest *newQuest;
  char done = FALSE;
  ulong numb;


  if (!qst) return NULL;  // ya never know

 // copy qst into newQuest and display the menu

  newQuest = copyQuest(qst);
  if (!newQuest)
  {
    cout << "\n\nError allocating newQuest in editQuest(): aborting.\n\n";
    return NULL;
  }

  displayEditQuestMenu(newQuest, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteQuest(newQuest);

      _outtext("\n\n");

      return NULL;
    }

    if (allowJump)
    {
      if ((checkMenuKey(ch, FALSE) == MENUKEY_NEXT) &&
          (mobNumb != getHighestQuestMobNumber()))
      {
        done = TRUE;
      }

      if ((checkMenuKey(ch, FALSE) == MENUKEY_PREV) &&
          (mobNumb != getLowestQuestMobNumber()))
      {
        done = TRUE;
      }

      if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP) done = TRUE;
    }

   // if interpEditQuestMenu is TRUE, user has quit

    if (!done) done = interpEditQuestMenu(ch, newQuest, mobName, mobNumb, mob);

    if (done == QUEST_DELETED)
    {
      return NULL;
    }

    if (done)
    {
      if (!compareQuestInfo(qst, newQuest))   // changes have been made
      {
        deleteQuestAssocLists(qst);

        memcpy(qst, newQuest, sizeof(quest));

        delete newQuest;

        madeChanges = TRUE;
      }
      else deleteQuest(newQuest);

      _outtext("\n\n");

      if (allowJump)
      {
        if (checkMenuKey(ch, FALSE) == MENUKEY_JUMP)
        {
          switch (jumpQuest(&numb, mobNumb))
          {
            case MENU_JUMP_ERROR : return mob;
            case MENU_JUMP_VALID : return findMob(numb);
            default : _outtext("ERR in editQuest()\n");
                      getkey();
                      return NULL;
          }
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_NEXT)
        {
          return getNextQuestMob(mobNumb);
        }
        else
        if (checkMenuKey(ch, FALSE) == MENUKEY_PREV)
        {
          return getPrevQuestMob(mobNumb);
        }
      }

      return NULL;
    }
  }
}


void editQuest(quest *qst, char *mobName, long mobNumb,
               dikuMob *mob, const char allowJump)
{
  dikuMob *mob2;

  do
  {
    mob2 = realEditQuest(qst, mobName, mobNumb, mob, allowJump);

    if (!mob2) qst = NULL;
    else
    {
      mob = mob2;
      mobName = mob->mobShortName;
      mobNumb = mob->mobNumber;
      qst = mob->questPtr;
    }
  } while (qst);
}


char jumpQuest(ulong *numb, ulong mobNumb)
{
  struct rccoord coords;
  char strn[128], numbStrn[64];
  ulong numb2;


  coords = _gettextposition();
//  coords.row -= 2;

  _settextposition(coords.row, 1);
  clrline(coords.row);

  while (TRUE)
  {
    sprintf(strn,
"&+CJump to which quest mob (%u-%u, ? for list, $ to search): &n",
            getLowestQuestMobNumber(), getHighestQuestMobNumber());
    displayColorString(strn);

    utoa(mobNumb, numbStrn, 10);

    getStrn(strn, 6, 1, 7, '°', numbStrn, FALSE, FALSE);

   // check user input

    if (!strcmp(strn, "?"))
    {
      displayQuestList();
    }
    else
    if (!strcmp(strn, "$"))
    {
      strn[0] = 0;

      _setbkcolor(0);
      displayColorString("\n&+CEnter substring to search mob quest list for: &n");
      getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
      upstrn(strn);

      displayQuestList(strn);
    }
    else
    {
      numb2 = strtoul(strn, NULL, 10);

      if ((!strnumer(strn)) || (numb2 >= numbLookupEntries) ||
          !findQuest(numb2))
      {
        clrline(coords.row, 7, 0);
        _settextposition(coords.row, 1);
        _setbkcolor(0);

        displayColorString(getMenuPromptName());

        return MENU_JUMP_ERROR;
      }
      else
      {
        *numb = numb2;
        return MENU_JUMP_VALID;
      }
    }
  }
}
