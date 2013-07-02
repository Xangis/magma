//
//  File: objtypes.cpp   originally part of dikuEdit
//
//  Usage: innumerable functions used to get info on enumerated values
//         used for objects
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "object.h"

extern _CONST flagDef armorNeckFlagDef[], armorBodyFlagDef[], armorHeadFlagDef[],
               armorLegsFlagDef[], armorFeetFlagDef[], armorHandsFlagDef[],
               armorArmsFlagDef[], armorAboutFlagDef[], armorWristFlagDef[],
               armorEyesFlagDef[], armorFaceFlagDef[], armorHorseFlagDef[],
               armorMiscFlagDef[], contFlagDef[], totemSphereFlagDef[],
               shieldMiscFlagDef[];

//
// getObjTypeStrn : Returns a descriptive string based on the object type
//
//   objType : la la la
//

const char *getObjTypeStrn(const ulong objType)
{
  if ((objType > ITEM_HIGHEST) || (objType < ITEM_LOWEST))
    return "out of range";

  switch (objType)
  {
    case ITEM_LIGHT : return "light source";
    case ITEM_SCROLL : return "scroll";
    case ITEM_WAND : return "wand";
    case ITEM_STAFF : return "staff";
    case ITEM_WEAPON : return "weapon";
    case ITEM_FIREWEAPON : return "ranged weapon";
    case ITEM_MISSILE : return "missile";
    case ITEM_TREASURE : return "treasure";
    case ITEM_ARMOR : return "armor";
    case ITEM_POTION : return "potion";
    case ITEM_WORN : return "worn (not armor)";
    case ITEM_OTHER : return "other";
    case ITEM_TRASH : return "trash";
    case ITEM_TRAP : return "trap";
    case ITEM_CONTAINER : return "container";
    case ITEM_NOTE : return "note";
    case ITEM_DRINKCON : return "drink container";
    case ITEM_KEY : return "key";
    case ITEM_FOOD : return "food";
    case ITEM_MONEY : return "money";
    case ITEM_PEN : return "pen/quill";
    case ITEM_BOAT : return "boat";
    case ITEM_BATTERY : return "battery (\?\?\?)";
    case ITEM_TELEPORT : return "teleport";
    case ITEM_TIMER : return "timer (not imped)";
    case ITEM_VEHICLE : return "vehicle";
    case ITEM_SHIP : return "ship";
    case ITEM_SWITCH : return "switch";
    case ITEM_QUIVER : return "quiver";
    case ITEM_PICK : return "lockpick";
    case ITEM_INSTRUMENT : return "instrument";
    case ITEM_SPELLBOOK : return "spellbook";
    case ITEM_TOTEM : return "totem";
    case ITEM_STORAGE : return "storage chest";
    case ITEM_SCABBARD : return "scabbard";
    case ITEM_SHIELD : return "shield";
    case ITEM_CORPSE_NPC : return "illegal item type";
    case ITEM_CORPSE_PC : return "illegal item type";
    case ITEM_PORTAL : return "portal (don't use)";
    case ITEM_PILL : return "illegal item type";
    case ITEM_MESSAGE_BOARD : return "illegal item type";
    case ITEM_WANTED_LIST : return "illegal item type";
    case ITEM_WALL : return "illegal item type";
    case ITEM_AMMO : return "illegal item type";
    case ITEM_ROPE : return "rope";
    case ITEM_DISGUISE_KIT : return "illegal item type";
    case ITEM_CONTROL_PANEL : return "illegal item type";
    case ITEM_VEHICLE_KEY : return "illegal item type";
    case ITEM_SUMMON : return "summon";

    default : return "unrecognized type";
  }
}


//
// getObjTypeStrnShort : Returns a descriptive string based on the object type
//
//   objType : la la la
//

const char *getObjTypeStrnShort(const ulong objType)
{
  if ((objType > ITEM_HIGHEST) || (objType < ITEM_LOWEST))
    return "invalid";

  switch (objType)
  {
    case ITEM_LIGHT : return "light";
    case ITEM_SCROLL : return "scroll";
    case ITEM_WAND : return "wand";
    case ITEM_STAFF : return "staff";
    case ITEM_WEAPON : return "weapon";
    case ITEM_FIREWEAPON : return "ranged";
    case ITEM_MISSILE : return "missile";
    case ITEM_TREASURE : return "treasure";
    case ITEM_ARMOR : return "armor";
    case ITEM_POTION : return "potion";
    case ITEM_WORN : return "worn";
    case ITEM_OTHER : return "other";
    case ITEM_TRASH : return "trash";
    case ITEM_TRAP : return "trap";
    case ITEM_CONTAINER : return "cont.";
    case ITEM_NOTE : return "note";
    case ITEM_DRINKCON : return "drink cont";
    case ITEM_KEY : return "key";
    case ITEM_FOOD : return "food";
    case ITEM_MONEY : return "money";
    case ITEM_PEN : return "pen/quill";
    case ITEM_BOAT : return "boat (shallow-water)";
    case ITEM_BATTERY : return "battery";
    case ITEM_TELEPORT : return "teleport";
    case ITEM_TIMER : return "timer";
    case ITEM_VEHICLE : return "vehicle";
    case ITEM_SHIP : return "ship (deep-water)";
    case ITEM_SWITCH : return "switch";
    case ITEM_QUIVER : return "quiver";
    case ITEM_PICK : return "lockpick";
    case ITEM_INSTRUMENT : return "instr.";
    case ITEM_SPELLBOOK : return "spellbook";
    case ITEM_TOTEM : return "totem";
    case ITEM_STORAGE : return "storage";
    case ITEM_SCABBARD : return "scabbard";
    case ITEM_SHIELD : return "shield";
    case ITEM_DISGUISE_KIT : return "illegal item type";
    case ITEM_CONTROL_PANEL : return "illegal item type";
    case ITEM_VEHICLE_KEY : return "illegal item type";
    case ITEM_SUMMON : return "summon";

    default : return "unrecog";
  }
}


//
// getObjApplyStrn : Returns a descriptive string based on where the
//                   application is applied.
//
//   applyWhere : gee whiz
//

const char *getObjApplyStrn(const ulong applyWhere)
{
  if (applyWhere > APPLY_HIGHEST) return "apply loc too high";

  switch (applyWhere)
  {
    case APPLY_NONE : return "none";
    case APPLY_STR : return "strength";
    case APPLY_DEX : return "dexterity";
    case APPLY_INT : return "intelligence";
    case APPLY_WIS : return "wisdom";
    case APPLY_CON : return "constitution";
    case APPLY_SEX : return "sex (do not use)";
    case APPLY_CLASS : return "class (do not use)";
    case APPLY_LEVEL : return "level (do not use)";
    case APPLY_AGE : return "age";
    case APPLY_CHAR_WEIGHT : return "weight";
    case APPLY_CHAR_HEIGHT : return "height";
    case APPLY_MANA : return "mana";
    case APPLY_HIT : return "hit points";
    case APPLY_MOVE : return "movement points";
    case APPLY_GOLD : return "gold (do not use)";
    case APPLY_EXP : return "experience (do not use)";
//    case APPLY_AC : return "armor class (not used)";
    case APPLY_AC : return "armor class";
    case APPLY_HITROLL : return "hitroll";
    case APPLY_DAMROLL : return "damroll";
    case APPLY_SAVING_PARA : return "save vs. paralyze";
    case APPLY_SAVING_POISON : return "save vs. poison";
    case APPLY_SAVING_DISEASE : return "save vs. disease";
    case APPLY_SAVING_BREATH : return "save vs. breath";
    case APPLY_SAVING_SPELL : return "save vs. spell";
    case APPLY_FIRE_PROT : return "fire prot (wearer)";
    case APPLY_AGILITY : return "agility";
    case APPLY_POWER : return "power";
    case APPLY_CHARISMA : return "charisma";
//    case APPLY_KARMA : return "karma";
    case APPLY_LUCK : return "luck";
    case APPLY_STR_MAX : return "max strength";
    case APPLY_DEX_MAX : return "max dexterity";
    case APPLY_INT_MAX : return "max intelligence";
    case APPLY_WIS_MAX : return "max wisdom";
    case APPLY_CON_MAX : return "max constitution";
    case APPLY_AGI_MAX : return "max agility";
    case APPLY_POW_MAX : return "max power";
    case APPLY_CHA_MAX : return "max charisma";
//    case APPLY_KARMA_MAX : return "max karma";
    case APPLY_LUCK_MAX : return "max luck";
    case APPLY_STR_RACE : return "racial strength";
    case APPLY_DEX_RACE : return "racial dexterity";
    case APPLY_INT_RACE : return "racial intelligence";
    case APPLY_WIS_RACE : return "racial wisdom";
    case APPLY_CON_RACE : return "racial constitution";
    case APPLY_AGI_RACE : return "racial agility";
    case APPLY_POW_RACE : return "racial power";
    case APPLY_CHA_RACE : return "racial charisma";
//    case APPLY_KARMA_RACE : return "racial karma";
    case APPLY_LUCK_RACE : return "racial luck";
    case APPLY_CURSE : return "curse";

    default : return "unrecognized apply type";
  }
}


//
// getObjApplyStrnShort : Returns a descriptive string based on where the
//                   application is applied.
//
//   applyWhere : gee whiz
//

const char *getObjApplyStrnShort(const ulong applyWhere)
{
  if (applyWhere > APPLY_HIGHEST) return "too high";

  switch (applyWhere)
  {
    case APPLY_NONE : return "none";
    case APPLY_STR : return "str";
    case APPLY_DEX : return "dex";
    case APPLY_INT : return "int";
    case APPLY_WIS : return "wis";
    case APPLY_CON : return "con";
    case APPLY_SEX : return "sex";
    case APPLY_CLASS : return "class";
    case APPLY_LEVEL : return "level";
    case APPLY_AGE : return "age";
    case APPLY_CHAR_WEIGHT : return "weight";
    case APPLY_CHAR_HEIGHT : return "height";
    case APPLY_MANA : return "mana";
    case APPLY_HIT : return "hitpt";
    case APPLY_MOVE : return "movept";
    case APPLY_GOLD : return "gold";
    case APPLY_EXP : return "exp";
//    case APPLY_AC : return "AC (temp)";
    case APPLY_HITROLL : return "hitr";
    case APPLY_DAMROLL : return "damr";
    case APPLY_SAVING_PARA : return "para";
    case APPLY_SAVING_POISON : return "poison";
    case APPLY_SAVING_DISEASE : return "disease";
    case APPLY_SAVING_BREATH : return "breath";
    case APPLY_SAVING_SPELL : return "spell";
    case APPLY_FIRE_PROT : return "fire prot";
    case APPLY_AGILITY : return "agi";
    case APPLY_POWER : return "pow";
    case APPLY_CHARISMA : return "cha";
    case APPLY_KARMA : return "karma";
    case APPLY_LUCK : return "luck";
    case APPLY_STR_MAX : return "max str";
    case APPLY_DEX_MAX : return "max dex";
    case APPLY_INT_MAX : return "max int";
    case APPLY_WIS_MAX : return "max wis";
    case APPLY_CON_MAX : return "max con";
    case APPLY_AGI_MAX : return "max agi";
    case APPLY_POW_MAX : return "max pow";
    case APPLY_CHA_MAX : return "max cha";
    case APPLY_KARMA_MAX : return "max kar";
    case APPLY_LUCK_MAX : return "max luck";
    case APPLY_STR_RACE : return "race str";
    case APPLY_DEX_RACE : return "race dex";
    case APPLY_INT_RACE : return "race int";
    case APPLY_WIS_RACE : return "race wis";
    case APPLY_CON_RACE : return "race con";
    case APPLY_AGI_RACE : return "race agi";
    case APPLY_POW_RACE : return "race pow";
    case APPLY_CHA_RACE : return "race cha";
    case APPLY_KARMA_RACE : return "race kar";
    case APPLY_LUCK_RACE : return "race luck";
    case APPLY_CURSE : return "curse";

    default : return "unrecog";
  }
}


