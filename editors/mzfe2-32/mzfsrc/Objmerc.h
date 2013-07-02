#ifndef _OBJMERC_H
#define _OBJMERC_H
//#include <classlib\listimp.h>
#include <classlib\arrays.h>

typedef short sh_int;
typedef long  int32;

// Some MUDs are using 32 bits for VNUMS.  I need to continue compiling
// for 16 bit vnums for safety's sake, then switch to 32 bit later so I'm
// creating a type for vnums in the code
typedef short vnum_int;

#define MAX_STRING_LENGTH	 8192
#define MAX_INPUT_LENGTH	  160

#define nelems(a) (sizeof (a)/sizeof (a)[0])

extern const char HelpFileName[];

// @@@;
class TAreaDocument;
class TRoom;
class TReset;

//
// class TPointer
// ----- --------
//
template<class T> class TSafePointerBase {
  public:
//    T&   operator  *() {return *P;}
//    T&   operator  *() { if (P==0) THROW(TXOwl("Bad Deref *")); return *P;}
    T&   operator  *();
         operator T*() {return P;}
    int  operator  !() {return P==0;}
    void operator  ~() {P = 0;}
//    void operator delete(void* p) { ((TSafePointerBase<T>*)p)->P=0;}
//    void operator delete(void* p) { delete ((TSafePointerBase<T>*)p)->P;}
    void do_delete() { delete P; }
  protected:
    TSafePointerBase(T* pointer) : P(pointer){}
//   ~TPointerBase() {delete P;}
    TSafePointerBase() : P(0){}
    T* P;
  private:
//    void* operator new(size_t){return 0;}  // prohibit use of new
};

template<class T> class TMercPointer : public TSafePointerBase<T> {
  public:
    TMercPointer() : TSafePointerBase<T>(){}
    TMercPointer(T* pointer) : TSafePointerBase<T>(pointer){}
    T* operator =(T* src) {/*delete P;*/ return P = src;}
    T* operator =(const TMercPointer<T>& src)
                          {/*delete P;*/ return P = src.P;}
//    T* operator->() { if (P==0) THROW(TXOwl("Bad deref ->")); return P;}
    T* operator->();
};

template<class T> T& TSafePointerBase<T>::operator *()
{
   if (P==0)
      THROW(TXOwl("Bad Deref *"));
   return *P;
}

template<class T> T* TMercPointer<T>::operator->()
{
   if (P==0)
      THROW(TXOwl("Bad deref ->"));
   return P;
}

class TMercObj {
public:
   TMercObj(TAreaDocument *doc);
   const virtual TMercObj *GetName(char *lpOut, int nOutSize) const = 0;

   TAreaDocument *AreaDoc;
};

// Help table types.
class THelpData: public TMercObj {
public:
   THelpData(TAreaDocument *doc);
   const virtual TMercObj *GetName(char *lpOut, int nOutSize) const;
   int operator==(const THelpData&) const;

   sh_int level;
   string keyword;
   string text;
};
typedef TMercPointer<THelpData> PHelpData;

// Shop types.
#define MAX_TRADE	 5
class TShopData: public TMercObj {
public:
    TShopData(TAreaDocument *doc);
    TShopData(const TShopData&);
    const virtual TMercObj *GetName(char *lpOut, int nOutSize) const;
    int operator==(const TShopData&) const;

    vnum_int 	keeper;			/* Vnum of shop keeper mob	*/
    sh_int 	buy_type [ MAX_TRADE ];	/* Item types shop will buy	*/
    sh_int 	profit_buy;		/* Cost multiplier for buying	*/
    sh_int 	profit_sell;		/* Cost multiplier for selling	*/
    sh_int 	open_hour;		/* First opening hour		*/
    sh_int 	close_hour;		/* First closing hour		*/
    string  comment;
};
typedef TMercPointer<TShopData> PShopData;

// 'where' definitions.  TO_OBJECT and TO_WEAPON are not used in area files
#define TO_AFFECTS	0
//#define TO_OBJECT	1
#define TO_IMMUNE	2
#define TO_RESIST	3
#define TO_VULN		4
//#define TO_WEAPON	5

// An affect.
class TAffect /*: public TMercObj*/ {
public:
    TAffect();
//    TAffect() { location = modifier = 0; }
    TAffect(const TAffect&);
    int operator==(const TAffect&) const;
    // These are not used in area files (except bitvector is in ROM, see below)
//    int32 		type;
//    int32 		duration;
//    int32		bitvector;
//    bool       deleted;
    sh_int 		location;
    sh_int 		modifier;

