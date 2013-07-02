/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia3.cpp (3 of 3)
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TObjTypeLibrary for codebases
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

// Classlib includes
#include <classlib\assoc.h>
#include <classlib\dict.h>
#include <classlib\sets.h>

// OWL includes
//#include <owl\inputdia.h>

// C includes
#include <malloc.h>

// Local includes
#include "objdia.h"
//#include "appdia.h"
#include "tobjlib.h"

static DataType far RomDefault[] = {
   { "light", ITEM_LIGHT,
   },

   { "scroll", ITEM_SCROLL,
   },

   { "wand", ITEM_WAND,
   },

   { "staff", ITEM_STAFF,
   },

   { "weapon", ITEM_WEAPON,
   },

   { "treasure", ITEM_TREASURE,
   },

   { "armor", ITEM_ARMOR,
   },

   { "potion", ITEM_POTION,
   },

   { "clothing", 0,  // (Not a legal Merc object type)
   },

   { "furniture", ITEM_FURNITURE,
   },

   { "trash", ITEM_TRASH,
   },

   { "container", ITEM_CONTAINER,
   },

   { "drink", ITEM_DRINK_CON,
   },

   { "key", ITEM_KEY,
   },

   { "food", ITEM_FOOD,
   },

   { "money", ITEM_MONEY,
   },

   { "boat", ITEM_BOAT,
   },

   { "npc_corpse", ITEM_CORPSE_NPC,
   },

   { "fountain", ITEM_FOUNTAIN,
   },

   { "pill", ITEM_PILL,
   },

   { "map", 0,
   },

   { "portal", 0,
   },

   { "warp_stone", 0,
   },

   { "room_key", 0,
   },

   { "gem", 0,
   },

   { "jewelry", 0,
   },

   { "jukebox", 0,
   },

   { 0,
   }
};

static SlotType far RomSlots[][5] = {
   {
      { "",             -1        },
      { "",             -1        },
      { "hours",        WORD_TYPE },
      { "",             -1        },
      { "",             -1        }
   },

   {
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE }
   },

   {
      { "level",        WORD_TYPE },
      { "max-charges",  WORD_TYPE },
      { "charges",      WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   {
      { "level",        WORD_TYPE },
      { "max-charges",  WORD_TYPE },
      { "charges",      WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   {
      { "weapon-type",  WORD_TYPE },
      { "dice",         WORD_TYPE },
      { "dtype",        WORD_TYPE },
      { "dam-msg",      WORD_TYPE },
      { "weapon-flags", FLAG_TYPE }
   },

   {
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        }
   },

   {
      { "ac vs pierce", WORD_TYPE },
      { "ac vs bash",   WORD_TYPE },
      { "ac vs slash",  WORD_TYPE },
      { "ac vs exotic", WORD_TYPE },
      { "bulk",         WORD_TYPE },
   },

   {
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE }
   },

   {
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        }
   },

   {
      { "people",        WORD_TYPE },
      { "weight",        WORD_TYPE },
      { "furn-flags",    FLAG_TYPE },
      { "heal-bonus",    WORD_TYPE },
      { "mana-bonus",    WORD_TYPE },
   },

   {
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "container", ITEM_CONTAINER,
      { "weight-ttl",    WORD_TYPE },
      { "cont-flags",    FLAG_TYPE },
      { "",              -1        },
      { "weight-single", WORD_TYPE },
      { "weight-mult",   WORD_TYPE },
   },

   { // "drink", ITEM_DRINK_CON,
      { "max-amount",    WORD_TYPE },
      { "curr-amount",   WORD_TYPE },
      { "liquid",        LIST_TYPE },
      { "drink-flags",   FLAG_TYPE },
      { "",              -1        }
   },

   { // "key", ITEM_KEY,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "food", ITEM_FOOD,
      { "hours-full",       WORD_TYPE },
      { "hours-not hungry", WORD_TYPE },
      { "",                 -1        },
      { "poison-flag",      WORD_TYPE },
      { "",                 -1        },
   },

   { // "money", ITEM_MONEY,
      { "silver",        -1        },
      { "gold",          -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "boat", ITEM_BOAT,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "npc_corpse", ITEM_CORPSE_NPC,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "fountain", ITEM_FOUNTAIN,
      { "max-amount",        WORD_TYPE },
      { "curr-amount",        WORD_TYPE },
      { "liquid",        LIST_TYPE },
      { "",              -1 },
      { "",              -1 }
   },

   { // "pill", ITEM_PILL,
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE }
   },

   { // "map", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "portal", 0,
      { "charges",       WORD_TYPE },
      { "exit-flags",    FLAG_TYPE },
      { "gate-flags",    FLAG_TYPE },
      { "vnum",          WORD_TYPE },
      { "",              -1        }
   },

   { // "warp_stone", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "room_key", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "gem", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "jewelry", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "jukebox", 0,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // 0,
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        }
   }
};

