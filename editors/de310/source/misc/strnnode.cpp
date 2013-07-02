//
//  File: strnnode.cpp   originally part of dikuEdit
//
//  Usage: functions for manipulating stringNode structures/lists
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#ifdef __UNIX__
#  include <unistd.h>
#else
#  include <io.h>
#endif

#include "../fh.h"
#include "../system.h"
#include "strnnode.h"    // header for string node functions

using namespace std;

//
// displayStringNodesNoColor : Displays a list of string nodes without color
//                             interpretation
//                             (currently not used)
//
//  *rootNode : root stringNode
//

/*
void displayStringNodesNoColor(stringNode *rootNode)
{
  while (rootNode)
  {
    _outtext(rootNode->string);
    _outtext("\n");

    rootNode = rootNode->Next;
  }
}
*/


//
// getNumbStringNodes : returns number of nodes in list
//

ulong getNumbStringNodes(stringNode *strnHead)
{
  ulong i = 0;

  while (strnHead)
  {
    i++;
    strnHead = strnHead->Next;
  }

  return i;
}


//
// fixStringNodes : Removes trailing spaces and deletes blank nodes at end
//                  of list
//

void fixStringNodes(stringNode **strnHead, const char remStartLines)
{
  stringNode *strnNode = *strnHead, *oldNode;


 // get to end of list and delete trailing spaces in one fell swoop

  while (strnNode)
  {
    remTrailingSpaces(strnNode->string);

    oldNode = strnNode;
    strnNode = strnNode->Next;
  }

  strnNode = oldNode;

 // check for blank lines, from the end to the start

 // first, get rid of blank lines at end of desc

  while (strnNode && *strnHead)
  {
    if (!strlen(strnNode->string))
    {
      if (strnNode->Last) strnNode->Last->Next = NULL;

//      if (strnNode == *strnHead) *strnHead = NULL;
//      else *strnHead = NULL; // must be first in list

      oldNode = strnNode->Last;

      if (strnNode == *strnHead) *strnHead = NULL;

      delete strnNode;

      strnNode = oldNode;
    }
    else break;
  }

 // then, get rid of blank lines at start of desc

  if (remStartLines)
  {
    while (*strnHead)
    {
      if (!strlen((*strnHead)->string))
      {
        strnNode = *strnHead;

        *strnHead = (*strnHead)->Next;
        (*strnHead)->Last = NULL;

        delete strnNode;
      }
      else break;
    }
  }
}


//
// readStringNodes : Reads strings from inFile, creating a list of stringNodes
//                   until an end-point determined by EOSN is hit.  The head
//                   of the list is returned to the calling function.
//
//  *inFile : file to read strings from
//     EOSN : "end of string nodes" - determines when to stop
//

stringNode *readStringNodes(FILE *inFile, const char EOSN,
                            const char remStartLines)
{
  stringNode *root = NULL, *node, *oldnode = NULL;
  char strn[256];


  while (TRUE)
  {
    if ((!fgets(strn, MAX_STRNNODE_LEN, inFile)) && (EOSN != ENDOFFILE))
    {
      cout << "\n\n"
"Warning: Hit EOF while reading file in readStringNodes().\n" <<
"         Bad things may begin happening.  You have been warned.\n\n" <<
"         Press a key...\n";

      getkey();

      fixStringNodes(&root, remStartLines);

//      exit(1);
      return root;
    }

    nolf(strn);

    if ((EOSN == TILDE_LINE) && !strcmp(strn, "~"))
    {
      fixStringNodes(&root, remStartLines);

      return root;
    }
    else if((EOSN == VERT_LINE) && !strcmp(strn, "|"))
    {
      fixStringNodes(&root, remStartLines);

      return root;
    }
    else
    if ((EOSN == ENDOFFILE) && feof(inFile))
    {
      fixStringNodes(&root, remStartLines);

      return root;
    }
/*
    else
    {
      cout << "\n\nreadStringNodes(): called with an invalid EOSN value ("
           << EOSN << ").  This is an internal error.\n\n";

      return NULL;
    }
*/

    node = new stringNode;
    if (!node)
    {
      cout << "\n\nRan out of memory allocating a new stringNode in"
              " readStringNodes.  Aborting..\n\n";

      exit(1);
    }

    node->Next = NULL;
    strcpy(node->string, strn);

    if (!root) root = node;

    node->Last = oldnode;
    if (oldnode) oldnode->Next = node;
    oldnode = node;
  }
}


//
// writeStringNodes : Writes strings to outFile from a list of stringNodes.
//
//    *outFile : file to write strings to
//   *strnNode : head of string node list to write
//

void writeStringNodes(FILE *outFile, stringNode *strnNode)
{
  while (strnNode)
  {
    fputs(strnNode->string, outFile);
    fputs("\n", outFile);

    strnNode = strnNode->Next;
  }
}


//
// writeStringNodesFixed : "Fixes" a list of string nodes by trimming
//                         trailing spaces and deleting lines that consist
//                         of nothing - the originals are modified.
//

