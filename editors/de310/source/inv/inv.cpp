//
//  File: inv.cpp        originally part of dikuEdit
//
//  Usage: functions for manipulating objects into/out of inventory
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../fh.h"
#include "../types.h"

#include "../misc/editable.h"
#include "../misc/master.h"
#include "../obj/objhere.h"
#include "../mob/mobhere.h"

extern editableListNode *inventory;
extern char madeChanges;
extern ulong numbObjs, numbMobs;

//
// showInventory : Shows yer inventory
//

void showInventory(void)
{
  editableListNode *editNode = inventory;
  objectHere *objHere;
  mobHere *mob;
  char strn[256], name[256], keystrn[256];


  displayColorString("\n\n&+MYou are carrying -&n\n\n");

  if (editNode)
  {
    while (editNode)
    {
      switch (editNode->entityType)
      {
        case ENTITY_OBJECT  :
          objHere = (objectHere *)(editNode->entityPtr);

          if (objHere->objectPtr)
          {
            strcpy(name, objHere->objectPtr->objShortName);
            getReadableKeywordStrn(objHere->objectPtr->keywordListHead,
                                   keystrn);
          }
          else
          {
            strcpy(name, "(object not in this .OBJ file)");
            strcpy(keystrn, "keywords unknown");
          }

          sprintf(strn, "%s&n (#%u) (%s) &+c(%u%%)&n\n",
                  name, objHere->objectNumb, keystrn, objHere->randomChance);

          displayColorString(strn);
          break;

        case ENTITY_MOB     :
          mob = (mobHere *)(editNode->entityPtr);

          if (mob->mobPtr)
          {
            strcpy(name, mob->mobPtr->mobShortName);
            getReadableKeywordStrn(mob->mobPtr->keywordListHead, keystrn);
          }
          else
          {
            strcpy(name, "(mob not in this .MOB file)");
            strcpy(keystrn, "keywords unknown");
          }

          sprintf(strn, "%s&n (#%u) (%s) &+c(%u%%)&n\n",
                  name, mob->mobNumb, keystrn, mob->randomChance);

          displayColorString(strn);
          break;


        default : sprintf(strn, "Unrecognized/nutty entity type (#%d)\n",
                          editNode->entityType);
                  _outtext(strn);
                  break;
      }

      editNode = editNode->Next;
    }
  }
  else _outtext("nothing\n");

  _outtext("\n");
}


//
// takeEntityFromEntity : Picks up the entity based on the keyword in *strn
//                        (starting at pos), checking *room's keyword list.  If
//                        deleteOriginal is true, the original is deleted.
//
//           *strn : string entered by user
//             pos : position keyword starts at
//
//           *room : room user is picking stuff up from
//
//  deleteOriginal : if TRUE, deletes original object
//

