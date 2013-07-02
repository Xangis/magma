/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia1.cpp (1 of 2)
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TObjDialog (TDialog) and related.
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

// Classlib includes
#include <classlib\assoc.h>
#include <classlib\dict.h>
#include <classlib\sets.h>

// OWL includes
#include <owl\inputdia.h>

// C includes
#include <malloc.h>

// Local includes
#include "objdia.h"
#include "xtradia.h"
#include "appdia.h"
#include "mobdia.h"
#include "tobjlib.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TObjDialog, TDialog)
//{{TObjDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDC_PREV, CmPrev),
    EV_BN_CLICKED(IDC_NEXT, CmNext),
    EV_BN_CLICKED(IDC_LIGHT, CmTypeChanged),
    EV_BN_CLICKED(IDC_SCROLL, CmTypeChanged),
    EV_BN_CLICKED(IDC_WAND, CmTypeChanged),
    EV_BN_CLICKED(IDC_STAFF, CmTypeChanged),
    EV_BN_CLICKED(IDC_WEAPON, CmTypeChanged),
    EV_BN_CLICKED(IDC_TREASURE, CmTypeChanged),
    EV_BN_CLICKED(IDC_POTION, CmTypeChanged),
    EV_BN_CLICKED(IDC_FURNITURE, CmTypeChanged),
    EV_BN_CLICKED(IDC_TRASH, CmTypeChanged),
    EV_BN_CLICKED(IDC_CONTAINER, CmTypeChanged),
    EV_BN_CLICKED(IDC_DRINK, CmTypeChanged),
    EV_BN_CLICKED(IDC_KEY, CmTypeChanged),
    EV_BN_CLICKED(IDC_FOOD, CmTypeChanged),
    EV_BN_CLICKED(IDC_GOLD, CmTypeChanged),
    EV_BN_CLICKED(IDC_NPC_CORPSE, CmTypeChanged),
    EV_BN_CLICKED(IDC_FOUNTAIN, CmTypeChanged),
    EV_BN_CLICKED(IDC_PILL, CmTypeChanged),
    EV_BN_CLICKED(IDC_BOAT, CmTypeChanged),
    EV_BN_CLICKED(IDC_TAKE, CmTakeChanged),
    EV_BN_CLICKED(IDC_EDITEXTRA, CmEditExtra),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_LBN_SELCHANGE(IDC_APPLIES_LIST, CmApplySelected),
    EV_BN_CLICKED(IDC_ADDEXTRA, CmAddExtra),
    EV_BN_CLICKED(IDC_DELEXTRA, CmDelExtra),
    EV_BN_CLICKED(IDC_ADDAPPLY, CmAddApply),
    EV_BN_CLICKED(IDC_DELETEAPPLY, CmDelApply),
    EV_LBN_DBLCLK(IDC_EXTRADESCR, CmEditExtra),
    EV_BN_CLICKED(IDC_ARMOR, CmTypeChanged),
    EV_BN_CLICKED(IDC_FINGER, CmWieldChanged),
    EV_BN_CLICKED(IDC_NECK, CmWieldChanged),
    EV_BN_CLICKED(IDC_BODY, CmWieldChanged),
    EV_BN_CLICKED(IDC_HEAD, CmWieldChanged),
    EV_BN_CLICKED(IDC_LEGS, CmWieldChanged),
    EV_BN_CLICKED(IDC_FEET, CmWieldChanged),
    EV_BN_CLICKED(IDC_HANDS, CmWieldChanged),
    EV_BN_CLICKED(IDC_ARMS, CmWieldChanged),
    EV_BN_CLICKED(IDC_SHIELD, CmWieldChanged),
    EV_BN_CLICKED(IDC_ABOUTBODY, CmWieldChanged),
    EV_BN_CLICKED(IDC_WAIST, CmWieldChanged),
    EV_BN_CLICKED(IDC_WRIST, CmWieldChanged),
    EV_BN_CLICKED(IDC_WIELD, CmWieldChanged),
    EV_BN_CLICKED(IDC_HOLD, CmWieldChanged),
    EV_BN_CLICKED(IDC_WITHPRIDE, CmWieldChanged),
    EV_BN_CLICKED(IDHELP, CmHelp),
//    EV_BN_CLICKED(IDC_TYPECUSTOM1, CmTypeChanged),
//    EV_BN_CLICKED(IDC_TYPECUSTOM2, CmTypeChanged),
//    EV_BN_CLICKED(IDC_TYPECUSTOM3, CmTypeChanged),
    EV_BN_CLICKED(IDC_OTHER, CmTypeChanged),
    EV_BN_CLICKED(IDC_CODE, CmRomProps),
    EV_BN_CLICKED(IDC_CUSTOMIZE, CmCustomize),
    EV_BN_CLICKED(IDC_ADD1, CmAddFlag1),
    EV_BN_CLICKED(IDC_ADD2, CmAddFlag2),
    EV_BN_CLICKED(IDC_ADD3, CmAddFlag3),
    EV_BN_CLICKED(IDC_ADD4, CmAddFlag4),
    EV_BN_CLICKED(IDC_ADD5, CmAddFlag5),
//    EV_BN_CLICKED(IDC_REM1, CmRemFlag1),
//    EV_BN_CLICKED(IDC_REM2, CmRemFlag2),
//    EV_BN_CLICKED(IDC_REM3, CmRemFlag3),
//    EV_BN_CLICKED(IDC_REM4, CmRemFlag4),
//    EV_BN_CLICKED(IDC_REM5, CmRemFlag5),
    EV_EN_KILLFOCUS(IDC_OTHERTEXT, CmOtherTextKillFocus),
//{{TObjDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TObjDialog Implementation}}


static TObjDialogXfer TObjDialogData;

TObjDialog::TObjDialog (TWindow* parent, const TObject *obj, int nNdx,
        TLibrary* pLibrary, TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkObj( *obj )
{
//{{TObjDialogXFER_USE}}
    NameList = new TEdit(this, IDC_NAMELIST, 255);
    ShortDescr = new TEdit(this, IDC_SHORTDESCR, 255);
    LongDescr = new TEdit(this, IDC_LONGDESCR, 2048);
    Light = new TRadioButton(this, IDC_LIGHT, 0);
    Scroll = new TRadioButton(this, IDC_SCROLL, 0);
    Wand = new TRadioButton(this, IDC_WAND, 0);
    Staff = new TRadioButton(this, IDC_STAFF, 0);
    Weapon = new TRadioButton(this, IDC_WEAPON, 0);
    Armor = new TRadioButton(this, IDC_ARMOR, 0);
    Potion = new TRadioButton(this, IDC_POTION, 0);
    Furniture = new TRadioButton(this, IDC_FURNITURE, 0);
    Trash = new TRadioButton(this, IDC_TRASH, 0);
    Container = new TRadioButton(this, IDC_CONTAINER, 0);
    DrinkCon = new TRadioButton(this, IDC_DRINK, 0);
    Key = new TRadioButton(this, IDC_KEY, 0);
    Food = new TRadioButton(this, IDC_FOOD, 0);
    Gold = new TRadioButton(this, IDC_GOLD, 0);
    Boat = new TRadioButton(this, IDC_BOAT, 0);
    NPCCorpse = new TRadioButton(this, IDC_NPC_CORPSE, 0);
    Fountain = new TRadioButton(this, IDC_FOUNTAIN, 0);
    Pill = new TRadioButton(this, IDC_PILL, 0);
    ObjType = new TStatic(this, IDC_OBJTYPE, 255);
    Take = new TCheckBox(this, IDC_TAKE, 0);
    Finger = new TCheckBox(this, IDC_FINGER, 0);
    Neck = new TCheckBox(this, IDC_NECK, 0);
    Body = new TCheckBox(this, IDC_BODY, 0);
    Head = new TCheckBox(this, IDC_HEAD, 0);
    Legs = new TCheckBox(this, IDC_LEGS, 0);
    Feet = new TCheckBox(this, IDC_FEET, 0);
    Hands = new TCheckBox(this, IDC_HANDS, 0);
    Arms = new TCheckBox(this, IDC_ARMS, 0);
    Shield = new TCheckBox(this, IDC_SHIELD, 0);
    AboutBody = new TCheckBox(this, IDC_ABOUTBODY, 0);
    Waist = new TCheckBox(this, IDC_WAIST, 0);
    Wrist = new TCheckBox(this, IDC_WRIST, 0);
    Hold = new TCheckBox(this, IDC_HOLD, 0);
    WithPride = new TCheckBox(this, IDC_WITHPRIDE, 0);
    Glow = new TCheckBox(this, IDC_GLOW, 0);
    Hum = new TCheckBox(this, IDC_HUM, 0);
    Evil = new TCheckBox(this, IDC_EVIL, 0);
    Magic = new TCheckBox(this, IDC_MAGIC, 0);
    NoDrop = new TCheckBox(this, IDC_NODROP, 0);
    AntiGood = new TCheckBox(this, IDC_ANTIGOOD, 0);
    AntiEvil = new TCheckBox(this, IDC_ANTIEVIL, 0);
    AntiNeutral = new TCheckBox(this, IDC_ANTINEUTRAL, 0);
    NoRemove = new TCheckBox(this, IDC_NOREMOVE, 0);
    Invis = new TCheckBox(this, IDC_INVIS, 0);
    Metallic = new TCheckBox(this, IDC_METALLIC, 0);
    VNum = new TStatic(this, IDC_VNUM, 255);
    Wield = new TCheckBox(this, IDC_WIELD, 0);
    Weight = new TEdit(this, IDC_WEIGHT, 255);
    Treasure = new TRadioButton(this, IDC_TREASURE, 0);

    for (int i = 0; i < nelems(Prompt); i++)
       {
       Prompt[i] = new TStatic(this, IDC_PROMPT1 + i, 32);
       Value[i] = new TComboBox(this, IDC_VALUE1 + i, 32);
       }
    AppliesList = new TListBox(this, IDC_APPLIES_LIST);
    ApplyAmount = new TEdit(this, IDC_APPLY_AMOUNT, 255);
    ExtraList = new TListBox(this, IDC_EXTRADESCR);
    WieldType = new TStatic(this, IDC_WIELDTYPE, 255);
    Limit = new TStatic(this, IDC_LIMIT, 255);
    Custom1 = new TRadioButton(this, IDC_TYPECUSTOM1, 0);
    Custom2 = new TRadioButton(this, IDC_TYPECUSTOM2, 0);
    Custom3 = new TRadioButton(this, IDC_TYPECUSTOM3, 0);
    Other = new TRadioButton(this, IDC_OTHER, 0);
    OtherText = new TEdit(this, IDC_OTHERTEXT, 255);

    // These constructors don't have any data, their order doesn't matter.
    for (i = 0; i < nelems(m_AddFlag); i++) {
       m_AddFlag[i] = new TButton(this, IDC_ADD1+i);
//       m_RemFlag[i] = new TButton(this, IDC_REM1+i);
    }
    Code = new TRadioButton(this, IDC_CODE);

    SetTransferBuffer(&TObjDialogData);
//{{TObjDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    TheActualObject = obj;
    nIndex = nNdx;
    m_mode = WorkObj.AreaDoc->GetMode();
//    m_Library = new TObjTypeLibrary(poundsApp::szModesShort[m_mode],
//      m_mode);
    m_Library = pLibrary;

    VNum->EnableTransfer();
    Limit->EnableTransfer();
    LoadTransfer();

    static int nIconIDs[] = {
       // Unknown type
       IDI_TOMANYWIELDS,
//       IDI_OBJECTS,                                   // Custom unnumbered
       IDI_LIGHT, IDI_SCROLL, IDI_WAND, IDI_STAFF,
       IDI_WEAPON, IDI_OBJECTS, IDI_OBJECTS, IDI_TREASURE, IDI_ARMOR,
       IDI_POTION, IDI_OBJECTS, IDI_FURNITURE, IDI_TRASH, IDI_OBJECTS,
       IDI_CONTAINER,
       IDI_OBJECTS, IDI_DRINK_CON, IDI_KEY,
       IDI_FOOD, IDI_GOLD, IDI_OBJECTS, IDI_BOAT,
       IDI_NPC_CORPSE, IDI_OBJECTS /* @@@ sb pc corpse*/,
       IDI_FOUNTAIN, IDI_PILL,

       // Custom type
       IDI_OBJECTS

       };
//       // Custom types
//       IDI_OBJECTS, IDI_OBJECTS, IDI_OBJECTS };

    for (i = 0; i < nelems(hObjTypeIcon); i++)
       hObjTypeIcon[i] = GetApplication()->LoadIcon(MAKEINTRESOURCE(nIconIDs[i]));

    static int nLookupArmorIcon[] = { IDI_NOTAKE, IDI_TOMANYWIELDS,
       IDI_TAKE, IDI_FINGER, IDI_NECK, IDI_BODY,
       IDI_HEAD, IDI_LEGS, IDI_FEET, IDI_HANDS, IDI_ARMS, IDI_SHIELD,
       IDI_ABOUTBODY, IDI_WAIST, IDI_WRIST, IDI_WIELD, IDI_HOLD, IDI_PRIDE };

    for (i = 0; i < nelems(hWearIcon); i++)
       hWearIcon[i] = GetApplication()->LoadIcon(MAKEINTRESOURCE(
          nLookupArmorIcon[i]));
}


TObjDialog::~TObjDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    for (int i = 0; i < nelems(hObjTypeIcon); i++)
       DestroyIcon(hObjTypeIcon[i]);
    for (i = 0; i < nelems(hWearIcon); i++)
       DestroyIcon(hWearIcon[i]);

//    m_Library->Save();
//    delete m_Library;
}

