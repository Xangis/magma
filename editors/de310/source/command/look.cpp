//
//  File: look.cpp       originally part of dikuEdit
//
//  Usage: functions related to looking at rooms, maps, obj contents..
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"

#include "../obj/objhere.h"
#include "../misc/master.h"
#include "../misc/editable.h"

extern dikuRoom *currentRoom;

const char *sector_symbol[NUMB_SECT_TYPES] =
{
  "^",                          /* * larger towns */
  "+",                          /* * roads */
  ".",                          /* * plains/fields */
  "*",                          /* * forest */
  "^",                          /* * hills */
  "M",                          /* * mountains */
  "r",                          /* * water shallow */
  " ",                          /* * water boat */
  "",                           /* * noground */
  "",                           /* * underwater */
  "",                           /* * underwater ground */
  " ",                          /* * fire plane */
  " ",                          /* * water ship */
  ".",                          /* * UD wild */
  "^",                          /* * UD city */
  ".",                          /* * UD inside */
  " ",                          /* * UD water */
  " ",                          /* * UD noswim */
  "",                           /* * UD noground */
  "",                           /* * air plane */
  "",                           /* * water plane */
  " ",                          /* * earth plane */
  "",                           /* * etheral plane */
  "",                           /* * astral plane */
  ".",                          /* desert */
  ".",                          /* arctic tundra */
  "*",                          /* swamp */
  "M",                          /* UD mountains */
  " ",                          /* UD slime */
  ",",                          /* UD low ceilings */
  " ",                          /* UD liquid mithril */
  ".",                          /* UD mushroom forest */
  " ",                          /* Castle Wall */
  "#",                          /* Castle Gate */
  "."                           /* Castle Itself */
};

const char *color_symbol[NUMB_SECT_TYPES] =
{
  "=wl",
  "+L",
  "+g",
  "=gl",
  "+y",
  "+y",
  "=cl",
  "=bB",
  "+w",
  "+w",
  "+w",
  "=rR",
  "=bB",
  "=mL",
  "=wL",
  "+m",
  "=bB",
  "=bB",
  "+w",
  "+w",
  "=lL",
  "+w",
  "+w",
  "+w",
  "=yY",
  "=yY",
  "=mL",
  "+L",
  "=gG",
  "+M",
  "=wW",
  "+M",
  "=wW",
  "=wL",
  "+g"
};


//
// isUDmapRoom
//

inline int isUDmapRoom(const ulong numb)
{
  return ((numb >= 1000000) && (numb <= 1099999));
}


//
// displayMap - mostly stolen from map.c on Duris, modified a tad for DE
//

