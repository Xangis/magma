//
//  File: mobtypes.cpp   originally part of dikuEdit
//
//  Usage: various functions for checking mob stats or readable strings
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "mob.h"
#include "../inline.h"
#include "../boolean.h"
#include "../fh.h"

//
// getMobSpeciesStrn
//

const char *getMobSpeciesStrn(const char *id)
{
  if (!strcmp(id, MOB_HUMAN)) return "human";
  else if (!strcmp(id, MOB_BARBARIAN)) return "barbarian";
  else if (!strcmp(id, MOB_DROW_ELF)) return "drow elf";
  else if (!strcmp(id, MOB_GREY_ELF)) return "grey elf";
  else if (!strcmp(id, MOB_MOUNT_DWARF)) return "mountain dwarf";
  else if (!strcmp(id, MOB_DUERGAR)) return "duergar";
  else if (!strcmp(id, MOB_HALFLING)) return "halfling";
  else if (!strcmp(id, MOB_GNOME)) return "gnome";
  else if (!strcmp(id, MOB_OGRE)) return "ogre";
  else if (!strcmp(id, MOB_TROLL)) return "troll";
  else if (!strcmp(id, MOB_HALF_ELF)) return "half-elf";
  else if (!strcmp(id, MOB_ILLITHID)) return "illithid";
  else if (!strcmp(id, MOB_ORC)) return "orc";
  else if (!strcmp(id, MOB_THRIKREEN)) return "thri-kreen";
  else if (!strcmp(id, MOB_CENTAUR)) return "centaur";
  else if (!strcmp(id, MOB_GITHYANKI)) return "githyanki";
  else if (!strcmp(id, MOB_GITHZERAI)) return "githzerai";
  else if (!strcmp(id, MOB_MINOTAUR)) return "minotaur";
  else if (!strcmp(id, MOB_AQUAELF)) return "aquaelf";
  else if (!strcmp(id, MOB_SAHUAGIN)) return "sahuagin";
  else if (!strcmp(id, MOB_GOBLIN)) return "goblin";
  else if (!strcmp(id, MOB_RAKSHASA)) return "rakshasa";
  else if (!strcmp(id, MOB_FLIND)) return "flind";
  else if (!strcmp(id, MOB_FIRE_ELEM)) return "fire elem";
  else if (!strcmp(id, MOB_AIR_ELEM)) return "air elem";
  else if (!strcmp(id, MOB_WATER_ELEM)) return "water elem";
  else if (!strcmp(id, MOB_EARTH_ELEM)) return "earth elem";
  else if (!strcmp(id, MOB_DEMON)) return "demon";
  else if (!strcmp(id, MOB_DEVIL)) return "devil";
  else if (!strcmp(id, MOB_UNDEAD)) return "undead";
  else if (!strcmp(id, MOB_VAMPIRE)) return "vampire";
  else if (!strcmp(id, MOB_GHOST)) return "ghost";
  else if (!strcmp(id, MOB_LYCANTH)) return "lycanthrope";
  else if (!strcmp(id, MOB_GIANT)) return "giant";
  else if (!strcmp(id, MOB_HALF_ORC)) return "half-orc";
  else if (!strcmp(id, MOB_GOLEM)) return "golem";
  else if (!strcmp(id, MOB_FAERIE)) return "faerie";
  else if (!strcmp(id, MOB_DRAGON)) return "dragon";
  else if (!strcmp(id, MOB_DRAGONKIN)) return "dragonkin";
  else if (!strcmp(id, MOB_REPTILE)) return "reptile";
  else if (!strcmp(id, MOB_SNAKE)) return "snake";
  else if (!strcmp(id, MOB_INSECT)) return "insect";
  else if (!strcmp(id, MOB_ARACHNID)) return "arachnid";
  else if (!strcmp(id, MOB_FISH)) return "fish";
  else if (!strcmp(id, MOB_BIRD)) return "bird";
  else if (!strcmp(id, MOB_HORSE)) return "horse";
  else if (!strcmp(id, MOB_PRIMATE)) return "primate";
  else if (!strcmp(id, MOB_HUMANOID)) return "humanoid";
  else if (!strcmp(id, MOB_ANIMAL)) return "animal";
  else if (!strcmp(id, MOB_TREE)) return "tree";
  else if (!strcmp(id, MOB_HERBIVORE)) return "herbivore";
  else if (!strcmp(id, MOB_CARNIVORE)) return "carnivore";
  else if (!strcmp(id, MOB_PARASITE)) return "parasite";
  else if (!strcmp(id, MOB_BEHOLDER)) return "beholder";
  else if (!strcmp(id, MOB_DRACOLICH)) return "dracolich";
  else if (!strcmp(id, MOB_ABOLETH)) return "aboleth";
  else if (!strcmp(id, MOB_IXICHITL)) return "ixichitl";
  else if (!strcmp(id, MOB_DERRO)) return "derro";
  else if (!strcmp(id, MOB_KOBOLD)) return "kobold";
  else if (!strcmp(id, MOB_OBJECT)) return "object";
  else if (!strcmp(id, MOB_MIST )) return "mist";
  else if (!strcmp(id, MOB_WORM )) return "worm";
  else if (!strcmp(id, MOB_PLANT )) return "plant";
  else if (!strcmp(id, MOB_BAT )) return "bat";
  else if (!strcmp(id, MOB_BEAR )) return "bear";
  else if (!strcmp(id, MOB_HARPY )) return "harpy";
  else if (!strcmp(id, MOB_HYDRA )) return "hydra";
  else if (!strcmp(id, MOB_GOD )) return "god";
  else if (!strcmp(id, MOB_RAT )) return "rat";
  else if (!strcmp(id, MOB_HOBGOBLIN )) return "hobgoblin";
  else if (!strcmp(id, MOB_HALFKOBOLD )) return "halfkobold";
  else if (!strcmp(id, MOB_HALFDWARF )) return "halfdwarf";
  else if (!strcmp(id, MOB_WEREWOLF )) return "werewolf";
  else if (!strcmp(id, MOB_RUSTMONSTER )) return "rust monster";
  else if (!strcmp(id, MOB_WEMIC )) return "wemic";
  else if (!strcmp(id, MOB_UMBERHULK )) return "umber hulk";
  else if (!strcmp(id, MOB_ANGEL )) return "angel";
  else if (!strcmp(id, MOB_SLAAD )) return "slaad";
  else if (!strcmp(id, MOB_BOAR )) return "boar";
  else if (!strcmp(id, MOB_SHADOW_ELEM )) return "shadow elemental";
  else if (!strcmp(id, MOB_NEOGI )) return "neogi";
  else return "unrecog. species";
}