void TObjDialog::CmPrev ()
{
    // INSERT>> Your code here.
    const TObject *o;
    if ((o = WorkObj.AreaDoc->GetObjectFromIndex(nIndex-1)) != 0)
       {
   	 TransferData(tdGetData);

       // Try to save the values.  If they cannot be saved, the user wants
       // a second chance editing
       if (!SaveTransfer())
         return;

       nIndex--;
       LoadAnotherObject(o);
       return;
       }

    MessageBeep(-1);
}


void TObjDialog::CmNext ()
{
    // INSERT>> Your code here.
    const TObject *o;
    if ((o = WorkObj.AreaDoc->GetObjectFromIndex(nIndex+1)) != 0)
       {
   	 TransferData(tdGetData);

       // Try to save the values.  If they cannot be saved, the user wants
       // a second chance editing
       if (!SaveTransfer())
         return;

       nIndex++;
       LoadAnotherObject(o);
       return;
       }

    MessageBeep(-1);
}

const char *TObjDialog::GetAffectName(int n)
{
   static char * FAR szNames[] = {
      "None", "Strength", "Dexterity", "Intelligence", "Wisdom", "Constitution",
      "Sex (*)", "Class (*)", "Level (*)", "Age", "Height (*)", "Weight (*)",
      "Mana", "Hit points", "Moves", "Gold (*)", "Experience (*)",
      "Armor Class", "Hit roll", "Damage Roll", "Save vs Paralysis (*)",
      "Save vs Rod (*)", "Save vs Petrification (*)", "Save vs Breath",
      "Save vs Spell",
      // TFC
      "Charisma (FC)", "Luck (FC)", "Anti-Theft (FC)", "No SLEEP (FC)",
      "Blocking (FC)", "No CHARM (FC)", "Familiarity (FC)", "True Seeing (FC)",
      "Obscurement (FC)",
      // Unused
      "Unknown34", "Unknown35", "Unknown36", "Unknown37", "Unknown38",
      "Unknown39",
      // Troll-only
      "Alignment (T)"
   };

   if (n < nelems(szNames))
      return szNames[n];
   return "Error";
}

void TObjDialog::LoadTransfer()
{
    strcpy(TObjDialogData.NameList, WorkObj.name.c_str());
    strcpy(TObjDialogData.ShortDescr, WorkObj.short_descr.c_str());
    strcpy(TObjDialogData.LongDescr, WorkObj.description.c_str());

    // Item type
    TObjDialogData.Light = (WorkObj.item_type == ITEM_LIGHT);
    TObjDialogData.Scroll = (WorkObj.item_type == ITEM_SCROLL);
    TObjDialogData.Wand = (WorkObj.item_type == ITEM_WAND);
    TObjDialogData.Staff = (WorkObj.item_type == ITEM_STAFF);
    TObjDialogData.Weapon = (WorkObj.item_type == ITEM_WEAPON);
    TObjDialogData.Treasure = (WorkObj.item_type == ITEM_TREASURE);
    TObjDialogData.Armor = (WorkObj.item_type == ITEM_ARMOR);
    TObjDialogData.Potion = (WorkObj.item_type == ITEM_POTION);
    TObjDialogData.Furniture = (WorkObj.item_type == ITEM_FURNITURE);
    TObjDialogData.Trash = (WorkObj.item_type == ITEM_TRASH);
    TObjDialogData.Container = (WorkObj.item_type == ITEM_CONTAINER);
    TObjDialogData.DrinkCon = (WorkObj.item_type == ITEM_DRINK_CON);
    TObjDialogData.Key = (WorkObj.item_type == ITEM_KEY);
    TObjDialogData.Food = (WorkObj.item_type == ITEM_FOOD);
    TObjDialogData.Gold = (WorkObj.item_type == ITEM_MONEY);
    TObjDialogData.Boat = (WorkObj.item_type == ITEM_BOAT);
    TObjDialogData.NPCCorpse = (WorkObj.item_type == ITEM_CORPSE_NPC);
    TObjDialogData.Fountain = (WorkObj.item_type == ITEM_FOUNTAIN);
    TObjDialogData.Pill = (WorkObj.item_type == ITEM_PILL);
    TObjDialogData.Other = (WorkObj.item_type > ITEM_PILL || WorkObj.item_type == 0);
    strcpy(TObjDialogData.OtherText, WorkObj.strType.c_str());

    // Wear position
    TObjDialogData.Take = IS_SET(WorkObj.wear_flags, ITEM_TAKE);
    TObjDialogData.Finger =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_FINGER);
    TObjDialogData.Neck =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_NECK);
    TObjDialogData.Body =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_BODY);
    TObjDialogData.Head =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_HEAD);
    TObjDialogData.Legs =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_LEGS);
    TObjDialogData.Feet =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_FEET);
    TObjDialogData.Hands =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_HANDS);
    TObjDialogData.Arms =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_ARMS);
    TObjDialogData.Shield =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_SHIELD);
    TObjDialogData.AboutBody =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_ABOUT);
    TObjDialogData.Waist =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_WAIST);
    TObjDialogData.Wrist =  IS_SET(WorkObj.wear_flags, ITEM_WEAR_WRIST);
    TObjDialogData.Wield =  IS_SET(WorkObj.wear_flags, ITEM_WIELD);
    TObjDialogData.Hold =  IS_SET(WorkObj.wear_flags, ITEM_HOLD);
    TObjDialogData.WithPride =  IS_SET(WorkObj.wear_flags, ITEM_WORN_PRIDE);

    // Extra flags
    TObjDialogData.Glow = IS_SET(WorkObj.extra_flags, ITEM_GLOW);
    TObjDialogData.Hum = IS_SET(WorkObj.extra_flags, ITEM_HUM);
    TObjDialogData.Evil = IS_SET(WorkObj.extra_flags, ITEM_EVIL);
    TObjDialogData.Magic = IS_SET(WorkObj.extra_flags, ITEM_MAGIC);
    TObjDialogData.NoDrop = IS_SET(WorkObj.extra_flags, ITEM_NODROP);
    TObjDialogData.AntiGood = IS_SET(WorkObj.extra_flags, ITEM_ANTI_GOOD);
    TObjDialogData.AntiEvil = IS_SET(WorkObj.extra_flags, ITEM_ANTI_EVIL);
    TObjDialogData.AntiNeutral = IS_SET(WorkObj.extra_flags, ITEM_ANTI_NEUTRAL);
    TObjDialogData.NoRemove = IS_SET(WorkObj.extra_flags, ITEM_NOREMOVE);
    TObjDialogData.Invis = IS_SET(WorkObj.extra_flags, ITEM_INVIS);
    TObjDialogData.Metallic = IS_SET(WorkObj.extra_flags, ITEM_METALLIC);

    wsprintf(TObjDialogData.VNum, "%d", WorkObj.vnum);
    wsprintf(TObjDialogData.Limit, "%ld", WorkObj.count);		// @@@ TFC only
    wsprintf(TObjDialogData.Weight, "%ld", WorkObj.weight);

    TObjDialogData.AppliesList.Clear();
    BOOL fFirst = TRUE;
    for (TArrayAsVectorIterator<TAffect *> it(WorkObj.affected); it; it++)
       {
       TObjDialogData.AppliesList.AddString(
          GetAffectName(it.Current()->location), fFirst);
       fFirst = FALSE;
       }

    TObjDialogData.ExtraList.Clear();
    for (TArrayAsVectorIterator<TExtraDescr *> it2(WorkObj.extra_descr); it2; it2++)
       TObjDialogData.ExtraList.AddString(it2.Current()->keyword.c_str());

    m_strType = WorkObj.strType;
}

