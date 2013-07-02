//
//  File: unequip.cpp    originally part of dikuEdit
//
//  Usage: functions for removing objects from a mob's equipment list
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "../misc/editable.h"
#include "../obj/objhere.h"
#include "../mob/mobhere.h"

extern char madeChanges;

//
// unequipMobSpecific
//

void unequipMobSpecific(const char *mobStrn, const char *objStrn,
                        dikuRoom *room)
{
  char whatMatched;
  editableListNode *mobNode;

  objectHere *objHere = NULL;//, *copyObj;
  mobHere *mobH;
  char tempStrn[768];
  long i, where = -1;

  checkEditableList(mobStrn, room->editableListHead, &whatMatched,
                    &mobNode, 1);

  switch (whatMatched)
  {
    case NO_MATCH : _outtext("\nNo match on first keyword.\n\n");  return;

    case ENTITY_OBJ_ED :
       _outtext(
"\nStrangely, object edescs cannot have any equipment to unequip.\n\n");
       return;

    case ENTITY_R_EDESC :
       _outtext(
"\nStrangely, room edescs cannot have any equipment to unequip.\n\n");
       return;

    case ENTITY_OBJECT :
       _outtext(
"\nStrangely, objects cannot have any equipment to unequip.\n\n");
       return;

    case ENTITY_MOB :
       mobH = ((mobHere *)(mobNode->entityPtr));

       if (!mobEquippingSomething(mobH))
       {
         _outtext("\nThat mob is not equipping anything.\n\n");
         return;
       }

      // try to find equipment that matches second keyword

       for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
       {
         objHere = mobH->equipment[i];

         if (objHere && objHere->objectPtr &&
             scanKeyword(objStrn, objHere->objectPtr->keywordListHead))
         {
           where = i;
           break;
         }
       }

       if (where == -1)
       {
         _outtext(
"\nNone of the objects equipped by the mob match keyword specified.\n\n");
         return;
       }

//       copyObj = copyObjHere(objHere, FALSE);

       mobH->equipment[where] = NULL;
       addObjHeretoList(&(((mobHere *)(mobNode->entityPtr))->objectHead),
                        objHere);

       sprintf(tempStrn, "\n'%s&n' unequipped on '%s'.\n\n",
               objHere->objectPtr->objShortName, mobH->mobPtr->mobShortName);
       displayColorString(tempStrn);

/*
       addObjHeretoList(&(((mobHere *)(mobNode->entityPtr))->objectHead),
                        copyObj);

       deleteObjHere(((mobHere *)(mobNode->entityPtr))->equipment[where],
                     FALSE);
       mobH->equipment[where] = NULL;
*/

       madeChanges = TRUE;

       return;

    default : sprintf(tempStrn, "\nUnrecognized value - %d.\n\n", whatMatched);
              _outtext(tempStrn);  return;
  }
}


//
// unequipMob : takes crap from a mob's equipment list and unequips them
//

void unequipMob(const char *strn, dikuRoom *room)
{
  char tempStrn[768], whatMatched, objName[512], mobName[512];
  editableListNode *mobNode;
  objectHere *objHere;//, *copyObj;
  long i, where = -1;


 // if there are two arguments or more, unequip specific piece

  if (numbArgs(strn) >= 2)
  {
    unequipMobSpecific(getArg(strn, 1, mobName), getArg(strn, 2, objName),
                       room);

    return;
  }

  checkEditableList(strn, room->editableListHead, &whatMatched,
                    &mobNode, 1);

  switch (whatMatched)
  {
    case NO_MATCH : _outtext("\nNo match.\n\n");  return;

    case ENTITY_MOB :

      // make sure mob has some equipment

       if (!mobEquippingSomething((mobHere *)(mobNode->entityPtr)))
       {
         _outtext("\nThat mob is not equipping anything.\n\n");
         return;
       }

       _outtext("\n");

       for (i = WEAR_LOW; i <= WEAR_TRUEHIGH; i++)
       {
         objHere = ((mobHere *)(mobNode->entityPtr))->equipment[i];
         if (!objHere) continue;

//         copyObj = copyObjHere(objHere, FALSE);

         if (objHere->objectPtr)
         {
           strcpy(objName, objHere->objectPtr->objShortName);
         }
         else
         {
           strcpy(objName, "(object type not in this .OBJ file)");
         }

         if (((mobHere *)(mobNode->entityPtr))->mobPtr)
         {
           strcpy(mobName, ((mobHere *)(mobNode->entityPtr))->mobPtr->
                            mobShortName);
         }
         else
         {
           strcpy(mobName, "(mob type in this .MOB file)");
         }

         addObjHeretoList(&((mobHere *)(mobNode->entityPtr))->objectHead,
                          objHere);
//                          copyObj);

//         deleteObjHere(objHere, FALSE);
         ((mobHere *)(mobNode->entityPtr))->equipment[i] = NULL;

         sprintf(tempStrn, "'%s&n' unequipped on '%s&n'.\n",
                 objName, mobName);

         displayColorString(tempStrn);
       }

       _outtext("\n");

       madeChanges = TRUE;

       return;

    default : _outtext("\nYou can't unequip anything on that.\n\n");  return;
  }
}