/*
void writeStringNodesFixed(FILE *outFile, stringNode **strnHead)
{
  fixStringNodes(strnHead);

  writeStringNodes(outFile, *strnHead);
}
*/


//
// deleteStringNodes : Deletes a list of string nodes
//
//   *strnNode : head of list to delete
//

void deleteStringNodes(stringNode *strnNode)
{
  stringNode *nextNode;


  while (strnNode)
  {
    nextNode = strnNode->Next;

    delete strnNode;

    strnNode = nextNode;
  }
}


//
// copyStringNodes : Reads strings from the list pointed to by strnNode,
//                   "copying" them into a new list.  The pointer to the new
//                   list is returned.
//
//  *strnNode : head string node to copy
//

stringNode *copyStringNodes(stringNode *strnNode)
{
  stringNode *node, *oldNode = NULL, *headNode = NULL;


  while (strnNode)
  {
    node = new stringNode;
    if (!node)
    {
      cout << "Error: Error allocating new stringNode in copyStringNodes()."
              "  Aborting..\n";

      exit(1);
    }

    if (!headNode) headNode = node;

    memcpy(node, strnNode, sizeof(stringNode));
    node->Next = NULL;

    strnNode->Last = oldNode;
    if (oldNode) oldNode->Next = node;
    oldNode = node;

    strnNode = strnNode->Next;
  }

  return headNode;
}


//
// compareStringNodes : Compares two lists of string nodes - returns TRUE if
//                      they match
//
//   *list1 : first list to compare against
//   *list2 : second list to compare against
//

char compareStringNodes(stringNode *list1, stringNode *list2)
{
  if (list1 == list2) return TRUE;
  if (!list1 || !list2) return FALSE;

  while (list1 && list2)
  {
    if (strcmp(list1->string, list2->string)) return FALSE;

    list1 = list1->Next;
    list2 = list2->Next;
  }

  if ((!list1 && list2) || (list1 && !list2)) return FALSE;

  return TRUE;
}


//
// editStringNodes : Saves a list of string nodes to a temporary file, calls a
//                   user-defined editor to edit em, then rereads the file.
//                   The function returns a pointer to the new list.
//
//   *strnNode : head of list to edit
//

stringNode *editStringNodes(stringNode *strnNode, const char remStartLines)
{
  stringNode *strnNode2;
  FILE *tmpFile;
  char strn[256];


 // open the tmp file

  if ((tmpFile = fopen(TMPFILE_NAME, "wt")) == NULL)
  {
    _outtext("\n"
"Error: couldn't open " TMPFILE_NAME " for writing.  Aborting..\n\n");

    return NULL;
  }

 // write the current desc and close the file

  writeStringNodes(tmpFile, strnNode);

  fclose(tmpFile);

 // execute the editor

//  system(TEXT_EDITOR " " TMPFILE_NAME);
//  strcpy(strn, textEditor);
/*
  strcpy(strn, getEditorName());
  strcat(strn, " ");
  strcat(strn, TMPFILE_NAME);
*/
  sprintf(strn, "%s %s", getEditorName(), TMPFILE_NAME);

  if (system(strn))
  {
    _outtext("\nCouldn't load command shell to execute editor\n\n");
    return strnNode;
  }

  _clearscreen(7, 0);

  switch (errno)
  {
    case 0       : break;  // no error
    case E2BIG   : _outtext("\nArg list too big (E2BIG)\n\n"); break;
    case EACCES  : _outtext("\nAccess violation (EACCES)\n\n"); break;
    case EMFILE  : _outtext("\nNo file handles avail (EMFILE)\n\n"); break;
    case ENOENT  : _outtext("\nPath specified not found (ENOENT)\n\n"); break;
    case ENOMEM  : _outtext("\nNot enough memory to exec (ENOMEM)\n\n"); break;

    default : sprintf(strn, "\nGot error #%d trying to execute editor\n\n",
                      errno);
              _outtext(strn);
  }

 // read the new description

  if ((tmpFile = fopen(TMPFILE_NAME, "rt")) == NULL)
  {
    _outtext("\n"
"Error: couldn't reopen " TMPFILE_NAME " for reading.  Aborting..\n\n");
    getkey();

    return NULL;
  }

  strnNode2 = readStringNodes(tmpFile, ENDOFFILE, remStartLines);

 // close the file and delete it

  fclose(tmpFile);

 // if unlink fails, try remove since it's fun

//  if (unlink(TMPFILE_NAME))
    remove(TMPFILE_NAME);

 // remove trailing spaces and blank lines

  fixStringNodes(&strnNode2, remStartLines);

 // compare the new and the old

//  _outtext("\n\n");
//  _clearscreen(7, 0);

 // the two lists match - delete the new and return the pointer to the old list

  if (compareStringNodes(strnNode, strnNode2))
  {
    deleteStringNodes(strnNode2);

    return strnNode;
  }
  else  // two lists don't match - delete the old and return the new
  {
    deleteStringNodes(strnNode);

    return strnNode2;
  }
}