BOOL TObjDialog::SaveTransfer()
{
    if (!SaveValues())
      return FALSE;

    WorkObj.name = TObjDialogData.NameList;
    WorkObj.short_descr = TObjDialogData.ShortDescr;
    WorkObj.description = TObjDialogData.LongDescr;

    // Item type
    if (TObjDialogData.Light)          WorkObj.item_type = ITEM_LIGHT;
    else if (TObjDialogData.Scroll)    WorkObj.item_type = ITEM_SCROLL;
    else if (TObjDialogData.Wand) 		WorkObj.item_type = ITEM_WAND;
    else if (TObjDialogData.Staff) 		WorkObj.item_type = ITEM_STAFF;
    else if (TObjDialogData.Weapon) 	WorkObj.item_type = ITEM_WEAPON;
    else if (TObjDialogData.Treasure) 	WorkObj.item_type = ITEM_TREASURE;
    else if (TObjDialogData.Armor) 		WorkObj.item_type = ITEM_ARMOR;
    else if (TObjDialogData.Potion) 	WorkObj.item_type = ITEM_POTION;
    else if (TObjDialogData.Furniture) WorkObj.item_type = ITEM_FURNITURE;
    else if (TObjDialogData.Trash) 		WorkObj.item_type = ITEM_TRASH;
    else if (TObjDialogData.Container) WorkObj.item_type = ITEM_CONTAINER;
    else if (TObjDialogData.DrinkCon)  WorkObj.item_type = ITEM_DRINK_CON;
    else if (TObjDialogData.Key) 		WorkObj.item_type = ITEM_KEY;
    else if (TObjDialogData.Food) 		WorkObj.item_type = ITEM_FOOD;
    else if (TObjDialogData.Gold) 		WorkObj.item_type = ITEM_MONEY;
    else if (TObjDialogData.Boat) 		WorkObj.item_type = ITEM_BOAT;
    else if (TObjDialogData.NPCCorpse) WorkObj.item_type = ITEM_CORPSE_NPC;
    else if (TObjDialogData.Fountain)  WorkObj.item_type = ITEM_FOUNTAIN;
    else if (TObjDialogData.Pill)  		WorkObj.item_type = ITEM_PILL;
    else                               WorkObj.item_type = (sh_int) atoi(m_strType.c_str());

    // ROM item type
    WorkObj.strType = m_strType;

    // Wear position
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Take, ITEM_TAKE);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Finger, ITEM_WEAR_FINGER);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Neck, ITEM_WEAR_NECK);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Body, ITEM_WEAR_BODY);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Head, ITEM_WEAR_HEAD);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Legs, ITEM_WEAR_LEGS);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Feet, ITEM_WEAR_FEET);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Hands, ITEM_WEAR_HANDS);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Arms, ITEM_WEAR_ARMS);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Shield, ITEM_WEAR_SHIELD);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.AboutBody, ITEM_WEAR_ABOUT);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Waist, ITEM_WEAR_WAIST);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Wrist, ITEM_WEAR_WRIST);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Wield, ITEM_WIELD);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.Hold, ITEM_HOLD);
    SET_RESET_BIT(WorkObj.wear_flags, TObjDialogData.WithPride, ITEM_WORN_PRIDE);

    // Extra flags
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Glow, ITEM_GLOW);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Hum, ITEM_HUM);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Evil, ITEM_EVIL);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Magic, ITEM_MAGIC);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.NoDrop, ITEM_NODROP);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.AntiGood, ITEM_ANTI_GOOD);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.AntiEvil, ITEM_ANTI_EVIL);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.AntiNeutral, ITEM_ANTI_NEUTRAL);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.NoRemove, ITEM_NOREMOVE);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Invis, ITEM_INVIS);
    SET_RESET_BIT(WorkObj.extra_flags, TObjDialogData.Metallic, ITEM_METALLIC);

    // Weight
    WorkObj.weight = atoi(TObjDialogData.Weight);

	 // Save possible changes to current apply
    char szBuffer[16];
    if (nCurrApply >= 0)
       {
		 ApplyAmount->GetText(szBuffer, sizeof szBuffer);
       WorkObj.affected[nCurrApply]->modifier = (sh_int) atoi(szBuffer);
       }

    WorkObj.AreaDoc->ObjectSave(WorkObj, nIndex);
    return TRUE;
}

void TObjDialog::LoadAnotherObject(const TObject *o)
{
   TheActualObject = o;
   WorkObj = *o;
   LoadTransfer();
   TransferData(tdSetData);
   CmTakeChanged();
//   CmWieldChanged();
   CmTypeChanged();
   nCurrApply = -1;
   CmApplySelected();
}

void TObjDialog::SetPrompts()
{
#if 0
   static char *szPrompts[][4] = {
      { "", "", "", "" },                                   // 'other'

      { "", "", "hours", "" },										// light
      { "level", "spell", "spell", "spell" },           		// scroll
      { "level", "max charge", "charge left", "spell" },		// wand
      { "level", "max charge", "charge left", "spell" },		// staff
      { "", "min dam", "max dam", "type" },						// weapon
      { "?", "?", "?", "?" },
      { "?", "?", "?", "?" },
		{ "", "", "", "" },											   // treasure
		{ "ac", "", "", "" },											// armor
      { "level", "spell", "spell", "spell" },           		// potion
      { "?", "?", "?", "?" },
      { "", "", "", "" },												// other
      { "", "", "", "" },												// trash
      { "?", "?", "?", "?" },
		{ "capacity", "type", "key", "" },							// container
      { "?", "?", "?", "?" },
		{ "capacity", "left", "liquid", "poison" },				// drink
      { "", "", "", "" },												// key
      { "hours", "", "", "" },										// food
      { "# coins", "", "", "" },										// gold
      { "?", "?", "?", "?" },
      { "", "", "", "" },												// boat
      { "", "", "", "" },												// corpse
      { "?", "?", "?", "?" },                               // (no PC corpses!)
      { "", "", "", "" },												// fountain
      { "level", "spell", "spell", "spell" },           		// pill

      { "", "", "", "" },												// custom 1
      { "", "", "", "" },												// custom 2
      { "", "", "", "" },												// custom 3
   };
#endif
#if 0
   sh_int nType = 0;
   if (Light->GetCheck() == BF_CHECKED)
      nType = ITEM_LIGHT;
   else if (Scroll->GetCheck() == BF_CHECKED)
      nType = ITEM_SCROLL;
   else if (Wand->GetCheck() == BF_CHECKED)
      nType = ITEM_WAND;
   else if (Staff->GetCheck() == BF_CHECKED)
      nType = ITEM_STAFF;
   else if (Weapon->GetCheck() == BF_CHECKED)
      nType = ITEM_WEAPON;
   else if (Treasure->GetCheck() == BF_CHECKED)
      nType = ITEM_TREASURE;
   else if (Armor->GetCheck() == BF_CHECKED)
      nType = ITEM_ARMOR;
   else if (Potion->GetCheck() == BF_CHECKED)
      nType = ITEM_POTION;
   else if (Furniture->GetCheck() == BF_CHECKED)
      nType = ITEM_FURNITURE;
   else if (Trash->GetCheck() == BF_CHECKED)
      nType = ITEM_TRASH;
   else if (Container->GetCheck() == BF_CHECKED)
      nType = ITEM_CONTAINER;
   else if (DrinkCon->GetCheck() == BF_CHECKED)
      nType = ITEM_DRINK_CON;
   else if (Key->GetCheck() == BF_CHECKED)
      nType = ITEM_KEY;
   else if (Food->GetCheck() == BF_CHECKED)
      nType = ITEM_FOOD;
   else if (Gold->GetCheck() == BF_CHECKED)
      nType = ITEM_MONEY;
   else if (Boat->GetCheck() == BF_CHECKED)
      nType = ITEM_BOAT;
   else if (NPCCorpse->GetCheck() == BF_CHECKED)
      nType = ITEM_CORPSE_NPC;
   else if (Fountain->GetCheck() == BF_CHECKED)
      nType = ITEM_FOUNTAIN;
   else if (Pill->GetCheck() == BF_CHECKED)
      nType = ITEM_PILL;
#if 0
   else if (Custom1->GetCheck() == BF_CHECKED)
      nType = 27;
   else if (Custom2->GetCheck() == BF_CHECKED)
      nType = 28;
   else if (Custom3->GetCheck() == BF_CHECKED)
      nType = 29;
#endif
#endif
//   else {
      char szBuffer[32];
      OtherText->GetText(szBuffer, sizeof szBuffer);
      m_strType = szBuffer;
//   }

#if 0
   for (int i = 0; i < 4; i++)
      Prompt[i]->SetText(szPrompts[nType][i]);
#else
   BOOL fRom = (WorkObj.AreaDoc->GetMode() == poundsApp::RomMode ||
                WorkObj.AreaDoc->GetMode() == poundsApp::Rom24Mode);
   int nMaxObjects = (fRom) ? 5 : 4;
   for (int i = 0; i < nMaxObjects; i++) {
      const char *pszPrompt = m_Library->GetPromptText(m_strType, i);
      Prompt[i]->SetText(pszPrompt);

      // Fill any lists with data from the dictionary
      Value[i]->ClearList();
//      if (m_Library->GetTypeInfo(WorkObj.strType, i) == LIST_TYPE) [
      if (m_Library->GetTypeInfo(m_strType, i) == LIST_TYPE) {
         m_Library->LoadListBox(pszPrompt, Value[i]);
//         m_Library->LoadComboBox(pszPrompt, Value[i]);
      }

//      BOOL fFlags = (m_Library->GetTypeInfo(WorkObj.strType, i) == 0);
      BOOL fFlags = (m_Library->GetTypeInfo(m_strType, i) == 0);
      m_AddFlag[i]->EnableWindow(fFlags);
//      m_RemFlag[i]->EnableWindow(fFlags);
   }
#endif

//   nCurrentType = nType;
//   Metallic->EnableWindow(nCurrentType == ITEM_ARMOR &&
//                             WorkObj.AreaDoc->GetMode() == poundsApp::TFCMode);
}