void takeEntityFromEntity(char *objStrn, char *containerStrn, dikuRoom *room,
                          const uchar deleteOriginal)
{
  char contMatched = NO_MATCH, deleteCont = FALSE;
  editableListNode *contNode;
  long objnumb = -1, contnumb = -1;
  ulong blah;

  editableListNode *invNode, *invPrevNode;
  objectHere *objHere = NULL, *anotherObjHere;
  mobHere *mob = NULL;
  char tempStrn[768], tempStrn2[256];


  if (strnumer(objStrn)) objnumb = atoi(objStrn);
  if (strnumer(containerStrn))
  {
    contnumb = atoi(containerStrn);

    objHere = findObjHere(contnumb, room, &blah, TRUE);

    if (!objHere)
    {
      mob = findMobHere(contnumb, room, &blah, TRUE);
      if (mob)
      {
        contMatched = ENTITY_MOB;

        contNode = new editableListNode;  // this is a small memory leak -
        if (!contNode)                    // shouldn't be anymore
        {
          _outtext("Mem error in takeEntFromEnt\n");
          return;
        }

        memset(contNode, 0, sizeof(editableListNode));

        if (mob->mobPtr) contNode->keywordListHead =
                         copyStringNodes(mob->mobPtr->keywordListHead);

        contNode->entityPtr = mob;
        contNode->entityType = ENTITY_MOB;
//        contNode->entityOwner =    not needed
        contNode->Next = NULL;

        deleteCont = TRUE;
      }
    }
    else
    {
      contMatched = ENTITY_OBJECT;

      contNode = new editableListNode;  // this is a small memory leak -
      if (!contNode)                    // should be fixed
      {
        _outtext("Mem error in takeEntFromEnt\n");
        return;
      }

      memset(contNode, 0, sizeof(editableListNode));

      if (objHere->objectPtr) contNode->keywordListHead =
                       copyStringNodes(objHere->objectPtr->keywordListHead);

      contNode->entityPtr = objHere;
      contNode->entityType = ENTITY_OBJECT;
//      contNode->entityOwner =    not needed
      contNode->Next = NULL;

      deleteCont = TRUE;
    }
  }

  if (contMatched == NO_MATCH)
  {
    checkEditableList(containerStrn, room->editableListHead, &contMatched,
                      &contNode, 1);
  }

  switch (contMatched)
  {
    case NO_MATCH : _outtext("No match on second keyword (container/mob).\n");  return;

    case ENTITY_OBJ_ED :
       _outtext(
"Sorry, but you can't get anything from an object extra desc.\n");
       return;

    case ENTITY_R_EDESC :
       _outtext(
"Sorry, but you can't get anything from a room extra desc.\n");
       return;

    case ENTITY_OBJECT :

       if (!objHere) objHere = (objectHere *)(contNode->entityPtr);

      // allow user to get shit out of an undefined object type

       if (objHere->objectPtr &&
           (objHere->objectPtr->objType != ITEM_CONTAINER))
       {
         _outtext("Object matched for keyword is not a container.\n");

         if (deleteCont) delete contNode;
         return;
       }

       if (!(objHere->objInside))
       {
         _outtext("Nothing is in that container.\n");

         if (deleteCont) delete contNode;
         return;
       }

      // try to find object that matches second keyword

      // do the 'grab by vnum' macarena

       if (!strcmp(objStrn, "ALL"))
       {
         objHere = anotherObjHere = copyObjHereList(objHere->objInside, FALSE);

         while (objHere)
         {
           itoa(objHere->objectNumb, tempStrn, 10);
           strcpy(tempStrn2, containerStrn);
           takeEntityFromEntity(tempStrn, tempStrn2, room, deleteOriginal);

           objHere = objHere->Next;
         }

         deleteObjHereList(anotherObjHere, FALSE);

         if (deleteCont) delete contNode;
         return;
       }

       objHere = objHere->objInside;

       while (objHere)
       {
         if (objnumb >= 0)
         {
           if (objHere->objectNumb == objnumb) break;
         }
         else
         {
           if ((objHere->objectPtr) &&
               (scanKeyword(objStrn, objHere->objectPtr->keywordListHead)))
             break;
         }

         objHere = objHere->Next;
       }

       if (!objHere)
       {
         _outtext(
"None of the objects inside container match keyword/vnum specified.\n");

         if (deleteCont) delete contNode;
         return;
       }
//       else _outtext("\nMatching obj found inside container.\n\n");

      // create new node for inventory and stick it in the list

       invNode = new editableListNode;
       if (!invNode)
       {
         _outtext("ERROR: takeEntityFromEntity() - error alloc'ing invNode.\n");

         if (deleteCont) delete contNode;
         return;
       }

       memset(invNode, 0, sizeof(editableListNode));

       if (objHere->objectPtr)
       {
         invNode->keywordListHead =
           copyStringNodes(objHere->objectPtr->keywordListHead);
       }

       invNode->entityType = ENTITY_OBJECT;
       invNode->Next = NULL;

      // copy all objectHere info so that everything works right - may be
      // some memory leaks in here, but screw it

       invNode->entityPtr = copyObjHere(objHere, FALSE);
       ((objectHere *)(invNode->entityPtr))->Next = NULL;

      // add invNode to inventory

       if (!inventory) inventory = invNode;
       else
       {
         invPrevNode = inventory;
         while (invPrevNode->Next)
         {
           invPrevNode = invPrevNode->Next;
         }

         invPrevNode->Next = invNode;
       }

       if (objHere->objectPtr)
       {
         strcpy(objStrn, objHere->objectPtr->objShortName);
       }
       else strcpy(objStrn, "(object not in this .OBJ file)");

       if (((objectHere *)(contNode->entityPtr))->objectPtr)
       {
         strcpy(containerStrn,
                ((objectHere *)(contNode->entityPtr))->objectPtr->objShortName);
       }
       else strcpy(containerStrn, "(object not in this .OBJ file)");

       sprintf(tempStrn, "'%s&n' taken from '%s&n'.\n",
               objStrn, containerStrn);

       if (deleteOriginal)
       {
         deleteObjHereinList(&(((objectHere *)(contNode->entityPtr))->objInside),
                             objHere, TRUE);

         madeChanges = TRUE;
         createPrompt();

        // these probably aren't necessary but maybe in the future, who knows

         deleteMasterKeywordList(room->masterListHead);
         room->masterListHead = createMasterKeywordList(room);

         deleteEditableList(room->editableListHead);
         room->editableListHead = createEditableList(room);
       }

       if (deleteCont) delete contNode;

//       _outtext("Object taken from container.\n");

       displayColorString(tempStrn);

       return;


    case ENTITY_MOB :

       if (!mob) mob = (mobHere *)(contNode->entityPtr);

       if (!(mob->objectHead))
       {
         _outtext("Nothing is being carried by that mob.\n");
         return;
       }

      // try to find object on mob that matches second keyword

      // do the 'grab by vnum' macarena

       if (!strcmp(objStrn, "ALL"))
       {
         objHere = anotherObjHere = copyObjHereList(mob->objectHead, FALSE);

         while (objHere)
         {
           itoa(objHere->objectNumb, tempStrn, 10);
           strcpy(tempStrn2, containerStrn);
           takeEntityFromEntity(tempStrn, tempStrn2, room, deleteOriginal);

           objHere = objHere->Next;
         }

         deleteObjHereList(anotherObjHere, FALSE);

         if (deleteCont) delete contNode;
         return;
       }

       objHere = mob->objectHead;

       while (objHere)
       {
         if (objnumb >= 0)
         {
           if (objHere->objectNumb == objnumb) break;
         }
         else
         {
           if (objHere->objectPtr)
           {
             if (scanKeyword(objStrn, objHere->objectPtr->keywordListHead))
               break;
           }
         }

         objHere = objHere->Next;
       }

       if (!objHere)
       {
         _outtext(
"None of the objects being carried by the mob match keyword specified.\n");

         if (deleteCont) delete contNode;
         return;
       }
//       else _outtext("Matching obj found on mob.\n");

      // create new node for inventory and stick it in the list

       invNode = new editableListNode;
       if (!invNode)
       {
         _outtext("ERROR: takeEntityFromEntity() - error alloc'ing invNode.\n");

         if (deleteCont) delete contNode;
         return;
       }

       memset(invNode, 0, sizeof(editableListNode));

       if (objHere->objectPtr)
       {
         invNode->keywordListHead =
           copyStringNodes(objHere->objectPtr->keywordListHead);
       }

       invNode->entityType = ENTITY_OBJECT;
       invNode->Next = NULL;

      // copy all objectHere info so that everything works right - may be
      // some memory leaks in here, but screw it

       invNode->entityPtr = copyObjHere(objHere, FALSE);
       ((objectHere *)(invNode->entityPtr))->Next = NULL;

      // add invNode to inventory

       if (!inventory) inventory = invNode;
       else
       {
         invPrevNode = inventory;
         while (invPrevNode->Next)
         {
           invPrevNode = invPrevNode->Next;
         }

         invPrevNode->Next = invNode;
       }

       if (objHere->objectPtr)
       {
         strcpy(objStrn, objHere->objectPtr->objShortName);
       }
       else strcpy(objStrn, "(object not in this .OBJ file)");

       if (((objectHere *)(contNode->entityPtr))->objectPtr)
       {
         strcpy(containerStrn,
                ((mobHere *)(contNode->entityPtr))->mobPtr->mobShortName);
       }
       else strcpy(containerStrn, "(mob not in this .MOB file)");

       sprintf(tempStrn, "'%s&n' taken from '%s&n'.\n",
               objStrn, containerStrn);

       if (deleteOriginal)
       {
         deleteObjHereinList(&(((mobHere *)(contNode->entityPtr))->objectHead),
                             objHere, TRUE);

         madeChanges = TRUE;
         createPrompt();

        // these probably aren't necessary but maybe in the future, who knows

         deleteMasterKeywordList(room->masterListHead);
         room->masterListHead = createMasterKeywordList(room);

         deleteEditableList(room->editableListHead);
         room->editableListHead = createEditableList(room);
       }

       if (deleteCont) delete contNode;

       displayColorString(tempStrn);
//       _outtext("Object taken from mob.\n");

       return;

    default : sprintf(tempStrn, "Unrecognized value - %d.\n", contMatched);
              _outtext(tempStrn);  return;
  }
}


