#ifndef _EDESC_H_

#include "../misc/strnnode.h"

// extra description stuff

#define MAX_EDESCKEY_LEN   MAX_STRNNODE_LEN - 16

typedef struct _extraDesc
{
  stringNode *keywordListHead;
                    // instead of one string that contains all the keywords,
                    // this list contains all the keywords parsed, so that
                    // each node has a keyword

  stringNode *extraDescStrnHead;

//  void *extraDescOwnerPtr;  // pointer to "owner" of extra desc
//  char extraDescOwnerType;  // type of owner

  struct _extraDesc *Last;
  struct _extraDesc *Next;
} extraDesc;


#define _EDESC_H_
#endif
