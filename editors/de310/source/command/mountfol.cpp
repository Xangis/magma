//
//  File: mountfol.cpp   originally part of dikuEdit
//
//  Usage: functions for making mobs follow/ride other mobs
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "../room/room.h"
#include "../mob/mobhere.h"
#include "../misc/master.h"

extern char madeChanges;

//
// mountMob
//

void mountMob(const char *args, dikuRoom *room)
{
  char arg1[256], arg2[256], strn[1024], mountName[256], mountingName[256],
       whatMatched;
  editableListNode *matchingNode;
  mobHere *mounting = NULL, *mount = NULL, *mob;
  long i;


  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if ((arg1[0] == '\0') || (arg2[0] == '\0'))
  {
    _outtext(
"\nThe 'mount' command requires two arguments - the mounting mob and the mount.\n\n");
    return;
  }

 // check arg1 (mounting) to see if it's a vnum or a keywordish string

  if (!strnumer(arg1))
  {
    checkEditableList(arg1, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in first arg (mounting mob).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't mount, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't mount, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't mount, silly.\n\n");
                           return;

      case ENTITY_MOB :
         mounting = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg1);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) mounting = mob;

      mob = mob->Next;
    }

    if (!mounting)
    {
      _outtext("\nNo mobs found that match vnum in first arg.\n\n");
      return;
    }
  }


 // check arg2 (mount) to see if it's a vnum or a keywordish string

  if (!strnumer(arg2))
  {
    checkEditableList(arg2, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in second arg (mob being mounted).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't be mounted, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't be mounted, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't be mounted, silly.\n\n");
                           return;

      case ENTITY_MOB :
         mount = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg2);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) mount = mob;

      mob = mob->Next;
    }

    if (!mount)
    {
      _outtext("\nNo mobs found that match vnum in second arg.\n\n");
      return;
    }
  }

  if (mount == mounting)
  {
    _outtext("\nA mob can't ride itself..\n\n");
    return;
  }

  if (mount->riding == mounting)
  {
    _outtext("\nWe can't have mobs riding each other now, can we?\n\n");
    return;
  }

  if (mounting->riding == mount)
  {
    _outtext("\nThat mount is already ridden by that mob.\n\n");
    return;
  }

  if (mounting->riding)
  {
    _outtext("\nThe mounting mob already is riding another.\n\n");
    return;
  }

  if (mount->riddenBy)
  {
    _outtext("\nThat mount is already being ridden by another mob.\n\n");
    return;
  }

  mount->riddenBy = mounting;
  mounting->riding = mount;

  if (mount->mobPtr) strcpy(mountName, mount->mobPtr->mobShortName);
  else strcpy(mountName, "(mob not in this .MOB file)");

  if (mounting->mobPtr) strcpy(mountingName, mounting->mobPtr->mobShortName);
  else strcpy(mountingName, "(mob not in this .MOB file)");

  sprintf(strn, "\nMob '%s&n' now rides mob '%s&n'.\n\n", mountingName, mountName);
  displayColorString(strn);

  madeChanges = TRUE;
}


//
// unmountMob
//

void unmountMob(const char *args, dikuRoom *room)
{
  char arg1[256], strn[1024], mountingName[256], mountName[256],
       whatMatched;
  editableListNode *matchingNode;
  mobHere *mounting = NULL, *mob;
  long i;


  getArg(args, 1, arg1);

  if (arg1[0] == '\0')
  {
    _outtext(
"\nThe 'unmount' command requires one argument - the mob that is mounting a mob.\n\n");
    return;
  }

 // check arg1 (mounting) to see if it's a vnum or a keywordish string

  if (!strnumer(arg1))
  {
    checkEditableList(arg1, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in first arg (mob that is mounting).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't mount, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't mount, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't mount, silly.\n\n");
                           return;

      case ENTITY_MOB :
         mounting = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg1);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) mounting = mob;

      mob = mob->Next;
    }

    if (!mounting)
    {
      _outtext("\nNo mobs found that match vnum in first arg (mounting mob).\n\n");
      return;
    }
  }

  if (!mounting->riding)
  {
    _outtext("\nThat mob isn't riding anything.\n\n");
    return;
  }

  if (mounting->riding->mobPtr) strcpy(mountName, mounting->riding->mobPtr->mobShortName);
  else strcpy(mountName, "(mob not in this .MOB file)");

  mounting->riding->riddenBy = NULL;
  mounting->riding = NULL;

  if (mounting->mobPtr) strcpy(mountingName, mounting->mobPtr->mobShortName);
  else strcpy(mountingName, "(mob not in this .MOB file)");

  sprintf(strn, "\nMob '%s&n' no longer rides mob '%s&n'.\n\n", mountingName, mountName);
  displayColorString(strn);

  madeChanges = TRUE;
}


