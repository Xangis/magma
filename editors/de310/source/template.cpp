//
//  File: template.cpp   originally part of dikuEdit
//
//  Usage: template variable declarations and setting function
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  ifdef _WIN32
#    include <mapiutil.h>
#    include <mapiwin.h>
#    include <wincon.h>
#  endif

#  include "graphcon.h"
#endif

#include "types.h"
#include "boolean.h"
#include "const.h"
#include "fh.h"
#include "de.h"
#include "keys.h"

extern char madeChanges;

// template variables

ulong roomFlagTemplates[NUMB_FLAG_TEMPLATES];

ulong objExtraFlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objWearFlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objExtra2FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAntiFlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAnti2FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAff1FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAff2FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAff3FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong objAff4FlagTemplates[NUMB_FLAG_TEMPLATES];

ulong mobActionFlagTemplates[NUMB_FLAG_TEMPLATES];
ulong mobAff1FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong mobAff2FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong mobAff3FlagTemplates[NUMB_FLAG_TEMPLATES];
ulong mobAff4FlagTemplates[NUMB_FLAG_TEMPLATES];

// different keys for templates depending on OS

#ifndef __UNIX__
_CONST usint templateKeys[NUMB_FLAG_TEMPLATES]    =
                            { K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7 };
_CONST usint setTemplateKeys[NUMB_FLAG_TEMPLATES] =
                                             { K_Shift_F1, K_Shift_F2,
                                               K_Shift_F3, K_Shift_F4,
                                               K_Shift_F5, K_Shift_F6,
                                               K_Shift_F7 };
#else

// ugly any-terminal compatible keys..  bah

_CONST usint templateKeys[NUMB_FLAG_TEMPLATES]    =
      { '!', '@', '#', '$', '%', '^', '&' };
_CONST usint setTemplateKeys[NUMB_FLAG_TEMPLATES] =
      { '*', '(', ')', '_', '+', '{', '}' };
#endif


//
// setTemplateArgs : Sets a flag template based on user args
//

void setTemplateArgs(const char *args, const char updateChanges,
                     const char displayStuff)
{
  ulong i, tempNumb, value, numblines = 1;
  char dummy[256], flagname[256], strn[256], foundTemplate = FALSE;
  usint ch;


  if ((!strlen(args)) && (displayStuff)) // display template settings
  {
    _outtext("\n\n");

    for (i = 0; i < NUMB_FLAG_TEMPLATES; i++)
    {
      if (roomFlagTemplates[i])
      {
        sprintf(strn, "template #%u - roomflag = %u\n", i,
                roomFlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (objExtraFlagTemplates[i])
      {
        sprintf(strn, "template #%u - objextra = %u\n", i,
                objExtraFlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (objWearFlagTemplates[i])
      {
        sprintf(strn, "template #%u - objwear  = %u\n", i,
                objWearFlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (objAff1FlagTemplates[i])
      {
        sprintf(strn, "template #%u - objaff1  = %u\n", i,
                objAff1FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (objAff2FlagTemplates[i])
      {
        sprintf(strn, "template #%u - objaff2  = %u\n", i,
                objAff2FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }


      if (objAff3FlagTemplates[i])
      {
        sprintf(strn, "template #%u - objaff3  = %u\n", i,
                objAff3FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }


      if (objAff4FlagTemplates[i])
      {
        sprintf(strn, "template #%u - objaff4  = %u\n", i,
                objAff4FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }


      if (mobActionFlagTemplates[i])
      {
        sprintf(strn, "template #%u - mobact   = %u\n", i,
                mobActionFlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (mobAff1FlagTemplates[i])
      {
        sprintf(strn, "template #%u - mobaff1  = %u\n", i,
                mobAff1FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (mobAff2FlagTemplates[i])
      {
        sprintf(strn, "template #%u - mobaff2  = %u\n", i,
                mobAff2FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (mobAff3FlagTemplates[i])
      {
        sprintf(strn, "template #%u - mobaff3  = %u\n", i,
                mobAff3FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }

      if (mobAff4FlagTemplates[i])
      {
        sprintf(strn, "template #%u - mobaff4  = %u\n", i,
                mobAff4FlagTemplates[i]);
        _outtext(strn);
        numblines += numbLinesStringNeeds(strn);
        foundTemplate = TRUE;
      }

      if (numblines >= (getScreenHeight() - 3))
      {
        displayColorString("\n&+CPress any key to continue or Q to quit..&n");

        ch = toupper(getkey());

        _outtext("\n");

        if (ch == 'Q') return;
      }
    }

    if (!foundTemplate) _outtext("No templates are defined.\n");
    _outtext("\n");
    return;
  }

  if (numbArgs(args) != 3)
  {
    _outtext("\n"
"Format of 'settemplate' command is 'settemp <flagname> <temp #> <value>'.\n\n");
    return;
  }

  sscanf(args, "%s%u%u", flagname, &tempNumb, &value);

  if (tempNumb >= NUMB_FLAG_TEMPLATES)
  {
    _outtext("\n"
"There are only five flag templates available, numbered from 0-4.\n\n");
    return;
  }

 // make sure flagname is a valid flag name

  upstrn(flagname);

  if (!strcmp(flagname, "ROOMFLAG"))
  {
    roomFlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJEXTRA"))
  {
    objExtraFlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJWEAR"))
  {
    objWearFlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJAFF1"))
  {
    objAff1FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJAFF2"))
  {
    objAff2FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJAFF3"))
  {
    objAff3FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "OBJAFF4"))
  {
    objAff4FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "MOBACT"))
  {
    mobActionFlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "MOBAFF1"))
  {
    mobAff1FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "MOBAFF2"))
  {
    mobAff2FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "MOBAFF3"))
  {
    mobAff3FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  if (!strcmp(flagname, "MOBAFF4"))
  {
    mobAff4FlagTemplates[tempNumb] = value;

    if (displayStuff)
    {
      sprintf(dummy, "\nTemplate #%u's %s value set to %u.\n\n",
              tempNumb, flagname, value);
      _outtext(dummy);
    }
  }
  else
  {
    _outtext("\n"
"Invalid flag name specified - valid flagnames are roomflag, objextra, objwear,\n"
"objaff1, objaff2, objaff3, objaff4, mobact, mobaff1, mobaff2, mobaff3,\n"
"and mobaff4.\n\n");

    return;
  }

  if (updateChanges) madeChanges = TRUE;
}
