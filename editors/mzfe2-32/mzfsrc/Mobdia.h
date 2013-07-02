#if !defined(__mobdia_h)              // Sentry, use file only if it's not already included.
#define __mobdia_h

/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         mobdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TMobDialog (TDialog).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\static.h>
#include <owl\checkbox.h>
#include <owl\radiobut.h>
#include <owl\combobox.h>
#include <owl\edit.h>
#include <OWL\DIALOG.H>
//#include <owl\validate.h>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"            // Area file database

////////////////////////////////////////////////////////////////////////////
//{{TDialog = TMobDialog}}
struct TMobDialogXfer {
//{{TMobDialogXFER_DATA}}
    char    NameList[ 255 ];
    char    ShortDescr[ 255 ];
    char    LongDescr[ 255 ];
    char    LookDescr[ 4096 ];
    TComboBoxData  SpecFun;
    char    Level[ 255 ];
    BOOL    Male;
    BOOL    Female;
    BOOL    Neuter;
    BOOL    Either;
    char    Align[ 255 ];
    BOOL    Sentinel;
    BOOL    Scavenger;
    BOOL    Aggresive;
    BOOL    StayArea;
    BOOL    Wimpy;
    BOOL    TrainPCs;
    BOOL    PracticePCs;
    BOOL    Gamble;
    BOOL    Invisible;
    BOOL    DetectInvis;
    BOOL    DetectHidden;
    BOOL    FaerieFire;
    BOOL    Infravision;
    BOOL    ProtEvil;
    BOOL    Sneak;
    BOOL    Hide;
    BOOL    Flying;
    BOOL    PassDoor;
//    BOOL    WaterWalk;
//    BOOL    Mute;
//    BOOL    Gills;
    BOOL    Sanctuary;
    BOOL    SuperWimpy;
    char    VNum[ 255 ];
    BOOL    ProtGood;
    BOOL    NoSleep;
    BOOL    NoSummon;
//    char    SexIcon[ 255 ];
//    char    SpecIcon[ 255 ];
    char    HitPoints[ 255 ];
    char    ArmorClass[ 255 ];
    char    Thac0[ 255 ];
//    BOOL    AssistChance;
//    BOOL    AssistAlways;
//    BOOL    AssistSame;
    BOOL    AntiMagic;
    BOOL    FireShield;
    BOOL    ImprovedInvis;
    BOOL    Invulnerable;
    BOOL    NoSteal;
    BOOL    NoCharm;
//    char    Race[ 255 ];
    TComboBoxData Race;
//{{TMobDialogXFER_DATA_END}}
};

#define MAX_CUSTOM_SPEC 64

