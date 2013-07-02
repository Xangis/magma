/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         mobdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TMobDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "mobdia.h"
#include "shopdia.h"
#include "appdia.h"
#include "mobprog.h"

////////////////////////////////////////////////////////////////////////////
// TMobDialog

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TMobDialog, TDialog)
//{{TMobDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDC_PREV, CmPrev),
    EV_BN_CLICKED(IDC_NEXT, CmNext),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDC_MALE, CmSexChanged),
    EV_BN_CLICKED(IDC_FEMALE, CmSexChanged),
    EV_BN_CLICKED(IDC_NEUTER, CmSexChanged),
    EV_BN_CLICKED(IDC_EITHER, CmSexChanged),
    EV_CBN_SELCHANGE(IDC_SPECFUN, CmSpecChanged),
    EV_BN_CLICKED(IDHELP, CmHelp),
//    EV_BN_CLICKED(IDC_ASSISTALWAYS, CmAlwaysAssist),
    EV_BN_CLICKED(IDC_RESETS, CmViewResets),
    EV_BN_CLICKED(IDC_MOBPROG, CmMobProg),
    EV_BN_CLICKED(IDC_MAKEKEEPER, CmMakeKeeper),
//{{TMobDialogRSP_TBL_END}}
    EV_CBN_KILLFOCUS(IDC_RACE, CmRaceChanged),
//    EV_CBN_SELCHANGE(IDC_RACE, CmRaceChanged),
    EV_BN_CLICKED(IDC_CODE, CmRomProps),
    EV_BN_CLICKED(IDC_PARTS, CmParts),
    EV_BN_CLICKED(IDC_BITS, CmBits),
END_RESPONSE_TABLE;


// Make part of class
struct SpecialNameAndIcon {
   char *szName;
//   int nMode;
   poundsApp::currMode nMode;
   int nIcon;
};

static SpecialNameAndIcon Specials[nelems(((TMobDialog*)0)->hMobSpecIcon)] = {
   { "[none]",           poundsApp::MercMode, IDI_SPEC_NONE },
   { "breath_any",       poundsApp::MercMode, IDI_SPEC_BREATH },
   { "breath_acid",      poundsApp::MercMode, IDI_SPEC_BREATH },
   { "breath_fire",      poundsApp::MercMode, IDI_SPEC_BREATH },
   { "breath_frost",     poundsApp::MercMode, IDI_SPEC_BREATH },
   { "breath_gas",       poundsApp::MercMode, IDI_SPEC_BREATH },
   { "breath_lightning", poundsApp::MercMode, IDI_SPEC_BREATH },
   { "cast_adept",       poundsApp::MercMode, IDI_SPEC_ADEPT },
   { "cast_cleric",      poundsApp::MercMode, IDI_SPEC_CLERIC },
   { "cast_judge",       poundsApp::MercMode, IDI_SPEC_JUDGE },

   { "cast_mage",        poundsApp::MercMode, IDI_SPEC_MAGE },
   { "cast_undead",      poundsApp::MercMode, IDI_SPEC_UNDEAD },
   { "executioner",      poundsApp::MercMode, IDI_SPEC_EXEC },
   { "fido",             poundsApp::MercMode, IDI_SPEC_FIDO },
   { "guard",            poundsApp::MercMode, IDI_SPEC_GUARD },
   { "janitor",          poundsApp::MercMode, IDI_SPEC_JANITOR },
   { "mayor",            poundsApp::MercMode, IDI_SPEC_MAYOR },
   { "poison",           poundsApp::MercMode, IDI_SPEC_POISON },
   { "thief",            poundsApp::MercMode, IDI_SPEC_THIEF },

   { "snake_charm",      poundsApp::TFCMode, IDI_SPEC_CHARM }, // @@@ bad icons
   { "cast_from_list",   poundsApp::TFCMode, IDI_SPEC_CAST_LIST },
   { "shopkeeper",       poundsApp::TFCMode, IDI_SPEC_SHOPKEEPER },
   { "obj_bribe",        poundsApp::TFCMode, IDI_SPEC_BRIBE },
   { "agg_no_object",    poundsApp::TFCMode, IDI_SPEC_NO_OBJECT },
   { "agg_if_object",    poundsApp::TFCMode, IDI_SPEC_OBJECT },
   { "follow_mob",       poundsApp::TFCMode, IDI_SPEC_FOLLOW },
   { "circuit",          poundsApp::TFCMode, IDI_SPEC_CIRCUIT },

   { "cast_ghost",       poundsApp::EnvyMode, IDI_SPEC_GHOST },
   { "cast_psionicist",  poundsApp::EnvyMode, IDI_SPEC_PSIONICIST },
   { "repairman",        poundsApp::EnvyMode, IDI_SPEC_REPAIRMAN },

   { "cast_ghost",       poundsApp::Envy2Mode, IDI_SPEC_GHOST },
   { "cast_psionicist",  poundsApp::Envy2Mode, IDI_SPEC_PSIONICIST },
   { "repairman",        poundsApp::Envy2Mode, IDI_SPEC_REPAIRMAN },

   { "nasty",            poundsApp::Rom24Mode, IDI_SPEC_NASTY },
   { "troll_member",     poundsApp::Rom24Mode, IDI_SPEC_TROLL },
   { "ogre_member",      poundsApp::Rom24Mode, IDI_SPEC_OGRE },
   { "patrolman",        poundsApp::Rom24Mode, IDI_SPEC_GUARD },

   { "",                (poundsApp::currMode) -1, IDI_UNKNOWN_SPEC }
};

//{{TMobDialog Implementation}}


static TMobDialogXfer TMobDialogData;

