#if !defined(__areadoc_h)              // Sentry, use file only if it's not already included.
#define __areadoc_h

/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areadoc.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TAreaDocument (TFileDocument).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include <OWL\FILEDOC.H>
#include <classlib\arrays.h>

// Allow this program to be compiled on BC++ 4.0, which had no "bool" type
#if __BORLANDC__ <= 0x452
typedef enum {false, true} bool;
#endif

#include "pounds.rh"            // Definition of all resources.
#include "pounds.h"
#include "objmerc.h"

// Race stuff.  (Needed to load ROM zones.)
struct rom_race_type // ROM
{
	 char *	name;		/* call name of the race */
	 BOOL	pc_race;		/* can be chosen by pcs */
	 int32 act;			/* act bits for the race */
	 int32 aff;			/* aff bits for the race */
	 int32 off;			/* off bits for the race */
	 int32 imm;			/* imm bits for the race */
	 int32 res;			/* res bits for the race */
	 int32 vuln;		/* vuln bits for the race */
	 int32 form;		/* default form flag for the race */
	 int32 parts;		/* default parts for the race */
};

#define MAX_RACE 42
struct  envy_race_type // envy 2
{
    char *              name;
    int32               race_abilities;
    int                 size;
    int                 str_mod;
    int                 int_mod;
    int                 wis_mod;
    int                 dex_mod;
    int                 con_mod;
    char *              dmg_message;
    char *              hate;
};

class TComboBoxData;

struct ChangeNotice { int nWhich; int nIndex; const TMercObj *pObj; };

const int vnAreaAppend = vnCustomBase+0;
const int vnAreaDelete = vnCustomBase+1;
const int vnAreaModify = vnCustomBase+2;

//NOTIFY_SIG(vnAreaAppend, ChangeNotice)
NOTIFY_SIG(vnAreaAppend, ChangeNotice*)
NOTIFY_SIG(vnAreaDelete, ChangeNotice*)
NOTIFY_SIG(vnAreaModify, ChangeNotice*)

#define EV_VN_AREAAPPEND  VN_DEFINE(vnAreaAppend,  VnAppend,  pointer)
#define EV_VN_AREADELETE  VN_DEFINE(vnAreaDelete,  VnDelete,  pointer)
#define EV_VN_AREAMODIFY  VN_DEFINE(vnAreaModify,  VnModify,  pointer)

//{{TFileDocument = TAreaDocument}}
class TAreaDocument : public TFileDocument {
friend class TMobDialog;	// @@@ had to do this for shopkeeper editing...
friend class TRoomDialog;  // @@@ had to do this for object reset editing...
friend class TContainerDialog; // @@@ For container editing...
public:
    TAreaDocument (TDocument* parent = 0);
    virtual ~TAreaDocument ();

    static const sh_int TAreaDocument::rev_dir[6];
    poundsApp::currMode GetMode() const;
    const poundsApp* GetMZF() const {return MuditorApp; }
	 void Renumber( sh_int nNewBase );

	 // Generic routines
	 const TMercObj *GetItem(int nWhich, int nIndex, char *lpOut, int nOutSize);
	 void  EditItem(int nWhich, int nIndex);
	 void  DeleteItem(int nWhich, int nIndex);
	 void  CopyItem(int nWhich, int nIndex);

	 // Mobile routines
	 const TMobile* GetMobile(vnum_int vnum) { return get_mob_index(vnum); }
	 const TMobile* GetMobileFromIndex(int nNdx) const;
	 TMobile* MakeNewMobile();
	 sh_int NextMobVnum();
	 void MobilesToCombo(TComboBoxData&, vnum_int vnum) const;
	 void MobileSave(const TMobile& room, int nIndex);

	 // Room routines
	 const TRoom* GetRoom(vnum_int vnum)     { return get_room_index(vnum); }
	 const TRoom* GetRoomFromIndex(int nNdx) const;
	 int GetIndexFromRoom(const TRoom *) const;
	 TRoom* MakeNewRoom();
	 sh_int NextRoomVnum();
	 TRoom* RoomSave(const TRoom& room, int nIndex);
	 void RoomsToCombo(TComboBoxData&, vnum_int vnum) const;
	 void RoomsToList(TListBoxData&, sh_int nMobVnum) const;

