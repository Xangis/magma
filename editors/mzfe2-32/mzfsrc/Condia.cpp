/*  Project pounds

    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         condia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TContainerDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "condia.h"
#include "appdia.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TContainerDialog, TDialog)
//{{TContainerDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmDone),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_BN_CLICKED(IDC_ADDITEM, CmAddItem),
//{{TContainerDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TContainerDialog Implementation}}


static TContainerDialogXfer TContainerDialogData;

TContainerDialog::TContainerDialog (TWindow* parent, TAreaDocument *doc,
       const TObject *obj, TResId resId, TModule* module):
    TDialog(parent, resId, module),
    TheContainer( obj ),
    AreaDoc( doc )
{
//{{TContainerDialogXFER_USE}}
    Name = new TStatic(this, IDC_NAME, 255);
    Contents = new TListBox(this, IDC_CONTENTS);

    SetTransferBuffer(&TContainerDialogData);
//{{TContainerDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    Name->EnableTransfer();
    obj->GetName(TContainerDialogData.Name, sizeof TContainerDialogData.Name);

    // Figure out the current contents of the container and add to list box
    BOOL fFirst = TRUE;
    TContainerDialogData.Contents.Clear();
    for (TArrayAsVectorIterator<TReset *> itRes(AreaDoc->TheResets); itRes; itRes++)
       {
       TReset *pReset = itRes.Current();
       if (pReset->command == 'P' && pReset->arg3 == obj->vnum)
 		    {
          char szBuffer[512];
          const TObject *pO = AreaDoc->GetObject(pReset->arg1);
          if (pO)
             pO->GetName(szBuffer, sizeof szBuffer);
          else
             wsprintf(szBuffer, "#%d", pReset->arg1);

          TContainerDialogData.Contents.AddString(szBuffer, fFirst);
          fFirst = FALSE;
          }
       }
}


TContainerDialog::~TContainerDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TContainerDialog::CmDone ()
{
    // INSERT>> Your code here.
    CmOk();
}


void TContainerDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_CONTAIN);
}


void TContainerDialog::CmAddItem ()
{
    // INSERT>> Your code here.
    int32 nLoc;
    if (TObjectListDialog(this, AreaDoc, &nLoc).Execute() == IDOK
           && nLoc != CB_ERR)
       {
       // Get the name of the object
       char szBuffer[512];
       const TObject *obj = AreaDoc->GetObject((sh_int) nLoc);
       obj->GetName(szBuffer, sizeof szBuffer);

       // Create a temporary reset for this obj, save it, then delete it
       TReset *r = new TReset(AreaDoc);
       r->command = 'P';
       r->arg1 = (sh_int) nLoc;
       r->arg2 = 100;		// @@@ NO LIMITED OBJECT SUPPORT
		 r->arg3 = TheContainer->vnum;
       r->comment = obj->short_descr + " to container " + TheContainer->name;
       AreaDoc->SaveReset(*r, -1);
       delete r;

		 // Add to the list box
       Contents->SetSelIndex(Contents->AddString(szBuffer));
       }
}

