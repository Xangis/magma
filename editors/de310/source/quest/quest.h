#ifndef _QUEST_H_

#include "../misc/strnnode.h"

typedef struct _questMessage   // user types keyword, mob spits out stuff
{                              // (M-format)
  stringNode *keywordListHead;
  stringNode *questMessageHead;

  struct _questMessage *Next;
} questMessage;

// constants for item types players give to mobs (G-lines)

#define QUEST_GITEM_LOWEST   0
#define QUEST_GITEM_OBJ      0  // specific obj vnum
#define QUEST_GITEM_COINS    1  // coins
#define QUEST_GITEM_OBJTYPE  2  // certain object type
#define QUEST_GITEM_HIGHEST  3

// constants for items/stuff player gets from quest (R-lines)

#define QUEST_RITEM_LOWEST       0
#define QUEST_RITEM_OBJ          0 // specific obj vnum
#define QUEST_RITEM_COINS        1 // coins
#define QUEST_RITEM_SKILL        2 // gives skill
#define QUEST_RITEM_EXP          3 // gives experience
#define QUEST_RITEM_SPELL        4 // gives spell
#define QUEST_RITEM_SPLCST       5 // casts spell on player
#define QUEST_RITEM_TRANSFERMOBS 6 // Transfers mobs from a room
#define QUEST_RITEM_CREATEMOB    7 // Creates a mob
#define QUEST_RITEM_CREATEPET    8 // Creates a mob as player's pet
#define QUEST_RITEM_TRANSFERPLR  9 // Transfers player to a room
#define QUEST_RITEM_TRANSFERGRP 10 // Transfers player's group to a room
#define QUEST_RITEM_HIGHEST     11

#define QUEST_GIVEITEM       0  // handy constants used here and there
#define QUEST_RECVITEM       1

typedef struct _questItem
{
  char itemType;  // item type
  long itemVal;   // item vnum/value

  struct _questItem *Next;
} questItem;

typedef struct _questQuest   // user gives mob obj, mob does something -
{                            // a "true" quest (Q-format)
  stringNode *questReplyHead;   // reply to user
  questItem *questPlayRecvHead; // items player gets for quest (R-format)
  questItem *questPlayGiveHead; // items player gives for quest (G-format)

  stringNode *disappearHead;    // text when disappearing, if any (D-format)

  struct _questQuest *Next;
} questQuest;


#define MAX_QUESTRECVSTRING_LEN  61  // quest receive strings are shortened to
                                     // this

typedef struct _quest
{
//  ulong mobNumb;  // may not be needed, but it's handy

  questMessage *messageHead;  // (M-format)
  questQuest *questHead;      // (Q-format)
} quest;

#define _QUEST_H_
#endif
