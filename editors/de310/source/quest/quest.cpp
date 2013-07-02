//
//  File: quest.cpp      originally part of dikuEdit
//
//  Usage: functions used to manipulate mob quest records
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#include "quest.h"

using namespace std;

extern dikuMob **mobLookup, *mobHead;
extern ulong numbLookupEntries;

//
// findQuest
//

INLINEDEF quest *findQuest(ulong numb)
{
  dikuMob *mob = findMob(numb);

  if (!mob) return NULL;

  return mob->questPtr;
}


//
// getLowestQuestMobNumber : making sure that a mob does indeed have a
//                           quest somewhere in the zone should be done
//                           before calling this function
//

INLINEDEF ulong getLowestQuestMobNumber(void)
{
  long i, highest = getHighestMobNumber();

  for (i = getLowestMobNumber(); i <= highest; i++)
  {
    if (findMob(i) && findMob(i)->questPtr) return i;
  }

  return numbLookupEntries;
}


//
// getHighestQuestMobNumber : making sure that a mob does indeed have a
//                            quest somewhere in the zone should be done
//                            before calling this function
//

INLINEDEF ulong getHighestQuestMobNumber(void)
{
  long i, lowest = getLowestMobNumber();

  for (i = getHighestMobNumber(); i >= lowest; i--)
  {
    if (findMob(i) && findMob(i)->questPtr) return i;
  }

  return 0;
}


//
// getPrevQuestMob : find quest mob right before mobNumb, numerically
//

dikuMob *getPrevQuestMob(const ulong mobNumb)
{
  ulong i = mobNumb - 1;

  if (mobNumb <= getLowestQuestMobNumber()) return NULL;

  while (!mobLookup[i] || !mobLookup[i]->questPtr) i--;

  return mobLookup[i];
}


//
// getNextQuestMob : find quest mob right after mobNumb, numerically
//

dikuMob *getNextQuestMob(const ulong mobNumb)
{
  ulong i = mobNumb + 1;

  if (mobNumb >= getHighestQuestMobNumber()) return NULL;

  while (!mobLookup[i] || !mobLookup[i]->questPtr) i++;

  return mobLookup[i];
}


//
// compareQuestMessage
//

char compareQuestMessage(questMessage *msg1, questMessage *msg2)
{
  if (msg1 == msg2) return TRUE;
  if (!msg1 || !msg2) return FALSE;

  if (!compareStringNodes(msg1->keywordListHead, msg2->keywordListHead))
    return FALSE;
  if (!compareStringNodes(msg1->questMessageHead, msg2->questMessageHead))
    return FALSE;

  return TRUE;
}


//
// copyQuestMessage
//

questMessage *copyQuestMessage(questMessage *msg)
{
  questMessage *newMsg;

  if (!msg) return NULL;

  newMsg = new questMessage;
  if (!newMsg)
  {
    cout << "\n\nerror alloc'ing memory in copyQuestMessage() - b'bye\n\n";

    return NULL;
  }

  memcpy(newMsg, msg, sizeof(questMessage));

  newMsg->keywordListHead = copyStringNodes(msg->keywordListHead);
  newMsg->questMessageHead = copyStringNodes(msg->questMessageHead);

  return newMsg;
}


//
// createQuestMessage
//

questMessage *createQuestMessage(void)
{
  questMessage *msg = new questMessage;

  if (!msg)
  {
    cout << "\n\nerror allocating questMessage in createQuestMessage\n\n";
    return NULL;
  }

  memset(msg, 0, sizeof(questMessage));

  msg->keywordListHead = createKeywordList("DEFAULT~");

  return msg;
}


//
// deleteQuestMessage
//

void deleteQuestMessage(questMessage *msg)
{
  if (!msg) return;

  deleteStringNodes(msg->keywordListHead);
  deleteStringNodes(msg->questMessageHead);
  delete msg;
}


//
// deleteQuestMessageList
//

void deleteQuestMessageList(questMessage *msg)
{
  questMessage *next;

  while (msg)
  {
    next = msg->Next;
    deleteQuestMessage(msg);
    msg = next;
  }
}


//
// addQuestMessagetoList
//

