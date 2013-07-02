#ifndef _DISPZONE_H_

typedef struct _zoneExit
{
  dikuRoom *roomIn;  // room exit is in
  long outRoom;      // room exit leads to (outside of zone)
  char exitDir;      // direction of exit

  struct _zoneExit *Next;
} zoneExit;

#define _DISPZONE_H_
#endif
