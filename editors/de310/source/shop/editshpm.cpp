//
//  File: editshpm.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing shop messages
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
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
// displayEditShopMessagesMenu : Displays edit shop messages menu
//

void displayEditShopMessagesMenu(const char *mobName, const long mobNumb)
{
  char newName[256], *strn;


  strn = new char[4096];
  if (!strn)
  {
    _outtext("\n\nout of memory - cannot alloc strn in displayEditShopMessagesMenu\n\n");
    return;
  }

  _clearscreen(7, 0);

  _settextposition(1, 1);
  _settextcolor(7);
  _setbkcolor(0);

  fixMobName(mobName, newName);

  sprintf(strn, "&+gEditing shop messages for mob #&+c%d&+w, &+L\"&n%s&+L\"&n\n\n",
          mobNumb, newName);

  displayColorString(strn);

  sprintf(strn,
"   &+YA&+L.&n &+wEdit 'shop not selling that item' message\n"
"   &+YB&+L.&n &+wEdit 'player doesn't have item trying to sell' message\n"
"   &+YC&+L.&n &+wEdit 'shop doesn't buy that type of item' message\n"
"   &+YD&+L.&n &+wEdit 'shop doesn't have enough money' message\n"
"   &+YE&+L.&n &+wEdit 'player doesn't have enough money' message\n"
"   &+YF&+L.&n &+wEdit message shown when selling an item to player\n"
"   &+YG&+L.&n &+wEdit message shown when buying an item from player\n"
"\n"
"   &+YH&+L.&n &+wEdit message shown when opening\n"
"   &+YI&+L.&n &+wEdit message shown when closing\n"
"\n"
"   &+YJ&+L.&n &+wEdit message shown when shopkeeper is racist\n"
"\n"
MENU_COMMON
"\n"
"%s", getMenuPromptName());

  displayColorString(strn);

  delete[] strn;
}


//
// interpEditShopMessagesMenu :
//                      Interprets user input for edit shop msgs menu - returns
//                      TRUE if the user hits 'Q', FALSE otherwise
//
//     ch : user input
//   *shp : shop to edit
//

char interpEditShopMessagesMenu(const usint ch, shop *shp, const char *mobName,
                                const long mobNumb)
{
  struct rccoord coords;
  char strn[MAX_SHOPSTRING_LEN];


 // edit 'shop not selling item' message

  if (ch == 'A')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew 'shop not selling item' message: &n");

      strcpy(strn, shp->notSellingItem);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

      if (numbPercentS(strn) >= 1)
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);

        _setbkcolor(0);
        displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
        getkey();
      }
      else
      {
        strcpy(shp->notSellingItem, strn);
        break;
      }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit 'player doesn't have item' message

  if (ch == 'B')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew 'player doesn't have item' message: &n");

      strcpy(strn, shp->playerNoItem);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

      if (numbPercentS(strn) >= 1)
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);

        _setbkcolor(0);
        displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
        getkey();
      }
      else
      {
        strcpy(shp->playerNoItem, strn);
        break;
      }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit 'shop doesn't trade in this item type' message

  if (ch == 'C')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew 'shop doesn't buy this item' message: &n");

      strcpy(strn, shp->shopNoTradeItem);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

      if (numbPercentS(strn) >= 1)
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);

        _setbkcolor(0);
        displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
        getkey();
      }
      else
      {
        strcpy(shp->shopNoTradeItem, strn);
        break;
      }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit 'shop doesn't have the money' message

  if (ch == 'D')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew 'shop doesn't have the money' message: &n");

      strcpy(strn, shp->shopNoMoney);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

      if (numbPercentS(strn) >= 1)
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);

        _setbkcolor(0);
        displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
        getkey();
      }
      else
      {
        strcpy(shp->shopNoMoney, strn);
        break;
      }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit 'player doesn't have the money' message

  if (ch == 'E')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew 'PC doesn't have the money' message: &n");

      strcpy(strn, shp->playerNoMoney);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

      if (numbPercentS(strn) >= 1)
      {
        _settextposition(coords.row, 1);
        clrline(coords.row);

        _setbkcolor(0);
        displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
        getkey();
      }
      else
      {
        strcpy(shp->playerNoMoney, strn);
        break;
      }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit sell message

  if (ch == 'F')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew sell message: &n");

      strcpy(strn, shp->sellMessage);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

        if (numbPercentS(strn) >= 1)
        {
          _settextposition(coords.row, 1);
          clrline(coords.row);

          _setbkcolor(0);
          displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
          getkey();
        }
        else
        {
          strcpy(shp->sellMessage, strn);
          break;
        }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit buy message

  if (ch == 'G')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    while (TRUE)
    {
      _settextposition(coords.row, 1);
      _setbkcolor(0);
      displayColorString("&+YNew buy message: &n");

      strcpy(strn, shp->buyMessage);

      getStrn(strn, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
              strn, FALSE, FALSE);

        if (numbPercentS(strn) >= 1)
        {
          _settextposition(coords.row, 1);
          clrline(coords.row);

          _setbkcolor(0);
          displayColorString("&+YError - this message must not have '%%' anywhere.  Press a key..");
          getkey();
        }
        else
        {
          strcpy(shp->buyMessage, strn);
          break;
        }
    }

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit opening message

  if (ch == 'H')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    _setbkcolor(0);
    displayColorString("&+YNew opening message: &n");

    getStrn(shp->openMessage, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
            shp->openMessage, FALSE, FALSE);

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit closing message

  if (ch == 'I')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    _setbkcolor(0);
    displayColorString("&+YNew closing message: &n");

    getStrn(shp->closeMessage, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
            shp->closeMessage, FALSE, FALSE);

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // edit racist message

  if (ch == 'J')
  {
    coords = _gettextposition();

    _settextposition(coords.row, 1);

    clrline(coords.row);

    _setbkcolor(0);
    displayColorString("&+YNew racist message: &n");

    getStrn(shp->racistMessage, MAX_SHOPSTRING_LEN - 1 - 1, 1, 7, '°',
            shp->racistMessage, FALSE, FALSE);

    displayEditShopMessagesMenu(mobName, mobNumb);
  }
  else

 // quit

  if (checkMenuKey(ch, FALSE) == MENUKEY_SAVE) return TRUE;

  return FALSE;
}