TMobDialog::TMobDialog (TWindow* parent, const TMobile *mob, int nNdx,
        TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkMob( *mob )
{
//{{TMobDialogXFER_USE}}
    NameList = new TEdit(this, IDC_NAMELIST, 255);
    ShortDescr = new TEdit(this, IDC_SHORT_DESCR, 255);
    LongDescr = new TEdit(this, IDC_LONG_DESCR, 255);
    LookDescr = new TEdit(this, IDC_LOOK, 4096);
    SpecFun = new TComboBox(this, IDC_SPECFUN, 1);
    BOOL fRom = (WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode ||
                 WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
    if (fRom)
      Level = new TEdit(this, IDC_LEVEL, 255);
    else
      Level = new TThacoEdit(this, IDC_LEVEL, 255);
    Male = new TRadioButton(this, IDC_MALE, 0);
    Female = new TRadioButton(this, IDC_FEMALE, 0);
    Neuter = new TRadioButton(this, IDC_NEUTER, 0);
    Either = new TRadioButton(this, IDC_EITHER, 0);
    Align = new TEdit(this, IDC_ALIGN, 255);
    Sentinel = new TCheckBox(this, IDC_SENTINEL, 0);
    Scavenger = new TCheckBox(this, IDC_SCAVENGER, 0);
    Aggresive = new TCheckBox(this, IDC_AGGRESSIVE, 0);
    StayArea = new TCheckBox(this, IDC_STAY_AREA, 0);
    Wimpy = new TCheckBox(this, IDC_WIMPY, 0);
    TrainPCs = new TCheckBox(this, IDC_TRAIN, 0);
    PracticePCs = new TCheckBox(this, IDC_PRACTICE, 0);
    Gamble = new TCheckBox(this, IDC_GAMBLE, 0);
    Invisible = new TCheckBox(this, IDC_INVISIBLE, 0);
    DetectInvis = new TCheckBox(this, IDC_DETECTINVIS, 0);
    DetectHidden = new TCheckBox(this, IDC_DETECT_HIDDEN, 0);
    FaerieFire = new TCheckBox(this, IDC_FAERIE_FIRE, 0);
    Infravision = new TCheckBox(this, IDC_INFRAVISION, 0);
    ProtEvil = new TCheckBox(this, IDC_PROT_FROM_EVIL, 0);
    Sneak = new TCheckBox(this, IDC_SNEAK, 0);
    Hide = new TCheckBox(this, IDC_HIDE, 0);
    Flying = new TCheckBox(this, IDC_FLYING, 0);
    PassDoor = new TCheckBox(this, IDC_PASSDOOR, 0);
//    WaterWalk = new TCheckBox(this, IDC_WATERWALK, 0);
//    Mute = new TCheckBox(this, IDC_MUTE, 0);
//    Gills = new TCheckBox(this, IDC_GILLS, 0);
    Sanctuary = new TCheckBox(this, IDC_SANCT, 0);
    SuperWimpy = new TCheckBox(this, IDC_SUPER_WIMPY, 0);
    VNum = new TStatic(this, IDC_VNUM, 255);
    ProtGood = new TCheckBox(this, IDC_PROT_FROM_GOOD, 0);
    NoSleep = new TCheckBox(this, IDC_NOSLEEP, 0);
    NoSummon = new TCheckBox(this, IDC_NOSUMMON, 0);
    SexIcon = new TStatic(this, IDC_SEXICON, 255);
    SpecIcon = new TStatic(this, IDC_SPECICON, 255);
    HitPoints = new TStatic(this, IDC_HP, 255);
    ArmorClass = new TStatic(this, IDC_AC, 255);
    Thac0 = new TStatic(this, IDC_THAC0, 255);
//    AssistChance = new TCheckBox(this, IDC_ASSIST, 0);
//    AssistAlways = new TCheckBox(this, IDC_ASSISTALWAYS, 0);
//    AssistSame = new TCheckBox(this, IDC_ASSISTSAME, 0);
    AntiMagic = new TCheckBox(this, IDC_ANTIMAGIC, 0);
    FireShield = new TCheckBox(this, IDC_FIRESHIELD, 0);
    ImprovedInvis = new TCheckBox(this, IDC_IMPROVED_INVIS, 0);
    Invulnerable = new TCheckBox(this, IDC_INVULNERABLE, 0);
    NoSteal = new TCheckBox(this, IDC_NOSTEAL, 0);
    NoCharm = new TCheckBox(this, IDC_NOCHARM, 0);
    RacePrompt = new TStatic(this, IDC_RACEPROMPT, 0);
    Race = new TComboBox(this, IDC_RACE, 255);
    Code = new TButton(this, IDC_CODE);
    HpPrompt = new TStatic(this, IDC_HPPROMPT);
    AcPrompt = new TStatic(this, IDC_ACPROMPT);
    Thac0Prompt = new TStatic(this, IDC_THAC0PROMPT);
    Bits = new TButton(this, IDC_BITS);
    Parts = new TButton(this, IDC_PARTS);

    SetTransferBuffer(&TMobDialogData);
//{{TMobDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheActualMobile = mob;
    nIndex = nNdx;

    char szBuffer[32], szSpecInfo[64];
    for (int i = 0; i < MAX_CUSTOM_SPEC; i++)
       {
       wsprintf(szBuffer, "specfun%d", i);
       GetProfileString("MAKE.ZONES.FAST", szBuffer, "", szSpecInfo,
          sizeof szSpecInfo);
       if (szSpecInfo[0])
          szCustomSpec[i] = strcpy(new char[strlen(szSpecInfo)+1], szSpecInfo);
       else
          szCustomSpec[i] = 0;
       }

    VNum->EnableTransfer();
    HitPoints->EnableTransfer();
    ArmorClass->EnableTransfer();
    Thac0->EnableTransfer();
//    Level->SetValidator(new TThacoValidator(this));

    for (i = 0; i < nelems(hMobSpecIcon); i++)
       hMobSpecIcon[i] = GetApplication()->LoadIcon(
          MAKEINTRESOURCE(Specials[i].nIcon));

    static int nSexIcon[] = { IDI_NEUTER, IDI_MALE, IDI_FEMALE, IDI_EITHER };

    for (i = 0; i < nelems(hMobSexIcon); i++)
       hMobSexIcon[i] = GetApplication()->LoadIcon(
          MAKEINTRESOURCE(nSexIcon[i]));

    InitRaceComboData();
    LoadTransfer();
}


TMobDialog::~TMobDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    for (int i = 0; i < nelems(hMobSpecIcon); i++)
       DestroyIcon(hMobSpecIcon[i]);

    for (i = 0; i < nelems(hMobSexIcon); i++)
       DestroyIcon(hMobSexIcon[i]);

    for (i = 0; i < MAX_CUSTOM_SPEC; i++)
       delete[] szCustomSpec[i];
}


void TMobDialog::CmPrev ()
{
    // INSERT>> Your code here.
    const TMobile *m;
    if ((m = WorkMob.AreaDoc->GetMobileFromIndex(nIndex-1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex--;
       LoadAnotherMobile(m);
       return;
       }

    MessageBeep(-1);
}


void TMobDialog::CmNext ()
{
    // INSERT>> Your code here.
    const TMobile *m;
    if ((m = WorkMob.AreaDoc->GetMobileFromIndex(nIndex+1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex++;
       LoadAnotherMobile(m);
       return;
       }

    MessageBeep(-1);
}

// @@@ Make part of class
#define UMAX( a, b )		( ( a ) > ( b ) ? ( a ) : ( b ) )
/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_xx )
{
    // @@@ I think this needs to be 47 for EnvyMud...
    return value_00 + level * ( value_xx - value_00 ) / 32;
}

void TMobDialog::LoadTransfer()
{
	 strcpy(TMobDialogData.NameList, WorkMob.player_name.c_str());
	 strcpy(TMobDialogData.ShortDescr, WorkMob.short_descr.c_str());
	 strcpy(TMobDialogData.LongDescr, WorkMob.long_descr.c_str());
	 strcpy(TMobDialogData.LookDescr, WorkMob.description.c_str());

    TMobDialogData.SpecFun.Clear();

    // Add the spec funs specific to this server type
    BOOL fKnownSpec = FALSE;
    for (int i = 0; i < nelems(Specials); i++)
       if (Specials[i].nMode == poundsApp::MercMode ||
           Specials[i].nMode == (int) WorkMob.AreaDoc->GetMode())
       {
       BOOL fSelected;
       if (WorkMob.spec_fun.is_null())
          fSelected = (i == 0);
       else
          fSelected = (WorkMob.spec_fun == string("spec_") + Specials[i].szName);

       fKnownSpec |= fSelected;
       TMobDialogData.SpecFun.AddStringItem(Specials[i].szName, i, fSelected);
       }

    // Add customized spec-funs
    for (i = 0; i < MAX_CUSTOM_SPEC; i++)
       if (szCustomSpec[i])
          {
          BOOL fSelected = (WorkMob.spec_fun == string("spec_") + szCustomSpec[i]);
          fKnownSpec |= fSelected;
          TMobDialogData.SpecFun.AddStringItem(szCustomSpec[i], nelems(Specials)-1, fSelected);
          }

    // If this mobile has an unknown spec fun, add that too
    if (!fKnownSpec)
       TMobDialogData.SpecFun.AddStringItem(WorkMob.spec_fun.c_str(),
          nelems(Specials)-1, TRUE);

    wsprintf(TMobDialogData.Level, "%d", WorkMob.level);
    TMobDialogData.Male = (WorkMob.sex == SEX_MALE);
    TMobDialogData.Female = (WorkMob.sex == SEX_FEMALE);
    TMobDialogData.Neuter = (WorkMob.sex == SEX_NEUTRAL);
    TMobDialogData.Either = (WorkMob.sex == SEX_EITHER);
    wsprintf(TMobDialogData.Align, "%d", WorkMob.alignment);

    LoadActAff();

    BOOL fRom = (WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode ||
                 WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
    BOOL fEnvy2 = WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode;
    if (fRom || fEnvy2)
//       strcpy(TMobDialogData.Race, WorkMob.strrace.c_str());
       TMobDialogData.Race.SelectString(WorkMob.strrace.c_str());

    wsprintf(TMobDialogData.VNum, "%d", WorkMob.vnum);
}

void TMobDialog::LoadActAff()
{
    // Action flags
    TMobDialogData.Sentinel = IS_SET(WorkMob.act, ACT_SENTINEL);
    TMobDialogData.Scavenger = IS_SET(WorkMob.act, ACT_SCAVENGER);
    TMobDialogData.Aggresive = IS_SET(WorkMob.act, ACT_AGGRESSIVE);
    TMobDialogData.StayArea = IS_SET(WorkMob.act, ACT_STAY_AREA);
    TMobDialogData.Wimpy = IS_SET(WorkMob.act, ACT_WIMPY);
    TMobDialogData.TrainPCs = IS_SET(WorkMob.act, ACT_TRAIN);
    TMobDialogData.PracticePCs = IS_SET(WorkMob.act, ACT_PRACTICE);
    if (WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode || WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode)
       TMobDialogData.Gamble = IS_SET(WorkMob.act, ACT_GAMBLE);
    else if (WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode)
       {
       TMobDialogData.SuperWimpy = IS_SET(WorkMob.act, ACT_SUPERWIMP);
//       TMobDialogData.AssistChance = IS_SET(WorkMob.act, ACT_ASSIST);
//       TMobDialogData.AssistAlways = IS_SET(WorkMob.act, ACT_ASSISTALWAYS);
//       TMobDialogData.AssistSame = IS_SET(WorkMob.act, ACT_ASSISTSAME);
       }

    // Affected by
    TMobDialogData.Invisible = IS_SET(WorkMob.affected_by, AFF_INVISIBLE);
    TMobDialogData.DetectInvis = IS_SET(WorkMob.affected_by, AFF_DETECT_INVIS);
    TMobDialogData.DetectHidden = IS_SET(WorkMob.affected_by, AFF_DETECT_HIDDEN);
    TMobDialogData.FaerieFire = IS_SET(WorkMob.affected_by, AFF_FAERIE_FIRE);
    TMobDialogData.Infravision = IS_SET(WorkMob.affected_by, AFF_INFRARED);
    TMobDialogData.ProtEvil = IS_SET(WorkMob.affected_by, AFF_PROTECT);
    TMobDialogData.Sneak = IS_SET(WorkMob.affected_by, AFF_SNEAK);
    TMobDialogData.Hide = IS_SET(WorkMob.affected_by, AFF_HIDE);
    TMobDialogData.Sanctuary = IS_SET(WorkMob.affected_by, AFF_SANCTUARY);
    TMobDialogData.Flying = IS_SET(WorkMob.affected_by, AFF_FLYING);
    TMobDialogData.PassDoor = IS_SET(WorkMob.affected_by, AFF_PASS_DOOR);
//    if (WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode || WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode) // @@@ Make these vis
//       {
//       TMobDialogData.WaterWalk = IS_SET(WorkMob.affected_by, AFF_WATERWALK);
//       TMobDialogData.Mute = IS_SET(WorkMob.affected_by, AFF_MUTE);
//       TMobDialogData.Gills = IS_SET(WorkMob.affected_by, AFF_GILLS);
//       }
//    else
//    if (WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode)
//       {
       TMobDialogData.ProtGood = IS_SET(WorkMob.affected_by, AFF_PROTECT_GOOD);
       TMobDialogData.NoSleep = IS_SET(WorkMob.affected_by, AFF_NOSLEEP);
       TMobDialogData.NoSummon = IS_SET(WorkMob.affected_by, AFF_NOSUMMON);
       TMobDialogData.NoCharm = IS_SET(WorkMob.affected_by, AFF_NOCHARM);
       TMobDialogData.AntiMagic = IS_SET(WorkMob.affected_by, AFF_ANTI_MAGIC);
       TMobDialogData.FireShield = IS_SET(WorkMob.affected_by, AFF_FIRESHIELD);
       TMobDialogData.ImprovedInvis = IS_SET(WorkMob.affected_by, AFF_IMPROVED_INVIS);
       TMobDialogData.Invulnerable = IS_SET(WorkMob.affected_by, AFF_INVULNERABLE);
       TMobDialogData.NoSteal = IS_SET(WorkMob.affected_by, AFF_NOSTEAL);
//       }
}

void TMobDialog::SetThac0(BOOL fROM, int nLevel)
{
    if (fROM) {
        HitPoints->ShowWindow(SW_HIDE);
        ArmorClass->ShowWindow(SW_HIDE);
        Thac0->ShowWindow(SW_HIDE);
        HpPrompt->ShowWindow(SW_HIDE);
        AcPrompt->ShowWindow(SW_HIDE);
        Thac0Prompt->ShowWindow(SW_HIDE);
        Bits->ShowWindow(SW_SHOWNORMAL);
        Parts->ShowWindow(SW_SHOWNORMAL);
        return;
    }
/*
    Stats are calculated thusly:
    min_hp = (level - 1) * 8 + (level-1) * (level - 1) / 4
    max_hp = (level + 1) * 8 + (level+1) * (level + 1)
    ac = interpolate(level, 100, -100)
    thac0 = interpolate(level, 20, 0)
    min_dam = (level - 1) / 2
    max_dam = (level + 1) * 3 / 2
*/
    Bits->ShowWindow(SW_HIDE);
    Parts->ShowWindow(SW_HIDE);

    int minlevel = UMAX(1, nLevel -1), maxlevel = nLevel + 1;
    int min_hp = minlevel * 8 + minlevel * minlevel / 4;
    int max_hp = maxlevel * 8 + maxlevel * maxlevel;
    int ac = interpolate(nLevel, 100, -100);
    int thac0 = interpolate(nLevel, 20, 0);

    wsprintf(TMobDialogData.HitPoints, "%d - %d", min_hp, max_hp);
    wsprintf(TMobDialogData.ArmorClass, "%d", ac);
    wsprintf(TMobDialogData.Thac0, "%d", thac0);

    HitPoints->SetText(TMobDialogData.HitPoints);
    ArmorClass->SetText(TMobDialogData.ArmorClass);
    Thac0->SetText(TMobDialogData.Thac0);
}

void TMobDialog::SaveTransferToWork()
{
    WorkMob.player_name = TMobDialogData.NameList;
	 WorkMob.short_descr = TMobDialogData.ShortDescr;
	 WorkMob.long_descr = TMobDialogData.LongDescr;
	 WorkMob.description = TMobDialogData.LookDescr;
//    WorkMob.strrace = TMobDialogData.Race;
    WorkMob.strrace = TMobDialogData.Race.GetSelection();

    char szBuffer[128];
    TMobDialogData.SpecFun.GetSelString(szBuffer, sizeof szBuffer);
    if (strcmp(szBuffer, "[none]") == 0)
       WorkMob.spec_fun = "";
    else
       {
       if (memcmp(szBuffer, "spec_", 5) == 0)
          WorkMob.spec_fun = szBuffer;
       else
          {
          WorkMob.spec_fun = "spec_";
          WorkMob.spec_fun += szBuffer;
          }
       }

	 WorkMob.level = (sh_int) atoi(TMobDialogData.Level);

    if (TMobDialogData.Male) 			WorkMob.sex = SEX_MALE;
    else if (TMobDialogData.Female) WorkMob.sex = SEX_FEMALE;
    else if (TMobDialogData.Neuter) WorkMob.sex = SEX_NEUTRAL;
    else if (TMobDialogData.Either) WorkMob.sex = SEX_EITHER;

    WorkMob.alignment = (sh_int) atoi(TMobDialogData.Align);

    SaveActAff();
}

void TMobDialog::SaveTransfer()
{
    SaveTransferToWork();
    WorkMob.AreaDoc->MobileSave(WorkMob, nIndex);
}

void TMobDialog::SaveActAff ()
{
    // Action flags
    SET_RESET_BIT(WorkMob.act, TMobDialogData.Sentinel, ACT_SENTINEL);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.Scavenger, ACT_SCAVENGER);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.Aggresive, ACT_AGGRESSIVE);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.StayArea, ACT_STAY_AREA);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.Wimpy, ACT_WIMPY);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.TrainPCs, ACT_TRAIN);
    SET_RESET_BIT(WorkMob.act, TMobDialogData.PracticePCs, ACT_PRACTICE);
    if (WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode || WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode)
       SET_RESET_BIT(WorkMob.act, TMobDialogData.Gamble, ACT_GAMBLE)
    else if (WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode)
       {
       SET_RESET_BIT(WorkMob.act, TMobDialogData.SuperWimpy, ACT_SUPERWIMP);
//       SET_RESET_BIT(WorkMob.act, TMobDialogData.AssistChance, ACT_ASSIST);
//       SET_RESET_BIT(WorkMob.act, TMobDialogData.AssistAlways, ACT_ASSISTALWAYS);
//       SET_RESET_BIT(WorkMob.act, TMobDialogData.AssistSame, ACT_ASSISTSAME);
       }

    // Affected by
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Invisible, AFF_INVISIBLE);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.DetectInvis, AFF_DETECT_INVIS);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.DetectHidden, AFF_DETECT_HIDDEN);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.FaerieFire, AFF_FAERIE_FIRE);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Infravision, AFF_INFRARED);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.ProtEvil, AFF_PROTECT);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Sneak, AFF_SNEAK);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Hide, AFF_HIDE);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Sanctuary, AFF_SANCTUARY);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Flying, AFF_FLYING);
    SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.PassDoor, AFF_PASS_DOOR);
