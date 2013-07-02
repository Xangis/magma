//
//  File: editable.cpp   originally part of dikuEdit
//
//  Usage: functions for manipulating editableListNode records
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"

#include "editable.h"
#include "master.h"

using namespace std;

//
// createEditableList : Creates an "editable entities list" for the specified
//                      room node, returning a pointer to the head of the
//                      list.
//
//   *room : dikuRoom to create editable list for
//

editableListNode *createEditableList(dikuRoom *room)
{
  editableListNode *listHead = NULL, *listNode, *oldListNode = NULL;
  extraDesc *extraDescNode;
  objectHere *objHere;
  mobHere *mobHNode;


 // first, grab all of the keyword lists for extra descs in the room

  extraDescNode = room->extraDescHead;

  while (extraDescNode)
  {
    listNode = new editableListNode;

    if (!listNode)
    {
      cout << "Error creating editableListNode in createEditableList().  "
              "Aborting..\n";

      return NULL;
    }

    if (!listHead) listHead = listNode;

    listNode->keywordListHead = copyStringNodes(extraDescNode->keywordListHead);
    listNode->entityPtr = extraDescNode;
    listNode->entityType = ENTITY_R_EDESC;
    listNode->Next = NULL;

    if (oldListNode) oldListNode->Next = listNode;
    oldListNode = listNode;

    extraDescNode = extraDescNode->Next;
  }

 // now, grab all of the keyword lists for the objects in the room

  objHere = room->objectHead;

  while (objHere)
  {
    if (!objHere->objectPtr)
    {
      objHere = objHere->Next;
      continue;
    }

    listNode = new editableListNode;
    if (!listNode)
    {
      cout << "Error creating editableListNode in createEditableList().  "
              "Aborting..\n";

      exit(1);
    }

    if (!listHead) listHead = listNode;

   // copy the object's keywords

    listNode->keywordListHead =
         copyStringNodes(objHere->objectPtr->keywordListHead);
    listNode->entityPtr = objHere;
    listNode->entityOwner = room;
    listNode->entityType = ENTITY_OBJECT;
    listNode->Next = NULL;

    if (oldListNode) oldListNode->Next = listNode;
    oldListNode = listNode;

    objHere = objHere->Next;
  }

 // grab extra descs after all object keywords

  objHere = room->objectHead;

  while (objHere)
  {
    if (!objHere->objectPtr)
    {
      objHere = objHere->Next;
      continue;
    }

   // copy extra desc node stuff

    extraDescNode = objHere->objectPtr->extraDescHead;

    while (extraDescNode)
    {
      listNode = new editableListNode;

      if (!listNode)
      {
        cout << "Error creating editableListNode in createEditableList()." <<
                "  Aborting..\n";

        exit(1);
      }

      if (!listHead) listHead = listNode;  // ain't gonna happen but hey

     // copy the extra desc description

      listNode->keywordListHead =
           copyStringNodes(extraDescNode->keywordListHead);
      listNode->entityPtr = extraDescNode;
      listNode->entityOwner = objHere->objectPtr;
      listNode->entityType = ENTITY_OBJ_ED;
      listNode->Next = NULL;

      if (oldListNode) oldListNode->Next = listNode;
      oldListNode = listNode;


      extraDescNode = extraDescNode->Next;
    }

    objHere = objHere->Next;
  }

 // finally, grab all the mob keywords

  mobHNode = room->mobHead;

  while (mobHNode)
  {
    if (!mobHNode->mobPtr)
    {
      mobHNode = mobHNode->Next;
      continue;
    }

    listNode = new editableListNode;

    if (!listNode)
    {
      cout << "Error creating masterKeywordListNode in "
              "createMasterKeywordList().  Aborting..\n";

      exit(1);
    }

    if (!listHead) listHead = listNode;

   // copy the extra desc description

    listNode->keywordListHead =
         copyStringNodes(mobHNode->mobPtr->keywordListHead);
    listNode->entityOwner = room;
    listNode->entityPtr = mobHNode;
    listNode->entityType = ENTITY_MOB;
    listNode->Next = NULL;

    if (oldListNode) oldListNode->Next = listNode;
    oldListNode = listNode;


    mobHNode = mobHNode->Next;
  }


  return listHead;
}


//
// checkEditableList : Scans through the "editable" list passed to
//                     the function for the keyword specified by strn and
//                     strnPos.  whatMatched is set to the entity type
//                     that matched and *matchingNode is set to the address of
//                     the matching node.
//
//          *strn : string as entered by user
//        strnPos : position of keyword
//      *editNode : head of editableListNode list
//   *whatMatched : set based on what type of entity matched
// **matchingNode : upon a match, the matching node's address is put in this
//                  puppy
//  currentEntity : the number of entities we've already scanned through -
//                  entityToGrab takes this into account
//

