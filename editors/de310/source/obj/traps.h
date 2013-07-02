#ifndef _TRAPS_H_

#define TRAP_DAM_LOWEST         0
#define TRAP_DAM_SLEEP          0
#define TRAP_DAM_TELEPORT       1
#define TRAP_DAM_FIRE           2
#define TRAP_DAM_COLD           3
#define TRAP_DAM_ACID           4
#define TRAP_DAM_ENERGY         5
#define TRAP_DAM_BLUNT          6
#define TRAP_DAM_PIERCE         7
#define TRAP_DAM_SLASH          8
#define TRAP_DAM_DISPEL         9
#define TRAP_DAM_GATE           10
#define TRAP_DAM_SUMMON         11
#define TRAP_DAM_WITHER         12
#define TRAP_DAM_HARM           13
#define TRAP_DAM_POISON         14
#define TRAP_DAM_PARALYSIS      15
#define TRAP_DAM_STUN           16
#define TRAP_DAM_DISEASE        17
#define TRAP_DAM_ROCKFALL       18
#define TRAP_DAM_GAS            19
#define TRAP_DAM_HOLY           20
#define TRAP_DAM_UNHOLY         21
#define TRAP_DAM_SUFFOCATE      22
#define TRAP_DAM_DISINTEGRATION 23
#define TRAP_DAM_CRUSHING       24
#define TRAP_DAM_HIGHEST        24

#define NUMB_TRAP_EFF_FLAGS  20

#define TRAP_EFF_MOVE         1 /* trigger on movement */
#define TRAP_EFF_OBJECT       2 /* trigger on get or put */
#define TRAP_EFF_ROOM         4 /* affect all in room */
#define TRAP_EFF_NORTH        8 /* movement in this direction */
#define TRAP_EFF_EAST        16
#define TRAP_EFF_SOUTH       32
#define TRAP_EFF_WEST        64
#define TRAP_EFF_UP         128
#define TRAP_EFF_DOWN       256
#define TRAP_EFF_OPEN       512 /* trigger on open */
#define TRAP_EFF_MULTI     1024 
#define TRAP_EFF_GLYPH     2048
#define TRAP_EFF_WEAR      4096
#define TRAP_EFF_UNEQUIP   8192
#define TRAP_EFF_STEAL    16384
#define TRAP_EFF_ENCHANT  32768
#define TRAP_EFF_CURSE    65536
#define TRAP_THROW       131072
#define TRAP_EATDRINK    262144
#define TRAP_DRAG        524288

#define _TRAPS_H_
#endif
