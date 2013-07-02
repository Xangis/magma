#ifndef _COMMAND_H_

#include "types.h"

#define MAX_COMMAND_LEN       41

typedef struct _command
{
  char commandStrn[MAX_COMMAND_LEN];  // full command
  uchar command;  // command to execute - various values - blah

  struct _command *Next;
} command;

#define _COMMAND_H_
#endif
