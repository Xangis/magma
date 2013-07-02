#if !defined(__objdia_h)              // Sentry, use file only if it's not already included.
#define __objdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TObjDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\button.h>
#include <owl\listbox.h>
#include <owl\static.h>
#include <owl\checkbox.h>
#include <owl\radiobut.h>
#include <owl\edit.h>
#include <OWL\DIALOG.H>
#include <owl\combobox.h>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"            // Area file database
#include "objlib.h"

//{{TDialog = TObjDialog}}
struct TObjDialogXfer {
//{{TObjDialogXFER_DATA}}
    char    NameList[ 255 ];
    char    ShortDescr[ 255 ];
    char    LongDescr[ 2048 ];
    BOOL    Light;
    BOOL    Scroll;
    BOOL    Wand;
    BOOL    Staff;
    BOOL    Weapon;
    BOOL    Armor;
    BOOL    Potion;
    BOOL    Furniture;
    BOOL    Trash;
    BOOL    Container;
    BOOL    DrinkCon;
    BOOL    Key;
    BOOL    Food;
    BOOL    Gold;
    BOOL    Boat;
    BOOL    NPCCorpse;
    BOOL    Fountain;
    BOOL    Pill;
    BOOL    Take;
    BOOL    Finger;
    BOOL    Neck;
    BOOL    Body;
    BOOL    Head;
    BOOL    Legs;
    BOOL    Feet;
    BOOL    Hands;
    BOOL    Arms;
    BOOL    Shield;
    BOOL    AboutBody;
    BOOL    Waist;
    BOOL    Wrist;
    BOOL    Hold;
    BOOL    WithPride;
    BOOL    Glow;
    BOOL    Hum;
    BOOL    Evil;
    BOOL    Magic;
    BOOL    NoDrop;
    BOOL    AntiGood;
    BOOL    AntiEvil;
    BOOL    AntiNeutral;
    BOOL    NoRemove;
    BOOL    Invis;
    BOOL    Metallic;
    char    VNum[ 255 ];
    BOOL    Wield;
    char    Weight[ 255 ];
    BOOL    Treasure;
    TComboBoxData Value[5];
    TListBoxData  AppliesList;
    char    ApplyAmount[ 255 ];
    TListBoxData  ExtraList;
    char    Limit[ 255 ];
//    char    ObjType[ 255 ];
//    char    WieldType[ 255 ];
    BOOL    Custom1;
    BOOL    Custom2;
    BOOL    Custom3;
    BOOL    Other;
    char    OtherText[ 255 ];
//{{TObjDialogXFER_DATA_END}}
};

