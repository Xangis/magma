#ifndef _MOBHERE_H_

#include "../zone/zone.h"
#include "mob.h"
#include "../obj/objhere.h"

#define EQ_WEARABLE      0  // eq is wearable, enjoy
#define EQ_SLOT_FILLED 127  // slot is already filled
#define EQ_RESALIGN    126  // can't wear because of align
#define EQ_RESCLASS    125  // can't wear because of class
#define EQ_NO_WEARBITS 124  // obj has no wear bits
#define EQ_ERROR       123  // mob pointer is null or some other silliness
#define EQ_NOHANDS     122  // mob can use it, but has no hands free
#define EQ_WEAR_NOTSET 121  // "where" location specified, but appropriate
                            // wear bit not set
#define EQ_WRONGRACE   120  // wrong race - human trying to wear a
                            // horseshoe, whatever
#define EQ_RACECANTUSE 119  // thri-kreen can't use earrings etc etc
#define EQ_NOBELTATTCH 118  // trying to attach a belt thingy but no belt
#define EQ_RESRACE     117  // restricted by race (anti/anti2 bits)
#define EQ_RESSEX      116  // restricted by sex (anti2 bits)

// "mob here" record - used for mobs in a room

typedef struct _mobHere
{
  ulong mobNumb;       // number of mob
  dikuMob *mobPtr;     // points to address of mob type

  objectHere *objectHead;  // objects mob is carrying
  objectHere *equipment[WEAR_TRUEHIGH + 1];

  ulong randomChance;

//  char tableMob;     // if TRUE, mobNumb is number of table

  struct _mobHere *riding;     // mob this mob is riding, if any
  struct _mobHere *riddenBy;   // mob this mob is ridden by, if any

  struct _mobHere *following;  // mob this mob is following, if any

  struct _mobHere *Next;
} mobHere;


#define _MOBHERE_H_
#endif
