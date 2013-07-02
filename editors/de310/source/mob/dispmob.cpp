//
//  File: dispmob.cpp    originally part of dikuEdit
//
//  Usage: functions for displaying mob information
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "mob.h"

extern dikuRoom *roomHead;
extern dikuMob *mobHead;
extern _CONST flagDef mobActFlagDef[], aff1FlagDef[], aff2FlagDef[], aff3FlagDef[], aff4FlagDef[];

//
// displayMobTypeList : Displays the list of mob types starting at mobHead
//

void displayMobTypeList(void)
{
  dikuMob *mob = mobHead;
  ulong i = 0;
  char strn[256];


  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");

    return;
  }

  _outtext("\n\n");

  while (mob)
  {
    sprintf(strn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);
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

    mob = mob->Next;
  }

  _outtext("\n");
}


//
// displayMobTypeList : Displays the list of mob types loaded into
//                      game, starting at mobHead
//

void displayMobTypeList(const char *strn)
{
  dikuMob *mob = mobHead;
  ulong i = 0, numb;
  char outStrn[512], foundMob = FALSE, vnum;


  if (strlen(strn) == 0)
  {
    displayMobTypeList();
    return;
  }

  _setbkcolor(0);

  if (!mob)
  {
    _outtext("\nThere are currently no mob types.\n\n");

    return;
  }

  vnum = strnumer(strn);
  numb = atoi(strn);

  _outtext("\n\n");

  while (mob)
  {
    if ((vnum && (numb == mob->mobNumber)) ||
        (scanKeyword(strn, mob->keywordListHead)))
    {
      sprintf(outStrn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);

      foundMob = TRUE;
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

  if (!foundMob) _outtext("No matching mob types found.\n");
  _outtext("\n");
}


//
// displayMobHereList
//

void displayMobHereList(void)
{
  dikuRoom *room = roomHead;
  mobHere *mob;
  char strn[512], mobName[512];
  ulong i = 0;


  _setbkcolor(0);

  _outtext("\n\n");

  while (room)
  {
    mob = room->mobHead;

    while (mob)
    {
      if (mob->mobPtr) strcpy(mobName, mob->mobPtr->mobShortName);
      else strcpy(mobName, "(mob type not in this .MOB file)");

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

      mob = mob->Next;
    }

    room = room->Next;
  }

/*
  if (i)
  {
    displayColorString("\n&+CPress any key to continue..&n");
    getkey();
    _outtext("\n\n");
  }
*/

  _outtext("\n");
}


//
// displayMobEqPos
//

void displayMobEqPos(const mobHere *mob, const char pos, const char *posStrn)
{
  objectHere *obj;
  char strn[512], strn2[256];

  if (!mob || (pos < WEAR_LOW) || (pos > WEAR_TRUEHIGH)) return;

  obj = getMobEquipPos(mob, pos);
  if (obj)
  {
    sprintf(strn, "%s %s&n", posStrn,
            obj->objectPtr ? obj->objectPtr->objShortName :
                             "(object not in this .OBJ file)");

    if ((getShowObjVnumVal()) || (!obj->objectPtr))
    {
      sprintf(strn2, " (#%u)", obj->objectNumb);
      strcat(strn, strn2);
    }

    sprintf(strn2, " &+c(%u%%)&n", obj->randomChance);
    strcat(strn, strn2);

    strcat(strn, "\n");

    displayColorString(strn);
  }
}


//
// displayMobEquip : Displays a mob's equipment list
//
//   *mobEquip : Pointer to equipment list
//

void displayMobEquip(const mobHere *mobEq)
{
//  displayMobEqPos(mobEq, WEAR_NOTWORN,   "<not worn - &+Roops&n>    ");
  displayMobEqPos(mobEq, WEAR_BADGE,     "<worn as a badge>    ");
  displayMobEqPos(mobEq, WEAR_HORN,      "<worn on horns>      ");
  displayMobEqPos(mobEq, WEAR_WHOLEHEAD, "<worn on whole head> ");
  displayMobEqPos(mobEq, WEAR_HEAD,      "<worn on head>       ");
  displayMobEqPos(mobEq, WEAR_EYES,      "<worn on eyes>       ");
  displayMobEqPos(mobEq, WEAR_EARRING_L, "<worn in ear>        ");
  displayMobEqPos(mobEq, WEAR_EARRING_R, "<worn in ear>        ");
  displayMobEqPos(mobEq, WEAR_NOSE,      "<worn in nose>       ");
  displayMobEqPos(mobEq, WEAR_FACE,      "<worn on face>       ");
  displayMobEqPos(mobEq, WEAR_QUIVER,    "<worn as quiver>     ");
  displayMobEqPos(mobEq, WEAR_NECK_1,    "<worn around neck>   ");
  displayMobEqPos(mobEq, WEAR_NECK_2,    "<worn around neck>   ");
  displayMobEqPos(mobEq, WEAR_BACK,      "<worn on back>       ");
  displayMobEqPos(mobEq, WEAR_WHOLEBODY, "<worn on whole body> ");
  displayMobEqPos(mobEq, WEAR_BODY,      "<worn on body>       ");
  displayMobEqPos(mobEq, WEAR_ABOUT,     "<worn about body>    ");
  displayMobEqPos(mobEq, WEAR_WAIST,     "<worn about waist>   ");
  displayMobEqPos(mobEq, WEAR_ATTACH_BELT_1,  "<attached to belt>   ");
  displayMobEqPos(mobEq, WEAR_ATTACH_BELT_2,  "<attached to belt>   ");
  displayMobEqPos(mobEq, WEAR_ATTACH_BELT_3,  "<attached to belt>   ");
  displayMobEqPos(mobEq, WEAR_ARMS,      "<worn on arms>       ");
  displayMobEqPos(mobEq, WEAR_ARMS_2,    "<worn on lower arms> ");
  displayMobEqPos(mobEq, WEAR_SHIELD,    "<held as shield>     ");
  displayMobEqPos(mobEq, WEAR_WRIST_L,   "<worn around wrist>  ");
  displayMobEqPos(mobEq, WEAR_WRIST_R,   "<worn around wrist>  ");
  displayMobEqPos(mobEq, WEAR_WRIST_LR,  "<worn on lower wrist>");
  displayMobEqPos(mobEq, WEAR_WRIST_LR,  "<worn on lower wrist>");
  displayMobEqPos(mobEq, WEAR_HANDS,     "<worn on hands>      ");
  displayMobEqPos(mobEq, WEAR_HANDS_2,   "<worn on lower hands>");
  displayMobEqPos(mobEq, WEAR_FINGER_L,  "<worn on finger>     ");
  displayMobEqPos(mobEq, WEAR_FINGER_R,  "<worn on finger>     ");
  displayMobEqPos(mobEq, WIELD_TWOHANDS, "<wielding two-handed>");
  displayMobEqPos(mobEq, WIELD_TWOHANDS2,"<wielding two-handed>");
  displayMobEqPos(mobEq, WIELD_PRIMARY,  "<primary weapon>     ");
  displayMobEqPos(mobEq, WIELD_SECOND,   "<secondary weapon>   ");
  displayMobEqPos(mobEq, WIELD_THIRD,    "<third weapon>       ");
  displayMobEqPos(mobEq, WIELD_FOURTH,   "<fourth weapon>      ");
  displayMobEqPos(mobEq, WEAR_LIGHT,     "<as light-not imped> ");
  displayMobEqPos(mobEq, HOLD,           "<held>               ");
  displayMobEqPos(mobEq, HOLD2,          "<held>               ");
  displayMobEqPos(mobEq, WEAR_LEGS,      "<worn on legs>       ");
  displayMobEqPos(mobEq, WEAR_LEGS_REAR, "<worn on rear legs>  ");
  displayMobEqPos(mobEq, WEAR_FEET,      "<worn on feet>       ");
  displayMobEqPos(mobEq, WEAR_FEET_REAR, "<worn on rear feet>  ");
  displayMobEqPos(mobEq, WEAR_TAIL,      "<worn on tail>       ");

  _outtext("\n");
}


//
// displayMobMisc : Displays "miscellaneous" info on a mob - level, align, and
//                  class
//
//    *mob : mob to display info on
//

void displayMobMisc(const mobHere *mob)
{
  char outstrn[512];
  dikuMob *mobT;


  if (!mob)
  {
    _outtext("Error - NULL mob passed to displayMobMisc.\n");
    return;
  }

  mobT = mob->mobPtr;

  if (!mobT)
  {
    _outtext(
"This mob is not in this .MOB file; thus, no information is available.\n");

    return;
  }

  sprintf(outstrn,
"&+W%s&n is &+clevel %u&n, &+malign %d&n, of class '&+Y%s&n', and a%s &+g%s&n.\n",
          mobT->mobShortName, mobT->level,
          mobT->alignment, getMobClassStrn(mobT->mobClass),
          getVowelN(getMobSpeciesStrn(mobT->mobSpecies)[0]),
          getMobSpeciesStrn(mobT->mobSpecies));

  displayColorString(outstrn);

  if (mobT->questPtr)
  {
    displayColorString("&+YThis mob has associated quest info.&n\n");
  }

  if (mobT->shopPtr)
  {
    displayColorString("&+YThis mob has associated shop info.&n\n");
  }
}


//
// displayMobPositionList
//

void displayMobPositionList(void)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = POSITION_LOWEST_LEGAL; i <= POSITION_HIGHEST; i++)
  {
    strcpy(strn2, getMobPosStrn(i));
    if (!strcmp(strn2, "unrecognized")) continue;

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

//  _outtext("\n\n");
  _outtext("\n");
}


//
// displayMobSexList
//

void displayMobSexList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = SEX_LOWEST; i <= SEX_HIGHEST; i++)
  {
    sprintf(strn, "  %u - %s\n", i, getMobSexStrn(i));
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

//  _outtext("\n\n");
  _outtext("\n");
}