    // ROM 2.4 (We are not keeping 'where' and 'bitvector', even though ROM
    // uses them, as we store those bits on the object.
//    sh_int		where;
//    int32		bitvector;
};
typedef TMercPointer<TAffect> PAffect;

// ACT bits for mobs.
// Used in #MOBILES.
#define ACT_IS_NPC		   1		/* Auto set for mobs	*/
#define ACT_SENTINEL		   2		/* Stays in one room	*/
#define ACT_SCAVENGER		4		/* Picks up objects	*/
#define ACT_AGGRESSIVE	  32		/* Attacks PC's		*/
#define ACT_STAY_AREA	  64		/* Won't leave area	*/
#define ACT_WIMPY		    128		/* Flees when hurt	*/
#define ACT_PET			 256		/* Auto set for pets	*/
#define ACT_TRAIN		    512		/* Can train PC's	*/
#define ACT_PRACTICE		 1024		/* Can practice PC's	*/
// Envy-only
#define ACT_GAMBLE       2048    /* Runs a gambling game */
#define ACT_MOVED      131072    /* Dont ever set!       */
// TFC-only
#define ACT_SUPERWIMP    2048
#define ACT_ASSISTSAME   4096    /* Will assist same type of creature */
#define ACT_ASSIST       8192    /* Chance to assist all types */
#define ACT_ASSISTALWAYS 16384   /* Will always assist all types */
// Troll-only (@@@ These are not handled yet)
#define ACT_NOPUSH		      16384	   /* Cannot be pushed.  Walker   */
#define ACT_NOSUMMON		      32768L    /* Cannot be summoned.  Walker */
#define ACT_NOCHARM		      65536L    /* Cannot be charmed.  Walker  */
#define ACT_TAME		       1048576L		/* Can be tamed.  James */
#define ACT_MAYOR		       2097152L	/* Can change a home town.  Walker */
#define ACT_SWIMMER	       4194304L 	/* Mob can swim. Walker */
#define ACT_FULLSPEC	    1073741824L	/* Full MOB specification.  Walker */
// ROM-only
#define ACT_UNDEAD		      16384
#define ACT_CLERIC		      65536L
#define ACT_MAGE		        131072L
#define ACT_THIEF		        262144L
#define ACT_WARRIOR		     524288L
#define ACT_NOALIGN		    1048576L
#define ACT_NOPURGE		    2097152L
#define ACT_IS_HEALER		67108864L
#define ACT_GAIN		     134217728L
#define ACT_UPDATE_ALWAYS 268435456L   // @@@ What is this?
// ROM 2.4-only
#define ACT_IS_CHANGER	  536870912L


// Bits for 'affected_by'.
// Used in #MOBILES.  In ROM, also used in objects.
#define AFF_BLIND		            1     /*NON-TFC*/
#define AFF_INVISIBLE            2  /*Mob is invisible*/
#define AFF_DETECT_EVIL		      4     /*NON-TFC*/
#define AFF_DETECT_INVIS         8  /*Mob can see invisible players*/
#define AFF_DETECT_MAGIC	     16     /*NON-TFC*/
#define AFF_DETECT_HIDDEN       32  /*Mob can see 'hidden' players (thief skill)*/
#define AFF_HOLD		           64     /*NON-TFC*/
#define AFF_SANCTUARY          128  /*Mob takes half damage from opponent.*/
#define AFF_FAERIE_FIRE        256  /*Should not normally be used.*/
#define AFF_INFRARED           512  /*Mob can see in the dark.*/
#define AFF_CURSE		         1024     /*NON-TFC*/
#define AFF_FLAMING		      2048		/* Unused	*/
#define AFF_POISON		      4096     /* NON-TFC */
#define AFF_PROTECT		      8192     /* (old name for PROTECT_EVIL) */
#define AFF_PROTECT_EVIL      8192  /*Damage from an Evil source is reduced by 25%*/
#define AFF_PARALYSIS		  16384		/* Unused	*/
#define AFF_SNEAK            32768L /*Can not see mob entering/exiting a room*/
#define AFF_HIDE             65536L /*Mob is hidden.  Player needs detect hidden*/
												/*or other means to see a hidden mob*/
