/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         msetdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TMobResetDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "msetdia.h"
#include "appdia.h"
#include "randia.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TMobResetDialog, TDialog)
//{{TMobResetDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDC_ADDINV, CmAddInventory),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDC_PREV, CmPrev),
    EV_BN_CLICKED(IDC_NEXT, CmNext),
    EV_BN_CLICKED(IDC_DELINV, CmDeleteInv),
    EV_BN_CLICKED(IDC_RANDOM, CmAddRandom),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TMobResetDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TMobResetDialog Implementation}}


static TMobResetDialogXfer TMobResetDialogData;

// Make part of class...
static char *szRandomClass[] = { "WEAPON", "ARMOR", "POTION", "MISC" };

TMobResetDialog::TMobResetDialog (TWindow* parent, TMobReset *mobreset,
       BOOL fSet, int nNdx, TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkReset( *mobreset )
{
//{{TMobResetDialogXFER_USE}}
    Mobile = new TComboBox(this, IDC_MOBILE, 1);
    Location = new TComboBox(this, IDC_LOCATION, 1);
    Count = new TEdit(this, IDC_COUNT, sizeof TMobResetDialogData.Count);
    for (int i = 0; i < nelems(Wields); i++)
       Wields[i] = new TComboBox(this, IDC_WIELD_LIGHT + i, 1);
    Inventory = new TListBox(this, IDC_INV);
    ButtonNext = new TButton(this, IDC_NEXT);
    ButtonPrev = new TButton(this, IDC_PREV);
    Comment = new TEdit(this, IDC_RESCOMMENT, sizeof TMobResetDialogData.Comment);
    ButtonRandom = new TButton(this, IDC_RANDOM);
    Wield18Prompt = new TStatic(this, IDC_WEAR_CODE_PROMPT);
    RoomCount = new TEdit(this, IDC_ROOMCOUNT, sizeof TMobResetDialogData.RoomCount);
    RoomCountPrompt = new TStatic(this, IDC_ROOMCOUNTPROMPT);

    SetTransferBuffer(&TMobResetDialogData);
//{{TMobResetDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheMobReset = mobreset;
    nIndex = nNdx;
    fCanSetRoom = fSet;

    LoadTransfer();
}


TMobResetDialog::~TMobResetDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TMobResetDialog::CmAddInventory ()
{
    // INSERT>> Your code here.
    int32 nLoc;
    if (TObjectListDialog(this, WorkReset.AreaDoc, &nLoc).Execute() == IDOK
           && nLoc != CB_ERR)
       {
       char szBuffer[512];
       TReset *r = new TReset(WorkReset.AreaDoc);
#if 0
		 if (nLoc < 1000)
          {
			 nLoc = -nLoc - 1000;
          r->command = 'C';
          r->arg1 = (sh_int) nLoc;
          r->arg2 = 0;		   // @@@ NO NON-CRAPPY RANDOM EQUIPMENT
          WorkReset.Inventory.Add(r);
          wsprintf(szBuffer, "RANDOM %s (%d)", szRandomClass[r->arg1],
             r->arg2);
          }
       else
          {
#endif
          r->command = 'G';
          r->arg1 = (sh_int) nLoc;
          r->arg2 = 100;		// @@@ NO LIMITED OBJECT SUPPORT
          WorkReset.Inventory.Add(r);
          WorkReset.AreaDoc->GetObject((sh_int) nLoc)->GetName(szBuffer,
             sizeof szBuffer);
//          }

       Inventory->SetSelIndex(Inventory->AddString(szBuffer));
       }
}

void TMobResetDialog::LoadTransfer()
{
    TMobResetDialogData.Mobile.Clear();
    WorkReset.AreaDoc->MobilesToCombo(TMobResetDialogData.Mobile,
       WorkReset.arg1);
    TMobResetDialogData.Location.Clear();
    WorkReset.AreaDoc->RoomsToCombo(TMobResetDialogData.Location,
       WorkReset.arg3);
    wsprintf(TMobResetDialogData.Count, "%d", WorkReset.arg2);
    wsprintf(TMobResetDialogData.RoomCount, "%d", WorkReset.arg4);

    static unsigned uWearFlag[] = { 0, ITEM_WEAR_FINGER, ITEM_WEAR_FINGER,
       ITEM_WEAR_NECK, ITEM_WEAR_NECK, ITEM_WEAR_BODY, ITEM_WEAR_HEAD,
       ITEM_WEAR_LEGS, ITEM_WEAR_FEET, ITEM_WEAR_HANDS, ITEM_WEAR_ARMS,
       ITEM_WEAR_SHIELD, ITEM_WEAR_ABOUT, ITEM_WEAR_WAIST, ITEM_WEAR_WRIST,
       ITEM_WEAR_WRIST, ITEM_WIELD, ITEM_HOLD };

    // Calculate what is worn at each position, and the inventory
    TMobResetDialogData.Inventory.Clear();
    for (int i = 0; i < MAX_WEAR; i++)
       Wearing[i] = -1;

    char szBuffer[512];
    for (TArrayAsVectorIterator<TReset *> it(WorkReset.Wields); it; it++)
       {
       TReset *res = it.Current();
//       if (res->command == 'E')
//          {
          // is the item a light?
          const TObject* obj = WorkReset.AreaDoc->GetObject(res->arg1);
          if (obj && obj->item_type == ITEM_LIGHT)
             Wearing[0] = res->arg1;
          else
             // @@@ Check arg3 range here...
             Wearing[res->arg3] = res->arg1;
//          }
       }


    for (TArrayAsVectorIterator<TReset *> it2(WorkReset.Inventory); it2; it2++)
       {
       TReset *res = it2.Current();
       if (res->command == 'G')
          {
          const TObject* obj = WorkReset.AreaDoc->GetObject(res->arg1);
          if (obj)
             obj->GetName(szBuffer, sizeof szBuffer);
          else
             wsprintf(szBuffer, "#%d: Unknown object", res->arg1);
          TMobResetDialogData.Inventory.AddStringItem(szBuffer, res->arg1);
          }
       else // C, F
          {
          wsprintf(szBuffer, "RANDOM %s (%d)", szRandomClass[res->arg1],
             res->arg2);
          // @@@ Bug:  Doesn't allow more powerful random objects...
          TMobResetDialogData.Inventory.AddStringItem(szBuffer, -1000 - res->arg1);
          }
       }

    for (i = 0; i < nelems(Wields); i++)
       {
       TMobResetDialogData.Wields[i].Clear();
       TMobResetDialogData.Wields[i].AddStringItem("[none]", -1,
          (Wearing[i] == -1));
       if (i == 0)
          WorkReset.AreaDoc->ObjectsToComboType(TMobResetDialogData.Wields[i], ITEM_LIGHT,
             Wearing[0]);
       else
          WorkReset.AreaDoc->ObjectsToComboWear(
             TMobResetDialogData.Wields[i], uWearFlag[i], Wearing[i]);
       }

    const char *p = WorkReset.comment.c_str();
    while (*p == '\t' || *p == ' ')
       p++;
    strcpy(TMobResetDialogData.Comment, p);
}

void TMobResetDialog::SaveTransfer()
{
//    int n = Keeper->GetSelIndex();
//    if (n != LB_ERR)
//       WorkShop.keeper = (sh_int) Keeper->GetItemData(n);
    int n = Mobile->GetSelIndex();
    if (n != CB_ERR)
       WorkReset.arg1 = (sh_int) Mobile->GetItemData(n);
    n = Location->GetSelIndex();
    if (n != CB_ERR)
       WorkReset.arg3 = (sh_int) Location->GetItemData(n);
    WorkReset.arg2 = (sh_int) atoi(TMobResetDialogData.Count);
    WorkReset.arg4 = (sh_int) atoi(TMobResetDialogData.RoomCount);

    // @@@ Bug:  None of these report to the document...
//    WorkReset.Wields.Flush();		// Should I delete the things flushed?
    TArrayAsVectorIterator<TReset *> it(WorkReset.Wields);
    for (int i = 0; i < MAX_WEAR; i++)
       {
       n = Wields[i]->GetSelIndex();
       if (n != CB_ERR)
          {
          sh_int nVnumObj = (sh_int) Wields[i]->GetItemData(n);
          if (nVnumObj != Wearing[i])   // Check for additions, deletions, changes
             {
             const TObject *obj;
             if (nVnumObj > 0)
                obj = WorkReset.AreaDoc->GetObject(nVnumObj);

             if (nVnumObj == -1) // Deletion?
                {
                for (it.Restart(); it; it++)
                   {
                   TReset *res = it.Current();
                   if (res->arg1 == Wearing[i] && res->arg3 == i)
                      {
                      WorkReset.Wields.Detach(res);  // @@@ delete?
                      break;
                      }
                   }
                }
             else if (Wearing[i] <= 0) // Addition?
                {
                TReset *res = new TReset(WorkReset.AreaDoc);
                res->command = 'E';		// Equip
                res->arg1 = (sh_int) nVnumObj;
                if (obj)
                   res->arg2 = (sh_int) obj->count;
                else
                   res->arg2 = 100;		// @@@ Sets limit to 100 if non-zone obj!
                res->arg3 = (sh_int) i;
                WorkReset.Wields.Add(res);
                }
             else
                {
                for (it.Restart(); it; it++)
                   {
                   if (it.Current()->arg3 == i)
                      {
                      TReset *res = it.Current();
                      res->arg1 = (sh_int) nVnumObj;
                      if (obj)
                         res->arg2 = (sh_int) obj->count;
                      else
                         res->arg2 = 100;		// @@@ Sets limit to 100 if non-zone obj!
                      break;
                      }
                   }
                }
             }
          }
       }

    WorkReset.comment = TMobResetDialogData.Comment;
    if (WorkReset.comment.is_null())
       {
       const TMobile *pMob = WorkReset.AreaDoc->GetMobile(WorkReset.arg1);
       const TRoom *pLoc = WorkReset.AreaDoc->GetRoom(WorkReset.arg3);
#if 1
       if (pMob && pLoc)
          WorkReset.comment = pMob->short_descr + " to " + pLoc->name;
#else
       char szMob[512];
       char szLoc[512];
       if (pMob)
          pMob->GetName(szMob, sizeof szMob);
       else
          wsprintf(szMob, "#%d: ???", WorkReset.arg1);

       if (pLoc)
          pLoc->GetName(szLoc, sizeof szLoc);
       else
          wsprintf(szLoc, "#%d: ???", WorkReset.arg3);

       WorkReset.comment = string(szMob) + " to " + string(szLoc);
#endif
       }

    WorkReset.AreaDoc->SaveMobReset(WorkReset, nIndex);
#if 1 // @@@
   // Verify a legal wields
   for (TArrayAsVectorIterator<TReset *> it2(WorkReset.Wields); it2; it2++)
      ;
#endif

}


void TMobResetDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}


void TMobResetDialog::CmPrev ()
{
    // INSERT>> Your code here.
    const TMobReset *m;
    if ((m = WorkReset.AreaDoc->GetMobResetFromIndex(nIndex-1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex--;
       LoadAnotherMobReset(m);
       return;
       }

    MessageBeep(-1);
}


void TMobResetDialog::CmNext ()
{
    // INSERT>> Your code here.
    const TMobReset *m;
    if ((m = WorkReset.AreaDoc->GetMobResetFromIndex(nIndex+1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex++;
       LoadAnotherMobReset(m);
       return;
       }

    MessageBeep(-1);
}

void TMobResetDialog::LoadAnotherMobReset(const TMobReset *m)
{
   TheMobReset = m;
   WorkReset = *m;
   LoadTransfer();
   TransferData(tdSetData);
}


void TMobResetDialog::CmDeleteInv ()
{
    // INSERT>> Your code here.
    int n = Inventory->GetSelIndex();
    if (n < 0)
       {
       MessageBeep(-1);
       return;
       }

    WorkReset.Inventory.Detach(n);
    Inventory->DeleteString(n);
}


void TMobResetDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    Location->EnableWindow(fCanSetRoom);
    ButtonNext->EnableWindow(fCanSetRoom);
    ButtonPrev->EnableWindow(fCanSetRoom);

    switch (WorkReset.AreaDoc->GetMode()) {
    case poundsApp::TFCMode:
       ButtonRandom->EnableWindow(TRUE);
       Wield18Prompt->SetText("Pride");
       break;

    case poundsApp::EnvyMode:
    case poundsApp::Envy2Mode:
       Wield18Prompt->SetText("Wield2");
       break;

    case poundsApp::RomMode:
    case poundsApp::Rom24Mode:
       Wield18Prompt->SetText("Float");
       RoomCount->EnableWindow(TRUE);
       RoomCountPrompt->EnableWindow(TRUE);
       break;

    default:
       Wield18Prompt->SetText("");
       Wields[18]->EnableWindow(FALSE);
    }
}


void TMobResetDialog::CmAddRandom ()
{
    // INSERT>> Your code here.
    char Command = 'C';
    int  Type = 0, Level = 0;
    if (TRandomDialog (this, &Command, &Type, &Level).Execute() == IDOK)
       {
       char szBuffer[512];
       TReset *r = new TReset(WorkReset.AreaDoc);
       r->command = Command;
       r->arg1 = (sh_int) Type;
       r->arg2 = (sh_int) Level;
       WorkReset.Inventory.Add(r);
       wsprintf(szBuffer, "RANDOM %s (%d)", szRandomClass[r->arg1],
          r->arg2);

       Inventory->SetSelIndex(Inventory->AddString(szBuffer));
       }
}


void TMobResetDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_MOBRESET);
}

