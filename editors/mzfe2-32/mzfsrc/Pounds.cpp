/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pounds.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of poundsApp (TApplication).      
*/


#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include <dir.h>

#include "pounds.h"
#include "pndsmdic.h"
#include "pndsmdi1.h"
#include "pndsabtd.h"                        // Definition of about dialog.
#include "areaview.h"
#include "mapview.h"
#include "typedia.h"
#include "panaldia.h"
#include "objlib.h"
#include "objdia.h"

TLibrary* CreateLibrary(poundsApp::currMode mode);

//
// Generated help file.
//
const char HelpFileName[] = "pounds.hlp";

const char *poundsApp::szModesShort[] = {
   "Merc2",
   "TFC1",
   "Envy1",
   "ROM23",
   "Envy2",
   "Ogham16NT",
   "ROM24"
};

#define MAX_CUSTOM_SPELLS 128

// Drag / Drop support:
TFileDrop::TFileDrop (char* fileName, TPoint& p, BOOL inClient, TModule*)
{
    char    exePath[MAXPATH];
    
    exePath[0] = 0;
    FileName = strcpy(new char[strlen(fileName) + 1], fileName);
    Point = p;
    InClientArea = inClient;
}

TFileDrop::~TFileDrop ()
{
    delete FileName;
}

const char *TFileDrop::WhoAmI ()
{
  return FileName;
}


//{{poundsApp Implementation}}

//{{DOC_VIEW}}
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TEditView, DocType1);
//{{DOC_VIEW_END}}
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TRoomListView, DocType2);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TObjectListView, DocType3);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TMobileListView, DocType4);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, THelpListView, DocType5);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TShopListView, DocType6);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TResetListView, DocType7);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TMobResetListView, DocType8);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TMapView, DocType9);
DEFINE_DOC_TEMPLATE_CLASS(TAreaDocument, TPandelDialog, DocType10);

