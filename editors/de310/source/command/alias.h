#ifndef _ALIAS_H_

#include "command.h"

#define MAX_ALIAS_LEN   41

typedef struct _alias
{
  char aliasStrn[MAX_ALIAS_LEN];     // name of alias (i.e. "EE")
  char commandStrn[MAX_COMMAND_LEN]; // aliased string ("EDIT EXIT")

  struct _alias *Next;
} alias;


#define ALIAS_ADDED       0
#define ALIAS_REPLACED    1
//#define ALIAS_DELETED     2  // should no longer be needed
#define ALIAS_BLANK       3
#define ALIAS_ITSELF      4  // alias aliases itself
#define ALIAS_ERROR       5
#define ALIAS_DISPLAY     6  // user typed "alias bleh" with no other args

#define MAX_ALIAS_ARGS    9

#define _ALIAS_H_
#endif