void TObjDialog::CmTypeChanged ()
{
    // INSERT>> Your code here.
    sh_int nType = 0;
    if (Light->GetCheck() == BF_CHECKED)
        nType = ITEM_LIGHT;
    else if (Scroll->GetCheck() == BF_CHECKED)
      nType = ITEM_SCROLL;
    else if (Wand->GetCheck() == BF_CHECKED)
      nType = ITEM_WAND;
    else if (Staff->GetCheck() == BF_CHECKED)
      nType = ITEM_STAFF;
    else if (Weapon->GetCheck() == BF_CHECKED)
      nType = ITEM_WEAPON;
    else if (Treasure->GetCheck() == BF_CHECKED)
      nType = ITEM_TREASURE;
    else if (Armor->GetCheck() == BF_CHECKED)
      nType = ITEM_ARMOR;
    else if (Potion->GetCheck() == BF_CHECKED)
      nType = ITEM_POTION;
    else if (Furniture->GetCheck() == BF_CHECKED)
      nType = ITEM_FURNITURE;
    else if (Trash->GetCheck() == BF_CHECKED)
      nType = ITEM_TRASH;
    else if (Container->GetCheck() == BF_CHECKED)
      nType = ITEM_CONTAINER;
    else if (DrinkCon->GetCheck() == BF_CHECKED)
      nType = ITEM_DRINK_CON;
    else if (Key->GetCheck() == BF_CHECKED)
      nType = ITEM_KEY;
    else if (Food->GetCheck() == BF_CHECKED)
      nType = ITEM_FOOD;
    else if (Gold->GetCheck() == BF_CHECKED)
      nType = ITEM_MONEY;
    else if (Boat->GetCheck() == BF_CHECKED)
      nType = ITEM_BOAT;
    else if (NPCCorpse->GetCheck() == BF_CHECKED)
      nType = ITEM_CORPSE_NPC;
    else if (Fountain->GetCheck() == BF_CHECKED)
      nType = ITEM_FOUNTAIN;
    else if (Pill->GetCheck() == BF_CHECKED)
      nType = ITEM_PILL;

    if (nType) {
      nCurrentType = nType;
      const string& strType = m_Library->GetStringFromType(nCurrentType);
      // Set OtherText.  This will be used to set m_strType later
      OtherText->SetText(strType.c_str());
//      m_strType = m_Library->GetStringFromType(nCurrentType);
//      OtherText->SetText(m_strType.c_str());
      OtherText->EnableWindow(FALSE);
    } else {
      // The 'other' radio button must be checked
      nCurrentType = ITEM_PILL+1; // @@@
      OtherText->EnableWindow(TRUE);
    }

    SetPrompts();
    LoadValues();
    ObjType->SendMessage(STM_SETICON, (WPARAM) hObjTypeIcon[nCurrentType], 0);

//    OtherText->EnableWindow(Other->GetCheck() == BF_CHECKED);
    if (WorkObj.AreaDoc->GetMode() == poundsApp::TFCMode)
       Metallic->EnableWindow(nCurrentType == ITEM_ARMOR);
}


void TObjDialog::CmTakeChanged ()
{
    // INSERT>> Your code here.
    BOOL fTakable = (Take->GetCheck() == BF_CHECKED);

    Finger->EnableWindow(fTakable);
    Neck->EnableWindow(fTakable);
    Body->EnableWindow(fTakable);
    Head->EnableWindow(fTakable);
    Legs->EnableWindow(fTakable);
    Feet->EnableWindow(fTakable);
    Hands->EnableWindow(fTakable);
    Arms->EnableWindow(fTakable);
    Shield->EnableWindow(fTakable);
    AboutBody->EnableWindow(fTakable);
    Waist->EnableWindow(fTakable);
    Wrist->EnableWindow(fTakable);
    Wield->EnableWindow(fTakable);
    Hold->EnableWindow(fTakable);
    WithPride->EnableWindow(fTakable &&
                            WorkObj.AreaDoc->GetMode() == poundsApp::TFCMode);

    CmWieldChanged();
}


void TObjDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
//    LongDescr->SendMessage(WM_SETFONT,
//       (WPARAM) (HFONT) WorkObj.AreaDoc->GetMZF()->hFixedFont, 0);

#if 0
    // If any custom object types have been specified, load them here
    char szBuffer[32];
    GetProfileString("MAKE.ZONES.FAST", "ObjType27", "", szBuffer,
       sizeof szBuffer);
    if (szBuffer[0])
       Custom1->SetCaption(szBuffer);
    else
       Custom1->ShowWindow(SW_HIDE);

    GetProfileString("MAKE.ZONES.FAST", "ObjType28", "", szBuffer,
       sizeof szBuffer);
    if (szBuffer[0])
       Custom2->SetCaption(szBuffer);
    else
       Custom2->ShowWindow(SW_HIDE);

    GetProfileString("MAKE.ZONES.FAST", "ObjType29", "", szBuffer,
       sizeof szBuffer);
    if (szBuffer[0])
       Custom3->SetCaption(szBuffer);
    else
       Custom3->ShowWindow(SW_HIDE);
#endif
    // If this is not ROM, disable the ROM features
    BOOL fRom = (WorkObj.AreaDoc->GetMode() == poundsApp::RomMode ||
                 WorkObj.AreaDoc->GetMode() == poundsApp::Rom24Mode);
    if (fRom) {
       Metallic->SetWindowText("No Purge");
    } else {
       Prompt[4]->SetText("");
       Prompt[4]->EnableWindow(FALSE);
       Value[4]->EnableWindow(FALSE);
       m_AddFlag[4]->EnableWindow(FALSE);
       Code->EnableWindow(FALSE);

       BOOL fEnvy = (WorkObj.AreaDoc->GetMode() == poundsApp::EnvyMode ||
                     WorkObj.AreaDoc->GetMode() == poundsApp::Envy2Mode);
       if (fEnvy) {
          Metallic->SetWindowText("Poison");
       }
    }

    CmTakeChanged();
    CmTypeChanged();
    nCurrApply = -1;
    CmApplySelected();
//    CmWieldChanged();
}

void TObjDialog::LoadValues()
{
//    // @@@ use nCurrentType
//    int32 nVal[5];
//    for (int i = 0; i < nelems(Value); i++) {
////       Value[i]->ClearList();
//       nVal[i] = atol(WorkObj.value[i].c_str());
//       }

#if 0
    const char *szText; // = 0;
    int n;
    int nSlot;
    switch (nCurrentType)
       {
       case ITEM_WEAPON:
          for (i = 0; i < 3; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }

          n = 0;
		    szText = TAreaDocument::DamageType(n);
          while (szText)
             {
             Value[3]->AddString(szText);
             if (n == nVal[3])
                Value[3]->SetText(szText);
             n++;
             szText = TAreaDocument::DamageType(n);
             }
          break;

       case ITEM_SCROLL:
       case ITEM_POTION:
       case ITEM_PILL:
          Value[0]->AddString(WorkObj.value[0].c_str());
          Value[0]->SetText(WorkObj.value[0].c_str());

#if 0
          // @@@ This needs to handle unknown spell #s
          for (n = -1; n <= TAreaDocument::MaxSpell(); n++)
             {
             szText = TAreaDocument::SpellName(n);
             if (szText)
                for (i = 1; i < 4; i++)
                   {
                   Value[i]->AddString(szText);
                   if (nVal[i] == n)
                      Value[i]->SetText(szText);
                   }
             }
#else
          n = 0;
          while ((szText = TAreaDocument::SpellIndex(n, &nSlot)) != 0)
           {
           if (nSlot != 0)
             {
             for (i = 1; i < 4; i++)
                {
                Value[i]->AddString(szText);
                if (nVal[i] == nSlot)
                   Value[i]->SetText(szText);
                }
             }
           n++;
           }
#endif
          break;

       case ITEM_WAND:
       case ITEM_STAFF:
          for (i = 0; i < 3; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }

#if 0
          // @@@ This needs to handle unknown spell #s
          for (n = -1; n <= TAreaDocument::MaxSpell(); n++)
             {
             szText = TAreaDocument::SpellName(n);
             if (szText)
                {
                Value[3]->AddString(szText);
                if (nVal[3] == n)
                   Value[3]->SetText(szText);
                }
             }
#else
          n = 0;
          while ((szText = TAreaDocument::SpellIndex(n, &nSlot)) != 0)
            {
            if (nSlot != 0)
             {
             Value[3]->AddString(szText);
             if (nVal[3] == nSlot)
                Value[3]->SetText(szText);
             }
            n++;
            }
#endif
          break;

       case ITEM_CONTAINER:
//@@@   value[0]    weight capacity
//@@@   value[1]    flags: 1 closeable, 2 pickproof, 4 closed, 8 locked
//@@@   value[2]    key vnum
//@@@   value[3]    unused
          for (i = 0; i < 4; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }
          break;

       case ITEM_DRINK_CON:
          for (i = 0; i < 2; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }

          n = 0;
		    szText = TAreaDocument::LiquidType(n);
          while (szText)
             {
             Value[2]->AddString(szText);
             if (n == nVal[2])
                Value[2]->SetText(szText);
             n++;
             szText = TAreaDocument::LiquidType(n);
             }

          Value[3]->AddString("False");
          Value[3]->AddString("True");
          Value[3]->SetText((nVal[3]) ? "True" : "False");
          break;

//       case ITEM_FURNITURE:
//       case ITEM_TRASH:
//       case ITEM_TREASURE:
//       case ITEM_ARMOR:
//       case ITEM_LIGHT:
//       case ITEM_KEY:
//       case ITEM_FOOD:
//       case ITEM_MONEY:
//       case ITEM_BOAT:
//       case ITEM_CORPSE_NPC:
//       case ITEM_FOUNTAIN:
       default:
          for (i = 0; i < 4; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }
          break;
       }
#endif

   // Put the slot value into the combobox's edit area
   int i;
   for (i = 0; i < nelems(Value); i++) {
//      Value[i]->AddString(WorkObj.value[i].c_str());
      Value[i]->SetText(WorkObj.value[i].c_str());
   }
}


void TObjDialog::CmApplySelected ()
{
    // INSERT>> Your code here.
    char szBuffer[16];
    if (nCurrApply >= 0)
       {
		 ApplyAmount->GetText(szBuffer, sizeof szBuffer);
       WorkObj.affected[nCurrApply]->modifier = (sh_int) atoi(szBuffer);
       }
    // @@@ Don't think I am actually using apply-amount...

    int n = AppliesList->GetSelIndex();
    if (n < 0)
       {
       ApplyAmount->SetText("");
       nCurrApply = -1;
       }
    else
       {
       nCurrApply = n;
       nApplyAmount = WorkObj.affected[n]->modifier;
       wsprintf(szBuffer, "%d", nApplyAmount);
       ApplyAmount->SetText(szBuffer);
       }
}


void TObjDialog::CmEditExtra ()
{
    // INSERT>> Your code here.
    int n = ExtraList->GetSelIndex();
    if (n >= 0)
       {
       if (TExtraDialog(this, WorkObj.extra_descr[n], WorkObj.AreaDoc).Execute() == IDOK)
          {
          ExtraList->DeleteString(n);
          ExtraList->InsertString(WorkObj.extra_descr[n]->keyword.c_str(), n);
          }
       }
    else
       MessageBeep(-1);
}


void TObjDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);

    // Try to save the values.  If they cannot be saved, the user wants
    // a second chance editing
    if (!SaveTransfer())
      return;

    TDialog::CmOk();
}

