/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         helpdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of THelpDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "helpdia.h"


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(THelpDialog, TDialog)
//{{THelpDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{THelpDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{THelpDialog Implementation}}


static THelpDialogXfer THelpDialogData;

THelpDialog::THelpDialog (TWindow* parent, const THelpData *TheHelp, int n,
       TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkHelp( *TheHelp )
{
//{{THelpDialogXFER_USE}}
    HelpLevel = new TEdit(this, IDC_LEVEL, 8);
    Keywords = new TEdit(this, IDC_KEYWORDS, 255);
    HelpText = new TEdit(this, IDC_HELPTEXT, 4096);

    SetTransferBuffer(&THelpDialogData);
//{{THelpDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    nIndex = n;

    wsprintf(THelpDialogData.HelpLevel, "%d", TheHelp->level);
    strcpy(THelpDialogData.Keywords, TheHelp->keyword.c_str());
    strcpy(THelpDialogData.HelpText, TheHelp->text.c_str());
}


THelpDialog::~THelpDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void THelpDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    WorkHelp.level = (sh_int) atoi(THelpDialogData.HelpLevel);
    WorkHelp.keyword = THelpDialogData.Keywords;
    WorkHelp.text = THelpDialogData.HelpText;
    WorkHelp.AreaDoc->HelpSave(WorkHelp, nIndex);
    TDialog::CmOk();
}


void THelpDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_HELPS);
}


void THelpDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    HelpText->SendMessage(WM_SETFONT,
       (WPARAM) (HFONT) WorkHelp.AreaDoc->GetMZF()->hFixedFont, 0);
}

