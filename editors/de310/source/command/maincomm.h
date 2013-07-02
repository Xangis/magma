// MAINCOMM.H - whee

#ifndef _MAINCOMM_H_

#define COMM_STAT          1   // 0 may be reserved, someday
#define COMM_EXITS         2
#define COMM_LOOKUP        3
#define COMM_WHERE         4
#define COMM_LOOK          5
#define COMM_WHICH         6
#define COMM_ZONEEXITS     7
#define COMM_MASSSET       8

#define COMM_WEST         20
#define COMM_EAST         21
#define COMM_NORTH        22
#define COMM_SOUTH        23
#define COMM_UP           24
#define COMM_DOWN         25
#define COMM_NORTHWEST    26
#define COMM_SOUTHWEST    27
#define COMM_NORTHEAST    28
#define COMM_SOUTHEAST    29

#define COMM_GOTO         40
#define COMM_EDIT         41
#define COMM_CREATEEDIT   42
#define COMM_AT           43

#define COMM_PURGE        60
#define COMM_CREATE       61
#define COMM_LOAD         62
#define COMM_DELETE       63

#define COMM_SETRAND      79
#define COMM_INVENTORY    80
#define COMM_GET          81
#define COMM_GETC         82
#define COMM_DROP         83
#define COMM_DROPC        84
#define COMM_PUT          85
#define COMM_PUTC         86
#define COMM_EQUIP        87
#define COMM_UNEQUIP      88
#define COMM_MOUNT        89
#define COMM_UNMOUNT      90
#define COMM_FOLLOW       91
#define COMM_UNFOLLOW     92

#define COMM_SAVE        100
#define COMM_QUIT        101

#define COMM_SHELL       120
#define COMM_FREESTACK   121
#define COMM_CONFIG      122

#define COMM_SWAPEXITS   140
#define COMM_SWAPEXITSNS 141
#define COMM_SWAPEXITSWE 142
#define COMM_SWAPEXITSUD 143
#define COMM_RENUMBER    144
#define COMM_RENUMBROOM  145
#define COMM_RENUMBOBJECT 146
#define COMM_RENUMBMOB   147
#define COMM_SETZONENUMB 148
#define COMM_FIXFLAGS    149
#define COMM_KEY         150
#define COMM_FIXCOND     151
#define COMM_GUILD       152
#define COMM_SCREENHEIGHT 153
#define COMM_SCREENWIDTH 154  // woo!
#define COMM_FIXGUILD    155

#define COMM_RECSIZE     160
#define COMM_LONGHELP    161
#define COMM_SHORTHELP   162
#define COMM_VERSION     163
#define COMM_MISCINFO    164

#define COMM_LIST        180
#define COMM_COMMANDS    181

#define COMM_ALIAS       200
#define COMM_UNALIAS     201
#define COMM_SETVAR      202
#define COMM_UNSETVAR    203
#define COMM_TOGGLE      204
#define COMM_LIMIT       205

#define COMM_CLONE       220
#define COMM_DUMPTEXT    221
#define COMM_DUMPTEXTCOL 222
#define COMM_EDITDISPLAY 223
#define COMM_EDITCHECK   224
#define COMM_SETTEMPLATE 225
#define COMM_COPY        226

#define COMM_GRID        240
#define COMM_CHECK       241
#define COMM_LINKROOMS   242
#define COMM_CHECKMAP    243

#define _MAINCOMM_H_
#endif