BOOL TObjDialog::SaveValues()
{
#if 0
    // @@@ use nCurrentType
    char szBuffer[5][64];
    int32 nVal[5];
    for (int i = 0; i < nelems(Value); i++)
       {
       Value[i]->GetText(szBuffer[i], sizeof szBuffer[i]);
       nVal[i] = atol(szBuffer[i]);
       }

    switch (nCurrentType)
       {
       case ITEM_WEAPON:
          nVal[3] = TAreaDocument::DamageType(szBuffer[3]);
          break;

       case ITEM_SCROLL:
       case ITEM_POTION:
       case ITEM_PILL:
          for (i = 1; i < 4; i++)
             {
             nVal[i] = TAreaDocument::SpellValue(szBuffer[i]);
//             // @@@ Find a better way to do this...
//             if (nVal[i] == 0)
//                nVal[i] = -1;
             }
          break;

       case ITEM_WAND:
       case ITEM_STAFF:
          nVal[3] = TAreaDocument::SpellValue(szBuffer[3]);
          break;

#if 0
       case ITEM_CONTAINER:
//@@@   value[0]    weight capacity
//@@@   value[1]    flags: 1 closeable, 2 pickproof, 4 closed, 8 locked
//@@@   value[2]    key vnum
//@@@   value[3]    unused
          for (i = 0; i < 4; i++)
             {
             Value[i]->AddString(WorkObj.value[i].c_str());
             Value[i]->SetText(WorkObj.value[i].c_str());
             }
          break;
#endif
       case ITEM_DRINK_CON:
		    nVal[2] = TAreaDocument::LiquidVal(szBuffer[2]);
          nVal[3] = (UPPER(szBuffer[3][0]) == 'T');
          break;
       }

   for (i = 0; i < 4; i++)
      {
      wsprintf(szBuffer[i], "%ld", nVal[i]);
      WorkObj.value[i] = szBuffer[i];
      }
#else
   char szBuffer[512];
   for (int i = 0; i < nelems(Value); i++) {
      Value[i]->GetText(szBuffer, sizeof szBuffer);
      if (m_Library->GetTypeInfo(WorkObj.strType, i) == LIST_TYPE) { // A List?
//         const char *pszPrompt = m_Library->GetPromptText(WorkObj.strType, i);
         const char *pszPrompt = m_Library->GetPromptText(m_strType, i);
         if (!m_Library->IsListItem(pszPrompt, szBuffer)) {
            MessageBeep(MB_ICONQUESTION);
            char szMsg[512];
            wsprintf(szMsg, "%s is not defined in the MZF database as a "
               "valid %s.  Would you like to add it permanently?",
               szBuffer, pszPrompt);
            if (MessageBox(szMsg, "M.Z.F.", MB_ICONQUESTION|MB_YESNO) == IDYES) {
               char szPrompt[128], szMercType[16];
               wsprintf(szPrompt, "Merc translation for %s:", szBuffer);
               strcpy(szMercType, "0");
               TInputDialog dlgMerc(this, "Add a list item", szPrompt,
                  szMercType, sizeof szMercType);
               if (dlgMerc.Execute() != IDOK) {
                  // If the user won't add to the database, we don't save values
                  Value[i]->SetFocus();
                  return FALSE;
               }
               m_Library->AddListItem(pszPrompt, szBuffer, atol(szMercType));
            } else {
               // If the user won't add to the database, we don't save values
               Value[i]->SetFocus();
               return FALSE;
            }
         }
      // @@@ We should check for flags here as well...
      }

      WorkObj.value[i] = szBuffer;
      }

   // Values were successfully saved
   return TRUE;
#endif
}


void TObjDialog::CmAddExtra ()
{
    // INSERT>> Your code here.
    TExtraDescr *xtra = new TExtraDescr;
    if (TExtraDialog(this, xtra, WorkObj.AreaDoc).Execute() == IDOK)
       {
       WorkObj.extra_descr.Add(xtra);
       ExtraList->AddString(xtra->keyword.c_str());
       }
    else
       delete xtra;
}


void TObjDialog::CmDelExtra ()
{
    // INSERT>> Your code here.
    int n = ExtraList->GetSelIndex();
    if (n >= 0)
       {
       ExtraList->DeleteString(n);
       WorkObj.extra_descr.Detach(n);  // @@@ Should we delete?
       }
}


void TObjDialog::CmAddApply ()
{
    // INSERT>> Your code here.
    CmApplySelected();	// Save the old apply, just in case
    int nLoc;
    if (TApplyDialog(this, &nLoc).Execute() == IDOK && nLoc > 0)
       {
       TAffect *pAff = new TAffect;
       pAff->location = (sh_int) nLoc;
       pAff->modifier = 0;
       WorkObj.affected.Add(pAff);
       AppliesList->AddString(GetAffectName(nLoc));
       }
}


void TObjDialog::CmDelApply ()
{
    // INSERT>> Your code here.
    int n = AppliesList->GetSelIndex();
    if (n >= 0) {
       AppliesList->DeleteString(n);
       WorkObj.affected.Detach(n);  // @@@ Should we delete?

       // Bring up the first apply remaining, if any
       AppliesList->SetSelIndex(0);
       // Set the number field to match what is in the list box
       CmApplySelected();
    } else
       MessageBeep(MB_ICONHAND);
}


void TObjDialog::CmWieldChanged ()
{
    // INSERT>> Your code here.
// @@@ Should I check for lights?
// @@@ Should I combine code with SaveTransfer?

    // Calculate current wear bits
    int32 wear = 0;
    SET_RESET_BIT(wear, Take->GetCheck() == BF_CHECKED, ITEM_TAKE);
    SET_RESET_BIT(wear, Finger->GetCheck() == BF_CHECKED, ITEM_WEAR_FINGER);
    SET_RESET_BIT(wear, Neck->GetCheck() == BF_CHECKED, ITEM_WEAR_NECK);
    SET_RESET_BIT(wear, Body->GetCheck() == BF_CHECKED, ITEM_WEAR_BODY);
    SET_RESET_BIT(wear, Head->GetCheck() == BF_CHECKED, ITEM_WEAR_HEAD);
    SET_RESET_BIT(wear, Legs->GetCheck() == BF_CHECKED, ITEM_WEAR_LEGS);
    SET_RESET_BIT(wear, Feet->GetCheck() == BF_CHECKED, ITEM_WEAR_FEET);
    SET_RESET_BIT(wear, Hands->GetCheck() == BF_CHECKED, ITEM_WEAR_HANDS);
    SET_RESET_BIT(wear, Arms->GetCheck() == BF_CHECKED, ITEM_WEAR_ARMS);
    SET_RESET_BIT(wear, Shield->GetCheck() == BF_CHECKED, ITEM_WEAR_SHIELD);
    SET_RESET_BIT(wear, AboutBody->GetCheck() == BF_CHECKED, ITEM_WEAR_ABOUT);
    SET_RESET_BIT(wear, Waist->GetCheck() == BF_CHECKED, ITEM_WEAR_WAIST);
    SET_RESET_BIT(wear, Wrist->GetCheck() == BF_CHECKED, ITEM_WEAR_WRIST);
    SET_RESET_BIT(wear, Wield->GetCheck() == BF_CHECKED, ITEM_WIELD);
    SET_RESET_BIT(wear, Hold->GetCheck() == BF_CHECKED, ITEM_HOLD);
    SET_RESET_BIT(wear, WithPride->GetCheck() == BF_CHECKED, ITEM_WORN_PRIDE);

    // Calculate nIcon
    int nIcon;
    // a non-takable thing?
    if (wear == 0)
       nIcon = 0;

    // Armor w/o take bit?
    else if ((wear & ITEM_TAKE) == 0 && (wear & ~ITEM_TAKE))
       {
       nIcon = 1;
       NoRemove->EnableWindow(FALSE);
       }

    // Just takeable, not wieldable?
    else if (wear == ITEM_TAKE)
       {
       nIcon = 2;
       NoRemove->EnableWindow(FALSE);
       }

    else
       {
       NoRemove->EnableWindow(TRUE);
       int n = 3;
       for (wear >>= 1; ; wear >>= 1, n++)
          if (wear & 1)
             { /*nIcon = n;*/ break; }

       if (wear & ~1)
          nIcon = 1;
       else
          nIcon = n;
       }

    WieldType->SendMessage(STM_SETICON, (WPARAM) hWearIcon[nIcon], 0);
}


void TObjDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_OBJDIA);
}

void TObjDialog::CmRomProps ()
{
    // No need to retrieve the control values, as nothing on this dialog
    // is needed for the temp dialog.


    TObject TempObj(WorkObj);
    if (TRomObjectDialog(this, TempObj).Execute() == IDOK) {
        WorkObj = TempObj;

        // There is no need to update the controls as nothing on the Object
        // dialog affects this dialog.
    }
}

void TObjDialog::CmCustomize ()
{
    TObjTypeLibrary libTemp( *TYPESAFE_DOWNCAST(m_Library, TObjTypeLibrary) );
//    TLibrary *libTemp = m_Library->Clone();
    if (TCustObjectDialog(this, m_strType, libTemp).Execute() == IDOK) {
       // Make type library changes permanent, if any
       if (libTemp.IsDirty()) {
          *m_Library = libTemp;
          SetPrompts();
       }
    }
}

void TObjDialog::CmOtherTextKillFocus()
{
   char szBuffer[32];
   OtherText->GetText(szBuffer, sizeof szBuffer);
   if (m_strType != szBuffer)
      CmTypeChanged();
}

void TObjDialog::CmAddFlag1()
{
   AddFlag(0);
}

void TObjDialog::CmRemFlag1()
{
   RemFlag(0);
}

void TObjDialog::CmAddFlag2()
{
   AddFlag(1);
}

void TObjDialog::CmRemFlag2()
{
   RemFlag(1);
}

void TObjDialog::CmAddFlag3()
{
   AddFlag(2);
}

void TObjDialog::CmRemFlag3()
{
   RemFlag(2);
}

void TObjDialog::CmAddFlag4()
{
   AddFlag(3);
}

void TObjDialog::CmRemFlag4()
{
   RemFlag(3);
}

void TObjDialog::CmAddFlag5()
{
   AddFlag(4);
}

void TObjDialog::CmRemFlag5()
{
   RemFlag(4);
}

void TObjDialog::AddFlag(int n)
{
   // Get the current value for this flag slot
   char szBuffer[128];
   Value[n]->GetText(szBuffer, sizeof szBuffer);
   int32 nFlag = TAreaDocument::Str2Flag(szBuffer);

   // Create a copy of the type library, which can be edited inside the
   // dialog
   TObjTypeLibrary libTemp( *TYPESAFE_DOWNCAST(m_Library, TObjTypeLibrary) );
//   TObjTypeLibrary libTemp( *m_Library );

   TFlagDialog d(this, &libTemp, m_Library->GetPromptText(m_strType, n),
      &nFlag, WorkObj.AreaDoc->GetMode());
   if (d.Execute() == IDOK) {
      // Make type library changes permanent, if any
      if (libTemp.IsDirty())
         *m_Library = libTemp;

      TAreaDocument::Flags2Str(WorkObj.AreaDoc->GetMode(), szBuffer, nFlag);
      Value[n]->SetText(szBuffer);
   }
}

void TObjDialog::RemFlag(int)
{
   MessageBeep(MB_ICONINFORMATION);
   MessageBox("This will bring up a list of flags for this value",
      "M.Z.F", MB_ICONINFORMATION|MB_OK);
}

////////////////////////////////////////////////////////////////////////////

static TRomObjDialogXfer TRomObjDialogData;

