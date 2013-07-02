//
//  File: put.cpp        originally part of dikuEdit
//
//  Usage: functions for giving/putting objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

extern char madeChanges;
extern ulong numbObjs;
extern editableListNode *inventory;

//
// putEntity : Put/gives an entity into/to another
//
//           *args : string entered by user
//
//           *room : room that target entity is in
//
//  deleteOriginal : if TRUE, original is deleted
//

void putEntity(char *args, dikuRoom *room, char deleteOriginal)
{
  char argone[256], argtwo[256], tempStrn[256], objName[256], mobName[256];
  editableListNode *containerNode, *containeeNode, *invNode, *anotherInvNode;
  objectHere *objHere = NULL, *newObj;  // may not need this
  mobHere *mob = NULL;
  char whatMatched = NO_MATCH;
  long vnum = -1;
  ulong blah;


  getArg(args, 1, argone);
  getArg(args, 2, argtwo);

  if ((argone[0] == '\0') || (argtwo[0] == '\0'))
  {
    _outtext(
"Not enough args - format is 'put <obj name/vnum> <target name/vnum>'.\n");
    return;
  }

 // check inventory for keyword/obj matching vnum in arg one

  if (strnumer(argone))
  {
    vnum = atoi(argone);

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
            containeeNode = invNode;
          }

          break;

        case ENTITY_MOB    :
          if (((mobHere *)(invNode->entityPtr))->mobNumb == vnum)
          {
            mob = (mobHere *)(invNode->entityPtr);
            whatMatched = ENTITY_MOB;
            containeeNode = invNode;
          }

          break;
      }

      if (whatMatched != NO_MATCH) break;

      invNode = invNode->Next;
    }

   // not found, instead load it on the fly and give it to the mob

/*
    objHere = new objectHere;  // mem leak, so blow me (shouldn't be anymore)
    if (!objHere) return;  // blah

    memset(objHere, 0, sizeof(objectHere));

    objHere->objectNumb = vnum;
    objHere->objectPtr = findObj(vnum);
*/
    objHere = createObjHere(vnum, FALSE);
    if (!objHere) return;

    whatMatched = ENTITY_OBJECT;

    containeeNode = new editableListNode;  // another mem leak - damn
    if (!containeeNode)
    {
      deleteObjHere(objHere, FALSE);
      return;
    }

    memset(containeeNode, 0, sizeof(editableListNode));

    containeeNode->entityPtr = objHere;