void displayMap(const dikuRoom *room)
{
  int x, x2, n, where, what, holder = -1, temp;
  int x_low = 0;		/* Mapper sizes */
  int x_high = 0;
  int n_low = 0;
  int n_high = 0;
  int mapx, mapy;
  char *buf;
  ulong roomNumb = room->roomNumber;
  dikuRoom *temp_room;

  buf = new char[16384];
  if (!buf)
  {
    _outtext("\n\ndisplayMap(): couldn't alloc buf\n\n");
    return;
  }

  if (roomNumb > 200000) {
	mapx = 200;
	mapy = 200;
  } else {
	mapx = 100;
	mapy = 100;
  }

  x_high = ((getScreenHeight() >> 1) - 5) * mapx;
  x_low = -x_high;

  n_high = (getScreenWidth() >> 1) - 2;
  n_low = -n_high;

  if ((n_high < 0) || (x_high < 0))
  {
    _outtext("\nYour screen settings are too small to display the map.\n\n");
    return;
  }

 /* return, so map isnt on prompt line */

  _outtext("\n\n");

  for (x = x_low; x <= x_high; x += mapx) 
  {
    buf[0] = '\0';

    /* north/south map wrap */
    if ((((roomNumb / 10000) * 10000) <
    	 (((roomNumb + x) / 10000) * 10000)) &&
	     !isUDmapRoom(roomNumb) && (roomNumb < 200000)) 
    {
      x2 = x + 20000;
      if ((x2 + roomNumb) > 199999)
	    x2 -= 90000;
    } 
    else 
    if ((((roomNumb / 10000) * 10000) >
	     (((roomNumb + x) / 10000) * 10000)) &&
	    !isUDmapRoom(roomNumb) && (roomNumb < 200000)) 
    {
      x2 = x - 20000;
      if ((x2 + roomNumb) < 110000)
	    x2 += 90000;
    } 
    else
      x2 = x;

    /* send a space, each line */
//    _outtext("           ");
    _outtext("  ");

    for (n = n_low; n <= n_high; n++) 
    {
      /* East/west wrap */
      temp = (roomNumb + x2) - (((roomNumb + x2) / 100) * 100);

      if ((temp + n < 0) && !isUDmapRoom(roomNumb) && (roomNumb < 200000)) 
      {
        where = (roomNumb + x2 - 9901 + n);
        if ((where / 10000) == 13 || (where / 10000) == 16 || (where / 10000) == 19)
          where += 30000;
      } 
      else 
      if ((temp + n > 99) && !isUDmapRoom(roomNumb) && (roomNumb < 200000)) 
      {
        where = (roomNumb + x2 + 9901 + n);
        if ((where / 10000) == 14 || (where / 10000) == 17 || (where / 10000) == 20)
          where -= 30000;
      } 
      else where = (roomNumb + x2 + n);

      temp_room = findRoom(where);

      if (!temp_room && where < 1000000)
	    what = 12;		/* ocean */
      else if (!temp_room && where >= 200000)
            what = 12;
      else if (!temp_room && where >= 1000000)
	    what = 21;		/* rock */
      else
        what = temp_room->sectorType;

      if (what < 0) what = 0;
      if (what > 34) what = 34;

      if (n == 0 && x == 0) {	/* you */
	    sprintf(buf + strlen(buf), "&n&+W@");
	    holder = -1;
      } 
#if 0
      else if ((whats_in_maproom(ch, where) == CONTAINS_SHIP)) {
	    sprintf(buf, "&n&+WS");
	    holder = -1;
      } else if ((whats_in_maproom(ch, where) == CONTAINS_DRAGON)) {
	    sprintf(buf, "&n&=LRD");
	    holder = -1;
      } else if ((whats_in_maproom(ch, where) == CONTAINS_MOB)) {
	    sprintf(buf, "&n&=LBM");
	    holder = -1;
      } else if ((whats_in_maproom(ch, where) == CONTAINS_PC)) {
	    sprintf(buf, "&n&=LWP");
	    holder = -1;
      } else if (ch->specials.z_cord < 0 && what != 12) {	/* underwater */
	    sprintf(buf, "&=lL ");	/* showing black if not water */
	    holder = -1;
      } 
#endif
      else if ((holder != what) || (n == n_low)) 
      {
        sprintf(buf + strlen(buf), "&n&%s%s", color_symbol[what], sector_symbol[what]);
        holder = what;
      } 
      else 
      {
        sprintf(buf + strlen(buf), "%s", sector_symbol[what]);
        holder = what;
      }

//      displayColorString(buf);

/*
      if (n == n_high) 
      {
        displayColorString("&n\n");
      }
*/
    }

    strcat(buf, "&n\n");

    displayColorString(buf);
  }

  delete[] buf;
}


//
// displayContainerContents
//

void displayContainerContents(objectHere *objInside, const char indent)
{
  char objName[512], strn[512], strn2[512];


  while (objInside)
  {
    if (objInside->objectPtr)
    {
      strcpy(objName, objInside->objectPtr->objShortName);
    }
    else strcpy(objName, "(object type not in this .OBJ file)");

    if (indent) sprintf(strn, "  %s&n", objName);
    else sprintf(strn, "%s&n", objName);

    if ((getShowObjVnumVal()) || (!objInside->objectPtr))
    {
      sprintf(strn2, " (#%u)", objInside->objectNumb);
      strcat(strn, strn2);
    }

    sprintf(strn2, " &+c(%u%%)&n", objInside->randomChance);
    strcat(strn, strn2);

    if (objInside->objectPtr)
    {
      if (objInside->objectPtr->extraBits.objExtraBits.magic)
      {
        strcat(strn, " (&+bmagic&n)");
      }

      if (objInside->objectPtr->extraBits.objExtraBits.invisible)
      {
        strcat(strn, " (&+Linvis&n)");
      }

      if (objInside->objectPtr->extraBits.objExtraBits.secret)
      {
        strcat(strn, " (&+Lhidden&n)");
      }

      if (objInside->objectPtr->extraBits.objExtraBits.buried)
      {
        strcat(strn, " (&+Lburied&n)");
      }
    }

    strcat(strn, "\n");

    displayColorString(strn);

    objInside = objInside->Next;
  }
}