#define AFF_SLEEP		       131072L    /* NON-TFC */
#define AFF_CHARM		       262144L    /* NON-TFC */
#define AFF_FLYING          524288L    /*Mob can fly.*/
#define AFF_PASS_DOOR       1048576L   /*Mob can enter/exit through closed doors.*/
// Envy-only
#define AFF_WATERWALK           2097152L
#define AFF_SUMMONED            4194304L
#define AFF_MUTE                8388608L
#define AFF_GILLS              16777216L
// Envy 2.0-only
#define AFF_VAMP_BITE          33554432L	 //    (internally set)
#define AFF_GHOUL	             67108864L	 //    (internally set)
#define AFF_ENVYFLAMING       134217728L   // @@@
// TFC and ROM only
#define AFF_DETECT_GOOD          64  // Actually, this is in ROM too
#define AFF_PROTECT_GOOD      16384  // Damage from a Good source is reduced by 25%
// TFC-only
#define AFF_NOSTEAL            2048      /* was FLAMING   */
#define AFF_NOTRACE         2097152L
#define AFF_NOSLEEP         4194304L /*Mob not affected by sleep spell.*/
#define AFF_NOSUMMON        8388608L /*Mob can not be summoned.*/
#define AFF_NOCHARM        16777216L /*Mob can not be charmed.*/
#define AFF_ANTI_MAGIC     33554432L
#define AFF_INVULNERABLE   67108864L
#define AFF_IMPROVED_INVIS 134217728L
#define AFF_FIRESHIELD     268435456L
// Troll (@@@ These are not handled yet)
#define AFF_WIZINVIS	       33554432L	/* Support for WIZINVIS Mobs.  Walker */
#define AFF_ENDURANCE	    67108864L /* Ranger 'endurance' spell.  Walker */
#define AFF_WEB			    AFF_HOLD
#define AFF_TRUE_SIGHT		 AFF_GILLS   /* For True Sight spell.  Walker */
#define AFF_TAME	          AFF_FLAMING	/* Tamed mob. James. */
#define AFF_MOUNT	          AFF_PARALYSIS   /* For mounter AND mountie. James. */
// ROM-only
#define AFF_HASTE		        2097152L     // @@@ Not handled by MZF yet
#define AFF_CALM		        4194304L     // @@@ Not handled by MZF yet
#define AFF_PLAGUE           8388608L
#define AFF_WEAKEN		    16777216L
#define AFF_DARK_VISION		 33554432L
#define AFF_BERSERK		    67108864L
#define AFF_SWIM		      134217728L
#define AFF_REGENERATION   268435456L     // @@@ Not handled by MZF yet
// ROM 2.4-only @@@ not handled yet
#define AFF_SLOW		      536870912L

// ROM 2.3 size
#define SIZE_TINY			0
#define SIZE_SMALL			1
#define SIZE_MEDIUM			2
#define SIZE_LARGE			3
#define SIZE_HUGE			4
#define SIZE_GIANT			5


// Sex.
// Used in #MOBILES.
#define SEX_NEUTRAL		      0
#define SEX_MALE		         1
#define SEX_FEMALE		      2

// Rom 2.4 new sex type
#define SEX_EITHER            3


// Item types.
// Used in #OBJECTS.
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		         3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		   8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		  12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		  15
#define ITEM_DRINK_CON		  17
#define ITEM_KEY		        18
#define ITEM_FOOD		        19
#define ITEM_MONEY		     20
#define ITEM_BOAT		        22
#define ITEM_CORPSE_NPC		  23
#define ITEM_CORPSE_PC		  24
#define ITEM_FOUNTAIN		  25
#define ITEM_PILL		        26
// Troll
#define ITEM_PORTAL		     27
#define ITEM_RUNE		        28
// ROM ?
#define ITEM_PROTECT		     27
#define ITEM_MAP		        28
// ROM 2.4
#define ITEM_ROMPORTAL       29
#define ITEM_WARPSTONE       30
#define ITEM_UNK1            31
#define ITEM_GEM             32
#define ITEM_JEWELRY         33
#define ITEM_JUKEBOX		     34



// Extra flags.
// Used in #OBJECTS.
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_DARK		      4
#define ITEM_LOCK		      8
#define ITEM_EVIL		     16
#define ITEM_INVIS		  32
#define ITEM_MAGIC		  64
#define ITEM_NODROP		 128
#define ITEM_BLESS		 256
#define ITEM_ANTI_GOOD	 512
#define ITEM_ANTI_EVIL	1024
#define ITEM_ANTI_NEUTRAL 2048
#define ITEM_NOREMOVE	4096
#define ITEM_INVENTORY	8192
// Envy-only
#define ITEM_POISONED         16384
// Envy 2.0-only
#define ITEM_VAMPIRE_BANE     32768L   // @@@
#define ITEM_HOLY             65536L   // @@@
// TFC-only
#define ITEM_METALLIC  16384
// Troll (@@@ These are not handled yet)
#define ITEM_BONY		         32768L /* bony object flag for warrior reinforcement */
#define ITEM_ANTI_MAGE		 1048576L
#define ITEM_ANTI_CLERIC	 2097152L
#define ITEM_ANTI_WARRIOR	 4194304L
#define ITEM_ANTI_THIEF		 8388608L
#define ITEM_ANTI_PSI	   16777216L
#define ITEM_ANTI_RANGER   33554432L
#define ITEM_ANTI_DRUID    67108864L
// ROM
#define ITEM_NOPURGE		      16384
#define ITEM_ROT_DEATH		   32768L
#define ITEM_VIS_DEATH		   65536L
#define ITEM_NONMETAL		   262144L
#define ITEM_NOLOCATE		   524288L
#define ITEM_MELT_DROP		   1048576L
#define ITEM_HAD_TIMER		   2097152L
#define ITEM_SELL_EXTRACT	   4194304L
#define ITEM_BURN_PROOF		   16777216L
#define ITEM_NOUNCURSE		   33554432L