////////////////////////////////////////////////////////////////////////////
//class TObjTypeLibrary;
class TObjDialog : public TDialog {
    const TObject *TheActualObject;
    int nIndex;
    TObject WorkObj; // @@@
    sh_int nCurrentType;
    int nCurrApply;
    int nApplyAmount;
    HICON hObjTypeIcon[28];
    HICON hWearIcon[18];
    TLibrary *m_Library;
    string m_strType;
    poundsApp::currMode m_mode;

public:
    TObjDialog (TWindow* parent, const TObject *obj, int nNdx,
       TLibrary* pLibrary, TResId resId = IDD_OBJDIA, TModule* module = 0);
    virtual ~TObjDialog ();

//{{TObjDialogXFER_DEF}}
protected:
    TEdit *NameList;
    TEdit *ShortDescr;
    TEdit *LongDescr;
    TRadioButton *Light;
    TRadioButton *Scroll;
    TRadioButton *Wand;
    TRadioButton *Staff;
    TRadioButton *Weapon;
    TRadioButton *Armor;
    TRadioButton *Potion;
    TRadioButton *Furniture;
    TRadioButton *Trash;
    TRadioButton *Container;
    TRadioButton *DrinkCon;
    TRadioButton *Key;
    TRadioButton *Food;
    TRadioButton *Gold;
    TRadioButton *Boat;
    TRadioButton *NPCCorpse;
    TRadioButton *Fountain;
    TRadioButton *Pill;
    TCheckBox *Take;
    TCheckBox *Finger;
    TCheckBox *Neck;
    TCheckBox *Body;
    TCheckBox *Head;
    TCheckBox *Legs;
    TCheckBox *Feet;
    TCheckBox *Hands;
    TCheckBox *Arms;
    TCheckBox *Shield;
    TCheckBox *AboutBody;
    TCheckBox *Waist;
    TCheckBox *Wrist;
    TCheckBox *Hold;
    TCheckBox *WithPride;
    TCheckBox *Glow;
    TCheckBox *Hum;
    TCheckBox *Evil;
    TCheckBox *Magic;
    TCheckBox *NoDrop;
    TCheckBox *AntiGood;
    TCheckBox *AntiEvil;
    TCheckBox *AntiNeutral;
    TCheckBox *NoRemove;
    TCheckBox *Invis;
    TCheckBox *Metallic;
    TStatic *VNum;
    TCheckBox *Wield;
    TEdit *Weight;
    TRadioButton *Treasure;
    TStatic *Prompt[5];
    TComboBox *Value[5];
    TListBox *AppliesList;
    TEdit *ApplyAmount;
    TListBox *ExtraList;
    TStatic *ObjType;
    TStatic *WieldType;
    TStatic *Limit;
    TRadioButton *Custom1;
    TRadioButton *Custom2;
    TRadioButton *Custom3;
    TRadioButton *Other;
    TEdit *OtherText;
    TButton *m_AddFlag[5];
    TButton *Code;
//    TButton *m_RemFlag[5];

//{{TObjDialogXFER_DEF_END}}
    void LoadTransfer();
    BOOL SaveTransfer();               // Returns FALSE if the saving was aborted
    void LoadAnotherObject(const TObject *);
    void SetPrompts();
    const char *GetAffectName(int n);
    void LoadValues();
    BOOL SaveValues();                 // Returns FALSE if the value saving was aborted
    void AddFlag(int n);
    void RemFlag(int n);

//{{TObjDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TObjDialogVIRTUAL_END}}

//{{TObjDialogRSP_TBL_BEGIN}}
protected:
    void CmPrev ();
    void CmNext ();
    void CmTypeChanged ();
    void CmTakeChanged ();
    void CmApplySelected ();
    void CmEditExtra ();
    void CmOk ();
    void CmAddExtra ();
    void CmDelExtra ();
    void CmAddApply ();
    void CmDelApply ();
    void CmWieldChanged ();
//    void CmWaist ();
    void CmHelp ();
//{{TObjDialogRSP_TBL_END}}
    void CmRomProps ();
    void CmCustomize ();
    void CmAddFlag1();
    void CmRemFlag1();
    void CmAddFlag2();
    void CmRemFlag2();
    void CmAddFlag3();
    void CmRemFlag3();
    void CmAddFlag4();
    void CmRemFlag4();
    void CmAddFlag5();
    void CmRemFlag5();
    void CmOtherTextKillFocus();
DECLARE_RESPONSE_TABLE(TObjDialog);
};    //{{TObjDialog}}

////////////////////////////////////////////////////////////////////////////
// TRomObjDialog
////////////////////////////////////////////////////////////////////////////

struct TRomObjDialogXfer {
//    char Condition[2];
    TComboBoxData Condition;
    TComboBoxData Material;
    char Level[8];
    TListBoxData Affects;
    // @@@ I haven't bound these to controls yet...
    TListBoxData Imm;
    TListBoxData Res;
    TListBoxData Vuln;
    BOOL NoPurge;
    BOOL RotDeath;
    BOOL VisDeath;
    BOOL MeltDrop;
    BOOL ShellExtract;
    BOOL NonMetal;
    BOOL NoLocate;
    BOOL BurnProof;
    BOOL NounCurse;
};

////////////////////////////////////////////////////////////////////////////
struct Bit2String {
   int32 bit;
   const char *psz;
};

////////////////////////////////////////////////////////////////////////////
class TRomObjectDialog: public TDialog {
    TObject& WorkObj;

//    TEdit *Condition;
    TComboBox *Condition;
    TComboBox *Material;
    TEdit *Level;
    TListBox *Affects;
    TListBox *Imm;
    TListBox *Res;
    TListBox *Vuln;
    TCheckBox *NoPurge;
    TCheckBox *RotDeath;
    TCheckBox *VisDeath;
    TCheckBox *MeltDrop;
    TCheckBox *ShellExtract;
    TCheckBox *NonMetal;
    TCheckBox *NoLocate;
    TCheckBox *BurnProof;
    TCheckBox *NounCurse;

    void LoadTransfer();
    void LoadEnchantments(int32 flags, TListBoxData& box,
        Bit2String *pTable, int nSize);
public:
    TRomObjectDialog(TWindow *parent, TObject& TempObj, TModule *module=0);

protected:
DECLARE_RESPONSE_TABLE(TRomObjectDialog);
   void CmAddAff();
   void CmAddImm();
   void CmAddRes();
   void CmAddVuln();
   void CmDelAff();
   void CmDelImm();
   void CmDelRes();
   void CmDelVuln();
   void CmHelp ();
   void CmOk();
};

