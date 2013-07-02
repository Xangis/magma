/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         xtradia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TExtraDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "xtradia.h"
#include "areadoc.h"
#include "pounds.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TExtraDialog, TDialog)
//{{TExtraDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_EN_CHANGE(IDC_KEYWORD, CmKeywordChanged),
//{{TExtraDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TExtraDialog Implementation}}


static TExtraDialogXfer TExtraDialogData;

TExtraDialog::TExtraDialog (TWindow* parent, TExtraDescr *xtra,
        TAreaDocument *doc, TResId resId, TModule* module):
    TDialog(parent, resId, module)
{
//{{TExtraDialogXFER_USE}}
    Keyword = new TEdit(this, IDC_KEYWORD, 255);
    Description = new TEdit(this, IDC_DESCR, 4096);
    Ok = new TButton(this, IDOK);

    SetTransferBuffer(&TExtraDialogData);
//{{TExtraDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheDescr = xtra;
    AreaDoc = doc;

    strcpy(TExtraDialogData.Keyword, xtra->keyword.c_str());
    strcpy(TExtraDialogData.Description, xtra->description.c_str());
}


TExtraDialog::~TExtraDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TExtraDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);

    TheDescr->keyword = TExtraDialogData.Keyword;
    TheDescr->description = TExtraDialogData.Description;

    TDialog::CmOk();
}


void TExtraDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT,
       IDD_EXTRADIA);
}


void TExtraDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    Description->SendMessage(WM_SETFONT,
       (WPARAM) (HFONT) AreaDoc->GetMZF()->hFixedFont, 0);

    CmKeywordChanged();
}

void TExtraDialog::CmKeywordChanged ()
{
   Ok->EnableWindow(Keyword->GetTextLen() != 0);
}