	 // Object routines
	 const TObject* GetObject(vnum_int vnum) { return get_obj_index(vnum); }
	 const TObject* GetObjectFromIndex(int nNdx) const;
	 TObject *MakeNewObject();
	 sh_int NextObjVnum();
	 void ObjectsToComboWear(TComboBoxData&, int32 wear_pos, vnum_int vnum);
	 void ObjectsToComboType(TComboBoxData&, sh_int type, vnum_int vnum) const;
	 void ObjectsToList(TListBoxData&) const;
	 void ObjectSave(const TObject& room, int nIndex);

	 // Help routines
	 THelpData *MakeNewHelp();
	 void HelpSave(const THelpData& help, int nIndex);

	 // Shop routines
	 TShopData* MakeNewShop();
	 void ShopSave(const TShopData& shop, int nIndex);
	 int GetShopIndexFromMobileVNum(sh_int mobile);

	 // Mob reset routines
	 TMobReset *MakeNewMobReset();
	 void SaveMobReset(const TMobReset& mset, int nIndex);
	 const TMobReset* GetMobResetFromIndex(int nNdx) const;
	 int GetIndexFromMobReset(const TMobReset *) const;

	 // Generic reset routines
	 TReset* SaveReset(const TReset& rset, int nIndex);
	 int GetIndexFromReset(const TReset *);


	 // Database magic #s
	 static const char *DamageType(int n);
	 static int MaxSpell();
	 static const char *SpellName(int nSlot, BOOL fForSave=FALSE);
	 static const char *SpellIndex(int nNdx, int *nSlot);
	 static const char *LiquidType(int n);

	 static int DamageType(const char *);
	 static int SpellValue(const char *);
	 static int LiquidVal(const char *);

    // Conversion routines
    static int32 Str2Flag(const char *p);
    static void Flags2Str(poundsApp::currMode mode, char *p, int32 nFlags);

	 string name;			 // Zone's name as it appears in #AREAS
	 sh_int recall;		 // This zone's recall square, for EnvyMud
	 TAreaData area_data; // This zone's area data, for Troll
	 BOOL fUsesAreaData;  // Does this area file use area_data?

    // ROM 2.4 stuff
    string shortname;
    static int32 flag_convert(char letter);
    static sh_int position_lookup(const char *p);
    static const char *position_name(sh_int n);
    static void LoadPositionComboBox(TComboBoxData *pCBData, int n);
    static struct rom_race_type far rom_race_table[];

    // Envy stuff
    static struct envy_race_type far envy_race_table[MAX_RACE];


protected:
	 BOOL   fIsOpen;
	 poundsApp *MuditorApp;
	 char   szSection[64];      // For zone error checking
	 char   szLastSuccess[256]; // For zone error checking
    TLibrary *m_Library;

//    string name;			// Zone's name as it appears in #AREAS

	 sh_int nNextMobVNum;
	 int32 nDefaultAction;
	 int32 nDefaultAffectedBy;
	 TArrayAsVector<TMobile *> TheMobiles;

    sh_int nNextObjVNum;
    TArrayAsVector<TObject *> TheObjects;

    sh_int nNextRoomVNum;
    int nDefaultSectorType;
    int32 nDefaultSectorFlags;
	 TArrayAsVector<TRoom *> TheRooms;

//    TArrayAsVector<TVNumObj *> TheMobiles;
//    TArrayAsVector<TVNumObj *> TheObjects;
//	 TArrayAsVector<TVNumObj *> TheRooms;

    TArrayAsVector<THelpData *> TheHelps;
    TArrayAsVector<TShopData *> TheShops;
    TArrayAsVector<TReset *> TheResets;
    TArrayAsVector<TMobReset *> TheMobResets;