// Wear flags.
// Used in #OBJECTS.
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		      8192
#define ITEM_HOLD		        16384
// TFC only
#define ITEM_WORN_PRIDE      32768L
// ROM only (@@@ Not handled yet)
#define ITEM_TWO_HANDS		  32768L
// ROM 2.4 only @@@ not handled
#define ITEM_NO_SAC		     32768L    // Although this is a WEAR flag bit,
                                       // it seems to be used in ROM 2.4 as
                                       // an EXTRA flag would.  Doesn't
                                       // wield at all.
#define ITEM_WEAR_FLOAT		  65536L


// Apply types (for affects).
// Used in #OBJECTS.
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	  23
#define APPLY_SAVING_SPELL	     24
// Envy 2.0 only
#define APPLY_RACE              25 // @@@
// TFC only
#define APPLY_CHARISMA          25
#define APPLY_LUC               26 /* Luck */
#define APPLY_NOSTEAL           27
#define APPLY_NOSLEEP           28
#define APPLY_NOSUMMON          29 /* Also called blocking */
#define APPLY_NOCHARM           30 /* Also called steadfastness? */
#define APPLY_NOSEXCHANGE       31 /* Also called familiarity */
#define APPLY_TRUESEE           32
#define APPLY_NOINFO            33 /* Also called Obscurement */
// Troll
#define APPLY_ALIGN		        40
// ROM only @@@ not handled
#define APPLY_SPELL_AFFECT	     25

// Values for containers (value[1]).
// Used in #OBJECTS.
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8
// ROM 2.4 only
#define CONT_PUT_ON		     16


// Well known room virtual numbers.
#define ROOM_VNUM_TEMPLE	   3001

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		            1
#define ROOM_NO_MOB		         4
#define ROOM_INDOORS		         8
#define ROOM_PRIVATE		       512
#define ROOM_SAFE		         1024
#define ROOM_SOLITARY		   2048
#define ROOM_PET_SHOP		   4096
#define ROOM_NO_RECALL		   8192
// Envy-only
#define ROOM_CONE_OF_SILENCE 16384
// TFC only
//#define ROOM_TUNNEL             ??
#define ROOM_PEACEFUL        16384
// Troll
#define ROOM_BFS_MARK		  65536L  /* For tracking and hunting.  Walker */
#define ROOM_NO_MAGIC		 131072L  /* Rooms w/o magic.  Walker */
#define ROOM_ARENA		    262144L  /* Arena room.  Walker */
// ROM only (@@@ Not Imp)
#define ROOM_IMP_ONLY		  16384L
#define ROOM_GODS_ONLY		  32768L
#define ROOM_HEROES_ONLY	  65536L
#define ROOM_NEWBIES_ONLY	 131072L
#define ROOM_LAW		       262144L
// ROM 2.4 only
#define ROOM_NOWHERE		    524288L

// Troll
/*
 * Area flags.   NEW!!!
 * Used in #AREADATA.
 */
#define AREA_NO_FIX_EXITS	      1	 /* Do not call fix_exits().  Walker */

// Directions.
// Used in #ROOMS.
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5



// Exit flags.
// Used in #ROOMS.
#define EX_ISDOOR		            1
#define EX_CLOSED		            2
#define EX_LOCKED		            4
#define EX_PICKPROOF		        32
// Envy-only
#define EX_BASHED                8
#define EX_BASHPROOF            16
#define EX_PASSPROOF            64
// Troll (@@@ Not handled yet)
#define EX_HIDDEN		          128  /* Secret/unseen exits.  Walker */
// ROM only
#define EX_NOPASS		           64  // Same as Envy PASSPROOF
// Although ROM defines these flags, they are not actually in use
#define EX_EASY			       128
#define EX_HARD			       256
#define EX_INFURIATING		    512
#define EX_NOCLOSE		      1024
#define EX_NOLOCK		         2048

