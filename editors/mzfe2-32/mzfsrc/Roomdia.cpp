/*  Project pounds

    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         roomdia.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TRoomDialog (TDialog).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "roomdia.h"
#include "xtradia.h"
#include "exitdia.h"
#include "msetdia.h"
#include "appdia.h"
#include "condia.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TRoomDialog, TDialog)
//{{TRoomDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDC_PREV, CmPrev),
    EV_BN_CLICKED(IDC_NEXT, CmNext),
    EV_BN_CLICKED(IDC_INSIDE, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_NORTH, CmNorth),
    EV_BN_CLICKED(IDC_EAST, CmEast),
    EV_BN_CLICKED(IDC_SOUTH, CmSouth),
    EV_BN_CLICKED(IDC_WEST, CmWest),
    EV_BN_CLICKED(IDC_UP, CmUp),
    EV_BN_CLICKED(IDC_DOWN, CmDown),
    EV_BN_CLICKED(IDC_EDITEXTRA, CmEditExtra),
    EV_BN_CLICKED(IDC_ADDEXTRA, CmAddExtra),
    EV_BN_CLICKED(IDC_AIR, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_CITYSTREETS, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_DESERT, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_FIELD, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_FOREST, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_HILLS, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_MOUNTAIN, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_UNDERWATER, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_WATERNOSWIM, CmSectorTypeClicked),
    EV_BN_CLICKED(IDC_WATERSWIM, CmSectorTypeClicked),
    EV_BN_CLICKED(IDOK, CmOk),
    EV_LBN_DBLCLK(IDC_EXTRALIST, CmEditExtra),
    EV_BN_CLICKED(IDC_WALKCREATE, CmWalkCreate),
    EV_BN_CLICKED(IDC_DELEXTRA, CmDelExtra),
    EV_BN_CLICKED(IDC_DELEXIT, CmDelExit),
    EV_BN_CLICKED(IDC_SETEXIT, CmSetExit),
    EV_BN_CLICKED(IDC_ISDOOR, CmIsDoorChanged),
    EV_BN_CLICKED(IDC_CREATEMODE, CmCreateMode),
    EV_BN_CLICKED(IDC_WALKMODE, CmWalkMode),
    EV_BN_CLICKED(IDCANCEL, CmCancel),
    EV_BN_CLICKED(IDC_ADDMOB, CmAddMobReset),
    EV_BN_CLICKED(IDC_MOBEQU, CmMobEquip),
    EV_BN_CLICKED(IDC_DELMOB, CmDelMobReset),
    EV_LBN_DBLCLK(IDC_MOBS, CmMobEquip),
    EV_BN_CLICKED(IDC_ADDOBJ, CmAddObj),
    EV_BN_CLICKED(IDHELP, CmHelp),
    EV_BN_CLICKED(IDC_OBJCONT, CmObjectContents),
    EV_LBN_SELCHANGE(IDC_OBJS, CmIsContainer),
    EV_BN_CLICKED(IDC_CODE, CmRomProps),
//{{TRoomDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TRoomDialog Implementation}}


static TRoomDialogXfer far TRoomDialogData;

TRoomDialog::TRoomDialog (TWindow* parent, const TRoom *room, int n,
        TResId resId, TModule* module):
    TDialog(parent, resId, module), WorkRoom( *room )
{
//{{TRoomDialogXFER_USE}}
    Name = new TEdit(this, IDC_ROOMNAME, 255);
    Description = new TEdit(this, IDC_ROOMDESCR, 4096);
    SectorType = new TStatic(this, IDC_SECTORTYPE, 255);
    Inside = new TRadioButton(this, IDC_INSIDE, 0);
    CityStreets = new TRadioButton(this, IDC_CITYSTREETS, 0);
    Field = new TRadioButton(this, IDC_FIELD, 0);
    Forest = new TRadioButton(this, IDC_FOREST, 0);
    Hills = new TRadioButton(this, IDC_HILLS, 0);
    Mountain = new TRadioButton(this, IDC_MOUNTAIN, 0);
    WaterSwim = new TRadioButton(this, IDC_WATERSWIM, 0);
    WaterBoat = new TRadioButton(this, IDC_WATERNOSWIM, 0);
    Underwater = new TRadioButton(this, IDC_UNDERWATER, 0);
    Air = new TRadioButton(this, IDC_AIR, 0);
    Desert = new TRadioButton(this, IDC_DESERT, 0);
    Dark = new TCheckBox(this, IDC_DARK, 0);
    NoMob = new TCheckBox(this, IDC_NOMOB, 0);
    Indoors = new TCheckBox(this, IDC_INDOORS, 0);
    Private = new TCheckBox(this, IDC_PRIVATE, 0);
    Safe = new TCheckBox(this, IDC_SAFE, 0);
    Solitary = new TCheckBox(this, IDC_SOLITARY, 0);
    PetShop = new TCheckBox(this, IDC_PETSHOP, 0);
    NoRecall = new TCheckBox(this, IDC_NORECALL, 0);
    VNum = new TStatic(this, IDC_VNUM, 8);
    ExtraDescrList = new TListBox(this, IDC_EXTRALIST);
    for (int i = 0; i < nelems(Exit); i++)
       Exit[ i ] = new TRadioButton(this, IDC_NORTH + i, 0);
    ToPrompt = new TStatic(this, IDC_TOPROMPT, 255);
    ToText = new TStatic(this, IDC_TOTEXT, 255);
    KeyPrompt = new TStatic(this, IDC_KEYPROMPT, 255);
    KeyText = new TStatic(this, IDC_KEYTEXT, 255);
    WordPrompt = new TStatic(this, IDC_WORDPROMPT, 255);
    WordText = new TStatic(this, IDC_WORDTEXT, 255);
    ExitDescrPrompt = new TStatic(this, IDC_DESCRPROMPT, 255);
    ExitDescrText = new TStatic(this, IDC_DESCRTEXT, 255);
    IsDoor = new TCheckBox(this, IDC_ISDOOR, 0);
    IsPickProof = new TCheckBox(this, IDC_ISPICKPROOF, 0);
    WalkCreate = new TButton(this, IDC_WALKCREATE);
    DelExit = new TButton(this, IDC_DELEXIT);
    SetExit = new TButton(this, IDC_SETEXIT);
    WalkMode = new TRadioButton(this, IDC_WALKMODE, 0);
    CreateMode = new TRadioButton(this, IDC_CREATEMODE, 0);
    MobList = new TListBox(this, IDC_MOBS);
    ObjList = new TListBox(this, IDC_OBJS);
    RandomDoors = new TEdit(this, IDC_RANDOMDOORS, 255);
    DoorStatus = new TStatic(this, IDC_DOORSTATUS, 255);
    AddMob = new TButton(this, IDC_ADDMOB);
    AddObj = new TButton(this, IDC_ADDOBJ);
    IsBashProof = new TCheckBox(this, IDC_ISBASHPROOF, 0);
    IsPassProof = new TCheckBox(this, IDC_PASSPROOF, 0);
    Peaceful = new TCheckBox(this, IDC_PEACEFUL, 0);
    RFlag65536 = new TCheckBox(this, IDC_RFLAG65536, 0);
    RFlag131072 = new TCheckBox(this, IDC_RFLAG131072, 0);
    RFlag262144 = new TCheckBox(this, IDC_RFLAG2621444, 0);
    RFlag32768 = new TCheckBox(this, IDC_CONESIL, 0); // @@@ Change this in the .rh
    ObjectContents = new TButton(this, IDC_OBJCONT);
    Code = new TButton(this, IDC_CODE);

    SetTransferBuffer(&TRoomDialogData);
//{{TRoomDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
    AreaDoc = room->AreaDoc;
    TheActualRoom = room;
    nIndex = n;
    nCurrExit = -1;

    for (i = 0; i < nelems(ModifiedRooms); i++)
       {
       ModifiedRooms[i] = 0;
       fDeletedOtherSide[i] = 0;
       }

    LoadTransfer();

    for (i = 0; i < nelems(hSectorIcons); i++)
       hSectorIcons[i] = GetApplication()->LoadIcon(MAKEINTRESOURCE(1010+i));

    for (i = 0; i < nelems(hDoorIcons); i++)
       hDoorIcons[i] = GetApplication()->LoadIcon(MAKEINTRESOURCE(1600+i));

    VNum->EnableTransfer();
    IsDoor->DisableTransfer();
    IsPickProof->DisableTransfer();
    IsBashProof->DisableTransfer();
    IsPassProof->DisableTransfer();

    fCreateMode = FALSE;
	 TRoomDialogData.WalkMode = !fCreateMode;
    TRoomDialogData.CreateMode = fCreateMode;
}


TRoomDialog::~TRoomDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    for (int i = 0; i < nelems(hSectorIcons); i++)
       DestroyIcon(hSectorIcons[i]);
    for (i = 0; i < nelems(hDoorIcons); i++)
       DestroyIcon(hDoorIcons[i]);
}


void TRoomDialog::CmPrev ()
{
    // INSERT>> Your code here.
    const TRoom *r;
    if ((r = AreaDoc->GetRoomFromIndex(nIndex-1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex--;
       LoadAnotherRoom(r);
       return;
       }

    MessageBeep(-1);
}

void TRoomDialog::LoadAnotherRoom(const TRoom *r)
{
   TheActualRoom = r;
   WorkRoom = *r;
   LoadTransfer();
   TransferData(tdSetData);
   CmSectorTypeClicked();
   EnableExits();
   CmIsContainer();
}

void TRoomDialog::CmNext ()
{
    // INSERT>> Your code here.
    const TRoom *r;
    if ((r = AreaDoc->GetRoomFromIndex(nIndex+1)) != 0)
       {
   	 TransferData(tdGetData);
       SaveTransfer();
       nIndex++;
       LoadAnotherRoom(r);
       return;
       }

    MessageBeep(-1);
}

void TRoomDialog::LoadTransfer()
{
    strcpy(TRoomDialogData.Name, WorkRoom.name.c_str());
    strcpy(TRoomDialogData.Description, WorkRoom.description.c_str());

    TRoomDialogData.Inside = (WorkRoom.sector_type == SECT_INSIDE);
    TRoomDialogData.CityStreets = (WorkRoom.sector_type == SECT_CITY);
    TRoomDialogData.Field = (WorkRoom.sector_type == SECT_FIELD);
    TRoomDialogData.Forest = (WorkRoom.sector_type == SECT_FOREST);
    TRoomDialogData.Hills = (WorkRoom.sector_type == SECT_HILLS);
    TRoomDialogData.Mountain = (WorkRoom.sector_type == SECT_MOUNTAIN);
    TRoomDialogData.WaterSwim = (WorkRoom.sector_type == SECT_WATER_SWIM);
    TRoomDialogData.WaterBoat = (WorkRoom.sector_type == SECT_WATER_NOSWIM);
    TRoomDialogData.Underwater = (WorkRoom.sector_type == SECT_UNDERWATER);
    TRoomDialogData.Air = (WorkRoom.sector_type == SECT_AIR);
    TRoomDialogData.Desert = (WorkRoom.sector_type == SECT_DESERT);

    TRoomDialogData.Dark = ((WorkRoom.room_flags & ROOM_DARK) == ROOM_DARK);
    TRoomDialogData.NoMob = ((WorkRoom.room_flags & ROOM_NO_MOB) == ROOM_NO_MOB);
    TRoomDialogData.Indoors = ((WorkRoom.room_flags & ROOM_INDOORS) == ROOM_INDOORS);
    TRoomDialogData.Private = ((WorkRoom.room_flags & ROOM_PRIVATE) == ROOM_PRIVATE);
    TRoomDialogData.Safe = ((WorkRoom.room_flags & ROOM_SAFE) == ROOM_SAFE);
    TRoomDialogData.Solitary = ((WorkRoom.room_flags & ROOM_SOLITARY) == ROOM_SOLITARY);
//    TRoomDialogData.PetShop = ((WorkRoom.room_flags & ROOM_PET_SHOP) == ROOM_PET_SHOP);
    TRoomDialogData.NoRecall = ((WorkRoom.room_flags & ROOM_NO_RECALL) == ROOM_NO_RECALL);
    TRoomDialogData.Peaceful = ((WorkRoom.room_flags & ROOM_PEACEFUL) == ROOM_PEACEFUL);
    TRoomDialogData.RFlag32768 = ((WorkRoom.room_flags & 32768L) == 32768L);
    TRoomDialogData.RFlag65536 = ((WorkRoom.room_flags & 65536L) == 65536L);
    TRoomDialogData.RFlag131072 = ((WorkRoom.room_flags & 131072L) == 131072L);
    TRoomDialogData.RFlag262144 = ((WorkRoom.room_flags & 262144L) == 262144L);

    wsprintf(TRoomDialogData.VNum, "%d", WorkRoom.vnum);

    TRoomDialogData.ExtraDescrList.Clear();
    BOOL f = TRUE;
    for (TArrayAsVectorIterator<TExtraDescr*> it(WorkRoom.extra_descr); it; it++)
       {
       TRoomDialogData.ExtraDescrList.AddString(it.Current()->keyword.c_str(),
          f);
       f = FALSE;
       }

    char szBuffer[512];
    TRoomDialogData.MobList.Clear();
    f = TRUE;
//    for (TArrayAsVectorIterator<TMobReset*> it2(WorkRoom.MobResets); it2; it2++)
    // @@@ Due to some weirdness, we have to read from the actual room here, so that
    // we have pointers to the actual resets instead of copies...  Dunno how to fix
    // this
    for (TArrayAsVectorIterator<TMobReset*> it2(TheActualRoom->MobResets); it2; it2++)
       {
       vnum_int vnum = it2.Current()->arg1;
       const TMobile *mob = AreaDoc->GetMobile(vnum);
       if (mob)
          mob->GetName(szBuffer, sizeof szBuffer);
       else
          wsprintf(szBuffer, "#%d", vnum);

//       TRoomDialogData.MobList.AddStringItem(szBuffer, vnum, f);
       TRoomDialogData.MobList.AddStringItem(szBuffer,
          (DWORD) it2.Current(), f);
       f = FALSE;
       }

    TRoomDialogData.ObjList.Clear();
    f = TRUE;
    for (TArrayAsVectorIterator<TReset*> it3(WorkRoom.ObjResets); it3; it3++)
       {
       vnum_int vnum = it3.Current()->arg1;
       const TObject *obj = AreaDoc->GetObject(vnum);
       if (obj)
          obj->GetName(szBuffer, sizeof szBuffer);
       else
          wsprintf(szBuffer, "#%d", vnum);

       TRoomDialogData.ObjList.AddStringItem(szBuffer, vnum, f);
       f = FALSE;
       }

    if (WorkRoom.RandomDoor)
       wsprintf(TRoomDialogData.RandomDoors, "%d", WorkRoom.RandomDoor->arg2);
    else
       TRoomDialogData.RandomDoors[0] = '\0';
}

void TRoomDialog::SaveTransfer()
{
    static char *szAction[] = { " Open", " Close", " Lock" };
    static char *szDir[] = { " North", " East", " South", " West",
       " Up", " Down" };

    // Get the information from the transfer buffer into the working room copy
    WorkRoom.name = TRoomDialogData.Name;
    WorkRoom.description = TRoomDialogData.Description;

    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Dark, ROOM_DARK);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.NoMob, ROOM_NO_MOB);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Indoors, ROOM_INDOORS);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Private, ROOM_PRIVATE);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Safe, ROOM_SAFE);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Solitary, ROOM_SOLITARY);
//    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.PetShop, ROOM_PET_SHOP);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.NoRecall, ROOM_NO_RECALL);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.Peaceful, ROOM_PEACEFUL);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.RFlag32768, 32768L);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.RFlag65536, 65536L);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.RFlag131072, 131072L);
    SET_RESET_BIT(WorkRoom.room_flags, TRoomDialogData.RFlag262144, 262144L);

    // Save any changes to the door flags
    CheckDoorFlags();

    // Save any changes to the door resets
    for (int i = 0; i < nelems(WorkRoom.exit); i++)
		 {
       BOOL fHadAReset = (TheActualRoom->is_exit[i] &&
          TheActualRoom->exit[i].nInitialPosition != 3);
       BOOL fNeedsAReset  = (WorkRoom.is_exit[i] &&
          WorkRoom.exit[i].nInitialPosition != 3);

       if (fHadAReset)
          {
          int n = AreaDoc->GetIndexFromReset(
             TheActualRoom->exit[i].InitialPosition);
          if (!fNeedsAReset)
             {
#if 0
             // @@@
             if (n >= 0)
                AreaDoc->DeleteItem(5, n);	// Delete the reset
#else
				 MessageBox("Sorry, deleting resets is out of order",
                "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
             WorkRoom.exit[i].nInitialPosition =
                WorkRoom.exit[i].InitialPosition->arg3;
#endif
             }
          else
             {
             TReset *res = new TReset(*WorkRoom.exit[i].InitialPosition);
//             TReset *res = new TReset(AreaDoc);
//             res->command = 'D';
//             res->arg2 = i;
             res->arg3 = (sh_int) WorkRoom.exit[i].nInitialPosition;
             res->comment = "; " + WorkRoom.name + szAction[res->arg3] + szDir[i];
//             res->comment = "@@@Give this reset a comment";
             if (n >= 0)
                AreaDoc->SaveReset(*res, n); // Modify the reset
             delete res;
             }
          }
       else if (fNeedsAReset)
          {
          TReset *res = new TReset(AreaDoc);
          res->command = 'D';
          res->arg1 = WorkRoom.vnum;
          res->arg2 = (sh_int) i;
			 res->arg3 = (sh_int) WorkRoom.exit[i].nInitialPosition;
			 res->comment = "; " + WorkRoom.name + szAction[res->arg3] + szDir[i];
			 // Create the reset
			 WorkRoom.exit[i].InitialPosition = AreaDoc->SaveReset(*res, -1);
			 delete res;
			 }
		 }

	 TReset *res = 0;
	 int nNdx;

	 // Save the room randomization
	 int nRoomRandomization = atoi(TRoomDialogData.RandomDoors);
	 if (nRoomRandomization == 0)
		 {
		 if (WorkRoom.RandomDoor)
			 // @@@
			 MessageBox("Sorry, deleting exit randomization is out of order",
				 "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
		 }
	 else if (nRoomRandomization > 5)
		 MessageBox("Exit randomization limit is 5 (all exits)",
				 "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
	 else
		 {
		 // Do we need a new random door?
		 if (!WorkRoom.RandomDoor)
			 {
			 res = new TReset(AreaDoc);
			 res->command = 'R';
			 res->arg1 = WorkRoom.vnum;
			 res->comment = "; Randomize " + WorkRoom.name;
			 nNdx = -1;
			 }
		 // Change in the randomization?
		 else if (WorkRoom.RandomDoor->arg2 != nRoomRandomization)
			 {
			 res = new TReset(*WorkRoom.RandomDoor);
			 nNdx = AreaDoc->GetIndexFromReset(WorkRoom.RandomDoor);
			 }
		 }

	 // Save the working room to database, keep TheActualRoom in case it
	 // changes
	 TheActualRoom = WorkRoom.AreaDoc->RoomSave(WorkRoom, nIndex);

	 // We need to write the random door *after* the room so that it has the
	 // correct room name in the list of reset arguments on the list box.
	 if (res)
		 {
		 res->arg2 = (sh_int) nRoomRandomization;
		 // @@@ CAST AWAY CONST
		 ((TRoom*) TheActualRoom)->RandomDoor = AreaDoc->SaveReset(*res, nNdx);
		 delete res;
		 }

	 // Update any modified other sides.  WorkRoom is used as a temporary
	 // @@@ Add support for changing the other side in another zone...
	 for (i = 0; i < nelems(ModifiedRooms); i++)
		 {
		 if (ModifiedRooms[i])
			 {
			 int nNdx = AreaDoc->GetIndexFromRoom(ModifiedRooms[i]);
			 WorkRoom = *ModifiedRooms[i];
			 if (fDeletedOtherSide[i])
				 {
				 if (WorkRoom.exit[TAreaDocument::rev_dir[i]].InitialPosition)
					 MessageBox("Warning:  A 'D' door reset is not being deleted",
						 "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
				 WorkRoom.is_exit[TAreaDocument::rev_dir[i]] = FALSE;
				 WorkRoom.AreaDoc->RoomSave(WorkRoom, nNdx);
				 fDeletedOtherSide[i] = 0;
				 }
			 else
				 {
				 if (ModifiedOtherSides[i].InitialPosition)
					 MessageBox("Warning:  A 'D' door reset is not being deleted",
						 "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
				 WorkRoom.exit[TAreaDocument::rev_dir[i]] = ModifiedOtherSides[i];
				 WorkRoom.is_exit[TAreaDocument::rev_dir[i]] = TRUE;
				 WorkRoom.AreaDoc->RoomSave(WorkRoom, nNdx);
				 }

			 ModifiedRooms[i] = 0;
			 }
		 }
}


void TRoomDialog::CmSectorTypeClicked ()
{
	 // INSERT>> Your code here.
	 if (Inside->GetCheck() == BF_CHECKED) 			WorkRoom.sector_type = SECT_INSIDE;
	 else if (CityStreets->GetCheck() == BF_CHECKED) WorkRoom.sector_type = SECT_CITY;
	 else if (Field->GetCheck() == BF_CHECKED) 		WorkRoom.sector_type = SECT_FIELD;
	 else if (Forest->GetCheck() == BF_CHECKED) 		WorkRoom.sector_type = SECT_FOREST;
	 else if (Hills->GetCheck() == BF_CHECKED) 		WorkRoom.sector_type = SECT_HILLS;
	 else if (Mountain->GetCheck() == BF_CHECKED) 	WorkRoom.sector_type = SECT_MOUNTAIN;
	 else if (WaterSwim->GetCheck() == BF_CHECKED) 	WorkRoom.sector_type = SECT_WATER_SWIM;
    else if (WaterBoat->GetCheck() == BF_CHECKED) 	WorkRoom.sector_type = SECT_WATER_NOSWIM;
    else if (Underwater->GetCheck() == BF_CHECKED) WorkRoom.sector_type = SECT_UNDERWATER;
    else if (Air->GetCheck() == BF_CHECKED) 			WorkRoom.sector_type = SECT_AIR;
    else if (Desert->GetCheck() == BF_CHECKED) 		WorkRoom.sector_type = SECT_DESERT;

    SectorType->SendMessage(STM_SETICON,
       (WPARAM) hSectorIcons[WorkRoom.sector_type], 0);
}


void TRoomDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.
    char szBuffer[32];
    GetPrivateProfileString("MAKE.ZONES.FAST", "RFlag32768", "32768", szBuffer,
       sizeof szBuffer, "mzf.ini");
    RFlag32768->SetCaption(szBuffer);

    GetPrivateProfileString("MAKE.ZONES.FAST", "RFlag65536", "65536", szBuffer,
       sizeof szBuffer, "mzf.ini");
    RFlag65536->SetCaption(szBuffer);

    GetPrivateProfileString("MAKE.ZONES.FAST", "RFlag131072", "131072", szBuffer,
       sizeof szBuffer, "mzf.ini");
    RFlag131072->SetCaption(szBuffer);

    GetPrivateProfileString("MAKE.ZONES.FAST", "RFlag262144", "262144", szBuffer,
       sizeof szBuffer, "mzf.ini");
    RFlag262144->SetCaption(szBuffer);

    Description->SendMessage(WM_SETFONT,
       (WPARAM) (HFONT) AreaDoc->GetMZF()->hFixedFont, 0);

    // Display the current sector type
    CmSectorTypeClicked();

    // Set the door exits,
    EnableExits();

    // Enable Envy-only sector type, set ROM flag
    BOOL fEnvy = (AreaDoc->GetMode() == poundsApp::EnvyMode || AreaDoc->GetMode() == poundsApp::Envy2Mode);
    Underwater->EnableWindow(fEnvy);
    if (fEnvy)
      Peaceful->SetWindowText("Silence");

    // Enable ROM-only special dialog, and set ROM flag
    BOOL fRom = (AreaDoc->GetMode() == poundsApp::RomMode || AreaDoc->GetMode() == poundsApp::Rom24Mode);
    if (fRom)
      Peaceful->SetWindowText("Imp Only");
    Code->EnableWindow(fRom);

    // Don't let the player add mob resets during initial creation of this room
    AddMob->EnableWindow(nIndex != -1);
    AddObj->EnableWindow(nIndex != -1);

    // Check to see if the 'objects' list is hilighting a container
    CmIsContainer();
}

void TRoomDialog::EnableExits()
{
   // Find a valid exit
   if (nCurrExit < 0 || !fCreateMode && !WorkRoom.is_exit[nCurrExit])
      for (int i = 0; i < nelems(Exit); i++)
         if (WorkRoom.is_exit[i])
            {
            nCurrExit = i;
            break;
            }

   // Set the dialog to display the current exit and exit choices
   for (int i = 0; i < nelems(Exit); i++)
      {
      Exit[i]->SetCheck((i == nCurrExit) ? BF_CHECKED : BF_UNCHECKED);
      Exit[i]->EnableWindow(fCreateMode || WorkRoom.is_exit[i]);
      }

   // Call view an exit to display the new exit, tricking CheckDoorFlags into
   // not checking the door flags because it should only set them...
   int nNewExit = nCurrExit;
   nCurrExit = -1;
   ViewAnExit(nNewExit);
}

void TRoomDialog::CheckDoorFlags()
{
    if (nCurrExit >= 0 && WorkRoom.is_exit[nCurrExit])
       {
       // Get the new exit_info exit flags
       sh_int exit_info = WorkRoom.exit[nCurrExit].exit_info;
       SET_RESET_BIT(WorkRoom.exit[nCurrExit].exit_info, IsDoor->GetCheck() == BF_CHECKED,
          EX_ISDOOR);
       SET_RESET_BIT(WorkRoom.exit[nCurrExit].exit_info, IsPickProof->GetCheck() == BF_CHECKED,
          EX_PICKPROOF);
       if (AreaDoc->GetMode() == poundsApp::EnvyMode || AreaDoc->GetMode() == poundsApp::Envy2Mode)
          {
          SET_RESET_BIT(WorkRoom.exit[nCurrExit].exit_info, IsBashProof->GetCheck() == BF_CHECKED,
             EX_BASHPROOF);
          SET_RESET_BIT(WorkRoom.exit[nCurrExit].exit_info, IsPassProof->GetCheck() == BF_CHECKED,
             EX_PASSPROOF);
          }

       // Change door back to this room?
       if (exit_info != WorkRoom.exit[nCurrExit].exit_info)
          {
          // Do we already have a cached reverse exit?
          if (ModifiedRooms[nCurrExit])
             {
             if (MessageBox("Modify reverse exit?", "MAKE.ZONES.FAST",
                      MB_ICONQUESTION|MB_YESNO) == IDYES)
                // No point in checking for fDeletedOtherSide, so we don't
                ModifiedOtherSides[nCurrExit].exit_info =
                   WorkRoom.exit[nCurrExit].exit_info;
             }
          else
             {
             TRoom *To = WorkRoom.exit[nCurrExit].to_room;
             if (To)
                {
                TExit *RevExit = (To->is_exit[TAreaDocument::rev_dir[nCurrExit]]) ?
                   &To->exit[TAreaDocument::rev_dir[nCurrExit]] : 0;
                if (RevExit && RevExit->vnum == WorkRoom.vnum &&
                   RevExit->exit_info != WorkRoom.exit[nCurrExit].exit_info &&
                   MessageBox("Modify reverse exit?", "MAKE.ZONES.FAST",
                      MB_ICONQUESTION|MB_YESNO) == IDYES)
                   {
                   ModifiedRooms[nCurrExit] = To;
                   ModifiedOtherSides[nCurrExit] = *RevExit;
                   ModifiedOtherSides[nCurrExit].exit_info =
                      WorkRoom.exit[nCurrExit].exit_info;
                   }
                }
             }
          }
       }
}

void TRoomDialog::ViewAnExit(int n)
{
    CheckDoorFlags();
    nCurrExit = n;

    BOOL fEnvy = (AreaDoc->GetMode() == poundsApp::EnvyMode || AreaDoc->GetMode() == poundsApp::Envy2Mode);
    BOOL fRom = (AreaDoc->GetMode() == poundsApp::RomMode || AreaDoc->GetMode() == poundsApp::Rom24Mode);

    if (n >= 0 && WorkRoom.is_exit[n])
       {
       char szBuffer[256];

       ToPrompt->EnableWindow(TRUE);
//       ToText->EnableWindow(TRUE);
       KeyPrompt->EnableWindow(TRUE);
//       KeyText->EnableWindow(TRUE);
       WordPrompt->EnableWindow(TRUE);
//       WordText->EnableWindow(TRUE);
       ExitDescrPrompt->EnableWindow(TRUE);
//       ExitDescrText->EnableWindow(TRUE);
		 if (WorkRoom.exit[n].to_room)
//          ToText->SetText(WorkRoom.exit[n].to_room->name.c_str());
          WorkRoom.exit[n].to_room->GetName(szBuffer, sizeof szBuffer);
       else
          {
          wsprintf(szBuffer, "#%d", WorkRoom.exit[n].vnum);
          }
       ToText->SetText(szBuffer);

       const TObject *Key;
       if (WorkRoom.exit[n].key == -1)
          szBuffer[0] = '\0';
       else if ((Key = AreaDoc->GetObject(WorkRoom.exit[n].key)) == 0)
          wsprintf(szBuffer, "#%d", WorkRoom.exit[n].key);
       else
          Key->GetName(szBuffer, sizeof szBuffer);
       KeyText->SetText(szBuffer);

       IsDoor->EnableWindow(TRUE);
       IsDoor->SetCheck(((WorkRoom.exit[n].exit_info & EX_ISDOOR) ==
          EX_ISDOOR) ? BF_CHECKED : BF_UNCHECKED);
       IsPickProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);
       IsPickProof->SetCheck(((WorkRoom.exit[n].exit_info & EX_PICKPROOF) ==
          EX_PICKPROOF) ? BF_CHECKED : BF_UNCHECKED);
       if (fEnvy){
          IsBashProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);
          IsBashProof->SetCheck(((WorkRoom.exit[n].exit_info & EX_BASHPROOF) ==
             EX_BASHPROOF) ? BF_CHECKED : BF_UNCHECKED);
       }
       if (fEnvy || fRom) {
          IsPassProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);
          IsPassProof->SetCheck(((WorkRoom.exit[n].exit_info & EX_PASSPROOF) ==
             EX_PASSPROOF) ? BF_CHECKED : BF_UNCHECKED);
       }

       WordText->SetText(WorkRoom.exit[n].keyword.c_str());
       ExitDescrText->SetText(WorkRoom.exit[n].description.c_str());

       WalkCreate->EnableWindow(WorkRoom.exit[nCurrExit].to_room != 0);
       WalkCreate->SetWindowText("&Go");
       DelExit->EnableWindow(TRUE);

//       TReset *reset = WorkRoom.exit[nCurrExit].InitialPosition;
       DoorStatus->SendMessage(STM_SETICON,
          (WPARAM) hDoorIcons[WorkRoom.exit[nCurrExit].nInitialPosition], 0);
//          (WPARAM) hDoorIcons[(reset) ? reset->arg3 : 3], 0);
       }
    else
       {
       ToPrompt->EnableWindow(FALSE);
//       ToText->EnableWindow(FALSE);
       KeyPrompt->EnableWindow(FALSE);
//       KeyText->EnableWindow(FALSE);
       WordPrompt->EnableWindow(FALSE);
//       WordText->EnableWindow(FALSE);
       ExitDescrPrompt->EnableWindow(FALSE);
//       ExitDescrText->EnableWindow(FALSE);
       ToText->SetText("");
       KeyText->SetText("");
       WordText->SetText("");
       ExitDescrText->SetText("");

       IsDoor->EnableWindow(fCreateMode);
       IsDoor->SetCheck(BF_UNCHECKED);
       IsPickProof->EnableWindow(fCreateMode &&
                                 IsDoor->GetCheck() == BF_CHECKED);
       IsPickProof->SetCheck(BF_UNCHECKED);
       if (fEnvy) {
          IsBashProof->EnableWindow(fCreateMode &&
                                 IsDoor->GetCheck() == BF_CHECKED);
          IsBashProof->SetCheck(BF_UNCHECKED);
       }

       if (fEnvy || fRom) {
          IsPassProof->EnableWindow(fCreateMode &&
                                 IsDoor->GetCheck() == BF_CHECKED);
          IsPassProof->SetCheck(BF_UNCHECKED);
       }

       WalkCreate->EnableWindow(fCreateMode);
       // @@@ This doesn't seem to work...
//       WalkCreate->SendMessage(WM_SETTEXT, 0,
///          (WPARAM) ((fCreateMode) ? "&Create" : "&Go"));
       WalkCreate->SetWindowText((fCreateMode) ? "&Make" : "&Go");
       DelExit->EnableWindow(FALSE);

       DoorStatus->SendMessage(STM_SETICON, (WPARAM) hDoorIcons[3], 0);
       }

    SetExit->EnableWindow(nCurrExit >= 0);
}


void TRoomDialog::CmNorth ()
{
    // INSERT>> Your code here.
    ViewAnExit(0);
}


void TRoomDialog::CmEast ()
{
    // INSERT>> Your code here.
    ViewAnExit(1);
}


void TRoomDialog::CmSouth ()
{
    // INSERT>> Your code here.
    ViewAnExit(2);
}


void TRoomDialog::CmWest ()
{
    // INSERT>> Your code here.
    ViewAnExit(3);
}


void TRoomDialog::CmUp ()
{
    // INSERT>> Your code here.
    ViewAnExit(4);
}


void TRoomDialog::CmDown ()
{
    // INSERT>> Your code here.
    ViewAnExit(5);
}


void TRoomDialog::CmEditExtra ()
{
    // INSERT>> Your code here.
    int n = ExtraDescrList->GetSelIndex();
    if (n >= 0)
       {
       if (TExtraDialog(this, WorkRoom.extra_descr[n], AreaDoc).Execute() == IDOK)
          {
          ExtraDescrList->DeleteString(n);
          ExtraDescrList->InsertString(WorkRoom.extra_descr[n]->keyword.c_str(),
             n);
          }
       }
    else
       MessageBeep(-1);
}


void TRoomDialog::CmAddExtra ()
{
    // INSERT>> Your code here.
    TExtraDescr *xtra = new TExtraDescr;
    if (TExtraDialog(this, xtra, AreaDoc).Execute() == IDOK)
       {
       WorkRoom.extra_descr.Add(xtra);
       ExtraDescrList->AddString(xtra->keyword.c_str());
       }
    else
       delete xtra;
}

void TRoomDialog::CmDelExit ()
{
    // INSERT>> Your code here.
    if (nCurrExit < 0 || !WorkRoom.is_exit[nCurrExit])
       {
       MessageBeep(-1);
       return;
       }

    if (fDeletedOtherSide[nCurrExit])
       {
       // @@@ Should be able too...
       MessageBox("Sorry, I can't delete the other side for this"
          " direction again.  Please do it by hand",
          "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
       return;
       }

    TRoom *To = (ModifiedRooms[nCurrExit]) ? ModifiedRooms[nCurrExit] :
                                             WorkRoom.exit[nCurrExit].to_room;
    if (To)
       {
       // @@@ This code is incorrect if the exit is in ModifiedRevExit...
       TExit *RevExit;
           // Do we already have a cached reverse exit?
       if (ModifiedRooms[nCurrExit] ||
           // Or, is there a reverse exit we can find
           (RevExit = (To->is_exit[TAreaDocument::rev_dir[nCurrExit]]) ?
               &To->exit[TAreaDocument::rev_dir[nCurrExit]] : 0) != 0 &&
           // And if so, does it point back here?
            RevExit->vnum == WorkRoom.vnum)
          {
             char szBuffer[512];
             To->GetName(szBuffer, sizeof szBuffer);
             char szText[1024];
             wsprintf(szText, "Delete reverse exit at %s?", szBuffer);
             int nDeleteRev = MessageBox(szText, "MAKE.ZONES.FAST",
                MB_YESNOCANCEL);
             if (nDeleteRev == IDCANCEL)
                return;
             if (nDeleteRev == IDYES)
                {
                ModifiedRooms[nCurrExit] = To;
                fDeletedOtherSide[nCurrExit] = TRUE;
                }
          }
       }

    WorkRoom.is_exit[nCurrExit] = FALSE;
    EnableExits();
}


void TRoomDialog::CmOk ()
{
    // INSERT>> Your code here.
  	 TransferData(tdGetData);
    SaveTransfer();
    TDialog::CmOk();
}


void TRoomDialog::CmWalkCreate ()
{
    // INSERT>> Your code here.
    if (nCurrExit < 0)
       {
       MessageBeep(-1);
       return;
       }

    if (WorkRoom.is_exit[nCurrExit])
       {
       const TRoom *r;
       if ((r = WorkRoom.exit[nCurrExit].to_room) == 0)
          {
          MessageBeep(-1);
          return;
          }

       TransferData(tdGetData);
       SaveTransfer();
       nIndex = AreaDoc->GetIndexFromRoom(r);
       LoadAnotherRoom(r);
       }
    else if (!fCreateMode)
       {
       MessageBeep(-1);
       return;
       }
    else
       {
       // Save this room, we are leaving it...
       TransferData(tdGetData);
       SaveTransfer();

       // Save the room for pointing the exit in the new room back to
       const TRoom *OldActualRoom = TheActualRoom;
//       int nOldVNum = WorkRoom.vnum;

       // Get the new exit_info exit flags
       sh_int exit_info = 0;
       SET_RESET_BIT(exit_info, IsDoor->GetCheck() == BF_CHECKED,
          EX_ISDOOR);
       SET_RESET_BIT(exit_info, IsPickProof->GetCheck() == BF_CHECKED,
          EX_PICKPROOF);
       if (AreaDoc->GetMode() == poundsApp::EnvyMode || AreaDoc->GetMode() == poundsApp::Envy2Mode)
          {
          SET_RESET_BIT(exit_info, IsBashProof->GetCheck() == BF_CHECKED,
             EX_BASHPROOF);
          SET_RESET_BIT(exit_info, IsPassProof->GetCheck() == BF_CHECKED,
             EX_PASSPROOF);
          }

       // Create a room...
       TRoom *r = AreaDoc->MakeNewRoom();
       nIndex = -1;									// Doesn't yet exist
       LoadAnotherRoom(r);

       // Set up so we create the exit to the new room on Ok for the new room
       int nRevDir = TAreaDocument::rev_dir[nCurrExit];
       // @@@ CAST AWAY CONST
       ModifiedRooms[nRevDir] = (TRoom *) OldActualRoom;

       // Construct the exit from the old room to the new room
       ModifiedOtherSides[nRevDir].vnum = WorkRoom.vnum;
       // @@@ CAST AWAY CONST
       ModifiedOtherSides[nRevDir].to_room = (TRoom *) TheActualRoom;

       ModifiedOtherSides[nRevDir].exit_info = exit_info;

       // Construct the exit from the new room to the old room
//       WorkRoom.exit[nRevDir].vnum = nOldVNum;
       WorkRoom.exit[nRevDir].vnum = OldActualRoom->vnum;
       // @@@ CAST AWAY CONST
       WorkRoom.exit[nRevDir].to_room = (TRoom *) OldActualRoom;
       WorkRoom.exit[nRevDir].exit_info = exit_info;
       WorkRoom.is_exit[nRevDir] = TRUE;

       Name->SetFocus();
       Name->SetSelection(0, -1);
       }
}


void TRoomDialog::CmDelExtra ()
{
    // INSERT>> Your code here.
    int n = ExtraDescrList->GetSelIndex();
    if (n >= 0)
       {
       ExtraDescrList->DeleteString(n);
       WorkRoom.extra_descr.Detach(n);  // @@@ Should we delete?
       }
}


void TRoomDialog::CmSetExit ()
{
    // INSERT>> Your code here.
    if (nCurrExit < 0)
       MessageBeep(-1);
    else
       {
       CheckDoorFlags();
       // @@@ CAST AWAY CONST
       if (TExitDialog(this, AreaDoc, &WorkRoom.exit[nCurrExit], nCurrExit,
            WorkRoom.vnum, ModifiedRooms, (TRoom *) TheActualRoom,
            ModifiedOtherSides).Execute() == IDOK)
          {
          WorkRoom.is_exit[nCurrExit] = TRUE;
          ViewAnExit(nCurrExit);
          }
       }
}


void TRoomDialog::CmIsDoorChanged ()
{
    // INSERT>> Your code here.
    IsPickProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);

    BOOL fEnvy = (AreaDoc->GetMode() == poundsApp::EnvyMode ||
                  AreaDoc->GetMode() == poundsApp::Envy2Mode);
    BOOL fRom = (AreaDoc->GetMode() == poundsApp::RomMode ||
                 AreaDoc->GetMode() == poundsApp::Rom24Mode);
    if (fEnvy || fRom) {
       IsPassProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);
       if (fEnvy)
          IsBashProof->EnableWindow(IsDoor->GetCheck() == BF_CHECKED);
    }
}


void TRoomDialog::CmCreateMode ()
{
    // INSERT>> Your code here.
    // If any door flags have been altered but not saved, save them
    CheckDoorFlags();

    // Set mode to room creation
	 fCreateMode = TRUE;
    EnableExits();
}


void TRoomDialog::CmWalkMode ()
{
    // INSERT>> Your code here.
    // If any door flags have been altered but not saved, save them
    CheckDoorFlags();

    // Set mode to room walkabout, to prevent accidental room creation
    fCreateMode = FALSE;
    EnableExits();
}


void TRoomDialog::CmCancel ()
{
    // INSERT>> Your code here.
    // Is this room in the database yet?  If not, delete it.
    if (nIndex == -1)
       delete TheActualRoom;

    TDialog::CmCancel();
}


void TRoomDialog::CmAddMobReset ()
{
    // INSERT>> Your code here.
    TMobReset* mres = AreaDoc->MakeNewMobReset();
    mres->arg3 = WorkRoom.vnum;
	 int nMobCount = WorkRoom.MobResets.GetItemsInContainer();
    TMobResetDialog(this, mres, FALSE, -1).Execute();
    delete mres;

    // Was a new mob added? (Note: checking the actual room, not WorkRoom!)
    int nNewCount = TheActualRoom->MobResets.GetItemsInContainer();
    if (nMobCount != nNewCount)
 	    {
       // Update the list box
       const TMobReset *mres = TheActualRoom->MobResets[nNewCount-1];
       const TMobile *mob = AreaDoc->GetMobile(mres->arg1);
       char szBuffer[512];
       if (mob)
          mob->GetName(szBuffer, sizeof szBuffer);
       else
          wsprintf(szBuffer, "#%d", mres->arg1);
       int n = MobList->AddString(szBuffer);
       if (n != LB_ERR)
          {
//          MobList->SetItemData(n, mres->arg1);
          MobList->SetItemData(n, (DWORD) mres);
          MobList->SetSelIndex(n);
          }

       // Keep the workroom's list current with the actual room
//       TMobReset *newMSet = new TMobReset(*mres);
//       WorkRoom.MobResets.Add(newMSet);
       // @@@ Cast-away const
       WorkRoom.MobResets.Add((TMobReset *) mres);
       }
}


void TRoomDialog::CmMobEquip ()
{
    // INSERT>> Your code here.
    int n = MobList->GetSelIndex();
    if (n == LB_ERR)
       return;

//    TMobReset* mres = WorkRoom.MobResets[n];
	 TMobReset* mres = (TMobReset *) MobList->GetItemData(n);
    int nNdx = AreaDoc->GetIndexFromMobReset(mres);
    if (nNdx != -1)
       TMobResetDialog(this, mres, FALSE, nNdx).Execute();
    else
       MessageBeep(-1);
}


void TRoomDialog::CmDelMobReset ()
{
    // INSERT>> Your code here.
    int n = MobList->GetSelIndex();
    if (n == LB_ERR)
       return;

    // @@@ How do I remove the actual mob reset?  (Probably when OK is pressed
    // go out and delete all those mobs that don't have a corresponding reset
    // in WorkRoom.  Nah, I'll do it now...
	 TMobReset* mres = (TMobReset *) MobList->GetItemData(n);
    char szBuffer[512], szMsg[512+32];
    mres->GetName(szBuffer, sizeof szBuffer);
    wsprintf(szMsg, "Delete %s?  Are you sure?", szBuffer);
    if (MessageBox(szMsg, "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_YESNO) == IDYES)
       {
       int nNdx = AreaDoc->GetIndexFromMobReset(mres);
       if (nNdx != -1)
          {
          AreaDoc->DeleteItem(6, nNdx);
          WorkRoom.MobResets.Detach(n);
          MobList->DeleteString(n);
          }
       else
          MessageBeep(-1);
       }
}

// @@@ Doesn't indicate level of object placed on the ground, no way to set...
void TRoomDialog::CmAddObj ()
{
    // INSERT>> Your code here.
    int32 nLoc;
    if (TObjectListDialog(this, AreaDoc, &nLoc).Execute() == IDOK
           && nLoc != CB_ERR)
       {
       // Get the name of the object
       char szBuffer[512];
       const TObject *obj = WorkRoom.AreaDoc->GetObject((sh_int) nLoc);
       obj->GetName(szBuffer, sizeof szBuffer);

       // Create a temporary reset for this obj, save it, then delete it
       TReset *r = new TReset(AreaDoc);
       r->command = 'O';
       r->arg1 = (sh_int) nLoc;
       r->arg2 = 100;		// @@@ NO LIMITED OBJECT SUPPORT
       r->arg3 = WorkRoom.vnum;
       r->comment = obj->short_descr + " to " + WorkRoom.name;
       WorkRoom.AreaDoc->SaveReset(*r, -1);
       delete r;

       // Get the new permanent reset out of the area database, add to room
       int nNewCount = AreaDoc->TheResets.GetItemsInContainer();
       // Update the list box
       const TReset *res = AreaDoc->TheResets[nNewCount-1];
       // @@@ CAST AWAY CONST
       WorkRoom.ObjResets.Add((TReset *) res);

		 // Add to the list box
       ObjList->SetSelIndex(ObjList->AddString(szBuffer));
       CmIsContainer();
       }
}


void TRoomDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROOM);
}


void TRoomDialog::CmObjectContents ()
{
    // INSERT>> Your code here.

    // We shouldn't get here if there isn't a container, but check anyway
    int n = ObjList->GetSelIndex();
    if (n < 0)
       {
       MessageBeep(-1);
       return;
       }

    vnum_int vnum = (vnum_int) ObjList->GetItemData(n);
    const TObject* obj = AreaDoc->GetObject(vnum);

    if (!obj || obj->item_type != ITEM_CONTAINER)
       {
       MessageBeep(-1);
       return;
       }

    // First, we need to check that only one of this kind of item exists
    int nCount = 0;
    for (TArrayAsVectorIterator<TReset *> itRes(AreaDoc->TheResets); itRes; itRes++)
       {
       TReset *pReset = itRes.Current();
       if (pReset->arg1 == vnum &&
           (pReset->command == 'G' || pReset->command == 'E' || pReset->command == 'O'))
          nCount++;
       }

    if (nCount != 1)
       {
       MessageBeep(-1);
       MessageBox("A requirement of the server is that there can only be one "
          "of a container object in the game, if that object is to have items "
          "reset into it.", "MAKE.ZONES.FAST", MB_ICONHAND|MB_OK);
       return;
       }

    TContainerDialog(this, AreaDoc, obj).Execute();
}

void TRoomDialog::CmIsContainer ()
{
   int n = ObjList->GetSelIndex();
   if (n < 0)
      {
      ObjectContents->EnableWindow(FALSE);
      return;
      }

   vnum_int vnum = (vnum_int) ObjList->GetItemData(n);
   const TObject* obj = AreaDoc->GetObject(vnum);

   ObjectContents->EnableWindow(obj && obj->item_type == ITEM_CONTAINER);
}

void TRoomDialog::CmRomProps ()
{
    // No need to retrieve the control values, as nothing on this dialog
    // is needed for the temp dialog.

    TRoom TempRoom(WorkRoom);
    if (TRomRoomDialog(this, &TempRoom).Execute() == IDOK) {
        WorkRoom = TempRoom;

        // There is no need to update the controls as nothing on the Object
        // dialog affects this dialog.
    }
}

////////////////////////////////////////////////////////////////////////////
// TRomRoomDialog
////////////////////////////////////////////////////////////////////////////

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TRomRoomDialog, TDialog)
//{{TRomRoomDialogRSP_TBL_BEGIN}}
    EV_BN_CLICKED(IDOK, CmOk),
    EV_BN_CLICKED(IDHELP, CmHelp),
//{{TRomRoomDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TRomRoomDialog Implementation}}


static TRomRoomDialogXfer far TRomRoomDialogData;

TRomRoomDialog::TRomRoomDialog (TWindow* parent, TRoom *pRoom,
        TModule* module):
    TDialog(parent, IDD_ROMROOM, module),

    // Members
    m_pRoom( pRoom )
{
//{{TRomRoomDialogXFER_USE}}
   Law      = new TCheckBox(this, IDC_LAW);
   Nowhere  = new TCheckBox(this, IDC_NOWHERE);
   HealRate = new TEdit(this, IDC_HEALRATE, sizeof TRomRoomDialogData.HealRate);
   ManaRate = new TEdit(this, IDC_MANARATE, sizeof TRomRoomDialogData.ManaRate);
   Owner    = new TEdit(this, IDC_OWNER,    sizeof TRomRoomDialogData.Owner);
   Clan     = new TEdit(this, IDC_CLAN,     sizeof TRomRoomDialogData.Clan);

    SetTransferBuffer(&TRomRoomDialogData);
//{{TRomRoomDialogXFER_USE_END}}

    // INSERT>> Your constructor code here.
   TRomRoomDialogData.Law = !!(m_pRoom->room_flags & ROOM_LAW);
   TRomRoomDialogData.Nowhere = !!(m_pRoom->room_flags & ROOM_NOWHERE);
   wsprintf(TRomRoomDialogData.HealRate, "%d", m_pRoom->heal_rate);
   wsprintf(TRomRoomDialogData.ManaRate, "%d", m_pRoom->mana_rate);
   strncpy(TRomRoomDialogData.Owner, m_pRoom->owner.c_str(), sizeof TRomRoomDialogData.Owner-1);
   strncpy(TRomRoomDialogData.Clan, m_pRoom->clan.c_str(), sizeof TRomRoomDialogData.Clan-1);
}


TRomRoomDialog::~TRomRoomDialog ()
{
    Destroy();

    // INSERT>> Your destructor code here.
}

void TRomRoomDialog::SetupWindow ()
{
    TDialog::SetupWindow();

    // INSERT>> Your code here.

    // Rom 2.3 doesn't have 'nowhere'
    Nowhere->EnableWindow(m_pRoom->AreaDoc->GetMode() == poundsApp::Rom24Mode);
}

void TRomRoomDialog::CmHelp ()
{
    // INSERT>> Your code here.
    poundsApp::HelpState = WinHelp(HelpFileName, HELP_CONTEXT, IDD_ROMROOM);
}

void TRomRoomDialog::CmOk ()
{
   // INSERT>> Your code here.
   TransferData(tdGetData);

   SET_RESET_BIT(m_pRoom->room_flags, TRomRoomDialogData.Law, ROOM_LAW);
   SET_RESET_BIT(m_pRoom->room_flags, TRomRoomDialogData.Nowhere, ROOM_NOWHERE);
   m_pRoom->heal_rate = (sh_int) atoi(TRomRoomDialogData.HealRate);
   m_pRoom->mana_rate = (sh_int) atoi(TRomRoomDialogData.ManaRate);
   m_pRoom->owner = TRomRoomDialogData.Owner;
   m_pRoom->clan = TRomRoomDialogData.Clan;

   TDialog::CmOk();
}

