#if !defined(__helpdia_h)              // Sentry, use file only if it's not already included.
#define __helpdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         helpdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for THelpDialog (TDialog).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\button.h>
#include <owl\edit.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//{{TDialog = THelpDialog}}
struct THelpDialogXfer {
//{{THelpDialogXFER_DATA}}
    char    HelpLevel[ 8 ];
    char    Keywords[ 255 ];
    char    HelpText[ 4096 ];
//{{THelpDialogXFER_DATA_END}}
};


class THelpDialog : public TDialog {
    int nIndex;
    THelpData WorkHelp;
public:
    THelpDialog (TWindow* parent, const THelpData *TheHelp, int n, TResId resId = IDD_HELPS,
       TModule* module = 0);
    virtual ~THelpDialog ();

//{{THelpDialogXFER_DEF}}
protected:
    TEdit *HelpLevel;
    TEdit *Keywords;
    TEdit *HelpText;

//{{THelpDialogXFER_DEF_END}}

//{{THelpDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{THelpDialogVIRTUAL_END}}

//{{THelpDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{THelpDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(THelpDialog);
};    //{{THelpDialog}}


#endif                                      // __helpdia_h sentry.

