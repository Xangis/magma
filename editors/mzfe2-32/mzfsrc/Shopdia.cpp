/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         shopdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TShopDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "shopdia.h"


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TShopDialog, TDialog)
//{{TShopDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TShopDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TShopDialog Implementation}}


static TShopDialogXfer TShopDialogData;

TShopDialog::TShopDialog (TWindow* parent, TAreaDocument *doc, TShopData *shop,
       int nNdx, BOOL lock, TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkShop( *shop )
{
//{{TShopDialogXFER_USE}}
    Keeper = new TComboBox(this, IDC_KEEPER, 1);
    for (int i = 0; i < nelems(Buy); i++)
       Buy[i] = new TComboBox(this, IDC_BUY1+i, 1);
    ProfitBuy = new TEdit(this, IDC_PROFITBUY, 5);
    ProfitSell = new TEdit(this, IDC_PROFITSELL, 5);
    OpenHour = new TEdit(this, IDC_OPENHOUR, 3);
    CloseHour = new TEdit(this, IDC_CLOSEHOUR, 3);
    Comment = new TEdit(this, IDC_COMMENT, 255);

    SetTransferBuffer(&TShopDialogData);
//{{TShopDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheShop = shop;
    nIndex = nNdx;
    AreaDoc = doc;
    fLockKeeper = lock;

    TShopDialogData.Keeper.Clear();
    doc->MobilesToCombo(TShopDialogData.Keeper, WorkShop.keeper);

    wsprintf(TShopDialogData.ProfitBuy, "%d", WorkShop.profit_buy);
    wsprintf(TShopDialogData.ProfitSell, "%d", WorkShop.profit_sell);
    wsprintf(TShopDialogData.OpenHour, "%d", WorkShop.open_hour);
    wsprintf(TShopDialogData.CloseHour, "%d", WorkShop.close_hour);

      // Cloathing, maps, portals, warpstones, gems, and jewelry are ROM 2.4 only
     static char * far szItemTypes[] = { "[none]", "lights", "scrolls", "wands",
       "staves", "weapons", "(*) unused", "(*) unused", "treasure", "armor",
       "potion", "(*) clothing", "furniture", "trash", "(*) unused", "containers",
       "(*) unused", "drinks", "keys", "food", "(*) gold", "(*) unused",
       "boats", "npc corpses", "player corpses", "(*) fountains", "pills",
       "(*) unused", "(*) maps", "(*) portals", "(*) warpstones",
       "(*) unused", "(*) gems", "(*) jewelry" };
    for (i = 0; i < nelems(Buy); i++)
       {
       TShopDialogData.Buy[i].Clear();
       for (int j = 0; j < nelems(szItemTypes); j++)
          TShopDialogData.Buy[i].AddString(szItemTypes[j],
                                           j == WorkShop.buy_type[i]);
       }

    const char *p = WorkShop.comment.c_str();
    if (*p == '\0')
       {
       const TMobile *mob = AreaDoc->GetMobile(WorkShop.keeper);
       if (mob)
          strcpy(TShopDialogData.Comment, mob->short_descr.c_str());
       else
          strcpy(TShopDialogData.Comment, "(mobile is from another zone)");
       }
    else
       {
       while (*p == '\t' || *p == ' ')
          p++;
       strcpy(TShopDialogData.Comment, p);
       }
}


TShopDialog::~TShopDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TShopDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);

    int n = Keeper->GetSelIndex();
    if (n != LB_ERR)
       WorkShop.keeper = (sh_int) Keeper->GetItemData(n);

    WorkShop.profit_buy = (sh_int) atoi(TShopDialogData.ProfitBuy);
    WorkShop.profit_sell = (sh_int) atoi(TShopDialogData.ProfitSell);
    WorkShop.open_hour = (sh_int) atoi(TShopDialogData.OpenHour);
    WorkShop.close_hour = (sh_int) atoi(TShopDialogData.CloseHour);

    for (int i = 0; i < nelems(Buy); i++)
       WorkShop.buy_type[i] = (sh_int) TShopDialogData.Buy[i].GetSelIndex();

    WorkShop.comment = TShopDialogData.Comment;

    AreaDoc->ShopSave(WorkShop, nIndex);

    TDialog::CmOk();
}


void TShopDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    if (fLockKeeper)
       Keeper->EnableWindow(FALSE);
}


void TShopDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_SHOP);
}

