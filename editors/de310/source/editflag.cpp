//
//  File: editflag.cpp   originally part of dikuEdit
//
//  Usage: generalized functions for editing bit values in 32-bit
//         bitvectors
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdlib.h>
#include <ctype.h>

#include "fh.h"
#include "de.h"
#include "types.h"
#include "keys.h"
#include "const.h"
#include "misc/menu.h"
#include "dispflag.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "graphcon.h"
#endif

#include "flagdef.h"

extern _CONST usint templateKeys[], setTemplateKeys[];
extern char madeChanges;

//
// canEditFlag
//

INLINEDEF char canEditFlag(const flagDef &flagInf, long flagState)
{
  if (flagState) flagState = 1;  // makes things easier to check below

  if (flagInf.editable || getEditUneditableFlagsVal() ||
      (flagState != flagInf.defVal)) return TRUE;

  return FALSE;
}


//
// isFlagValid
//

INLINEDEF char isFlagValid(const flagDef &flagInf, ulong flagState)
{
  if (getEditUneditableFlagsVal()) return TRUE;

  if (!flagInf.editable)
  {
    if (flagState) flagState = 1;  // makes things easier to check below

    if (flagState == flagInf.defVal) return TRUE;
    else return FALSE;
  }
  else return TRUE;
}


// all the following functions have to be doubled up since occasionally we
// pass object value fields, which are signed longs (versus unsigned for
// everything else)..  I suppose I could typecast, but that's just evil.
// gotta be an easier way.  bah.

// templates would work, too, but ..  nah

//
// checkCommonFlagKeys : returns TRUE if flags have been changed
//
//               ch : key entered by user
//
//       *flagValue : pointer to flag location
//        numbFlags : number of valid flags
//
//     headerOffset : y-offset due to menu heading
//
//              row : row input prompt is on
//

uchar checkCommonFlagKeys(const usint ch, const flagDef *flagArr,
                          ulong *flagValue, const uchar numbFlags,
                          const uchar *colXpos, const uchar *colTopFlag,
                          const uchar numbCols, const uchar headerOffset,
                          const uchar row)
{
  char strn[256];
  ulong i, strnVal, val;


 // enter flag value directly

  if ((ch == ';') || (ch == ':') || (ch == K_Ctrl_D))
  {
    _settextposition(row, 1);
    clrline(row);

    displayColorString("&+CEnter new flags value: &n");

    utoa(*flagValue, strn, 10);

    getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

    if (strnumer(strn))
    {
     // don't allow user to change uneditable bits via value entry

      strnVal = strtoul(strn, NULL, 10);

      for (i = 0; i < numbFlags; i++)
      {
        val = strnVal & (1 << i);

        if (!isFlagValid(flagArr[i + 1], val))
        {
          strnVal ^= (1 << i);
        }
      }

      *flagValue = strnVal;

      updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                  headerOffset, TRUE);
    }

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // turn all bits on

  if ((ch == '>') || (ch == '.') || (ch == K_Ctrl_N))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i))) *flagValue |= (1 << i);
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, FALSE);  // shouldn't be any non-set..  shrug

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // turn all bits off

  if ((ch == '<') || (ch == ',') || (ch == K_Ctrl_F))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i)))
      {
        *flagValue &= ~(1 << i);
      }
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, TRUE);

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // toggle all bits (it's neato keen, the kids love it)

  if ((ch == '?') || (ch == '/') || (ch == K_Ctrl_T))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i))) *flagValue ^= (1 << i);
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, TRUE);

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else return FALSE;

  return TRUE;
}


//
// checkCommonFlagKeys : returns TRUE if flags have been changed
//
//               ch : key entered by user
//
//       *flagValue : pointer to flag location
//        numbFlags : number of valid flags
//
//     headerOffset : y-offset due to menu heading
//
//              row : row input prompt is on
//

