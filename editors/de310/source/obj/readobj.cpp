//
//  File: readobj.cpp    originally part of dikuEdit
//
//  Usage: functions used to read object info from the .OBJ file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../fh.h"
#include "../types.h"

#include "../vardef.h"
#include "../edesc/edesc.h"
#include "object.h"

using namespace std;

extern dikuObject *objectHead, **objLookup;
extern variable *varHead;
extern char oldObjConv, madeChanges, readFromSubdirs;
extern ulong numbLookupEntries, numbObjTypes, lowestObjNumber, highestObjNumber;

//
// checkLastObjStrn : checks for existance of affect flags for an obj,
//                    reading in values in either case - returns TRUE if there
//                    is an error, FALSE otherwise
//
//        *strn : string read from file
//  *objectNode : pointer to object strn was read from
//

char checkLastObjStrn(const char *strn, dikuObject *objectNode)
{
  ulong args = numbArgs(strn);


  if (!objectNode) return TRUE;  // can't be too careful

  if (args == 3)
  {
    sscanf(strn, "%d%u%u", &(objectNode->weight), &(objectNode->value),
                           &(objectNode->condition));
  }
  else
  if( args == 4)
  {
    sscanf(strn, "%d%u%u%d", &(objectNode->weight), &(objectNode->value),
                           &(objectNode->condition), &objectNode->maxload);
  }
  else if (args == 5)
  {
    sscanf(strn, "%d%u%u%u%u", &(objectNode->weight), &(objectNode->value),
                               &(objectNode->condition),
                               &(objectNode->affect1Bits.longIntFlags),
                               &(objectNode->affect2Bits.longIntFlags));

    setVarBoolVal(&varHead, VAR_OBJAFFECT_NAME, TRUE, FALSE);
  }
  else if (args == 6)
  {
    sscanf(strn, "%d%u%u%u%u%d", &(objectNode->weight), &(objectNode->value),
                               &(objectNode->condition),
                               &(objectNode->affect1Bits.longIntFlags),
                               &(objectNode->affect2Bits.longIntFlags),
                               &(objectNode->maxload));

    setVarBoolVal(&varHead, VAR_OBJAFFECT_NAME, TRUE, FALSE);
  }
  else
  if (args == 7)
  {
    sscanf(strn, "%d%u%u%u%u%u%u", &(objectNode->weight), &(objectNode->value),
                               &(objectNode->condition),
                               &(objectNode->affect1Bits.longIntFlags),
                               &(objectNode->affect2Bits.longIntFlags),
                               &(objectNode->affect3Bits.longIntFlags),
                               &(objectNode->affect4Bits.longIntFlags));
    setVarBoolVal(&varHead, VAR_OBJAFFECT_NAME, TRUE, FALSE);
  }
  else
  if (args == 8)
  {
    sscanf(strn, "%d%u%u%u%u%u%u%d", &(objectNode->weight),
                               &(objectNode->value),
                               &(objectNode->condition),
                               &(objectNode->affect1Bits.longIntFlags),
                               &(objectNode->affect2Bits.longIntFlags),
                               &(objectNode->affect3Bits.longIntFlags),
                               &(objectNode->affect4Bits.longIntFlags),
                               &(objectNode->maxload ));
    setVarBoolVal(&varHead, VAR_OBJAFFECT_NAME, TRUE, FALSE);
  }
  else return TRUE;

  return FALSE;
}


//
// readObjExtraDesc : Reads an extra description for a specified room - returns
//                    the address of the new extra description node
//
//    *objFile          : object file info
//    *objNode          : pointer to current object node
//    *extraDescOldNode : pointer to previous extraDesc node, if any
//