//
// lookStuff : Common stuff between "LOOK" and "L"
//
//   *args : string as entered by user
//

void lookStuff(char *args)
{
  char objName[512], strn[512], strn2[256];

  stringNode *strnNode = NULL;

  objectHere *objHere, *objInside = NULL, /**mobEquip = NULL,*/ *mobObj = NULL;
  mobHere *mob = NULL;

  char whatMatched = NO_MATCH, deleteMatching = FALSE, mobEquip = FALSE;
  masterKeywordListNode *matchingNode = NULL;

  ulong blah;
  long vnum = -1;


 // remove "at " if it exists (i.e. "look at blah")

  if (strleft(args, "AT "))
  {
    deleteChar(args, 0);
    deleteChar(args, 0);
    deleteChar(args, 0);
  }

 // check to see if keyword matches any of the keywords for the room's
 // stuff - keywords for extra descs, that is

  if (strnumer(args))
  {
    vnum = atoi(args);

    objHere = findObjHere(vnum, currentRoom, &blah, TRUE, TRUE);

    if (!objHere)
    {
      mob = findMobHere(vnum, currentRoom, &blah, TRUE);
      if (mob)
      {
        whatMatched = ENTITY_MOB;

       // this was a small leak, but should now be taken care of properly

        matchingNode = new masterKeywordListNode;
        if (!matchingNode)
        {
          _outtext("\nMem error in lookStuff\n\n");
          return;
        }

        memset(matchingNode, 0, sizeof(masterKeywordListNode));

        if (mob->mobPtr)
        {
          matchingNode->keywordListHead =
             copyStringNodes(mob->mobPtr->keywordListHead);
          strnNode = mob->mobPtr->mobDescHead;
        }

        matchingNode->entityPtr = mob;
        matchingNode->entityType = ENTITY_MOB;
//        matchingNode->entityOwner =    not needed
        matchingNode->Next = NULL;

        deleteMatching = TRUE;
      }
    }
    else
    {
      whatMatched = ENTITY_OBJECT;

     // used to be a leak

      matchingNode = new masterKeywordListNode;
      if (!matchingNode)
      {
        _outtext("\nMem error in lookStuff\n\n");
        return;
      }

      memset(matchingNode, 0, sizeof(masterKeywordListNode));

      if (objHere->objectPtr) matchingNode->keywordListHead =
                       copyStringNodes(objHere->objectPtr->keywordListHead);

      matchingNode->entityPtr = objHere;
      matchingNode->entityType = ENTITY_OBJECT;
//      matchingNode->entityOwner =    not needed
      matchingNode->Next = NULL;

      deleteMatching = TRUE;
    }
  }

  if (whatMatched == NO_MATCH)
  {
    strnNode = checkMasterKeywordList(args, currentRoom->masterListHead,
                                      &whatMatched, &matchingNode);
  }

 // now, do stuff based on the value of whatMatched

  if (whatMatched == ENTITY_OBJECT)
  {
   // check for objects inside of this object - if there are any, set
   // objInside equal to the head of the list

    objHere = (objectHere *)(matchingNode->entityPtr);

    objInside = objHere->objInside;
  }
  else
  if (whatMatched == ENTITY_MOB)
  {
    mob = (mobHere *)(matchingNode->entityPtr);

//    mobEquip = mob->equipHead;
    mobEquip = mobEquippingSomething(mob);
    mobObj = mob->objectHead;
  }

  if (strnNode)
  {
    _outtext("\n");
    displayStringNodes(strnNode);

    if (whatMatched == ENTITY_MOB)  // display a bit of handy info on the mob
    {
      displayMobMisc(mob);
    }

    _outtext("\n");

    if (!objInside && !mobEquip && !mobObj)
    {
      if (deleteMatching) delete matchingNode;
      return;
    }
  }
  else
  if (whatMatched == ENTITY_OBJECT)
  {
    _outtext("\nThat object/object extra desc has no description.\n\n");

    if (!objInside)
    {
      if (deleteMatching) delete matchingNode;
      return;
    }
  }
  else
  if (whatMatched == ENTITY_MOB)
  {
    _outtext("\nThat mob has no description.\n");

    displayMobMisc(mob);
    _outtext("\n");

//    if ((!mobEquip) && (!mobObj)) return;
  }
  else
  if (whatMatched == ENTITY_R_EDESC)
  {
    _outtext("\nThat room extra desc has no description (go figure).\n\n");

//    if (matchingNode) delete matchingNode;
    return;
  }
  else _outtext("\nLook at what?\n\n");


  if (objInside)
  {
    displayColorString("\n&+YObject contains:&n\n");

    displayContainerContents(objInside, TRUE);

    _outtext("\n");

    if (deleteMatching) delete matchingNode;
    return;
  }

  if (mobEquip)
  {
    displayMobEquip(mob);

    if (!mobObj)
    {
      if (deleteMatching) delete matchingNode;
      return;
    }
  }

  if (mobObj)
  {
    if (!mobEquip) _outtext("\n");

    displayColorString("&+YCarrying:&n\n");

    while (mobObj)
    {
      if (mobObj->objectPtr)
      {
        strcpy(objName, mobObj->objectPtr->objShortName);
      }
      else strcpy(objName, "(object type not in this .OBJ file)");

      sprintf(strn, "  %s&n", objName);

      if (getShowObjVnumVal() || !mobObj->objectPtr)
      {
        sprintf(strn2, " (#%u)", mobObj->objectNumb);
        strcat(strn, strn2);
      }

      sprintf(strn2, " &+c(%u%%)&n", mobObj->randomChance);
      strcat(strn, strn2);

      strcat(strn, "\n");

      displayColorString(strn);

      mobObj = mobObj->Next;
    }

    _outtext("\n");
  }

  if (deleteMatching) delete matchingNode;
}


