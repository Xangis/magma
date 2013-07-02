// MASTER.H - header file for MASTER.C - stuff related to the master keyword
//            list

#ifndef _MASTER_H_

#include "../misc/strnnode.h"

#define NO_MATCH      69   // used when no match is found

#define ENTITY_LOWEST  0
#define ENTITY_OBJ_ED  0   // obj. extra desc
#define ENTITY_OBJECT  1   // object
#define ENTITY_R_EDESC 2   // room extra desc - previously ENTITY_ROOM
#define ENTITY_MOB     3   // mob
#define ENTITY_EXIT    4   // exit - used in inventory
#define ENTITY_HIGHEST 4

typedef struct _masterKeywordListNode
{
  stringNode *keywordListHead;  // keywords for this entity

  void *entityPtr;  // pointer to the entity
  char entityType;  // type of entity

  stringNode *entityDesc;  // description

  struct _masterKeywordListNode *Next;  // pointer to next node
} masterKeywordListNode;

#define _MASTER_H_
#endif
