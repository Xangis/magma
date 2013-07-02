/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         exitdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TExitDialog (TDialog).      
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "exitdia.h"


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TExitDialog, TDialog)
//{{TExitDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TExitDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TExitDialog Implementation}}


static TExitDialogXfer TExitDialogData;

TExitDialog::TExitDialog (TWindow* parent, TAreaDocument *doc, TExit *ex,
      int nDir, vnum_int vnum, TRoom **modRooms, TRoom *room, TExit modSides[],
      TResId resId, TModule* module):
   TDialog(parent, resId, module)
{
//{{TExitDialogXFER_USE}}
    ToRoom = new TComboBox(this, IDC_TOROOM, 1);
    Key = new TComboBox(this, IDC_KEY, 1);
    Keyword = new TEdit(this, IDC_KEYWORDS, 255);
    ExitDescr = new TEdit(this, IDC_EXITDESCR, 255);
    DoorOpen = new TRadioButton(this, IDC_OPEN, 0);
    DoorClosed = new TRadioButton(this, IDC_CLOSED, 0);
    DoorLocked = new TRadioButton(this, IDC_LOCKED, 0);
    LeaveAlone = new TRadioButton(this, IDC_LEAVEALONE, 0);
    RomFlags = new TGroupBox(this, IDC_ROMFLAGS);
    Easy = new TCheckBox(this, IDC_EASY);
    Hard = new TCheckBox(this, IDC_HARD);
    Infuriating = new TCheckBox(this, IDC_INFURIATING);
    NoClose = new TCheckBox(this, IDC_NOCLOSE);
    NoLock = new TCheckBox(this, IDC_NOLOCK);

    SetTransferBuffer(&TExitDialogData);
//{{TExitDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    AreaDoc = doc;
    TheExit = ex;
    nDirection = nDir;
    nRoomVNum = vnum;
    ModifiedRooms = modRooms;
    TheRoom = room;
    ModifiedOtherSides = modSides;

    TExitDialogData.ToRoom.Clear();
    TExitDialogData.ToRoom.AddStringItem("[none]", -1, ex->vnum <= 0);
    AreaDoc->RoomsToCombo(TExitDialogData.ToRoom, ex->vnum);
    TExitDialogData.Key.Clear();
    TExitDialogData.Key.AddStringItem("[none]", -1, ex->key <= 0);
    AreaDoc->ObjectsToComboType(TExitDialogData.Key, ITEM_KEY, ex->key);
    strcpy(TExitDialogData.Keyword, ex->keyword.c_str());
    strcpy(TExitDialogData.ExitDescr, ex->description.c_str());

    TExitDialogData.DoorOpen = ((ex->exit_info & EX_ISDOOR) &&
       (ex->InitialPosition && ex->InitialPosition->arg3 == 0));
    TExitDialogData.DoorClosed = (ex->InitialPosition &&
       ex->InitialPosition->arg3 == 1);
    TExitDialogData.DoorLocked = (ex->InitialPosition &&
       ex->InitialPosition->arg3 == 2);
    TExitDialogData.LeaveAlone = (!ex->InitialPosition);

    // I put in support for these ROM flags before I realized they were
    // not being used...
#if 0
    // ROM-specific flags
    TExitDialogData.Easy = !!(ex->exit_info & EX_EASY);
    TExitDialogData.Hard = !!(ex->exit_info & EX_HARD);
    TExitDialogData.Infuriating = !!(ex->exit_info & EX_INFURIATING);
    TExitDialogData.NoClose = !!(ex->exit_info & EX_NOCLOSE);
    TExitDialogData.NoLock = !!(ex->exit_info & EX_NOLOCK);
#endif
}


TExitDialog::~TExitDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.

}


void TExitDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);

    int n = TExitDialogData.ToRoom.GetSelIndex();
    if (n == LB_ERR)
       return;

    int nBox = IDCANCEL;
    sh_int nVNum = (sh_int) TExitDialogData.ToRoom.GetItemDatas()[n];
    // @@@ CAST AWAY CONST
    TRoom *newDestination = (TRoom *) AreaDoc->GetRoom(nVNum);
    // Did the to-room change?
    if (newDestination != TheExit->to_room)
       {
       TExit *oldRevExit =
          (TheExit->to_room &&
           TheExit->to_room->is_exit[TAreaDocument::rev_dir[nDirection]]) ?
             &TheExit->to_room->exit[TAreaDocument::rev_dir[nDirection]] : 0;
       // Did the old to-room have an exit back to this room?
       if (oldRevExit && oldRevExit->vnum == nRoomVNum)
          {
          nBox = MessageBox(
             "The other side of this exit will *NOT* be disconnected.",
             "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_OKCANCEL);
          if (nBox == IDCANCEL)
             return;
          }

       if (newDestination)
          {
          TExit *newRevExit =
             (newDestination->is_exit[TAreaDocument::rev_dir[nDirection]]) ?
                &newDestination->exit[TAreaDocument::rev_dir[nDirection]] : 0;
          if (newRevExit)
             {
             if (newRevExit->vnum != nRoomVNum)
                {
                nBox = MessageBox(
                   "The other side of this exit already has an exit coming back,"
                   " and it doesn't come back here.", "MAKE.ZONES.FAST",
                   MB_ICONQUESTION|MB_OKCANCEL);
                if (nBox == IDCANCEL)
                   return;
                }
             else
                MessageBox("Warning:  Changes to the exit will not propogate to the "
                   "other side.  If you changed anything, change the other side.",
                   "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
             }
          else
             {
             nBox = MessageBox("Create reverse Exit?", "MAKE.ZONES.FAST",
                MB_ICONQUESTION|MB_YESNOCANCEL);
             if (nBox == IDCANCEL)
                return;
             }
          }
       }

    TheExit->vnum = nVNum;
    // @@@ Note cast away const
    TheExit->to_room = (TRoom *) newDestination;

    n = TExitDialogData.Key.GetSelIndex();
    TheExit->key = (sh_int) TExitDialogData.Key.GetItemDatas()[n];
    TheExit->keyword = TExitDialogData.Keyword;
    TheExit->description = TExitDialogData.ExitDescr;

#if 0
    // ROM-specific flags
    SET_RESET_BIT(TheExit->exit_info, TExitDialogData.Easy, EX_EASY);
    SET_RESET_BIT(TheExit->exit_info, TExitDialogData.Hard, EX_HARD);
    SET_RESET_BIT(TheExit->exit_info, TExitDialogData.Infuriating, EX_INFURIATING);
    SET_RESET_BIT(TheExit->exit_info, TExitDialogData.NoClose, EX_NOCLOSE);
    SET_RESET_BIT(TheExit->exit_info, TExitDialogData.NoLock, EX_NOLOCK);
#endif

    if (TheExit->exit_info & EX_ISDOOR)
       {
       if (TExitDialogData.DoorOpen)
          TheExit->nInitialPosition = 0;
       else if (TExitDialogData.DoorClosed)
          TheExit->nInitialPosition = 1;
       else if (TExitDialogData.DoorLocked)
          TheExit->nInitialPosition = 2;
       else if (TExitDialogData.LeaveAlone)
          TheExit->nInitialPosition = 3;
		 }

    // Are we to create a reverse exit?
    if (nBox == IDYES)
       {
       ModifiedRooms[nDirection] = newDestination;
       ModifiedOtherSides[nDirection] = *TheExit;
       ModifiedOtherSides[nDirection].vnum = nRoomVNum;
//       ModifiedOtherSides[nDirection]->to_room = AreaDoc->GetRoom(vnum);
       ModifiedOtherSides[nDirection].to_room = TheRoom;
       }

    TDialog::CmOk();
}


void TExitDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    // @@@If the user has deleted or created another side he can't change the
    // to-room
    ExitDescr->SendMessage(WM_SETFONT,
       (WPARAM) (HFONT) AreaDoc->GetMZF()->hFixedFont, 0);

    if (ModifiedRooms[nDirection])
       ToRoom->EnableWindow(FALSE);

    BOOL fEnable = ((TheExit->exit_info & EX_ISDOOR) == EX_ISDOOR);
    DoorOpen->EnableWindow(fEnable);
    DoorClosed->EnableWindow(fEnable);
    DoorLocked->EnableWindow(fEnable);
    LeaveAlone->EnableWindow(fEnable);

    BOOL fRom = (AreaDoc->GetMode() == poundsApp::RomMode || AreaDoc->GetMode() == poundsApp::Rom24Mode);
    RomFlags->EnableWindow(fRom);
    Easy->EnableWindow(fRom);
    Hard->EnableWindow(fRom);
    Infuriating->EnableWindow(fRom);
    NoClose->EnableWindow(fRom);
    NoLock->EnableWindow(fRom);
}


void TExitDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_EXIT);
}

