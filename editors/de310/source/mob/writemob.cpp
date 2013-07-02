//
//  File: writemob.cpp   originally part of dikuEdit
//
//  Usage: functions for writing mob info to the .MOB file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <iostream>

#include "../fh.h"
#include "../types.h"

#include "mob.h"

using namespace std;

extern ulong numbMobTypes;
extern char readFromSubdirs;

//
// writeMobtoFile
//

void writeMobtoFile(FILE *mobFile, dikuMob *mob)
{
  char strn[256];


 // first, write the mob number

  fprintf(mobFile, "#%u\n", mob->mobNumber);

 // next, the mob keyword list

  createKeywordString(mob->keywordListHead, strn);
  lowstrn(strn);
  strcat(strn, "\n");

  fputs(strn, mobFile);

 // next, the short name of the mob

  strcpy(strn, mob->mobShortName);
  strcat(strn, "~\n");

  fputs(strn, mobFile);

 // next, the long name of the mob

  strcpy(strn, mob->mobLongName);
  strcat(strn, "\n");

  fputs(strn, mobFile);

 // add another line with nothing but a tilde cuz it must be there

  fputs("~\n", mobFile);

 // write the mob desc

  writeStringNodes(mobFile, mob->mobDescHead);

 // terminate the description

  fputs("~\n", mobFile);

 // next, write the action, affect1, and affect2 flags, alignment and "S"

  fprintf(mobFile, "%u %u %u %u %u %d S\n", mob->actionBits.longIntFlags,
                                      mob->affect1Bits.longIntFlags,
                                      mob->affect2Bits.longIntFlags,
                                      mob->affect3Bits.longIntFlags,
                                      mob->affect4Bits.longIntFlags,
                                      mob->alignment);

 // next, the species, hometown, and mob class

//  upstrn(mob->mobSpecies);  // already done on all input
  fprintf(mobFile, "%s %d %u\n", mob->mobSpecies, mob->mobHometown,
                                 mob->mobClass);

 // next, the level, thac0, ac, hit points and damage

  fprintf(mobFile, "%u %d %d %s %s\n", mob->level, mob->thac0, mob->ac,
                                       mob->hitPoints, mob->mobDamage);

 // next, the gold and exp

  fprintf(mobFile, "%u.%u.%u.%u %d\n", mob->copper, mob->silver, mob->gold,
                                       mob->platinum, mob->exp);

 // almost finally, the pos, default pos, and sex

  fprintf(mobFile, "%d %d %d\n", mob->position, mob->defaultPos, mob->sex);

/* This is disabled because it just doesen't work right yet
 // last, write the mob prog

  if( mob->mobProgHead )
  {
    writeStringNodes(mobFile, mob->mobProgHead);
  }

  fputs("~\n", mobFile);
* End of disabled code */
}


//
// writeMobFile : Write the mob file - contains all the mobs
//

void writeMobFile(const char *filename /*= NULL*/)
{
  FILE *mobFile;
  char mobFilename[81] = "\0";
  char strn[256];
  dikuMob *mob;

  ulong lowest = getLowestMobNumber(), highest = getHighestMobNumber(),
        numb = lowest;


 // assemble the filename of the mob file

  if (readFromSubdirs) strcpy(mobFilename, "mob/");

  if (filename) strcat(mobFilename, filename);
  else strcat(mobFilename, getMainZoneNameStrn());

  strcat(mobFilename, ".mob");


 // open the mob file for writing

  if ((mobFile = fopen(mobFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << mobFilename << " for writing - aborting\n";

    return;
  }

  sprintf(strn, "Writing %s - %u mob type%s\n",
          mobFilename, numbMobTypes, plural(numbMobTypes));

  _outtext(strn);

  while (numb <= highest)
  {
    mob = findMob(numb);

    if (mob)
    {
      writeMobtoFile(mobFile, mob);
    }

    numb++;
  }

  fclose(mobFile);
}
