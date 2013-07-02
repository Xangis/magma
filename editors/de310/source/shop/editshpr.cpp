//
//  File: editshpr.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shop racist values
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "shop.h"

//
// displayEditShopRacistMenu : Displays edit shop racist menu
//

void displayEditShopRacistMenu(shop *shp, const char *mobName,
                               const long mobNumb)
{
  char newName[256], *strn;


  strn = new char[4096];
  if (!strn)
  {
    _outtext("\n\nout of memory - cannot alloc strn in displayEditShopRacistMenu\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&+gEditing shop racist info for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wIs shopkeeper racist?                %s\n"
  "   &+YB&+L.&n &+wEdit shopkeeper race &+c(%s)\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getYesNoStrn(shp->racist),
        getShopRaceStrn(shp->shopkeeperRace),
        getMenuPromptName());
  }
  else
  {
    sprintf(strn,
  "   &+YA&+L.&n &+wIs shopkeeper racist?                %s\n"
  "   &+YB&+L.&n &+wEdit shopkeeper race\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getYesNoStrn(shp->racist),
        getMenuPromptName());
  }

  displayColorString(strn);

  delete[] strn;
}


//
// interpEditShopRacistMenu :
//                      Interprets user input for edit shop racist menu -
//                      returns TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *shp : shop to edit
//

char interpEditShopRacistMenu(const usint ch, shop *shp, const char *mobName,
                              const long mobNumb)
{
  struct rccoord coords;
  char strn[256];


 // toggle shop racism

  if (ch == 'A')
  {
    shp->racist = !shp->racist;

    displayEditShopRacistMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop race value

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    while (TRUE)
    {
      displayColorString("&+YNew shopkeeper race (? for list): &n");

      itoa(shp->shopkeeperRace, strn, 10);
      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayShopRaceList();
      else break;
    }

    shp->shopkeeperRace = atoi(strn);

    displayEditShopRacistMenu(shp, mobName, mobNumb);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editShopRacist : edit shop racism crap
//

void editShopRacist(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  char oldracist = shp->racist, oldrace = shp->shopkeeperRace;


 // display the menu (yay)

  displayEditShopRacistMenu(shp, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      shp->racist = oldracist;
      shp->shopkeeperRace = oldrace;

      return;
    }

   // if interpEditShopRacistMenu is TRUE, user has exited menu

    if (interpEditShopRacistMenu(ch, shp, mobName, mobNumb)) return;
  }
}
