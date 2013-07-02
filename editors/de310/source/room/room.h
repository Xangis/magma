// ROOM.H - various stuff related to rooms


#ifndef _ROOM_H_

#include "roombits.h"
#include "exit.h"

#include "../obj/object.h"
#include "../obj/objhere.h"

#include "../mob/mob.h"
#include "../mob/mobhere.h"

#include "../misc/master.h"
#include "../misc/editable.h"

// For room compare functions

#define RC_NO_MATCH               0   // no match
#define RC_MATCH_NOT_LL           1   // match, but not in linked lists
#define RC_MATCH_LL               2   // match in linked lists (and perhaps in
                                      // the "main" record as well)

// Room mana crap

#define MANA_LOWEST     0
#define APPLY_MANA_ALL  0
#define MANA_GOOD       1
#define MANA_NEUTRAL    2
#define MANA_EVIL       3
#define MANA_HIGHEST    3

// new sector types

#define SECT_LOWEST              0
#define SECT_INSIDE              0 // Uses as if walking indoors
#define SECT_CITY                1 // Uses as if walking in a city
#define SECT_FIELD               2 // Uses as if walking in a field
#define SECT_FOREST              3 // Uses as if walking in a forest
#define SECT_HILLS               4 // Uses as if walking in hills
#define SECT_MOUNTAIN            5 // Uses as if climbing in mountains
#define SECT_WATER_SWIM          6 // Uses as if swimming
#define SECT_WATER_NOSWIM        7 // Impossible to swim water - requires a boat
#define SECT_NO_GROUND           8 // (must be flying/levitating)
#define SECT_UNDERWATER          9 // Need water breathing here
#define SECT_UNDERWATER_GR      10 // Underwater, on the bottom.  Can bash
#define SECT_PLANE_OF_FIRE      11 // Used in the Elemental Plane of Fire
#define SECT_OCEAN              12 // Use if in the ocean or vast seas only!
#define SECT_UNDRWLD_WILD       13 // [underworld flags: intended to be used for
#define SECT_UNDRWLD_CITY       14 //  underground settings; drow, duergars, mind
#define SECT_UNDRWLD_INSIDE     15 //  flayers, etc -- details to be fleshed
#define SECT_UNDRWLD_WATER      16 //  out later]
#define SECT_UNDRWLD_NOSWIM     17
#define SECT_UNDRWLD_NOGROUND   18
#define SECT_PLANE_OF_AIR       19 // Used in the Elemental Plane of Air
#define SECT_PLANE_OF_WATER     20 // Used in the Elemental Plane of Water
#define SECT_PLANE_OF_EARTH     21 // Used in the Elemental Plane of Earth
#define SECT_ETHEREAL           22
#define SECT_ASTRAL             23
#define SECT_DESERT             24
#define SECT_ARCTIC             25
#define SECT_SWAMP              26
#define SECT_UNDRWLD_MOUNTAIN   27
#define SECT_UNDRWLD_SLIME      28
#define SECT_UNDRWLD_LOWCEIL    29
#define SECT_UNDRWLD_LIQMITH    30
#define SECT_UNDRWLD_MUSHROOM   31
#define SECT_CASTLE_WALL        32
#define SECT_CASTLE_GATE        33
#define SECT_CASTLE             34
#define SECT_HIGHEST            34
#define NUMB_SECT_TYPES         35

// room info

#define MAX_ROOMNAME_LEN  (usint)128
#define MAX_ROOMSHORT_LEN (usint)39
#define MAX_ROOMSHORTSHORT_LEN (usint)30
                              // names are shortened to this if necessary when
                              // printing to screen ...

typedef struct _dikuRoom
{
  ulong roomNumber;  // room number - woah

  char roomName[MAX_ROOMNAME_LEN];  // name of room
  stringNode *roomDescHead;

  ulong zoneNumber;
  roomFlagRec roomFlags;
  ulong sectorType;
  ulong resourceInfo;

  int fallChance; // fall percentage

  int current;    // current
  int currentDir; // current direction

  ulong manaFlag;   // mana stuff
  long manaApply;

  roomExit *exits[NUMB_EXITS];

  extraDesc *extraDescHead;

  objectHere *objectHead;
  mobHere *mobHead;

  masterKeywordListNode *masterListHead;
  editableListNode *editableListHead;

  char defaultRoom;

  struct _dikuRoom *Last;
  struct _dikuRoom *Next;
} dikuRoom;


#define _ROOM_H_
#endif
