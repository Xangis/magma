// ZONE.H - useful stuff for zones

#ifndef _ZONE_H_

#include "../types.h"

#define NUMB_ZONEMISC_FLAGS   4

#define ZONE_SILENT           BIT_1  // entire zone is silent (possibly works)
#define ZONE_SAFE             BIT_2  // entire zone is safe (probably works)
#define ZONE_HOMETOWN         BIT_3  // zone is a hometown
#define ZONE_NO_TELEPORT      BIT_4  // can't teleport around the zone

// zone misc flags

typedef struct _zoneMiscBitFlags
{
  unsigned silent      : 1;
  unsigned safe        : 1;
  unsigned hometown    : 1;
  unsigned noteleport  : 1;

  unsigned slack       : 28;
} zoneMiscBitFlags;

typedef union _zoneMiscFlagRec
{
  ulong longIntFlags;

  zoneMiscBitFlags zoneMiscBits;
} zoneMiscFlagRec;


// zone reset values

#define ZONE_RESET_LOWEST   0
#define ZONE_NO_RESET       0
#define ZONE_RESET_EMPTY    1
#define ZONE_RESET_ALWAYS   2
#define ZONE_RESET_HIGHEST  2

// mob equipment areas

//#define WEAR_NOTWORN          -1
#define WEAR_LOW               0
#define WEAR_LIGHT             0  // not imped (obsolete)
#define WEAR_FINGER_R          1
#define WEAR_FINGER_L          2
#define WEAR_NECK_1            3
#define WEAR_NECK_2            4
#define WEAR_BODY              5
#define WEAR_HEAD              6
#define WEAR_LEGS              7
#define WEAR_FEET              8
#define WEAR_HANDS             9
#define WEAR_ARMS              10
#define WEAR_SHIELD            11
#define WEAR_ABOUT             12
#define WEAR_WAIST             13
#define WEAR_WRIST_R           14
#define WEAR_WRIST_L           15
#define WIELD_PRIMARY          16
#define WIELD_SECOND           17
#define HOLD                   18
#define WEAR_EYES              19
#define WEAR_FACE              20
#define WEAR_EARRING_R         21
#define WEAR_EARRING_L         22
#define WEAR_QUIVER            23
#define WEAR_BADGE             24
#define WIELD_THIRD            25
#define WIELD_FOURTH           26
#define WEAR_BACK              27
#define WEAR_ATTACH_BELT_1     28
#define WEAR_ATTACH_BELT_2     29
#define WEAR_ATTACH_BELT_3     30
#define WEAR_ARMS_2            31
#define WEAR_HANDS_2           32
#define WEAR_WRIST_LR          33
#define WEAR_WRIST_LL          34
#define WEAR_HORSE_BODY        35
#define WEAR_LEGS_REAR         36  /* never used */
#define WEAR_TAIL              37
#define WEAR_FEET_REAR         38  /* never actually used in mud */
#define WEAR_NOSE              39
#define WEAR_HORN              40
#define WEAR_HIGH              40

#define WEAR_WHOLEBODY  WEAR_HIGH + 1  // returned by checkMobEquipSlot() -
#define WEAR_WHOLEHEAD  WEAR_HIGH + 2  // used as indices in eq array just like
#define WIELD_TWOHANDS  WEAR_HIGH + 3  // the rest
#define WIELD_TWOHANDS2 WEAR_HIGH + 4 /* for thrikreen */
#define HOLD2           WEAR_HIGH + 5
#define WEAR_TRUEHIGH   HOLD2


// record etc for zones themselves

#define MAX_ZONENAME_LEN    (usint)81
#define MAX_ZONESHORT_LEN   (usint)41

typedef struct _dikuZone
{
  ulong zoneNumber;  // zone number

  char zoneName[MAX_ZONENAME_LEN];  // name of zone

  ulong topRoomNumb;

  ulong lifeLow;     // replaces lifeSpan - number is randomly chosen between
  ulong lifeHigh;    // these two limits at bootup (per reset?)

  uchar resetMode;
  zoneMiscFlagRec miscBits;
} dikuZone;

#define _ZONE_H_
#endif
