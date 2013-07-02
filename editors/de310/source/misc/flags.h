#ifndef _FLAGS_H_

//#define NO_COLUMN       127  // used for updateFlags() in display.c

// used all over da place

#define ROOM_FLAGS    1
#define OBJ_FLAGS     2
#define MOB_FLAGS     4
#define ALL_FLAGS     (ROOM_FLAGS | OBJ_FLAGS | MOB_FLAGS)

#define EXTR_FL      1
#define WEAR_FL      2
#define AFF1_FL      3
#define AFF2_FL      4
#define AFF3_FL      5
#define AFF4_FL      6
#define MACT_FL      7
#define EXT2_FL      8
#define ANTI_FL      9
#define ANT2_FL     10

#define _FLAGS_H_
#endif
