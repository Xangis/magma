//
//  File: readmob.cpp    originally part of dikuEdit
//
//  Usage: functions for reading mob info from the .MOB file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "../types.h"
#include "../fh.h"

#include "mob.h"

using namespace std;

extern char readFromSubdirs, madeChanges;
extern ulong numbMobs, numbLookupEntries, numbMobTypes, highestMobNumber, lowestMobNumber;
extern dikuMob **mobLookup, *mobHead;

//
// convertMobMoney : Converts a dot-separated "money string" into separate
//                   integer amounts - for instance, "4.3.2.1" is 4 copper,
//                   3 silver, 2 gold, and 1 platinum - returns TRUE if
//                   there is an error, FALSE otherwise
//
//  *strn : string as read from mob file
//   *mob : corresponding mob record
//

char convertMobMoney(char *strn, dikuMob *mob)
{
  char substrn[81];
  ulong i, subi = 0;


 // first comes the copper

  for (i = 0; (strn[i] != '.') && (strn[i] != '\0'); i++, subi++)
  {
    substrn[subi] = strn[i];
  }

  if (strn[i] == '\0') return TRUE;

  substrn[subi] = '\0';

  if (!strnumer(substrn)) return TRUE;

  mob->copper = atoi(substrn);
  subi = 0;
  i++;

 // next, silver

  for (; (strn[i] != '.') && (strn[i] != '\0'); i++, subi++)
  {
    substrn[subi] = strn[i];
  }

  if (strn[i] == '\0') return TRUE;

  substrn[subi] = '\0';

  if (!strnumer(substrn)) return TRUE;

  mob->silver = atoi(substrn);
  subi = 0;
  i++;

 // next, gold

  for (; (strn[i] != '.') && (strn[i] != '\0'); i++, subi++)
  {
    substrn[subi] = strn[i];
  }

  if (strn[i] == '\0') return TRUE;

  substrn[subi] = '\0';

  if (!strnumer(substrn)) return TRUE;

  mob->gold = atoi(substrn);
  subi = 0;
  i++;

 // and finally, platinum

  for (; strn[i] != '\0'; i++, subi++)
  {
    substrn[subi] = strn[i];
  }

  substrn[subi] = '\0';

  if (!strnumer(substrn)) return TRUE;

  mob->platinum = atoi(substrn);

  return FALSE;
}


//
// readMobFromFile
//

