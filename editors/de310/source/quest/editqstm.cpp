//
//  File: editqstm.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing mob quest messages
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <iostream>
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

using namespace std;

extern char madeChanges;

//
// displayEditQuestMessageMenu : Displays the edit extra desc menu
//
//   *extraDescNode : extra desc being edited
//

void displayEditQuestMessageMenu(questMessage *msg)
{
  char strn[256], keywords[256], *outStrn = new char[4096];


 // clear the screen

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

 // turn the keyword list into something that's readable by mere mortals

  getReadableKeywordStrn(msg->keywordListHead, keywords);

 // display the menu

  sprintf(strn, "&+gEditing quest message &+L\"&n%s&+L\"&n\n\n", keywords);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit quest message triggers\n"
  "   &+YB&+L.&n &+wEdit quest message reply &+c(exists: %s)&n\n"
  "\n"
  MENU_COMMON
  "\n", getYesNoStrn((void *)(msg->questMessageHead)));
  }
  else
  {
    strcpy(outStrn,
  "   &+YA&+L.&n &+wEdit quest message triggers\n"
  "   &+YB&+L.&n &+wEdit quest message reply\n"
  "\n"
  MENU_COMMON
  "\n");
  }

  strcat(outStrn, getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditQuestMessageMenu : Interpret command user typed - returns TRUE
//                              if the user hits 'Q'
//
//               ch : command typed by user
//   *extraDescNode : node being edited
//

char interpEditQuestMessageMenu(char ch, questMessage *msg)
{
  char strn[256];
  struct rccoord coords;


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // edit keywords

  if (ch == 'A')
  {
   // create "readable" version of the keywords

    createKeywordString(msg->keywordListHead, strn);
    strn[strlen(strn) - 1] = '\0';  // get rid of the tilde

   // get user input

    coords = _gettextposition();

    _settextposition(coords.row, 1);

    displayColorString("&+YKeywords: &n");

    getStrn(strn, 256 - 1, 1, 7, '°', strn, FALSE, FALSE);

   // add the ~ at the end

    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    strcat(strn, "~");

   // rebuild the keyword list

    deleteStringNodes(msg->keywordListHead);

    msg->keywordListHead = createKeywordList(strn);

   // display the menu

    displayEditQuestMessageMenu(msg);
  }
  else

 // edit description

  if (ch == 'B')
  {
    msg->questMessageHead = editStringNodes(msg->questMessageHead, FALSE);

    displayEditQuestMessageMenu(msg);
  }

  return FALSE;
}


//
// editQuestMessage : Edit a quest msg..  More accurately, the "main" menu
//                    function for the menu to do just that.  Returns TRUE if
//                    changes have been made.
//
//     *msg : quest message being edited
//

uchar editQuestMessage(questMessage *msg)
{
  char ch;
  questMessage *newMsg;


  if (!msg)
  {
    cout << "\n\n*msg in editQuestMessage is NULL: aborting.\n\n";
    return FALSE;
  }

  newMsg = copyQuestMessage(msg);
  if (!newMsg)
  {
    cout << "\n\nError copying questMessage in editQuestMessage(): aborting.\n\n";

    return FALSE;
  }

 // display the menu

  displayEditQuestMessageMenu(newMsg);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteQuestMessage(newMsg);

      _outtext("\n\n");

      return FALSE;
    }


   // if interpEditQuestMessageMenu is TRUE, user has quit (hit 'Q')

    if (interpEditQuestMessageMenu(ch, newMsg))
    {

     // see if changes have been made in the message

      if (!compareQuestMessage(msg, newMsg))
      {
        deleteStringNodes(msg->keywordListHead);
        deleteStringNodes(msg->questMessageHead);

        memcpy(msg, newMsg, sizeof(questMessage));

        delete newMsg;

        madeChanges = TRUE;

        _outtext("\n\n");

        return TRUE;
      }
      else
      {
        deleteQuestMessage(newMsg);

        _outtext("\n\n");

        return FALSE;
      }
    }
  }
}
