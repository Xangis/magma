// SYSTEM.H - system-dependent stuff

#ifndef _SYSTEM_H_


// name of "main" filename for default objs etc (no extension)

#define MAIN_DEFAULT_NAME  "de"
#define DEF_WLD_EXT        ".dwd"  // extension of def world
#define DEF_OBJ_EXT        ".dob"  // ext of def obj
#define DEF_MOB_EXT        ".dmb"  // ext of def mob
#define DEF_EXIT_EXT       ".dex"  // ext of def exit

#define MAIN_SETFILE_NAME  "de.set"


// name of temp files

#define TMPFILE_NAME "dikued.tmp"


// text buffer stuff

#define TEXT_MEM_ADDR   0xB8000
#define TEXT_MEM_SIZE   80 * 25 * 2  // 80 columns by 25 rows, 2 bytes per char


// screen width in columns

#define DEFAULT_SCREEN_WIDTH   80


// screen height in rows

#define DEFAULT_SCREEN_HEIGHT  25


// help file

#define DE_MAINHELP    "docs\\de.txt"
#define DE_SHORTHELP   "docs\\commands.txt"

#define _SYSTEM_H_
#endif
