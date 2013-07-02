/*  Project pounds

    Copyright © 1994-1996. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areadoc.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file (1 of 3) for implementation of TAreaDocument (TFileDocument).
*/

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  Rom Mud improvements copyright (C) Alander                             *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*  ROM 2.4 is copyright 1993-1996 Russ Taylor                              *
*      Russ Taylor (rtaylor@pacinfo.com)                                   *
*                                                                          *
* Russ told me "You're welcome to use the ROM formats and small (let's say *
* < 1000 lines total) amounts of code to write zone editors, with only one *
* restriction:  I can't free you from the Merc and Diku licenses."         *
*
* I haven't kept track of how many lines of ROM I used... it is probably a *
* lot, what with all of those tables from const.c and the db.c and db2.c   *
* stuff.--Slash                                                            *
***************************************************************************/

// Borland C++ includes
#include <classlib\file.h>

// C includes
#include <dir.h>

// MZF includes
#include "areadoc.h"
#include "roomdia.h"
#include "objdia.h"
#include "mobdia.h"
#include "msetdia.h"
#include "shopdia.h"
#include "helpdia.h"

poundsApp *TheMuditorApp;
//extern struct race_type race_table[];

/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void TAreaDocument::smash_tilde( string& str )
{
    for (int n = str.length()-1; n >= 0; n--)
       if (str[n] == '~')
          str[n] = '-';
}

/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void TAreaDocument::trim( string& str )
{
    for (int n = str.length()-1; n >= 0; n--)
       if (str[n] > ' ')
          break;
       else
          str.resize(n);
}

#if 0
TVNumObj *TAreaDocument::GetVNumIndex(int nWhich, vnum_int vnum)
{
   TArrayAsVector<TVNumObj *> *p;
   switch (nWhich)
      {
      case 0:
         p = &TheMobiles;
         break;
      case 1:
         p = &TheObjects;
         break;
      case 2:
         p = &TheRooms;
         break;
      default:
         bug("GetVNumIndex(): Bad nWhich");
      }

   for (TArrayAsVectorIterator<TVNumObj *> it(*p); it; it++)
      if (it.Current()->vnum == vnum)
         return it.Current();

   return 0;
}
#endif

const TMercObj *TAreaDocument::GetItem(int nWhich, int nIndex, char *lpOut,
   int nOutSize)
{
//   TArrayAsVector<TVNumObj *> *p;
   switch (nWhich)
      {
      case 0:
         return (nIndex >= TheMobiles.GetItemsInContainer()) ? 0 :
                                   TheMobiles[nIndex]->GetName(lpOut, nOutSize);
      case 1:
         return (nIndex >= TheObjects.GetItemsInContainer()) ? 0 :
                                   TheObjects[nIndex]->GetName(lpOut, nOutSize);
      case 2:
         return (nIndex >= TheRooms.GetItemsInContainer()) ? 0 :
                                   TheRooms[nIndex]->GetName(lpOut, nOutSize);
      case 3:
         return (nIndex >= TheHelps.GetItemsInContainer()) ? 0 :
                                   TheHelps[nIndex]->GetName(lpOut, nOutSize);
      case 4:
         return (nIndex >= TheShops.GetItemsInContainer()) ? 0 :
                                   TheShops[nIndex]->GetName(lpOut, nOutSize);
      case 5:
         return (nIndex >= TheResets.GetItemsInContainer()) ? 0 :
                                   TheResets[nIndex]->GetName(lpOut, nOutSize);
      case 6:
         return (nIndex >= TheMobResets.GetItemsInContainer()) ? 0 :
                                   TheMobResets[nIndex]->GetName(lpOut, nOutSize);
      }

//   bug("GetVNumIndex(): Bad nWhich");
   THROW( TXOwl("GetVNumIndex(): Bad nWhich") );

   // NOTREACHED
//   return 0;
}

TMobile* TAreaDocument::get_mob_index	( vnum_int vnum )
{
   if (!IsOpen() && !Open(ofRead|ofWrite))
      return 0;

   for (TArrayAsVectorIterator<TMobile *> it(TheMobiles); it; it++)
      if (it.Current()->vnum == vnum)
         return it.Current();

   return 0;
}

TObject* TAreaDocument::get_obj_index	( vnum_int vnum )
{
   if (!IsOpen() && !Open(ofRead|ofWrite))
      return 0;

   for (TArrayAsVectorIterator<TObject *> it(TheObjects); it; it++)
      if (it.Current()->vnum == vnum)
         return it.Current();

   return 0;
}

