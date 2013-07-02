//
//  File: loaded.cpp     originally part of dikuEdit
//
//  Usage: functions used to manipulate 'numb loaded' list
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "loaded.h"

#include "../obj/objhere.h"
#include "../mob/mobhere.h"
#include "../misc/master.h"
#include "../command/command.h"

using namespace std;

extern entityLoaded *numbLoadedHead;
extern dikuObject *objHead;
extern dikuMob *mobHead;
extern command *setLimitCmdHead;
extern char madeChanges;
extern char display;

//
// addEntity : Add a specific entity to the list - that is, increment the
//             number loaded for this specific entity.
//
//   entityType : type of entity that is being added
//   entityNumb : number of entity that is being added
//

void addEntity(const char entityType, const ulong entityNumb)
{
  entityLoaded *numbLoadedNode = numbLoadedHead, *oldNode = NULL;


 // first, check to see if node referencing this entity type and numb already
 // exists

  while (numbLoadedNode)
  {
    if (((numbLoadedNode->entityType) == entityType) &&
        ((numbLoadedNode->entityNumb) == entityNumb))
    {
      numbLoadedNode->numberLoaded++;

      if ((numbLoadedNode->numberLoaded > numbLoadedNode->overrideLoaded) &&
          numbLoadedNode->overrideLoaded)
      {
        numbLoadedNode->overrideLoaded = numbLoadedNode->numberLoaded;
      }

      return;
    }

    oldNode = numbLoadedNode;
    numbLoadedNode = numbLoadedNode->Next;
  }

 // no matching node found, create a new one

  numbLoadedNode = new entityLoaded;

  if (!numbLoadedNode)
  {
    cout << "Error creating new entityLoaded struct in addEntity().  "
            "Aborting..\n";

    exit(1);
  }

  if (!numbLoadedHead) numbLoadedHead = numbLoadedNode;

  memset(numbLoadedNode, 0, sizeof(entityLoaded));

  numbLoadedNode->entityType = entityType;
  numbLoadedNode->entityNumb = entityNumb;
  numbLoadedNode->numberLoaded = 1;

 // add to end of list

  if (oldNode) oldNode->Next = numbLoadedNode;
}


//
// setEntityOverride
//

void setEntityOverride(const char entityType, const ulong entityNumb,
                       const long override, const long globallimit)
{
  entityLoaded *numbLoadedNode = numbLoadedHead, *oldNode = NULL;


 // first, check to see if node referencing this entity type and numb already
 // exists

  while (numbLoadedNode)
  {
    if (((numbLoadedNode->entityType) == entityType) &&
        ((numbLoadedNode->entityNumb) == entityNumb))
    {
      numbLoadedNode->overrideLoaded = override;
      numbLoadedNode->globaloverrideLoaded = globallimit;

      return;
    }

    oldNode = numbLoadedNode;
    numbLoadedNode = numbLoadedNode->Next;
  }

 // no matching node found, create a new one

  numbLoadedNode = new entityLoaded;

  if (!numbLoadedNode)
  {
    cout << "Error creating new entityLoaded struct in addEntityOverride().  "
            "Aborting..\n";

    exit(1);
  }

  if (!numbLoadedHead) numbLoadedHead = numbLoadedNode;

  memset(numbLoadedNode, 0, sizeof(entityLoaded));

  numbLoadedNode->entityType = entityType;
  numbLoadedNode->entityNumb = entityNumb;

// I have no idea why this is commented out - Veygoth
// numbLoadedNode->numberLoaded = 0;

  numbLoadedNode->overrideLoaded = override;
  numbLoadedNode->globaloverrideLoaded = globallimit;

 // add it to the list

  if (oldNode) oldNode->Next = numbLoadedNode;
}


//
// getFirstEntityOverride
//

entityLoaded *getFirstEntityOverride(entityLoaded *startNode)
{
  while (startNode)
  {
    if (startNode->overrideLoaded > 0) return startNode;
    startNode = startNode->Next;
  }

  return NULL;
}


//
// deleteEntityinLoaded : Deletes a specific entity from the list
//
//   entityType : type of entity that is being added
//   entityNumb : number of entity that is being added
//

