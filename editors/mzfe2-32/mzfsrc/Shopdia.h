#if !defined(__shopdia_h)              // Sentry, use file only if it's not already included.
#define __shopdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         shopdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TShopDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\edit.h>
#include <owl\combobox.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//{{TDialog = TShopDialog}}
struct TShopDialogXfer {
//{{TShopDialogXFER_DATA}}
    TComboBoxData  Keeper;
    TComboBoxData  Buy[5];
    char    ProfitBuy[ 5 ];
    char    ProfitSell[ 5 ];
    char    OpenHour[ 3 ];
    char    CloseHour[ 3 ];
    char    Comment[ 255 ];
//{{TShopDialogXFER_DATA_END}}
};


class TShopDialog : public TDialog {
    TShopData* TheShop;
    TShopData WorkShop;
    int nIndex;
    TAreaDocument *AreaDoc;
    BOOL fLockKeeper;
public:
    TShopDialog (TWindow* parent, TAreaDocument *doc, TShopData *shop, int nNdx,
       BOOL lock, TResId resId = IDD_SHOP, TModule* module = 0);
    virtual ~TShopDialog ();

//{{TShopDialogXFER_DEF}}
protected:
    TComboBox *Keeper;
    TComboBox *Buy[5];
    TEdit *ProfitBuy;
    TEdit *ProfitSell;
    TEdit *OpenHour;
    TEdit *CloseHour;
    TEdit *Comment;

//{{TShopDialogXFER_DEF_END}}

//{{TShopDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TShopDialogVIRTUAL_END}}

//{{TShopDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TShopDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TShopDialog);
};    //{{TShopDialog}}


#endif                                      // __shopdia_h sentry.