//{{DOC_MANAGER}}
//DocType1 __dvt1("All Files (*.*)", "*.*", 0, "TXT", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtHidden);
//{{DOC_MANAGER_END}}
DocType2 __dvt2("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen);
DocType3 __dvt3("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType4 __dvt4("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType5 __dvt5("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType6 __dvt6("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType7 __dvt7("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType8 __dvt8("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType9 __dvt9("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);
DocType10 __dvt10("Area Files (*.are)", "*.are", 0, "ARE", dtOverwritePrompt | dtAutoDelete | dtUpdateDir | dtAutoOpen | dtHidden);

BOOL poundsApp::HelpState;                   // Has the help engine been used.

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(poundsApp, TApplication)
//{{poundsAppRSP_TBL_BEGIN}}
    EV_OWLVIEW(dnCreate, EvNewView),
    EV_OWLVIEW(dnClose,  EvCloseView),
    EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
    EV_COMMAND(CM_HELPCONTENTS, CmHelpContents),
    EV_COMMAND(CM_HELPUSING, CmHelpUsing),
    EV_WM_DROPFILES,
    EV_WM_WININICHANGE,
    EV_COMMAND(CM_HELPFORMAT, CmFileFormat),
    EV_COMMAND(CM_HELPCREDITS, CmHelpCredits),
    EV_COMMAND(CM_HELPMERC, CmHelpMerc),
    EV_COMMAND(CM_HELPENVY, CmHelpEnvy),
    EV_COMMAND(CM_HELPROM, CmHelpRom),
//{{poundsAppRSP_TBL_END}}
END_RESPONSE_TABLE;

int poundsApp::xFont = GetProfileInt("MAKE.ZONES.FAST", "xFont", 0);
int poundsApp::yFont = GetProfileInt("MAKE.ZONES.FAST", "yFont", 0);

//////////////////////////////////////////////////////////
// poundsApp
// =====
//
poundsApp::poundsApp () : TApplication("MAKE.ZONES.FAST"),
   hFixedFont("Courier New", yFont, xFont)
{
    HelpState = FALSE;
    ContextHelp = FALSE;
    HelpCursor = 0;

    Printer = 0;
    Printing = 0;

    DocManager = new TDocManager(dmMDI | dmMenu);

    // INSERT>> Your constructor code here.
    CurrentMode = (currMode)
       GetProfileInt("MAKE.ZONES.FAST", "Mode", (int) MercMode);

    // Initialize the library
    m_Library = 0;
    SetMode(CurrentMode);

    fMOBprograms = GetProfileInt("MAKE.ZONES.FAST", "MOBprogs", TRUE);

    // Read the 'spell customization' from the ini file.  These apply to
    // all modes
    nSlotsUsed = 0;
    ppCustomSpells = new SpellAndSlot*[MAX_CUSTOM_SPELLS];
    char szBuffer[32], szSpellInfo[64];
    for (int i = 0; i < MAX_CUSTOM_SPELLS; i++)
       {
       wsprintf(szBuffer, "spell%d", i);
       GetProfileString("MAKE.ZONES.FAST", szBuffer, "0", szSpellInfo,
          sizeof szSpellInfo);
       char *p = strchr(szSpellInfo, '/');
       if (p)
          ppCustomSpells[nSlotsUsed++] = new SpellAndSlot(atoi(szSpellInfo), p+1);
       }
    for (i = nSlotsUsed; i < MAX_CUSTOM_SPELLS; i++)
       ppCustomSpells[i] = 0;
}

// Return the spell using slot nSlot
const char *poundsApp::SpellName(int nSlot) const
{
   for (int i = 0; i < MAX_CUSTOM_SPELLS; i++)
      if (ppCustomSpells[i] && nSlot == ppCustomSpells[i]->nSlot)
         return ppCustomSpells[i]->strName.c_str();

   return 0;
}

int poundsApp::SpellValue(const char *p)
{
   for (int i = 0; i < MAX_CUSTOM_SPELLS; i++)
      if (ppCustomSpells[i] && stricmp(p, ppCustomSpells[i]->strName.c_str()) == 0)
         return ppCustomSpells[i]->nSlot;

   return 0;
}

// Return the nth spell
const char *poundsApp::SpellIndex(int nNdx, int *nSlot) const
{
   *nSlot = ppCustomSpells[nNdx]->nSlot;
   return ppCustomSpells[nNdx]->strName.c_str();
}

poundsApp::~poundsApp ()
{
    m_Library->Save();
    delete m_Library;

    // @@@ The following bit of code has three problems.  It tries to
    // GlobalFree() two pieces of memory it isn't allowed to, and it tries to
    // delete a dc that is invalid.  It got these values inside the constructor
    // for TPrinter, which has a TPrintDialog::TData structure which is filled
    // in with invalid (?) values.
    if (Printer)
        delete Printer;

    for (int i = 0; i < MAX_CUSTOM_SPELLS; i++)
       delete ppCustomSpells[i];

    delete[] ppCustomSpells;

    // INSERT>> Your destructor code here.
    char szBuffer[16];
    wsprintf(szBuffer, "%d", CurrentMode);
    WriteProfileString("MAKE.ZONES.FAST", "Mode", szBuffer);
    wsprintf(szBuffer, "%d", fMOBprograms);
    WriteProfileString("MAKE.ZONES.FAST", "MOBprogs", szBuffer);
}

#if 0
TPrintDialog::TData::~TData()
{
  if (hDevMode) {
    ::GlobalUnlock(hDevMode);
//@@@    ::GlobalFree(hDevMode);
  }
  if (hDevNames) {
    ::GlobalUnlock(hDevNames);
//@@@    ::GlobalFree(hDevNames);
  }
  if (hDC)
    ::DeleteDC(hDC);
}

//TPrinter::~TPrinter()
//{
//  delete Data;
//}

//TPrinter::TPrinter(TApplication* app)
//{
//  Data = new TPrintDialog::TData;
//  Error = 0;
//  Application = app ? app : ::GetApplicationObject();
//  CHECK(Application);
//  GetDefaultPrinter();
//}

//
// Updata Data with info on the user's default printer.
//
//void
//TPrinter::GetDefaultPrinter()
//{
//  //
//  // Have a printDialog go get the default printer information for us
//  // into DevMode and DevNames. We never actually Execute() the dialog.
//  //
////@@@  SETCLEAR(Data->Flags, 0, PD_RETURNDC);
//  Data->Flags |= PD_RETURNDC;
//  TPrintDialog printDialog(Application->GetMainWindow(), *Data);
//  printDialog.GetDefaultPrinter();
//}

//TPrintDialog::TPrintDialog(TWindow*        parent,
//                           TData&          data,
//                           const char far* printTemplateName,
//                           const char far* setupTemplateName,
//                           const char far* title,
//                           TModule*        module)
//:
//  TCommonDialog(parent, title, module),
//  Data(data)
//{
//  memset(&pd, 0, sizeof(PRINTDLG));
//  pd.lStructSize = sizeof(PRINTDLG);
//  pd.hwndOwner = Parent ? Parent->HWindow : 0;
//  pd.hInstance = *GetModule();
//  pd.Flags = PD_ENABLEPRINTHOOK | PD_ENABLESETUPHOOK | Data.Flags;
//  pd.Flags &= ~PD_RETURNDEFAULT;
//
//  if (printTemplateName) {
//    (LPCSTR)pd.lpPrintTemplateName = printTemplateName;
//    pd.Flags |= PD_ENABLEPRINTTEMPLATE;
//  }
//  else
//    pd.Flags &= ~PD_ENABLEPRINTTEMPLATE;
//
//  if (setupTemplateName) {
//    (LPCSTR)pd.lpSetupTemplateName = setupTemplateName;
//    pd.Flags |= PD_ENABLESETUPTEMPLATE;
//  }
//  else
//    pd.Flags &= ~PD_ENABLESETUPTEMPLATE;
//
//  pd.lpfnPrintHook = 0;
//  pd.lpfnSetupHook = 0;
//
//  pd.nFromPage = (uint16)Data.FromPage;
//  pd.nToPage = (uint16)Data.ToPage;
//  pd.nMinPage = (uint16)Data.MinPage;
//  pd.nMaxPage = (uint16)Data.MaxPage;
//  pd.nCopies = (uint16)Data.Copies;
//}

bool TPrintDialog::GetDefaultPrinter()
{
  pd.Flags |= PD_RETURNDEFAULT | PD_RETURNDC; // @@@ PD_RETURNDC

  Data.ClearDevMode();
  Data.ClearDevNames();
  bool stat = (DoExecute() == IDOK);
  return stat;
}
#endif

BOOL poundsApp::CanClose ()
{
    BOOL result = TApplication::CanClose();

    //
    // Close the help engine if we used it.
    //
    if (result && HelpState)
        GetMainWindow()->WinHelp(HelpFileName, HELP_QUIT, 0L);

    return result;
}


void poundsApp::SetupSpeedBar (TDecoratedMDIFrame *frame)
{
    //
    // Create default toolbar New and associate toolbar buttons with commands.
    //
//    TControlBar* cb = new TControlBar(frame);
	 TControlBar* cb = new TTipControlBar(tooltip, frame);
	 cb->Insert(*new TButtonGadget(CM_MDIFILENEW, CM_MDIFILENEW));
	 cb->Insert(*new TButtonGadget(CM_MDIFILEOPEN, CM_MDIFILEOPEN));
	 cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_EDITCUT, CM_EDITCUT));
    cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY));
//    cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE));
//    cb->Insert(*new TSeparatorGadget(6));
//    cb->Insert(*new TButtonGadget(CM_EDITUNDO, CM_EDITUNDO));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_VIEWCREATE, CM_VIEWCREATE));
    cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
    cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));
    cb->Insert(*new TButtonGadget(CM_EDITADD, CM_EDITADD));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_FILEPRINT, CM_FILEPRINT));
    cb->Insert(*new TButtonGadget(CM_FILEPRINTPREVIEW, CM_FILEPRINTPREVIEW));
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_HELPCONTENTS, CM_HELPCONTENTS));

    // Add fly-over help hints.
    cb->SetHintMode(TGadgetWindow::EnterHints);

    frame->Insert(*cb, TDecoratedFrame::Top);
}