//    if (WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode || WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode)
//       {
//       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.WaterWalk, AFF_WATERWALK);
//       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Mute, AFF_MUTE);
//       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Gills, AFF_GILLS);
//       }
//    else
//    if (WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode)
//       {
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.ProtGood, AFF_PROTECT_GOOD);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.NoSleep, AFF_NOSLEEP);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.NoSummon, AFF_NOSUMMON);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.NoCharm, AFF_NOCHARM);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.AntiMagic, AFF_ANTI_MAGIC);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.FireShield, AFF_FIRESHIELD);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.ImprovedInvis, AFF_IMPROVED_INVIS);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.Invulnerable, AFF_INVULNERABLE);
       SET_RESET_BIT(WorkMob.affected_by, TMobDialogData.NoSteal, AFF_NOSTEAL);
//       }
}

void TMobDialog::LoadAnotherMobile(const TMobile *m)
{
   TheActualMobile = m;
   WorkMob = *m;
   LoadTransfer();
   TransferData(tdSetData);
   CmSexChanged();
   CmSpecChanged();
   BOOL fRom = (WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode ||
                 WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
   SetThac0(fRom, WorkMob.level);
//   CmAlwaysAssist();
}


void TMobDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}


void TMobDialog::CmMakeKeeper ()
{
    // INSERT>> Your code here.
    if (nIndex == -1)
       {
       MessageBox("Sorry, you cannot currently make a mobile a shopkeeper "
          "until you have written him to the database by clicking OK on him.",
          "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
       return;
       }


    int nShopIndex = WorkMob.AreaDoc->GetShopIndexFromMobileVNum(WorkMob.vnum);
    // @@@ This is kinda messed up, TAreaDocument should be doing this...
    TShopData* shop;
    if (nShopIndex < 0)
       {
       if (MessageBox("Make this mobile a shopkeeper?",
            "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_YESNO) != IDYES)
          return;
       else
          {
          shop = WorkMob.AreaDoc->MakeNewShop();
          shop->keeper = WorkMob.vnum;
          }
       }
    else
       shop = WorkMob.AreaDoc->TheShops[nShopIndex];

    TShopDialog(WorkMob.AreaDoc->GetDocManager().GetApplication()->GetMainWindow(),
       WorkMob.AreaDoc, shop, nShopIndex, TRUE).Execute();
    if (nShopIndex < 0)
       delete shop;
//    WorkMob.AreaDoc->EditItem(4, nShopIndex);
}


void TMobDialog::CmSexChanged ()
{
    // INSERT>> Your code here.
    int nCurrentSex = SEX_NEUTRAL;
    if (Male->GetCheck() == BF_CHECKED)
       nCurrentSex = SEX_MALE;
    else if (Female->GetCheck() == BF_CHECKED)
       nCurrentSex = SEX_FEMALE;

    // Rom 2.4 only
    else if (Either->GetCheck() == BF_CHECKED)
       nCurrentSex = SEX_EITHER;

    SexIcon->SendMessage(STM_SETICON, (WPARAM) hMobSexIcon[nCurrentSex], 0);
}


void TMobDialog::CmSpecChanged ()
{
    // INSERT>> Your code here.
    int n = SpecFun->GetSelIndex();
    if (n >= 0)
      {
      n = (int) SpecFun->GetItemData(n);
      if (n >= 0 && n < nelems(hMobSpecIcon))
         SpecIcon->SendMessage(STM_SETICON, (WPARAM) hMobSpecIcon[n], 0);
      }
}


void TMobDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
//    LookDescr->SendMessage(WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), 0);
    LookDescr->SendMessage(WM_SETFONT,
       (WPARAM) (HFONT) WorkMob.AreaDoc->GetMZF()->hFixedFont, 0);
//    CmAlwaysAssist ();

    // Envy-only acts and affects
    BOOL fEnvy2 = WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode;
    Gamble->EnableWindow(WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode ||
                         fEnvy2);

    // TFC-only acts and affects
    BOOL fTFC = WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode;
    SuperWimpy->EnableWindow(fTFC);
//    AssistChance->EnableWindow(fTFC);
//    AssistAlways->EnableWindow(fTFC);
//    AssistSame->EnableWindow(fTFC);

    // ROM 2.4-only stuff
    BOOL fRom24 = WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode;
    Either->EnableWindow(fRom24);

    // Other/combined stuff
    BOOL fRom = fRom24 || (WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
    BOOL fEnvy = fEnvy2 || (WorkMob.AreaDoc->GetMode() == poundsApp::EnvyMode);
    if (fRom) {
       NoSteal->SetWindowText("Flaming");
       NoSummon->SetWindowText("Plague");
       NoCharm->SetWindowText("Weaken");
       AntiMagic->SetWindowText("Dk Vis");
       Invulnerable->SetWindowText("Berserk");
       ImprovedInvis->SetWindowText("Swim");
    } else if (fEnvy) {
       NoSummon->SetWindowText("Mute");
       NoCharm->SetWindowText("Gills");
    }
    NoSummon->EnableWindow(fTFC || fEnvy || fRom);
    NoCharm->EnableWindow(fTFC || fEnvy || fRom);
    AntiMagic->EnableWindow(fTFC || fRom);
    ImprovedInvis->EnableWindow(fTFC || fRom);
    Invulnerable->EnableWindow(fTFC || fRom);
    NoSteal->EnableWindow(fTFC || fRom);

    Code->EnableWindow(fRom);
    RacePrompt->EnableWindow(fEnvy2 || fRom);
    Race->EnableWindow(fRom || fEnvy2);
    ProtGood->EnableWindow(fRom || fTFC);

    CmSexChanged();
    CmSpecChanged();

    SetThac0(fRom, WorkMob.level);
}

#if 0
//==============================================================================
BOOL TThacoValidator::IsValidInput(char far* p, BOOL suppressFill)
{
   mobdia->SetThac0();
   return TValidator::IsValidInput(p, suppressFill);
}
#endif

DEFINE_RESPONSE_TABLE1(TThacoEdit, TEdit)
  EV_WM_KILLFOCUS,
END_RESPONSE_TABLE;

void TThacoEdit::EvKillFocus(HWND hWndGetFocus)
{
   GetText(TMobDialogData.Level, sizeof TMobDialogData.Level);
   mobdia->SetThac0(FALSE, atoi(TMobDialogData.Level));
   TEdit::EvKillFocus(hWndGetFocus);
}


void TMobDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_MOBDIA);
}


//void TMobDialog::CmAlwaysAssist ()
//{
//    // INSERT>> Your code here.
//    BOOL f = (WorkMob.AreaDoc->GetMode() == poundsApp::TFCMode) &&
//             (AssistAlways->GetCheck() != BF_CHECKED);
//
//    AssistChance->EnableWindow(f);
//    AssistSame->EnableWindow(f);
//}


void TMobDialog::CmViewResets ()
{
    // INSERT>> Your code here.
    TResetRoomDialog (this, WorkMob.AreaDoc, WorkMob.vnum).Execute();
}


void TMobDialog::CmMobProg ()
{
    // INSERT>> Your code here.
    TMobProgDia(this, &WorkMob, WorkMob.mobprogs).Execute();
}

void TMobDialog::CmRomProps ()
{
    // Update the work object, in case something we need changed (e.g. race)
    TransferData(tdGetData);
    SaveTransferToWork();

//    if (TRomMobDialog(this, IDD_ROMMOB).Execute() == IDOK) [
    TMobile TempMob(WorkMob);
//    if (TRomMobDialog(this, TempMob).Execute() == IDOK) {
    if (TNewRomMobDialog(this, TempMob).Execute() == IDOK) {
        WorkMob = TempMob;

        LoadTransfer();
        TransferData(tdSetData);
//        SetThac0(TRUE, WorkMob.level);
    }
}

void TMobDialog::CmParts ()
{
    // Update the work object, in case something we need changed (e.g. race)
    TransferData(tdGetData);
//    SaveTransfer();
    SaveTransferToWork();

    TMobile TempMob(WorkMob);
    if (TMobPartsDialog(this, TempMob).Execute() == IDOK) {
        WorkMob = TempMob;

        LoadTransfer();
        TransferData(tdSetData);
    }
}

void TMobDialog::CmBits ()
{
    // Update the work object, in case something we need changed (e.g. race)
    TransferData(tdGetData);
//    SaveTransfer();
    SaveTransferToWork();

    TMobile TempMob(WorkMob);
    if (TMobBitsDialog(this, TempMob).Execute() == IDOK) {
        WorkMob = TempMob;

        LoadTransfer();
        TransferData(tdSetData);
    }
}

void TMobDialog::CmRaceChanged ()
{
   char szNewRace[64];
   Race->GetText(szNewRace, sizeof szNewRace);
   if (TMobDialogData.Race.GetSelection() != szNewRace) {
//      MessageBox(szNewRace, "@@@New Race Is");

      // Save the bits on the screen to the work object
      TransferData(tdGetData);
//      MessageBox(TMobDialogData.Race.GetSelection().c_str(), "@@@tdSetData saved");
      SaveActAff();

      WorkMob.strrace = szNewRace;
      BOOL fRom = (WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode ||
                   WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
      BOOL fEnvy2 = WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode;

      // Remove the bits the old race implied
      if (fRom) {
         WorkMob.act         &= ~TAreaDocument::rom_race_table[WorkMob.race].act;
         WorkMob.affected_by &= ~TAreaDocument::rom_race_table[WorkMob.race].aff;
         WorkMob.off_flags	  &= ~TAreaDocument::rom_race_table[WorkMob.race].off;
         WorkMob.imm_flags	  &= ~TAreaDocument::rom_race_table[WorkMob.race].imm;
         WorkMob.res_flags	  &= ~TAreaDocument::rom_race_table[WorkMob.race].res;
         WorkMob.vuln_flags  &= ~TAreaDocument::rom_race_table[WorkMob.race].vuln;
         WorkMob.form		  &= ~TAreaDocument::rom_race_table[WorkMob.race].form;
         WorkMob.parts		  &= ~TAreaDocument::rom_race_table[WorkMob.race].parts;
      } else if (fEnvy2) {
         WorkMob.affected_by &= ~TAreaDocument::envy_race_table[WorkMob.race].race_abilities;
      }

      // Get the table entry for the new race
      WorkMob.race = WorkMob.AreaDoc->race_lookup(szNewRace);

      if (fRom) {
         WorkMob.act         |= TAreaDocument::rom_race_table[WorkMob.race].act;
         WorkMob.affected_by |= TAreaDocument::rom_race_table[WorkMob.race].aff;
         WorkMob.off_flags	  |= TAreaDocument::rom_race_table[WorkMob.race].off;
         WorkMob.imm_flags	  |= TAreaDocument::rom_race_table[WorkMob.race].imm;
         WorkMob.res_flags	  |= TAreaDocument::rom_race_table[WorkMob.race].res;
         WorkMob.vuln_flags  |= TAreaDocument::rom_race_table[WorkMob.race].vuln;
         WorkMob.form		  |= TAreaDocument::rom_race_table[WorkMob.race].form;
         WorkMob.parts		  |= TAreaDocument::rom_race_table[WorkMob.race].parts;
      } else if (fEnvy2) {
         WorkMob.affected_by |= TAreaDocument::envy_race_table[WorkMob.race].race_abilities;
      }

      // Load the transfer buffer with any changes caused by race
      LoadActAff();
//      MessageBox(TMobDialogData.Race.GetSelection().c_str(), "@@@About to transfer back to dlg");
      TransferData(tdSetData);
//      MessageBox("@@@Race Changed");
//      Race->Transfer(&TMobDialogData.Race, tdGetData);
   }
}

void TMobDialog::InitRaceComboData()
{
    TMobDialogData.Race.Clear();

    BOOL fRom = (WorkMob.AreaDoc->GetMode() == poundsApp::Rom24Mode ||
                 WorkMob.AreaDoc->GetMode() == poundsApp::RomMode);
    BOOL fEnvy2 = (WorkMob.AreaDoc->GetMode() == poundsApp::Envy2Mode);

    if (fRom) {
      for (int i = 0; TAreaDocument::rom_race_table[i].name; i++)
       TMobDialogData.Race.AddString(TAreaDocument::rom_race_table[i].name,
         FALSE);
    } else if (fEnvy2) {
      for (int i = 0; i < nelems(TAreaDocument::envy_race_table); i++)
       TMobDialogData.Race.AddString(TAreaDocument::envy_race_table[i].name,
         FALSE);
    }
}

#if 0
////////////////////////////////////////////////////////////////////////////
// TRomMobDialog

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TRomMobDialog, TDialog)
//{{TRomMobDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TRomMobDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

static TRomMobDialogXfer TRomMobDialogData;

TRomMobDialog::TRomMobDialog (TWindow* parent, TMobile& TempMob, TModule *module):
    TDialog(parent, IDD_ROMMOB, module), WorkMob( TempMob )
{
    HitRoll = new TEdit(this, IDC_HITROLL, sizeof TRomMobDialogData.HitRoll);
    HitDice = new TEdit(this, IDC_HITDICE, sizeof TRomMobDialogData.HitDice);
    DamDice = new TEdit(this, IDC_DAMDICE, sizeof TRomMobDialogData.DamDice);
    DamType = new TComboBox(this, IDC_DAMTYPE, 32);
    ManaDice = new TEdit(this, IDC_MANADICE, sizeof TRomMobDialogData.ManaDice);
    for (int i = 0; i < nelems(ArmorClass); i++)
       ArmorClass[i] = new TEdit(this, IDC_AC1+i, sizeof TRomMobDialogData.ArmorClass[0]);
    StartPos = new TComboBox(this, IDC_STARTPOS, 32);
    DefPos = new TComboBox(this, IDC_DEFPOS, 32);
    Size = new TComboBox(this, IDC_ROMSIZE, 32);
    Material = new TComboBox(this, IDC_MATERIAL, 32);
    Gold = new TEdit(this, IDC_GOLD, sizeof TRomMobDialogData.Gold);
    MobGroup = new TEdit(this, IDC_MOBGROUP, sizeof TRomMobDialogData.MobGroup);
    for (i = 0; i < nelems(anRomForm); i++)
        Form[i] = new TRadioButton(this, IDC_FORMA+i);
    for (i = 0; i < nelems(anRomPart); i++)
        Part[i] = new TRadioButton(this, IDC_PARTA+i);
    for (i = 0; i < nelems(anRomImm); i++)
        Imm[i] = new TRadioButton(this, IDC_IMMA+i);
    for (i = 0; i < nelems(anRomRes); i++)
        Res[i] = new TRadioButton(this, IDC_RESB+i);
    for (i = 0; i < nelems(anRomVuln); i++)
        Vuln[i] = new TRadioButton(this, IDC_VULNC+i);
    for (i = 0; i < nelems(anRomAct); i++)
        Act[i] = new TRadioButton(this, IDC_UNDEAD+i);
    for (i = 0; i < nelems(anRomAssist); i++)
        Assist[i] = new TRadioButton(this, IDC_ALL+i);
    for (i = 0; i < nelems(anRomOff); i++)
        Off[i] = new TRadioButton(this, IDC_AREAATTACK+i);

    SetTransferBuffer(&TRomMobDialogData);
    LoadTransfer();
}

// @@@ Make part of class
const char *aszRomAttacks[] = {
    "none",
    "slice",
    "stab",
    "slash",
    "whip",
    "claw",
    "blast",
    "pound",
    "crush",
    "grep",
    "bite",
    "pierce",
    "suction",
    "beating",
    "digestion",
    "charge",
    "slap",
    "punch",
    "wrath",
    "magic",
    "divine",
    "cleave",
    "scratch",
    "peck",
    "peckb",
    "chop",
    "sting",
    "smash",
    "shbite",
    "flbite",
    "frbite",
    "acbite",
    "chomp",
    "drain",
    "thrust",
    "slime",
    "shock",
    "thwack",
    "flame",
    "chill"
};

void TRomMobDialog::LoadTransfer()
{
    wsprintf(TRomMobDialogData.HitRoll, "%d", WorkMob.hitroll);
    wsprintf(TRomMobDialogData.HitDice, "%dd%d+%d", WorkMob.hitnodice, WorkMob.hitsizedice, WorkMob.hitplus);
    wsprintf(TRomMobDialogData.DamDice, "%dd%d+%d", WorkMob.damnodice, WorkMob.damsizedice, WorkMob.damplus);
    TRomMobDialogData.DamType.Clear();
    int i;
    for (i = 0; i < nelems(aszRomAttacks); i++)
      TRomMobDialogData.DamType.AddStringItem(aszRomAttacks[i], i, WorkMob.strDamType == aszRomAttacks[i]);
//    TRomMobDialogData.DamType.AddStringItem(WorkMob.strDamType.c_str(), 0, TRUE);
    wsprintf(TRomMobDialogData.ManaDice, "%dd%d+%d", WorkMob.mana[DICE_NUMBER], WorkMob.mana[DICE_TYPE], WorkMob.mana[DICE_BONUS]);
    for (i = 0; i < nelems(TRomMobDialogData.ArmorClass); i++)
       wsprintf(TRomMobDialogData.ArmorClass[i], "%d", WorkMob.rom_ac[i]);

//    static const char *aszRomPosition[] = { "stand", "rest", "sleep" };
//    TRomMobDialogData.StartPos.Clear();
//    TRomMobDialogData.DefPos.Clear();
//    for (i = 0; i < nelems(aszRomPosition); i++) {
//        TRomMobDialogData.StartPos.AddStringItem(aszRomPosition[i], i, WorkMob.start_pos == i);
//        TRomMobDialogData.DefPos.AddStringItem(aszRomPosition[i], i, WorkMob.default_pos == i);
//    }

    WorkMob.AreaDoc->LoadPositionComboBox(&TRomMobDialogData.StartPos,
      WorkMob.start_pos);
    WorkMob.AreaDoc->LoadPositionComboBox(&TRomMobDialogData.DefPos,
      WorkMob.default_pos);

    // @@@ Move to document
    static const char *aszRomSizes[] = { "tiny", "small", "medium", "large", "huge", "giant" };
    TRomMobDialogData.Size.Clear();
    for (i = 0; i < nelems(aszRomSizes); i++) {
        TRomMobDialogData.Size.AddStringItem(aszRomSizes[i], i, WorkMob.size == i);
    }

    TRomMobDialogData.Material.Clear();
    TRomMobDialogData.Material.AddStringItem(WorkMob.material.c_str(), 0, TRUE);

    wsprintf(TRomMobDialogData.Gold, "%d", WorkMob.gold);
    wsprintf(TRomMobDialogData.MobGroup, "%d", WorkMob.mob_group);

    for (i = 0; i < nelems(anRomForm); i++)
      TRomMobDialogData.Form[i] = ((WorkMob.form & anRomForm[i]) == anRomForm[i]);

    for (i = 0; i < nelems(anRomPart); i++)
      TRomMobDialogData.Part[i] = ((WorkMob.parts & anRomPart[i]) == anRomPart[i]);

    for (i = 0; i < nelems(anRomImm); i++)
      TRomMobDialogData.Imm[i] = ((WorkMob.imm_flags & anRomImm[i]) == anRomImm[i]);

    for (i = 0; i < nelems(anRomRes); i++)
      TRomMobDialogData.Res[i] = ((WorkMob.res_flags & anRomRes[i]) == anRomRes[i]);

    for (i = 0; i < nelems(anRomVuln); i++)
      TRomMobDialogData.Vuln[i] = ((WorkMob.vuln_flags & anRomVuln[i]) == anRomVuln[i]);

    for (i = 0; i < nelems(anRomAct); i++)
      TRomMobDialogData.Act[i]    = ((WorkMob.act & anRomAct[i]) == anRomAct[i]);
    for (i = 0; i < nelems(anRomAssist); i++)
      TRomMobDialogData.Assist[i] = ((WorkMob.off_flags & anRomAssist[i]) == anRomAssist[i]);
    for (i = 0; i < nelems(anRomOff); i++)
      TRomMobDialogData.Off[i]    = ((WorkMob.off_flags & anRomOff[i]) == anRomOff[i]);
}

int32 TRomMobDialog::anRomForm[26] = {
   FORM_EDIBLE,
   FORM_POISON,
   FORM_MAGICAL,
   FORM_INSTANT_DECAY,
   FORM_OTHER,
   FORM_ANIMAL,
   FORM_SENTIENT,
   FORM_UNDEAD,
   FORM_CONSTRUCT,
   FORM_MIST,
   FORM_INTANGIBLE,
   FORM_BIPED,
   FORM_CENTAUR,
   FORM_INSECT,
   FORM_SPIDER,
   FORM_FISH,
   FORM_WORM,
   FORM_BLOB,
   FORM_MAMMAL,
   FORM_BIRD,
   FORM_REPTILE,
   FORM_SNAKE,
   FORM_DRAGON,
   FORM_AMPHIBIAN,
   FORM_CRUSTACEAN,
   FORM_COLD_BLOOD,
};

int32 TRomMobDialog::anRomPart[22] = {
   PART_HEAD,
   PART_ARMS,
   PART_LEGS,
   PART_HEART,
   PART_BRAINS,
   PART_GUTS,
   PART_HANDS,
   PART_FEET,
   PART_FINGERS,
   PART_EAR,
   PART_TENTACLES,
   PART_LONG_TONGUE,
   PART_EYESTALKS,
   PART_EYE,
   PART_FINS,
   PART_WINGS,
   PART_TAIL,
   PART_CLAWS,
   PART_FANGS,
   PART_HORNS,
   PART_SCALES,
   PART_TUSKS
};

int32 TRomMobDialog::anRomImm[19] = {
   IMM_SUMMON,
   IMM_CHARM,
   IMM_MAGIC,
   IMM_WEAPON,
   IMM_BASH,
   IMM_PIERCE,
   IMM_SLASH,
   IMM_FIRE,
   IMM_COLD,
   IMM_LIGHTNING,
   IMM_ACID,
   IMM_POISON,
   IMM_NEGATIVE,
   IMM_HOLY,
   IMM_ENERGY,
   IMM_MENTAL,
   IMM_DISEASE,
   IMM_DROWNING,
   IMM_LIGHT
};

int32 TRomMobDialog::anRomRes[18] = {
   RES_CHARM,
   RES_MAGIC,
   RES_WEAPON,
   RES_BASH,
   RES_PIERCE,
   RES_SLASH,
   RES_FIRE,
   RES_COLD,
   RES_LIGHTNING,
   RES_ACID,
   RES_POISON,
   RES_NEGATIVE,
   RES_HOLY,
   RES_ENERGY,
   RES_MENTAL,
   RES_DISEASE,
   RES_DROWNING,
   RES_LIGHT
};

int32 TRomMobDialog::anRomVuln[20] = {
   VULN_MAGIC,
   VULN_WEAPON,
   VULN_BASH,
   VULN_PIERCE,
   VULN_SLASH,
   VULN_FIRE,
   VULN_COLD,
   VULN_LIGHTNING,
   VULN_ACID,
   VULN_POISON,
   VULN_NEGATIVE,
   VULN_HOLY,
   VULN_ENERGY,
   VULN_MENTAL,
   VULN_DISEASE,
   VULN_DROWNING,
   VULN_LIGHT,
   VULN_WOOD,
   VULN_SILVER,
   VULN_IRON
};

int32 TRomMobDialog::anRomAct[11] = {
   ACT_UNDEAD,
   ACT_CLERIC,
   ACT_MAGE,
   ACT_THIEF,
   ACT_WARRIOR,
   ACT_NOALIGN,
   ACT_NOPURGE,
   ACT_IS_HEALER,
   ACT_GAIN,
   ACT_UPDATE_ALWAYS,
   ACT_IS_CHANGER
};

int32 TRomMobDialog::anRomAssist[6] = {
   ASSIST_ALL,
   ASSIST_ALIGN,
   ASSIST_RACE,
   ASSIST_PLAYERS,
   ASSIST_GUARD,
   ASSIST_VNUM
};

int32 TRomMobDialog::anRomOff[15] = {
   OFF_AREA_ATTACK,
   OFF_BACKSTAB,
   OFF_BASH,
   OFF_BERSERK,
   OFF_DISARM,
   OFF_DODGE,
   OFF_FADE,
   OFF_FAST,
   OFF_KICK,
   OFF_KICK_DIRT,
   OFF_PARRY,
   OFF_RESCUE,
   OFF_TAIL,
   OFF_TRIP,
   OFF_CRUSH
};

void TRomMobDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}

void TRomMobDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMMOB);
}

void TRomMobDialog::SaveTransfer()
{
    WorkMob.hitroll = atoi(TRomMobDialogData.HitRoll);
    StringToDice(TRomMobDialogData.HitDice,
      &WorkMob.hitnodice, &WorkMob.hitsizedice, &WorkMob.hitplus);
    StringToDice(TRomMobDialogData.DamDice,
      &WorkMob.damnodice, &WorkMob.damsizedice, &WorkMob.damplus);
    WorkMob.strDamType = TRomMobDialogData.DamType.GetSelection();
//    StringToDice(TRomMobDialogData.ManaDice,
//      &WorkMob.mana[DICE_NUMBER], &WorkMob.mana[DICE_TYPE], &WorkMob.mana[DICE_BONUS]);
    int32 nNo, nSz, nPl;
    StringToDice(TRomMobDialogData.ManaDice, &nNo, &nSz, &nPl);
    WorkMob.mana[DICE_NUMBER] = (sh_int) nNo;
    WorkMob.mana[DICE_TYPE] = (sh_int) nSz;
    WorkMob.mana[DICE_BONUS] = (sh_int) nPl;
    int i;
    for (i = 0; i < nelems(TRomMobDialogData.ArmorClass); i++)
      WorkMob.rom_ac[i] = (sh_int) atoi(TRomMobDialogData.ArmorClass[i]);

    WorkMob.start_pos =
      TAreaDocument::position_lookup(
         TRomMobDialogData.StartPos.GetSelection().c_str());
    WorkMob.default_pos =
      TAreaDocument::position_lookup(
         TRomMobDialogData.DefPos.GetSelection().c_str());

    WorkMob.size = (sh_int) TRomMobDialogData.Size.GetSelIndex();

    WorkMob.material = TRomMobDialogData.Material.GetSelection();

    WorkMob.gold = atol(TRomMobDialogData.Gold);
    WorkMob.mob_group = (vnum_int) atol(TRomMobDialogData.MobGroup);

    for (i = 0; i < nelems(anRomForm); i++)
        if (TRomMobDialogData.Form[i])
            WorkMob.form |= anRomForm[i];
        else
            WorkMob.form &= ~anRomForm[i];

    for (i = 0; i < nelems(anRomPart); i++)
        if (TRomMobDialogData.Part[i])
            WorkMob.parts |= anRomPart[i];
        else
            WorkMob.parts &= ~anRomPart[i];

    for (i = 0; i < nelems(anRomImm); i++)
        if (TRomMobDialogData.Imm[i])
            WorkMob.imm_flags |= anRomImm[i];
        else
            WorkMob.imm_flags &= ~anRomImm[i];

    for (i = 0; i < nelems(anRomRes); i++)
        if (TRomMobDialogData.Res[i])
            WorkMob.res_flags |= anRomRes[i];
        else
            WorkMob.res_flags &= ~anRomRes[i];

    for (i = 0; i < nelems(anRomVuln); i++)
        if (TRomMobDialogData.Vuln[i])
            WorkMob.vuln_flags |= anRomVuln[i];
        else
            WorkMob.vuln_flags &= ~anRomVuln[i];

    for (i = 0; i < nelems(anRomAct); i++)
        if (TRomMobDialogData.Act[i])
            WorkMob.act |= anRomAct[i];
        else
            WorkMob.act &= ~anRomAct[i];

    for (i = 0; i < nelems(anRomAssist); i++)
        if (TRomMobDialogData.Assist[i])
            WorkMob.off_flags |= anRomAssist[i];
        else
            WorkMob.off_flags &= ~anRomAssist[i];

    for (i = 0; i < nelems(anRomOff); i++)
        if (TRomMobDialogData.Off[i])
            WorkMob.off_flags |= anRomOff[i];
        else
            WorkMob.off_flags &= ~anRomOff[i];
}

