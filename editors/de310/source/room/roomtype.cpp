//
//  File: roomtype.cpp   originally part of dikuEdit
//
//  Usage: functions that return strings based on various enumerated
//         bits of roominess
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../fh.h"
#include "../types.h"

#include "room.h"

//
// getManaFlagStrn
//

const char *getManaFlagStrn(const ulong manaFlag)
{
  switch (manaFlag)
  {
    case APPLY_MANA_ALL : return "all";
    case MANA_GOOD : return "good-only";
    case MANA_NEUTRAL : return "neutral-only";
    case MANA_EVIL : return "evil-only";

    default : return "unrecognized";
  }
}


//
// getRoomSectorStrn
//

char *getRoomSectorStrn(const uchar sectorType, const char addColor,
                        const char addSpaces, const char addBrackets,
                        char *strn)
{
  switch (sectorType)
  {
    case SECT_INSIDE           :
              strcpy(strn, "&+Y[Inside]&n  ");  break;

    case SECT_CITY             :
              strcpy(strn, "&+y[City]&n  ");  break;

    case SECT_FIELD            :
              strcpy(strn, "&+y[Field]&n  ");  break;

    case SECT_FOREST           :
              strcpy(strn, "&+g[Forest]&n  ");  break;

    case SECT_HILLS            :
              strcpy(strn, "&+y[Hills]&n  ");  break;

    case SECT_MOUNTAIN         :
              strcpy(strn, "&+w[Mountains]&n  ");  break;

    case SECT_WATER_SWIM       :
              strcpy(strn, "&+c[Water - swimmable]&n  ");  break;

    case SECT_WATER_NOSWIM     :
              strcpy(strn, "&+c[Water - requires boat]&n  ");  break;

    case SECT_NO_GROUND        :
              strcpy(strn, "&+L[No ground]&n  ");  break;

    case SECT_UNDERWATER       :
              strcpy(strn, "&+b[Underwater]&n  ");  break;

    case SECT_UNDERWATER_GR    :
              strcpy(strn, "&+b[Underwater - ground]&n  ");  break;

    case SECT_PLANE_OF_FIRE    :
              strcpy(strn, "&+r[Plane of fire]&n  ");  break;

    case SECT_OCEAN            :
              strcpy(strn, "&+c[Ocean - requires ship]&n  ");  break;

    case SECT_UNDRWLD_WILD     :
              strcpy(strn, "&+g[Underworld - wild]&n  ");  break;

    case SECT_UNDRWLD_CITY     :
              strcpy(strn, "&+y[Underworld - city]&n  ");  break;

    case SECT_UNDRWLD_INSIDE   :
              strcpy(strn, "&+Y[Underworld - inside]&n  ");  break;

    case SECT_UNDRWLD_WATER    :
              strcpy(strn, "&+c[Underworld - water (swimmable)]&n  ");  break;

    case SECT_UNDRWLD_NOSWIM   :
              strcpy(strn, "&+c[Underworld - water (req boat)]&n  ");  break;

    case SECT_UNDRWLD_NOGROUND :
              strcpy(strn, "&+L[Underworld - no ground]&n  ");  break;

    case SECT_PLANE_OF_AIR     :
              strcpy(strn, "&+W[Plane of air]&n  ");  break;

    case SECT_PLANE_OF_WATER   :
              strcpy(strn, "&+b[Plane of water]&n  ");  break;

    case SECT_PLANE_OF_EARTH   :
              strcpy(strn, "&+y[Plane of earth]&n  ");  break;

    case SECT_ETHEREAL         :
              strcpy(strn, "&+c[Ethereal plane]&n  ");  break;

    case SECT_ASTRAL           :
              strcpy(strn, "&+L[Astral plane]&n  ");  break;

    case SECT_DESERT           :
              strcpy(strn, "&+r[Desert]&n  ");  break;

    case SECT_ARCTIC           :
              strcpy(strn, "&+W[Arctic]&n  ");  break;

    case SECT_SWAMP            :
              strcpy(strn, "&+G[Swamp]&n  ");  break;

    default                    :
              sprintf(strn, "&+R[Unrecognized - %u]&n  ", sectorType);  break;
  }

  if (!addColor) remColorCodes(strn);
  if (!addBrackets)
  {
    deleteChar(strn, 0);
    deleteChar(strn, strlen(strn) - 3);
  }
  if (!addSpaces)
  {
    deleteChar(strn, strlen(strn) - 1);
    deleteChar(strn, strlen(strn) - 1);
  }

  return strn;
}
