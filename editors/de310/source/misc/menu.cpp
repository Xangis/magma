//
//  File: menu.cpp       originally part of dikuEdit
//
//  Usage: functions for handling common menu input
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../misc/menu.h"
#include "../keys.h"
#include "../types.h"

//
// checkMenuKey : keys common to 99% of all menus, return various keyed
//                values
//
//       ch : character entered by user
// flagMenu : if TRUE, check for Ctrl-Q and Ctrl-X instead of Q and X and
//            don't check for NEXT_CH and PREV_CH
//

char checkMenuKey(const usint ch, const char flagMenu)
{
  if (flagMenu)
  {
    switch (ch)
    {
      case MENUKEY_SAVE_CH  :
      case K_Ctrl_Q         : return MENUKEY_SAVE;

      case MENUKEY_ABORT_CH :
      case K_Ctrl_X         : return MENUKEY_ABORT;

      default : return MENUKEY_OTHER;
    }
  }
  else
  {
    switch (ch)
    {
      case MENUKEY_SAVE_CH  :
      case 'Q'              : return MENUKEY_SAVE;

      case MENUKEY_ABORT_CH :
      case 'X'              : return MENUKEY_ABORT;

      case '.'              :
      case MENUKEY_NEXT_CH  : return MENUKEY_NEXT;

      case ','              :
      case MENUKEY_PREV_CH  : return MENUKEY_PREV;

      case MENUKEY_JUMP_CH  : return MENUKEY_JUMP;

      default : return MENUKEY_OTHER;
    }
  }
}
