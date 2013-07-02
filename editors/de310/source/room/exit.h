// EXIT.H - header for EXIT.C

#ifndef _EXIT_H_

// character values as read from the world files - only used in room/readwld.c,
// probably

#define EXIT_FIRST_CH      '0'
#define EXIT_NORTH_CH      '0'
#define EXIT_EAST_CH       '1'
#define EXIT_SOUTH_CH      '2'
#define EXIT_WEST_CH       '3'
#define EXIT_UP_CH         '4'
#define EXIT_DOWN_CH       '5'
#define EXIT_NORTHWEST_CH  '6'
#define EXIT_SOUTHWEST_CH  '7'
#define EXIT_NORTHEAST_CH  '8'
#define EXIT_SOUTHEAST_CH  '9'
#define EXIT_LAST_CH       '9'

// numeric values of exits based on direction - no longer used, outmoded
// by values below

/*
#define EXIT_NORTH_N        0
#define EXIT_EAST_N         1
#define EXIT_SOUTH_N        2
#define EXIT_WEST_N         3
#define EXIT_UP_N           4
#define EXIT_DOWN_N         5
#define EXIT_NORTHWEST_N    6
#define EXIT_SOUTHWEST_N    7
#define EXIT_NORTHEAST_N    8
#define EXIT_SOUTHEAST_N    9
*/

#define NO_EXIT            -1  // returned by various exit-related functions
                               // as general "no exit/no match found" indicator
#define USER_CHOICE        -2  // used to signify that the user should be
                               // be prompted for exit direction

// values of exits as referenced in room exits array and in .wld files

#define EXIT_LOWEST         0
#define NORTH               0
#define EAST                1
#define SOUTH               2
#define WEST                3
#define UP                  4
#define DOWN                5
#define NORTHWEST           6
#define SOUTHWEST           7
#define NORTHEAST           8
#define SOUTHEAST           9
#define EXIT_HIGHEST        9

#define NUMB_EXITS         10


#define MAX_EXITKEY_LEN  (usint)256  // used in room/readwld.c

typedef struct _roomExit
{
  stringNode *exitDescHead;
                    // exit description

  stringNode *keywordListHead;
                    // instead of one string that contains all the keywords,
                    // this list contains all the keywords parsed, so that
                    // each node has a keyword

  uchar worldDoorType;  // exit "type" read from .wld
  uchar zoneDoorState;  // exit "state" read from .zon

  long keyNumb;     // key object numb required to open door

  long destRoom;   // destination room
} roomExit;

// values used for bitvector returned by getExitAvailFlags() and used to
// determine which exits a room already has occupied

#define EXIT_NORTH_FLAG          1
#define EXIT_SOUTH_FLAG          2
#define EXIT_WEST_FLAG           4
#define EXIT_EAST_FLAG           8
#define EXIT_UP_FLAG            16
#define EXIT_DOWN_FLAG          32
#define EXIT_NORTHWEST_FLAG     64
#define EXIT_SOUTHWEST_FLAG    128
#define EXIT_NORTHEAST_FLAG    256
#define EXIT_SOUTHEAST_FLAG    512

#define EXIT_ALL_EXITS_FLAG (EXIT_NORTH_FLAG | EXIT_SOUTH_FLAG | EXIT_WEST_FLAG | EXIT_EAST_FLAG | \
         EXIT_UP_FLAG | EXIT_DOWN_FLAG | EXIT_NORTHWEST_FLAG | EXIT_SOUTHWEST_FLAG | \
         EXIT_NORTHEAST_FLAG | EXIT_SOUTHEAST_FLAG)
//#define EXIT_NONE_FLAG   2047


#define _EXIT_H_
#endif
