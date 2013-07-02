/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         typedia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TFileFormatDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "typedia.h"
#include "objmerc.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TFileFormatDialog, TDialog)
//{{TFileFormatDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
//    EV_BN_CLICKED(IDC_ENVY, CmModeChanged),
//    EV_BN_CLICKED(IDC_TFCMUD, CmModeChanged),
//    EV_BN_CLICKED(IDC_MERC, CmModeChanged),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_BN_CLICKED(IDC_SHORTER, CmShorter),
    EV_BN_CLICKED(IDC_TALLER, CmTaller),
    EV_BN_CLICKED(IDC_THINNER, CmThinner),
    EV_BN_CLICKED(IDC_WIDER, CmWider),
//{{TFileFormatDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TFileFormatDialog Implementation}}


static TFileFormatDialogXfer TFileFormatDialogData;

TFileFormatDialog::TFileFormatDialog (TWindow* parent, TResId resId, TModule* module):
    TDialog(parent, resId, module),
    hTestFixedFont( 0 ),
    xFont( poundsApp::xFont ),
    yFont( poundsApp::yFont )
{
//{{TFileFormatDialogXFER_USE}}
    MercMode = new TRadioButton(this, IDC_MERC, 0);
    TFCMode = new TRadioButton(this, IDC_TFCMUD, 0);
    EnvyMode = new TRadioButton(this, IDC_ENVY, 0);
    MobProgs = new TCheckBox(this, IDC_MOBPROGS, 0);
    RomMode = new TRadioButton(this, IDC_ROM, 0);
    Envy20Mode = new TRadioButton(this, IDC_ENVY20, 0);
    OghamMode = new TRadioButton(this, IDC_OGHAM, 0);
    Rom24Mode = new TRadioButton(this, IDC_ROM24, 0);

    SetTransferBuffer(&TFileFormatDialogData);
//{{TFileFormatDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    MuditorApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
    TFileFormatDialogData.MercMode =
       (MuditorApp->CurrentMode == poundsApp::MercMode);
    TFileFormatDialogData.TFCMode =
       (MuditorApp->CurrentMode == poundsApp::TFCMode);
    TFileFormatDialogData.EnvyMode =
       (MuditorApp->CurrentMode == poundsApp::EnvyMode);
    TFileFormatDialogData.RomMode =
       (MuditorApp->CurrentMode == poundsApp::RomMode);
    TFileFormatDialogData.Envy20Mode =
       (MuditorApp->CurrentMode == poundsApp::Envy2Mode);
    TFileFormatDialogData.OghamMode =
       (MuditorApp->CurrentMode == poundsApp::OghamMode);
    TFileFormatDialogData.Rom24Mode =
       (MuditorApp->CurrentMode == poundsApp::Rom24Mode);

    TFileFormatDialogData.MobProgs = MuditorApp->fMOBprograms;
}


TFileFormatDialog::~TFileFormatDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    delete hTestFixedFont;
}


void TFileFormatDialog::CmOk ()
{
    // INSERT>> Your code here.
    if (MercMode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::MercMode;
       MuditorApp->SetMode(poundsApp::MercMode);
    else if (TFCMode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::TFCMode;
       MuditorApp->SetMode(poundsApp::TFCMode);
    else if (EnvyMode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::EnvyMode;
       MuditorApp->SetMode(poundsApp::EnvyMode);
    else if (Envy20Mode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::Envy2Mode;
       MuditorApp->SetMode(poundsApp::Envy2Mode);
    else if (OghamMode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::OghamMode;
       MuditorApp->SetMode(poundsApp::OghamMode);
    else if (Rom24Mode->GetCheck() == BF_CHECKED)
//       MuditorApp->CurrentMode = poundsApp::Rom24Mode;
       MuditorApp->SetMode(poundsApp::Rom24Mode);
    else
//       MuditorApp->CurrentMode = poundsApp::RomMode;
       MuditorApp->SetMode(poundsApp::RomMode);

    MuditorApp->fMOBprograms = (MobProgs->GetCheck() == BF_CHECKED);
// This error message was getting annoying
//    if (MuditorApp->fMOBprograms)
//       MessageBox("Notice:  If you are using MOBprograms in .prg files, they "
//          "will be moved into the .are file when you save your zone.",
//          "M.Z.F.", MB_ICONINFORMATION|MB_OK);

	 char szBuffer[16];
    if (xFont != poundsApp::xFont || yFont != poundsApp::yFont)
       {
       wsprintf(szBuffer, "%d", xFont);
       WriteProfileString("MAKE.ZONES.FAST", "xFont", szBuffer);
       wsprintf(szBuffer, "%d", yFont);
       WriteProfileString("MAKE.ZONES.FAST", "yFont", szBuffer);

       MessageBox("You must exit and restart MZF for your font-size changes to "
          "go into effect.", "M.Z.F", MB_ICONINFORMATION|MB_OK);
       }

    TDialog::CmOk();
}


void TFileFormatDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    FontSizeChanged();
//    CmModeChanged();
}

void TFileFormatDialog::FontSizeChanged ()
{
    ::SendDlgItemMessage(HWindow, IDC_TESTFONT, WM_SETFONT,
       (WPARAM) (HFONT) GetStockObject(ANSI_FIXED_FONT), MAKELPARAM(FALSE, 0));

    delete hTestFixedFont;
    hTestFixedFont = new TFont("Courier New", yFont, xFont);

    ::SendDlgItemMessage(HWindow, IDC_TESTFONT, WM_SETFONT,
       (WPARAM) (HFONT) *hTestFixedFont, MAKELPARAM(TRUE, 0));
}

//void TFileFormatDialog::CmModeChanged ()
//{
//    // INSERT>> Your code here.
//    MobProgs->EnableWindow(TFCMode->GetCheck() == BF_UNCHECKED);
//}


void TFileFormatDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT,
       IDD_FTYPEDIALOG);
}


void TFileFormatDialog::CmShorter ()
{
    // INSERT>> Your code here.
    if (yFont < -1)
       {
       yFont++;
       FontSizeChanged();
       }
    else
       MessageBeep(-1);
}


void TFileFormatDialog::CmTaller ()
{
    // INSERT>> Your code here.
       yFont--;
       FontSizeChanged();
}


void TFileFormatDialog::CmThinner ()
{
    // INSERT>> Your code here.
    if (yFont < -1)
       {
       xFont++;
       FontSizeChanged();
       }
    else
       MessageBeep(-1);
}


void TFileFormatDialog::CmWider ()
{
    // INSERT>> Your code here.
       xFont--;
       FontSizeChanged();
}