void addQuestMessagetoList(questMessage **msgListHead, questMessage *msgToAdd)
{
  questMessage *msg;


  if (!msgToAdd) return;

  if (!*msgListHead) *msgListHead = msgToAdd;
  else
  {
    msg = *msgListHead;

    while (msg)
    {
      if (!msg->Next) break;

      msg = msg->Next;
    }

    if (!msg)
    {
      _outtext("\nError in addQuestMessagetoList() - msg is NULL.\n");
      return;
    }

    msg->Next = msgToAdd;
  }
}


//
// deleteQuestMessageinList
//

void deleteQuestMessageinList(questMessage **msgHead, questMessage *msg)
{
  questMessage *prevMsg;


  if ((!(*msgHead)) || (!msg)) return;

  if (msg == (*msgHead))
  {
    *msgHead = (*msgHead)->Next;

    deleteQuestMessage(msg);
  }
  else
  {
    prevMsg = *msgHead;

    while (prevMsg)
    {
      if (prevMsg->Next == msg) break;

      prevMsg = prevMsg->Next;
    }

   // couldn't find message that points to one to delete - oops

    if (!prevMsg)
    {
      _outtext("\n\nERROR: deleteQuestMessageinList() - notify author\n\n");

      return;
    }

    prevMsg->Next = msg->Next;

    deleteQuestMessage(msg);
  }
}


//
// createQuestItem
//

questItem *createQuestItem(void)
{
  questItem *item;

  item = new questItem;
  if (!item)
  {
    cout << "\n\nerror alloc'ing item in createQuestItem.  aborting.\n\n";
    return NULL;
  }

  memset(item, 0, sizeof(questItem));

  return item;
}


//
// addQuestItemtoList
//

void addQuestItemtoList(questItem **itemListHead, questItem *itemToAdd)
{
  questItem *item;


  if (!itemToAdd) return;

  if (!*itemListHead) *itemListHead = itemToAdd;
  else
  {
    item = *itemListHead;

    while (item)
    {
      if (!item->Next) break;

      item = item->Next;
    }

    if (!item)
    {
      _outtext("\nError in addQuestItemtoList() - msg is NULL.\n");
      return;
    }

    item->Next = itemToAdd;
  }
}


//
// deleteQuestItem
//

void deleteQuestItem(questItem *item)
{
  if (!item) return;

  delete item;  // nobody ever said it had to be hard
}


//
// deleteQuestIteminList
//

void deleteQuestIteminList(questItem **itemHead, questItem *item)
{
  questItem *prevItem;


  if ((!(*itemHead)) || !item) return;

  if (item == (*itemHead))
  {
    *itemHead = (*itemHead)->Next;

    deleteQuestItem(item);
  }
  else
  {
    prevItem = *itemHead;

    while (prevItem)
    {
      if (prevItem->Next == item) break;

      prevItem = prevItem->Next;
    }

   // couldn't find item that points to one to delete - oops

    if (!prevItem)
    {
      _outtext("\n\nERROR: deleteQuestIteminList() - notify author\n\n");

      return;
    }

    prevItem->Next = item->Next;

    deleteQuestItem(item);
  }
}


//
// deleteQuestItemList
//

void deleteQuestItemList(questItem *item)
{
  questItem *old;

  while (item)
  {
    old = item->Next;
    delete item;

    item = old;
  }
}


//
// createQuestQuest
//

questQuest *createQuestQuest(void)
{
  questQuest *qst = new questQuest;

  if (!qst)
  {
    cout << "\n\nerror allocating questQuest in createQuestQuest\n\n";
    return NULL;
  }

  memset(qst, 0, sizeof(questQuest));

  return qst;
}


//
// deleteQuestQuest
//

void deleteQuestQuest(questQuest *qst)
{
  if (!qst) return;

  deleteStringNodes(qst->questReplyHead);
  deleteQuestItemList(qst->questPlayRecvHead);
  deleteQuestItemList(qst->questPlayGiveHead);
  deleteStringNodes(qst->disappearHead);

  delete qst;
}


//
// deleteQuestQuestList
//

void deleteQuestQuestList(questQuest *qst)
{
  questQuest *next;

  while (qst)
  {
    next = qst->Next;
    deleteQuestQuest(qst);
    qst = next;
  }
}


//
// addQuestQuesttoList
//

