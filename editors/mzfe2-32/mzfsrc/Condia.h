#if !defined(__condia_h)              // Sentry, use file only if it's not already included.
#define __condia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         condia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TContainerDialog (TDialog).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\listbox.h>
#include <owl\static.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//{{TDialog = TContainerDialog}}
struct TContainerDialogXfer {
//{{TContainerDialogXFER_DATA}}
    char    Name[ 255 ];
    TListBoxData  Contents;
//{{TContainerDialogXFER_DATA_END}}
};


class TContainerDialog : public TDialog {
    const TObject *TheContainer;
    TAreaDocument *AreaDoc;

public:
    TContainerDialog (TWindow* parent, TAreaDocument *, const TObject *obj,
       TResId resId = IDD_CONTAIN, TModule* module = 0);
    virtual ~TContainerDialog ();

//{{TContainerDialogXFER_DEF}}
protected:
    TStatic *Name;
    TListBox *Contents;

//{{TContainerDialogXFER_DEF_END}}

//{{TContainerDialogRSP_TBL_BEGIN}}
protected:
    void CmDone ();
    void CmHelp ();
    void CmAddItem ();
//{{TContainerDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TContainerDialog);
};    //{{TContainerDialog}}


#endif                                      // __condia_h sentry.

