#if !defined(__xtradia_h)              // Sentry, use file only if it's not already included.
#define __xtradia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         xtradia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TExtraDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\edit.h>
#include <OWL\DIALOG.H>
#include <owl\button.h>

#include "pounds.rh"            // Definition of all resources.
#include "objmerc.h"

//{{TDialog = TExtraDialog}}
struct TExtraDialogXfer {
//{{TExtraDialogXFER_DATA}}
    char    Keyword[ 255 ];
    char    Description[ 4096 ];
//{{TExtraDialogXFER_DATA_END}}
};


class TExtraDialog : public TDialog {
public:
    TExtraDialog::TExtraDialog (TWindow* parent, TExtraDescr *xtra,
       TAreaDocument *doc, TResId resId = IDD_EXTRADIA, TModule* module = 0);
    virtual ~TExtraDialog ();

//{{TExtraDialogXFER_DEF}}
protected:
    TEdit *Keyword;
    TEdit *Description;
    TButton *Ok;

//{{TExtraDialogXFER_DEF_END}}
    TExtraDescr *TheDescr;
    TAreaDocument *AreaDoc;

//{{TExtraDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TExtraDialogVIRTUAL_END}}

//{{TExtraDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
    void CmKeywordChanged ();
//{{TExtraDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TExtraDialog);
};    //{{TExtraDialog}}


#endif                                      // __xtradia_h sentry.