void addQuestQuesttoList(questQuest **qstListHead, questQuest *qstToAdd)
{
  questQuest *qst;


  if (!qstToAdd) return;

  if (!*qstListHead) *qstListHead = qstToAdd;
  else
  {
    qst = *qstListHead;

    while (qst)
    {
      if (!qst->Next) break;

      qst = qst->Next;
    }

    if (!qst)
    {
      _outtext("\nError in addQuestQuesttoList() - qst is NULL.\n");
      return;
    }

    qst->Next = qstToAdd;
  }
}


//
// deleteQuestQuestinList
//

void deleteQuestQuestinList(questQuest **qstHead, questQuest *qst)
{
  questQuest *prevQst;


  if ((!(*qstHead)) || !qst) return;

  if (qst == (*qstHead))
  {
    *qstHead = (*qstHead)->Next;

    deleteQuestQuest(qst);
  }
  else
  {
    prevQst = *qstHead;

    while (prevQst)
    {
      if (prevQst->Next == qst) break;

      prevQst = prevQst->Next;
    }

   // couldn't find quest that points to one to delete - oops

    if (!prevQst)
    {
      _outtext("\n\nERROR: deleteQuestQuestinList() - notify author\n\n");

      return;
    }

    prevQst->Next = qst->Next;

    deleteQuestQuest(qst);
  }
}


//
// copyQuestMessageList
//

questMessage *copyQuestMessageList(questMessage *headQstMsg)
{
  questMessage *newHead = NULL, *oldMsg = NULL, *msg;


  while (headQstMsg)
  {
    msg = new questMessage;
    if (!msg)
    {
      cout << "\n\nerror alloc'ing memory in copyQuestMessageList() - b'bye\n\n";

      return NULL;
    }

    memset(msg, 0, sizeof(questMessage));

    if (!newHead) newHead = msg;
    if (oldMsg) oldMsg->Next = msg;
    oldMsg = msg;

    msg->keywordListHead = copyStringNodes(headQstMsg->keywordListHead);
    msg->questMessageHead = copyStringNodes(headQstMsg->questMessageHead);


    headQstMsg = headQstMsg->Next;
  }

  return newHead;
}


//
// copyQuestItemList
//

questItem *copyQuestItemList(questItem *headQstItem)
{
  questItem *newHead = NULL, *oldItem = NULL, *item;


  while (headQstItem)
  {
    item = new questItem;
    if (!item)
    {
      cout << "\n\nerror alloc'ing memory in copyQuestItemList() - b'bye\n\n";

      return NULL;
    }

   // easy...  too easy

    memcpy(item, headQstItem, sizeof(questItem));

    if (!newHead) newHead = item;
    if (oldItem) oldItem->Next = item;
    oldItem = item;

    headQstItem = headQstItem->Next;
  }

  return newHead;
}


//
// compareQuestItemList
//

char compareQuestItemList(questItem *item1, questItem *item2)
{
  if (item1 == item2) return TRUE;
  if (!item1 || !item2) return FALSE;

  while (item1 && item2)
  {
    if (item1->itemType != item2->itemType) return FALSE;
    if (item1->itemVal != item2->itemVal) return FALSE;

    item1 = item1->Next;
    item2 = item2->Next;
  }

  if ((!item1 && item2) || (item1 && !item2)) return FALSE;

  return TRUE;
}


//
// compareQuestQuest
//

char compareQuestQuest(questQuest *qst1, questQuest *qst2)
{
  if (qst1 == qst2) return TRUE;
  if (!qst1 || !qst2) return FALSE;

  if (!compareStringNodes(qst1->questReplyHead, qst2->questReplyHead))
    return FALSE;
  if (!compareStringNodes(qst1->disappearHead, qst2->disappearHead))
    return FALSE;
  if (!compareQuestItemList(qst1->questPlayRecvHead, qst2->questPlayRecvHead))
    return FALSE;
  if (!compareQuestItemList(qst1->questPlayGiveHead, qst2->questPlayGiveHead))
    return FALSE;

  return TRUE;
}


//
// copyQuestQuest
//

