//
//  File: readqst.cpp    originally part of dikuEdit
//
//  Usage: functions for reading mob quest info from the .QST file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#include "readqst.h"

using namespace std;

extern char readFromSubdirs;


//
// readQuestFileMessage : read a quest message for a mob's quest - returns
//                        pointer to message
//
//   *questFile : file that contains quest info
//    questNumb : mob number quest belongs to - used for error messages
//

questMessage *readQuestFileMessage(FILE *questFile, const ulong questNumb)
{
  questMessage *questMsg;
  char strn[256];


  questMsg = new questMessage;
  if (!questMsg)
  {
    cout << "\n" <<
"Error: couldn't alloc questMsg in readQuestFileMessage().\n" <<
"       aborting..\n";

    exit(1);
  }

  memset(questMsg, 0, sizeof(questMessage));

  if (!fgets(strn, 256, questFile))
  {
    cout << "\n" <<
"Error: hit EOF reading quest message keyword list for mob #" << questNumb << ".\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remLeadingSpaces(strn);
  remTrailingSpaces(strn);

  questMsg->keywordListHead = createKeywordList(strn);
  questMsg->questMessageHead = readStringNodes(questFile, TILDE_LINE, FALSE);

  return questMsg;
}


//
// readQuestFileQuest : read quest info for a mob quest - returns pointer to
//                      info
//
//      *questFile : file reading from
//  *endofMobQuest : set to TRUE if end of entire mob quest has been hit
//        *hitWhat : set if another message or quest info block is hit
//       questNumb : mob number - used for error messages
//

questQuest *readQuestFileQuest(FILE *questFile, char *endofMobQuest,
                               char *hitWhat, const ulong questNumb)
{
  questQuest *questQst;
  questItem *lastRecv = NULL, *lastGive = NULL, *qstItem;
  char strn[256], secondChar[256], dummy[64], done = FALSE;
  long numb;


  *endofMobQuest = FALSE;
  *hitWhat = HIT_NOTHING;

  questQst = new questQuest;
  if (!questQst)
  {
    cout << "\n" <<
"Error: couldn't alloc questQst in readQuestFileQuest().\n" <<
"       aborting..\n";

    exit(1);
  }

  memset(questQst, 0, sizeof(questQuest));

  questQst->questReplyHead = readStringNodes(questFile, TILDE_LINE, FALSE);

 // now, read R, G, and D lines

  while (!done)
  {
    if (!fgets(strn, 256, questFile))
    {
      cout << "\n" <<
"Error: hit EOF reading quest quest info line for mob #" << questNumb << ".\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    strn[0] = toupper(strn[0]);

    switch (strn[0])
    {
     // crap given to player

      case 'R' : if (numbArgs(strn) != 3)
                 {
                   cout << "\n" <<
"Error: R-line for quest info in mob #" << questNumb << "'s quest contains an\n"
"       invalid number of fields.  string read was '" << strn << "'.\n"
"       aborting..\n";

                   exit(1);
                 }

                 sscanf(strn, "%s%s%d", dummy, secondChar, &numb);

                 qstItem = new questItem;
                 memset(qstItem, 0, sizeof(questItem));

                 qstItem->itemVal = numb;
                 if (lastRecv) lastRecv->Next = qstItem;
                 lastRecv = qstItem;

                 if (!questQst->questPlayRecvHead)
                   questQst->questPlayRecvHead = qstItem;

                 upstrn(secondChar);
                 if (strlen(secondChar) != 1)
                 {
                   cout << "\n" <<
"Error: second character after R-line for quest for mob #" << questNumb << " not valid -\n"
"       (entire string read was '" << strn << "').  aborting..\n\n";

                   exit(1);
                 }

                 switch (secondChar[0])
                 {
                   case 'C' : qstItem->itemType = QUEST_RITEM_COINS; break;
                    // coins

                   case 'I' : qstItem->itemType = QUEST_RITEM_OBJ; break;
                    // item

                   case 'S' : qstItem->itemType = QUEST_RITEM_SKILL; break;
                    // skill

                   case 'E' : qstItem->itemType = QUEST_RITEM_EXP; break;
                    // experience

                   case 'G' : qstItem->itemType = QUEST_RITEM_SPELL; break;
                    // experience
 
                   case 'K' : qstItem->itemType = QUEST_RITEM_SPLCST; break;
                    // experience

                   case 'T' : qstItem->itemType = QUEST_RITEM_TRANSFERMOBS; break;
                    // experience

                   case 'M' : qstItem->itemType = QUEST_RITEM_CREATEMOB; break;
                    // experience

                   case 'P' : qstItem->itemType = QUEST_RITEM_CREATEPET; break;
                    // experience

                   case 'R' : qstItem->itemType = QUEST_RITEM_TRANSFERPLR; break;
                    // experience

                   case 'W' : qstItem->itemType = QUEST_RITEM_TRANSFERGRP; break;
                    // experience

                   default : cout << "\n" <<
"Error: Illegal character specified after R-line in .QST file (quest #" << questNumb << ").\n" <<
"       Line read was '" << strn << "'.  Aborting..\n";
                             exit(1);
                 }

                 break;

     // crap player gives to mob
      case 'G' : if (numbArgs(strn) != 3)
                 {
                   cout << "\n" <<
"Error: G-line for quest info in mob #" << questNumb << "'s quest contains an\n"
"       invalid number of fields.  string read was '" << strn << "'.\n"
"       aborting..\n";

                   exit(1);
                 }

                 sscanf(strn, "%s%s%d", dummy, secondChar, &numb);

                 qstItem = new questItem;
                 memset(qstItem, 0, sizeof(questItem));

                 qstItem->itemVal = numb;
                 if (lastGive) lastGive->Next = qstItem;
                 lastGive = qstItem;

                 if (!questQst->questPlayGiveHead)
                   questQst->questPlayGiveHead = qstItem;

                 upstrn(secondChar);
                 if (strlen(secondChar) != 1)
                 {
                   cout << "\n" <<
"Error: second character after R-line for quest for mob #" << questNumb << " not valid -\n"
"       (entire string read was '" << strn << "').  aborting..\n\n";

                   exit(1);
                 }

                 switch (secondChar[0])
                 {
                   case 'I' : qstItem->itemType = QUEST_GITEM_OBJ; break;
                    // item

                   case 'C' : qstItem->itemType = QUEST_GITEM_COINS; break;
                    // coins

                   case 'T' : qstItem->itemType = QUEST_GITEM_OBJTYPE; break;
                    // certain type of object

                   default : cout << "\n" <<
"Error: Illegal character specified after G-line in .QST file (quest #" << questNumb << ").\n" <<
"       Line read was '" << strn << "'.  Aborting..\n";
                             exit(1);
                 }

                 break;

     // message mob spouts when running off (if a quest has two D entries
     // [for whatever reason], new one replaces old one)
      case 'D' : if (questQst->disappearHead)
                   deleteStringNodes(questQst->disappearHead);
                 questQst->disappearHead =
                   readStringNodes(questFile, TILDE_LINE, FALSE);
                 break;

     // hit an M-line
      case 'M' : done = TRUE;  *hitWhat = HIT_MESSAGE;
                 return questQst;

     // hit another Q-line
      case 'Q' : done = TRUE;  *hitWhat = HIT_QUEST;
                 return questQst;

     // end of mob quest info
      case 'S' : done = *endofMobQuest = TRUE;
                 return questQst;

     // unrecognized character
      default : cout << "\n" <<
"error: illegal character specified in quest info line for quest #" << questNumb << " -\n" <<
"       entire string read was '" << strn << "'.  aborting..\n";

                exit(1);
    }
  }

  return questQst;
}


//
// readQuestFile : Reads all the quest info, sticking it where it belongs -
//                 returns TRUE if the file was read successfully, FALSE
//                 otherwise
//
//   *filename : if non-NULL, specifies filename, else value in MAINZONENAME
//               is used
//

char readQuestFile(const char *filename /*= NULL*/)
{
  FILE *questFile;
  quest *questPtr;
  questMessage *questMsg, *questMsgPrev;
  questQuest *questQst, *questQstPrev;
  dikuMob *mob;

  char questFilename[256] = "\0", strn[256], done, endofMobQuest,
       hitWhat = HIT_NOTHING;

  ulong mobNumb;


 // assemble the filename of the quest file

  if (readFromSubdirs) strcpy(questFilename, "qst/");

  if (filename) strcat(questFilename, filename);
  else strcat(questFilename, getMainZoneNameStrn());

  strcat(questFilename, ".qst");

 // open the quest file for reading

  if ((questFile = fopen(questFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << questFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << questFilename << "...\n";

  while (!feof(questFile))
  {
   // first, read the quest's mob number - may hit EOF here

    if (!fgets(strn, 255, questFile)) break;

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);

    if (strn[0] != '#')
    {
      cout << "\n" <<
  "Error - '#' expected at start of quest's mob vnum ref, instead found "
           << "'" << strn[0] << "'\n";

      cout << "\nEntire string read was '" << strn << "'.\n";

      exit(1);
    }

   // remove the durn '#' at the start of the string

    deleteChar(strn, 0);

    if (!strnumer(strn))
    {
      cout << "\n" <<
"Error: Non-numerics found in mob number ref in .QST file (string read was\n" <<
"       '#" << strn << "').  Aborting..\n";

      exit(1);
    }

   // allocate and check that we succeeded..  if you fail on a quest rec,
   // you've got some problems, since they're like 12 bytes

    questPtr = new quest;
    if (!questPtr)
    {
      cout << "\n" <<
"Error allocating memory for quest rec in readQuestFile() - you're so low\n"
"on memory, it's scary.  Aborting..\n";

      exit(1);
    }

    memset(questPtr, 0, sizeof(quest));

    mobNumb = atoi(strn);

    mob = findMob(mobNumb);
    if (!mob)
    {
      cout << "\n" <<
"Error: .QST file specifies a mob vnum that doesn't exist (#" << mobNumb << ")\n" <<
"       Aborting, but only because you've forced me to.\n\n";

      exit(1);
    }

   // check for dupe quest

    if (mob->questPtr)
    {
      cout << "\n" <<
"Error: .QST file specifies a quest for a mob (#" << mobNumb << ") that already has\n"
"       one - aborting..\n";

      exit(1);
    }

    mob->questPtr = questPtr;

   // this while loop reads command by command, one command per iteration

    done = FALSE;
    questMsgPrev = NULL;
    questQstPrev = NULL;

    while (!done)
    {

     // Read a command

      if (hitWhat == HIT_MESSAGE)  // work a little magic
      {
        strn[0] = 'M';
      }
      else
      if (hitWhat == HIT_QUEST)
      {
        strn[0] = 'Q';
      }
      else
      {
        if (!fgets(strn, 256, questFile))
        {
          cout << "\n" <<
"Error - Hit EOF on quest file while reading commands (quest #" << mobNumb << ")\n" <<
"        Aborting..\n";

          exit(1);
        }

        nolf(strn);
        remTrailingSpaces(strn);
        remLeadingSpaces(strn);

        if (strlen(strn) != 1)
        {
          cout << "\n" <<
"Error: line for mob #" << mobNumb << "'s quest invalid - string read was\n" <<
"       '" << strn << "'.  aborting..\n";

          exit(1);
        }
      }

      switch (strn[0])
      {
        case 'M' : questMsg = readQuestFileMessage(questFile, mobNumb);

                   if (questMsgPrev) questMsgPrev->Next = questMsg;
                   questMsgPrev = questMsg;

                   if (!questPtr->messageHead)
                     questPtr->messageHead = questMsg;

                   break;

        case 'Q' : questQst = readQuestFileQuest(questFile, &endofMobQuest,
                                                 &hitWhat, mobNumb);
                   if (questQstPrev) questQstPrev->Next = questQst;
                   questQstPrev = questQst;

                   if (!questPtr->questHead)
                     questPtr->questHead = questQst;

                   if (endofMobQuest) done = TRUE;  // hit S

                   break;

        case 'S' : done = TRUE;
                   break;  // done reading commands for this mob's quest

        default : cout <<
"Unrecognized command in .QST file: string read was '" << strn << "'.\n" <<
"(quest/mob #" << mobNumb << ") - aborting...\n";

                  exit(1);
      }
    }
  }

  fclose(questFile);

  return TRUE;
}