TRoom* TAreaDocument::get_room_index	( vnum_int vnum )
{
   if (!IsOpen() && !Open(ofRead|ofWrite))
      return 0;

   for (TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
      if (it.Current()->vnum == vnum)
         return it.Current();

   return 0;
}

void TAreaDocument::DeleteItem(int nWhich, int nIndex)
{
   ChangeNotice cn = { nWhich, nIndex };
   int nMobResetCount;

//   int nExitCount;

   // @@@ Note:  Certain views depend on data from other views.  For example,
   // Shops needs mobs, Resets needs mobs, objs and rooms, MobResets needs
   // mobs, objs, rooms and resets.
   switch (nWhich)
      {
      case 0: // Mobiles
         cn.pObj = TheMobiles[nIndex];
         // Check for mob resets, issue warning
         TheMobiles.Detach(nIndex);
         break;
      case 1: // Objects
         cn.pObj = TheObjects[nIndex];
         // @@@ Check for resets, issue warning
         TheObjects.Detach(nIndex);
         break;
      case 2: // Rooms
         cn.pObj = TheRooms[nIndex];

         {
         // Are any mobs reset in this room?
         nMobResetCount = 0;
         for (TArrayAsVectorIterator<TMobReset *> it2(TheMobResets); it2; it2++)
            // arg3 is the room index
            if (it2.Current()->arg3 == TheRooms[nIndex]->vnum)
               nMobResetCount++;
         }

         if (nMobResetCount)
            if (MessageBox(0, "Warning:  Editor will not automatically delete "
                  "mobiles reset to this room.  Continue?", "MAKE.ZONES.FAST",
                  MB_ICONQUESTION|MB_YESNO) == IDNO)
               return;

         {
         // Delete the exits other rooms have to this room.
//         nExitCount = 0;
         for (TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
            for (int i = 0; i < 6; i++)
               if (it.Current()->exit[i].vnum == TheRooms[nIndex]->vnum)
                  {
                  it.Current()->is_exit[i] = FALSE;
//                  nExitCount++;
                  }
         }

         TheRooms.Detach(nIndex);
         break;
      case 3: // Helps
         cn.pObj = TheHelps[nIndex];
         TheHelps.Detach(nIndex);
         break;
      case 4: // Shops
         cn.pObj = TheShops[nIndex];
         TheShops.Detach(nIndex);
         break;
      case 5: // Resets
#if 1
         MessageBox(0, "Sorry, Resets can't be deleted.", "MZF", MB_OK);
         return;
#else
         cn.pObj = TheResets[nIndex];
         TheResets.Detach(nIndex);

         // If the reset is duplicated in mob resets, get rid of it there too
         {
         for (TArrayAsVectorIterator<TMobReset *> it(TheMobResets); it; it++)
            if (it.Current() == cn.pObj)
               {
               TheMobResets.Detach(it.Current());
               break;
               }
         }
         break;
#endif
      case 6: // MobResets
#if 0
         MessageBox(0, "Sorry, Mob Resets can't be deleted.", "MZF", MB_OK);
         return;
#else
         {
         // Look up this reset
         PMobReset pMobReset = TheMobResets[nIndex];

         // Get rid of the mobile's wielded eq
         ChangeNotice cn1 = { 5 };
         for (TArrayAsVectorIterator<TReset*> itWields(pMobReset->Wields); itWields; itWields++)
            {
            cn1.nIndex = GetIndexFromReset(itWields.Current());
            cn1.pObj = itWields.Current();
            TheResets.Detach(itWields.Current());
            NotifyViews(vnAreaDelete, (long) &cn1);
            }

         // Get rid of the mobile's inventory
         for (TArrayAsVectorIterator<TReset*> itInv(pMobReset->Inventory); itInv; itInv++)
            {
            cn1.nIndex = GetIndexFromReset(itInv.Current());
            cn1.pObj = itInv.Current();
            TheResets.Detach(itInv.Current());
            NotifyViews(vnAreaDelete, (long) &cn1);
            }

         // Remove the mob reset from the TRoom that keeps it...
         PRoom pRoom = get_room_index ( pMobReset->arg3 );
         if (pRoom)
            pRoom->MobResets.Detach(pMobReset);

         // Get rid of the mobile itself
         cn.pObj = pMobReset;
         TheMobResets.Detach(nIndex);

         // If the mob reset is duplicated in resets, get rid of it there too
         cn1.nIndex = GetIndexFromReset(pMobReset);
         cn1.pObj = pMobReset;
         TheResets.Detach(pMobReset);
         NotifyViews(vnAreaDelete, (long) &cn1);
         }
         break;
#endif
      }

  SetDirty(TRUE);
  NotifyViews(vnAreaDelete, (long) &cn);
}

void TAreaDocument::EditItem(int nWhich, int nIndex)
{
   TObject *obj;
   TMobile *mob;
   TRoom *room;
   TMobReset *mobreset;
   TShopData *shop;
   THelpData *help;

   // This prevents an app fault if a new document is created and then a view
   // to something without vnums is opened and 'add' is selected
   if (!IsOpen() && !Open(ofRead|ofWrite))
      return;

   switch (nWhich)
      {
      case 0:
         mob = (nIndex < 0) ? MakeNewMobile() : TheMobiles[nIndex];
         TMobDialog(GetDocManager().GetApplication()->GetMainWindow(),
            mob, nIndex).Execute();
         if (nIndex < 0)
            delete mob;
         break;
      case 1:
         obj = (nIndex < 0) ? MakeNewObject() : TheObjects[nIndex];
         TObjDialog(GetDocManager().GetApplication()->GetMainWindow(),
            obj, nIndex,
            const_cast<poundsApp*>(GetMZF())->GetLibrary()).Execute();
         if (nIndex < 0)
            delete obj;
         break;
      case 2:
         room = (nIndex < 0) ? MakeNewRoom() : TheRooms[nIndex];
         TRoomDialog(GetDocManager().GetApplication()->GetMainWindow(),
            room, nIndex).Execute();
//         if (nIndex < 0)
//            delete room;
         break;
      case 3:
         help = (nIndex < 0) ? MakeNewHelp() : TheHelps[nIndex];
         THelpDialog (GetDocManager().GetApplication()->GetMainWindow(), help,
            nIndex).Execute();
         if (nIndex < 0)
            delete help;
         break;
      case 4:
         shop = (nIndex < 0) ? MakeNewShop() : TheShops[nIndex];
//         if (nIndex < 0)
//            MessageBox(0, "Sorry, you can't make new shops yet...", "MZF", MB_OK);
//         else
            TShopDialog(GetDocManager().GetApplication()->GetMainWindow(),
               this, shop, nIndex, FALSE).Execute();
         if (nIndex < 0)
            delete shop;
         break;
      case 5:
         MessageBox(0, "Sorry, Reset editing is not ready.", "MZF", MB_OK);
         break;
      case 6:
         mobreset = (nIndex < 0) ? MakeNewMobReset() : TheMobResets[nIndex];
         TMobResetDialog(GetDocManager().GetApplication()->GetMainWindow(),
            mobreset, TRUE, nIndex).Execute();
         if (nIndex < 0)
            delete mobreset;
         break;
      }
}

// @@@ Need to add multiple file format support to this...
void TAreaDocument::CopyItem(int nWhich, int nIndex)
{
   if (nIndex < 0)
      return;

//   TObject *obj;
//   TMobile *mob;
//   TRoom *room;
//   TShopData *shop;

   HANDLE h = GlobalAlloc(GMEM_MOVEABLE, 32767);
   ostrstream buf((char *) GlobalLock(h), 32767, ios::out|ios::binary);
   TClipboard& clip = GetDocManager().GetApplication()->GetMainWindow()->OpenClipboard();
//   TClipboard clip;

   switch (nWhich)
      {
      case 0:
         WriteMobile(TheMobiles[nIndex], buf);
         break;
      case 1:
         WriteObject(TheObjects[nIndex], buf);
         break;
      case 2:
         WriteRoom(TheRooms[nIndex], buf);
         break;
      case 3:
         goto error; // @@@
      case 4:
         WriteShop(TheShops[nIndex], buf);
         break;
      case 5:
         WriteReset(TheResets[nIndex], buf);
         break;
      case 6:
         WriteMobReset(TheMobResets[nIndex], buf);
         break;
      }

//   clip.OpenClipboard(GetDocManager().GetApplication()->GetMainWindow()->HWindow);
   buf << '\0';
   if (clip.EmptyClipboard())
      clip.SetClipboardData(CF_TEXT, h);
   clip.CloseClipboard();
   return;

error:
   GlobalUnlock(h);
   GlobalFree(h);
}

const TMobile* TAreaDocument::GetMobileFromIndex(int nNdx) const
{
   return (nNdx >= TheMobiles.GetItemsInContainer()) ? 0 : TheMobiles[nNdx];
}

void TAreaDocument::MobileSave(const TMobile& mob, int nIndex)
{
   ChangeNotice cn = { 0, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);
      cn.nIndex = TheMobiles.GetItemsInContainer();
      TheMobiles.Add(new TMobile(mob));
      NotifyViews(vnAreaAppend, (long) &cn);
    	nDefaultAction = mob.act;
	   nDefaultAffectedBy = mob.affected_by;
      }
   else
      {
      if (!(*TheMobiles[nIndex] == mob))
         SetDirty(TRUE);
      *TheMobiles[nIndex] = mob;
      NotifyViews(vnAreaModify, (long) &cn);
      }
}

TMobile* TAreaDocument::MakeNewMobile()
{
   TMobile *mob = new TMobile(this);

	mob->player_name = "mob";
	mob->short_descr = "the mob";
	mob->long_descr = "the mob stands here, completely clueless.";
	mob->description = "the mob looks back at you!\r\n";
	mob->vnum = NextMobVnum();
	mob->sex = SEX_MALE;
	mob->level = 1;
	mob->act = nDefaultAction;
	mob->affected_by = nDefaultAffectedBy;
	mob->alignment = 0;

   return mob;
}

sh_int TAreaDocument::NextMobVnum()
{
   while (GetMobile(nNextMobVNum))
      nNextMobVNum++;

   return nNextMobVNum;
}

const TRoom* TAreaDocument::GetRoomFromIndex(int nNdx) const
{
   return (nNdx >= TheRooms.GetItemsInContainer()) ? 0 : TheRooms[nNdx];
}

int TAreaDocument::GetIndexFromRoom(const TRoom *r) const
{
   int nNdx = 0;
   for ( TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++, nNdx++)
      if (it.Current() == r)
         return nNdx;

   // If the room is not in the structure, return must be -1 for a created room.
//   // @@@Note cast away const...  Make this a static...
//   ((TAreaDocument*) this)->bug("TAreaDocument::GetIndexFromRoom(): Couldn't find that room...");

//   // NOTREACHED
   return -1;
}

// @@@ Make part of class...
static TRoom *BeingAdded;

TRoom *TAreaDocument::RoomSave(const TRoom& room, int nIndex)
{
   ChangeNotice cn = { 2, nIndex };

   if (nIndex < 0)
      {
      nDefaultSectorFlags = room.room_flags;
      nDefaultSectorType = room.sector_type;

      SetDirty(TRUE);
      cn.nIndex = TheRooms.GetItemsInContainer();
      *BeingAdded = room;
      TheRooms.Add(BeingAdded);
      NotifyViews(vnAreaAppend, (long) &cn);
      TRoom *r = BeingAdded;
      BeingAdded = 0;
      return r;
      }

   if (!(*TheRooms[nIndex] == room))
      SetDirty(TRUE);
   *TheRooms[nIndex] = room;
   NotifyViews(vnAreaModify, (long) &cn);
   return TheRooms[nIndex];
}

TRoom *TAreaDocument::MakeNewRoom()
{
   char szBuffer[256];

   TRoom *room = new TRoom(this);
   room->name = "A room";
   room->vnum = NextRoomVnum();
   wsprintf(szBuffer, "You are standing in room #%d.\r\n", room->vnum);
   room->description = szBuffer;
   room->room_flags = nDefaultSectorFlags;
   room->sector_type = nDefaultSectorType;

   BeingAdded = room;
   return room;
}

sh_int TAreaDocument::NextRoomVnum()
{
   while (GetRoom(nNextRoomVNum))
      nNextRoomVNum++;

   return nNextRoomVNum;
}

const TObject* TAreaDocument::GetObjectFromIndex(int nNdx) const
{
   return (nNdx >= TheObjects.GetItemsInContainer()) ? 0 : TheObjects[nNdx];
}

void TAreaDocument::ObjectSave(const TObject& obj, int nIndex)
{
   ChangeNotice cn = { 1, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);
      cn.nIndex = TheObjects.GetItemsInContainer();
      TheObjects.Add(new TObject(obj));
      NotifyViews(vnAreaAppend, (long) &cn);
      }
   else
      {
      if (!(*TheObjects[nIndex] == obj))
         {
         SetDirty(TRUE);
         *TheObjects[nIndex] = obj;
         }
      NotifyViews(vnAreaModify, (long) &cn);
      }
}

TObject* TAreaDocument::MakeNewObject()
{
   TObject *obj = new TObject(this);

   obj->name = "sword";
   obj->short_descr = "a sword";
   obj->description = "A sword rests here.";
   obj->vnum = NextObjVnum();
   obj->item_type = ITEM_WEAPON;
   obj->extra_flags = 0;
   obj->wear_flags = ITEM_TAKE|ITEM_WIELD;
   obj->weight = 5;        // sh_int in Merc, int in Envy

//   obj->value[0] = "0";
//   obj->value[1] = "1";
//   obj->value[2] = "6";
//   obj->value[3] = "11";

   return obj;
}

sh_int TAreaDocument::NextObjVnum()
{
   while (GetObject(nNextObjVNum))
      nNextObjVNum++;

   return nNextObjVNum;
}

poundsApp::currMode TAreaDocument::GetMode() const
{
    return MuditorApp->CurrentMode;
}

void TAreaDocument::MobilesToCombo(TComboBoxData& box, vnum_int vnum) const
{
   char szBuffer[1024];

   for ( TArrayAsVectorIterator<TMobile *> it(TheMobiles); it; it++)
      {
      it.Current()->GetName(szBuffer, sizeof szBuffer);
      box.AddStringItem(szBuffer, it.Current()->vnum,
         it.Current()->vnum  == vnum);
      }
}

void TAreaDocument::RoomsToCombo(TComboBoxData& box, vnum_int vnum) const
{
   char szBuffer[1024];

   for ( TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
      {
      it.Current()->GetName(szBuffer, sizeof szBuffer);
      box.AddStringItem(szBuffer, it.Current()->vnum,
         it.Current()->vnum  == vnum);
      }
}

void TAreaDocument::RoomsToList(TListBoxData& box, sh_int nMobVnum) const
{
   char szBuffer[1024];

   BOOL fFlag = TRUE;
   for ( TArrayAsVectorIterator<TMobReset *> it(TheMobResets); it; it++)
      if (it.Current()->arg1 == nMobVnum)
         {
         // @@@ CAST AWAY CONST
         const TRoom *pRoom = ((TAreaDocument*) this)->GetRoom(it.Current()->arg3);
         if (pRoom)
            pRoom->GetName(szBuffer, sizeof szBuffer);
         else
            wsprintf(szBuffer, "#%d", it.Current()->arg3);

         box.AddStringItem(szBuffer, (DWORD) it.Current(), fFlag);
         fFlag = FALSE;
         }
}

// Flags are considered to be ORs, so WEAR_WIELD|WEAR_ABOUT_BODY gives sword&robe
void TAreaDocument::ObjectsToComboWear(TComboBoxData& box, int32 wear_pos,
   vnum_int vnum)
{
   char szBuffer[1024];

   for ( TArrayAsVectorIterator<TObject *> it(TheObjects); it; it++)
      if (it.Current()->wear_flags & wear_pos)
         {
         it.Current()->GetName(szBuffer, sizeof szBuffer);
         box.AddStringItem(szBuffer, it.Current()->vnum,
            it.Current()->vnum  == vnum);
         }
}

void TAreaDocument::ObjectsToComboType(TComboBoxData& box, sh_int type,
   vnum_int vnum) const
{
   char szBuffer[1024];

   for ( TArrayAsVectorIterator<TObject *> it(TheObjects); it; it++)
      if (it.Current()->item_type == type)
         {
         it.Current()->GetName(szBuffer, sizeof szBuffer);
         box.AddStringItem(szBuffer, it.Current()->vnum,
            it.Current()->vnum  == vnum);
         }
}

void TAreaDocument::ObjectsToList(TListBoxData& box) const
{
   char szBuffer[1024];
   for ( TArrayAsVectorIterator<TObject *> it(TheObjects); it; it++)
      {
      it.Current()->GetName(szBuffer, sizeof szBuffer);
      box.AddStringItem(szBuffer, it.Current()->vnum);
      }
}

THelpData *TAreaDocument::MakeNewHelp()
{
   return new THelpData(this);
}

void TAreaDocument::HelpSave(const THelpData& help, int nIndex)
{
   ChangeNotice cn = { 3, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);
      cn.nIndex = TheHelps.GetItemsInContainer();
      TheHelps.Add(new THelpData(help));
      NotifyViews(vnAreaAppend, (long) &cn);
      }
   else
      {
      if (!(*TheHelps[nIndex] == help))
         {
         SetDirty(TRUE);
         *TheHelps[nIndex] = help;
         }
      NotifyViews(vnAreaModify, (long) &cn);
      }
}