void TRomMobDialog::StringToDice(const char *psz,
   int32 *pnNo, int32 *pnSz, int32 *pnPl)
{
   const char *pszSz = strchr(psz, 'd');
   if (!pszSz)
      return;
   pszSz++;
   const char *pszPl = strchr(pszSz, '+');
   if (!pszPl)
      return;
   pszPl++;

   *pnNo = atol(psz);
   *pnSz = atol(pszSz);
   *pnPl = atol(pszPl);
}
#endif

////////////////////////////////////////////////////////////////////////////
DEFINE_RESPONSE_TABLE1(TNewRomMobDialog, TDialog)
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;

static TNewRomMobDialogXfer TNewRomMobDialogData;

TNewRomMobDialog::TNewRomMobDialog(TWindow *parent, TMobile& TempMob, TModule *module):
    TDialog(parent, IDD_ROMMOBPROP, module), WorkMob( TempMob )
{
    HitRoll = new TEdit(this, IDC_HITROLL, sizeof TNewRomMobDialogData.HitRoll);
    HitDice = new TEdit(this, IDC_HITDICE, sizeof TNewRomMobDialogData.HitDice);
    DamDice = new TEdit(this, IDC_DAMDICE, sizeof TNewRomMobDialogData.DamDice);
    DamType = new TComboBox(this, IDC_DAMTYPE, 32);
    ManaDice = new TEdit(this, IDC_MANADICE, sizeof TNewRomMobDialogData.ManaDice);
    for (int i = 0; i < nelems(ArmorClass); i++)
       ArmorClass[i] = new TEdit(this, IDC_AC1+i, sizeof TNewRomMobDialogData.ArmorClass[0]);
    StartPos = new TComboBox(this, IDC_STARTPOS, 32);
    DefPos = new TComboBox(this, IDC_DEFPOS, 32);
    Size = new TComboBox(this, IDC_ROMSIZE, 32);
    Material = new TComboBox(this, IDC_MATERIAL, 32);
    Gold = new TEdit(this, IDC_GOLD, sizeof TNewRomMobDialogData.Gold);
    MobGroup = new TEdit(this, IDC_MOBGROUP, sizeof TNewRomMobDialogData.MobGroup);
    for (i = 0; i < nelems(anRomAct); i++)
        Act[i] = new TRadioButton(this, IDC_UNDEAD+i);
    for (i = 0; i < nelems(anRomAssist); i++)
        Assist[i] = new TRadioButton(this, IDC_ALL+i);
    for (i = 0; i < nelems(anRomOff); i++)
        Off[i] = new TRadioButton(this, IDC_AREAATTACK+i);

    SetTransferBuffer(&TNewRomMobDialogData);
    LoadTransfer();
}

