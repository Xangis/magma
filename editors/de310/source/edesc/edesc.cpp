//
//  File: edesc.cpp      originally part of dikuEdit
//
//  Usage: functions for manipulating extra desc records
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include "../misc/menu.h"
#include "../fh.h"
#include "../types.h"
#include "edesc.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

using namespace std;

extern char madeChanges;

//
// getNumbExtraDescs : Returns the number of extra descs in a list
//
//   *extraDescNode : head of list
//

INLINEDEF ulong getNumbExtraDescs(extraDesc *extraDescNode)
{
  ulong i = 0;


  while (extraDescNode)
  {
    i++;

    extraDescNode = extraDescNode->Next;
  }

  return i;
}

//
// copyExtraDescs : Copies a list of extra descs, returning the address of the
//                  head of the new list
//
//   *srcExtraDesc : head node of list to copy
//

extraDesc *copyExtraDescs(extraDesc *srcExtraDesc)
{
  extraDesc *newExtraDesc, *prevExtraDesc = NULL, *headExtraDesc = NULL;


  if (!srcExtraDesc) return NULL;

  while (srcExtraDesc)
  {
    newExtraDesc = new extraDesc;
    if (!newExtraDesc)
    {
      cout << "\n\nError creating new extraDesc in copyExtraDescs().  "
              "Out of memory.\n\n";

      return NULL;
    }

    memset(newExtraDesc, 0, sizeof(extraDesc));

    newExtraDesc->Last = prevExtraDesc;
//    newExtraDesc->Next = NULL;

    if (!headExtraDesc) headExtraDesc = newExtraDesc;

    if (prevExtraDesc) prevExtraDesc->Next = newExtraDesc;
    prevExtraDesc = newExtraDesc;

    newExtraDesc->keywordListHead =
           copyStringNodes(srcExtraDesc->keywordListHead);
    newExtraDesc->extraDescStrnHead =
           copyStringNodes(srcExtraDesc->extraDescStrnHead);


    srcExtraDesc = srcExtraDesc->Next;
  }

  return headExtraDesc;
}


//
// deleteExtraDescs : Deletes a list of extraDescs
//
//   *srcExtraDesc : head of list to delete
//

void deleteExtraDescs(extraDesc *srcExtraDesc)
{
  extraDesc *nextExtraDesc;


  while (srcExtraDesc)
  {
    nextExtraDesc = srcExtraDesc->Next;

    deleteStringNodes(srcExtraDesc->keywordListHead);
    deleteStringNodes(srcExtraDesc->extraDescStrnHead);

    delete srcExtraDesc;

    srcExtraDesc = nextExtraDesc;
  }
}


//
// copyOneExtraDesc : Copies just the specified extraDesc, returning the
//                    address of the copy
//
//   *srcExtraDesc : source to copy from
//

extraDesc *copyOneExtraDesc(const extraDesc *srcExtraDesc)
{
  extraDesc *newExtraDesc;


  if (!srcExtraDesc) return NULL;

  newExtraDesc = new extraDesc;
  if (!newExtraDesc)
  {
    cout << "\n\nError creating new extraDesc in copyOneExtraDesc().  "
            "Out of memory.\n\n";

    return NULL;
  }

  memcpy(newExtraDesc, srcExtraDesc, sizeof(extraDesc));

  newExtraDesc->keywordListHead =
         copyStringNodes(srcExtraDesc->keywordListHead);
  newExtraDesc->extraDescStrnHead =
         copyStringNodes(srcExtraDesc->extraDescStrnHead);

  return newExtraDesc;
}


//
// deleteOneExtraDesc : As you might guess, deletes just the specified
//                      extraDesc
//
//   *extraDescNode : extraDesc to delete
//

void deleteOneExtraDesc(extraDesc *extraDescNode)
{
  if (!extraDescNode) return;

  deleteStringNodes(extraDescNode->keywordListHead);
  deleteStringNodes(extraDescNode->extraDescStrnHead);

  delete extraDescNode;
}


//
// compareExtraDescs : Compares two lists of extra descs - returns TRUE if
//                     they match
//
//  *list1 : the first list
//  *list2 : take a guess, I'm sure you'll be pretty close..
//

char compareExtraDescs(extraDesc *list1, extraDesc *list2)
{
  if (list1 == list2) return TRUE;
  if (!list1 || !list2) return FALSE;

  while (list1 && list2)
  {
    if (!compareStringNodes(list1->keywordListHead,
                            list2->keywordListHead)) return FALSE;

    if (!compareStringNodes(list1->extraDescStrnHead,
                            list2->extraDescStrnHead)) return FALSE;

    list1 = list1->Next;
    list2 = list2->Next;
  }

  if ((!list1 && list2) || (list1 && !list2)) return FALSE;

  return TRUE;
}


