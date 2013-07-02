// MOB.H - useful stuff for mobs

#ifndef _MOB_H_

#include "mobbits.h"
#include "../quest/quest.h"
#include "../shop/shop.h"

// mob positions

#define POSITION_LOWEST       0
#define POSITION_LOWEST_LEGAL 4  // lowest "legal" value for a loaded mob
#define POSITION_DEAD         0
#define POSITION_MORTALLYW    1
#define POSITION_INCAP        2
#define POSITION_STUNNED      3
#define POSITION_SLEEPING     4
#define POSITION_RESTING      5
#define POSITION_SITTING      6
#define POSITION_FIGHTING     7
#define POSITION_STANDING     8
#define POSITION_KNEELING     9
#define POSITION_HIGHEST      9


// mob sex

#define SEX_LOWEST    0
#define SEX_NEUTER    0
#define SEX_MALE      1
#define SEX_FEMALE    2
#define SEX_HIGHEST   2


// mob class info

#define CLASS_LOWEST          0
#define CLASS_NONE            0
#define CLASS_WARRIOR         1
#define CLASS_RANGER          2
#define CLASS_PSIONICIST      3
#define CLASS_PALADIN         4
#define CLASS_ANTIPALADIN     5
#define CLASS_CLERIC          6
#define CLASS_MONK            7
#define CLASS_DRUID           8
#define CLASS_SHAMAN          9
#define CLASS_SORCERER       10
#define CLASS_NECROMANCER    11
#define CLASS_CONJURER       12
#define CLASS_THIEF          13
#define CLASS_ASSASSIN       14
#define CLASS_MERCENARY      15
#define CLASS_BARD           16
#define CLASS_ILLUSIONIST    17
#define CLASS_ELEM_FIRE      18
#define CLASS_ELEM_EARTH     19
#define CLASS_ELEM_AIR       20
#define CLASS_ELEM_WATER     21
#define CLASS_HUNTER         22
#define CLASS_CHRONOMANCER   23
#define CLASS_WARLOCK        24
#define CLASS_ENSLAVER       25
#define CLASS_HIGHEST        25


// mob species

#define MOB_HUMAN        "PH"
#define MOB_BARBARIAN    "PB"
#define MOB_DROW_ELF     "PL"
#define MOB_GREY_ELF     "PE"
#define MOB_MOUNT_DWARF  "PM"
#define MOB_DUERGAR      "PD"
#define MOB_HALFLING     "PF"
#define MOB_GNOME        "PG"
#define MOB_OGRE         "PO"
#define MOB_TROLL        "PT"
#define MOB_HALF_ELF     "P2"
#define MOB_ILLITHID     "MF"
#define MOB_ORC          "HO"
#define MOB_THRIKREEN    "TK"
#define MOB_CENTAUR      "CT"
#define MOB_GITHYANKI    "GI"
#define MOB_GITHZERAI    "GZ"
#define MOB_MINOTAUR     "MT"
#define MOB_AQUAELF      "PA"
#define MOB_SAHUAGIN     "SA"
#define MOB_GOBLIN       "HG"
#define MOB_RAKSHASA     "RA"
#define MOB_FLIND        "FL"
#define MOB_FIRE_ELEM    "EF"
#define MOB_AIR_ELEM     "EA"
#define MOB_WATER_ELEM   "EW"
#define MOB_EARTH_ELEM   "EE"
#define MOB_DEMON        "X"
#define MOB_DEVIL        "Y"
#define MOB_UNDEAD       "U"
#define MOB_VAMPIRE      "UV"
#define MOB_GHOST        "UG"
#define MOB_LYCANTH      "L"
#define MOB_GIANT        "G"
#define MOB_HALF_ORC     "H2"
#define MOB_GOLEM        "OG"
#define MOB_FAERIE       "HF"
#define MOB_DRAGON       "D"
#define MOB_DRAGONKIN    "DK"
#define MOB_REPTILE      "R"
#define MOB_SNAKE        "RS"
#define MOB_INSECT       "I"
#define MOB_ARACHNID     "AS"
#define MOB_FISH         "F"
#define MOB_BIRD         "B"
#define MOB_HORSE        "AE"
#define MOB_PRIMATE      "AA"
#define MOB_HUMANOID     "H"
#define MOB_ANIMAL       "A"
#define MOB_TREE         "VT"
#define MOB_HERBIVORE    "AH"
#define MOB_CARNIVORE    "AC"
#define MOB_PARASITE     "AP"
#define MOB_BEHOLDER     "BH"
#define MOB_DRACOLICH    "UD"
#define MOB_IXICHITL     "IX"
#define MOB_DERRO        "DE"
#define MOB_KOBOLD       "K"
#define MOB_OBJECT       "O"
#define MOB_MIST         "M"
#define MOB_WORM         "AW"
#define MOB_PLANT        "VP"
#define MOB_BAT          "BB"
#define MOB_BEAR         "BE"
#define MOB_ABOLETH      "AB"
#define MOB_HARPY        "HP"
#define MOB_HYDRA        "DH"
#define MOB_GOD          "GD"
#define MOB_RAT          "AR"
#define MOB_HOBGOBLIN    "HB"
#define MOB_HALFKOBOLD   "K2"
#define MOB_HALFDWARF    "D2"
#define MOB_WEREWOLF     "LW"
#define MOB_RUSTMONSTER  "RM"
#define MOB_WEMIC        "WE"
#define MOB_UMBERHULK    "UH"
#define MOB_ANGEL        "AN"
#define MOB_SLAAD        "SL"
#define MOB_BOAR         "BO"
#define MOB_SHADOW_ELEM  "ES"
#define MOB_NEOGI        "NE"

