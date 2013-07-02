//
//  File: misccomm.cpp   originally part of dikuEdit
//
//  Usage: command dispatchers for all non-main command lists
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "../room/room.h"

#include "misccomm.h"

extern dikuRoom *currentRoom, *defaultRoom;
extern roomExit *defaultExit;
extern dikuObject *defaultObject;
extern dikuMob *defaultMob;
extern dikuZone zoneRec;
extern command *copyDescCmdHead, *copyCmdHead, *copyDefaultCmdHead;
extern char madeChanges;
extern _CONST char *exitnames[];

//
// lookupExecCommand
//

const void lookupExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case LOOKUPCMD_ROOM   : displayRoomList(args); break;
    case LOOKUPCMD_OBJECT : displayObjectTypeList(args, FALSE); break;
    case LOOKUPCMD_MOB    : displayMobTypeList(args); break;

    default : _outtext(
"\nlookupExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// createExecCommand
//

const void createExecCommand(const uchar command, const char *args)
{
  switch (command)
  {

   // allow these funcs to accept vnum args

    case CREATECMD_ROOM   : createRoomPrompt(args); break;
    case CREATECMD_OBJECT : createObjectTypeUser(args); break;
    case CREATECMD_MOB    : createMobTypeUser(args); break;
    case CREATECMD_EXIT   : preCreateExit(args, currentRoom);
                            /*_outtext("\n\n");*/ break;

    default : _outtext(
"\ncreateExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// loadExecCommand
//

const void loadExecCommand(const uchar command, const char *args)
{
  switch (command)
  {

   // allow these funcs to accept vnum args

    case LOADCMD_OBJECT : createObjectHereStrn(args); break;
    case LOADCMD_MOB    : createMobHereStrn(args); break;

    default : _outtext(
"\nloadExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// editExecCommand
//

const void editExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case EDITCMD_ROOM   : preEditRoom(args); break;
    case EDITCMD_OBJECT : editObjectTypeStrn(args); break;
    case EDITCMD_MOB    : editMobTypeStrn(args); break;
    case EDITCMD_QUEST  : editQuestStrn(args); break;
    case EDITCMD_SHOP   : editShopStrn(args); break;
    case EDITCMD_EXIT   : preEditExit(args, currentRoom); break;
    case EDITCMD_ZONE   : editZone(&zoneRec); break;
    case EDITCMD_DEFROOM: editDefaultRoom(); break;
    case EDITCMD_DEFOBJ : editDefaultObject(); break;
    case EDITCMD_DEFMOB : editDefaultMob(); break;
    case EDITCMD_DEFEXIT: editDefaultExit(); break;
    case EDITCMD_DESC   : editRoomDesc(currentRoom, TRUE);
                          displayRoom(currentRoom); break;

    default : _outtext(
"\neditExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// listExecCommand
//

const void listExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case LISTCMD_ROOM    : displayRoomList(args); break;
    case LISTCMD_EXIT    : displayExitList(args); break;
    case LISTCMD_OBJECT  : displayObjectTypeList(args, FALSE); break;
    case LISTCMD_MOB     : displayMobTypeList(args); break;
    case LISTCMD_QUEST   : displayQuestList(args); break;
    case LISTCMD_SHOP    : displayShopList(args); break;
    case LISTCMD_OBJHERE : displayObjectHereList(); break;
    case LISTCMD_MOBHERE : displayMobHereList(); break;
    case LISTCMD_LOADED  : displayLoadedList(args); break;
    case LISTCMD_LOOKUP  : preDisplayLookupList(args); break;

    default : _outtext(
"\nlistExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// createEditExecCommand
//

const void createEditExecCommand(const uchar command, const char *args)
{
  dikuObject *obj;
  dikuMob *mob;
  char strn[256];
  long numb;


  switch (command)
  {
    case CRTEDITCMD_ROOM   : if (strlen(args))
                             {
                               if (createRoomPrompt(args)) preEditRoom(args);
                             }
                             else
                             {
                               itoa(getFirstFreeRoomNumber(), strn, 10);
                               if (createRoomPrompt(strn)) preEditRoom(strn);
                             }

                             break;

    case CRTEDITCMD_OBJECT : numb = createObjectTypeUser(args);
                             if ((numb >= 0) && (obj = findObj(numb)))
                               editObjType(obj, TRUE);
                             break;

    case CRTEDITCMD_MOB    : numb = createMobTypeUser(args);
                             if ((numb >= 0) && (mob = findMob(numb)))
                               editMobType(mob, TRUE);
                             break;

    case CRTEDITCMD_EXIT   : createEditExit(args); break;

    default : _outtext(
"\ncreateEditExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// deleteExecCommand
//

const void deleteExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case DELETECMD_ROOM      : deleteRoomUser(args); break;
    case DELETECMD_EXIT      : preDeleteExitPrompt(args, currentRoom); break;
    case DELETECMD_OBJECT    : deleteObjectTypeUser(args); break;
    case DELETECMD_MOB       : deleteMobTypeUser(args); break;
    case DELETECMD_QUEST     : deleteQuestUser(args); break;
    case DELETECMD_SHOP      : deleteShopUser(args); break;
    case DELETECMD_UNUSED    : deleteUnusedObjMobTypes(); break;

    case DELETECMD_OBJHERE   : deleteObjectHereUser(args, currentRoom);
                               break;
    case DELETECMD_MOBHERE   : deleteMobHereUser(args, currentRoom);
                               break;

    case DELETECMD_DEFROOM   : deleteDefaultRoom(); break;
    case DELETECMD_DEFOBJECT : deleteDefaultObject(); break;
    case DELETECMD_DEFMOB    : deleteDefaultMob(); break;
    case DELETECMD_DEFEXIT   : deleteDefaultExit(); break;

    default : _outtext(
"\ndeleteExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// statExecCommand
//

const void statExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case STATCMD_ROOM   : displayRoomInfo(args); break;
    case STATCMD_OBJECT : displayObjectInfo(args, currentRoom); break;
    case STATCMD_MOB    : displayMobInfo(args, currentRoom); break;
    case STATCMD_ZONE   : displayZoneInfo(); break;

    default : _outtext(
"\nstatExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// setLimitExecCommandStartup
//

const void setLimitExecCommandStartup(const uchar command, const char *args)
{
  switch (command)
  {
    case SETLIMCMD_OBJECT : setLimitOverrideObj(args, FALSE); break;
    case SETLIMCMD_MOB    : setLimitOverrideMob(args, FALSE); break;

    default : _outtext(
"\nsetLimitExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// setLimitExecCommand
//

const void setLimitExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
    case SETLIMCMD_OBJECT : setLimitOverrideObj(args, TRUE); break;
    case SETLIMCMD_MOB    : setLimitOverrideMob(args, TRUE); break;

    default : _outtext(
"\nsetLimitExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// cloneExecCommand
//

const void cloneExecCommand(const uchar command, const char *args)
{
  long vnum, toclone = -1;
  char arg1[128], arg2[128];


  if (!strlen(args))
  {
    _outtext("\n"
"Specify a vnum and optional clone amount as the second and third arguments.\n\n");
    return;
  }

  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (arg2[0] == '\0')  // no second arg
  {
    if (!strnumer(arg1))
    {
      _outtext("\nThe second argument should specify the vnum to clone.\n\n");
      return;
    }

    toclone = 1;
  }
  else
  {
    if (!strnumer(arg1) || !strnumer(arg2))
    {
      _outtext("\n"
"The second and third arguments should specify the vnum and amount to clone.\n\n");
      return;
    }
  }

  vnum = atoi(arg1);
  if (toclone == -1) toclone = atoi(arg2);

  switch (command)
  {
    case CLONECMD_ROOM   : cloneRoom(vnum, toclone); break;
    case CLONECMD_OBJECT : cloneObjectType(vnum, toclone); break;
    case CLONECMD_MOB    : cloneMobType(vnum, toclone); break;

    default : _outtext(
"\ncloneExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// copyDescExecCommand
//

const void copyDescExecCommand(const uchar command, const char *args)
{
  ulong tocopy, copyto;
  char arg1[128], arg2[128];


  if (!strlen(args))
  {
    _outtext("\n"
"Specify a vnum to be copied and a vnum to copy to (optional for rooms) as the\n"
"third and fourth arguments.\n\n");
    return;
  }

  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (arg2[0] == '\0')  // no second arg
  {
    if (command != COPYDESCCMD_ROOM)
    {
      _outtext("\nWhen copying mob descs, a target must be specified.\n\n");
      return;
    }

    itoa(currentRoom->roomNumber, arg2, 10);
  }

  if ((!strnumer(arg1)) || (!strnumer(arg2)))
  {
    _outtext("\n"
"The third and fourth arguments should specify the vnum of the desc to be copied\n"
"and the vnum of the entity to copy the desc to, respectively.\n\n");

    return;
  }

  tocopy = atoi(arg1);
  copyto = atoi(arg2);

  switch (command)
  {
    case COPYDESCCMD_ROOM   : copyRoomDesc(tocopy, copyto); break;
    case COPYDESCCMD_MOB    : copyMobDesc(tocopy, copyto); break;

    default : _outtext(
"\ncopyDescExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// copyDefaultExecCommand
//

const void copyDefaultExecCommand(const uchar command, const char *args)
{
  char strn[256], outstrn[256];
  ulong i;
  long tocopy = -1, val;
  dikuRoom *room;
  dikuObject *obj;
  dikuMob *mob;


  if (!strlen(args) && (command == COPYDEFCMD_ROOM))
  {
    tocopy = currentRoom->roomNumber;
  }
  else
  if (!strlen(args) || (!strnumer(args) && (command != COPYDEFCMD_EXIT)))
  {
    _outtext("\n"
"Specify <room|object|mob|exit> as the second arg and a vnum to be copied\n"
"(optional for rooms, exit direction for exits) to the default as the third\n"
"argument.\n\n");

    return;
  }

  if (tocopy == -1) tocopy = atoi(args);

  switch (command)
  {
    case COPYDEFCMD_ROOM   :
      room = findRoom(tocopy);

      if (!room)
      {
        _outtext("\n"
"Room with vnum specified to copy not found.\n\n");
        break;
      }

      if (defaultRoom)
      {
        deleteRoomInfo(defaultRoom, FALSE, FALSE, TRUE);
      }

      defaultRoom = copyRoomInfo(room, FALSE, TRUE);
      defaultRoom->Next = NULL;

      for (i = 0; i < NUMB_EXITS; i++)
      {
        deleteRoomExit(defaultRoom->exits[i], FALSE);
        defaultRoom->exits[i] = NULL;
      }

      deleteObjHereList(defaultRoom->objectHead, FALSE);
      defaultRoom->objectHead = NULL;

      deleteMobHereList(defaultRoom->mobHead, FALSE);
      defaultRoom->mobHead = NULL;

      deleteMasterKeywordList(defaultRoom->masterListHead);
      defaultRoom->masterListHead = NULL;

      deleteEditableList(defaultRoom->editableListHead);
      defaultRoom->editableListHead = NULL;

//      deleteRoomAssocLists(defaultRoom);  // gotta keep edescs

      defaultRoom->defaultRoom = TRUE;
      defaultRoom->roomNumber = 0;

      sprintf(strn, "\n"
"Settings for room #%u copied into default room.\n\n",
              tocopy);
      _outtext(strn);

      madeChanges = TRUE;

      break;

    case COPYDEFCMD_EXIT :
      val = getDirfromKeyword(args);
      if (val == NO_EXIT)
      {
        _outtext("\nCopy which exit?\n\n");
        return;
      }

      if (currentRoom->exits[val])
      {
        deleteRoomExit(defaultExit, FALSE);
        defaultExit = copyRoomExit(currentRoom->exits[val], FALSE);

        strcpy(strn, exitnames[val]);
        strn[0] = toupper(strn[0]);

        sprintf(outstrn, "\n%s exit of current room copied into default exit.\n\n",
                strn);
        _outtext(outstrn);

        madeChanges = TRUE;

        break;
      }
      else
      {
        _outtext("\nNo exit in that direction to copy.\n\n");
        break;
      }

    case COPYDEFCMD_OBJECT :
      obj = findObj(tocopy);

      if (!obj)
      {
        _outtext("\n"
"Object type with vnum specified to copy not found.\n\n");
        break;
      }

      if (defaultObject)
      {
        deleteObjectType(defaultObject, FALSE);
      }

      defaultObject = copyObjectType(obj, FALSE);
      defaultObject->Next = NULL;

      defaultObject->defaultObj = TRUE;
      defaultObject->objNumber = 0;

      sprintf(strn, "\n"
"Settings for object #%u copied into default object.\n\n",
              tocopy);
      _outtext(strn);

      madeChanges = TRUE;

      break;

    case COPYDEFCMD_MOB    :
      mob = findMob(tocopy);

      if (!mob)
      {
        _outtext("\n"
"Mob type with vnum specified to copy not found.\n\n");
        break;
      }

      if (defaultMob)
      {
        deleteMobType(defaultMob, FALSE);
      }

      defaultMob = copyMobType(mob, FALSE);
      defaultMob->Next = NULL;

      defaultMob->defaultMob = TRUE;
      defaultMob->mobNumber = 0;

      sprintf(strn, "\n"
"Settings for mob #%u copied into default mob.\n\n",
              tocopy);
      _outtext(strn);

      madeChanges = TRUE;

      break;

    default : _outtext(
"\ncopyDefaultExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// copyExecCommand
//

const void copyExecCommand(const uchar command, const char *args)
{
  switch (command)
  {
   // allow these funcs to accept vnum args

    case COPYCMD_DEFAULT :
       checkCommands(args, copyDefaultCmdHead, "\n"
"Specify one of ROOM, OBJECT, MOB, or EXIT as the second argument.\n\n",
                     copyDefaultExecCommand, FALSE); break;

    case COPYCMD_DESC    :
       checkCommands(args, copyDescCmdHead,
                     "\nSpecify one of ROOM or MOB as the second argument.\n\n",
                     copyDescExecCommand, FALSE); break;

    default : _outtext(
"\ncopyExecCommand() - internal error (no match found for command in list)\n\n");
  }
}


//
// copyCommand
//

void copyCommand(const char *args)
{
  checkCommands(args, copyCmdHead, "\n"
"Specify one of DEFAULT or DESC as the first argument.\n\n",
                copyExecCommand, FALSE);
}


//
//
//

/*
void setTempExecCommand(uchar command, char *args)
{
  switch (command)
  {
    case SETLIMCMD_OBJECT : setLimitOverrideObj(args); break;
    case SETLIMCMD_MOB    : setLimitOverrideMob(args); break;

    default : _outtext(
"\nsetLimitExecCommand() - internal error (no match found for command in list)\n\n");
  }
}
*/
