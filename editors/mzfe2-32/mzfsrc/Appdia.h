#if !defined(__appdia_h)              // Sentry, use file only if it's not already included.
#define __appdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         appdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TApplyDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\listbox.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//=============================================================================
//{{TDialog = TApplyDialog}}
struct TApplyDialogXfer {
//{{TApplyDialogXFER_DATA}}
    TListBoxData  ApplyType;
//{{TApplyDialogXFER_DATA_END}}
};

class TApplyDialog : public TDialog {
    int *nLocation;
public:
    TApplyDialog (TWindow* parent, int *loc, TResId resId = IDD_APPLY,
       TModule* module = 0);
    virtual ~TApplyDialog ();

//{{TApplyDialogXFER_DEF}}
protected:
    TListBox *ApplyType;

//{{TApplyDialogXFER_DEF_END}}

//{{TApplyDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TApplyDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TApplyDialog);
};    //{{TApplyDialog}}

//=============================================================================
struct TObjectListDialogXfer {
    TListBoxData  ObjectListType;
};

class TObjectListDialog : public TDialog {
    int32 *nLocation;
public:
    TObjectListDialog (TWindow* parent, TAreaDocument *doc, int32 *vnum,
        TResId resId = IDD_OBJLIST, TModule* module = 0);
    virtual ~TObjectListDialog ();

protected:
    TListBox *ObjectListType;

protected:
    void CmOk ();
    void CmHelp ();
DECLARE_RESPONSE_TABLE(TObjectListDialog);
};

//=============================================================================
struct TResetRoomDialogXfer {
    TListBoxData  ResetRoomType;
};

class TResetRoomDialog : public TDialog {
    TAreaDocument *AreaDoc;
public:
    TResetRoomDialog (TWindow* parent, TAreaDocument *doc, sh_int nVnumMobile,
       TResId resId = IDD_RESETROOM, TModule* module = 0);
    virtual ~TResetRoomDialog ();

protected:
    TListBox *ResetRoomType;

protected:
    void CmOk ();
    void CmHelp ();
    void CmPlaceNew ();
DECLARE_RESPONSE_TABLE(TResetRoomDialog);
};

#endif                                      // __appdia_h sentry.