extraDesc *readObjExtraDesc(FILE *objFile, dikuObject *objNode,
                            extraDesc *extraDescOldNode)
{
  extraDesc *extraDescNode;
  char strn[MAX_EDESCKEY_LEN + 5];  // + 5 for linefeeds, etc (can't be too
                                    // careful)


 // create new extra description node

  extraDescNode = new extraDesc;
  if (!extraDescNode)
  {
    cout << "Error allocating memory for extraDescNode - aborting\n";

    exit(1);
  }

 // set stuff

  memset(extraDescNode, 0, sizeof(extraDesc));

  if (extraDescOldNode) extraDescOldNode->Next = extraDescNode;
  extraDescNode->Last = extraDescOldNode;

  if (!objNode->extraDescHead)
  {
    objNode->extraDescHead = extraDescNode;
  }

 // read the extra description keyword list - all on one line

  if (!fgets(strn, MAX_EDESCKEY_LEN, objFile))
  {
    cout << "\n" <<
"Unexpected EOF while reading extra desc. keyword list for object #" <<
            objNode->objNumber << "\n\n";

    exit(1);
  }

  if (!lastCharLF(strn))  // couldn't read whole string
  {
    cout << "\n" <<
"Error - couldn't read entire keyword string for extra desc for object\n" <<
"        #" << objNode->objNumber << " - max length that can be read is " << MAX_EDESCKEY_LEN << ".\n" <<
"        aborting..\n";

    exit(1);
  }

 // remove linefeeds and check for tilde at the end of the keyword strn

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strn[strlen(strn) - 1] != '~')
  {
    cout << "\n" <<
"Error - keyword list for object #" << objNode->objNumber << "'s extra desc. doesn't\n"
"        end in a ~.  aborting..\n\n";

    exit(1);
  }

 // generate a keyword list with this string

  extraDescNode->keywordListHead = createKeywordList(strn);

 // now, read the actual description

  extraDescNode->extraDescStrnHead = readStringNodes(objFile, TILDE_LINE, FALSE);


  return extraDescNode;
}


//
// readObjectFromFile
//

