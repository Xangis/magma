//
//  File: purge.cpp      originally part of dikuEdit
//
//  Usage: functions for handling the 'purge' command
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"

#include "../room/room.h"
#include "../obj/objhere.h"
#include "../mob/mobhere.h"
#include "../zone/zone.h"
#include "../misc/editable.h"

extern dikuRoom *currentRoom, *roomHead;
extern char madeChanges;
extern editableListNode *inventory;

//
// purge
//

void purge(char *args)
{
  char strn[512], name[512], whatMatched;
  dikuRoom *room = roomHead;
  objectHere *obj;
  mobHere *mob;
  editableListNode *matchingNode;
  usint ch;
  long i;


 // user entered "purge", with no arguments

  if (!strlen(args))
  {
    if ((!currentRoom->objectHead) && (!currentRoom->mobHead))
    {
      _outtext("\nNothing in this room to purge.\n\n");
      return;
    }

    displayColorString(
"\n&+cAre you sure you want to purge all mobs and objects in this room (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");
      return;
    }
    else displayColorString("Yes&n\n");

    deleteObjHereList(currentRoom->objectHead, TRUE);
    currentRoom->objectHead = NULL;

    deleteMobHereList(currentRoom->mobHead, TRUE);
    currentRoom->mobHead = NULL;

    deleteMasterKeywordList(currentRoom->masterListHead);
    currentRoom->masterListHead = NULL;

    deleteEditableList(currentRoom->editableListHead);
    currentRoom->editableListHead = NULL;

    madeChanges = TRUE;

    _outtext("All objects and mobs in current room deleted.\n\n");
    return;
  }

 // if user typed ALL, delete all mobs and objs

  if (!strcmp(args, "ALL"))
  {
    displayColorString(
"\n&+cDelete all objects and mobs in zone (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");
      return;
    }

    displayColorString("Yes&n\n");

    while (room)  // set to roomHead above
    {
      deleteObjHereList(room->objectHead, TRUE);
      room->objectHead = NULL;

      deleteMobHereList(room->mobHead, TRUE);
      room->mobHead = NULL;

      room = room->Next;
    }

