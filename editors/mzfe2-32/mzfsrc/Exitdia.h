#if !defined(__exitdia_h)              // Sentry, use file only if it's not already included.
#define __exitdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         exitdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TExitDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\combobox.h>
#include <OWL\DIALOG.H>
#include <owl\groupbox.h>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"				  // Area file database

//{{TDialog = TExitDialog}}
struct TExitDialogXfer {
//{{TExitDialogXFER_DATA}}
    TComboBoxData  ToRoom;
    TComboBoxData  Key;
    char    Keyword[ 255 ];
    char    ExitDescr[ 255 ];
    BOOL    DoorOpen;
    BOOL    DoorClosed;
    BOOL    DoorLocked;
    BOOL    LeaveAlone;
    BOOL    Easy;
    BOOL    Hard;
    BOOL    Infuriating;
    BOOL    NoClose;
    BOOL    NoLock;
//{{TExitDialogXFER_DATA_END}}
};


class TExitDialog : public TDialog {
    TExit *TheExit;
    TAreaDocument *AreaDoc;
    int nDirection;
    sh_int nRoomVNum;
    TRoom **ModifiedRooms;
    TExit *ModifiedOtherSides;
    TRoom *TheRoom;
public:
    TExitDialog (TWindow* parent, TAreaDocument *, TExit *, int nDir, vnum_int vnum,
       TRoom *modRooms[], TRoom *room, TExit modSides[],
       TResId resId = IDD_EXIT, TModule* module = 0);
    virtual ~TExitDialog ();

//{{TExitDialogXFER_DEF}}
protected:
    TComboBox *ToRoom;
    TComboBox *Key;
    TEdit *Keyword;
    TEdit *ExitDescr;
    TRadioButton *DoorOpen;
    TRadioButton *DoorClosed;
    TRadioButton *DoorLocked;
    TRadioButton *LeaveAlone;
    TGroupBox *RomFlags;
    TCheckBox *Easy;
    TCheckBox *Hard;
    TCheckBox *Infuriating;
    TCheckBox *NoClose;
    TCheckBox *NoLock;

//{{TExitDialogXFER_DEF_END}}

//{{TExitDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TExitDialogVIRTUAL_END}}

//{{TExitDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TExitDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TExitDialog);
};    //{{TExitDialog}}


#endif                                      // __exitdia_h sentry.

