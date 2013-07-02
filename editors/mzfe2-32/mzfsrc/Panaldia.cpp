/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         panaldia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TPandelDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

// OWL includes
#include <owl\inputdia.h>

// Local includes
#include "panaldia.h"


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TPandelDialog, TDialog)
//{{TPandelDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_BN_CLICKED(IDC_RENUMBER, CmRenumber),
    EV_BN_CLICKED(IDC_TOCLIPBOARD, CmToClipboard),
    EV_BN_CLICKED(IDC_WEAKER, CmWeaker),
    EV_BN_CLICKED(IDC_STRONGER, CmStronger),
    EV_BN_CLICKED(IDC_USEAREADATA, CmUseAreaDataClicked),
//{{TPandelDialogRSP_TBL_END}}
  EV_VN_ISWINDOW,
//  EV_VN_DOCCLOSED,
END_RESPONSE_TABLE;

// @@@ Make part of class
static int nNumberOfDialogs;

//{{TPandelDialog Implementation}}


static TPandelDialogXfer TPandelDialogData;

TPandelDialog::TPandelDialog (TAreaDocument& doc, TWindow* parent, TResId resId, TModule* module):
    TView(doc), TDialog(parent, resId, module)
{
//{{TPandelDialogXFER_USE}}
    Name = new TEdit(this, IDC_NAME, 255);
    RecallPrompt = new TStatic(this, IDC_RECALLPROMPT, 255);
    RecallRoom = new TComboBox(this, IDC_RECALL, 1);
    Toughest = new TStatic(this, IDC_TOUGHEST, 255);
    Weakest = new TStatic(this, IDC_WEAKEST, 255);
    UseAreaData = new TCheckBox(this, IDC_USEAREADATA, 0);
    TimeZone = new TEdit(this, IDC_TIMEZONE, 255);
    Repop = new TEdit(this, IDC_REPOP, 255);
    MaxChange = new TEdit(this, IDC_MAXCHANGE, 255);
    MinTemp = new TEdit(this, IDC_MINTEMP, 255);
    MaxTemp = new TEdit(this, IDC_MAXTEMP, 255);
    MinPrecip = new TEdit(this, IDC_MINPRECIP, 255);
    MaxPrecip = new TEdit(this, IDC_MAXPRECIP, 255);
    MinWind = new TEdit(this, IDC_MINWIND, 255);
    MaxWind = new TEdit(this, IDC_MAXWIND, 255);
    MinMagic = new TEdit(this, IDC_MINMAGIC, 255);
    MaxMagic = new TEdit(this, IDC_MAXMAGIC, 255);
    ExitCohere = new TCheckBox(this, IDC_EXITCOHER, 0);

    SetTransferBuffer(&TPandelDialogData);
//{{TPandelDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    AreaDoc = &doc;
    if (!AreaDoc->IsOpen())
       AreaDoc->Open(ofRead|ofWrite);


    nNumberOfDialogs++;
    strcpy(TPandelDialogData.Name, doc.name.c_str());
    Weakest->EnableTransfer();
    Toughest->EnableTransfer();

	 // @@@ Needs to be moved to a LoadData() so the Vn notifications work...
    const TMobile *mobWeakest = 0, *mobStrongest = 0, *mob;
    int nNdx = 0;
    do {
       if ((mob = doc.GetMobileFromIndex(nNdx)) == 0)
          break;
       else
          {
          if (!mobWeakest || mobWeakest->level > mob->level)
             mobWeakest = mob;
          if (!mobStrongest || mobStrongest->level < mob->level)
             mobStrongest = mob;
          }
       nNdx++;
       } while (mob);

    if (mobWeakest)
       mobWeakest->GetName(TPandelDialogData.Weakest,
          sizeof TPandelDialogData.Weakest);
    else
       TPandelDialogData.Weakest[0] = '\0';

    if (mobStrongest)
       mobStrongest->GetName(TPandelDialogData.Toughest,
          sizeof TPandelDialogData.Toughest);
    else
       TPandelDialogData.Toughest[0] = '\0';

//    if (AreaDoc->GetMode() == poundsApp::EnvyMode)
//       {
       TPandelDialogData.RecallRoom.Clear();
       TPandelDialogData.RecallRoom.AddStringItem("[Temple/Healer]", -1,
          (AreaDoc->recall == -1));
       AreaDoc->RoomsToCombo(TPandelDialogData.RecallRoom, AreaDoc->recall);
//       }

    // Troll stuff
    TPandelDialogData.UseAreaData = AreaDoc->fUsesAreaData;
    wsprintf(TPandelDialogData.TimeZone, "%d", AreaDoc->area_data.timezone);
    strcpy(TPandelDialogData.Repop, AreaDoc->area_data.repop.c_str());
    wsprintf(TPandelDialogData.MaxChange, "%d",
       AreaDoc->area_data.Weather.MaxChange);
    wsprintf(TPandelDialogData.MinTemp, "%d",
       AreaDoc->area_data.Weather.MinTemp);
    wsprintf(TPandelDialogData.MaxTemp, "%d",
       AreaDoc->area_data.Weather.MaxTemp);
    wsprintf(TPandelDialogData.MinPrecip, "%d",
       AreaDoc->area_data.Weather.MinPrecip);
    wsprintf(TPandelDialogData.MaxPrecip, "%d",
       AreaDoc->area_data.Weather.MaxPrecip);
    wsprintf(TPandelDialogData.MinWind, "%d",
       AreaDoc->area_data.Weather.MinWind);
    wsprintf(TPandelDialogData.MaxWind, "%d",
       AreaDoc->area_data.Weather.MaxWind);
    wsprintf(TPandelDialogData.MinMagic, "%d",
       AreaDoc->area_data.Weather.MinMagic);
    wsprintf(TPandelDialogData.MaxMagic, "%d",
       AreaDoc->area_data.Weather.MaxMagic);
    TPandelDialogData.ExitCohere =
       (AreaDoc->area_data.area_flags & AREA_NO_FIX_EXITS) == AREA_NO_FIX_EXITS;

}