//{{TDialog = TCustObjDialog}}
struct TCustObjectDialogXfer {
//{{TCustDialogXFER_DATA}}
   char EditPrompt[5][32];
   BOOL Flag[5];
   BOOL List[5];
   BOOL Word[5];
   char MercType[4];
//{{TCustObjDialogXFER_DATA_END}}
};

////////////////////////////////////////////////////////////////////////////
class TCustObjectDialog: public TDialog {
    string m_strType;

    TEdit *m_EditPrompt[5];
    TStatic *m_StaticPrompt[5];
    TRadioButton *m_Flag[5];
    TRadioButton *m_List[5];
    TRadioButton *m_Word[5];
    TButton *m_AddFlag[5];
//    TButton *m_RemFlag[5];
    TStatic *m_ObjName;
    TStatic *m_ObjIcon;
    TEdit *m_MercType;

    TLibrary *m_Library;

    void ChangedPrompt(int n);
    void ChangedType(int n);
    void UpdateDictionary(int n);

public:
    TCustObjectDialog(TWindow *parent, string& strType, TLibrary&, TModule *module=0);

//{{TCustObjDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TCustObjDialogVIRTUAL_END}}

//{{TCustObjDialogRSP_TBL_BEGIN}}
protected:
DECLARE_RESPONSE_TABLE(TCustObjectDialog);
   void CmChangedPrompt1();
   void CmChangedPrompt2();
   void CmChangedPrompt3();
   void CmChangedPrompt4();
   void CmChangedPrompt5();
   void CmClickedFlag1();
   void CmClickedFlag2();
   void CmClickedFlag3();
   void CmClickedFlag4();
   void CmClickedFlag5();
   void CmClickedWord1();
   void CmClickedWord2();
   void CmClickedWord3();
   void CmClickedWord4();
   void CmClickedWord5();
   void CmClickedList1();
   void CmClickedList2();
   void CmClickedList3();
   void CmClickedList4();
   void CmClickedList5();
   void CmAddFlag();
   void CmRemFlag();
   void CmHelp ();
   void CmOk();
};    //{{TCustObjDialog}}

////////////////////////////////////////////////////////////////////////////
//{{TDialog = TFlagDialog}}
struct TFlagDialogXfer {
//{{TFlagDialogXFER_DATA}}
    TListBoxData  FlagList;
    char Flag[16];
    char Meaning[64];
//{{TFlagDialogXFER_DATA_END}}
};

////////////////////////////////////////////////////////////////////////////
class TFlagDialog : public TDialog {
protected:
    TLibrary *m_pLibrary;
    string strPrompt;
    int32 *m_pnFlags;
    poundsApp::currMode m_mode;

public:
    TFlagDialog (TWindow* parent, TLibrary *pLibrary,
      const char *pszPrompt, int32 *pnFlags, poundsApp::currMode,
      TModule* module = 0);
    virtual ~TFlagDialog ();

//{{TFlagDialogXFER_DEF}}
protected:
    TListBox *FlagList;
    TEdit *Flag;
    TEdit *Meaning;

//{{TFlagDialogXFER_DEF_END}}

//{{TFlagDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TFlagDialogVIRTUAL_END}}

//{{TFlagDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
    void CmDefFlag ();
    void CmPurgeFlag ();
    void CmFlagSelection ();
    void CmFlagChanged ();
    void CmMeaningChanged ();
//{{TFlagDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TFlagDialog);
};    //{{TFlagDialog}}

////////////////////////////////////////////////////////////////////////////
//{{TDialog = TBitDialog}}
struct TBitDialogXfer {
//{{TBitDialogXFER_DATA}}
    TListBoxData  FlagList;
//{{TBitDialogXFER_DATA_END}}
};

////////////////////////////////////////////////////////////////////////////
class TBitDialog : public TDialog {
protected:
    int32 *m_pnFlags;

public:
    TBitDialog (TWindow* parent, Bit2String *pBits, int nBits,
        int32 *pnFlags, TModule* module = 0);
    virtual ~TBitDialog ();

//{{TBitDialogXFER_DEF}}
protected:
    TListBox *FlagList;

//{{TBitDialogXFER_DEF_END}}

//{{TBitDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TBitDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TBitDialog);
};    //{{TBitDialog}}

#endif                                      // __objdia_h sentry.

