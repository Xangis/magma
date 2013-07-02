//
//  File: maincomm.cpp   originally part of dikuEdit
//
//  Usage: function called once command and arguments are known -
//         calls appropriate function
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <string.h>

#include "../fh.h"
#include "../types.h"
#include "../vardef.h"

#include "maincomm.h"
#include "../system.h"

extern variable *varHead;
extern dikuRoom *currentRoom;
extern command *cmdHead;
extern alias *aliasHead;

//
// commandQuit
//

void commandQuit(void)
{
  if (confirmChanges())
  {
    _outtext("\n\nQuitting...\n");

    exit(0);
  }
}


//
// mainExecCommand
//

const void mainExecCommand(const uchar command, const char *inargs)
{
  char strn[512], args[512];

  strcpy(args, inargs);

  if ((command != COMM_SETVAR) && (command != COMM_DUMPTEXT))
  {
    upstrn(args);  // it works, dammit
    remTrailingSpaces(args);
  }

  switch (command)
  {
    case COMM_QUIT        : commandQuit(); break;

    case COMM_EXITS       : displayRoomExitInfo(currentRoom); break;
    case COMM_SWAPEXITS   : swapExits(args, currentRoom); break;
    case COMM_SWAPEXITSNS : swapExitsNorthSouth(); break;
    case COMM_SWAPEXITSWE : swapExitsWestEast(); break;
    case COMM_SWAPEXITSUD : swapExitsUpDown(); break;

    case COMM_LOOKUP      : lookup(args); break;
    case COMM_WHICH       : which(args); break;
    case COMM_MASSSET     : massSet(args); break;
    case COMM_WHERE       : where(args); break;
    case COMM_AT          : at(args); break;

    case COMM_NORTH       : goDirection(currentRoom->exits[NORTH], NORTH); break;
    case COMM_SOUTH       : goDirection(currentRoom->exits[SOUTH], SOUTH); break;
    case COMM_WEST        : goDirection(currentRoom->exits[WEST], WEST); break;
    case COMM_EAST        : goDirection(currentRoom->exits[EAST], EAST); break;
    case COMM_UP          : goDirection(currentRoom->exits[UP], UP); break;
    case COMM_DOWN        : goDirection(currentRoom->exits[DOWN], DOWN); break;
    case COMM_NORTHWEST   : goDirection(currentRoom->exits[NORTHWEST], NORTHWEST); break;
    case COMM_SOUTHWEST   : goDirection(currentRoom->exits[SOUTHWEST], SOUTHWEST); break;
    case COMM_NORTHEAST   : goDirection(currentRoom->exits[NORTHEAST], NORTHEAST); break;
    case COMM_SOUTHEAST   : goDirection(currentRoom->exits[SOUTHEAST], SOUTHEAST); break;

    case COMM_GOTO        : gotoRoomStrn(args); break;
    case COMM_LOOK        : look(args); break;

    case COMM_ZONEEXITS   : displayZoneExits(); break;

    case COMM_CREATEEDIT  : createEdit(args); break;
    case COMM_EDIT        : editEntity(args, currentRoom); break;

    case COMM_PURGE       : purge(args); break;

    case COMM_LIST        : list(args); break;
    case COMM_COMMANDS    : listCommands(cmdHead); break;
    case COMM_LOAD        : loadSumpn(args); break;
    case COMM_CREATE      : createSumpn(args); break;
    case COMM_DELETE      : deleteSumpn(args); break;
    case COMM_STAT        : statSumpn(args); break;
    case COMM_CLONE       : cloneSumpn(args); break;
    case COMM_COPY        : copyCommand(args); break;
    case COMM_SETRAND     : setEntityRandomVal(args, currentRoom); break;

    case COMM_KEY         : showKeyUsed(args); break;
    case COMM_GUILD       : toggleGuildVar(); break;
    case COMM_SCREENHEIGHT: if (!varExists(varHead, VAR_SCREENHEIGHT_NAME) &&
                                !args[0])
                            {
                              sprintf(strn,
"\nScreen height var is not set - default is %u.\n\n", DEF_SCREENHEIGHT_VAL);
                              _outtext(strn);
                            }
                            else
                            {
                              sprintf(strn, "%s %s", VAR_SCREENHEIGHT_NAME, args);
                              varCmd(strn, TRUE, &varHead);
                            }
                            break;
    case COMM_SCREENWIDTH : if (!varExists(varHead, VAR_SCREENWIDTH_NAME) &&
                                !args[0])
                            {
                              sprintf(strn,
"\nScreen width var is not set - default is %u.\n\n", DEF_SCREENWIDTH_VAL);
                              _outtext(strn);
                            }
                            else
                            {
                              sprintf(strn, "%s %s", VAR_SCREENWIDTH_NAME, args);
                              varCmd(strn, TRUE, &varHead);
                            }
                            break;

    case COMM_INVENTORY   : showInventory(); break;

    case COMM_GET         : _outtext("\n");
                            takeEntity(args, currentRoom, TRUE);
                            _outtext("\n");
                            break;
    case COMM_GETC        : _outtext("\n");
                            takeEntity(args, currentRoom, FALSE);
                            _outtext("\n");
                            break;

    case COMM_DROP        : _outtext("\n");
                            dropEntity(args, currentRoom, TRUE);
                            _outtext("\n");
                            break;
    case COMM_DROPC       : _outtext("\n");
                            dropEntity(args, currentRoom, FALSE);
                            _outtext("\n");
                            break;

    case COMM_PUT         : _outtext("\n");
                            putEntity(args, currentRoom, TRUE);
                            _outtext("\n");
                            break;
    case COMM_PUTC        : _outtext("\n");
                            putEntity(args, currentRoom, FALSE);
                            _outtext("\n");
                            break;

    case COMM_EQUIP       : equipMob(args, currentRoom, TRUE); break;
    case COMM_UNEQUIP     : unequipMob(args, currentRoom); break;

    case COMM_MOUNT       : mountMob(args, currentRoom); break;
    case COMM_UNMOUNT     : unmountMob(args, currentRoom); break;

    case COMM_FOLLOW      : followMob(args, currentRoom); break;
    case COMM_UNFOLLOW    : unfollowMob(args, currentRoom); break;

    case COMM_CONFIG      : editConfig(); break;
    case COMM_EDITDISPLAY : editDisplay(); break;

    case COMM_SHORTHELP   : dumpTextFile(DE_SHORTHELP, TRUE); break;
    case COMM_LONGHELP    : dumpTextFile(DE_MAINHELP, TRUE); break;

    case COMM_RENUMBER    : renumberAll(args); break;
    case COMM_RENUMBROOM  : renumberRoomsUser(args); break;
    case COMM_RENUMBOBJECT: renumberObjectsUser(args); break;
    case COMM_RENUMBMOB   : renumberMobsUser(args); break;
    case COMM_SETZONENUMB : setZoneNumbStrn(args); break;

    case COMM_FIXFLAGS    : fixAllFlags(); break;
    case COMM_FIXCOND     : resetObjCond(); break;
    case COMM_FIXGUILD    : fixGuildStuff(); break;

    case COMM_RECSIZE     : displayRecordSizeInfo(); break;
    case COMM_MISCINFO    : displayMiscInfo(); break;

    case COMM_CHECK       : checkAll(); break;
    case COMM_EDITCHECK   : editCheck(); break;
    case COMM_SAVE        : writeFiles(); break;

#ifndef __UNIX__
    case COMM_SHELL       : execShell(); break;
    case COMM_FREESTACK   : displayFreeStack(); break;
#endif

    case COMM_DUMPTEXT    : _outtext("\n");
                            dumpTextFile(args, FALSE);
                            _outtext("\n");
                            break;

    case COMM_DUMPTEXTCOL : _outtext("\n");
                            dumpTextFile(args, TRUE);
                            _outtext("\n");
                            break;

    case COMM_VERSION     : displayVersionInfo(); break;

    case COMM_ALIAS       : aliasCmd(args, TRUE, &aliasHead); break;
    case COMM_UNALIAS     : unaliasCmd(args, &aliasHead); break;
    case COMM_SETVAR      : varCmd(args, TRUE, &varHead); break;
    case COMM_UNSETVAR    : unvarCmd(args, &varHead); break;
    case COMM_TOGGLE      : toggleVar(&varHead, args); break;
    case COMM_LIMIT       : setLimitArgs(args); break;

    case COMM_SETTEMPLATE : setTemplateArgs(args, TRUE, TRUE); break;

    case COMM_GRID        : createGridInterp(args); break;
    case COMM_LINKROOMS   : linkRoomsInterp(args); break;
    case COMM_CHECKMAP    : checkMapTrueness(); break;

    default : _outtext(
"\nmainExecCommand() - internal error (no match found for command in list)\n\n");
  }
}
