//
//  File: editshpc.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing various shop time values
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "shop.h"

//
// displayEditShopTimesMenu : Displays edit shop times menu
//

void displayEditShopTimesMenu(shop *shp, const char *mobName,
                              const long mobNumb)
{
  char newName[256], *strn;


  strn = new char[4096];
  if (!strn)
  {
    _outtext("\n\nout of memory - cannot alloc strn in displayEditShopTimesMenu\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&+gEditing shop times for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wEdit first opening time &+c(%u)\n"
  "   &+YB&+L.&n &+wEdit first closing time &+c(%u)\n"
  "   &+YC&+L.&n &+wEdit second opening time &+c(%u)\n"
  "   &+YD&+L.&n &+wEdit second closing time &+c(%u)\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", shp->firstOpen, shp->firstClose, shp->secondOpen, shp->secondClose,
        getMenuPromptName());
  }
  else
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wEdit first opening time\n"
  "   &+YB&+L.&n &+wEdit first closing time\n"
  "   &+YC&+L.&n &+wEdit second opening time\n"
  "   &+YD&+L.&n &+wEdit second closing time\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getMenuPromptName());
  }

  displayColorString(strn);

  delete[] strn;
}


//
// interpEditShopTimesMenu :
//                      Interprets user input for edit shop times menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *shp : shop to edit
//

char interpEditShopTimesMenu(const usint ch, shop *shp, const char *mobName,
                             const long mobNumb)
{
  struct rccoord coords;
  char strn[256];


 // edit first opening time

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+YNew first opening time: &n");

    itoa(shp->firstOpen, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    shp->firstOpen = atoi(strn);

    displayEditShopTimesMenu(shp, mobName, mobNumb);
  }
  else

 // edit first closing time

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+YNew first closing time: &n");

    itoa(shp->firstClose, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    shp->firstClose = atoi(strn);

    displayEditShopTimesMenu(shp, mobName, mobNumb);
  }
  else

 // edit second opening time

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+YNew second opening time: &n");

    itoa(shp->secondOpen, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    shp->secondOpen = atoi(strn);

    displayEditShopTimesMenu(shp, mobName, mobNumb);
  }
  else

 // edit second closing time

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString("&+YNew second closing time: &n");

    itoa(shp->secondClose, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    shp->secondClose = atoi(strn);

    displayEditShopTimesMenu(shp, mobName, mobNumb);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editShopTimes : edit shop times
//

void editShopTimes(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  uchar firstOold = shp->firstOpen, firstCold = shp->firstClose,
        secondOold = shp->secondOpen, secondCold = shp->secondClose;


 // display the menu (yay)

  displayEditShopTimesMenu(shp, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      shp->firstOpen = firstOold;
      shp->firstClose = firstCold;
      shp->secondOpen = secondOold;
      shp->secondClose = secondCold;

      return;
    }

   // if interpEditShopTimesMenu is TRUE, user has exited menu

    if (interpEditShopTimesMenu(ch, shp, mobName, mobNumb)) return;
  }
}