//
// getObjCraftsmanshipStrnShort
//

const char *getObjCraftsmanshipStrnShort(const ulong craft)
{
  if (/*(craft < OBJCRAFT_LOWEST) || */(craft > OBJCRAFT_HIGHEST))
    return "unrecog";

  switch (craft)
  {
    case OBJCRAFT_TERRIBLE        : return "terribly made";
    case OBJCRAFT_EXTREMELY_POOR  : return "extr. poorly made";
    case OBJCRAFT_VERY_POOR       : return "very poorly made";
    case OBJCRAFT_FAIRLY_POOR     : return "fairly poorly made";
    case OBJCRAFT_WELL_BELOW      : return "well below average";
    case OBJCRAFT_BELOW_AVG       : return "below average";
    case OBJCRAFT_SLIGHTLY_BELOW  : return "slightly below avg";
    case OBJCRAFT_AVERAGE         : return "average";
    case OBJCRAFT_SLIGHTLY_ABOVE  : return "slightly above avg";
    case OBJCRAFT_ABOVE_AVG       : return "above average";
    case OBJCRAFT_WELL_ABOVE      : return "well above average";
    case OBJCRAFT_EXCELLENT       : return "excellently made";
    case OBJCRAFT_GOOD_ARTISIAN   : return "skilled artisian";
    case OBJCRAFT_VERY_ARTISIAN   : return "v.skilled artisian";
    case OBJCRAFT_GREAT_ARTISIAN  : return "master artisian";
    case OBJCRAFT_SPOOGEALICIOUS  : return "one-of-a-kind";

    default : return "unrecog";
  }
}


//
// getObjCraftsmanshipStrn
//

const char *getObjCraftsmanshipStrn(const ulong craft)
{
  if (/*(craft < OBJCRAFT_LOWEST) || */(craft > OBJCRAFT_HIGHEST))
    return "unrecog";

  switch (craft)
  {
    case OBJCRAFT_TERRIBLE        : return "terribly made";
    case OBJCRAFT_EXTREMELY_POOR  : return "extremely poorly made";
    case OBJCRAFT_VERY_POOR       : return "very poorly made";
    case OBJCRAFT_FAIRLY_POOR     : return "fairly poorly made";
    case OBJCRAFT_WELL_BELOW      : return "well below average";
    case OBJCRAFT_BELOW_AVG       : return "below average";
    case OBJCRAFT_SLIGHTLY_BELOW  : return "slightly below average";
    case OBJCRAFT_AVERAGE         : return "average";
    case OBJCRAFT_SLIGHTLY_ABOVE  : return "slightly above average";
    case OBJCRAFT_ABOVE_AVG       : return "above average";
    case OBJCRAFT_WELL_ABOVE      : return "well above average";
    case OBJCRAFT_EXCELLENT       : return "excellently made";
    case OBJCRAFT_GOOD_ARTISIAN   : return "made by a skilled artisian";
    case OBJCRAFT_VERY_ARTISIAN   : return "made by a very skilled artisian";
    case OBJCRAFT_GREAT_ARTISIAN  : return "made by a master artisian";
    case OBJCRAFT_SPOOGEALICIOUS  : return "one-of-a-kind craftsmanship";

    default : return "unrecog";
  }
}


//
// getObjSizeStrnShort
//

const char *getObjSizeStrnShort(const long size)
{
  if ((size < OBJSIZE_LOWEST) || (size > OBJSIZE_HIGHEST))
    return "unrecog";

  switch (size)
  {
    case OBJSIZE_NONE      : return "none";
    case OBJSIZE_TINY      : return "tiny";
    case OBJSIZE_SMALL     : return "small";
    case OBJSIZE_MEDIUM    : return "medium";
    case OBJSIZE_LARGE     : return "large";
    case OBJSIZE_HUGE      : return "huge";
    case OBJSIZE_GIANT     : return "giant";
    case OBJSIZE_GARGANT   : return "gargantuan";
    case OBJSIZE_SMALLMED  : return "small-medium";
    case OBJSIZE_MEDLARGE  : return "medium-large";
    case OBJSIZE_MAGICAL   : return "magical";

    default : return "unrecog";
  }
}


//
// getObjSizeStrn
//

const char *getObjSizeStrn(const long size)
{
  if ((size < OBJSIZE_LOWEST) || (size > OBJSIZE_HIGHEST))
    return "unrecog";

  switch (size)
  {
    case OBJSIZE_NONE      :
return "none (parasites, bacteria)                             [very small]";
    case OBJSIZE_TINY      :
return "tiny (insects, birds, snakes ...)                      [2' or less]";
    case OBJSIZE_SMALL     :
return "small (halfling, gnome, duergar, goblin, faerie ...)   [2-4']";
    case OBJSIZE_MEDIUM    :
return "medium (human, mountain dwarf, illithid, orc ...)      [4-7']";
    case OBJSIZE_LARGE     :
return "large (barbarian, troll, horse, centaur, minotaur ...) [7-12']";
    case OBJSIZE_HUGE      :
return "huge (golem, ogre, elementals)                         [12-25']";
    case OBJSIZE_GIANT     :
return "giant (demon, devil, giant, tree)                      [25-50']";
    case OBJSIZE_GARGANT   :
return "gargantuan (dragon, dragonkin)                         [50'+]";
    case OBJSIZE_SMALLMED  :
return "small-medium (any small or medium race)                [2-7']";
    case OBJSIZE_MEDLARGE  :
return "medium-large (any medium or large race)                [4-12']";
    case OBJSIZE_MAGICAL   :
return "magical (any)                                          [use VERY rarely]";

    default : return "unrecog";
  }
}


//
// getMaterialStrn
//

const char *getMaterialStrn(const long material)
{
  if ((material < MAT_LOWEST) || (material > MAT_HIGHEST))
    return "&+Runrecog&n";

  switch (material)
  {
    case MAT_NONSUBSTANTIAL : return "&+Lnon-substantial&n";
    case MAT_FLESH : return "&+rflesh&n";
    case MAT_CLOTH : return "cloth";
    case MAT_BARK : return "&+ybark&n";
    case MAT_SOFTWOOD : return "&+ysoft wood&n";
    case MAT_HARDWOOD : return "&+yhard wood&n";
    case MAT_SILICON : return "&+Wglass&n";
    case MAT_CRYSTAL : return "&+gcrystal&n";
    case MAT_CERAMIC : return "&+yceramic/clay&n";
    case MAT_BONE : return "&+Wbone&n";
    case MAT_STONE : return "&+Lstone&n";
    case MAT_HIDE : return "&+yhide&n";
    case MAT_LEATHER : return "&+yleather&n";
    case MAT_CURED_LEATHER : return "&+ycured leather&n";
    case MAT_IRON : return "&+ciron&n";
    case MAT_STEEL : return "&+Csteel&n";
    case MAT_BRASS : return "&+ybrass&n";
    case MAT_MITHRIL : return "&+mmithril&n";
    case MAT_ADAMANTIUM : return "&+Madamantium&n";
    case MAT_BRONZE : return "&+ybronze&n";
    case MAT_COPPER : return "&+ycopper&n";
    case MAT_SILVER : return "&+csilver&n";
    case MAT_ELECTRUM : return "&+Celectrum&n";
    case MAT_GOLD : return "&+Ygold&n";
    case MAT_PLATINUM : return "&+Wplatinum&n";
    case MAT_GEM : return "&+ggem&n";
    case MAT_DIAMOND : return "&+Wdiamond&n";
    case MAT_PAPER : return "&+Wpaper&n";
    case MAT_PARCHMENT : return "&+yparchment&n";
    case MAT_LEAVES : return "&+gleaves&n";
    case MAT_RUBY : return "&+rruby&n";
    case MAT_EMERALD : return "&+gemerald&n";
    case MAT_SAPPHIRE : return "&+bsapphire&n";
    case MAT_IVORY : return "&+Wivory&n";
    case MAT_DRAGONSCALE : return "&+gdragonscale&n";
    case MAT_OBSIDIAN : return "&+Lobsidian&n";
    case MAT_GRANITE : return "&+Lgranite&n";
    case MAT_MARBLE : return "&+Wmarble&n";
    case MAT_LIMESTONE : return "&+glimestone&n";
    case MAT_LIQUID : return "&+cliquid&n";
    case MAT_BAMBOO : return "&+ybamboo&n";
    case MAT_REEDS : return "&+greeds&n";
    case MAT_HEMP : return "&+ghemp&n";
    case MAT_GLASSTEEL : return "&+cglassteel&n";
    case MAT_EGGSHELL : return "&+Weggshell&n";
    case MAT_CHITINOUS : return "&+ychitinous&n";
    case MAT_REPTILESCALE : return "&+greptile scale&n";
    case MAT_GENERICFOOD : return "&+Ygeneric food&n";
    case MAT_RUBBER : return "&+Lrubber&n";
    case MAT_FEATHER : return "&+Wfeather&n";
    case MAT_WAX : return "&+Wwax&n";
    case MAT_PEARL : return "&+Wpearl&n";
    case MAT_SILK : return "&+Wsilk&n";
    case MAT_SPIDERSILK : return "&+Lspider&+Wsilk&n";
    case MAT_JADE : return "&+gjade&n";
    case MAT_LEAD : return "&+Llead&n";

    default : return "&+Runrecog&n";
  }
}


//
// getLiqTypeStrn
//

