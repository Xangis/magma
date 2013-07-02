//
//  File: master.cpp     originally part of dikuEdit
//
//  Usage: functions for manipulating masterKeywordListNode records and
//         lists
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "../fh.h"
#include "master.h"
#include "../edesc/edesc.h"
#include "../obj/objhere.h"
#include "../mob/mobhere.h"
#include "../room/room.h"

using namespace std;

//
// createMasterKeywordList : Creates a "master keyword list" for the specified
//                           room node, returning a pointer to the head of the
//                           list.
//
//   *room : dikuRoom to create master keyword list for
//

masterKeywordListNode *createMasterKeywordList(dikuRoom *room)
{
  masterKeywordListNode *listHead = NULL, *listNode, *oldListNode = NULL;
  extraDesc *extraDescNode;
  objectHere *objHere;
  mobHere *mobHNode;


 // first, grab all of the keyword lists for extra descs in the room

  extraDescNode = room->extraDescHead;

  while (extraDescNode)
  {
    listNode = new masterKeywordListNode;

    if (!listNode)
    {
      cout << "Error creating masterKeywordListNode in "
              "createMasterKeywordList().  Aborting..\n";

      exit(1);
    }

    if (!listHead) listHead = listNode;

    listNode->keywordListHead = copyStringNodes(extraDescNode->keywordListHead);
    listNode->entityPtr = extraDescNode;
    listNode->entityDesc = extraDescNode->extraDescStrnHead;
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

    extraDescNode = objHere->objectPtr->extraDescHead;

   // object has no description

    if (!extraDescNode)
    {
      listNode = new masterKeywordListNode;

      if (!listNode)
      {
        cout << "Error creating masterKeywordListNode in "
                "createMasterKeywordList().  Aborting..\n";

        exit(1);
      }

      if (!listHead) listHead = listNode;

     // copy the object's keywords and set the entity desc pointer to NULL

      listNode->keywordListHead =
           copyStringNodes(objHere->objectPtr->keywordListHead);
      listNode->entityPtr = objHere;
      listNode->entityDesc = NULL;
      listNode->entityType = ENTITY_OBJECT;
      listNode->Next = NULL;

      if (oldListNode) oldListNode->Next = listNode;
      oldListNode = listNode;
    }
    else
    while (extraDescNode)
    {
      listNode = new masterKeywordListNode;

      if (!listNode)
      {
        cout << "Error creating masterKeywordListNode in "
                "createMasterKeywordList().  Aborting..\n";

        exit(1);
      }

      if (!listHead) listHead = listNode;

     // copy the extra desc description

      listNode->keywordListHead =
           copyStringNodes(extraDescNode->keywordListHead);
      listNode->entityPtr = objHere;
      listNode->entityDesc = extraDescNode->extraDescStrnHead;
      listNode->entityType = ENTITY_OBJECT;//_ED;
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

    listNode = new masterKeywordListNode;
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
    listNode->entityPtr = mobHNode;
    listNode->entityDesc = mobHNode->mobPtr->mobDescHead;
    listNode->entityType = ENTITY_MOB;
    listNode->Next = NULL;

    if (oldListNode) oldListNode->Next = listNode;
    oldListNode = listNode;


    mobHNode = mobHNode->Next;
  }


  return listHead;
}


//
// getNumbMasterListNodes : Returns the number of nodes in a specific
//                          masterKeywordListNode list
//
//    *head : take a wild guess, bucko
//

/*
ulong getNumbMasterListNodes(masterKeywordListNode *head)
{
  ulong i = 0;


  while (head)
  {
    i++;

    head = head->Next;
  }

  return i;
}
*/


//
// checkMasterKeywordList : Scans through the master keyword list passed to
//                          the function for the keyword specified by strn and
//                          strnPos.  whatMatched is set to the entity type
//                          that matched and a pointer to the entity
//                          description is returned.  returns a pointer to
//                          the description of entity matched (if any)
//
//          *strn : string as entered by user
//        strnPos : position of keyword
//   *keywordNode : head of masterKeywordListNode list
//   *whatMatched : set based on what type of entity matched
// **matchingNode : upon a match, the matching node's address is put in this
//                  puppy
//

stringNode *checkMasterKeywordList(const char *strn,
                                   masterKeywordListNode *keywordNode,
                                   char *whatMatched,
                                   masterKeywordListNode **matchingNode)
{
  char entityToGrab = 1, entityGrabbing = 1, strn2[256];


  *whatMatched = NO_MATCH;
  *matchingNode = NULL;
  strcpy(strn2, strn);

  if (!strn[0]) return NULL;

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

//  _outtext("\nstrn2 = ");
//  _outtext(strn2);
//  _outtext("\n\n");

  while (keywordNode)
  {
    if (scanKeyword(strn2, keywordNode->keywordListHead))
    {
      if ((keywordNode->entityType >= ENTITY_LOWEST) &&
          (keywordNode->entityType <= ENTITY_HIGHEST))
      {
        if (entityToGrab == entityGrabbing)
        {
          *whatMatched = keywordNode->entityType;

          *matchingNode = keywordNode;

          return (keywordNode->entityDesc);
        }
        else entityGrabbing++;
      }
      else
      {
        cout << "Error: keywordNode->entityType in checkMasterKeywordList()"
                " is invalid (" << keywordNode->entityType << ").  Aborting."
                "\n";

        exit(1);
      }
    }

    keywordNode = keywordNode->Next;
  }

  return NULL;
}


//
// copyMasterKeywordList : Copies a master keyword list, returning the address
//                         of the head of the new list
//
//    *srcNode : list to copy from
//

/*
masterKeywordListNode *copyMasterKeywordList(masterKeywordListNode *srcNode)
{
  masterKeywordListNode *newNode, *prevNode = NULL, *headNode = NULL;


  if (!srcNode) return NULL;


  while (srcNode)
  {
    newNode = new masterKeywordListNode;
    if (!newNode)
    {
      cout << "\n\nERROR: copyMasterKeywordList() - ran out of mem alloc'ing "
           << "new masterKeywordListNode node.  Aborting..\n\n";

      return NULL;
    }

    memset(newNode, 0, sizeof(masterKeywordListNode));
//    newNode->Next = NULL;

    if (!headNode) headNode = newNode;

    if (prevNode) prevNode->Next = newNode;
    prevNode = newNode;


   // first, the easy stuff

    memcpy(newNode, srcNode, sizeof(masterKeywordListNode));

   // now, the not quite as easy but still relatively simple stuff

    newNode->keywordListHead = copyStringNodes(srcNode->keywordListHead);
    newNode->entityDesc = copyStringNodes(srcNode->entityDesc);


    srcNode = srcNode->Next;
  }


  return headNode;
}
*/


//
// deleteMasterKeywordList : Deletes a master keyword list
//
//   *node : head of list to delete
//

void deleteMasterKeywordList(masterKeywordListNode *node)
{
  masterKeywordListNode *nextNode;


  while (node)
  {
    nextNode = node->Next;

    deleteStringNodes(node->keywordListHead);
//    deleteStringNodes(node->entityDesc);  // whoopsie - not right now anyway

    delete node;

    node = nextNode;
  }
}
