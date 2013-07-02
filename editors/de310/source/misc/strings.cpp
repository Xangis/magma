//
//  File: strings.cpp    originally part of dikuEdit
//
//  Usage: functions galore for manipulating strings
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>
#include "../types.h"
#include "../boolean.h"
#include "../fh.h"


//
// deleteChar : "Deletes" the character in the specified position in the
//              specified string, moving all characters after the deleted
//              character back one.  The new string's address is returned
//
//     *strn : string to alter
//   strnPos : char position to delete
//

char *deleteChar(char *strn, const ulong strnPos)
{
  ulong i, len = strlen(strn);


  if (strnPos >= len) return strn;

  for (i = strnPos; i < len; i++)
  {
    strn[i] = strn[i + 1];
  }

  return strn;
}


//
// insertChar : Inserts a character at the position in the string passed to the
//              function
//
//    *strn : pointer to string
//  strnPos : position to insert character at
//       ch : character to insert
//

char *insertChar(char *strn, const ulong strnPos, const char ch)
{
  ulong i, len = strlen(strn);
  char *newStrn;

  newStrn = new char[strlen(strn) + 2];
  if (!newStrn)
  {
    _outtext("\ninsertChar(): couldn't alloc newStrn\n\n");
    return strn;
  }

  for (i = 0; i < strnPos; i++)
  {
    newStrn[i] = strn[i];
  }

  newStrn[i] = ch;

  for (; i < len; i++)
  {
    newStrn[i + 1] = strn[i];
  }

  newStrn[i + 1] = '\0';
  strcpy(strn, newStrn);
  delete[] newStrn;

  return strn;
}


//
// remLeadingSpaces : Removes spaces from the start of a string, returning the
//                    new string.
//
//  *strn : string to alter
//

char *remLeadingSpaces(char *strn)
{
  while (strn[0] == ' ') deleteChar(strn, 0);

  return strn;
}


//
// remTrailingSpaces : Removes spaces from the end of a string, returning the
//                     new string, as well as altering the one passed to the
//                     function.
//
//  *strn : string to alter
//

char *remTrailingSpaces(char *strn)
{
  long i = strlen(strn) - 1;


  while (i >= 0)
  {
    if (strn[i] == ' ') strn[i--] = '\0';
    else break;
  }

  return strn;
}


//
// remSpacesBetweenArgs : Removes excess spaces between "arguments"
//

char *remSpacesBetweenArgs(char *strn)
{
  ulong i = 0;

  while (strn[i] != '\0')
  {
    if (strn[i] == ' ')
    {
      while (strn[i + 1] == ' ')
      {
        deleteChar(strn, i + 1);
      }
    }

    i++;
  }

  return strn;
}


//
// nolf : "Removes" linefeeds ('\n') and carriage returns ('\r') from strings
//        by changing them into null characters.
//
//  *strn : string to alter
//

char *nolf(char *strn)
{
  ulong i, len = strlen(strn);


  for (i = 0; i < len; i++)
  {
    if ((strn[i] == '\n') || (strn[i] == '\r'))
    {
      strn[i] = '\0';
      break;
    }
  }

  return strn;
}


//
// numbArgs : returns number of arguments in string - removes spaces before
//            string, after string, and between args (doesn't modify original)
//            then counts number of spaces in string
//

usint numbArgs(const char *strn)
{
  char *copyStrn;
  usint i, numbArgs = 1;
  int len;


  copyStrn = new char[strlen(strn) + 1];
  if (!copyStrn)
  {
    _outtext("\nerror: couldn't alloc mem for copyStrn in numbArgs()\n\n");
    return 0;
  }

  strcpy(copyStrn, strn);
  nolf(copyStrn);
  remTrailingSpaces(copyStrn);
  remLeadingSpaces(copyStrn);
  remSpacesBetweenArgs(copyStrn);

  len = strlen(copyStrn);

  for (i = 0; i < len; i++)
  {
    if (copyStrn[i] == ' ') numbArgs++;
  }

  delete[] copyStrn;
  return numbArgs;
}