const char *getLiqTypeStrn(const ulong liquidType)
{
  if (liquidType > LIQ_HIGHEST) return "liq type too high";

  switch (liquidType)
  {
    case LIQ_WATER      : return "water";
    case LIQ_BEER       : return "beer";
    case LIQ_WINE       : return "wine";
    case LIQ_ALE        : return "ale";
    case LIQ_DARKALE    : return "dark ale";
    case LIQ_WHISKY     : return "whiskey";
    case LIQ_LEMONADE   : return "lemonade";
    case LIQ_FIREBRT    : return "firebreather";
    case LIQ_LOCALSPC   : return "local spec";
    case LIQ_SLIME      : return "slime";
    case LIQ_MILK       : return "milk";
    case LIQ_TEA        : return "tea";
    case LIQ_COFFEE     : return "coffee";
    case LIQ_BLOOD      : return "blood";
    case LIQ_SALTWATER  : return "salt water";
    case LIQ_COKE       : return "coke";
    case LIQ_FOUNTAIN   : return "fountain";
    case LIQ_HOLYWATER  : return "holy water";
    case LIQ_MISO       : return "miso";
    case LIQ_MINESTRONE : return "minestrone";
    case LIQ_DUTCH      : return "dutch";
    case LIQ_SHARK      : return "shark soup";
    case LIQ_BIRD       : return "bird soup";
    case LIQ_CHAMPAGNE  : return "champagne";
    case LIQ_PEPSI      : return "pepsi";
    case LIQ_LITWATER   : return "lit water";
    case LIQ_SAKE       : return "sake";
    case LIQ_POISON     : return "poison";
    case LIQ_UNHOLYWAT  : return "unholy water";
    case LIQ_SCHNAPPES  : return "peppermint schnappes";

    default : return "unrecognized type";
  }
}


//
// getWeapDamStrn
//

const char *getWeapDamStrn(const ulong weaponType)
{
  if ((weaponType > WEAP_HIGHEST) || (weaponType < WEAP_LOWEST))
     return "out of range";

  switch (weaponType)
  {
    case WEAP_WHIP     : return "slashing (whips)";
    case WEAP_SLASH    : return "slashing (slashes)";
    case WEAP_CRUSH    : return "bludgeoning (crushes)";
    case WEAP_POUND    : return "bludgeoning (pounds)";
    case WEAP_PIERCE   : return "piercing (pierces)";

    default : return "unrecognized type";
  }
}


//
// getWeapTypeStrn
//

const char *getWeapTypeStrn(const ulong weaponType)
{
  if ((weaponType > WEAPON_HIGHEST) || (weaponType < WEAPON_LOWEST))
     return "out of range";

  switch (weaponType)
  {
    case WEAPON_AXE : return "axe";
    case WEAPON_DAGGER : return "daggerf";
    case WEAPON_FLAIL : return "flail";
    case WEAPON_HAMMER : return "hammer";
    case WEAPON_LONGSWORD : return "longsword";
    case WEAPON_MACE : return "mace";
    case WEAPON_SPIKED_MACE : return "spiked mace";
    case WEAPON_POLEARM : return "polearm";
    case WEAPON_SHORTSWORD : return "shortsword";
    case WEAPON_CLUB : return "club";
    case WEAPON_SPIKED_CLUB : return "spiked club";
    case WEAPON_STAFF : return "staff";
    case WEAPON_2HANDSWORD : return "two-handed sword";
    case WEAPON_WHIP : return "whip";
    case WEAPON_PICK : return "pick";
    case WEAPON_LANCE : return "lance";
    case WEAPON_SICKLE : return "sickle";
    case WEAPON_FORK : return "fork/rake";
    case WEAPON_HORN : return "horn";
    case WEAPON_NUMCHUCKS : return "nunchaku";
    case WEAPON_SPEAR : return "spear";
    case WEAPON_BATTLEAXE : return "battle axe";
    case WEAPON_KATANA : return "katana";
    case WEAPON_BASTARDSWORD : return "bastard sword";
    case WEAPON_MORNINGSTAR : return "morningstar";
    case WEAPON_RAPIER : return "rapier";
    case WEAPON_SCIMITAR : return "scimitar";
    case WEAPON_SABRE : return "sabre";
    case WEAPON_CUTLASS : return "cutlass";
    case WEAPON_WARHAMMER : return "warhammer";
    case WEAPON_CLAYMORE : return "claymore";
    case WEAPON_KHOPESH : return "khopesh";
    case WEAPON_FLAMBERGE : return "flamberge";
    case WEAPON_GREATAXE : return "great axe";
    case WEAPON_HALBERD : return "halberd";
    case WEAPON_GLAIVE : return "glaive";
    case WEAPON_FALCHION : return "falchion";
    case WEAPON_NAGINATA : return "naginata";
    case WEAPON_PIKE : return "pike";
    case WEAPON_WAKIZASHI : return "wakizashi";
    case WEAPON_SCYTHE : return "scythe";
    case WEAPON_PARRYINGDAGGER : return "parrying dagger";
    case WEAPON_KNIFE : return "knife";
    case WEAPON_DIRK : return "dirk";
    case WEAPON_KRIS : return "kris";
    case WEAPON_CLAW : return "claw";
    case WEAPON_FANG : return "fang";
    case WEAPON_GYTHKA : return "gythka";
    case WEAPON_CHATKCHA : return "chatkcha";
    case WEAPON_SCEPTER : return "scepter";
    case WEAPON_QUARTERSTAFF : return "quarterstaff";
    case WEAPON_STILETTO : return "stiletto";
    case WEAPON_TRIDENT : return "trident";

    default : return "unrecognized type";
  }
}


//
// getArmorTypeStrn
//

const char *getArmorTypeStrn(const long armorType)
{
  if ((armorType > ARMOR_TYPE_HIGHEST) || (armorType < ARMOR_TYPE_LOWEST))
     return "out of range";

  switch (armorType)
  {
    case ARMOR_TYPE_NONE            : return "none";
    case ARMOR_TYPE_LEATHER         : return "leather";
    case ARMOR_TYPE_STUDDED_LEATHER : return "studded leather";
    case ARMOR_TYPE_PADDED_LEATHER  : return "padded leather";
    case ARMOR_TYPE_RING            : return "ringmail";
    case ARMOR_TYPE_HIDE            : return "hide";
    case ARMOR_TYPE_SCALE           : return "scalemail";
    case ARMOR_TYPE_CHAIN           : return "chainmail";
    case ARMOR_TYPE_SPLINT          : return "splintmail";
    case ARMOR_TYPE_BANDED          : return "banded mail";
    case ARMOR_TYPE_PLATE           : return "platemail";
    case ARMOR_TYPE_FIELD_PLATE     : return "field plate";
    case ARMOR_TYPE_FULL_PLATE      : return "full plate";

    default : return "unrecognized";
  }
}


//
// getArmorWornStrn
//

const char *getArmorWornStrn(const long armorWorn)
{
  if ((armorWorn > ARMOR_WEAR_HIGHEST) || (armorWorn < ARMOR_WEAR_LOWEST))
     return "out of range";

  switch (armorWorn)
  {
    case ARMOR_WEAR_FINGER       : return "worn on finger";
    case ARMOR_WEAR_NECK         : return "worn on neck";
    case ARMOR_WEAR_BODY         : return "worn on body";
    case ARMOR_WEAR_HEAD         : return "worn on head";
    case ARMOR_WEAR_LEGS         : return "worn on legs";
    case ARMOR_WEAR_FEET         : return "worn on feet";
    case ARMOR_WEAR_HANDS        : return "worn on hands";
    case ARMOR_WEAR_ARMS         : return "worn on arms";
    case ARMOR_WEAR_ABOUT        : return "worn about body";
    case ARMOR_WEAR_WAIST        : return "worn around waist";
    case ARMOR_WEAR_WRIST        : return "worn on wrist";
    case ARMOR_WEAR_EYES         : return "worn over eye(s)";
    case ARMOR_WEAR_FACE         : return "worn on face";
    case ARMOR_WEAR_EAR          : return "worn in ear";
    case ARMOR_WEAR_QUIVER       : return "worn as quiver";
    case ARMOR_WEAR_BADGE        : return "worn as badge";
    case ARMOR_WEAR_BACK         : return "worn on back";
    case ARMOR_WEAR_ATTACHBELT   : return "worn attached to belt";
    case ARMOR_WEAR_HORSEBODY    : return "worn on horse's body";
    case ARMOR_WEAR_TAIL         : return "worn on tail";
    case ARMOR_WEAR_NOSE         : return "worn in nose";
    case ARMOR_WEAR_HORNS        : return "worn on horns";

    default : return "unrecognized";
  }
}


//
// getArmorWornStrnShort
//

const char *getArmorWornStrnShort(const long armorWorn)
{
  if ((armorWorn > ARMOR_WEAR_HIGHEST) || (armorWorn < ARMOR_WEAR_LOWEST))
     return "out of range";

  switch (armorWorn)
  {
    case ARMOR_WEAR_FINGER       : return "on finger";
    case ARMOR_WEAR_NECK         : return "on neck";
    case ARMOR_WEAR_BODY         : return "on body";
    case ARMOR_WEAR_HEAD         : return "on head";
    case ARMOR_WEAR_LEGS         : return "on legs";
    case ARMOR_WEAR_FEET         : return "on feet";
    case ARMOR_WEAR_HANDS        : return "on hands";
    case ARMOR_WEAR_ARMS         : return "on arms";
    case ARMOR_WEAR_ABOUT        : return "about body";
    case ARMOR_WEAR_WAIST        : return "around waist";
    case ARMOR_WEAR_WRIST        : return "on wrist";
    case ARMOR_WEAR_EYES         : return "over eye(s)";
    case ARMOR_WEAR_FACE         : return "on face";
    case ARMOR_WEAR_EAR          : return "in ear";
    case ARMOR_WEAR_QUIVER       : return "as quiver";
    case ARMOR_WEAR_BADGE        : return "as badge";
    case ARMOR_WEAR_BACK         : return "on back";
    case ARMOR_WEAR_ATTACHBELT   : return "att. to belt";
    case ARMOR_WEAR_HORSEBODY    : return "horse's body";
    case ARMOR_WEAR_TAIL         : return "on tail";
    case ARMOR_WEAR_NOSE         : return "in nose";
    case ARMOR_WEAR_HORNS        : return "on horns";

    default : return "unrecognized";
  }
}


//
// getArmorThicknessStrn
//

const char *getArmorThicknessStrn(const long armorThickness)
{
  if ((armorThickness > ARMOR_THICKNESS_HIGHEST) ||
      (armorThickness < ARMOR_THICKNESS_LOWEST))
     return "out of range";

  switch (armorThickness)
  {
    case ARMOR_THICKNESS_VERY_THIN   : return "very thin";
    case ARMOR_THICKNESS_THIN        : return "thin";
    case ARMOR_THICKNESS_AVERAGE     : return "average";
    case ARMOR_THICKNESS_THICK       : return "thick";
    case ARMOR_THICKNESS_VERY_THICK  : return "very thick";

    default : return "unrecognized";
  }
}


//
// getShieldTypeStrn
//

const char *getShieldTypeStrn(const long shieldType)
{
  if ((shieldType > SHIELDTYPE_HIGHEST) || (shieldType < SHIELDTYPE_LOWEST))
     return "out of range";

  switch (shieldType)
  {
    case SHIELDTYPE_STRAPARM : return "strapped to the arm";
    case SHIELDTYPE_HANDHELD : return "hand-held";

    default : return "unrecognized type";
  }
}


//
// getShieldShapeStrn
//

