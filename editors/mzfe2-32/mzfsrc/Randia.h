#if !defined(__randia_h)              // Sentry, use file only if it's not already included.
#define __randia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         randia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TRandomDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\radiobut.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.


//{{TDialog = TRandomDialog}}
struct TRandomDialogXfer {
//{{TRandomDialogXFER_DATA}}
    BOOL    Weapon;
    BOOL    Armor;
    BOOL    Potion;
    BOOL    Misc;
    BOOL    Shoddy;
    BOOL    Decent;
    BOOL    Fine;
    BOOL    Extraordinary;
    BOOL    Inventory;
    BOOL    Wield;
//{{TRandomDialogXFER_DATA_END}}
};


class TRandomDialog : public TDialog {
    char *pCommand;
    int *pnType;
    int *pnLevel;
public:
    TRandomDialog (TWindow* parent, char *Command, int *Type, int *Level,
       TResId resId = IDD_RANDOMDIALOG, TModule* module = 0);
    virtual ~TRandomDialog ();

//{{TRandomDialogXFER_DEF}}
protected:
    TRadioButton *Weapon;
    TRadioButton *Armor;
    TRadioButton *Potion;
    TRadioButton *Misc;
    TRadioButton *Shoddy;
    TRadioButton *Decent;
    TRadioButton *Fine;
    TRadioButton *Extraordinary;
    TRadioButton *Inventory;
    TRadioButton *Wield;

//{{TRandomDialogXFER_DEF_END}}

//{{TRandomDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TRandomDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TRandomDialog);
};    //{{TRandomDialog}}


#endif                                      // __randia_h sentry.

