//
//  File: initcomm.cpp   originally part of dikuEdit
//
//  Usage: multitudinous functions for initializing command lists
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../fh.h"
#include "../types.h"

#include "maincomm.h"
#include "misccomm.h"
#include "setcomm.h"

extern command *cmdHead, *lookupCmdHead, *createCmdHead, *loadCmdHead,
               *editCmdHead, *listCmdHead, *createEditCmdHead, *setCmdHead,
               *deleteCmdHead, *statCmdHead, *setLimitCmdHead, *cloneCmdHead,
               *copyDescCmdHead, *copyDefaultCmdHead, *copyCmdHead;

//
// initMainCommands
//

void initMainCommands(void)
{
  addCommand(&cmdHead, "LOOK", COMM_LOOK);

  addCommand(&cmdHead, "WEST", COMM_WEST);
  addCommand(&cmdHead, "EAST", COMM_EAST);
  addCommand(&cmdHead, "NORTH", COMM_NORTH);
  addCommand(&cmdHead, "SOUTH", COMM_SOUTH);
  addCommand(&cmdHead, "UP", COMM_UP);
  addCommand(&cmdHead, "DOWN", COMM_DOWN);
  addCommand(&cmdHead, "NORTHWEST", COMM_NORTHWEST);
  addCommand(&cmdHead, "SOUTHWEST", COMM_SOUTHWEST);
  addCommand(&cmdHead, "NORTHEAST", COMM_NORTHEAST);
  addCommand(&cmdHead, "SOUTHEAST", COMM_SOUTHEAST);
  addCommand(&cmdHead, "NW", COMM_NORTHWEST);
  addCommand(&cmdHead, "SW", COMM_SOUTHWEST);
  addCommand(&cmdHead, "NE", COMM_NORTHEAST);
  addCommand(&cmdHead, "SE", COMM_SOUTHEAST);

  addCommand(&cmdHead, "ALIAS", COMM_ALIAS);
  addCommand(&cmdHead, "UNALIAS", COMM_UNALIAS);
  addCommand(&cmdHead, "SET", COMM_SETVAR);
  addCommand(&cmdHead, "UNSET", COMM_UNSETVAR);
  addCommand(&cmdHead, "SETTEMPLATE", COMM_SETTEMPLATE);
  addCommand(&cmdHead, "TOGGLE", COMM_TOGGLE);
  addCommand(&cmdHead, "LIMIT", COMM_LIMIT);

  addCommand(&cmdHead, "STAT", COMM_STAT);
  addCommand(&cmdHead, "EXITS", COMM_EXITS);
  addCommand(&cmdHead, "LOOKUP", COMM_LOOKUP);
  addCommand(&cmdHead, "WHERE", COMM_WHERE);
  addCommand(&cmdHead, "MASSSET", COMM_MASSSET);
  addCommand(&cmdHead, "WHICH", COMM_WHICH);
  addCommand(&cmdHead, "GOTO", COMM_GOTO);
  addCommand(&cmdHead, "AT", COMM_AT);
  addCommand(&cmdHead, "EDIT", COMM_EDIT);
  addCommand(&cmdHead, "LIST", COMM_LIST);
  addCommand(&cmdHead, "CREATE", COMM_CREATE);
  addCommand(&cmdHead, "CLONE", COMM_CLONE);
  addCommand(&cmdHead, "COPY", COMM_COPY);
  addCommand(&cmdHead, "COMMANDS", COMM_COMMANDS);

  addCommand(&cmdHead, "ZONEEXITS", COMM_ZONEEXITS);

  addCommand(&cmdHead, "CREATEEDIT", COMM_CREATEEDIT);
  addCommand(&cmdHead, "CE", COMM_CREATEEDIT);

  addCommand(&cmdHead, "LOAD", COMM_LOAD);

  addCommand(&cmdHead, "PURGE", COMM_PURGE);
  addCommand(&cmdHead, "DELETE", COMM_DELETE);

  addCommand(&cmdHead, "RANDOM", COMM_SETRAND);
  addCommand(&cmdHead, "SETRANDOM", COMM_SETRAND);

  addCommand(&cmdHead, "KEY", COMM_KEY);
  addCommand(&cmdHead, "HEIGHT", COMM_SCREENHEIGHT);
  addCommand(&cmdHead, "WIDTH", COMM_SCREENWIDTH);

  addCommand(&cmdHead, "INVENTORY", COMM_INVENTORY);

  addCommand(&cmdHead, "GET", COMM_GET);
  addCommand(&cmdHead, "GETC", COMM_GETC);

  addCommand(&cmdHead, "DROP", COMM_DROP);
  addCommand(&cmdHead, "DROPC", COMM_DROPC);

  addCommand(&cmdHead, "PUT", COMM_PUT);
  addCommand(&cmdHead, "PUTC", COMM_PUTC);

  addCommand(&cmdHead, "GIVE", COMM_PUT);
  addCommand(&cmdHead, "GIVEC", COMM_PUTC);

  addCommand(&cmdHead, "EQUIP", COMM_EQUIP);
  addCommand(&cmdHead, "UNEQUIP", COMM_UNEQUIP);

  addCommand(&cmdHead, "MOUNT", COMM_MOUNT);
  addCommand(&cmdHead, "UNMOUNT", COMM_UNMOUNT);

  addCommand(&cmdHead, "FOLLOW", COMM_FOLLOW);
  addCommand(&cmdHead, "UNFOLLOW", COMM_UNFOLLOW);

  addCommand(&cmdHead, "SAVE", COMM_SAVE);
  addCommand(&cmdHead, "WRITE", COMM_SAVE);

  addCommand(&cmdHead, "QUIT", COMM_QUIT);
  addCommand(&cmdHead, "EXIT", COMM_QUIT);

#ifndef __UNIX__
  addCommand(&cmdHead, "SHELL", COMM_SHELL);
  addCommand(&cmdHead, "FREESTACK", COMM_FREESTACK);
#endif
  addCommand(&cmdHead, "DUMPTEXT", COMM_DUMPTEXT);
  addCommand(&cmdHead, "DUMPTEXTCOLOR", COMM_DUMPTEXTCOL);
  addCommand(&cmdHead, "DTC", COMM_DUMPTEXTCOL);

  addCommand(&cmdHead, "CONFIG", COMM_CONFIG);
  addCommand(&cmdHead, "SETUP", COMM_CONFIG);
  addCommand(&cmdHead, "DISPLAYCONFIG", COMM_EDITDISPLAY);
  addCommand(&cmdHead, "CHECKCONFIG", COMM_EDITCHECK);

  addCommand(&cmdHead, "SWAPEXITS", COMM_SWAPEXITS);
  addCommand(&cmdHead, "SWAPNS", COMM_SWAPEXITSNS);
  addCommand(&cmdHead, "SWAPWE", COMM_SWAPEXITSWE);
  addCommand(&cmdHead, "SWAPUD", COMM_SWAPEXITSUD);

  addCommand(&cmdHead, "RENUMBER", COMM_RENUMBER);
  addCommand(&cmdHead, "RENUMBERROOMS", COMM_RENUMBROOM);
  addCommand(&cmdHead, "RENUMBEROBJECTS", COMM_RENUMBOBJECT);
  addCommand(&cmdHead, "RENUMBERMOBS", COMM_RENUMBMOB);
  addCommand(&cmdHead, "RENROOMS", COMM_RENUMBROOM);
  addCommand(&cmdHead, "RENOBJECTS", COMM_RENUMBOBJECT);
  addCommand(&cmdHead, "RENMOBS", COMM_RENUMBMOB);

  addCommand(&cmdHead, "SETZONENUMB", COMM_SETZONENUMB);
  addCommand(&cmdHead, "SZN", COMM_SETZONENUMB);

  addCommand(&cmdHead, "FIXFLAGS", COMM_FIXFLAGS);
  addCommand(&cmdHead, "FIXCOND", COMM_FIXCOND);
  addCommand(&cmdHead, "FIXGUILDSTUFF", COMM_FIXGUILD);
  addCommand(&cmdHead, "GUILD", COMM_GUILD);

  addCommand(&cmdHead, "RECSIZE", COMM_RECSIZE);
  addCommand(&cmdHead, "RS", COMM_RECSIZE);
  addCommand(&cmdHead, "INFO", COMM_MISCINFO);

  addCommand(&cmdHead, "HELP", COMM_LONGHELP);
  addCommand(&cmdHead, "?", COMM_SHORTHELP);

  addCommand(&cmdHead, "VERSION", COMM_VERSION);

  addCommand(&cmdHead, "GRID", COMM_GRID);
  addCommand(&cmdHead, "CHECK", COMM_CHECK);
  addCommand(&cmdHead, "LINKROOMS", COMM_LINKROOMS);
  addCommand(&cmdHead, "CHECKMAP", COMM_CHECKMAP);
}