// Sector types.
// Used in #ROOMS.
#define SECT_INSIDE		      0
#define SECT_CITY		         1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		   5
#define SECT_WATER_SWIM		   6
#define SECT_WATER_NOSWIM	   7
#define SECT_AIR		         9
#define SECT_DESERT		     10
#define SECT_MAX		        11
// Envy-only
#define SECT_UNDERWATER  	   8



// Equpiment wear locations.
// Used in #RESETS.
#define WEAR_NONE		        -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		   1
#define WEAR_FINGER_R		   2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		         5
#define WEAR_HEAD		         6
#define WEAR_LEGS		         7
#define WEAR_FEET		         8
#define WEAR_HANDS		      9
#define WEAR_ARMS		        10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		        17
// Envy 2.0 only
#define WEAR_WIELD_2         18		/* by Thelonius */ // @@@
// TFC-only
#define WEAR_PRIDE	        18 /* Also called WEAR_DECOR */
#define WEAR_DECOR           18
//#define MAX_WEAR		     19
// ROM-only (2.4?)
#define WEAR_FLOAT		     18
#define MAX_WEAR		     19

#define UPPER( c ) ((char)toupper(c))
//#define UPPER( c )		( ( c ) >= 'a' && ( c ) <= 'z'                \
//				                ? ( c ) + 'A' - 'a' : ( c ) )

#define LOWER( c ) ((char)tolower(c))
//#define LOWER( c )		( ( c ) >= 'A' && ( c ) <= 'Z'                \
//				                ? ( c ) + 'a' - 'A' : ( c ) )

#define IS_SET( flag, bit )	( (( flag ) &   ( bit )) == ( bit ) )
#define SET_BIT( var, bit )	( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )	( ( var )  &= ~( bit ) )
#define IS_GOOD( ch )		( ch->alignment >=  350 )
#define IS_EVIL( ch )		( ch->alignment <= -350 )
//#define SET_RESET_BIT(orig,flg,toggle) \
//  { ((flg) ? SET_BIT(orig, toggle) : REMOVE_BIT(orig, toggle)) }
#define SET_RESET_BIT(orig,flg,toggle) { if (flg) SET_BIT(orig,toggle); else REMOVE_BIT(orig,toggle); }
//#define SET_RESET_BIT(orig, flg, toggle) ((flg) ? (orig) : (toggle))

/* dice */
#define DICE_NUMBER			0
#define DICE_TYPE			1
#define DICE_BONUS			2

/* ROM AC types */
#define AC_PIERCE			0
#define AC_BASH				1
#define AC_SLASH			2
#define AC_EXOTIC			3

class TVNumObj: public TMercObj {
public:
    TVNumObj(TAreaDocument *doc);
    TVNumObj(const TVNumObj& other);
    int operator==(const TVNumObj& other) const { return vnum == other.vnum; }

    vnum_int vnum;
    vnum_int new_vnum;
};

class TMobProgData: public TMercObj {
public:
    TMobProgData(TAreaDocument *doc);
    TMobProgData(TMobProgData& other);
    const virtual TMercObj *GetName(char *lpOut, int nOutSize) const;
    int operator==(const TMobProgData& other) const;
    const TMobProgData& operator=(const TMobProgData& other);
    static void DeleteAMobProg(TMobProgData *& p, void *);

//    MPROG_DATA *next;
//    int         type;
    string      type;
    string      arglist;
    string      comlist;
    string      MobFile;   // Filename of MOBprogam
};
typedef TMercPointer<TMobProgData> PMobProgData;

// Prototype for a mob.
// This is the in-memory version of #MOBILES.
class TMobile: public TVNumObj {
public:
    TMobile(TAreaDocument *);
    TMobile(const TMobile&);
    ~TMobile();
    const virtual TVNumObj *GetName(char *lpOut, int nOutSize) const;
	 char HUGE *MobileToText(char HUGE *) const;
    const TMobile& operator=(const TMobile&);
    BOOL is_affected(int32 sn) const { return IS_SET( affected_by, sn ); }
    int operator==(const TMobile& other) const;

    string     spec_fun;
    string     spec_comment;
    PShopData	pShop;
    string		player_name;
    string		short_descr;
    string		long_descr;
    string		description;
//	 sh_int 		vnum;
//	 int32 		count;
//	 int32 		killed;
    sh_int 		sex;
    sh_int 		level;
    int32		act;
    int32		affected_by;
    sh_int 		alignment;

