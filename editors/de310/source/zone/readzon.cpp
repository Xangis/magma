//
//  File: readzon.cpp    originally part of dikuEdit
//
//  Usage: functions for reading zone info from the .ZON file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"

#include "zone.h"
#include "readzon.h"
#include "../misc/loaded.h"
#include "../keys.h"
#include "../vardef.h"

using namespace std;

extern char madeChanges;
extern variable *varHead;
extern dikuZone zoneRec;
extern ulong numbObjs, numbMobs;
extern char readFromSubdirs;


//
// addLastObjLoaded : Adds a lastObjHereLoaded node to a list of em.
//
//    **lastHead : pointer to pointer to head of list
//
//       objNumb : obj type vnum being added
//       *objPtr : pointer to objectHere
//

void addLastObjLoaded(lastObjHereLoaded **lastHead, const ulong objNumb,
                      objectHere *objPtr)
{
  lastObjHereLoaded *lastNode = *lastHead, *newNode, *prevNode;
  char strn[256];


  if (!objPtr)
  {
    sprintf(strn, "\nError: added NULL objPtr to lastObjLoaded list (#%u)\n\n",
            objNumb);

    _outtext(strn);
    getkey();
  }

  while (lastNode)
  {
    if ((lastNode->objNumb) == objNumb)
    {
      lastNode->objHerePtr = objPtr;
      return;
    }

    prevNode = lastNode;
    lastNode = lastNode->Next;
  }

  newNode = new lastObjHereLoaded;
  if (!newNode)
  {
    _outtext("\nError: addLastObjLoaded() - alloc'ing newNode\n\n");
    return;
  }

  memset(newNode, 0, sizeof(lastObjHereLoaded));

  newNode->Next = NULL;
  newNode->objHerePtr = objPtr;
  newNode->objNumb = objNumb;

  if (!*lastHead) *lastHead = newNode;
  else prevNode->Next = newNode;
}


//
// getLastObjLoaded : Runs through a list of lastObjHereLoadeds, returning the
//                    address of the first match's objHere node
//
//     *lastHead : head of list
//       objNumb : object number being looked for
//

objectHere *getLastObjLoaded(lastObjHereLoaded *lastHead, const ulong objNumb)
{
  lastObjHereLoaded *lastNode = lastHead;


  while (lastNode)
  {
    if ((lastNode->objNumb) == objNumb) return lastNode->objHerePtr;

    lastNode = lastNode->Next;
  }

  return NULL;
}


//
// deleteLastObjLoadedList : Runs through a list of lastObjHereLoadeds,
//                           deleting the bastard.
//
//     *lastHead : head of list
//

void deleteLastObjLoadedList(lastObjHereLoaded *lastHead)
{
  lastObjHereLoaded *nextNode;


  while (lastHead)
  {
    nextNode = lastHead->Next;
    delete lastHead;

    lastHead = nextNode;
  }
}


//
// addLimitSpec : Adds a limitSpecified node to a list of em.
//
//    **specHead : pointer to pointer to head of list
//
//       entType : type of entity (misc/master.h)
//       entNumb : vnum of entity being added
//         limit : limit specified
//    globallimit: mud-wide limit on loads

void addLimitSpec(limitSpecified **specHead, const char entType,
                  const long entNumb, const ulong limit,
                  const ulong globallimit )
{
  limitSpecified *specNode = *specHead, *newNode, *prevNode;


  while (specNode)
  {
    if ((specNode->entityType == entType) && (specNode->vnum == entNumb) &&
        (( specNode->limit < limit) || (specNode->globallimit < globallimit)))
    {
      if( specNode->limit < limit )
        specNode->limit = limit;
      if( specNode->globallimit < globallimit )
       specNode->globallimit = globallimit;
      return;
    }

    prevNode = specNode;
    specNode = specNode->Next;
  }


  newNode = new limitSpecified;
  if (!newNode)
  {
    _outtext("\nError: addLimitSpec() - alloc'ing newNode\n\n");
    return;
  }

  memset(newNode, 0, sizeof(limitSpecified));

  newNode->Next = NULL;
  newNode->entityType = entType;
  newNode->vnum = entNumb;
  newNode->limit = limit;
  newNode->globallimit = globallimit;

  if (!*specHead) *specHead = newNode;
  else prevNode->Next = newNode;
}


//
// getLimitSpec : Runs through a list of limitSpecified, returning the
//                limit
//
//     *specHead : head of list
//       entType : entity type looking for
//       entNumb : numb of entity looking for
//

ulong getLimitSpec(limitSpecified *specHead, char entType, ulong entNumb)
{
  limitSpecified *specNode = specHead;


  while (specNode)
  {
    if ((specNode->entityType == entType) &&
        (specNode->vnum == entNumb)) return specNode->limit;

    specNode = specNode->Next;
  }

  return 0;
}


//
// deleteLimitSpecList : Runs through a list of limitSpecifieds,
//                       deleting the bastard.
//
//     *limitHead : head of list
//

void deleteLimitSpecList(limitSpecified *limitHead)
{
  limitSpecified *nextNode;


  while (limitHead)
  {
    nextNode = limitHead->Next;
    delete limitHead;

    limitHead = nextNode;
  }
}


//
// setOverrideFromLimSpec
//

void setOverrideFromLimSpec(limitSpecified *specHead)
{
  entityLoaded *numbLoadedNode;

  while (specHead)
  {
    numbLoadedNode = getNumbEntitiesNode(specHead->entityType, specHead->vnum);

    if ((specHead->limit > numbLoadedNode->numberLoaded) &&
        (numbLoadedNode->overrideLoaded < specHead->limit))
    {
      setEntityOverride(specHead->entityType, specHead->vnum, specHead->limit, specHead->globallimit);
    }

    specHead = specHead->Next;
  }
}


//
// removeComments : Removes anything after and including an asterisk (for use
//                  with zone commands)
//
//   *strn : String to alter
//

void removeComments(char *strn)
{
  ulong len = strlen(strn), i;


  if (strn[0] == '*')
  {
    strn[0] = '\0';   // added ~2.89..  should work
    return;
  }

 // first, remove the asterisk itself

  for (i = 0; i < len; i++)
  {
    if (strn[i] == '*')
    {
      strn[i] = '\0';
      break;
    }
  }

 // next, remove all the spaces preceeding the asterisk

  remTrailingSpaces(strn);
}


