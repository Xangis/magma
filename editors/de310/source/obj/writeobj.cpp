//
//  File: writeobj.cpp   originally part of dikuEdit
//
//  Usage: functions for writing object info to the .OBJ file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <iostream>

#include "../fh.h"
#include "../types.h"

#include "object.h"

using namespace std;

extern dikuObject *objectHead;
extern char readFromSubdirs, madeChanges;
extern ulong numbObjs, numbObjTypes, numbLookupEntries;

//
// writeObjecttoFile
//

void writeObjecttoFile(FILE *objectFile, const dikuObject *obj)
{
  char strn[256], i;
  extraDesc *descNode;


 // write the object number

  fprintf(objectFile, "#%u\n", obj->objNumber);

 // next, the object keyword list

  createKeywordString(obj->keywordListHead, strn);
  lowstrn(strn);
  strcat(strn, "\n");

  fputs(strn, objectFile);

 // next, the short desc of the object

  strcpy(strn, obj->objShortName);
  strcat(strn, "~\n");

  fputs(strn, objectFile);

 // next, the long desc of the object

  strcpy(strn, obj->objLongName);
  strcat(strn, "~\n");

  fputs(strn, objectFile);

 // add another line with nothing but a tilde cuz it must be there

  fputs("~\n", objectFile);

 // next, write the type, material, size, space, extra flag, wear flag,
 // extra2 flag, anti flag and anti2 flag

  fprintf(objectFile, "%u %u %u %u " "%u %u " "%u %u %u %u %u\n",
                                    obj->objType,
                                    obj->material,
                                    obj->size,
                                    obj->space,

                                    obj->craftsmanship,
                                    obj->damResistBonus,

                                    obj->extraBits.longIntFlags,
                                    obj->wearBits.longIntFlags,
                                    obj->extra2Bits.longIntFlags,
                                    obj->antiBits.longIntFlags,
                                    obj->anti2Bits.longIntFlags);

 // next, the "values"

  fprintf(objectFile, "%d %d %d %d %d %d %d %d\n",
          obj->objValues[0], obj->objValues[1],
          obj->objValues[2], obj->objValues[3],
          obj->objValues[4], obj->objValues[5],
          obj->objValues[6], obj->objValues[7]);

 // next, the weight, value, and cost/day - if aff1 and/or aff2 flags are
 // non-zero, write those too

  if (obj->affect1Bits.longIntFlags || obj->affect2Bits.longIntFlags ||
      obj->affect3Bits.longIntFlags || obj->affect4Bits.longIntFlags)
  {
    if( obj->maxload > 0 )
    {
      fprintf(objectFile, "%d %u %d %u %u %u %u %d\n",
            obj->weight, obj->value, obj->condition,
            obj->affect1Bits.longIntFlags, obj->affect2Bits.longIntFlags,
            obj->affect3Bits.longIntFlags, obj->affect4Bits.longIntFlags,
            obj->maxload );
    }
    else
    {
      fprintf(objectFile, "%d %u %d %u %u %u %u\n",
            obj->weight, obj->value, obj->condition,
            obj->affect1Bits.longIntFlags, obj->affect2Bits.longIntFlags,
            obj->affect3Bits.longIntFlags, obj->affect4Bits.longIntFlags);
    }
  }
  else if( obj->maxload > 0 )
  {
    fprintf(objectFile, "%d %u %d %d\n", obj->weight,
                                         obj->value,
                                         obj->condition,
                                         obj->maxload );
  }
  else
  {
    fprintf(objectFile, "%d %u %d\n", obj->weight,
                                      obj->value,
                                      obj->condition);
  }

 // write all the extra descs

  descNode = obj->extraDescHead;

  while (descNode)
  {
    fputs("E\n", objectFile);

   // write the keyword list

    createKeywordString(descNode->keywordListHead, strn);

    strcat(strn, "\n");

    fputs(strn, objectFile);

   // next, the extra description itself

    writeStringNodes(objectFile, descNode->extraDescStrnHead);

   // terminate the description

    fputs("~\n", objectFile);

    descNode = descNode->Next;
  }

 // write the object apply info

  for (i = 0; i < NUMB_OBJ_APPLIES; i++)
  {
    if (obj->objApply[i].applyWhere)
    {
      fprintf(objectFile, "A\n%u %d\n", obj->objApply[i].applyWhere,
                                        obj->objApply[i].applyModifier);
    }
  }

 // write trap info, if any

  if (obj->trapAff)
  {
    fprintf(objectFile, "T\n%u %d %d %u %d\n",
            obj->trapAff, obj->trapDam, obj->trapCharge, obj->trapLevel,
            obj->trapPercent );
  }
}


//
// writeObjectFile : Write the object file - contains all the objects
//

void writeObjectFile(const char *filename /*= NULL*/)
{
  FILE *objectFile;
  char objectFilename[81] = "\0";
  char strn[256];
  dikuObject *obj = objectHead;

  ulong lowest = getLowestObjNumber(), highest = getHighestObjNumber(),
        numb = lowest;


 // assemble the filename of the object file

  if (readFromSubdirs) strcpy(objectFilename, "obj/");

  if (filename) strcat(objectFilename, filename);
  else strcat(objectFilename, getMainZoneNameStrn());

  strcat(objectFilename, ".obj");


 // open the object file for writing

  if ((objectFile = fopen(objectFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << objectFilename << " for writing - aborting\n";

    return;
  }

  sprintf(strn, "Writing %s - %u object type%s\n",
          objectFilename, numbObjTypes, plural(numbObjTypes));

  _outtext(strn);

  while (numb <= highest)
  {
    obj = findObj(numb);

    if (obj) writeObjecttoFile(objectFile, obj);

    numb++;
  }

  fclose(objectFile);
}