    // The following are unused by Merc, we read and write them however
    int32 		hitroll;
    int32 		ac;
    int32 		hitnodice;
    int32 		hitsizedice;
    int32 		hitplus;
    int32 		damnodice;
    int32 		damsizedice;
    int32 		damplus;
    int32		gold;

    TArrayAsVector<TMobProgData*> mobprogs;

    // ROM 2.3
    sh_int		mana[3];
    sh_int		rom_ac[4];
    sh_int 		dam_type;
    int32		off_flags;
    int32		imm_flags;
    int32		res_flags;
    int32		vuln_flags;
    sh_int		start_pos;
    sh_int		default_pos;
    sh_int		race;
    int32	   form;
    int32	   parts;
    sh_int		size;
    string     material;

    // ROM 2.4
    vnum_int   mob_group;  // ROM mobile group
    string strDamType;

    // These are the exact opposites of act, affected_by, off_flags,
    // imm_flags, res_flags, vuln_flags, form, and race.  They are needed
    // because ROM 2.4 race gets into the picture--Slash
    int32      anti_action;
    int32      anti_affect;
    int32      anti_offensive;
    int32      anti_immunity;
    int32      anti_resistance;
    int32      anti_vulnerability;
    int32      anti_form;
    int32      anti_parts;

    // Envy 2.0 & ROM 2.4
    string      strrace;
};
typedef TMercPointer<TMobile> PMobile;

// Extra description data for a room or object.
class TExtraDescr /*: public TMercObj*/ {
public:
//    TExtraDescr(TAreaDocument *Doc);
    TExtraDescr(const TExtraDescr& other);
    TExtraDescr() {}
    int operator==(const TExtraDescr& other) const;

    string keyword;     /* Keyword in look/examine          */
    string description; /* What to see                      */
};
typedef TMercPointer<TExtraDescr> PExtraDescr;


// Prototype for an object.
class TObject: public TVNumObj {
public:
    TObject(TAreaDocument *Doc);
    TObject(const TObject&);
    ~TObject();
    const TObject& operator=(const TObject& other);
    int operator==(const TObject& other) const;

    const /*virtual*/ TVNumObj *GetName(char *lpOut, int nOutSize) const;
	 char HUGE *ObjToText(char HUGE *p) const;
    BOOL is_obj_stat(int32 stat) const { return IS_SET(extra_flags, stat); }

//    TListImp<TExtraDescr*> extra_descr;
    TArrayAsVector<TExtraDescr*> extra_descr;
//    TListImp<TAffect*> affected;
    TArrayAsVector<TAffect*> affected;
    string		name;
    string		short_descr;
    string		description;
    sh_int		item_type;
    int32 		extra_flags;
    int32 		wear_flags;
    int32 		count;
    int32 		weight;        // sh_int in Merc, int in Envy
    int32		cost;			/* Unused by Merc */
    string     value[5];   // Value[4] only used by ROM

    // ROM
    sh_int 		condition;
	 string     material;
	 sh_int     level;

    // These are not in ROM.  But they are used to 'collapse' the 'affected'
    // bits from the flags
    int32		affected_by;
    int32      immunity;
    int32      resistance;
    int32      vulnerability;

    // Rom 2.4
    string     strType;
};
typedef TMercPointer<TObject> PObject;

// Exit data.
class TExit: public TMercObj {
public:
	 TExit(TAreaDocument *doc=0);
	 int operator==(const TExit& other) const;

   TRoom      *to_room;
   vnum_int 	vnum;
   sh_int 		exit_info;
   vnum_int 	key;
   string		keyword;
   string		description;

	 TReset *InitialPosition;
	 int nInitialPosition; // 0 = open, 1 = closed, 2 = locked, 3 = leave alone
};
typedef TMercPointer<TExit> PExit;


/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

// Area-reset definition.
class TReset: public TMercObj {
public:
    TReset(TAreaDocument *doc);
    TReset(const TReset&);
    const virtual TMercObj *GetName(char *lpOut, int nOutSize) const;
    int operator==(const TReset& other) const;
    const TReset& operator=(const TReset&);

    char		command;
    sh_int 	arg1;
    sh_int 	arg2;
    sh_int 	arg3;
    string  comment;

    // ROM 2.4
    sh_int  arg4;
};
typedef TMercPointer<TReset> PReset;

class TMobReset: public TReset {
public:
    TMobReset(TAreaDocument *doc);
    TMobReset(const TMobReset&);
    const virtual TMercObj *GetName(char *lpOut, int nOutSize) const;
    int operator==(const TMobReset& other) const;
    const TMobReset& operator=(const TMobReset&);