questQuest *copyQuestQuest(questQuest *qstQst)
{
  questQuest *newQst;


  if (!qstQst) return NULL;

  newQst = new questQuest;
  if (!newQst)
  {
    cout << "\n\nerror alloc'ing memory in copyQuestQuest() - b'bye\n\n";

    return NULL;
  }

  memcpy(newQst, qstQst, sizeof(questQuest));

  newQst->questReplyHead = copyStringNodes(qstQst->questReplyHead);
  newQst->questPlayRecvHead = copyQuestItemList(qstQst->questPlayRecvHead);
  newQst->questPlayGiveHead = copyQuestItemList(qstQst->questPlayGiveHead);
  newQst->disappearHead = copyStringNodes(qstQst->disappearHead);


  return newQst;
}


//
// copyQuestQuestList
//

questQuest *copyQuestQuestList(questQuest *headQstQst)
{
  questQuest *newHead = NULL, *oldQst = NULL, *qst;


  while (headQstQst)
  {
    qst = new questQuest;
    if (!qst)
    {
      cout << "\n\nerror alloc'ing memory in copyQuestQuestList() - b'bye\n\n";

      return NULL;
    }

    memset(qst, 0, sizeof(questQuest));

    if (!newHead) newHead = qst;
    if (oldQst) oldQst->Next = qst;
    oldQst = qst;

    qst->questReplyHead = copyStringNodes(headQstQst->questReplyHead);
    qst->questPlayRecvHead = copyQuestItemList(headQstQst->questPlayRecvHead);
    qst->questPlayGiveHead = copyQuestItemList(headQstQst->questPlayGiveHead);
    qst->disappearHead = copyStringNodes(headQstQst->disappearHead);


    headQstQst = headQstQst->Next;
  }

  return newHead;
}


//
// copyQuest
//

quest *copyQuest(quest *srcQst)
{
  quest *newQst;


  if (!srcQst) return NULL;

  newQst = new quest;
  if (!newQst)
  {
    cout << "\n\ncopyQuest(): error allocating memory.  aborting..\n\n";

    return NULL;
  }

  memcpy(newQst, srcQst, sizeof(quest));

  newQst->messageHead = copyQuestMessageList(srcQst->messageHead);
  newQst->questHead = copyQuestQuestList(srcQst->questHead);

  return newQst;
}


//
// createQuest
//

quest *createQuest(void)
{
  quest *qst = new quest;

  if (!qst)
  {
    cout << "\n\nerror alloc'ing quest node in createQuest(): aborting\n\n";
    return NULL;
  }

  memset(qst, 0, sizeof(quest));

  return qst;
}


//
// deleteQuest
//

void deleteQuest(quest *qst)
{
  if (!qst) return;

  deleteQuestMessageList(qst->messageHead);
  deleteQuestQuestList(qst->questHead);
  delete qst;
}


//
// deleteQuestAssocLists
//

void deleteQuestAssocLists(quest *qst)
{
  if (!qst) return;

  deleteQuestMessageList(qst->messageHead);
  deleteQuestQuestList(qst->questHead);
}


//
// compareQuestMessageLists
//

char compareQuestMessageLists(questMessage *msg1, questMessage *msg2)
{
  if (msg1 == msg2) return TRUE;
  if (!msg1 || !msg2) return FALSE;

  while (msg1 && msg2)
  {
    if (!compareStringNodes(msg1->keywordListHead, msg2->keywordListHead))
      return FALSE;
    if (!compareStringNodes(msg1->questMessageHead, msg2->questMessageHead))
      return FALSE;

    msg1 = msg1->Next;
    msg2 = msg2->Next;
  }

  if ((!msg1 && msg2) || (msg1 && !msg2)) return FALSE;

  return TRUE;
}


//
// compareQuestQuestLists
//

char compareQuestQuestLists(questQuest *qst1, questQuest *qst2)
{
  if (qst1 == qst2) return TRUE;
  if (!qst1 || !qst2) return FALSE;

  while (qst1 && qst2)
  {
    if (!compareStringNodes(qst1->questReplyHead, qst2->questReplyHead))
      return FALSE;
    if (!compareStringNodes(qst1->disappearHead, qst2->disappearHead))
      return FALSE;
    if (!compareQuestItemList(qst1->questPlayRecvHead, qst2->questPlayRecvHead))
      return FALSE;
    if (!compareQuestItemList(qst1->questPlayGiveHead, qst2->questPlayGiveHead))
      return FALSE;

    qst1 = qst1->Next;
    qst2 = qst2->Next;
  }

  if ((!qst1 && qst2) || (qst1 && !qst2)) return FALSE;

  return TRUE;
}


