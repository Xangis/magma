// LOADED.H - header for LOADED.C

#ifndef _LOADED_H_

#include "../types.h"

typedef struct _entityLoaded
{
  char entityType;      // type of entity - #defines are in MASTER.H
  long entityNumb;      // entity's id number

  long numberLoaded;    // number of entity loaded
  ulong overrideLoaded; // if non-zero, overrides numberLoaded
  ulong globaloverrideLoaded; // Global max of loads

  struct _entityLoaded *Next;  // next one, baby
} entityLoaded;

#define _LOADED_H_
#endif