//
// takeEntity : Picks up the entity based on the keyword in *strn (starting
//              at pos), checking *room's keyword list.  If deleteOriginal
//              is true, the original is deleted (woah).
//
//           *args : string entered by user
//
//           *room : room user is picking stuff up from
//
//  deleteOriginal : if TRUE, deletes original object
//

void takeEntity(char *args, dikuRoom *room, const char deleteOriginal)
{
  char tempStrn[768], whatMatched = NO_MATCH, strn1[256], strn2[256],
       objOnly = FALSE, mobOnly = FALSE, deleteMatching = FALSE;
  editableListNode *matchingNode, *invNode, *invPrevNode;
  objectHere *oldObjHere, *objHere = NULL;
  mobHere *oldMobHere, *mob = NULL;
  ulong blah;
  long vnum = -1;


 // if more than one arg, taking obj from mob/obj

  if (numbArgs(args) >= 2)
  {
    takeEntityFromEntity(getArg(args, 1, strn1), getArg(args, 2, strn2), room,
                         deleteOriginal);
    return;
  }

  if (!strcmp(args, "ALL"))
  {
   // simply run through all objs, grabbing by vnum - add option to also
   // pick up mobs when doing a "get all"

    objHere = oldObjHere = copyObjHereList(room->objectHead, FALSE);

    while (objHere)
    {
//      strcpy(tempStrn, "O.");
      itoa(objHere->objectNumb, tempStrn, 10);
//      strcat(tempStrn, strn1);

      takeEntity(tempStrn, room, deleteOriginal);

      objHere = objHere->Next;
    }

    deleteObjHereList(oldObjHere, FALSE);

    return;
  }

/*
  if (strleft(args, "M.")) mobOnly = TRUE;
  else if (strleft(args, "O.")) objOnly = TRUE;

  if (mobOnly || objOnly)
  {
    deleteChar(args, 0);
    deleteChar(args, 0);
  }
*/

  if (strnumer(args))
  {
    vnum = atoi(args);
    if (!mobOnly) objHere = findObjHere(vnum, room, &blah, TRUE, TRUE);
    else objHere = NULL;

    if (!objHere && !mobOnly)
    {
      mob = findMobHere(vnum, room, &blah, TRUE);
      if (mob)
      {
        if (mob->riding || mob->riddenBy || mob->following)
        {
          _outtext(
"Sorry, but you cannot pick up mobs being ridden by, riding, or following\n"
"another mob.\n");

          return;
        }

        if (mob->mobPtr && mob->mobPtr->shopPtr && !deleteOriginal)
        {
          _outtext(
"Sorry, but you cannot pick up copies of mobs with shops.\n");

          return;
        }

        whatMatched = ENTITY_MOB;

        matchingNode = new editableListNode;  // this is a small memory leak
        if (!matchingNode)                    // shouldn't be anymore
        {
          _outtext("Mem error in takeEnt\n");
          return;
        }

        memset(matchingNode, 0, sizeof(editableListNode));

        if (mob->mobPtr) matchingNode->keywordListHead =
                         copyStringNodes(mob->mobPtr->keywordListHead);

        matchingNode->entityPtr = mob;
        matchingNode->entityType = ENTITY_MOB;
        matchingNode->entityOwner = room;
        matchingNode->Next = NULL;

        deleteMatching = TRUE;
      }
    }
    else
    {
      whatMatched = ENTITY_OBJECT;

      matchingNode = new editableListNode;  // this is a small memory leak
      if (!matchingNode)                    // not anymore
      {
        _outtext("Mem error in takeEnt\n");
        return;
      }

      memset(matchingNode, 0, sizeof(editableListNode));

      if (objHere->objectPtr) matchingNode->keywordListHead =
                       copyStringNodes(objHere->objectPtr->keywordListHead);

      matchingNode->entityPtr = objHere;
      matchingNode->entityType = ENTITY_OBJECT;
      matchingNode->entityOwner = room;
      matchingNode->Next = NULL;

      deleteMatching = TRUE;
    }
  }

  if (whatMatched == NO_MATCH)
  {
    checkEditableList(args, room->editableListHead, &whatMatched,
                      &matchingNode, 1);
  }

  switch (whatMatched)
  {
    case NO_MATCH : _outtext("No match.\n");  return;

    case ENTITY_OBJ_ED :
       _outtext("Sorry, but object extra descs aren't gettable.\n");
       return;

    case ENTITY_R_EDESC :
       _outtext("Sorry, but room extra descs aren't gettable.\n");
       return;

    case ENTITY_OBJECT :

       if (!objHere) objHere = (objectHere *)(matchingNode->entityPtr);

      // create new node for inventory and stick it in the list

       invNode = new editableListNode;
       if (!invNode)
       {
         _outtext("ERROR: takeEntity() - error alloc'ing invNode.\n");

         if (deleteMatching) delete matchingNode;
         return;
       }

      // copy crap in matchingNode to invNode

       memcpy(invNode, matchingNode, sizeof(editableListNode));

       if (matchingNode->keywordListHead)
       {
         invNode->keywordListHead =
            copyStringNodes(matchingNode->keywordListHead);
       }
       else invNode->keywordListHead = NULL;

       invNode->Next = NULL;

       if (!inventory) inventory = invNode;
       else
       {
         invPrevNode = inventory;
         while (invPrevNode->Next)
         {
           invPrevNode = invPrevNode->Next;
         }

         invPrevNode->Next = invNode;
       }

      // copy all objectHere info so that everything works right - may be
      // some memory leaks in here, but screw it

       oldObjHere = (objectHere *)(invNode->entityPtr);

       invNode->entityPtr = copyObjHere(oldObjHere, FALSE);
       ((objectHere *)(invNode->entityPtr))->Next = NULL;

       objHere = (objectHere *)(invNode->entityPtr);

       if (!objHere->objectPtr)
       {
         strcpy(tempStrn, "'(object not in this .OBJ file)' taken.\n");
       }
       else
       {
         sprintf(tempStrn, "'%s&n' taken.\n",
                 objHere->objectPtr->objShortName);
       }

       if (deleteOriginal)
       {
         deleteObjHereinList(
&(((dikuRoom *)(matchingNode->entityOwner))->objectHead), oldObjHere, TRUE);
// used to be invNode instead of matchingNode

         madeChanges = TRUE;
         createPrompt();

         deleteMasterKeywordList(room->masterListHead);
         room->masterListHead = createMasterKeywordList(room);

         deleteEditableList(room->editableListHead);
         room->editableListHead = createEditableList(room);
       }

       if (deleteMatching) delete matchingNode;

       displayColorString(tempStrn);

       return;

    case ENTITY_MOB :

       if (!mob) mob = (mobHere *)(matchingNode->entityPtr);

       if (mob->riding || mob->riddenBy || mob->following)
       {
         _outtext(
"Sorry, but you cannot pick up mobs being ridden, riding, or following\n"
"another mob.\n");

         return;
       }

       if (mob->mobPtr && mob->mobPtr->shopPtr && !deleteOriginal)
       {
         _outtext(
"Sorry, but you cannot pick up copies of mobs with shops.\n");

         return;
       }

      // add new inventory node to list

       invNode = new editableListNode;
       if (!invNode)
       {
         _outtext("ERROR: takeEntity() - error alloc'ing invNode.\n");

         if (deleteMatching) delete matchingNode;
         return;
       }

       memcpy(invNode, matchingNode, sizeof(editableListNode));

       if (matchingNode->keywordListHead)
       {
         invNode->keywordListHead =
           copyStringNodes(matchingNode->keywordListHead);
       }
       else matchingNode->keywordListHead = NULL;

       invNode->Next = NULL;

       if (!inventory) inventory = invNode;
       else
       {
         invPrevNode = inventory;
         while (invPrevNode->Next)
         {
           invPrevNode = invPrevNode->Next;
         }

         invPrevNode->Next = invNode;
       }

      // copy entire mobHere

       oldMobHere = (mobHere *)(invNode->entityPtr);

       invNode->entityPtr = copyMobHere(oldMobHere, FALSE);
       ((mobHere *)(invNode->entityPtr))->Next = NULL;

       if (oldMobHere->mobPtr)
       {
         sprintf(tempStrn, "'%s&n' taken.\n",
                 ((mobHere *)(invNode->entityPtr))->mobPtr->mobShortName);
       }
       else
       {
         sprintf(tempStrn, "'(mob not in this .MOB file)' taken.\n");
       }

       if (deleteOriginal)
       {
         deleteMobHereinList(
&(((dikuRoom *)(matchingNode->entityOwner))->mobHead), oldMobHere, TRUE);
// used to be invNode instead of matchingNode

         madeChanges = TRUE;
         createPrompt();

         deleteMasterKeywordList(room->masterListHead);
         room->masterListHead = createMasterKeywordList(room);

         deleteEditableList(room->editableListHead);
         room->editableListHead = createEditableList(room);
       }

       if (deleteMatching) delete matchingNode;

//       _outtext("Mob taken.\n");
       displayColorString(tempStrn);

       return;


    default : sprintf(tempStrn, "Unrecognized value - %d.\n", whatMatched);
              _outtext(tempStrn);  return;
  }
}


