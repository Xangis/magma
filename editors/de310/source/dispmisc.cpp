//
//  File: dispmisc.cpp   originally part of dikuEdit
//
//  Usage: generally contains functions that display a list of
//         values/items
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  ifdef _WIN32
#    include <mapiutil.h>
#    include <mapiwin.h>
#    include <wincon.h>
#  endif

#  include "graphcon.h"
#endif

#include "types.h"
#include "fh.h"
#include "const.h"

#include "obj/armor.h"
#include "obj/shields.h"
#include "obj/object.h"

extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern _CONST char *exitnames[];

//
// displayObjSizeList
//

void displayObjSizeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n"
"  These values are used to determine which race can wear the object -\n"
"  size only matters for objects that can be worn/wielded and any other\n"
"  objects that could logically depend on the size of their user/wearer -\n"
"  however, you should set a logical size for every object\n\n");

  for (i = OBJSIZE_LOWEST; i <= OBJSIZE_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getObjSizeStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayCraftsmanshipList
//

void displayCraftsmanshipList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = OBJCRAFT_LOWEST; i <= OBJCRAFT_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getObjCraftsmanshipStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayArmorTypeList
//

void displayArmorTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = ARMOR_TYPE_LOWEST; i <= ARMOR_TYPE_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getArmorTypeStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayArmorWornList
//

void displayArmorWornList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = ARMOR_WEAR_LOWEST; i <= ARMOR_WEAR_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getArmorWornStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayArmorThicknessList
//

void displayArmorThicknessList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = ARMOR_THICKNESS_LOWEST; i <= ARMOR_THICKNESS_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getArmorThicknessStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayShieldTypeList
//

void displayShieldTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SHIELDTYPE_LOWEST; i <= SHIELDTYPE_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getShieldTypeStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayShieldShapeList
//

void displayShieldShapeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SHIELDSHAPE_LOWEST; i <= SHIELDSHAPE_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getShieldShapeStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayShieldSizeList
//

void displayShieldSizeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SHIELDSIZE_LOWEST; i <= SHIELDSIZE_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getShieldSizeStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayMaterialList
//

void displayMaterialList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = MAT_LOWEST; i <= MAT_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getMaterialStrn(i));
    displayColorString(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayInstrumentList
//

void displayInstrumentList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = INSTRUMENT_LOWEST; i <= INSTRUMENT_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getInstrumentTypeStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displaySpellList
//

void displaySpellList(const char *searchStrn /*= NULL*/)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256], strn3[256], foundOne = FALSE;

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SPELL_LOWEST; i <= SPELL_HIGHEST; i++)
  {
    strcpy(strn2, getSpellTypeStrn(i));

    if (searchStrn)
    {
      strcpy(strn3, strn2);
      upstrn(strn3);
    }

    if (!searchStrn || (searchStrn && strstr(strn3, searchStrn)))
    {
      foundOne = TRUE;

      if (!strcmp(strn2, "unrecognized type")) continue;

      sprintf(strn, "  %3u - %s\n", i, strn2);
      _outtext(strn);

      numbItems += numbLinesStringNeeds(strn);

      if (numbItems >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");
        if (toupper(getkey()) == 'Q')
        {
          _outtext("\n\n");

          return;
        }

        _outtext("\n\n");

        numbItems = 0;
      }
    }
  }

  if (searchStrn && !foundOne) _outtext("No matching spells found.\n");
  _outtext("\n");
}


//
// displayLiquidList
//

void displayLiquidList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = LIQ_LOWEST; i <= LIQ_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getLiqTypeStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayDirectionList
//

void displayDirectionList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = EXIT_LOWEST; i <= EXIT_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, exitnames[i]);
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayWeaponDamageList
//

void displayWeaponDamageList(void)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = WEAP_LOWEST; i <= WEAP_HIGHEST; i++)
  {
    strcpy(strn2, getWeapDamStrn(i));
    if (!strcmp(strn2, "unrecognized type")) continue;

    sprintf(strn, "  %2u - %s\n", i, strn2);
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayWeaponTypeList
//

void displayWeaponTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = WEAPON_LOWEST; i <= WEAPON_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getWeapTypeStrn(i));
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// displayMissileTypeList
//

void displayMissileTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = MISSILE_LOWEST; i <= MISSILE_HIGHEST; i++)
  {
    strcpy(strn2, getMissileTypeStrn(i));
    if (!strcmp(strn2, "unrecognized type")) continue;

    sprintf(strn, "  %2u - %s\n", i, strn2);
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}

//
// displayLangTypeList
//