DEFINE_RESPONSE_TABLE1(TRomObjectDialog, TDialog)
    EV_BN_CLICKED(IDC_ADDAFFECT, CmAddAff),
    EV_BN_CLICKED(IDC_ADDIMM, CmAddImm),
    EV_BN_CLICKED(IDC_ADDRES, CmAddRes),
    EV_BN_CLICKED(IDC_ADDVULN, CmAddVuln),
    EV_BN_CLICKED(IDC_DELAFFECT, CmDelAff),
    EV_BN_CLICKED(IDC_DELIMM, CmDelImm),
    EV_BN_CLICKED(IDC_DELRES, CmDelRes),
    EV_BN_CLICKED(IDC_DELVULN, CmDelVuln),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
END_RESPONSE_TABLE;

TRomObjectDialog::TRomObjectDialog (TWindow* parent, TObject& TempObj, TModule *module):
    TDialog(parent, IDD_ROMOBJ, module), WorkObj( TempObj )
{
    Condition = new TComboBox(this, IDC_CONDITION);
    Material = new TComboBox(this, IDC_MATERIAL, 32);
    Level = new TEdit(this, IDC_LEVEL, sizeof TRomObjDialogData.Level);
    Affects = new TListBox(this, IDC_AFFECT);
    Imm = new TListBox(this, IDC_IMM);
    Res = new TListBox(this, IDC_RES);
    Vuln = new TListBox(this, IDC_VULN);
    NoPurge = new TCheckBox(this, IDC_NOPURGE);
    RotDeath = new TCheckBox(this, IDC_ROTDEATH);
    VisDeath = new TCheckBox(this, IDC_VISDEATH);
    MeltDrop = new TCheckBox(this, IDC_MELTDROP);
    ShellExtract = new TCheckBox(this, IDC_SELLEXTRACT);
    NonMetal = new TCheckBox(this, IDC_NONMETAL);
    NoLocate = new TCheckBox(this, IDC_NOLOCATE);
    BurnProof = new TCheckBox(this, IDC_BURNPROOF);
    NounCurse = new TCheckBox(this, IDC_NOUNCURSE);

    SetTransferBuffer(&TRomObjDialogData);
    LoadTransfer();
}

#if 0
static char *aszRomMaterials[] = {
   // 'oldstyle' isn't a material, it is what a lot of the objects in
   // stock ROM are made of, because no one has bothered to set them.
   "oldstyle",

   // Here are the real objects from the ROM builder's guide.  Rom,
   // however, doesn't use these for anything yet.
   "plastic",
   "slime",
   "jelly",
   "wax",
   "rubber",
   "oil",
   "balm",
   "cream",
   "hard leather",
   "soft leather",
   "kid leather",
   "fur",
   "snakeskin",
   "gut",
   "food",
   "meat",
   "bread",
   "wood",
   "hardwood",
   "softwood",
   "bamboo",
   "ebony",
   "cork",
   "horn",
   "light laen",
   "sponge",
   "elastic",
   "silk",
   "satin",
   "lace",
   "wool",
   "linen",
   "canvas",
   "cloth",
   "velvet",
   "felt",
   "paper",
   "parchment",
   "vellum",
   "hemp",
   "feathers",
   "webbing",
   "cardboard",
   "steel",
   "gold",
   "silver",
   "mithril",
   "platinum",
   "iron",
   "lead",
   "copper",
   "electrum",
   "bronze",
   "brass",
   "wire",
   "tin",
   "pewter",
   "metal",
   "dark laen",
   "porcelain",
   "ivory",
   "marble",
   "stone",
   "quartz",
   "corundum",
   "flint",
   "lodestone",
   "granite",
   "enamel",
   "obsidian",
   "adamantite",
   "glass",
   "pottery",
   "crystal",
   "ice",
   "bone",
   "shell",
   "coral",
   "energy",
   "fire",
   "air",
   "water",
   "acid",
   "coal",
   "sandstone",
   "clay",
   "ash",
   "earth",
   "diamond",
   "etherealness",
   "nothingness",
   "dragonscale",
   "blue dragonscale",
   "black dragonscale",
   "white dragonscale",
   "red dragonscale"
};
#endif

static struct RomCondition {
   char cCondition;
   const char *pszPercent;
} aRomConditions[] = {
   { 'P', "(Perfect) 100%" },
   { 'G', "(Good)     90%" },
   { 'A', "(Average)  75%" },
   { 'W', "(Worn)     50%" },
   { 'D', "(Dumpy)    25%" },
   { 'B', "(Bad)      10%" },
   { 'R', "(Ruined)    0%" }
};

static Bit2String far aRomAffects[] = {
   { AFF_BLIND,         "Blind" },
   { AFF_INVISIBLE,     "Invisible" },
   { AFF_DETECT_EVIL,   "Detect Evil" },
   { AFF_DETECT_INVIS,  "Detect Invis" },
   { AFF_DETECT_MAGIC,  "Detect Magic" },
   { AFF_DETECT_HIDDEN, "Detect Hidden" },
   { AFF_DETECT_GOOD,   "Detect Good" },
   { AFF_SANCTUARY,     "Sanctuary" },
   { AFF_FAERIE_FIRE,   "Faerie Fire" },
   { AFF_INFRARED,      "Infrared" },
   { AFF_CURSE,         "Curse" },
   { AFF_FLAMING,       "(**) Flaming" },
   { AFF_POISON,        "Poison" },
   { AFF_PROTECT_EVIL,  "Protect Evil" },
   { AFF_PROTECT_GOOD,  "Protect Good" },
   { AFF_SNEAK,         "Sneak" },
   { AFF_HIDE,          "Hide" },
   { AFF_SLEEP,         "Sleep" },
   { AFF_CHARM,         "Charm" },
   { AFF_FLYING,        "Flying" },
   { AFF_PASS_DOOR,     "Pass Door" },
   { AFF_HASTE,         "Haste" },
   { AFF_CALM,          "Calm" },
   { AFF_PLAGUE,        "Plague" },
   { AFF_WEAKEN,        "Weaken" },
   { AFF_DARK_VISION,   "Dark Vision" },
   { AFF_BERSERK,       "Berserk" },
   { AFF_SWIM,          "(**) Swim" },
   { AFF_REGENERATION,  "Regeneration" },
   { AFF_SLOW,          "Slow" }
};

static Bit2String far aRomImmunities[] = {
   { IMM_SUMMON,         "Summon" },
   { IMM_CHARM,          "Charm" },
   { IMM_MAGIC,          "Magic" },
   { IMM_WEAPON,         "Weapon" },
   { IMM_BASH,           "Bash" },
   { IMM_PIERCE,         "Pierce" },
   { IMM_SLASH,          "Slash" },
   { IMM_FIRE,           "Fire" },
   { IMM_COLD,           "Cold" },
   { IMM_LIGHTNING,      "Lightning" },
   { IMM_ACID,           "Acid" },
   { IMM_POISON,         "Poison" },
   { IMM_NEGATIVE,       "Negative" },
   { IMM_HOLY,           "Holy" },
   { IMM_ENERGY,         "Energy" },
   { IMM_MENTAL,         "Mental" },
   { IMM_DISEASE,        "Disease" },
   { IMM_DROWNING,       "Drowning" },
   { IMM_LIGHT,		    "Light" }
};

static Bit2String far aRomResistance[] = {
   { RES_CHARM,          "Charm" },
   { RES_MAGIC,          "Magic" },
   { RES_WEAPON,         "Weapon" },
   { RES_BASH,           "Bash" },
   { RES_PIERCE,         "Pierce" },
   { RES_SLASH,          "Slash" },
   { RES_FIRE,           "Fire" },
   { RES_COLD,           "Cold" },
   { RES_LIGHTNING,      "Lightning" },
   { RES_ACID,           "Acid" },
   { RES_POISON,         "Poison" },
   { RES_NEGATIVE,       "Negative" },
   { RES_HOLY,           "Holy" },
   { RES_ENERGY,         "Energy" },
   { RES_MENTAL,         "Mental" },
   { RES_DISEASE,        "Disease" },
   { RES_DROWNING,       "Drowning" },
   { RES_LIGHT,          "Light" },
};

static Bit2String far aRomVulnerability[] = {
   { VULN_MAGIC,         "Magic" },
   { VULN_WEAPON,        "Weapon" },
   { VULN_BASH,          "Bash" },
   { VULN_PIERCE,        "Pierce" },
   { VULN_SLASH,         "Slash" },
   { VULN_FIRE,          "Fire" },
   { VULN_COLD,          "Cold" },
   { VULN_LIGHTNING,     "Lightning" },
   { VULN_ACID,          "Acid" },
   { VULN_POISON,        "Poison" },
   { VULN_NEGATIVE,      "Negative" },
   { VULN_HOLY,          "Holy" },
   { VULN_ENERGY,        "Energy" },
   { VULN_MENTAL,        "Mental" },
   { VULN_DISEASE,       "Disease" },
   { VULN_DROWNING,      "Drowning" },
   { VULN_LIGHT,         "Light" },
   { VULN_WOOD,          "Wood" },
   { VULN_SILVER,        "Silver" },
   { VULN_IRON,          "Iron" }
};

void TRomObjectDialog::LoadTransfer()
{
    int i;
    for (i = 0; i < nelems(aRomConditions); i++)
       TRomObjDialogData.Condition.AddStringItem(
         aRomConditions[i].pszPercent, aRomConditions[i].cCondition,
         WorkObj.condition == aRomConditions[i].cCondition);

    TRomObjDialogData.Material.Clear();
    BOOL fFoundMaterial = FALSE;
    for (i = 0; i<nelems(TNewRomMobDialog::aszRomMaterials); i++) {
       BOOL fMaterial = (WorkObj.material==TNewRomMobDialog::aszRomMaterials[i]);
       TRomObjDialogData.Material.AddStringItem(TNewRomMobDialog::aszRomMaterials[i], 0,
         fMaterial);
       fFoundMaterial = (fFoundMaterial || fMaterial);
    }
    // If the material wasn't in our list from the builder's guide, add the
    // current material to the list.
    if (!fFoundMaterial) {
       TRomObjDialogData.Material.AddStringItem(WorkObj.material.c_str(), 0,
         TRUE);
    }

    wsprintf(TRomObjDialogData.Level, "%d", WorkObj.level);

//    BOOL fFirst = TRUE;
//    TRomObjDialogData.Affects.Clear();
//    for (i = 0; i<nelems(aRomAffects); i++) {
//      if ((aRomAffects[i].bits & WorkObj.affected_by) == aRomAffects[i].bits) {
//         TRomObjDialogData.Affects.AddStringItem(aRomAffects[i].pszAff,
//            aRomAffects[i].bits, fFirst);
//         fFirst = FALSE;
//      }
//    }

   LoadEnchantments(WorkObj.affected_by, TRomObjDialogData.Affects,
      aRomAffects, nelems(aRomAffects));
   LoadEnchantments(WorkObj.immunity, TRomObjDialogData.Imm,
      aRomImmunities, nelems(aRomImmunities));
   LoadEnchantments(WorkObj.resistance, TRomObjDialogData.Res,
      aRomResistance, nelems(aRomResistance));
   LoadEnchantments(WorkObj.vulnerability, TRomObjDialogData.Vuln,
      aRomVulnerability, nelems(aRomVulnerability));

    TRomObjDialogData.NoPurge = IS_SET(WorkObj.extra_flags, ITEM_NOPURGE);
    TRomObjDialogData.RotDeath = IS_SET(WorkObj.extra_flags, ITEM_ROT_DEATH);
    TRomObjDialogData.VisDeath = IS_SET(WorkObj.extra_flags, ITEM_VIS_DEATH);
    TRomObjDialogData.MeltDrop = IS_SET(WorkObj.extra_flags, ITEM_MELT_DROP);
    TRomObjDialogData.ShellExtract = IS_SET(WorkObj.extra_flags, ITEM_SELL_EXTRACT);
    TRomObjDialogData.NonMetal = IS_SET(WorkObj.extra_flags, ITEM_NONMETAL);
    TRomObjDialogData.NoLocate = IS_SET(WorkObj.extra_flags, ITEM_NOLOCATE);
    TRomObjDialogData.BurnProof = IS_SET(WorkObj.extra_flags, ITEM_BURN_PROOF);
    TRomObjDialogData.NounCurse = IS_SET(WorkObj.extra_flags, ITEM_NOUNCURSE);
}