class TMobDialog : public TDialog {
private:
    const TMobile *TheActualMobile;
    int nIndex;
    TMobile WorkMob;
    HICON hMobSexIcon[4];
    char *szCustomSpec[MAX_CUSTOM_SPEC];

public:
    TMobDialog (TWindow* parent, const TMobile *mob, int nNdx,
       TResId resId = IDD_MOBDIA, TModule* module = 0);
    virtual ~TMobDialog ();
    HICON hMobSpecIcon[38];

//{{TMobDialogXFER_DEF}}
protected:
    TEdit *NameList;
    TEdit *ShortDescr;
    TEdit *LongDescr;
    TEdit *LookDescr;
    TComboBox *SpecFun;
    TEdit *Level;
    TRadioButton *Male;
    TRadioButton *Female;
    TRadioButton *Neuter;
    TRadioButton *Either;
    TEdit *Align;
    TCheckBox *Sentinel;
    TCheckBox *Scavenger;
    TCheckBox *Aggresive;
    TCheckBox *StayArea;
    TCheckBox *Wimpy;
    TCheckBox *TrainPCs;
    TCheckBox *PracticePCs;
    TCheckBox *Gamble;
    TCheckBox *Invisible;
    TCheckBox *DetectInvis;
    TCheckBox *DetectHidden;
    TCheckBox *FaerieFire;
    TCheckBox *Infravision;
    TCheckBox *ProtEvil;
    TCheckBox *Sneak;
    TCheckBox *Hide;
    TCheckBox *Flying;
    TCheckBox *PassDoor;
//    TCheckBox *WaterWalk;
//    TCheckBox *Mute;
//    TCheckBox *Gills;
    TCheckBox *Sanctuary;
    TCheckBox *SuperWimpy;
    TStatic *VNum;
    TCheckBox *ProtGood;
    TCheckBox *NoSleep;
    TCheckBox *NoSummon;
    TStatic *SexIcon;
    TStatic *SpecIcon;
    TStatic *HitPoints;
    TStatic *ArmorClass;
    TStatic *Thac0;
//    TCheckBox *AssistChance;
//    TCheckBox *AssistAlways;
//    TCheckBox *AssistSame;
    TCheckBox *AntiMagic;
    TCheckBox *FireShield;
    TCheckBox *ImprovedInvis;
    TCheckBox *Invulnerable;
    TCheckBox *NoSteal;
    TCheckBox *NoCharm;
    TStatic   *RacePrompt;
    TComboBox *Race;
    TButton   *Code;
    TStatic   *HpPrompt;
    TStatic   *AcPrompt;
    TStatic   *Thac0Prompt;
    TButton   *Bits;
    TButton   *Parts;

//{{TMobDialogXFER_DEF_END}}
    void LoadTransfer();
    void SaveTransfer();
    void LoadAnotherMobile(const TMobile *);
    void InitRaceComboData();
    void LoadActAff();
    void SaveActAff();
    void SaveTransferToWork();

//{{TMobDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TMobDialogVIRTUAL_END}}
//    void SetThac0 (int nLevel);
    void SetThac0(BOOL fROM, int nLevel);

//{{TMobDialogRSP_TBL_BEGIN}}
protected:
    void CmPrev ();
    void CmNext ();
    void CmOk ();
    void CmMakeKeeper ();
    void CmSexChanged ();
    void CmSpecChanged ();
    void CmHelp ();
//    void CmAlwaysAssist ();
    void CmViewResets ();
    void CmMobProg ();
//{{TMobDialogRSP_TBL_END}}
    void CmRomProps ();
    void CmRaceChanged ();
    void CmParts ();
    void CmBits ();
DECLARE_RESPONSE_TABLE(TMobDialog);
};    //{{TMobDialog}}

////////////////////////////////////////////////////////////////////////////
#if 0
class TThacoValidator : public TValidator {
   TMobDialog *mobdia;
public:
   TThacoValidator(TMobDialog *AMobDia) { mobdia = AMobDia; }
   BOOL TThacoValidator::IsValidInput(char far*, BOOL suppressFill);
};
#endif

////////////////////////////////////////////////////////////////////////////
class TThacoEdit : public TEdit {
   TMobDialog *mobdia;
public:
    TThacoEdit(TMobDialog*   parent,
          int        resourceId,
          UINT       textLen = 0,
          TModule*   module = 0):
       TEdit(parent, resourceId, textLen, module)
       { mobdia = parent; }

  protected:
    void         EvKillFocus(HWND hWndGetFocus);
  DECLARE_RESPONSE_TABLE(TThacoEdit);
};

#if 0
////////////////////////////////////////////////////////////////////////////
struct TRomMobDialogXfer {
    char HitRoll[5];
    char HitDice[32];
    char DamDice[32];
    TComboBoxData DamType;
    char ManaDice[32];
    char ArmorClass[4][5];
    TComboBoxData StartPos;
    TComboBoxData DefPos;
    TComboBoxData Size;
    TComboBoxData Material;
    char Gold[16];
    char MobGroup[8];
    BOOL Form[26];
    BOOL Part[22];
    BOOL Imm[19];
    BOOL Res[18];
    BOOL Vuln[20];
    BOOL Act[11];
    BOOL Assist[6];
    BOOL Off[15];
};

class TRomMobDialog: public TDialog {
    TMobile& WorkMob;

    TEdit *HitRoll;
    TEdit *HitDice;
    TEdit *DamDice;
    TComboBox *DamType;
    TEdit *ManaDice;
    TEdit *ArmorClass[4];
    TComboBox *StartPos;
    TComboBox *DefPos;
    TComboBox *Size;
    TComboBox *Material;
    TEdit *Gold;
    TEdit *MobGroup;
    TRadioButton *Form[26];
    TRadioButton *Part[22];
    TRadioButton *Imm[19];
    TRadioButton *Res[18];
    TRadioButton *Vuln[20];
    TRadioButton *Act[11];
    TRadioButton *Assist[6];
    TRadioButton *Off[15];