static DataType MercDefault[] = {
   { "light", ITEM_LIGHT,
   },

   { "scroll", ITEM_SCROLL,
   },

   { "wand", ITEM_WAND,
   },

   { "staff", ITEM_STAFF,
   },

   { "weapon", ITEM_WEAPON,
   },

   { "treasure", ITEM_TREASURE,
   },

   { "armor", ITEM_ARMOR,
   },

   { "potion", ITEM_POTION,
   },

   { "furniture", ITEM_FURNITURE,
   },

   { "trash", ITEM_TRASH,
   },

   { "container", ITEM_CONTAINER,
   },

   { "drink", ITEM_DRINK_CON,
   },

   { "key", ITEM_KEY,
   },

   { "food", ITEM_FOOD,
   },

   { "money", ITEM_MONEY,
   },

   { "boat", ITEM_BOAT,
   },

   { "npc_corpse", ITEM_CORPSE_NPC,
   },

   { "fountain", ITEM_FOUNTAIN,
   },

   { "pill", ITEM_PILL,
   },

   { 0, 0
   }
};

static SlotType far MercSlots[][5] = {
   { // "light", ITEM_LIGHT,
      { "",             -1        },
      { "",             -1        },
      { "hours",        WORD_TYPE },
      { "",             -1        },
      { "",             -1        }
   },

   { // "scroll", ITEM_SCROLL,
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   { // "wand", ITEM_WAND,
      { "level",        WORD_TYPE },
      { "max-charges",  WORD_TYPE },
      { "charges",      WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   { // "staff", ITEM_STAFF,
      { "level",        WORD_TYPE },
      { "max-charges",  WORD_TYPE },
      { "charges",      WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   { // "weapon", ITEM_WEAPON,
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "weapon-type",  LIST_TYPE },
      { "",             -1        }
   },

   { // "treasure", ITEM_TREASURE,
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        }
   },

   { // "armor", ITEM_ARMOR,
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
   },

   { // "potion", ITEM_POTION,
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        },
   },

   { // "furniture", ITEM_FURNITURE,
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
      { "",             -1        },
   },

   { // "trash", ITEM_TRASH,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "container", ITEM_CONTAINER,
      { "weight-ttl",    WORD_TYPE },
      { "cont-flags",    FLAG_TYPE },
      { "key-vnum",      WORD_TYPE },
      { "",             -1        },
      { "",             -1        },
   },

   { // "drink", ITEM_DRINK_CON,
      { "max-amount",    WORD_TYPE },
      { "curr-amount",   WORD_TYPE },
      { "liquid",        LIST_TYPE },
      { "drink-flags",   FLAG_TYPE },
      { "",              -1        }
   },

   { // "key", ITEM_KEY,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "food", ITEM_FOOD,
      { "hours-full",       WORD_TYPE },
      { "",                 -1        },
      { "",                 -1        },
      { "poison-flag",      WORD_TYPE },
      { "",                 -1        },
   },

   { // "money", ITEM_MONEY,
      { "gold",          -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "boat", ITEM_BOAT,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "npc_corpse", ITEM_CORPSE_NPC,
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        },
      { "",              -1        }
   },

   { // "fountain", ITEM_FOUNTAIN,
      { "",                 -1        },
      { "",                 -1        },
      { "",                 -1        },
      { "",              -1 },
      { "",              -1 }
   },

   { // "pill", ITEM_PILL,
      { "level",        WORD_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "spell",        LIST_TYPE },
      { "",             -1        }
   },

   { // 0, 0
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        },
      { 0,               -1        }
   }
};

void TObjTypeLibrary::DefaultTypes(DataType pDT[], SlotType pSlot[][5])
{
   // Load the types from MZF, as the .ini file has not been created for
   // this codebase
   while (pDT->pszName) {
      TTypeData *ptd = new TTypeData(*pSlot);
      ptd->nMercType = pDT->nMercType;

      TTypeAssoc *pAssoc = new TTypeAssoc(pDT->pszName, ptd);
      m_TypeDict.Add(*pAssoc);

      pDT++;
      pSlot++;
   }
}

// weapon types
#define WEAPON_FLAMING		  1
#define WEAPON_FROST		     2
#define WEAPON_VAMPIRIC		  4
#define WEAPON_SHARP		     8
#define WEAPON_VORPAL		 16
#define WEAPON_TWO_HANDS	 32
#define WEAPON_SHOCKING		 64
#define WEAPON_POISON		128

// gate flags
#define GATE_NORMAL_EXIT	1
#define GATE_NOCURSE		   2
#define GATE_GOWITH		   4
#define GATE_BUGGY		   8
#define GATE_RANDOM		  16

// furniture flags
#define STAND_AT		    1
#define STAND_ON		    2
#define STAND_IN		    4
#define SIT_AT			    8
#define SIT_ON			   16
#define SIT_IN			   32
#define REST_AT		   64
#define REST_ON		  128
#define REST_IN		  256
#define SLEEP_AT		  512
#define SLEEP_ON		 1024
#define SLEEP_IN		 2048
#define PUT_AT			 4096
#define PUT_ON			 8192
#define PUT_IN			16384
#define PUT_INSIDE   32768L

static PromptFlags RomWeaponFlags[] = {
   { WEAPON_FLAMING,	  "flaming" },
   { WEAPON_FROST,	  "frost" },
   { WEAPON_VAMPIRIC,  "vampiric" },
   { WEAPON_SHARP,	  "sharp" },
   { WEAPON_VORPAL,	  "vorpal" },
   { WEAPON_TWO_HANDS, "two-handed" },
   { WEAPON_SHOCKING,  "shocking" },
   { WEAPON_POISON,	  "poisoned" },
   { 0,                0 }
};

static PromptFlags far RomFurnitureFlags[] = {
   { STAND_AT,   "stand at" },
   { STAND_ON,   "stand on" },
   { STAND_IN,   "stand in" },
   { SIT_AT,     "sit at" },
   { SIT_ON,     "sit on" },
   { SIT_IN,     "sit in" },
   { REST_AT,    "rest at" },
   { REST_ON,    "rest on" },
   { REST_IN,    "rest in" },
   { SLEEP_AT,   "sleep at" },
   { SLEEP_ON,   "sleep on" },
   { SLEEP_IN,   "sleep in" },
   { PUT_AT,     "put at" },
   { PUT_ON,     "put on" },
   { PUT_IN,     "put in" },
   { PUT_INSIDE, "put inside" },
   { 0,          0 }
};

static PromptFlags RomContainerFlags[] = {
   { CONT_CLOSEABLE, "closeable" },
   { CONT_PICKPROOF, "pickproof" },
   { CONT_CLOSED,    "closed" },
   { CONT_LOCKED,    "locked" },
   { CONT_PUT_ON,    "put on" },
   { 0,              0 }
};

// Although the Rom 2.4 builder's documentation says that A is the poison
// flag, actually any flag set will be poisoned.
static PromptFlags RomDrinkFlags[] = {
   { 1,              "poisoned" },
   { 0,              0 }
};

static PromptFlags RomGateFlags[] = {
   { GATE_NORMAL_EXIT, "normal exit" },
   { GATE_NOCURSE,     "no cursed" },
   { GATE_GOWITH,      "go with" },
   { GATE_BUGGY,       "buggy" },
   { GATE_RANDOM,      "random" },
   { 0,                0 }
};

static PromptFlags RomExitFlags[] = {
   { EX_ISDOOR,      "door" },
   { EX_CLOSED,      "closed" },
   { EX_LOCKED,      "locked" },
   { EX_PICKPROOF,   "pickproof" },
   { EX_NOPASS,      "'pass door' proof" },
   { EX_EASY,        "easy *" },
   { EX_HARD,        "hard *" },
   { EX_INFURIATING, "infuriating *" },
   { EX_NOCLOSE,     "uncloseable" },
   { EX_NOLOCK,      "unlockable" },
   { 0,              0 }
};

static FlagPromptInfo RomFlags[] = {
   { "weapon-flags", RomWeaponFlags },
   { "furn-flags",   RomFurnitureFlags },
   { "cont-flags",   RomContainerFlags },
   { "drink-flags",  RomDrinkFlags },
   { "exit-flags",   RomExitFlags },
   { "gate-flags",   RomGateFlags },
   { 0,              0 }
};

static struct ListConv far RomAttacks[] = {
    { "hit",      0 },
    { "none",     0 },
    { "slice",    1 },
    { "stab",     2 },
    { "slash",    3 },
    { "whip",     4 },
    { "claw",     5 },
    { "blast",    6 },
    { "pound",    7 },
    { "crush",    8 },
    { "grep",     9 },
    { "bite",    10 },
    { "pierce",  11 },
    { "suction", 12 },
    {	"beating",   0 },
    { "digestion", 0 },
    {	"charge",    0 },
    { "slap",      0 },
    {	"punch",     0 },
    {	"wrath",     0 },
    {	"magic",     6 },
    { "divine",    6 },
    {	"cleave",    1 },
    {	"scratch",   5 },
    { "peck",     10 },
    { "peckb",    10 },
    { "chop",      0 },
    { "sting",     0 },
    { "smash",     7 },
    { "shbite",   10 },
    {	"flbite",   10 },
    {	"frbite",   10 },
    {	"acbite",   10 },
    {	"chomp",    10 },
    { "drain",     0 },
    { "thrust",    0 },
    { "slime",     0 },
    {	"shock",     0 },
    { "thwack",    0 },
    { "flame",     0 },
    { "chill",     0 },
    { 0,           0 },
};

// It is important these are in the proper order, for conversion from Merc
// to ROM.  'Proper' means that if we have a Merc drink of type '2', we
// want it to go to 'red wine' and not 'elvish wine'
static struct ListConv far RomDrinks[] = {
    { "water",             0 },
    { "beer",              1 },
    { "red wine",          2 },
    { "ale",               3 },
    { "dark ale",          4 },

    { "whisky",            5 },
    { "lemonade",          6 },
    { "firebreather",      7 },
    { "local specialty",   8 },
    { "slime mold juice",  9 },

    { "milk",             10 },
    { "tea",              11 },
    { "coffee",           12 },
    { "blood",            13 },
    { "salt water",       14 },

    { "coke",             15 },
    { "root beer",        15 },
    { "elvish wine",       2 },
    { "white wine",        2 },
    { "champagne",         2 },

    { "mead",              1 },
    { "rose wine",         2 },
    { "benedictine wine",  2 },
    { "vodka",             7 },
    { "cranberry juice",   6 },

    { "orange juice",      6 },
    { "absinthe",          7 },
    { "brandy",            5 },
    { "aquavit",           0 },
    { "schnapps",          7 },

    { "icewine",           2 },
    { "amontillado",       2 },
    { "sherry",            2 },
    { "framboise",         2 },
    { "rum",               5 },

    { "cordial",           2 },
    { 0,                     0 }
};

static ListConv far RomSpells[] = {
	{ "acid blast",    70 },
	{ "armor",		      1 },
	{ "bless",		      3 },
	{ "blindness",      4 },
	{ "burning hands",	   5 },
	{ "call lightning",	   6 },
   { "calm",			      509 },
	{ "cancellation",	   507 },
	{ "cause critical",	    63 },
	{ "cause light",		    62 },
	{ "cause serious",	    64 },
	{ "chain lightning",    500 },
	{ "change sex",		     82 },
	{ "charm person",	      7 },
	{ "chill touch",		      8 },
	{ "colour spray",	     10 },
	{ "continual light",     57 },
	{ "control weather",     11 },
	{ "create food",		     12 },
	{ "create rose",		    511 },
	{ "create spring",	     80 },
	{ "create water",	     13 },
	{ "cure blindness",	     14 },
	{ "cure critical",	     15 },
	{ "cure disease",	    501 },
	{ "cure light",		     16 },
	{ "cure poison",		     43 },
	{ "cure serious",	     61 },
	{ "curse",		           17 },
	{ "demonfire",		    505 },
	{ "detect evil",		     18 },
   { "detect good",        513 },
	{ "detect hidden",	     44 },
	{ "detect invis",	     19 },
	{ "detect magic",	     20 },
	{ "detect poison",	     21 },
	{ "dispel evil",		     22 },
   { "dispel good",        512 },
	{ "dispel magic",	     59 },
	{ "earthquake",		     23 },
	{ "enchant armor",	    510 },
	{ "enchant weapon",	     24 },
	{ "energy drain",	     25 },
	{ "faerie fire",		     72 },
	{ "faerie fog",		     73 },
	{ "farsight",		       521 },
	{ "fireball",		        26 },
	{ "fireproof",		    523 },
	{ "flamestrike",		     65 },
	{ "fly",			        56 },
	{ "floating disc",	    522 },
   { "frenzy",             504 },
	{ "gate",			        83 },
	{ "giant strength",	     39 },
	{ "harm",			        27 },
	{ "haste",		          502 },
	{ "heal",			        28 },
	{ "heat metal",		    516 },
	{ "holy word",		    506 },
	{ "identify",		        53 },
	{ "infravision",		     77 },
	{ "invisibility",	     29 },
	{ "know alignment",	     58 },
	{ "lightning bolt",	     30 },
	{ "locate object",	     31 },
	{ "magic missile",	     32 },
	{ "mass healing",	    508 },
	{ "mass invis",		     69 },
   { "nexus",              520 },
	{ "pass door",		     74 },
	{ "plague",		       503 },
	{ "poison",		        33 },
   { "portal",             519 },
	{ "protection evil",     34 },
   { "protection good",    514 },
   { "ray of truth",       518 },
	{ "recharge",		       517 },
	{ "refresh",		        81 },
	{ "remove curse",	        35 },
	{ "sanctuary",		        36 },
	{ "shield",		           67 },
	{ "shocking grasp",	     53 },
	{ "sleep",		           38 },
   { "slow",                515 },
	{ "stone skin",		     66 },
	{ "summon",               40 },
	{ "teleport",		         2 },
	{ "ventriloquate",	     41 },
	{ "weaken",               68 },
	{ "word of recall",	     42 },

	{ "acid breath",		    200 },
	{ "fire breath",		    201 },
	{ "frost breath",        202 },
	{ "gas breath",          203 },
	{ "lightning breath",    204 },

   { "general purpose",     205 },
   { "high explosive",      206 },

   // MZF needs a null spell because it is going to check
   // that every spell is in its database and not all potions
   // for example have all four spells in use.
   { "",                    0 },

   { 0,                     0 }
};

static struct ListItemInfo RomLists[] = {
   { "dam-msg", RomAttacks },
   { "spell",   RomSpells  },
   { "liquid",  RomDrinks  },
   { 0,         0          }
};

static struct ListConv MercAttacks[] = {
	{ "hit",      0 },
	{ "slice",    1 },
   { "stab",     2 },
   { "slash",    3 },
   { "whip",     4 },
   { "claw",     5 },
	{ "blast",    6 },
   { "pound",    7 },
   { "crush",    8 },
   { "grep",     9 },
   { "bite",    10 },
	{ "pierce",  11 },
   { "suction", 12 },
   { 0,          0 }
};

static struct ListConv far MercDrinks[] = {
    { "water",             0 },
    { "beer",              1 },
    { "red wine",          2 },
    { "ale",               3 },
    { "dark ale",          4 },

    { "whisky",            5 },
    { "lemonade",          6 },
    { "firebreather",      7 },
    { "local specialty",   8 },
    { "slime mold juice",  9 },

    { "milk",             10 },
    { "tea",              11 },
    { "coffee",           12 },
    { "blood",            13 },
    { "salt water",       14 },

    { "coke",             15 },

    { 0,                   0 }
};

static ListConv far MercSpells[] = {
	{ "acid blast",    70 },
	{ "armor",		      1 },
	{ "bless",		      3 },
	{ "blindness",      4 },
	{ "burning hands",	   5 },
	{ "call lightning",	   6 },
	{ "cause critical",	    63 },
	{ "cause light",		    62 },
	{ "cause serious",	    64 },
	{ "change sex",		     82 },
	{ "charm person",	      7 },
	{ "chill touch",		      8 },
	{ "colour spray",	     10 },
	{ "continual light",     57 },
	{ "control weather",     11 },
	{ "create food",		     12 },
	{ "create rose",		    511 },
	{ "create spring",	     80 },
	{ "create water",	     13 },
	{ "cure blindness",	     14 },
	{ "cure critical",	     15 },
	{ "cure disease",	    501 },
	{ "cure light",		     16 },
	{ "cure poison",		     43 },
	{ "cure serious",	     61 },
	{ "curse",		           17 },
	{ "detect evil",		     18 },
	{ "detect hidden",	     44 },
	{ "detect invis",	     19 },
	{ "detect magic",	     20 },
	{ "detect poison",	     21 },
	{ "dispel evil",		     22 },
	{ "dispel magic",	     59 },
	{ "earthquake",		     23 },
	{ "enchant weapon",	     24 },
	{ "energy drain",	     25 },
	{ "faerie fire",		     72 },
	{ "faerie fog",		     73 },
	{ "fireball",		        26 },
	{ "flamestrike",		     65 },
	{ "fly",			        56 },
	{ "gate",			        83 },
	{ "giant strength",	     39 },
	{ "harm",			        27 },
	{ "heal",			        28 },
	{ "heat metal",		    516 },
	{ "holy word",		    506 },
	{ "identify",		        53 },
	{ "infravision",		     77 },
	{ "invisibility",	     29 },
	{ "know alignment",	     58 },
	{ "lightning bolt",	     30 },
	{ "locate object",	     31 },
	{ "magic missile",	     32 },
	{ "mass invis",		     69 },
	{ "pass door",		     74 },
	{ "poison",		        33 },
	{ "protection evil",     34 },
	{ "refresh",		        81 },
	{ "remove curse",	        35 },
	{ "sanctuary",		        36 },
	{ "shield",		           67 },
	{ "shocking grasp",	     53 },
	{ "sleep",		           38 },
	{ "stone skin",		     66 },
	{ "summon",               40 },
	{ "teleport",		         2 },
	{ "ventriloquate",	     41 },
	{ "weaken",               68 },
	{ "word of recall",	     42 },

	{ "acid breath",		    200 },
	{ "fire breath",		    201 },
	{ "frost breath",        202 },
	{ "gas breath",          203 },
	{ "lightning breath",    204 },

   { "general purpose",     205 },
   { "high explosive",      206 },

   // MZF needs a 'null' spell because it is going to check
   // that every spell is in its database and not all potions
   // for example have all four spells in use.
   { "''",                    0 },

   { 0,                       0 }
};

static struct ListItemInfo MercLists[] = {
   { "weapon-type", MercAttacks },
   { "spell",  MercSpells },
   { "liquid", MercDrinks },
   { 0,        0         }
};

static PromptFlags MercContainerFlags[] = {
   { CONT_CLOSEABLE, "closeable" },
   { CONT_PICKPROOF, "pickproof" },
   { CONT_CLOSED,    "closed" },
   { CONT_LOCKED,    "locked" },
   { 0,              0 }
};

static FlagPromptInfo MercFlags[] = {
   { "cont-flags",   MercContainerFlags },
   { "drink-flags",  RomDrinkFlags },
   { 0,              0 }
};

static struct ListConv EnvyAttacks[] = {
	{ "hit",      0 },
	{ "slice",    1 },
   { "stab",     2 },
   { "slash",    3 },
   { "whip",     4 },
   { "claw",     5 },
	{ "blast",    6 },
   { "pound",    7 },
   { "crush",    8 },
   { "grep",     9 },
   { "bite",    10 },
	{ "pierce",  11 },
   { "suction", 12 },
	{ "chop",    13 },
   { 0,          0 }
};

static ListConv far EnvySpells[] = {
	{ "acid blast",    70 },
	{ "armor",		      1 },
	{ "bless",		      3 },
	{ "blindness",      4 },
	{ "burning hands",	   5 },
	{ "call lightning",	   6 },
	{ "cause critical",	    63 },
	{ "cause light",		    62 },
	{ "cause serious",	    64 },
	{ "change sex",		     82 },
	{ "charm person",	      7 },
	{ "chill touch",		      8 },
	{ "colour spray",	     10 },
	{ "continual light",     57 },
	{ "control weather",     11 },
	{ "create food",		     12 },
	{ "create spring",	     80 },
	{ "create water",	     13 },
	{ "cure blindness",	     14 },
	{ "cure critical",	     15 },
	{ "cure disease",	    501 },
	{ "cure light",		     16 },
	{ "cure poison",		     43 },
	{ "cure serious",	     61 },
	{ "curse",		           17 },
	{ "detect evil",		     18 },
	{ "detect hidden",	     44 },
	{ "detect invis",	     19 },
	{ "detect magic",	     20 },
	{ "detect poison",	     21 },
	{ "dispel evil",		     22 },
	{ "dispel magic",	     59 },
	{ "earthquake",		     23 },
	{ "enchant weapon",	     24 },
	{ "energy drain",	     25 },
	{ "faerie fire",		     72 },
	{ "faerie fog",		     73 },
	{ "fireball",		        26 },
	{ "flamestrike",		     65 },
	{ "fly",			        56 },
	{ "gate",			        83 },
	{ "giant strength",	     39 },
	{ "harm",			        27 },
	{ "heal",			        28 },
	{ "heat metal",		    516 },
	{ "holy word",		    506 },
	{ "identify",		        53 },
	{ "infravision",		     77 },
	{ "invisibility",	     29 },
	{ "know alignment",	     58 },
	{ "lightning bolt",	     30 },
	{ "locate object",	     31 },
	{ "magic missile",	     32 },
	{ "mass invis",		     69 },
	{ "pass door",		     74 },
	{ "poison",		        33 },
	{ "protection evil",     34 },
	{ "refresh",		        81 },
	{ "remove curse",	        35 },
	{ "sanctuary",		        36 },
	{ "shield",		           67 },
	{ "shocking grasp",	     53 },
	{ "sleep",		           38 },
	{ "stone skin",		     66 },
	{ "summon",               40 },
	{ "teleport",		         2 },
	{ "ventriloquate",	     41 },
	{ "weaken",               68 },
	{ "word of recall",	     42 },

	{ "acid breath",		    200 },
	{ "fire breath",		    201 },
	{ "frost breath",        202 },
	{ "gas breath",          203 },
	{ "lightning breath",    204 },

   { "general purpose",     205 },
   { "high explosive",      206 },

   // Envy 2? spells I missed before
   { "breathe water",       400 },
   { "cone of silence",     401 },
	{ "destroy cursed",      402 },
	{ "exorcise",            403 },
	{ "flaming shield",      404 },
	{ "mass heal",           405 },
   { "mute",                406 },
	{ "polymorph other",     407 },
   { "recharge item",       408 },
   { "remove alignment",    409 },
   { "remove silence",      410 },
	{ "turn undead",         411 },

   // Psionicist spells
   {    "adrenaline control",   300 },
   {    "agitation",            301 },
   {    "aura sight",           302 },
   {    "awe",                  303 },
   {    "ballistic attack",     304 },
   {    "biofeedback",          305 },
   {    "cell adjustment",      306 },
   {    "chameleon power",      307 },
   {    "combat mind",          308 },
   {    "complete healing",     309 },
   {    "control flames",       310 },
   {    "create sound",         311 },
   {    "death field",          312 },
   {    "detonate",             313 },
   {    "disintegrate",         314 },
   {    "displacement",         315 },
   {    "domination",           316 },
   {    "ectoplasmic form",     317 },
   {    "ego whip",             318 },
   {    "energy containment",   319 },
   {    "enhance armor",        320 },
   {    "enhanced strength",    321 },
   {    "flesh armor",          322 },
   {    "heighten senses",      323 },
   {    "inertial barrier",     324 },
   {    "inflict pain",         325 },
   {    "intellect fortress",   326 },
   {    "lend health",          327 },
   {    "levitation",           328 },
   {    "mental barrier",       329 },
   {    "mind thrust",          330 },
   {    "project force",        331 },
   {    "psionic blast",        332 },
   {    "psychic crush",        333 },
   {    "psychic drain",        334 },
   {    "psychic healing",      335 },
   {    "shadow form",          336 },
   {    "share strength",       337 },
   {    "thought shield",       338 },
   {    "ultrablast",           339 },

   // MZF needs a null spell because it is going to check
   // that every spell is in its database and not all potions
   // for example have all four spells in use.
   { "",                    0 },

   { 0,                       0 }
};

static struct ListItemInfo EnvyLists[] = {
   { "weapon-type", EnvyAttacks },
   { "spell",  EnvySpells },
   { "liquid", MercDrinks },
   { 0,        0         }
};

void TObjTypeLibrary::DefaultFlags (FlagPromptInfo *pFPI)
{
   while (pFPI->pszPrompt) {
      TFlagSet& set = *new TFlagSet;
      PromptFlags *pPF = pFPI->pPF;
      while (pPF->flag) {
         set.Add(*new TMudFlag(pPF->flag, *new string(pPF->pszMeaning)));
         pPF++;
      }

      m_FlagDict.Add(*new TFlagAssoc(*new string(pFPI->pszPrompt), set));
      pFPI++;
   }
}

void TObjTypeLibrary::DefaultLists (ListItemInfo *pLII)
{
   while (pLII->pszItem) {
      TStringSet& set = *new TStringSet;
//      char **ppsz = pLII->pLI;
      ListConv *ppsz = pLII->pLI;
      while (ppsz->pszName) {
//         set.Add(*new string(*ppsz));
         set.Add(*new TListItem(ppsz->nMercValue, ppsz->pszName));
         ppsz++;
      }

      m_ListDict.Add(*new TStringAssoc(*new string(pLII->pszItem), set));
      pLII++;
   }
}

void TObjTypeLibrary::CreateLibraryFromTemplate()
{
   switch (CurrentMode) {
   case poundsApp::MercMode:
   case poundsApp::TFCMode:
   default:
      DefaultTypes(MercDefault, MercSlots);
      DefaultLists(MercLists);
      DefaultFlags(MercFlags);
      break;

   case poundsApp::EnvyMode:
   case poundsApp::Envy2Mode:
      DefaultTypes(MercDefault, MercSlots);
      DefaultLists(EnvyLists);
      DefaultFlags(MercFlags);
      break;

   case poundsApp::RomMode:
   case poundsApp::Rom24Mode:
      DefaultTypes(RomDefault, RomSlots);
      DefaultLists(RomLists);
      DefaultFlags(RomFlags);
      break;
   }

   m_fDirty = TRUE;
}


