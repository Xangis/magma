//
//  File: writeset.cpp   originally part of dikuEdit
//
//  Usage: functions for writing stuff to .SET file
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../fh.h"
#include "../types.h"
#include "../de.h"
#include "set.h"
#include "../command/alias.h"
#include "../vardef.h"

using namespace std;

extern dikuRoom *currentRoom;
extern variable *varHead;
extern entityLoaded *numbLoadedHead;
extern char readFromSubdirs;
extern command *setCmdHead;
extern ulong roomFlagTemplates[], objExtraFlagTemplates[], objWearFlagTemplates[],
             objAff1FlagTemplates[], objAff2FlagTemplates[],
             objAff3FlagTemplates[], objAff4FlagTemplates[],  mobActionFlagTemplates[],
             mobAff1FlagTemplates[], mobAff2FlagTemplates[],
             mobAff3FlagTemplates[], mobAff4FlagTemplates[];
extern alias *aliasHead;

//
// writeSettingsFile
//

void writeSettingsFile(const char *filename, const char writeLimits)
{
  FILE *setFile;
  char strn[MAX_SETLINE_LEN], strn2[256], setFilename[81] = "\0";
  alias *aliasNode = aliasHead;
  variable *varNode;
  entityLoaded *loadNode = numbLoadedHead;
  ulong i;


  if (readFromSubdirs) strcpy(setFilename, "set/");
  strcat(setFilename, filename);

  if ((setFile = fopen(setFilename, "wt")) == NULL)
  {
    cout << "Couldn't open " << setFilename << " for writing, aborting..\n";

    return;
  }
  else
  {
    _outtext("Writing ");
    _outtext(setFilename);
    _outtext("...\n");
  }

 //
 // format of setfile is as follows -
 //
 // set <var> <value>   or
 // alias <alias> <aliased string>   or
 // limit <obj|mob> <vnum> <limit>
 //

  while (aliasNode)
  {
    strcpy(strn, "alias ");

    strcpy(strn2, aliasNode->aliasStrn);
    lowstrn(strn2);
    strcat(strn, strn2);

    strcat(strn, " ");

    strcpy(strn2, aliasNode->commandStrn);
    lowstrn(strn2);
    strcat(strn, strn2);

    strcat(strn, "\n");

    fputs(strn, setFile);

    aliasNode = aliasNode->Next;
  }

 // write currentRoom vnum variable thingy

  if ((getStartRoomActiveVal()) && (currentRoom->roomNumber))
  {
//    sprintf(strn, "set startroom %u\n", currentRoom->roomNumber);
    itoa(currentRoom->roomNumber, strn, 10);
    addVar(&varHead, VAR_STARTROOM_NAME, strn);

//    fputs(strn, setFile);
  }

  varNode = varHead;

  while (varNode)
  {
    strcpy(strn, "set ");

    strcpy(strn2, varNode->varName);
    lowstrn(strn2);
    strcat(strn, strn2);

    strcat(strn, " ");

    strcpy(strn2, varNode->varValue);
//    lowstrn(strn2);
    strcat(strn, strn2);

    strcat(strn, "\n");

    fputs(strn, setFile);

    varNode = varNode->Next;
  }

  if (writeLimits)
  {
    while (loadNode)
    {
      loadNode = getFirstEntityOverride(loadNode);
      if (!loadNode) break;

      strcpy(strn, "limit ");

      switch (loadNode->entityType)
      {
        case ENTITY_OBJECT : strcat(strn, "object "); break;
        case ENTITY_MOB    : strcat(strn, "mob "); break;

        default : strcat(strn, "invalid_entity_type ");
      }

      itoa(loadNode->entityNumb, strn2, 10);
      strcat(strn, strn2);

      strcat(strn, " ");

      itoa(loadNode->overrideLoaded, strn2, 10);
      strcat(strn, strn2);

      strcat(strn, "\n");

      fputs(strn, setFile);

      loadNode = loadNode->Next;
    }
  }

  for (i = 0; i < NUMB_FLAG_TEMPLATES; i++)
  {
    if (roomFlagTemplates[i])
    {
      sprintf(strn, "settemp roomflag %u %u\n", i, roomFlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objExtraFlagTemplates[i])
    {
      sprintf(strn, "settemp objextra %u %u\n", i, objExtraFlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objWearFlagTemplates[i])
    {
      sprintf(strn, "settemp objwear %u %u\n", i, objWearFlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objAff1FlagTemplates[i])
    {
      sprintf(strn, "settemp objaff1 %u %u\n", i, objAff1FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objAff2FlagTemplates[i])
    {
      sprintf(strn, "settemp objaff2 %u %u\n", i, objAff2FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objAff3FlagTemplates[i])
    {
      sprintf(strn, "settemp objaff3 %u %u\n", i, objAff3FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (objAff4FlagTemplates[i])
    {
      sprintf(strn, "settemp objaff4 %u %u\n", i, objAff4FlagTemplates[i]);
      fputs(strn, setFile);
    }


    if (mobActionFlagTemplates[i])
    {
      sprintf(strn, "settemp mobact %u %u\n", i, mobActionFlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (mobAff1FlagTemplates[i])
    {
      sprintf(strn, "settemp mobaff1 %u %u\n", i, mobAff1FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (mobAff2FlagTemplates[i])
    {
      sprintf(strn, "settemp mobaff2 %u %u\n", i, mobAff2FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (mobAff3FlagTemplates[i])
    {
      sprintf(strn, "settemp mobaff3 %u %u\n", i, mobAff3FlagTemplates[i]);
      fputs(strn, setFile);
    }

    if (mobAff4FlagTemplates[i])
    {
      sprintf(strn, "settemp mobaff4 %u %u\n", i, mobAff4FlagTemplates[i]);
      fputs(strn, setFile);
    }
  }

  fclose(setFile);
}

//
//  File: readset.cpp    originally part of dikuEdit
//
//  Usage: functions for reading .SET file
//
//  Copyright 1995-98 (C) Michael Glosenger
//
//
// readSettingsFile : reads the .SET file specified by filename (subdir
//                    is stuck at start of filename, no extension is added,
//                    though) line-by-line until eof
//
//   filename : filename to read (with extension)
//

void readSettingsFile(const char *filename)
{
  FILE *setFile;
  char strn[MAX_SETLINE_LEN], strn2[256], setFilename[81] = "\0";
  ulong line = 1;


  if (readFromSubdirs) strcpy(setFilename, "set/");
  strcat(setFilename, filename);

  if ((setFile = fopen(setFilename, "rt")) == NULL)
  {
    cout << "Couldn't open " << setFilename << ", skipping\n";

    return;
  }
  else
  {
//    _outtext("Reading ");
//    _outtext(filename);
//    _outtext("...\n");
    cout << "Reading " << setFilename << "...\n";
  }

 //
 // format of setfile is as follows -
 //
 // set <var> <value>   or
 // alias <alias> <aliased string>   or
 // limit <obj|mob> <vnum> <limit>   or
 // random <obj|mob> <vnum> <room numb> <random val> (not imped) or
 // settemplate <i forget>
 //

  while (!feof(setFile))
  {
    if (!fgets(strn, MAX_SETLINE_LEN, setFile)) break;

    nolf(strn);
    remLeadingSpaces(strn);
    upfirstarg(strn);

    sprintf(strn2,
"Error in line %u - specify one of SET, ALIAS, LIMIT, or SETTEMPLATE\n",
            line);

    checkCommands(strn, setCmdHead, strn2, setExecCommandFile, TRUE);

    line++;
  }

  fclose(setFile);
}
