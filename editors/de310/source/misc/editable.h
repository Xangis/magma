// EDITABLE.H - stuff

#ifndef _EDITABLE_H_

#include "strnnode.h"

//#define NO_MATCH      -1   // used when no match is found

#define EDITABLE_LOWEST  0
#define EDITABLE_OBJ     0   // object itself (not a description)
#define EDITABLE_R_EDESC 1   // room extra desc
#define EDITABLE_MOB     2   // mob
#define EDITABLE_OBJDESC 3   // object description
#define EDITABLE_HIGHEST 3

typedef struct _editableListNode
{
  stringNode *keywordListHead;  // keywords for this entity

  void *entityPtr;  // pointer to the entity
  void *entityOwner;  // pointer to entity "owner" - used for extra descs
  char entityType;  // type of entity

  struct _editableListNode *Next;  // pointer to next node
} editableListNode;


#define _EDITABLE_H_
#endif
