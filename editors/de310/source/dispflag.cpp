//
//  File: dispflag.cpp   originally part of dikuEdit
//
//  Usage: functions related to displaying a menu of flags with
//         the flags represented by a 32-bit bitvector
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <conio.h>
#else
#  ifdef _WIN32
#    include <mapiutil.h>
#    include <mapiwin.h>
#    include <wincon.h>
#  endif

#  include "graphcon.h"
#endif

#include "fh.h"
#include "dispflag.h"
#include "misc/menu.h"
#include "types.h"
#include "flagdef.h"

extern char bold;

//
// displayFlagMenu : displays a menu of flags onscreen, with corresponding
//                   keystrokes, based on information passed in flagArr
//
//         val : current value of bitvector being edited
//     flagArr : array of flagDef records, stores flag info for bitvector being
//               displayed
//
//     topLine : line to display atop the menu
//    flagName : name of flag, used but once
//
//     colXpos : variable-sized array, columns start here
//  colTopFlag : variable-sized array, dynamically created - is set
//               to which flag is at the top of each column (first flag = 0)
//
//    numbCols : number of columns to format flags in - if 0, minimum number
//               is chosen
//

void displayFlagMenu(const ulong val, const flagDef *flagArr, const char *topLine,
                     const char *flagName, uchar **colXpos, uchar **colTopFlag,
                     uchar *numbCols)
{
  long numbFlags = flagArr[0].editable, i, j, k;
  uchar *numbFlagsinCol;
  struct rccoord coord;
  char *buff = new char[1024], ch,
       ypos, lowestLine = 0;  // yeah, lowestLine is cheesy, but it works


  if (!numbFlags || !numbCols) return;  // DOH!

  _clearscreen(7,0);
  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);
  bold = FALSE;

  displayColorString(topLine);

  if (!buff)
  {
    _outtext("\ndisplayFlagMenu(): ran out of memory on buff\n\n");
    return;
  }

  if (*numbCols == 0)
  {
    if (numbFlags <= ROWS_PER_SCR) *numbCols = 1;
    else
    {
      *numbCols = numbFlags / ROWS_PER_SCR;
      if ((numbFlags % ROWS_PER_SCR) != 0) (*numbCols)++;

      if (!*numbCols)
      {
        _outtext("\ndisplayFlagMenu(): silly internal error #1\n\n");
        return;
      }
    }
  }

 // make sure numb columns isn't too big

  if (*numbCols > ((numbFlags / ROWS_PER_SCR) + 1))
    *numbCols = (numbFlags / ROWS_PER_SCR) + 1;

 // make sure numb columns isn't too small

  if ((numbFlags / *numbCols) > ROWS_PER_SCR)
    *numbCols = (numbFlags / ROWS_PER_SCR) + 1;

 // allocate and set numbFlagsinCol

  numbFlagsinCol = new uchar[*numbCols];
  if (!numbFlagsinCol)
  {
    _outtext("\ndisplayFlagMenu(): ran out of memory on numbFlagsinCol\n\n");
    return;
  }

  for (i = 0; i < *numbCols; i++)
  {
    numbFlagsinCol[i] = ROWS_PER_SCR;
  }

  numbFlagsinCol[(*numbCols) - 1] = numbFlags % ROWS_PER_SCR;

 // allocate and set colTopFlag - assumed to be created by calling func -
 // why?  better to dynamically alloc

  *colTopFlag = new uchar[*numbCols];
  if (!*colTopFlag)
  {
    _outtext("\ndisplayFlagMenu(): colTopFlag is NULL\n\n");
    return;
  }

  (*colTopFlag)[0] = 1;  // first flag contains info ..

  for (i = 1; i < *numbCols; i++)
  {
    (*colTopFlag)[i] = (*colTopFlag)[i - 1] + numbFlagsinCol[i - 1];
  }

 // let's dump out the buffer, shall we?

 // 80 column screen width is assumed

  j = (80 - 2) / *numbCols;

 // allocate and set colXpos

  *colXpos = new uchar[*numbCols];
  if (!*colXpos)
  {
    _outtext("\ndisplayFlagMenu(): ran out of memory on colXpos\n\n");
    return;
  }

  for (i = 1; i < *numbCols; i++)
  {
    (*colXpos)[i] = (j * i);
  }

  (*colXpos)[0] = 0;

  for (j = 0; j < *numbCols; j++)
  {
    if (j != (*numbCols - 1)) k = (*colTopFlag)[j + 1];
    else k = numbFlags + 1;

    for (i = (*colTopFlag)[j]; i < k; i++)
    {
      ypos = HEADER_OFFSETY + (i - (*colTopFlag)[j]);
      if (j == 0) /* first column kludge */
        ypos = ypos -1;

      _settextposition(ypos, (*colXpos)[j]);
      if (ypos > lowestLine) lowestLine = ypos;

      ch = 'A' + (i - 1);
      if (ch > 'Z') ch -= 42;

     // strangely, Linux ncurses likes the linefeed for the first
     // column, since otherwise it doesn't linefeed each flag.  go figure.

      sprintf(buff, "   %s%c&+L. &n%s\n",
              flagArr[i].editable ? "&+Y" : "&+L",
              ch, flagArr[i].flagLong);

      displayColorString(buff, FALSE);
    }
  }

 // bottom of menu stuff

  sprintf(buff,
"   &+Y;&+L.&n &+wEnter %s directly&n\n"
"\n"
MENU_COMMON
"\n"
"%s",    flagName,
         getMenuPromptName());

  _settextposition(lowestLine + 2, 1);

  displayColorString(buff, FALSE);

  coord = _gettextposition();

 // show flag settings

  updateFlags(val, numbFlags, *numbCols, *colXpos, *colTopFlag, HEADER_OFFSETY,
              FALSE);

  _settextposition(coord.row, coord.col);

 // get rid of baggage

  delete[] buff;
  delete[] numbFlagsinCol;
}


