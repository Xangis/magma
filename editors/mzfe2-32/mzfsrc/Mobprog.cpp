/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         mobprog.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TMobProgDia (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "mobprog.h"
#include "pounds.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TMobProgDia, TDialog)
//{{TMobProgDiaRSP_TBL_BEGIN}}
    EV_LBN_SELCHANGE(IDC_TRIGGERS, CmTriggerSelChange),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TMobProgDiaRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TMobProgDia Implementation}}


static TMobProgDiaXfer TMobProgDiaData;

TMobProgDia::TMobProgDia (TWindow* parent, TMobile *pMob,
      TArrayAsVector<TMobProgData*>& mobprogs, TResId resId, TModule* module):
   TDialog(parent, resId, module),
   orig( &mobprogs ),
   newprogs( 1, 0, 5 )
{
//{{TMobProgDiaXFER_USE}}
    Triggers = new TListBox(this, IDC_TRIGGERS);
    Program = new TEdit(this, IDC_PROGRAM, 2048);
    TheMobile = new TStatic(this, IDC_THEMOBILE, 255);
    FileName = new TStatic(this, IDC_FILENAME, 255);

    SetTransferBuffer(&TMobProgDiaData);
//{{TMobProgDiaXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheMob = pMob;

    // Copy the mobprogs & init the dialog box
    TheMobile->EnableTransfer();
    FileName->EnableTransfer();
    
    char szBuffer[256];
    BOOL fFirst = TRUE;
    TMobProgDiaData.Triggers.Clear();
    for (TArrayAsVectorIterator<TMobProgData*> it(mobprogs); it; it++)
       {
       wsprintf(szBuffer, "%s %s", it.Current()->type.c_str(),
          it.Current()->arglist.c_str());
       TMobProgDiaData.Triggers.AddString(szBuffer, fFirst);

       if (fFirst)
          {
          strcpy(TMobProgDiaData.FileName, it.Current()->MobFile.c_str());
          strcpy(TMobProgDiaData.Program, it.Current()->comlist.c_str());
          fFirst = FALSE;
          }

       newprogs.Add(new TMobProgData(*it.Current()));
       }

    pMob->GetName(TMobProgDiaData.TheMobile,
       sizeof (TMobProgDiaData.TheMobile));
}


TMobProgDia::~TMobProgDia ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TMobProgDia::CmTriggerSelChange ()
{
    // INSERT>> Your code here.
    int n = Triggers->GetSelIndex();
    if (n >= 0)
       {
       const TMobProgData *mprg = newprogs[n];
       Program->SetCaption(mprg->comlist.c_str());
       FileName->SetCaption(mprg->MobFile.c_str());
       }
}


void TMobProgDia::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_MOBPROG);
}