//
// getMobPosStrn
//

const char *getMobPosStrn(const ulong position)
{
  switch (position)
  {
    case POSITION_DEAD       : return "invalid position";
    case POSITION_MORTALLYW  : return "invalid position";
    case POSITION_INCAP      : return "invalid position";
    case POSITION_STUNNED    : return "invalid position";
    case POSITION_SLEEPING   : return "sleeping";
    case POSITION_RESTING    : return "resting";
    case POSITION_SITTING    : return "sitting";
    case POSITION_FIGHTING   : return "fighting";
    case POSITION_STANDING   : return "standing";
    case POSITION_KNEELING   : return "kneeling";

    default : return "unrecognized";
  }
}


//
// getMobSexStrn
//

const char *getMobSexStrn(const uchar sex)
{
  switch (sex)
  {
    case SEX_NEUTER    : return "neuter";
    case SEX_MALE      : return "male";
    case SEX_FEMALE    : return "female";

    default : return "unrecognized";
  }
}


//
// getMobHometownStrn
//

const char *getMobHometownStrn(const char hometown)
{
  switch (hometown)
  {
    case HOME_NONE         : return "none";
    case HOME_THARNADIA    : return "Tharnadia";
    case HOME_UGTA         : return "Ugta";
    case HOME_ARACHDRATHOS : return "Arachdrathos";
    case HOME_IXARKON      : return "Ixarkon";
    case HOME_SYLVANDAWN   : return "Sylvandawn";
    case HOME_KIMORDRIL    : return "Kimordril";
    case HOME_KHILDARAK    : return "Khildarak";
    case HOME_WOODSEER     : return "Woodseer";
    case HOME_ASHRUMITE    : return "Ashrumite";
    case HOME_FAANG        : return "Faang";
    case HOME_GHORE        : return "Ghore";
    case HOME_BLOODSTONE   : return "Bloodstone";
    case HOME_SHADY        : return "Shady Grove";
    case HOME_SARMIZ       : return "Sarmiz'Duul";
    case HOME_MARIGOT      : return "Marigot";

    default : return "unrecognized";
  }
}


