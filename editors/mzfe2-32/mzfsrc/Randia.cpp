/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         randia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TRandomDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "randia.h"
#include "pounds.h"
#include "objmerc.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TRandomDialog, TDialog)
//{{TRandomDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TRandomDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TRandomDialog Implementation}}


static TRandomDialogXfer TRandomDialogData;

TRandomDialog::TRandomDialog (TWindow* parent, char *Command, int *Type,
       int *Level, TResId resId, TModule* module):
    TDialog(parent, resId, module)
{
//{{TRandomDialogXFER_USE}}
    Weapon = new TRadioButton(this, IDC_WEAPON, 0);
    Armor = new TRadioButton(this, IDC_ARMOR, 0);
    Potion = new TRadioButton(this, IDC_POTION, 0);
    Misc = new TRadioButton(this, IDC_MISC, 0);
    Shoddy = new TRadioButton(this, IDC_SHODDY, 0);
    Decent = new TRadioButton(this, IDC_DECENT, 0);
    Fine = new TRadioButton(this, IDC_FINE, 0);
    Extraordinary = new TRadioButton(this, IDC_EXTRA, 0);
    Inventory = new TRadioButton(this, IDC_INV, 0);
    Wield = new TRadioButton(this, IDC_WIELD, 0);

    SetTransferBuffer(&TRandomDialogData);
//{{TRandomDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    pCommand = Command;
    pnType = Type;
    pnLevel = Level;

    TRandomDialogData.Weapon = (*Type == 0);
    TRandomDialogData.Armor =  (*Type == 1);
    TRandomDialogData.Potion = (*Type == 2);
    TRandomDialogData.Misc =   (*Type == 3);
    TRandomDialogData.Shoddy = (*Level == 0);
    TRandomDialogData.Decent = (*Level == 1);
    TRandomDialogData.Fine =   (*Level == 2);
    TRandomDialogData.Extraordinary = (*Level == 3);
    TRandomDialogData.Inventory = (*Command == 'C');
    TRandomDialogData.Wield =  (*Command == 'F');
}


TRandomDialog::~TRandomDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TRandomDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);

    if      (TRandomDialogData.Weapon) *pnType = 0;
    else if (TRandomDialogData.Armor)  *pnType = 1;
    else if (TRandomDialogData.Potion) *pnType = 2;
    else if (TRandomDialogData.Misc)   *pnType = 3;
    if      (TRandomDialogData.Shoddy) *pnLevel = 0;
    else if (TRandomDialogData.Decent) *pnLevel = 1;
    else if (TRandomDialogData.Fine)   *pnLevel = 2;
    else if (TRandomDialogData.Extraordinary) *pnLevel = 3;
    if      (TRandomDialogData.Inventory) *pCommand = 'C';
    else if (TRandomDialogData.Wield)  *pCommand = 'F';

    TDialog::CmOk();
}


void TRandomDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT,
       IDD_RANDOMDIALOG);
}

