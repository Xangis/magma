//
//  File: writeqst.cpp   originally part of dikuEdit
//
//  Usage: functions used for writing quest info to the .QST file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <iostream>

#include "../types.h"
#include "../fh.h"

#include "quest.h"

using namespace std;

extern char readFromSubdirs;
extern dikuMob *mobHead;

//
// writeQuesttoFile : Writes a quest to a file
//

void writeQuesttoFile(FILE *questFile, quest *qst, const long mobNumb)
{
  questMessage *msgNode;
  questQuest *qstNode;
  questItem *qstItem;
  char strn[256];


  if (!qst || !questFile)
  {
    _outtext("\nerror: writeQuesttoFile() - NULL pointer passed\n\n");
    return;
  }

  fprintf(questFile, "#%u\n", mobNumb);

 // next, run through M and Q formats

  msgNode = qst->messageHead;
  qstNode = qst->questHead;

  while (msgNode)
  {
    fputs("M\n", questFile);
    fprintf(questFile, "%s\n", createKeywordString(msgNode->keywordListHead, strn));
    writeStringNodes(questFile, msgNode->questMessageHead);
    fputs("~\n", questFile);

    msgNode = msgNode->Next;
  }

  while (qstNode)
  {
    fputs("Q\n", questFile);
    writeStringNodes(questFile, qstNode->questReplyHead);
    fputs("~\n", questFile);

    qstItem = qstNode->questPlayRecvHead;
    while (qstItem)
    {
      switch (qstItem->itemType)
      {
        case QUEST_RITEM_OBJ : fprintf(questFile, "R I %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_COINS : fprintf(questFile, "R C %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_SKILL : fprintf(questFile, "R S %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_EXP : fprintf(questFile, "R E %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_SPELL: fprintf(questFile, "R G %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_SPLCST: fprintf(questFile, "R K %d\n", qstItem->itemVal); break;
        case QUEST_RITEM_TRANSFERMOBS: fprintf(questFile, "R T %d\n", qstItem->itemVal);break;
        case QUEST_RITEM_CREATEMOB: fprintf(questFile, "R M %d\n", qstItem->itemVal);break;
        case QUEST_RITEM_CREATEPET: fprintf(questFile, "R P %d\n", qstItem->itemVal);break;
        case QUEST_RITEM_TRANSFERPLR: fprintf(questFile, "R R %d\n", qstItem->itemVal);break;
        case QUEST_RITEM_TRANSFERGRP: fprintf(questFile, "R W %d\n", qstItem->itemVal);break;

        default : break;  // error, but hell, who cares
      }

      qstItem = qstItem->Next;
    }

    qstItem = qstNode->questPlayGiveHead;
    while (qstItem)
    {
      switch (qstItem->itemType)
      {
        case QUEST_GITEM_OBJ : fprintf(questFile, "G I %d\n", qstItem->itemVal); break;
        case QUEST_GITEM_COINS : fprintf(questFile, "G C %d\n", qstItem->itemVal); break;
        case QUEST_GITEM_OBJTYPE : fprintf(questFile, "G T %d\n", qstItem->itemVal); break;

        default : break;  // error - invalid type
      }

      qstItem = qstItem->Next;
    }

    if (qstNode->disappearHead)
    {
      fputs("D\n", questFile);
      writeStringNodes(questFile, qstNode->disappearHead);
      fputs("~\n", questFile);
    }

    qstNode = qstNode->Next;
  }

 // terminate the quest info

  fputs("S\n", questFile);
}


//
// writeQuestFile : Write the quest file
//

void writeQuestFile(const char *filename /*= NULL*/)
{
  FILE *questFile;
  char questFilename[81] = "\0";
  char strn[256];
  dikuMob *mob = mobHead;

  ulong lowest = getLowestMobNumber(), highest = getHighestMobNumber(),
        numb = lowest, i;


 // assemble the filename of the quest file

  if (readFromSubdirs) strcpy(questFilename, "qst/");

  if (filename) strcat(questFilename, filename);
  else strcat(questFilename, getMainZoneNameStrn());

  strcat(questFilename, ".qst");

 // open the world file for writing

  if ((questFile = fopen(questFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << questFilename << " for writing - aborting\n";

    return;
  }

  i = getNumbQuestMobs();

  sprintf(strn, "Writing %s - %u quest%s\n", questFilename, i, plural(i));

  _outtext(strn);

  while (numb <= highest)
  {
    mob = findMob(numb);

    if (mob && mob->questPtr)
      writeQuesttoFile(questFile, mob->questPtr, mob->mobNumber);

    numb++;
  }

  fclose(questFile);
}