//////////////////////////////////////////////////////////
// poundsApp
// =====
// Application intialization.
//
void poundsApp::InitMainWindow ()
{
    mdiClient = new poundsMDIClient;
    TDecoratedMDIFrame* frame = new TDecoratedMDIFrame(Name, MDI_MENU, *mdiClient, TRUE);

    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

    //
    // Assign ICON w/ this application.
    //
//    frame->SetIcon(this, IDI_MDIAPPLICATION);
    frame->SetIcon(this, IDI_RESETS);

    //
    // Menu associated with window and accelerator table associated with table.
    //
    frame->AssignMenu(MDI_MENU);

    //
    // Associate with the accelerator table.
	 //
	 frame->Attr.AccelTable = MDI_MENU;


	 SetupSpeedBar(frame);

	 TStatusBar *sb = new TTipStatusBar(tooltip, frame, TGadget::Recessed,
//	 TStatusBar *sb = new TStatusBar(frame, TGadget::Recessed,
												TStatusBar::CapsLock        |
												TStatusBar::NumLock         |
												TStatusBar::ScrollLock      |
												TStatusBar::Overtype);
	 frame->Insert(*sb, TDecoratedFrame::Bottom);

	 SetMainWindow(frame);

	 //
    // Windows 3-D controls.
    //
    EnableCtl3d(TRUE);
}