//
// lookExit : Looks at an exit.  Woo
//
//  *exitNode : exit to "look" at
//

void lookExit(const roomExit *exitNode)
{
  char zoneState, worldType, strn[1024], strn2[512], strn3[512];
  dikuObject *keyObj;
  stringNode *strnNode;


  if (exitNode)
  {
    zoneState = getZoneDoorState(exitNode);
    worldType = getWorldDoorType(exitNode);

    if (exitNode->exitDescHead)
    {
      _outtext("\n");
      displayStringNodes(exitNode->exitDescHead);
      if (!worldType) _outtext("\n");
    }
    else
    {
      _outtext("\nNo description for that exit.\n");
      if (!worldType) _outtext("\n");
    }

   // first, check door type

    if (worldType)
    {
      strcpy(strn, "[");

      if ((worldType & 3) == 3)
      {
        strcat(strn, "&+Rclosable&+L/&+Rlockable&+L/&+Runpickable&+L/");
      }
      else
      if (worldType & 1)
      {
        strcat(strn, "&n&+cclosable&+L/");
      }
      else
      if (worldType & 2)
      {
        strcat(strn, "&+Cclosable&+L/&+Clockable&+L/");
      }

      if (worldType & 4)
      {
        strcat(strn, "&+Lsecret/");
      }

      if (worldType & 8)
      {
        strcat(strn, "&n&+rblocked&+L/");
      }

      strn[strlen(strn) - 1] = '\0';
      strcat(strn, "&n | ");

      if (zoneState & 1)
      {
        strcat(strn, "&n&+cclosed&+L/");
      }
      else
      if (zoneState & 2)
      {
        strcat(strn, "&+Cclosed&+L/&+Clocked&+L/");
      }
      else strcat(strn, "&+Wopen&+L/");

      if (zoneState & 4)
      {
        strcat(strn, "&+Lsecret&+L/");
      }

      if (zoneState & 8)
      {
        strcat(strn, "&n&+rblocked&+L/");
      }

      strn[strlen(strn) - 1] = '\0';


      strcat(strn, "&n]\n\n");

      if (worldType & 2)  // lockable door
      {
        if (exitNode->keyNumb == -1)
        {
          strcat(strn, "No key can unlock this door (keyNumb = -1).\n\n");
        }
        else
        if (exitNode->keyNumb == -2)
        {
          strnNode = exitNode->keywordListHead;

          if (!strnNode)
          {
            strcat(strn,
            "ERROR: Door key number is -2 (magic word door), but there are no exit keywords.\n\n");
          }
          else
          {
           // magic word to open door is last keyword

            while (strnNode->Next)
            {
              strnNode = strnNode->Next;
            }

            sprintf(strn2,
            "Door is magical - the magic word \"%s\" will open it.\n\n",
                    strnNode->string);

            strcat(strn, strn2);
          }
        }
        else
        {
          keyObj = findObj(exitNode->keyNumb);

          if (!keyObj && getVnumCheckVal())
          {
            sprintf(strn2, "Key object #%d not found.\n\n", exitNode->keyNumb);
            strcat(strn, strn2);
          }
          else
          {
            if (!keyObj) strcpy(strn3, "(object not in this zone)");
            else strcpy(strn3, keyObj->objShortName);

            sprintf(strn2, "Door requires obj #%d \"%s\"&n to unlock it.\n\n",
                    exitNode->keyNumb, strn3);

            strcat(strn, strn2);
          }
        }
      }

      displayColorString(strn);
    }
  }
  else
  {
    _outtext("\nNo exit in that direction!\n\n");
  }
}