uchar checkCommonFlagKeys(const usint ch, const flagDef *flagArr,
                          long *flagValue, const uchar numbFlags,
                          const uchar *colXpos, const uchar *colTopFlag,
                          const uchar numbCols, const uchar headerOffset,
                          const uchar row)
{
  char strn[256];
  long i, strnVal, val;


 // enter flag value directly

  if ((ch == ';') || (ch == ':') || (ch == K_Ctrl_D))
  {
    _settextposition(row, 1);
    clrline(row);

    displayColorString("&+CEnter new flags value: &n");

    utoa(*flagValue, strn, 10);

    getStrn(strn, 10, 1, 7, '°', strn, FALSE, FALSE);

    if (strnumer(strn))
    {
     // don't allow user to change uneditable bits via value entry

      strnVal = strtoul(strn, NULL, 10);

      for (i = 0; i < numbFlags; i++)
      {
        val = strnVal & (1 << i);

        if (!isFlagValid(flagArr[i + 1], val))
        {
          strnVal ^= (1 << i);
        }
      }

      *flagValue = strnVal;

      updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                  headerOffset, TRUE);
    }

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // turn all bits on

  if ((ch == '>') || (ch == '.') || (ch == K_Ctrl_N))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i))) *flagValue |= (1 << i);
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, FALSE);

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // turn all bits off

  if ((ch == '<') || (ch == ',') || (ch == K_Ctrl_F))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i)))
      {
        *flagValue &= ~(1 << i);
      }
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, TRUE);

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else

 // toggle all bits (it's neato keen, the kids love it)

  if ((ch == '?') || (ch == '/') || (ch == K_Ctrl_T))
  {
    for (i = 0; i < numbFlags; i++)
    {
      if (canEditFlag(flagArr[i + 1], *flagValue & (1 << i))) *flagValue ^= (1 << i);
    }

    updateFlags(*flagValue, numbFlags, numbCols, colXpos, colTopFlag,
                headerOffset, TRUE);

    clrline(row, 7, 0);
    _settextposition(row, 1);
    _setbkcolor(0);

    displayColorString(getMenuPromptName());
  }
  else return FALSE;

  return TRUE;
}


//
// interpEditFlags
//

char interpEditFlags(usint ch, const flagDef *flagArr, ulong *flagVal,
                     const uchar numbFlags, const uchar *colXpos,
                     const uchar *colTopFlag, const uchar numbCols,
                     ulong templates[NUMB_FLAG_TEMPLATES])
{
  struct rccoord xy = _gettextposition();
  ulong i;


  if (checkMenuKey(ch, TRUE) == MENUKEY_SAVE) return TRUE;

 // check common keys

  if (checkCommonFlagKeys(ch, flagArr, flagVal, numbFlags,
                          colXpos, colTopFlag, numbCols,
                          HEADER_OFFSETY, xy.row))
  {
    return FALSE;
  }

  if ((ch >= '1') && (ch < 'A')) ch += 42;

  if ((ch >= 'A') && (ch <= ('A' + (numbFlags - 1))))
  {
    ch -= 'A';

    if (canEditFlag(flagArr[ch + 1], *flagVal & (1 << ch)))
    {
      *flagVal ^= (1 << ch);

      updateSingleFlag(*flagVal, ch, numbFlags, numbCols, colXpos, colTopFlag,
                       HEADER_OFFSETY);
    }
  }

 // check for template key

  if (templates)
  {
    for (i = 0; i < NUMB_FLAG_TEMPLATES; i++)
    {
      if (ch == templateKeys[i])
      {
        *flagVal = templates[i];

        updateFlags(*flagVal, numbFlags, numbCols, colXpos, colTopFlag,
                    HEADER_OFFSETY, TRUE);

        break;
      }

      if (ch == setTemplateKeys[i])
      {
        templates[i] = *flagVal;
        madeChanges = TRUE;

        break;
      }
    }
  }

  return FALSE;
}


