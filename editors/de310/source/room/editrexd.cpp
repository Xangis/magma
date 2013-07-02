//
//  File: editrexd.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing room extra descs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"

#include "editrexd.h"
#include "room.h"
#include "exit.h"

#include "../misc/menu.h"
#include "../keys.h"

using namespace std;
//
// displayEditRoomExtraDescMenu : displays the edit extra desc menu for rooms
//
//            *room : pointer to room being edited
//   *extraDescNode : head of extraDesc list being edited
//  *numbExtraDescs : pointer to data that is set to the number of extra descs
//                    the room contains
//

void displayEditRoomExtraDescMenu(const dikuRoom *room,
                                  extraDesc *extraDescNode,
                                  uchar *numbExtraDescs)
{
  char strn[256], strn2[256], newName[256], ch = 'A';
  ulong numbExtraDescNodes;


 // clear the screen, set colors..

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixRoomName(room->roomName, newName);

 // display title

  sprintf(strn,
          "&+gEditing extra descs for room #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          room->roomNumber, newName);

  displayColorString(strn);

 // if room has no extra descs, say so

  if (!extraDescNode)
  {
    displayColorString(" &+wThis room has no extra descriptions.&n\n\n");
  }

 // set numbExtraDescNodes and *numbExtraDescs

  numbExtraDescNodes = getNumbExtraDescs(extraDescNode);

  if (numbExtraDescNodes > MAX_EDITABLE_EXTRADESCS)
        numbExtraDescNodes = MAX_EDITABLE_EXTRADESCS;
                               // max supported at this time

  *numbExtraDescs = numbExtraDescNodes;

 // run through the list of extraDescNodes, stopping when we hit the end of
 // the list or when the max supported displayable is hit

  while (extraDescNode)
  {
    getReadableKeywordStrn(extraDescNode->keywordListHead, strn2);

    sprintf(strn, "   &+Y%c&+L.&n  &+w%s\n", ch, strn2);
    displayColorString(strn);

    ch++;
    if (ch > ('A' + (numbExtraDescNodes - 1))) break;
                                  // do a li'l check - ch starts at 'A'

    extraDescNode = extraDescNode->Next;
  }


 // display the rest of the stuff

  displayColorString(
"\n"
"   &+YY&+L.&n  &+wDelete an extra desc\n"
"   &+YZ&+L.&n  &+wCreate a new extra desc\n"
"\n"
MENU_COMMON
"\n");

  displayColorString(getMenuPromptName());
}


//
// interpEditRoomExtraDescMenu : Interprets user input - returns TRUE if user
//                               hit 'Q', FALSE otherwise
//
//               ch : user input
//            *room : room record associated with extra descs being edited
//  **extraDescNode : pointer to pointer to extra desc being edited
//  *numbExtraDescs : number of extra descs
//

char interpEditRoomExtraDescMenu(usint ch, dikuRoom *room,
                                 extraDesc **extraDescHead,
                                 uchar *numbExtraDescs)
{
  ulong i = 0;
//  usint input;
  extraDesc *descNode = *extraDescHead;


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // create a new desc

  if (ch == 'Z')
  {
    createExtraDesc(extraDescHead, NULL);

    displayEditRoomExtraDescMenu(room, *extraDescHead, numbExtraDescs);

    return FALSE;
  }
  else

 // delete a desc

  if ((ch == 'Y') && *extraDescHead)
  {
    deleteExtraDescUser(extraDescHead, numbExtraDescs, 2, room);

    return FALSE;

/*
    _settextposition(24, 1);

    displayColorString(
"&+CEnter letter of extra desc to delete (Q to abort): &n");

    do
    {
      input = toupper(getkey());
    }
    while (!((input == 'Q') ||
            ((input >= 'A') && (input <= ('A' + ((*numbExtraDescs) - 1))))));

    if (input != 'Q')
    {
  //    if (input > 'Q') input--;  // this shouldn't happen w/ a limit of 15

      input -= 'A';

      while (descNode)
      {
        if (i == input)
        {
          if (descNode->Last)
          {
            descNode->Last->Next = descNode->Next;
          }

          if (descNode->Next)
          {
            descNode->Next->Last = descNode->Last;
          }

          if (descNode == *extraDescHead)//room->extraDescHead)
          {
//            room->extraDescHead = *extraDescHead = descNode->Next;
            *extraDescHead = descNode->Next;
          }

          deleteOneExtraDesc(descNode);

          displayEditRoomExtraDescMenu(room, *extraDescHead, numbExtraDescs);

          return FALSE;
        }

        i++;
        descNode = descNode->Next;
      }

      if (!descNode || (i > input))  // error
      {
        _outtext("\n\nWhoopsie - big error in interpEditRoomExtraDescMenu() - "
                "aborting..");

        exit(1);
      }
    }

    displayEditRoomExtraDescMenu(room, *extraDescHead, numbExtraDescs);
*/
  }
  else

  if ((ch >= 'A') && (ch <= ('A' + ((*numbExtraDescs) - 1))))
  {
//    if (ch > 'Q') ch--;  // this shouldn't happen w/ a limit of 15

    ch -= 'A';

    while (descNode)
    {
     // go to the wonderful "secondary" edit extra desc menu

      if (i == ch)
      {
        if (editExtraDesc(descNode))
        {
//          deleteMasterKeywordList(room->masterListHead);  // done below
//          room->masterListHead = createMasterKeywordList(room);

//          deleteEditableList(room->editabledListHead);
//          room->editableListHead = createEditableList(room);
        }

        displayEditRoomExtraDescMenu(room, *extraDescHead, numbExtraDescs);

        return FALSE;
      }

      i++;
      descNode = descNode->Next;
    }

    if (!descNode || (i > ch))  // error
    {
      cout << "\n\nWhoopsie - big error in interpEditRoomExtraDescMenu() - "
              "aborting..";

      exit(1);
    }
  }

  return FALSE;
}


//
// editRoomExtraDesc : The "main" function for the edit room extra desc menu -
//                     lists all the extra descs, etc.
//
//   *room : room being edited
//

void editRoomExtraDesc(dikuRoom *room)
{
  uchar ch;
  uchar numbExtraDescs;
  extraDesc *newExtraDescHead = copyExtraDescs(room->extraDescHead);


 // display the menu

  displayEditRoomExtraDescMenu(room, newExtraDescHead, &numbExtraDescs);


  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteExtraDescs(newExtraDescHead);

      _outtext("\n\n");

      return;
    }

    if (interpEditRoomExtraDescMenu(ch, room, &newExtraDescHead,
                                    &numbExtraDescs))
    {
      if (!compareExtraDescs(room->extraDescHead, newExtraDescHead))
      {
        deleteExtraDescs(room->extraDescHead);
        room->extraDescHead = newExtraDescHead;

        deleteMasterKeywordList(room->masterListHead);
        room->masterListHead = createMasterKeywordList(room);

        deleteEditableList(room->editableListHead);
        room->editableListHead = createEditableList(room);
      }
      else deleteExtraDescs(newExtraDescHead);

      return;
    }
  }
}
