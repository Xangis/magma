//
//  File: editshpb.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shop boolean values
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../misc/menu.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "shop.h"

//
// displayEditShopBooleansMenu : Displays edit shop times menu
//

void displayEditShopBooleansMenu(shop *shp, const char *mobName,
                                 const long mobNumb)
{
  char newName[256], *strn;


  strn = new char[4096];
  if (!strn)
  {
    _outtext("\n\nout of memory - cannot alloc strn in displayEditShopBooleansMenu\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&+gEditing shop toggles for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  sprintf(strn,
  "   &+YA&+L.&n &+wDoes shop roam?                            %s\n"
  "   &+YB&+L.&n &+wIs shop's room NO_MAGIC?                   %s\n"
  "   &+YC&+L.&n &+wAllow shopkeeper to be killed?             %s\n"
  "\n"
  MENU_COMMON
  "\n"
  "%s", getYesNoStrn(shp->roaming),
        getYesNoStrn(shp->noMagic),
        getYesNoStrn(shp->killable),

        getMenuPromptName());

  displayColorString(strn);

  delete[] strn;
}


//
// interpEditShopBooleansMenu :
//                      Interprets user input for edit shop times menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *shp : shop to edit
//

char interpEditShopBooleansMenu(const usint ch, shop *shp, const char *mobName,
                                const long mobNumb)
{

 // toggle shop roam value

  if (ch == 'A')
  {
    shp->roaming = !shp->roaming;

    displayEditShopBooleansMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop no magic value

  if (ch == 'B')
  {
    shp->noMagic = !shp->noMagic;

    displayEditShopBooleansMenu(shp, mobName, mobNumb);
  }
  else

 // edit shop killable value

  if (ch == 'C')
  {
    shp->killable = !shp->killable;

    displayEditShopBooleansMenu(shp, mobName, mobNumb);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editShopBooleans : edit shop times
//

void editShopBooleans(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  char oldRoam = shp->roaming, oldMagic = shp->noMagic,
       oldKill = shp->killable;


 // display the menu (yay)

  displayEditShopBooleansMenu(shp, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      shp->roaming = oldRoam;
      shp->noMagic = oldMagic;
      shp->killable = oldKill;

      return;
    }

   // if interpEditShopBooleansMenu is TRUE, user has exited menu

    if (interpEditShopBooleansMenu(ch, shp, mobName, mobNumb)) return;
  }
}