dikuObject *readObjectFromFile(FILE *objectFile, char *nextStrn,
                               const uchar checkDupes, const uchar incNumbObjs)
{
  uchar applies = 0, preConv = oldObjConv;//, setMaxSP = FALSE;
  char strn[256], lastObjStrn[256];

  dikuObject *objectNode;
  extraDesc *extraDescOldNode = NULL;

  long i;


  strcpy(strn, nextStrn);

 // Read the object number, but only if it hasn't been previously read
 // below

  if (!strlen(strn))
  {
    while (TRUE)
    {
      if (!fgets(strn, 256, objectFile))
      {
        return NULL;  // reached end of object file
      }

      if (!lastCharLF(strn))  // holy shit - that's one long vnum
      {
        cout << "\n" <<
"error: line in .OBJ file is too long - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

        exit(1);
      }

      nolf(strn);
      remTrailingSpaces(strn);
      remLeadingSpaces(strn);
      if (strlen(strn)) break;
    }
  }

 // allocate memory for dikuObject

  objectNode = new dikuObject;
  if (!objectNode)
  {
    cout << "\n" <<
"Uh-oh, ran out of memory allocating memory for a new dikuObj - aborting..\n";

    exit(1);
  }

 // set everything in object record to 0/NULL

  memset(objectNode, 0, sizeof(dikuObject));

 // interpret vnum string

  if (strn[0] != '#')
  {
    cout << "\n" <<
"Line for object that should have '#' and vnum doesn't - string read was\n"
"'" << strn << "'.  Aborting..\n";

    exit(1);
  }

  deleteChar(strn, 0);

  i = atoi(strn);

  if (checkDupes && findObj(i))
  {
    cout << "\n" <<
"Error - object #" << i << " has more than one entry in the .OBJ file.\n" <<
"        aborting..\n";

    exit(1);
  }

  if (i >= numbLookupEntries)
  {
    cout << "\n" <<
"Error: obj #" << i << "'s vnum is too high - only vnums up to " << (numbLookupEntries - 1) << " are allowed.\n" <<
"       To remedy, load dikuEdit with the -ll=<limit> command-line switch.\n\n";

    exit(1);
  }

  objectNode->objNumber = i;

 // Now, read the object keywords

  if (!fgets(strn, 256, objectFile))
  {
    cout << "\n" <<
"Error - data for obj #" << objectNode->objNumber << " not complete - hit EOF while\n" <<
"        reading object keyword list.  Aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line for object #" << objectNode->objNumber << "'s keywords is too long (max length\n"
"       supported is 256) - first 256\n"
"       characters are '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strn[strlen(strn) - 1] != '~')
  {
    cout << "\n" <<
"Error - data for obj #" << objectNode->objNumber << " not complete - object keyword\n" <<
"        list doesn't end in ~.  Aborting..\n";

    exit(1);
  }

  objectNode->keywordListHead = createKeywordList(strn);

  if (!strcmp(strn, "$~"))  // end of file nuttiness
  {
    if (!fgets(nextStrn, 256, objectFile))
    {
/*
      cout << "\n" <<
"error - hit EOF while reading line after '$~' keyword line for object #" << objectNode->objNumber << "\n" <<
 "       aborting..\n";

      exit(1);
*/
      return objectNode;
    }

    if (!lastCharLF(nextStrn))
    {
      cout << "\n" <<
  "error: line after '$~' keyword line for object #" << objectNode->objNumber << " is too long -\n" <<
  "       (max length supported is 256) - first 256 characters are\n" <<
  "       '" << nextStrn << "'.\n" <<
  "       aborting..\n";

      exit(1);
    }

    nolf(nextStrn);

    return objectNode;
  }

 // read the short object name

  if (!fgets(strn, MAX_OBJSNAME_LEN, objectFile))
  {
    cout << "\n" <<
"error - data for obj #" << objectNode->objNumber << " not complete - hit EOF while\n" <<
"        reading short name.  Aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line for object #" << objectNode->objNumber << "'s short name is too long (max length\n" <<
"       supported is " << MAX_OBJSNAME_LEN << ") - first " << MAX_OBJSNAME_LEN << " characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);

  i = strlen(strn) - 1;  // it's handy

  if (strn[i] != '~')
  {
    cout << "Data for obj #" << objectNode->objNumber << " not complete - "
         << "object short name doesn't end in ~.  Aborting..\n";

    exit(1);
  }

  strn[i] = '\0';  // get rid of the tilde
  remLeadingSpaces(strn);
  remTrailingSpaces(strn);

  strcpy(objectNode->objShortName, strn);

 // read the long object name

  if (!fgets(strn, MAX_OBJLNAME_LEN, objectFile))
  {
    cout << "\n" <<
"error - data for obj #" << objectNode->objNumber << " not complete - hit EOF while\n" <<
"        reading long name.  Aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line for object #" << objectNode->objNumber << "'s long name is too long (max length\n" <<
"       supported is " << MAX_OBJLNAME_LEN << ") - first " << MAX_OBJLNAME_LEN << " characters are\n" <<
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
"error - data for obj #" << objectNode->objNumber << " not complete - object long name\n" <<
"        doesn't end in ~.  Aborting..\n";

    cout << "\n" <<
"In fact, it ends in '" << strn[i] << "' (complete string is '" << strn << "').\n";


    exit(1);
  }

  strn[i] = '\0';  // get rid of the tilde
  remLeadingSpaces(strn);
  remTrailingSpaces(strn);

  strcpy(objectNode->objLongName, strn);

 // now, make sure the tilde on its own line is here - required

  if (!fgets(strn, 256, objectFile))
  {
    cout << "\n" <<
"Data for obj #" << objectNode->objNumber << " not complete - hit EOF while looking for\n" <<
"lone tilde after object names.  Aborting..\n\n";

    exit(1);
  }

 // this will likely never happen, but hell

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: line for object #" << objectNode->objNumber << "'s lone tilde is too long\n" <<
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if (strcmp(strn, "~"))
  {
    cout << "\n" <<
"Error in obj #" << objectNode->objNumber << " data - no \"lone tilde\" found after\n" <<
"object names.  (String read was '" << strn << "'.)  Aborting..\n\n";

    exit(1);
  }


 // read object type, extra flag, and wear flag; value 0, 1, 2, and 3;
 // and weight, value, and cost/day

 // read first line of misc obj info - type, extra flag, and wear flag

  if (!fgets(strn, 256, objectFile))
  {
    cout << "\n" <<
"error - hit EOF while reading first line of info beyond object name info for\n" <<
"        object #" << objectNode->objNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: first line specifying misc obj info for object #" << objectNode->objNumber << " is too long -\n" <<
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if ((numbArgs(strn) != 3) && (numbArgs(strn) != 5) && (numbArgs(strn) != 6) &&
      (numbArgs(strn) != 9) && (numbArgs(strn) != 11) /*&& (numbArgs(strn) != 12)*/)
  {
    cout << "\n" <<
"error - first line specifying misc obj info after object name info for\n" <<
"        object #" << objectNode->objNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 3, 5, 6, 9, or 11).  aborting..\n";

    exit(1);
  }

 // undocumented feature - read 12 arg lists since I had 12 arg obj files
 // for a few days..  shrug

/*
  if (numbArgs(strn) == 12)
  {
    sscanf(strn, "%u%u%u%u" "%u%u%u" "%u%u%u%u%u",
           &(objectNode->objType),
           &(objectNode->material),
           &(objectNode->size),
           &(objectNode->space),

           &i,
           &(objectNode->craftsmanship),
           &(objectNode->damResistBonus),

           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags),
           &(objectNode->extra2Bits.longIntFlags),
           &(objectNode->antiBits.longIntFlags),
           &(objectNode->anti2Bits.longIntFlags));

    madeChanges = TRUE;
  }
  else
*/
  if (numbArgs(strn) == 11)
  {
    sscanf(strn, "%u%u%u%u" "%u%u" "%u%u%u%u%u",
           &(objectNode->objType),
           &(objectNode->material),
           &(objectNode->size),
           &(objectNode->space),

           &(objectNode->craftsmanship),
           &(objectNode->damResistBonus),

           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags),
           &(objectNode->extra2Bits.longIntFlags),
           &(objectNode->antiBits.longIntFlags),
           &(objectNode->anti2Bits.longIntFlags));
  }
  else
  if (numbArgs(strn) == 9)
  {
    sscanf(strn, "%u%u%u%u%u%u%u%u%u",
           &(objectNode->objType),
           &(objectNode->material),
           &(objectNode->size),
           &(objectNode->space),
           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags),
           &(objectNode->extra2Bits.longIntFlags),
           &(objectNode->antiBits.longIntFlags),
           &(objectNode->anti2Bits.longIntFlags));
  }
  else
  if (numbArgs(strn) == 6)
  {
    sscanf(strn, "%u%u%u%u%u%u",
           &(objectNode->objType),
           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags),
           &(objectNode->extra2Bits.longIntFlags),
           &(objectNode->antiBits.longIntFlags),
           &(objectNode->anti2Bits.longIntFlags));

    madeChanges = oldObjConv = TRUE;
  }
  else
  if (numbArgs(strn) == 5)
  {
    sscanf(strn, "%u%u%u%u%u",
           &(objectNode->objType),
           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags),
           &(objectNode->extra2Bits.longIntFlags),
           &(objectNode->antiBits.longIntFlags));

    madeChanges = oldObjConv = TRUE;
  }
  else  // 3 args
  {
    sscanf(strn, "%u%u%u",
           &(objectNode->objType),
           &(objectNode->extraBits.longIntFlags),
           &(objectNode->wearBits.longIntFlags));

    madeChanges = oldObjConv = TRUE;
  }

 // initialize crap to default values if not new format

  if ((numbArgs(strn) != 11) /*&& (numbArgs(strn) != 12)*/)
  {
//    setMaxSP = TRUE;
//    objectNode->maxSP = 100;
    objectNode->craftsmanship = OBJCRAFT_AVERAGE;
  }

  if (oldObjConv != preConv)
  {
    cout << "\n" <<
".OBJ file contains at least one object that does not have information\n" <<
"on object material, size, and volume.  Object(s) will automatically be\n" <<
"converted to new format, including copying material value from val4\n" <<
"of armor/worn objects to new location.  All other objects will have their\n" <<
"material, volume, and size set to default values and should be reviewed.\n\n" <<
"Press any key to continue...";

    getkey();

    cout << "\n";
  }

 // read second line of misc obj info - object values

  if (!fgets(strn, 256, objectFile))
  {
    cout << "\n" <<
"error - hit EOF while reading second line of info beyond object name info for\n" <<
"        object #" << objectNode->objNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(strn))
  {
    cout << "\n" <<
"error: second line specifying misc obj info for object #" << objectNode->objNumber << " is too long -\n" <<
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(strn);
  remTrailingSpaces(strn);
  remLeadingSpaces(strn);

  if ((numbArgs(strn) != 4) && (numbArgs(strn) != 8))
  {
    cout << "\n" <<
"error - second line specifying misc obj info after object name info for\n" <<
"        object #" << objectNode->objNumber << " doesn't have the correct number of fields\n" <<
"        (" << numbArgs(strn) << " instead of 4 or 8).  aborting..\n";

    exit(1);
  }

  if (numbArgs(strn) == 4)
  {
    sscanf(strn, "%d%d%d%d",
           &(objectNode->objValues[0]),
           &(objectNode->objValues[1]),
           &(objectNode->objValues[2]),
           &(objectNode->objValues[3]));

    madeChanges = TRUE;
  }
  else
  if (numbArgs(strn) == 8)
  {
    sscanf(strn, "%d%d%d%d" "%d%d%d%d",
           &(objectNode->objValues[0]),
           &(objectNode->objValues[1]),
           &(objectNode->objValues[2]),
           &(objectNode->objValues[3]),
           &(objectNode->objValues[4]),
           &(objectNode->objValues[5]),
           &(objectNode->objValues[6]),
           &(objectNode->objValues[7]));
  }

  if (!fgets(lastObjStrn, 256, objectFile))
  {
    cout << "\n" <<
"error - hit EOF while reading third line beyond object name info for\n" <<
"        object #" << objectNode->objNumber << ".  aborting..\n";

    exit(1);
  }

  if (!lastCharLF(lastObjStrn))
  {
    cout << "\n" <<
"error: third line specifying misc obj info for object #" << objectNode->objNumber << " is too long -\n" <<
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << lastObjStrn << "'.\n" <<
"       aborting..\n";

    exit(1);
  }

  nolf(lastObjStrn);

 // check and "fix" lastObjStrn

  if (checkLastObjStrn(lastObjStrn, objectNode))
  {
    cout << "\n" <<
"error - last line of required object info for obj type #" << objectNode->objNumber << " is invalid..\n" <<
"        string read was '" << lastObjStrn << "'.  aborting..\n\n";

    exit(1);
  }

//  if (setMaxSP) objectNode->maxSP = objectNode->condition;

 // Now, check for E, A, #, or EOF - E is extra description, A is an "object
 // apply modifier", # at the start of a string means that we've hit the next
 // object, and EOF means the list of objects is done

  while (TRUE)
  {
    if (!fgets(strn, 256, objectFile)) break;  // EOF

    if (!lastCharLF(strn))
    {
      cout << "\n" <<
"error: line after misc obj info for object #" << objectNode->objNumber << " is too long -\n" <<
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

      exit(1);
    }

    nolf(strn);
    remTrailingSpaces(strn);
    remLeadingSpaces(strn);

    if (strn[0] == '\0') // blank line
    {
      continue;
    }
    else
    if (!strcmp(strn, "E"))  // extra description
    {
      extraDescOldNode = readObjExtraDesc(objectFile, objectNode,
                                          extraDescOldNode);
    }
    else
    if (!strcmp(strn, "A"))  // apply modifier
    {
      if (applies >= NUMB_OBJ_APPLIES)
      {
        cout << "\n" <<
"error - too many object apply modifiers for object #" << objectNode->objNumber << ".  (max number of\n"
"        apply modifiers supported is " << NUMB_OBJ_APPLIES << ".)  aborting..\n";

        exit(1);
      }

      if (!fgets(strn, 256, objectFile))
      {
        cout << "\n" <<
"error - hit EOF while reading info for object apply modifier #" << (usint)(applies + 1) << " for\n" <<
"        object #" << objectNode->objNumber << ".  Aborting..\n";

        exit(1);
      }

      if (!lastCharLF(strn))
      {
        cout << "\n" <<
"error: line that specifies apply info for object #" << objectNode->objNumber << " (apply #" << (usint)(applies + 1) << ")\n"
"       is too long - (max length supported is 256) - first 256 characters are\n" <<
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
"error - line that specifies apply info for object #" << objectNode->objNumber << ", apply #" << (usint)(applies + 1) << "\n" <<
"        doesn't have the correct number of fields specified (" << numbArgs(strn) << "\n" <<
"        instead of 2).  aborting..\n";

        exit(1);
      }

      sscanf(strn, "%u%d",
             &(objectNode->objApply[applies].applyWhere),
             &(objectNode->objApply[applies].applyModifier));

      applies++;
    }
    else

   // a trap!

    if (!strcmp(strn, "T"))  // trap
    {
      if (!fgets(strn, 256, objectFile))
      {
        cout << "\n" <<
"error - hit EOF while reading info for trap for obj type #" << objectNode->objNumber << ".\n" <<
"        aborting..\n";

        exit(1);
      }

      if (!lastCharLF(strn))
      {
        cout << "\n" <<
"error: line that specifies trap info for object #" << objectNode->objNumber << " is too long -\n"
"       (max length supported is 256) - first 256 characters are\n" <<
"       '" << strn << "'.\n" <<
"       aborting..\n";

        exit(1);
      }

      nolf(strn);
      remTrailingSpaces(strn);
      remLeadingSpaces(strn);

     // support old format

      if (numbArgs(strn) == 3)
      {
        sscanf(strn, "%u%d%d",
               &(objectNode->trapAff),
               &(objectNode->trapDam),
               &(objectNode->trapCharge));

        objectNode->trapLevel = objectNode->condition;
        objectNode->trapPercent = 100;
        objectNode->condition = 100;
      }
      else
      if (numbArgs(strn) == 4)
      {
        sscanf(strn, "%u%d%d%u",
               &(objectNode->trapAff),
               &(objectNode->trapDam),
               &(objectNode->trapCharge),
               &(objectNode->trapLevel));
        objectNode->trapPercent = 100;
      }
      else
      if (numbArgs(strn) == 5)
      {
        sscanf(strn, "%u%d%d%u",
               &(objectNode->trapAff),
               &(objectNode->trapDam),
               &(objectNode->trapCharge),
               &(objectNode->trapLevel),
               &(objectNode->trapPercent));
      }
      else
      {
        cout << "\n" <<
"error - line that specifies trap info for object #" << objectNode->objNumber << " doesn't\n" <<
"        have the correct number of fields specified (" << numbArgs(strn) << " instead\n" <<
"        of 3, 4, or 5.).  aborting..\n";

        exit(1);
      }
    }
    else

   // hit next object, stick string in nextStrn

    if (strn[0] == '#')
    {
      extraDescOldNode = NULL;

      strcpy(nextStrn, strn);

      break;
    }
    else
    {
      cout << "\n" <<
"error - unrecognized extra stuff - \"" << strn << "\" found in data for\n" <<
"        obj #" << objectNode->objNumber << "\n";

      exit(1);
    }
  }

  if (incNumbObjs)
  {
    numbObjTypes++;

    i = objectNode->objNumber;

    objLookup[i] = objectNode;

    if (i > highestObjNumber) highestObjNumber = i;
    if (i < lowestObjNumber)  lowestObjNumber = i;
  }

  return objectNode;
}