TNewRomMobDialog::~TNewRomMobDialog ()
{
    Destroy();
}

void TNewRomMobDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}

void TNewRomMobDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMMOBPROP);
}

void TNewRomMobDialog::SaveTransfer()
{
    WorkMob.hitroll = atoi(TNewRomMobDialogData.HitRoll);
    StringToDice(TNewRomMobDialogData.HitDice,
      &WorkMob.hitnodice, &WorkMob.hitsizedice, &WorkMob.hitplus);
    StringToDice(TNewRomMobDialogData.DamDice,
      &WorkMob.damnodice, &WorkMob.damsizedice, &WorkMob.damplus);
    WorkMob.strDamType = TNewRomMobDialogData.DamType.GetSelection();
//    StringToDice(TRomMobDialogData.ManaDice,
//      &WorkMob.mana[DICE_NUMBER], &WorkMob.mana[DICE_TYPE], &WorkMob.mana[DICE_BONUS]);
    int32 nNo, nSz, nPl;
    StringToDice(TNewRomMobDialogData.ManaDice, &nNo, &nSz, &nPl);
    WorkMob.mana[DICE_NUMBER] = (sh_int) nNo;
    WorkMob.mana[DICE_TYPE] = (sh_int) nSz;
    WorkMob.mana[DICE_BONUS] = (sh_int) nPl;
    int i;
    for (i = 0; i < nelems(TNewRomMobDialogData.ArmorClass); i++)
      WorkMob.rom_ac[i] = (sh_int) atoi(TNewRomMobDialogData.ArmorClass[i]);

    WorkMob.start_pos =
      TAreaDocument::position_lookup(
         TNewRomMobDialogData.StartPos.GetSelection().c_str());
    WorkMob.default_pos =
      TAreaDocument::position_lookup(
         TNewRomMobDialogData.DefPos.GetSelection().c_str());

    WorkMob.size = (sh_int) TNewRomMobDialogData.Size.GetSelIndex();

    WorkMob.material = TNewRomMobDialogData.Material.GetSelection();

    WorkMob.gold = atol(TNewRomMobDialogData.Gold);
    WorkMob.mob_group = (vnum_int) atol(TNewRomMobDialogData.MobGroup);

    for (i = 0; i < nelems(anRomAct); i++)
        if (TNewRomMobDialogData.Act[i])
            WorkMob.act |= anRomAct[i];
        else
            WorkMob.act &= ~anRomAct[i];

    for (i = 0; i < nelems(anRomAssist); i++)
        if (TNewRomMobDialogData.Assist[i])
            WorkMob.off_flags |= anRomAssist[i];
        else
            WorkMob.off_flags &= ~anRomAssist[i];

    for (i = 0; i < nelems(anRomOff); i++)
        if (TNewRomMobDialogData.Off[i])
            WorkMob.off_flags |= anRomOff[i];
        else
            WorkMob.off_flags &= ~anRomOff[i];
}

