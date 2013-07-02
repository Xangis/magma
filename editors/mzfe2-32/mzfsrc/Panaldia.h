#if !defined(__panaldia_h)              // Sentry, use file only if it's not already included.
#define __panaldia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         panaldia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TPandelDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\checkbox.h>
#include <owl\button.h>
#include <owl\combobox.h>
#include <owl\static.h>
#include <owl\edit.h>
#include <OWL\DIALOG.H>
#include <owl\docview.h>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//{{TDialog = TPandelDialog}}
struct TPandelDialogXfer {
//{{TPandelDialogXFER_DATA}}
    char    Name[ 255 ];
//    char    RecallPrompt[ 255 ];
    TComboBoxData  RecallRoom;
    char    Toughest[ 255 ];
    char    Weakest[ 255 ];
    BOOL    UseAreaData;
    char    TimeZone[ 255 ];
    char    Repop[ 255 ];
    char    MaxChange[ 255 ];
    char    MinTemp[ 255 ];
    char    MaxTemp[ 255 ];
    char    MinPrecip[ 255 ];
    char    MaxPrecip[ 255 ];
    char    MinWind[ 255 ];
    char    MaxWind[ 255 ];
    char    MinMagic[ 255 ];
    char    MaxMagic[ 255 ];
    BOOL    ExitCohere;
//{{TPandelDialogXFER_DATA_END}}
};


class TPandelDialog : public TDialog, public TView {
    TAreaDocument *AreaDoc;
public:
    TPandelDialog (TAreaDocument& doc, TWindow *parent=0, TResId resId = IDD_PANELDIALOG, TModule* module = 0);
    virtual ~TPandelDialog ();

    TWindow* GetWindow()  {return (TWindow*)this;}
    BOOL     SetDocTitle(const char far* docname, int index)
                          {return TDialog::SetDocTitle(docname, index); }
    //
    // overridden virtuals from TWindow
    //
    BOOL CanClose()
       { return TDialog::CanClose() &&
         ( Doc->NextView(this) || Doc->NextView(0) != this || Doc->CanClose()); }
    BOOL Create();

    static const char far* StaticName() { return "Control Panel"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
//    BOOL VnIsWindow(HWND hWnd) {return HWindow == hWnd;}
    BOOL VnIsWindow(HWND hWnd);
//    bool VnDocClosed(int omode);

//{{TPandelDialogXFER_DEF}}
protected:
    TEdit *Name;
    TStatic *RecallPrompt;
    TComboBox *RecallRoom;
    TStatic *Toughest;
    TStatic *Weakest;
    TCheckBox *UseAreaData;
    TEdit *TimeZone;
    TEdit *Repop;
    TEdit *MaxChange;
    TEdit *MinTemp;
    TEdit *MaxTemp;
    TEdit *MinPrecip;
    TEdit *MaxPrecip;
    TEdit *MinWind;
    TEdit *MaxWind;
    TEdit *MinMagic;
    TEdit *MaxMagic;
    TCheckBox *ExitCohere;

//{{TPandelDialogXFER_DEF_END}}

//{{TPandelDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TPandelDialogVIRTUAL_END}}

//{{TPandelDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmRenumber ();
    void CmHelp ();
    void CmToClipboard ();
    void CmWeaker ();
    void CmStronger ();
    void CmUseAreaDataClicked ();
//{{TPandelDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TPandelDialog);
};    //{{TPandelDialog}}


#endif                                      // __panaldia_h sentry.