//
// updateSingleFlag : Updates single flag status indicator on-screen
//
//       flagVal : value of bitvector
//      flagNumb : number of flag to update (0 = first flag)
//     numbFlags : number of valid flags in bitvector
//
//      numbCols : number of columns displayed onscreen (> 0)
//       colXpos : x-position of each column (array with numbCols elems)
//    colTopFlag : top flag of each column (0 = first flag, array of numbCols
//                 elems)
//  headerOffset : amount to offset by rows due to header
//

void updateSingleFlag(const ulong flagVal, const uchar flagNumb,
                      const char numbFlags, const uchar numbCols,
                      const uchar *colXpos, const uchar *colTopFlag,
                      const uchar headerOffset)
{
  struct rccoord xy;
  uchar ypos;
  ulong val;
  long j;


  if (flagNumb >= numbFlags) return;

  xy = _gettextposition();

  _setbkcolor(0);

 // figure out which column this flag is in

  for (j = numbCols - 1; j >= 0; j--)
  {
    if (flagNumb >= (colTopFlag[j] - 1)) break;
  }

  if (j < 0) return;  // error

  val = flagVal & (1 << flagNumb);

  ypos = headerOffset + (flagNumb - colTopFlag[j]) + 1;

  if (j == 0)
    _settextposition(ypos, colXpos[j] + 2);
  else
    _settextposition(ypos, colXpos[j] + 1);

  if (val) displayColorString(FLAG_ACTIVE_STR, FALSE);
  else _outtext(" ");

  _settextposition(xy.row, xy.col);
}


//
// updateFlags : Updates flag status indicators on-screen
//
//       flagVal : value of flag being displayed
//     numbFlags : number of valid flags in bitvector
//
//      numbCols : number of columns displayed onscreen (> 0)
//       colXpos : x-position of each column (array with numbCols elems)
//    colTopFlag : top flag of each column (0 = first flag, array of numbCols
//                 elems)
//  headerOffset : amount to offset by rows due to header
// displayNonset : if TRUE, writes spaces over non-set flag locations
//

void updateFlags(const ulong flagVal, const char numbFlags,
                 const uchar numbCols, const uchar *colXpos,
                 const uchar *colTopFlag, const uchar headerOffset,
                 const uchar displayNonset)
{
  struct rccoord xy;
  ulong i, j, k, val;
  uchar ypos, drewone = FALSE;


//  xy = _gettextposition();

  _setbkcolor(0);

  for (j = 0; j < numbCols; j++)
  {
    if (j != (numbCols - 1)) k = colTopFlag[j + 1];
    else k = numbFlags + 1;

    for (i = colTopFlag[j]; i < k; i++)
    {
      val = flagVal & (1 << (i - 1));
      if (!val && !displayNonset) continue;

      ypos = headerOffset + (i - colTopFlag[j]);

      if (j == 0)
        _settextposition(ypos, colXpos[j] + 2);
      else
        _settextposition(ypos, colXpos[j] + 1);

      if (val) displayColorString(FLAG_ACTIVE_STR, FALSE);
      else _outtext(" ");

      drewone = TRUE;
    }
  }

//  if (drewone) _settextposition(xy.row, xy.col);
}
