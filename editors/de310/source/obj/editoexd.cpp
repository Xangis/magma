//
//  File: editoexd.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing object extra descs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"

#include "object.h"
#include "../edesc/edesc.h"

#include "../misc/menu.h"
#include "editoexd.h"

using namespace std;

//
// displayEditObjExtraDescMenu : displays the edit extra desc menu for objs
//
//             *obj : pointer to obj being edited
//   *extraDescNode : head of extraDesc list being edited
//  *numbExtraDescs : pointer to data that is set to the number of extra descs
//                    the obj contains
//

void displayEditObjExtraDescMenu(dikuObject *obj, extraDesc *extraDescNode,
                                 uchar *numbExtraDescs)
{
  char strn[256], strn2[256], newName[256], ch = 'A';
  ulong numbExtraDescNodes;


 // clear the screen, set colors..

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixObjName(obj->objShortName, newName);

 // display title

  sprintf(strn,
          "&+gEditing extra descs for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  displayColorString(strn);

 // if obj has no extra descs, say so

  if (!extraDescNode)
  {
    displayColorString(" &+wThis obj has no extra descriptions.&n\n\n");
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

    sprintf(strn, "   &+Y%c&+L.&n &+w%s\n", ch, strn2);
    displayColorString(strn);

    ch++;
    if (ch > ('A' + (numbExtraDescNodes - 1))) break;
                                  // do a li'l check - ch starts at 'A'

    extraDescNode = extraDescNode->Next;
  }


 // display the rest of the stuff

  displayColorString(
"\n"
"   &+YU&+L.&n &+wEdit keywords that replace old keywords upon identification\n"
"   &+YV&+L.&n &+wEdit short name that replaces old name upon identification\n"
"   &+YW&+L.&n &+wEdit long name that replaces old name upon identification\n"
"\n"
"   &+YY&+L.&n &+wDelete an extra desc\n"
"   &+YZ&+L.&n &+wCreate a new extra desc\n"
"\n"
MENU_COMMON
"\n");

  displayColorString(getMenuPromptName());
}


//
// interpEditObjExtraDescMenu : Interprets user input - returns TRUE if user
//                              hit 'Q', FALSE otherwise
//
//               ch : user input
//             *obj : obj record associated with extra descs being edited
//  **extraDescNode : pointer to pointer to extra desc being edited
//  *numbExtraDescs : number of extra descs
//

char interpEditObjExtraDescMenu(usint ch, dikuObject *obj,
                                extraDesc **extraDescHead,
                                uchar *numbExtraDescs,
                                char *addedKeyword)
{
  ulong i = 0;
  extraDesc *descNode;
  char strn[256];
//  stringNode *strnNode, *strnNode2;


  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

 // user picked Z to create a new desc

  if (ch == 'Z')
  {
    if ((!*extraDescHead) && getFirstObjEdescKeyVal())
    {
      createExtraDesc(extraDescHead,
                      createKeywordString(obj->keywordListHead, strn));
    }
    else createExtraDesc(extraDescHead, NULL);

    displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);

    return FALSE;
  }
  else

 // turn all this crap into a separate function

  if ((ch == 'Y') && *extraDescHead)
  {
    deleteExtraDescUser(extraDescHead, numbExtraDescs, 1, obj);

    return FALSE;
  }
  else

 // special identification edescs - keywords

  if (ch == 'U')
  {
    descNode = getEdescinList(*extraDescHead, "_ID_NAME_");

   // shucks, no match

    if (!descNode)
    {
      descNode = createExtraDesc(extraDescHead, "_ID_NAME_~");

     // add '_id_' keyword to object if it doesn't have it

      if (!scanKeyword("_ID_", obj->keywordListHead))
      {
        addKeywordtoList(&obj->keywordListHead, "_ID_");

        *addedKeyword = TRUE;
      }
    }

//    editExtraDesc(descNode);
    descNode->extraDescStrnHead =
        editStringNodes(descNode->extraDescStrnHead, TRUE);

    displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);

    return FALSE;
  }
  else

 // special identification edescs - short name

  if (ch == 'V')
  {
    descNode = getEdescinList(*extraDescHead, "_ID_SHORT_");

   // shucks, no match

    if (!descNode)
    {
      descNode = createExtraDesc(extraDescHead, "_ID_SHORT_~");

     // add '_id_' keyword to object if it doesn't have it

      if (!scanKeyword("_ID_", obj->keywordListHead))
      {
        addKeywordtoList(&obj->keywordListHead, "_ID_");

        *addedKeyword = TRUE;
      }
    }

//    editExtraDesc(descNode);
    descNode->extraDescStrnHead =
        editStringNodes(descNode->extraDescStrnHead, TRUE);

    displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);

    return FALSE;
  }
  else

 // special identification edescs - long name

  if (ch == 'W')
  {
    descNode = getEdescinList(*extraDescHead, "_ID_DESC_");

   // shucks, no match

    if (!descNode)
    {
      descNode = createExtraDesc(extraDescHead, "_ID_DESC_~");

     // add '_id_' keyword to object if it doesn't have it

      if (!scanKeyword("_ID_", obj->keywordListHead))
      {
        addKeywordtoList(&obj->keywordListHead, "_ID_");

        *addedKeyword = TRUE;
      }
    }

//    editExtraDesc(descNode);
    descNode->extraDescStrnHead =
        editStringNodes(descNode->extraDescStrnHead, TRUE);

    displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);

    return FALSE;
  }
  else

  if ((ch >= 'A') && (ch <= ('A' + ((*numbExtraDescs) - 1))))
  {
    ch -= 'A';

    descNode = *extraDescHead;
    while (descNode)
    {
     // go to the wonderful "secondary" edit extra desc menu

      if (i == ch)
      {
        if (editExtraDesc(descNode))
        {
//          deleteMasterKeywordList(obj->masterListHead);
//          obj->masterListHead = createMasterKeywordList(obj);
        }

        displayEditObjExtraDescMenu(obj, *extraDescHead, numbExtraDescs);

        return FALSE;
      }

      i++;
      descNode = descNode->Next;
    }

    if (!descNode || (i > ch))  // error
    {
      cout << "\n\nWhoopsie - big error in interpEditObjExtraDescMenu() - "
              "aborting..";

      exit(1);
    }
  }

  return FALSE;
}


//
// editObjExtraDesc : The "main" function for the edit obj extra desc menu -
//                    lists all the extra descs, etc. - returns TRUE if
//                    user aborted, FALSE otherwise
//
//   *obj : obj being edited
//

char editObjExtraDesc(dikuObject *obj, char *addedIdentKeyword)
{
  uchar ch;
  uchar numbExtraDescs;
  extraDesc *newExtraDescHead = copyExtraDescs(obj->extraDescHead);


 // display the menu

  displayEditObjExtraDescMenu(obj, newExtraDescHead, &numbExtraDescs);


  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      deleteExtraDescs(newExtraDescHead);

      _outtext("\n\n");

      return TRUE;
    }

    if (interpEditObjExtraDescMenu(ch, obj, &newExtraDescHead,
                                    &numbExtraDescs, addedIdentKeyword))
    {
      if (!compareExtraDescs(obj->extraDescHead, newExtraDescHead))
      {
        deleteExtraDescs(obj->extraDescHead);
        obj->extraDescHead = newExtraDescHead;

/*
        deleteMasterKeywordList(obj->masterListHead);
        obj->masterListHead = createMasterKeywordList(obj);

        deleteEditableList(obj->editableListHead);
        obj->editableListHead = createEditableList(obj);
*/
      }
      else deleteExtraDescs(newExtraDescHead);

      return FALSE;
    }
  }
}