void deleteEntityinLoaded(const char entityType, const ulong entityNumb)
{
  entityLoaded *numbLoadedNode = numbLoadedHead, *oldNode = NULL;


  while (numbLoadedNode)
  {
    if ((numbLoadedNode->entityType == entityType) &&
        (numbLoadedNode->entityNumb == entityNumb))
    {
      break;
    }

    oldNode = numbLoadedNode;
    numbLoadedNode = numbLoadedNode->Next;
  }

  if (!numbLoadedNode) return;  // couldn't find a match

  if (numbLoadedNode == numbLoadedHead) numbLoadedHead = numbLoadedHead->Next;
  else oldNode->Next = numbLoadedNode->Next;

  delete numbLoadedNode;
}


//
// decEntity : Decrements number loaded for a specific entity
//
//   entityType : type of entity that is being decremented
//   entityNumb : number of entity that is being decremented
//

void decEntity(const char entityType, const ulong entityNumb)
{
  entityLoaded *numbLoadedNode = numbLoadedHead;


  while (numbLoadedNode)
  {
    if (((numbLoadedNode->entityType) == entityType) &&
        ((numbLoadedNode->entityNumb) == entityNumb))
    {
      numbLoadedNode->numberLoaded--;
      if (numbLoadedNode->numberLoaded <= 0)
      {
        deleteEntityinLoaded(entityType, entityNumb);
      }

      return;
    }

    numbLoadedNode = numbLoadedNode->Next;
  }

 // whoopsie, no match

  _outtext("\nWarning: decEntity() - attempted to decrement non-existent entry\n\n");
}


//
// getNumbEntities : Gets the number of a specific entities loaded
//
//   entityType : type of entity
//   entityNumb : id numb of entity
//

ulong getNumbEntities(const char entityType, const ulong entityNumb,
                      const char includeOverride)
{
  entityLoaded *numbLoadedNode = numbLoadedHead;


  while (numbLoadedNode)
  {
    if (((numbLoadedNode->entityType) == entityType) &&
        ((numbLoadedNode->entityNumb) == entityNumb))
    {
      if (includeOverride && (numbLoadedNode->overrideLoaded > 0))
      {
        return (numbLoadedNode->overrideLoaded);
      }
      else return (numbLoadedNode->numberLoaded);
    }

    numbLoadedNode = numbLoadedNode->Next;
  }

  return 0;
}


//
// getNumbEntitiesNode : Gets the number of a specific entity loaded
//
//   entityType : type of entity
//   entityNumb : id numb of entity
//

entityLoaded *getNumbEntitiesNode(const char entityType, const ulong entityNumb)
{
  entityLoaded *numbLoadedNode = numbLoadedHead;


  while (numbLoadedNode)
  {
    if (((numbLoadedNode->entityType) == entityType) &&
        ((numbLoadedNode->entityNumb) == entityNumb))
    {
      return numbLoadedNode;
    }

    numbLoadedNode = numbLoadedNode->Next;
  }

  return NULL;
}


//
// loadAllinObjHereList : Adds all the entities in an objHere list to the
//                        entityLoaded list
//

void loadAllinObjHereList(objectHere *objHead)
{
  objectHere *objHere = objHead;

  while (objHere)
  {
    if (objHere->objInside) loadAllinObjHereList(objHere->objInside);

    addEntity(ENTITY_OBJECT, objHere->objectNumb);

    objHere = objHere->Next;
  }
}


//
// loadAllinMobHereList : Same as above, only for mobHere list
//

void loadAllinMobHereList(mobHere *mobHead)
{
  mobHere *mob = mobHead;

  while (mob)
  {
    addEntity(ENTITY_MOB, mob->mobNumb);

    mob = mob->Next;
  }
}


//
// resetNumbLoaded : Looks for an entityLoaded record in the numbLoadedNode
//                   list that matches the type and oldNumb, and changes the
//                   entityNumb to newNumb
//

void resetNumbLoaded(const char entityType, const long oldNumb,
                     const long newNumb)
{
  entityLoaded *numbLoadedNode = numbLoadedHead;


  while (numbLoadedNode)
  {
    if ((numbLoadedNode->entityType == entityType) &&
        (numbLoadedNode->entityNumb == oldNumb))
    {
      numbLoadedNode->entityNumb = newNumb;
      return;
    }

    numbLoadedNode = numbLoadedNode->Next;
  }
}


//
// displayLoadedList
//