char checkEditableList(const char *strn, editableListNode *editNode,
                       char *whatMatched, editableListNode **matchingNode,
                       char currentEntity)
{
  uchar entityToGrab = 1;//, currentEntity = 1;
  char strn2[256];


//  sprintf(strn2, "\neditNode passed to checkEditableList() is %x\n", editNode);
//  _outtext(strn2);

  *whatMatched = NO_MATCH;
  *matchingNode = NULL;

  if (!strn[0]) return 1;

//  _outtext("About to copy strn into strn2..\n");
  strcpy(strn2, strn);

  if (!editNode) return 1;

 // alrighty, check for numeric stuff

  if (isdigit(strn2[0]) && (strn2[0] != '0'))
  {
    if (strn2[1] == '.')
    {
      entityToGrab = strn2[0] - '0';

     // remove the "#." part

      deleteChar(strn2, 0);
      deleteChar(strn2, 0);
    }
  }

 // scan through the edit nodes, and enjoy it, dammit

  while (editNode)
  {
   // if user-specified word matches keyword list for this node, go crazy

    if (scanKeyword(strn2, editNode->keywordListHead))
    {
     // make sure editNode->entityType isn't out of range

      if ((editNode->entityType >= ENTITY_LOWEST) &&
          (editNode->entityType <= ENTITY_HIGHEST))
      {
       // if user specified something by dot notation (or even if they didn't,
       // but hey) make sure we're grabbing the right object

        if (entityToGrab == currentEntity)
        {
          *whatMatched = editNode->entityType;
          *matchingNode = editNode;

          return currentEntity;
        }

       // if not, increment currentEntity and go crazy

        else currentEntity++;
      }
      else
      {
        cout << "Error: editNode->entityType in checkEditableList()"
                " is invalid (" << editNode->entityType << ").  Aborting."
                "\n";

        return -1;
      }
    }

    editNode = editNode->Next;
  }

  return currentEntity;
}


//
// copyEditableList : Copies an editable list, returning the address
//                    of the head of the new list
//
//    *srcNode : list to copy from
//

editableListNode *copyEditableList(editableListNode *srcNode)
{
  editableListNode *newNode, *prevNode = NULL, *headNode = NULL;


  if (!srcNode) return NULL;

  while (srcNode)
  {
    newNode = new editableListNode;
    if (!newNode)
    {
      cout << "\n\nERROR: copyEditableList() - ran out of mem alloc'ing "
           << "new editableListNode node.  Aborting..\n\n";

      return NULL;
    }

    memset(newNode, 0, sizeof(editableListNode));
//    newNode->Next = NULL;

    if (!headNode) headNode = newNode;

    if (prevNode) prevNode->Next = newNode;
    prevNode = newNode;


   // first, the easy stuff

    memcpy(newNode, srcNode, sizeof(editableListNode));

   // now, the not quite as easy but still relatively simple stuff

    newNode->keywordListHead = copyStringNodes(srcNode->keywordListHead);


    srcNode = srcNode->Next;
  }


  return headNode;
}


//
// deleteEditableList : Deletes an editable list
//
//   *node : head of list to delete
//

void deleteEditableList(editableListNode *node)
{
  editableListNode *nextNode;


  while (node)
  {
    nextNode = node->Next;

    deleteStringNodes(node->keywordListHead);
    delete node;

    node = nextNode;
  }
}


//
// deleteEditableinList : Deletes a specific editable in a list that starts
//                        at *editHead, updating whatever needs to be updated
//                        for the list to be valid.
//

void deleteEditableinList(editableListNode **editHead, editableListNode *edit)
{
  editableListNode *prevEdit;
//  char strn[256];


  if ((!*editHead) || !edit)
  {
    _outtext("\ndeleteEditableinList: either editHead or edit is NULL\n");

    return;
  }

//  sprintf(strn, "\ndeleteEditable: editHead is %x, head->next is %x,\n"
//           "node to delete is %x, node->next is %x.\n\n",
//           *editHead, (*editHead)->Next, edit, edit->Next);

//  _outtext(strn);

  if (edit == (*editHead))
  {
    *editHead = (*editHead)->Next;

    deleteStringNodes(edit->keywordListHead);
    delete edit;
  }
  else
  {
    prevEdit = *editHead;

    while (prevEdit)
    {
      if (prevEdit->Next == edit) break;

      prevEdit = prevEdit->Next;
    }

   // couldn't find an edit that points to edit being deleted - error

    if (!prevEdit)
    {
      _outtext("\n\nERROR: deleteEditableinList() - notify author\n\n");

      return;
    }

    prevEdit->Next = edit->Next;

    deleteStringNodes(edit->keywordListHead);
    delete edit;
  }

//  sprintf(strn, "\ndeleteEditable: editHead is now %x, head->next is %x.\n",
//          *editHead, (*editHead)->Next);

//  _outtext(strn);
}