const char *getShieldShapeStrn(const long shieldShape)
{
  if ((shieldShape > SHIELDSHAPE_HIGHEST) || (shieldShape < SHIELDSHAPE_LOWEST))
     return "out of range";

  switch (shieldShape)
  {
    case SHIELDSHAPE_CIRCULAR : return "circular";
    case SHIELDSHAPE_SQUARE   : return "square";
    case SHIELDSHAPE_RECTVERT : return "rectangular - aligned vertically";
    case SHIELDSHAPE_RECTHORZ : return "rectangular - aligned horizontally";
    case SHIELDSHAPE_OVALVERT : return "oval - aligned vertically";
    case SHIELDSHAPE_OVALHORZ : return "oval - aligned horizontally";
    case SHIELDSHAPE_TRIBIGUP : return "triangle - wide side on top";
    case SHIELDSHAPE_TRISMLUP : return "triangle - wide side on bottom";

    default : return "unrecognized type";
  }
}


//
// getShieldShapeStrnShort
//

const char *getShieldShapeStrnShort(const long shieldShape)
{
  if ((shieldShape > SHIELDSHAPE_HIGHEST) || (shieldShape < SHIELDSHAPE_LOWEST))
     return "out of range";

  switch (shieldShape)
  {
    case SHIELDSHAPE_CIRCULAR : return "circular";
    case SHIELDSHAPE_SQUARE   : return "square";
    case SHIELDSHAPE_RECTVERT : return "rect - vertical";
    case SHIELDSHAPE_RECTHORZ : return "rect - horizontal";
    case SHIELDSHAPE_OVALVERT : return "oval - vertical";
    case SHIELDSHAPE_OVALHORZ : return "oval - horizontal";
    case SHIELDSHAPE_TRIBIGUP : return "tri - wide top";
    case SHIELDSHAPE_TRISMLUP : return "tri - wide bottom";

    default : return "unrecognized type";
  }
}


//
// getShieldSizeStrn
//

const char *getShieldSizeStrn(const long shieldSize)
{
  if ((shieldSize > SHIELDSIZE_HIGHEST) || (shieldSize < SHIELDSIZE_LOWEST))
     return "out of range";

  switch (shieldSize)
  {
    case SHIELDSIZE_TINY   : return "tiny   - really small        (less than 1')";
    case SHIELDSIZE_SMALL  : return "small  - bucklers, etc       (1-2')";
    case SHIELDSIZE_MEDIUM : return "medium - normal shields      (2-3')";
    case SHIELDSIZE_LARGE  : return "large  - larger than average (3-5')";
    case SHIELDSIZE_HUGE   : return "huge   - really big          (5'+)";

    default : return "unrecognized type";
  }
}


//
// getShieldSizeStrnShort
//

const char *getShieldSizeStrnShort(const long shieldSize)
{
  if ((shieldSize > SHIELDSIZE_HIGHEST) || (shieldSize < SHIELDSIZE_LOWEST))
     return "out of range";

  switch (shieldSize)
  {
    case SHIELDSIZE_TINY   : return "tiny";
    case SHIELDSIZE_SMALL  : return "small";
    case SHIELDSIZE_MEDIUM : return "medium";
    case SHIELDSIZE_LARGE  : return "large";
    case SHIELDSIZE_HUGE   : return "huge";

    default : return "unrecognized type";
  }
}


//
// getSpellTypeStrn
//