void TRomObjectDialog::CmHelp ()
{
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMOBJ);
}

void TRomObjectDialog::CmOk()
{
    TransferData(tdGetData);

    WorkObj.condition = (sh_int) TRomObjDialogData.Condition.
      GetItemDatas()[TRomObjDialogData.Condition.GetSelIndex()];

    char szBuffer[64];
    TRomObjDialogData.Material.GetSelString(szBuffer, sizeof szBuffer);
    WorkObj.material = szBuffer;

    WorkObj.level = (sh_int) atoi(TRomObjDialogData.Level);

    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.NoPurge,      ITEM_NOPURGE);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.RotDeath,     ITEM_ROT_DEATH);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.VisDeath,     ITEM_VIS_DEATH);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.MeltDrop,     ITEM_MELT_DROP);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.ShellExtract, ITEM_SELL_EXTRACT);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.NonMetal,     ITEM_NONMETAL);
     SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.NoLocate,     ITEM_NOLOCATE);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.BurnProof,    ITEM_BURN_PROOF);
    SET_RESET_BIT(WorkObj.extra_flags, TRomObjDialogData.NounCurse,    ITEM_NOUNCURSE);

    TDialog::CmOk();
}

void TRomObjectDialog::CmAddAff()
{
    TBitDialog dlg(this, aRomAffects, nelems(aRomAffects),
      &WorkObj.affected_by);

    if (dlg.Execute() == IDOK) {
        LoadEnchantments(WorkObj.affected_by, TRomObjDialogData.Affects,
            aRomAffects, nelems(aRomAffects));
        Affects->Transfer(&TRomObjDialogData.Affects, tdSetData);

    }
}

void TRomObjectDialog::CmAddImm()
{
    TBitDialog dlg(this, aRomImmunities, nelems(aRomImmunities),
      &WorkObj.immunity);

    if (dlg.Execute() == IDOK) {
        LoadEnchantments(WorkObj.immunity, TRomObjDialogData.Imm,
            aRomImmunities, nelems(aRomImmunities));
        Imm->Transfer(&TRomObjDialogData.Imm, tdSetData);
    }
}

void TRomObjectDialog::CmAddRes()
{
    TBitDialog dlg(this, aRomResistance, nelems(aRomResistance),
      &WorkObj.resistance);

    if (dlg.Execute() == IDOK) {
        LoadEnchantments(WorkObj.resistance, TRomObjDialogData.Res,
            aRomResistance, nelems(aRomResistance));
        Res->Transfer(&TRomObjDialogData.Res, tdSetData);
    }
}

void TRomObjectDialog::CmAddVuln()
{
    TBitDialog dlg(this, aRomVulnerability, nelems(aRomVulnerability),
      &WorkObj.vulnerability);

    if (dlg.Execute() == IDOK) {
        LoadEnchantments(WorkObj.vulnerability, TRomObjDialogData.Vuln,
           aRomVulnerability, nelems(aRomVulnerability));
        Vuln->Transfer(&TRomObjDialogData.Vuln, tdSetData);

    }
}

void TRomObjectDialog::CmDelAff()
{
    int n = Affects->GetSelIndex();
    if (n != LB_ERR) {
        WorkObj.affected_by &= ~Affects->GetItemData(n);
        LoadEnchantments(WorkObj.affected_by, TRomObjDialogData.Affects,
            aRomAffects, nelems(aRomAffects));
        Affects->Transfer(&TRomObjDialogData.Affects, tdSetData);
    }
}

void TRomObjectDialog::CmDelImm()
{
    int n = Imm->GetSelIndex();
    if (n != LB_ERR) {
        WorkObj.immunity &= ~Imm->GetItemData(n);
        LoadEnchantments(WorkObj.immunity, TRomObjDialogData.Imm,
            aRomImmunities, nelems(aRomImmunities));
        Imm->Transfer(&TRomObjDialogData.Imm, tdSetData);
    }
}

void TRomObjectDialog::CmDelRes()
{
    int n = Res->GetSelIndex();
    if (n != LB_ERR) {
        WorkObj.resistance &= ~Res->GetItemData(n);
        LoadEnchantments(WorkObj.resistance, TRomObjDialogData.Res,
            aRomResistance, nelems(aRomResistance));
        Res->Transfer(&TRomObjDialogData.Res, tdSetData);
    }
}

void TRomObjectDialog::CmDelVuln()
{
    int n = Vuln->GetSelIndex();
    if (n != LB_ERR) {
        WorkObj.vulnerability &= ~Vuln->GetItemData(n);
        LoadEnchantments(WorkObj.vulnerability, TRomObjDialogData.Vuln,
           aRomVulnerability, nelems(aRomVulnerability));
        Vuln->Transfer(&TRomObjDialogData.Vuln, tdSetData);
    }
}

void TRomObjectDialog::LoadEnchantments(int32 flags, TListBoxData& box,
   Bit2String *pTable, int nSize)
{
    BOOL fFirst = TRUE;
    box.Clear();
    for (int i = 0; i<nSize; i++, pTable++) {
      if ((pTable->bit & flags) == pTable->bit) {
         box.AddStringItem(pTable->psz, pTable->bit, fFirst);
         fFirst = FALSE;
      }
    }
}

////////////////////////////////////////////////////////////////////////////
static TCustObjectDialogXfer TCustObjectDialogData;

DEFINE_RESPONSE_TABLE1(TCustObjectDialog, TDialog)
//{{TCustObjectDialogRSP_TBL_BEGIN}}
    EV_EN_CHANGE(IDC_EDIT1, CmChangedPrompt1),
    EV_EN_CHANGE(IDC_EDIT2, CmChangedPrompt2),
    EV_EN_CHANGE(IDC_EDIT3, CmChangedPrompt3),
    EV_EN_CHANGE(IDC_EDIT4, CmChangedPrompt4),
    EV_EN_CHANGE(IDC_EDIT5, CmChangedPrompt5),
    EV_BN_CLICKED(IDC_FLAG1, CmClickedFlag1),
    EV_BN_CLICKED(IDC_FLAG2, CmClickedFlag2),
    EV_BN_CLICKED(IDC_FLAG3, CmClickedFlag3),
    EV_BN_CLICKED(IDC_FLAG4, CmClickedFlag4),
    EV_BN_CLICKED(IDC_FLAG5, CmClickedFlag5),
    EV_BN_CLICKED(IDC_WORD1, CmClickedWord1),
    EV_BN_CLICKED(IDC_WORD2, CmClickedWord2),
    EV_BN_CLICKED(IDC_WORD3, CmClickedWord3),
    EV_BN_CLICKED(IDC_WORD4, CmClickedWord4),
    EV_BN_CLICKED(IDC_WORD5, CmClickedWord5),
    EV_BN_CLICKED(IDC_LIST1, CmClickedList1),
    EV_BN_CLICKED(IDC_LIST2, CmClickedList2),
    EV_BN_CLICKED(IDC_LIST3, CmClickedList3),
    EV_BN_CLICKED(IDC_LIST4, CmClickedList4),
    EV_BN_CLICKED(IDC_LIST5, CmClickedList5),
    EV_BN_CLICKED(IDC_ADD1, CmAddFlag),
    EV_BN_CLICKED(IDC_ADD2, CmAddFlag),
    EV_BN_CLICKED(IDC_ADD3, CmAddFlag),
    EV_BN_CLICKED(IDC_ADD4, CmAddFlag),
    EV_BN_CLICKED(IDC_ADD5, CmAddFlag),
    EV_BN_CLICKED(IDC_REM1, CmRemFlag),
    EV_BN_CLICKED(IDC_REM2, CmRemFlag),
    EV_BN_CLICKED(IDC_REM3, CmRemFlag),
    EV_BN_CLICKED(IDC_REM4, CmRemFlag),
    EV_BN_CLICKED(IDC_REM5, CmRemFlag),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TCustObjectDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

TCustObjectDialog::TCustObjectDialog(TWindow *parent, string& strType,
      TLibrary& lib, TModule *module):
   TDialog(parent, IDD_OBJTYPEDIA, module),
   m_strType( strType )
{
   int i;
   // Because of the way the transfer buffer is layed out, we don't dare
   // combine these into one big loop.  The constructors must be called
   // in this order if they have data
   for (i = 0; i < nelems(m_EditPrompt); i++)
      m_EditPrompt[i] = new TEdit(this, IDC_EDIT1+i, 32);
   for (i = 0; i < nelems(m_EditPrompt); i++)
      m_StaticPrompt[i] = new TStatic(this, IDC_PROMPT1+i);
   for (i = 0; i < nelems(m_EditPrompt); i++)
      m_Flag[i] = new TRadioButton(this, IDC_FLAG1+i, 0);
   for (i = 0; i < nelems(m_EditPrompt); i++)
      m_List[i] = new TRadioButton(this, IDC_LIST1+i, 0);
   for (i = 0; i < nelems(m_EditPrompt); i++)
      m_Word[i] = new TRadioButton(this, IDC_WORD1+i, 0);

   // These constructors don't have any data, their order doesn't matter.
   for (i = 0; i < nelems(m_EditPrompt); i++) {
      m_AddFlag[i] = new TButton(this, IDC_ADD1+i);
//      m_RemFlag[i] = new TButton(this, IDC_REM1+i);
   }
   m_ObjName = new TStatic(this, IDC_OTHERTEXT);
   m_ObjIcon = new TStatic(this, IDC_OBJTYPE);
   m_MercType = new TEdit(this, IDC_OBJNUM,
      sizeof TCustObjectDialogData.MercType);

   SetTransferBuffer(&TCustObjectDialogData);

   m_Library = &lib;

   for (i = 0; i < nelems(m_EditPrompt); i++) {
      strcpy(TCustObjectDialogData.EditPrompt[i],
         m_Library->GetPromptText(m_strType, i));
      int n = m_Library->GetTypeInfo(m_strType, i);
      TCustObjectDialogData.Flag[i] = (n == 0);
      TCustObjectDialogData.Word[i] = (n == 1);
      TCustObjectDialogData.List[i] = (n == 2);
   }

   TTypeData *pTD = m_Library->GetTypeEntry(m_strType);
   if (pTD)
      wsprintf(TCustObjectDialogData.MercType, "%d", pTD->nMercType);
}