	 TArrayAsVector<TReset *> Wields;
    TArrayAsVector<TReset *> Inventory;
};
typedef TMercPointer<TMobReset> PMobReset;

// Room type.
class TRoom: public TVNumObj {
public:
    TRoom(TAreaDocument *doc);
    TRoom(const TRoom& other);
    ~TRoom();
    const TRoom& operator=(const TRoom& other);
    int operator==(const TRoom& other) const;
    const virtual TVNumObj *GetName(char *lpOut, int nOutSize) const;
    char HUGE *RoomToText(char HUGE *) const;

	 TArrayAsVector<TExtraDescr*> extra_descr;
	 TExit      exit[6];
	 BOOL       is_exit[6];
	 string		name;
	 string		description;
	 int32 		room_flags;
	 int 		   sector_type;

    // ROM
    sh_int		heal_rate;
    sh_int 		mana_rate;
    string		clan;
    string     owner;

	 TArrayAsVector<TMobReset *> MobResets;
	 TArrayAsVector<TReset *> ObjResets;
	 TReset *RandomDoor;
};
typedef TMercPointer<TRoom> PRoom;

#if 0
/* Aff2 bits.  Can not be used to #MOBILES, since aff2 is not part of
	Diku structure. TFC only. */

#define AF2_ATTEMPT_BLYND   1
#define AF2_NOSEXCHANGE     2
#define AF2_NOINFO          4
#define AF2_DEAFNESS        8
#define AF2_SILENCE        16
#define AF2_WIZARD_MARK    32
#define AF2_FORGET         64
#define AF2_FEEBLEMIND    128
#define AF2_MINDBLANK     256
#define AF2_COMPREHEND    512
#define AF2_OBSCURE      1024
#define AF2_DISEASE      2048
#define AF2_CONFUSION    4096
#define AF2_TONGUES      8192
#define AF2_ONSET       16384
#endif

/*
 * Area definition. (Troll only)
 */
typedef struct	area_weather		AREA_WEATHER;

struct	area_weather
{
	int		MinTemp;
	int		MaxTemp;
	int		MinPrecip;
	int		MaxPrecip;
	int		MinWind;
	int		MaxWind;
	int		MinMagic;
	int		MaxMagic;
	int		MaxChange;
};

class TAreaData
{
public:
    BOOL fCustomWeather;
    AREA_WEATHER	Weather;	/* For NEW Weather System.  James. */
//    char *		name; // (Stored in TAreaDocument)
    string	repop;		/* From The Isles code.  Walker */
    int		area_flags;	/* Support area flags.  Walker */
//    int     recall;   // (Stored in TAreaDocument)
    int		timezone;	/* For NEW Weather System.  James. */
};

/*
// RT ASCII conversions -- used so we can have letters in this file
#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee			1073741824
*/

// ROM 2.3 Stuff
// OFF bits for mobiles
#define OFF_AREA_ATTACK            (1)
#define OFF_BACKSTAB               (2)
#define OFF_BASH                   (4)
#define OFF_BERSERK                (8)
#define OFF_DISARM                (16)
#define OFF_DODGE                 (32)
#define OFF_FADE                  (64)
#define OFF_FAST                 (128)
#define OFF_KICK                 (256)
#define OFF_KICK_DIRT            (512)
#define OFF_PARRY               (1024)
#define OFF_RESCUE              (2048)
#define OFF_TAIL                (4096)
#define OFF_TRIP                (8192)
#define OFF_CRUSH		          (16384)
// Assist bits for mobiles
#define ASSIST_ALL       	(32768L)
#define ASSIST_ALIGN	        (65536L)
#define ASSIST_RACE    	     	(131072L)
#define ASSIST_PLAYERS      	(262144L)
#define ASSIST_GUARD        	(524288L)
#define ASSIST_VNUM		(1048576L)

// return values for check_imm
#define IS_NORMAL		0
#define IS_IMMUNE		1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

// IMM bits for mobs
#define IMM_SUMMON              (1)       // A
#define IMM_CHARM               (2)       // B
#define IMM_MAGIC               (4)       // C
#define IMM_WEAPON              (8)       // D
#define IMM_BASH                (16)      // E
#define IMM_PIERCE              (32)      // F
#define IMM_SLASH               (64)      // G
#define IMM_FIRE                (128)     // H
#define IMM_COLD                (256)     // I
#define IMM_LIGHTNING           (512)     // J
#define IMM_ACID                (1024)    // K
#define IMM_POISON              (2048)    // L
#define IMM_NEGATIVE            (4096)    // M
#define IMM_HOLY                (8192)    // N
#define IMM_ENERGY              (16384)   // O
#define IMM_MENTAL              (32768L)  // P
#define IMM_DISEASE             (65536L)  // Q
#define IMM_DROWNING            (131072L) // R
#define IMM_LIGHT		           (262144L) // S