    void LoadTransfer();
    void SaveTransfer();

    static void StringToDice(const char *psz,
      int32 *pnNo, int32 *pnSz, int32 *pnPl);

    static int32 anRomForm[26];
    static int32 anRomPart[22];
    static int32 anRomImm[19];
    static int32 anRomRes[18];
    static int32 anRomVuln[20];
    static int32 anRomAct[11];
    static int32 anRomAssist[6];
    static int32 anRomOff[15];
public:
    TRomMobDialog(TWindow *parent, TMobile& TempMob, TModule *module=0);

//{{TRomMobDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TRomMobDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TRomMobDialog);
};    //{{TRomMobDialog}}
#endif

////////////////////////////////////////////////////////////////////////////
struct TNewRomMobDialogXfer {
    char HitRoll[5];
    char HitDice[32];
    char DamDice[32];
    TComboBoxData DamType;
    char ManaDice[32];
    char ArmorClass[4][5];
    TComboBoxData StartPos;
    TComboBoxData DefPos;
    TComboBoxData Size;
    TComboBoxData Material;
    char Gold[16];
    char MobGroup[8];
    BOOL Act[11];
    BOOL Assist[6];
    BOOL Off[15];
};

class TNewRomMobDialog: public TDialog {
    TMobile& WorkMob;

    TEdit *HitRoll;
    TEdit *HitDice;
    TEdit *DamDice;
    TComboBox *DamType;
    TEdit *ManaDice;
    TEdit *ArmorClass[4];
    TComboBox *StartPos;
    TComboBox *DefPos;
    TComboBox *Size;
    TComboBox *Material;
    TEdit *Gold;
    TEdit *MobGroup;
    TRadioButton *Act[11];
    TRadioButton *Assist[6];
    TRadioButton *Off[15];

    void SaveTransfer();
    void LoadTransfer();
    static void StringToDice(const char *psz,
      int32 *pnNo, int32 *pnSz, int32 *pnPl);
    static int32 anRomAct[11];
    static int32 anRomAssist[6];
    static int32 anRomOff[15];
    
public:
    static const char * far aszRomAttacks[40];
    static char * far aszRomMaterials[97];

public:
    TNewRomMobDialog(TWindow *parent, TMobile& TempMob, TModule *module=0);
    virtual ~TNewRomMobDialog ();

protected:
    void CmOk ();
    void CmHelp ();

DECLARE_RESPONSE_TABLE(TNewRomMobDialog);
};

////////////////////////////////////////////////////////////////////////////
struct TMobBitsDialogXfer {
    BOOL Imm[19];
    BOOL Res[18];
    BOOL Vuln[20];
};

class TMobBitsDialog: public TDialog {
    TMobile& WorkMob;

    TRadioButton *Imm[19];
    TRadioButton *Res[18];
    TRadioButton *Vuln[20];

    void SaveTransfer();
    void LoadTransfer();

    static int32 anRomImm[19];
    static int32 anRomRes[18];
    static int32 anRomVuln[20];

public:
    TMobBitsDialog(TWindow *parent, TMobile& TempMob, TModule *module=0);
    virtual ~TMobBitsDialog ();

protected:
    void CmOk ();
    void CmHelp ();

DECLARE_RESPONSE_TABLE(TMobBitsDialog);
};

////////////////////////////////////////////////////////////////////////////
struct TMobPartsDialogXfer {
    BOOL Form[26];
    BOOL Part[22];
};

class TMobPartsDialog: public TDialog {
    TMobile& WorkMob;

    TRadioButton *Form[26];
    TRadioButton *Part[22];

    void SaveTransfer();
    void LoadTransfer();

    static int32 anRomForm[26];
    static int32 anRomPart[22];

public:
    TMobPartsDialog(TWindow *parent, TMobile& TempMob, TModule *module=0);
    virtual ~TMobPartsDialog ();

protected:
    void CmOk ();
    void CmHelp ();

DECLARE_RESPONSE_TABLE(TMobPartsDialog);
};


#endif                                      // __mobdia_h sentry.

