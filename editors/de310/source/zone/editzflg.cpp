//
//  File: editzflg.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing the many zone flags
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "zone.h"

extern _CONST flagDef zoneMiscFlagDef[];

//
// editZoneMiscFlags : Edit zone misc flags - "main" function
//
//   *zone : zone to edit
//

void editZoneMiscFlags(dikuZone *zone)
{
  char strn[256], newName[MAX_ZONENAME_LEN];

  fixZoneName(zone->zoneName, newName);

  sprintf(strn, "&+gEditing misc flags for zone #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          zone->zoneNumber, newName);

  displayColorString(strn);

  editFlags(zoneMiscFlagDef, &(zone->miscBits.longIntFlags), strn,
            "zone misc flags", NULL, 1);
}