//
// lookInObj : Displays the contents of an object, if it has any
//
//   *strn : string as entered by user - contains keyword
// strnPos : position in strn where keyword starts
//

void lookInObj(const char *args)
{
  char strn[512];

  objectHere *objHere, *objInside;

  masterKeywordListNode *matchingNode;
  char whatMatched;

  stringNode *strnNode;


  strnNode = checkMasterKeywordList(args, currentRoom->masterListHead,
                                    &whatMatched, &matchingNode);

  if (whatMatched == NO_MATCH)
  {
    _outtext("\nNo matching item found.\n\n");

    return;
  }

  if (whatMatched != ENTITY_OBJECT)
  {
    _outtext("\n"
"You can't look inside that (the match for the keyword isn't an object).\n\n");

    return;
  }

 // check for objects inside of the currentRoom that match the keyword

  objHere = (objectHere *)(matchingNode->entityPtr);

 // whoopsie, object isn't a container ..  but that shouldn't matter

  if (!objHere->objectPtr)
  {
    _outtext(
"\nInformation on object to look inside not in this zone file.\n\n");

//    return;
  }
  else
  if (objHere->objectPtr->objType != ITEM_CONTAINER)
  {
    _outtext("\n");
    displayColorString(objHere->objectPtr->objShortName);
    displayColorString("&n is not a container.\n\n");

    return;
  }

  objInside = objHere->objInside;

 // cycle through the objInside list until we're done

  if (objInside)
  {
    _outtext("\n");

    if (objHere->objectPtr)
      displayColorString(objHere->objectPtr->objShortName);
    else
    {
      sprintf(strn, "(object not in this .OBJ file) (#%u)",
              objHere->objectNumb);
      _outtext(strn);
    }

    displayColorString("&n contains:\n");

    displayContainerContents(objInside, FALSE);

    _outtext("\n");
  }
  else
  {

   // objInside is NULL - container is empty

    _outtext("\n");

    if (objHere->objectPtr)
      displayColorString(objHere->objectPtr->objShortName);
    else
    {
      sprintf(strn, "(object not in this .OBJ file) (#%u)",
              objHere->objectNumb);
      _outtext(strn);
    }

    displayColorString("&n is empty.\n\n");
  }
}


//
// look : Checks args to look command for certain stuff
//
//  *args : arguments to 'look' command
//

void look(char *args)
{
  char val;

  if (!strlen(args))
  {
    displayRoom(currentRoom);

    return;
  }

  val = getDirfromKeyword(args);
  if (val != NO_EXIT)
  {
    lookExit(currentRoom->exits[val]);
    return;
  }

  if (strleft(args, "IN "))
  {
    deleteChar(args, 0);  // this has gotta be in here for some fucked up reason
    deleteChar(args, 0);  // that is beyond my comprehension - if it's extremely
    deleteChar(args, 0);  // obvious and I'm just a dumbass, blow me

    lookInObj(args);
    return;
  }

  lookStuff(args);  // this function could be removed, but I love it far too
                    // much
}