//
// readObjectFile : Reads all the object records from the user-specified object
//                  file, returning TRUE if the file was read successfully
//                  and FALSE otherwise
//
//    *filename : pointer to a filename string, if NULL, string in the
//                MAINZONENAME var is used
//

char readObjectFile(const char *filename /*= NULL*/)
{
  FILE *objectFile;
  char objectFilename[256] = "\0", nextStrn[256] = "\0";

  dikuObject *obj, *oldObjectNode = NULL;

  ulong lastObj = 0;


 // assemble the filename of the object file

  if (readFromSubdirs) strcpy(objectFilename, "obj/");

  if (filename) strcat(objectFilename, filename);
  else strcat(objectFilename, getMainZoneNameStrn());

  strcat(objectFilename, ".obj");

 // open the object file for reading

  if ((objectFile = fopen(objectFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << objectFilename << ", skipping\n";

    return FALSE;
  }

  cout << "Reading " << objectFilename << "...\n";

 // this while loop reads object by object, one object per iteration

  while (!feof(objectFile))
  {
    obj = readObjectFromFile(objectFile, nextStrn, TRUE, TRUE);
    if (!obj) break;

    if (!objectHead) objectHead = obj;

    if (oldObjectNode) oldObjectNode->Next = obj;
    oldObjectNode = obj;

    if (obj->objNumber < lastObj)
    {
      cout << "\n" <<
"Warning: object numbers out of order - objects #" << lastObj <<
" and #" << obj->objNumber << "\n";

      madeChanges = TRUE;

      getkey();
    }
    else lastObj = obj->objNumber;

   // check for '_ID_' nuttiness

    if ((getEdescinList(obj->extraDescHead, "_ID_NAME_") ||
         getEdescinList(obj->extraDescHead, "_ID_SHORT_") ||
         getEdescinList(obj->extraDescHead, "_ID_DESC_")) &&
        !scanKeyword("_ID_", obj->keywordListHead))
    {
      cout << "\n" <<
"Warning: object number " << obj->objNumber << " has an 'identification extra desc'\n" <<
"         but doesn't have the '_ID_' keyword in its keyword list.  It has been\n" <<
"         automatically added.\n"
"\n"
"Press any key to continue...";

      addKeywordtoList(&obj->keywordListHead, "_ID_");

      getkey();

      cout << "\n\n";

      madeChanges = TRUE;
    }
  }

  fclose(objectFile);

 // we're done reading the file - but now we get to check if we need to
 // convert em over.  fun begins.

  if (oldObjConv)
  {
    obj = objectHead;
    while (obj)
    {
      obj->size = OBJSIZE_MEDIUM;

      if ((obj->objType == ITEM_WORN) || (obj->objType == ITEM_ARMOR))
      {
        switch (obj->objValues[3])
        {
          case  1 : obj->material = MAT_IRON; break;
          case  2 : obj->material = MAT_MITHRIL; break;
          case  3 : obj->material = MAT_HARDWOOD; break;
          case  4 : obj->material = MAT_CLOTH; break;
          case  5 : obj->material = MAT_LEATHER; break;
          case  6 : obj->material = MAT_SILICON; break;
          case  7 : obj->material = MAT_CRYSTAL; break;
          case  8 : obj->material = MAT_ADAMANTIUM; break;
          case  9 : obj->material = MAT_BONE; break;
          case 10 : obj->material = MAT_STONE; break;

          default : obj->material = MAT_IRON; break;
        }

        obj->objValues[3] = 0;
      }
      else obj->material = MAT_IRON;

      obj = obj->Next;
    }
  }

  return TRUE;
}