dikuMob *readMobFromFile(FILE *mobFile, uchar checkDupes, uchar incNumbMobs)
{
  char key;
  char strn[512], moneyStrn[64];
  dikuMob *mobNode;
  ulong i;//, len;


 // Read the mob number (remove empty lines, first)

  while (TRUE)
  {
    if (!fgets(strn, 512, mobFile)) return NULL;  // hit EOF

    if (!lastCharLF(strn))  // holy shit - that's one long vnum
    {
      cout << "\n" <<
"error: line in .MOB file is too long - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);
    if (strlen(strn)) break;
  }

 // allocate memory for dikuMob

  mobNode = new dikuMob;
  if (!mobNode)
  {
    cout << "\n" <<
"Uh-oh, ran out of memory allocating memory for a new dikuMob.  aborting..\n";

    exit(1);
  }

 // set everything in mob record to 0/NULL

  memset(mobNode, 0, sizeof(dikuMob));

 // do stuff to vnum string

  if (strn[0] != '#')
  {
    cout << "\n" <<
"Line for mob that should have '#' and vnum doesn't - string read was\n"
"'" << strn << "'.  Aborting..\n";

    exit(1);
  }

  deleteChar(strn, 0);

  i = atoi(strn);

  if (checkDupes && findMob(i))
  {
    cout << "\n"
"Error - mob #" << i << " has more than one entry in the .MOB file.  Aborting..\n";

    exit(1);
  }

  if (i >= numbLookupEntries)
  {
    cout << "\n" <<
"Error: mob #" << i << "'s vnum is too high - only vnums up to " << (numbLookupEntries - 1) << " are allowed.\n" <<
"       To remedy, load dikuEdit with the -ll=<limit> command-line switch.\n\n";

    exit(1);
  }

  mobNode->mobNumber = i;

 // Now, read the mob keywords

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"Error - data for mob #" << mobNode->mobNumber << " not complete - hit EOF while reading\n" <<
"        mob keyword list.  Aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // holy shit - that's one long vnum
  {
    cout << "\n" <<
"error: line that specifies mob keywords for mob #" << mobNode->mobNumber << " is too long\n" <<
"       (longer than 512 characters) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strn[strlen(strn) - 1] != '~')
  {
    cout << "\n" <<
"Error - data for mob #" << mobNode->mobNumber << " not complete - mob keyword list\n" <<
"        doesn't end in ~.  (String read was '" << strn << "'.)\n" <<
"        aborting..\n\n";

    exit(1);
  }

  mobNode->keywordListHead = createKeywordList(strn);

  if (!strcmp(strn, "$~"))  // end of file nuttiness
  {
    return mobNode;
  }

 // read the short mob name

  if (!fgets(strn, MAX_MOBSNAME_LEN, mobFile))
  {
    cout << "\n" <<
"error - data for mob #" << mobNode->mobNumber << " not complete - hit EOF while reading\n" <<
"        short name.  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line that specifies mob short name for mob #" << mobNode->mobNumber << " is too long\n" <<
"       (longer than " << MAX_MOBSNAME_LEN << " characters) - first " << MAX_MOBSNAME_LEN << " characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);

  i = strlen(strn) - 1;  // it's handy
  if (strn[i] != '~')
  {
    cout << "\n" <<
"Error - data for mob #" << mobNode->mobNumber << " not complete - mob short name\n" <<
"        doesn't end in ~.  (String read was '" << strn << "'.)\n" <<
"        aborting..\n\n";

    exit(1);
  }

  strn[i] = '\0';  // get rid of the tilde

  remTrailingSpaces(strn);
  remLeadingSpaces(strn);
  strcpy(mobNode->mobShortName, strn);

 // read the long mob name

  if (!fgets(strn, MAX_MOBLNAME_LEN, mobFile))
  {
    cout << "\n"
"Error - data for mob #" << mobNode->mobNumber << " not complete - hit EOF while\n" <<
"        reading long name.  aborting..\n\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line that specifies mob long name for mob #" << mobNode->mobNumber << " is too long\n" <<
"       (longer than " << MAX_MOBLNAME_LEN << " characters) - first " << MAX_MOBLNAME_LEN << " characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);

  remTrailingSpaces(strn);
  remLeadingSpaces(strn);
  strcpy(mobNode->mobLongName, strn);

 // now, make sure the tilde on its own line is here - required

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"Data for mob #" << mobNode->mobNumber << " not complete - hit EOF while looking\n" <<
"for lone tilde on line after mob long name.  Aborting..\n\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // heh, somebody fucked up big time, eh?
  {
    cout << "\n" <<
"error: tilde line after mob names for mob #" << mobNode->mobNumber << " is too long\n" <<
"       (longer than 512 characters) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remLeadingSpaces(strn);
  remTrailingSpaces(strn);

  if (strcmp(strn, "~"))
  {
    cout << "\n" <<
"Error - mob #" << mobNode->mobNumber << "'s \"lone tilde\" line is invalid\n" <<
"(should be after mob's short and long names).  (String read was\n" <<
"'" << strn << "'.)  aborting..\n";

    exit(1);
  }

 // now, read the mob description

  mobNode->mobDescHead = readStringNodes(mobFile, TILDE_LINE, TRUE);

 // read the rest of the mob info

 // read first line of misc mob info - action, aff1, aff2 flags, align, S

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"error - hit EOF while reading first line of info beyond mob desc for\n" <<
"        mob #" << mobNode->mobNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: first line specifying misc mob info for mob #" << mobNode->mobNumber << " is too long -\n" <<
"       (max length supported is 512) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if ((numbArgs(strn) != 5) && (numbArgs(strn) != 7))
  {
    cout << "\n" <<
"error - first line specifying misc mob info after mob desc for\n" <<
"        mob #" << mobNode->mobNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 5 or 7).  aborting..\n";

    exit(1);
  }

  if (numbArgs(strn) == 7)
  {
    sscanf(strn, "%u%u%u%u%u%d%s",
           &(mobNode->actionBits.longIntFlags),
           &(mobNode->affect1Bits.longIntFlags),
           &(mobNode->affect2Bits.longIntFlags),
           &(mobNode->affect3Bits.longIntFlags),
           &(mobNode->affect4Bits.longIntFlags),
           &(mobNode->alignment),
           strn);  // "S"
  }
  else  // 5 args
  {
    sscanf(strn, "%u%u%u%d%s",
           &(mobNode->actionBits.longIntFlags),
           &(mobNode->affect1Bits.longIntFlags),
           &(mobNode->affect2Bits.longIntFlags),
           &(mobNode->alignment),
           strn);  // "S"
  }

 // check for required stuff

  if (strcmp(strn, "S"))
  {
    cout << "\n" <<
"error - letter after mob alignment for mob #" << mobNode->mobNumber << " not equal to S -\n"
"        aborting..\n";

    cout << "\nThe string read was '" << strn << "'.\n";

    exit(1);
  }

 // read second line of misc mob info - species, hometown, class

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"error - hit EOF while reading second line of info beyond mob desc for\n" <<
"        mob #" << mobNode->mobNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: second line specifying misc mob info for mob #" << mobNode->mobNumber << " is too long -\n" <<
"       (max length supported is 512) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 3)
  {
    cout << "\n" <<
"error - second line specifying misc mob info after mob desc for\n" <<
"        mob #" << mobNode->mobNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 3).  aborting..\n";

    exit(1);
  }

  sscanf(strn, "%s%d%u",
         mobNode->mobSpecies,
         &(mobNode->mobHometown),
         &(mobNode->mobClass));

 // upcase species string, check mob class

  upstrn(mobNode->mobSpecies);

  if (strlen(mobNode->mobSpecies) >= MAX_SPECIES_LEN)
  {
    cout << "\n" <<
"error - mob species string for mob #" << mobNode->mobNumber << " is too long (over " << MAX_SPECIES_LEN - 1 << "\n"
"        characters).  shortening to " << MAX_SPECIES_LEN - 1 << " characters..\n";

    mobNode->mobSpecies[MAX_SPECIES_LEN - 1] = '\0';
    madeChanges = TRUE;

    getkey();
  }
/*
  if (mobNode->mobClass > CLASS_HIGHEST)
  {
    cout << "\n" <<
"error - mob class for mob #" << mobNode->mobNumber << " too high (higher than " << CLASS_HIGHEST << ",\n"
"        class specified is " << (usint)(mobNode->mobClass) << ".)  aborting..\n";

    exit(1);
  }
*/

 // read third line of misc mob info - level, thac0, ac, hit points, damage

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"error - hit EOF while reading third line of info beyond mob desc for\n" <<
"        mob #" << mobNode->mobNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: third line specifying misc mob info for mob #" << mobNode->mobNumber << " is too long -\n" <<
"       (max length supported is 512) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 5)
  {
    cout << "\n" <<
"error - third line specifying misc mob info after mob desc for\n" <<
"        mob #" << mobNode->mobNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 5).  aborting..\n";

    exit(1);
  }

  sscanf(strn, "%d%d%d%s%s",
         &(mobNode->level),
         &(mobNode->thac0),
         &(mobNode->ac),
         mobNode->hitPoints,
         mobNode->mobDamage);

 // check mob hit points/damage strings

  checkDieStrnValidity(mobNode->hitPoints, mobNode->mobNumber, "hit point");
  checkDieStrnValidity(mobNode->mobDamage, mobNode->mobNumber, "damage");

 // read fourth line of misc mob info - money, experience

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"error - hit EOF while reading fourth line of info beyond mob desc for\n" <<
"        mob #" << mobNode->mobNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: fourth line specifying misc mob info for mob #" << mobNode->mobNumber << " is too long -\n" <<
"       (max length supported is 512) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 2)
  {
    cout << "\n" <<
"error - fourth line specifying misc mob info after mob desc for\n" <<
"        mob #" << mobNode->mobNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 2).  aborting..\n";

    exit(1);
  }

  sscanf(strn, "%s%u",
         moneyStrn, &(mobNode->exp));

 // get amount of money mob has

  if (convertMobMoney(moneyStrn, mobNode))
  {
    cout << "\n" <<
"error: field that specifies mob money for mob #" << mobNode->mobNumber << " is somehow invalid -\n" <<
"       string read was '" << moneyStrn << "'.  aborting..\n";

    exit(1);
  }

 // read fifth line of misc mob info - pos, default pos, sex

  if (!fgets(strn, 512, mobFile))
  {
    cout << "\n" <<
"error - hit EOF while reading fifth line of info beyond mob desc for\n" <<
"        mob #" << mobNode->mobNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: fifth line specifying misc mob info for mob #" << mobNode->mobNumber << " is too long -\n" <<
"       (max length supported is 512) - first 512 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (numbArgs(strn) != 3)
  {
    cout << "\n" <<
"error - fifth line specifying misc mob info after mob desc for\n" <<
"        mob #" << mobNode->mobNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 3).  aborting..\n";

    exit(1);
  }

  sscanf(strn, "%d%d%d",
         &(mobNode->position), &(mobNode->defaultPos), &(mobNode->sex));

  if ((mobNode->position < POSITION_LOWEST_LEGAL) ||
      (mobNode->position > POSITION_HIGHEST))
  {
    cout << "\n" <<
"Warning: position value for mob #" << mobNode->mobNumber << " (" << (sint)(mobNode->position) << ") is out of legal range.\n" <<
"\n" <<
"Setting position to 'standing'...\n";

    mobNode->position = POSITION_STANDING;
    madeChanges = TRUE;

    getkey();
  }

  if ((mobNode->defaultPos < POSITION_LOWEST_LEGAL) ||
      (mobNode->defaultPos > POSITION_HIGHEST))
  {
    cout << "\n" <<
"Warning: default pos value for mob #" << mobNode->mobNumber << " (" << (sint)(mobNode->defaultPos) << ") is out of legal range.\n" <<
"\n" <<
"Setting default position to 'standing'...\n";

    mobNode->defaultPos = POSITION_STANDING;
    madeChanges = TRUE;

    getkey();
  }

/* This code is not enabled or functional quite yet.
  // Check for MobProg data
  key = getc(mobFile);

  if( key != '>')
  {
      ungetc( key, mobFile);
  }
  else
  {
      ungetc( key, mobFile);
      mobNode->mobProgHead = readStringNodes(mobFile, TILDE_LINE, TRUE);
  }
* End of non-enabled stupid code */
  if (incNumbMobs)
  {
    numbMobTypes++;

    i = mobNode->mobNumber;

    mobLookup[i] = mobNode;

    if (i > highestMobNumber) highestMobNumber = i;
    if (i < lowestMobNumber)  lowestMobNumber = i;
  }

  return mobNode;
}


//
// readMobFile : Reads all the mob records from the user-specified mob file -
//               returns TRUE if successful, FALSE otherwise
//
//   *filename : if non-NULL, used as filename of mob file, otherwise value in
//               MAINZONENAME is used
//

char readMobFile(const char *filename /* = NULL */)
{
  FILE *mobFile;
  char mobFilename[81] = "\0";

  dikuMob *mob, *oldMobNode = NULL;

  long lastMob = 0;


 // assemble the filename of the mob file and upstrn it so it looks purdy..

  if (readFromSubdirs) strcpy(mobFilename, "mob/");

  if (filename) strcat(mobFilename, filename);
  else strcat(mobFilename, getMainZoneNameStrn());

  strcat(mobFilename, ".mob");

 // open the mob file for reading

  if ((mobFile = fopen(mobFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << mobFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << mobFilename << "...\n";

 // this while loop reads mob by mob, one mob per iteration

  while (!feof(mobFile))
  {
    mob = readMobFromFile(mobFile, TRUE, TRUE);
    if (!mob) break;  // eof

    if (!mobHead) mobHead = mob;

    if (oldMobNode) oldMobNode->Next = mob;
    oldMobNode = mob;

    if (mob->mobNumber < lastMob)
    {
      cout << "\n" <<
"Warning: mob numbers out of order - mobs #" << lastMob <<
" and #" << mob->mobNumber << "\n\n";

      madeChanges = TRUE;

      getkey();
    }
    else lastMob = mob->mobNumber;
  }

  fclose(mobFile);
  return TRUE;
}