//
// loadObj : Load (put, place, whatever) an object into a room
//
//   *strn : String as read from zone file
//

void loadObj(const char *strn, lastObjHereLoaded **lastObjLoadedHead,
             limitSpecified **limitSpecHead, const char loadFromTable,
             const ulong linesRead)
{
  ulong objNumb, maxExist, roomNumb, globalmaxExist, unused;
  long randVal;
  objectHere *objHereNode;
  dikuRoom *roomNode;
  char tempstrn[10];
  char ifFlag;
  usint ch;

 // allocate and init objhere node

  objHereNode = new objectHere;
  if (!objHereNode)
  {
    cout << "Error allocating mem for objHereNode in loadObj() - aborting.\n";

    exit(1);
  }

  memset(objHereNode, 0, sizeof(objectHere));

 // check command string

  if (numbArgs(strn) == 5)  // old format
  {
    sscanf(strn, "%s%d%d%u%d",
           tempstrn, &ifFlag, &objNumb, &maxExist, &roomNumb);

    if (!loadFromTable)
    {
      randVal = 100;

      madeChanges = TRUE;
    }
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    if (loadFromTable)
    {
      cout <<
"error: 'load random obj from table' command has incorrect number of arguments"
           << endl <<
"       (" << numbArgs(strn) << " instead of 5, line #" << linesRead << ")."
           << endl <<
"       aborting.." << endl;

      exit(1);
    }

    sscanf(strn, "%s%d%d" "%u%d%d" "%u%u%u",
           tempstrn, &ifFlag, &objNumb,
           &maxExist, &roomNumb, &randVal,
           &globalmaxExist, &unused, &unused);
  }
  else  // error
  {
    cout << "error in obj load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if (!loadFromTable && ((randVal > 100) || (randVal <= 0)))
  {
    cout <<
"error in obj load command for object #" << objNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

 // check limit

  if (!loadFromTable)
  {
    if ((getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= maxExist) &&
        getCheckLimitsVal())
    {
      cout << "Already " << maxExist << " or more instances of object #"
           << objNumb << " loaded, skipping..\n";
      getkey();

      delete objHereNode;
      madeChanges = TRUE;

      return;
    }

    if ((globalmaxExist != 0) && (getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= globalmaxExist) &&
        getCheckLimitsVal())
    {
      cout << "Already " << globalmaxExist << " or more instances of object #"
           << objNumb << " loaded, skipping..\n";
      getkey();

      delete objHereNode;
      madeChanges = TRUE;

      return;
    }

    objHereNode->randomChance = randVal;
  }
  else
  {
   // should check all possible limit breaches for table..

//    objHereNode->tableObj = TRUE;
    objHereNode->randomChance = 100;  // shrug
  }

 // set appropriate vars..

  objHereNode->objectNumb = objNumb;

  roomNode = findRoom(roomNumb);
  if (!roomNode)
  {
    cout << "\n" <<
"Warning: load object ('O') command for obj #" << objNumb << " failed because the target\n" <<
"         room #" << roomNumb << " does not exist.\n\n";
    delete objHereNode;

    getkey();
    madeChanges = TRUE;

    return;
  }

 // add object to room

  addObjHeretoList(&(roomNode->objectHead), objHereNode);

 // set object's object type pointer

  objHereNode->objectPtr = findObj(objHereNode->objectNumb);
  if ((!(objHereNode->objectPtr)) && getVnumCheckVal() && !loadFromTable)
  {
    cout << "\n" <<
"Error: object #" << objNumb << " not found - load object ('O') command in zone file\n" <<
"       requested it.\n";

    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";

    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

  if (!loadFromTable)
  {
   // add the object to the entityLoaded list

    addEntity(ENTITY_OBJECT, objNumb);

   // add limit specified to limit spec list

    addLimitSpec(limitSpecHead, ENTITY_OBJECT, objNumb, maxExist, globalmaxExist);

   // set global max loaded if specified in .zon file.
   if( (globalmaxExist != 0) && (objHereNode->objectPtr != NULL) )
   {
     objHereNode->objectPtr->globalmaxload = globalmaxExist;
   }

  }

 // add the object to the lastObjLoaded list

  addLastObjLoaded(lastObjLoadedHead, objNumb, objHereNode);

  numbObjs++;
}


//
// loadMob : Load (place, put, blah) a mob into a room - returns the room
//           numb loaded into, or -1 if error
//
//      *strn : string as read from zone file
//  **lastMob : last mob loaded - used for equip and give commands
//

long loadMob(const char *strn, mobHere **lastMob, mobHere **lastMobEquippable,
             limitSpecified **limitSpecHead, const char loadFromTable,
             const ulong linesRead)
{
  ulong mobNumb, maxExist, roomNumb, unused;
  long randVal;
  mobHere *mobHereNode;
  dikuRoom *roomNode;
  char tempstrn[10], ifflag;
  usint ch;

  mobHereNode = new mobHere;
  if (!mobHereNode)
  {
    cout << "Error allocating mem for mobHereNode in loadMob - aborting.\n";

    exit(1);
  }

  memset(mobHereNode, 0, sizeof(mobHere));

  if (numbArgs(strn) == 5)  // old format
  {
    sscanf(strn, "%s%d%d%u%d",
           tempstrn, &ifflag, &mobNumb, &maxExist, &roomNumb);

    if (!loadFromTable)
    {
      randVal = 100;

      madeChanges = TRUE;
    }
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    if (loadFromTable)
    {
      cout <<
"error: 'load random mob from table' command has incorrect number of arguments"
           << endl <<
"       (" << numbArgs(strn) << " instead of 5, line #" << linesRead << ")."
           << endl <<
"       aborting.." << endl;

      exit(1);
    }

    sscanf(strn, "%s%d%d" "%u%d%d" "%u%u%u",
           tempstrn, &ifflag, &mobNumb,
           &maxExist, &roomNumb, &randVal,
           &unused, &unused, &unused);
  }
  else
  {
    cout << "error in mob load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if (!loadFromTable && ((randVal > 100) || (randVal <= 0)))
  {
    cout <<
"error in mob load command for mob #" << mobNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

  if (!loadFromTable)
  {
    if ((getNumbEntities(ENTITY_MOB, mobNumb, FALSE) >= maxExist) &&
        getCheckLimitsVal())
    {
      cout << "Already " << maxExist << " or more instances of mob #" << mobNumb
           << " loaded, skipping..\n";
      getkey();

      delete mobHereNode;
      madeChanges = TRUE;

      return -1;  // bah
    }

    mobHereNode->randomChance = randVal;
  }
  else
  {
   // should check all possible limit breaches for table..

//    mobHereNode->tableMob = TRUE;
    mobHereNode->randomChance = 100;  // shrug
  }

  mobHereNode->mobNumb = mobNumb;
  roomNode = findRoom(roomNumb);

  if (!roomNode)
  {
    cout << "\n" <<
"Warning: load mob ('M') command for mob #" << mobNumb << " failed because the target\n" <<
"         room #" << roomNumb << " does not exist.\n\n";
    delete mobHereNode;

    getkey();  // death is bad

    madeChanges = TRUE;
    return -1;
  }

  addMobHeretoList(&(roomNode->mobHead), mobHereNode);

  mobHereNode->mobPtr = findMob(mobHereNode->mobNumb);
  if ((!(mobHereNode->mobPtr)) && getVnumCheckVal() && !loadFromTable)
  {
    cout << "\n" <<
"Error: mob #" << mobNumb << " requested by load mob ('M') command in zone file\n" <<
"       not found.\n";


    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

  if (!loadFromTable)
  {
   // add the mob to the entityLoaded list

    addEntity(ENTITY_MOB, mobNumb);

   // add limit specified to limit spec list

    addLimitSpec(limitSpecHead, ENTITY_MOB, mobNumb, maxExist, 0);
  }

  numbMobs++;

  *lastMob = *lastMobEquippable = mobHereNode;

  return roomNumb;
}


//
// setDoorState : Set the state of a door in a room
//
//   *strn : String as read from zone file
//

void setDoorState(const char *strn, const ulong linesRead)
{
  dikuRoom *room;
  roomExit *exitNode = NULL;
  long dstate, exitnumb, ifflag;
  char dummy[10],// roomStrn[64],
       dtype, dbits, wtype, wbits;
  ulong roomnumb, unused;
  usint ch;


 // get info from string

  if (numbArgs(strn) == 5)  // old format
  {
    sscanf(strn, "%s%d%d%d%d",
           dummy, &ifflag, &roomnumb, &exitnumb, &dstate);
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    sscanf(strn, "%s%d%d" "%d%d%u" "%u%u%u",
           dummy, &ifflag, &roomnumb,
           &exitnumb, &dstate, &unused,
           &unused, &unused, &unused);
  }
  else  // error
  {
    cout << "error in door state command (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // find the corresponding room and make sure it exists

  room = findRoom(roomnumb);

  if (!room)
  {
    cout << "\n" <<
"Warning: set door state ('D') command for exit #" << exitnumb << " invalid because the\n" <<
"         target room #" << roomnumb << " does not exist.\n\n";

    getkey();

    madeChanges = TRUE;
    return;
  }

 // find which exit this belongs to

  if ((exitnumb < 0) || (exitnumb >= NUMB_EXITS))
  {
    cout << "\n" <<
"Warning: exit number (" << exitnumb << ") specified in set door state 'D' command invalid.\n" <<

    getkey();
    madeChanges = TRUE;
  }
  else exitNode = room->exits[exitnumb];

 // check for exit existance

  if (!exitNode)
  {
    cout << "\n" <<
"Warning: set door state ('D') command issued for a non-existant exit in\n" <<
"         room #" << roomnumb << ".  (Exit number specified was #" << exitnumb << ".)\n"
"         Ignoring..\n";

    getkey();
    madeChanges = TRUE;
    return;
  }

 // check to see if door state is a valid value

  if ((dstate < 0) || (dstate > 15) || ((dstate & 3) == 3))
  {
    cout << "\n" <<
"Error: set door state ('D') command for exit #" << exitnumb << " in room #" << roomnumb << "\n"
"       invalid because the door state specified is an impossible value (" << (int)(dstate) << ").\n" <<
"       (Less than 0, greater than 15, or trying to set state to 3).\n" <<
"       Aborting..\n\n";

    exit(1);
  }

  dtype = dstate & 3;
  dbits = dstate & 12;

  wtype = exitNode->worldDoorType & 3;
  wbits = exitNode->worldDoorType & 12;

 // make sure a zone door state is being set on an existant door (as long
 // as the zone door state isn't "no door/open/this should really be fixed
 // so that we can differentiate between no door state command vs. a door
 // state command with 'door type' 0")

  if ((dtype) && (!wtype))
  {
//    do
//    {
      cout << "\n"
  "Warning: set door state command specified in .ZON sets door state to a non-zero\n" <<
  "         value, but the door type specified for the exit in the .WLD is 'no\n" <<
  "         door'.  (Room #" << roomnumb << ", exit #" << exitnumb <<
  ", type in .WLD " << (usint)(exitNode->worldDoorType) << ", door state in .ZON " <<
   (int)dstate << ".)\n";

      cout << "\n"
//  "Set door type in .WLD file equal to that in .ZON file (W), or\n" <<
//  "set door type in .ZON file equal to that in .WLD file (Z), or\n" <<
//  "you can even hit L to look at the room with the flawed exit (W/z/l)? ";
"Set door type in .WLD file equal to that in .ZON file (W), or\n" <<
"set door type in .ZON file equal to that in .WLD file (Z) (W/z)? ";

    do
    {
      ch = toupper(getkey());

//      if (ch == 'L') displayRoom(room);
    } while ((ch != 'W') && (ch != 'Z') && (ch != K_Enter));

    if (ch != 'Z')
    {
      cout << "W\n";

      exitNode->worldDoorType = dtype | wbits;

      madeChanges = TRUE;
    }
    else
    {
      cout << "Z\n";

      exitNode->zoneDoorState = wtype | dbits;

      madeChanges = TRUE;
    }
  }
  else exitNode->zoneDoorState = dstate;
}


//
// putObjObj : Put an object inside another object (hopefully a container)
//
//   *strn : String as read from zone file
//

void putObjObj(const char *strn, lastObjHereLoaded **lastObjLoaded,
               limitSpecified **limitSpecHead, char *pauseonCont,
               const ulong linesRead)
{
  objectHere *objHereNode, *objContainerNode;
  dikuObject *objContainee, *objContainer;
               // objContainer is object that objContainee is being put into

  char dummy[10], ifflag;
  ulong objConteeNumb, objContNumb, maxExist, globalmaxExist, unused;
  long randVal;
  usint ch;


 // allocate memory for objHereNode

  objHereNode = new objectHere;
  if (!objHereNode)
  {
    cout << "Error allocating memory for objectHere struct in putObjObj()."
            "  Aborting..\n";

    exit(1);
  }

  memset(objHereNode, 0, sizeof(objectHere));

 // get info from string

  if (numbArgs(strn) == 5)  // old format
  {
    sscanf(strn, "%s%d%d%u%d",
           dummy, &ifflag, &objConteeNumb, &maxExist, &objContNumb);

    randVal = 100;

    madeChanges = TRUE;
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    sscanf(strn, "%s%d%d" "%u%d%d" "%u%u%u",
           dummy, &ifflag, &objConteeNumb,
           &maxExist, &objContNumb, &randVal,
           &globalmaxExist, &unused, &unused);
  }
  else  // error
  {
    cout << "error in obj load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if ((randVal > 100) || (randVal <= 0))
  {
    cout <<
"error in obj load command for object #" << objConteeNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

  if ((getNumbEntities(ENTITY_OBJECT, objConteeNumb, FALSE) >= maxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << maxExist << " or more instances of object #"
         << objConteeNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

  if ((globalmaxExist != 0) && (getNumbEntities(ENTITY_OBJECT, objConteeNumb, FALSE) >= globalmaxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << globalmaxExist << " or more instances of object #"
         << objConteeNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

 // find the "containee" object and make sure it exists

  objContainee = findObj(objConteeNumb);

  if ((!objContainee) && (getVnumCheckVal()))
  {
    cout << "\n" <<
"Error: put obj in obj ('P') command for obj #" << objConteeNumb << " invalid\n" <<
"       because this object type does not exist.\n\n";


    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

  objContainerNode = getLastObjLoaded(*lastObjLoaded, objContNumb);

  if (!objContainerNode)
  {
    cout << "\n" <<
"Error: put obj in obj ('P') command for obj #" << objConteeNumb << " invalid because\n" <<
"       the container to place the first object into (#" << objContNumb << ") has not\n" <<
"       been previously loaded.\n"
"\n"
"Skipping load, press any key to continue..\n\n";

    delete objHereNode;
    madeChanges = TRUE;
//    exit(1);
    getkey();
    return;
  }

  objContainer = objContainerNode->objectPtr;

/*
  if (!objContainer)  // shouldn't happen
  {
    cout << "Error: put obj in obj ('P') command for obj #" << objContNumb
         << " invalid because the container to place the first object into "
            "(#"
         << objContNumb
         << ") has not been previously loaded.\n";

    exit(1);
  }
*/

  if ((!objContainer) && (*pauseonCont))
  {
    cout << "\n" <<
"Error: put obj in obj ('P') command for obj #" << objContNumb << " (container) invalid\n" <<
"       because the container's object type is not in this zone - stick item\n" <<
"       (and all items in a similar situation) in the container anyway (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n\n";
      exit(1);
    }
    else cout << "Yes\n\n";

    *pauseonCont = FALSE;
  }
  else
  if ((objContainer) && ((objContainer->objType) != ITEM_CONTAINER))
  {
    cout << "\n" <<
"Error: put obj in obj ('P') command for obj #" << objContNumb << " is invalid because\n" <<
"       this object is not a container.\n";

    exit(1);
  }

 // set stuff for objHereNode - instance of "containee" object

  objHereNode->objectNumb = objConteeNumb;
  objHereNode->objectPtr = objContainee;
  objHereNode->randomChance = randVal;

  addLastObjLoaded(lastObjLoaded, objConteeNumb, objHereNode);

 // pointers in objHereNode already set to null by memset() command above..

 // add the new link to the chain, or die trying

  addObjHeretoList(&(objContainerNode->objInside), objHereNode);

 // finally, add the "containee" object to the entityLoaded list

  addEntity(ENTITY_OBJECT, objConteeNumb);

 // add limit specified to limit spec list

  addLimitSpec(limitSpecHead, ENTITY_OBJECT, objConteeNumb, maxExist, globalmaxExist);

   // set global max loaded if specified in .zon file.
   if( (globalmaxExist != 0) && (objHereNode->objectPtr != NULL) )
   {
     objHereNode->objectPtr->globalmaxload = globalmaxExist;
   }

  numbObjs++;
}


//
// equipMobObj : Equip an object on a mob
//
//         *strn : string as read from zone file
//  *lastMobRead : mob to equip object with
//

void equipMobObj(const char *strn, mobHere *lastMobRead,
                 lastObjHereLoaded **lastObjLoaded,
                 limitSpecified **limitSpecHead, const ulong linesRead)
{
  objectHere *objHereNode;
  dikuObject *dikuObj;
  char dummy[10], ifflag, strn2[256], incarry = FALSE;
  ulong objNumb, maxExist, equipWhere, globalmaxExist, unused;
  usint ch;
  long randVal;

 // allocate memory for objHereNode

  objHereNode = new objectHere;
  if (!objHereNode)
  {
    cout << "\n" <<
"Error allocating memory for objectHere struct in equipMobObj().  Aborting..\n";

    exit(1);
  }

  memset(objHereNode, 0, sizeof(objectHere));

 // get info from string

  if (numbArgs(strn) == 5)  // old format
  {
    sscanf(strn, "%s%d%d%u%u",
           dummy, &ifflag, &objNumb, &maxExist, &equipWhere);

    randVal = 100;

    madeChanges = TRUE;
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    sscanf(strn, "%s%d%d" "%u%u%d" "%u%u%u",
           dummy, &ifflag, &objNumb,
           &maxExist, &equipWhere, &randVal,
           &globalmaxExist, &unused, &unused);
  }
  else  // error
  {
    cout << "error in mob equip (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if ((randVal > 100) || (randVal <= 0))
  {
    cout <<
"error in obj load command for object #" << objNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

 // check equipWhere

  if (equipWhere > WEAR_HIGH)
  {
    cout << "\n" <<
"Error: equip mob with obj ('E') command for obj #" << objNumb << "\n" <<
"       failed because equip position (#" << equipWhere << ") is invalid.\n" <<
"       Aborting..\n";

    exit(1);
  }

  if ((getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= maxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << maxExist << " or more instances of object #"
         << objNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

  if ((globalmaxExist != 0 ) && (getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= globalmaxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << globalmaxExist << " or more instances of object #"
         << objNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

 // find the object to be equipped and make sure it exists

  dikuObj = findObj(objNumb);

  if ((!dikuObj) && (getVnumCheckVal()))
  {
    cout << "\n" <<
"Error: equip mob with obj ('E') command for obj #" << objNumb << " invalid because this\n" <<
"       object type does not exist.\n\n";

    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

 // find the mob and make sure it exists

  if (!lastMobRead)
  {
    cout << "\n" <<
"Error: equip mob with obj ('E') command invalid because no mob has been loaded\n" <<
"       previous to the command (obj # is " << objNumb << ").\n"
"\n"
"Skipping load, press any key..\n\n";

    getkey();
    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

  ch = getMobHereEquipSlot(lastMobRead, dikuObj, equipWhere);
  if (ch > WEAR_TRUEHIGH)
  {
    cout << "\n" <<
"Error: equip mob with obj ('E') command failed - obj #" << objNumb << " is being\n" <<
"       equipped on mob #" << lastMobRead->mobNumb << ".\n" <<
"\n" <<
"       error was '" << getCanEquipErrStrn(ch, equipWhere, strn2) <<
"'.\n\n";

    cout << "\n" <<
"Stick object in mob's carried list and continue (Y/n)? ";

    if (toupper(getkey()) == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    incarry = madeChanges = TRUE;

    cout << "Yes\n\n";
//    exit(1);
  }

 // set stuff

  objHereNode->objectNumb = objNumb;
  objHereNode->objectPtr = dikuObj;
//  if (!incarry) objHereNode->whereEquipped = ch;
    // it may be WHOLEHEAD or BODY, TWOHANDS
//equipWhere;
  objHereNode->randomChance = randVal;

 // pointers in objHereNode already set to null by memset() command above..

 // add the new node to the equipHead list of the mob

  if (!incarry) lastMobRead->equipment[ch] = objHereNode;
  else addObjHeretoList(&(lastMobRead->objectHead), objHereNode);

 // add the object being equipped to the entityLoaded list and
 // lastObjLoaded list

  addEntity(ENTITY_OBJECT, objNumb);
  addLastObjLoaded(lastObjLoaded, objNumb, objHereNode);

 // add limit specified to limit spec list

  addLimitSpec(limitSpecHead, ENTITY_OBJECT, objNumb, maxExist, globalmaxExist);

   // set global max loaded if specified in .zon file.
   if( (globalmaxExist != 0) && (objHereNode->objectPtr != NULL) )
   {
     objHereNode->objectPtr->globalmaxload = globalmaxExist;
   }

  numbObjs++;
}


//
// giveMobObj : Give an object to a mob
//
//         *strn : string as read from zone file
//  *lastMobRead : mob to give object to
//

void giveMobObj(const char *strn, mobHere *lastMobRead,
                lastObjHereLoaded **lastObjLoaded,
                limitSpecified **limitSpecHead, const ulong linesRead)
{
  objectHere *objHereNode;
  dikuObject *dikuObj;
  char dummy[10], ifflag;
  ulong objNumb, maxExist, globalmaxExist, unused;
  usint ch;
  long randVal;

 // allocate memory for objHereNode

  objHereNode = new objectHere;
  if (!objHereNode)
  {
    cout << "Error allocating memory for objectHere struct in giveMobObj()."
            "  Aborting..\n";

    exit(1);
  }

  memset(objHereNode, 0, sizeof(objectHere));

 // get info from string

  if (numbArgs(strn) == 4)  // old format
  {
    sscanf(strn, "%s%d%d%u", dummy, &ifflag, &objNumb, &maxExist);

    randVal = 100;
    madeChanges = TRUE;
  }
  else if (numbArgs(strn) == 9)  // new format
  {
    sscanf(strn, "%s%d%d" "%u%u%d" "%u%u%u",
           dummy, &ifflag, &objNumb,
           &maxExist, &unused, &randVal,
           &globalmaxExist, &unused, &unused);
  }
  else  // error
  {
    cout << "error in obj load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if ((randVal > 100) || (randVal <= 0))
  {
    cout <<
"error in obj load command for object #" << objNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

 // check number loaded

  if ((getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= maxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << maxExist << " or more instances of object #"
         << objNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

  if ((globalmaxExist != 0 ) && (getNumbEntities(ENTITY_OBJECT, objNumb, FALSE) >= globalmaxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << globalmaxExist << " or more instances of object #"
         << objNumb << " loaded, skipping..\n";
    getkey();

    delete objHereNode;
    madeChanges = TRUE;

    return;
  }

 // find the object to be equipped and make sure it exists

  dikuObj = findObj(objNumb);

  if ((!dikuObj) && (getVnumCheckVal()))
  {
    cout << "\n"
"Error: give obj to mob ('G') command for obj #" << objNumb << " invalid because this\n" <<
"       object type does not exist.\n";


    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

 // find the mob and make sure it exists

  if (!lastMobRead)
  {
    cout << "\n" <<
"Error: give obj to mob ('G') command invalid because no mob has been loaded\n" <<
"       previous to the command.\n"
"\n"
"Skipping load, press any key to continue..\n";

    getkey();
    madeChanges = TRUE;
    delete objHereNode;

//    exit(1);
    return;
  }

 // set stuff

  objHereNode->objectNumb = objNumb;
  objHereNode->objectPtr = dikuObj;
  objHereNode->randomChance = randVal;

 // pointers in objHereNode already set to null by memset() command above..

 // add the new node to the objectHead list of the mob

  addObjHeretoList(&(lastMobRead->objectHead), objHereNode);

 // finally, add the object being given to the entityLoaded list

  addEntity(ENTITY_OBJECT, objNumb);
  addLastObjLoaded(lastObjLoaded, objNumb, objHereNode);

 // add limit specified to limit spec list

  addLimitSpec(limitSpecHead, ENTITY_OBJECT, objNumb, maxExist, globalmaxExist);

   // set global max loaded if specified in .zon file.
   if( (globalmaxExist != 0) && (objHereNode->objectPtr != NULL) )
   {
     objHereNode->objectPtr->globalmaxload = globalmaxExist;
   }

  numbObjs++;
}


//
// mountMob : One mob mounts another (and enjoys it!  cringe)
//
//          *strn : string as read from zone file
//  **lastMobRead : mob to mount
//

void mountMob(const char *strn, mobHere **lastMobRead,
              limitSpecified **limitSpecHead, const ulong linesRead)
{
  char dummy[10], ifflag;
  ulong mobNumb, maxExist, roomNumb, unused;
  dikuMob *mob;
  dikuRoom *room;
  mobHere *mobHereNode;
  usint ch;
  long randVal;

 // allocate memory for mobHereNode - mount

  mobHereNode = new mobHere;
  if (!mobHereNode)
  {
    cout << "Error allocating memory for mobHere struct in mountMob()."
            "  Aborting..\n";

    exit(1);
  }

  memset(mobHereNode, 0, sizeof(mobHere));

 // get info from string

  if (numbArgs(strn) == 5)
  {
    sscanf(strn, "%s%d%d%u%u", dummy, &ifflag, &mobNumb, &maxExist, &roomNumb);

    randVal = 100;
    madeChanges = TRUE;
  }
  else if (numbArgs(strn) == 9)
  {
    sscanf(strn, "%s%d%d" "%u%u%d" "%u%u%u",
           dummy, &ifflag, &mobNumb,
           &maxExist, &roomNumb, &randVal,
           &unused, &unused, &unused);
  }
  else
  {
    cout << "error in mob load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if ((randVal > 100) || (randVal <= 0))
  {
    cout <<
"error in mount command for mob #" << mobNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

 // check limit

  if ((getNumbEntities(ENTITY_MOB, mobNumb, FALSE) >= maxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << maxExist << " or more instances of mob #"
         << mobNumb << " loaded, skipping..\n";
    getkey();

    delete mobHereNode;
    madeChanges = TRUE;

    return;
  }

 // find the mob to be the mount and make sure it exists

  mob = findMob(mobNumb);

  if ((!mob) && (getVnumCheckVal()))
  {
    cout << "\n"
"Error: mount mob ('R') command for mob #" << mobNumb << " invalid because this\n" <<
"       mob (mount) type does not exist.\n";


    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

 // make sure a mob has been loaded previous

  if (!(*lastMobRead))
  {
    cout << "\n" <<
"Error: mount mob ('R') command invalid because no mob has been loaded\n" <<
"       previous to the command.\n"
"\n"
"Skipping load, press any key..\n\n";

    getkey();
    madeChanges = TRUE;

    delete mobHereNode;

    return;

//    exit(1);
  }

 // set stuff

  mobHereNode->mobNumb = mobNumb;
  mobHereNode->mobPtr = mob;
  mobHereNode->randomChance = randVal;

  room = findRoom(roomNumb);
  if (!room)
  {
    cout << "\n" <<
"Error: mount mob ('R') command invalid because invalid room vnum specified\n"
"       (#" << roomNumb << ").\n"
"\n"
"Skipping load, press any key..\n\n";

    getkey();
    madeChanges = TRUE;

    delete mobHereNode;

    return;

//    exit(1);
  }

  addMobHeretoList(&(room->mobHead), mobHereNode);

 // pointers in mobHereNode already set to null by memset() command above..

 // make sure mob isn't already riding something

  if ((*lastMobRead)->riding)
  {
    cout << "Error: mount mob ('R') command invalid because mob #" << (*lastMobRead)->mobNumb << "\n" <<
            "       is already riding a mount (mob #" << (*lastMobRead)->riding->mobNumb << ").  Aborting..\n";

    exit(1);
  }

  (*lastMobRead)->riding = mobHereNode;
  mobHereNode->riddenBy = *lastMobRead;

 // finally, add the mount being ridden to the entityLoaded list

  addEntity(ENTITY_MOB, mobNumb);
  numbMobs++;

 // add limit specified to limit spec list

  addLimitSpec(limitSpecHead, ENTITY_MOB, mobNumb, maxExist, 0);

  *lastMobRead = mobHereNode;  // reset last mob read to mount
}


//
// followMob : One mob follows another
//
//          *strn : string as read from zone file
//   *lastMobRead : mob to follow
//  **lastMobEquippable : this gets reset
//

void followMob(const char *strn, mobHere *lastMobRead, mobHere **lastMobEquippable,
               limitSpecified **limitSpecHead, const ulong linesRead)
{
  char dummy[10], ifflag;
  ulong mobNumb, maxExist, roomNumb, unused;
  dikuMob *mob;
  dikuRoom *room;
  mobHere *mobHereNode;
  usint ch;
  long randVal;


 // allocate memory for mobHereNode - follower

  mobHereNode = new mobHere;
  if (!mobHereNode)
  {
    cout << "Error allocating memory for mobHere struct in followMob()."
            "  Aborting..\n";

    exit(1);
  }

  memset(mobHereNode, 0, sizeof(mobHere));

 // get info from string

  if (numbArgs(strn) == 5)
  {
    sscanf(strn, "%s%d%d%u%u", dummy, &ifflag, &mobNumb, &maxExist, &roomNumb);

    randVal = 100;
    madeChanges = TRUE;
  }
  else if (numbArgs(strn) == 9)
  {
    sscanf(strn, "%s%d%d" "%u%u%d" "%u%u%u",
           dummy, &ifflag, &mobNumb,
           &maxExist, &roomNumb, &randVal,
           &unused, &unused, &unused);
  }
  else
  {
    cout << "error in mob load (line #" << linesRead << ") - wrong number of args\n";
    exit(1);
  }

 // check random value

  if ((randVal > 100) || (randVal <= 0))
  {
    cout <<
"error in follow command for mob #" << mobNumb << " (line #" << linesRead <<
") -\n";

    if (randVal == 0)
    {
      cout << "random value is set to 0% - setting to 100%\n";
    }
    else
    {
      cout << "random value is not from 1-100% (value read was " << randVal << ") - setting to 100%\n";
    }

    randVal = 100;
    madeChanges = TRUE;
  }

 // check limit

  if ((getNumbEntities(ENTITY_MOB, mobNumb, FALSE) >= maxExist) &&
      getCheckLimitsVal())
  {
    cout << "Already " << maxExist << " or more instances of mob #"
         << mobNumb << " loaded, skipping..\n";
    getkey();

    delete mobHereNode;
    madeChanges = TRUE;

    return;
  }

 // find the mob to be the mount and make sure it exists

  mob = findMob(mobNumb);

  if (!mob && getVnumCheckVal())
  {
    cout << "\n"
"Error: follow mob ('F') command for mob #" << mobNumb << " invalid because this\n" <<
"       mob type does not exist.\n";


    cout << "\n" <<
"Turn vnum checking off and continue (Y/n)? ";

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch == 'N')
    {
      cout << "No\n";
      exit(1);
    }

    cout << "Yes\n";
    setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, TRUE);
  }

 // make sure a mob has been loaded previous

  if (!lastMobRead)
  {
    cout << "\n" <<
"Error: follow mob ('F') command invalid because no mob has been loaded\n" <<
"       previous to the command.\n"
"\n"
"Skipping load, press any key..\n\n";

    getkey();
    madeChanges = TRUE;

    delete mobHereNode;

    return;

//    exit(1);
  }

 // set stuff

  mobHereNode->mobNumb = mobNumb;
  mobHereNode->mobPtr = mob;
  mobHereNode->following = lastMobRead;
  mobHereNode->randomChance = randVal;

  room = findRoom(roomNumb);
  if (!room)
  {
    cout << "\n" <<
"Error: follow mob ('F') command invalid because invalid room vnum specified\n"
"       (#" << roomNumb << ").\n"
"\n"
"Skipping load, press any key..\n\n";

    getkey();
    madeChanges = TRUE;

    delete mobHereNode;

    return;

//    exit(1);
  }

  addMobHeretoList(&(room->mobHead), mobHereNode);

 // finally, add the follower to the entityLoaded list

  addEntity(ENTITY_MOB, mobNumb);
  numbMobs++;

 // add limit specified to limit spec list

  addLimitSpec(limitSpecHead, ENTITY_MOB, mobNumb, maxExist, 0);

  *lastMobEquippable = mobHereNode;  // reset last mob read to mount
}


//
// readZoneFile : Reads all the zone info and commands from the user-specified
//                zone file - if filename is non-NULL, it is used as the name
//                of the file to be read, else value in MAINZONENAME is used -
//                returns FALSE if couldn't read file
//

char readZoneFile(const char *filename /*= NULL*/)
{
  FILE *zoneFile;
  mobHere *lastMobRead = NULL, *lastMobEquippable = NULL;

 // lastMobRead is used for F commands, lastMobEquippable is used
 // for E, G commands.  lastMobRead reset only by M.  lastMobEquippable
 // reset by M F and R

  objectHere *lastObjRead = NULL;
  lastObjHereLoaded *lastObjLoaded = NULL;

  limitSpecified *limitSpecHead = NULL;

  char zoneFilename[81] = "\0", strn[512], contPause = TRUE;

  ulong i, linesRead = 0, unused, temp;

 // note - every zone command now has 8 args - the 5th arg is always the
 //        %age to load, 6-8 are unused as of now (8 args include if-flag)

 // weather info is gone - first #zone numb, CR/LF, name, ~, CR/LF,
 //                        then top room, reset mode, flags, lifespan min,
 //                        lifespan max, CR/LF, and rest of data


 // assemble the filename of the zone file

  if (readFromSubdirs) strcpy(zoneFilename, "zon/");

  if (filename) strcat(zoneFilename, filename);
  else strcat(zoneFilename, getMainZoneNameStrn());

  strcat(zoneFilename, ".zon");

 // open the zone file for reading

  if ((zoneFile = fopen(zoneFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << zoneFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << zoneFilename << "...\n";

 // first, read the zone number

  if (!fgets(strn, 512, zoneFile))
  {
    cout << "\n" <<
"error - hit EOF while reading zone number - aborting...\n";

    exit(1);
  }

  nolf(strn);
  linesRead++;

  if (strn[0] != '#')
  {
    cout << "\n" <<
"error - '#' expected at start of zone number, instead found "
         << "'" << strn[0] << "'\n";

    cout << "\nEntire string read was '" << strn << "'.\n";

    exit(1);
  }

 // remove the durn '#' at the start of the string

  deleteChar(strn, 0);

 // set the zone number

  zoneRec.zoneNumber = atoi(strn);

 // read the zone name

  if (!fgets(strn, MAX_ZONENAME_LEN, zoneFile))
  {
    cout << "Data for zone #" << zoneRec.zoneNumber << " not complete - "
         << "hit EOF while reading name.  Aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire zone name - max length that can be read is\n" <<
"        " << MAX_ZONENAME_LEN << " characters.\n" <<
"        string read was '" << strn << "'.\n" <<
"        aborting..\n";

    exit(1);
  }

 // remove linefeeds and check for tilde at the end of the keyword strn

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  linesRead++;

  i = strlen(strn) - 1;  // it's handy

  if (strn[i] != '~')
  {
    cout << "\n" <<
"error - data for zone #" << zoneRec.zoneNumber << " not complete - zone name doesn't\n" <<
"        end in ~.  Aborting..\n";

    exit(1);
  }

  strn[i] = '\0';  // get rid of the tilde
  strcpy(zoneRec.zoneName, strn);

 // check if the zone file is new format or the old format - new format
 // has 6 args on this line, old format has 4

 // weather info is gone - first #zone numb, CR/LF, name, ~, CR/LF,
 //                        then top room, reset mode, flags, lifespan min,
 //                        lifespan max, CR/LF, and rest of data

  if (!fgets(strn, 512, zoneFile))
  {
    cout << "Data for zone #" << zoneRec.zoneNumber << " not complete - "
         << "hit EOF while reading name.  Aborting..\n";

    exit(1);
  }

  if (numbArgs(strn) == 4)  // old format
  {
    cout << "\n" <<
"Zone file is in old format.  dikuEdit will convert the zone file into the new\n" <<
"format when you save the zone.\n\n" <<
"Press any key to continue..";

    getkey();
    cout << "\n";

   // interpret the string

    sscanf(strn, "%u%u%u%u",
           &(zoneRec.topRoomNumb),
           &temp,
           &(zoneRec.resetMode),
           &(zoneRec.miscBits.longIntFlags));

   // read the rest of the zone info

    if (fscanf(zoneFile, "%u%u%u\n"   "%u%u%u%u\n" "%u%u%u%u\n"
                         "%u%u%u%u\n" "%u%u%u%u\n" "%u%u%u%u\n",

              // yeeha..  read all the now-useless stuff

               &unused, &unused, &unused,
               &unused, &unused, &unused, &unused,
               &unused, &unused, &unused, &unused,
               &unused, &unused, &unused, &unused,
               &unused, &unused, &unused, &unused,
               &unused, &unused, &unused, &unused) == EOF)
    {
      cout << "\n" <<
  "error - hit EOF while reading info beyond zone name for zone #" << zoneRec.zoneNumber << ".\n" <<
  "        (reading old format zone file)  aborting..\n";

      exit(1);
    }

   // set new lifeLow and lifeHigh vals based on old lifeSpan var

    if ((temp - 5) > 0) zoneRec.lifeLow = temp - 5;
    else zoneRec.lifeLow = 1;

    zoneRec.lifeHigh = temp + 5;

    madeChanges = TRUE;

    linesRead += 7;
  }
  else if (numbArgs(strn) == 6)  // new format
  {
    sscanf(strn, "%u%u%u%u%u%u\n",
           &(zoneRec.topRoomNumb),
           &(zoneRec.resetMode),
           &(zoneRec.miscBits.longIntFlags),
           &(zoneRec.lifeLow),
           &(zoneRec.lifeHigh),
           &unused);

    linesRead++;
  }
  else  // error
  {
    cout <<
"Error - data after zone name is invalid (should have either 4 or 6 args).\n"
"Line read was '" << strn << "'.  aborting..\n";

    exit(1);
  }

 // check for common errors in zone data

  if (!findRoom(zoneRec.topRoomNumb))
  {
    cout << "\n" <<
"Warning: Top room (#" << zoneRec.topRoomNumb << ") for zone not found.\n";

    madeChanges = TRUE;

    getkey();
  }
  else
  if (zoneRec.topRoomNumb != getHighestRoomNumber())
  {
    cout << "\n" <<
"Warning: Top room (#" << zoneRec.topRoomNumb << ") for zone not equal to\n"
"         actual top room number (#" << getHighestRoomNumber() << ").\n";

    madeChanges = TRUE;

    getkey();
  }

  if (zoneRec.lifeLow >= zoneRec.lifeHigh)
  {
    cout << "\n" <<
"Warning: Low lifespan value for zone less than or equal to high lifespan value\n" <<
"         (" << zoneRec.lifeLow << " vs " << zoneRec.lifeHigh << ".)\n\n" <<
"Setting low lifespan value to " << zoneRec.lifeHigh << " and high lifespan value to " <<
zoneRec.lifeHigh + 5 << "...\n";

    madeChanges = TRUE;

    zoneRec.lifeLow = zoneRec.lifeHigh;
    zoneRec.lifeHigh += 5;

    getkey();
  }

 // this while loop reads command by command, one command per iteration

  while (TRUE)
  {

   // Read a command

    if (!fgets(strn, 512, zoneFile))
    {
     // if not ignoring the 'S' that is supposed to be at the end of
     // each zone file, hitting EOF with no 'S' is an error - otherwise,
     // assume we ignored the appropriate 'S' and close file as we would
     // normally

      if (!getIgnoreZoneSVal())
      {
        cout << "\n" <<
  "error - hit EOF on zone file while reading commands - aborting..\n";

        exit(1);
      }
      else
      {
        fclose(zoneFile);  deleteLastObjLoadedList(lastObjLoaded);

        setOverrideFromLimSpec(limitSpecHead);
        deleteLimitSpecList(limitSpecHead);

        return TRUE;
      }
    }

    linesRead++;

    if (!lastCharLF(strn))  // couldn't read whole string
    {
      cout << "\n" <<
"Error - couldn't read entire command in .ZON file - line #" << linesRead << "...\n" <<
"        max length that can be read is 512 characters.\n" <<
"        first 512 characters read were '" << strn << "'.\n" <<
"        aborting..\n";

      exit(1);
    }

    nolf(strn);
    removeComments(strn);
    remLeadingSpaces(strn);

    switch (toupper(strn[0]))
    {
      case '*' : break;  // ignore rest of line (comment)
      case '\0': break;  // blank line - ignore it

     // load a mob

      case 'M' : loadMob(strn, &lastMobRead, &lastMobEquippable,
                         &limitSpecHead, FALSE, linesRead);
                 break;

     // load a random mob from a table

/*
      case 'Y' : loadMob(strn, &lastMobRead, &lastMobEquippable,
                         &limitSpecHead, TRUE, linesRead);
                 break;
*/

     // load an object

      case 'O' : loadObj(strn, &lastObjLoaded, &limitSpecHead, FALSE,
                         linesRead);
                 break;

     // load a random object from a table

/*
      case 'C' : loadObj(strn, &lastObjLoaded, &limitSpecHead, TRUE, linesRead);
                 break;
*/

     // give a mob an object

      case 'G' : giveMobObj(strn, lastMobEquippable, &lastObjLoaded,
                            &limitSpecHead, /*FALSE,*/ linesRead);
                 break;

     // give mob a random object from a table

/*
      case 'A' : giveMobObj(strn, lastMobEquippable, &lastObjLoaded,
                            &limitSpecHead, linesRead);
                 break;
*/

     // equip mob with object

      case 'E' : equipMobObj(strn, lastMobEquippable, &lastObjLoaded,
                             &limitSpecHead, linesRead);
                 break;

     // put object in object

      case 'P' : putObjObj(strn, &lastObjLoaded, &limitSpecHead, &contPause,
                           linesRead);
                 break;

     // set door state

      case 'D' : setDoorState(strn, linesRead);
                 break;

     // mount mob on another

      case 'R' : mountMob(strn, &lastMobEquippable, &limitSpecHead, linesRead);
                 break;

     // have mob follow another

      case 'F' : followMob(strn, lastMobRead, &lastMobEquippable,
                           &limitSpecHead, linesRead);
                 break;

     // end of zone file

      case 'S' : if (!getIgnoreZoneSVal())
                 {
                   fclose(zoneFile);  deleteLastObjLoadedList(lastObjLoaded);

                   setOverrideFromLimSpec(limitSpecHead);
                   deleteLimitSpecList(limitSpecHead);

                   return TRUE;
                 } else break;

      default  : cout << "\n"
"error - unrecognized command in zone file: string read was\n" <<
"'" << strn << "' (line #" << linesRead << ").  Aborting...\n";

                 exit(1);
    }
  }
}