//////////////////////////////////////////////////////////
// poundsApp
// =====
// Response Table handlers:
//
void poundsApp::EvNewView (TView& view)
{
    TMDIClient *mdiClient = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), TMDIClient);
    if (mdiClient) {
//        BOOL fControlPanel = (TYPESAFE_DOWNCAST(&view, TPandelDialog)) ?
//           TRUE : FALSE;
        BOOL fControlPanel = (TYPESAFE_DOWNCAST(&view, TDialog)) ?
           TRUE : FALSE;
        poundsMDIChild* child = new poundsMDIChild(*mdiClient,
           view.GetViewName(), // caption
           view.GetWindow(),
           fControlPanel);

        // Associate ICON w/ this child window.
        if (TYPESAFE_DOWNCAST(&view, TRoomListView))
           child->SetIcon(this, IDI_ROOMS);
        else if (TYPESAFE_DOWNCAST(&view, TMobileListView))
           child->SetIcon(this, IDI_MOBILES);
        else if (TYPESAFE_DOWNCAST(&view, TObjectListView))
           child->SetIcon(this, IDI_OBJECTS);
        else if (TYPESAFE_DOWNCAST(&view, TResetListView))
           child->SetIcon(this, IDI_RESETS);
        else if (TYPESAFE_DOWNCAST(&view, TMobResetListView))
           child->SetIcon(this, IDI_RESETS);
        else if (TYPESAFE_DOWNCAST(&view, THelpListView))
           child->SetIcon(this, IDI_HELPS);
        else if (TYPESAFE_DOWNCAST(&view, TShopListView))
           child->SetIcon(this, IDI_SHOP);
        else if (TYPESAFE_DOWNCAST(&view, TMapView))
           child->SetIcon(this, IDI_ROOMS);
        else if (fControlPanel)
           {
           child->Attr.Style &= ~(WS_THICKFRAME|WS_MAXIMIZEBOX);
//           child->Attr.Style |= (WS_HSCROLL | LBS_NOINTEGRALHEIGHT);
           child->SetIcon(this, IDI_PANEL); // @@@
           }
        else
           child->SetIcon(this, IDI_DOC);
        // @@@ Need icon for control panel and a few others

        child->Create();
    }
//    if (Printer)
//       MessageBox(0, "Valid 'Printer' member", "@@@", MB_OK);
}


void poundsApp::EvCloseView (TView&)
{
}


//////////////////////////////////////////////////////////
// poundsApp
// =====
// Menu Help Contents command
void poundsApp::CmHelpContents ()
{
    //
    // Show the help table of contents.
    //
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTENTS, 0L);
}


//////////////////////////////////////////////////////////
// poundsApp
// =====
// Menu Help Using Help command
void poundsApp::CmHelpUsing ()
{
    //
    // Display the contents of the Windows help file.
    //
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_HELPONHELP, 0L);
}


//////////////////////////////////////////////////////////
// poundsApp
// ===========
// Menu Help About pounds.exe command
void poundsApp::CmHelpAbout ()
{
    //
    // Show the modal dialog.
    //
    poundsAboutDlg(MainWindow).Execute();

//    if (Printer)
//       MessageBox(0, "Valid 'Printer' member", "@@@", MB_OK);
}


void poundsApp::InitInstance ()
{
    TApplication::InitInstance();

    SetMenuForFormat();

    // Accept files via drag/drop in the frame window.
    GetMainWindow()->DragAcceptFiles(TRUE);
}