//
// initLookupCommands
//

void initLookupCommands(void)
{
  addCommand(&lookupCmdHead, "ROOM", LOOKUPCMD_ROOM);
  addCommand(&lookupCmdHead, "OBJECT", LOOKUPCMD_OBJECT);
  addCommand(&lookupCmdHead, "MOB", LOOKUPCMD_MOB);
  addCommand(&lookupCmdHead, "CHAR", LOOKUPCMD_MOB);
}


//
// initCreateCommands
//

void initCreateCommands(void)
{
  addCommand(&createCmdHead, "ROOM", CREATECMD_ROOM);
  addCommand(&createCmdHead, "OBJECT", CREATECMD_OBJECT);
  addCommand(&createCmdHead, "MOB", CREATECMD_MOB);
  addCommand(&createCmdHead, "CHAR", CREATECMD_MOB);
  addCommand(&createCmdHead, "EXIT", CREATECMD_EXIT);
}


//
// initLoadCommands
//

void initLoadCommands(void)
{
  addCommand(&loadCmdHead, "OBJECT", LOADCMD_OBJECT);
  addCommand(&loadCmdHead, "MOB", LOADCMD_MOB);
  addCommand(&loadCmdHead, "CHAR", LOADCMD_MOB);
}


//
// initEditCommands
//