void TCustObjectDialog::SetupWindow()
{
   TDialog::SetupWindow();

   m_ObjName->SetText(m_strType.c_str());

   char szBuffer[32];
   for (int i = 0; i < nelems(m_EditPrompt); i++) {
      m_EditPrompt[i]->GetText(szBuffer, sizeof szBuffer);
      m_StaticPrompt[i]->SetText(szBuffer);

      BOOL fFlags = (m_Flag[i]->GetCheck() == BF_CHECKED);
      m_AddFlag[i]->EnableWindow(fFlags);
//      m_RemFlag[i]->EnableWindow(fFlags);
   }
}

void TCustObjectDialog::CmHelp ()
{
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT,
      IDD_OBJTYPEDIA);
}

void TCustObjectDialog::CmOk()
{
   TransferData(tdGetData);

   TTypeData *pTD = m_Library->GetTypeEntry(m_strType);
   if (pTD)
      pTD->nMercType = (sh_int) atoi(TCustObjectDialogData.MercType);

   TDialog::CmOk();
}

void TCustObjectDialog::CmChangedPrompt1()
{
   ChangedPrompt(0);
}

void TCustObjectDialog::CmChangedPrompt2()
{
   ChangedPrompt(1);
}

void TCustObjectDialog::CmChangedPrompt3()
{
   ChangedPrompt(2);
}

void TCustObjectDialog::CmChangedPrompt4()
{
   ChangedPrompt(3);
}

void TCustObjectDialog::CmChangedPrompt5()
{
   ChangedPrompt(4);
}

void TCustObjectDialog::CmClickedFlag1()
{
   ChangedType(0);
}

void TCustObjectDialog::CmClickedFlag2()
{
   ChangedType(1);
}

void TCustObjectDialog::CmClickedFlag3()
{
   ChangedType(2);
}

void TCustObjectDialog::CmClickedFlag4()
{
   ChangedType(3);
}

void TCustObjectDialog::CmClickedFlag5()
{
   ChangedType(4);
}

void TCustObjectDialog::CmClickedWord1()
{
   ChangedType(0);
}

void TCustObjectDialog::CmClickedWord2()
{
   ChangedType(1);
}

void TCustObjectDialog::CmClickedWord3()
{
   ChangedType(2);
}

void TCustObjectDialog::CmClickedWord4()
{
   ChangedType(3);
}

void TCustObjectDialog::CmClickedWord5()
{
   ChangedType(4);
}

void TCustObjectDialog::CmClickedList1()
{
   ChangedType(0);
}

void TCustObjectDialog::CmClickedList2()
{
   ChangedType(1);
}

void TCustObjectDialog::CmClickedList3()
{
   ChangedType(2);
}

void TCustObjectDialog::CmClickedList4()
{
   ChangedType(3);
}

void TCustObjectDialog::CmClickedList5()
{
   ChangedType(4);
}

void TCustObjectDialog::CmAddFlag()
{
   MessageBeep(MB_ICONINFORMATION);
   MessageBox("This will bring up a list of valid flags for this value",
      "M.Z.F", MB_ICONINFORMATION|MB_OK);
}

void TCustObjectDialog::CmRemFlag()
{
   MessageBeep(MB_ICONINFORMATION);
   MessageBox("This will bring up a list of flags for this value",
      "M.Z.F", MB_ICONINFORMATION|MB_OK);
}

void TCustObjectDialog::ChangedPrompt(int n)
{
   UpdateDictionary(n);
}

void TCustObjectDialog::ChangedType(int n)
{
   UpdateDictionary(n);
}

void TCustObjectDialog::UpdateDictionary(int n)
{
   char szBuffer[32];
   m_EditPrompt[n]->GetText(szBuffer, sizeof szBuffer);
   m_StaticPrompt[n]->SetText(szBuffer);

   int nType = -1;
   BOOL fFlags = FALSE;
   if (m_Flag[n]->GetCheck() == BF_CHECKED) {
      nType = 0; fFlags = TRUE;
   } else if (m_Word[n]->GetCheck() == BF_CHECKED)
      nType = 1;
   else if (m_List[n]->GetCheck() == BF_CHECKED)
      nType = 2;

   m_AddFlag[n]->EnableWindow(fFlags);
//   m_RemFlag[n]->EnableWindow(fFlags);

   m_Library->SetTypeInfo(m_strType, n, szBuffer, nType);
}

////////////////////////////////////////////////////////////////////////////

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TFlagDialog, TDialog)
//{{TFlagDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_BN_CLICKED(IDC_DEFFLAG, CmDefFlag),
    EV_BN_CLICKED(IDC_PURGEFLAG, CmPurgeFlag),
    EV_LBN_SELCHANGE(IDC_FLAGLIST, CmFlagSelection),
    EV_EN_CHANGE(IDC_NEWFLAG, CmFlagChanged),
    EV_EN_CHANGE(IDC_NEWMEANING, CmMeaningChanged),
//{{TFlagDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TFlagDialog Implementation}}


static TFlagDialogXfer TFlagDialogData;

TFlagDialog::TFlagDialog (TWindow* parent, TLibrary *pLibrary,
      const char *pszPrompt, int32 *pnFlags, poundsApp::currMode mode,
      TModule* module):
   // Ancestors
   TDialog(parent, IDD_FLAGDIA, module),

   // Members
   strPrompt(pszPrompt),
   m_pnFlags( pnFlags ),
   m_mode( mode )
{
//{{TFlagDialogXFER_USE}}
    FlagList = new TListBox(this, IDC_FLAGLIST);
    Flag = new TEdit(this, IDC_NEWFLAG, sizeof TFlagDialogData.Flag);
    Meaning = new TEdit(this, IDC_NEWMEANING, sizeof TFlagDialogData.Meaning);

    SetTransferBuffer(&TFlagDialogData);
//{{TFlagDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    m_pLibrary = pLibrary;
}


TFlagDialog::~TFlagDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
}

void TFlagDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    FlagList->ClearList();
    m_pLibrary->LoadListBoxFromFlags(strPrompt.c_str(), FlagList, *m_pnFlags);
    CmFlagSelection();
}

void TFlagDialog::CmOk ()
{
   *m_pnFlags = 0;
   int i = 0;
   int32 nFlag;
   for ( ;; i++ ) {
      int n = FlagList->GetSel(i);
      if (n > 0) {
         nFlag = FlagList->GetItemData(i);
         *m_pnFlags |= nFlag;
      } else if (n < 0)
         break;
   }

   TDialog::CmOk();
}

void TFlagDialog::CmHelp ()
{
   poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_FLAGDIA);
}

void TFlagDialog::CmDefFlag ()
{
   char szMeaning[64];
   Meaning->GetText(szMeaning, sizeof szMeaning);
   char szBuffer[32];
   Flag->GetText(szBuffer, sizeof szBuffer);
   int32 flag = TAreaDocument::Str2Flag(szBuffer);
   if (!m_pLibrary->AddFlag(strPrompt.c_str(), flag, szMeaning)) {
      MessageBox("Could not add flag.", "M.Z.F.", MB_ICONHAND|MB_OK);
      return;
   }

   FlagList->ClearList();
   m_pLibrary->LoadListBoxFromFlags(strPrompt.c_str(), FlagList, *m_pnFlags);
   CmFlagSelection();
}

void TFlagDialog::CmPurgeFlag ()
{
   char szFlag[32];
   Flag->GetText(szFlag, sizeof szFlag);
   int32 flag = TAreaDocument::Str2Flag(szFlag);
   if (!m_pLibrary->RemoveFlag(strPrompt.c_str(), flag)) {
      MessageBox("Could not remove flag.", "M.Z.F.", MB_ICONHAND|MB_OK);
      return;
   }

   FlagList->ClearList();
   m_pLibrary->LoadListBoxFromFlags(strPrompt.c_str(), FlagList, *m_pnFlags);
   CmFlagSelection();
}

void TFlagDialog::CmFlagSelection ()
{
   int n = FlagList->GetCaretIndex();
   if (n != LB_ERR) {
      char szBuffer[64];
      DWORD dw = FlagList->GetItemData(n);
      if (dw != (DWORD) LB_ERR) {
         TAreaDocument::Flags2Str(m_mode, szBuffer, dw);
         Flag->SetText(szBuffer);
         FlagList->GetString(szBuffer, n);
         Meaning->SetText(szBuffer);
      }
   }

   SetDefaultId(IDOK);
}

void TFlagDialog::CmFlagChanged ()
{
   SetDefaultId(IDC_DEFFLAG);
}

void TFlagDialog::CmMeaningChanged ()
{
   SetDefaultId(IDC_DEFFLAG);
}

////////////////////////////////////////////////////////////////////////////
// TBitDialog

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TBitDialog, TDialog)
//{{TBitDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TFlagDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TBitDialog Implementation}}


static TBitDialogXfer TBitDialogData;

TBitDialog::TBitDialog (TWindow* parent, Bit2String *pTable, int nSize,
        int32 *pnFlags, TModule* module):
   // Ancestors
   TDialog(parent, IDD_ENCHANTMENTS, module),

   // Members
   m_pnFlags( pnFlags )
{
//{{TBitDialogXFER_USE}}
    FlagList = new TListBox(this, IDC_FLAGLIST);

    SetTransferBuffer(&TBitDialogData);
//{{TBitDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    BOOL fFirst = TRUE;
    TBitDialogData.FlagList.Clear();
    for (int i = 0; i<nSize; i++, pTable++) {
      if ((pTable->bit & *m_pnFlags) == 0) {
         TBitDialogData.FlagList.AddStringItem(pTable->psz, pTable->bit,
            fFirst);
         fFirst = FALSE;
      }
    }
}


TBitDialog::~TBitDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
}

void TBitDialog::CmOk ()
{
   int n = FlagList->GetSelIndex();
   if (n != LB_ERR) {
      *m_pnFlags |= FlagList->GetItemData(n);
   }

   TDialog::CmOk();
}

void TBitDialog::CmHelp ()
{
   poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ENCHANTMENTS);
}

