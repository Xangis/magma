//
//  File: misc.cpp       originally part of dikuEdit
//
//  Usage: truly miscellaneous functions that fit nowhere else
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#ifdef __UNIX__
#  include <unistd.h>
#else
#  include <io.h>
#endif

#include "../types.h"
#include "../fh.h"
#include "../de.h"

#include "../room/room.h"
#include "../obj/object.h"
#include "../mob/mob.h"
#include "../command/alias.h"
#include "../keys.h"
#include "../vardef.h"
#include "../misc/loaded.h"

#include "../misc/misc.h"

using namespace std;

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

extern char madeChanges;
extern dikuRoom *currentRoom;
extern variable *varHead;
extern char promptString[256];
extern ulong numbRooms, numbExits, numbObjTypes, numbObjs, numbMobTypes, numbMobs,
             lowestRoomNumber, highestRoomNumber, lowestObjNumber, highestObjNumber,
             lowestMobNumber, highestMobNumber;
extern char roomOnly, objOnly, mobOnly, readFromSubdirs, zoneLoadedWithQuest, zoneLoadedWithShop;
extern ulong numbLookupEntries;
extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern dikuMob *mobHead;
extern dikuZone zoneRec;
extern dikuRoom **roomLookup;
extern dikuObject **objLookup;
extern dikuMob **mobLookup;
extern command *lookupCmdHead;

//
// getScreenHeight : returns number of lines 'screen' has
//

INLINEDEF long getScreenHeight(void)
{
  return getScreenHeightVal();
}


//
// getScreenWidth : returns number of columns 'screen' has
//

INLINEDEF long getScreenWidth(void)
{
  return getScreenWidthVal();
}


//
// numbLinesStringNeeds
//

INLINEDEF ulong numbLinesStringNeeds(const char *strn)
{
  ulong truelen = truestrlen(strn), lines;

  lines = (truelen / getScreenWidth()) + 1;

  if (truelen && ((truelen % getScreenWidth()) == 0)) lines--;

  return lines;
}


//
// confirmChanges : Checks madeChanges and checks to see if the user wants to
//                  save em or not - returns TRUE if everything was successful,
//                  FALSE otherwise
//

