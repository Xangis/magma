//
//  File: qsttypes.cpp   originally part of dikuEdit
//
//  Usage: functions that return the significane of various bits of
//         quest errata
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../fh.h"
#include "../types.h"

#include "quest.h"

//
// checkForQuestItemHelp : checks to see if this quest item type has a help
//                         list of some kind available
//

char checkForQuestItemHelp(const char itemType, const char giveRecv)
{
  if (giveRecv == QUEST_GIVEITEM)
  {
    switch (itemType)
    {
      case QUEST_GITEM_OBJ     : return TRUE;
      case QUEST_GITEM_OBJTYPE : return TRUE;

      default : return FALSE;
    }
  }
  else
  {
    switch (itemType)
    {
      case QUEST_RITEM_OBJ    : return TRUE;
//      case QUEST_RITEM_SKILL  : return FALSE;  // someday..
      case QUEST_RITEM_TRANSFERMOBS : return TRUE;
      case QUEST_RITEM_CREATEMOB: return TRUE;
      case QUEST_RITEM_CREATEPET: return TRUE;
      case QUEST_RITEM_TRANSFERPLR: return TRUE;
      case QUEST_RITEM_TRANSFERGRP: return TRUE;

      default : return FALSE;
    }
  }
}


//
// displayQuestItemHelp : displays "help" on quest item
//

void displayQuestItemHelp(const char itemType, const char giveRecv)
{
  if (giveRecv == QUEST_GIVEITEM)
  {
    switch (itemType)
    {
      case QUEST_GITEM_OBJ     : displayObjectTypeList();  return;
      case QUEST_GITEM_OBJTYPE : displayObjTypeList();  return;

      default : return;
    }
  }
  else
  {
    switch (itemType)
    {
      case QUEST_RITEM_OBJ    : displayObjectTypeList();  return;
//      case QUEST_RITEM_SKILL  : return;  // someday..
      case QUEST_RITEM_TRANSFERMOBS: displayRoomList(); return;
      case QUEST_RITEM_CREATEMOB:    displayMobTypeList(); return;
      case QUEST_RITEM_CREATEPET:    displayMobTypeList(); return;
      case QUEST_RITEM_TRANSFERPLR:  displayRoomList(); return;
      case QUEST_RITEM_TRANSFERGRP:  displayRoomList(); return;


      default : return;
    }
  }
}


//
// getQuestItemTypeStrn
//

const char *getQuestItemTypeStrn(const char itemType, const char giveRecv)
{
  if (giveRecv == QUEST_GIVEITEM)
  {
    switch (itemType)
    {
      case QUEST_GITEM_OBJ     : return "object";
      case QUEST_GITEM_COINS   : return "copper";
      case QUEST_GITEM_OBJTYPE : return "obj type";

      default : return "unrecognized";
    }
  }
  else
  {
    switch (itemType)
    {
      case QUEST_RITEM_OBJ    : return "object";
      case QUEST_RITEM_COINS  : return "coins";
      case QUEST_RITEM_SKILL  : return "skill";
      case QUEST_RITEM_EXP    : return "experience";
      case QUEST_RITEM_SPELL  : return "spell";
      case QUEST_RITEM_SPLCST : return "cast spell";
      case QUEST_RITEM_TRANSFERMOBS : return "transfer mobs from room";
      case QUEST_RITEM_CREATEMOB : return "create mob";
      case QUEST_RITEM_CREATEPET : return "create pet";
      case QUEST_RITEM_TRANSFERPLR : return "transfer player";
      case QUEST_RITEM_TRANSFERGRP : return "transfer group";

      default : return "unrecognized";
    }
  }
}


//
// displayQuestItemTypeList
//

void displayQuestItemTypeList(const char giveRecv)
{
  ulong i;
  char strn[256];

  _setbkcolor(0);

  _outtext("\n\n");

  if (giveRecv == QUEST_GIVEITEM)
  {
    for (i = QUEST_GITEM_LOWEST; i < QUEST_GITEM_HIGHEST; i++)
    {
      sprintf(strn, "  %u - %s\n", i, getQuestItemTypeStrn(i, giveRecv));

      displayColorString(strn);
    }
  }
  else
  {
    for (i = QUEST_RITEM_LOWEST; i < QUEST_RITEM_HIGHEST; i++)
    {
      sprintf(strn, "  %u - %s\n", i, getQuestItemTypeStrn(i, giveRecv));

      displayColorString(strn);
    }
  }

  displayColorString("\n&+CPress any key to continue..&n");
  getkey();

  _outtext("\n\n");
}