TPandelDialog::~TPandelDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    nNumberOfDialogs--;
}


void TPandelDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    AreaDoc->name = TPandelDialogData.Name;
    AreaDoc->recall = (sh_int)
       RecallRoom->GetItemData(RecallRoom->GetSelIndex());

    // Troll stuff
    AreaDoc->fUsesAreaData = TPandelDialogData.UseAreaData;
    AreaDoc->area_data.timezone = atoi(TPandelDialogData.TimeZone);
    AreaDoc->area_data.repop = TPandelDialogData.Repop;

    AreaDoc->area_data.fCustomWeather = TRUE; // @@@ Should check all below...
    AreaDoc->area_data.Weather.MaxChange = atoi(TPandelDialogData.MaxChange);
    AreaDoc->area_data.Weather.MinTemp   = atoi(TPandelDialogData.MinTemp);
    AreaDoc->area_data.Weather.MaxTemp   = atoi(TPandelDialogData.MaxTemp);
    AreaDoc->area_data.Weather.MinPrecip = atoi(TPandelDialogData.MinPrecip);
    AreaDoc->area_data.Weather.MaxPrecip = atoi(TPandelDialogData.MaxPrecip);
    AreaDoc->area_data.Weather.MinWind   = atoi(TPandelDialogData.MinWind);
    AreaDoc->area_data.Weather.MaxWind   = atoi(TPandelDialogData.MaxWind);
    AreaDoc->area_data.Weather.MinMagic  = atoi(TPandelDialogData.MinMagic);
    AreaDoc->area_data.Weather.MaxMagic  = atoi(TPandelDialogData.MaxMagic);

    SET_RESET_BIT(AreaDoc->area_data.area_flags, TPandelDialogData.ExitCohere,
       AREA_NO_FIX_EXITS);

    AreaDoc->SetDirty(TRUE);

    TDialog::CmOk();
}

BOOL TPandelDialog::Create()
{
  TRY {
    TDialog::Create();   // throws exception TWindow::TXWindow
  }
  CATCH( (TXOwl& x) {
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    return TRUE;   // cannot return FALSE - throws another exception
  })
#if 0
  if (Doc->GetDocPath() == 0) {
    return TRUE;           // new file, no data to display
  }
  if (!LoadData())
	 NotOK();
#endif
//  if (nNumberOfDialogs != 1)
//     NotOK();
  return TRUE;
}


void TPandelDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    BOOL fRecall = (AreaDoc->GetMode() == poundsApp::EnvyMode || AreaDoc->GetMode() == poundsApp::Envy2Mode);
    RecallPrompt->EnableWindow(fRecall);
    RecallRoom->EnableWindow(fRecall);
    CmUseAreaDataClicked();
}

BOOL TPandelDialog::VnIsWindow(HWND hWnd)
{
   return ((HWindow == hWnd) || IsChild(hWnd));
}


void TPandelDialog::CmRenumber ()
{
    // INSERT>> Your code here.
    char szBuffer[8] = "9900";
    if (TInputDialog(0, "MAKE.ZONES.FAST", "Enter new beginning VNUM",
          szBuffer, sizeof szBuffer).Execute() != IDOK)
       return;

    int n = atoi(szBuffer);
    if (n <= 0 || n > 32700)
       {
       MessageBox("That is not a valid VNUM.", "MAKE.ZONES.FAST",
          MB_ICONHAND|MB_OK);
       return;
       }

    AreaDoc->TAreaDocument::Renumber( (sh_int) n );
}


void TPandelDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT,
       IDD_PANELDIALOG);
}


void TPandelDialog::CmToClipboard ()
{
    // INSERT>> Your code here.
    TClipboard& cb = OpenClipboard();

    // Allocate a meg in case we need that much
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, 1000000L);
	 char HUGE *szBuffer = (char HUGE *) GlobalLock(hGlobal);
	 char HUGE *p = szBuffer;

    const TMobile *mob;
    int nNdx = 0;
    do {
       if ((mob = AreaDoc->GetMobileFromIndex(nNdx)) == 0)
          break;
       else
			 p = mob->MobileToText(p);

       nNdx++;
       } while (mob);

    const TObject *obj;
    nNdx = 0;
    do {
       if ((obj = AreaDoc->GetObjectFromIndex(nNdx)) == 0)
          break;
       else
       	 p = obj->ObjToText(p);

       nNdx++;
       } while (obj);

    const TRoom *room;
    nNdx = 0;
    do {
       if ((room = AreaDoc->GetRoomFromIndex(nNdx)) == 0)
          break;
       else
       	 p = room->RoomToText(p);

       nNdx++;
       } while (room);

    GlobalUnlock(hGlobal);
	 GlobalReAlloc(hGlobal, p - szBuffer, GMEM_MOVEABLE);
    cb.SetClipboardData(CF_TEXT, hGlobal);
    cb.CloseClipboard();
}


void TPandelDialog::CmWeaker ()
{
    // INSERT>> Your code here.
    const TMobile *mob;
    int nNdx = 0;
    do {
       if ((mob = AreaDoc->GetMobileFromIndex(nNdx)) == 0)
          break;
       else
          {
          TMobile mobNew(*mob);
			 mobNew.level -= (sh_int)((mob->level + 9) / 10);
          AreaDoc->MobileSave(mobNew, nNdx);
          }
       nNdx++;
       } while (mob);
}


void TPandelDialog::CmStronger ()
{
    // INSERT>> Your code here.
    const TMobile *mob;
    int nNdx = 0;
    do {
       if ((mob = AreaDoc->GetMobileFromIndex(nNdx)) == 0)
          break;
       else
          {
          TMobile mobNew(*mob);
          mobNew.level += (sh_int)((mob->level + 9) / 10);
          AreaDoc->MobileSave(mobNew, nNdx);
          }
       nNdx++;
       } while (mob);
}


void TPandelDialog::CmUseAreaDataClicked ()
{
    // INSERT>> Your code here.
    BOOL f = (UseAreaData->GetCheck() == BF_CHECKED);
    TimeZone->EnableWindow(f);
    Repop->EnableWindow(f);
    MaxChange->EnableWindow(f);
    MinTemp->EnableWindow(f);
    MaxTemp->EnableWindow(f);
    MinPrecip->EnableWindow(f);
    MaxPrecip->EnableWindow(f);
    MinWind->EnableWindow(f);
    MaxWind->EnableWindow(f);
    MinMagic->EnableWindow(f);
    MaxMagic->EnableWindow(f);
    ExitCohere->EnableWindow(f);
}

//bool TPandelDialog::VnDocClosed(int omode)
//{
//  if (!(omode & ofWrite))  // make sure someone else's write
//    return false;
//  return true;
//}

