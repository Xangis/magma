//
//  File: getstrn.cpp    originally part of dikuEdit
//
//  Usage: getString function and supporting
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
//#include <ncurses.h>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "getstrn.h"

extern stringNode *commandHistory;

//
// dispGetStrnField : Displays the visible part of getStrn field, checking
//                    various shit
//
//             strn : string
//         startPos : position in strn to start displaying at
//
//           maxLen : width of getStrn input field - well uhh, sort of
//       wideString : set to true if maxLen > maxDisplayable (it's handy)
//   maxDisplayable : maximum displayable in field onscreen
//
//                x : x-coordinate where field starts
//                y : y-coordinate
//

INLINEDEF void dispGetStrnField(
                      const char *strn, const ulong startPos, const ulong maxLen,
                      const char wideString, const ulong maxDisplayable,
                      const char x, const char y, const char fillEmpty,
                      const char *fillerChStrn)
{
  ulong i, subpos = 0;
  char *substrn;


  substrn = new char[maxLen];
  if (!substrn)
  {
    _outtext("\n\ndispGetStrnField(): couldn't alloc substrn\n\n");
    return;
  }

  _settextposition(y, x);

  if (wideString)
  {
    for (i = startPos; (i < (startPos + maxDisplayable)) && (i < maxLen); i++)
    {
      substrn[subpos++] = strn[i];
    }

    substrn[subpos] = '\0';

    _outtext(substrn);

    if (fillEmpty)// && (fillerChStrn[0] != ' '))
    {
      for (i = strlen(substrn); i < maxDisplayable; i++)
      {
        _outtext((char *)fillerChStrn);
      }
    }
  }
  else
  {
    _outtext((char *)strn);

    if (fillEmpty)// && (fillerChStrn[0] != ' '))
    {
      for (i = strlen(strn); i < maxLen; i++)
      {
        _outtext((char *)fillerChStrn);
      }
    }
  }

  delete[] substrn;
}


//
// getStrn : Gets a string from the user - spaces from cursor pos to
//           cursor pos + maxLen get filled with bgcol-colored fillerCh.
//           Text is drawn in fgcol, bgcol.  oldStrn is put into input string
//           if it is passed to the function.  lalala.
//
//     *strn : string that contains the string user entered
//    maxLen : maximum length of string
//
//     bgcol : background color
//     fgcol : foreground color
//
//  fillerCh : character used for "empty field"
//  *oldStrn : old string - if a non-null string, is in field at start
//
//  addToCommandHist :
//             if TRUE, adds the string entered to the global history
//             list
//
//    prompt : if TRUE, the escape key is treated differently - it clears the
//             input field like Ctrl-Y instead of returning oldStrn
//

