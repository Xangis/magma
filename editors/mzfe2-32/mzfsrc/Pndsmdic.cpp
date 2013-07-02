/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pndsmdic.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of poundsMDIClient (TMDIClient).      
*/


#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include <dir.h>

#include "pounds.h"
#include "pndsmdic.h"
#include "pndsmdi1.h"
#include "apxprint.h"
#include "apxprev.h"


//{{poundsMDIClient Implementation}}


//
// Build a response table for all messages/commands handled
// by poundsMDIClient derived from TMDIClient.
//
DEFINE_RESPONSE_TABLE1(poundsMDIClient, TMDIClient)
//{{poundsMDIClientRSP_TBL_BEGIN}}
    EV_COMMAND(CM_FILEPRINT, CmFilePrint),
    EV_COMMAND(CM_FILEPRINTERSETUP, CmFilePrintSetup),
    EV_COMMAND(CM_FILEPRINTPREVIEW, CmFilePrintPreview),
    EV_COMMAND_ENABLE(CM_FILEPRINT, CmPrintEnable),
    EV_COMMAND_ENABLE(CM_FILEPRINTERSETUP, CmPrintEnable),
    EV_COMMAND_ENABLE(CM_FILEPRINTPREVIEW, CmPrintEnable),
    EV_WM_DROPFILES,
//{{poundsMDIClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//////////////////////////////////////////////////////////
// poundsMDIClient
// ===========
// Construction/Destruction handling.
 poundsMDIClient::poundsMDIClient ()
 : TMDIClient ()
{
    // Change the window's background color
    SetBkgndColor(RGB(0xff, 0xff, 0xff));

    ChildCount = 0;

    // INSERT>> Your constructor code here.

}


 poundsMDIClient::~poundsMDIClient ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


//////////////////////////////////////////////////////////
// poundsMDIClient
// ===========
// MDIClient site initialization.
void poundsMDIClient::SetupWindow ()
{
    // Default SetUpWindow processing.
    TMDIClient::SetupWindow ();

    // Accept files via drag/drop in the client window.
    DragAcceptFiles(TRUE);
}


//////////////////////////////////////////////////////////
// poundsMDIClient
// ==========
// Menu File Print command
void poundsMDIClient::CmFilePrint ()
{
    //
    // Create Printer object if not already created.
    // 
    poundsApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
    if (theApp) {
        if (!theApp->Printer)
            theApp->Printer = new TPrinter;

        //
        // Create Printout window and set characteristics.
        //
        APXPrintOut printout(theApp->Printer, Title, GetActiveMDIChild(), TRUE);

        theApp->Printing++;

        //
        // Bring up the Print dialog and print the document.
        //
        theApp->Printer->Print(GetActiveMDIChild()->GetClientWindow(), printout, TRUE);

        theApp->Printing--;
    }
}


//////////////////////////////////////////////////////////
// poundsMDIClient
// ==========
// Menu File Print Setup command
void poundsMDIClient::CmFilePrintSetup ()
{
    poundsApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
    if (theApp) {
        if (!theApp->Printer)
            theApp->Printer = new TPrinter;

        //
        // Bring up the Print Setup dialog.
        //
        theApp->Printer->Setup(this);
    }
}


//////////////////////////////////////////////////////////
// poundsMDIClient
// ==========
// Menu File Print Preview command
void poundsMDIClient::CmFilePrintPreview ()
{
    poundsApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
    if (theApp) {
        if (!theApp->Printer)
            theApp->Printer = new TPrinter;

        theApp->Printing++;

        PreviewWindow *prevW = new PreviewWindow(Parent, theApp->Printer, GetActiveMDIChild(), "Print Preview", new TLayoutWindow(0));
        prevW->Create();

        GetApplication()->BeginModal(GetApplication()->GetMainWindow());

        // We must destroy the preview window explicitly.  Otherwise, the window will not be destroyed until
        // it's parent the MainWindow is destroyed.
        prevW->Destroy();
        delete prevW;

        theApp->Printing--;
    }
}

#if 0 // @@@
// @@@ Move
class _OWLCLASS TMyPrinter: public TPrinter {
  public:
    TMyPrinter(TApplication* app = 0): TPrinter(app) {}
//    virtual ~TPrinter();

  protected:
    virtual void GetDefaultPrinter();
};

void TMyPrinter::GetDefaultPrinter()
{
  //
  // Have a printDialog go get the default printer information for us
  // into DevMode and DevNames. We never actually Execute() the dialog.
  //
//@@@  SETCLEAR(Data->Flags, 0, PD_RETURNDC);
  Data->Flags |= PD_RETURNDC;
  TPrintDialog printDialog(Application->GetMainWindow(), *Data);
  printDialog.GetDefaultPrinter();
}

// @@@ Move
bool TPrintDialog::GetDefaultPrinter()
{
  pd.Flags |= PD_RETURNDEFAULT;
  Data.ClearDevMode();
  Data.ClearDevNames();
  return DoExecute() == IDOK;
}
#endif

//////////////////////////////////////////////////////////
// poundsMDIClient
// ==========
// Menu enabler used by Print, Print Setup and Print Preview.
void poundsMDIClient::CmPrintEnable (TCommandEnabler &tce)
{
    if (GetActiveMDIChild()) {
        poundsApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), poundsApp);
        if (theApp) {
            // If we have a Printer already created just test if all is okay.
            // Otherwise create a Printer object and make sure the printer
            // really exists and then delete the Printer object.
            if (!theApp->Printer) {
                theApp->Printer = new TPrinter;

                tce.Enable(theApp->Printer->GetSetup().Error == 0);
            } else
                tce.Enable(theApp->Printer->GetSetup().Error == 0);
        }
    } else
        tce.Enable(FALSE);
}


void poundsMDIClient::EvDropFiles (TDropInfo)
{
    Parent->ForwardMessage();
}