//
// compareOneExtraDesc : Compares two extra desc nodes - returns TRUE if
//                       they match
//
//  *node1 : the first list
//  *node2 : take a guess, I'm sure you'll be pretty close - well, maybe
//

char compareOneExtraDesc(extraDesc *node1, extraDesc *node2)
{
  if (node1 == node2) return TRUE;
  if (!node1 || !node2) return FALSE;

  if (!compareStringNodes(node1->keywordListHead,
                          node2->keywordListHead)) return FALSE;

  if (!compareStringNodes(node1->extraDescStrnHead,
                          node2->extraDescStrnHead)) return FALSE;


  return TRUE;
}


//
// getEdescinList
//

extraDesc *getEdescinList(extraDesc *descHead, const char *keyword)
{
  extraDesc *descNode = descHead;

  if (!descHead) return NULL;

  while (descNode)
  {
    if (scanKeyword(keyword, descNode->keywordListHead)) return descNode;

    descNode = descNode->Next;
  }

  return NULL;
}

//
//  File: editdesc.cpp   originally part of dikuEdit
//
//  Usage: functions related to users editing user descs
//
//  Copyright 1995-98 (C) Michael Glosenger
//
//
// displayEditExtraDescMenu : Displays the edit extra desc menu
//
//   *extraDescNode : extra desc being edited
//

void displayEditExtraDescMenu(const extraDesc *extraDescNode)
{
  char strn[256], keywords[256], *outStrn = new char[4096];


 // clear the screen

  clrscr();

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

 // turn the keyword list into something that's readable by mere mortals

  getReadableKeywordStrn(extraDescNode->keywordListHead, keywords);

 // display the menu

  sprintf(strn, "&n&+gEditing extra desc &+L\"&n%s&+L\"&n\n\n", keywords);

  displayColorString(strn);

/*  // too spammy
  if (displayIdentInfo)
  {
    displayColorString(
"  'identification' extra descs are used to reset the object's keywords,\n"
"  short name, and long name upon having the 'identify' spell cast upon it -\n"
"  simply put the new keywords, short name, or long name on the first line\n"
"  of the extra desc description.  Note that all objects with the identification\n"
"  extra desc need the keyword '_id_' in their keyword list.  It should be\n"
"  added automatically upon editing an identification extra desc or loading\n"
"  the .OBJ file, but you can't be too careful..\n\n");
  }
*/

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit extra desc keywords\n"
  "   &+YB&+L.&n &+wEdit extra desc description &+c(exists: %s)&n\n"
  "\n"
  MENU_COMMON
  "\n", getYesNoStrn((void *)(extraDescNode->extraDescStrnHead)));
  }
  else
  {
    strcpy(outStrn,
  "   &+YA&+L.&n &+wEdit extra desc keywords\n"
  "   &+YB&+L.&n &+wEdit extra desc description\n"
  "\n"
  MENU_COMMON
  "\n");
  }

  strcat(outStrn, getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// interpEditRoomExtraDescMenu : Interpret command user typed - returns TRUE
//                               if the user hits 'Q'
//
//               ch : command typed by user
//   *extraDescNode : node being edited
//

char interpEditExtraDescMenu(const usint ch, extraDesc *extraDescNode)
{
  char strn[256];
  struct rccoord coords;


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // edit keywords

  if (ch == 'A')
  {
   // create "readable" version of the keywords

    createKeywordString(extraDescNode->keywordListHead, strn);
    strn[strlen(strn) - 1] = '\0';  // get rid of the tilde

   // get user input

    coords = _gettextposition();

    _settextposition(coords.row, 1);

    displayColorString("&+YKeywords: &n");

    getStrn(strn, MAX_EDESCKEY_LEN - 1, 1, 7, '°', strn, FALSE, FALSE);

   // add the ~ at the end

    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    if (strn[strlen(strn) - 1] != '~') strcat(strn, "~");

   // rebuild the keyword list

    deleteStringNodes(extraDescNode->keywordListHead);

    extraDescNode->keywordListHead = createKeywordList(strn);

   // display the menu

    displayEditExtraDescMenu(extraDescNode);
  }
  else

 // edit description

  if (ch == 'B')
  {
    extraDescNode->extraDescStrnHead =
        editStringNodes(extraDescNode->extraDescStrnHead, FALSE);

    displayEditExtraDescMenu(extraDescNode);
  }

  return FALSE;
}


//
// editExtraDesc : Edit an extra desc..  More accurately, the "main" menu
//                 function for the menu to do just that.  Returns TRUE if
//                 changes have been made.
//
//     *extraDescNode : extra desc being edited
//

uchar editExtraDesc(extraDesc *extraDescNode)
{
  usint ch;
  extraDesc *newExtraNode;


  newExtraNode = copyOneExtraDesc(extraDescNode);
  if (!newExtraNode)
  {
    _outtext("\n\nError copying extraDesc in editExtraDesc(): aborting.\n\n");

    return FALSE;
  }

 // display the menu

  displayEditExtraDescMenu(newExtraNode);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteOneExtraDesc(newExtraNode);

      _outtext("\n\n");

      return FALSE;
    }


   // if interpEditExtraDescMenu is TRUE, user has quit (hit 'Q')

    if (interpEditExtraDescMenu(ch, newExtraNode))
    {

     // see if changes have been made in the extra desc

      if (!compareOneExtraDesc(extraDescNode, newExtraNode))
      {
        deleteStringNodes(extraDescNode->keywordListHead);
        deleteStringNodes(extraDescNode->extraDescStrnHead);

        memcpy(extraDescNode, newExtraNode, sizeof(extraDesc));

        delete newExtraNode;

        madeChanges = TRUE;

        _outtext("\n\n");

        return TRUE;
      }
      else
      {
        deleteOneExtraDesc(newExtraNode);

        _outtext("\n\n");

        return FALSE;
      }
    }
  }
}


