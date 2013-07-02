#if !defined(__msetdia_h)              // Sentry, use file only if it's not already included.
#define __msetdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         msetdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TMobResetDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\button.h>
#include <owl\listbox.h>
#include <owl\edit.h>
#include <owl\combobox.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"            // Area database

//{{TDialog = TMobResetDialog}}
struct TMobResetDialogXfer {
//{{TMobResetDialogXFER_DATA}}
    TComboBoxData  Mobile;
    TComboBoxData  Location;
    char    Count[ 16 ];
#if 0
    TComboBoxData  Wielded;
    TComboBoxData  FingerLeft;
    TComboBoxData  FingerRight;
    TComboBoxData  Neck1;
    TComboBoxData  Neck2;
    TComboBoxData  Body;
    TComboBoxData  Head;
    TComboBoxData  Legs;
    TComboBoxData  Feet;
    TComboBoxData  Hands;
    TComboBoxData  Arms;
    TComboBoxData  Shield;
    TComboBoxData  AboutBody;
    TComboBoxData  Waist;
    TComboBoxData  WristLeft;
    TComboBoxData  WristRight;
    TComboBoxData  Hold;
#endif
    TComboBoxData  Wields[MAX_WEAR];
    TListBoxData  Inventory;
    char    Comment[ 255 ];
    char    RoomCount[16];
//{{TMobResetDialogXFER_DATA_END}}
};


class TMobResetDialog : public TDialog {
    const TMobReset *TheMobReset;
    TMobReset WorkReset;
    int nIndex;
    sh_int Wearing[MAX_WEAR];
    BOOL fCanSetRoom;
public:
    TMobResetDialog (TWindow* parent, TMobReset *mobreset, int nNdx,
       BOOL fSet, TResId resId = IDD_MOBRESET, TModule* module = 0);
    virtual ~TMobResetDialog ();

//{{TMobResetDialogXFER_DEF}}
protected:
    TComboBox *Mobile;
    TComboBox *Location;
    TEdit *Count;
#if 0
    TComboBox *Wielded;
    TComboBox *FingerLeft;
    TComboBox *FingerRight;
    TComboBox *Neck1;
    TComboBox *Neck2;
    TComboBox *Body;
    TComboBox *Head;
    TComboBox *Legs;
    TComboBox *Feet;
    TComboBox *Hands;
    TComboBox *Arms;
    TComboBox *Shield;
    TComboBox *AboutBody;
    TComboBox *Waist;
    TComboBox *WristLeft;
    TComboBox *WristRight;
    TComboBox *Hold;
#endif
    TComboBox *Wields[MAX_WEAR];
    TListBox *Inventory;
    TButton *ButtonNext;
    TButton *ButtonPrev;
    TEdit *Comment;
    TButton *ButtonRandom;
    TStatic *Wield18Prompt;
    TEdit   *RoomCount;
    TStatic *RoomCountPrompt;

//{{TMobResetDialogXFER_DEF_END}}
//{{TMobResetDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TMobResetDialogVIRTUAL_END}}

//{{TMobResetDialogRSP_TBL_BEGIN}}
protected:
    void CmAddInventory ();
    void CmOk ();
    void CmPrev ();
    void CmNext ();
    void CmDeleteInv ();
    void CmAddRandom ();
    void CmHelp ();
//{{TMobResetDialogRSP_TBL_END}}
    void LoadTransfer();
    void SaveTransfer();
    void LoadAnotherMobReset(const TMobReset *);
DECLARE_RESPONSE_TABLE(TMobResetDialog);
};    //{{TMobResetDialog}}


#endif                                      // __msetdia_h sentry.