const char *getSpellTypeStrn(const long spellType)
{
  if (spellType == -1) return "unused spell slot";

  if ((spellType > SPELL_HIGHEST) || (spellType < SPELL_LOWEST))
    return "out of range";

  switch (spellType)
  {
    case SPELL_ARMOR                   : return "armor";
    case SPELL_TELEPORT                : return "teleport";
    case SPELL_BLESS                   : return "bless";
    case SPELL_BLINDNESS               : return "blindness";
    case SPELL_BURNING_HANDS           : return "burning hands";
    case SPELL_CALL_LIGHTNING          : return "call lightning";
    case SPELL_CHARM_PERSON            : return "charm person";
    case SPELL_CHILL_TOUCH             : return "chill touch";
    case SPELL_FULL_HEAL               : return "full heal";
    case SPELL_CONE_OF_COLD            : return "cone of cold";
    case SPELL_CONTROL_WEATHER         : return "control weather (ignored)";
    case SPELL_CREATE_FOOD             : return "create food";
    case SPELL_CREATE_WATER            : return "create water";
    case SPELL_CURE_BLIND              : return "cure blind";
    case SPELL_CURE_CRITIC             : return "cure critic";
    case SPELL_CURE_LIGHT              : return "cure light";
    case SPELL_CURSE                   : return "curse";
    case SPELL_CONTINUAL_LIGHT         : return "continual light";
    case SPELL_DETECT_INVISIBLE        : return "detect invisible";
    case SPELL_MINOR_CREATION          : return "minor creation";
    case SPELL_FLAMESTRIKE             : return "flamestrike";
    case SPELL_DISPEL_EVIL             : return "dispel evil";
    case SPELL_EARTHQUAKE              : return "earthquake";
    case SPELL_ENCHANT_WEAPON          : return "enchant weapon (ignored)";
    case SPELL_ENERGY_DRAIN            : return "energy drain";
    case SPELL_FIREBALL                : return "fireball";
    case SPELL_HARM                    : return "harm";
    case SPELL_HEAL                    : return "heal";
    case SPELL_INVISIBLE               : return "invisible";
    case SPELL_LIGHTNING_BOLT          : return "lightning bolt";
    case SPELL_LOCATE_OBJECT           : return "locate object";
    case SPELL_MAGIC_MISSILE           : return "magic missile";
    case SPELL_POISON                  : return "poison";
    case SPELL_PROTECT_FROM_EVIL       : return "prot from evil";
    case SPELL_REMOVE_CURSE            : return "remove curse";
    case SPELL_STONE_SKIN              : return "stone skin";
    case SPELL_SHOCKING_GRASP          : return "shocking grasp";
    case SPELL_SLEEP                   : return "sleep";
    case SPELL_STRENGTH                : return "strength";
    case SPELL_SUMMON                  : return "summon (ignored)";
    case SPELL_HASTE                   : return "haste";
    case SPELL_WORD_OF_RECALL          : return "word of recall";
    case SPELL_REMOVE_POISON           : return "remove poison";
    case SPELL_SENSE_LIFE              : return "sense life";
    case SPELL_IDENTIFY                : return "identify";
    case SPELL_VENTRILOQUATE           : return "ventriloquate (ignored)";
    case SPELL_FIRESTORM               : return "firestorm";
    case SPELL_FIRE_BREATH             : return "fire breath";
    case SPELL_GAS_BREATH              : return "gas breath (ignored)";
    case SPELL_FROST_BREATH            : return "frost breath";
    case SPELL_ACID_BREATH             : return "acid breath (ignored)";
    case SPELL_LIGHTNING_BREATH        : return "lightning breath (ignored)";
    case SPELL_FARSEE                  : return "farsee";
    case SPELL_FEAR                    : return "fear";
    case SPELL_RECHARGER               : return "recharger (ignored)";
    case SPELL_VITALITY                : return "vitality";
    case SPELL_CURE_SERIOUS            : return "cure serious";
    case SPELL_DESTROY_UNDEAD          : return "destroy undead ";
    case SPELL_METEOR_SWARM            : return "meteor swarm (ignored)";
    case SPELL_CREEPING                : return "creeping doom";
    case SPELL_MINOR_GLOBE             : return "minor globe of invuln";
    case SPELL_CHAIN_LIGHTNING         : return "chain lightning";
    case SPELL_RELOCATE                : return "relocate (ignored)";
    case SPELL_VIGORIZE_LIGHT          : return "vig light";
    case SPELL_VIGORIZE_SERIOUS        : return "vig serious";
    case SPELL_VIGORIZE_CRITIC         : return "vig critic";
    case SPELL_DISPEL_INVISIBLE        : return "dispel invis (ignored)";
    case SPELL_WIZARD_EYE              : return "wizard eye (ignored)";
    case SPELL_CLAIRVOYANCE            : return "clairvoyance (ignored)";
    case SPELL_REJUVENATE_MAJOR        : return "rejuv major";
    case SPELL_RAY_OF_ENFEEBLEMENT     : return "ray of enfeeb";
    case SPELL_DISPEL_GOOD             : return "dispel good";
    case SPELL_DEXTERITY               : return "dexterity";
    case SPELL_REJUVENATE_MINOR        : return "rejuv minor";
    case SPELL_AGE                     : return "aging";
    case SPELL_CYCLONE                 : return "cyclone";
    case SPELL_BIGBYS_CLENCHED_FIST    : return "bigby's fist";
    case SPELL_CONJURE_ELEMENTAL       : return "conj elemental (ignored)";
    case SPELL_VITALIZE_MANA           : return "vitalize mana";
    case SPELL_DIMENSION_DOOR          : return "dimension door (ignored)";
    case SPELL_PROTECT_FROM_GOOD       : return "prot from good";
    case SPELL_ANIMATE_DEAD            : return "animate dead";
    case SPELL_LEVITATE                : return "levitate";
    case SPELL_FLY                     : return "fly";
    case SPELL_WATERBREATH             : return "water breathing";
    case SPELL_PLANE_SHIFT             : return "plane shift (ignored)";
    case SPELL_GATE                    : return "gate (ignored)";
    case SPELL_RESURRECT               : return "resurrect (ignored)";
    case SPELL_MASS_CHARM              : return "mass charm (ignored)";
    case SPELL_DETECT_EVIL             : return "detect evil";
    case SPELL_DETECT_GOOD             : return "detect good";
    case SPELL_DETECT_MAGIC            : return "detect magic";
    case SPELL_DISPEL_MAGIC            : return "dispel magic";
    case SPELL_PRESERVE                : return "preserve";
    case SPELL_MASS_INVIS              : return "mass invis";
    case SPELL_PROTECT_FROM_FIRE       : return "prot from fire";
    case SPELL_PROTECT_FROM_COLD       : return "prot from cold";
    case SPELL_PROTECT_FROM_LIGHTNING  : return "prot from lightning";
    case SPELL_DARKNESS                : return "darkness";
    case SPELL_MINOR_PARALYSIS         : return "minor paralysis";
    case SPELL_MAJOR_PARALYSIS         : return "major paralysis";
    case SPELL_SLOW                    : return "slowness";
    case SPELL_WITHER                  : return "wither";
    case SPELL_PROTECT_FROM_GAS        : return "prot from gas";
    case SPELL_PROTECT_FROM_ACID       : return "prot from acid";
    case SPELL_INFRAVISION             : return "infravision";
    case SPELL_PRISMATIC_SPRAY         : return "prismatic spray";
    case SPELL_FIRESHIELD              : return "fireshield";
    case SPELL_COLOR_SPRAY             : return "color spray";
    case SPELL_INCENDIARY_CLOUD        : return "incendiary cloud";
    case SPELL_ICE_STORM               : return "ice storm";
    case SPELL_DISINTEGRATE            : return "disintegrate";
    case SPELL_CAUSE_LIGHT             : return "cause light";
    case SPELL_CAUSE_SERIOUS           : return "cause serious";
    case SPELL_CAUSE_CRITICAL          : return "cause critical";
    case SPELL_ACID_BLAST              : return "acid blast";
    case SPELL_FAERIE_FIRE             : return "faerie fire";
    case SPELL_FAERIE_FOG              : return "faerie fog";
    case SPELL_PWORD_KILL              : return "power word kill (ignored)";
    case SPELL_PWORD_BLIND             : return "power word blind (ignored)";
    case SPELL_PWORD_STUN              : return "power word stun (ignored)";
    case SPELL_UNHOLY_WORD             : return "unholy word";
    case SPELL_HOLY_WORD               : return "holy word";
    case SPELL_SUNRAY                  : return "sunray";
    case SPELL_FEEBLEMIND              : return "feeblemind";
    case SPELL_SILENCE                 : return "silence";
    case SPELL_TURN_UNDEAD             : return "turn undead";
    case SPELL_COMMAND_UNDEAD          : return "command undead (ignored)";
    case SPELL_SLOW_POISON             : return "slow poison";
    case SPELL_COLDSHIELD              : return "coldshield";
    case SPELL_COMPREHEND_LANGUAGES    : return "comprehend lang";
    case SPELL_WRAITHFORM              : return "wraithform";
    case SPELL_VAMPIRIC_TOUCH          : return "vampiric touch";
    case SPELL_PROT_UNDEAD             : return "prot undead";
    case SPELL_PROT_FROM_UNDEAD        : return "prot from undead";
    case SPELL_COMMAND_HORDE           : return "command horde (ignored)";
    case SPELL_HEAL_UNDEAD             : return "heal undead (ignored)";
    case SPELL_ENTANGLE                : return "entangle (ignored)";
    case SPELL_CREATE_SPRING           : return "create spring";
    case SPELL_BARKSKIN                : return "barkskin";
    case SPELL_MOONWELL                : return "moonwell (ignored)";
    case SPELL_CREATE_DRACOLICH        : return "create dracolich";
    case SPELL_GLOBE                   : return "globe of invuln";
    case SPELL_EMBALM                  : return "embalm";
    case SPELL_SHADOW_BREATH_1         : return "shadow breath 1";
    case SPELL_SHADOW_BREATH_2         : return "shadow breath 2";
    case SPELL_WALL_OF_FLAMES          : return "wall of flames (ignored)";
    case SPELL_WALL_OF_ICE             : return "wall of ice (ignored)";
    case SPELL_WALL_OF_STONE           : return "wall of stone (ignored)";
    case SPELL_WALL_OF_IRON            : return "wall of iron (ignored)";
    case SPELL_WALL_OF_FORCE           : return "wall of force (ignored)";
    case SPELL_WALL_OF_BONES           : return "wall of bones (ignored)";
    case SPELL_LIGHTNING_CURTAIN       : return "lightning curtain (ignored)";
    case SPELL_MOLECULAR_CONTROL       : return "molecular control (ignored)";
    case SPELL_MOLECULAR_AGITATION     : return "molecular agitation";
    case SPELL_ADRENALINE_CONTROL      : return "adrenaline control";
    case SPELL_AURA_SIGHT              : return "aura sight (ignored)";
    case SPELL_AWE                     : return "awe (ignored)";
    case SPELL_BALLISTIC_ATTACK        : return "ballistic attack";
    case SPELL_BIOFEEDBACK             : return "biofeedback";
    case SPELL_CELL_ADJUSTMENT         : return "cell adjustment";
    case SPELL_COMBAT_MIND             : return "combat mind";
    case SPELL_CONTROL_FLAMES          : return "control flames (ignored)";
    case SPELL_CREATE_SOUND            : return "create sound (ignored)";
    case SPELL_DEATH_FIELD             : return "death field";
    case SPELL_DETONATE                : return "detonate";
    case SPELL_DISINTEGRATE2           : return "disintegrate 2 (ignored)";
    case SPELL_DISPLACEMENT            : return "displacement (ignored)";
    case SPELL_DOMINATION              : return "domination";
    case SPELL_ECTOPLASMIC_FORM        : return "ectoplasmic form (ignored)";
    case SPELL_EGO_WHIP                : return "ego whip";
    case SPELL_ENERGY_CONTAINMENT      : return "energy contain (ignored)";
    case SPELL_ENHANCE_ARMOR           : return "enhance armor (ignored)";
    case SPELL_ENHANCED_STR            : return "enhanced str (ignored)";
    case SPELL_ENHANCED_DEX            : return "enhanced dex (ignored)";
    case SPELL_ENHANCED_AGI            : return "enhanced agi (ignored)";
    case SPELL_ENHANCED_CON            : return "enhanced con (ignored)";
    case SPELL_FLESH_ARMOR             : return "flesh armor";
    case SPELL_INERTIAL_BARRIOR        : return "intertial barrier";
    case SPELL_INFLICT_PAIN            : return "inflict pain";
    case SPELL_INTELLECT_FORTRESS      : return "intellect fortress";
    case SPELL_LEND_HEALTH             : return "lend health (ignored)";
    case SPELL_LEVITATION              : return "levitation";
    case SPELL_CONFUSE                 : return "confusion";
    case SPELL_DISEASE                 : return "disease";
    case SPELL_CHARM_ANIMAL            : return "charm animal (ignored)";
    case SPELL_SOULSHIELD              : return "soulshield";
    case SPELL_INVIS_MAJOR             : return "improved invis";
    case SPELL_MASS_HEAL               : return "mass heal";
    case SPELL_ICE_MISSILE             : return "ice missile";
    case SPELL_SPIRIT_ARMOR            : return "spirit armor";
    case SPELL_WOLFSPEED               : return "wolfspeed";
    case SPELL_TRANSFER_WELLNESS       : return "transfer wellness (ignored)";
    case SPELL_FLAMEBURST              : return "flameburst";
    case SPELL_SCALDING_BLAST          : return "scalding burst";
    case SPELL_PYTHONSTING             : return "pythonsting";
    case SPELL_SNAILSPEED              : return "snailspeed";
    case SPELL_MOLEVISION              : return "molevision";
    case SPELL_PURIFY_SPIRIT           : return "purify spirit";
    case SPELL_PANTHERSPEED            : return "pantherspeed";
    case SPELL_MOUSESTRENGTH           : return "mousestrength";
    case SPELL_SUMMON_BEAST            : return "summon beast";
    case SPELL_HAWKVISION              : return "hawkvision";
    case SPELL_SCORCHING_TOUCH         : return "scorching touch";
    case SPELL_MENDING                 : return "mending";
    case SPELL_SOUL_DISTURBANCE        : return "soul disturbance";
    case SPELL_SPIRIT_SIGHT            : return "spirit sight";
    case SPELL_BEARSTRENGTH            : return "bearstrength";
    case SPELL_SHREWTAMENESS           : return "shrewtameness";
    case SPELL_SENSE_SPIRIT            : return "sense spirit";
    case SPELL_MALISON                 : return "malison";
    case SPELL_WELLNESS                : return "wellness";
    case SPELL_GREATER_MENDING         : return "greater mending";
    case SPELL_LIONRAGE                : return "lionrage";
    case SPELL_SPIRIT_ANGUISH          : return "spirit anguish";
    case SPELL_EARTHEN_GRASP           : return "earthen grasp";
    case SPELL_SUMMON_SPIRIT           : return "summon spirit (ignored)";
    case SPELL_GREATER_SOUL_DISTURB    : return "greater soul dist";
    case SPELL_SPIRIT_WARD             : return "spirit ward";
    case SPELL_ELEPHANTSTRENGTH        : return "elephantstrength";
    case SPELL_GREATER_PYTHONSTING     : return "greater pythonsting";
    case SPELL_SCATHING_WIND           : return "scathing wind";
    case SPELL_REVEAL_TRUE_FORM        : return "reveal true form";
    case SPELL_GREATER_SPIRIT_SIGHT    : return "greater spirit sight";
    case SPELL_EARTHEN_RAIN            : return "earthen rain";
    case SPELL_GREATER_SPIRIT_WARD     : return "greater spirit ward";
    case SPELL_SUSTENANCE              : return "sustenance";
    case SPELL_CREATE_EARTHEN_PROJ     : return "create earthen proj (ignored)";
    case SPELL_ANIMATE_EARTHEN_PROJ    : return "animate earthen proj (ignored)";
    case SPELL_GREATER_SUST            : return "greater sustenance";
    case SPELL_COLD_WARD               : return "cold ward";
    case SPELL_FIRE_WARD               : return "fire ward";
    case SPELL_LESSER_MENDING          : return "lesser mending";
    case SPELL_MOLTEN_SPRAY            : return "molten spray";
    case SPELL_TRUE_SEEING             : return "true seeing";
    case SPELL_BOGUS_TEST              : return "test";
    case SPELL_RAVENFLIGHT             : return "ravenflight";
    case SPELL_GREATER_RAVENFLIGHT     : return "greater ravenflight";
    case SPELL_LORE                    : return "lore (ignored)";
    case SPELL_WORMHOLE                : return "wormhole (ignored)";
    case SPELL_ETHERPORTAL             : return "etherportal (ignored)";
    case SPELL_FULL_HARM               : return "full harm";
    case SPELL_CONJURE_GREATER_ELEMENTAL : return "conjure greater elem";
    case SPELL_GROUP_RECALL            : return "group recall";
    case SPELL_GREATER_EARTHEN_GRASP   : return "greater earthen grasp";
    case SPELL_GROUP_STONE_SKIN        : return "group stone skin";
    case SPELL_ENLARGE                 : return "enlarge";
    case SPELL_REDUCE                  : return "reduce";
    case SPELL_REVEAL_SPIRIT_ESSENCE   : return "reveal spirit essence";
    case SPELL_SPIRIT_JUMP             : return "spirit jump (ignored)";
    case SPELL_BEHOLDER_DISIN          : return "beholder disintegrate (ignored)";
    case SPELL_BEHOLDER_DAMAGE         : return "beholder damage (ignored)";
    case SPELL_ARIEKS_SHATTERING_ICEBALL : return "ariek's shattering iceball";
    case SPELL_ENRAGE                  : return "enrage (ignored)";
    case SPELL_BLINDING_BREATH         : return "blinding breath (ignored)";
    case SPELL_MIND_BLANK              : return "mind blank (ignored)";
    case SPELL_SIGHT_LINK              : return "sight link (ignored)";
    case SPELL_CANIBALIZE              : return "cannibalize (ignored)";
    case SPELL_TOWER_IRON_WILL         : return "tower iron will";
    case SPELL_MIRROR_IMAGE            : return "mirror image (ignored)";
    case SPELL_REVEAL_TRUE_NAME        : return "reveal true name (ignored)";
    case SPELL_BLUR                    : return "blur";
    case SPELL_PRISMATIC_CUBE          : return "prismatic cube";
    case SPELL_JUDGEMENT               : return "judgement";
    case SPELL_GREATER_WRAITHFORM      : return "greater wraithform (ignored)";
    case SPELL_ELEMENTAL_FORM          : return "elemental form";
    case SPELL_RAISE_SPECTRE           : return "raise spectre";
    case SPELL_RAISE_WRAITH            : return "raise wraith (ignored)";
    case SPELL_RAISE_VAMPIRE           : return "raise vampire (ignored)";
    case SPELL_RAISE_LICH              : return "raise lich";
    case SPELL_PALADIN_AURA            : return "paladin aura (ignored)";
    case SPELL_MINDSHIELD                  : return "mindshield (ignored)";
    case SPELL_AIRY_WATER                  : return "airy water (ignored)";
    case SPELL_CURE_DISEASE            : return "cure disease";
    case SPELL_POLYMORPH_OBJECT        : return "polymorph into object (ignored)";
    case SPELL_IMMOLATE                : return "immolate";
    case SPELL_APOCALYPSE	           : return "apocalypse";
    case SPELL_SENSE_FOLLOWER	       : return "sense followers";
    case SPELL_PASS_WITHOUT_TRACE      : return "pass without trace (ignored)";
    case SPELL_ETHER_WARP                  : return "etherwarp (ignored)";
    case SPELL_STARSHELL               : return "starshell (ignored)";
    case SPELL_CLOAK_OF_FEAR           : return "cloak of fear (ignored)";
    case SPELL_VAMPIRE                 : return "vampiric trance";
    case SPELL_BIGBYS_CRUSHING_HAND    : return "bigby's crushing hand";
    case SPELL_STORNOGS_SPHERES	       : return "stornog's spheres";
    case SPELL_STORNOGS_GREATER_SPHERES : return "stornog's greater spheres (ignored)";
    case SPELL_GROUP_GLOBE             : return "group globe";
    case SPELL_ETHER_SENSE             : return "ethersense (ignored)";
    case SPELL_CALL_OF_THE_WILD        : return "call of the wild (ignored)";
    case SPELL_HOLY_SACRIFICE          : return "holy sacrifice (ignored)";
    case SPELL_BATTLE_ECSTASY          : return "battle ecstasy";
    case SPELL_DAZZLE                  : return "dazzle (ignored)";
    case SPELL_STINKING_CLOUD          : return "stinking cloud";
    case SPELL_ACCELERATE_METABOLISM   : return "accelerate metabolism";
    case SPELL_SLOWSPELL               : return "slowspell";
    case SPELL_CHRONO_BLAST            : return "chrono blast";
    case SPELL_TEMPORAL_FREEDOM        : return "temporal freedom";
    case SPELL_SENSE_CONTINUUM         : return "sense continuum";
    case SPELL_ALACRITY                : return "alacrity";
    case SPELL_PRECOGNATIVE_SENSE      : return "precognative sense";
    case SPELL_MINOR_EXTENSION         : return "minor extension";
    case SPELL_DEVOLUTIONARY_WARRIOR   : return "devolutionary warrior";
    case SPELL_EVOLUTIONARY_SAVANT     : return "evolutionary savant";
    case SPELL_TEMPORAL_STASIS         : return "temporal stasis";
    case SPELL_CHOKE                   : return "choke";
    case SPELL_GREATER_EXTENSION       : return "greater extension";
    case SPELL_AREA_SLOWNESS           : return "area slowness";
    case SPELL_GROUP_HASTE             : return "group haste";
    case SPELL_AFTERCLAP               : return "afterclap";
    case SPELL_AREA_SLOWSPELL          : return "area slowspell";
    case SPELL_GROUP_ALACRITY          : return "group alacrity";
    case SPELL_TIMEFLOW                : return "timeflow";
    case SPELL_PERMENANCY              : return "permenancy";
    case SPELL_STALL_TIME              : return "stall time";
    case SPELL_ILLUSION_PROWESS        : return "illusion of prowess";
    case SPELL_ILLUSION_INCOMPETENCE   : return "illusion of incompetence";
    case SPELL_SHADOW_SHIELD           : return "shadow shield";
    case SPELL_PRISMATIC_ORB           : return "prismatic orb";
    case SPELL_MAGNETISM               : return "magnetism";
    case SPELL_SHADOW_MAGIC            : return "shadow magic";
    case SPELL_SHADOW_MONSTERS         : return "shadow monsters";
    case SPELL_SHADOW_VEIL             : return "shadow veil";
    case SPELL_DEMI_SHADOW_MONSTERS    : return "demi shadow monsters";
    case SPELL_MISDIRECTION            : return "misdirection";
    case SPELL_DEMI_SHADOW_MAGIC       : return "demi shadow magic";
    case SPELL_SHADES                  : return "shades";
    case SPELL_HYPNOTIC_PATTERN        : return "hypnotic pattern";
    case SPELL_ENTHRALL                : return "enthrall";
    case SPELL_MIRAGE_ARCANA           : return "mirage arcana";
    case SPELL_NIGHTMARES              : return "nightmares";
    case SPELL_HELLFIRE                : return "hellfire";
    case SPELL_SANCTUARY               : return "sanctuary";
    case SPELL_SNAKE_SPIRIT            : return "snake spirit";

    default : return "unrecognized type";
  }
}


