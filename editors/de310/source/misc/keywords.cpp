//
//  File: keywords.cpp   originally part of dikuEdit
//
//  Usage: functions used to handle keyword lists
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"

//
// createKeywordList : Taking keywordStrn as input, breaks the individual
//                     words in the string (separated by spaces) into
//                     "keywords."  Each keyword goes into its own stringNode,
//                     and a linked list is created.  The head of this linked
//                     list is returned to the calling function.
//
//   *keywordStrn : Pointer to the keyword string as read from the data file
//

stringNode *createKeywordList(const char *keywordStrn)
{
  ulong i, i2 = 0, len = strlen(keywordStrn);
  stringNode *homeSNode, *SNode, *oldSNode = NULL;


  if (keywordStrn[0] == '~') return NULL;

  homeSNode = SNode = new stringNode;
  if (!homeSNode)
  {
    _outtext("\ncreateKeywordList: couldn't allocate memory for homeSNode\n\n");

    return NULL;
  }

  homeSNode->string[0] = '\0';
  homeSNode->Next = NULL;

  for (i = 0; i < len; i++)
  {
    if (((keywordStrn[i] == ' ') || (keywordStrn[i] == '~')) && (i2 > 0))
    {
     // null-terminate the string

      SNode->string[i2] = '\0';
      i2 = 0;

     // uppercase it..

      upstrn(SNode->string);

      if (oldSNode) oldSNode->Next = SNode;
      oldSNode = SNode;

     // reset SNode to NULL so that a new one is only created if it is needed

      SNode = NULL;
    }
    else
    {
      if (!SNode)
      {
        SNode = new stringNode;

        if (!SNode)
        {
          _outtext("\ncreateKeywordList(): couldn't allocate memory for SNode\n\n");

          return NULL;
        }

        SNode->Next = NULL;
        SNode->string[0] = '\0';
      }

      SNode->string[i2] = keywordStrn[i];

      i2++;
    }
  }

  if (homeSNode->string[0] == '\0')
  {
    delete homeSNode;

    return NULL;
  }
  else return homeSNode;
}


//
// addKeywordtoList : returns FALSE if there's an error, TRUE if success
//

char addKeywordtoList(stringNode **keywordHead, const char *keyword)
{
  stringNode *strnNode, *strnNode2;

  if (!keywordHead) return FALSE;  // might happen

  strnNode = new stringNode;
  if (!strnNode) return FALSE;

  memset(strnNode, 0, sizeof(stringNode));

  strcpy(strnNode->string, keyword);

  if (!*keywordHead) *keywordHead = strnNode;
  else
  {
    strnNode2 = *keywordHead;
    while (strnNode2->Next)
    {
      strnNode2 = strnNode2->Next;
    }

    strnNode2->Next = strnNode;
    strnNode->Last = strnNode2;
  }

  return TRUE;
}


//
// getReadableKeywordStrn :
//                  Creates a user-readable string of keywords from the list
//                  pointed to by keywordHead, with each keyword separated by
//                  a comma.
//
//  *keywordHead : pointer to the head of the stringNode keyword list
//      *endStrn : pointer to string that contains user-readable string
//

char *getReadableKeywordStrn(stringNode *keywordHead, char *endStrn)
{
  endStrn[0] = '\0';

  while (keywordHead)
  {
    strcat(endStrn, keywordHead->string);

    keywordHead = keywordHead->Next;
    if (keywordHead) strcat(endStrn, ", ");
  }

  return endStrn;
}


//
// scanKeyword : Searches through the list pointed to by keywordListHead for
//               a substring constructed using userinput and keywordpos.  If
//               a match is found, TRUE is returned, else FALSE is returned.
//
//          *userinput : Points to the string where the substring to search for
//                       resides
//    *keywordListHead : Pointer to the head of a list of stringNodes
//          keywordpos : Point in userinput to start at in order to make the
//                       substring
//

char scanKeyword(const char *userinput, stringNode *keywordListHead)
{
  char *keyword;


  if (!keywordListHead) return FALSE;

  keyword = new char[strlen(userinput) + 1];
  if (!keyword)
  {
    _outtext("\n\nscanKeyword(): couldn't alloc keyword\n\n");
    return FALSE;
  }

  strcpy(keyword, userinput);

  remLeadingSpaces(keyword);
  remTrailingSpaces(keyword);

  while (keywordListHead)
  {
    if (!strcmp(keyword, keywordListHead->string))
    {
      delete[] keyword;
      return TRUE;
    }

    keywordListHead = keywordListHead->Next;
  }

  delete[] keyword;

  return FALSE;
}


//
// createKeywordString : Creates a standard Diku tilde-terminated keyword
//                       string from a list of stringNodes, altering strn
//                       and returning the address of the new string.
//
//   *strnNode : Head of linked list of stringNodes
//    *keyStrn : String to put new string into
//

char *createKeywordString(stringNode *strnNode, char *keyStrn)
{
  if (!keyStrn)
  {
    _outtext("\n\ncreateKeywordString(): got NULL ptr\n\n");
    return "error in createKeywordString~";
  }

  keyStrn[0] = '\0';

  while (strnNode)
  {
    strcat(keyStrn, strnNode->string);

    strnNode = strnNode->Next;

    if (strnNode) strcat(keyStrn, " ");
  }

  strcat(keyStrn, "~");

  lowstrn(keyStrn);

  return keyStrn;
}
