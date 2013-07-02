//
//  File: setrand.cpp    originally part of dikuEdit
//
//  Usage: functions for handling 'setrand' command
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "../misc/master.h"
#include "../mob/mobhere.h"
#include "../obj/objhere.h"

//
// setEntityRandomVal
//

void setEntityRandomVal(const char *args, dikuRoom *room)
{
  char whatMatched, arg1[512], arg2[512], outstrn[512];
  masterKeywordListNode *matchingNode;
  objectHere *obj;
  mobHere *mob;

  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (!strnumer(arg2) || (atoi(arg2) <= 0) || (atoi(arg2) > 100))
  {
    _outtext("\nInput for second argument is invalid (should be 1-100).\n\n");
    return;
  }

  checkMasterKeywordList(arg1, room->masterListHead,
                         &whatMatched, &matchingNode);

  switch (whatMatched)
  {
    case NO_MATCH :
       _outtext("\nNothing in this room matches that keyword.\n\n");
       return;

    case ENTITY_OBJECT :
       obj = ((objectHere *)(matchingNode->entityPtr));

       obj->randomChance = atoi(arg2);

       sprintf(outstrn, "\nRandom chance for '%s&n' to load set to %u%%.\n\n",
               (obj->objectPtr) ? obj->objectPtr->objShortName :
                                  "(object not in this .OBJ file)",
               obj->randomChance);
       displayColorString(outstrn);

       return;

    case ENTITY_MOB :
       mob = ((mobHere *)(matchingNode->entityPtr));

       mob->randomChance = atoi(arg2);

       sprintf(outstrn, "\nRandom chance for '%s&n' to load set to %u%%.\n\n",
               (mob->mobPtr) ? mob->mobPtr->mobShortName :
                                  "(mob not in this .MOB file)",
               mob->randomChance);
       displayColorString(outstrn);

       return;

    default :
       _outtext("\nInvalid target, shmuck-boy.\n\n");
       return;
  }
}