const char * far TNewRomMobDialog::aszRomAttacks[] = {
    "none",
    "slice",
    "stab",
    "slash",
    "whip",
    "claw",
    "blast",
    "pound",
    "crush",
    "grep",
    "bite",
    "pierce",
    "suction",
    "beating",
    "digestion",
    "charge",
    "slap",
    "punch",
    "wrath",
    "magic",
    "divine",
    "cleave",
    "scratch",
    "peck",
    "peckb",
    "chop",
    "sting",
    "smash",
    "shbite",
    "flbite",
    "frbite",
    "acbite",
    "chomp",
    "drain",
    "thrust",
    "slime",
    "shock",
    "thwack",
    "flame",
    "chill"
};

int32 TNewRomMobDialog::anRomAct[11] = {
   ACT_UNDEAD,
   ACT_CLERIC,
   ACT_MAGE,
   ACT_THIEF,
   ACT_WARRIOR,
   ACT_NOALIGN,
   ACT_NOPURGE,
   ACT_IS_HEALER,
   ACT_GAIN,
   ACT_UPDATE_ALWAYS,
   ACT_IS_CHANGER
};

int32 TNewRomMobDialog::anRomAssist[6] = {
   ASSIST_ALL,
   ASSIST_ALIGN,
   ASSIST_RACE,
   ASSIST_PLAYERS,
   ASSIST_GUARD,
   ASSIST_VNUM
};

int32 TNewRomMobDialog::anRomOff[15] = {
   OFF_AREA_ATTACK,
   OFF_BACKSTAB,
   OFF_BASH,
   OFF_BERSERK,
   OFF_DISARM,
   OFF_DODGE,
   OFF_FADE,
   OFF_FAST,
   OFF_KICK,
   OFF_KICK_DIRT,
   OFF_PARRY,
   OFF_RESCUE,
   OFF_TAIL,
   OFF_TRIP,
   OFF_CRUSH
};

char * far TNewRomMobDialog::aszRomMaterials[] = {
   // 'oldstyle' isn't a material, it is what a lot of the objects in
   // stock ROM are made of, because no one has bothered to set them.
   "oldstyle",    // 1

   // Here are the real objects from the ROM builder's guide.  Rom,
   // however, doesn't use these for anything yet.
   "plastic",
   "slime",
   "jelly",
   "wax",         // 5
   "rubber",
   "oil",
   "balm",
   "cream",
   "hard leather",   // 10
   "soft leather",
   "kid leather",
   "fur",
   "snakeskin",
   "gut",            // 15
   "food",
   "meat",
   "bread",
   "wood",
   "hardwood",       // 20
   "softwood",
   "bamboo",
   "ebony",
   "cork",
   "horn",           // 25
   "light laen",
   "sponge",
   "elastic",
   "silk",
   "satin",          // 30
   "lace",
   "wool",
   "linen",
   "canvas",
   "cloth",          // 35
   "velvet",
   "felt",
   "paper",
   "parchment",
   "vellum",         // 40
   "hemp",
   "feathers",
   "webbing",
   "cardboard",
   "steel",          // 45
   "gold",
   "silver",
   "mithril",
   "platinum",
   "iron",           // 50
   "lead",
   "copper",
   "electrum",
   "bronze",
   "brass",          // 55
   "wire",
   "tin",
   "pewter",
   "metal",
   "dark laen",      // 60
   "porcelain",
   "ivory",
   "marble",
   "stone",
   "quartz",         // 65
   "corundum",
   "flint",
   "lodestone",
   "granite",
   "enamel",         // 70
   "obsidian",
   "adamantite",
   "glass",
   "pottery",
   "crystal",        // 75
   "ice",
   "bone",
   "shell",
   "coral",
   "energy",         // 80
   "fire",
   "air",
   "water",
   "acid",
   "coal",           // 85
   "sandstone",
   "clay",
   "ash",
   "earth",
   "diamond",        // 90
   "etherealness",
   "nothingness",
   "dragonscale",
   "blue dragonscale",
   "black dragonscale", // 95
   "white dragonscale",
   "red dragonscale"    // 97
};

void TNewRomMobDialog::LoadTransfer()
{
    wsprintf(TNewRomMobDialogData.HitRoll, "%d", WorkMob.hitroll);
    wsprintf(TNewRomMobDialogData.HitDice, "%dd%d+%d", WorkMob.hitnodice, WorkMob.hitsizedice, WorkMob.hitplus);
    wsprintf(TNewRomMobDialogData.DamDice, "%dd%d+%d", WorkMob.damnodice, WorkMob.damsizedice, WorkMob.damplus);
    TNewRomMobDialogData.DamType.Clear();
    int i;
    for (i = 0; i < nelems(aszRomAttacks); i++)
      TNewRomMobDialogData.DamType.AddStringItem(aszRomAttacks[i], i, WorkMob.strDamType == aszRomAttacks[i]);
    wsprintf(TNewRomMobDialogData.ManaDice, "%dd%d+%d", WorkMob.mana[DICE_NUMBER], WorkMob.mana[DICE_TYPE], WorkMob.mana[DICE_BONUS]);
    for (i = 0; i < nelems(TNewRomMobDialogData.ArmorClass); i++)
       wsprintf(TNewRomMobDialogData.ArmorClass[i], "%d", WorkMob.rom_ac[i]);

    WorkMob.AreaDoc->LoadPositionComboBox(&TNewRomMobDialogData.StartPos,
      WorkMob.start_pos);
    WorkMob.AreaDoc->LoadPositionComboBox(&TNewRomMobDialogData.DefPos,
      WorkMob.default_pos);

    // @@@ Move to document
    static const char *aszRomSizes[] = { "tiny", "small", "medium", "large", "huge", "giant" };
    TNewRomMobDialogData.Size.Clear();
    for (i = 0; i < nelems(aszRomSizes); i++) {
        TNewRomMobDialogData.Size.AddStringItem(aszRomSizes[i], i, WorkMob.size == i);
    }

//    TRomMobDialogData.Material.Clear();
//    TRomMobDialogData.Material.AddStringItem(WorkMob.material.c_str(), 0, TRUE);
    TNewRomMobDialogData.Material.Clear();
    BOOL fFoundMaterial = FALSE;
    for (i = 0; i<nelems(aszRomMaterials); i++) {
       BOOL fMaterial = (WorkMob.material==aszRomMaterials[i]);
       TNewRomMobDialogData.Material.AddStringItem(aszRomMaterials[i], 0,
         fMaterial);
       fFoundMaterial = (fFoundMaterial || fMaterial);
    }
    // If the material wasn't in our list from the builder's guide, add the
    // current material to the list.
    if (!fFoundMaterial) {
       TNewRomMobDialogData.Material.AddStringItem(WorkMob.material.c_str(), 0,
         TRUE);
    }


    wsprintf(TNewRomMobDialogData.Gold, "%d", WorkMob.gold);
    wsprintf(TNewRomMobDialogData.MobGroup, "%d", WorkMob.mob_group);

    for (i = 0; i < nelems(anRomAct); i++)
      TNewRomMobDialogData.Act[i]    = ((WorkMob.act & anRomAct[i]) == anRomAct[i]);
    for (i = 0; i < nelems(anRomAssist); i++)
      TNewRomMobDialogData.Assist[i] = ((WorkMob.off_flags & anRomAssist[i]) == anRomAssist[i]);
    for (i = 0; i < nelems(anRomOff); i++)
      TNewRomMobDialogData.Off[i]    = ((WorkMob.off_flags & anRomOff[i]) == anRomOff[i]);
}