//
// deleteExtraDescUser :
//

void deleteExtraDescUser(extraDesc **extraDescHead,
                         uchar *numbExtraDescs, const char mode,
                         void *ptr)
{
  extraDesc *descNode;
  usint ch, i = 0;
  struct rccoord oldxy = _gettextposition();


  _settextposition(24, 1);

  displayColorString(
"&+CEnter letter of extra desc to delete (Q to abort): &n");

  do
  {
    ch = toupper(getkey());

    if (ch == 'Q')
    {
//      displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);
      clrline(24);
      _settextposition(oldxy.row, oldxy.col);

      return;
    }
  } while (!((ch >= 'A') && (ch <= ('A' + ((*numbExtraDescs) - 1)))));

  ch -= 'A';

  descNode = *extraDescHead;

  while (descNode)
  {
    if (i == ch)
    {
      if (descNode->Last)
      {
        descNode->Last->Next = descNode->Next;
      }

      if (descNode->Next)
      {
        descNode->Next->Last = descNode->Last;
      }

      if (descNode == *extraDescHead)
      {
        *extraDescHead = descNode->Next;
      }

      deleteOneExtraDesc(descNode);

      if (mode == 1)
        displayEditObjExtraDescMenu(
          (dikuObject *)ptr, *extraDescHead, numbExtraDescs);
      else if (mode == 2)
        displayEditRoomExtraDescMenu(
          (dikuRoom *)ptr, *extraDescHead, numbExtraDescs);

      return;
    }

    i++;
    descNode = descNode->Next;
  }

  if (!descNode || (i > ch))  // error
  {
    _outtext("\n\nWhoopsie - big error in interpEditObjExtraDescMenu() - "
             "aborting..");

    exit(1);
  }
}

//
//  File: crtdesc.cpp    originally part of dikuEdit
//
//  Usage: function(s) for creating extra descs
//
//  Copyright 1995-98 (C) Michael Glosenger
//

//
// createExtraDesc : Function to add a new extra desc to the end of a list
//
//   **extraDescHead : pointer to pointer to head of extra desc list
//

extraDesc *createExtraDesc(extraDesc **extraDescHead, const char *keywordStrn)
{
  extraDesc /**lastExtraDesc,*/ *newExtraDesc, *extraDescNode;


 // create a new extra desc

  newExtraDesc = new extraDesc;
  if (!newExtraDesc)
  {
    cout << "\n\nError creating new extraDesc in createExtraDesc().  "
            "(memory problem)  Aborting.\n\n";

    return NULL;
  }

  memset(newExtraDesc, 0, sizeof(extraDesc));
  if (!keywordStrn)
  {
    newExtraDesc->keywordListHead = createKeywordList("DEFAULT~");
  }
  else
  {
    newExtraDesc->keywordListHead = createKeywordList(keywordStrn);
  }

 // get to the end of the list

  if (!(*extraDescHead))  // no list exists - well damn...  bleh......
  {
    *extraDescHead = newExtraDesc;
  }
  else
  {
    extraDescNode = *extraDescHead;

    while (extraDescNode->Next)
    {
//      lastExtraDesc = extraDescNode;
//      if (!extraDescNode->Next) break;

      extraDescNode = extraDescNode->Next;
    }

    newExtraDesc->Last = extraDescNode;
    extraDescNode->Next = newExtraDesc;
//    extraDescNode = lastExtraDesc;
  }

  return newExtraDesc;
}