char *getStrn(char *strn, const ulong maxLen, const char bgcol,
              const char fgcol, const uchar fillerCh, const char *oldStrn,
              const char addToCommandHist, const char prompt)
{
  ulong strnPos = 0;    // points to where the next character
                        // to be typed should go

  ulong endofStrn = 0;  // points to where the null character
                        // should be placed

  ulong startPos = 0;   // position in editStrn that field
                        // starts at onscreen - used for
                        // fields that are wider than one
                        // line can fit

  char editStrn[MAX_GETSTRN_LEN];
                        // string that is manipulated as user types
  char oneCharStrn[2];  // used when one character has to be output, cuz
                        // outtext only takes strings
  char fillerChStrn[2]; // ditto, only this is for the "empty" parts of the
                        // input field

  usint ch;      // used for character input
  ulong i,       // counter, etc.
        oldLen = strlen(oldStrn);
                 // golly

  struct rccoord xy = _gettextposition();
  usint startx = xy.col, starty = xy.row,
                                // used to store start position of field
       firstHistMod = TRUE;     // I forget what this is for, but it's a
                                // very good variable.  enjoy it while it
                                // lasts.

  ulong maxDisplayable = getScreenWidth() - startx;
     // 80th column shouldn't be filled - at any rate, this variable
     // stores the number of characters displayable on the screen at once

  char wideString = (maxLen > maxDisplayable);
     // if the max string length is longer than is displayable on the screen
     // at once, this is TRUE

  stringNode *commandHistPos = commandHistory;
     // I don't remember, but it stores something handy



  if (oldLen > maxLen) return "ERR1";
  if (maxLen >= MAX_GETSTRN_LEN) return "ERR2";

 // preset stuff

  oneCharStrn[1] = '\0';

  fillerChStrn[0] = fillerCh;
  fillerChStrn[1] = '\0';

 // get to end of command history

  while (commandHistPos)
  {
    if (!commandHistPos->Next) break;

    commandHistPos = commandHistPos->Next;
  }

 // copy oldStrn into editStrn

  memset(editStrn, 0, maxLen * sizeof(char));
  strcpy(editStrn, oldStrn);
//  strnPos = endofStrn = strlen(editStrn);
//  strnPos = endofStrn = oldLen;

 // set strnPos and endofStrn

  if (wideString && (oldLen > maxDisplayable))
  {
//    strnPos = maxDisplayable;
//    strnPos = oldLen;
//    endofStrn = oldLen;
    startPos = oldLen - maxDisplayable;
  }
//  else strnPos = endofStrn = oldLen;
  strnPos = endofStrn = oldLen;

//  memset(strn, 0, maxLen * sizeof(char));

 // copy editStrn into strn

//  strcpy(strn, editStrn);

 // set colors

  _settextcolor(fgcol);
  _setbkcolor(bgcol);

 // display editStrn

//  outtext(oldStrn);
//  outtext(editStrn);
  dispGetStrnField(editStrn, startPos, maxLen, wideString, maxDisplayable,
                   startx, starty, TRUE, fillerChStrn);

 // draw the empty space stuff after the old string

/*
  if (wideString)
  {
    for (i = oldLen; i < maxDisplayable; i++)
    {
      outtext(fillerChStrn);
    }
  }
  else
  {
    for (i = oldLen; i < maxLen; i++)
    {
      outtext(fillerChStrn);
    }
  }
*/

 // reset the text position to be right after the old string

  _settextposition(starty, startx + strnPos);

 // now, the input stuff

 // the first character entered has significance because ...  well just because
 // so treat it differently

  do
  {
    ch = getkey();
  } while ((ch != K_Escape) && (ch != K_Return) && (ch != K_LeftArrow)
        && (ch != K_RightArrow) && (ch != K_End) && (ch != K_Home)
        && ((ch < K_FirstPrint) || (ch > K_LastPrint))
        && (ch != K_Backspace) && (ch != K_Tab) && (ch != K_Ctrl_Y)
        && (ch != K_Delete) && ((ch != K_UpArrow) && (addToCommandHist))
        && ((ch != K_DownArrow) && (addToCommandHist)));

 // user hit escape or return ..

  if (((ch == K_Escape) && !prompt) || (ch == K_Return))
  {
    strcpy(strn, editStrn);

    if (addToCommandHist) addCommand(editStrn, &commandHistory);
    return strn;
  }
  else
  if ((ch >= K_FirstPrint) && (ch <= K_LastPrint))
  {
    editStrn[0] = ch;
    editStrn[1] = '\0';
    strnPos = endofStrn = 1;
    startPos = 0;

    _settextposition(starty, startx);
    _outtext(editStrn);

    if (wideString)
    {
      for (i = 1; i < maxDisplayable; i++)
      {
       _outtext(fillerChStrn);
      }
    }
    else
    {
      for (i = 1; i < oldLen; i++)  // saves time..
      {
        _outtext(fillerChStrn);
      }
    }
/*
    for (i = 1; i < oldLen; i++)
    {
      _outtext(fillerChStrn);
    }
*/

    _settextposition(starty, startx + 1);
  }
  else
  if ((ch == K_Backspace) || (ch == K_Ctrl_Y) || (ch == K_Delete) ||
      ((ch == K_Escape) && prompt))
  {
    editStrn[0] = '\0';
    strnPos = endofStrn = 0;

    _settextposition(starty, startx);

    dispGetStrnField(editStrn, startPos, maxLen, wideString,
                     maxDisplayable, startx, starty, TRUE, fillerChStrn);
/*
    if (wideString)
    {
      for (i = 0; i < maxDisplayable; i++)
      {
        _outtext(fillerChStrn);
      }
    }
    else
    {
      for (i = 0; i < maxLen; i++)
      {
        _outtext(fillerChStrn);
      }
    }
*/

    _settextposition(starty, startx);
  }
  else
  if ((ch == K_LeftArrow) && (strnPos))
  {
    strnPos--;

    _settextposition(starty, startx + (strnPos - startPos));
  }
  else
/*
  if ((ch == K_RightArrow) && (strnPos))
  {
    strnPos--;

    _settextposition(starty, startx + strnPos);
  }
  else
*/
/*
  if (ch == K_End)  // no action needs to be taken
  {
  }
  else
*/
  if (ch == K_Home)
  {
    strnPos = startPos = 0;

    dispGetStrnField(editStrn, startPos, maxLen, wideString,
                     maxDisplayable, startx, starty, FALSE, fillerChStrn);

    _settextposition(starty, startx);
  }
  else
  if ((ch == K_UpArrow) && addToCommandHist && commandHistPos &&
      ((commandHistPos->Last) || firstHistMod))
  {
    if (!firstHistMod) commandHistPos = commandHistPos->Last;
    else firstHistMod = FALSE;

    if (commandHistPos)
    {
      strcpy(editStrn, commandHistPos->string);
      strnPos = endofStrn = strlen(editStrn);

      if (endofStrn > maxDisplayable) startPos = endofStrn - maxDisplayable;
      else startPos = 0;

      dispGetStrnField(editStrn, startPos, maxLen, wideString, maxDisplayable,
                       startx, starty, TRUE, fillerChStrn);

      _settextposition(starty, startx + (strnPos - startPos));
/*
      _settextposition(starty, startx);

      for (i = 0; i < maxLen; i++)
      {
        _outtext(fillerChStrn);
      }

      _settextposition(starty, startx);

      _outtext(editStrn);
*/
    }
  }
/*
  else
  if ((ch == K_DownArrow) && (addToCommandHist))  // do nothing
  {
  }
*/

  while (TRUE)
  {
   // get a character from the user

    ch = getkey();

    if ((endofStrn < maxLen) && (ch >= K_FirstPrint) && (ch <= K_LastPrint))
    {
      if (strnPos != endofStrn)
      {
        insertChar(editStrn, strnPos, ch);
      }
      else
      {
        editStrn[strnPos] = ch;
      }

      if (wideString)
      {
//        if ((strnPos - startPos) >= maxDisplayable) startPos++;
//        if (strnPos == endofStrn) startPos++;
        if ((strnPos - startPos) >= maxDisplayable) startPos++;
      }

      strnPos++;
      endofStrn++;
      editStrn[endofStrn] = '\0';

      if (wideString)
      {
        dispGetStrnField(editStrn, startPos, maxLen, wideString,
                         maxDisplayable, startx, starty, FALSE, fillerChStrn);
      }
      else
      {
        _settextposition(starty, startx);
        _outtext(editStrn);
      }

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if (ch == K_Tab)
    {
    }
    else
    if ((ch == K_Ctrl_Y) || ((ch == K_Escape) && prompt))
    {
      strcpy(editStrn, "\0");
      strnPos = endofStrn = startPos = 0;

//      _settextposition(starty, startx);

      dispGetStrnField(editStrn, startPos, maxLen, wideString,
                       maxDisplayable, startx, starty, TRUE, fillerChStrn);
/*
      for (i = 0; i < maxLen; i++)
      {
        _outtext(fillerChStrn);
      }
*/

      _settextposition(starty, startx);
    }
    else

   // user hit escape, input field isn't prompt, so restore the field to the
   // previous

    if ((ch == K_Escape) && !prompt)
    {
      _settextposition(starty, startx);

      strcpy(editStrn, oldStrn);
      strcpy(strn, editStrn);

      startPos = 0;


      dispGetStrnField(editStrn, startPos, maxLen, wideString,
                       maxDisplayable, startx, starty, TRUE, " ");
/*
      _outtext((char *)oldStrn);

      for (i = oldLen; i < maxLen; i++)
      {
        _outtext(" ");  // #32
      }
*/

      if (addToCommandHist) addCommand(editStrn, &commandHistory);
      return strn;
    }
    else
    if (ch == K_Return)
    {
      editStrn[endofStrn] = '\0';

      dispGetStrnField(editStrn, startPos, maxLen, wideString,
                       maxDisplayable, startx, starty, TRUE, " ");
/*
      _settextposition(starty, startx + endofStrn);

      if (wideString)
      {
        for (i = endofStrn; i < maxDisplayable; i++)
        {
          _outtext(" ");  // #32
        }
      }
      else
      {
        for (i = endofStrn; i < maxLen; i++)
        {
          _outtext(" ");  // #32
        }
      }
*/

      strcpy(strn, editStrn);

      if (addToCommandHist) addCommand(editStrn, &commandHistory);
      return strn;
    }
    else
    if ((ch == K_Backspace) && (strnPos > 0))
    {
      deleteChar(editStrn, strnPos - 1);
      if (wideString && (strnPos == startPos)) startPos--;
      strnPos--;
      endofStrn--;
      editStrn[endofStrn] = '\0';

      _settextposition(starty, startx);

      if (wideString)
      {
        dispGetStrnField(editStrn, startPos, maxLen, wideString,
                         maxDisplayable, startx, starty, TRUE, fillerChStrn);
      }
      else
      {
        _settextposition(starty, startx + strnPos);
        for (i = strnPos; i < endofStrn; i++)
        {
          oneCharStrn[0] = editStrn[i];
          _outtext(oneCharStrn);
        }

        _outtext(fillerChStrn);
      }
//      _outtext(editStrn);
//      _outtext(fillerChStrn);

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if ((ch == K_LeftArrow) && strnPos)
    {
      if (wideString && (strnPos == startPos))
      {
        startPos--;

        dispGetStrnField(editStrn, startPos, maxLen, wideString,
                         maxDisplayable, startx, starty, FALSE, fillerChStrn);
      }

      strnPos--;

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if ((ch == K_RightArrow) && (strnPos != endofStrn))
    {
     // if field is wider than screen, check to see if we're at the edge of
     // the field - need to scroll it over

     // bah, this is fucked somewhere - if the character right before the
     // last is the last in the field, hitting right arrow at the end of
     // the field won't advance it..  blah

      if (wideString)
      {
        if (((strnPos - startPos) > maxDisplayable) &&
            ((startPos + strnPos) < maxLen))
        {
          startPos++;

          dispGetStrnField(editStrn, startPos, maxLen, wideString,
                           maxDisplayable, startx, starty, FALSE,
                           fillerChStrn);
        }
      }

      strnPos++;

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if ((ch == K_Delete) && (strnPos != endofStrn))
    {
      deleteChar(editStrn, strnPos);
      endofStrn--;
      editStrn[endofStrn] = '\0';

      if (wideString)
      {
        dispGetStrnField(editStrn, startPos, maxLen, wideString,
                         maxDisplayable, startx, starty, TRUE,
                         fillerChStrn);
      }
      else
      {
        _settextposition(starty, startx + strnPos);
        for (i = strnPos; i < endofStrn; i++)
        {
          oneCharStrn[0] = editStrn[i];
          _outtext(oneCharStrn);
        }

        _outtext(fillerChStrn);
      }

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if (ch == K_End)
    {
      if (wideString && (endofStrn > maxDisplayable))
      {
        if ((endofStrn - maxDisplayable) > 0)
          startPos = endofStrn - maxDisplayable;
/*
        else
          startPos = 0;
*/
      }
      else startPos = 0;

      strnPos = endofStrn;

      dispGetStrnField(editStrn, startPos, maxLen, wideString,
                       maxDisplayable, startx, starty, TRUE, fillerChStrn);

      _settextposition(starty, startx + (strnPos - startPos));
    }
    else
    if (ch == K_Home)
    {
      strnPos = startPos = 0;

      dispGetStrnField(editStrn, startPos, maxLen, wideString,
                       maxDisplayable, startx, starty, TRUE, fillerChStrn);

      _settextposition(starty, startx);
    }
    else
    if ((ch == K_UpArrow) && addToCommandHist && commandHistPos &&
        (commandHistPos->Last || firstHistMod))
    {
      if (!firstHistMod) commandHistPos = commandHistPos->Last;
      else firstHistMod = FALSE;

      if (commandHistPos)
      {
        strcpy(editStrn, commandHistPos->string);
        strnPos = endofStrn = strlen(editStrn);

        if (endofStrn > maxDisplayable) startPos = endofStrn - maxDisplayable;
        else startPos = 0;

        dispGetStrnField(editStrn, startPos, maxLen, wideString, maxDisplayable,
                         startx, starty, TRUE, fillerChStrn);

        _settextposition(starty, startx + (strnPos - startPos));
/*
        _settextposition(starty, startx);

        for (i = 0; i < maxLen; i++)
        {
          _outtext(fillerChStrn);
        }

        _settextposition(starty, startx);

        _outtext(editStrn);
*/
      }
    }
    else
    if ((ch == K_DownArrow) && addToCommandHist && commandHistPos &&
        commandHistPos->Next)
    {
      commandHistPos = commandHistPos->Next;

      strcpy(editStrn, commandHistPos->string);
      strnPos = endofStrn = strlen(editStrn);

      if (endofStrn > maxDisplayable) startPos = endofStrn - maxDisplayable;
      else startPos = 0;

      dispGetStrnField(editStrn, startPos, maxLen, wideString, maxDisplayable,
                       startx, starty, TRUE, fillerChStrn);

      _settextposition(starty, startx + (strnPos - startPos));
/*
      _settextposition(starty, startx);

      for (i = 0; i < maxLen; i++)
      {
        _outtext(fillerChStrn);
      }

      _settextposition(starty, startx);

      _outtext(editStrn);
*/
    }
  }
}