//    containeeNode->entityType = ENTITY_OBJECT;  not needed

    deleteOriginal = FALSE;
  }

  if (!strcmp(argone, "ALL"))
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
          sprintf(tempStrn, "error: unrecognized entityType (#%d)\n",
                  invNode->entityType);
          _outtext(tempStrn);
          continue;
      }

      strcat(tempStrn, " ");
      strcat(tempStrn, argtwo);

      putEntity(tempStrn, room, deleteOriginal);

      invNode = invNode->Next;
    }

    deleteEditableList(anotherInvNode);

    return;
  }

  if (whatMatched == NO_MATCH)
  {
    checkEditableList(argone, inventory, &whatMatched, &containeeNode, 1);
  }

  switch (whatMatched)
  {
    case NO_MATCH :
       _outtext(
"Cannot find anything in inventory that matches first keyword.\n");
       return;

    case ENTITY_OBJ_ED :
       _outtext("Sorry, but object extra descs aren't puttable.\n");
       return;

    case ENTITY_R_EDESC :
       _outtext("Sorry, but room extra descs aren't puttable.\n");
       return;

    case ENTITY_OBJECT :

      // attempt to find the container object (argtwo) - objects cannot
      // be specified by vnum, but mobs can

       whatMatched = NO_MATCH;

       if (strnumer(argtwo))
       {
         vnum = atoi(argtwo);

         mob = findMobHere(vnum, room, &blah, TRUE);
         if (mob)
         {
           whatMatched = ENTITY_MOB;

           containerNode = new editableListNode;  // this is a small memory leak
           if (!containerNode)
           {
             _outtext("Mem error in takeEnt\n");
             return;
           }

           memset(containerNode, 0, sizeof(editableListNode));

           if (mob->mobPtr) containerNode->keywordListHead =
                            copyStringNodes(mob->mobPtr->keywordListHead);

           containerNode->entityPtr = mob;
           containerNode->entityType = ENTITY_MOB;
//           matchingNode->entityOwner = room;
           containerNode->Next = NULL;
         }
       }

       if (whatMatched == NO_MATCH)
       {
         checkEditableList(argtwo, room->editableListHead,
                           &whatMatched, &containerNode, 1);
       }

       switch (whatMatched)
       {
         case NO_MATCH :
            _outtext(
"Cannot find any entities that match second keyword.\n");
            return;

         case ENTITY_OBJECT :
            if ((((objectHere *)(containerNode->entityPtr))->objectPtr->
                   objType) != ITEM_CONTAINER)
            {
              sprintf(tempStrn,
                      "Object found (%s), but it is not a container.\n",
                      ((objectHere *)(containerNode->entityPtr))->objectPtr->
                        objShortName);
              displayColorString(tempStrn);

              return;
            }

           // now, stick the object in the container

            newObj = copyObjHere((objectHere *)(containeeNode->entityPtr),
                                 FALSE);
            newObj->Next = NULL;

            objHere = ((objectHere *)(containerNode->entityPtr))->objInside;

            if (!objHere)
            {
              ((objectHere *)(containerNode->entityPtr))->objInside = newObj;
            }
            else
            {
              while (objHere->Next)
              {
                objHere = objHere->Next;
              }

/*
              if (!objHere)
              {
                _outtext("ERROR: putEntity() - no end node found\n");
                return;
              }
*/

              objHere->Next = newObj;
            }

            addEntity(ENTITY_OBJECT, newObj->objectNumb);
            loadAllinObjHereList(newObj->objInside);
            numbObjs++;
            createPrompt();
            madeChanges = TRUE;

            if (((objectHere *)(containeeNode->entityPtr))->objectPtr)
            {
              strcpy(objName,
         ((objectHere *)(containeeNode->entityPtr))->objectPtr->objShortName);
            }
            else
            {
              strcpy(objName, "(object not in this .OBJ file)");
            }

           // containerNode currently always points to a valid obj

            sprintf(tempStrn,
"'%s&n' put into '%s&n'.\n", objName,
((objectHere *)(containerNode->entityPtr))->objectPtr->objShortName);

            displayColorString(tempStrn);

            if (deleteOriginal)
            {
              deleteEditableinList(&inventory, containeeNode);
            }

            break;

         case ENTITY_MOB :

           // now, stick the object in the mob

            newObj = copyObjHere((objectHere *)(containeeNode->entityPtr),
                                 FALSE);
            newObj->Next = NULL;

            objHere = ((mobHere *)(containerNode->entityPtr))->objectHead;

            if (!objHere)
            {
              ((mobHere *)(containerNode->entityPtr))->objectHead = newObj;
            }
            else
            {
              while (objHere->Next)
              {
                objHere = objHere->Next;
              }

/*
              if (!objHere)
              {
                _outtext("ERROR: putEntity() - no end node found\n");
                return;
              }
*/

              objHere->Next = newObj;
            }

            addEntity(ENTITY_OBJECT, newObj->objectNumb);
            loadAllinObjHereList(newObj->objInside);
            numbObjs++;
            createPrompt();
            madeChanges = TRUE;

            if (((objectHere *)(containeeNode->entityPtr))->objectPtr)
            {
              strcpy(objName,
         ((objectHere *)(containeeNode->entityPtr))->objectPtr->objShortName);
            }
            else
            {
              strcpy(objName, "(object not in this .OBJ file)");
            }

            if (((mobHere *)(containerNode->entityPtr))->mobPtr)
            {
              strcpy(mobName,
         ((mobHere *)(containerNode->entityPtr))->mobPtr->mobShortName);
            }
            else
            {
              strcpy(mobName, "(mob not in this .MOB file)");
            }

            sprintf(tempStrn, "'%s&n' given to '%s&n'.\n",
                    objName, mobName);

            displayColorString(tempStrn);

            if (deleteOriginal)
            {
              deleteEditableinList(&inventory, containeeNode);
            }

            break;

         default :
            _outtext(
"You can't put anything in that (target is neither an object nor a mob).\n");
            return;
       }

       return;

    case ENTITY_MOB :
       _outtext("A mob cannot be placed inside anything.\n");
       return;


    default : sprintf(tempStrn, "Unrecognized value - %d.\n", whatMatched);
              _outtext(tempStrn);  return;
  }
}