//
// editShopMessages : edit shop messages
//

void editShopMessages(shop *shp, const char *mobName, const long mobNumb)
{
  usint ch;
  char noSellOld[MAX_SHOPSTRING_LEN], playerNoOld[MAX_SHOPSTRING_LEN],
       shopNoTradeOld[MAX_SHOPSTRING_LEN], shopNoMoneyOld[MAX_SHOPSTRING_LEN],
       playerNoMoneyOld[MAX_SHOPSTRING_LEN], sellMsgOld[MAX_SHOPSTRING_LEN],
       buyMsgOld[MAX_SHOPSTRING_LEN], openMsgOld[MAX_SHOPSTRING_LEN],
       closeMsgOld[MAX_SHOPSTRING_LEN], raceMsgOld[MAX_SHOPSTRING_LEN];


 // copy all the message crap

  strcpy(noSellOld, shp->notSellingItem);
  strcpy(playerNoOld, shp->playerNoItem);
  strcpy(shopNoTradeOld, shp->shopNoTradeItem);
  strcpy(shopNoMoneyOld, shp->shopNoMoney);
  strcpy(playerNoMoneyOld, shp->playerNoMoney);
  strcpy(sellMsgOld, shp->sellMessage);
  strcpy(buyMsgOld, shp->buyMessage);
  strcpy(openMsgOld, shp->openMessage);
  strcpy(closeMsgOld, shp->closeMessage);
  strcpy(raceMsgOld, shp->racistMessage);

 // display the menu (yay)

  displayEditShopMessagesMenu(mobName, mobNumb);

  while (TRUE)
  {
    ch = toupper(getkey());

    if (checkMenuKey(ch, FALSE) == MENUKEY_ABORT)
    {
      strcpy(shp->notSellingItem, noSellOld);
      strcpy(shp->playerNoItem, playerNoOld);
      strcpy(shp->shopNoTradeItem, shopNoTradeOld);
      strcpy(shp->shopNoMoney, shopNoMoneyOld);
      strcpy(shp->playerNoMoney, playerNoMoneyOld);
      strcpy(shp->sellMessage, sellMsgOld);
      strcpy(shp->buyMessage, buyMsgOld);
      strcpy(shp->openMessage, openMsgOld);
      strcpy(shp->closeMessage, closeMsgOld);
      strcpy(shp->racistMessage, raceMsgOld);

      return;
    }

   // if interpEditShopMessagesMenu is TRUE, user has exited menu

    if (interpEditShopMessagesMenu(ch, shp, mobName, mobNumb)) return;
  }
}
