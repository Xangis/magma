//
//  File: editzmsc.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing misc. zone info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "../misc/menu.h"
#include "zone.h"

//
// displayEditZoneMiscMenu : Displays the edit zone miscellany menu
//
//    *zone : zone being edited
//

void displayEditZoneMiscMenu(dikuZone *zone)
{
  char strn[256], newName[256], *outStrn = new char[1024];


  _clearscreen(7, 0);

  if (!outStrn)
  {
    _outtext("\n\ndisplayEditZoneMiscMenu(): error allocating outStrn\n\n");

    return;
  }

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixZoneName(zone->zoneName, newName);

  sprintf(strn, "&+gEditing miscellany for zone #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          zone->zoneNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wZone low lifespan value (minutes)   %u&n\n"
"   &+YB&+L.&n &+wZone high lifespan value (minutes)  %u&n\n"
"   &+YC&+L.&n &+wZone reset mode                     %u (%s)&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  zone->lifeLow,
                  zone->lifeHigh,
                  zone->resetMode, getZoneResetStrn(zone->resetMode),
                  getMenuPromptName());

  displayColorString(outStrn);

  delete[] outStrn;
}


//
// editZoneLowLifespan
//

void editZoneLowLifespan(dikuZone *zone)
{
  struct rccoord coords;
  char strn[256];


  coords = _gettextposition();
  _settextposition(coords.row, 1);

  displayColorString("&+YNew value for zone low lifespan value: &n");

  coords = _gettextposition();

  while (TRUE)
  {
    itoa(zone->lifeLow, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    if ((strnumer(strn)) && (atoi(strn)) && (atoi(strn) < zone->lifeHigh)) break;

    _settextposition(coords.row, coords.col);
  }

  zone->lifeLow = atoi(strn);
}


//
// editZoneLifespan
//

void editZoneHighLifespan(dikuZone *zone)
{
  struct rccoord coords;
  char strn[256];


  coords = _gettextposition();
  _settextposition(coords.row, 1);

  displayColorString("&+YNew value for zone high lifespan value: &n");

  coords = _gettextposition();

  while (TRUE)
  {
    itoa(zone->lifeHigh, strn, 10);

    getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

    if ((strnumer(strn)) && (atoi(strn)) && (atoi(strn) > zone->lifeLow)) break;

    _settextposition(coords.row, coords.col);
  }

  zone->lifeHigh = atoi(strn);
}


//
// editZoneResetMode
//

void editZoneResetMode(dikuZone *zone)
{
  struct rccoord coords;
  char strn[256];


  coords = _gettextposition();

  _settextposition(coords.row, 1);

  while (TRUE)
  {
    displayColorString("&+YNew zone reset mode (? for list): &n");

    itoa(zone->resetMode, strn, 10);

    getStrn(strn, 1, 1, 7, '°', strn, FALSE, FALSE);

    if (!strcmp(strn, "?")) displayZoneResetModeList();
    else break;
  }

  zone->resetMode = atoi(strn);
}


//
// interpEditZoneMiscMenu : Interprets user input for edit zone misc menu -
//                          returns TRUE if the user hit 'Q'
//
//      ch : user input
//   *zone : zone to edit
//

char interpEditZoneMiscMenu(usint ch, dikuZone *zone)
{

 // edit zone low lifespan

  if (ch == 'A')
  {
    editZoneLowLifespan(zone);

    displayEditZoneMiscMenu(zone);
  }
  else

 // edit zone high lifespan

  if (ch == 'B')
  {
    editZoneHighLifespan(zone);

    displayEditZoneMiscMenu(zone);
  }
  else

  // edit zone reset mode

  if (ch == 'C')
  {
    editZoneResetMode(zone);

    displayEditZoneMiscMenu(zone);
  }
  else

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editZoneMisc : Edit zone misc - "main" function
//
//   *zone : zone to edit
//

void editZoneMisc(dikuZone *zone)
{
  usint ch;

  ulong oldTopRoomNumb = zone->topRoomNumb,
        oldLowLife = zone->lifeLow,
        oldHighLife = zone->lifeHigh;

  uchar oldResetMode = zone->resetMode;


  displayEditZoneMiscMenu(zone);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      zone->topRoomNumb = oldTopRoomNumb;
      zone->lifeLow = oldLowLife;
      zone->lifeHigh = oldHighLife;
      zone->resetMode = oldResetMode;

      _outtext("\n\n");

      return;
    }

    if (interpEditZoneMiscMenu(ch, zone)) return;
  }
}