//
// compareQuestInfo
//

char compareQuestInfo(quest *qst1, quest *qst2)
{
  if (qst1 == qst2) return TRUE;
  if (!qst1 || !qst2) return FALSE;


//  if (qst1->mobNumb != qst2->mobNumb) return FALSE;

  if (!compareQuestMessageLists(qst1->messageHead, qst2->messageHead))
    return FALSE;

  if (!compareQuestQuestLists(qst1->questHead, qst2->questHead))
    return FALSE;


  return TRUE;
}


//
// fixQuestRecvString : Trims *oldName to MAX_BLAHSHORT_LEN, setting *newName
//                      with the new string and returning newName.
//
//   *oldName : old name
//   *newName : new name
//

INLINEDEF char *fixQuestRecvString(const char *oldName, char *newName)
{
  return fixString(oldName, newName, MAX_QUESTRECVSTRING_LEN, !getShowColorVal());
}


//
// getQuestRecvString : returns a readable string of what all player gets
//                      from mob's quest
//

char *getQuestRecvString(questItem *item, char *strn)
{
  dikuObject *obj;
  dikuRoom *room;
  dikuMob *mob;
  char strn2[256], strn3[256];


  if (!item)
  {
    strcpy(strn, "(nothing)");
    return strn;
  }

  strn[0] = '\0';

  while (item)
  {
    switch (item->itemType)
    {
      case QUEST_RITEM_OBJ:
        obj = findObj(item->itemVal);

        if (!obj) sprintf(strn2, "obj #%d, ", item->itemVal);
        else
        {
          strcpy(strn3, obj->objShortName);
          sprintf(strn2, "%s&n, ", strn3);
        }

        strcat(strn, strn2);

        break;

      case QUEST_RITEM_TRANSFERMOBS:
      case QUEST_RITEM_TRANSFERPLR:
      case QUEST_RITEM_TRANSFERGRP:
        room = findRoom(item->itemVal);

        if (!room) sprintf(strn2, "room #%d, ", item->itemVal);
        else
        {
          strcpy(strn3, room->roomName);
          sprintf(strn2, "%s&n, ", strn3);
        }

        strcat(strn, strn2);

        break;

      case QUEST_RITEM_CREATEMOB:
      case QUEST_RITEM_CREATEPET:

        mob = findMob(item->itemVal);

        if (!mob) sprintf(strn2, "mob #%d, ", item->itemVal);
        else
        {
          strcpy(strn3, mob->mobShortName);
          sprintf(strn2, "%s&n, ", strn3);
        }

        strcat(strn, strn2);

        break;

      case QUEST_RITEM_COINS:
        sprintf(strn2, "%d copper, ", item->itemVal);
        strcat(strn, strn2);
        break;

      case QUEST_RITEM_SKILL:
        sprintf(strn2, "skill #%d, ", item->itemVal);
        strcat(strn, strn2);
        break;

      case QUEST_RITEM_EXP:
        sprintf(strn2, "%d exp, ", item->itemVal);
        strcat(strn, strn2);
        break;

      case QUEST_RITEM_SPELL:
        sprintf(strn2, "spell %d, ", item->itemVal);
        strcat(strn, strn2);
        break;

      case QUEST_RITEM_SPLCST:
        sprintf(strn2, "spellcast %d, ", item->itemVal);
        strcat(strn, strn2);
        break;

      default :
        sprintf(strn2, "error - type #%d, ", item->itemType);
        strcat(strn, strn2);
        break;
    }

    item = item->Next;
  }

 // get rid of comma and space

  if (strlen(strn))
  {
    deleteChar(strn, strlen(strn) - 1);
    deleteChar(strn, strlen(strn) - 1);
  }

  fixQuestRecvString(strn, strn2);
  strcpy(strn, strn2);

  return strn;
}


//
// getNumbMessageNodes
//

ulong getNumbMessageNodes(questMessage *messageHead)
{
  ulong numb = 0;

  while (messageHead)
  {
    numb++;
    messageHead = messageHead->Next;
  }

  return numb;
}


//
// getNumbQuestNodes
//

ulong getNumbQuestNodes(questQuest *questHead)
{
  ulong numb = 0;

  while (questHead)
  {
    numb++;
    questHead = questHead->Next;
  }

  return numb;
}