void TNewRomMobDialog::StringToDice(const char *psz,
   int32 *pnNo, int32 *pnSz, int32 *pnPl)
{
   const char *pszSz = strchr(psz, 'd');
   if (!pszSz)
      return;
   pszSz++;
   const char *pszPl = strchr(pszSz, '+');
   if (!pszPl)
      return;
   pszPl++;

   *pnNo = atol(psz);
   *pnSz = atol(pszSz);
   *pnPl = atol(pszPl);
}

////////////////////////////////////////////////////////////////////////////
DEFINE_RESPONSE_TABLE1(TMobBitsDialog, TDialog)
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;

static TMobBitsDialogXfer TMobBitsDialogData;

TMobBitsDialog::TMobBitsDialog(TWindow *parent, TMobile& TempMob, TModule *module):
    TDialog(parent, IDD_ROMMOBBITS, module), WorkMob( TempMob )
{
    int i;
    for (i = 0; i < nelems(anRomImm); i++)
        Imm[i] = new TRadioButton(this, IDC_IMMA+i);
    for (i = 0; i < nelems(anRomRes); i++)
        Res[i] = new TRadioButton(this, IDC_RESB+i);
    for (i = 0; i < nelems(anRomVuln); i++)
        Vuln[i] = new TRadioButton(this, IDC_VULNC+i);

    SetTransferBuffer(&TMobBitsDialogData);
    LoadTransfer();
}

TMobBitsDialog::~TMobBitsDialog ()
{
    Destroy();
}

void TMobBitsDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}

void TMobBitsDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMMOBBITS);
}

void TMobBitsDialog::SaveTransfer()
{
    int i;

    for (i = 0; i < nelems(anRomImm); i++)
        if (TMobBitsDialogData.Imm[i])
            WorkMob.imm_flags |= anRomImm[i];
        else
            WorkMob.imm_flags &= ~anRomImm[i];

    for (i = 0; i < nelems(anRomRes); i++)
        if (TMobBitsDialogData.Res[i])
            WorkMob.res_flags |= anRomRes[i];
        else
            WorkMob.res_flags &= ~anRomRes[i];

    for (i = 0; i < nelems(anRomVuln); i++)
        if (TMobBitsDialogData.Vuln[i])
            WorkMob.vuln_flags |= anRomVuln[i];
        else
            WorkMob.vuln_flags &= ~anRomVuln[i];
}

void TMobBitsDialog::LoadTransfer()
{
    int i;
    
    for (i = 0; i < nelems(anRomImm); i++)
      TMobBitsDialogData.Imm[i] = ((WorkMob.imm_flags & anRomImm[i]) == anRomImm[i]);

    for (i = 0; i < nelems(anRomRes); i++)
      TMobBitsDialogData.Res[i] = ((WorkMob.res_flags & anRomRes[i]) == anRomRes[i]);

    for (i = 0; i < nelems(anRomVuln); i++)
      TMobBitsDialogData.Vuln[i] = ((WorkMob.vuln_flags & anRomVuln[i]) == anRomVuln[i]);
}

int32 TMobBitsDialog::anRomImm[19] = {
   IMM_SUMMON,
   IMM_CHARM,
   IMM_MAGIC,
   IMM_WEAPON,
   IMM_BASH,
   IMM_PIERCE,
   IMM_SLASH,
   IMM_FIRE,
   IMM_COLD,
   IMM_LIGHTNING,
   IMM_ACID,
   IMM_POISON,
   IMM_NEGATIVE,
   IMM_HOLY,
   IMM_ENERGY,
   IMM_MENTAL,
   IMM_DISEASE,
   IMM_DROWNING,
   IMM_LIGHT
};

int32 TMobBitsDialog::anRomRes[18] = {
   RES_CHARM,
   RES_MAGIC,
   RES_WEAPON,
   RES_BASH,
   RES_PIERCE,
   RES_SLASH,
   RES_FIRE,
   RES_COLD,
   RES_LIGHTNING,
   RES_ACID,
   RES_POISON,
   RES_NEGATIVE,
   RES_HOLY,
   RES_ENERGY,
   RES_MENTAL,
   RES_DISEASE,
   RES_DROWNING,
   RES_LIGHT
};

int32 TMobBitsDialog::anRomVuln[20] = {
   VULN_MAGIC,
   VULN_WEAPON,
   VULN_BASH,
   VULN_PIERCE,
   VULN_SLASH,
   VULN_FIRE,
   VULN_COLD,
   VULN_LIGHTNING,
   VULN_ACID,
   VULN_POISON,
   VULN_NEGATIVE,
   VULN_HOLY,
   VULN_ENERGY,
   VULN_MENTAL,
   VULN_DISEASE,
   VULN_DROWNING,
   VULN_LIGHT,
   VULN_WOOD,
   VULN_SILVER,
   VULN_IRON
};

////////////////////////////////////////////////////////////////////////////
DEFINE_RESPONSE_TABLE1(TMobPartsDialog, TDialog)
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;

static TMobPartsDialogXfer TMobPartsDialogData;

TMobPartsDialog::TMobPartsDialog(TWindow *parent, TMobile& TempMob, TModule *module):
    TDialog(parent, IDD_ROMMOBPARTS, module), WorkMob( TempMob )
{
    int i;
    for (i = 0; i < nelems(anRomForm); i++)
        Form[i] = new TRadioButton(this, IDC_FORMA+i);
    for (i = 0; i < nelems(anRomPart); i++)
        Part[i] = new TRadioButton(this, IDC_PARTA+i);

    SetTransferBuffer(&TMobPartsDialogData);
    LoadTransfer();
}

TMobPartsDialog::~TMobPartsDialog ()
{
    Destroy();
}

void TMobPartsDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}

void TMobPartsDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMMOBPARTS);
}

void TMobPartsDialog::SaveTransfer()
{
    int i;

    for (i = 0; i < nelems(anRomForm); i++)
        if (TMobPartsDialogData.Form[i])
            WorkMob.form |= anRomForm[i];
        else
            WorkMob.form &= ~anRomForm[i];

    for (i = 0; i < nelems(anRomPart); i++)
        if (TMobPartsDialogData.Part[i])
            WorkMob.parts |= anRomPart[i];
        else
            WorkMob.parts &= ~anRomPart[i];
}

void TMobPartsDialog::LoadTransfer()
{
    int i;

    for (i = 0; i < nelems(anRomForm); i++)
      TMobPartsDialogData.Form[i] = ((WorkMob.form & anRomForm[i]) == anRomForm[i]);

    for (i = 0; i < nelems(anRomPart); i++)
      TMobPartsDialogData.Part[i] = ((WorkMob.parts & anRomPart[i]) == anRomPart[i]);
}

int32 TMobPartsDialog::anRomForm[26] = {
   FORM_EDIBLE,
   FORM_POISON,
   FORM_MAGICAL,
   FORM_INSTANT_DECAY,
   FORM_OTHER,
   FORM_ANIMAL,
   FORM_SENTIENT,
   FORM_UNDEAD,
   FORM_CONSTRUCT,
   FORM_MIST,
   FORM_INTANGIBLE,
   FORM_BIPED,
   FORM_CENTAUR,
   FORM_INSECT,
   FORM_SPIDER,
   FORM_FISH,
   FORM_WORM,
   FORM_BLOB,
   FORM_MAMMAL,
   FORM_BIRD,
   FORM_REPTILE,
   FORM_SNAKE,
   FORM_DRAGON,
   FORM_AMPHIBIAN,
   FORM_CRUSTACEAN,
   FORM_COLD_BLOOD,
};

int32 TMobPartsDialog::anRomPart[22] = {
   PART_HEAD,
   PART_ARMS,
   PART_LEGS,
   PART_HEART,
   PART_BRAINS,
   PART_GUTS,
   PART_HANDS,
   PART_FEET,
   PART_FINGERS,
   PART_EAR,
   PART_TENTACLES,
   PART_LONG_TONGUE,
   PART_EYESTALKS,
   PART_EYE,
   PART_FINS,
   PART_WINGS,
   PART_TAIL,
   PART_CLAWS,
   PART_FANGS,
   PART_HORNS,
   PART_SCALES,
   PART_TUSKS
};