void displayLangTypeList(void)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = LANG_LOWEST; i <= LANG_HIGHEST; i++)
  {
    strcpy(strn2, getLangTypeStrn(i));
    if (!strcmp(strn2, "unrecognized type")) continue;

    sprintf(strn, "  %2u - %s\n", i, strn2);
    _outtext(strn);

    numbItems += numbLinesStringNeeds(strn);

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n");
}


//
// listCommands
//

void listCommands(command *startCmd)
{
  command *cmd = startCmd;
  char strn[256], i = 0;
  int numb = 0;

  _setbkcolor(0);
  _outtext("\n\n");

  while (cmd)
  {
   // let's display three per line, shall we?

    sprintf(strn, "%-25s", cmd->commandStrn);
    numb++;

    cmd = cmd->Next;

    if (cmd)
    {
      sprintf(strn + strlen(strn), "%-25s", cmd->commandStrn);
      numb++;

      cmd = cmd->Next;
    }
    else strcat(strn, "\n");

    if (cmd)
    {
      sprintf(strn + strlen(strn), "%-25s\n", cmd->commandStrn);
      numb++;

      cmd = cmd->Next;
    }
    else strcat(strn, "\n");

    displayColorString(strn);

    i += numbLinesStringNeeds(strn);

    if ((i >= (getScreenHeight() - 3)) || !cmd)
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

//    if (cmd) cmd = cmd->Next;
  }

  sprintf(strn, "Total number of commands: %u\n\n", numb);
  _outtext(strn);
}


//
// checkPause : returns TRUE if user wants to quit out of the list
//

char checkPause(ulong &i)
{
  if (i >= (getScreenHeight() - 3))
  {
    displayColorString("\n&+CPress any key to continue or Q to quit..&n");
    if (toupper(getkey()) == 'Q')
    {
      _outtext("\n\n");

      return TRUE;
    }

    _outtext("\n\n");

    i = 0;
  }

  return FALSE;
}


//
// showKeyUsed
//

void showKeyUsed(const char *args)
{
  dikuObject *obj = objectHead;
  dikuRoom *room = roomHead;
  ulong i = 0, j, vnum;
  char outStrn[512], foundKey = FALSE;


  if (strlen(args) == 0)
  {
    displayObjectTypeList("KEY", FALSE);  // list objects with 'key' in their
    return;                               // keyword list
  }

  _setbkcolor(0);

 // allow looking for objects that don't exist in this zone..  we only
 // search by vnum after all

  if (strnumer(args))
  {
    obj = findObj(atoi(args));

    vnum = atoi(args);
  }
  else
  {
    while (obj)
    {
      if (scanKeyword(args, obj->keywordListHead))
      {
        vnum = obj->objNumber;
        break;
      }

      obj = obj->Next;
    }

    if (!obj)
    {
      _outtext("\nNo object could be found that matches that keyword.\n\n");
      return;
    }
  }

  if (obj && (obj->objType != ITEM_KEY))
  {
    sprintf(outStrn, "\n'%s&n' is not a key.\n\n", obj->objShortName);
    displayColorString(outStrn);

    return;
  }

 // find the thingies that need this key and display them

  displayColorString("\n");

  sprintf(outStrn, "\n'%s&n' (#%u) is needed for -\n\n",
          obj ? obj->objShortName : "(object not in this .OBJ file)",
          vnum);
  displayColorString(outStrn);

  i = 2;

 // first, rooms

  while (room)
  {
    for (j = 0; j < NUMB_EXITS; j++)
    {
      if (exitNeedsKey(room->exits[j], vnum))
      {
        sprintf(outStrn, "  &+C%s&n exit of &+groom #&+c%u&n, '%s&n'\n",
                exitnames[j], room->roomNumber, room->roomName);
        displayColorString(outStrn);

        i += numbLinesStringNeeds(outStrn);

        if (checkPause(i)) return;

        foundKey = TRUE;
      }
    }

    room = room->Next;
  }

 // then, objects (containers)

  obj = objectHead;

  while (obj)
  {
    if ((obj->objType == ITEM_CONTAINER) && (obj->objValues[2] == vnum))
    {
      sprintf(outStrn, "  &+gobject #&+c%u&n, '%s&n' (&+ycontainer&n)\n",
              obj->objNumber, obj->objShortName);
      displayColorString(outStrn);

      i += numbLinesStringNeeds(outStrn);

      if (checkPause(i)) return;

      foundKey = TRUE;
    }

    obj = obj->Next;
  }

  if (foundKey) _outtext("\n");
  else _outtext("That key is not used on any doors or containers.\n\n");
}