    // High-level reads
    void	load_area       ( istream& is );
    void	load_helps      ( istream& is );
	 void load_recall     ( istream& is );
    void	load_mobiles    ( istream& is, poundsApp::currMode );
    void	load_mobprogs   ( istream& is );
    void	load_objects    ( istream& is, poundsApp::currMode );
    void	load_resets     ( istream& is );
    void	load_rooms      ( istream& is, poundsApp::currMode );
    void	load_shops      ( istream& is );
    void	load_specials   ( istream& is );
    void	load_troll_area ( istream& is );
    void load_socials    ( istream& is );
    void load_creator    ( istream& is );
    void	fix_exits       ( );

    // Low-level readers
    char	fread_letter	( istream& is );
    int32	fread_number	( istream& is, const char *operation );
    string fread_string	( istream& is, const char *operation );
    string	fread_to_eol	( istream& is );
    char * fread_word	( istream& is, const char *operation );
    char *fread_word2( istream& is, const char *operation );
    PMobProgData mprog_file_read(const string& f, PMobProgData mprg,
       PMobile pMobIndex);
    void mprog_read_programs(istream& is, PMobile pMobIndex);

    // Codebase-level readers
    void LoadRom24Mobile(TMobile *pMobIndex, vnum_int vnum, istream& is);
    void LoadMobile(TMobile *pMobIndex, vnum_int vnum, istream& is);
    void LoadRom24Object(TObject *pMobIndex, vnum_int vnum, istream& is);
    void LoadObject(TObject *pMobIndex, vnum_int vnum, istream& is);


    // ROM support
    int32 fread_flag( istream& is, const char *operation );
    sh_int race_lookup (const string& name) const;
    BOOL str_prefix( const char *astr, const char *bstr ) const;
    static const char *aszRomPosition[4];

    // High level writers
	 void WriteHelps(ostream& os);
	 void WriteMobs(ostream& os);
    void WriteMobProgs(ostream& os);
	 void WriteObjects(ostream& os);
	 void WriteRooms(ostream& os);
	 void WriteResets(ostream& os);
	 void WriteShops(ostream& os);
	 void WriteSpecials(ostream& os);
	 void WriteAreaData(ostream& os);

    // Medium level writers
	 void WriteHelp(THelpData *, ostream& os);
	 void WriteMobile(TMobile *, ostream& os);
    void WriteMobProg(TMobProgData *, ostream& os);
	 void WriteObject(TObject *, ostream& os);
	 void WriteRoom(TRoom *, ostream& os);
	 void WriteReset(TReset *, ostream& os, BOOL fChild = FALSE);
    void WriteMobReset(TMobReset *, ostream& os);
	 void WriteShop(TShopData *, ostream& os);

    // ROM medium level writers
    void WriteRom24Mobile(TMobile *mob, ostream& os);
    void WriteRom24Object(TObject *obj, ostream& os);
	 void WriteRomRoom(TRoom *, ostream& os);

    // Ogham support--medium level writers
    void WriteOgham(ostream& os);
    void WriteOghamRegion(ostream& os, const char *p);
    void WriteOghamRooms(ostream& os, const char *p);
    void WriteOghamMobiles(ostream& os, const char *p);
    void WriteOghamObjects(ostream& os, const char *p);

    // Low level writers
    void WriteFlags(ostream& os, int32 flags);

    // ROM low lever writers
    void WriteRomFlags(ostream& os, int32 flags);

	 // Helper functions
    static void smash_tilde( string& str );
    static void trim( string& str );
    static int exitinfo2locktype(int exitinfo);

//    TVNumObj *GetVNumIndex(int nWhich, vnum_int vnum);
    TMobile* get_mob_index	( vnum_int vnum );
    TObject* get_obj_index	( vnum_int vnum );
    TRoom* get_room_index	( vnum_int vnum );

//    void bug(char *, int=0);
    void SyntaxError(istream& is, const char *operation, const char *msg);

//{{TAreaDocumentVIRTUAL_BEGIN}}
public:
    virtual BOOL IsOpen ();
    virtual BOOL Open (int mode, LPCSTR path = 0);
    virtual BOOL Commit (BOOL force = FALSE);
    virtual BOOL Close ();
    virtual BOOL CanClose ();
//{{TAreaDocumentVIRTUAL_END}}
};    //{{TAreaDocument}}
#endif                                      // __areadoc_h sentry.

