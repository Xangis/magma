/*  Project pounds

    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         appdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TApplyDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "appdia.h"
#include "msetdia.h"

#define nelems(a) (sizeof (a)/sizeof (a)[0])

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TApplyDialog, TDialog)
//{{TApplyDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_LBN_DBLCLK(IDC_APPLY, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TApplyDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TApplyDialog Implementation}}


static TApplyDialogXfer TApplyDialogData;

TApplyDialog::TApplyDialog (TWindow* parent, int *loc, TResId resId, TModule* module):
    TDialog(parent, resId, module)
{
//{{TApplyDialogXFER_USE}}
    ApplyType = new TListBox(this, IDC_APPLY);

    SetTransferBuffer(&TApplyDialogData);
//{{TApplyDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    nLocation = loc;

    poundsApp *MuditorApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
    BOOL fTFC = (MuditorApp && MuditorApp->CurrentMode == poundsApp::TFCMode);

    // @@@ Combine with TObjDialog (?)
    static char * far szNames[] = {
      "(*) None", "Strength", "Dexterity", "Intelligence", "Wisdom",
      "Constitution",
      "(*) Sex", "(*) Class", "(*) Level", "Age", "(*) Height", "(*) Weight",
      "Mana", "Hit points", "Moves", "(*) Gold", "(*) Experience",
      "Armor Class", "Hit roll", "Damage Roll", "(*) Save vs Paralysis",
      "(*) Save vs Rod", "(*) Save vs Petrification", "Save vs Breath",
      "Save vs Spell",

      // TFC only
      "+Charisma", "+Luck", "+Anti-Theft", "+No SLEEP",
      "+Blocking", "+No CHARM", "+Familiarity", "+True Seeing", "+Obscurement"

      // Unused
      "(*) Unknown34", "(*) Unknown35", "(*) Unknown36", "(*) Unknown37",
      "(*) Unknown38", "(*) Unknown39",
      
      // Troll-only
      "Alignment (T)"
    };

    TApplyDialogData.ApplyType.Clear();
    for (int i = 0; i < nelems(szNames); i++)
       if (szNames[i][0] == '+')
          {
          if (fTFC)
             TApplyDialogData.ApplyType.AddStringItem(szNames[i]+1, i, FALSE);
          }
       else if (szNames[i][0] != '(')
          TApplyDialogData.ApplyType.AddStringItem(szNames[i], i, (i == 0));
}


TApplyDialog::~TApplyDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TApplyDialog::CmOk ()
{
    // INSERT>> Your code here.
    *nLocation = (int) ApplyType->GetItemData(ApplyType->GetSelIndex());
    if (*nLocation < 0)
       nLocation = 0;

    TDialog::CmOk();
}

//==============================================================================
DEFINE_RESPONSE_TABLE1(TObjectListDialog, TDialog)
    EV_BN_CLICKED(IDOK, CmOk),
    EV_LBN_DBLCLK(IDC_APPLY, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;

static TObjectListDialogXfer TObjectListDialogData;

TObjectListDialog::TObjectListDialog (TWindow* parent, TAreaDocument *doc,
        int32 *loc, TResId resId, TModule* module):
    TDialog(parent, resId, module)
{
    ObjectListType = new TListBox(this, IDC_APPLY);

    SetTransferBuffer(&TObjectListDialogData);

    nLocation = loc;
    TObjectListDialogData.ObjectListType.Clear();
    doc->ObjectsToList(TObjectListDialogData.ObjectListType);
}


TObjectListDialog::~TObjectListDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TObjectListDialog::CmOk ()
{
    // INSERT>> Your code here.
    *nLocation = ObjectListType->GetItemData(ObjectListType->GetSelIndex());
    if (*nLocation == CB_ERR)
       nLocation = 0;

    TDialog::CmOk();
}


void TApplyDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_APPLY);
}

void TObjectListDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_OBJLIST);
}

//==============================================================================
//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TResetRoomDialog, TDialog)
    EV_BN_CLICKED(IDOK, CmOk),
    EV_LBN_DBLCLK(IDC_APPLY, CmOk),
    EV_BN_CLICKED(IDC_NEWRESET, CmPlaceNew),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;


static TResetRoomDialogXfer TResetRoomDialogData;

TResetRoomDialog::TResetRoomDialog (TWindow* parent, TAreaDocument *doc,
       sh_int nVnumMobile, TResId resId, TModule* module):
    TDialog(parent, resId, module)
{
    ResetRoomType = new TListBox(this, IDC_APPLY);

    SetTransferBuffer(&TResetRoomDialogData);

    AreaDoc = doc;
    TResetRoomDialogData.ResetRoomType.Clear();
    doc->RoomsToList(TResetRoomDialogData.ResetRoomType, nVnumMobile);
}


TResetRoomDialog::~TResetRoomDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TResetRoomDialog::CmOk ()
{
    // INSERT>> Your code here.
	 DWORD dw = ResetRoomType->GetItemData(ResetRoomType->GetSelIndex());
    if (dw == (DWORD) LB_ERR)
	    return;

    TMobReset *mobreset = (TMobReset *) dw;
    int nNdx = AreaDoc->GetIndexFromMobReset(mobreset);
    if (nNdx != -1)
       TMobResetDialog(this, mobreset, TRUE, nNdx).Execute();
}

void TResetRoomDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_RESETROOM);
}

void TResetRoomDialog::CmPlaceNew ()
{
    TMobReset *mobreset = AreaDoc->MakeNewMobReset();
    TMobResetDialog(this, mobreset, TRUE, -1).Execute();
    delete mobreset;
}