//
// getNumbItemNodes
//

ulong getNumbItemNodes(questItem *itemHead)
{
  ulong numb = 0;

  while (itemHead)
  {
    numb++;
    itemHead = itemHead->Next;
  }

  return numb;
}


//
// getMessageNodeNumb : number 0 is first node
//

questMessage *getMessageNodeNumb(const ulong numb, questMessage *msgHead)
{
  ulong i = 0;

  while (msgHead)
  {
    if (i == numb) return msgHead;

    msgHead = msgHead->Next;
    i++;
  }

  return NULL;
}


//
// getQuestNodeNumb : number 0 is first node
//

questQuest *getQuestNodeNumb(const ulong numb, questQuest *qstHead)
{
  ulong i = 0;

  while (qstHead)
  {
    if (i == numb) return qstHead;

    qstHead = qstHead->Next;
    i++;
  }

  return NULL;
}


//
// getItemNodeNumb : number 0 is first node
//

questItem *getItemNodeNumb(const ulong numb, questItem *itemHead)
{
  ulong i = 0;

  while (itemHead)
  {
    if (i == numb) return itemHead;

    itemHead = itemHead->Next;
    i++;
  }

  return NULL;
}


//
// scanQuestItemListforItem
//

#if 0
questItem *scanMobQuestforItem(const char *input, quest *mobQuest,
                               const char checkGive, const char checkRecv)
{
  int vnum = -1;
  questItem *qitem;
  questQuest *qquest;


  if (!input || !mobQuest || (!checkGive && !checkRecv)) return NULL;

  if (strnumer(input)) vnum = atoi(input);

  qquest = mobQuest->questHead;

  if (checkGive)
  {
    while (qquest)
    {
    }

    qitem = mobQuest->
  }
}
#endif


//
// getQuestItemStrn
//

char *getQuestItemStrn(const questItem *item, const char itemList,
                       char *strn)
{
  dikuObject *obj;
  dikuMob *mob;
  dikuRoom *room;
  char newName[256];


  strcpy(strn, "\0");
  if (!item) return "\0";

  if (itemList == QUEST_GIVEITEM)  // player gives mob
  {
    switch (item->itemType)
    {
      case QUEST_GITEM_OBJ     :
        obj = findObj(item->itemVal);

        if (!obj) sprintf(strn, "obj #%d", item->itemVal);
        else
        {
          fixObjName(obj->objShortName, newName);
          sprintf(strn, "\"%s&n\" (#%d)", newName, item->itemVal);
        }

        break;

      case QUEST_GITEM_COINS   :
        sprintf(strn, "%d copper", item->itemVal);
        break;

      case QUEST_GITEM_OBJTYPE :
        sprintf(strn, "obj type #%d (%s)", item->itemVal,
                getObjTypeStrn(item->itemVal));
        break;

      default                  :
        sprintf(strn, "err in type - %d", item->itemType);
        break;
    }
  }
  else
  if (itemList == QUEST_RECVITEM)  // player receives from mob
  {
    switch (item->itemType)
    {
      case QUEST_RITEM_OBJ     :
        obj = findObj(item->itemVal);

        if (!obj) sprintf(strn, "obj #%d", item->itemVal);
        else
        {
          fixObjName(obj->objShortName, newName);
          sprintf(strn, "\"%s\"&n (#%d)", newName, item->itemVal);
        }

        break;

      case QUEST_RITEM_COINS   :
        sprintf(strn, "%d copper", item->itemVal);
        break;

      case QUEST_RITEM_SKILL   :
        sprintf(strn, "skill #%d", item->itemVal);
        break;

      case QUEST_RITEM_EXP     :
        sprintf(strn, "%d exp", item->itemVal);
        break;

      case QUEST_RITEM_SPELL:
        sprintf(strn, "spell %d, ", item->itemVal);
        break;

      case QUEST_RITEM_SPLCST:
        sprintf(strn, "spellcast %d, ", item->itemVal);
        break;

      case QUEST_RITEM_TRANSFERMOBS:
        room = findRoom(item->itemVal);

        if (!room) sprintf(strn, "trans mobs from room #%d", item->itemVal);
        else
        {
          fixRoomName(room->roomName, newName);
          sprintf(strn, "Trans mobs: \"%s\"&n (#%d)", newName, item->itemVal);
        }

        break;

      case QUEST_RITEM_CREATEMOB:
        mob = findMob(item->itemVal);

        if (!mob) sprintf(strn, "Create mob #%d", item->itemVal);
        else
        {
          fixMobName(mob->mobShortName, newName);
          sprintf(strn, "Create mob: \"%s\"&n (#%d)", newName, item->itemVal);
        }

        break;

      case QUEST_RITEM_CREATEPET:
        mob = findMob(item->itemVal);

        if (!mob) sprintf(strn, "Create pet #%d", item->itemVal);
        else
        {
          fixMobName(mob->mobShortName, newName);
          sprintf(strn, "Create pet: \"%s\"&n (#%d)", newName, item->itemVal);
        }

        break;

      case QUEST_RITEM_TRANSFERPLR:
        room = findRoom(item->itemVal);

        if (!room) sprintf(strn, "trans player to room #%d", item->itemVal);
        else
        {
          fixRoomName(room->roomName, newName);
          sprintf(strn, "Trans player: \"%s\"&n (#%d)", newName, item->itemVal);
        }
        break;

      case QUEST_RITEM_TRANSFERGRP:
        room = findRoom(item->itemVal);

        if (!room) sprintf(strn, "trans group to room #%d", item->itemVal);
        else
        {
          fixRoomName(room->roomName, newName);
          sprintf(strn, "Trans group: \"%s\"&n (#%d)", newName, item->itemVal);
        }

        break;

      default:
        sprintf(strn, "err in type - %d", item->itemType);
        break;
    }
  }
  else                             // error
  {
    sprintf(strn, "err in itemList - %d", itemList);
  }

  return strn;
}