TShopData *TAreaDocument::MakeNewShop()
{
   return new TShopData(this);
}

void TAreaDocument::ShopSave(const TShopData& shop, int nIndex)
{
   ChangeNotice cn = { 4, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);
      cn.nIndex = TheShops.GetItemsInContainer();
      TheShops.Add(new TShopData(shop));
      NotifyViews(vnAreaAppend, (long) &cn);
      }
   else
      {
      if (!(*TheShops[nIndex] == shop))
         {
         SetDirty(TRUE);
         *TheShops[nIndex] = shop;
         }
      NotifyViews(vnAreaModify, (long) &cn);
      }
}

int TAreaDocument::GetShopIndexFromMobileVNum(sh_int mobile)
{
   if (!IsOpen() && !Open(ofRead|ofWrite))
      return -1;

   int nNdx = 0;
   for (TArrayAsVectorIterator<TShopData *> it(TheShops); it; it++, nNdx++)
      if (it.Current()->keeper == mobile)
         return nNdx;

   return -1;
}

TMobReset *TAreaDocument::MakeNewMobReset()
{
   TMobReset *mobreset = new TMobReset(this);
//   mobreset->command = 'M';
   mobreset->arg2 = 1;
   return mobreset;
}

void TAreaDocument::SaveMobReset(const TMobReset& mset, int nIndex)
{
   ChangeNotice cn = { 6, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);

      // First, add a leading empty comment line
      TReset *comment = new TReset(mset.AreaDoc);
      SaveReset(*comment, -1);
      delete comment;
      
      cn.nIndex = TheMobResets.GetItemsInContainer();
      TMobReset *newMSet = new TMobReset(mset);

      TheMobResets.Add(newMSet);
      TRoom *room = get_room_index(mset.arg3);
      if (room)
         room->MobResets.Add(newMSet);
      NotifyViews(vnAreaAppend, (long) &cn);

      // Notify the #RESETs view
#if 1
      cn.nWhich = 5;
      cn.nIndex = TheResets.GetItemsInContainer();
      TheResets.Add(newMSet);
      NotifyViews(vnAreaAppend, (long) &cn);
      for (TArrayAsVectorIterator<TReset *> it(newMSet->Wields); it; it++)
         {
         cn.nIndex = TheResets.GetItemsInContainer();
         TheResets.Add(it.Current());
         NotifyViews(vnAreaAppend, (long) &cn);
         }
      for (TArrayAsVectorIterator<TReset *> it2(newMSet->Inventory); it2; it2++)
         {
         cn.nIndex = TheResets.GetItemsInContainer();
         TheResets.Add(it2.Current());
         NotifyViews(vnAreaAppend, (long) &cn);
         }
#else
      // @@@ This will app fault when saving, also doesn't display wields
      SaveReset(mset, nIndex);
#endif
      }
   else
      {
      if (!(*TheMobResets[nIndex] == mset))
         {
         SetDirty(TRUE);
         *TheMobResets[nIndex] = mset;
         }

      NotifyViews(vnAreaModify, (long) &cn);

      // @@@ Add code to update the resets view...
      }
}

const TMobReset* TAreaDocument::GetMobResetFromIndex(int nNdx) const
{
   return (nNdx >= TheMobResets.GetItemsInContainer()) ? 0 : TheMobResets[nNdx];
}

int TAreaDocument::GetIndexFromMobReset(const TMobReset *mres) const
{
   int nNdx = 0;
   for ( TArrayAsVectorIterator<TMobReset *> it(TheMobResets); it; it++, nNdx++)
      if (it.Current() == mres)
         return nNdx;

   return -1;
}

TReset *TAreaDocument::SaveReset(const TReset& rset, int nIndex)
{
   ChangeNotice cn = { 5, nIndex };

   if (nIndex < 0)
      {
      SetDirty(TRUE);
      cn.nIndex = TheResets.GetItemsInContainer();
      TReset *newRSet = new TReset(rset);
      TheResets.Add(newRSet);
      NotifyViews(vnAreaAppend, (long) &cn);
      return newRSet;
      }
   else
      {
      if (!(*TheResets[nIndex] == rset))
         {
         SetDirty(TRUE);
         *TheResets[nIndex] = rset;
         }

      NotifyViews(vnAreaModify, (long) &cn);
      }

   return TheResets[nIndex];
}

int TAreaDocument::GetIndexFromReset(const TReset *res)
{
   int nNdx = 0;
   for ( TArrayAsVectorIterator<TReset *> it(TheResets); it; it++, nNdx++)
      if (it.Current() == res)
         return nNdx;

   return -1;
}

BOOL TAreaDocument::Commit (BOOL force)
{
	 if (!IsDirty() && !force)
		 return TRUE;

    // @@@ This should be ok from Open(), don't know...
    TheMuditorApp = MuditorApp =
       TYPESAFE_DOWNCAST(GetDocManager().GetApplication(), poundsApp);

    // @@@
    if (MuditorApp->CurrentMode == poundsApp::RomMode) {
       GetDocManager().GetApplication()->GetMainWindow()->MessageBox(
          "Saving in ROM 2.3 mode is not ready.  ROM 2.3 mode is "
          "currently only for conversion to other file formats.",
          "M.Z.F.", MB_ICONHAND|MB_OK);
       return FALSE;
       }

//    if (MuditorApp->CurrentMode == poundsApp::Envy2Mode)
//       {
//       GetDocManager().GetApplication()->GetMainWindow()->MessageBox(
//          "Saving in Envy 2.0 mode is not ready.  Envy 2.0 mode is currently only "
//          "for conversion to other file formats.  Look for better Envy 2.0 support "
//          "in the next version", "M.Z.F.", MB_ICONHAND|MB_OK);
//       return TRUE;
//       }

    // We are about to create a new OutStream, so lets back up the old
    // stream
    char szOldName[MAXPATH];
    strcpy(szOldName, GetDocPath());
    char *p = strrchr(szOldName, '.');
    if (!p)
      p = szOldName + strlen(szOldName);
    strcpy(p, ".bak");
    TFile::Remove(szOldName);
    TFile::Rename(GetDocPath(), szOldName);
//    if (rename(GetDocPath(), szOldName)) {
//      if (GetDocManager().GetApplication()->GetMainWindow()->MessageBox(
//            "Could not create .bak file.  Save anyway?",
//            "M.Z.F.", MB_ICONQUESTION|MB_YESNO) == IDNO)
//         return FALSE;
//    }

	 TPointer<TOutStream> os = OutStream(ofWrite|ofBinary);
	 if (!os)
		 return FALSE;

    if (MuditorApp->CurrentMode == poundsApp::OghamMode)
       WriteOgham(*os);
    else
       {
         WriteAreaData(*os);
//       if (!fUsesAreaData)
//    	    *os << "#AREA\t" << name << "~\r\n\r\n\r\n";
//       else
//          WriteAreaData(*os);

	    WriteHelps(*os);
       if (!fUsesAreaData && recall != -1 &&
                  (MuditorApp->CurrentMode == poundsApp::EnvyMode ||
                   MuditorApp->CurrentMode == poundsApp::Envy2Mode))
          *os << "#RECALL\r\n" << recall << "\r\n";
	    WriteMobs(*os);
       WriteMobProgs(*os);
	    WriteObjects(*os);
	    WriteRooms(*os);
	    WriteResets(*os);
	    WriteShops(*os);
	    WriteSpecials(*os);
	    *os << "#$\r\n";
       }

    BOOL status = os->good();
	 SetDirty(FALSE);
	 if (!status)
		PostError(IDS_WRITEERROR, MB_OK);
	 return status;
}

int TAreaDocument::exitinfo2locktype(int exitinfo)
{
   if ((exitinfo & EX_ISDOOR) != EX_ISDOOR)  // no door at all
      return 0;

   return (((exitinfo & EX_PICKPROOF) == EX_PICKPROOF) ? 1 : 0) +
          (((exitinfo & EX_BASHPROOF) == EX_BASHPROOF) ? 2 : 0) +
          (((exitinfo & EX_PASSPROOF) == EX_PASSPROOF) ? 4 : 0) + 1;
}