//
// getInstrumentTypeStrn
//

const char *getInstrumentTypeStrn(const ulong instType)
{
  if ((instType > INSTRUMENT_HIGHEST) || (instType < INSTRUMENT_LOWEST))
    return "out of range";

  switch (instType)
  {
    case INSTRUMENT_FLUTE    : return "flute";
    case INSTRUMENT_LYRE     : return "lyre";
    case INSTRUMENT_MANDOLIN : return "mandolin";
    case INSTRUMENT_HARP     : return "harp";
    case INSTRUMENT_DRUMS    : return "drums";
    case INSTRUMENT_HORN     : return "horn";
    case INSTRUMENT_PIPES    : return "pipes";
    case INSTRUMENT_FIDDLE   : return "fiddle";
    case INSTRUMENT_DULCIMER : return "dulcimer";

    default : return "unrecognized type";
  }
}


//
// getMissileTypeStrn
//

const char *getMissileTypeStrn(const ulong missileType)
{
  if ((missileType > MISSILE_HIGHEST) || (missileType < MISSILE_LOWEST))
     return "out of range";

  switch (missileType)
  {
    case MISSILE_ARROW              : return "arrow";
    case MISSILE_HAND_CBOW_QUARREL  : return "hand cbow quarrel";
    case MISSILE_LIGHT_CBOW_QUARREL : return "light cbow quarrel";
    case MISSILE_HEAVY_CBOW_QUARREL : return "heavy cbow quarrel";
    case MISSILE_SLING_BULLET       : return "sling bullet";

    default : return "unrecognized type";
  }
}

const char *getLangTypeStrn(const ulong langType)
{
  if ((langType > LANG_HIGHEST) || (langType < LANG_LOWEST))
     return "out of range";

  switch (langType)
  {
    case LANG_UNKNOWN               : return "universal/any";
    case LANG_HUMAN                 : return "human";
    case LANG_ELVEN                 : return "elven";
    case LANG_DWARVEN               : return "dwarven";
    case LANG_CENTAUR               : return "centaur";
    case LANG_OGRE                  : return "ogre";
    case LANG_ORC                   : return "orcish";
    case LANG_TROLL                 : return "troll";
    case LANG_AQUA_ELF              : return "aquatic elf";
    case LANG_SAURIAL               : return "saurial";
    case LANG_THRI_KREEN            : return "thri-kreen";
    case LANG_DRAGON                : return "dragon";
    case LANG_MAGIC                 : return "magic";
    case LANG_GOBLIN                : return "goblin";
    case LANG_HALFLING              : return "halfling";
    case LANG_GITHYANKI             : return "githyanki";
    case LANG_DROW                  : return "drow";
    case LANG_KOBOLD                : return "kobold";
    case LANG_GNOME                 : return "gnome";
    case LANG_ANIMAL                : return "animal";
    case LANG_DUERGAR               : return "duergar";
    case LANG_GITHZERAI             : return "githzerai";
    case LANG_FLIND                 : return "flind";
    case LANG_RAKSHASA              : return "rakshasa";
    case LANG_GNOLL                 : return "gnoll";
    case LANG_BARBARIAN             : return "barbarian";

    default : return "unrecognized type";
  }
}

//
// getTotemSphereStrn
//

char *getTotemSphereStrn(const ulong sphere, char *strn)
{
  strn[0] = '\0';

  if (sphere & 1) strcat(strn, "&+glA&n|");
  if (sphere & 2) strcat(strn, "&+GgA&n|");
  if (sphere & 4) strcat(strn, "&+ylE&n|");
  if (sphere & 8) strcat(strn, "&+YgE&n|");
  if (sphere & 16) strcat(strn, "&+LlS&n|");
  if (sphere & 32) strcat(strn, "&+WgS&n|");

  if (strn[0] && (strn[strlen(strn) - 1] == '|'))
    strn[strlen(strn) - 1] = '\0';

  return strn;
}


//
// getObjTrapAffStrn
//

char *getObjTrapAffStrn(const ulong flag, char *strn)
{
  char ch = 0;

  if (!flag)
  {
    strcpy(strn, "none");
    return strn;
  }

  strn[0] = '\0';

  if (flag & TRAP_EFF_MOVE) strcat(strn, "&+WM&n|");
  if (flag & TRAP_EFF_OBJECT) strcat(strn, "&+YO&n|");
  if (flag & TRAP_EFF_ROOM) strcat(strn, "&+rR&n|");
  if (flag & TRAP_EFF_NORTH) strcat(strn, "&+yN&n");
  if (flag & TRAP_EFF_SOUTH) strcat(strn, "&+yS&n");
  if (flag & TRAP_EFF_WEST) strcat(strn, "&+yW&n");
  if (flag & TRAP_EFF_EAST) strcat(strn, "&+yE&n");
  if (flag & TRAP_EFF_UP) strcat(strn, "&+yU&n");
  if (flag & TRAP_EFF_DOWN) strcat(strn, "&+yD&n");
  if (flag & TRAP_EFF_GLYPH) strcat(strn, "&+BG&n");

  if (flag & TRAP_EFF_OPEN)
  {
    if (strn[0]) ch = strn[strlen(strn - 1)];

    if (ch && (ch != '|')) strcat(strn, "|");
    strcat(strn, "&+cO&n|");
  }

  if (flag & TRAP_EFF_MULTI) strcat(strn, "&+GM&n|");

  if (strn[0] && (strn[strlen(strn) - 1] == '|'))
    strn[strlen(strn) - 1] = '\0';

  return strn;
}


//
// getObjTrapDamStrn
//

