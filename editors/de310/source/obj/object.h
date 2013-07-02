// OBJECT.H - useful stuff for objects

#ifndef _OBJECT_H_

#include "objbits.h"
#include "../mob/mobbits.h"
#include "../edesc/edesc.h"

#include "../types.h"
#include "../misc/strnnode.h"
//#include "../de.h"

#include "liquids.h"
#include "weapons.h"
#include "spells.h"
#include "instrmnt.h"
#include "missiles.h"
#include "traps.h"
#include "material.h"
#include "objsize.h"
#include "shields.h"
#include "armor.h"

// object types

#define ITEM_LOWEST      1
#define ITEM_LIGHT       1
#define ITEM_SCROLL      2
#define ITEM_WAND        3
#define ITEM_STAFF       4
#define ITEM_WEAPON      5
#define ITEM_FIREWEAPON  6
#define ITEM_MISSILE     7
#define ITEM_TREASURE    8
#define ITEM_ARMOR       9
#define ITEM_POTION     10
#define ITEM_WORN       11
#define ITEM_OTHER      12
#define ITEM_TRASH      13
#define ITEM_TRAP       14
#define ITEM_CONTAINER  15
#define ITEM_NOTE       16
#define ITEM_DRINKCON   17
#define ITEM_KEY        18
#define ITEM_FOOD       19
#define ITEM_MONEY      20
#define ITEM_PEN        21
#define ITEM_BOAT       22
#define ITEM_BOOK       23
#define ITEM_BATTERY    24
#define ITEM_TELEPORT   25
#define ITEM_TIMER      26
#define ITEM_VEHICLE    27
#define ITEM_SHIP       28
#define ITEM_SWITCH     29
#define ITEM_QUIVER     30
#define ITEM_PICK       31
#define ITEM_INSTRUMENT 32
#define ITEM_SPELLBOOK  33
#define ITEM_TOTEM      34
#define ITEM_STORAGE    35
#define ITEM_SCABBARD   36
#define ITEM_SHIELD     37
#define ITEM_CORPSE_NPC 38
#define ITEM_CORPSE_PC  39
#define ITEM_PORTAL     40
#define ITEM_PILL       41
#define ITEM_MESSAGE_BOARD 43
#define ITEM_WANTED_LIST   45
#define ITEM_WALL          46
#define ITEM_AMMO          48
#define ITEM_ROPE          50
#define ITEM_DISGUISE_KIT  51
#define ITEM_CONTROL_PANEL 52
#define ITEM_VEHICLE_KEY   53
#define ITEM_SUMMON        54
#define ITEM_HIGHEST    54


#define ITEM_DEFAULT    ITEM_TRASH

// craftsmanship values

#define OBJCRAFT_LOWEST          0
#define OBJCRAFT_TERRIBLE        0 // terribly made
#define OBJCRAFT_EXTREMELY_POOR  1 // extremely poorly made
#define OBJCRAFT_VERY_POOR       2 // very poorly made
#define OBJCRAFT_FAIRLY_POOR     3 // fairly poorly made
#define OBJCRAFT_WELL_BELOW      4 // well below average
#define OBJCRAFT_BELOW_AVG       5 // below average
#define OBJCRAFT_SLIGHTLY_BELOW  6 // slightly below average
#define OBJCRAFT_AVERAGE         7 // average
#define OBJCRAFT_SLIGHTLY_ABOVE  8 // slightly above average
#define OBJCRAFT_ABOVE_AVG       9 // above average
#define OBJCRAFT_WELL_ABOVE     10 // well above average
#define OBJCRAFT_EXCELLENT      11 // excellently made
#define OBJCRAFT_GOOD_ARTISIAN  12 // clearly made by a skilled artisian
#define OBJCRAFT_VERY_ARTISIAN  13 // clearly made by a very skilled artisian
#define OBJCRAFT_GREAT_ARTISIAN 14 // clearly made by a master at the craft
#define OBJCRAFT_SPOOGEALICIOUS 15 // made by a true master - i.e. a master
                                   // dwarf artisian making chainmail, or
                                   // just some really spiffy guy making
                                   // great shit
#define OBJCRAFT_HIGHEST        15

// object apply "locations"