//
// getArg
//

char *getArg(const char *strn, const ulong argNumb, char *arg)
{
  char *copyStrn = new char[4096];
  usint len, i, currArg = 1, argPos = 0;


  if (!copyStrn)
  {
    _outtext("\nerror alloc'ing copyStrn in getArg\n\n");
    arg[0] = '\0';
    return arg;
  }

  if ((argNumb > numbArgs(strn)) || (argNumb == 0))
  {
    arg[0] = '\0';
    delete[] copyStrn;
    return arg;
  }

  strcpy(copyStrn, strn);
  nolf(copyStrn);
  remTrailingSpaces(copyStrn);
  remLeadingSpaces(copyStrn);
  remSpacesBetweenArgs(copyStrn);

  len = strlen(copyStrn);

  for (i = 0; i < len; i++)
  {
    if (currArg == argNumb)
    {
      while ((copyStrn[i] != ' ') && (copyStrn[i] != '\0'))
      {
        arg[argPos] = copyStrn[i];
        i++;
        argPos++;
      }

      arg[argPos] = '\0';
      delete[] copyStrn;
      return arg;
    }

    if (copyStrn[i] == ' ') currArg++;
//    printf("%c\n", copyStrn[i]);
  }

  arg[0] = '\0';
  delete[] copyStrn;
  return arg;
}


//
// upstrn : Upcases an entire string, altering the original and returning
//          the address of the altered string
//
//  *strn : string to upcase
//

char *upstrn(char *strn)
{
  ulong i, len = strlen(strn);


  for (i = 0; i < len; i++)
  {
    strn[i] = toupper(strn[i]);
  }

  return strn;
}


//
// upfirstarg : uppercases only the first argument in the string (assumes
//              no leading spaces)
//

char *upfirstarg(char *strn)
{
  ulong i = 0;

  while ((strn[i] != '\0') && (strn[i] != ' '))
  {
    strn[i] = toupper(strn[i]);
    i++;
  }

  return strn;
}


//
// lowstrn : Lowercases an entire string, altering the original and returning
//           the address of the altered string
//
//  *strn : string to lowercase
//

char *lowstrn(register char *strn)
{
  register ulong i, len = strlen(strn);


  for (i = 0; i < len; i++)
  {
    strn[i] = tolower(strn[i]);
  }

  return strn;
}


//
// strnumer : returns TRUE if string is composed entirely of numerics
//

char strnumer(register const char *strn, const usint pos /*= 0*/)
{
  register usint len = strlen(strn), i;


  if (pos >= len) return FALSE;  // hmm

  for (i = pos; i < len; i++)
  {
    if (!isdigit(strn[i])) return FALSE;
  }

  return TRUE;
}


//
// strleft : Checks to see if a substrn is on the "left" side of strn,
//           returning TRUE if so and FALSE if not
//
//    *strn : string to search for substrn in
// *substrn : substring to search for
//

char strleft(const char *strn, const char *substrn)
{
  register ulong i, len = strlen(strn), sublen = strlen(substrn);


  if (sublen > len) return FALSE;

  for (i = 0; i < sublen; i++)
  {
    if (strn[i] != substrn[i]) return FALSE;
  }

  return TRUE;
}


//
// strright : Checks if substrn exists in the "right" side of strn - if so,
//            TRUE is returned, if not, FALSE
//
//    *strn : string to search
// *substrn : string to search for inside of strn
//

char strright(const char *strn, const char *substrn)
{
  ulong i, len = strlen(strn), sublen = strlen(substrn);


  if (sublen > len) return FALSE;

  for (i = 1; i <= sublen; i++)
  {
    if (strn[len - i] != substrn[sublen - i]) return FALSE;
  }

  return TRUE;
}


//
// strnSect : Runs through strn, putting the section of strn from start to end
//            in newStrn.  the address of newStrn is returned
//