//
// dropEntity : Copies an entity from user's inventory to room pointed to by
//              room
//
//           *args : string entered by user
//           *room : room user is dropping object into
//  deleteOriginal : if TRUE, deletes original from inventory
//

void dropEntity(char *args, dikuRoom *room, const char deleteOriginal)
{
  char tempStrn[768], whatMatched = NO_MATCH;
  editableListNode *matchingNode, *invNode, *anotherInvNode;
  objectHere *prevObjHere, *objHere = NULL;
  mobHere *prevMobHere, *mob = NULL;
  long vnum = -1, i;


  if (strnumer(args))
  {
    vnum = atoi(args);

    invNode = inventory;
    while (invNode)
    {
      switch (invNode->entityType)
      {
        case ENTITY_OBJECT :
          if (((objectHere *)(invNode->entityPtr))->objectNumb == vnum)
          {
            objHere = (objectHere *)(invNode->entityPtr);
            whatMatched = ENTITY_OBJECT;
            matchingNode = invNode;
          }

          break;

        case ENTITY_MOB    :
          if (((mobHere *)(invNode->entityPtr))->mobNumb == vnum)
          {
            mob = (mobHere *)(invNode->entityPtr);
            whatMatched = ENTITY_MOB;
            matchingNode = invNode;
          }

          if (mob->mobPtr && mob->mobPtr->shopPtr && !deleteOriginal)
          {
            _outtext(
"Sorry, but you cannot drop copies of mobs with shops.\n");

            return;
          }

          break;
      }

      if (whatMatched != NO_MATCH) break;

      invNode = invNode->Next;
    }
  }

  if (!strcmp(args, "ALL"))
  {
    invNode = anotherInvNode = copyEditableList(inventory);

    while (invNode)
    {
      switch (invNode->entityType)
      {
        case ENTITY_OBJECT :
          itoa(((objectHere *)(invNode->entityPtr))->objectNumb, tempStrn, 10);
          break;

        case ENTITY_MOB :
          itoa(((mobHere *)(invNode->entityPtr))->mobNumb, tempStrn, 10);
          break;

        default :
          sprintf(tempStrn, "error: unrecognized/unsupported entityType (#%d)\n",
                  invNode->entityType);
          _outtext(tempStrn);
          continue;
      }

      dropEntity(tempStrn, room, deleteOriginal);

      invNode = invNode->Next;
    }

    deleteEditableList(anotherInvNode);

    return;
  }

  if (whatMatched == NO_MATCH)
  {
    checkEditableList(args, inventory, &whatMatched, &matchingNode, 1);
  }

  switch (whatMatched)
  {
    case NO_MATCH : _outtext("No match.\n");  return;

    case ENTITY_OBJ_ED :
       _outtext("Sorry, but object extra descs aren't droppable.\n");
       return;

    case ENTITY_R_EDESC :
       _outtext("Sorry, but room extra descs aren't droppable.\n");
       return;

    case ENTITY_OBJECT :

      // copy objHere in matchingNode so everything works

       objHere = copyObjHere((objectHere *)(matchingNode->entityPtr), FALSE);
       objHere->Next = NULL;

      // add object to room's object list

       if (!room->objectHead) room->objectHead = objHere;
       else
       {
         prevObjHere = room->objectHead;

         while (prevObjHere->Next)
         {
           prevObjHere = prevObjHere->Next;
         }

         prevObjHere->Next = objHere;
       }

       madeChanges = TRUE;

       addEntity(ENTITY_OBJECT, objHere->objectNumb);

       loadAllinObjHereList(objHere->objInside);

       numbObjs++;
       createPrompt();

       deleteMasterKeywordList(room->masterListHead);
       room->masterListHead = createMasterKeywordList(room);

       deleteEditableList(room->editableListHead);
       room->editableListHead = createEditableList(room);

       if (((objectHere *)(matchingNode->entityPtr))->objectPtr)
       {
         sprintf(tempStrn, "'%s&n' dropped.\n",
                 ((objectHere *)(matchingNode->entityPtr))->objectPtr->
                  objShortName);
       }
       else
       {
         strcpy(tempStrn, "'(object not in this .OBJ file)' dropped.\n");
       }

       if (deleteOriginal)
       {
         deleteEditableinList(&inventory, matchingNode);
       }

       displayColorString(tempStrn);

       return;

    case ENTITY_MOB :

      // copy mob info so it's all good

       mob = copyMobHere((mobHere *)(matchingNode->entityPtr), FALSE);
       mob->Next = NULL;

      // oh no!  a mem leak on error!  -shouldn't be anymore

       if (mob->mobPtr && mob->mobPtr->shopPtr && !deleteOriginal)
       {
         _outtext(
"Sorry, but you cannot drop copies of mobs with shops.\n");

         deleteMobHere(mob, FALSE);  // should work

         return;
       }

      // add mob to room's mob list

       if (!room->mobHead) room->mobHead = mob;
       else
       {
         prevMobHere = room->mobHead;

         while (prevMobHere->Next)
         {
           prevMobHere = prevMobHere->Next;
         }

         prevMobHere->Next = mob;
       }

       madeChanges = TRUE;

       addEntity(ENTITY_MOB, mob->mobNumb);

       loadAllinObjHereList(mob->objectHead);
       for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
         loadAllinObjHereList(mob->equipment[i]);

       numbMobs++;
       createPrompt();

       deleteMasterKeywordList(room->masterListHead);
       room->masterListHead = createMasterKeywordList(room);

       deleteEditableList(room->editableListHead);
       room->editableListHead = createEditableList(room);

       if (((mobHere *)(matchingNode->entityPtr))->mobPtr)
       {
         sprintf(tempStrn, "'%s&n' dropped.\n",
                 ((mobHere *)(matchingNode->entityPtr))->mobPtr->
                  mobShortName);
       }
       else
       {
         strcpy(tempStrn, "'(mob not in this .MOB file)' dropped.\n");
       }

       if (deleteOriginal)
       {
         deleteEditableinList(&inventory, matchingNode);
       }

       displayColorString(tempStrn);

       return;


    default : sprintf(tempStrn, "Unrecognized value - %d.\n", whatMatched);
              _outtext(tempStrn);  return;
  }
}
