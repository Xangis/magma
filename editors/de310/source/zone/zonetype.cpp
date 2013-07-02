//
//  File: zonetype.cpp   originally part of dikuEdit
//
//  Usage: hundreds of functions for the thousands of zone values
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include "../types.h"

#include "zone.h"

//
// getZoneResetStrn
//

const char *getZoneResetStrn(const ulong resetMode)
{
  if (resetMode > ZONE_RESET_HIGHEST) return "out of range";

  switch (resetMode)
  {
    case ZONE_NO_RESET     : return "never resets";
    case ZONE_RESET_EMPTY  : return "resets when empty";
    case ZONE_RESET_ALWAYS : return "resets at lifespan";

    default : return "unrecognized";
  }
}