const char *getObjTrapDamStrn(const long type)
{
  switch (type)
  {
    case TRAP_DAM_SLEEP          : return "&+Msleep&n";
    case TRAP_DAM_TELEPORT       : return "&+Wteleport&n";
    case TRAP_DAM_FIRE           : return "&+rfire&n";
    case TRAP_DAM_COLD           : return "&+ccold&n";
    case TRAP_DAM_ACID           : return "&+gacid&n";
    case TRAP_DAM_ENERGY         : return "&+benergy&n";
    case TRAP_DAM_BLUNT          : return "blunt";
    case TRAP_DAM_PIERCE         : return "&+Lpiercing&n";
    case TRAP_DAM_SLASH          : return "&+Cslashing&n";
    case TRAP_DAM_DISPEL         : return "&+Bdispel&n";
    case TRAP_DAM_GATE           : return "&+mgate&n";
    case TRAP_DAM_SUMMON         : return "&+Wsummon&n";
    case TRAP_DAM_WITHER         : return "&+Lwither&n";
    case TRAP_DAM_HARM           : return "&+Rharm&n";
    case TRAP_DAM_POISON         : return "&+Gpoison&n";
    case TRAP_DAM_PARALYSIS      : return "&+Lparalysis&n";
    case TRAP_DAM_STUN           : return "&+Ystun&n";
    case TRAP_DAM_DISEASE        : return "&+ydis&+geas&+ye&n";
    case TRAP_DAM_ROCKFALL       : return "&+yfalling rock&n";
    case TRAP_DAM_GAS            : return "&+ggas&n";
    case TRAP_DAM_HOLY           : return "&+Wholy&n";
    case TRAP_DAM_UNHOLY         : return "&+Lunholy&n";
    case TRAP_DAM_SUFFOCATE      : return "&+wasphyxiation&n";
    case TRAP_DAM_DISINTEGRATION : return "&+gdisintegration&n";
    case TRAP_DAM_CRUSHING       : return "&+ycrushing&n";

    default : return "unrec type";
  }
}


//
// getObjValueStrn : Returns a descriptive string based on objType and
//                   valueField
//
//      objType : hmm
//   valueField : gee
//     objValue : golly
//

char *getObjValueStrn(const ulong objType, const ulong valueField,
                      const long objValue, char *strn, char *strn2,
                      const char showCurrentValInfo, const dikuObject *obj)
{
  if (valueField > (NUMB_OBJ_VALS - 1)) return "value field out of range";

  if ((objType > ITEM_HIGHEST) || (objType < ITEM_LOWEST))
    return "obj type out of range";

  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_POTION : /*return "spell level";*/
               case ITEM_SCROLL : /*return "spell level";*/
               case ITEM_WAND   : /*return "spell level";*/
               case ITEM_STAFF  : return "spell level";
               case ITEM_WEAPON : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "weapon type [%s]",
                                            getWeapTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "weapon type");

                                  return strn;
               case ITEM_MISSILE: return "to-hit modifier";
               case ITEM_FIREWEAPON: return "range";
               case ITEM_ARMOR  : return "armor class";
/*
                                  //return "armor class (temp)";  // temporary..

                                  if (armorPosTypeMatters(obj->objValues[1]))
                                  {
                                    if (showCurrentValInfo)
                                    {
                                      sprintf(strn, "armor type [%s]",
                                              getArmorTypeStrn(objValue));
                                    }
                                    else strcpy(strn, "armor type");
                                  }
                                  else strcpy(strn, "unused");

                                  return strn;
*/
               case ITEM_CONTAINER : return "max holdable weight";
               case ITEM_NOTE   : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "language [%s]",
                                            getLangTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "language");

                                  return strn;
               case ITEM_DRINKCON : return "max. drink units";
               case ITEM_FOOD   : return "number of hours filled";
               case ITEM_MONEY  : return "numb copper coins";
               case ITEM_SHIP   : return "vnum of key (if any)";
               case ITEM_VEHICLE : return "vnum of key (if any)";
               case ITEM_TELEPORT : return "target room";
               case ITEM_SUMMON : return "target room";
               case ITEM_SWITCH : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "trigger command [%s]",
                                            getCommandStrn(objValue, TRUE));
                                  }
                                  else strcpy(strn, "trigger command");

                                  return strn;
               case ITEM_QUIVER : return "max capacity";
               case ITEM_PICK : return "% chance added to picking lock";
               case ITEM_INSTRUMENT : if (showCurrentValInfo)
                                      {
                                        sprintf(strn, "instrument type [%s]",
                                                getInstrumentTypeStrn(objValue));
                                      }
                                      else strcpy(strn, "instrument type");

                                      return strn;
               case ITEM_TOTEM : if (showCurrentValInfo)
                                 {
                                   sprintf(strn, "sphere(s) [%s]",
                                           getTotemSphereStrn(objValue, strn2));
                                 }
                                 else strcpy(strn, "sphere(s)");

                                 return strn;
               case ITEM_SHIELD : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "shield type [%s]",
                                            getShieldTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "shield type");

                                  return strn;

               default : return "not used";
             }

    case 1 : switch (objType)
             {
               case ITEM_POTION :
               case ITEM_SCROLL : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "spell type [%s]",
                                            getSpellTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "spell type");

                                  return strn;
               case ITEM_WAND   :
               case ITEM_STAFF  : return "max charges";
               case ITEM_MISSILE :
               case ITEM_WEAPON : return "number of damage dice";
               case ITEM_FIREWEAPON: return "rate of fire";
/*
               case ITEM_ARMOR  : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "main wear location [%s]",
                                            getArmorWornStrnShort(objValue));
                                  }
                                  else strcpy(strn, "main wear location");

                                  return strn;
*/
               case ITEM_CONTAINER :
               case ITEM_QUIVER : return "container flags";
               case ITEM_DRINKCON : return "number of drink units left";
               case ITEM_KEY    : return "% chance of key breaking";
               case ITEM_MONEY  : return "numb silver coins";
               case ITEM_VEHICLE: return "vnum of boarding room";
               case ITEM_SHIP: return "vnum of boarding room";
               case ITEM_TELEPORT : if (showCurrentValInfo)
                                    {
                                      sprintf(strn, "activating command [%s]",
                                              getCommandStrn(objValue, TRUE));
                                    }
                                    else strcpy(strn, "activating command");

                                    return strn;
               case ITEM_SUMMON : if (showCurrentValInfo)
                                    {
                                      sprintf(strn, "activating command [%s]",
                                              getCommandStrn(objValue, TRUE));
                                    }
                                    else strcpy(strn, "activating command");

                                    return strn;
               case ITEM_SWITCH : return "room # with blocked exit";
               case ITEM_PICK   : return "% chance of pick breaking";
               case ITEM_INSTRUMENT : return "level of effect";
               case ITEM_TOTEM : return "ward/ward type (unused)";
               case ITEM_SHIELD : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "shield shape [%s]",
                                            getShieldShapeStrnShort(objValue));
                                  }
                                  else strcpy(strn, "shield shape");

                                  return strn;

               default : return "not used";
             }

    case 2 : switch (objType)
             {
               case ITEM_LIGHT  : return "# hours light lasts";
               case ITEM_POTION :
               case ITEM_SCROLL : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "spell type [%s]",
                                            getSpellTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "spell type");

                                  return strn;
               case ITEM_WAND   :
               case ITEM_STAFF  : return "# charges left";
               case ITEM_MISSILE:
               case ITEM_WEAPON : return "range of damage dice";
              // update this to show flags (a la totem spheres, traps)
//               case ITEM_ARMOR  : return "coverage locations";
               case ITEM_CONTAINER : return "item that opens container";
               case ITEM_DRINKCON : if (showCurrentValInfo)
                                    {
                                      sprintf(strn, "type of liquid [%s]",
                                              getLiqTypeStrn(objValue));
                                    }
                                    else strcpy(strn, "type of liquid");

                                    return strn;
               case ITEM_MONEY  : return "numb gold coins";
               case ITEM_VEHICLE: return "vnum of control room";
               case ITEM_SHIP: return "vnum of control room";
               case ITEM_TELEPORT : return "# charges [-1 = infinite]";
               case ITEM_SWITCH : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "dir of blocked exit [%s]",
                                            getExitStrn(objValue));
                                  }
                                  else strcpy(strn, "dir of blocked exit");

                                  return strn;

               case ITEM_QUIVER : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "missile type [%s]",
                                            getMissileTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "missile type");

                                  return strn;
               case ITEM_INSTRUMENT : return "break chance (1000max)";
               case ITEM_SPELLBOOK : return "number of pages";
               case ITEM_TOTEM : return "level of ward (unused)";
               case ITEM_SHIELD : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "shield size [%s]",
                                            getShieldSizeStrnShort(objValue));
                                  }
                                  else strcpy(strn, "shield size");

                                  return strn;

               default : return "not used";
             }

    case 3 : switch (objType)
             {
               case ITEM_POTION :
               case ITEM_WAND   :
               case ITEM_STAFF  :
               case ITEM_SCROLL : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "spell type [%s]",
                                            getSpellTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "spell type");

                                  return strn;

               case ITEM_FIREWEAPON :
               case ITEM_MISSILE: if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "missile type [%s]",
                                            getMissileTypeStrn(objValue));
                                  }
                                  else strcpy(strn, "missile type");

                                  return strn;
               case ITEM_WEAPON : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "weap damage [%s]",
                                            getWeapDamStrn(objValue));
                                  }
                                  else strcpy(strn, "weapon type");

                                  return strn;
               case ITEM_ARMOR  : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "armor thickness [%s]",
                                            getArmorThicknessStrn(objValue));
                                  }
                                  else strcpy(strn, "armor thickness");

                                  return strn;
               case ITEM_CONTAINER : return "max space capacity";
               case ITEM_VEHICLE: return "maximum occupants";
               case ITEM_SHIP: return "maximum occupants";
               case ITEM_DRINKCON:
               case ITEM_FOOD   : return "poisoned if non-zero";
               case ITEM_MONEY  : return "numb platinum coins";
               case ITEM_SWITCH : return "0 = wall moves, 1 = switch moves";
               case ITEM_QUIVER : return "current amount of missiles";
               case ITEM_INSTRUMENT : return "min level to use";
               case ITEM_SHIELD : return "armor class";
               case ITEM_KEY : return "ship room #1";

               default : return "not used";
             }

    case 4 : switch (objType)
             {
               case ITEM_WEAPON : return "poison type (0 = none)";
               case ITEM_ARMOR : return "misc. armor flags";
               case ITEM_POTION : return "damage caused by drinking";
               case ITEM_VEHICLE: return "max speed (1 to 100)";
               case ITEM_SHIP: return "max speed (1 to 100)";
               case ITEM_SHIELD : if (showCurrentValInfo)
                                  {
                                    sprintf(strn, "shield thickness [%s]",
                                            getArmorThicknessStrn(objValue));
                                  }
                                  else strcpy(strn, "shield thickness");

                                  return strn;
               case ITEM_SWITCH : return "required room #  (0 for any)";
               case ITEM_KEY : return "ship room #2";


               default : return "not used";
             }

    case 5 : switch (objType)
             {
               case ITEM_SHIELD : return "misc. shield flags";
               case ITEM_VEHICLE: return "hull points (1 to 1000)";
               case ITEM_SHIP: return "hull points (1 to 10000)";
               default : return "not used";
             }

    case 6 : switch( objType )
             {
               case ITEM_VEHICLE: return "firepower (1 to 100)";
               case ITEM_SHIP: return "firepower (1 to 500)";
               default: return "not used";
             }

    case 7 : switch( objType )
             {
               case ITEM_VEHICLE: return "cargo hold capacity";
               case ITEM_SHIP: return "cargo hold capacity";
               default : return "mud-wide object limit (0=unlimited)";
             }

    default : return "unrecognized value field";
  }
}


