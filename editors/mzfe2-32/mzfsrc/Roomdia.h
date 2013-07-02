#if !defined(__roomdia_h)              // Sentry, use file only if it's not already included.
#define __roomdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         roomdia.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TRoomDialog (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\button.h>
#include <owl\listbox.h>
#include <owl\checkbox.h>
#include <owl\radiobut.h>
#include <owl\static.h>
#include <owl\edit.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"

//{{TDialog = TRoomDialog}}
struct TRoomDialogXfer {
//{{TRoomDialogXFER_DATA}}
    char    Name[ 255 ];
    char    Description[ 4096 ];
//    char    SectorType[ 255 ];
    BOOL    Inside;
    BOOL    CityStreets;
    BOOL    Field;
    BOOL    Forest;
    BOOL    Hills;
    BOOL    Mountain;
    BOOL    WaterSwim;
    BOOL    WaterBoat;
    BOOL    Underwater;
    BOOL    Air;
    BOOL    Desert;
    BOOL    Dark;
    BOOL    NoMob;
    BOOL    Indoors;
    BOOL    Private;
    BOOL    Safe;
    BOOL    Solitary;
    BOOL    PetShop;
    BOOL    NoRecall;
    char    VNum[ 8 ];
    TListBoxData  ExtraDescrList;
    BOOL    Exit[ 6 ];
//    char    ToPrompt[ 255 ];
//    char    ToText[ 255 ];
//    char    KeyPrompt[ 255 ];
//    char    KeyText[ 255 ];
//    char    WordPrompt[ 255 ];
//    char    WordText[ 255 ];
//    char    ExitDescrPrompt[ 255 ];
//    char    ExitDescrText[ 255 ];
//    BOOL    IsDoor;
//    BOOL    IsPickProof;
    BOOL    WalkMode;
    BOOL    CreateMode;
    TListBoxData  MobList;
    TListBoxData  ObjList;
    char    RandomDoors[ 255 ];
//    char    DoorStatus[ 255 ];
//    BOOL    IsBashProof;
//    BOOL    IsPassProof;
    BOOL    Peaceful;
    BOOL    RFlag65536;
    BOOL    RFlag131072;
    BOOL    RFlag262144;
    BOOL    RFlag32768;
//{{TRoomDialogXFER_DATA_END}}
};


class TRoomDialog : public TDialog {
    const TRoom *TheActualRoom;
    TRoom WorkRoom;
    int    nIndex;
    TAreaDocument *AreaDoc;
    HICON hSectorIcons[SECT_MAX];
    HICON hDoorIcons[4];
    int    nCurrExit;
    TRoom *ModifiedRooms[6]; // @@@ Make const?
    TExit ModifiedOtherSides[6];
    BOOL  fDeletedOtherSide[6];
    BOOL  fCreateMode;

public:
    TRoomDialog (TWindow* parent, const TRoom *room, int n,
       TResId resId = IDD_ROOM, TModule* module = 0);
    virtual ~TRoomDialog ();

//{{TRoomDialogXFER_DEF}}
protected:
    TEdit *Name;
    TEdit *Description;
    TStatic *SectorType;
    TRadioButton *Inside;
    TRadioButton *CityStreets;
    TRadioButton *Field;
    TRadioButton *Forest;
    TRadioButton *Hills;
    TRadioButton *Mountain;
    TRadioButton *WaterSwim;
    TRadioButton *WaterBoat;
    TRadioButton *Underwater;
    TRadioButton *Air;
    TRadioButton *Desert;
    TCheckBox *Dark;
    TCheckBox *NoMob;
    TCheckBox *Indoors;
    TCheckBox *Private;
    TCheckBox *Safe;
    TCheckBox *Solitary;
    TCheckBox *PetShop;
    TCheckBox *NoRecall;
    TStatic *VNum;
    TListBox *ExtraDescrList;
    TRadioButton *Exit[ 6 ];
    TStatic *ToPrompt;
    TStatic *ToText;
    TStatic *KeyPrompt;
    TStatic *KeyText;
    TStatic *WordPrompt;
    TStatic *WordText;
    TStatic *ExitDescrPrompt;
    TStatic *ExitDescrText;
    TCheckBox *IsDoor;
    TCheckBox *IsPickProof;
    TButton *WalkCreate;
    TButton *DelExit;
    TButton *SetExit;
    TRadioButton *WalkMode;
    TRadioButton *CreateMode;
    TListBox *MobList;
    TListBox *ObjList;
    TEdit *RandomDoors;
    TStatic *DoorStatus;
    TButton *AddMob;
    TButton *AddObj;
    TCheckBox *IsBashProof;
    TCheckBox *IsPassProof;
    TCheckBox *Peaceful;
    TCheckBox *RFlag65536;
    TCheckBox *RFlag131072;
    TCheckBox *RFlag262144;
    TCheckBox *RFlag32768;
    TButton *ObjectContents;
    TButton *Code;

//{{TRoomDialogXFER_DEF_END}}
    void LoadTransfer();
    void SaveTransfer();
    void EnableExits();
    void ViewAnExit(int n);
    void LoadAnotherRoom(const TRoom *);
    void CheckDoorFlags();

//{{TRoomDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TRoomDialogVIRTUAL_END}}

//{{TRoomDialogRSP_TBL_BEGIN}}
protected:
    void CmPrev ();
    void CmNext ();
    void CmSectorTypeClicked ();
    void CmNorth ();
    void CmEast ();
    void CmSouth ();
    void CmWest ();
    void CmUp ();
    void CmDown ();
    void CmEditExtra ();
    void CmAddExtra ();
    void CmDelExit ();
    void CmOk ();
    void CmWalkCreate ();
    void CmDelExtra ();
    void CmSetExit ();
    void CmIsDoorChanged ();
    void CmCreateMode ();
    void CmWalkMode ();
    void CmCancel ();
    void CmAddMobReset ();
    void CmMobEquip ();
    void CmDelMobReset ();
    void CmAddObj ();
    void CmHelp ();
    void CmObjectContents ();
    void CmIsContainer ();
    void CmRomProps ();
//{{TRoomDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TRoomDialog);
};    //{{TRoomDialog}}


//{{TDialog = TRomRoomDialog}}
struct TRomRoomDialogXfer {
//{{TRomRoomDialogXFER_DATA}}
   BOOL Law;
   BOOL Nowhere;
   char HealRate[16];
   char ManaRate[16];
   char Owner[64];
   char Clan[64];
//{{TRomRoomDialogXFER_DATA_END}}
};


class TRomRoomDialog : public TDialog {
    TRoom *m_pRoom;

public:
    TRomRoomDialog (TWindow* parent, TRoom *pRoom, TModule* module = 0);
    virtual ~TRomRoomDialog ();

//{{TRomRoomDialogXFER_DEF}}
protected:
   TCheckBox *Law;
   TCheckBox *Nowhere;
   TEdit *HealRate;
   TEdit *ManaRate;
   TEdit *Owner;
   TEdit *Clan;
//{{TRomRoomDialogXFER_DEF_END}}

//{{TRomRoomDialogVIRTUAL_BEGIN}}
public:
    virtual void SetupWindow ();
//{{TRomRoomDialogVIRTUAL_END}}

//{{TRomRoomDialogRSP_TBL_BEGIN}}
protected:
    void CmOk ();
    void CmHelp ();
//{{TRomRoomDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TRomRoomDialog);
};    //{{TRomRoomDialog}}


#endif                                      // __roomdia_h sentry.

