//
//  File: editqstq.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing actual 'quest' bits of
//         a mob's quest info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


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
#include "editqstq.h"
#include "quest.h"

using namespace std;

extern char madeChanges;

//
// displayEditQuestQuestMenu : Displays the edit quest quest menu
//
//   *qst : quest quest being edited
//

void displayEditQuestQuestMenu(questQuest *qst, const char *mobName,
                               const long mobNumb)
{
  char strn[256], newName[256], ch;
  questItem *recv = qst->questPlayRecvHead, *give = qst->questPlayGiveHead;
  ulong numbShown = 0;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobNameShort(mobName, newName);

  sprintf(strn, "&+gEditing specific quest info for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wEdit mob's reply to finishing this quest &+c(exists: %s)&n\n"
  "   &+YB&+L.&n &+wEdit mob's disappearance message &+c(exists: %s)&n\n\n",
            getYesNoStrn((void *)(qst->questReplyHead)),
            getYesNoStrn((void *)(qst->disappearHead)));
  }
  else
  {
    strcpy(strn,
  "   &+YA&+L.&n &+wEdit mob's reply to finishing this quest\n"
  "   &+YB&+L.&n &+wEdit mob's disappearance message\n\n");
  }

  displayColorString(strn);

  if (!recv && !give)
  {
    displayColorString("&+w  This quest contains no give/receive information.\n");
  }

 // run through stuff player gives to mob

  while (give)
  {
    if ((numbShown + FIRST_QSTQST_CH) > LAST_QSTQST_CH)
      ch = FIRST_POSTALPHA_CH +
           (numbShown - (LAST_QSTQST_CH - FIRST_QSTQST_CH)) - 1;
    else ch = numbShown + FIRST_QSTQST_CH;

    sprintf(strn, "   &+Y%c&+L.&n &+wMob wants %s&n\n",
            ch, getQuestItemStrn(give, QUEST_GIVEITEM, newName));

    displayColorString(strn);

    numbShown++;

    if ((numbShown % MAX_QSTQST_ENTRIES) == 0)
    {
      displayColorString("\n&+CPress any key to continue..&n");
      getkey();
      displayColorString("\n\n");
    }

    give = give->Next;
  }

 // run through stuff mob gives to player

  while (recv)
  {
    if ((numbShown + FIRST_QSTQST_CH) > LAST_QSTQST_CH)
      ch = FIRST_POSTALPHA_CH +
          (numbShown - (LAST_QSTQST_CH - FIRST_QSTQST_CH)) - 1;
    else ch = numbShown + FIRST_QSTQST_CH;

    sprintf(strn, "   &+Y%c&+L.&n &+wMob gives %s&n\n",
            ch, getQuestItemStrn(recv, QUEST_RECVITEM, newName));

    displayColorString(strn);

    numbShown++;

    if ((numbShown % MAX_QSTQST_ENTRIES) == 0)
    {
      displayColorString("\n&+CPress any key to continue..&n");
      getkey();
      displayColorString("\n\n");
    }

    recv = recv->Next;
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
// interpEditQuestQuestMenu : Interpret command user typed - returns TRUE
//                            if the user hits 'Q'
//
//               ch : command typed by user
//   *extraDescNode : node being edited
//

char interpEditQuestQuestMenu(char ch, questQuest *qst, const char *mobName,
                              const long mobNumb)
{
//  char strn[256];
  struct rccoord coords;
  ulong numbRecv = getNumbItemNodes(qst->questPlayRecvHead),
        numbGive = getNumbItemNodes(qst->questPlayGiveHead);
  questItem *item;


 // delete stuff

  if (ch == 'Y') //&& ((numbRecv) || (numbGive)))
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString(
"&+cEnter letter of entry to delete (&+CB to delete disappearance info&n&+c): ");

    do
    {
      ch = toupper(getkey());

      if ((ch == 'X') || (ch == K_Enter) || (ch == K_Escape))
      {
        displayEditQuestQuestMenu(qst, mobName, mobNumb);
        return FALSE;
      }

      if ((ch == 'B') && (qst->disappearHead)) break;

      if ((ch >= FIRST_POSTALPHA_CH) && (ch < 'A'))
         ch += (LAST_QSTQST_CH - FIRST_POSTALPHA_CH) + 1;  // it's magic
    } while (!((numbRecv + numbGive) > (ch - FIRST_QSTQST_CH)));

    if ((ch == 'B') && (qst->disappearHead))
    {
      deleteStringNodes(qst->disappearHead);
      qst->disappearHead = NULL;

      displayEditQuestQuestMenu(qst, mobName, mobNumb);

      return FALSE;
    }
    else

    if ((numbGive + numbRecv) > (ch - FIRST_QSTQST_CH))
    {
      if (((ch - FIRST_QSTQST_CH) <= (numbGive - 1)) && numbGive)  // give
      {
        deleteQuestIteminList(&(qst->questPlayGiveHead),
           getItemNodeNumb(ch - FIRST_QSTQST_CH, qst->questPlayGiveHead));
      }
      else  // receive
      {
        deleteQuestIteminList(&(qst->questPlayRecvHead),
           getItemNodeNumb(ch - FIRST_QSTQST_CH - numbGive,
                           qst->questPlayRecvHead));
      }
    }

    displayEditQuestQuestMenu(qst, mobName, mobNumb);
  }
  else

 // create something

  if (ch == 'Z')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+c"
//"Create new item &+Cg&n&+civen to mob, &+Cr&n&+ceceived by PCs, or e&+CX&n&+cit (&+Cg/r/X&n&+c)? ");
"Create new item given to &+Cm&n&+cob, given to &+CP&n&+cC, or e&+CX&n&+cit (&+Cm/p/X&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    }
    while ((ch != 'M') && (ch != 'P') && (ch != 'G') && (ch != 'R') &&
           (ch != 'X') && (ch != K_Enter));

    switch (ch)
    {
      case 'G' :  // support old key
      case 'M' :
        item = createQuestItem();
        addQuestItemtoList(&(qst->questPlayGiveHead), item);

        break;

      case 'R' :  // support old key
      case 'P' :
        item = createQuestItem();
        addQuestItemtoList(&(qst->questPlayRecvHead), item);

        break;

      case 'X' :
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

    displayEditQuestQuestMenu(qst, mobName, mobNumb);
  }
  else

 // edit mob reply

  if (ch == 'A')
  {
    qst->questReplyHead = editStringNodes(qst->questReplyHead, FALSE);

    displayEditQuestQuestMenu(qst, mobName, mobNumb);
  }
  else

 // edit mob disappearance message

  if (ch == 'B')
  {
    if (!qst->disappearHead)
    {
      coords = _gettextposition();

      _settextposition(coords.row, 1);

      clrline(coords.row);
      displayColorString(
"&+cThis mob currently has no disappearance message - create one (&+CY/n&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

      if (ch == 'N')
      {
        displayEditQuestQuestMenu(qst, mobName, mobNumb);

        return FALSE;
      }
    }

    qst->disappearHead = editStringNodes(qst->disappearHead, FALSE);

    displayEditQuestQuestMenu(qst, mobName, mobNumb);
  }
  else

 // get input, and love it, cherish it, and best of all, sex it up a little

  if (((ch >= FIRST_QSTQST_CH) || (ch >= FIRST_POSTALPHA_CH)) &&
      (numbRecv || numbGive))
  {
    if ((ch >= FIRST_POSTALPHA_CH) && (ch < 'A'))
      ch += (LAST_QSTQST_CH - FIRST_POSTALPHA_CH) + 1;  // it's magic

    if ((numbRecv + numbGive) > (ch - FIRST_QSTQST_CH))
    {
      // edit one or the other

      if (((ch - FIRST_QSTQST_CH) <= (numbGive - 1)) && numbGive)
                                         // edit items given to mob
      {
        editQuestItem(getItemNodeNumb(ch - FIRST_QSTQST_CH,
                       qst->questPlayGiveHead), mobName, mobNumb,
                       QUEST_GIVEITEM);

        displayEditQuestQuestMenu(qst, mobName, mobNumb);
      }
      else
      {
        editQuestItem(getItemNodeNumb(ch - FIRST_QSTQST_CH - numbGive,
                       qst->questPlayRecvHead), mobName, mobNumb,
                       QUEST_RECVITEM);

        displayEditQuestQuestMenu(qst, mobName, mobNumb);
      }
    }
  }
  else

  if ((checkMenuKey(ch, FALSE) == MENUKEY_SAVE) && (ch != 'Q')) return TRUE;

  return FALSE;
}


//
// editQuestQuest : Edit a 'quest quest'..  More accurately, the "main" menu
//                  function for the menu to do just that.  Returns TRUE if
//                  changes have been made.
//
//     *qst : quest quest being edited
//

uchar editQuestQuest(questQuest *qst, const char *mobName, const long mobNumb)
{
  char ch;
  questQuest *newQst;


  if (!qst)
  {
    cout << "\n\n*qst in editQuestQuest is NULL: aborting.\n\n";
    return FALSE;
  }

  newQst = copyQuestQuest(qst);
  if (!newQst)
  {
    cout << "\n\nError copying questQuest in editQuestQuest(): aborting.\n\n";

    return FALSE;
  }

 // display the menu

  displayEditQuestQuestMenu(newQst, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteQuestQuest(newQst);

      _outtext("\n\n");

      return FALSE;
    }


   // if interpEditQuestQuestMenu is TRUE, user has quit (hit 'Q')

    if (interpEditQuestQuestMenu(ch, newQst, mobName, mobNumb))
    {

     // see if changes have been made in the message

      if (!compareQuestQuest(qst, newQst))
      {
        deleteStringNodes(qst->questReplyHead);
        deleteQuestItemList(qst->questPlayRecvHead);
        deleteQuestItemList(qst->questPlayGiveHead);
        deleteStringNodes(qst->disappearHead);

        memcpy(qst, newQst, sizeof(questQuest));

        delete newQst;

        madeChanges = TRUE;

        _outtext("\n\n");

        return TRUE;
      }
      else
      {
        deleteQuestQuest(newQst);

        _outtext("\n\n");

        return FALSE;
      }
    }
  }
}
