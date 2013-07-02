//
//  File: editshpt.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shop 'bought' list
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "../types.h"
#include "../fh.h"
#include "../misc/menu.h"
#include "../keys.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "shop.h"
#include "../quest/editqst.h"

#include "editshp.h"

//
// displayEditShopBoughtMenu : Displays edit room menu
//
//   *room : room being edited
//

void displayEditShopBoughtMenu(shop *shp, const char *mobName,
                               const long mobNumb)
{
  char strn[1024], newName[256], ch;
  ulong numbShown = 0, numbItems = getNumbShopBought(shp->tradedItemList), i;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&n&+gEditing shop traded list for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (numbItems == 0)
  {
    displayColorString("&+w  This shop does not buy any item types.\n");
  }

  for (i = 0; i < numbItems; i++)
  {
    if ((numbShown + 'A') > LAST_SHOP_CH)
      ch = FIRST_SHOP_POSTALPHA_CH + (numbShown - (LAST_SHOP_CH - 'A')) - 1;
    else ch = numbShown + 'A';

    sprintf(strn, "   &+Y%c&+L.&n &+w%s&n (#%u)\n",
            ch, getObjTypeStrn(shp->tradedItemList[i]),
                shp->tradedItemList[i]);

    displayColorString(strn);

    numbShown++;

    if ((numbShown % MAX_ENTRIES) == 0)
    {
      displayColorString("\n&+CPress any key to continue..&n");
      getkey();
      displayColorString("\n\n");
    }
  }

  sprintf(strn,
"\n"
"   &+YY&+L.&n &+wDelete an item type&n\n"
"   &+YZ&+L.&n &+wAdd an item type&n\n"
"\n"
MENU_COMMON
"\n"
"%s", getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditShopBoughtMenu : Interprets user input for edit room menu - returns
//                            TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//  *room : room to edit
//

char interpEditShopBoughtMenu(usint ch, shop *shp, const char *mobName,
                              const long mobNumb)
{
  struct rccoord coords;
  ulong numbItems = getNumbShopBought(shp->tradedItemList), i;
  char strn[256], oldNumb[256];


 // delete an entry (or all of them)

  if ((ch == 'Y') && numbItems)
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString(
"&+cEnter letter of item type to delete (&+CY to delete all item types&n&+c): ");

    do
    {
      ch = toupper(getkey());

      if (ch == 'Y')
      {
        shp->tradedItemList[0] = 0;

        displayEditShopBoughtMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if ((ch == 'X') || (ch == K_Escape) || (ch == K_Enter))
      {
        displayEditShopBoughtMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if ((ch >= FIRST_SHOP_POSTALPHA_CH) && (ch < 'A'))
         ch += (LAST_SHOP_CH - FIRST_SHOP_POSTALPHA_CH) + 1;  // it's magic
    } while (!(numbItems > (ch - 'A')));

    if (numbItems > (ch - 'A'))
    {
      for (i = ch - 'A'; shp->tradedItemList[i]; i++)
      {
        shp->tradedItemList[i] = shp->tradedItemList[i + 1];
      }
    }

    displayEditShopBoughtMenu(shp, mobName, mobNumb);
    return FALSE;
  }
  else

 // add an entry

  if ((ch == 'Z') && (numbItems != MAX_NUMBSHOPITEMS))
  {
    while (TRUE)
    {
      coords = _gettextposition();
      _settextposition(coords.row, 1);
      clrline(coords.row);

      displayColorString(
  "&+YEnter number of object type to add (? for list): ");

      getStrn(strn, 3, 1, 7, '°', "\0", FALSE, FALSE);

      if (!strlen(strn))
      {
        displayEditShopBoughtMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if (!strcmp(strn, "?")) displayObjTypeList();
      else

     // only break out of loop if string is numeric

      if (strnumer(strn)) break;
    }

    shp->tradedItemList[numbItems] = atoi(strn);
    shp->tradedItemList[numbItems + 1] = 0;

    displayEditShopBoughtMenu(shp, mobName, mobNumb);
    return FALSE;
  }
  else

 // get input, and love it, cherish it, and best of all, sex it up a little

  if (((ch >= 'A') || (ch >= FIRST_SHOP_POSTALPHA_CH)) && (numbItems))
  {
    if ((ch >= FIRST_SHOP_POSTALPHA_CH) && (ch < 'A'))
      ch += (LAST_SHOP_CH - FIRST_SHOP_POSTALPHA_CH) + 1;  // it's magic

    if (numbItems > (ch - 'A'))
    {
      itoa(shp->tradedItemList[ch - 'A'], oldNumb, 10);

      while (TRUE)
      {
        coords = _gettextposition();
        _settextposition(coords.row, 1);
        clrline(coords.row);

        displayColorString(
    "&+YEnter new object type to buy (? for list): ");

        getStrn(strn, 3, 1, 7, '°', oldNumb, FALSE, FALSE);

        if (!strlen(strn))
        {
          displayEditShopBoughtMenu(shp, mobName, mobNumb);
          return FALSE;
        }

        if (!strcmp(strn, "?")) displayObjTypeList();
        else

       // only break out of loop if string is numeric and, if vnum checking is
       // on, specifies a valid vnum

        if (strnumer(strn)) break;
      }
    }

    shp->tradedItemList[ch - 'A'] = atoi(strn);
    displayEditShopBoughtMenu(shp, mobName, mobNumb);
    return FALSE;
  }
  else

 // quit - including nice little hack to prevent 'Q' from registering

  if ((checkMenuKey(ch, FALSE) == MENUKEY_SAVE) && (ch != 'Q')) return TRUE;

  return FALSE;
}


//
// editShopBought : edit a shop's sold list
//

void editShopBought(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  ulong oldList[MAX_NUMBSHOPITEMS];


  memcpy(oldList, shp->tradedItemList, sizeof(ulong) * MAX_NUMBSHOPITEMTYPES);

  displayEditShopBoughtMenu(shp, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      memcpy(shp->tradedItemList, oldList,
             sizeof(ulong) * MAX_NUMBSHOPITEMTYPES);

      _outtext("\n\n");

      return;
    }

    if (interpEditShopBoughtMenu(ch, shp, mobName, mobNumb)) return;
  }
}