//    numbObjs = numbMobs = 0;
//    updateAllMandElists();  // update rooms with objs or mobs

    updateAllObjMandElists();  // can't use updateAllMandElists() since it
    updateAllMobMandElists();  // checks extra descs as well

    madeChanges = TRUE;  // let's assume the zone had something in it
    createPrompt();

    displayColorString("&+cAll objects and mobs in zone deleted.&n\n\n");
    return;
  }

 // delete all objects

  if (!strcmp(args, "ALLOBJ"))
  {
    displayColorString(
"\n&+cDelete all objects in zone (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");
      return;
    }

    displayColorString("Yes&n\n");

    while (room)  // set to roomHead above
    {
      deleteObjHereList(room->objectHead, TRUE);
      room->objectHead = NULL;

      mob = room->mobHead;
      while (mob)
      {
        for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
        {
          if (mob->equipment[i])
          {
            deleteObjHere(mob->equipment[i], TRUE);
            mob->equipment[i] = NULL;
          }
        }

        deleteObjHereList(mob->objectHead, TRUE);
        mob->objectHead = NULL;

        mob = mob->Next;
      }

      room = room->Next;
    }

    updateAllObjMandElists();  // update rooms with objects
    madeChanges = TRUE;  // hell, why not
    createPrompt();

    displayColorString("&+cAll objects in zone deleted.&n\n\n");
    return;
  }

 //  delete all mobs

  if (!strcmp(args, "ALLMOB"))
  {
    displayColorString(
"\n&+cDelete all mobs in zone (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("No&n\n\n");
      return;
    }

    displayColorString("Yes&n\n");

    while (room)  // set to roomHead above
    {
      deleteMobHereList(room->mobHead, TRUE);
      room->mobHead = NULL;

      room = room->Next;
    }

    updateAllMobMandElists();  // update rooms with mobs
    madeChanges = TRUE;  // hell, why not
    createPrompt();

    displayColorString("&+cAll mobs in zone deleted.&n\n\n");
    return;
  }

 // if user typed INV, delete everything in inventory

  if (!strcmp(args, "INV"))
  {
    if (!inventory)
    {
      _outtext("\nNo inventory to purge!\n\n");
      return;
    }

    deleteEditableList(inventory);
    inventory = NULL;

    _outtext("\nAll objects/mobs in inventory deleted.\n\n");

//    madeChanges = TRUE;  // no reason - when inventory is saveable
    return;
  }

 // user entered argument - check to see if it's a vnum or a keywordish string

  if (!strnumer(args))
  {
    checkEditableList(args, currentRoom->editableListHead, &whatMatched,
                      &matchingNode, 1);

    switch (whatMatched)
    {
      case NO_MATCH : _outtext("\nNo match found for that keyword.\n\n");
                      return;

      case ENTITY_OBJ_ED : _outtext("\nCannot purge object extra descs.\n\n");
                           return;

      case ENTITY_R_EDESC : _outtext("\nCannot purge room extra descs.\n\n");
                            return;

      case ENTITY_OBJECT :
         sprintf(strn, "\nObject '%s&n' (#%u) purged from current room.\n\n",
                 ((objectHere *)(matchingNode->entityPtr))->objectPtr->
                  objShortName,
                 ((objectHere *)(matchingNode->entityPtr))->objectNumb);

         deleteObjHereinList(&(currentRoom->objectHead),
                             (objectHere *)(matchingNode->entityPtr), TRUE);

         displayColorString(strn);

         deleteMasterKeywordList(currentRoom->masterListHead);
         currentRoom->masterListHead = createMasterKeywordList(currentRoom);

         deleteEditableList(currentRoom->editableListHead);
         currentRoom->editableListHead = createEditableList(currentRoom);

         madeChanges = TRUE;

         return;

      case ENTITY_MOB :
         sprintf(strn, "\nMob '%s&n' (#%u) purged from current room.\n\n",
                 ((mobHere *)(matchingNode->entityPtr))->mobPtr->
                  mobShortName,
                 ((mobHere *)(matchingNode->entityPtr))->mobNumb);

         deleteMobHereinList(&(currentRoom->mobHead),
                             (mobHere *)(matchingNode->entityPtr), TRUE);

         displayColorString(strn);

         deleteMasterKeywordList(currentRoom->masterListHead);
         currentRoom->masterListHead = createMasterKeywordList(currentRoom);

         deleteEditableList(currentRoom->editableListHead);
         currentRoom->editableListHead = createEditableList(currentRoom);

         madeChanges = TRUE;

         return;

      default :
         sprintf(strn, "\nUnrecognized value - %d.\n\n",
                 whatMatched);
         _outtext(strn);
         return;
    }
  }

  i = atoi(args);

  obj = currentRoom->objectHead;

  while (obj)
  {
    if (obj->objectNumb == i)
    {
      if (obj->objectPtr) strcpy(name, obj->objectPtr->objShortName);
      else strcpy(name, "(object not in this .OBJ file)");

      sprintf(strn, "\nObject '%s&n' (#%u) purged from this room.\n\n",
              name, i);

      displayColorString(strn);

      deleteObjHereinList(&(currentRoom->objectHead), obj, TRUE);

      deleteMasterKeywordList(currentRoom->masterListHead);
      currentRoom->masterListHead = createMasterKeywordList(currentRoom);

      deleteEditableList(currentRoom->editableListHead);
      currentRoom->editableListHead = createEditableList(currentRoom);

      madeChanges = TRUE;

      return;
    }

    obj = obj->Next;
  }

  mob = currentRoom->mobHead;

  while (mob)
  {
    if (mob->mobNumb == i)
    {
      if (mob->mobPtr) strcpy(name, mob->mobPtr->mobShortName);
      else strcpy(name, "(mob not in this .MOB file)");

      sprintf(strn, "\nMob '%s&n' (#%u) purged from this room.\n\n",
              name, i);

      displayColorString(strn);

      deleteMobHereinList(&(currentRoom->mobHead), mob, TRUE);

      deleteMasterKeywordList(currentRoom->masterListHead);
      currentRoom->masterListHead = createMasterKeywordList(currentRoom);

      deleteEditableList(currentRoom->editableListHead);
      currentRoom->editableListHead = createEditableList(currentRoom);

      madeChanges = TRUE;

      return;
    }

    mob = mob->Next;
  }

  _outtext("\nNo objects/mobs matching vnum specified found.\n\n");
}