#define APPLY_LOWEST             0
#define APPLY_NONE               0
#define APPLY_STR                1
#define APPLY_DEX                2
#define APPLY_INT                3
#define APPLY_WIS                4
#define APPLY_CON                5
#define APPLY_SEX                6
#define APPLY_CLASS              7
#define APPLY_LEVEL              8
#define APPLY_AGE                9
#define APPLY_CHAR_WEIGHT       10
#define APPLY_CHAR_HEIGHT       11
#define APPLY_MANA              12
#define APPLY_HIT               13
#define APPLY_MOVE              14
#define APPLY_GOLD              15
#define APPLY_EXP               16
#define APPLY_AC                17
#define APPLY_ARMOR             17  // same as APPLY_AC
#define APPLY_HITROLL           18
#define APPLY_DAMROLL           19
#define APPLY_SAVING_PARA       20
#define APPLY_SAVING_POISON     21
#define APPLY_SAVING_DISEASE    22
#define APPLY_SAVING_BREATH     23
#define APPLY_SAVING_SPELL      24
#define APPLY_FIRE_PROT         25
#define APPLY_AGILITY           26
#define APPLY_POWER             27
#define APPLY_CHARISMA          28
#define APPLY_KARMA             29
#define APPLY_LUCK              30
#define APPLY_STR_MAX           31  /* these 10 can raise a stat above 100, I will */
#define APPLY_DEX_MAX           32  /* personally rip the lungs out of anyone using */
#define APPLY_INT_MAX           33  /* these on easy-to-get items.  JAB */
#define APPLY_WIS_MAX           34
#define APPLY_CON_MAX           35
#define APPLY_AGI_MAX           36
#define APPLY_POW_MAX           37
#define APPLY_CHA_MAX           38
#define APPLY_KARMA_MAX         39
#define APPLY_LUCK_MAX          40
#define APPLY_STR_RACE          41  /* these 10 override the racial stat_factor */
#define APPLY_DEX_RACE          42  /* so that setting APPLY_STR_RACE <ogre> will, */
#define APPLY_INT_RACE          43  /* for example, give you gauntlets of ogre strength. */
#define APPLY_WIS_RACE          44  /* these aren't imped yet, but I figured I'd add */
#define APPLY_CON_RACE          45  /* them so I don't forget about it. */
#define APPLY_AGI_RACE          46
#define APPLY_POW_RACE          47
#define APPLY_CHA_RACE          48
#define APPLY_KARMA_RACE        49
#define APPLY_LUCK_RACE         50
#define APPLY_CURSE             51
#define APPLY_HIGHEST           51

#define NUMB_OBJ_ARMOR_MISC_FLAGS    1
#define NUMB_OBJ_CONT_FLAGS          5
#define NUMB_OBJ_TOTEM_SPHERE_FLAGS  6
#define NUMB_OBJ_SHIELD_MISC_FLAGS   1

// Language defines
#define LANG_LOWEST     0
#define LANG_UNKNOWN    0
#define LANG_HUMAN      1
#define LANG_ELVEN      2
#define LANG_DWARVEN    3
#define LANG_CENTAUR    4
#define LANG_OGRE       5
#define LANG_ORC        6
#define LANG_TROLL      7
#define LANG_AQUA_ELF   8
#define LANG_SAURIAL    9
#define LANG_THRI_KREEN 10
#define LANG_DRAGON    12
#define LANG_MAGIC     14
#define LANG_GOBLIN    15
#define LANG_GOD       16
#define LANG_HALFLING  17
#define LANG_GITHYANKI 18
#define LANG_DROW      19
#define LANG_KOBOLD    20
#define LANG_GNOME     21
#define LANG_ANIMAL    24
#define LANG_DUERGAR   25
#define LANG_GITHZERAI 26
#define LANG_FLIND     27
#define LANG_RAKSHASA  28
#define LANG_GNOLL     29
#define LANG_BARBARIAN 30
#define LANG_HIGHEST   30

// record for "object apply" stuff

typedef struct _objApplyRec
{
  uchar applyWhere;

  long applyModifier;
} objApplyRec;


// record etc for objects themselves

#define MAX_OBJLNAME_LEN  (usint)256
#define MAX_OBJSNAME_LEN  (usint)256
#define MAX_OBJSHORT_LEN  (usint)41  // object names are "fixed" to this length
#define MAX_OBJKEY_LEN    MAX_STRNNODE_LEN - 16
                                     // used for input field in edit obj menu

#define NUMB_OBJ_VALS      (usint)8
#define NUMB_OBJ_APPLIES   (usint)2

typedef struct _dikuObject
{
  ulong objNumber;  // object number

  char objShortName[MAX_OBJSNAME_LEN];  // short name of object
  char objLongName[MAX_OBJLNAME_LEN];   // long name of object

  stringNode *keywordListHead;  // keyword list head

  extraDesc *extraDescHead;     // list of extra descs

  uchar objType;
  ulong material;
  ulong size;
  ulong space;

  objExtraFlagRec extraBits;
  objWearFlagRec wearBits;
  objExtra2FlagRec extra2Bits;
  objAntiFlagRec antiBits;
  objAnti2FlagRec anti2Bits;

 // "secret" object bits - the kids dig em

  affect1FlagRec affect1Bits;
  affect2FlagRec affect2Bits;
  affect3FlagRec affect3Bits;
  affect4FlagRec affect4Bits;

  long objValues[NUMB_OBJ_VALS];

  long weight;
  long maxload;
  long globalmaxload;
  ulong value;
  ulong condition;
//  ulong maxSP;  // no need
  ulong craftsmanship;
  ulong damResistBonus;

  objApplyRec objApply[NUMB_OBJ_APPLIES];

  ulong trapAff;
  long trapDam;
  long trapCharge;
  ulong trapLevel;
  long trapPercent;

  char defaultObj;

  struct _dikuObject *Next;
} dikuObject;

#define _OBJECT_H_
#endif