//
// displayMobClassList
//

void displayMobClassList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = CLASS_LOWEST; i <= CLASS_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getMobClassStrn(i));
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

//  _outtext("\n\n");
  _outtext("\n");
}


//
// displayMobHometownList
//

void displayMobHometownList(void)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = HOME_LOWEST; i <= HOME_HIGHEST; i++)
  {
    sprintf(strn, "  %2u - %s\n", i, getMobHometownStrn(i));
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

//  _outtext("\n\n");
  _outtext("\n");
}


//
// displayMobSpeciesList
//

void displayMobSpeciesList(void)
{
  usint ch;


  _setbkcolor(0);

  _outtext("\n\n"
"   PH - Human\n"
"   PB - Barbarian\n"
"   PL - Drow Elf\n"
"   PE - Grey Elf\n"
"   PM - Mountain Dwarf\n"
"   PD - Duergar\n"
"   PF - Halfling\n"
"   PG - Gnome\n"
"   PO - Ogre\n"
"   PT - Troll\n"
"   P2 - Half Elf\n"
"   MF - Illithid\n"
"   HO - Orc\n"
"   HG - Goblin\n"
"   TK - Thri-Kreen\n"
"   CT - Centaur\n"
"   GI - Githyanki\n"
"   MT - Minotaur\n"
"   PA - Aquaelf\n"
"   SA - Sahuagin\n"
"   RA - Raksasha\n\n"
"Press any key to continue or Q to quit...");

  ch = toupper(getkey());
  if (ch == 'Q')
  {
    _outtext("\n\n");
    return;
  }

  _outtext("\n\n"
"   FL - Flind\n"
"   EF - Fire Elemental\n"
"   EA - Air Elemental\n"
"   EW - Water Elemental\n"
"   EE - Earth Elemental\n"
"   X  - Demon\n"
"   Y  - Devil\n"
"   U  - Undead\n"
"   UV - Vampire\n"
"   UG - Ghost\n"
"   L  - Lycanth\n"
"   G  - Giant\n"
"   MF - Mind Flayer\n"
"   H2 - Half Orc\n"
"   OG - Golem\n"
"   HF - Faerie\n"
"   D  - Dragon\n"
"   DK - Dragonkin\n"
"   R  - Reptile\n"
"   RS - Snake\n"
"   I  - Insect\n"
"   AS - Arachnid\n\n"
"Press any key to continue or Q to quit...");

  ch = toupper(getkey());
  if (ch == 'Q')
  {
    _outtext("\n\n");
    return;
  }

  _outtext("\n\n"
"   F  - Fish\n"
"   B  - Bird\n"
"   AE - Horse\n"
"   AA - Primate\n"
"   H  - Humanoid\n"
"   A  - Animal\n"
"   VT - Tree\n"
"   AH - Herbivore\n"
"   AC - Carnivore\n"
"   AP - Parasite\n"
"   BH - Beholder\n"
"   UD - Dracolich\n"
"   K  - Kobold\n"
"   O  - Object\n"
"   HB - Hobgoblin\n"
"   M  - Mist\n"
"   AW - Worm\n"
"   BE - Bear\n"
"   DH - Hydra\n"
"   BB - Bat\n"
"   VP - Plant\n"
"   HP - Harpy\n\n"
"Press any key to continue or Q to quit...");

  ch = toupper(getkey());
  if (ch == 'Q')
  {
    _outtext("\n\n");
    return;
  }

  _outtext("\n\n"
"   AR - Rat\n"
"   LW - Werewolf\n"
"   D2 - Half-Dwarf\n"
"   K2 - Half-Kobold\n"
"   UH - Umber Hulk\n"
"   WE - Wemic\n"
"   AN - Angel\n"
"   SL - Slaad\n"
"   BO - Boar\n"
"   GZ - Githzerai\n"
"   H2 - Half-Orc\n"
"   RM - Rust Monster\n"
"   DE - Derro\n"
"   AB - Aboleth\n"
"   ES - Shadow Elemental\n"
"   NE - Neogi\n\n" );
//"Press any key to continue...");

//  getkey();

//  _outtext("\n\n");
}


