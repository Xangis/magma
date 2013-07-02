//
//  File: editshps.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shop 'sold' list
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"

#if (!defined(_WIN32) && !defined(__UNIX__))
#  include <graph.h>
#else
#  include "../graphcon.h"
#endif

#include "editshp.h"
#include "../misc/menu.h"
#include "../keys.h"

#include "../quest/editqst.h"

//
// displayEditShopSoldMenu : Displays edit room menu
//
//   *room : room being edited
//

void displayEditShopSoldMenu(shop *shp, const char *mobName, const long mobNumb)
{
  char strn[1024], objName[256], newName[256], ch;
  dikuObject *obj;
  ulong numbShown = 0, numbItems = getNumbShopSold(shp->producedItemList), i;


  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&n&+gEditing shop sold list for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  if (numbItems == 0)
  {
    displayColorString("&+w  This shop is not selling anything.\n");
  }

  for (i = 0; i < numbItems; i++)
  {
    if ((numbShown + 'A') > LAST_SHOP_CH)
      ch = FIRST_SHOP_POSTALPHA_CH + (numbShown - (LAST_SHOP_CH - 'A')) - 1;
    else ch = numbShown + 'A';

    obj = findObj(shp->producedItemList[i]);

    if (!obj) strcpy(objName, "(object not in this .OBJ file)");
    else fixObjName(obj->objShortName, objName);

    sprintf(strn, "   &+Y%c&+L.&n &+w%s&n (#%u)\n",
            ch, objName, shp->producedItemList[i]);

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
"   &+YY&+L.&n &+wDelete an item&n\n"
"   &+YZ&+L.&n &+wAdd an item&n\n"
"\n"
MENU_COMMON
"\n"
"%s", getMenuPromptName());

  displayColorString(strn);
}


//
// interpEditShopSoldMenu : Interprets user input for edit room menu - returns
//                          TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//  *room : room to edit
//

char interpEditShopSoldMenu(usint ch, shop *shp, const char *mobName,
                            const long mobNumb)
{
  struct rccoord coords;
  ulong numbItems = getNumbShopSold(shp->producedItemList), i;
  char strn[256], oldNumb[256];


 // delete an entry (or all of them)

  if ((ch == 'Y') && numbItems)
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);
    clrline(coords.row);

    displayColorString(
"&+cEnter letter of item to delete (&+CY to delete all items&n&+c): ");

    do
    {
      ch = toupper(getkey());

      if (ch == 'Y')
      {
        shp->producedItemList[0] = 0;

        displayEditShopSoldMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if ((ch == 'X') || (ch == K_Escape) || (ch == K_Enter))
      {
        displayEditShopSoldMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if ((ch >= FIRST_SHOP_POSTALPHA_CH) && (ch < 'A'))
         ch += (LAST_SHOP_CH - FIRST_SHOP_POSTALPHA_CH) + 1;  // it's magic
    } while (!(numbItems > (ch - 'A')));

    if (numbItems > (ch - 'A'))
    {
      for (i = ch - 'A'; shp->producedItemList[i]; i++)
      {
        shp->producedItemList[i] = shp->producedItemList[i + 1];
      }
    }

    displayEditShopSoldMenu(shp, mobName, mobNumb);
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
  "&+YEnter vnum of object to add (? for list): ");

      getStrn(strn, 6, 1, 7, '°', "\0", FALSE, FALSE);

      if (!strlen(strn))
      {
        displayEditShopSoldMenu(shp, mobName, mobNumb);
        return FALSE;
      }

      if (!strcmp(strn, "?")) displayObjectTypeList();
      else

     // only break out of loop if string is numeric and, if vnum checking is
     // on, specifies a valid vnum

      if ((strnumer(strn)) && (!getVnumCheckVal() || findObj(atoi(strn))))
        break;
    }

    shp->producedItemList[numbItems] = atoi(strn);
    shp->producedItemList[numbItems + 1] = 0;

    displayEditShopSoldMenu(shp, mobName, mobNumb);
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
      itoa(shp->producedItemList[ch - 'A'], oldNumb, 10);

      while (TRUE)
      {
        coords = _gettextposition();
        _settextposition(coords.row, 1);
        clrline(coords.row);

        displayColorString(
    "&+YEnter new object vnum to sell (? for list): ");

        getStrn(strn, 6, 1, 7, '°', oldNumb, FALSE, FALSE);

        if (!strlen(strn))
        {
          displayEditShopSoldMenu(shp, mobName, mobNumb);
          return FALSE;
        }

        if (!strcmp(strn, "?")) displayObjectTypeList();
        else

       // only break out of loop if string is numeric and, if vnum checking is
       // on, specifies a valid vnum

        if ((strnumer(strn)) && (!getVnumCheckVal() || findObj(atoi(strn))))
          break;
      }
    }

    shp->producedItemList[ch - 'A'] = atoi(strn);
    displayEditShopSoldMenu(shp, mobName, mobNumb);
    return FALSE;
  }
  else

 // quit - including nice little hack to prevent 'Q' from registering

  if ((checkMenuKey(ch, FALSE) == MENUKEY_SAVE) && (ch != 'Q')) return TRUE;

  return FALSE;
}


//
// editShopSold : edit a shop's sold list
//

void editShopSold(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  ulong oldList[MAX_NUMBSHOPITEMS];


  memcpy(oldList, shp->producedItemList, sizeof(ulong) * MAX_NUMBSHOPITEMS);

  displayEditShopSoldMenu(shp, mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      memcpy(shp->producedItemList, oldList,
             sizeof(ulong) * MAX_NUMBSHOPITEMS);

      _outtext("\n\n");

      return;
    }

    if (interpEditShopSoldMenu(ch, shp, mobName, mobNumb)) return;
  }
}
