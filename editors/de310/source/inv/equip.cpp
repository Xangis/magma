//
//  File: equip.cpp      originally part of dikuEdit
//
//  Usage: functions for (un)equipping mobs with objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "../misc/editable.h"

extern char madeChanges;

//
// equipEquiponMob : returns TRUE if successful, FALSE otherwise
//

uchar equipEquiponMob(mobHere *mob, objectHere *obj, const char echoIt)
{
  char tempStrn[512], ch;


  if (!obj->objectPtr) return FALSE;

  ch = getMobHereEquipSlot(mob, obj->objectPtr, -1);
  if (echoIt)
    displayColorString(getVerboseCanEquipStrn(ch, obj->objectPtr->objShortName,
                                              tempStrn));

 // if no error, add the shit

  if (ch <= WEAR_TRUEHIGH)
  {
//    mob->equipment[ch] = copyObjHere(obj, FALSE);
    mob->equipment[ch] = obj;
//    obj->Next = NULL;

    return TRUE;
  }

  return FALSE;
}


//
// equipMobSpecific
//

void equipMobSpecific(char *args, dikuRoom *room, const char echoIt /*= TRUE*/)
{
  char mobStrn[256], objStrn[256];
  char whatMatched;
  editableListNode *mobNode;

  mobHere *mobH;
  objectHere *objHere;
  long vnum;
  char tempStrn[768], deleteFromCarried = TRUE, deleteObjHonFail = FALSE;


  getArg(args, 1, mobStrn);
  getArg(args, 2, objStrn);

  checkEditableList(mobStrn, room->editableListHead, &whatMatched,
                    &mobNode, 1);

  switch (whatMatched)
  {
    case NO_MATCH :
       if (echoIt) _outtext("\nNo match on first keyword (mob name).\n\n");
       return;

    case ENTITY_OBJ_ED :
       if (echoIt) _outtext("\nStrangely, object edescs cannot equip anything.\n\n");
       return;

    case ENTITY_R_EDESC :
       if (echoIt) _outtext("\nStrangely, room edescs cannot equip anything.\n\n");
       return;

    case ENTITY_OBJECT :
       if (echoIt) _outtext("\nStrangely, objects cannot equip anything.\n\n");
       return;

    case ENTITY_MOB :
       mobH = ((mobHere *)(mobNode->entityPtr));

       if (!mobH->objectHead && !strnumer(objStrn))
       {
         if (echoIt) _outtext("\nThat mob is not carrying anything.\n\n");
         return;
       }

      // try to find object that matches second keyword

       objHere = mobH->objectHead;
       while (objHere)
       {
         if (objHere->objectPtr &&
             scanKeyword(objStrn, objHere->objectPtr->keywordListHead))
           break;

         objHere = objHere->Next;
       }

       if (!objHere)
       {
        // load up on the fly, baby

         if (strnumer(objStrn))
         {
           vnum = atoi(objStrn);

           if (!findObj(vnum))
           {
             if (echoIt)
               _outtext(
"\nCannot equip mob with objects not in this .OBJ file.\n\n");

             return;
           }

           objHere = createObjHere(vnum, TRUE);
           if (!objHere)
           {
             if (echoIt) _outtext("\nerror in equipMobSpecific(): out of mem\n\n");
             return;
           }

           deleteFromCarried = FALSE;
           deleteObjHonFail = TRUE;
         }
         else
         {
           if (echoIt)
             _outtext(
"\nNone of the objects carried by the mob match keyword specified.\n\n");
           return;
         }
       }

       if (echoIt) _outtext("\n");

       if (equipEquiponMob(((mobHere *)(mobNode->entityPtr)), objHere, echoIt))
       {
         if (deleteFromCarried)
         {
           removeObjHerefromList(&(((mobHere *)(mobNode->entityPtr))->
                                 objectHead), objHere);
         }

         madeChanges = TRUE;
       }
       else if (deleteObjHonFail) deleteObjHere(objHere, TRUE);

       if (echoIt) _outtext("\n");

       return;

    default : if (echoIt)
              {
                sprintf(tempStrn,
"\nerror in equipMobSpecific - unrecognized value (%d).\n\n", whatMatched);
                _outtext(tempStrn);
              }

              return;
  }
}


//
// equipMob : takes crap from a mob's carried list and equips them
//

void equipMob(char *strn, dikuRoom *room, const char echoIt)
{
  char tempStrn[768], whatMatched, mobName[512];
  editableListNode *matchingNode;
  objectHere *objHere, *objNext;


  if (numbArgs(strn) > 1)
  {
    equipMobSpecific(strn, room, echoIt);
    return;
  }

  checkEditableList(strn, room->editableListHead, &whatMatched,
                    &matchingNode, 1);

  switch (whatMatched)
  {
    case NO_MATCH :
       if (echoIt) _outtext("\nNo match.\n\n");
       return;

    case ENTITY_MOB :

      // make sure mob has stuff in its object list

       if ((!((mobHere *)(matchingNode->entityPtr))->objectHead))
       {
         if (echoIt)
           _outtext("\nThat mob has nothing in its inventory to equip.\n\n");
         return;
       }

       objHere = ((mobHere *)(matchingNode->entityPtr))->objectHead;

       if (echoIt) _outtext("\n");

       while (objHere)
       {
         objNext = objHere->Next;

         if (equipEquiponMob((mobHere *)(matchingNode->entityPtr), objHere, echoIt))
         {
           removeObjHerefromList(
&(((mobHere *)(matchingNode->entityPtr))->objectHead), objHere);

           madeChanges = TRUE;
         }

         objHere = objNext;
       }

       if (echoIt)
       {
         if (((mobHere *)(matchingNode->entityPtr))->mobPtr)
         {
           strcpy(mobName,
        ((mobHere *)(matchingNode->entityPtr))->mobPtr->mobShortName);
         }
         else strcpy(mobName, "(mob not in this .MOB file)");

         sprintf(tempStrn,
  "\nAll equippable objects carried by '%s&n' equipped.\n\n",
                 mobName);

         displayColorString(tempStrn);
       }

       return;

    default : if (echoIt)
                _outtext("\nYou can't equip anything on that.\n\n");  return;
  }
}