char confirmChanges(void)
{
  usint ch;

  if (madeChanges ||
      ((getStartRoomActiveVal()) &&
       (getRoomStartVal() != currentRoom->roomNumber)))
  {
    _outtext("\n\nChanges were made - save them (Y/n)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'N')
    {
      _outtext("Yes");

      if (writeFiles()) return FALSE;
    }
    else _outtext("No");
  }

  return TRUE;
}


//
// writeFiles : writes all valid info to their respective files..  it's great
//              returns TRUE if an error was hit, FALSE otherwise
//

char writeFiles(void)
{
  char strn[256];
  FILE *file;


  if (roomHasAllFollowers() >= 0)
  {
    sprintf(strn, "\n"
"error: room #%u has no non-followers.  At least one mob must be a\n"
"       non-follower.  Aborting save..\n\n", roomHasAllFollowers());

    _outtext(strn);

    return TRUE;
  }

  if (checkAllShops()) return TRUE;

  if (getCheckSaveVal())
  {
    if (checkAll())
    {
      if (getNoSaveonCheckErrVal())
      {
        _outtext("\nErrors found while checking entities - aborting...\n\n");
        return TRUE;
      }
      else _outtext("\nErrors found while checking entities - continuing...\n\n");
    }
  }
  else _outtext("\n\n");

//  verifyZoneFlags();  // shouldn't be necessary

  writeZoneFile();
  writeWorldFile();
  writeObjectFile();
  writeMobFile();


  if (checkForMobWithQuest()) writeQuestFile();
  else

 // only delete .QST if necessary

  if (zoneLoadedWithQuest)
  {
    if (readFromSubdirs) strcpy(strn, "qst/");
    else strn[0] = '\0';

    strcat(strn, getMainZoneNameStrn());
    strcat(strn, ".qst");

   // go through various functions in an attempt to purge the damn file

// Commented out by Veygoth - unlink does not exist.
/*    if (unlink(strn))
    {
      if (remove(strn))
      {
        file = fopen(strn, "wt");
        if (!file)
        {
          _outtext("\nError deleting/truncating ");
          _outtext(strn);
          _outtext("...\n");
        }
        else fclose(file);
      }
    }
*/
  }


 // check all shops to make sure mob loaded as shop has all the shit it needs
 // in its inventory

  if (checkForMobWithShop())
  {
//    if (!checkAllShops()) writeShopFile();
//    else return FALSE;
    writeShopFile();
  }
  else

 // only delete .SHP if necessary

  if (zoneLoadedWithShop)
  {
    if (readFromSubdirs) strcpy(strn, "shp/");
    else strn[0] = '\0';

    strcat(strn, getMainZoneNameStrn());
    strcat(strn, ".shp");

// Unlink does not exist - Veygoth
/*    if (unlink(strn))
    {
      if (remove(strn))
      {
        file = fopen(strn, "wt");

        if (!file)
        {
          _outtext("\nError deleting/truncating ");
          _outtext(strn);
          _outtext("...\n");
        }
        else fclose(file);
      }
    }
*/
  }

  writeDefaultFiles(getMainZoneNameStrn());

  strcpy(strn, getMainZoneNameStrn());
  strcat(strn, ".set");
  writeSettingsFile(strn, TRUE);  // write zonename.set

  _outtext("\n");

 // prevent extraneous saves

  if (getStartRoomActiveVal())
  {
    itoa(currentRoom->roomNumber, strn, 10);
    addVar(&varHead, VAR_STARTROOM_NAME, strn);
  }

  madeChanges = FALSE;  // golly

  return FALSE;
}


//
// createPrompt : creates the prompt
//

void createPrompt(void)
{
  if (!strcmp(getMainPromptStrn(), "default"))
  {
    sprintf(promptString, "&n&+g< %ur %ue %uot %uol %umt %uml >\n<> &n",
            numbRooms, numbExits, numbObjTypes, numbObjs,
            numbMobTypes, numbMobs);
  }
  else
  {
//    strcpy(promptString, getMainPromptStrn());
//    strcat(promptString, "\n&n&+g<> &n");
    sprintf(promptString, "%s\n&n&+g<> &n", getMainPromptStrn());
  }
}


//
// displayRecordSizeInfo
//

void displayRecordSizeInfo(void)
{
  char strn[2048];


  sprintf(strn,
"\n"
"\n"
"dikuRoom   : %u\n"
"dikuObject : %u\n"
"dikuMob    : %u\n"
"dikuZone   : %u\n"
"quest      : %u\n"
"shop       : %u\n"
"\n"
"roomExit   : %u\n"
"extraDesc  : %u\n"
"objHere    : %u\n"
"mobHere    : %u\n"
"\n"
"masterNode : %u\n"
"editNode   : %u\n"
"stringNode : %u\n"
"entityLoad : %u\n"
"\n"
"&+CPress any key to continue..&n",

sizeof(dikuRoom),
sizeof(dikuObject),
sizeof(dikuMob),
sizeof(dikuZone),
sizeof(quest),
sizeof(shop),

sizeof(roomExit),
sizeof(extraDesc),
sizeof(objectHere),
sizeof(mobHere),

sizeof(masterKeywordListNode),
sizeof(editableListNode),
sizeof(stringNode),
sizeof(entityLoaded));

  displayColorString(strn);
  getkey();

  sprintf(strn,
"\n"
"\n"
"command    : %u\n"
"alias      : %u\n"
"variable   : %u\n"
"\n",

sizeof(command),
sizeof(alias),
sizeof(variable));

  _outtext(strn);
}


//
// getOnOffStrn : whee
//

INLINEDEF const char *getOnOffStrn(const uchar value)
{
  if (value) return "on";
  else return "off";
}


//
// getYesNoStrn : whee
//

INLINEDEF const char *getYesNoStrn(const uchar value)
{
  if (value) return "yes";
  else return "no";
}


//
// getYesNoStrn : pointer version
//

INLINEDEF const char *getYesNoStrn(const void *ptr)
{
  if (ptr) return "yes";
  else return "no";
}


//
// plural
//

INLINEDEF const char *plural(const long numb)
{
  if (numb == 1) return "";
  else return "s";
}


//
// getVowelN
//

INLINEDEF const char *getVowelN(const char ch)
{
  switch (toupper(ch))
  {
    case 'A':
    case 'E':
    case 'I':
    case 'O':
    case 'U': return "n";

    default : return "";
  }
}


//
// checkPreArgs
//

void checkPreArgs(const int argc, const char *argv[])
{
  ulong i;
  char strn[1024];

  for (i = 0; i < argc; i++)
  {
    strcpy(strn, argv[i]);

    if ((strn[0] == '-') || (strn[0] == '/')) deleteChar(strn, 0);
    else continue;

    upstrn(strn);

    if ((!strcmp(strn, "READDIKUDIR")) ||
        (!strcmp(strn, "RDD")) ||
        (!strcmp(strn, "D")))
    {
      readFromSubdirs = TRUE;
    }
  }
}


//
// checkArgs
//

void checkArgs(const int argc, const char *argv[])
{
  ulong i;
  char strn[1024];

  for (i = 0; i < argc; i++)
  {
    strcpy(strn, argv[i]);

    if ((strn[0] == '-') || (strn[0] == '/')) deleteChar(strn, 0);
    else continue;

    upstrn(strn);

    if ((!strcmp(strn, "NOVNUMCHECK")) ||
        (!strcmp(strn, "NVC")) ||
        (!strcmp(strn, "NOVC")))
    {
      setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, FALSE, FALSE);
    }
    else
    if ((!strcmp(strn, "VNUMCHECK")) ||
        (!strcmp(strn, "VC")))
    {
      setVarBoolVal(&varHead, VAR_VNUMCHECK_NAME, TRUE, FALSE);
    }
    else
    if ((!strcmp(strn, "NOEQCLASSCHECK")) ||
        (!strcmp(strn, "NECC")) ||
        (!strcmp(strn, "NOECC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKMOBCLASSEQ_NAME, FALSE, FALSE);
    }
    else
    if ((!strcmp(strn, "EQCLASSCHECK")) ||
        (!strcmp(strn, "ECC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKMOBCLASSEQ_NAME, TRUE, FALSE);
    }
    else
    if ((!strcmp(strn, "OBJAFFECT")) ||
        (!strcmp(strn, "OA")))
    {
      setVarBoolVal(&varHead, VAR_OBJAFFECT_NAME, TRUE, FALSE);
    }
    else
    if ((!strcmp(strn, "EDITALLFLAGS")) ||
        (!strcmp(strn, "EAF")))
    {
      setVarBoolVal(&varHead, VAR_EDITUNEDITABLEFLAGS_NAME, TRUE, FALSE);
    }
    else
    if ((!strcmp(strn, "NOLIMITCHECK")) ||
        (!strcmp(strn, "NLC")) ||
        (!strcmp(strn, "NOLC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKLIMITS_NAME, FALSE, FALSE);
    }
    else
    if ((!strcmp(strn, "LIMITCHECK")) ||
        (!strcmp(strn, "LC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKLIMITS_NAME, TRUE, FALSE);
    }
    else
    if ((!strcmp(strn, "NOZONEFLAGCHECK")) ||
        (!strcmp(strn, "NZFC")) ||
        (!strcmp(strn, "NOZFC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKZONEFLAGS_NAME, FALSE, FALSE);
    }
    else
    if ((!strcmp(strn, "ZONEFLAGCHECK")) ||
        (!strcmp(strn, "ZFC")))
    {
      setVarBoolVal(&varHead, VAR_CHECKZONEFLAGS_NAME, TRUE, FALSE);
    }

/*
    if ((!strcmp(strn, "READDIKUDIR")) ||
        (!strcmp(strn, "RDD")) ||
        (!strcmp(strn, "D")))
    {
      readFromSubdirs = TRUE;
    }
*/
    else
    if ((!strcmp(strn, "IGNOREZONES")) ||
        (!strcmp(strn, "IZS")))
    {
      setVarBoolVal(&varHead, VAR_IGNOREZONES_NAME, TRUE, FALSE);
    }
    else
    if (!strcmp(strn, "R"))
    {
      roomOnly = TRUE;
    }
    else
    if (!strcmp(strn, "O"))
    {
      objOnly = TRUE;
    }
    else
    if (!strcmp(strn, "M"))
    {
      mobOnly = TRUE;
    }
    else
    if (strleft(strn, "LL="))
    {
      while (strn[0])
      {
        deleteChar(strn, 0);
        if (strn[0] == '=')
        {
          deleteChar(strn, 0);
          break;
        }
      }

      if (strn[0] && strnumer(strn) && atoi(strn))
        numbLookupEntries = atoi(strn);
    }
  }
}


//
// renumberCurrent
//

void renumberCurrent(void)
{
  char strn[256];


  renumberRooms(FALSE, 0);

  sprintf(strn, "\nRooms have been renumbered (starting at %u).\n",
          roomHead->roomNumber);
  _outtext(strn);

  if (objectHead)
  {
    renumberObjs(FALSE, 0);

    sprintf(strn, "Objects have been renumbered (starting at %u).\n",
            objectHead->objNumber);
    _outtext(strn);
  }

  if (mobHead)
  {
    renumberMobs(FALSE, 0);

    sprintf(strn, "Mobs have been renumbered (starting at %u).\n",
            mobHead->mobNumber);
    _outtext(strn);
  }

  _outtext("\n");
}


//
// renumberAll : renumbers everything starting from new vnum
//

void renumberAll(const char *args)
{
  char strn[256];   // used for output

  long usernumb,    // number specified in args
       oldNumb,     // old starting number of whatever list
       tempstart;   // list is temporarily renumbered to this to prevent bugs


  if (strlen(args) == 0)
  {
    renumberCurrent();
    return;
  }

  if (!strnumer(args))
  {
    _outtext(
"\nThe 'renumber' command's first argument must be a positive number.\n\n");
    return;
  }

  usernumb = atoi(args);

  if (roomHead)
  {

   // make sure we're not going over the limit (with renumbering up one to
   // prevent bugs or with the end result)

    tempstart = getHighestRoomNumber() + 1;

    if (((usernumb + numbRooms) >= numbLookupEntries) ||
        ((tempstart + numbRooms) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber rooms due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = roomHead->roomNumber;
      roomHead->roomNumber = tempstart;
      renumberRooms(TRUE, oldNumb);

      oldNumb = roomHead->roomNumber;
      roomHead->roomNumber = usernumb;
      renumberRooms(TRUE, oldNumb);
    }
  }

  if (objectHead)
  {
    tempstart = getHighestObjNumber() + 1;

    if (((usernumb + numbObjTypes) >= numbLookupEntries) ||
        ((tempstart + numbObjTypes) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber objs due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = objectHead->objNumber;
      objectHead->objNumber = tempstart;
      renumberObjs(TRUE, oldNumb);

      oldNumb = objectHead->objNumber;
      objectHead->objNumber = usernumb;
      renumberObjs(TRUE, oldNumb);
    }
  }

  if (mobHead)
  {
    tempstart = getHighestMobNumber() + 1;

    if (((usernumb + numbMobTypes) >= numbLookupEntries) ||
        ((tempstart + numbMobTypes) >= numbLookupEntries))
    {
      sprintf(strn, "\n"
"You cannot renumber mobs due to the current max vnum limit of %u -\n"
"to remedy, load dikuEdit with the -ll=<loaded> switch to set a higher limit.\n\n",
              numbLookupEntries - 1);

      _outtext(strn);
    }
    else
    {
      oldNumb = mobHead->mobNumber;
      mobHead->mobNumber = tempstart;
      renumberMobs(TRUE, oldNumb);

      oldNumb = mobHead->mobNumber;
      mobHead->mobNumber = usernumb;
      renumberMobs(TRUE, oldNumb);
    }
  }

  sprintf(strn,
"\nAll rooms, objects, and mobs renumbered starting at %u.\n\n", usernumb);

  _outtext(strn);
}


//
// lookup
//

void lookup(const char *args)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;

  char outStrn[512], tempStrn[512],
       foundSomething = FALSE, foundRoom = FALSE, foundObj = FALSE,
       displayedEqual = FALSE, vnum;

  ulong i = 0;//, len;
  long num;


  _setbkcolor(0);

  if (strlen(args) == 0)
  {
    _outtext("\nThe \"lookup\" command requires at least one argument.\n\n");
    return;
  }

  if (numbArgs(args) >= 2)
  {
    checkCommands(args, lookupCmdHead,
"\nSpecify one of ROOM, OBJ, or MOB as the first argument.\n\n",
                  lookupExecCommand, FALSE);

    return;
  }

  vnum = strnumer(args);
  if (vnum) num = atoi(args);

  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    if (!vnum)
    {
      strcpy(tempStrn, room->roomName);
      upstrn(tempStrn);
    }

    if ((!vnum && strstr(tempStrn, args)) ||
        (vnum && (room->roomNumber == num)))
    {
      sprintf(outStrn, "&n%s&n (#%u)\n", room->roomName, room->roomNumber);

      foundSomething = foundRoom = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

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
//        oneScreen = FALSE;
      }
    }

    room = room->Next;
  }

  while (obj)
  {
    if ((!vnum && scanKeyword(args, obj->keywordListHead)) ||
        (vnum && (obj->objNumber == num)))
    {
      if (foundRoom && !displayedEqual)
      {
        _outtext("===\n");
        i++;
        displayedEqual = TRUE;
      }

      sprintf(outStrn, "%s&n (#%u)\n", obj->objShortName, obj->objNumber);

      foundSomething = foundObj = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

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
    }

    obj = obj->Next;
  }

  displayedEqual = FALSE;

  while (mob)
  {
    if ((!vnum && scanKeyword(args, mob->keywordListHead)) ||
        (vnum && (mob->mobNumber == num)))
    {
      if (foundObj && !displayedEqual)
      {
        _outtext("===\n");
        i++;
        displayedEqual = TRUE;
      }

      sprintf(outStrn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);

      foundSomething = TRUE;
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

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
    }

    mob = mob->Next;
  }

  if (!foundSomething) _outtext("No matching rooms/objects/mobs found.\n");
  _outtext("\n");
}


//
// where
//

void where(const char *keyStrn)
{
  dikuRoom *room = roomHead;
  dikuObject *obj;
  mobHere *mob;
  questItem *qitem;
  questQuest *qquest;
  char strn[512], mobName[512], foundMatch = FALSE,
       vnum = strnumer(keyStrn), match2 = FALSE;
  ulong i = 0;
  long num, j;


  if (vnum)
  {
    num = atoi(keyStrn);
    i = 0;
  }

  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    sprintf(strn, " in %s&n (#%u)",
            room->roomName, room->roomNumber);

    if (!vnum)
    {
      if (displayEntireObjectHereList(room->objectHead, strn, &i, keyStrn,
                                      room->roomNumber, &foundMatch))
       return;
    }
    else
    {
      if (displayEntireObjectHereList(room->objectHead, strn, &i, num,
                                      room->roomNumber, &foundMatch))
       return;
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

    mob = room->mobHead;
    while (mob)
    {
      if (vnum || mob->mobPtr)
      {
        if (mob->mobPtr) strcpy(mobName, mob->mobPtr->mobShortName);
        else strcpy(mobName, "(mob not in this .MOB file)");

        if
((!vnum && scanKeyword(keyStrn, mob->mobPtr->keywordListHead)) ||
  (vnum && (mob->mobNumb == num)))
        {
          sprintf(strn, "%s&n (#%u) in %s&n (#%u)\n",
                  mobName, mob->mobNumb,
                  room->roomName, room->roomNumber);
          displayColorString(strn);

          i += numbLinesStringNeeds(strn);

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

          foundMatch = TRUE;
        }

       // check quests for items given to PCs

        if (mob->mobPtr && mob->mobPtr->questPtr)
        {
          qquest = mob->mobPtr->questPtr->questHead;

          while (qquest)
          {
            qitem = qquest->questPlayRecvHead;
            while (qitem)
            {
              if (vnum)
              {
                if (qitem->itemVal == num)
                {
                  obj = findObj(qitem->itemVal);

                  sprintf(strn,
"%s&n (#%d) given by %s&n (#%u)'s quest in %s&n (#%u)\n",
                          obj ? obj->objShortName :
                          "(object not in this .OBJ file)",
                          num, mobName, mob->mobNumb,
                          room->roomName, room->roomNumber);

                  match2 = TRUE;
                }
              }
              else
              {
                obj = findObj(qitem->itemVal);

                if (obj && scanKeyword(keyStrn, obj->keywordListHead))
                {
                  sprintf(strn,
"%s&n (#%d) given by %s&n (#%u)'s quest in %s&n (#%u)\n",
                          obj->objShortName, qitem->itemVal,
                          mobName, mob->mobNumb,
                          room->roomName, room->roomNumber);

                  match2 = TRUE;
                }
              }

              if (match2)
              {
                displayColorString(strn);
                i += numbLinesStringNeeds(strn);

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

                foundMatch = TRUE;
                match2 = FALSE;
              }

              qitem = qitem->Next;
            }

            qquest = qquest->Next;
          }
        }

       // check for shops that sell item

        if (mob->mobPtr && mob->mobPtr->shopPtr)
        {
          for (j = 0; j < MAX_NUMBSHOPITEMS; j++)
          {
            if (vnum)
            {
              if (mob->mobPtr->shopPtr->producedItemList[j] == num)
              {
                obj = findObj(mob->mobPtr->shopPtr->producedItemList[j]);

                sprintf(strn,
"%s&n (#%d) sold by %s&n (#%u)'s shop in %s&n (#%u)\n",
                        obj ? obj->objShortName :
                        "(object not in this .OBJ file)",
                        num, mobName, mob->mobNumb,
                        room->roomName, room->roomNumber);

                match2 = TRUE;
              }
            }
            else
            {
              obj = findObj(mob->mobPtr->shopPtr->producedItemList[j]);

              if (obj && scanKeyword(keyStrn, obj->keywordListHead))
              {
                sprintf(strn,
"%s&n (#%u) sold by %s&n (#%u)'s shop in %s&n (#%u)\n",
                        obj->objShortName,
                        obj->objNumber,
                        mobName, mob->mobNumb,
                        room->roomName, room->roomNumber);

                match2 = TRUE;
              }
            }

            if (match2)
            {
              displayColorString(strn);
              i += numbLinesStringNeeds(strn);

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

              foundMatch = TRUE;
              match2 = FALSE;
            }
          }
        }
      }
//      else strcpy(mobName, "(mob type not in this .MOB file)");

      sprintf(strn, " equipped by %s&n (#%u)  [#%u]",
              mobName, mob->mobNumb, room->roomNumber);

      if (!vnum)
      {
        for (j = WEAR_LOW; j <= WEAR_TRUEHIGH; j++)
          if (displayEntireObjectHereList(mob->equipment[j], strn, &i, keyStrn,
                                          room->roomNumber, &foundMatch))
           return;
      }
      else
      {
        for (j = WEAR_LOW; j <= WEAR_TRUEHIGH; j++)
          if (displayEntireObjectHereList(mob->equipment[j], strn, &i, num,
                                          room->roomNumber, &foundMatch))
           return;
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

      sprintf(strn, " carried by %s&n (#%u)  [#%u]",
              mobName, mob->mobNumb, room->roomNumber);

      if (!vnum)
      {
        if (displayEntireObjectHereList(mob->objectHead, strn, &i, keyStrn,
                                        room->roomNumber, &foundMatch))
         return;
      }
      else
      {
        if (displayEntireObjectHereList(mob->objectHead, strn, &i, num,
                                        room->roomNumber, &foundMatch))
         return;
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

      mob = mob->Next;
    }

    room = room->Next;
  }

  if (!foundMatch) _outtext("No matching mobs or objects found.\n");

/*
  if (i)
  {
    displayColorString("\n&+CPress any key to continue..&n");
    getkey();
  }
*/

  _outtext("\n");
}


//
// dumpTextFile
//

void dumpTextFile(const char *filename, const char showColor)
{
  FILE *file;
  char strn[2048];
  ulong i = 0;


  if ((file = fopen(filename, "rt")) == NULL)
  {
    _outtext("\nFilename specified not found.\n\n");
    return;
  }

  while (!feof(file))
  {
    fgets(strn, 2048, file);
    if (feof(file)) break;

    if (showColor) displayColorString(strn);
    else _outtext(strn);

    _settextcolor(7);
    _setbkcolor(0);

    if (showColor) i += numbLinesStringNeeds(strn);
    else i += (strlen(strn) / getScreenWidth()) + 1;  // good enough

    if (i >= (getScreenHeight() - 3))
    {
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          fclose(file);
          return;
        }

        _outtext("\n\n");

        i = 0;
      }
    }
  }

  fclose(file);
}


//
// displayVersionInfo
//

INLINEDEF void displayVersionInfo(void)
{
  displayColorString(
"\n&+YdikuEdit v" DE_VERSION "&n, originally by Michael Glosenger and modified for Basternae by Veygoth.\n\n");
}


//
// verifyZoneFlags
//

void verifyZoneFlags(void)
{
  dikuRoom *room;
  struct rccoord coords;
  char strn[64], bigstrn[256];


 // check that zone number isn't 0 (0 is bad)

  if (zoneRec.zoneNumber == 0)
  {
    _setbkcolor(0);

    displayColorString("\n&+C"
"Zone's zone number is equal to 0 - specify a valid zone number:&n ");

    coords = _gettextposition();

    if (roomHead->zoneNumber) itoa(roomHead->zoneNumber, bigstrn, 10);
    else strcpy(bigstrn, "1");

    while (TRUE)
    {
      getStrn(strn, 10, 1, 7, '°', bigstrn, FALSE, FALSE);

      if ((atoi(strn) == 0) || !strnumer(strn))
        _settextposition(coords.row, coords.col);
      else break;
    }

    zoneRec.zoneNumber = atoi(strn);

    madeChanges = TRUE;

    _settextcolor(7);
    _setbkcolor(0);
  }

  room = roomHead;

  if (roomHead->zoneNumber == 0) roomHead->zoneNumber = zoneRec.zoneNumber;

 // if "check zone flags" var is set, check all room zone flags vs. zone
 // numb flag

  if (getCheckZoneFlagsVal())
  {
    while (room)
    {
/* Disabled comparing because Basternae uses this value as the extended
// terrain info on a worldmap
      if (room->zoneNumber != zoneRec.zoneNumber)
      {
        sprintf(bigstrn, "\n"
  "Warning - zone flag of room #%u doesn't match zone flag of zone (room is\n"
  "          %u, zone is %u).  Setting room flag equal to %u...\n\n",
        room->roomNumber, room->zoneNumber, zoneRec.zoneNumber,
        zoneRec.zoneNumber);

        _outtext(bigstrn);

        getkey();

        room->zoneNumber = zoneRec.zoneNumber;

        madeChanges = TRUE;
      }
*/
      room = room->Next;
    }
  }
}


//
// getBoolYesNo : if strn == 'Y', TRUE, if 'N', FALSE, otherwise, -1
//

INLINEDEF char getBoolYesNo(const char *strn)
{
  if ((!strcmp(strn, "Y")) || (!strcmp(strn, "1"))) return TRUE;
  if ((!strcmp(strn, "N")) || (!strcmp(strn, "0"))) return FALSE;
  return -1;
}


//
// getYesNoBool : if TRUE, return 'Y', if FALSE, return 'N'
//

INLINEDEF const char *getYesNoBool(const char boolean)
{
  if (boolean) return "Y";
  else return "N";
}


//
// toggleGuildVar
//

void toggleGuildVar(void)
{
  if (!varExists(varHead, VAR_CHECKGUILDSTUFF_NAME))
  {
//    setVarBoolVal(&varHead, VAR_CHECKGUILDSTUFF_NAME, TRUE, TRUE);
    setVarArgs(VAR_CHECKGUILDSTUFF_NAME " TRUE", TRUE, TRUE);
  }
  else
    toggleVar(&varHead, VAR_CHECKGUILDSTUFF_NAME);
}


//
// at
//

void at(const char *args)
{
  char atwhere[256], strn[256];
  dikuRoom *room, *oldRoom;

  if (!strlen(args))
  {
    _outtext("\nAt where do what?  Eh?\n\n");
    return;
  }

  getArg(args, 1, atwhere);

  room = getRoomKeyword(atwhere, TRUE);
  if (!room)
  {
    _outtext("\nCouldn't find a room that matched argument one.\n\n");
    return;
  }

 // get rid of arg1

  strcpy(strn, args);

  while (strn[0] != ' ')
  {
    deleteChar(strn, 0);
    if (strn[0] == '\0')
    {
      _outtext("\nSpecify a command to execute at the specified location.\n\n");
      return;
    }
  }

  remLeadingSpaces(strn);

  oldRoom = currentRoom;
  currentRoom = room;

  interpCommands(strn, FALSE);

  currentRoom = oldRoom;
}


//
// displayLookupList
//

void displayLookupList(const char whichLookup, ulong start)
{
  ulong i = 0, j, low, high;
  char strn[256];


  _setbkcolor(0);

  if ((whichLookup < ROOM_LOOKUP) || (whichLookup > MOB_LOOKUP)) return;

  switch (whichLookup)
  {
    case ROOM_LOOKUP : low = getLowestRoomNumber();  high = getHighestRoomNumber();
                       break;
    case OBJ_LOOKUP  : if (!numbObjTypes)
                       {
                         _outtext("\nThere are currently no object types.\n\n");
                         return;
                       }
                       low = getLowestObjNumber();  high = getHighestObjNumber();
                       break;
    case MOB_LOOKUP  : if (!numbMobTypes)
                       {
                         _outtext("\nThere are currently no mob types.\n\n");
                         return;
                       }
                       low = getLowestMobNumber();  high = getHighestMobNumber();
                       break;
  }

  if (start < low) start = low;
  if (start > high)
  {
    sprintf(strn, "\n\nStarting vnum specified too high - reset to %u.", low);
    _outtext(strn);

    start = low;
    i = 3;
  }

  _outtext("\n\n");

  for (j = start; j <= high; j++)
  {
    switch (whichLookup)
    {
      case ROOM_LOOKUP : if (roomLookup[j])
                           sprintf(strn, "  room #%u - 0x%x (#%u)\n",
                                   j, roomLookup[j], roomLookup[j]->roomNumber);
                         else strn[0] = '\0';
                         break;
      case OBJ_LOOKUP  : if (objLookup[j])
                           sprintf(strn, "  object #%u - 0x%x (#%u)\n",
                                   j, objLookup[j], objLookup[j]->objNumber);
                         else strn[0] = '\0';
                         break;
      case MOB_LOOKUP  : if (mobLookup[j])
                           sprintf(strn, "  mob #%u - 0x%x (#%u)\n",
                                   j, mobLookup[j], mobLookup[j]->mobNumber);
                         else strn[0] = '\0';
                         break;
    }

    if (strn[0])
    {
      displayColorString(strn);
      i += numbLinesStringNeeds(strn);

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
    }
  }

  _outtext("\n");
}


//
// preDisplayLookupList
//

void preDisplayLookupList(const char *args)
{
  char arg1[256], arg2[256];
  ulong numb;

  getArg(args, 1, arg1);
  getArg(args, 2, arg2);

  if (!strlen(arg1))
  {
    _outtext("\nThe third argument should be one of <room|object|mob>.\n\n");
    return;
  }

  if (strlen(arg2))
  {
    if (!strnumer(arg2))
    {
      _outtext("\nThe fourth argument should be the vnum to start the list at.\n\n");
      return;
    }

    numb = atoi(arg2);
  }
  else numb = 0;

  if (strleft("ROOM", arg1)) displayLookupList(ROOM_LOOKUP, numb);
  else if (strleft("OBJECT", arg1)) displayLookupList(OBJ_LOOKUP, numb);
  else if (strleft("MOB", arg1) || strleft("CHAR", arg1)) displayLookupList(MOB_LOOKUP, numb);
  else _outtext("\nThe second argument should be one of <room|object|mob>.\n\n");
}


//
// displayMiscInfo
//

void displayMiscInfo(void)
{
  char strn[2048];

  sprintf(strn,
"\n"
"Vnum range    : 0-%u\n"
"\n"
"Room low vnum : %u\n"
"Room high vnum: %u\n"
"\n"
"Obj low vnum  : %u\n"
"Obj high vnum : %u\n"
"\n"
"Mob low vnum  : %u\n"
"Mob high vnum : %u\n"
"\n", numbLookupEntries - 1,
      lowestRoomNumber, highestRoomNumber,
      lowestObjNumber, highestObjNumber,
      lowestMobNumber, highestMobNumber);

  _outtext(strn);
}


//
// getMoneyStrn
//

char *getMoneyStrn(ulong coins, char *strn)
{
  char catstrn[256];


  if (!coins) strcpy(strn, "0&+yc&n");
  else
  {
    strn[0] = '\0';

    if (coins / 1000)
    {
      sprintf(strn, "%u&+Wp&n, ", coins / 1000);
    }

    coins %= 1000;

    if (coins / 100)
    {
      sprintf(catstrn, "%u&+Yg&n, ", coins / 100);
      strcat(strn, catstrn);
    }

    coins %= 100;

    if (coins / 10)
    {
      sprintf(catstrn, "%us, ", coins / 10);
      strcat(strn, catstrn);
    }

    coins %= 10;

    if (coins)
    {
      sprintf(catstrn, "%u&+yc&n", coins);
      strcat(strn, catstrn);
    }

   // get rid of comma and space if they exist

    if (strn[strlen(strn) - 1] == ' ') strn[strlen(strn) - 2] = '\0';
  }

  return strn;
}


//
// isset
//

INLINEDEF char isset(const ulong flags, const ulong match)
{
  if (flags & match) return TRUE;

  return FALSE;
}


//
// deleteUnusedObjMobTypes
//

void deleteUnusedObjMobTypes(void)
{
  usint ch, objsDel = 0, mobsDel = 0;
  dikuObject *obj = objectHead, *nextO;
  dikuMob *mob = mobHead, *nextM;
  char strn[256];

  displayColorString(
   "\n&+cDelete all unused object and mob types (&+Cy/N&n&+c)? ");

  do
  {
    ch = toupper(getkey());
  } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

  if (ch != 'Y')
  {
    displayColorString("No\n\n");
    return;
  }

  displayColorString("Yes&n\n");

  while (obj)
  {
    if (!isObjTypeUsed(obj->objNumber))
    {
      nextO = obj->Next;
      deleteObjectTypeinList(&objectHead, obj, TRUE);

      objsDel++;

      obj = nextO;
    }
    else obj = obj->Next;
  }

  while (mob)
  {
    if (!getNumbEntities(ENTITY_MOB, mob->mobNumber, FALSE))
    {
      nextM = mob->Next;
      deleteMobTypeinList(&mobHead, mob, TRUE);

      mobsDel++;

      mob = nextM;
    }
    else mob = mob->Next;
  }

  sprintf(strn, "%u object type%s and %u mob type%s deleted.\n\n",
          objsDel, plural(objsDel), mobsDel, plural(mobsDel));

  _outtext(strn);
}


//
// resetEntityPointersByNumb
//

void resetEntityPointersByNumb(const char resetObjs /*= TRUE*/,
                               const char resetMobs /*= TRUE*/)
{
  dikuRoom *room = roomHead;
  mobHere *mob;
  char changed = FALSE;

  if (!resetObjs && !resetMobs) return;  // ya never know

  while (room)
  {
    if (resetObjs && room->objectHead)
    {
      resetObjHEntityPointersByNumb(room->objectHead);
      changed = TRUE;
    }

    if (resetMobs && room->mobHead)
    {
      mob = room->mobHead;
      while (mob)
      {
        mob->mobPtr = findMob(mob->mobNumb);
        mob = mob->Next;
      }

      changed = TRUE;
    }

    if (changed)
    {
      deleteMasterKeywordList(room->masterListHead);
      room->masterListHead = createMasterKeywordList(room);

      deleteEditableList(room->editableListHead);
      room->editableListHead = createEditableList(room);

      changed = FALSE;
    }

    room = room->Next;
  }
}


//
// resetObjCond
//

void resetObjCond(void)
{
  ulong i, high = getHighestObjNumber();
  dikuObject *obj;

  for (i = getLowestObjNumber(); i <= high; i++)
  {
    if (obj = findObj(i))
    {
      if (obj->condition < 100) obj->condition = 100;
      madeChanges = TRUE;
    }
  }

  _outtext("\nAll objects with conditions less than 100 (if any) set to 100.\n\n");
}


//
// fixGuildStuff
//

void fixGuildStuff(void)
{
  dikuObject *obj;
  dikuMob *mob;
  ulong i, high, numbNoshow = 0, numbAggr = 0, numbHunt = 0, numbNonNeut = 0;
  usint ch;
  char strn[256], found;

  if (!getCheckGuildStuffVal())
  {
    displayColorString("\n&+c"
"The 'check guild stuff' variable is not set - do you wish to reset object\n&+c"
"and mob flags based on required guild settings anyway (&+Cy/N&n&+c)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'Y')
    {
      displayColorString("no\n\n");
      return;
    }

    displayColorString("yes\n");
  }

 // reset all object noshow bits

  high = getHighestObjNumber();

  for (i = getLowestObjNumber(); i <= high; i++)
  {
    if (obj = findObj(i))
    {
      if (obj->extraBits.objExtraBits.noshow)
      {
        obj->extraBits.objExtraBits.noshow = FALSE;
        numbNoshow++;
      }
    }
  }

 // now reset every freakin mob aggro bit (quite a few) and the hunter
 // bit

  high = getHighestMobNumber();

  for (i = getLowestMobNumber(); i <= high; i++)
  {
    if (mob = findMob(i))
    {
      found = FALSE;

      if (mob->actionBits.mobActionBits.aggressive)
      {
        mob->actionBits.mobActionBits.aggressive = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggresEvil)
      {
        mob->actionBits.mobActionBits.aggresEvil = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggresGood)
      {
        mob->actionBits.mobActionBits.aggresGood = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggresNeut)
      {
        mob->actionBits.mobActionBits.aggresNeut = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggrEvilR)
      {
        mob->actionBits.mobActionBits.aggrEvilR = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggrGoodR)
      {
        mob->actionBits.mobActionBits.aggrGoodR = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.aggrOutcast)
      {
        mob->actionBits.mobActionBits.aggrOutcast = FALSE;
        if (!found) numbAggr++;
        found = TRUE;
      }

      if (mob->actionBits.mobActionBits.hunter)
      {
        mob->actionBits.mobActionBits.hunter = FALSE;
        numbHunt++;
      }

      if (mob->alignment != 0)
      {
        mob->alignment = 0;
        numbNonNeut++;
      }
    }
  }

  sprintf(strn, "&n\n"
"Reset %u object%s with the 'noshow' bit set, %u mob%s with 'aggro' bits\n"
"set, %u mob%s with the 'hunter' bit set, and %u non-neutral mob%s.\n\n",
          numbNoshow, plural(numbNoshow), numbAggr, plural(numbAggr),
          numbHunt, plural(numbHunt), numbNonNeut, plural(numbNonNeut));

  displayColorString(strn);

  if (numbNoshow || numbAggr || numbHunt || numbNonNeut) madeChanges = TRUE;
}


//
// editRoomsOnly : called via command-line switch
//

void editRoomsOnly(void)
{
  dikuRoom *room;
  int ch;


  roomLookup = new dikuRoom*[numbLookupEntries];
  if (!roomLookup)
  {
    cout << "\n" <<
"Error allocating memory for room lookup tables - the current size of\n" <<
 numbLookupEntries << " entries will take up " <<
(numbLookupEntries * sizeof(dikuRoom*)) << " bytes - try reducing\n"
"the size with the -ll=<loaded> command-line switch, or buy more RAM.\n";

    exit(1);
  }

  memset(roomLookup, 0, numbLookupEntries * sizeof(dikuRoom*));

  readWorldFile();

// init stuff

#ifdef __UNIX__
  initUnixCursesScreen();
#endif
//    startInit();
  _clearscreen(7, 0);

  if (!roomHead)
  {
    _outtext(
"There are currently no rooms to edit.  Room-edit mode requires that at least\n"
"one room exists.\n\n");

    exit(1);
  }

  room = roomHead;

  do
  {
    room = editRoom(room, TRUE);
  } while (room);

  if (madeChanges)
  {
    _outtext("\n\nChanges were made - save them (Y/n)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'N')
    {
      _outtext("Yes\n\n");

      writeWorldFile();
    }
    else _outtext("No\n\n");
  }
}


//
// editObjsOnly :
//

void editObjsOnly(void)
{
  dikuObject *obj;
  int ch;


  objLookup = new dikuObject*[numbLookupEntries];
  if (!objLookup)
  {
    cout << "\n" <<
"Error allocating memory for object lookup tables - the current size of\n" <<
 numbLookupEntries << " entries will take up " <<
(numbLookupEntries * sizeof(dikuObject*)) << " bytes - try reducing\n"
"the size with the -ll=<loaded> command-line switch, or buy more RAM.\n";

    exit(1);
  }

  memset(objLookup, 0, numbLookupEntries * sizeof(dikuObject*));

  readObjectFile();

// init stuff

#ifdef __UNIX__
  initUnixCursesScreen();
#endif
//    startInit();
  _clearscreen(7, 0);

  if (!objectHead)
  {
    _outtext(
"There are currently no object types to edit.  Object-edit mode requires that\n"
"at least one object type exists.\n\n");

    exit(1);
  }

  obj = objectHead;

  editObjType(obj, TRUE);

  if (madeChanges)
  {
    _outtext("\n\nChanges were made - save them (Y/n)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'N')
    {
      _outtext("Yes\n\n");

      writeObjectFile();
    }
    else _outtext("No\n\n");
  }
}


//
// editMobsOnly :
//

void editMobsOnly(void)
{
  dikuMob *mob;
  int ch;


  mobLookup = new   dikuMob*[numbLookupEntries];
  if (!mobLookup)
  {
    cout << "\n" <<
"Error allocating memory for mob lookup tables - the current size of\n" <<
 numbLookupEntries << " entries will take up " <<
(numbLookupEntries * sizeof(dikuMob*)) << " bytes - try reducing\n"
"the size with the -ll=<loaded> command-line switch, or buy more RAM.\n";

    exit(1);
  }

  memset(mobLookup, 0, numbLookupEntries * sizeof(dikuMob*));

  readMobFile();

// init stuff

#ifdef __UNIX__
  initUnixCursesScreen();
#endif
//    startInit();
  _clearscreen(7, 0);

  if (!mobHead)
  {
    _outtext(
"There are currently no mob types to edit.  Mob-edit mode requires that at\n"
"least one mob type exists.\n\n");

    exit(1);
  }

  mob = mobHead;

  editMobType(mob, TRUE);

  if (madeChanges)
  {
    _outtext("\n\nChanges were made - save them (Y/n)? ");

    do
    {
      ch = toupper(getkey());
    } while ((ch != 'Y') && (ch != 'N') && (ch != K_Enter));

    if (ch != 'N')
    {
      _outtext("Yes\n\n");

      writeMobFile();
    }
    else _outtext("No\n\n");
  }
}


//
// validANSIletter
//

INLINEDEF char validANSIletter(const char ch)
{
  switch (toupper(ch))
  {
    case 'B' :
    case 'C' :
    case 'L' :
    case 'R' :
    case 'G' :
    case 'M' :
    case 'Y' :
    case 'W' : return TRUE;

    default : return FALSE;
  }
}


//
// durisANSIcode : checks text at specified pos in string for "duris ANSIness"
//                 returns number of characters that need to be skipped to
//                 get past the code
//
//     strn : string being checked
//      pos : position in string to check at
//

INLINEDEF uchar durisANSIcode(const char *strn, const ulong pos)
{
  if (pos >= strlen(strn)) return 0;  // can't be too careful

  if ((strn[pos] != '&') || !strn[pos + 1]) return 0;

 // &n

  if (toupper(strn[pos + 1]) == 'N') return 2;

 // &+(whatever)

  if ((strn[pos + 1] == '+') && validANSIletter(strn[pos + 2])) return 3;

 // &-(whatever)

  if ((strn[pos + 1] == '-') && validANSIletter(strn[pos + 2])) return 3;

 // &=(bg)(fg)

  if ((strn[pos + 1] == '=') &&
      (strn[pos + 2] && validANSIletter(strn[pos + 2])) &&
      (validANSIletter(strn[pos + 3])))
    return 4;

  return 0;
}
