//
//  File: editzone.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing zone info
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "../misc/menu.h"

#include "zone.h"

extern char madeChanges;

//
// displayEditZoneMenu : Displays edit zone menu
//
//   *zone : zone being edited
//

void displayEditZoneMenu(const dikuZone *zone)
{
  char strn[256], newName[256], *outStrn = new char[2048];


  if (!outStrn)
  {
    _outtext("\n\ndisplayEditZoneMenu(): couldn't alloc outStrn\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixZoneName(zone->zoneName, newName);

  sprintf(strn, "&n&+gEditing zone #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          zone->zoneNumber, newName);

  displayColorString(strn);

  if (getShowMenuInfoVal())
  {
    sprintf(outStrn,
  "   &+YA&+L.&n &+wEdit zone name&n\n"
  "   &+YB&+L.&n &+wEdit zone number&n\n"
  "   &+YC&+L.&n &+wEdit zone lifespan values &+c(%u-%u)&n, reset mode &+c(%u %s)&n\n"
  "   &+YD&+L.&n &+wEdit miscellaneous bits &+c(%u)&n\n"
  "\n"
  MENU_COMMON
  "\n",
            zone->lifeLow, zone->lifeHigh, zone->resetMode,
              getZoneResetStrn(zone->resetMode),

            zone->miscBits.longIntFlags);
  }
  else
  {
    strcpy(outStrn,
  "   &+YA&+L.&n &+wEdit zone name&n\n"
  "   &+YB&+L.&n &+wEdit zone number&n\n"
  "   &+YC&+L.&n &+wEdit zone lifespan values, reset mode&n\n"
  "   &+YD&+L.&n &+wEdit zone miscellaneous bits&n\n"
  "\n"
  MENU_COMMON
  "\n");
  }

  displayColorString(outStrn);
  displayColorString(getMenuPromptName());

  delete[] outStrn;
}


//
// interpEditZoneMenu : Interprets user input for edit zone menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//  *zone : zone to edit
//

char interpEditZoneMenu(const usint ch, dikuZone *zone)
{
  struct rccoord coords;
  char strn[256], newName[256];
  ulong numb;


 // edit zone name

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew zone name: &n");

   // length - 1 - 1 because we gotta allow space for the tilde at the end

    getStrn(zone->zoneName, MAX_ZONENAME_LEN - 1 - 1 - 1, 1, 7, '°',
            zone->zoneName, FALSE, FALSE);

    _settextposition(1, 1);
    clrline(1, 0, 0);

    fixZoneName(zone->zoneName, newName);

    _setbkcolor(0);
    sprintf(strn, "&+gEditing zone #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            zone->zoneNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // "edit" zone number

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);
    displayColorString("&+YNew zone number: &n");

    itoa(zone->zoneNumber, strn, 10);

    getStrn(strn, 4, 1, 7, '°', strn, FALSE, FALSE);

    numb = atoi(strn);

    setZoneNumb(numb, FALSE);
    zone->zoneNumber = numb;  // since we're not editing the actual zone
                              // rec..

    _settextposition(1, 1);
    clrline(1, 0, 0);

    fixZoneName(zone->zoneName, newName);

    _setbkcolor(0);
    sprintf(strn, "&+gEditing zone #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
            zone->zoneNumber, newName);

    displayColorString(strn);

    clrline(coords.row, 7, 0);
    _settextposition(coords.row, 1);

    displayColorString(getMenuPromptName());
  }
  else

 // edit zone top room numb, lifespan, reset mode

  if (ch == 'C')
  {
    editZoneMisc(zone);

    displayEditZoneMenu(zone);
  }
  else

 // edit zone miscellaneous flags

  if (ch == 'D')
  {
    editZoneMiscFlags(zone);

    displayEditZoneMenu(zone);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editZone : "main" function for edit zone menu
//
//  *zone : zone being edited
//

void editZone(dikuZone *zone)
{
  usint ch;
  dikuZone newZone;


  if (!zone) return;  // ya never know

 // copy zone into newZone and display the menu

  memcpy(&newZone, zone, sizeof(dikuZone));

  displayEditZoneMenu(&newZone);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      _outtext("\n\n");

      return;
    }

   // if interpEditZoneMenu is TRUE, user has quit

    if (interpEditZoneMenu(ch, &newZone))
    {
      if (memcmp(zone, &newZone, sizeof(dikuZone)))   // changes have been made
      {
        memcpy(zone, &newZone, sizeof(dikuZone));

        madeChanges = TRUE;
      }

      _outtext("\n\n");

      return;
    }
  }
}
