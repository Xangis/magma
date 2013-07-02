#ifndef _READZON_H_

typedef struct _lastObjHereLoaded
{
  ulong objNumb;
  objectHere *objHerePtr;

  struct _lastObjHereLoaded *Next;
} lastObjHereLoaded;


typedef struct _limitSpecified
{
  char entityType;
  ulong vnum;
  ulong limit;
  ulong globallimit; // For objs only

  struct _limitSpecified *Next;
} limitSpecified;


#define _READZON_H_
#endif