// RES bits for mobs
#define RES_CHARM		           (2)       // B
#define RES_MAGIC               (4)       // C
#define RES_WEAPON              (8)       // D
#define RES_BASH                (16)      // E
#define RES_PIERCE              (32)      // F
#define RES_SLASH               (64)      // G
#define RES_FIRE                (128)     // H
#define RES_COLD                (256)     // I
#define RES_LIGHTNING           (512)     // J
#define RES_ACID                (1024)    // K
#define RES_POISON              (2048)    // L
#define RES_NEGATIVE            (4096)    // M
#define RES_HOLY                (8192)    // N
#define RES_ENERGY              (16384)   // O
#define RES_MENTAL              (32768L)  // P
#define RES_DISEASE             (65536L)  // Q
#define RES_DROWNING            (131072L) // R
#define RES_LIGHT		           (262144L) // S

// VULN bits for mobs
#define VULN_MAGIC              (4)       // C
#define VULN_WEAPON             (8)       // D
#define VULN_BASH               (16)      // E
#define VULN_PIERCE             (32)      // F
#define VULN_SLASH              (64)      // G
#define VULN_FIRE               (128)     // H
#define VULN_COLD               (256)     // I
#define VULN_LIGHTNING          (512)     // J
#define VULN_ACID               (1024)    // K
#define VULN_POISON             (2048)    // L
#define VULN_NEGATIVE           (4096)    // M
#define VULN_HOLY               (8192)    // N
#define VULN_ENERGY             (16384)   // O
#define VULN_MENTAL             (32768L)  // P
#define VULN_DISEASE            (65536L)  // Q
#define VULN_DROWNING           (131072L) // R
#define VULN_LIGHT		        (262144L) // S
#define VULN_WOOD               (8388608L)   // T
#define VULN_SILVER             (16777216L)  // U
#define VULN_IRON		           (33554432L)  // V

// body form
#define FORM_EDIBLE             (1)    // A
#define FORM_POISON             (2)    // B
#define FORM_MAGICAL            (4)    // C
#define FORM_INSTANT_DECAY      (8)    // D
#define FORM_OTHER              (16)  // E - defined by material bit

// actual form
#define FORM_ANIMAL             (64)   // G
#define FORM_SENTIENT           (128)  // H
#define FORM_UNDEAD             (256)  // I
#define FORM_CONSTRUCT          (512)  // J
#define FORM_MIST               (1024) // K
#define FORM_INTANGIBLE         (2048) // L

#define FORM_BIPED              (4096) // M
#define FORM_CENTAUR            (8192) // N
#define FORM_INSECT             (16384)   // O
#define FORM_SPIDER             (32768L)  // P
#define FORM_CRUSTACEAN         (65536L)  // Q
#define FORM_WORM               (131072L) // R
#define FORM_BLOB		(262144L)            // S

#define FORM_MAMMAL             (2097152L)   // V
#define FORM_BIRD               (4194304L)   // W
#define FORM_REPTILE            (8388608L)   // X
#define FORM_SNAKE              (16777216L)  // Y
#define FORM_DRAGON             (33554432L)  // Z
#define FORM_AMPHIBIAN          (67108864L)  // aa
#define FORM_FISH               (134217728L) // bb
#define FORM_COLD_BLOOD		(268435456L)      // cc

// body parts
#define PART_HEAD               (1) // A
#define PART_ARMS               (2) // B
#define PART_LEGS               (4) // C
#define PART_HEART              (8) // D
#define PART_BRAINS             (16)   // E
#define PART_GUTS               (32)   // F
#define PART_HANDS              (64)   // G
#define PART_FEET               (128)  // H
#define PART_FINGERS            (256)  // I
#define PART_EAR                (512)  // J
#define PART_EYE		(1024)            // K
#define PART_LONG_TONGUE        (2048) // L
#define PART_EYESTALKS          (4096) // M
#define PART_TENTACLES          (8192) // N
#define PART_FINS               (16384)   // O
#define PART_WINGS              (32768L)  // P
#define PART_TAIL               (65536L)  // Q
// for combat
#define PART_CLAWS              (1048576L)   // U
#define PART_FANGS              (2097152L)   // V
#define PART_HORNS              (4194304L)   // W
#define PART_SCALES             (8388608L)   // X
#define PART_TUSKS		(16777216L)          // Y


#endif