void poundsApp::EvDropFiles (TDropInfo drop)
{
    // Number of files dropped.
    int totalNumberOfFiles = drop.DragQueryFileCount();

    TFileList* files = new TFileList;

    for (int i = 0; i < totalNumberOfFiles; i++) {
        // Tell DragQueryFile the file interested in (i) and the length of your buffer.
        int     fileLength = drop.DragQueryFileNameLen(i) + 1;
        char    *fileName = new char[fileLength];

        drop.DragQueryFile(i, fileName, fileLength);

        // Getting the file dropped. The location is relative to your client coordinates,
        // and will have negative values if dropped in the non client parts of the window.
        //
        // DragQueryPoint copies that point where the file was dropped and returns whether
        // or not the point is in the client area.  Regardless of whether or not the file
        // is dropped in the client or non-client area of the window, you will still receive
        // the file name.
        TPoint  point;
        BOOL    inClientArea = drop.DragQueryPoint(point);
        files->Add(new TFileDrop(fileName, point, inClientArea, this));
    }

    // Open the files that were dropped.
    AddFiles(files);

    // Release the memory allocated for this handle with DragFinish.
    drop.DragFinish();
}


void poundsApp::AddFiles (TFileList* files)
{
    // Open all files dragged in.
    TFileListIter fileIter(*files);

    while (fileIter) {
        TDocTemplate* tpl = GetDocManager()->MatchTemplate(fileIter.Current()->WhoAmI());
        if (tpl)
            tpl->CreateDoc(fileIter.Current()->WhoAmI());
        fileIter++;
    }
}


BOOL poundsApp::ProcessAppMsg (MSG& msg)
{
    if (msg.message == WM_COMMAND) {
        if (ContextHelp || (GetKeyState(VK_F1) < 0)) {
            ContextHelp = FALSE;
            GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT, msg.wParam);
            return TRUE;
        }
    } else
        switch (msg.message) {
        case WM_KEYDOWN:
            if (msg.wParam == VK_F1) {
                // If the Shift/F1 then set the help cursor and turn on the modal help state.
                if (::GetKeyState(VK_SHIFT) < 0) {
                    ContextHelp = TRUE;
                    HelpCursor = ::LoadCursor(GetMainWindow()->GetModule()->GetInstance(), MAKEINTRESOURCE(IDC_HELPCURSOR));
                    ::SetCursor(HelpCursor);
                    return TRUE;        // Gobble up the message.
                } else {
                    // If F1 w/o the Shift key then bring up help's main index.
                    GetMainWindow()->WinHelp(HelpFileName, HELP_INDEX, 0L);
                    return TRUE;        // Gobble up the message.
                }
            } else {
                if (ContextHelp && (msg.wParam == VK_ESCAPE)) {
                    if (HelpCursor)
                        ::DestroyCursor(HelpCursor);
                    ContextHelp = FALSE;
                    HelpCursor = 0;
                    GetMainWindow()->SetCursor(0, IDC_ARROW);
                    return TRUE;    // Gobble up the message.
                }
            }
            break;

        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
            if (ContextHelp) {
                ::SetCursor(HelpCursor);
                return TRUE;        // Gobble up the message.
            }
            break;

        case WM_INITMENU:
            if (ContextHelp) {
                ::SetCursor(HelpCursor);
                return TRUE;        // Gobble up the message.
				}
            break;

        case WM_ENTERIDLE:
            if (msg.wParam == MSGF_MENU)
                if (GetKeyState(VK_F1) < 0) {
                ContextHelp = TRUE;
                GetMainWindow()->PostMessage(WM_KEYDOWN, VK_RETURN, 0L);
                return TRUE;       // Gobble up the message.
            }
            break;
        
        default:
            ;
        };  // End of switch

    // Continue normal processing.
    return TApplication::ProcessAppMsg(msg);
}