//
// interpEditFlags - signed long version
//

char interpEditFlags(usint ch, const flagDef *flagArr, long *sflagVal,
                     const uchar numbFlags, const uchar *colXpos,
                     const uchar *colTopFlag, const uchar numbCols,
                     ulong templates[NUMB_FLAG_TEMPLATES])
{
  struct rccoord xy = _gettextposition();
  ulong i;


  if (checkMenuKey(ch, TRUE) == MENUKEY_SAVE) return TRUE;

 // check common keys

  if (checkCommonFlagKeys(ch, flagArr, sflagVal, numbFlags,
                          colXpos, colTopFlag, numbCols,
                          HEADER_OFFSETY, xy.row))
  {
    return FALSE;
  }

  if ((ch >= '1') && (ch < 'A')) ch += 42;

  if ((ch >= 'A') && (ch <= ('A' + (numbFlags - 1))))
  {
    ch -= 'A';

    if (canEditFlag(flagArr[ch + 1], *sflagVal & (1 << ch)))
    {
      *sflagVal ^= (1 << ch);

      updateSingleFlag(*sflagVal, ch, numbFlags, numbCols, colXpos, colTopFlag,
                       HEADER_OFFSETY);
    }
  }

 // check for template key

  if (templates)
  {
    for (i = 0; i < NUMB_FLAG_TEMPLATES; i++)
    {
      if (ch == templateKeys[i])
      {
        *sflagVal = templates[i];

        updateFlags(*sflagVal, numbFlags, numbCols, colXpos, colTopFlag,
                    HEADER_OFFSETY, TRUE);

        break;
      }

      if (ch == setTemplateKeys[i])
      {
        templates[i] = *sflagVal;
        madeChanges = TRUE;

        break;
      }
    }
  }

  return FALSE;
}


//
// editFlags
//

void editFlags(const flagDef *flagArr, ulong *flagVal, const char *title,
               const char *flagName, ulong *templates, uchar numbCols)
{
  usint ch;
  uchar *colXpos = NULL, *colTopFlag = NULL;
  ulong oldFlags = *flagVal;


  displayFlagMenu(*flagVal, flagArr, title, flagName,
                  &colXpos, &colTopFlag, &numbCols);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, TRUE) == MENUKEY_ABORT)
    {
      *flagVal = oldFlags;

      _outtext("\n\n");

      if (colXpos) delete[] colXpos;
      if (colTopFlag) delete[] colTopFlag;

      return;
    }

    if (interpEditFlags(ch, flagArr, flagVal, flagArr[0].editable,
                        colXpos, colTopFlag, numbCols, templates))
    {
      if (colXpos) delete[] colXpos;
      if (colTopFlag) delete[] colTopFlag;

      return;
    }
  }
}


//
// editFlags - long flagVal version, for those damn obj value fields
//

void editFlags(const flagDef *flagArr, long *sflagVal, const char *title,
               const char *flagName, ulong *templates, uchar numbCols)
{
  usint ch;
  uchar *colXpos = NULL, *colTopFlag = NULL;
  long oldFlags = *sflagVal;
//  ulong *flagVal = (ulong *)sflagVal;


  if (flagArr[0].editable > 15) return;
    // can't edit more than 15 flags with a signed long

  displayFlagMenu(*sflagVal, flagArr, title, flagName,
                  &colXpos, &colTopFlag, &numbCols);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, TRUE) == MENUKEY_ABORT)
    {
      *sflagVal = oldFlags;

      _outtext("\n\n");

      if (colXpos) delete[] colXpos;
      if (colTopFlag) delete[] colTopFlag;

      return;
    }

    if (interpEditFlags(ch, flagArr, sflagVal, flagArr[0].editable,
                        colXpos, colTopFlag, numbCols, templates))
    {
      if (colXpos) delete[] colXpos;
      if (colTopFlag) delete[] colTopFlag;

      return;
    }
  }
}