void TAreaDocument::WriteRooms(ostream& os)
{
	if (TheRooms.GetItemsInContainer() < 1)
      return;

   os << "#ROOMS\r\n";
   for (TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
      switch (MuditorApp->CurrentMode) {
      case poundsApp::Rom24Mode:
         WriteRomRoom(it.Current(), os);
         break;

      default:
         WriteRoom(it.Current(), os);
         break;
      }

	os << "#0\r\n\r\n\r\n";
}

void TAreaDocument::WriteHelps(ostream& os)
{
	if (TheHelps.GetItemsInContainer() < 1)
      return;

   os << "#HELPS\r\n";
   for (TArrayAsVectorIterator<THelpData *> it(TheHelps); it; it++)
      WriteHelp(it.Current(), os);

   os << "0 $~\r\n\r\n\r\n";
}

void TAreaDocument::WriteMobs(ostream& os)
{
	if (TheMobiles.GetItemsInContainer() < 1)
      return;

   os << "#MOBILES\r\n";
   for (TArrayAsVectorIterator<TMobile *> it(TheMobiles); it; it++)
      switch (MuditorApp->CurrentMode) {
      case poundsApp::Rom24Mode:
         WriteRom24Mobile(it.Current(), os);
         break;
      default:
         WriteMobile(it.Current(), os);
         break;
      }

   os << "#0\r\n\r\n\r\n";
}

void TAreaDocument::WriteMobProgs(ostream& /*os*/)
{
//   os << "#MOBPROGS\n";
   // @@@ Should check if it is ok to write...  Should write
//   os << "S\n\n\n";
}

void TAreaDocument::WriteFlags(ostream& os, int32 flags)
{
   // @@@ 32?
   DWORD dw = 1;
   BOOL fFirst = TRUE;
   for (int i = 0; i < 31 && flags; i++, flags >>= 1, dw <<= 1)
      {
      if (flags&1)
         {
         if (fFirst)
            fFirst = FALSE;
         else
            os << '|';
         os << dw;
         }
      }

   if (fFirst)
      os << '0';
}

void TAreaDocument::WriteObjects(ostream& os)
{
	if (TheObjects.GetItemsInContainer() < 1)
      return;

   os << "#OBJECTS\r\n";
   for (TArrayAsVectorIterator<TObject *> it(TheObjects); it; it++)
      switch (MuditorApp->CurrentMode) {
      case poundsApp::Rom24Mode:
         WriteRom24Object(it.Current(), os);
         break;

      default:
         WriteObject(it.Current(), os);
         break;
      }

   os << "#0\r\n\r\n\r\n";
}

void TAreaDocument::WriteResets(ostream& os)
{
	if (TheResets.GetItemsInContainer() < 1 &&
       TheMobResets.GetItemsInContainer() < 1)
      return;

	os << "#RESETS\r\n";
#if 0
   for (TArrayAsVectorIterator<TMobReset *> it2(TheMobResets); it2; it2++)
      WriteMobReset(it2.Current(), os);
#endif

   for (TArrayAsVectorIterator<TReset *> it(TheResets); it; it++)
      WriteReset(it.Current(), os);
//      {
//      const TReset *reset = it.Current();
//
//      os << reset->command;
//      switch (reset->command)
//         {
//         case '*':
//            os << reset->comment << '\n';
//            break;
//         case 'G':
//         case 'R':
//         case 'C':
//         case 'F':
//            os << " 1 ";
//            os << reset->arg1 << ' ' << reset->arg2 << "\t\t\t" <<
//                  reset->comment << '\n';
//            break;
//         default:
//            os << " 1 ";
//            os << reset->arg1 << ' ' << reset->arg2 << ' ' <<
//                  reset->arg3 << "\t\t\t" << reset->comment << '\n';
//            break;
//         }
//      }

   os << "S\r\n\r\n\r\n";
}

void TAreaDocument::WriteShops(ostream& os)
{
	if (TheShops.GetItemsInContainer() < 1)
      return;

   os << "#SHOPS\r\n";
	// Examine all shops.
   for (TArrayAsVectorIterator<TShopData *> it(TheShops); it; it++)
      WriteShop(it.Current(), os);
//      {
//      const TShopData *shop = it.Current();
//
//      os << shop->keeper << ' ';
//      for (int i = 0; i < MAX_TRADE; i++)
//         os << shop->buy_type[i] << ' ';
//      os << shop->profit_buy << ' ' << shop->profit_sell << ' ';
//      os << shop->open_hour << ' ' << shop->close_hour << ' ';
//      os << /*@@@shop->comment <<*/ '\n';
//      }
   os << "0\r\n\r\n\r\n";
}

void TAreaDocument::WriteSpecials(ostream& os)
{
   BOOL fFirst = TRUE;

   for (TArrayAsVectorIterator<TMobile *> it(TheMobiles); it; it++)
      {
      const TMobile *mob = it.Current();
      if (!mob->spec_fun.is_null())
         {
         if (fFirst)
            {
            os << "#SPECIALS\r\n";
            fFirst = FALSE;
            }

         os << "M  " << mob->vnum << ' ' << mob->spec_fun;
         if (mob->spec_comment.is_null())
            {
            if (!mob->short_descr.is_null())
               os << "\t\t\t" << mob->short_descr << "\r\n";
            }
         else
            {
            if (!mob->spec_comment.is_null() && mob->spec_comment[0] > ' ')
               os << "\t\t\t";
            os << mob->spec_comment << "\r\n";
            }
         }
      }

    if (!fFirst)
   	 os << "S\r\n\r\n\r\n";
}

/*
 * Troll-only
 *
 * #AREADATA
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * Weather <weather data>
 * Timezone 12
 * Flags 4096
 * End
 */
void TAreaDocument::WriteAreaData(ostream& os)
{
   // Troll header
   if (fUsesAreaData) {
      os << "#AREADATA\r\n";
      os << "Name " << name << "~\r\n";
      if (!area_data.repop.is_null())
         os << "Repop\t" << area_data.repop << "~\r\n";
      if (recall != -1)
         os << "#RECALL\r\n" << recall << "\r\n";
      if (area_data.fCustomWeather)
         os << "Weather\t" << area_data.Weather.MinTemp << ' ' <<
                         area_data.Weather.MaxTemp << ' ' <<
                         area_data.Weather.MinPrecip << ' ' <<
                         area_data.Weather.MaxPrecip << ' ' <<
                         area_data.Weather.MinWind << ' ' <<
                         area_data.Weather.MaxWind << ' ' <<
                         area_data.Weather.MinMagic << ' ' <<
                         area_data.Weather.MaxMagic << ' ' <<
                         area_data.Weather.MaxChange << "\r\n";
      os << "Timezone\t" << area_data.timezone << "\r\n";
      os << "Flags\t" << area_data.area_flags << "\r\n";
      os << "End\r\n\r\n";

   // ROM 2.4 header
   } else if (MuditorApp->CurrentMode == poundsApp::Rom24Mode) {
      os << "#AREA\r\n";
      char szName[MAXFILE], szExt[MAXEXT];
      fnsplit(GetDocPath(), 0, 0, szName, szExt);
      os << szName << szExt << "~\r\n";
//      os << "divsouls.are" << "~\r\n";
      os << shortname << "~\r\n";
      os << name << "~\r\n";

      // Here, we assume that zones are allocated in groups of 100, and
      // that the number of rooms is the best estimate.
      vnum_int nFirstVnum, nLastVnum;
      if (TheRooms.GetItemsInContainer()) {
         nFirstVnum = (vnum_int) (TheRooms[0]->vnum / 100 * 100);
         nLastVnum = (vnum_int) (TheRooms[TheRooms.GetItemsInContainer()-1]->vnum
            / 100 * 100 + 99);
      } else {
         nFirstVnum = nLastVnum = 0;
      }

      os << nFirstVnum << ' ' << nLastVnum << "\r\n\r\n";

   // Merc/Envy/TFC header
   } else {
 	   os << "#AREA\t" << name << "~\r\n\r\n\r\n";
   }
}

#pragma argsused
void TAreaDocument::WriteHelp(THelpData *help, ostream& os)
{
   os << help->level << ' ' << help->keyword << "~\r\n" <<
         help->text << "~\r\n\r\n";
}

void TAreaDocument::WriteMobile(TMobile *mob, ostream& os)
{
//   extern struct race_type race_table[];

   os << '#' << mob->vnum << "\r\n";
   os << mob->player_name << "~\r\n";
   os << mob->short_descr << "~\r\n";
   os << mob->long_descr << "\r\n~\r\n";    // (Add back on the crlf we stripped)
   trim(mob->description);
   os << mob->description << "\r\n~\r\n";

   // If we are writing Envy 2.0, then we may write out some AFF bits that
   // originally came from the mobiles race and not its AFF bits.  This
   // shouldn't cause any trouble, it is just redundent.
   WriteFlags(os, mob->act /* | race_table[mob->race].act*/);
   os << ' ';
   WriteFlags(os, mob->affected_by /*| race_table[mob->race].aff*/);

//      os << mob->act << ' ' << mob->affected_by << ' ' << mob->alignment <<
//          " S\r\n";
   os << ' ' << mob->alignment << " S\r\n";
   os << mob->level << ' ' << mob->hitroll << ' ' << mob->ac << ' ' <<
         mob->hitnodice << 'd' << mob->hitsizedice << '+' << mob->hitplus <<
         ' ' << mob->damnodice << 'd' << mob->damsizedice << '+' <<
         mob->damplus << "\r\n";
   os << mob->gold << ' ' << "0\r\n";

//   os << "8 8 "
   os << "8 ";
   if (MuditorApp->CurrentMode == poundsApp::Envy2Mode)
      os << mob->strrace << "~ ";
   else
      os << "8 ";

   // Here we de-sex any ROM 2.4 either so that other muds have something to read.
   if (mob->sex == SEX_EITHER)
      os << SEX_NEUTRAL;
   else
      os << mob->sex;
   os << "\r\n";

   if (MuditorApp->fMOBprograms)
      if (mob->mobprogs.GetItemsInContainer() > 0)
         {
         for (TArrayAsVectorIterator<TMobProgData*> it(mob->mobprogs); it; it++)
            {
            if (!it.Current()->MobFile.is_null())
               MessageBox(0, "Warning: mprog file being inlined", "M.Z.F.",
                  MB_ICONINFORMATION|MB_OK);
            os << '>' << it.Current()->type << ' ' << it.Current()->arglist <<
                  "~\r\n" << it.Current()->comlist << "~\r\n";
            }

         os << "|\r\n";
         }
#if 0
         {
         char szBuffer[256] = "Warning:  Stripping MOBprogs from ";
         char szMobName[256];
         mob->GetName(szMobName, sizeof szMobName);
         strcat(szBuffer, szMobName);
         MessageBox(0, szBuffer, "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
         }
#endif
}

#pragma argsused
void TAreaDocument::WriteMobProg(TMobProgData *mobprog, ostream& os)
{
   // @@@
}

void TAreaDocument::WriteObject(TObject *obj, ostream& os)
{
   // Write the object to the stream
   os << '#' << obj->vnum << "\r\n";
   os << obj->name << "~\r\n";
   os << obj->short_descr << "~\r\n";
   os << obj->description << "~\r\n";
   os << "~\r\n";

   // If we encounter a ROM object with an unknown type, we will write
   // it out as 'furniture' so that Merc/Envy can read at least
   // something.
   if (obj->item_type == 0)
      os << ITEM_FURNITURE << ' ';
   else
      os << obj->item_type << ' ';

   WriteFlags(os, obj->extra_flags);
   os << ' ';
   WriteFlags(os, obj->wear_flags);
   os << "\r\n";

   // @@@ Make GetMZF return something non-const, or make GetLibrary
   // a const function.
   TLibrary *pLibrary = const_cast<poundsApp*>(GetMZF())->GetLibrary();

   // How many objects for this mode?
//   int nMaxObjects =
//      (MuditorApp->CurrentMode == poundsApp::RomMode ||
//       MuditorApp->CurrentMode == poundsApp::Rom24Mode) ? 5 : 4;
   const int nMaxObjects = 4;
   BOOL fEnvy = (MuditorApp->CurrentMode == poundsApp::EnvyMode ||
          MuditorApp->CurrentMode == poundsApp::Envy2Mode);
   for (int i = 0; i < nMaxObjects; i++) {
      int nT = pLibrary->GetTypeInfo(obj->strType, i);
      char szValue[64];
      switch (nT) {
      case FLAG_TYPE:
         // Convert the flag
         {
         int32 n = TAreaDocument::Str2Flag(obj->value[i].c_str());
         TAreaDocument::Flags2Str(MuditorApp->CurrentMode, szValue, n);
         }
         break;

      case WORD_TYPE:
      default:
         // Convert the word
         {
         // If we don't have a number, we will force 0
         int32 n = atol(obj->value[i].c_str());
         wsprintf(szValue, "%ld", n);
         break;
         }

      case LIST_TYPE:
         // We need to convert strings back into numbers, with the exception
         // of Envy spells.  Envy values also have '~' after them.

         // Look up the list.  @@@ Problem:  We are looking it up in the new
         // database, not the original database for this codebase.
         {
         const char *pszPrompt = pLibrary->GetPromptText(obj->strType, i);
         // If we fail lookin
         int32 n = 0;
         if (fEnvy && strcmp(pszPrompt, "spell") == 0)
            strcpy(szValue, obj->value[i].c_str());
         else {
            // This is Envy, or if it is it is not a spell.  Convert to number.
            if (pLibrary->ListStr2Number(pszPrompt, obj->value[i].c_str(), &n)) {
               wsprintf(szValue, "%ld", n);
            } else
               strcpy(szValue, "0"); // We couldn't find the item...
         }
         }
      }

      os << szValue;
      if (fEnvy)
         os << '~';

//      if (fEnvy) {
//         switch ( obj->item_type ) {
//         default:
//            os << obj->value[i] << '~';
//            break;
//
//         case ITEM_PILL:
//         case ITEM_POTION:
//         case ITEM_SCROLL:
//            if (i == 1 || i == 2 || i == 3)
//               os << SpellName(atoi(obj->value[i].c_str()), TRUE) << '~';
//                  else
//                     os << obj->value[i] << '~';
//                  break;
//
//         case ITEM_STAFF:
//         case ITEM_WAND:
//                  if (i == 3)
//                     os << SpellName(atoi(obj->value[i].c_str()), TRUE) << '~';
//                  else
//                     os << obj->value[i] << '~';
//                  break;
//         } // switch (obj->item_type)
//      } else { // mode != envy
////         os << obj->value[i];
//         switch ( obj->item_type ) {
//         default:
//            os << obj->value[i];
//            break;
//
//         case ITEM_PILL:
//         case ITEM_POTION:
//         case ITEM_SCROLL:
//            if (i == 1 || i == 2 || i == 3)
//               os << SpellValue(obj->value[i].c_str());
//            else
//               os << obj->value[i];
//            break;
//
//         case ITEM_STAFF:
//         case ITEM_WAND:
//             if (i == 3)
//                os << SpellValue(obj->value[i].c_str());
//             else
//                os << obj->value[i];
//             break;
//         } // switch (obj->item_type)
//      }


//			if (atoi(obj->value[i].c_str()) == 0 && obj->value[i] != "0")
//            MessageBox(0, obj->value[i].c_str(), "Bad Value?", MB_ICONHAND|MB_OK);

         if (i != nMaxObjects-1)
            os << ' ';
      }
      os << "\r\n";

      os << obj->weight << ' ' << obj->cost << " 0\r\n";

      for (TArrayAsVectorIterator<TAffect *> it2(obj->affected); it2; it2++)
         {
         os << "A\r\n";
         os << it2.Current()->location << ' ' <<
               it2.Current()->modifier << "\r\n";
         }

      for (TArrayAsVectorIterator<TExtraDescr *> it3(obj->extra_descr); it3; it3++)
         {
         os << "E\r\n";
         os << it3.Current()->keyword << "~\r\n";
         os << it3.Current()->description << "~\r\n";
         }
}

void TAreaDocument::WriteRoom(TRoom *room, ostream& os)
{
      os << '#' << room->vnum << "\r\n";
      os << room->name << "~\r\n";
      trim(room->description);
      os << room->description << "\r\n~\r\n";
      os << "0 ";
      WriteFlags(os, room->room_flags);
      os << ' ' << room->sector_type << "\r\n";
   	for (int door = 0; door <= 5; door++ )
         {
    		if (room->is_exit[door])
            {
            os << 'D' << door << "\r\n";
            os << room->exit[door].description << "~\r\n";
            os << room->exit[door].keyword << "~\r\n";
            os << exitinfo2locktype(room->exit[door].exit_info) << ' ' <<
                   room->exit[door].key << ' ' << room->exit[door].vnum <<
						 "\r\n";
            }
         }

      for (TArrayAsVectorIterator<TExtraDescr *> it2(room->extra_descr); it2; it2++)
         {
         os << "E\r\n";
         os << it2.Current()->keyword << "~\r\n";
         os << it2.Current()->description << "~\r\n";
         }

      os << "S\r\n";
}

void TAreaDocument::WriteReset(TReset *reset, ostream& os, BOOL fChild)
{
   if (!fChild && strchr("GCFE", reset->command))
      return;

      if (reset->command != 'M')
         os << reset->command;
      switch (reset->command)
         {
         case '*':
            os << reset->comment << "\r\n";
            break;
         case 'C': // Random inv
         case 'F': // Random Eq
         case 'R': // Exit randomization
         case 'G': // Mob inv
            os << " 1 ";
            os << reset->arg1 << ' ' << reset->arg2;
            // Do we need to insert some white space?
            if (!reset->comment.is_null() && reset->comment[0] > ' ')
               os << "\t\t\t";
            os << reset->comment << "\r\n";
            break;
         case 'E': // Mob equip
            os << " 1 " << reset->arg1 << ' ' << reset->arg2 << ' ' <<
               reset->arg3;
            // Do we need to insert some white space?
            if (!reset->comment.is_null() && reset->comment[0] > ' ')
               os << "\t\t\t";
            os << reset->comment << "\r\n";
            break;
         case 'M': // WriteMobReset does M's
            WriteMobReset((TMobReset *) reset, os);
            break;
         case 'P': // Putting object in another object
            os << " 1 ";
            os << reset->arg1 << ' ' << reset->arg2 << ' ' <<
                  reset->arg3;
            // ROM 2.4
            if (MuditorApp->CurrentMode == poundsApp::Rom24Mode)
               // If we never had an arg4, we are converting from
               // another server format, and we will use arg2 which
               // is close.
               if (reset->arg4 == -1) {
                  os << ' ' << reset->arg2;
               } else {
                  os << ' ' << reset->arg4;
               }

            // Do we need to insert some white space?
            if (!reset->comment.is_null() && reset->comment[0] > ' ')
               os << "\t\t\t";
            os << reset->comment << "\r\n";
            break;
         default:
            os << " 1 ";
            os << reset->arg1 << ' ' << reset->arg2 << ' ' <<
                  reset->arg3;
            // Do we need to insert some white space?
            if (!reset->comment.is_null() && reset->comment[0] > ' ')
               os << "\t\t\t";
            os << reset->comment << "\r\n";
            break;
         }
}

void TAreaDocument::WriteMobReset(TMobReset *mres, ostream& os)
{
//   os << "*\r\nM 1 ";
   os << "M 1 ";
   os << mres->arg1 << ' ' << mres->arg2 << ' ' <<
         mres->arg3;

   // ROM 2.4
   if (MuditorApp->CurrentMode == poundsApp::Rom24Mode)
      // If we never had an arg4, we are converting from
      // another server format, and we will use arg2 which
      // is close.
      if (mres->arg4 == -1) {
         os << ' ' << mres->arg2;
      } else {
         os << ' ' << mres->arg4;
      }

   // Do we need to insert some white space?
   if (!mres->comment.is_null() && mres->comment[0] > ' ')
      os << "\t\t\t";

   os << mres->comment << "\r\n";

   for (TArrayAsVectorIterator<TReset *> it(mres->Wields); it; it++)
      WriteReset(it.Current(), os, TRUE);
   for (TArrayAsVectorIterator<TReset *> it2(mres->Inventory); it2; it2++)
      WriteReset(it2.Current(), os, TRUE);
}

void TAreaDocument::WriteShop(TShopData *shop, ostream& os)
{
      os << shop->keeper << ' ';
      for (int i = 0; i < MAX_TRADE; i++)
         os << shop->buy_type[i] << ' ';
      os << shop->profit_buy << ' ' << shop->profit_sell << ' ';
      os << shop->open_hour << ' ' << shop->close_hour;
      if (!shop->comment.is_null() && shop->comment[0] > ' ')
         os << "\t\t\t";
      os << shop->comment << "\r\n";
}

#if 0
// @@@ Make part of class?
static char *szDamageTypes[] = { "hit", "slice", "stab", "slash", "whip",
      "claw", "blast", "pound", "crush", "grep", "bite", "pierce", "suction",
      "chop" };

const char *TAreaDocument::DamageType(int n)
{
   if (n < nelems(szDamageTypes))
      return szDamageTypes[n];
   return 0;
}

// @@@ Make part of class
   static char *szLiquidTypes[] = { "Water", "Beer", "Wine", "Ale",
      "Darkale", "Whisky", "Lemonade", "Firebreather", "Local Specialty",
      "Slime", "Milk", "Tea", "Coffee", "Blood", "Saltwater", "Cola",
      // @@@ Following are Troll-only
      "slim drink", "chocolate milk", "rum",	"grog",
      "pangalactic gargleblaster", "kerosene", "vodka", "soup"
      };

const char *TAreaDocument::LiquidType(int n)
{
   if (n < nelems(szLiquidTypes))
      return szLiquidTypes[n];
   return 0;
}

int TAreaDocument::MaxSpell()
{
   return 206;
}

// Flags:
//    None: Diku
//    1: Merc
//    2: TFC
//    3: Envy
struct SpellAndSlot { char *p; int n; int Flags; };
SpellAndSlot far SpellList[] = {
   { "[none]"               , -1,  1 },
   { "acid blast"           , 70,  1 },
   { "alarm"                , 48,  2 }, /* not imp */
   { "animate dead",          97,  2 }, /* not imp */
   { "anti magic",            87,  2 }, /*not imp*/
   { "armor"                ,  1,    },
   { "bless"                ,  3,    },
   { "blindness"            ,  4,    },
   { "burning hands"        ,  5,    },
   { "call lightning"       ,  6,    },
   { "cause critical"       , 63,  1 },
   { "cause disease",        105,  2 },
   { "cause light"          , 62,  1 },
   { "cause serious"        , 64,  1 },
   { "change sex"           , 82,  1 },
   { "charm person"         ,  7,    },
   { "chill touch"          ,  8,    },
   { "colour spray"         , 10,    },
   { "command",              102,  2 }, /*not imp*/
   { "comprehend",            46,  2 }, /*not imp*/
   { "confusion",            113,  2 }, /* not imp*/
   { "continual light"      , 57,  1 },
   { "control weather"      , 11,    },
   { "create food"          , 12,    },
   { "create spring"        , 80,  1 },
   { "create water"         , 13,    },
   { "cure blindness"       , 14,    },
   { "cure critical"        , 15,    },
   { "cure deafness",         99,  2 }, /*not imp*/
   { "cure disease",         104,  2 },
   { "cure light"           , 16,    },
   { "cure poison"          , 43,  1 },
   { "cure serious"         , 61,  1 },
   { "chain lightning",       90,  2 }, /* not imp */
   { "curse"                , 17,    },
   { "darkroom",              86,  2 }, /* not imp*/
   { "deafness",              52,  2 }, /*not imp*/
   { "detect evil"          , 18,    },
   { "detect good",            9,  2 },
   { "detect hidden"        , 44,  1 },
   { "detect invis"         , 19,    },
   { "detect magic"         , 20,    },
   { "detect poison"        , 21,    },
   { "dismissal",             93,  2 }, /*not imp*/
   { "dispel evil"          , 22,    },
   { "dispel good",           37,  2 },
   { "dispel magic"         , 59,  1 },  // (Removed in Merc 2.2, back in Envy)
   { "earthquake"           , 23,    },
   { "enchant weapon"       , 24,    },
   { "energy drain"         , 25,    },
   { "explosive runes",       91,  2 }, /* not imp*/
   { "faerie fire"          , 72,  1 },
   { "faerie fog"           , 73,  1 },
   { "fear",                 106,  2 }, /*not imp*/
   { "feeblemind",            94,  2 },  /*not imp*/
   { "fireball"             , 26,    },
   { "fireshield",           114,  2 }, /* not imp*/
   { "firestorm",            109,  2 }, /*not imp*/
   { "flamestrike"          , 65,  1 },
   { "fly"                  , 56,  1 },
   { "forget",                60,  2 }, /*not imp*/
   { "fumble",                54,  2 }, /* not imp*/
   { "gate"                 , 83,  1 },
   { "giant strength"       , 39,    },
   { "glyph",                110,  2 }, /* not imp*/
   { "harm"                 , 27,    },
   { "heal"                 , 28,    },
   { "heat metal",           101,  2 }, /* not imp */
   { "identify"             , 53,    },
   { "improved identify"    , 55,  2 }, /*not imp*/
   { "infravision"          , 77,  1 },
   { "invis"                , 29,    },
   { "invulnerability"      , 84,  2 }, /*not imp*/
   { "jump",                  47,  2 },
   { "know alignment"       , 58,  1 },
   { "lightning bolt"       , 30,    },
   { "locate object"        , 31,    },
   { "magic missile"        , 32,    },
   { "magic mouth",           50,  2 }, /*not imp*/
   { "mass invis"           , 69,  1 },
   { "meteor swarm",          95,  2 }, /*not imp*/
   { "mind blank",            96,  2 }, /*not imp*/
   { "monsum one",            71,  2 }, /*not imp*/
   { "monsum two",            75,  2 }, /*not imp*/
   { "monsum three",          76,  2 }, /*not imp*/
   { "monsum four",           78,  2 }, /*not imp*/
   { "monsum five",           79,  2 }, /*not imp*/
   { "obscure languages",     91,  2 }, /*not imp*/
   { "pass door"            , 74,    },
   { "pass wo trace",        100,  2 }, /* not imp*/
   { "phantasmal force",      89,  2 }, /*not imp*/
   { "poison"               , 33,    },
   { "portal",                88,  2 }, /*not imp*/
   { "protection"           , 34,    },
   { "protection from good" , 45,  2 },
   { "quiet",                108,  2 }, /*not imp*/
   { "refresh"              , 81,  1 },
   { "remove curse"         , 35,    },
   { "remove fear",          107,  2 }, /*not imp*/
   { "sanctuary"            , 36,    },
   { "shield"               , 67,  1 },
   { "shocking grasp"       , 37,    },
   { "silence",              112,  2 }, /*not imp*/
   { "sleep"                , 38,    },
   { "stone skin"           , 66,  1 },
   { "summon"               , 40,    },
   { "summon elemental",     111,  2 }, /*not imp*/
   { "teleport"             ,  2,    },
   { "tongues",               98,  2 }, /*not imp*/
   { "true seeing",          103,  2 },
   { "vampiric touch",        85,  2 }, /*not imp*/
   { "ventriloquate"        , 41,    },
   { "weaken"               , 68,  1 },
   { "wizard mark",           49,  2 }, /*not imp*/
   { "word of recall"       , 42,    },

   { "acid breath"          ,200,  1 }, // (Dragon spells are not player castable,
   { "fire breath"          ,201,  1 }, //  but can be on wands, etc)
   { "frost breath"         ,202,  1 },
   { "gas breath"           ,203,  1 },
   { "lightening breath"    ,204,  1 },

   { "general purpose"      ,205,  1 }, // (MegaCity1 spells)
   { "high explosive"       ,206,  1 },

    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     *
     * (EnvyMud doesn't use slots, so... we made up our own #s, because we want
     * to be able to convert an envy zone to merc/tfc - Pound)
     */

   {    "adrenaline control",   300, 3 },
   {    "agitation",            301, 3 },
   {    "aura sight",           302, 3 },
   {    "awe",                  303, 3 },
   {    "ballistic attack",     304, 3 },
   {    "biofeedback",          305, 3 },
   {    "cell adjustment",      306, 3 },
   {    "chameleon power",      307, 3 },
   {    "combat mind",          308, 3 },
   {    "complete healing",     309, 3 },
   {    "control flames",       310, 3 },
   {    "create sound",         311, 3 },
   {    "death field",          312, 3 },
   {    "detonate",             313, 3 },
   {    "disintegrate",         314, 3 },
   {    "displacement",         315, 3 },
   {    "domination",           316, 3 },
   {    "ectoplasmic form",     317, 3 },
   {    "ego whip",             318, 3 },
   {    "energy containment",   319, 3 },
   {    "enhance armor",        320, 3 },
   {    "enhanced strength",    321, 3 },
   {    "flesh armor",          322, 3 },
   {    "heighten senses",      323, 3 },
   {    "inertial barrier",     324, 3 },
   {    "inflict pain",         325, 3 },
   {    "intellect fortress",   326, 3 },
   {    "lend health",          327, 3 },
   {    "levitation",           328, 3 },
   {    "mental barrier",       329, 3 },
   {    "mind thrust",          330, 3 },
   {    "project force",        331, 3 },
   {    "psionic blast",        332, 3 },
   {    "psychic crush",        333, 3 },
   {    "psychic drain",        334, 3 },
   {    "psychic healing",      335, 3 },
   {    "shadow form",          336, 3 },
   {    "share strength",       337, 3 },
   {    "thought shield",       338, 3 },
   {    "ultrablast",           339, 3 }
};

const char *TAreaDocument::SpellName(int nSlot, BOOL fForSave)
{
   // A -1 always means no spell when writing to the file
   if (fForSave && nSlot == -1)
      return "";

   // Give the application first crack at the spell, for overrides
   const char *p = TheMuditorApp->SpellName(nSlot);
   if (p)
      return p;

   // Look up in our list of spells for popular servers
   for (int i = 0; i < nelems(SpellList); i++)
      if (nSlot == SpellList[i].n)
         return SpellList[i].p;

   return 0;
}

const char *TAreaDocument::SpellIndex(int nNdx, int *nSlot)
{
   // Give the application first crack at the spell slot, for overrides
   if (nNdx < TheMuditorApp->nSlotsUsed)
      return TheMuditorApp->SpellIndex(nNdx, nSlot);
   nNdx -= TheMuditorApp->nSlotsUsed;

   if (nNdx < nelems(SpellList))
      {
      if (TheMuditorApp->CurrentMode == poundsApp::TFCMode)
         *nSlot = (SpellList[nNdx].Flags != 3) ? SpellList[nNdx].n : 0;
      else if (TheMuditorApp->CurrentMode == poundsApp::EnvyMode || TheMuditorApp->CurrentMode == poundsApp::Envy2Mode)
         *nSlot = (SpellList[nNdx].Flags != 2) ? SpellList[nNdx].n : 0;
      else // Merc
         *nSlot = (SpellList[nNdx].Flags < 2) ? SpellList[nNdx].n : 0;
      return SpellList[nNdx].p;
      }

   return 0;
}

int TAreaDocument::DamageType(const char *p)
{
   for (int i = 0; i < nelems(szDamageTypes); i++)
      if (strcmp(p, szDamageTypes[i]) == 0)
         return i;

   MessageBox(0, "Warning:  Unknown damage type converted to 'slash'",
      "M.Z.F", MB_ICONHAND|MB_OK);
   return 3;
}

int TAreaDocument::SpellValue(const char *p)
{
   if (p[0] == '\0')
      return 0;

   // Give the app customization first crack at this spell type      
   int n = TheMuditorApp->SpellValue(p);
   if (n)
      return n;

   for (int i = 0; i < nelems(SpellList); i++)
      if (stricmp(p, SpellList[i].p) == 0)
         return SpellList[i].n;

   char szBuffer[512];
   wsprintf(szBuffer,
      "Warning:  Unknown spell type '%s' converted to '[none]'", p);
   MessageBox(0, szBuffer, "M.Z.F", MB_ICONHAND|MB_OK);
   return 0;
}

int TAreaDocument::LiquidVal(const char *p)
{
   for (int i = 0; i < nelems(szLiquidTypes); i++)
      if (strcmp(p, szLiquidTypes[i]) == 0)
         return i;

   MessageBox(0, "Warning:  Unknown liquid type converted to water",
      "M.Z.F", MB_ICONHAND|MB_OK);
   return 0;
}
#endif

BOOL TAreaDocument::Close ()
{
    BOOL result;

    result = TFileDocument::Close();

    // INSERT>> Your code here.
    if (IsDirty())
       MessageBox(0, "@@@Closing Dirty...", "M.Z.F.", MB_ICONINFORMATION|MB_OK);

    return result;
}

BOOL TAreaDocument::CanClose ()
{
//    BOOL result;
//
//    result = TFileDocument::CanClose();
//
//    // INSERT>> Your code here.
//    return result;

   // The original Expert generated code did not call FlushDoc if there was
   // more than one view.
   return GetDocManager().FlushDoc(*this);  // do the UI in the doc manager
}

void TAreaDocument::WriteOgham(ostream& os)
{
   os << "////////////////////////////////////////////////////////////\r\n";
   os << "// " << name << "\r\n";
   os << "////////////////////////////////////////////////////////////\r\n";
   os << "\r\n\r\n\r\n";

   LPCSTR p = GetDocPath();
   char szName[MAXFILE];
   fnsplit(p, 0, 0, szName, 0);

   WriteOghamRegion(os, szName);
   WriteOghamRooms(os, szName);
}

void TAreaDocument::WriteOghamRegion(ostream& os, const char *p)
{
   os << "int " << p << "_reg(int user, int com, const char word[][WORD_LEN+1])\r\n";
   os << "{\r\n";
   os << "   return 0;\r\n";
   os << "}\r\n\r\n";
}

// @@@ Make part of class
void WriteCString(ostream& os, const char *p)
{
   os << '"';

   while (*p)
      {
      if (*p == '"')
         os << "\\\"";
      else if (*p == '\r')
         os << "\\r";
      else if (*p == '\n')
         os << "\\n";
      else
         os << *p;
         
      p++;
      }

   os << '"';
}

void TAreaDocument::WriteOghamRooms(ostream& os, const char *p)
{
	if (TheRooms.GetItemsInContainer() < 1)
      return;

   for (TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
      {
      // @@@ This doesn't yet take into account such things are room flags
      // and exit flags...

      os << "BOOL " << p << "_" << it.Current()->vnum << "(int user, int com, int dir, TRoom *this_room, const char word[][WORD_LEN+1])\r\n";
      os << "{\r\n";
      os << "   static int links[]={ "
         << it.Current()->exit[DIR_NORTH].vnum << ", "
         << it.Current()->exit[DIR_SOUTH].vnum << ", "
         << it.Current()->exit[DIR_EAST].vnum << ", "
         << it.Current()->exit[DIR_WEST].vnum << ", "
         << it.Current()->exit[DIR_UP].vnum << ", "
         << it.Current()->exit[DIR_DOWN].vnum << ", "
         << " -1, -1, -1, -1 };\r\n";
      os << "   switch(com) {\r\n";
      os << "	case 0:\r\n";
      os << "	   write_user(user, ";
      WriteCString(os, it.Current()->description.c_str());
      os << ");\r\n";
      os << "	   exits_contents(user, links, \"\");\r\n";
      os << "      return 0;\r\n";
      os << "\r\n";
      os << "	case 1:\r\n";
      os << "      return links[dir];\r\n";
      os << "	}\r\n";
      os << "\r\n";
      os << "   return 0;\r\n";
      os << "}\r\n\r\n";
      }
}

void TAreaDocument::WriteOghamMobiles(ostream& /*os*/, const char * /*p*/)
{
   // @@@ notimp
}

void TAreaDocument::WriteOghamObjects(ostream& /*os*/, const char * /*p*/)
{
   // @@@ notimp
}

void TAreaDocument::WriteRomFlags(ostream& os, int32 flags)
{
   // @@@ Possible convert this to use Flags2Str
   if (flags == 0) {
      os << '0';
      return;
   }

   static char flag_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde";
   for (int i = 0; i < 32 && flags; i++, flags >>= 1)
      if (flags&1) {
         os << flag_table[i];

//         // Lower case letters are doubled by sometimes in files, but I
//         // don't believe the server cares one way or the other.
//         if (islower(flag_table[i])) {
//            os << flag_table[i];
//         }
      }
}

void TAreaDocument::WriteRom24Mobile(TMobile *mob, ostream& os)
{
   os << '#' << mob->vnum << "\r\n";
   os << mob->player_name << "~\r\n";
   os << mob->short_descr << "~\r\n";
   os << mob->long_descr << "\r\n~\r\n";    // (Add back on the crlf we stripped)
   trim(mob->description);
   os << mob->description << "\r\n~\r\n";
   if (mob->strrace.is_null())
      os << "unique~\r\n";
   else
      os << mob->strrace << "~\r\n";

   WriteRomFlags(os, mob->act & ~rom_race_table[mob->race].act);
   os << ' ';
   WriteRomFlags(os, mob->affected_by & ~rom_race_table[mob->race].aff);
   os << ' ' << mob->alignment << ' ' << mob->mob_group << "\r\n";

   os << mob->level << ' ' << mob->hitroll << ' ';
   os << mob->hitnodice << 'd' << mob->hitsizedice << '+' << mob->hitplus << ' ';
   os << mob->mana[DICE_NUMBER] << 'd' << mob->mana[DICE_TYPE] << '+' << mob->mana[DICE_BONUS] << ' ';
   os << mob->damnodice << 'd' << mob->damsizedice << '+' << mob->damplus << ' ';
//      pMobIndex->dam_type		= (sh_int) fread_number( is, "Reading mobile damage type" );
   os << mob->strDamType << "\r\n";
   os << mob->rom_ac[AC_PIERCE] << ' ' << mob->rom_ac[AC_BASH] << ' ' << mob->rom_ac[AC_SLASH] << ' ' << mob->rom_ac[AC_EXOTIC] << "\r\n";

   WriteRomFlags(os, mob->off_flags & ~rom_race_table[mob->race].off);
   os << ' ';
   WriteRomFlags(os, mob->imm_flags & ~rom_race_table[mob->race].imm);
   os << ' ';
   WriteRomFlags(os, mob->res_flags & ~rom_race_table[mob->race].res);
   os << ' ';
   WriteRomFlags(os, mob->vuln_flags & ~rom_race_table[mob->race].vuln);
   os << "\r\n";

   // @@@ Should be moved into the document class
   static const char *aszRomSexes[] = { "none", "male", "female", "either" };
//   static const char *aszPositions[] = { "stand", "rest", "sleep" };
//   sh_int nStart = mob->start_pos, nDefault = mob->default_pos;
//   if (nStart >= nelems(aszPositions))
//      nStart = 0;
//   if (nDefault >= nelems(aszPositions))
//      nDefault = 0;
//   os << aszPositions[nStart] << ' ' << aszPositions[nDefault] << ' ' <<
//      aszRomSexes[mob->sex] << ' ' << mob->gold << "\r\n";
   os << position_name(mob->start_pos) << ' ' <<
      position_name(mob->default_pos) << ' ' <<
      aszRomSexes[mob->sex] << ' ' << mob->gold << "\r\n";

   WriteRomFlags(os, mob->form & ~rom_race_table[mob->race].form);
   os << ' ';
   WriteRomFlags(os, mob->parts & ~rom_race_table[mob->race].parts);
   os << ' ';
   static const char *aszRomSizes[] = { "tiny", "small", "medium", "large", "huge", "giant" };
   os << aszRomSizes[mob->size] << ' ';
   // Sometimes, we don't have a material when converting from another
   // file format.
   if (mob->material.is_null())
      os << "0";
   else
      os << mob->material;
   os << "\r\n";

   // If MOBprograms, read them now
   if (MuditorApp->fMOBprograms)
      if (mob->mobprogs.GetItemsInContainer() > 0)
         {
         for (TArrayAsVectorIterator<TMobProgData*> it(mob->mobprogs); it; it++)
            {
            if (!it.Current()->MobFile.is_null())
               MessageBox(0, "Warning: mprog file being inlined", "M.Z.F.",
                  MB_ICONINFORMATION|MB_OK);
            os << '>' << it.Current()->type << ' ' << it.Current()->arglist <<
                  "~\r\n" << it.Current()->comlist << "~\r\n";
            }

         os << "|\r\n";
         }
}

void TAreaDocument::WriteRom24Object(TObject *obj, ostream& os)
{
      os << '#' << obj->vnum << "\r\n";
      os << obj->name << "~\r\n";
      os << obj->short_descr << "~\r\n";
      os << obj->description << "~\r\n";
      os << obj->material << "~\r\n";
      os << obj->strType << ' ';
      WriteRomFlags(os, obj->extra_flags);
      os << ' ';
      WriteRomFlags(os, obj->wear_flags);
      os << "\r\n";

      TLibrary *pLibrary = const_cast<poundsApp*>(GetMZF())->GetLibrary();

		int nMaxObjects = 5;
      for (int i = 0; i < nMaxObjects; i++) {
         // If we have a flag on our hands, it might be a Merc flag.  Conv
         char szValue[64];
         if (pLibrary->GetTypeInfo(obj->strType, i) == FLAG_TYPE) {
            int32 n = TAreaDocument::Str2Flag(obj->value[i].c_str());
            TAreaDocument::Flags2Str(poundsApp::Rom24Mode, szValue, n);
            }
         else
            strcpy(szValue, obj->value[i].c_str());

         // If we are writing out something that isn't a word ('cause it
         // has whitespace), put it in single quotes.  This is for spells.
         if (szValue[0] == '\0')
            os << "''";
         else if (strchr(szValue, ' '))
            os << '\'' << szValue << '\'';
         else
            os << szValue;

         if (i != nMaxObjects-1)
            os << ' ';
      }
      os << "\r\n";

      os << obj->level << ' ' << obj->weight << ' ' << obj->cost << ' ' <<
         (char) obj->condition << "\r\n";

#if 1
      // Affects
      for (TArrayAsVectorIterator<TAffect *> it2(obj->affected); it2; it2++) {
//         if (it2.Current()->where == TO_OBJECT) {
            os << "A\r\n";
            os << it2.Current()->location << ' ' <<
                  it2.Current()->modifier << "\r\n";
//         }
      }

      // Now we write out any pure affect bitvectors.  If the original
      // zone had an affected_by/stat effect reset, it has been split into
      // two pieces.
      if (obj->affected_by) {
         os << "F\r\nA 0 0 ";
         WriteRomFlags(os, obj->affected_by);
         os << "\r\n";
      }
#else
      // Stat-affecting Flags.  If we have both affect bits and modifiers
      // we will first write out a combine flag for both.
      int32 flags = obj->affected_by;
      it2.Restart();
      if (flags && it2) {
         os << "F\r\nA ";
         os << it2.Current()->location << ' ' <<
                  it2.Current()->modifier << ' ';
         WriteRomFlags(os, flags);
         os << "\r\n";
         it2++;
         flags = 0;
      }

      // Now we write out any modifiers left over
      for ( ; it2; it2++) {
//         if (it2.Current()->where != TO_OBJECT) {
            os << "F\r\n";
//            static const char *szObjFlags = "A@IRV@";
            os << "A ";
//            os << szObjFlags[it2.Current()->where] << ' ';
            os << it2.Current()->location << ' ' <<
                  it2.Current()->modifier << ' ';
            os << "0\r\n";
//            WriteRomFlags(os, it2.Current()->bitvector);
//            os << "\r\n";
//         }
      }

      // Now we write out any pure bitvectors
      if (flags) {
         os << "F\r\nA 0 0 ";
         WriteRomFlags(os, flags);
         os << "\r\n";
      }
#endif
      if (obj->immunity) {
         os << "F\r\nI 0 0 ";
         WriteRomFlags(os, obj->immunity);
         os << "\r\n";
      }

      if (obj->resistance) {
         os << "F\r\nR 0 0 ";
         WriteRomFlags(os, obj->resistance);
         os << "\r\n";
      }

      if (obj->vulnerability) {
         os << "F\r\nV 0 0 ";
         WriteRomFlags(os, obj->vulnerability);
         os << "\r\n";
      }

      // Extra descriptions
      for (TArrayAsVectorIterator<TExtraDescr *> it3(obj->extra_descr); it3; it3++)
         {
         os << "E\r\n";
         os << it3.Current()->keyword << "~\r\n";
         os << it3.Current()->description << "~\r\n";
         }
}

void TAreaDocument::WriteRomRoom(TRoom *room, ostream& os)
{
      // Basic room stuff
      os << '#' << room->vnum << "\r\n";
      os << room->name << "~\r\n";
      trim(room->description);
      os << room->description << "\r\n~\r\n";
      os << "0 ";
      WriteRomFlags(os, room->room_flags);
      os << ' ' << room->sector_type << "\r\n";

      // Special fast/slow regen rooms
      if (room->heal_rate != 100 && room->mana_rate != 100)
         os << "H " << room->heal_rate << " M " << room->mana_rate << "\r\n";
      else if (room->heal_rate != 100)
         os << "H " << room->heal_rate << "\r\n";
      else if (room->mana_rate != 100)
            os << "M " << room->mana_rate << "\r\n";

      // Write each door
   	for (int door = 0; door <= 5; door++ ) {
    		if (room->is_exit[door]) {
            os << 'D' << door << "\r\n";
            os << room->exit[door].description << "~\r\n";
            os << room->exit[door].keyword << "~\r\n";
            os << exitinfo2locktype(room->exit[door].exit_info) << ' ' <<
                   room->exit[door].key << ' ' << room->exit[door].vnum <<
						 "\r\n";
         }
      }

      // Extra descriptions
      for (TArrayAsVectorIterator<TExtraDescr *> it2(room->extra_descr); it2; it2++) {
         os << "E\r\n";
         os << it2.Current()->keyword << "~\r\n";
         os << it2.Current()->description << "~\r\n";
      }

      // Clan rooms, if any
      if (!room->clan.is_null())
         os << "C " << room->clan << "\r\n";

      // Owners, if any.  I am not sure what this is, it doesn't seem to
      // be in the ROM 2.4 building document but it is in the server code.
      if (!room->owner.is_null())
         os << "O " << room->owner << "\r\n";

      os << "S\r\n";
}

#if 0
Appendix A:  Recommended Values
The following values should be followed closely for all mobiles -- try
not to go more than 1 level away from the recommended values unless you are absolutely sure you want your mobile to be harder or easier than a
default monster. In particular, avoid making them easier.  Certain action flags (namely the 4 class-related flags) can modify the recommendations, they are explained in detail at the end of this section.

level  hit pts    ac    damage          level  hit pts    ac    damage
  1   2d6+10      9     1d4+0            31   6d12+928    -10   4d6+9
  2   2d7+21      8     1d5+0            32  10d10+1000   -10   6d4+9 
  3   2d6+35      7     1d6+0            33  10d10+1100   -11   6d4+10 
  4   2d7+46      6     1d5+1            34  10d10+1200   -11   4d7+10 
  5   2d6+60      5     1d6+1            35  10d10+1300   -11   4d7+11 

  6   2d7+71      4     1d7+1            36  10d10+1400   -12   3d10+11 
  7   2d6+85      4     1d8+1            37  10d10+1500   -12   3d10+12 
  8   2d7+96      3     1d7+2            38  10d10+1600   -13   5d6+12 
  9   2d6+110     2     1d8+2            39  15d10+1700   -13   5d6+13 
 10   2d7+121     1     2d4+2            40  15d10+1850   -13   4d8+13 

 11   2d8+134     1     1d10+2           41  25d10+2000   -14   4d8+14 
 12   2d10+150    0     1d10+3           42  25d10+2250   -14   3d12+14 
 13   2d10+170   -1     2d5+3            43  25d10+2500   -15   3d12+15 
 14   2d10+190   -1     1d12+3           44  25d10+2750   -15   8d4+15 
 15   3d9+208    -2     2d6+3            45  25d10+3000   -15   8d4+16

 16   3d9+233    -2     2d6+4            46  25d10+3250   -16   6d6+16 
 17   3d9+258    -3     3d4+4            47  25d10+3500   -17   6d6+17 
 18   3d9+283    -3     2d7+4            48  25d10+3750   -18   6d6+18 
 19   3d9+308    -4     2d7+5            49  50d10+4000   -19   4d10+18 
 20   3d9+333    -4     2d8+5            50  50d10+4500   -20   5d8+19 

 21   4d10+360   -5     4d4+5            51  50d10+5000   -21   5d8+20 
 22   5d10+400   -5     4d4+6            52  50d10+5500   -22   6d7+20 
 23   5d10+450   -6     3d6+6            53  50d10+6000   -23   6d7+21
 24   5d10+500   -6     2d10+6           54  50d10+6500   -24   7d6+22 
 25   5d10+550   -7     2d10+7           55  50d10+7000   -25   10d4+23 

 26   5d10+600   -7     3d7+7            56  50d10+7500   -26   10d4+24 
 27   5d10+650   -8     5d4+7            57  50d10+8000   -27   6d8+24 
 28   6d12+703   -8     2d12+8           58  50d10+8500   -28   5d10+25 
 29   6d12+778   -9     2d12+8           59  50d10+9000   -29   8d6+26 
 30   6d12+853   -9     4d6+8            60  50d10+9500   -30   8d6+28 

Thief* mobiles should read their hp, ac, and damage at one level lower 
Mage mobiles read hp and ac at one level lower, and damage three levels 
lower 
Cleric mobiles read damage at two levels lower 
Warrior mobiles read hit points one level higher 
Armor class vs. magical attacks should be computed by this formula: 
(ac - 10) / n + 10, where n is 4 for most mobiles, 3 for thieves and 
clerics, and 2 for mages. 

Remember, +hit should only be given to very exceptional mobiles.

* a thief mobile either has ACT_THIEF set or is decidedly thief-like in  
nature. The same holds true for the other modifiers. 


Appendix B: Removing Flags 
There are times when a mobile will clearly belong to a certain race, but  
for whatever reason you will not it to have certain flags belonging to 
that race.  For examples, dragons may normally be resistant to fire, but you may wish to make a white dragon that is vulnerable to fire and immune to cold.  To remove flags from the mobile, add flag removal commands at the end of the entry (below the form/parts/size/material line).

The base syntax is: 
F <flag type> <flags>

Flag type may be either action, affect, offensive, immunity, resistance,  
vulnerability, form, or parts ( usually just written as act, aff, off, 
imm, res, vul, for, and par). 

For example, to make a dragon that is not vulnerable to cold or 
resistant to fire, use: 

F res H 
F vul I 

More than one flag can be removed on the same line, as long as they are 
all part of the same flag group.  So to remove fast and dodge from a wolf, you would type: 

F off FH 

Don't be afraid to move flags if you have to, and absolutely do NOT 
define a new race just because a few of the default flags don't agree with you. 

#endif

/*
plastic slime           jelly           wax             rubber
oil             balm            cream           hard/soft leather
kid leather     fur             snakeskin       gut             food
meat            bread           wood            hardwood        softwood
bamboo  ebony           cork            horn            light laen
sponge  elastic silk            satin           lace
wool            linen           canvas  cloth           velvet
felt            paper           parchment       vellum  hemp
feathers        webbing cardboard       steel           gold
silver  mithril platinum        iron            lead
copper  electrum        bronze  brass           wire
tin             pewter  metal           dark laen       porcelain
ivory           marble  stone           quartz  corundum
flint           lodestone       granite enamel  obsidian
adamantite      glass           pottery crystal ice
bone            shell           coral           energy  fire
air             water           acid            coal            sandstone
clay            ash             earth           diamond etherealness    
nothingness

dragonscale, blue dragonscale, black dragonscale, white dragonscale, red 
dragonscale.
*/

/*
ROM object extra flags

(Glowing)                       A               (Humming)               B
Dark (hidden)           C               Evil                    E
Invis                           F               Magic                   G
Nodrop                  H               Bless                   I
Anti-good                       J               Anti-evil               K
Anti-Neutral            L               Noremove                M
Inventory                       N               Nopurge         O
Rot_death                       P               Vis_death               Q
Nosac                           R               Nolocate                T
Melt_drop                       U               Sell extract    W
Burn proof                      Y

*/

/*
   ROM wield flags

   A     Take              H    Hands           O  Hold
B     Finger            I    Arms            Q  Float
C     Neck              J    Shield
D     Body              K    About body
E     Head              L    Waist
F     Legs              M    Wrist
G     Feet              N    Wield
*/

/*
   ROM object flags

   affect bits:
 *A  Blind           *I  Faerie_fire         Q  Hide       *Y  Weaken
  B  Invisible        J  Infrared           *R  Sleep       Z  Dark_vis
  C  Detect_evil      K  Curse              *S  Charm      *a  Berserk
  D  Detect_invis   **L  Flaming             T  Flying    **b  Swim
  E  Detect_magic    *M  Poisoned            U  Pass_door   c  Regen.
  F  Detect_hidden    N  Prot_evil           V  Haste       d  Slow
  G  Detect_good      O  Prot_good          *W  Calm
  H  Sanctuary        P  Sneak              *X  Plague
 * items will be detrimental to the character, possibly for cursed
items.
 ** not yet implemented
 */

 /*
   ROM spec-funs

   nasty_thief      Mobile attacks, robs, and runs (ALWAYS set thief flag)

The following specials are coded, but should NOT be used because they
are tailored to specific mobiles.  Check them out online for examples of
unique or area-specific specials that can possibly be coded for your mobiles:

cast_judge       Mobile fires bullets (for Mega-City One only)
troll_member     Mobile attacks ogre gang members (gangland mobs only)
ogre_member      Mobile attacks troll gang members (gangland mobs only)
patrolman        Mobile tries to break up fights (gangland mobs only)
mayor                The Midgaard mayor special (opens/closes city gates, 
etc.)

*/

/*
   ROM applies

   1   Strength             12  Mana 
2   Dexterity            13  Hitpoints 
3   Intelligence         14  Movement 
4   Wisdom               17  AC 
5   Constitution         18  Hitroll 
6   Sex                  19  Damroll 
7   Charisma             20  Spell (includes rods, staves, etc.)

*/