//
// checkForValueList : checks to see if this object type and field can have
//                     a list available of types - returns TRUE and FALSE as
//                     you might expect
//

char checkForValueList(const ulong objType, const ulong valueField)
{
  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_WEAPON :
//               case ITEM_ARMOR :
               case ITEM_TELEPORT :
               case ITEM_SUMMON :
               case ITEM_SWITCH :
               case ITEM_INSTRUMENT :
               case ITEM_SHIELD : return TRUE;
               case ITEM_NOTE: return TRUE;

               default : return FALSE;
             }

    case 1 : switch (objType)
             {
//               case ITEM_ARMOR :
//               case ITEM_WORN :
               case ITEM_SCROLL :
               case ITEM_POTION :
               case ITEM_TELEPORT :
               case ITEM_SUMMON :
               case ITEM_SWITCH :
               case ITEM_SHIELD : return TRUE;

               default : return FALSE;
             }

    case 2 : switch (objType)
             {
               case ITEM_SCROLL :
               case ITEM_POTION :
               case ITEM_CONTAINER :
               case ITEM_DRINKCON :
               case ITEM_SWITCH :
               case ITEM_QUIVER :
               case ITEM_SHIELD : return TRUE;

               default : return FALSE;
             }

    case 3 : switch (objType)
             {
               case ITEM_SCROLL :
               case ITEM_WAND   :
               case ITEM_STAFF  :
               case ITEM_WEAPON :
               case ITEM_ARMOR :
               case ITEM_FIREWEAPON :
               case ITEM_MISSILE:
               case ITEM_POTION : return TRUE;

               default : return FALSE;
             }

    case 4 : switch (objType)
             {
               case ITEM_SHIELD : return TRUE;

               default : return FALSE;
             }

    default : return FALSE;
  }
}


//
// checkForVerboseAvail : returns TRUE if an object field has not only help,
//                        but verbose help available
//

char checkForVerboseAvail(const ulong objType, const ulong valueField)
{
  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_SWITCH : return TRUE;

               default : return FALSE;
             }

    case 1 : switch (objType)
             {
               case ITEM_TELEPORT : return TRUE;
               case ITEM_SUMMON : return TRUE;

               default : return FALSE;
             }

    default : return FALSE;
  }
}


//
// checkForSearchAvail : Returns string that pertains to what the field is
//                       for if you can search by substring
//

const char *checkForSearchAvail(const ulong objType, const ulong valueField)
{
  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_TELEPORT : return "target room";  // target room
               case ITEM_SUMMON : return "target room";  // target room
               case ITEM_SWITCH : return "command";    // activating command

               default : return NULL;
             }

    case 1 : switch (objType)
             {
               case ITEM_SCROLL : //return "spell name";    // spell type
               case ITEM_POTION : return "spell name";    // spell type
               case ITEM_TELEPORT : return "command";  // activating command
               case ITEM_SUMMON : return "command";  // activating command
               case ITEM_SWITCH : return "target room";    // target room

               default : return NULL;
             }

    case 2 : switch (objType)
             {
               case ITEM_SCROLL : //return "spell name";     // spell type
               case ITEM_POTION : return "spell name";     // spell type
               case ITEM_CONTAINER : return "object name";  // item that opens container

               default : return NULL;
             }

    case 3 : switch (objType)
             {
               case ITEM_SCROLL : //return "spell name";   // spell type
               case ITEM_WAND   : //return "spell name";   // spell type
               case ITEM_STAFF  : //return "spell name";   // ditto
               case ITEM_POTION : return "spell name";   // ditto

               default : return NULL;
             }

    default : return NULL;
  }
}


//
// searchObjValue : Corresponds to above, actually search for the shite
//

void searchObjValue(const ulong objType, const ulong valueField)
{
  char strn[256] = "\0", outstrn[256];
  const char *searchVal;

 // just in case ..

  if (!(searchVal = checkForSearchAvail(objType, valueField))) return;

  _setbkcolor(0);

  sprintf(outstrn, "\n&+CEnter substring to search %ss for: &n", searchVal);
  displayColorString(outstrn);

  getStrn(strn, 25, 1, 7, '°', strn, FALSE, FALSE);
  upstrn(strn);  // needed, methinks

  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_TELEPORT : displayRoomList(strn);  // target room
                                    return;
               case ITEM_SWITCH : displayCommandList(strn); // command
                                  return;

               default : return;  // shouldn't happen ..
             }

    case 1 : switch (objType)
             {
               case ITEM_SCROLL :   // spell type
               case ITEM_POTION : displaySpellList(strn);
                                  return;
               case ITEM_TELEPORT : displayCommandList(strn);  // command
                                    return;
               case ITEM_SWITCH : displayRoomList(strn);    // target room
                                  return;

               default : return;
             }

    case 2 : switch (objType)
             {
               case ITEM_SCROLL :   // spell type
               case ITEM_POTION : displaySpellList(strn);
                                  return;
               case ITEM_CONTAINER : displayObjectTypeList(strn, TRUE);  // item that opens container
                                     return;

               default : return;
             }

    case 3 : switch (objType)
             {
               case ITEM_SCROLL :   // spell type
               case ITEM_WAND   :
               case ITEM_STAFF  :
               case ITEM_POTION : displaySpellList(strn);
                                  return;

               default : return;
             }

    default : return;
  }
}


//
// displayObjValueHelp : displays "help" on object values depending on obj type
//

void displayObjValueHelp(const ulong objType, const ulong valueField,
                         const char verbose)
{
  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_WEAPON : displayWeaponTypeList();  return;
//               case ITEM_ARMOR  : displayArmorTypeList();  return;
               case ITEM_TELEPORT : displayRoomList();  return;
               case ITEM_SUMMON : displayRoomList();  return;
               case ITEM_SWITCH : displayCommandList(verbose);  return;
               case ITEM_INSTRUMENT : displayInstrumentList();  return;
               case ITEM_SHIELD : displayShieldTypeList();  return;
               case ITEM_NOTE   : displayLangTypeList(); return;
               default : return;
             }

    case 1 : switch (objType)
             {
//               case ITEM_ARMOR  : displayArmorWornList();  return;
               case ITEM_SCROLL :
               case ITEM_POTION : displaySpellList();  return;
               case ITEM_TELEPORT : displayCommandList(verbose);  return;
               case ITEM_SUMMON : displayCommandList(verbose);  return;
               case ITEM_SWITCH : displayRoomList();  return;
               case ITEM_SHIELD : displayShieldShapeList();  return;

               default : return;
             }

    case 2 : switch (objType)
             {
               case ITEM_SCROLL :
               case ITEM_POTION : displaySpellList();  return;
               case ITEM_CONTAINER : displayObjectTypeList();  return;
               case ITEM_DRINKCON : displayLiquidList();  return;
               case ITEM_SWITCH : displayDirectionList();  return;
               case ITEM_QUIVER : displayMissileTypeList();  return;
               case ITEM_SHIELD : displayShieldSizeList();  return;

               default : return;
             }

    case 3 : switch (objType)
             {
               case ITEM_SCROLL :
               case ITEM_POTION :
               case ITEM_WAND   :
               case ITEM_STAFF  : displaySpellList();  return;
               case ITEM_FIREWEAPON :
               case ITEM_MISSILE: displayMissileTypeList();  return;
               case ITEM_WEAPON : displayWeaponDamageList();  return;
               case ITEM_ARMOR : displayArmorThicknessList();  return;

               default : return;
             }

    case 4 : switch (objType)
             {
               case ITEM_SHIELD : displayArmorThicknessList();  return;

               default : return;
             }

    default : return;
  }
}


//
// fieldRefsObjNumb
//

char fieldRefsObjNumb(const ulong objType, const ulong valueField)
{
  switch (valueField)
  {
    case 2 : switch (objType)
             {
               case ITEM_CONTAINER : return TRUE;

               default : return FALSE;
             }

    default : return FALSE;
  }
}


//
// fieldRefsRoomNumb
//

char fieldRefsRoomNumb(const ulong objType, const ulong valueField)
{
  switch (valueField)
  {
    case 0 : switch (objType)
             {
               case ITEM_TELEPORT : return TRUE;

               default : return FALSE;
             }

    case 1 : switch (objType)
             {
               case ITEM_SWITCH : return TRUE;

               default : return FALSE;
             }

    default : return FALSE;
  }
}


//
// fieldRefsVnum
//

/*
char fieldRefsVnum(ulong objType, ulong valueField)
{
  if (fieldRefsObjNumb(objType, valueField)) return TRUE;

  return fieldRefsRoomNumb(objType, valueField);
}
*/


//
// specialObjValEditRedundant
//

void specialObjValEditRedundant(dikuObject *obj, const uchar valueField,
                                const flagDef *flagArr, const uchar nameLen,
                                const char *bitvName)
{
  char strn[256], newName[256];

  if (valueField >= NUMB_OBJ_VALS) return;

  fixString(obj->objShortName, newName, nameLen, TRUE);

  sprintf(strn,
"&+gEditing %s for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          bitvName, obj->objNumber, newName);

  editFlags(flagArr, &(obj->objValues[valueField]), strn,
            bitvName, NULL, 0);
}


//
// specialObjValEdit
//

char specialObjValEdit(dikuObject *obj, const uchar valueField,
                       const char editing)
{
  uchar objType;
  int i;

  if (!obj) return FALSE;

  objType = obj->objType;

  switch (objType)
  {
    case ITEM_QUIVER    :
    case ITEM_CONTAINER :
      if (valueField == 1)
      {
        if (!editing) return TRUE;
        else
        {
          specialObjValEditRedundant(obj, 1, contFlagDef, MAX_OBJSHORT_LEN - 5,
                                     "container flags");

          return TRUE;
        }
      }

      return FALSE;

    case ITEM_TOTEM :
      if (valueField == 0)
      {
        if (!editing) return TRUE;
        else
        {
          specialObjValEditRedundant(obj, 0, totemSphereFlagDef,
                                     MAX_OBJSHORT_LEN - 15, "totem sphere flags");

          return TRUE;
        }
      }

      return FALSE;

    case ITEM_SHIELD :
      if (valueField == 5)
      {
        if (!editing) return TRUE;
        else
        {
          specialObjValEditRedundant(obj, 5, shieldMiscFlagDef,
                                     MAX_OBJSHORT_LEN - 15, "misc shield flags");

          return TRUE;
        }
      }

      return FALSE;

    default : return FALSE;
  }
}
