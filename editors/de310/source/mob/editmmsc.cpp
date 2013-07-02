//
//  File: editmmsc.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing misc mob info (species, class,
//         hometown, align)
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "mob.h"

//
// displayEditMobMiscMenu : Displays the edit mob miscellany menu
//
//    *mob : mob being edited
//

void displayEditMobMiscMenu(const dikuMob *mob)
{
  char strn[256], newName[256], outStrn[800];


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mob->mobShortName, newName);

  sprintf(strn, "&+gEditing miscellany for mob #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          mob->mobNumber, newName);

  displayColorString(strn);

  sprintf(outStrn,
"   &+YA&+L.&n &+wMob species                    %s (%s)&n\n"
"   &+YB&+L.&n &+wMob alignment                  %d&n\n"
"   &+YC&+L.&n &+wMob class                      %u (%s)&n\n"
"\n"
MENU_COMMON
"\n"
"%s",
                  mob->mobSpecies, getMobSpeciesStrn(mob->mobSpecies),
                  mob->alignment,
                  mob->mobClass, getMobClassStrn(mob->mobClass),
                  getMenuPromptName());

  displayColorString(outStrn);
}


//
// interpEditMobMiscMenu : Interprets user input for edit mob flags menu -
//                         returns TRUE if the user hit 'Q'
//
//     ch : user input
//   *mob : mob to edit
//

char interpEditMobMiscMenu(usint ch, dikuMob *mob, dikuMob *origMob)
{
  struct rccoord coords;
  char strn[256], oldSpecies[MAX_SPECIES_LEN], origSpecies[MAX_SPECIES_LEN];
  long oldVal, origVal;


 // mob species

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    strcpy(oldSpecies, mob->mobSpecies);

    while (TRUE)
    {
      displayColorString("&+CNew value for mob species (? for list):&n ");

      getStrn(strn, MAX_SPECIES_LEN - 1, 1, 7, '°', mob->mobSpecies, FALSE,
              FALSE);

      if (!strcmp(strn, "?")) displayMobSpeciesList();
      else break;  // later add some sorta range checking or sump'n
    }

    strcpy(origSpecies, mob->mobSpecies);

    strcpy(mob->mobSpecies, upstrn(strn));
    strcpy(origMob->mobSpecies, mob->mobSpecies);

    if (!mobsCanEquipEquipped(FALSE))
    {
      _setbkcolor(0);

      displayColorString("\n\n&+c"
  "Error - mobs that were equipping this object no longer can due to changes in\n&+c"
  "        mob species - revert species back to &+CP&n&+crevious, or stick all\n&+c"
  "        affected eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

      if (ch != 'P')
      {
        mobsCanEquipEquipped(TRUE);
      }
      else strcpy(mob->mobSpecies, oldSpecies);

      _outtext("\n\n");
    }

    strcpy(origMob->mobSpecies, origSpecies);

    displayEditMobMiscMenu(mob);
  }
  else

 // mob hometown

 // mob alignment

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    oldVal = mob->alignment;

    while (TRUE)
    {
      displayColorString("&+CNew mob alignment:&n ");

      itoa(mob->alignment, strn, 10);
      getStrn(strn, 6, 1, 7, '°', strn, FALSE, FALSE);

      break;  // range checking, blah blah
    }

    origVal = origMob->alignment;

    mob->alignment = atoi(strn);
    origMob->alignment = mob->alignment;

    if (!mobsCanEquipEquipped(FALSE))
    {
      _setbkcolor(0);

      displayColorString("\n\n&+c"
  "Error - mobs that were equipping this object no longer can due to changes in\n&+c"
  "        mob alignment - revert alignment back to &+CP&n&+crevious, or stick all\n&+c"
  "        affected eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

      if (ch != 'P')
      {
        mobsCanEquipEquipped(TRUE);
      }
      else mob->alignment = oldVal;

      _outtext("\n\n");
    }

    origMob->alignment = origVal;

    displayEditMobMiscMenu(mob);
  }
  else

 // mob class

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    oldVal = mob->mobClass;

    while (TRUE)
    {
      displayColorString("&+CNew mob class (? for list):&n ");

      itoa(mob->mobClass, strn, 10);

      getStrn(strn, 3, 1, 7, '°', strn, FALSE, FALSE);

      if (!strcmp(strn, "?")) displayMobClassList();
      else break;  // later add some sorta range checking or sump'n
    }

    origVal = origMob->mobClass;

    mob->mobClass = atoi(strn);
    origMob->mobClass = mob->mobClass;

    if (!mobsCanEquipEquipped(FALSE))
    {
      _setbkcolor(0);

      displayColorString("\n\n&+c"
  "Error - mobs that were equipping this object no longer can due to changes in\n&+c"
  "        mob class - revert class back to &+CP&n&+crevious, or stick all\n&+c"
  "        affected eq in mobs' &+CC&n&+carried list (&+Cp/C&n&+c)? ");

      do
      {
        ch = toupper(getkey());
      } while ((ch != 'P') && (ch != 'C') && (ch != K_Enter));

      if (ch != 'P')
      {
        mobsCanEquipEquipped(TRUE);
      }
      else mob->mobClass = oldVal;

      _outtext("\n\n");
    }

    origMob->mobClass = origVal;

    displayEditMobMiscMenu(mob);
  }
  else

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;


  return FALSE;
}


//
// editMobMisc : Edit mob misc #2 - "main" function
//
//   *mob : mob to edit
//

void editMobMisc(dikuMob *mob, dikuMob *origMob)
{
  usint ch;
  char oldSpecies[MAX_SPECIES_LEN], oldHometown = mob->mobHometown;
  sint oldAlignment = mob->alignment;


  strcpy(oldSpecies, mob->mobSpecies);

  displayEditMobMiscMenu(mob);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      strcpy(mob->mobSpecies, oldSpecies);
      mob->mobHometown = oldHometown;
      mob->alignment = oldAlignment;

      _outtext("\n\n");

      return;
    }

    if (interpEditMobMiscMenu(ch, mob, origMob)) return;
  }
}