//
// followMob
//

void followMob(const char *args, dikuRoom *room)
{
  char arg1[256], arg2[256], strn[1024], leaderName[256], followingName[256],
       whatMatched;
  editableListNode *matchingNode;
  mobHere *following = NULL, *leader = NULL, *mob;
  long i;


  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if ((arg1[0] == '\0') || (arg2[0] == '\0'))
  {
    _outtext(
"\nThe 'follow' command requires two arguments - the following mob and the leader.\n\n");
    return;
  }

 // check arg1 (following) to see if it's a vnum or a keywordish string

  if (!strnumer(arg1))
  {
    checkEditableList(arg1, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in first arg (mob that is following).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't follow, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't follow, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't follow, silly.\n\n");
                           return;

      case ENTITY_MOB :
         following = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg1);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) following = mob;

      mob = mob->Next;
    }

    if (!following)
    {
      _outtext("\nNo mobs found that match vnum in first arg (mob that is following).\n\n");
      return;
    }
  }


 // check arg2 (leader) to see if it's a vnum or a keywordish string

  if (!strnumer(arg2))
  {
    checkEditableList(arg2, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in second arg (mob that is leading).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't be followed, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't be followed, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't be followed, silly.\n\n");
                           return;

      case ENTITY_MOB :
         leader = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg2);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) leader = mob;

      mob = mob->Next;
    }

    if (!leader)
    {
      _outtext("\nNo mobs found that match vnum in second arg (mob that is leading).\n\n");
      return;
    }
  }

  if (leader == following)
  {
    _outtext("\nA mob can't follow itself..\n\n");
    return;
  }

  if (leader->following == following)
  {
    _outtext("\nWe can't have mobs following each other now, can we?\n\n");
    return;
  }

  if (following->following == leader)
  {
    _outtext("\nThat mob is already following that leader.\n\n");
    return;
  }

  if (following->following)
  {
    _outtext("\nThat mob is already following another.\n\n");
    return;
  }

  following->following = leader;

  if (leader->mobPtr) strcpy(leaderName, leader->mobPtr->mobShortName);
  else strcpy(leaderName, "(mob not in this .MOB file)");

  if (following->mobPtr) strcpy(followingName, following->mobPtr->mobShortName);
  else strcpy(followingName, "(mob not in this .MOB file)");

  sprintf(strn, "\nMob '%s&n' now follows mob '%s&n'.\n\n", followingName, leaderName);
  displayColorString(strn);

  madeChanges = TRUE;
}


//
// unfollowMob
//

void unfollowMob(const char *args, dikuRoom *room)
{
  char arg1[256], strn[1024], followingName[256], leaderName[256],
       whatMatched;
  editableListNode *matchingNode;
  mobHere *following = NULL, *mob;
  long i;


  getArg(args, 1, arg1);

  if (arg1[0] == '\0')
  {
    _outtext(
"\nThe 'unfollow' command requires one argument - the mob that is following a mob.\n\n");
    return;
  }

 // check arg1 (following) to see if it's a vnum or a keywordish string

  if (!strnumer(arg1))
  {
    checkEditableList(arg1, room->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for keyword in first arg (mob following another).\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nObject extra descs can't follow, silly.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nRoom extra descs can't follow, silly.\n\n");
                            return;

      case ENTITY_OBJECT : _outtext("\nObjects can't follow, silly.\n\n");
                           return;

      case ENTITY_MOB :
         following = ((mobHere *)(matchingNode->entityPtr));

         break;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n", whatMatched);
         _outtext(strn);
         return;
    }
  }
  else
  {
    i = atoi(arg1);

    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobNumb == i) following = mob;

      mob = mob->Next;
    }

    if (!following)
    {
      _outtext("\nNo mobs found that match vnum in first arg (mob that is following).\n\n");
      return;
    }
  }

  if (!following->following)
  {
    _outtext("\nThat mob isn't following anything.\n\n");
    return;
  }

  if (following->following->mobPtr) strcpy(leaderName, following->following->mobPtr->mobShortName);
  else strcpy(leaderName, "(mob not in this .MOB file)");

  following->following = NULL;

  if (following->mobPtr) strcpy(followingName, following->mobPtr->mobShortName);
  else strcpy(followingName, "(mob not in this .MOB file)");

  sprintf(strn, "\nMob '%s&n' no longer follows mob '%s&n'.\n\n", followingName, leaderName);
  displayColorString(strn);

  madeChanges = TRUE;
}