void initEditCommands(void)
{
  addCommand(&editCmdHead, "ROOM", EDITCMD_ROOM);
  addCommand(&editCmdHead, "OBJECT", EDITCMD_OBJECT);
  addCommand(&editCmdHead, "MOB", EDITCMD_MOB);
  addCommand(&editCmdHead, "CHAR", EDITCMD_MOB);
  addCommand(&editCmdHead, "QUEST", EDITCMD_QUEST);
  addCommand(&editCmdHead, "SHOP", EDITCMD_SHOP);
  addCommand(&editCmdHead, "EXIT", EDITCMD_EXIT);
  addCommand(&editCmdHead, "ZONE", EDITCMD_ZONE);
  addCommand(&editCmdHead, "DESC", EDITCMD_DESC);
  addCommand(&editCmdHead, "DEFROOM", EDITCMD_DEFROOM);
  addCommand(&editCmdHead, "DEFOBJ", EDITCMD_DEFOBJ);
  addCommand(&editCmdHead, "DEFMOB", EDITCMD_DEFMOB);
  addCommand(&editCmdHead, "DEFEXIT", EDITCMD_DEFEXIT);
}


//
// initListCommands
//

void initListCommands(void)
{
  addCommand(&listCmdHead, "ROOM", LISTCMD_ROOM);
  addCommand(&listCmdHead, "EXIT", LISTCMD_EXIT);
  addCommand(&listCmdHead, "OBJECT", LISTCMD_OBJECT);
  addCommand(&listCmdHead, "MOB", LISTCMD_MOB);
  addCommand(&listCmdHead, "CHAR", LISTCMD_MOB);
  addCommand(&listCmdHead, "QUEST", LISTCMD_QUEST);
  addCommand(&listCmdHead, "SHOP", LISTCMD_SHOP);
  addCommand(&listCmdHead, "OBJHERE", LISTCMD_OBJHERE);
  addCommand(&listCmdHead, "MOBHERE", LISTCMD_MOBHERE);
  addCommand(&listCmdHead, "OH", LISTCMD_OBJHERE);
  addCommand(&listCmdHead, "MH", LISTCMD_MOBHERE);
  addCommand(&listCmdHead, "LOADED", LISTCMD_LOADED);
  addCommand(&listCmdHead, "LOOKUP", LISTCMD_LOOKUP);
}


//
// initCreateEditCommands
//

void initCreateEditCommands(void)
{
  addCommand(&createEditCmdHead, "ROOM", CRTEDITCMD_ROOM);
  addCommand(&createEditCmdHead, "OBJECT", CRTEDITCMD_OBJECT);
  addCommand(&createEditCmdHead, "MOB", CRTEDITCMD_MOB);
  addCommand(&createEditCmdHead, "CHAR", CRTEDITCMD_MOB);
  addCommand(&createEditCmdHead, "EXIT", CREATECMD_EXIT);
}


//
// initSetCommands
//

void initSetCommands(void)
{
  addCommand(&setCmdHead, "SET", SETCMD_SET);
  addCommand(&setCmdHead, "ALIAS", SETCMD_ALIAS);
  addCommand(&setCmdHead, "LIMIT", SETCMD_LIMIT);
  addCommand(&setCmdHead, "RANDOM", SETCMD_RANDOM);
  addCommand(&setCmdHead, "SETTEMPLATE", SETCMD_SETTEMP);
}


//
// initDeleteCommands
//

