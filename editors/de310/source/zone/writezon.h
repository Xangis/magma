#ifndef _WRITEZON_H_

#include "../types.h"

#define COMMENT_POS     35   // x-position of comments

typedef struct _exitWritten
{
  ulong roomNumber;            // room exit is in
  uchar exitNumber;            // direction exit leads (0 = north, etc)

  struct _exitWritten *Next;
} exitWritten;

#define _WRITEZON_H_
#endif
