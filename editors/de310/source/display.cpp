//
//  File: display.cpp    originally part of dikuEdit
//
//  Usage: functions that display text onscreen and are not easily
//         classified into another file
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#if (!defined(__UNIX__) && !defined(_WIN32))
#  include <conio.h>
#endif

#include <string.h>
#include <ctype.h>
#include <iostream>

#include "fh.h"
#include "types.h"
#include "boolean.h"
#include "zone/zone.h"
#include "obj/object.h"
#include "misc/flags.h"

using namespace std;

extern char bold, flashing, promptString[];


//
// displayColorString : Displays a string in color, changing colors as per the
//                      Diku color codes
//
//      *strn : Pointer to string to display
//

void displayColorString(const char *strn, const char checkLF /* = TRUE*/)
{
  ulong len = strlen(strn), outtextpos = 0, i;
  char *outtextstrn, tempStrn[32], valid, oldcolor, newcolor,
       oldbk, newbk, ch;


  if (!strn[0]) return;  // shrug

  outtextstrn = new char[len + 64];
  if (!outtextstrn)
  {
    _outtext("\ndisplayColorString(): ran out of memory for display\n\n");

    return;
  }

  memset(outtextstrn, 0, sizeof(char) * (len + 64));

 // run through the string

  for (i = 0; i < len; i++)
  {
    if (strn[i] == '&')
    {
      i++;

      if (toupper(strn[i]) == 'N')
      {
        outtextstrn[outtextpos] = '\0';

#ifdef _WIN32
        _outtext(outtextstrn, checkLF);
#else
        _outtext(outtextstrn);
#endif

        outtextpos = 0;

        if (getShowColorVal())
        {
          tempStrn[0] = '&';
          tempStrn[1] = strn[i];
          tempStrn[2] = '\0';

#ifdef _WIN32
          _outtext(tempStrn, checkLF);
#else
          _outtext(tempStrn);
#endif
        }

        if (getInterpColorVal())
        {
          _settextcolor(7);
          _setbkcolor(0);
        }

        i++;
        bold = FALSE;
      }
      else
      if (strn[i] == '+')
      {
        i++;

        valid = TRUE;
        oldcolor = _gettextcolor();
        oldbk = _getbkcolor();

        ch = strn[i];
        if (bold) ch = toupper(ch);

        switch (ch)
        {
          case 'l' : _settextcolor(0);  break;  // black
          case 'L' : _settextcolor(8);  break;  // dark grey
          case 'b' : _settextcolor(1);  break;  // dark blue
          case 'B' : _settextcolor(9);  break;  // purplish blue
          case 'g' : _settextcolor(2);  break;  // dark green
          case 'G' : _settextcolor(10); break;  // bright green
          case 'c' : _settextcolor(3);  break;  // light blue (cyan)
          case 'C' : _settextcolor(11); break;  // bright blue
          case 'r' : _settextcolor(4);  break;  // dark red
          case 'R' : _settextcolor(12); break;  // bright red
          case 'm' : _settextcolor(5);  break;  // light purple
          case 'M' : _settextcolor(13); break;  // bright purple (pink)
          case 'y' : _settextcolor(6);  break;  // brown
          case 'Y' : _settextcolor(14); break;  // yellow
          case 'w' : _settextcolor(7);  break;  // grey
          case 'W' : _settextcolor(15); break;  // white
          default : outtextstrn[outtextpos++] = strn[i - 2];
                    outtextstrn[outtextpos++] = strn[i - 1];
                    outtextstrn[outtextpos++] = strn[i];
                    valid = FALSE;
             // no valid color code after the &+, so back i up and set valid
             // to false
        }

        if (valid)
        {
          newcolor = _gettextcolor();
          newbk = _getbkcolor();
          _settextcolor(oldcolor);
          _setbkcolor(oldbk);

          outtextstrn[outtextpos] = '\0';

#ifdef _WIN32
          _outtext(outtextstrn, checkLF);
#else
          _outtext(outtextstrn);
#endif

          if (getShowColorVal())
          {
            tempStrn[0] = '&';
            tempStrn[1] = '+';
            tempStrn[2] = strn[i];
            tempStrn[3] = '\0';
#ifdef _WIN32
            _outtext(tempStrn, checkLF);
#else
            _outtext(tempStrn);
#endif
          }

          if (getInterpColorVal())
          {
            _settextcolor(newcolor);
            _setbkcolor(newbk);
          }

          if (isupper(strn[i])) bold = TRUE;

          outtextpos = 0;
        }

        i++;
      }
      else
      if (strn[i] == '-')
      {
        i++;

        valid = TRUE;
        oldcolor = _gettextcolor();
        oldbk = _getbkcolor();

        switch (strn[i])
        {
          case 'L' : _settextcolor(0 + 16);  // flashing
          case 'l' : _setbkcolor(0);  break;  // black
          case 'B' : _settextcolor(1 + 16);  // flashing
          case 'b' : _setbkcolor(1);  break;  // dark blue
          case 'G' : _settextcolor(2 + 16);  // flashing
          case 'g' : _setbkcolor(2);  break;  // dark green
          case 'C' : _settextcolor(3 + 16);  // flashing
          case 'c' : _setbkcolor(3);  break;  // light blue (cyan)
          case 'R' : _settextcolor(4 + 16);  // flashing
          case 'r' : _setbkcolor(4);  break;  // dark red
          case 'M' : _settextcolor(5 + 16);  // flashing
          case 'm' : _setbkcolor(5);  break;  // light purple
          case 'Y' : _settextcolor(6 + 16);  // flashing
          case 'y' : _setbkcolor(6);  break;  // brown
          case 'W' : _settextcolor(7 + 16);  // flashing
          case 'w' : _setbkcolor(7);  break;  // grey
          default : outtextstrn[outtextpos++] = strn[i - 2];
                    outtextstrn[outtextpos++] = strn[i - 1];
                    outtextstrn[outtextpos++] = strn[i];
                    valid = FALSE;
             // no valid color code after the &-, so back i up and set valid
             // to false
        }

        if (valid)
        {
          newcolor = _gettextcolor();
          newbk = _getbkcolor();
          _settextcolor(oldcolor);
          _setbkcolor(oldbk);

          outtextstrn[outtextpos] = '\0';

#ifdef _WIN32
          _outtext(outtextstrn, checkLF);
#else
          _outtext(outtextstrn);
#endif

          if (getShowColorVal())
          {
            tempStrn[0] = '&';
            tempStrn[1] = '-';
            tempStrn[2] = strn[i];
            tempStrn[3] = '\0';
#ifdef _WIN32
            _outtext(tempStrn, checkLF);
#else
            _outtext(tempStrn);
#endif
          }

          if (getInterpColorVal())
          {
            _settextcolor(newcolor);
            _setbkcolor(newbk);
          }

          outtextpos = 0;
        }

        i++;
      }
      else
      if (strn[i] == '=')
      {
        i++;

        valid = TRUE;
        oldcolor = _gettextcolor();
        oldbk = _getbkcolor();

       // background letter comes first

        switch (strn[i])
        {
          case 'L' : _settextcolor(0 + 16);  // flashing
          case 'l' : _setbkcolor(0);  break;  // black
          case 'B' : _settextcolor(1 + 16);  // flashing
          case 'b' : _setbkcolor(1);  break;  // dark blue
          case 'G' : _settextcolor(2 + 16);  // flashing
          case 'g' : _setbkcolor(2);  break;  // dark green
          case 'C' : _settextcolor(3 + 16);  // flashing
          case 'c' : _setbkcolor(3);  break;  // light blue (cyan)
          case 'R' : _settextcolor(4 + 16);  // flashing
          case 'r' : _setbkcolor(4);  break;  // dark red
          case 'M' : _settextcolor(5 + 16);  // flashing
          case 'm' : _setbkcolor(5);  break;  // light purple
          case 'Y' : _settextcolor(6 + 16);  // flashing
          case 'y' : _setbkcolor(6);  break;  // brown
          case 'W' : _settextcolor(7 + 16);  // flashing
          case 'w' : _setbkcolor(7);  break;  // grey
          default : outtextstrn[outtextpos++] = strn[i - 2];
                    outtextstrn[outtextpos++] = strn[i - 1];
                    outtextstrn[outtextpos++] = strn[i];
                    outtextstrn[outtextpos++] = strn[i + 1];
                    valid = FALSE;
             // no valid color code after the &=, so back i up and set valid
             // to false
        }

       // then foreground letter

        i++;

        if (valid)
        {
          switch (strn[i])
          {
            case 'l' : _settextcolor(0);  break;  // black
            case 'L' : _settextcolor(8);  break;  // dark grey
            case 'b' : _settextcolor(1);  break;  // dark blue
            case 'B' : _settextcolor(9);  break;  // purplish blue
            case 'g' : _settextcolor(2);  break;  // dark green
            case 'G' : _settextcolor(10); break;  // bright green
            case 'c' : _settextcolor(3);  break;  // light blue (cyan)
            case 'C' : _settextcolor(11); break;  // bright blue
            case 'r' : _settextcolor(4);  break;  // dark red
            case 'R' : _settextcolor(12); break;  // bright red
            case 'm' : _settextcolor(5);  break;  // light purple
            case 'M' : _settextcolor(13); break;  // bright purple (pink)
            case 'y' : _settextcolor(6);  break;  // brown
            case 'Y' : _settextcolor(14); break;  // yellow
            case 'w' : _settextcolor(7);  break;  // grey
            case 'W' : _settextcolor(15); break;  // white
            default : outtextstrn[outtextpos++] = strn[i - 3];
                      outtextstrn[outtextpos++] = strn[i - 2];
                      outtextstrn[outtextpos++] = strn[i - 1];
                      outtextstrn[outtextpos++] = strn[i];
                      _settextcolor(oldcolor);
                      _setbkcolor(oldbk);
                      valid = FALSE;
               // no valid color code after the &=, so back i up and set valid
               // to false
          }
        }

        if (valid)
        {
          newcolor = _gettextcolor();
          newbk = _getbkcolor();
          _settextcolor(oldcolor);
          _setbkcolor(oldbk);

          outtextstrn[outtextpos] = '\0';

#ifdef _WIN32
          _outtext(outtextstrn, checkLF);
#else
          _outtext(outtextstrn);
#endif

          if (getShowColorVal())
          {
            tempStrn[0] = '&';
            tempStrn[1] = '=';
            tempStrn[2] = strn[i];
            tempStrn[3] = strn[i - 1];
            tempStrn[4] = '\0';
#ifdef _WIN32
            _outtext(tempStrn, checkLF);
#else
            _outtext(tempStrn);
#endif
          }

          if (getInterpColorVal())
          {
            _settextcolor(newcolor);
            _setbkcolor(newbk);
          }

          outtextpos = 0;
        }

        i++;
      }

     // none matched - decrement i

      else
      {
        outtextstrn[outtextpos++] = strn[i - 1];
        valid = FALSE;
      }
    }

    if (strn[i] != '&')
    {
      outtextstrn[outtextpos] = strn[i];
      outtextpos++;
    }
    else i--;

    if (i == (len - 1))
    {
      outtextstrn[outtextpos] = '\0';
#ifdef _WIN32
      _outtext(outtextstrn, checkLF);
#else
      _outtext(outtextstrn);
#endif
    }
  }

  delete[] outtextstrn;
}


