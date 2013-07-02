// ROOMBITS.H - stuff related to the room bits in general

#ifndef _ROOMBITS_H_

#include "../types.h"

// room bit flag defines

#define NUMB_ROOMBIT_FLAGS      32  // pretty self-explanatory

#define ROOM_DARK                1 // Light must be used to see anything.
#define ROOM_LIMIT_SCAN          2 // scan range is reduced to 1
#define ROOM_NO_MOB              4 // Wandering monsters will not walk into this room.
#define ROOM_INDOORS             8 // This is inside (a house,cave or dungeon for example)
#define ROOM_SILENT             16 // No speaking in this room.
#define ROOM_UNDERWATER         32 // Underwater room, need water breathing
#define ROOM_NO_RECALL          64 // Cannot recall from here
#define ROOM_NO_MAGIC          128 // No magical spells will work here.
#define ROOM_TUNNEL            256 // Narrow Tunnel
#define ROOM_PRIVATE           512 // Only two people allowed in here, not recommended.
#define ROOM_ARENA            1024 // Flag for arena rooms ONLY.
#define ROOM_SAFE             2048 // No steal, attacks permitted in room. Use RARELY.
#define ROOM_NO_PRECIP        4096 // If it's outdoors, but there's some sort of covering.
#define ROOM_SINGLE_FILE      8192 // Where people cant go around each other in a room.
#define ROOM_JAIL            16384 // This is a jail cell, used only in hometowns!
#define ROOM_NO_TELEPORT     32768 // A random teleport spell will ignore these rooms.
#define ROOM_PRIV_ZONE       65536 // Do not use.
#define ROOM_HEAL           131072 // Regeneration of hits/mana is much faster here.
#define ROOM_NO_HEAL        262144 // Inhibits good healing/regeneration here.
#define ROOM_LARGE          524288 // Used to be for "trap" - never used
#define ROOM_DOCKABLE      1048576 // Room is used by ocean going ships to load and unload.
#define ROOM_MAGIC_DARK    2097152 // Room has a magic darkness cast on it.
#define ROOM_MAGIC_LIGHT   4194304 // Room has a magic light spell cast on it.
#define ROOM_NO_SUMMON     8388608 // Cannot be summoned to or from or summon to or from.
#define ROOM_GUILD_ROOM   16777216 // Guild room
#define ROOM_TWILIGHT     33554432 // Twilight
#define ROOM_READD_DARK  (1 << 26)
#define ROOM_NO_GATE     (1 << 27)
#define ROOM_HOUSE       (1 << 28)
#define ROOM_ATRIUM      (1 << 29)
#define ROOM_BLOCKS_SIGHT (1 << 30)
#define ROOM_BFS_MARK    (1 << 31)


// room bit flag structure and union

typedef struct _roomBitFlags
{
  unsigned dark       : 1;
  unsigned limit_scan : 1;
  unsigned noMob      : 1;
  unsigned indoors    : 1;
  unsigned silent     : 1;
  unsigned underwater : 1;
  unsigned noRecall   : 1;
  unsigned noMagic    : 1;
  unsigned tunnel     : 1;
  unsigned privateRm  : 1;
  unsigned arena      : 1;
  unsigned safe       : 1;
  unsigned noPrecip   : 1;
  unsigned singleFile : 1;
  unsigned jail       : 1;
  unsigned noTeleport : 1;
  unsigned privZone   : 1;
  unsigned heal       : 1;
  unsigned noHeal     : 1;
  unsigned unused     : 1;
  unsigned dockable   : 1;
  unsigned magicDark  : 1;
  unsigned magicLight : 1;
  unsigned noSummon   : 1;
  unsigned guildRoom  : 1;
  unsigned twilight   : 1;
  unsigned readdDark  : 1;
  unsigned noGate     : 1;
  unsigned house      : 1;
  unsigned atrium     : 1;
  unsigned blocksSight: 1;
  unsigned bfsMark    : 1;
} roomBitFlags;

typedef union _roomFlagRec
{
  ulong longIntFlags;

  roomBitFlags roomBits;
} roomFlagRec;


#define _ROOMBITS_H_
#endif
