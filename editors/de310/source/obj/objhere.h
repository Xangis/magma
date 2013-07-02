#ifndef _OBJHERE_H_

#include "../types.h"
#include "object.h"

// "object here" record - used for objects in a room, mob, etc.

typedef struct _objectHere
{
  ulong objectNumb;       // number of object
  dikuObject *objectPtr;  // points to address of object type

  struct _objectHere *objInside;  // for containers, you see

  ulong randomChance;

//  ulong tableObj;     // if TRUE, objectNumb is table numb

  struct _objectHere *Next;
} objectHere;


#define _OBJHERE_H_
#endif
