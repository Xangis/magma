//
//  File: system.cpp     originally part of dikuEdit
//
//  Usage: extremely system-dependent pieces of code
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <malloc.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#ifndef __UNIX__
#  include <conio.h>
#endif
#include <string.h>

#if (!defined(__UNIX__) && !defined(_WIN32))
#  include <graph.h>
#endif

#include "types.h"
#include "fh.h"
#include "system.h"


#ifndef __UNIX__

using namespace std;

//
// execShell : Executes the system shell, restoring the screen when finished -
//             80x86, probably DOS-specific
//

void execShell(void)
{
  uchar *textMemPtr;


  textMemPtr = new uchar[TEXT_MEM_SIZE];

  if (!textMemPtr)
  {
    cout << "\n\nError allocating memory while shelling...  Aborting\n\n";
  }
  else
  {
    _outtext("\n\nShelling...  Type EXIT to return\n");

    memcpy(textMemPtr, (void *)TEXT_MEM_ADDR, TEXT_MEM_SIZE);

    system(getenv("COMSPEC"));

    memcpy((void *)TEXT_MEM_ADDR, textMemPtr, TEXT_MEM_SIZE);
    delete[] textMemPtr;

    _outtext("\nThou hast returned.\n\n");
  }
}


//
// clrscr : Clears the screen using a kid-tested and mother-approved keen-o
//          bean-o method - 80x86 specific
//
//  fg : foreground color to clear screen to
//  bg : ditto, except for background
//

#ifndef _WIN32
void clrscr(const char fg /*= 7*/, const char bg /*= 0*/)
{
  usint i;
  uchar *textPtr = (uchar *)TEXT_MEM_ADDR;
  uchar colorInfo = 0 | (bg << 4) | (fg);


  for (i = 0; i < DEFAULT_SCREEN_HEIGHT * DEFAULT_SCREEN_WIDTH; i++)
  {
    *textPtr = 0;
    textPtr++;

    *textPtr = colorInfo;
    textPtr++;
  }

  _settextposition(1, 1);
}
#endif


//
// clrline : line 1 is top row - 80x86 specific
//

#ifndef _WIN32
void clrline(const uchar line, const uchar fg /*= 7*/, const uchar bg /*= 0*/,
             const uchar clrch /*= ' '*/)
{
  ulong i;
  uchar *textPtr = (uchar *)TEXT_MEM_ADDR;
  uchar colorInfo = 0 | (bg << 4) | (fg);


  if (line > DEFAULT_SCREEN_HEIGHT) return;

  for (i = (((line - 1) * DEFAULT_SCREEN_WIDTH) << 1);
       i < ((line * DEFAULT_SCREEN_WIDTH) << 1);
       i++)
  {
    textPtr[i++] = clrch;
    textPtr[i] = colorInfo;
  }
}
#endif


//
// saveScreen : Copies the current screen buffer to a dynamically created
//              buffer, returning the address of the new buffer.
//

/*
uchar *saveScreen(void)
{
  uchar *textPtr = (uchar *)TEXT_MEM_ADDR;
  uchar *bufferPtr = new uchar[TEXT_MEM_SIZE];


  if (!bufferPtr)
  {
    cout << "\n\nERROR: saveScreen() ran out of memory for bufferPtr.\n\n";

    return NULL;
  }

  memcpy(bufferPtr, textPtr, TEXT_MEM_SIZE);

  return bufferPtr;
}
*/


//
// restoreScreen : Copies screenBuffer into the text buffer at B8000, deleting
//                 screenBuffer when done.
//

/*
void restoreScreen(uchar *screenBuffer)
{
  uchar *textPtr = (uchar *)TEXT_MEM_ADDR;


  if (!screenBuffer)
  {
    cout << "\n\nERROR: restoreScreen() - screenBuffer is NULL\n\n";

    return;
  }

  memcpy(textPtr, screenBuffer, TEXT_MEM_SIZE);

  delete[] screenBuffer;
}
*/


//
// freeStack : Returns the amount of free stack memory as a ulong
//

INLINEDEF ulong freeStack(void)
{
#ifndef _WIN32
  return stackavail();
#else
  return 0;
#endif
}


//
// displayFreeStack
//

void displayFreeStack(const char x /*= -1*/, const char y /*= -1*/,
                      const char extraLF /*= TRUE*/)
{
  char strn[256];


  if ((x != -1) && (y != -1))
  {
    _settextposition(y, x);
  }

  if (extraLF)
  {
    sprintf(strn, "\nThere are %u bytes free on the stack.\n\n", freeStack());
  }
  else
  {
    sprintf(strn, "There are %u bytes free on the stack.", freeStack());
  }

  _outtext(strn);
}


//
// freeMem : Returns the amount of free heap memory as a ulong - too bad it's
//           horribly inaccurate
//

/*
ulong freeMem(void)
{
  _heapgrow();
  return _memavl();
//  return _freect(1);
}
*/


//
// for lack of a better file to stick it in, this goes here - not needed
// currently (or ever?)
//

/*
ulong powi(register ulong base, register ulong exponent)
{
  register ulong i, result = base;


  if (exponent == 0) return 1;

  for (i = 0; i < (exponent - 1); i++)
  {
    result *= base;
  }

  return result;
}
*/


//
// getkey : "Gets" a key from the buffer, adding 500 if it is preceeded by
//          a null.  The new value is returned..
//
//          This function is so system-dependent, it's scary
//

#ifndef _WIN32
/*INLINEDEF*/ int getkey(void)
{
  int ch;

  if (!(ch = getch()))
  {
    return(getch() + 500);
  }

  return ch;
}
#endif

#endif