//
// displayMobInfo
//

void displayMobInfo(const char *args, dikuRoom *room)
{
  char *strn = new char[4096], *strn2 = new char[4096];
  char fstrn[2048], fstrn2[2048], fstrn3[2048], fstrn4[2048], fstrn5[2048];

  dikuMob *mob;


  if (!strn || !strn2)
  {
    _outtext("\n\nError allocating string buffer in displayMobInfo() - "
             "aborting..\n");

    if (strn) delete[] strn;
    if (strn2) delete[] strn2;

    return;  // no need to die completely, after all
  }

  if (!room)
  {
    _outtext("\n\nerror in displayMobInfo() - room ptr is NULL\n");

    delete[] strn;
    delete[] strn2;

    return;
  }

 // input-checking galore

  if (!strlen(args))
  {
    displayColorString("&n\nSpecify a mob vnum/keyword to stat.\n\n");
    return;
  }

  mob = getMatchingMob(args, room);
  if (!mob)
  {
    if (strnumer(args))
    {
      displayColorString("&n\nNo mob type exists with that vnum.\n\n");
      return;
    }
    else
    {
      displayColorString("&n\nNo mob type exists with that keyword.\n\n");
      return;
    }
  }

  sprintf(strn,
"\n"
"&+YVnum:&n %d  &+YShort name:&n %s&n\n"
"&+YLong name:&n %s&n\n"
"&+YKeywords:&n %s\n"
"\n"
"&+YMob action bits:&n %u (%s)\n"
"&+YMob aff1 bits  :&n %u (%s)\n"
"&+YMob aff2 bits  :&n %u (%s)\n"
"&+YMob aff3 bits  :&n %u (%s)\n"
"&+YMob aff4 bits  :&n %u (%s)\n"
"\n"
"&+YClass    :&n %u (%s)\n"
"&+YSpecies  :&n %s (%s)\n"
"&+YAlignment:&n %d\n"
"\n"
"&+YLevel/AC/THAC0 :&n %d/%d/%d\n"
"&+YDamage/HP/Exp  :&n %s/%s/%u\n"
"\n"
"&+YCopper/Silver/Gold/Platinum:&n %u/%u/%u/%u\n"
"&+YPos/Default Pos/Sex:&n %d (%s)/%d (%s)/%d (%s)\n"
"\n",

mob->mobNumber, mob->mobShortName,
mob->mobLongName,
getReadableKeywordStrn(mob->keywordListHead, strn2),

mob->actionBits.longIntFlags,
  getFlagStrn(mob->actionBits.longIntFlags, mobActFlagDef, fstrn),
mob->affect1Bits.longIntFlags,
  getFlagStrn(mob->affect1Bits.longIntFlags, aff1FlagDef, fstrn2),
mob->affect2Bits.longIntFlags,
  getFlagStrn(mob->affect2Bits.longIntFlags, aff2FlagDef, fstrn3),
mob->affect3Bits.longIntFlags,
  getFlagStrn(mob->affect3Bits.longIntFlags, aff3FlagDef, fstrn4),
mob->affect4Bits.longIntFlags,
  getFlagStrn(mob->affect4Bits.longIntFlags, aff4FlagDef, fstrn5),

mob->mobClass, getMobClassStrn(mob->mobClass),
mob->mobSpecies, getMobSpeciesStrn(mob->mobSpecies),
mob->alignment,

mob->level,
mob->ac,
mob->thac0, mob->mobDamage, mob->hitPoints,
mob->exp,

mob->copper, mob->silver, mob->gold, mob->platinum,

mob->position, getMobPosStrn(mob->position),
mob->defaultPos, getMobPosStrn(mob->defaultPos),
mob->sex, getMobSexStrn(mob->sex));

  displayColorString(strn);

  delete[] strn;
  delete[] strn2;
}
