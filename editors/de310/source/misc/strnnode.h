// STRNNODE.H - header file for STRNNODE.C

#ifndef _STRNNODE_H_

#include "../types.h"

// string node stuff

#define MAX_STRNNODE_LEN   (usint)256

typedef struct _stringNode
{
  char string[MAX_STRNNODE_LEN];

  struct _stringNode *Last;
  struct _stringNode *Next;
} stringNode;


// constants for readStringNodes

#define TILDE_LINE     0      // End of string nodes signaled by line with
                              // nothing but a tilde on it
#define ENDOFFILE      1      // Pretty self-explanatory methinks

#define VERT_LINE      2      // End of string nodes signaled by line with
                              // nothing but a | on it.
#define _STRNNODE_H_
#endif