void initDeleteCommands(void)
{
  addCommand(&deleteCmdHead, "ROOM", DELETECMD_ROOM);
  addCommand(&deleteCmdHead, "OBJECT", DELETECMD_OBJECT);
  addCommand(&deleteCmdHead, "MOB", DELETECMD_MOB);
  addCommand(&deleteCmdHead, "CHAR", DELETECMD_MOB);
  addCommand(&deleteCmdHead, "QUEST", DELETECMD_QUEST);
  addCommand(&deleteCmdHead, "SHOP", DELETECMD_SHOP);
  addCommand(&deleteCmdHead, "EXIT", DELETECMD_EXIT);
  addCommand(&deleteCmdHead, "UNUSED", DELETECMD_UNUSED);

  addCommand(&deleteCmdHead, "DEFROOM", DELETECMD_DEFROOM);
  addCommand(&deleteCmdHead, "DEFOBJECT", DELETECMD_DEFOBJECT);
  addCommand(&deleteCmdHead, "DEFMOB", DELETECMD_DEFMOB);
  addCommand(&deleteCmdHead, "DEFEXIT", DELETECMD_DEFEXIT);

  addCommand(&deleteCmdHead, "OBJHERE", DELETECMD_OBJHERE);
  addCommand(&deleteCmdHead, "OH", DELETECMD_OBJHERE);
  addCommand(&deleteCmdHead, "MOBHERE", DELETECMD_MOBHERE);
  addCommand(&deleteCmdHead, "MH", DELETECMD_MOBHERE);
}


//
// initStatCommands
//

void initStatCommands(void)
{
  addCommand(&statCmdHead, "ROOM", STATCMD_ROOM);
  addCommand(&statCmdHead, "OBJECT", STATCMD_OBJECT);
  addCommand(&statCmdHead, "MOB", STATCMD_MOB);
  addCommand(&statCmdHead, "CHAR", STATCMD_MOB);
  addCommand(&statCmdHead, "ZONE", STATCMD_ZONE);
}


//
// initSetLimitCommands
//

void initSetLimitCommands(void)
{
  addCommand(&setLimitCmdHead, "OBJECT", SETLIMCMD_OBJECT);
  addCommand(&setLimitCmdHead, "MOB", SETLIMCMD_MOB);
  addCommand(&setLimitCmdHead, "CHAR", SETLIMCMD_MOB);
}


//
// initCloneCommands
//

void initCloneCommands(void)
{
  addCommand(&cloneCmdHead, "ROOM", CLONECMD_ROOM);
  addCommand(&cloneCmdHead, "OBJECT", CLONECMD_OBJECT);
  addCommand(&cloneCmdHead, "MOB", CLONECMD_MOB);
  addCommand(&cloneCmdHead, "CHAR", CLONECMD_MOB);
}


//
// initCopyDescCommands
//

void initCopyDescCommands(void)
{
  addCommand(&copyDescCmdHead, "ROOM", COPYDESCCMD_ROOM);
  addCommand(&copyDescCmdHead, "MOB", COPYDESCCMD_MOB);
  addCommand(&copyDescCmdHead, "CHAR", COPYDESCCMD_MOB);
}


//
// initCopyDefaultCommands
//

void initCopyDefaultCommands(void)
{
  addCommand(&copyDefaultCmdHead, "ROOM", COPYDEFCMD_ROOM);
  addCommand(&copyDefaultCmdHead, "EXIT", COPYDEFCMD_EXIT);
  addCommand(&copyDefaultCmdHead, "OBJECT", COPYDEFCMD_OBJECT);
  addCommand(&copyDefaultCmdHead, "MOB", COPYDEFCMD_MOB);
  addCommand(&copyDefaultCmdHead, "CHAR", COPYDEFCMD_MOB);
}



//
// initCopyCommands
//

void initCopyCommands(void)
{
  addCommand(&copyCmdHead, "DEFAULT", COPYCMD_DEFAULT);
  addCommand(&copyCmdHead, "DESC", COPYCMD_DESC);
}


//
// initFlagFieldNames
//

// someday I may use these

/*
void initFlagFieldNames(void)
{
  addCommand(&flagFieldNameHead, "ROOMFLAG", FLAGNAME_ROOMFLAG);
  addCommand(&flagFieldNameHead, "OBJEXTRA", FLAGNAME_OBJEXTRA);
  addCommand(&flagFieldNameHead, "OBJWEAR", FLAGNAME_OBJWEAR);
  addCommand(&flagFieldNameHead, "OBJAFF1", FLAGNAME_OBJAFF1);
  addCommand(&flagFieldNameHead, "OBJAFF2", FLAGNAME_OBJAFF2);
  addCommand(&flagFieldNameHead, "MOBACT", FLAGNAME_MOBACT);
  addCommand(&flagFieldNameHead, "MOBAFF1", FLAGNAME_MOBAFF1);
  addCommand(&flagFieldNameHead, "MOBAFF2", FLAGNAME_MOBAFF2);
}
*/