void poundsApp::EvWinIniChange (char far* section)
{
    if (lstrcmp(section, "windows") == 0) {
        // If the device changed in the WIN.INI file then the printer
        // might have changed.  If we have a TPrinter (Printer) then
		  // check and make sure it's identical to the current device
        // entry in WIN.INI.
        if (Printer) {
            char printDBuffer[255];
            LPSTR printDevice = printDBuffer;
            LPSTR devName; // = 0;
            LPSTR driverName = 0;
            LPSTR outputName = 0;
        
            if (::GetProfileString("windows", "device", "", printDevice, sizeof(printDevice))) {
                // The string which should come back is something like:
                //
                //      HP LaserJet III,hppcl5a,LPT1:
                //
                // Where the format is:
                //
                //      devName,driverName,outputName
                //
                devName = printDevice;
                while (*printDevice) {
                    if (*printDevice == ',') {
                        *printDevice++ = 0;
                        if (!driverName)
                            driverName = printDevice;
                        else
                            outputName = printDevice;
						  } else
                        printDevice = AnsiNext(printDevice);
                }

                if ((Printer->GetSetup().Error != 0)                                ||
                    (lstrcmp(devName, Printer->GetSetup().GetDeviceName()) != 0)    ||
                    (lstrcmp(driverName, Printer->GetSetup().GetDriverName()) != 0) ||
                    (lstrcmp(outputName, Printer->GetSetup().GetOutputName()) != 0)) {
                
                    // New printer installed so get the new printer device now.
                    delete Printer;
                    Printer = new TPrinter;
                }
            } else {
                // No printer installed (GetProfileString failed).
                delete Printer;
                Printer = new TPrinter;
            }
        }
    }
}


int OwlMain (int , char* [])
{
#ifndef __FLAT__
	 char szDir[MAXDIR];
	 getcurdir(0, szDir);
	 OFSTRUCT of;
	 HFILE hFile = OpenFile("CTL3DV2.DLL", &of, OF_EXIST);
	 if (hFile != HFILE_ERROR && strstr(of.szPathName, szDir)) // @@@
		 {
		 MessageBox(0, "The file CTL3DV2.DLL must be in the \\WINDOWS\\SYSTEM "
			 "directory and *not* in the MZF directory.  Please move it.",
			 "M.Z.F.", MB_ICONHAND|MB_OK);
		 }
#endif

	 poundsApp App;
	 int result = App.Run();

    return result;
}

void poundsApp::CmFileFormat ()
{
    // INSERT>> Your code here.
    if (TFileFormatDialog(MainWindow).Execute() == IDOK)
       SetMenuForFormat();
}

void poundsApp::SetMenuForFormat()
{
   char szBuffer[64];
   const char *szModes[] = { "Merc 2.x", "TFC 1.x", "Envy 1.0", "ROM 2.3",
      "Envy 2.0", "Ogham 1.6/NT", "ROM 2.4" };
   TMenu menu(GetMainWindow()->HWindow);

   wsprintf(szBuffer, "&Save (%s format)\tCtrl-S", szModes[CurrentMode]);
   menu.ModifyMenu(CM_FILESAVE, MF_BYCOMMAND, CM_FILESAVE, szBuffer);

   wsprintf(szBuffer, "Save &As (%s format)...", szModes[CurrentMode]);
   menu.ModifyMenu(CM_FILESAVEAS, MF_BYCOMMAND, CM_FILESAVEAS, szBuffer);

   wsprintf(szBuffer, "&Open (%s format)...", szModes[CurrentMode]);
   menu.ModifyMenu(CM_MDIFILEOPEN, MF_BYCOMMAND, CM_MDIFILEOPEN, szBuffer);
}

void poundsApp::CmHelpCredits ()
{
    // INSERT>> Your code here.
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT,
       CM_HELPCREDITS);
}


void poundsApp::CmHelpMerc ()
{
    // INSERT>> Your code here.
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT,
       CM_HELPMERC);
}


void poundsApp::CmHelpEnvy ()
{
    // INSERT>> Your code here.
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT,
       CM_HELPENVY);
}

void poundsApp::CmHelpRom ()
{
    // INSERT>> Your code here.
    HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT,
       CM_HELPROM);
}


//void poundsApp::CmFileSaveEnabler (TCommandEnabler &tce)
//{
//    // INSERT>> Your code here.
//
//}


//void poundsApp::CmFileSaveEnabler (TCommandEnabler &tce)
//{
//    // INSERT>> Your code here.
//
//}

// @@@ We should go through and convert stuff while we have both modes by
// sending out some kind of message...
void poundsApp::SetMode(currMode newMode)
{
   if (m_Library)
      m_Library->Save();

   CurrentMode = newMode;

   delete m_Library;

   // CreateLibrary is in objdia.cpp for historical reasons
   m_Library = CreateLibrary(CurrentMode);
}