// hometown stuff

#define HOME_LOWEST              0
#define HOME_NONE                0
#define HOME_THARNADIA           1      /* human, half-elf */
#define HOME_IXARKON             2      /* illithid */
#define HOME_ARACHDRATHOS        3      /* drow elf */
#define HOME_SYLVANDAWN          4      /* grey-elf, half-elf */
#define HOME_KIMORDRIL           5      /* mountain dwarf */
#define HOME_KHILDARAK           6      /* duergar - trip's town */
#define HOME_WOODSEER            7      /* halfling */
#define HOME_ASHRUMITE           8      /* gnome */
#define HOME_FAANG               9      /* ogre */
#define HOME_GHORE              10      /* troll */
#define HOME_UGTA               11      /* barbarians */
#define HOME_BLOODSTONE         12      /* evil humans */
#define HOME_SHADY              13      /* orcs */
#define HOME_SARMIZ             14
#define HOME_MARIGOT            15
#define HOME_HIGHEST            15      /* number of last hometown */

// record etc for mobs themselves

#define MAX_MOBSNAME_LEN   (usint)256
#define MAX_MOBLNAME_LEN   (usint)256
#define MAX_MOBSHORT_LEN    (usint)41  // mob names are "fixed" to this length
#define MAX_MOBSHORTSHORT_LEN  (usint)26
                              // extra-short mob names
#define MAX_MOBKEY_LEN    MAX_STRNNODE_LEN - 16

#define MAX_SPECIES_LEN   (usint)3
#define MAX_MOBHP_LEN    (usint)21
#define MAX_MOBDAM_LEN   (usint)21

typedef struct _dikuMob
{
  ulong mobNumber;  // mob number

  stringNode *keywordListHead;  // keyword list head

  char mobShortName[MAX_MOBSNAME_LEN];  // short name of mob
  char mobLongName[MAX_MOBLNAME_LEN];   // long name of mob

  stringNode *mobDescHead;  // head of mob description
  stringNode *mobProgHead;  // Head of MobProg

  mobActionFlagRec actionBits;
  affect1FlagRec affect1Bits;
  affect2FlagRec affect2Bits;
  affect3FlagRec affect3Bits;
  affect4FlagRec affect4Bits;

  sint alignment;

  char mobSpecies[MAX_SPECIES_LEN];
  char mobHometown;
  int mobClass;

  uchar level;
  long thac0;
  long ac;
  char hitPoints[MAX_MOBHP_LEN];
  char mobDamage[MAX_MOBDAM_LEN];

  ulong exp;

  ulong copper;
  ulong silver;
  ulong gold;
  ulong platinum;

  char position;
  char defaultPos;
  char sex;

  quest *questPtr;
  shop *shopPtr;

  char defaultMob;

  struct _dikuMob *Next;
} dikuMob;


#define _MOB_H_
#endif
