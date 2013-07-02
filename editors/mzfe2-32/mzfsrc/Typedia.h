#if !defined(__typedia_h)              // Sentry, use file only if it's not already included.
#define __typedia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         typedia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TFileFormatDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\checkbox.h>
#include <owl\radiobut.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "pounds.h"

//{{TDialog = TFileFormatDialog}}
struct TFileFormatDialogXfer {
//{{TFileFormatDialogXFER_DATA}}
    BOOL    MercMode;
    BOOL    TFCMode;
    BOOL    EnvyMode;
    BOOL    MobProgs;
    BOOL    RomMode;
    BOOL    Envy20Mode;
    BOOL    OghamMode;
    BOOL    Rom24Mode;
//{{TFileFormatDialogXFER_DATA_END}}
};


class TFileFormatDialog : public TDialog {
    poundsApp *MuditorApp;
    TFont *hTestFixedFont;
    int xFont, yFont;
public:
    TFileFormatDialog (TWindow* parent, TResId resId = IDD_FTYPEDIALOG, TModule* module = 0);
    virtual ~TFileFormatDialog ();

//{{TFileFormatDialogXFER_DEF}}
protected:
    TRadioButton *MercMode;
    TRadioButton *TFCMode;
    TRadioButton *EnvyMode;
    TCheckBox *MobProgs;
    TRadioButton *RomMode;
    TRadioButton *Envy20Mode;
    TRadioButton *OghamMode;
    TRadioButton *Rom24Mode;

//{{TFileFormatDialogXFER_DEF_END}}
    void FontSizeChanged ();

//{{TFileFormatDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TFileFormatDialogVIRTUAL_END}}

//{{TFileFormatDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmModeChanged ();
    void CmHelp ();
    void CmShorter ();
    void CmTaller ();
    void CmThinner ();
    void CmWider ();
//    void CmEnvy20 ();
//{{TFileFormatDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TFileFormatDialog);
};    //{{TFileFormatDialog}}


#endif                                      // __typedia_h sentry.