//
// getMobClassStrn
//

const char *getMobClassStrn(const uchar mobClass)
{
  switch (mobClass)
  {
    case CLASS_NONE            : return "none";
    case CLASS_WARRIOR         : return "warrior";
    case CLASS_RANGER          : return "ranger";
    case CLASS_PSIONICIST      : return "psionicist";
    case CLASS_PALADIN         : return "paladin";
    case CLASS_ANTIPALADIN     : return "anti-paladin";
    case CLASS_CLERIC          : return "cleric";
    case CLASS_MONK            : return "monk";
    case CLASS_DRUID           : return "druid";
    case CLASS_SHAMAN          : return "shaman";
    case CLASS_SORCERER        : return "sorcerer";
    case CLASS_NECROMANCER     : return "necromancer";
    case CLASS_CONJURER        : return "conjurer";
    case CLASS_THIEF           : return "thief";
    case CLASS_ASSASSIN        : return "assassin";
    case CLASS_MERCENARY       : return "mercenary";
    case CLASS_BARD            : return "bard";
    case CLASS_ILLUSIONIST     : return "illusionist";
    case CLASS_ELEM_FIRE       : return "fire elementalist";
    case CLASS_ELEM_EARTH      : return "earth elementalist";
    case CLASS_ELEM_AIR        : return "air elementalist";
    case CLASS_ELEM_WATER      : return "water elementalist";
    case CLASS_HUNTER          : return "hunter";
    case CLASS_CHRONOMANCER    : return "chronomancer";
    case CLASS_WARLOCK         : return "warlock";
    case CLASS_ENSLAVER        : return "enslaver";

    default : return "unrecognized";
  }
}


//
// isAggro
//

INLINEDEF int isAggro(const dikuMob *mob)
{
  mobActionBitFlags ma;

  if (!mob) return FALSE;

  ma = mob->actionBits.mobActionBits;

  return (ma.aggressive || ma.aggresEvil || ma.aggresGood || ma.aggresNeut || 
          ma.aggrEvilR  || ma.aggrGoodR  || ma.aggrOutcast);
}


//
// castingClass
//

INLINEDEF int castingClass(const uchar mobClass)
{
  return ((mobClass == CLASS_RANGER)      || (mobClass == CLASS_PSIONICIST) ||
          (mobClass == CLASS_PALADIN)     || (mobClass == CLASS_ANTIPALADIN) ||
          (mobClass == CLASS_CLERIC)      || (mobClass == CLASS_DRUID) ||
          (mobClass == CLASS_ELEM_WATER)      || (mobClass == CLASS_ELEM_AIR) ||
          (mobClass == CLASS_ELEM_FIRE)      || (mobClass == CLASS_ELEM_EARTH) ||
          (mobClass == CLASS_ILLUSIONIST) ||
          (mobClass == CLASS_SHAMAN)      || (mobClass == CLASS_SORCERER) ||
          (mobClass == CLASS_NECROMANCER) || (mobClass == CLASS_CONJURER));
}
