//
//  File: initscr.cpp    originally part of dikuEdit
//
//  Usage: OS-specific functions for screen initalization, primarily
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#ifdef __UNIX__
#include <stdlib.h>
#include <ncurses.h>

#include "types.h"
#include "fh.h"

extern char cursesInit;

void exitUnixCursesCrap(void)
{
  endwin();
}


//
// initUnixCursesScreen
//

void initUnixCursesScreen(void)
{
  usint bg, fg;


  initscr();
  start_color();  // start the color, Beavis - add term checking later


 // init color pairs

//  for (bg = 0; bg < 8; bg++)
//  {
    for (fg = 0; fg < 8; fg++)
    {
//      if (fg >= 8) init_pair((bg * 16) + fg, fg - 8, bg);
      init_pair(fg + 1, fg, 0);
    }
//  }

  _settextcolor(7);
  _setbkcolor(0);
//  fg = 7;
//  bg = 0;

  cbreak();
  noecho();

  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  scrollok(stdscr, TRUE);

  atexit(exitUnixCursesCrap);

  cursesInit = TRUE;
}
#endif


#ifdef _WIN32

#include <mapiutil.h>
#include <mapiwin.h>
#include <wincon.h>

//extern HANDLE outpHand, inpHand;

//
// initWindowsScreen
//

void initWindowsScreen(void)
{
//  outpHand = GetStdHandle(STD_OUTPUT_HANDLE);
//  inpHand = GetStdHandle(STD_INPUT_HANDLE);
}
#endif