//
// getNumbQuestMobs
//

ulong getNumbQuestMobs(void)
{
  dikuMob *mob = mobHead;
  ulong numbQuests = 0,
        numb = getLowestMobNumber(), high = getHighestMobNumber();

  while (numb <= high)
  {
    mob = findMob(numb);
    if (mob && mob->questPtr) numbQuests++;

    numb++;
  }

  return numbQuests;
}


//
// checkForMobWithQuest
//

char checkForMobWithQuest(void)
{
  dikuMob *mob = mobHead;
  ulong numb = getLowestMobNumber(), high = getHighestMobNumber();

  while (numb <= high)
  {
    mob = findMob(numb);
    if (mob && mob->questPtr) return TRUE;

    numb++;
  }

  return FALSE;
}


//
// displayQuestList : Displays a list of mob types starting at mobHead
//                    that have quests
//

void displayQuestList(void)
{
  dikuMob *mob = mobHead;
  ulong i = 0;
  char strn[256];


  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");

    return;
  }

  if (!checkForMobWithQuest())
  {
    _outtext("\nNone of the current mob types have a quest.\n\n");
    return;
  }

  _outtext("\n\n");

  while (mob)
  {
    if (mob->questPtr)
    {
      sprintf(strn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);
      displayColorString(strn);

      i += numbLinesStringNeeds(strn);
    }

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      i = 0;
    }

    mob = mob->Next;
  }

  _outtext("\n");
}


//
// displayQuestList : Displays the list of mob types loaded into
//                    game with quests, starting at mobHead
//

void displayQuestList(const char *args)
{
  dikuMob *mob = mobHead;
  ulong i = 0;
  char outStrn[512], searchStrn[512], foundMob = FALSE;


  strcpy(searchStrn, args);

  remLeadingSpaces(searchStrn);
  upstrn(searchStrn);  // just in case..  (interp.c should do it, but hey)

  if (strlen(searchStrn) == 0)
  {
    displayQuestList();
    return;
  }

  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");
    return;
  }

  if (!checkForMobWithQuest())
  {
    _outtext("\nNone of the current mob types have a quest.\n\n");
    return;
  }

  _outtext("\n\n");

  while (mob)
  {
    if (mob->questPtr && (scanKeyword(searchStrn, mob->keywordListHead)))
    {
      sprintf(outStrn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);

      foundMob = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

      if (i >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return;
        }

        _outtext("\n\n");

        i = 0;
      }
    }

    mob = mob->Next;
  }

  if (!foundMob) _outtext("No matching mob types found.\n");
  _outtext("\n");
}