char *strnSect(const char *strn, char *newStrn, const ulong start,
               ulong end)
{
  register ulong i, len = strlen(strn), newPos = 0;


  if (start >= len)
  {
    newStrn[0] = '\0';

    return newStrn;
  }

  if (end >= len) end = len - 1;

  for (i = start; i <= end; i++)
  {
    newStrn[newPos++] = strn[i];
  }

  newStrn[newPos] = '\0';

  return newStrn;
}


//
// fixStringColor : "Fixes" a string that is over a certain length by trimming it to
//             the length (specified by maxLen) minus four and adding " .. " to
//             it.  The new string is stuck into newName as well as returned.
//
//   *oldName : string to be fixed
//   *newName : new string is dumped here
//     maxLen : length to trim string to
//

char *fixStringColor(const char *oldName, char *newName, ulong maxLen)
{
  ulong i, len = /*true*/strlen(oldName);


  for (i = 0; i < len; i++)
  {
    maxLen += durisANSIcode(oldName, i);  // whee
  }

  newName[0] = 0;

  if (len > (maxLen - 4))
  {
    for (i = 0; i < (maxLen - 4); i++)
    {
      newName[i] = oldName[i];
    }

    newName[i] = '\0';

    strcat(newName, " &n.. ");  // this code assumes that color codes are
                                // being interpreted.  damn.
  }
  else strcpy(newName, oldName);

  return newName;
}


//
// fixString : "Fixes" a string that is over a certain length by trimming it to
//             the length (specified by maxLen) minus four and adding " .. " to
//             it.  The new string is stuck into newName as well as returned.
//
//   *oldName : string to be fixed
//   *newName : new string is dumped here
//     maxLen : length to trim string to
//

char *fixString(const char *oldName, char *newName, const ulong maxLen,
                const char interpColor)
{
  ulong i, len = strlen(oldName);


  if (interpColor)
  {
    fixStringColor(oldName, newName, maxLen);
    return newName;
  }

  newName[0] = 0;

  if (len > (maxLen - 4))
  {
    for (i = 0; i < (maxLen - 4); i++)
    {
      newName[i] = oldName[i];
    }

    newName[i] = '\0';

    strcat(newName, " &n.. ");  // this code assumes that color codes are
                                // being interpreted.  damn.
  }
  else strcpy(newName, oldName);

  return newName;
}


//
// numbPercentS: returns number of '%s' substrings in strn
//

ulong numbPercentS(const char *strn)
{
  register ulong i, len = strlen(strn), found = 0;

  for (i = 0; i < len; i++)
  {
    if ((strn[i] == '%') && (toupper(strn[i + 1]) == 'S')) found++;
  }

  return found;
}


//
// lastCharLF : returns TRUE if last char is \n, FALSE otherwise
//

INLINEDEF char lastCharLF(const char *strn)
{
  if (!strn || !strn[0]) return FALSE;

  return (strn[strlen(strn) - 1] == '\n');
}


//
// numbLinefeeds
//

INLINEDEF ulong numbLinefeeds(const char *strn)
{
  ulong i, len = strlen(strn), numb = 0;

  for (i = 0; i < len; i++) if (strn[i] == '\n') numb++;

  return numb;
}


//
// truestrlen : checks for ANSI codes, doesn't count em in length - only
//              truly accurate for strings with no linefeeds (linefeed
//              characters [\n] are not counted as part of length)
//

ulong truestrlen(const char *strn)
{
  register ulong i = 0, numbSkip = 0, len = 0;

  if (!strn) return 0;  // never know..

 // if color codes are shown, we count pretty much everything

  if (getShowColorVal()) return strlen(strn) - numbLinefeeds(strn);

  while (strn[i])
  {
    if (strn[i] == '&')
    {
      numbSkip = durisANSIcode(strn, i);
    }

    if (numbSkip)
    {
      i += numbSkip;
      numbSkip = 0;
    }
    else
    {
      if (strn[i] != '\n') len++;
      i++;
    }
  }

  return len;
}