void displayLoadedList(const char *args)
{
  entityLoaded *node = numbLoadedHead;
  ulong i = 0;
  long numb = -1;
  char strn[256], entStrn[256];


  _setbkcolor(0);

  if (!node)
  {
    _outtext("\nThere are currently no entries in the numbLoaded list.\n\n");

    return;
  }

  if (strnumer(args)) numb = atoi(args);

  _outtext("\n\n");

  while (node)
  {
    if (numb >= 0)
    {
      if (node->entityNumb == numb)
      {
        switch (node->entityType)
        {
          case ENTITY_OBJECT  : strcpy(entStrn, "(object)");  break;
          case ENTITY_MOB     : strcpy(entStrn, "(mob)   ");  break;

          default : sprintf(entStrn, "(unrec type (%d))", node->entityType);
        }

        if (node->overrideLoaded)
        {
          sprintf(strn, "type: %d %s  vnum: %d  numb loaded: %u  override: %u  global limit: %u\n",
                  node->entityType, entStrn, node->entityNumb, node->numberLoaded,
                  node->overrideLoaded, node->globaloverrideLoaded );
        }
        else
        {
          sprintf(strn, "type: %d %s  vnum: %d  numb loaded: %u  global limit: %u\n",
                  node->entityType, entStrn, node->entityNumb, node->numberLoaded,
                  node->globaloverrideLoaded );
        }

        displayColorString(strn);
        i++;
      }
    }
    else
    {
      switch (node->entityType)
      {
        case ENTITY_OBJECT  : strcpy(entStrn, "(object)");  break;
        case ENTITY_MOB     : strcpy(entStrn, "(mob)   ");  break;

        default : sprintf(entStrn, "(unrec type (%d))", node->entityType);
      }

      if (node->overrideLoaded)
      {
        sprintf(strn, "type: %d %s  vnum: %d  numb loaded: %u  override: %u  global limit: %u\n",
                node->entityType, entStrn, node->entityNumb, node->numberLoaded,
                node->overrideLoaded, node->globaloverrideLoaded);
      }
      else
      {
        sprintf(strn, "type: %d %s   vnum: %d   numb loaded: %u  global limit: %u\n",
                node->entityType, entStrn, node->entityNumb, node->numberLoaded,
                node->globaloverrideLoaded);
      }

      displayColorString(strn);

      i += numbLinesStringNeeds(strn);
    }

    if (i >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      i = 0;
    }

    node = node->Next;
  }

  _outtext("\n");
}


//
// setLimitArgs : sets override limit
//

void setLimitArgs(const char *args)
{
  char args2[512];

  strcpy(args2, args);

  remLeadingSpaces(args2);
  upfirstarg(args2);

  checkCommands(args2, setLimitCmdHead,
                "\nSpecify one of OBJECT or MOB as the second argument.\n\n",
                setLimitExecCommand, FALSE);
}


//
// setLimitArgsStartup : sets override limit
//

void setLimitArgsStartup(const char *args)
{
  char args2[512];

  strcpy(args2, args);

  remLeadingSpaces(args2);
  upfirstarg(args2);

  checkCommands(args2, setLimitCmdHead,
                "\nSpecify one of OBJECT or MOB as the second argument.\n\n",
                setLimitExecCommandStartup, FALSE);
}


//
// setLimitOverrideObj : args passed should be <vnum> <limit>
//

void setLimitOverrideObj(const char *args, const char updateChanges)
{
  char arg1[256], arg2[256];
  ulong i, i2;


  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (!strnumer(arg1) || !strnumer(arg2))
  {
    _outtext("\nThe second and third args should be the vnum and the limit.\n\n");
    return;
  }

  i = atoi(arg1);
  i2 = atoi(arg2);

  setEntityOverride(ENTITY_OBJECT, i, i2, 0);

  if (display)
  {
    sprintf(arg1, "\n"
"Objects of type #%u (if any) will now be written to the zone file with\n"
"a limit of %u.\n\n",
            i, i2);

    displayColorString(arg1);
  }

  if (updateChanges) madeChanges = TRUE;
}


//
// setLimitOverrideMob : args passed should be <vnum> <limit>
//

void setLimitOverrideMob(const char *args, const char updateChanges)
{
  char arg1[256], arg2[256];
  ulong i, i2;


  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (!strnumer(arg1) || !strnumer(arg2))
  {
    _outtext("\nThe second and third args should be the vnum and the limit.\n\n");
    return;
  }

  i = atoi(arg1);
  i2 = atoi(arg2);

  setEntityOverride(ENTITY_MOB, i, i2, 0);

  if (display)
  {
    sprintf(arg1, "\n"
"Mobs of type #%u (if any) will now be written to the zone file with a\n"
"limit of %u.\n\n",
            i, i2);

    displayColorString(arg1);
  }

  if (updateChanges) madeChanges = TRUE;
}