//
// displayStringNodes : outputs a list of stringNodes to the screen, using
//                      displayColorString to display in color
//
//   *rootNode : root node of stringNodes
//

void displayStringNodes(stringNode *rootNode)
{
  while (rootNode)
  {
    displayColorString(rootNode->string);

    _settextcolor(7);  // as per Duris
    _setbkcolor(0);
    bold = flashing = FALSE;

    _outtext("\n");

    rootNode = rootNode->Next;
  }
}


//
// displayPrompt : Displays the prompt that will greet you so often in
//                 dikuEdit.
//

INLINEDEF void displayPrompt(void)
{
  displayColorString(promptString);
}


//
// remColorCodes : Removes the Duris color codes from a string, both altering
//                 the original and returning the address.
//
//   *strn : string to alter
//

char *remColorCodes(char *strn)
{
  ulong i, len = strlen(strn), pos = 0;
  char *newstrn = new char[len + 64];  // shrug


  if (!newstrn)
  {
    cout << "error in remColorCodes(): couldn't alloc newstrn\n";
    strn[0] = '\0';
    return strn;
  }

 // zero out the entire string

  memset(newstrn, 0, sizeof(char) * (len + 64));

 // run through the string, sweetcheeks

  for (i = 0; i < len; i++)
  {
    if (strn[i] == '&')
    {
      i++;

      if (toupper(strn[i]) == 'N') continue;
      else if ((strn[i] == '+') || (strn[i] == '-'))
      {
        i++;
        continue;
      }
      else if (strn[i] == '=')
      {
        i += 2;
        continue;
      }
      else i--;
    }

    newstrn[pos] = strn[i];
    pos++;
  }

  newstrn[pos] = '\0';

  strcpy(strn, newstrn);
  delete[] newstrn;

  return strn;
}
