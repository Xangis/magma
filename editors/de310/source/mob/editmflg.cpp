//
//  File: editmflg.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing mob flags (act, aff1, aff2)
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>

#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "mob.h"

extern _CONST flagDef mobActFlagDef[], aff1FlagDef[], aff2FlagDef[], aff3FlagDef[], aff4FlagDef[];
extern ulong mobActionFlagTemplates[], mobAff1FlagTemplates[], mobAff2FlagTemplates[], mobAff3FlagTemplates[], mobAff4FlagTemplates[];

//
// editMobActionFlags : Edit mob extra flags - "main" function
//
//   *mob : mob to edit
//

void editMobActionFlags(dikuMob *mob)
{
  char strn[256], newName[256];

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing action flags for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  editFlags(mobActFlagDef, &(mob->actionBits.longIntFlags), strn,
            "mob action flags", mobActionFlagTemplates, 3);
}


//
// editMobAffect1Flags : Edit mob extra flags - "main" function
//
//   *mob : mob to edit
//

void editMobAffect1Flags(dikuMob *mob)
{
  char strn[256], newName[256];

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing affect1 flags for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  editFlags(aff1FlagDef, &(mob->affect1Bits.longIntFlags), strn,
            "mob affect1 flags", mobAff1FlagTemplates, 3);
}


//
// editMobAffect2Flags : Edit mob extra flags - "main" function
//
//   *mob : mob to edit
//

void editMobAffect2Flags(dikuMob *mob)
{
  char strn[256], newName[256];

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing affect2 flags for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  editFlags(aff2FlagDef, &(mob->affect2Bits.longIntFlags), strn,
            "mob affect2 flags", mobAff2FlagTemplates, 3);
}


//
// editMobAffect3Flags : Edit mob extra flags - "main" function
//
//   *mob : mob to edit
//

void editMobAffect3Flags(dikuMob *mob)
{
  char strn[256], newName[256];

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing affect3 flags for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  editFlags(aff3FlagDef, &(mob->affect3Bits.longIntFlags), strn,
            "mob affect3 flags", mobAff3FlagTemplates, 3);
}


//
// editMobAffect4Flags : Edit mob extra flags - "main" function
//
//   *mob : mob to edit
//

void editMobAffect4Flags(dikuMob *mob)
{
  char strn[256], newName[256];

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing affect4 flags for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  editFlags(aff4FlagDef, &(mob->affect4Bits.longIntFlags), strn,
            "mob affect4 flags", mobAff4FlagTemplates, 3);
}
