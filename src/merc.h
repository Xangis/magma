#ifndef merc_h
#define merc_h 1

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
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
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
// Set this one to 1 if you have a big problem that causes the MUD to
// explode and you can't figure it out.  It'll log all kinds of useless
// messages to help you track it down - Veygoth
//#define HARDCORE_DEBUG 1

#include <stdint.h>

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_PROG_FUN( fun )         void fun( )
#define DECLARE_MOB_FUN( fun )		bool fun( )
#define DECLARE_OBJ_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_GAME_FUN( fun ) 	void fun( )
#define DECLARE_HIT_FUN( fun )          void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_PROG_FUN( fun )         PROG_FUN  fun
#define DECLARE_MOB_FUN( fun )		MOB_FUN   fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN   fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_GAME_FUN( fun )		GAME_FUN  fun
#define DECLARE_HIT_FUN( fun )          HIT_FUN   fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined( FALSE )
#define FALSE	 0
#endif

#if	!defined( TRUE )
#define TRUE	 1
#endif

#if	defined( _AIX )
#if	!defined( const )
#define const
#endif
typedef int				unlong;
typedef int				bool;
#define unix
#else
typedef unsigned long			unlong;
typedef unsigned char			bool;
#endif


/*
 * Ok here we define strdup so it can no longer be confused
 * with str_dup.  Suggested by erwin@pip.dknet.dk - Kahn.
 */
#define strdup  STRDUP_ERROR__USE_STR_DUP!
/* Defined in ssm.c */

char *	str_dup		args( ( const char *str ) );


/*
 * Bit defines, so you don't have to recalculate/think up
 * all the bitvalues every time  -- Zen --
 */
#define		BV00			(1 <<  0)	/*          1 */
#define		BV01			(1 <<  1)	/*          2 */
#define		BV02			(1 <<  2)	/*          4 */
#define		BV03			(1 <<  3)	/*          8 */
#define		BV04			(1 <<  4)	/*         16 */
#define		BV05			(1 <<  5)	/*         32 */
#define		BV06			(1 <<  6)	/*         64 */
#define		BV07			(1 <<  7)	/*        128 */
#define		BV08			(1 <<  8)	/*        256 */
#define		BV09			(1 <<  9)	/*        512 */
#define		BV10			(1 << 10)	/*       1024 */
#define		BV11			(1 << 11)	/*       2048 */
#define		BV12			(1 << 12)	/*       4096 */
#define		BV13			(1 << 13)	/*       8192 */
#define		BV14			(1 << 14)	/*      16384 */
#define		BV15			(1 << 15)	/*      32768 */
#define		BV16			(1 << 16)	/*      65536 */
#define		BV17			(1 << 17)	/*     131072 */
#define		BV18			(1 << 18)	/*     262144 */
#define		BV19			(1 << 19)	/*     524288 */
#define		BV20			(1 << 20)	/*    1048576 */
#define		BV21			(1 << 21)	/*    2097152 */
#define		BV22			(1 << 22)	/*    4194304 */
#define		BV23			(1 << 23)	/*    8388608 */
#define		BV24			(1 << 24)	/*   16777216 */
#define		BV25			(1 << 25)	/*   33554432 */
#define		BV26			(1 << 26)	/*   67108864 */
#define		BV27			(1 << 27)	/*  134217728 */
#define		BV28			(1 << 28)	/*  268435456 */
#define		BV29			(1 << 29)	/*  536870912 */
#define		BV30			(1 << 30)	/* 1073741824 */
#define         BV31 _ERROR_DO_NOT_TRY_TO_USE_THE_SIGN_BIT_ON_A_SIGNED_INTEGER_

/*  BV31 is the sign bit, we'll leave it alone  - erwin@pip.dknet.dk  */

/*
 * Structure types.
 */
typedef struct	who_data		WHO_DATA;
typedef struct	affect_data		AFFECT_DATA;
typedef struct  bitvect_data            BITVECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct  crime_data              CRIME_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct  obj_char                OBJ_CHAR;
typedef struct  innate_timer_data       INNATE_TIMER_DATA;
typedef union   obj_char_room           OBJ_CHAR_ROOM; // union containing an object, char, or room, for use
                                                       // with progs, but may be useful elsewhere - Veygoth
typedef struct  prog_data               PROG_DATA;     // container for the union and the int specifying whats in
                                                       // the union - Veygoth
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct  mob_prog_data		MPROG_DATA;
typedef struct  mob_prog_act_list	MPROG_ACT_LIST;
typedef struct  act_prog_data		ACT_PROG_DATA;
typedef struct	soc_index_data		SOC_INDEX_DATA;
typedef struct	class_type		CLASS_TYPE;
typedef struct	noun_type		NOUN_TYPE;
typedef struct	verb_type		VERB_TYPE;
typedef struct  wear_data               WEAR_DATA;
typedef struct	clan_data		CLAN_DATA;
typedef struct	hunt_hate_fear		HHF_DATA;
typedef struct	system_data		SYSTEM_DATA;
typedef struct	game_data		GAME_DATA;
typedef struct	history_data		HISTORY_DATA;
typedef struct	alias_data		ALIAS_DATA;
typedef struct  exptable                EXP_TABLE;        // Veygoth
typedef struct  quest_data              QUEST_DATA;       // Veygoth
typedef struct  quest_index_data        QUEST_INDEX_DATA; // Veygoth
typedef struct  quest_item              QUEST_ITEM;       // Veygoth
typedef struct  talk_data               TALK_DATA;        // Veygoth
typedef struct  event_data		EVENT_DATA;	  // Veygoth
typedef struct  mem_data                MEM_DATA;         // Veygoth
typedef struct  trophy_data             TROPHY_DATA;      // Veygoth
typedef struct  coin_data               COIN_DATA;        // Veygoth
typedef struct  frag_data               FRAG_DATA;        // Veygoth
typedef struct  fragger_data            FRAGGER_DATA;     // Veygoth
typedef struct  vehicle_data            VEHICLE_DATA;     // Veygoth
typedef struct  person_data             PERSON_DATA;      // Veygoth
typedef struct  cast_data               CAST_DATA;        // Veygoth
typedef struct  trap_data               TRAP_DATA;        // Veygoth
typedef struct  extended_terrain	EXTENDED_TERRAIN; // Veygoth
typedef struct  member_data             MEMBER_DATA;      // Veygoth
typedef struct  movement_script         MOVE_SCRIPT;      // Veygoth - ships and mobiles both

/*
 * Function types.
 */
typedef	void DO_FUN                     args( ( CHAR_DATA *ch,
					       char *argument ) );
typedef bool MOB_FUN                    args( ( CHAR_DATA *ch, int cmd ) );
typedef bool OBJ_FUN                    args( ( OBJ_DATA *obj,
					       CHAR_DATA *keeper, bool hit ) );
typedef void SPELL_FUN                  args( ( int sn, int level,
					       CHAR_DATA *ch, void *vo ) );
typedef void GAME_FUN                   args( ( CHAR_DATA *ch, 
					       CHAR_DATA *croupier,
					       int amount, int cheat,
					       char *argument ) );
typedef bool HIT_FUN			args( ( CHAR_DATA *ch,
					       CHAR_DATA *victim, int hit,
					       int dam ) );


/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 32771 // 32771 is a prime number.  32767 was not.
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  299

#define	MAX_WORD_HASH		   27	/* The latin alphabet has 26 letters */


/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_CHUNKS                160			/* Used in ssm.c */

#define MAX_SKILL		   96
#define MAX_SPELL                 421
#define MAX_SONG                   28
#define MAX_MONK_SKILL            125
#define MAX_TRADITION               1
#define MAX_CARRY 		   10
#define MAX_ATTACK                 17
#define MAX_CLASS		   22
#define MAX_RACE                   77
#define MAX_COIN_RACE              40 // Last race to carry coins
#define MAX_PC_RACE                20
#define MAX_LEVEL		   65
#define MAX_MOB_LEVEL              65
#define MAX_CIRCLE                 12 // Number of spell circles
#define MAX_ALIAS		   10
#define MAX_HISTORY		   10
#define MAX_LAST_FUNS              24
#define MAX_LAST_COMS              10
#define MAX_CLAN                   30 // Maximum members per clan/guild
#define MAX_FRAG                   10 // Number of people to keep track of
                                      // on each fraglist
#define MAX_FLY                     3 // Highest level a person can fly, starting
                                      // at 0.
#define BASE_SPELL_ADEPT           80
#define BASE_SKILL_ADEPT           25
#define MAX_SKILL_ADEPT            95
#define MAX_SPELL_ADEPT            99
#define MAX_BASE_ATTRIBUTE         100
#define L_DIR		           MAX_LEVEL
    // = Overlord
#define L_SEN		          ( L_DIR - 1 )
    // = Implementor
#define L_JUN	        	  ( L_SEN - 1 )
    // = Greater God
#define L_APP                     ( L_JUN - 1 )
    // = Lesser God
#define L_DEM                     ( L_APP - 1 )
    // = Demigod
#define L_IMM                     ( L_DEM - 1 )
    // = Immortal
#define L_AVA                     ( L_IMM - 1 )
    // = Avatar
#define L_BUI			  ( L_AVA - 1 )
    // = Builder
#define LEVEL_IMMORTAL		  L_AVA
#define LEVEL_HERO		  ( LEVEL_IMMORTAL - 1 ) // Builders are basically Heros
#define L_HER			  LEVEL_HERO
// Veygoth - this is the number of bitvectors in use.  If you fill up
// another batch of vectors, just increment this by one and add the
// definitions
#define MAX_MORTAL_LEVEL          56
#define MAX_ADVANCE_LEVEL         50
#define NUM_AFFECT_VECTORS        5
#define NUM_ITEM_EXTRA_VECTORS    2
#define NUM_ROOM_FLAGS            2

// This definition is used for corpse saving.  They are saved with vnums
// so that they can be loaded back up over crashes and be allowed to
// decay after a while.  If there are enough zones to make this number a
// problem simply up the number.  The mud will start saving the corpses
// at the new number and after a reboot they will load at the higer vnums.
// -- Veygoth
#define CORPSE_VNUM               90000

#define PULSE_PER_SECOND	    4
#define PULSE_MEMORIZE              4
#define PULSE_AGGRESS               5
#define PULSE_VIOLENCE		  (    3 * PULSE_PER_SECOND )
#define PULSE_VIOLENCE_UP         (    PULSE_VIOLENCE / 2   )
#define PULSE_HITGAIN             (    5 * PULSE_PER_SECOND )
#define PULSE_MANAGAIN            (    4 * PULSE_PER_SECOND  + 1)
#define PULSE_MOVEGAIN            (    3 * PULSE_PER_SECOND )
#define PULSE_ROOM		  (    3 * PULSE_PER_SECOND )
#define PULSE_OBJECT		  (    9 * PULSE_PER_SECOND )
#define PULSE_SONG                (    8 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  (    7 ) // Actually is 7 seconds,
                                           // but every 7 pulses is a mob
                                           // lag update -- Veygoth
#define NUM_MOB_PULSES            4 // Used with PULSE_MOBILE to determine the number of
                                    // mob pulses that pass between movement and special
                                    // function checks. -- Veygoth
#define PULSE_LIST		  (   40 * PULSE_PER_SECOND )
// OBJ_UPDATE pulse decrements an object's timer, so an object
// with a timer of 6 will last 3 minutes.
#define PULSE_OBJ_UPDATE	  (   30 * PULSE_PER_SECOND )
#define PULSE_CHAR_UPDATE	  (   30 * PULSE_PER_SECOND )
#define PULSE_WEATHER  		  (  120 * PULSE_PER_SECOND )
#define PULSE_AREA		  (  300 * PULSE_PER_SECOND )
#define PULSE_SAVE_CORPSES        (  600 * PULSE_PER_SECOND ) /* 10 minutes */
#define PULSE_SAVE_SYSDATA        (  900 * PULSE_PER_SECOND ) /* 15 minutes */
#define PULSE_DB_DUMP             ( 1800 * PULSE_PER_SECOND ) /* 30 minutes */
#define PULSE_CAMP		  (    5 * PULSE_PER_SECOND ) /* Govno */
#define SURFACE_MAP1_START_VNUM    200000
#define SURFACE_MAP1_END_VNUM      252141
#define SURFACE_MAP1_WIDTH            330
#define SURFACE_MAP1_HEIGHT           158
#define SURFACE_MAP2_START_VNUM    260000   
#define SURFACE_MAP2_END_VNUM      271479
#define SURFACE_MAP2_WIDTH            129
#define SURFACE_MAP2_HEIGHT            89
#define UD_MAP1_START_VNUM         280000
#define UD_MAP1_END_VNUM           290863
#define UD_MAP1_WIDTH                  97
#define UD_MAP1_HEIGHT                112
#define UD_MAP2_START_VNUM         300000
#define UD_MAP2_END_VNUM           305528
#define UD_MAP2_WIDTH                  97
#define UD_MAP2_HEIGHT                 57
#define UD_MAP3_START_VNUM         320000
#define UD_MAP3_END_VNUM           327809
#define UD_MAP3_WIDTH                 142
#define UD_MAP3_HEIGHT                 55

// 158 * 330 = 52040 rooms for the surface map x 112 bytes per room = 5821984 bytes for surface map (5.8MB)
// virtual rooms may end up being a good idea, but they would be limited and yet one more silly data
// chunk to keep track of -- Veygoth

// Defines for struct data types.  Useful when passing (void *) pointers - Lithos
#define TYPE_CHAR_DATA       1
#define TYPE_OBJ_DATA        2
#define TYPE_AFFECT_DATA     3
#define TYPE_ROOM_INDEX_DATA 4



/*
 * Player character key data struct
 * Stuff for new error trapping of corrupt pfiles.
 */
struct  key_data
{
    char	    key[12];	/* Increase if you make a key > 11 chars */
    int		    string;	/* TRUE for string, FALSE for int        */
    unlong	    deflt;	/* Default value or pointer              */
    void *	    ptrs[11];	/* Increase if you have > 9 parms/line   */
};

#define MAND		3344556	/* Magic # for manditory field           */
#define SPECIFIED	3344557 /* Key was used already.                 */
#define DEFLT		3344558 /* Use default from fread_char_obj       */



/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};



/*
 * Time and weather stuff.
 * Note that weather changes according to the month (winter).
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3
#define MOON_RISE                   4
#define MOON_SET                    5

// Split moon phases out separately
#define MOON_NEW                    0
#define MOON_QUARTER                1
#define MOON_HALF                   2
#define MOON_THREEQUARTER           3
#define MOON_FULL                   4
#define MAX_MOON_PHASE              4

#define LUNAR_CYCLE_DAYS            32

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
    int         moonphase; // Phase of the moon
    int         moonday;   // Day # of the lunar cycle
    int		windspeed;
    int		winddir;
    int		temperature;
};

/*
 * Color originally by Lope of Loping Through The MUD
 *
 * Redone by Veygoth to support the &+ color codes.
 */
#define MOD_CLEAR       "[0m"     /* Resets Color (&n) */
#define MOD_BOLD        "[1m"
#define MOD_FAINT       "[2m"
#define MOD_UNDERLINE   "[4m"
#define MOD_BLINK       "[5m"
#define MOD_REVERSE     "[7m"
// Foreground Colors
#define FG_BLACK        "[0;30m"  /* (&+l) */
#define FG_RED          "[0;31m"  /* (&+r) */
#define FG_GREEN        "[0;32m"  /* (&+g) */
#define FG_YELLOW       "[0;33m"  /* (&+y) */
#define FG_BLUE         "[0;34m"  /* (&+b) */
#define FG_MAGENTA      "[0;35m"  /* (&+m) */
#define FG_CYAN         "[0;36m"  /* (&+c) */
#define FG_WHITE        "[0;37m"  /* (&+w) */
// Bold Foreground Colors
#define FG_B_BLACK      "[1;30m" /* (&+L) */
#define FG_B_RED        "[1;31m" /* (&+R) */
#define FG_B_GREEN      "[1;32m" /* (&+G) */
#define FG_B_YELLOW     "[1;33m" /* (&+Y) */
#define FG_B_BLUE       "[1;34m" /* (&+B) */
#define FG_B_MAGENTA    "[1;35m" /* (&+M) */
#define FG_B_CYAN       "[1;36m" /* (&+C) */
#define FG_B_WHITE      "[1;37m" /* (&+W) */
// Background Colors
#define BG_BLACK        "[40m"
#define BG_RED          "[41m"
#define BG_GREEN        "[42m"
#define BG_YELLOW       "[43m"
#define BG_BLUE         "[44m"
#define BG_MAGENTA      "[45m"
#define BG_CYAN         "[46m"
#define BG_WHITE        "[47m"

/*
 * Connected state for a channel.
 */
#define CON_PLAYING			0
#define CON_GET_NAME			1
#define CON_GET_OLD_PASSWORD		2
#define CON_CONFIRM_NEW_NAME		3
#define CON_GET_NEW_PASSWORD		4
#define CON_CONFIRM_NEW_PASSWORD	5
//#define CON_DISPLAY_RACE                6  No longer used
#define CON_GET_NEW_RACE                7
#define CON_CONFIRM_NEW_RACE            8
#define CON_GET_NEW_SEX                 9
//#define CON_DISPLAY_CLASS              10  No longer used
#define CON_GET_NEW_CLASS              11
#define CON_CONFIRM_CLASS              12
#define CON_GET_COLOR                  13
#define CON_ROLL_STATS                 14
#define CON_READ_MOTD                  15
#define CON_MENU                       16 // Veygoth
#define CON_FIRST_BONUS                17
#define CON_SECOND_BONUS               18
#define CON_THIRD_BONUS                19
#define CON_CHOOSE_HOMETOWN            20

#define CON_PASSWD_GET_OLD            -10
#define CON_PASSWD_GET_NEW            -11
#define CON_PASSWD_CONFIRM_NEW        -12
#define CON_RETIRE_GET_PASSWORD       -13
#define CON_RETIRE_CONFIRM            -14

#define IS_PLAYING( d )		( d->connected <= CON_PLAYING )


/*
 * Race structures
 */
struct  race_type
{
    char *              name;
    char *              colorname; 		// added by Veygoth
    char *              key;
    int                 race_abilities;
    int                 size;
    int                 height;
    int                 weight;
    int                 base_align;
    int                 str_mod;
    int                 int_mod;
    int                 wis_mod;
    int                 dex_mod;
    int                 con_mod;
    int                 agi_mod;
    int                 cha_mod;
    int                 pow_mod;
    int                 luk_mod;
    int                 hp_gain;
    int                 mana_gain;
    int                 move_gain;
    int                 thirst_mod;
    int                 hunger_mod;
    char *              dmg_message;
    char *              walk_message;
    char *              hate;
    int			parts;
    int			resistant;
    int			immune;
    int			susceptible;
    int			language;
    int                 racewar_side;
    int                 base_age;
    int                 max_age;
    int                 *race_gsn;
};

#define RACE_HUMAN	0
#define RACE_BARBARIAN	1
#define RACE_GREYELF    2
#define RACE_RAKSHASA   3
#define RACE_HALFELF	4
#define RACE_FLIND      5
#define RACE_DROW       6
#define RACE_DWARF	7
#define RACE_DUERGAR	8
#define RACE_HALFLING	9
#define RACE_TROLL	10
#define RACE_OGRE	11
#define RACE_ORC	12
#define RACE_GNOME	13
#define RACE_CENTAUR	14
#define RACE_GITHYANKI  15
#define RACE_GOBLIN     16
#define RACE_MINOTAUR   17
#define RACE_GITHZERAI  18
#define RACE_THRIKREEN  19

extern int RACE_ELEM_FIRE;
extern int RACE_ELEM_AIR;
extern int RACE_ELEM_WATER;
extern int RACE_ELEM_EARTH;
extern int RACE_UNDEAD;
extern int RACE_GHOST;
extern int RACE_VAMPIRE;
extern int RACE_GOD;
extern int RACE_DEMON;
extern int RACE_DEVIL;
extern int RACE_DRACOLICH;

#define RACEWAR_NONE     0
#define RACEWAR_GOOD     1
#define RACEWAR_EVIL     2
#define RACEWAR_NEUTRAL  3
#define MAX_RACEWAR_SIDE 4

#define RACE_RANGE_ANY    0
#define RACE_RANGE_PLAYER 1
#define RACE_RANGE_ANIMAL 2
#define RACE_RANGE_HUMANOID 3
#define RACE_RANGE_SLUG   4

/* Race size defines by Maniac */
#define SIZE_ANY		      0 
#define SIZE_TINY		      1 
#define SIZE_SMALL		      2  
#define SIZE_PETITE		      3 // (small-medium)
#define SIZE_MEDIUM		      4 // (straight medium - humans)
#define SIZE_BIG		      5 // (medium-large)
#define SIZE_LARGE		      6 
#define SIZE_HUGE		      7 
#define SIZE_GIANT                    8 
#define SIZE_TITANIC		      9
#define SIZE_GARGANTUAN		      10 
#define SIZE_NONE                     11 // insubstantial
#define MAX_SIZE		      11 // maximum size value

/* Race ability bits */
#define RACE_NO_ABILITIES	      0
#define RACE_PC_AVAIL		   BV00
#define RACE_WATERBREATH	   BV01
#define RACE_FLY		   BV02
#define RACE_SWIM		   BV03
#define RACE_WATERWALK		   BV04
#define RACE_PASSDOOR		   BV05
#define RACE_INFRAVISION	   BV06
#define RACE_DETECT_ALIGN	   BV07
#define RACE_DETECT_INVIS	   BV08
#define RACE_DETECT_HIDDEN	   BV09
#define RACE_EXTRA_ARMS		   BV10 // Thrikreen and the like - extra attacks
#define RACE_FAERIE_FIRE 	   BV11
#define RACE_WEAPON_WIELD	   BV12
#define RACE_MUTE                  BV13 
#define RACE_BODYSLAM              BV14
#define RACE_CHARGE                BV15
#define RACE_ULTRAVISION           BV16
#define RACE_DOORBASH              BV17
#define RACE_SHRUG                 BV18
#define RACE_ODSNEAK               BV19  // Not Implemented
#define RACE_UDSNEAK               BV20  // Not Implemented
#define RACE_STRENGTH              BV21  // Not Implemented
#define RACE_STEAL                 BV22  // Not Implemented
#define RACE_ENLARGE               BV23
#define RACE_INVIS                 BV24
#define RACE_SUMMON_HOARDE         BV25  // Not Implemented
#define RACE_SHIFT_PRIME           BV26  // Not Implemented
#define RACE_SHIFT_ASTRAL          BV27  // Not Implemented
#define RACE_LEVITATE              BV28  // Not Implemented
#define RACE_BITE                  BV29  // Not Implemented
#define RACE_EXTRA_STRONG_WIELD    BV30  // allows a strong race to wield a twohander primary
                                         // and a shield or onehander secondary

/* Language bits */
// Keep in mind that these are stored in pfiles as a number, so it's a bad idea
// to delete or change anything that already exists - Veygoth
#define LANG_UNKNOWN                  0  /* Anything else                    */
#define LANG_COMMON		      1  /* Human base language              */
#define LANG_ELVEN		      2  /* Elven base language              */
#define LANG_DWARVEN		      3  /* Dwarven base language            */
#define LANG_CENTAUR                  4
#define LANG_OGRE		      5  /* Ogre base language               */
#define LANG_ORC                      6  /* Orc base language                */
#define LANG_TROLL                    7  /* Troll base language              */
#define LANG_AQUA_ELF                 8
#define LANG_SAURIAL                  9
#define LANG_THRI_KREEN              10
#define LANG_DRAGON		     12  /* Large reptiles, Dragons          */
#define LANG_MAGICAL		     14  /* Spells maybe?  Magical creatures */
#define LANG_GOBLIN		     15  /* Goblin base language             */
#define LANG_GOD                     16
#define LANG_HALFLING		     17  /* Halfling base language           */
#define LANG_GITHYANKI		     18  /* Gith Language                    */
#define LANG_DROW		     19  /* Drow base language               */
#define LANG_KOBOLD		     20  /* Kobold base language             */
#define LANG_GNOME		     21  /* Gnome base language              */
#define LANG_ANIMAL		     22  /* Animal language                  */
#define LANG_DUERGAR		     23
#define LANG_GITHZERAI               24
#define LANG_FLIND                   25
#define LANG_RAKSHASA                26
#define LANG_MINOTAUR                27
#define LANG_GNOLL                   28
#define LANG_BARBARIAN               29
#define MAX_LANG                     30

#define MAX_NOUN                     32
#define MAX_VERB                     6
#define NO_DOBJ                      0
#define DOBJ_ANY                     1
#define DOBJ_ANIMATE                 2
#define NO_IOBJ                      0
#define IOBJ_ANY                     1
#define IOBJ_ANIMATE                 2

/* Special function defines
 *
 */
#define PROC_NORMAL                  0
#define PROC_DEATH                   1

/* Innate abilities
 * Used for innate timers
 */
#define INNATE_STRENGTH              1
#define INNATE_INVIS                 2
#define INNATE_FAERIE_FIRE           3
#define INNATE_ENLARGE               4
#define INNATE_LEVITATE              5
#define INNATE_DARKNESS              6
#define INNATE_LIGHT                 7
#define INNATE_REMOVE_POISON         8
#define INNATE_BATTLE_ROAR           9
#define INNATE_CHAMELEON             10
#define INNATE_SHIFT_ASTRAL          11
#define INNATE_SHIFT_PRIME           12

/*
 * Poison types
 */
#define POISON_NONE                  0
#define POISON_DAMAGE                1
#define POISON_ATTRIBUTES            2
#define POISON_DAMAGE_MAJOR          3
#define POISON_MINOR_PARA            4
#define POISON_MINOR_PARA_LONG       5
#define POISON_MAJOR_PARA            6
#define POISON_MAJOR_PARA_LONG       7
#define POISON_PERM_CON              8
#define POISON_PERM_HP               9
#define POISON_NEAR_DEATH           10


/*
 * Drunkeness communication structure.
 */
struct  struckdrunk
{
    int                 min_drunk_level;
    int                 number_of_rep;
    char               *replacement[11];
};


/*
 * History data structure.
 */
struct history_data
{
    HISTORY_DATA *	next;
    HISTORY_DATA *	prev;
    char *		comm;
};


/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    unsigned int        descriptor;
    int		        connected;
    bool		fcommand;
    bool                color;
    char		inbuf		[ MAX_INPUT_LENGTH*4 ];
    char		flusher		[ MAX_INPUT_LENGTH*4 ];
    char		incomm		[ MAX_INPUT_LENGTH   ];
    HISTORY_DATA *	infirst;
    HISTORY_DATA *	inlast;
    int			histsize;
    int			repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *              flush_point;
    char *		outbuf;
    int			outsize;
    int			outtop;
    void *              olc_editing;
    char **             str_editing;
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    int 	        tohit;
    int         	todam;
    int                 carry;
    int         	wield;
};

struct	int_app_type
{
    int         	learn;
};

struct	wis_app_type
{
    int         	practice;
};

struct	agi_app_type
{
    int         	defensive;
};

struct	con_app_type
{
    int         	hitp;
    int         	shock;
};



/*
 * TO types for act.
 *    TO_ROOM : all in room at ch's fly_level
 *    TO_NOTVICT : all in room at ch's fly_level except victim
 *    TO_VICT : to victim
 *    TO_CHAR : to char
 *    TO_ROOM_VICT : to all in room at victim's fly_level except ch
 *    TO_ALL : all in room regardless of fly level
 *    TO_ABOVE : all in room with fly_level > ch
 *    TO_BELOW: all in room with fly_level < ch
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ROOM_VICT        4
#define TO_ALL              5
#define TO_ABOVE            6
#define TO_BELOW            7

/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA * 	next;
    int 	        level;
    char *      	keyword;
    char *      	text;
    AREA_DATA *		area;
};



/*
 * Shop types.
 */
#define MAX_BUY		 10 // Max number of item types shop will buy
#define MAX_SELL         20 // Max number of items shop can sell.

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    int 	keeper;			/* Vnum of shop keeper mob	*/
    int 	buy_type [ MAX_BUY ];	/* Item types shop will buy	*/
    int         sell_item[ MAX_SELL ];  /* Items that shop is selling   */
    int 	profit_buy;		/* Cost multiplier for buying	*/
    int 	profit_sell;		/* Cost multiplier for selling	*/
    int 	open_hour;		/* First opening hour		*/
    int 	close_hour;		/* First closing hour		*/
};



struct	game_data
{
    GAME_DATA *	next;			/* Next game in list		*/
    int 	croupier;		/* Vnum of croupier mob		*/
    GAME_FUN *	game_fun;		/* Game fun run by mob		*/
    int		bankroll;		/* Amount of gold in the bank	*/
    int		max_wait;		/* Number of pulse to decision	*/
    bool	cheat;			/* True if mob cheats		*/
};

#define CLASS_NONE	    0
#define CLASS_WARRIOR       1
#define CLASS_SORCERER      2
#define CLASS_PSIONICIST    3
#define CLASS_CLERIC        4
#define CLASS_THIEF         5
#define CLASS_ASSASSIN      6
#define CLASS_MERCENARY     7
#define CLASS_PALADIN       8
#define CLASS_ANTIPALADIN   9
#define CLASS_RANGER        10
#define CLASS_ELEMENT_EARTH 11
#define CLASS_ELEMENT_AIR   12
#define CLASS_ELEMENT_FIRE  13
#define CLASS_ELEMENT_WATER 14
#define CLASS_SHAMAN        15
#define CLASS_DRUID         16
#define CLASS_BARD          17
#define CLASS_HUNTER        18
#define CLASS_ILLUSIONIST   19
#define CLASS_MONK          20
#define CLASS_MYSTIC        21
/*
 * Per-class stuff.
 */
struct	class_type
{
    char *	name;			/* Full class name		*/
    char *	who_name;		/* Three-letter name for 'who'	*/
    int 	attr_prime;		/* Prime attribute		*/
    int 	weapon;			/* First weapon			*/
    int 	skill_adept;		/* Maximum skill level		*/
    int 	thac0_00;		/* Thac0 for level  0		*/
    int 	thac0_47;		/* Thac0 for level 47		*/
    int  	hp_min;			/* Min hp gained on leveling	*/
    int	        hp_max;			/* Max hp gained on leveling	*/
    bool	fMana;			/* Class gains mana on level	*/
};


#define CLAN_UNDEFINED                0
#define CLAN_CLAN                     1
#define CLAN_GUILD                    2
#define CLAN_ORDER                    3

#define	RANK_EXILED		      0		/* Exiled from a clan   */
#define	RANK_PAROLE		      1		/* Foot soldier         */
#define	RANK_NORMAL		      2		/* Foot soldier         */
#define	RANK_SENIOR		      3		/* Knight               */
#define	RANK_OFFICER		      4		/* Knight Lord          */
#define	RANK_DEPUTY		      5		/* Second in command    */
#define	RANK_LEADER		      6		/* Head clan leader     */
#define MAX_RANK                      7         /* Total number of ranks*/

struct  clan_data
{
    CLAN_DATA * next;           /* next clan in list                    */
    CHAR_DATA * applicant;      /* Latest clan applicant                */
    char *      filename;       /* Clan filename                        */
    char *      who_name;       /* Clan who name                        */
    char *      name;           /* Clan name                            */
    char *      motto;          /* Clan motto                           */
    char *      description;    /* A brief description of the clan      */
    char *      overlord;       /* Leader's Name                        */
    MEMBER_DATA * member[MAX_CLAN]; /* Name, rank, fine, join time for members */
    char *      rank_name[MAX_RANK];
    char *      ostra;          /* Ostracized list                      */
    int         copper;
    int         silver;
    int         gold;
    int         platinum;
    int         frags;          /* Number of total frags clan has       */
    int         pkills;         /* Number of pkills on behalf of clan   */
    int         pdeaths;        /* Number of pkills against clan        */
    int         mkills;         /* Number of mkills on behalf of clan   */
    int         mdeaths;        /* Number of clan deaths due to mobs    */
    int         illegal_pk;     /* Number of illegal pk's by clan (justice) */
    int         score;          /* Overall score                        */
    int         clan_type;      /* See clan type defines                */
    int         members;        /* Number of clan members               */
    int         clanobj1;       /* Vnum of first clan obj (ring)        */
    int         clanobj2;       /* Vnum of second clan obj (shield)     */
    int         clanobj3;       /* Vnum of third clan obj (weapon)      */
    int         recall;         /* Vnum of clan's recall room           */
    int         donation;       /* Vnum of clan's donation pit          */
    int         class;          /* For guilds                           */
};

struct member_data
{
    char * name;
    int rank;
    int fine;
    int join_time;
    bool filled;
};

/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
    time_t      date_stamp;
};



/*
 * For do_who output.
 */
struct	who_data
{
    WHO_DATA *	prev;
    WHO_DATA *	next;
    char *	text;
};



/*
 * For alias output.
 */
struct	alias_data
{
    ALIAS_DATA * next;
    char *	 cmd;
    char *	 subst;
};

struct trophy_data
{
    int vnum;
    int number;
};


/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    int 		skill;
    int                 spell;
    int                 song;
    int 		duration;
    int 		location;
    int 		modifier;
    int			bitvector[NUM_AFFECT_VECTORS];
    int			level;
    bool                deleted;
};

struct bitvect_data
{
    const int group;
    const int vector;
};

/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    int                 number;
    int                 killed;
};



/*
 * Resistant Immune Susceptible flags.
 */
#define RIS_NONE		      0
#define RIS_FIRE		   BV00
#define RIS_COLD		   BV01
#define RIS_ELECTRICITY		   BV02
#define RIS_ENERGY		   BV03
#define RIS_ACID		   BV04
#define RIS_POISON		   BV05
#define RIS_CHARM		   BV06
#define RIS_MENTAL		   BV07
#define RIS_WHITE_MANA		   BV08
#define RIS_BLACK_MANA		   BV09
#define RIS_DISEASE		   BV10
#define RIS_DROWNING		   BV11
#define RIS_LIGHT		   BV12
#define RIS_SOUND		   BV13
#define RIS_MAGIC		   BV14
#define RIS_NONMAGIC		   BV15
#define RIS_SILVER		   BV16
#define RIS_IRON		   BV17
#define RIS_WOOD		   BV18
#define RIS_WEAPON		   BV19
#define RIS_BASH		   BV20
#define RIS_PIERCE		   BV21
#define RIS_SLASH		   BV22
#define RIS_GAS                    BV23


/*
 * Damage types.
 */
#define DAM_NONE                      0		/*  weapon damage types */
#define DAM_BASH		      1
#define DAM_PIERCE		      2
#define DAM_SLASH		      3

#define DAM_OTHER		   1000		/* magical damage types */
#define DAM_FIRE		   1001
#define DAM_COLD		   1002
#define DAM_ELECTRICITY		   1003
#define DAM_ACID		   1004
#define DAM_POISON		   1005
#define DAM_CHARM		   1006
#define DAM_MENTAL                 1007
#define DAM_ENERGY                 1008
#define DAM_WHITE_MANA		   1009
#define DAM_BLACK_MANA		   1010
#define DAM_DISEASE                1011
#define DAM_DROWNING               1012
#define DAM_LIGHT                  1013
#define DAM_SOUND		   1014
#define DAM_HARM		   1015
#define DAM_DISINTEGRATION         1016
#define DAM_GAS                    1017
#define DAM_ASPHYXIATION           1018
#define DAM_WIND                   1019
#define DAM_CRUSHING               1020
#define DAM_EARTH                  1021 // When it damn well *has* to be elemental earth damage
#define DAM_WATER                  1022 // When it damn well *has* to be elemental water damage

/*
 * Return values for check_ris().
 */
#define IS_NORMAL               0
#define IS_RESISTANT            1
#define IS_IMMUNE               2
#define IS_SUSCEPTIBLE          3



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000

struct attack_type
{
     char *	name;
     int *	wpn_gsn;
     int	dam_type;
     HIT_FUN *	hit_fun;
};



/*
 * School flags, used to define skill realm, to be implemented.
 */
#define SCHOOL_NONE                   0		/* no school realm required  */
#define SCHOOL_ABJURATION          BV00		/* abjuration magics	     */
#define SCHOOL_ALTERATION          BV01		/* alteration magics	     */
#define SCHOOL_CONJURATION         BV02		/* conjuration magics	     */
#define SCHOOL_SUMMONING           BV03		/* summoning magics	     */
#define SCHOOL_ILLUSION            BV04		/* illusionist magics	     */
#define SCHOOL_PHANTASM            BV05		/* phantasm projection realm */
#define SCHOOL_INVOCATION          BV06		/* invocation magics         */
#define SCHOOL_EVOCATION           BV07		/* evocative magics	     */
#define SCHOOL_ENCHANTMENT         BV08		/* enchantment magics	     */
#define SCHOOL_CHARM               BV09		/* charm skills		     */
#define SCHOOL_DIVINATION          BV10		/* divinatory arts	     */
#define SCHOOL_NECROMANCY          BV11		/* necromantic skills	     */
#define SCHOOL_OFFENSIVE           BV13		/* offensive martial arts    */
#define SCHOOL_DEFENSIVE           BV14		/* defensive martial arts    */
#define SCHOOL_STEALTH             BV15		/* stealth related skills    */
#define SCHOOL_SURVIVAL            BV16		/* wilderness suvival skills */
/*
 * Shaman school flags added by Veygoth
 * These will be used to determine the type of totem needed to cast them.
 */
#define SCHOOL_ELEMENTAL	   BV17
#define SCHOOL_SPIRITUAL	   BV18
#define SCHOOL_ANIMAL		   BV19
/*
 * Bard flags added by Veygoth.  These will determine whether a spell
 * is primarily of a particular instrument.  Those that can take more than
 * one instrument type will have them specified here.  The mana type will
 * be the primary (most powerful) instrument.  All others will be about
 * 50% as effective
 */
#define SCHOOL_HORN                BV20
#define SCHOOL_FLUTE               BV21
#define SCHOOL_MANDOLIN            BV22
#define SCHOOL_LYRE                BV23
#define SCHOOL_DRUMS               BV24
#define SCHOOL_HARP                BV25
#define SCHOOL_PIPES               BV26
#define SCHOOL_FIDDLE              BV27
#define SCHOOL_DULCIMER            BV28

/*
 * Mana types.
 */
#define MANA_ANY                     -1
#define MANA_NONE                     0
#define MANA_EARTH                 BV00
#define MANA_AIR                   BV01
#define MANA_FIRE                  BV02
#define MANA_WATER                 BV03
// Bard mana types
#define MANA_HORN                BV11
#define MANA_FLUTE               BV12
#define MANA_MANDOLIN            BV13
#define MANA_LYRE                BV14
#define MANA_DRUMS               BV15
#define MANA_HARP                BV16
#define MANA_PIPES               BV17
#define MANA_FIDDLE              BV18
#define MANA_DULCIMER            BV19
#define MANA_INSTRUMENT          BV20 // Instrument required (voice not necessary unless flagged)
#define MANA_VOICE               BV21 // Voice requireed
#define MANA_DANCE               BV22 // Dance required
#define MANA_INSTR_AUGMENT       BV23 // Augmented by instrument
#define MANA_DANCE_AUGMENT       BV24 // Augmented by dance
#define MANA_VOICE_AUGMENT       BV25 // Augmented by voice


/*
 *  Target types.
 */
#define TAR_IGNORE		    0 // Generally used for area effects
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OBJ_HERE                5
#define TAR_TRAP                    6 // Not castable, used for traps
#define TAR_OBJ_NECRO		    7 // Obj in room, 3rd argument needed Govno
#define TAR_OBJ_CHAR                8 // Object or Character
#define TAR_CHAR_RANGE              9 // Offensive spell at a character, rangeable
#define TAR_CHAR_WORLD             10 // For spells such as relocate, portals, etc

// Obj-Char-room union, for use with mobprogs/objprogs/roomprogs
// -- Veygoth
union obj_char_room
{
	CHAR_DATA *ch;
	OBJ_DATA *obj;
	ROOM_INDEX_DATA *room;
};

struct prog_data
{
        int type;
        OBJ_CHAR_ROOM var;
};

// Union types for prog data
#define UNION_NONE 0
#define UNION_MOB  1
#define UNION_OBJ  2
#define UNION_ROOM 3

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers, mostly in limbo.are
 * Defined in #MOBILES.
 */
#define MOB_VNUM_MIRROR_IMAGE         1
#define MOB_VNUM_CLONE                2
#define MOB_VNUM_SHADOW_MONSTER       3
#define MOB_VNUM_DEMI_SHADOW_MONSTER  4
#define MOB_VNUM_SHADE                5
#define MOB_VNUM_OBJECT               6

#define MOB_VNUM_SKELETON	    240
#define MOB_VNUM_ZOMBIE		    241
#define MOB_VNUM_GHOUL		    242
#define MOB_VNUM_WIGHT		    243
#define MOB_VNUM_WRAITH		    244
#define MOB_VNUM_SHADOW		    245
#define MOB_VNUM_SPECTRE	    246
#define MOB_VNUM_LICH		    247
#define MOB_VNUM_VAMPIRE 	    248
#define MOB_VNUM_MUMMY		    249
#define MOB_VNUM_RED_DRACO    	    250
#define MOB_VNUM_BLUE_DRACO    	    251
#define MOB_VNUM_BLACK_DRACO        252
#define MOB_VNUM_GREEN_DRACO        253
#define MOB_VNUM_WHITE_DRACO        254

#define MOB_VNUM_CITYGUARD	   3060
/* #define MOB_VNUM_VAMPIRE	   3404 Govno removed until I figure out why its here.
                                        The vnum is wrong at any rate. 3404 is a ghast. */
#define MOB_VNUM_SECRETARY         3142
#define MOB_VNUM_MIDGAARD_MAYOR    3143

/*  These numbers don't jive with vnums of mobs in actual
    code.  Those in testport are same as in limbo.are
    as shown below  -Lithos
#define MOB_VNUM_AIR_ELEMENTAL     8914
#define MOB_VNUM_EARTH_ELEMENTAL   8915
#define MOB_VNUM_WATER_ELEMENTAL   8916
#define MOB_VNUM_FIRE_ELEMENTAL    8917
#define MOB_VNUM_DUST_ELEMENTAL    8918
*/
#define MOB_VNUM_AIR_ELEMENTAL     203
#define MOB_VNUM_EARTH_ELEMENTAL   202
#define MOB_VNUM_WATER_ELEMENTAL   201
#define MOB_VNUM_FIRE_ELEMENTAL    200
#define MOB_VNUM_DUST_ELEMENTAL    204
#define MOB_VNUM_SMOKE_ELEMENTAL    205
#define MOB_VNUM_ICE_ELEMENTAL    206
#define MOB_VNUM_OOZE_ELEMENTAL    207
#define MOB_VNUM_MAGMA_ELEMENTAL    208
#define MOB_VNUM_PLASMA_ELEMENTAL    209
#define MOB_VNUM_ASH_ELEMENTAL    210
#define MOB_VNUM_STEAM_ELEMENTAL    211
#define MOB_VNUM_SALT_ELEMENTAL    212
#define MOB_VNUM_LIGHTNING_ELEMENTAL    213
#define MOB_VNUM_VACUUM_ELEMENTAL    214
#define MOB_VNUM_MINERAL_ELEMENTAL    215
#define MOB_VNUM_CELESTIAL_ELEMENTAL    216
#define MOB_VNUM_CHAOS_ELEMENTAL    217
#define MOB_VNUM_FIRE_SERPENT      300
#define MOB_VNUM_WATER_NERNEID     301
#define MOB_VNUM_EARTH_PECH        302
#define MOB_VNUM_AIR_SLYPH         303

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_SIZEMINUS              BV00
#define ACT_SENTINEL		   BV01		/* Stays in one room	*/
#define ACT_SCAVENGER		   BV02		/* Picks up objects	*/
#define ACT_IS_NPC		   BV03		/* Auto set for mobs	*/
#define ACT_NICE_THIEF             BV04
#define ACT_AGGRESSIVE		   BV05		/* Attacks PC's		*/
#define ACT_STAY_AREA		   BV06		/* Won't leave area	*/
#define ACT_WIMPY		   BV07		/* Flees when hurt	*/
#define ACT_AGGROEVIL              BV08
#define ACT_AGGROGOOD              BV09
#define ACT_AGGRONEUT              BV10
#define ACT_MEMORY                 BV11
#define ACT_NOPARA                 BV12
#define ACT_NOSUMMON               BV13
#define ACT_NOBASH                 BV14
#define ACT_TEACHER                BV15
#define ACT_OUTLAW                 BV16
#define ACT_CANFLY                 BV17
#define ACT_CANSWIM                BV18
#define ACT_CANT_TK                BV19
#define ACT_MOVED                  BV20 // Ingame - Not settable in DE
#define ACT_PET                    BV21 // Ingame - Not settable in DE
#define ACT_NOEXP                  BV22
#define ACT_SIZEPLUS               BV23
#define ACT_WITNESS                BV24
#define ACT_NOCHARM                BV25
#define ACT_PROTECTOR              BV26
#define ACT_MOUNT                  BV27
#define ACT_AGGROEVILRACE          BV28
#define ACT_AGGROGOODRACE          BV29
#define ACT_HUNTER                 BV30
//#define ACT_AGGROOUTCAST           BV30

/* 
 * Body parts.
 */
#define PART_NONE		      0
#define PART_HEAD		   BV00
#define PART_ARMS		   BV01
#define PART_LEGS		   BV02
#define PART_HEART		   BV03
#define PART_BRAINS		   BV04
#define PART_GUTS		   BV05
#define PART_HANDS		   BV06
#define PART_FEET		   BV07
#define PART_FINGERS		   BV08
#define PART_EAR		   BV09
#define PART_EYE		   BV10
#define PART_LONG_TONGUE	   BV11
#define PART_EYESTALKS		   BV12
#define PART_TENTACLES		   BV13
#define PART_FINS		   BV14
#define PART_WINGS		   BV15
#define PART_TAIL		   BV16
/*
 * Body parts used in combat.
 */
#define PART_CLAWS		   BV17
#define PART_FANGS		   BV18
#define PART_HORNS		   BV19
#define PART_SCALES		   BV20
#define PART_TUSKS		   BV21
// Plants
#define PART_BARK                  BV22
#define PART_LEAVES                BV23
#define PART_BRANCHES              BV24
#define PART_TRUNK                 BV25
// Trophies
#define PART_SCALP                 BV26
#define PART_CRANIAL_CHITIN        BV27

/*
 * Defines for affect types.
 * The plan (my nafarious plane, mwahaahaahaa) is to change
 * affects to use one of the following affect defines rather
 * than skill, spell, song. 
 * - Lithos
 */
#define AFFECT_SKILL     1
#define AFFECT_SPELL     2
#define AFFECT_SONG      3
#define AFFECT_INNATE    4
#define AFFECT_UNKNOWN   5


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
// Bits have changed from an integer to a structure to allow for easy
// expansion of bitvectors, since anything that handles these affect bits
// will be passed the number of the variable that it is part of.
//
// If additional bitvectors are added, the loading code in db.c will
// set them to zero automatically.  It will be up to you to modify the
// file formats to load higher bitvectors - Veygoth
//
// values are set in bit.c
extern const BITVECT_DATA AFF_NONE;
extern const BITVECT_DATA AFF_AWARE;
extern const BITVECT_DATA AFF_BARKSKIN;
extern const BITVECT_DATA AFF_BATTLE_ECSTASY;
extern const BITVECT_DATA AFF_BLIND;
extern const BITVECT_DATA AFF_BLUR;
extern const BITVECT_DATA AFF_BOUND;
extern const BITVECT_DATA AFF_CAMPING;
extern const BITVECT_DATA AFF_CASTING;
extern const BITVECT_DATA AFF_CHANGE_SELF;
extern const BITVECT_DATA AFF_CHANGE_SEX;
extern const BITVECT_DATA AFF_CHARM;
extern const BITVECT_DATA AFF_CLIMBING;
extern const BITVECT_DATA AFF_COLDSHIELD;
extern const BITVECT_DATA AFF_COMP_LANG;
extern const BITVECT_DATA AFF_CURSE;
extern const BITVECT_DATA AFF_DAZZLE;
extern const BITVECT_DATA AFF_DENY_EARTH;
extern const BITVECT_DATA AFF_DENY_AIR;
extern const BITVECT_DATA AFF_DENY_FIRE;
extern const BITVECT_DATA AFF_DENY_WATER;
extern const BITVECT_DATA AFF_DETECT_EVIL;
extern const BITVECT_DATA AFF_DETECT_GOOD;
extern const BITVECT_DATA AFF_DETECT_HIDDEN;
extern const BITVECT_DATA AFF_DETECT_INVIS;
extern const BITVECT_DATA AFF_DETECT_MAGIC;
extern const BITVECT_DATA AFF_DETECT_UNDEAD;
extern const BITVECT_DATA AFF_ELEM_SIGHT;
extern const BITVECT_DATA AFF_FAERIE_FIRE;
extern const BITVECT_DATA AFF_FARSEE;
extern const BITVECT_DATA AFF_FEAR;
extern const BITVECT_DATA AFF_FLAMING;
extern const BITVECT_DATA AFF_FLYING;
extern const BITVECT_DATA AFF_GHOUL;
extern const BITVECT_DATA AFF_GILLS;
extern const BITVECT_DATA AFF_GREATER_SPIRIT_WARD;
extern const BITVECT_DATA AFF_HASTE;
extern const BITVECT_DATA AFF_HIDE;
extern const BITVECT_DATA AFF_HOLD;
extern const BITVECT_DATA AFF_HOLY_SACRIFICE;
extern const BITVECT_DATA AFF_INCOMPETENCE;
extern const BITVECT_DATA AFF_INERTIAL_BARRIER;
extern const BITVECT_DATA AFF_INFRARED;
extern const BITVECT_DATA AFF_INVISIBLE;
extern const BITVECT_DATA AFF_IS_FLEEING;
extern const BITVECT_DATA AFF_JUSTICE_TRACKER;
extern const BITVECT_DATA AFF_LAYHANDS_TIMER;
extern const BITVECT_DATA AFF_LEVITATE;
extern const BITVECT_DATA AFF_MAJOR_GLOBE;
extern const BITVECT_DATA AFF_MINOR_GLOBE;
extern const BITVECT_DATA AFF_MINOR_INVIS;
extern const BITVECT_DATA AFF_MINOR_PARA;
extern const BITVECT_DATA AFF_MISDIRECTION;
extern const BITVECT_DATA AFF_MUTE;
extern const BITVECT_DATA AFF_PASS_DOOR;
extern const BITVECT_DATA AFF_PLAGUE;
extern const BITVECT_DATA AFF_POISON;
extern const BITVECT_DATA AFF_PROTECT_ACID;
extern const BITVECT_DATA AFF_PROTECT_COLD;
extern const BITVECT_DATA AFF_PROTECT_EVIL;
extern const BITVECT_DATA AFF_PROTECT_FIRE;
extern const BITVECT_DATA AFF_PROTECT_GAS;
extern const BITVECT_DATA AFF_PROTECT_GOOD;
extern const BITVECT_DATA AFF_PROTECT_LIGHTNING;
extern const BITVECT_DATA AFF_PROWESS;
extern const BITVECT_DATA AFF_POLYMORPH;
extern const BITVECT_DATA AFF_SANCTUARY;
extern const BITVECT_DATA AFF_SENSE_LIFE;
extern const BITVECT_DATA AFF_SINGING;
extern const BITVECT_DATA AFF_SKL_AWARE;
extern const BITVECT_DATA AFF_SLEEP;
extern const BITVECT_DATA AFF_SLOW_POISON;
extern const BITVECT_DATA AFF_SLOWNESS;
extern const BITVECT_DATA AFF_SNEAK;
extern const BITVECT_DATA AFF_SOULSHIELD;
extern const BITVECT_DATA AFF_SPIRIT_WARD;
extern const BITVECT_DATA AFF_STONESKIN;
extern const BITVECT_DATA AFF_SUMMONED;
extern const BITVECT_DATA AFF_SUMMON_MOUNT_TIMER;
extern const BITVECT_DATA AFF_TRACK;
extern const BITVECT_DATA AFF_ULTRAVISION;
extern const BITVECT_DATA AFF_VACANCY;
extern const BITVECT_DATA AFF_VAMP_BITE;
extern const BITVECT_DATA AFF_VAMP_TOUCH;
extern const BITVECT_DATA AFF_WATERWALK;

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 *
 * Most of these objects should be found
 * in limbo.are which contains vnums below
 * 1000.
 */
#define OBJ_VNUM_OBJECT               1
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3
#define OBJ_VNUM_51_POTION            5
#define OBJ_VNUM_52_POTION            6
#define OBJ_VNUM_53_POTION            7
#define OBJ_VNUM_54_POTION            8
#define OBJ_VNUM_55_POTION            9
#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16
//#define OBJ_VNUM_GUTS		     17
//#define OBJ_VNUM_BRAINS		     18
#define OBJ_VNUM_IRON_RATION         20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_WATERSKIN	     22
#define OBJ_VNUM_PORTAL              23
#define OBJ_VNUM_SPRING              25
#define OBJ_VNUM_WINDSABER           26
#define OBJ_VNUM_CELESTIAL_SWORD     27
#define OBJ_VNUM_STONES              28
#define OBJ_VNUM_GATE                29
#define OBJ_VNUM_MOONWELL            30
#define OBJ_VNUM_WORMHOLE            31
#define OBJ_VNUM_HYPNOTIC_PATTERN    47
#define OBJ_VNUM_WALL_IRON           50
#define OBJ_VNUM_WALL_STONE          51
#define OBJ_VNUM_WALL_FIRE           52
#define OBJ_VNUM_WALL_ILLUSION       53
#define OBJ_VNUM_WALL_ICE            54
#define OBJ_VNUM_WALL_FORCE          55
#define OBJ_VNUM_LIGHTNING_CURTAIN   56
#define OBJ_VNUM_WALL_SPARKS         57
#define OBJ_VNUM_WALL_MIST           58
// Newbie items
// New design allows a start vnumber and a range for random newbie eq.
#define OBJ_VNUM_TORCH               19
#define OBJ_VNUM_QUILL               24
#define OBJ_VNUM_NEWBIE_VEST        100
#define NUM_NEWBIE_VEST              10
#define OBJ_VNUM_NEWBIE_HELM        110
#define NUM_NEWBIE_HELM               5
#define OBJ_VNUM_NEWBIE_SLEEVES     120
#define NUM_NEWBIE_SLEEVES            3
#define OBJ_VNUM_NEWBIE_PANTS       130
#define NUM_NEWBIE_PANTS              3
#define OBJ_VNUM_NEWBIE_BOOTS       140
#define NUM_NEWBIE_BOOTS              2
#define OBJ_VNUM_NEWBIE_CLOAK       150
#define NUM_NEWBIE_CLOAK              2
#define OBJ_VNUM_NEWBIE_SWORD       200
#define NUM_NEWBIE_SWORD             10
#define OBJ_VNUM_NEWBIE_DAGGER      210
#define NUM_NEWBIE_DAGGER            13
#define OBJ_VNUM_NEWBIE_2HSWORD     223
#define NUM_NEWBIE_2HSWORD           16
#define OBJ_VNUM_NEWBIE_KNIFE       239
#define NUM_NEWBIE_KNIFE              4
#define OBJ_VNUM_NEWBIE_SHORTSWORD  243
#define NUM_NEWBIE_SHORTSWORD        11
#define OBJ_VNUM_NEWBIE_MACE        254
#define NUM_NEWBIE_MACE              10
#define OBJ_VNUM_NEWBIE_STAFF       264
#define NUM_NEWBIE_STAFF             10
#define OBJ_VNUM_NEWBIE_TOTEM_A      41
#define OBJ_VNUM_NEWBIE_TOTEM_E      42
#define OBJ_VNUM_NEWBIE_TOTEM_S      43
#define OBJ_VNUM_NEWBIE_BACKPACK    286

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define TYPE_LIGHT                    1
#define TYPE_SCROLL                   2
#define TYPE_WAND                     3
#define TYPE_STAFF                    4
#define TYPE_WEAPON                   5
#define TYPE_RANGED_WEAPON            6 // was 31
#define TYPE_MISSILEWEAP              7
#define TYPE_TREASURE                 8
#define TYPE_ARMOR                    9
#define TYPE_POTION                  10
#define TYPE_CLOTHING		     11 // not armor
#define TYPE_OTHER                   12
#define TYPE_TRASH                   13
#define TYPE_TRAP                    14
#define TYPE_CONTAINER               15
#define TYPE_NOTE                    16
#define TYPE_DRINK_CON               17
#define TYPE_KEY                     18
#define TYPE_FOOD                    19
#define TYPE_MONEY                   20
#define TYPE_PEN                     21 // includes quill
#define TYPE_BOAT                    22
//#define TYPE_BOOK                    23 Was in DE for a long time but never used
#define TYPE_BATTERY                 24  // don't think this will be used.
#define TYPE_TELEPORT                25
#define TYPE_TIMER                   26
#define TYPE_VEHICLE                 27
#define TYPE_SHIP                    28
#define TYPE_SWITCH                  29
#define TYPE_QUIVER	             30
#define TYPE_LOCKPICK                31
#define TYPE_INSTRUMENT              32
#define TYPE_SPELLBOOK               33
#define TYPE_TOTEM                   34
#define TYPE_STORAGECHEST            35
#define TYPE_SCABBARD                36
#define TYPE_SHIELD                  37
#define TYPE_CORPSE_NPC              38 // was 23
#define TYPE_CORPSE_PC               39 // was 24
#define TYPE_PORTAL                  40
#define TYPE_PILL                    41 // was 26 
#define TYPE_HERB                    42 // Sniktiorg's smokeables, use like potion but smoked
#define TYPE_MESSAGE_BOARD           43
#define TYPE_PIPE                    44 // Sniktiorg's smokeables
#define TYPE_WANTED_LIST             45
#define TYPE_WALL                    46
// 47 unused
#define TYPE_AMMO                    48 // was 32
// 49 unused
#define TYPE_ROPE                    50 // For mercs to capture with.
#define MAX_ITEM_TYPE                51

// Totem types
#define TOTEM_L_ANIMAL		BV00
#define TOTEM_G_ANIMAL		BV01
#define TOTEM_L_ELEMENTAL       BV02
#define TOTEM_G_ELEMENTAL	BV03
#define TOTEM_L_SPIRIT		BV04
#define TOTEM_G_SPIRIT		BV05

// Edit types, used in string_append and related edit functions
#define EDIT_NONE            0
#define EDIT_DESCRIPTION     1
#define EDIT_CLAN_DESC       2
#define EDIT_NOTE            3
#define EDIT_OTHER           4

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
// Veygoth: modified to fit Magma area format.
//
extern const BITVECT_DATA ITEM_GLOW;
extern const BITVECT_DATA ITEM_NOSHOW;
extern const BITVECT_DATA ITEM_BURIED; // Not yet implemented
extern const BITVECT_DATA ITEM_NOSELL;
extern const BITVECT_DATA ITEM_EVIL;
extern const BITVECT_DATA ITEM_INVIS;
extern const BITVECT_DATA ITEM_MAGIC;
extern const BITVECT_DATA ITEM_NODROP;
extern const BITVECT_DATA ITEM_BLESS;
extern const BITVECT_DATA ITEM_ANTI_GOOD;
extern const BITVECT_DATA ITEM_ANTI_EVIL;
extern const BITVECT_DATA ITEM_ANTI_NEUTRAL;
extern const BITVECT_DATA ITEM_SECRET;
extern const BITVECT_DATA ITEM_FLOAT; // Not yet implemented
extern const BITVECT_DATA ITEM_NOBURN;
extern const BITVECT_DATA ITEM_NOLOCATE; // Not yet implemented
extern const BITVECT_DATA ITEM_NOID;
extern const BITVECT_DATA ITEM_NOSUMMON; // Not yet implemented
extern const BITVECT_DATA ITEM_LIT; 
extern const BITVECT_DATA ITEM_TRANSIENT;
extern const BITVECT_DATA ITEM_NOSLEEP; // Not yet implemented
extern const BITVECT_DATA ITEM_NOCHARM; // Not yet implemented
extern const BITVECT_DATA ITEM_TWOHANDED;
extern const BITVECT_DATA ITEM_NORENT; // Not yet implemented
extern const BITVECT_DATA ITEM_GOODONLY; // Not yet implemented
extern const BITVECT_DATA ITEM_HUM;
extern const BITVECT_DATA ITEM_LEVITATES;
extern const BITVECT_DATA ITEM_INVENTORY;
extern const BITVECT_DATA ITEM_WAS_DISARMED; // Not yet implemented - probably will not be used.
extern const BITVECT_DATA ITEM_WHOLEBODY; // Not yet implemented
extern const BITVECT_DATA ITEM_WHOLEHEAD; // Not yet implemented
// extra2 flags, Magma area format - Veygoth
extern const BITVECT_DATA ITEM_SILVER; // Not yet implemented
extern const BITVECT_DATA ITEM_THROW_RETURN; // Not yet implemented
extern const BITVECT_DATA ITEM_THROW_ONEROOM; // Not yet implemented
extern const BITVECT_DATA ITEM_THROW_TWOROOMS; // Not yet implemented
extern const BITVECT_DATA ITEM_ARTIFACT;
extern const BITVECT_DATA ITEM_ENLARGED; // Not yet implemented
extern const BITVECT_DATA ITEM_REDUCED; // Not yet implemented
extern const BITVECT_DATA ITEM_POISONED;

/*
 * Wear flags.
 * Used in #OBJECTS.
 * used by wear_table in const.c for equipping also
 */
#define ITEM_TAKE		   BV00
#define ITEM_WEAR_FINGER	   BV01
#define ITEM_WEAR_NECK		   BV02
#define ITEM_WEAR_BODY		   BV03
#define ITEM_WEAR_HEAD		   BV04
#define ITEM_WEAR_LEGS		   BV05
#define ITEM_WEAR_FEET		   BV06
#define ITEM_WEAR_HANDS		   BV07 
#define ITEM_WEAR_ARMS		   BV08
#define ITEM_WEAR_SHIELD	   BV09
#define ITEM_WEAR_ABOUT		   BV10 
#define ITEM_WEAR_WAIST		   BV11
#define ITEM_WEAR_WRIST		   BV12
#define ITEM_WIELD		   BV13
#define ITEM_HOLD		   BV14
// 15 and 16 not used
#define ITEM_WEAR_EYES             BV17
#define ITEM_WEAR_FACE             BV18
#define ITEM_WEAR_EAR              BV19
#define ITEM_QUIVER                BV20
#define ITEM_BADGE                 BV21
#define ITEM_WEAR_ONBACK           BV22
#define ITEM_ATTACH_BELT           BV23
#define ITEM_HORSE_BODY            BV24
#define ITEM_TAIL                  BV25
#define ITEM_NOSE                  BV26
#define ITEM_HORNS                 BV27
#define NUM_ITEM_WEAR_VECTORS      28

// Anti Flags for objects - Veygoth
//
// more than one set of bits - this should be converted to type
// extern const BITVECT_DATA and defined in bit.c
//
#define USE_ANYONE              BV00
#define USE_WARRIOR             BV01
#define USE_RANGER              BV02
#define USE_PALADIN             BV03
#define USE_ANTI                BV04
#define USE_CLERIC              BV05
#define USE_MONK                BV06 // For later use
#define USE_DRUID               BV07
#define USE_SHAMAN              BV08
#define USE_SORCERER            BV09
#define USE_NECRO               BV10 // For later use
#define USE_ELEMENTAL           BV11
#define USE_PSI                 BV12
#define USE_THIEF               BV13 // For later use
#define USE_ASSASSIN            BV14
#define USE_MERCENARY           BV15
#define USE_BARD                BV16 // For later use
#define USE_NOHUMAN             BV17
#define USE_NOGREYELF           BV18
#define USE_NOHALFELF           BV19
#define USE_NODWARF             BV20
#define USE_NOHALFLING          BV21
#define USE_NOGNOME             BV22
#define USE_NOBARBARIAN         BV23
#define USE_NODUERGAR           BV24
#define USE_NODROW              BV25
#define USE_NOTROLL             BV26
#define USE_NOOGRE              BV27
#define USE_ANTIGOODRACE        BV28
#define USE_NOORC               BV29
#define USE_ANTIEVILRACE        BV30
// Anti2 Flags
#define USE_NOTHRIKREEN       BV00 // For later use
#define USE_NOCENTAUR         BV01
#define USE_NOGITHYANKI       BV02
#define USE_NOMINOTAUR        BV03
#define USE_ANTIMALE            BV04
#define USE_ANTIFEMALE          BV05
#define USE_ANTINEUTER          BV06
#define USE_NOAQUAELF         BV07  // For later use
#define USE_NOSAHAUGIN        BV08  // For later use
#define USE_NOGOBLIN          BV09
#define USE_NORAKSHASA        BV10
#define USE_NOFLIND           BV11
#define USE_NOGITHZERAI      BV12
#define USE_NODUAL              BV13

// Material types - Veygoth
// material types for objects -- Veygoth
#define MAT_UNDEFINED              0
#define MAT_NONSUBSTANTIAL         1
#define MAT_FLESH                  2
#define MAT_CLOTH                  3
#define MAT_BARK                   4
#define MAT_SOFTWOOD               5
#define MAT_HARDWOOD               6
#define MAT_GLASS                  7
#define MAT_CRYSTAL                8
#define MAT_CERAMIC                9
#define MAT_BONE                   10
#define MAT_STONE                  11
#define MAT_HIDE                   12
#define MAT_LEATHER                13
#define MAT_CUREDLEATHER           14
#define MAT_IRON                   15
#define MAT_STEEL                  16
#define MAT_BRASS                  17
#define MAT_MITHRIL                18
#define MAT_ADAMANTIUM             19
#define MAT_BRONZE                 20
#define MAT_COPPER                 21
#define MAT_SILVER                 22
#define MAT_ELECTRUM               23
#define MAT_GOLD                   24
#define MAT_PLATINUM               25
#define MAT_GEM                    26
#define MAT_DIAMOND                27
#define MAT_PAPER                  28
#define MAT_PARCHMENT              29
#define MAT_LEAVES                 30
#define MAT_RUBY                   31
#define MAT_EMERALD                32
#define MAT_SAPPHIRE               33
#define MAT_IVORY                  34
#define MAT_DRAGONSCALE            35
#define MAT_OBSIDIAN               36
#define MAT_GRANITE                37
#define MAT_MARBLE                 38
#define MAT_LIMESTONE              39
#define MAT_LIQUID                 40
#define MAT_BAMBOO                 41
#define MAT_REEDS                  42
#define MAT_HEMP                   43
#define MAT_GLASSTEEL              44
#define MAT_EGGSHELL               45
#define MAT_CHITINOUS              46
#define MAT_REPTILESCALE           47
#define MAT_GENERICFOOD            48
#define MAT_RUBBER                 49
#define MAT_FEATHER                50
#define MAT_WAX                    51
#define MAT_PEARL                  52
#define MAT_SILK                   53
#define MAT_SPIDERSILK             54
#define MAT_JADE                   55
#define MAT_LEAD                   56
#define MAX_MATERIAL               56

// craftsmanship values - Veygoth
#define CRAFT_TERRIBLE		0
#define CRAFT_EXT_POOR          1
#define CRAFT_VERY_POOR         2
#define CRAFT_FAIRLY_POOR       3
#define CRAFT_WELL_BELOW_AVE    4
#define CRAFT_BELOW_AVE         5
#define CRAFT_SLIGHT_BELOW_AVE  6
#define CRAFT_AVERAGE           7
#define CRAFT_SLIGHT_ABOVE_AVE  8
#define CRAFT_ABOVE_AVE         9
#define CRAFT_WELL_ABOVE_AVE    10
#define CRAFT_EXCELLENT         11
#define CRAFT_SKILLED_ARTISAN   12
#define CRAFT_GIFTED_ARTISAN	13
#define CRAFT_MASTER_ARTISAN    14
#define CRAFT_ONEOFAKIND        15

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE               0
#define APPLY_STR                1
#define APPLY_DEX                2
#define APPLY_INT                3
#define APPLY_WIS                4
#define APPLY_CON                5
#define APPLY_SEX                6
#define APPLY_CLASS              7
#define APPLY_LEVEL              8
#define APPLY_AGE                9
#define APPLY_HEIGHT            10
#define APPLY_WEIGHT            11
#define APPLY_MANA              12
#define APPLY_HIT               13
#define APPLY_MOVE              14
#define APPLY_GOLD              15
#define APPLY_EXP               16
#define APPLY_AC                17
#define APPLY_HITROLL           18
#define APPLY_DAMROLL           19
#define APPLY_SAVING_PARA       20
#define APPLY_SAVING_ROD        21
#define APPLY_SAVING_PETRI      22
#define APPLY_SAVING_BREATH     23
#define APPLY_SAVING_SPELL      24
#define APPLY_FIRE_PROT         25
#define APPLY_AGI               26  
#define APPLY_POW               27
#define APPLY_CHA               28  
#define APPLY_LUK               30
#define APPLY_MAX_STR		31
#define APPLY_MAX_DEX		32
#define APPLY_MAX_INT		33
#define APPLY_MAX_WIS		34
#define APPLY_MAX_CON		35
#define APPLY_MAX_AGI		36
#define APPLY_MAX_POW		37
#define APPLY_MAX_CHA		38
#define APPLY_MAX_LUK		40
#define APPLY_RACE_STR		41
#define APPLY_RACE_DEX		42
#define APPLY_RACE_INT		43
#define APPLY_RACE_WIS		44
#define APPLY_RACE_CON		45
#define APPLY_RACE_AGI		46
#define APPLY_RACE_POW		47
#define APPLY_RACE_CHA		48
#define APPLY_RACE_LUK		50
#define APPLY_CURSE             51 /*Aren't these for 51-56 potions?*/
#define APPLY_RESISTANT         52 // was 26
#define APPLY_IMMUNE            53 // was 27
#define APPLY_SUSCEPTIBLE       54 // was 28
#define APPLY_RACE              55
#define APPLY_SIZE              56
#define APPLY_HIGHEST           57

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		   BV00
#define CONT_PICKPROOF		   BV01
#define CONT_CLOSED		   BV02
#define CONT_LOCKED		   BV03

/*
 * Values for portals (value[3]).
 * Used in #OBJECTS.
 */
#define PORTAL_NO_CURSED	   BV00
#define PORTAL_GO_WITH		   BV01
#define PORTAL_RANDOM		   BV02
#define PORTAL_BUGGY		   BV03
#define PORTAL_CLOSEABLE	   BV04
#define PORTAL_PICKPROOF	   BV05
#define PORTAL_CLOSED		   BV06
#define PORTAL_LOCKED		   BV07
/* Portal Values:
 * [0] - Destination room
 * [1] - Trigger command
 * [2] - Number of charges
 * [3] - Portal Flags
 * [4] - Key #, 0 for none
 */

/*
 * Values for ranged weapons (value[3]).
 * Used in #OBJECTS.
 */
#define RNG_BOW			      0
#define RNG_CROSSBOW		      1
#define RNG_CATAPULT		      2


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		100 // Requires limbo.are
#define ROOM_VNUM_CHAT      100 // Requires limbo.are
#define ROOM_VNUM_TEMPLE	3001 // Requires midgaard.are
#define ROOM_VNUM_ALTAR		3002 // Requires midgaard.are
#define ROOM_VNUM_START		3005 // Requires midgaard.are
#define ROOM_VNUM_ASTRAL_LOW  1900 // Requires astral.are
#define ROOM_VNUM_ASTRAL_HIGH 1979 // Requires astral.are

/*
 * Room flags.
 * Used in #ROOMS.
 */
// Rooms with an asterisk after their bitvector value do not have any
// supporting code and have no effect on the game yet. -- Veygoth
//
// we may have to create a second bitvector for rooms since this
// one is full.  If so we should define these as
// extern const BITVECT_DATA and define them in bit.c
//
extern const BITVECT_DATA ROOM_DARK;
extern const BITVECT_DATA ROOM_LIMITSCAN; // Veygoth - bit not used, from old versions of Tavril's DE
extern const BITVECT_DATA ROOM_NO_MOB;
extern const BITVECT_DATA ROOM_INDOORS;
extern const BITVECT_DATA ROOM_SILENT;
extern const BITVECT_DATA ROOM_UNDERWATER;
extern const BITVECT_DATA ROOM_NO_RECALL;
extern const BITVECT_DATA ROOM_NO_MAGIC;
extern const BITVECT_DATA ROOM_TUNNEL; // This bit means absolutely nothing so far.
extern const BITVECT_DATA ROOM_PRIVATE;
// BV10 Unused in DE.
extern const BITVECT_DATA ROOM_SAFE;
extern const BITVECT_DATA ROOM_NO_PRECIP;
extern const BITVECT_DATA ROOM_SINGLE_FILE; // This has not yet been implemented
extern const BITVECT_DATA ROOM_JAIL;        // This has not yet been implemented
extern const BITVECT_DATA ROOM_NO_TELEPORT;
extern const BITVECT_DATA ROOM_SOLITARY;
extern const BITVECT_DATA ROOM_HEAL;
extern const BITVECT_DATA ROOM_NO_HEAL;
extern const BITVECT_DATA ROOM_INN;
extern const BITVECT_DATA ROOM_DOCKABLE; // This has not yet been implemented
extern const BITVECT_DATA ROOM_MAGICDARK;
extern const BITVECT_DATA ROOM_MAGICLIGHT;
extern const BITVECT_DATA ROOM_NO_SUMMON; // This has not yet been implemented
extern const BITVECT_DATA ROOM_GUILDROOM; // This has not yet been implemented
extern const BITVECT_DATA ROOM_TWILIGHT;
extern const BITVECT_DATA ROOM_NO_PSIONICS; // This has not yet been implemented
extern const BITVECT_DATA ROOM_NO_GATE;
extern const BITVECT_DATA ROOM_BANK;
extern const BITVECT_DATA ROOM_PET_SHOP; // Unused in editor.
extern const BITVECT_DATA ROOM_NO_SCAN;

// Second set of room flags -- Veygoth
extern const BITVECT_DATA ROOM_EARTHEN_STARSHELL;
extern const BITVECT_DATA ROOM_AIRY_STARSHELL;
extern const BITVECT_DATA ROOM_FIERY_STARSHELL;
extern const BITVECT_DATA ROOM_WATERY_STARSHELL;
extern const BITVECT_DATA ROOM_HYPNOTIC_PATTERN;

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define DIR_NORTHWEST                 6
#define DIR_SOUTHWEST                 7
#define DIR_NORTHEAST                 8
#define DIR_SOUTHEAST                 9
#define MAX_DIR			     10


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		   BV00
#define EX_CLOSED		   BV01
#define EX_LOCKED		   BV02
// bv3 = rsclosed
// bv4 = rslocked
// bv5 = pickable
#define EX_SECRET                  BV06
#define EX_BLOCKED                 BV07
#define EX_PICKPROOF		   BV08
#define EX_WALLED                  BV09
#define EX_SPIKED                  BV10
#define EX_ILLUSION                BV11
#define EX_BASHED                  BV12
#define EX_BASHPROOF               BV13
#define EX_PASSPROOF               BV14
#define EX_TRAPPED                 BV15
#define EX_EAT_KEY                 BV16 // Destroys the key
#define EX_JAMMED                  BV17

struct trap_data
{
	int trigger;
	int damage;
	int charges;
	int level;
	int percent;
};

// Trap types
#define TRAP_SLEEP	0
#define TRAP_TELEPORT   1
#define TRAP_FIRE       2
#define TRAP_COLD       3
#define TRAP_ACID       4
#define TRAP_ENERGY     5
#define TRAP_BLUNT      6
#define TRAP_PIERCING   7
#define TRAP_SLASHING   8
#define TRAP_DISPEL     9
#define TRAP_GATE       10
#define TRAP_SUMMON     11
#define TRAP_WITHER     12
#define TRAP_HARM       13
#define TRAP_POISON     14
#define TRAP_PARALYSIS  15
#define TRAP_STUN       16
#define TRAP_DISEASE    17
#define MAX_TRAP        18

// Trap triggers
#define TRIG_MOVE	BV00
#define TRIG_GETPUT	BV01
#define TRIG_FULLROOM   BV02
#define TRIG_NORTH      BV03
#define TRIG_EAST	BV04
#define TRIG_SOUTH	BV05
#define TRIG_WEST	BV06
#define TRIG_UP		BV07
#define TRIG_DOWN	BV08
#define TRIG_OPEN	BV09
#define TRIG_UNLIMITED  BV10
#define TRIG_GLYPH      BV11
#define TRIG_WEAR       BV12
#define TRIG_UNEQUIP    BV13
#define TRIG_STEAL      BV14
#define TRIG_ENCHANT    BV15
#define TRIG_UNCURSE    BV16

/*
 * Sector types.
 * Used in #ROOMS.
 */
// Veygoth - Changed to be identical to DikuEdit's sector flags.
// Sector types 8-11 are different from that which stock UltraEnvy
// used, and 12+ are new types.  If using old Envy zones, these
// values will have to be converted for each individual room.
// It is safer to just not to use old Envy zones.
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_AIR		      8 // Was SECT_UNDERWATER
#define SECT_UNDERWATER  	      9 // Was SECT_AIR
#define SECT_UNDERWATER_GROUND       10 // Was SECT_DESERT
#define SECT_PLANE_FIRE              11 // Was SECT_ICELAND
#define SECT_OCEAN                   12
#define SECT_UNDERG_WILD             13
#define SECT_UNDERG_CITY             14
#define SECT_UNDERG_INSIDE           15
#define SECT_UNDERG_WATER            16
#define SECT_UNDERG_WATER_NOSWIM     17
#define SECT_UNDERG_NOGROUND         18
#define SECT_PLANE_AIR               19
#define SECT_PLANE_WATER             20
#define SECT_PLANE_EARTH             21
#define SECT_PLANE_ETHEREAL          22
#define SECT_PLANE_ASTRAL            23
#define SECT_DESERT                  24
#define SECT_ARCTIC                  25
#define SECT_SWAMP                   26
#define SECT_ROAD                    27
#define SECT_UNDERG_IMPASSABLE       28
#define SECT_UNDERG_OCEAN            29
#define SECT_MAX		     30

struct extended_terrain
{
	int  extended_sector;
	char *room_title;
	char *room_description;
	int  main_sector; // Actual base terrain type
	char *map_char;  // Ansi displayed for that sector
        int visibility;  // Standard range of squares a player can see (radius)
};

#define MAX_EXTENDED_TERRAIN 155
#define EXT_ZONEMARKER        45

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
//#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
//#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_HAND                    16
//#define WEAR_HOLD		     17
#define WEAR_HAND_2                  18
//#define WEAR_MISSILE_WIELD           19
#define WEAR_EYES                    19
#define WEAR_FACE                    20
#define WEAR_EAR_L                   21
#define WEAR_EAR_R                   22
#define WEAR_BADGE                   23
#define WEAR_ONBACK                  24
#define WEAR_BELT_1                  25
#define WEAR_BELT_2                  26
#define WEAR_BELT_3                  27
#define WEAR_QUIVER                  29
#define WEAR_TAIL                    30
#define WEAR_HORSE_BODY              31
#define WEAR_HORNS                   32
#define WEAR_NOSE                    33
#define WEAR_HAND_3                  34
#define WEAR_HAND_4                  35
#define WEAR_LOWER_ARMS              36
#define WEAR_LOWER_HANDS             37
#define WEAR_LWRIST_L                38 // Lower wrist for thri
#define WEAR_LWRIST_R                39 // Lower wrist for thri
#define MAX_WEAR                     40



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_UNCONSCIOUS               3
#define POS_STUNNED		      4
#define POS_SLEEPING		      5
#define POS_RECLINING                 6
#define POS_RESTING		      7
#define POS_SITTING                   8
#define POS_KNEELING                  9
#define POS_FIGHTING		      10
#define POS_STANDING		      11



/*
 * MUD ACT bits for sysdata.
 */
#define MUD_AUTOSAVE_DB		   BV00
#define MUD_VERBOSE_QUESTLOAD      BV01
#define MUD_NEWLOCK                BV02
#define MUD_LOG_PROGDATA           BV03 // Ridiculously verbose logging of prog info
#define MUD_TURBOLEVEL             BV04 // For quick leveling prior to pwipe
#define MUD_DEBUG                  BV05 // Enables and disables the LAST_FUN stuff.

/*
 * ACT bits for players.
 */
// Almost out of bits.  8 and 24 still available.
//
// - Veygoth
// Now all out :(
//
#define PLR_AUTOEXIT		   BV00
#define PLR_SHOUT                  BV01
#define PLR_CAMPING                BV02
#define PLR_IS_NPC		   BV03		/* Don't EVER set. */
#define PLR_MEMORIZING             BV04
#define PLR_MEDITATING             BV05
#define PLR_BLANK		   BV06
#define PLR_BRIEF		   BV07
#define PLR_VICIOUS                BV08
#define PLR_COMBINE		   BV09
#define PLR_PROMPT		   BV10
#define PLR_TELNET_GA		   BV11
#define PLR_HOLYLIGHT		   BV12
#define PLR_WIZINVIS		   BV13
#define PLR_WIZBIT		   BV14
#define	PLR_SILENCE		   BV15
#define PLR_NO_EMOTE		   BV16
#define PLR_MOVED		   BV17
#define PLR_TELL		   BV18
#define PLR_LOG			   BV19
#define PLR_DENY		   BV20
#define PLR_FREEZE		   BV21
#define PLR_COLOR_CON		   BV22 // players see multicolored con messages
#define PLR_MAP 		   BV23 // Added by Lohrr.
#define PLR_CAST_TICK              BV24
#define PLR_AFK			   BV25
#define PLR_COLOUR		   BV26
#define PLR_JUST_DIED	 	   BV27 // Just died flag, gives a player a few seconds of safe time after login
#define PLR_PAGER		   BV28
#define PLR_MOUNTABLE              BV29         /* Can be mounted       */
#define PLR_FOG                    BV30

//
// Another set of bits, to be used in pcdata->config
// - Lithos
//
#define CFG_SHOWTITLES             BV00


/* WIZNET flags */
#define WIZ_ON			   BV00		/* On|Off switch	*/
#define WIZ_TICKS		   BV01		/* Tick's.		*/
#define WIZ_LOGINS		   BV02   	/* Login|Logoff. 	*/
#define WIZ_DEATHS		   BV03   	/* Plr death's. 	*/
#define WIZ_RESETS		   BV04   	/* Area reset's. 	*/
#define WIZ_CRIME		   BV05   	/* KILLER & THIEF flag. */
#define WIZ_SACCING		   BV06   	/* Item saccing. 	*/
#define WIZ_LEVELS		   BV07   	/* Level's advancing. 	*/
#define WIZ_SECURE		   BV08   	/* log -> screen. 	*/
#define WIZ_SWITCHES		   BV09  	/* Switch. 		*/
#define WIZ_SNOOPS		   BV10  	/* Snoops. 		*/
#define WIZ_RESTORE		   BV11  	/* Restores. 		*/
#define WIZ_LOAD		   BV12  	/* oload, mload. 	*/
#define WIZ_NEWBIE		   BV13  	/* Newbie's. 		*/
#define WIZ_SPAM		   BV14		/* Spamming.	 	*/
#define WIZ_CHAT		   BV15		/* Wiznet chatting. 	*/
#define WIZ_DEBUG		   BV16		/* Debug info. 		*/
#define WIZ_PETITION               BV17         /* Petitions            */
#define WIZ_QUESTS                 BV18         /* Quests.              */
#define WIZ_HUNTING                BV19         /* Hunter actions.      */
#define WIZ_LSPAM                  BV20         /* Lithos spam channel. */
#define WIZ_HATING                 BV21         /* Hater actions.       */

/*
 * Obsolete bits.
 */
#if 0
#define PLR_CHAT		   BV08		/* Obsolete		*/
#define ACT_GAMBLE                 BV11         /* Runs a gambling game	*/
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_IMMTALK		   BV03
#define	CHANNEL_SHOUT		   BV06
#define	CHANNEL_YELL		   BV07
#define	CHANNEL_CLANTALK	   BV09

/*
 * Visibility bits.
 * Used in act_info.c - how_see function.
 */
#define SEE_INVISIBLE		   0
#define SEE_VISIBLE		   1
#define SEE_TOO_DARK		   2
#define SEE_SENSE_HIDDEN	   3
#define SEE_SENSE_INFRA	           4

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    MOB_FUN *           spec_fun;
    MOB_FUN *		spec_fun2;
    MOB_FUN *           death_fun;
    GAME_DATA *		pGame;
    SHOP_DATA *		pShop;
    AREA_DATA *         area;
    MPROG_DATA *        mobprogs;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    MOVE_SCRIPT *       move_script;
    int 		vnum;
    int 		count;
    int 		killed;
    int 		sex;
    int                 class;
    int 		level;
    int			act;
    int			affected_by[NUM_AFFECT_VECTORS];
    int 		alignment;
    int			race;
    int                 position;
    int			progtypes;
    int			resistant;
    int			immune;
    int			susceptible;
};

// this is the main quest header function for a mob
struct quest_index_data
{
    int        vnum; // vnum of mobile quest is attached to
    QUEST_DATA *quests; // the individual quests associated with each mob
    TALK_DATA  *messages; // talk data associated with a mob
    QUEST_INDEX_DATA *next;
};

// this should be the individual quests within a mob
// corresponds to the _questQuest struct in DE.
struct quest_data
{
    QUEST_DATA   *next;
    QUEST_ITEM   *receive;
    QUEST_ITEM   *give;
    char         *disappear;
    char         *complete;
};

struct talk_data
{
    TALK_DATA *next;
    char   *keywords;
    char   *message;
};

struct quest_item
{
    bool   complete;
//    bool   give; probably not necessary
    int    type;
    int    value;
    QUEST_ITEM *next;
};

// Structure for time-based events - Veygoth
struct event_data
{
    EVENT_DATA * next; // Next event
    int		type; // Event type
    void *	arg1; // Can be one of many types
    void *	arg2; // Can be one of many types
    int		time; // Time in pulses
    int		var;  // A miscellaneous variable
    bool        deleted;
};

// Types of events
// ---------------
// Not all these will necessarily be implemented.  Any that are will have
// an asterisk placed next to them.  Just because there is an event
// defined doesen't mean we should convert everything to events and break
// them up into subcomponents, since some things may run smoother when
// lumped together in a single function. - Veygoth
#define EVENT_NONE		0
#define EVENT_SAVE_SYSDATA	1 // Imped
#define EVENT_SAVE_CORPSES      2 // Imped
#define EVENT_HIT_GAIN		3 // Imped
#define EVENT_MANA_GAIN		4 // Imped
#define EVENT_MOVE_GAIN		5 // Imped
#define EVENT_RESET_AREA	6 // Imped
#define EVENT_MEMORIZE_UPDATE	7 // Imped
#define EVENT_WEATHER_UPDATE	8 // Imped
#define EVENT_MOBILE_UPDATE	9 // Imped
#define EVENT_OBJECT_UPDATE	10 // Imped
#define EVENT_SLOW_CHAR_UPDATE	11
#define EVENT_CHAR_UPDATE	12 // Imped
#define EVENT_LIST_UPDATE       13 // Imped
#define EVENT_VIOLENCE_UPDATE   14 // Imped
#define EVENT_AGGR_UPDATE	15
#define EVENT_HUNGER		16
#define EVENT_PULSE_REGENERATE	17
#define EVENT_CAST_TICK		18
#define EVENT_ROOM_AFF_ADD	19
#define EVENT_ROOM_AFF_REMOVE	20
#define EVENT_ROOM_PROC		21
#define EVENT_MOB_PROC		22
#define EVENT_OBJ_PROC		23
#define EVENT_ZONE_PROC		24
#define EVENT_QUEST_PROC	25
#define EVENT_SHIP_PROC		26
#define EVENT_ROOM_DMG_SPELL    27
#define EVENT_PERSON_DMG_SPELL	28
#define EVENT_SPELL_CAST        29
#define EVENT_CAMP		30 // Imped Govno
#define EVENT_DELAYED_COMMAND	31
#define EVENT_EXTRACT_CHAR      32
#define EVENT_BOMBARD           33 // Imped
#define EVENT_OBJ_DECAY		34
#define EVENT_WIERD             35
#define EVENT_SHIP_MOVE		36
#define EVENT_FIRE_PLANE	37
#define EVENT_AUTOSAVE		38
#define EVENT_TRACK		39
#define EVENT_SPELL_SCRIBING	40
#define EVENT_NIGHTMARES        41
#define EVENT_REMOVE_JUSTDIED   42
#define EVENT_SONG		43 // Imped
#define EVENT_BARD_EFFECTDECAY	44
#define EVENT_STUNNED		45
#define EVENT_KO		46
#define EVENT_BRAIN_DRAIN	47
#define EVENT_CONFLAGURATION    48 // Imped
#define EVENT_BERSERK		49
#define EVENT_MOB_HUNT		50
#define EVENT_UNDERWATER	51
#define EVENT_SWIMMING		52
#define EVENT_ROOM_UPDATE       53 // Imped
#define EVENT_AREA_UPDATE       54 // Imped
#define EVENT_OBJECT_SPECIAL    55
#define EVENT_ACID_ARROW        56 // Imped
#define EVENT_DARKNESS_END      57 // Imped
#define EVENT_IMMOLATE          58 // Imped
#define EVENT_CREEPING_DOOM     59 // Imped
#define EVENT_INCENDIARY_CLOUD  60 // Imped
#define EVENT_HEARTBEAT		    61 // Imped Govno .. Don't Touch
#define EVENT_STARSHELL_END     62 // Imped
#define EVENT_LIGHT_END         63 // Imped

struct hunt_hate_fear
{
    char *		name;
    CHAR_DATA *		who;
};

struct coin_data
{
    int copper;
    int silver;
    int gold;
    int platinum;
};

/*
 * One character (PC or NPC).
 */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		group_leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		riding;
    CHAR_DATA *		rider;
    CHAR_DATA *         next_in_group;
    PERSON_DATA *       followers;
    MOB_FUN *		spec_fun;
    MOB_FUN *           spec_fun2;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    int 		sex;
    int 		class;
    int 		race;
    int 		level;
    int  		trust;
    int                 size;
    int			played;
    int                 cast_spl;
    int                 cast_time;
    int 		perm_str;
    int 		perm_int;
    int 		perm_wis;
    int 		perm_dex;
    int 		perm_con;
    int                 perm_agi;
    int                 perm_cha;
    int                 perm_pow;
    int                 perm_luk;
    int 		mod_str;
    int 		mod_int;
    int 		mod_wis;
    int 		mod_dex;
    int 		mod_con;
    int                 mod_agi;
    int                 mod_cha;
    int                 mod_pow;
    int                 mod_luk;
    time_t		logon;
    time_t		save_time;
    time_t              last_note;
    int 		timer;
    int 		wait;
    int 		hit;
    int 		max_hit;
    int 		mana;
    int 		max_mana;
    int 		move;
    int 		max_move;
    COIN_DATA		money;
    int			exp;
    int			act;
    int			affected_by[NUM_AFFECT_VECTORS];
    int 		position;
    int 		carry_weight;
    int 		carry_number;
    int 		saving_throw[5];	// added by Veygoth
    int 		alignment;
    int 		hitroll;
    int 		damroll;
    int 		armor;
    int 		wimpy;
    int 		deaf;
    bool		deleted;
    MPROG_ACT_LIST *	mpact;
    int			mpactnum;
    HHF_DATA *		hunting;
    HHF_DATA *		hating;
    HHF_DATA *		fearing;
    int			resistant;
    int			immune;
    int			susceptible;
    int                 fly_level;
    int                 load_room_vnum;
};


// Struct created by Veygoth for movement
struct movement_script
{
    bool ship; // is it a ship or a mobile movement script?
    MOVE_SCRIPT *next;
    int vnum; // the vnum of the object or mobile which it is associated with
    int start_room;
    char *script;
    int pos; // script position
    bool running; // is the character in a valid position/room to be following the script?
};

struct obj_char
{
    int type;
    union {
	CHAR_DATA *ch;
	OBJ_DATA *obj;
    } pointer;
};

struct person_data
{
   PERSON_DATA *next;
   PERSON_DATA *prev;
   CHAR_DATA *who;
};

struct cast_data
{
   CAST_DATA *next;
   EVENT_DATA *event;
   CHAR_DATA *who;
};

struct innate_timer_data
{
    INNATE_TIMER_DATA *next;
    int type;
    int timer;
    CHAR_DATA *who;
};

/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    CHAR_DATA *         consent;
    CHAR_DATA *         ignore;
    CHAR_DATA *         guarding;
    MEM_DATA *          memorized;
    MEM_DATA *          memorized_last;
    char *		pwd;
    char *		pwdnew;
    char *		bamfin;
    char *		bamfout;
    char *              immskll;
    char *		title;
    char *              prompt;
    int 		condition	[ 3 ];
    int                 pagelen;
    int 		skl_lrn	[ MAX_SKILL ];
    int                 spl_lrn [ MAX_SPELL ];
    int                 sng_lrn [ MAX_SONG  ];
    int                 lang_lrn [ MAX_LANG ];
    int                 speaking; // Language player is currently speaking
    bool                switched;
    ALIAS_DATA *	alias_list;
    TROPHY_DATA         trophy[MAX_LEVEL];
    int                 security;
    int                 mprog_edit;
    CLAN_DATA *		clan;
    int                 mod_hit; // For plus hitpoint gear - Veygoth
    int			rank;
    int                 train;
    int                 frags;
    int			pkills;
    int			pdeaths;
    int			mkills;
    int			mdeaths;
    int			illegal_pk;
    int			wiznet;
    int                 config;
    int                 firstaid;
    int                 created;
    int                 orighome;
    int                 currhome;
    int                 height; // Not imped yet
    int                 weight; // Not imped yet
    int 		racemod_str;
    int 		racemod_int;
    int 		racemod_wis;
    int 		racemod_dex;
    int 		racemod_con;
    int                 racemod_agi;
    int                 racemod_cha;
    int                 racemod_pow;
    int                 racemod_luk;
    int 		maxmod_str;
    int 		maxmod_int;
    int 		maxmod_wis;
    int 		maxmod_dex;
    int 		maxmod_con;
    int                 maxmod_agi;
    int                 maxmod_cha;
    int                 maxmod_pow;
    int                 maxmod_luk;
    COIN_DATA           bank;
    int                 aggr_level;
    int                 tradition; // monk-mystic stuff
    int                 skill_points; // monk-mystic stuff
    int                 chi; // monk-mystic stuff
    int                 max_chi; // monk-mystic stuff
    int                 monk_skill[MAX_MONK_SKILL]; // monk-mystic stuff
    int                 monk_restriction; // monk-mystic stuff
    int                 stance; // monk-mystic stuff
    int                 wizinvis; // wizinvis level for immortals
    int                 lastrent; // Vnum of room last rented or camped in.
    int                 lost_hp; // Number of hitpoints lost last level, always the maximum.
    int                 new_hitpoints; // Have they been rerolled since the con changes?
    INNATE_TIMER_DATA   *innate_timers;
    bool                twohanded;
    int                 editing;
    char                *editbuf;
};

struct monk_skill
{
   char * skill_name;
   int points[MAX_TRADITION];
};

struct monk_stance
{
   char *name;
   int hit;
   int dam;
   int ac;
   int base_dmg;
   int dbl_atk;
   int hitplus[5];
   int damplus[5];
};

#define TRADITION_SCORPION   1

// These stances correspond to the actual monk skill numbers - Veygoth
#define STANCE_NONE          	0
#define STANCE_BEAR		1
#define STANCE_CAT		2
#define STANCE_COBRA		3
#define STANCE_CRANE		4
#define STANCE_DRAGON		5
#define STANCE_DRAGONFLY	6
#define STANCE_HAWK		7
#define STANCE_LEOPARD		8
#define STANCE_MANTIS		9
#define STANCE_MONKEY		10
#define STANCE_SNAKE		11
#define STANCE_TIGER		12
#define MAX_STANCE		13

struct fragger_data
{
    char *name;
    int frags;
};

// Guild frags will be stored separately with each guild's data.
// This is a huge struct, and updating and referencing it should be
// a huge chore - Veygoth
struct frag_data
{
        FRAGGER_DATA top_frags[MAX_FRAG];
        FRAGGER_DATA bottom_frags[MAX_FRAG];
        FRAGGER_DATA race_top_frags[MAX_PC_RACE][MAX_FRAG];
        FRAGGER_DATA race_bottom_frags[MAX_PC_RACE][MAX_FRAG];
        FRAGGER_DATA class_top_frags[MAX_CLASS][MAX_FRAG];
        FRAGGER_DATA class_bottom_frags[MAX_CLASS][MAX_FRAG];
        int total_race_class_frags[MAX_PC_RACE][MAX_CLASS];
        int total_side_frags[MAX_RACEWAR_SIDE];
};

/* Frag functions
 *
 * check_top_bot_frag
 * check_race_class_top_bot_frag
 *
 * When a person frags, they will gain/lose a frag, and so will their
 * race/class totals and their racewar side totals.
 *
 * A race total is done by adding up all classes for that race and a class
 * total is done by adding up all races for that class.
 *
 */

// Spell memorization structure
struct mem_data
{
	MEM_DATA *next;
        MEM_DATA *prev;         // access only during memorizing
	int sn;
	int memtime; 		// time in pulses
	int full_memtime; 	// time in pulses
	int circle;
	bool memmed;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		29

struct	liq_type
{
    char               *liq_name;
    char               *liq_color;
    int                 liq_affect [ 3 ];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	   /* Next in list                     */
    char             *keyword;     /* Keyword in look/examine          */
    char             *description; /* What to see                      */
    bool              deleted;
};



/*
 * Prototype for an object.
 */
// remember that if you change anything here you will have to modify the
// create_object function to give a value to the object instance - Veygoth
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    OBJ_FUN *		spec_fun;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    TRAP_DATA *         trap;
    MPROG_DATA *        objprogs;
    char *		name;
    char *		short_descr;
    char *		description;
    int 		vnum;
    int 		item_type;
    int 		extra_flags[NUM_ITEM_EXTRA_VECTORS];
    int			affected_by[NUM_AFFECT_VECTORS];
    int 		wear_flags;
    int                 anti_flags;
    int                 anti_flags2;
    int                 material;
    int                 size;
    int                 volume;
    int                 craftsmanship;
    int 		count;
    int 		weight;
    int			cost;			/* Unused */
    int 		level;
    int			progtypes;
    int                 condition;
    int			value	[ 8 ];
    AREA_DATA *         area;			/* OLC */
};

/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    OBJ_FUN *		spec_fun;
    CHAR_DATA *		carried_by;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    TRAP_DATA *         trap;
    char *		name;
    char *		short_descr;
    char *		description;
    int 		item_type;
    int 		extra_flags[NUM_ITEM_EXTRA_VECTORS];
    int			affected_by[NUM_AFFECT_VECTORS];
    int 		wear_flags;
    int                 anti_flags;
    int                 anti_flags2;
    int                 material;
    int                 size;
    int                 volume;
    int                 craftsmanship;
    int 		wear_loc;
    int 		weight;
    int			cost;
    int 		level;
    int 		timer;
    int			value	[ 8 ];
    int                 condition;
    bool                deleted;
    int                 fly_level;
};



/*
 * Per-noun stuff.
 */
struct	noun_type
{
    char *	name;			/* Full class name		*/
    bool	animate;		/* Can the noun be a subject?	*/
};

/*
 * Per-verb stuff.
 */
struct	verb_type
{
    char *	name;			/* Full class name	        */
    int 	dobject;		/* Direct object type		*/
    int 	iobject;		/* Indirect object type		*/
};

/*
 * Equipment wear table
 */
struct wear_data
{
    int wear_flag;
    int wear_location;
    int wear_location2;
    int wear_location3;
    char *wear_msg;
    char *wear_msg2;
    int part_needed;
    int race_not_allowed;
};

struct exptable
{
    int mob_exp;
    int lvl_exp;
}; 

#define FLY_GROUND   0
#define FLY_LOW      1
#define FLY_MEDIUM   2
#define FLY_HIGH     3

/*
 * Vehicle Data
 */
// Vehicle types - Veygoth
#define VEH_SHIP_ANY_WATER   1 // multi-purpose boats, medium size
#define VEH_SHIP_SHALLOWS    2 // rowboats and canoes
#define VEH_SHIP_DEEPWATER   3 // giant boats, submarines
#define VEH_SHIP_ANY_TERRAIN 4 // hovercraft, magic carpets, etc.
#define VEH_NO_WATER    5 // all-terrain land vehicles
#define VEH_FLATLAND    6 // flat land vehicles like carts and buggies
#define VEH_SHIP_SPELLJAMMER 7 // any-terrain spelljammer
#define VEH_SHIP_SJ_LAND     8 // spelljammer that can't go in the water
#define VEH_SHIP_SJ_WATER    9 // spelljammer that can't go on land
#define VEH_SHIP_SJ_SKYONLY 10 // spelljammer that can't land
#define VEH_WAGON           11 // wagon or other land vehicle
#define VEH_CREATURE        12 // Giant creature

// We can cover much of this just be creating a pointer to a ship or vehicle object.
struct vehicle_data
{
  VEHICLE_DATA *      next;
  bool                deleted;
  int                 type; // Type of vehicle
  char *              owner; // name of owner
  // we can't use char_data because some ships belong to characters not in the game.
  OBJ_DATA        *   ship; // Vnum of object for ship
  int                 entry_point; // room in which people start when they
                      // board the ship
  int                 control_panel;
  int                 hull;
  int                 fly_level;
  int                 occupants;
  int                 direction; // direction currently moving in
  int                 speed; // current speed
  int                 movement_timer;
  int                 movement_delay;
  int                 movement_pointer;
  char                movement_script[MAX_INPUT_LENGTH];
};

/*
 * Crime Data for Justice (Veygoth)
 */
struct crime_data
{
    CRIME_DATA *next;
    char *      criminal;
    char *      victim;
    char *      zone; // filename of zone where crime was committed
    int         crime;
    int         time;
    int         status;
};

#define CRIME_MURDER      BV01
#define CRIME_ATT_MURDER  BV02
#define CRIME_THEFT       BV03
#define CRIME_JAILBREAK   BV04
#define CRIME_ATT_THEFT   BV05
#define CRIME_WALLING     BV06
#define CRIME_BRIBERY     BV07
#define CRIME_SHAPECHANGE BV08
#define CRIME_DEADBEAT    BV09 // non payment of debt
#define CRIME_KIDNAPPING  BV10
#define CRIME_TREASON     BV11 // crime against town
#define CRIME_PLOOTING    BV12 // corpse looting
#define CRIME_CORPSEDRAG  BV13 // corpse dragging
#define CRIME_DISGUISING  BV14 // disguise
#define CRIME_FORGERY     BV15
#define CRIME_SUMMONING   BV16 // summoning creatures

#define CRIME_STAT_WANTED     BV01 // not yet captured
#define CRIME_STAT_PARDON     BV02 // pardoned
#define CRIME_STAT_FALSE      BV03 // falsely accused
#define CRIME_STAT_JAIL       BV04 // person is in jail
#define CRIME_STAT_DELETED    BV05 
#define CRIME_STAT_DISPATCHED BV06 // a guard has been dispatched to capture the offender

// Justice types
#define JUSTICE_GOOD         1 // crimes illegal, prot evil
#define JUSTICE_EVIL         2 // crimes legal, prot goodie
#define JUSTICE_NEUTRAL      3 // crimes illegal, all welcome
#define JUSTICE_NEUT_CLOSED  4 // crimes illegal, prot goodie and evil
#define JUSTICE_LAWFUL_EVIL  5 // crimes illegal, protected from goodies
#define JUSTICE_CHAOTIC_GOOD 6 // crimes legal, protected from evils
#define JUSTICE_CHAOTIC_NEUT 7 // crimes legal, prot goodie and evil
#define JUSTICE_CLOSED_GOOD  8 // crimes illegal, prot evil AND neutral
#define JUSTICE_CLOSED_EVIL  9 // crimes legal, prot good AND neutral

/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    int 		vnum;
    int 		exit_info;
    int 		key;
    char *		keyword;
    char *		description;
    EXIT_DATA           *next;
};

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

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    int 		arg0;
    int 		arg1;
    int 		arg2;
    int 		arg3;
    int                 arg4;
    int                 arg5;
    int                 arg6;
    int                 arg7;
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    char *		name;
    int                 defender; // vnum of defending mobs for invasion
    int                 defender_num; // number of defenders that spawn
    int                 barracks; // vnum of room where guards spawn, if not given guards will spawn in judge room
    int                 judge; // vnum of room in which judge presides
    int                 jail; // vnum of room in which jail is
    int                 squads; // number of guard squads that cna be dispatched
    int                 dispatched;
    int                 recall;
    int 		age;
    int 		nplayer;
    int                 justice;
    char *              filename;		/* OLC                     */
    char *              builders;		/* OLC Listing of builders */
    int                 security;		/* OLC Value 0-infinity    */
    int                 lvnum;			/* OLC Lower vnum          */
    int                 uvnum;			/* OLC Upper vnum          */
    int                 vnum;			/* OLC Area vnum           */
    int                 area_flags;		/* OLC                     */
    int                 low_r_vnum;
    int                 hi_r_vnum;
    int                 low_o_vnum;
    int                 hi_o_vnum;
    int                 low_m_vnum;
    int                 hi_m_vnum;
    int                 llv;
    int                 ulv;
    char *              author;
    char *              resetmsg;
    int                 times_reset;
    int                 resetmode;
    int                 llifespan;
    int                 ulifespan;
};



/*
 * System information and flags.
 */
struct	system_data
{
    int			max_players;		/* Maximum players this boot */
    int			all_time_max;		/* Maximum players ever      */
    char *		time_of_max;		/* Time of max players ever  */
    int			act;			/* Mud act flags             */
};


/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[ MAX_DIR ];
    char *		name;
    char *		description;
    int 		vnum;
    int 		room_flags[NUM_ROOM_FLAGS];
    int 		orig_room_flags;	/* OLC */
    int 		light;
    int                 current;
    int                 current_dir;
    int                 fall_chance;
    int 		sector_type;
    int                 extra_sector_info;      // Worldmap sector info
    RESET_DATA *        reset_first;		/* OLC */
    RESET_DATA *        reset_last;		/* OLC */
};


/*
 * Skills do not include spells anymore - Veygoth.
 */
struct	skill_type
{
    char *	name;			   /* Name of skill		 */
    int 	skill_level [ MAX_CLASS ]; /* Level needed by class	 */
    int *	pgsn;			   /* Pointer to associated gsn	 */
    int 	beats;			   /* Waiting time after use	 */
    char *	noun_damage;		   /* Damage message */
    char *	msg_off;		   /* Wear off message		 */
};

// May expand to include more damage messages - Veygoth.
struct	spell_type
{
    char *	name;			   /* Name of skill		 */
    int 	spell_circle[ MAX_CLASS ]; /* Circle needed by class     */
    SPELL_FUN *	spell_fun;		   /* Spell pointer (for spells) */
    int 	target;			   /* Legal targets		 */
    bool 	cast_combat;		   /* Can you cast it in a fight?*/
    int *	pgsn;			   /* Pointer to associated gsn	 */
    int 	min_mana;		   /* Minimum mana used		 */
    int 	beats;			   /* Waiting time after use	 */
    char *	msg_damage;		   /* Damage message		 */
    char *	msg_damage_vict;	   /* Damage message to victim	 */
    char *	msg_damage_room;	   /* Damage message to room	 */
    char *	msg_damage_self;	   /* Damage message target self */
    char *	msg_damage_self_rm;	   /* Room dmg msg, self = target*/
    char *      msg_kill;                  /* Kill Message               */
    char *	msg_off;		   /* Wear off message		 */
    int		realms;			   /* Skill realm requirements 
                                             (Instrument required for bards) */
    int 	mana_type;		   /* Mana type (for spells)
                                              Instrument type (for bards) */
    bool        can_scribe;                /* Can the spell be scribed?  */
};

struct	song_type
{
    char *	fullname;		   /* Name of skill              */
    int 	spell_circle[ MAX_CLASS ]; /* Circle needed by class     */
    char *	name;			   /* Name of skill		 */
    SPELL_FUN *	spell_fun;		   /* Spell pointer (for spells) */
    int 	target;			   /* Legal targets		 */
    bool 	cast_combat;		   /* Can you cast it in a fight?*/
    int *	pgsn;			   /* Pointer to associated gsn	 */
    int 	min_mana;		   /* Minimum mana used		 */
    int 	beats;			   /* Waiting time after use	 */
    char *	msg_damage;		   /* Damage message		 */
    char *	msg_damage_vict;	   /* Damage message to victim	 */
    char *	msg_damage_room;	   /* Damage message to room	 */
    char *	msg_damage_self;	   /* Damage message target self */
    char *	msg_damage_self_rm;	   /* Room dmg msg, self = target*/
    char *      msg_kill;                  /* Kill Message               */
    char *	msg_off;		   /* Wear off message		 */
    int		realms;			   /* Skill realm requirements 
                                             (Instrument required for bards) */
    int 	mana_type;		   /* Mana type (for spells)
                                              Instrument type (for bards) */
    bool        can_scribe;                /* Can the spell be scribed?  */
};



/* Mob program structures */
struct	act_prog_data
{
    ACT_PROG_DATA *  next;
    void *           vo;
};

struct  mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *           buf;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    void *           vo;
};

struct  mob_prog_data
{
    MPROG_DATA *    next;
    int             type;
    char *          arglist;
    char *          comlist;
};

bool    MOBtrigger;

// Prog types used for both mobs and objects - Veygoth
#define ERROR_PROG		     -1
#define IN_FILE_PROG		      0
#define ACT_PROG		   BV00
#define SPEECH_PROG		   BV01
#define RAND_PROG		   BV02
#define FIGHT_PROG		   BV03
#define DEATH_PROG		   BV04
#define HITPRCNT_PROG		   BV05 // percentage attack prog - mobs only
#define ENTRY_PROG		   BV06
#define GREET_PROG		   BV07
#define ALL_GREET_PROG		   BV08
#define GIVE_PROG		   BV09
#define BRIBE_PROG		   BV10
#define PROC_PROG                  BV11 // weapon combat proc - objects only
#define MOVE_PROG                  BV12 // movement script - Veygoth

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int     gsn_aware;                      /* Veygoth */
extern	int	gsn_appraise;
extern  int     gsn_alcohol_tolerance;          /* Veygoth */
extern  int	gsn_backstab;
extern  int     gsn_bandage;                    /* Veygoth */
extern  int     gsn_bash;                       /* Veygoth */
extern  int     gsn_berserk;			/* by Thelonius */
extern  int	gsn_bladeflash;
extern  int     gsn_blindfighting;              /* Veygoth */
extern  int     gsn_bodyslam;                   /* Veygoth */
extern  int     gsn_charge;                     /* Veygoth */
extern  int     gsn_circle;			/* by Thelonius */
extern  int	gsn_disarm;
extern  int	gsn_dodge;
extern  int     gsn_drums;                      /* Veygoth */
extern  int     gsn_dulcimer;                   /* Veygoth */
extern  int     gsn_fiddle;                     /* Veygoth */
extern  int     gsn_flute;                      /* Veygoth */
extern  int     gsn_guard;                      /* Veygoth */
extern  int     gsn_headbutt;                   /* Veygoth */
extern  int     gsn_harp;                       /* Veygoth */
extern  int	gsn_hide;
extern  int     gsn_horn;                       /* Veygoth */
extern  int     gsn_instant_kill;               /* Veygoth */
extern  int	gsn_listen;
extern  int     gsn_lyre;                       /* Veygoth */
extern  int     gsn_mandolin;                   /* Veygoth */
extern  int	gsn_mounted_combat;
extern  int	gsn_peek;
extern  int	gsn_pick_lock;
extern  int	gsn_pickpocket;
extern  int     gsn_pipes;                      /* Veygoth */
extern  int     gsn_poison;                     /* Lithos */
extern  int     gsn_poison_bite;                /* Lithos */
extern  int     gsn_poison_weapon;		/* by Thelonius */
extern  int     gsn_quick_chant;                /* Veygoth */
extern  int	gsn_scrolls;			/* by Binky / Thelonius */
extern  int	gsn_summon_mount;		/* Lohrr */
extern  int	gsn_capture;			/* by Binky / Thelonius */
extern  int	gsn_sneak;
extern  int	gsn_staves;			/* by Binky / Thelonius */
extern  int	gsn_steal;
extern  int     gsn_switch;                     /* Veygoth */
extern  int	gsn_untangle;			/* by Thelonius */
extern  int	gsn_wands;			/* by Binky / Thelonius */
extern  int     gsn_doorbash;  
extern  int     gsn_dual;			/* by Thelonius */
extern	int	gsn_enhanced_damage;
extern	int	gsn_kick;
extern	int	gsn_parry;
extern	int	gsn_rescue;
extern  int     gsn_riposte;                    /* Veygoth */
extern	int	gsn_second_attack;
extern  int     gsn_singing;
extern  int     gsn_springleap;                 /* Veygoth */
extern	int	gsn_third_attack;
extern	int	gsn_throat_crush;
extern  int     gsn_throw;
/*
 * Psionicist gsn's (by Thelonius).
 */
extern  int     gsn_chameleon;
extern  int     gsn_heighten;
extern  int     gsn_shadow;
/*
 * New gsn's (by Zen & other ppl).
 */
extern  int     gsn_shield_block;
extern  int     gsn_fast_healing;
extern  int     gsn_fourth_attack;
extern	int	gsn_brew;
extern	int	gsn_scribe;
extern	int	gsn_dirt;
extern  int     gsn_meditate;			/* by Zen */
extern  int     gsn_swim;			/* by Zen */
extern	int	gsn_track;
extern  int     gsn_trip;
extern  int     gsn_unarmed;
extern	int	gsn_whirlwind;
extern	int	gsn_mount;
extern  int     gsn_safe_fall;
extern  int     gsn_climb;
extern  int     gsn_tumble;

extern	int	gsn_hit;
extern	int	gsn_slash;
extern	int	gsn_pierce;
extern	int	gsn_whip;
extern	int	gsn_explode;
extern	int	gsn_pound;
extern	int	gsn_suction;
extern	int	gsn_shot;

/*
 * Race gsn's (by Kahn).
 */
extern  int     gsn_vampiric_bite;

// Misc gsn's by Veygoth
extern  int     gsn_carve;
extern  int     gsn_layhands_timer;

/*
 * Spell GSN's
 *
 * Try to keep these in alphabetical order for ease of reference
 */
extern  int     spl_acid_arrow;
extern  int     spl_acid_blast;
extern  int     spl_agitation;
extern  int     spl_agony;
extern  int     spl_aid;
extern  int     spl_airy_starshell;
extern  int     spl_airy_water;
extern	int	spl_animate_dead; 		/* Govno */
extern  int     spl_armor;
extern  int     spl_barkskin;
extern  int     spl_battle_ecstasy;
extern  int     spl_bear;
extern  int     spl_blending;
extern  int     spl_bless;
extern  int     spl_blur;
extern  int     spl_burning_hands;
extern	int	spl_blindness;
extern  int     spl_bombard;
extern  int     spl_burning_hands;
extern  int     spl_cause_critical;
extern  int     spl_cause_light;
extern  int     spl_cause_serious;
extern  int     spl_change_self;
extern	int	spl_charm_person;
extern  int     spl_chill_touch;
extern	int	spl_cloak_of_fear;		/* Govno */
extern  int     spl_cold_ward;
extern  int     spl_cone_of_cold;
extern  int     spl_concealment;
extern  int     spl_conflaguration;
extern  int     spl_cowardice;
extern	int	spl_create_dracolich;		/* Govno */
extern  int     spl_creeping_doom;
extern  int     spl_cure_blind;
extern  int     spl_cure_critical;
extern  int     spl_cure_light;
extern  int     spl_cutting_breeze;
extern  int     spl_ball_lightning;
extern  int     spl_cure_serious;
extern	int	spl_curse;
extern  int     spl_dazzle;
extern  int     spl_destroy_undead;
extern  int     spl_detect_evil;
extern  int     spl_detect_good;
extern  int     spl_detect_invis;
extern  int     spl_detect_magic;
extern  int     spl_detect_undead;
extern  int     spl_dexterity;
extern  int     spl_dirt_cloud;
extern  int     spl_disintegrate;
extern  int     spl_domination;
extern  int     spl_dust_blast;
extern  int     spl_earthen_grasp;
extern  int     spl_earthen_starshell;
extern  int     spl_earthen_tomb;
extern  int     spl_elemental_form;
extern  int     spl_fiery_starshell;
extern  int     spl_earthquake;
extern  int     spl_elemental_sight;
extern  int     spl_elephant;
extern  int     spl_enfeeblement;
extern  int     spl_enlarge;
extern  int     spl_enthrall;
extern  int     spl_energy_drain;
extern  int     spl_fear;
extern  int     spl_feeblemind;
extern  int     spl_fiery_starshell;
extern  int     spl_fireball;
extern  int     spl_fire_bolt;
extern  int     spl_fire_ward;
extern  int     spl_firestorm;
extern  int     spl_flameburst;
extern	int	spl_flame_shield;
extern	int	spl_flamestrike;
extern  int     spl_fly;
extern	int	spl_frost_shield;
extern  int     spl_fullheal;
extern  int     spl_full_harm;
extern  int     spl_giant_strength;
extern  int     spl_gr_earthen_grasp;
extern	int	spl_gr_mending;
extern  int     spl_harm;
extern  int     spl_haste;
extern  int     spl_hawk;
extern  int     spl_heal;
extern  int     spl_herbal_remedy;
extern  int     spl_hex;
extern  int     spl_holy_sacrifice;
extern  int     spl_hypnotic_pattern;
extern  int     spl_remove_poison;
extern  int     spl_resurrect;
extern	int	spl_heal_undead;		/* Govno */
extern  int     spl_holy_word;
extern  int     spl_ice_bolt;
extern  int     spl_ice_missile;
extern  int     spl_identify;
extern  int     spl_immolate;
extern  int     spl_incendiary_cloud;
extern  int     spl_incompetence;
extern  int     spl_iceball;
extern  int     spl_ice_storm;
extern  int     spl_infravision;
extern	int	spl_invis;
extern  int     spl_levitation;
extern  int     spl_lightanddark;
extern  int     spl_lightning_bolt;
extern  int     spl_lightning_curtain;
extern  int     spl_ls_herbal_remedy;
extern	int	spl_ls_mending;
extern  int     spl_lion;
extern  int     spl_magic_missile;
extern  int     spl_magnetism;
extern  int     spl_major_globe;
extern  int     spl_malison;
extern  int     spl_mass_heal;
extern	int	spl_mass_invis;
extern  int     spl_mass_vortex_lift;		/* by Zen */
extern	int	spl_mending;
extern  int     spl_mermaids_kiss;
extern  int     spl_meteor;
extern  int     spl_minor_para;
extern  int     spl_m_blending;
extern  int     spl_mirage_arcana;
extern  int     spl_misdirection;
extern  int     spl_mole;
extern  int     spl_molten;
extern  int     spl_mouse;
extern  int     spl_nightmares;
extern  int     spl_panther;
extern  int     spl_pebble;
extern  int     spl_phantasmal_killer;
extern	int	spl_plague;
extern	int	spl_poison;
extern  int     spl_polymorph;
extern  int     spl_prismatic_spray;
extern  int     spl_protect_acid;
extern  int     spl_protect_cold;
extern  int     spl_protect_fire;
extern  int     spl_protect_gas;
extern  int     spl_protect_lightning;
extern	int	spl_protect_undead;		/* Govno */
extern  int     spl_prot_evil;
extern  int     spl_prowess;
extern  int     spl_pythonsting;
extern	int	spl_raise_skeleton;		/* Govno */
extern  int     spl_raise_spectre;              /* Govno */
extern  int     spl_raise_vampire;              /* Govno */
extern  int     spl_raise_wraith;               /* Govno */
extern  int     spl_raise_lich;                 /* Govno */
extern  int     spl_raise_zombie;               /* Govno */
extern  int     spl_ravenflight;
extern  int     spl_recall;
extern  int     spl_reduce;
extern  int     spl_rejuvenate;
extern  int     spl_scalding;
extern  int     spl_scathing_wind;
extern  int     spl_scorching;
extern  int     spl_shield;
extern  int     spl_shocking_grasp;
extern	int	spl_shock_shield;
extern  int     spl_shrew;
extern  int     spl_silence;			/* by Thelonius */
extern	int	spl_sleep;
extern  int     spl_slowness;
extern  int     spl_snail;
extern  int     spl_soothe;
extern  int     spl_soul_disturbance;
extern  int     spl_soulshield;
extern  int     spl_spark;
extern  int     spl_spirit_anguish;
extern  int     spl_spirit_armor;
extern  int     spl_stoneskin;
extern  int     spl_stonestorm;
extern  int     spl_stornogs_spheres;
extern	int	spl_sunray;
extern  int     spl_sustenance;
extern  int     spl_telekinesis;
extern  int     spl_teleport;
extern	int	spl_turn_undead;
extern  int     spl_unholy_word;
extern  int     spl_vacancy;
extern  int     spl_vig_critical;
extern  int     spl_vig_light;
extern  int     spl_vig_serious;
extern  int     spl_vitality;
extern  int     spl_wall_of_fire;
extern  int     spl_wall_of_ice;
extern  int     spl_wall_of_mist;
extern  int     spl_wall_of_sparks;
extern  int     spl_water_breathing;		/* by Thelonius */
extern  int     spl_water_blast;
extern  int     spl_water_bolt;
extern  int     spl_watery_starshell;
extern  int     spl_wierd;
extern  int     spl_wither;
extern  int     spl_wolf;
extern  int     spl_ultra_death_ray;
extern  int     spl_cutting_breeze;
extern  int     spl_summon_elemental;
extern  int     spl_bonethunder;
// Traps
extern  int     trp_acid;
extern  int     trp_bash;
extern  int     trp_cold;
extern  int     trp_disease;
extern  int     trp_dispel;
extern  int     trp_energy;
extern  int     trp_fire;
extern  int     trp_gate;
extern  int     trp_harm;
extern  int     trp_para;
extern  int     trp_pierce;
extern  int     trp_poison;
extern  int     trp_slash;
extern  int     trp_sleep;
extern  int     trp_stun;
extern  int     trp_summon;
extern  int     trp_teleport;
extern  int     trp_wither;
/* More spl_xxx declarations, not alphabetical */
extern int spl_tridenttides;				//**Saeven
extern int spl_bird_prey;
extern int spl_darkness;
extern int spl_miners_intuition;
extern int spl_wall_iron;
extern int spl_wall_stone;
extern int spl_wall_force;
extern int spl_wall_illusion;
extern int spl_mirror_image;
extern int spl_shadow_monsters;
extern int spl_demi_shad_monster;
extern int spl_shades;
extern int spl_cloak_fear;
extern int spl_clone;
extern int spl_power_word_blind;
extern int spl_spook;
extern int spl_sunburst;
extern int spl_call_lightning;
extern int spl_gr_agony;
extern int spl_m_agony;
extern int spl_ravenous_vines;
extern int spl_change_sex;
extern int spl_color_spray;
extern int spl_prismatic_orb;
extern int spl_shadow_magic;
extern int spl_demi_shad_magic;
extern int spl_cone_silence;
extern int spl_continual_light;
extern int spl_twilight;
extern int spl_control_weather;
extern int spl_create_food;
extern int spl_conjure_windsaber;
extern int spl_celestial_sword;
extern int spl_create_skin;
extern int spl_create_spring;
extern int spl_create_watern;
extern int spl_purify_spirit;
extern int spl_purify;
extern int spl_cure_disease;
extern int spl_nourishment;
extern int spl_ls_nourishment;
extern int spl_gr_sustenance;
extern int spl_rain_maker;
extern int spl_destroy_cursed;
extern int spl_spirit_sight;
extern int spl_analyze_balance;
extern int spl_sense_life;
extern int spl_sense_spirit;
extern int spl_gr_spirit_sight;
extern int spl_true_seeing;
extern int spl_comp_lang;
extern int spl_detect_poison;
extern int spl_dispel_evil;
extern int spl_gleam_dawn;
extern int spl_gleam_dusk;
extern int spl_dispel_magic;
extern int spl_enchant_weapon;
extern int spl_exorcise;
extern int spl_faerie_fire;
extern int spl_faerie_fog;
extern int spl_reveal_true_form;
extern int spl_reveal_true_name;
extern int spl_dispel_invisible;
extern int spl_negate_veil;
extern int spl_negate_luster;
extern int spl_illumination;
extern int spl_bigby_fist;
extern int spl_akiaurn_word;
extern int spl_windsaber;
extern int spl_flight;
extern int spl_gr_ravenflight;
extern int spl_gate;
extern int spl_farsee;
extern int spl_know_alignment;
extern int spl_reveal_spirit_essence;
extern int spl_touch_nightshade;
extern int spl_locate_object;
extern int spl_minor_creation;
extern int spl_elemental_creation;
extern int spl_tidal_wave;
extern int spl_cyclone;
extern int spl_whirlwind;
extern int spl_inferno;
extern int spl_earthen_rain;
extern int spl_gr_soul_dist;
extern int spl_group_heal;
extern int spl_wellness;
extern int spl_pass_door;
extern int spl_gr_pythonsting;
extern int spl_call_wild;
extern int spl_minor_globe;
extern int spl_group_globe;
extern int spl_spirit_ward;
extern int spl_gr_spirit_ward;
extern int spl_deny_air;
extern int spl_deny_earth;
extern int spl_deny_fire;
extern int spl_deny_water;
extern int spl_protection_good;
extern int spl_harbor_balance;
extern int spl_adaptation;
extern int spl_recharge_item;
extern int spl_stamina;
extern int spl_gr_stamina;
extern int spl_remove_alignment;
extern int spl_preserve;
extern int spl_remove_curse;
extern int spl_negate_hex;
extern int spl_remove_silence;
extern int spl_sanctuary;
extern int spl_shadow_shield;
extern int spl_entangle;
extern int spl_thirst;
extern int spl_group_stone;
extern int spl_hurricane;
extern int spl_flashfire;
extern int spl_mass_dispel_magic;
extern int spl_stonestrength;
extern int spl_summon;
extern int spl_plane_shift;
extern int spl_flickering_flame;
extern int spl_ripple;
extern int spl_ventriloquate;
extern int spl_group_recall;
extern int spl_acid_breath;
extern int spl_fire_breath;
extern int spl_water_breath;
extern int spl_frost_breath;
extern int spl_gas_breath;
extern int spl_lightning_breath;
extern int spl_general_purpose;
extern int spl_high_explosive;
extern int spl_adrenaline_control;
extern int spl_aura_sight;
extern int spl_awe;
extern int spl_ballistic_attack;
extern int spl_biofeedback;
extern int spl_cell_adjustment;
extern int spl_combat_mind;
extern int spl_complete_healing;
extern int spl_control_flames;
extern int spl_create_sound;
extern int spl_deathfield;
extern int spl_detonate;
extern int spl_neurofrag;
extern int spl_displacement;
extern int spl_ectoplasmic_form;
extern int spl_ego_whip;
extern int spl_energy_containment;
extern int spl_enhance_armor;
extern int spl_enhanced_strength;
extern int spl_flesh_armor;
extern int spl_inertial_barrier;
extern int spl_inflict_pain;
extern int spl_intellect_fortress;
extern int spl_lend_health;
extern int spl_transfer_wellness;
extern int spl_mental_barrier;
extern int spl_mind_thrust;
extern int spl_project_force;
extern int spl_psionic_blast;
extern int spl_psychic_crush;
extern int spl_psychic_drain;
extern int spl_psychic_healing;
extern int spl_share_strength;
extern int spl_thought_shield;
extern int spl_ultrablast;
extern int spl_chain_lightning;
extern int spl_wizard_eye;
extern int spl_vortex_lift;
extern int spl_shadow_walk;
extern int spl_dimdoor;
extern int spl_migration;
extern int spl_shadow_door;
extern int spl_shadow_veil;
extern int spl_spirit_jump;
extern int spl_dispel_good;
extern int spl_judgement;
extern int spl_apocalypse;
extern int spl_power_word_lag;
extern int spl_power_word_stun;
extern int spl_etherportal;
extern int spl_moonwell;
extern int spl_wormhole;
extern int spl_create_buffet;
extern int spl_fireskin;
extern int spl_ethereal_shield;
extern int spl_slow_poison;
extern int spl_acid_spray;
extern int spl_aura;
extern int spl_earthen_smith;
extern int spl_fiery_smith;
extern int spl_airy_smith;
extern int spl_aquatic_smith;
extern int spl_hover;
extern int spl_azure_flame;
extern int spl_gust_of_frost;
extern int spl_tide_seas;
extern int spl_dismissal;
extern int spl_summon_serpent;
extern int spl_summon_nerneid;
extern int spl_summon_pech;
extern int spl_summon_slyph;
extern int spl_summon_gr_elem;


/* 
 * Race GSN declarations
 * be sure to add them in db.c
 *
 */
extern int    race_human;
extern int    race_barbarian;
extern int    race_grey_elf;
extern int    race_rakshasa;
extern int    race_half_elf;
extern int    race_flind;
extern int    race_drow_elf;
extern int    race_dwarf;
extern int    race_duergar;
extern int    race_halfling;
extern int    race_troll;
extern int    race_ogre;
extern int    race_orc;
extern int    race_gnome;
extern int    race_centaur;
extern int    race_githyanki;
extern int    race_goblin;
extern int    race_minotaur;
extern int    race_githzerai;
extern int    race_illithid;
extern int    race_thri_kreen;
extern int    race_aquatic_elf;
extern int    race_sahaugin;
extern int    race_half_orc;
extern int    race_half_dwarf;
extern int    race_half_kobold;
extern int    race_giant;
extern int    race_neogi;
extern int    race_kobold;
extern int    race_hobgoblin;
extern int    race_hobgoblin;
extern int    race_humanoid;
extern int    race_demon;
extern int    race_god;
extern int    race_devil;
extern int    race_angel;
extern int    race_faerie;
extern int    race_vampire;
extern int    race_werewolf;
extern int    RACE_DRAGON;
extern int    race_beholder;
extern int    race_derro;
extern int    race_golem;
extern int    race_dracolich;
extern int    race_dragonkin;
extern int    race_insect;
extern int    race_animal;
extern int    race_herbivore;
extern int    race_carnivore;
extern int    race_bird;
extern int    race_horse;
extern int    race_primate;
extern int    race_undead;
extern int    race_ghost;
extern int    race_harpy;
extern int    race_bear;
extern int    race_rust_monster;
extern int    race_fire_elemental;
extern int    race_air_elemental;
extern int    race_water_elemental;
extern int    race_earth_elemental;
extern int    race_bat;
extern int    race_plant;
extern int    race_tree;
extern int    race_rat;
extern int    race_parasite;
extern int    race_lycanthrope;
extern int    race_arachnid;
extern int    race_object;
extern int    race_mist;
extern int    race_snake;
extern int    race_worm;
extern int    race_fish;
extern int    race_ixitxachitl;
extern int    race_aboleth;
extern int    race_hydra;
extern int    race_reptile;
extern int    race_boar;


extern  char    *last_funs[MAX_LAST_FUNS];
extern  bool    last_funs_init;
extern  int     last_fun_count;
extern  char    *last_coms[MAX_LAST_COMS];
extern  bool    last_coms_init;
extern  int     last_com_count;

/*
 * Utility macros.
 */
#define UMIN( a, b )		( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )		( ( a ) > ( b ) ? ( a ) : ( b ) )
#define URANGE( a, b, c )	( ( b ) < ( a ) ? ( a )                       \
				                : ( ( b ) > ( c ) ? ( c )     \
						                  : ( b ) ) )
#define LOWER( c )		( ( c ) >= 'A' && ( c ) <= 'Z'                \
				                ? ( c ) + 'a' - 'A' : ( c ) )
#define UPPER( c )		( ( c ) >= 'a' && ( c ) <= 'z'                \
				                ? ( c ) + 'A' - 'a' : ( c ) )
#define IS_SET( flag, bit )	( ( flag ) &   ( bit ) )
#define SET_BIT( var, bit )     ( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )  ( ( var )  &= ~( bit ) )
#define TOGGLE_BIT( var, bit )  ( ( var )  ^=  ( bit ) )
// IS_AFFECTED now in bit.c - Veygoth


/*
 * Character macros.
 */
#define IS_NPC( ch )		( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )	( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )		( get_trust( ch ) >= LEVEL_HERO     )

#define IS_GOOD( ch )		( ch->alignment >=  350 )
#define IS_EVIL( ch )		( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )	( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define GET_RACEWAR( ch )       ( race_table[ch->race].racewar_side )
#define RACEWAR(ch, tch)        (!IS_IMMORTAL(ch) && !IS_IMMORTAL(tch) && \
                                 !IS_NPC(ch) && !IS_NPC(tch) && \
                                (GET_RACEWAR(ch) != GET_RACEWAR(ch)))

#define IS_AWAKE( ch )		( ch->position > POS_SLEEPING )

#define IS_OUTSIDE( ch )        ( !IS_ROOM( ( ch )->in_room, ROOM_INDOORS ) )

#define WAIT_STATE( ch, pulse ) ( ( ch )->wait += pulse )

#define MANA_BASE( ch, sn )     ( 30 + 15 *                                 \
                                ( spells_table[sn].spell_circle[ch->class] ) )

#define MANA_SCALE( ch, sn )    (spells_table[sn].spell_circle[ch->class] + 9 )

#define MANA_LEV( ch, sn )      ( ( ch->level - 1 ) - 5 * (                 \
                                spells_table[sn].spell_circle[ch->class] - 1 ) )

#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX( spells_table[sn].min_mana, \
                                MANA_BASE( ch, sn ) - MANA_SCALE( ch, sn) * \
                                MANA_LEV( ch, sn ) ) )

/*  This is the old mana cost function. - Lohrr
#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX (                 \
				spells_table[sn].min_mana,                  \
				100 / ( 2 + UMAX ( 0, ch->level -           \
		                spells_table[sn].spell_circle[ch->class] ) ) ) )
*/

#define IS_SWITCHED( ch )       ( ch->pcdata->switched )

#define	WCHAN( ch, chan )	( IS_SET( ch->pcdata->wiznet, chan ) )

#define CHECK_RES( ch, bit )	( IS_SET( ch->resistant                      \
				    | race_table[ch->race].resistant, bit ) )
#define CHECK_IMM( ch, bit )	( IS_SET( ch->immune                         \
				    | race_table[ch->race].immune, bit ) )
#define CHECK_SUS( ch, bit )	( IS_SET( ch->susceptible                    \
				    | race_table[ch->race].susceptible, bit ) )

#define HAS_CONSENT( ch, victim ) ( victim->pcdata->consent == ch )

#define IS_UNDEAD( ch )		((  (ch)->race == RACE_VAMPIRE || (ch)->race == RACE_GHOST || \
                                (ch)->race == RACE_DRACOLICH || (ch)->race == RACE_UNDEAD ))

#define IS_ELEMENTAL( ch )      (( (ch)->race == race_air_elemental || (ch)->race == race_fire_elemental || \
                                (ch)->race == race_earth_elemental || (ch)->race == race_water_elemental ))
/*
 * Object macros.
 */
#define CAN_WEAR( obj, part )	( IS_SET( ( obj)->wear_flags,  ( part ) ) )


/*
 * Description macros.
 */
#define PERS( ch, looker )	( can_see( looker, ( ch ) ) ?		     \
				( IS_NPC( ch ) ? ( ch )->short_descr	     \
				: ( ( race_table[ch->race].racewar_side ==   \
				  race_table[looker->race].racewar_side ||   \
				  (IS_IMMORTAL((ch))||IS_IMMORTAL(looker)))? \
				  ( ch )->name : race_name ( ( ch ) ) ) ) :  \
					"someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    int 		position;
    int 		level;
    int 		log;
    bool		show;
    bool                snapvis;  // Does the command cause the person to snap vis
    bool                unhide;   // Does the command cause the person to break hide
    bool                breakmed; // Does the command interrupt meditation
};



/*
 * Structure for a social in the socials table.
 */
struct	soc_index_data
{
    SOC_INDEX_DATA *	next;
    char *		name;
    char *		char_no_arg;
    char *		others_no_arg;
    char *		char_found;
    char *		others_found;
    char *		vict_found;
    char *		char_auto;
    char *		others_auto;
    bool		deleted;
};



/*
 * Global constants.
 */
// Veygoth - converted to 100-point based system, but allowing up to
// 250 for insane godlike things like the strength of a titan...
// 100 is normal human average.
extern	const	struct	str_app_type	str_app		[ 251 ];
extern	const	struct	int_app_type	int_app		[ 251 ];
extern	const	struct	wis_app_type	wis_app		[ 251 ];
extern	const	struct	agi_app_type	agi_app		[ 251 ];
extern	const	struct	con_app_type	con_app		[ 251 ];

extern		struct	class_type *	class_table	[ MAX_CLASS   ];
extern	const	struct	noun_type 	noun_table	[ MAX_NOUN   ];
extern	const	struct	verb_type 	verb_table	[ MAX_VERB   ];
extern  const   struct  wear_data       wear_table      [ NUM_ITEM_WEAR_VECTORS ];
extern	const	struct	cmd_type	cmd_table	[ ];
extern	const	struct	liq_type	liq_table	[ LIQ_MAX     ];
extern		struct	skill_type	skills_table	[ MAX_SKILL   ];
extern		struct	spell_type	spells_table	[ MAX_SPELL   ];
extern		struct	song_type	songs_table	[ MAX_SONG    ];
extern  const   struct  race_type       race_table      [ MAX_RACE ];
extern  const           char *          lang_table      [ MAX_LANG ];
extern  const   struct  attack_type     attack_table    [ ];
extern  const   struct  struckdrunk     drunk           [ ];
extern  const   int     memchart        [MAX_LEVEL]     [MAX_CIRCLE];
extern  const   int     lesser_memchart [MAX_LEVEL]     [MAX_CIRCLE];
extern  const   int     topdown_eq      [MAX_WEAR];
extern  const   struct  extended_terrain map_info[MAX_EXTENDED_TERRAIN];

/* WAIT_SEARCH = Lag created by search. */
extern	const	int			WAIT_SEARCH;

// Illegal name list
extern          char    strName                         [ ];

// Buffer to keep track of the last command used & room it was in.
extern char last_command[MAX_INPUT_LENGTH];
extern int  last_command_room;

// Targets for spells.c
extern  char *target_name;

// Race screen
char * help_races_color;
char * help_races_nocolor;

/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;
extern		GAME_DATA	  *	game_first;
extern		CLAN_DATA	  *	clan_first;
extern          QUEST_INDEX_DATA  *     quest_first;
extern          EVENT_DATA        *     event_first;
extern          MEM_DATA          *     mem_first;
extern          MEM_DATA          *     memdata_free;
extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		CAST_DATA	  *	cast_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		AFFECT_DATA	  *	affect_free;
extern          VEHICLE_DATA      *     vehicle_list;
extern          VEHICLE_DATA      *     vehicle_free;
extern          CRIME_DATA        *     crime_list;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;
extern          FRAG_DATA               fraglist;
extern		char			bug_buf		[ ];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[ ];
extern		char			log_buf		[ ];
extern		char			executable_directory []; // The folder containing the executable.
extern		char			executable_path []; // Path of the executable, including flename.
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;
extern          time_t                  down_time;
extern          time_t                  warning1;
extern          time_t                  warning2;
extern          bool                    Reboot;
extern		SYSTEM_DATA		sysdata;
extern          PROG_DATA               progdata;
extern		int			num_descriptors;

extern          int                     top_note;
extern          int                     top_event;
extern          int                     top_memdata;
extern          int                     top_ban;
extern          int                     top_desc;
extern		int                     top_social;
extern		int                     top_vehicle;
extern		int                     top_obj;
extern		int                     top_char;
extern		int                     top_pcdata;
extern          int			top_crime;
extern          int                     top_mvscript;

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN(	do_afk  	);
DECLARE_DO_FUN( do_aggressive   );
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN( do_ask          );
DECLARE_DO_FUN( do_assist       ); // Veygoth
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN( do_attributes   ); // Veygoth
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_aware        ); // Veygoth
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN( do_bandage      ); // Veygoth
DECLARE_DO_FUN( do_bash         ); // Veygoth
DECLARE_DO_FUN(	do_beep 	);
DECLARE_DO_FUN(	do_berserk 	);		/* by Thelonius */
DECLARE_DO_FUN(	do_bet  	);		/* by Thelonius */
DECLARE_DO_FUN( do_bitflag      ); // Veygoth
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_bodyslam     ); // Veygoth
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN( do_camp		); // Govno
DECLARE_DO_FUN( do_carve        ); // Veygoth
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_castlist     ); // Veygoth
DECLARE_DO_FUN(	do_cdonate	);
DECLARE_DO_FUN( do_chameleon    );		/* by Thelonius */
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN( do_charge       ); // Veygoth
DECLARE_DO_FUN( do_charlist     ); // Veygoth
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_circle 	);		/* by Thelonius */
DECLARE_DO_FUN(	do_claninfo	);
DECLARE_DO_FUN(	do_clans	);
DECLARE_DO_FUN( do_climb        );
DECLARE_DO_FUN( do_clone	); /* Govno */
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_color        );	/* Colour Command By Lope */
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN( do_consent      ); // Veygoth
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN( do_crashbug     ); // Govno
DECLARE_DO_FUN( do_crashmud     ); // Lithos
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_cwithdraw    ); // Veygoth
DECLARE_DO_FUN( do_decline      ); // Veygoth
DECLARE_DO_FUN( do_delete       ); // Lithos
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN( do_deposit      ); // Veygoth
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN( do_dice         ); // Veygoth
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN( do_disband      );
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN( do_disembark    ); // Veygoth
DECLARE_DO_FUN( do_disengage    ); // Veygoth
DECLARE_DO_FUN(	do_donate	);
DECLARE_DO_FUN(	do_doorbash 	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN( do_drag         ); // Veygoth
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN( do_exp          );
DECLARE_DO_FUN(	do_fee		);
DECLARE_DO_FUN(	do_feed		);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_fine         ); // Veygoth
DECLARE_DO_FUN(	do_pour		);
DECLARE_DO_FUN( do_find         ); // Veygoth
DECLARE_DO_FUN( do_finger       ); // Lohrr
DECLARE_DO_FUN( do_firstaid     ); // Veygoth
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN( do_fly          ); // Veygoth
DECLARE_DO_FUN( do_fog          ); // Veygoth
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN( do_forage       ); // Lithos
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN( do_forget       ); // Veygoth
DECLARE_DO_FUN( do_fraglist     ); // Veygoth
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_glance       ); // Veygoth
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN( do_gstat        );
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_guard        ); // Veygoth
DECLARE_DO_FUN( do_headbutt     ); // Veygoth
DECLARE_DO_FUN( do_heighten     );		/* by Thelonius */
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN( do_hide_item    ); // Veygoth
DECLARE_DO_FUN(	do_hold		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN( do_hometown     ); // Lohrr
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN( do_ignore       ); // Veygoth
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_imtlset	);
DECLARE_DO_FUN( do_innate       ); // Veygoth
DECLARE_DO_FUN( do_invoke       ); // Lithos
DECLARE_DO_FUN( do_justice      ); // Veygoth
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN( do_kickout      ); // Veygoth
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN( do_killproc     ); // Govno
DECLARE_DO_FUN( do_kneel        );
DECLARE_DO_FUN( do_layhands     ); // Lithos
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN( do_load         );
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN( do_lookout      ); // Veygoth
DECLARE_DO_FUN( do_lookpanel    ); // Veygoth
DECLARE_DO_FUN( do_loopmud      ); // Lithos
DECLARE_DO_FUN( do_lset         ); // Lohrr
DECLARE_DO_FUN( do_map          ); // Lithos
DECLARE_DO_FUN( do_meditate     );
DECLARE_DO_FUN( do_memdata      ); // Veygoth
DECLARE_DO_FUN( do_memorize     ); // Veygoth
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN( do_mobstat_save ); // Veygoth
DECLARE_DO_FUN( do_monk_sset    ); // Veygoth
DECLARE_DO_FUN( do_monkstat     ); // Veygoth
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN( do_namesucks    ); // Veygoth
DECLARE_DO_FUN(	do_newlock	);
DECLARE_DO_FUN( do_news         ); // Veygoth
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN( do_northeast    ); // Veygoth
DECLARE_DO_FUN( do_northwest    ); // Veygoth
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_numlock	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN( do_ordership    ); // Veygoth
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_ostracize    ); // Veygoth
DECLARE_DO_FUN(	do_owhere	);
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN( do_petition     ); // Veygoth
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN( do_play         ); // Veygoth
DECLARE_DO_FUN(	do_poison_weapon);		/* by Thelonius */
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_pray		); // Govno
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_ptell        ); // Veygoth
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_pwhere	);
DECLARE_DO_FUN( do_qstat        ); // Veygoth
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN( do_racestat     ); // Veygoth
DECLARE_DO_FUN( do_random       ); // Lohrr
DECLARE_DO_FUN( do_read         ); // Lithos
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN( do_recline      ); // Veygoth
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN( do_rename	); // Govno
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN( do_reset        );
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_retir	);
DECLARE_DO_FUN(	do_retire	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rfind	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_sign		); // Lohrr
DECLARE_DO_FUN(	do_cant		); // Lohrr
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_search	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_set          ); // Veygoth
DECLARE_DO_FUN( do_shadow       );		/* by Thelonius */
DECLARE_DO_FUN( do_shift        ); // Lithos
DECLARE_DO_FUN( do_shopstat     ); // Veygoth
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN( do_showdesc     ); // Veygoth
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN( do_sing         ); // Veygoth
DECLARE_DO_FUN( do_sit          ); // Veygoth
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_capture	);		/* by Binky / Thelonius */
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN( do_society      ); // Veygoth
DECLARE_DO_FUN( do_socname      ); // Veygoth
DECLARE_DO_FUN( do_songs        ); // Veygoth
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_southeast    ); // Veygoth
DECLARE_DO_FUN( do_southwest    ); // Veygoth
DECLARE_DO_FUN( do_speak        ); // Veygoth
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN( do_springleap   ); // Veygoth
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_splookup	); // Veygoth
DECLARE_DO_FUN( do_splset       ); // Veygoth
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_sstime	);
DECLARE_DO_FUN(	do_stake	);
DECLARE_DO_FUN( do_stance       ); // Veygoth
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stat         ); // Veygoth
DECLARE_DO_FUN( do_statdump     ); // Veygoth
DECLARE_DO_FUN( do_statcrime    ); // Veygoth
DECLARE_DO_FUN( do_statfear     ); // Veygoth
DECLARE_DO_FUN( do_stathate     ); // Veygoth
DECLARE_DO_FUN( do_stathunt     ); // Veygoth
DECLARE_DO_FUN( do_statvehicle  ); // Veygoth
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN( do_suicide      ); // Veygoth
DECLARE_DO_FUN( do_summon_mount ); // Lohrr
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN( do_throw        ); //Lithos
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_traps        ); // Veygoth
DECLARE_DO_FUN( do_trip         ); // Veygoth
DECLARE_DO_FUN( do_trophy       ); // Veygoth
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN( do_ungroup      );
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN( do_untangle     );		/* by Thelonius */
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN( do_use          ); // Veygoth
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_wield	);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	); // Veygoth
DECLARE_DO_FUN(	do_whirlwind	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN( do_will         ); // Veygoth
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN( do_withdraw     ); // Veygoth
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_worldmap     ); // Veygoth
DECLARE_DO_FUN( do_write        ); // Lithos
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);

DECLARE_DO_FUN( do_mpedit       );

DECLARE_DO_FUN(	do_brew		);
DECLARE_DO_FUN(	do_scribe	);
DECLARE_DO_FUN(	do_terminat	);
DECLARE_DO_FUN( do_terminate	);
DECLARE_DO_FUN(	do_enter	);
DECLARE_DO_FUN(	do_alia		);
DECLARE_DO_FUN(	do_alias	);
DECLARE_DO_FUN(	do_unalia	);
DECLARE_DO_FUN(	do_unalias	);
DECLARE_DO_FUN(	do_dirt		);
DECLARE_DO_FUN(	do_grats	);
DECLARE_DO_FUN(	do_wartalk	);
DECLARE_DO_FUN(	do_sober	);
DECLARE_DO_FUN(	do_clookup	);
DECLARE_DO_FUN(	do_track	);
DECLARE_DO_FUN(	do_mount	);
DECLARE_DO_FUN(	do_dismount	);

DECLARE_DO_FUN(	do_initiate	);
DECLARE_DO_FUN(	do_apply	);
DECLARE_DO_FUN(	do_exil		);
DECLARE_DO_FUN(	do_exile	);
DECLARE_DO_FUN(	do_promote	);
DECLARE_DO_FUN(	do_demote	);
DECLARE_DO_FUN(	do_leav		);
DECLARE_DO_FUN(	do_leave	);

DECLARE_DO_FUN(	do_showclan	);
DECLARE_DO_FUN(	do_setclan	);
DECLARE_DO_FUN(	do_makeclan	);
DECLARE_DO_FUN( do_killclan     );

DECLARE_DO_FUN(	do_mudconfig	);

DECLARE_DO_FUN(	do_shoot	);
DECLARE_DO_FUN(	do_reload	);
DECLARE_DO_FUN( do_wiznet       );
DECLARE_DO_FUN(	do_astat	);
DECLARE_DO_FUN(	do_history	);
DECLARE_DO_FUN( do_zreset	); // Govno


/*
 * Spell functions.
 * Defined in spells.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(      spell_acid_arrow        ); // Veygoth
DECLARE_SPELL_FUN(	spell_acid_blast	); // Xueqin
DECLARE_SPELL_FUN(	spell_acid_spray	); // Xueqin
DECLARE_SPELL_FUN(      spell_adaptation        ); // Veygoth
DECLARE_SPELL_FUN(      spell_aid               ); // Ski
DECLARE_SPELL_FUN(	spell_airy_smith	); // Xueqin
DECLARE_SPELL_FUN(      spell_airy_starshell    ); // Veygoth
DECLARE_SPELL_FUN(      spell_airy_water        );
DECLARE_SPELL_FUN(      spell_akiaurn_word      ); // Veygoth
DECLARE_SPELL_FUN(      spell_analyze_balance   ); // Veygoth
DECLARE_SPELL_FUN(	spell_animate_dead	); // Govno
DECLARE_SPELL_FUN(	spell_raise_skeleton	); // Govno
DECLARE_SPELL_FUN(	spell_raise_zombie	); // Govno
DECLARE_SPELL_FUN(	spell_raise_spectre	); // Govno
DECLARE_SPELL_FUN(	spell_raise_wraith	); // Govno
DECLARE_SPELL_FUN(	spell_raise_vampire	); // Govno
DECLARE_SPELL_FUN(	spell_raise_lich	); // Govno
DECLARE_SPELL_FUN(      spell_apocalypse        ); // Veygoth
DECLARE_SPELL_FUN(	spell_aquatic_smith	); // Xueqin
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_aura		); // Xueqin
DECLARE_SPELL_FUN(	spell_azure_flame	); // Xueqin
DECLARE_SPELL_FUN(      spell_barkskin          ); // Veygoth
DECLARE_SPELL_FUN(      spell_battle_ecstasy    ); // Lithos
DECLARE_SPELL_FUN(      spell_bearstrength      ); // Veygoth
DECLARE_SPELL_FUN(	spell_blending		); // Xueqin
DECLARE_SPELL_FUN(      spell_bigby_fist        );
DECLARE_SPELL_FUN(      spell_bird_prey         ); // Veygoth
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(      spell_blur              ); // Veygoth
DECLARE_SPELL_FUN(	spell_bombard		); // Xueqin
DECLARE_SPELL_FUN(	spell_bonethunder	); // Ski
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(      spell_call_wild         ); // Veygoth
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(      spell_celestial_sword   ); // Veygoth
DECLARE_SPELL_FUN(      spell_change_self       ); // Veygoth
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_cloak_of_fear	); // Govno
DECLARE_SPELL_FUN(      spell_clone             ); // Veygoth
DECLARE_SPELL_FUN(      spell_cold_ward         ); // Veygoth
DECLARE_SPELL_FUN(	spell_color_spray	);
DECLARE_SPELL_FUN(      spell_comp_lang         ); // Veygoth
DECLARE_SPELL_FUN(      spell_concealment       ); // Xueqin
DECLARE_SPELL_FUN(      spell_cone_of_cold      ); // Veygoth
DECLARE_SPELL_FUN(	spell_cone_of_silence	);	/* by Thelonius */
DECLARE_SPELL_FUN(      spell_conflaguration    ); // Veygoth
DECLARE_SPELL_FUN(      spell_conjure_windsaber ); // Veygoth
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(      spell_cowardice         ); // Veygoth
DECLARE_SPELL_FUN(	spell_create_dracolich	); // Govno
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_skin	); // Veygoth
DECLARE_SPELL_FUN(      spell_create_spring     ); // Veygoth
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(      spell_creeping_doom     ); // Veygoth
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_remove_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_cutting_breeze	); // Xueqin
DECLARE_SPELL_FUN(      spell_cyclone           ); // Veygoth
DECLARE_SPELL_FUN(      spell_darkness          ); // Veygoth
DECLARE_SPELL_FUN(      spell_dazzle            ); // Veygoth
DECLARE_SPELL_FUN(      spell_demi_shadow_magic ); // Veygoth
DECLARE_SPELL_FUN(      spell_demi_shadow_monsters); // Veygoth
DECLARE_SPELL_FUN(	spell_destroy_cursed    );
DECLARE_SPELL_FUN(	spell_destroy_undead    ); // Veygoth
DECLARE_SPELL_FUN(      spell_deny_air          ); // Veygoth
DECLARE_SPELL_FUN(      spell_deny_earth        ); // Veygoth
DECLARE_SPELL_FUN(      spell_deny_fire         ); // Veygoth
DECLARE_SPELL_FUN(      spell_deny_water        ); // Veygoth
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_undead	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(      spell_dexterity         ); // Veygoth
DECLARE_SPELL_FUN(      spell_dimdoor           ); // Veygoth
DECLARE_SPELL_FUN(	spell_dirt_cloud	); // Xueqin
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(      spell_dispel_invis      ); // Veygoth
DECLARE_SPELL_FUN(	spell_dispel_magic	);	/* by Thelonius */
DECLARE_SPELL_FUN(      spell_dismissal         ); // Lithos
DECLARE_SPELL_FUN(      spell_dust_blast        ); // Xueqin
DECLARE_SPELL_FUN(      spell_earthen_grasp     ); // Veygoth
DECLARE_SPELL_FUN(      spell_earthen_rain      ); // Veygoth
DECLARE_SPELL_FUN(	spell_earthen_smith	); // Xueqin
DECLARE_SPELL_FUN(      spell_earthen_starshell ); // Veygoth
DECLARE_SPELL_FUN(      spell_earthen_tomb      ); 
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(      spell_elemental_sight   ); // Lithos
DECLARE_SPELL_FUN(      spell_elemental_form    );
DECLARE_SPELL_FUN(      spell_elephantstrength  ); // Veygoth
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(      spell_enlarge           ); // Veygoth
DECLARE_SPELL_FUN(      spell_entangle          ); // Veygoth
DECLARE_SPELL_FUN(      spell_enthrall          ); // Veygoth
DECLARE_SPELL_FUN(      spell_etherportal	);
DECLARE_SPELL_FUN(	spell_exorcise  	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(      spell_farsee            ); // Veygoth
DECLARE_SPELL_FUN(      spell_fear              ); // Veygoth
DECLARE_SPELL_FUN(      spell_feeblemind        ); // Veygoth
DECLARE_SPELL_FUN(      spell_fiery_starshell   ); // Veygoth
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_ball_lightning	);
DECLARE_SPELL_FUN(      spell_fireskin          ); // Veygoth
DECLARE_SPELL_FUN(	spell_firestorm		); // Veygoth
DECLARE_SPELL_FUN(	spell_fire_bolt		); // Xueqin
DECLARE_SPELL_FUN(      spell_fire_ward         ); // Veygoth
DECLARE_SPELL_FUN(	spell_firey_smith	); // Xueqin
DECLARE_SPELL_FUN(      spell_flameburst        ); // Veygoth
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_flaming           );
DECLARE_SPELL_FUN(	spell_flashfire		); // Xueqin
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(      spell_fullharm          ); // Veygoth
DECLARE_SPELL_FUN(      spell_fullheal          ); // Veygoth
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(      spell_gr_earthen_grasp  ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_pythonsting    ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_ravenflight    ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_soul_dist      ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_spirit_sight   ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_spirit_ward    ); // Veygoth
DECLARE_SPELL_FUN(      spell_gr_sustenance     ); // Veygoth
DECLARE_SPELL_FUN(      spell_greater_mending   ); // Veygoth
DECLARE_SPELL_FUN(      spell_group_heal        ); // Ski
DECLARE_SPELL_FUN(      spell_group_globe       ); // Veygoth
DECLARE_SPELL_FUN(      spell_group_recall      ); // Veygoth
DECLARE_SPELL_FUN(	spell_group_stone	); // Xueqin
DECLARE_SPELL_FUN(	spell_gust_of_frost	); // Xueqin
DECLARE_SPELL_FUN(      spell_harbor_balance    ); // Veygoth
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(      spell_haste             ); // Veygoth
DECLARE_SPELL_FUN(      spell_hawkvision        ); // Veygoth
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(      spell_hypnotic_pattern  ); // Veygoth
DECLARE_SPELL_FUN(	spell_resurrect		);
DECLARE_SPELL_FUN(	spell_heal_undead	); // Govno
DECLARE_SPELL_FUN(      spell_hex               ); // Veygoth
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(      spell_holy_sacrifice    ); // Lithos
DECLARE_SPELL_FUN(      spell_holy_word         ); // Veygoth
DECLARE_SPELL_FUN(	spell_hover		); // Xueqin
DECLARE_SPELL_FUN(	spell_hurricane		); // Xueqin
DECLARE_SPELL_FUN(      spell_iceball           ); // Veygoth
DECLARE_SPELL_FUN(      spell_ice_bolt          ); // Veygoth
DECLARE_SPELL_FUN(      spell_ice_storm         ); // Veygoth
DECLARE_SPELL_FUN(      spell_ice_missile       ); // Veygoth
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(      spell_illumination      ); // Veygoth
DECLARE_SPELL_FUN(      spell_illusion_incompetence ); // Veygoth
DECLARE_SPELL_FUN(      spell_illusion_prowess  ); // Veygoth
DECLARE_SPELL_FUN(      spell_illusionary_wall  ); // Veygoth
DECLARE_SPELL_FUN(      spell_immolate          ); // Veygoth
DECLARE_SPELL_FUN(      spell_incendiary_cloud  ); // Veygoth
DECLARE_SPELL_FUN(      spell_inferno           ); // Veygoth
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(      spell_judgement         ); // Veygoth
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(      spell_lesser_mending    ); // Veygoth
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(     	spell_lightanddark	);     // Saeven**
DECLARE_SPELL_FUN(     	spell_tridenttides	);     // Saeven**  
DECLARE_SPELL_FUN(      spell_lightning_curtain ); // Veygoth
DECLARE_SPELL_FUN(      spell_lionrage          ); // Veygoth
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(      spell_ls_nourishment    ); // Veygoth
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(      spell_magnetism         ); // Veygoth
DECLARE_SPELL_FUN(      spell_major_globe       ); // Veygoth
DECLARE_SPELL_FUN(      spell_malison           ); // Veygoth
DECLARE_SPELL_FUN(  	spell_mass_dispel_magic ); // Xueqin
DECLARE_SPELL_FUN(	spell_mass_heal 	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(      spell_mending           ); // Veygoth
DECLARE_SPELL_FUN(      spell_mermaids_kiss     ); // Lohrr
DECLARE_SPELL_FUN(      spell_miners_intuition  ); // Veygoth
DECLARE_SPELL_FUN(	spell_minor_blending	); // Xueqin
DECLARE_SPELL_FUN(      spell_minor_creation    ); // Veygoth
DECLARE_SPELL_FUN(      spell_minor_globe       ); // Veygoth
DECLARE_SPELL_FUN(      spell_minor_para        ); // Veygoth
DECLARE_SPELL_FUN(      spell_mirage_arcana     ); // Veygoth
DECLARE_SPELL_FUN(      spell_mirror_image      ); // Veygoth
DECLARE_SPELL_FUN(      spell_misdirection      ); // Veygoth
DECLARE_SPELL_FUN(      spell_molevision        ); // Veygoth
DECLARE_SPELL_FUN(      spell_molten_spray      ); // Veygoth
DECLARE_SPELL_FUN(      spell_moonwell          ); // Lithos
DECLARE_SPELL_FUN(      spell_mousestrength     ); // Veygoth
DECLARE_SPELL_FUN(	spell_silence		); /* by Thelonius */
DECLARE_SPELL_FUN(      spell_negate_hex        ); // Veygoth
DECLARE_SPELL_FUN(      spell_negate_luster     ); // Veygoth
DECLARE_SPELL_FUN(      spell_negate_veil       ); // Veygoth
DECLARE_SPELL_FUN(      spell_neurofrag         ); // Veygoth
DECLARE_SPELL_FUN(      spell_nightmares        ); // Veygoth
DECLARE_SPELL_FUN(      spell_pantherspeed      ); // Veygoth
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_pebble		); // Xueqin
DECLARE_SPELL_FUN(      spell_phantasmal_killer ); // Veygoth
DECLARE_SPELL_FUN(      spell_plane_shift       ); // Veygoth
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_polymorph_other	);
DECLARE_SPELL_FUN(      spell_power_word_blind  ); // Veygoth
DECLARE_SPELL_FUN(      spell_power_word_lag    ); // Veygoth
DECLARE_SPELL_FUN(      spell_power_word_stun   ); // Veygoth
DECLARE_SPELL_FUN(      spell_preserve          ); // Veygoth
DECLARE_SPELL_FUN(      spell_prismatic_orb     ); // Veygoth
DECLARE_SPELL_FUN(      spell_prismatic_spray   ); // Veygoth
DECLARE_SPELL_FUN(	spell_protection_acid	); // Veygoth
DECLARE_SPELL_FUN(	spell_protection_cold	); // Veygoth
DECLARE_SPELL_FUN(	spell_protection_evil	);
DECLARE_SPELL_FUN(	spell_protection_fire	); // Veygoth
DECLARE_SPELL_FUN(	spell_protection_gas	); // Veygoth
DECLARE_SPELL_FUN(	spell_protection_lightning);
DECLARE_SPELL_FUN(	spell_protect_undead	); // Govno
DECLARE_SPELL_FUN(      spell_purify            ); // Lithos
DECLARE_SPELL_FUN(      spell_purify_spirit     ); // Veygoth
DECLARE_SPELL_FUN(      spell_pythonsting       ); // Veygoth
DECLARE_SPELL_FUN(      spell_rain_maker        ); // Veygoth
DECLARE_SPELL_FUN(      spell_ravenflight       ); // Veygoth
DECLARE_SPELL_FUN(      spell_ravenous_vines    ); // Veygoth
DECLARE_SPELL_FUN(	spell_recharge_item	);	/* by Thelonius */
DECLARE_SPELL_FUN(      spell_reduce            ); // Veygoth
DECLARE_SPELL_FUN(      spell_rejuvenate        ); // Veygoth
DECLARE_SPELL_FUN(	spell_remove_alignment	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_remove_silence	);	/* by Thelonius */
DECLARE_SPELL_FUN(      spell_reveal_spirit_essence); // Veygoth
DECLARE_SPELL_FUN(      spell_reveal_true_form  ); // Veygoth
DECLARE_SPELL_FUN(      spell_reveal_true_name  ); // Veygoth
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(      spell_scalding_blast    ); // Veygoth
DECLARE_SPELL_FUN(      spell_scathing_wind     ); // Veygoth
DECLARE_SPELL_FUN(      spell_scorching_touch   ); // Veygoth
DECLARE_SPELL_FUN(	spell_sense_life	); // Veygoth
DECLARE_SPELL_FUN(      spell_shades            ); // Veygoth
DECLARE_SPELL_FUN(      spell_shadow_magic      ); // Veygoth
DECLARE_SPELL_FUN(      spell_shadow_monsters   ); // Veygoth
DECLARE_SPELL_FUN(      spell_shadow_shield     ); // Veygoth
DECLARE_SPELL_FUN(      spell_shadow_veil       ); // Veygoth
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(      spell_shrewtameness     ); // Veygoth
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(      spell_slow_poison       ); // Veygoth
DECLARE_SPELL_FUN(      spell_slowness          ); // Veygoth
DECLARE_SPELL_FUN(      spell_snailspeed        ); // Veygoth
DECLARE_SPELL_FUN(      spell_soul_dist         ); // Veygoth
DECLARE_SPELL_FUN(      spell_soulshield        ); // Veygoth
DECLARE_SPELL_FUN(	spell_spark		); // Xueqin
DECLARE_SPELL_FUN(      spell_spirit_anguish    ); // Veygoth
DECLARE_SPELL_FUN(      spell_spirit_armor      ); // Veygoth
DECLARE_SPELL_FUN(      spell_spirit_jump       ); // Veygoth
DECLARE_SPELL_FUN(      spell_spirit_sight      ); // Veygoth
DECLARE_SPELL_FUN(      spell_spirit_ward       ); // Veygoth
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(      spell_stonestorm        ); // Veygoth
DECLARE_SPELL_FUN(  	spell_stonestrength 	); // Xueqin
DECLARE_SPELL_FUN(	spell_stornogs_spheres	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_summon_elemental	); // Lithos
DECLARE_SPELL_FUN(      spell_summon_serpent    ); // Lithos
DECLARE_SPELL_FUN(      spell_summon_nerneid    ); // Lithos
DECLARE_SPELL_FUN(      spell_summon_pech       ); // Lithos
DECLARE_SPELL_FUN(      spell_summon_slyph      ); // Lithos
DECLARE_SPELL_FUN(      spell_summon_gr_elem    ); // Lithos
DECLARE_SPELL_FUN(      spell_sunburst          ); // Veygoth
DECLARE_SPELL_FUN(      spell_sunray            ); // Veygoth
DECLARE_SPELL_FUN(      spell_sustenance        ); // Veygoth
DECLARE_SPELL_FUN(	spell_telekinesis	);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(      spell_thirst            ); // Lohrr
DECLARE_SPELL_FUN(      spell_tidal_wave        ); // Veygoth
DECLARE_SPELL_FUN(	spell_tide_seas		); // Xueqin
DECLARE_SPELL_FUN(      spell_touch_nightshade  ); // Veygoth
DECLARE_SPELL_FUN(      spell_transfer_wellness ); // Veygoth
DECLARE_SPELL_FUN(      spell_true_seeing       ); // Veygoth
DECLARE_SPELL_FUN(	spell_turn_undead	);
DECLARE_SPELL_FUN(      spell_twilight          ); // Veygoth
DECLARE_SPELL_FUN(	spell_ultra_death_ray	); // Ski
DECLARE_SPELL_FUN(      spell_unholy_word       ); // Veygoth
DECLARE_SPELL_FUN(      spell_vacancy           ); // Veygoth
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_vigorize_critical	); // Veygoth
DECLARE_SPELL_FUN(	spell_vigorize_light	); // Veygoth
DECLARE_SPELL_FUN(	spell_vigorize_serious	); // Veygoth
DECLARE_SPELL_FUN(      spell_vitality          ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_fire      ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_force     ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_ice       ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_iron      ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_mist      ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_sparks    ); // Veygoth
DECLARE_SPELL_FUN(      spell_wall_of_stone     ); // Veygoth
DECLARE_SPELL_FUN(      spell_water_blast       ); // Xueqin
DECLARE_SPELL_FUN(	spell_water_breathing	);	/* by Thelonius */
DECLARE_SPELL_FUN(      spell_water_bolt        ); // Xueqin
DECLARE_SPELL_FUN(      spell_watery_starshell  ); // Veygoth
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(      spell_wellness          ); // Veygoth
DECLARE_SPELL_FUN(      spell_whirlwind         ); // Veygoth
DECLARE_SPELL_FUN(      spell_wierd             ); // Veygoth
DECLARE_SPELL_FUN(      spell_windsaber         ); // Veygoth
DECLARE_SPELL_FUN(      spell_wither            ); // Veygoth
DECLARE_SPELL_FUN(      spell_wolfspeed         ); // Veygoth
DECLARE_SPELL_FUN(	spell_word_of_recall	);
/*
 * Mobile spell functions
 */
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_water_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(	spell_vampiric_bite	);
/* God toy.  Lohrr */
DECLARE_DO_FUN(	do_chill		);
/* Add/Reset languages for characters. */
DECLARE_DO_FUN(	do_add_lang		);
/*
 * Psi spell_functions, in spells.c (by Thelonius).
 */
DECLARE_SPELL_FUN(      spell_adrenaline_control);
DECLARE_SPELL_FUN(      spell_agitation         );
DECLARE_SPELL_FUN(      spell_aura_sight        );
DECLARE_SPELL_FUN(      spell_awe               );
DECLARE_SPELL_FUN(      spell_ballistic_attack  );
DECLARE_SPELL_FUN(      spell_biofeedback       );
DECLARE_SPELL_FUN(      spell_cell_adjustment   );
DECLARE_SPELL_FUN(      spell_combat_mind       );
DECLARE_SPELL_FUN(      spell_complete_healing  );
DECLARE_SPELL_FUN(      spell_control_flames    );
DECLARE_SPELL_FUN(      spell_create_sound      );
DECLARE_SPELL_FUN(      spell_death_field       );
DECLARE_SPELL_FUN(      spell_detonate          );
DECLARE_SPELL_FUN(      spell_disintegrate      );
DECLARE_SPELL_FUN(      spell_displacement      );
DECLARE_SPELL_FUN(      spell_domination        );
DECLARE_SPELL_FUN(      spell_ectoplasmic_form  );
DECLARE_SPELL_FUN(      spell_ego_whip          );
DECLARE_SPELL_FUN(      spell_energy_containment);
DECLARE_SPELL_FUN(      spell_enhance_armor     );
DECLARE_SPELL_FUN(      spell_enhanced_strength );
DECLARE_SPELL_FUN(      spell_flesh_armor       );
DECLARE_SPELL_FUN(      spell_inertial_barrier  );
DECLARE_SPELL_FUN(      spell_inflict_pain      );
DECLARE_SPELL_FUN(      spell_intellect_fortress);
DECLARE_SPELL_FUN(      spell_lend_health       );
DECLARE_SPELL_FUN(      spell_levitation        );
DECLARE_SPELL_FUN(      spell_mental_barrier    );
DECLARE_SPELL_FUN(      spell_mind_thrust       );
DECLARE_SPELL_FUN(      spell_project_force     );
DECLARE_SPELL_FUN(      spell_psionic_blast     );
DECLARE_SPELL_FUN(      spell_psychic_crush     );
DECLARE_SPELL_FUN(      spell_psychic_drain     );
DECLARE_SPELL_FUN(      spell_psychic_healing   );
DECLARE_SPELL_FUN(      spell_share_strength    );
DECLARE_SPELL_FUN(      spell_thought_shield    );
DECLARE_SPELL_FUN(      spell_ultrablast        );
/*
 * New spell functions, in spells.c (by Zen).
 */
DECLARE_SPELL_FUN(	spell_detect_good	);
DECLARE_SPELL_FUN(	spell_dispel_good	);
DECLARE_SPELL_FUN(      spell_meteor_swarm	);
DECLARE_SPELL_FUN(      spell_chain_lightning	);
DECLARE_SPELL_FUN(      spell_vortex_lift	);
DECLARE_SPELL_FUN(      spell_wizard_eye	);
DECLARE_SPELL_FUN(      spell_mass_vortex_lift	);
DECLARE_SPELL_FUN(      spell_nexus		);
DECLARE_SPELL_FUN(	spell_protection_good	);
DECLARE_SPELL_FUN(	spell_create_buffet	);
DECLARE_SPELL_FUN(	spell_cure_disease	);
DECLARE_SPELL_FUN(	spell_plague		);
DECLARE_SPELL_FUN(	spell_flame_shield	);
DECLARE_SPELL_FUN(	spell_frost_shield	);
DECLARE_SPELL_FUN(	spell_shock_shield	);
DECLARE_SPELL_FUN(	spell_ethereal_shield	);
/*
 * Traps
 */
DECLARE_SPELL_FUN(      trap_acid               );
DECLARE_SPELL_FUN(      trap_bash               );
DECLARE_SPELL_FUN(      trap_cold               );
DECLARE_SPELL_FUN(      trap_disease            );
DECLARE_SPELL_FUN(      trap_dispel             );
DECLARE_SPELL_FUN(      trap_energy             );
DECLARE_SPELL_FUN(      trap_fire               );
DECLARE_SPELL_FUN(      trap_gate               );
DECLARE_SPELL_FUN(      trap_harm               );
DECLARE_SPELL_FUN(      trap_para               );
DECLARE_SPELL_FUN(      trap_pierce             );
DECLARE_SPELL_FUN(      trap_poison             );
DECLARE_SPELL_FUN(      trap_slash              );
DECLARE_SPELL_FUN(      trap_sleep              );
DECLARE_SPELL_FUN(      trap_stun               );
DECLARE_SPELL_FUN(      trap_summon             );
DECLARE_SPELL_FUN(      trap_teleport           );
DECLARE_SPELL_FUN(      trap_wither             );
/*
 * Bard Songs
 */
DECLARE_SPELL_FUN(      song_armor              ); // Veygoth
DECLARE_SPELL_FUN(      song_babble             ); // Veygoth
DECLARE_SPELL_FUN(      song_calming            ); // Veygoth
DECLARE_SPELL_FUN(      song_chaos              ); // Veygoth
DECLARE_SPELL_FUN(      song_clumsiness         ); // Veygoth
DECLARE_SPELL_FUN(      song_corruption         ); // Veygoth
DECLARE_SPELL_FUN(      song_cowardice          ); // Veygoth
DECLARE_SPELL_FUN(      song_dragons            ); // Veygoth
DECLARE_SPELL_FUN(      song_feasting           ); // Veygoth
DECLARE_SPELL_FUN(      song_flight             ); // Veygoth
DECLARE_SPELL_FUN(      song_forgetfulness      ); // Veygoth
DECLARE_SPELL_FUN(      song_harming            ); // Veygoth
DECLARE_SPELL_FUN(      song_healing            ); // Veygoth
DECLARE_SPELL_FUN(      song_heroism            ); // Veygoth
DECLARE_SPELL_FUN(      song_invisibility       ); // Veygoth
DECLARE_SPELL_FUN(      song_idiocy             ); // Veygoth
DECLARE_SPELL_FUN(      song_nightmares         ); // Veygoth
DECLARE_SPELL_FUN(      song_obscurement        ); // Veygoth
DECLARE_SPELL_FUN(      song_purity             ); // Veygoth
DECLARE_SPELL_FUN(      song_quagmire           ); // Veygoth
DECLARE_SPELL_FUN(      song_revelation         ); // Veygoth
DECLARE_SPELL_FUN(      song_skylark            ); // Veygoth
DECLARE_SPELL_FUN(      song_sleep              ); // Veygoth
DECLARE_SPELL_FUN(      song_slowness           ); // Veygoth
DECLARE_SPELL_FUN(      song_susceptibility     ); // Veygoth
DECLARE_SPELL_FUN(      song_warding            ); // Veygoth
DECLARE_SPELL_FUN(      song_weakling           ); // Veygoth

/*
 * Weapon functions.
 * Defined in fight.c.
 */
DECLARE_HIT_FUN(        hit_vorpal              );
DECLARE_HIT_FUN(        hit_suck_disarm         );
bool check_shrug ( CHAR_DATA *ch, CHAR_DATA *victim );

/*
 * Prog functions used in prog_comm.c -- Veygoth
 */
DECLARE_DO_FUN( do_prgasound     );
DECLARE_DO_FUN( do_prgat         );
DECLARE_DO_FUN( do_prgecho       );
DECLARE_DO_FUN( do_prgechoaround );
DECLARE_DO_FUN( do_prgechoat     );
DECLARE_DO_FUN( do_prgforce      );
DECLARE_DO_FUN( do_prggoto       );
DECLARE_DO_FUN( do_prgjunk       );
DECLARE_DO_FUN( do_prgkill       );
DECLARE_DO_FUN( do_prgmload      );
DECLARE_DO_FUN( do_prgoload      );
DECLARE_DO_FUN( do_prgpurge      );
DECLARE_DO_FUN( do_prgtransfer   );
bool prog_move_script( CHAR_DATA *ch );

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined( _AIX )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( apollo )
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( hpux )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( linux )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( macintosh ) || defined( WIN32 )
#define NOCRYPT
#if	defined( unix )
#undef	unix
#endif
#endif

#if	defined( MIPS_OS )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( NeXT )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( sequent )
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined( sun )
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
size_t	fread		args( ( void *ptr, size_t size, size_t nitems,
			       FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined( ultrix )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

/*
 * Stuff for DEC UNIX on Alpha (OSF3.2C)
 * Fusion
 */
#if defined( _OSF_SOURCE )
char *	crypt           args( ( const char *key, const char *salt ) );
int     system          args( ( const char *string ) );
ssize_t read            args( ( int fd, void *buf, size_t nbyte ) );
ssize_t write           args( ( int fd, const void *buf, size_t nbyte ) );
int     close           args( ( int fd ) );
#endif


/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined( NOCRYPT )
#define crypt( s1, s2 )	( s1 )
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined( unix ) || defined( linux )
#define PLAYER_DIR	"../share/magma/player/"	/* Player files			*/
#define BACKUP_DIR	"../share/magma/backup/"	/* Backup player files		*/
#define SYSTEM_DIR	"../sys/"			/* System directory		*/
#define CLASS_DIR	"../share/magma/classes/"	/* New class loading scheme	*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#define AREA_DIR	"../share/magma/area/"	/* Area files			*/
#define CLAN_DIR	"../share/magma/clans/"	/* Clan files              	*/
#define MOB_DIR		"../share/magma/mobprogs/"	/* MOBProg files		*/
#define OBJ_DIR         "../share/magma/objprogs/"  /* OBJProg files                */
#endif

#if defined( WIN32 )
#define PLAYER_DIR	"..\\player\\"	/* Player files			*/
#define BACKUP_DIR	"..\\backup\\"	/* Backup player files		*/
#define SYSTEM_DIR	"..\\sys\\"	/* System directory		*/
#define CLASS_DIR	"..\\classes\\"	/* New class loading scheme	*/
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#define AREA_DIR	"..\\area\\"	/* Area files			*/
#define CLAN_DIR	"..\\clans\\"	/* Clan files              	*/
#define MOB_DIR		"..\\mobprogs\\"/* MOBProg files		*/
#define OBJ_DIR         "..\\objprogs\\"/* OBJProg files                */
#endif

#define AREA_LIST	 "AREA.LST"	/* List of areas		*/

#define CLASS_LIST	 "CLASS.LST"	/* List of classes		*/
#define CLANS_LIST	 "CLANS.LST"	/* List of clans		*/
#define NAME_LIST        "NAMES.LST"    /* List of illegal names        */
#define BUG_FILE	 "BUGS.TXT"	/* For 'bug' and bug( )		*/
#define CRASHBUG_FILE	 "CRASHBUGS.TXT" /* for 'crashbug'   Govno      */
#define IDEA_FILE	 "IDEAS.TXT"	/* For 'idea'			*/
#define TYPO_FILE	 "TYPOS.TXT"	/* For 'typo'			*/
#define NOTE_FILE	 "NOTES.TXT"	/* For 'notes'			*/
#define CLAN_FILE	 "CLANS.TXT"	/* For clan code		*/
#define SHUTDOWN_FILE	 "SHUTDOWN.TXT"	/* For 'shutdown'		*/
#define DOWN_TIME_FILE   "TIME.TXT"	/* For automatic shutdown       */
#define BAN_FILE         "BAN.TXT"	/* For banned site save         */
#define SOCIAL_FILE	 "SOCIALS.TXT"	/* For socials			*/
#define SYSDATA_FILE	 "SYSDATA.TXT"	/* For mud configuration	*/
#define ANSI_TITLE_FILE  "MUDTITLE.ANS"	/* For ansi mud title screen    */
#define ASCII_TITLE_FILE "MUDTITLE.ASC"	/* For ascii mud title screen   */
#define CRIME_FILE       "CRIMES.DAT"   /* For crimes/justice info      */
#define CORPSE_FILE      "CORPSES.DAT"  /* For saving of player corpses */
#define FRAG_FILE        "FRAGLIST.DAT" /* Frag stuff                   */
#define HEARTBEAT_FILE   "HEARTBEAT"    /* Heartbeat file Govno         */

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define AD	AFFECT_DATA
#define ED	EXTRA_DESCR_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define MF	MOB_FUN
#define OF	OBJ_FUN
#define SID	SOC_INDEX_DATA
#define GF      GAME_FUN
#define CLD     CLAN_DATA
#define QID     QUEST_INDEX_DATA
#define QD      QUEST_DATA
#define QI      QUEST_ITEM
#define TD      TALK_DATA
#define MD      MEM_DATA
#define EVD     EVENT_DATA
#define VD      VEHICLE_DATA
#define CRD     CRIME_DATA

/* act_comm.c */
void	add_follower		args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	add_follower_nomsg  	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower		args( ( CHAR_DATA *ch ) );
void	die_follower		args( ( CHAR_DATA *ch, char *name ) );
char *	race_name		args( ( CHAR_DATA *ch ) );
bool	is_vowel		args( ( char * arg ) );
bool	is_same_group		args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	remove_from_group	args( ( CHAR_DATA *ch, CHAR_DATA *member ) );
bool	is_note_to		args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
bool	is_same_clan		args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	send_ansi_title 	args( ( DESCRIPTOR_DATA *d ) );
void	send_ascii_title	args( ( DESCRIPTOR_DATA *d ) );
void    add_to_board            args( ( CHAR_DATA *ch, NOTE_DATA *note, NOTE_DATA *list ) );
void    delete_from_board       args( ( CHAR_DATA *ch, NOTE_DATA *note, NOTE_DATA *list ) );
void    look_at_board           args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );

/* act_info.c */
bool    is_same_room    args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
bool	check_blind	args( ( CHAR_DATA *ch ) );
void	show_room_info	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
void	show_room_affects args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
char    *get_sex        args( ( CHAR_DATA *ch ) );
char    *get_exp_string args( ( CHAR_DATA *ch ) );
char    *stat_string    args( ( int value ) );
char    *bonus_string   args( ( int value ) );
char    *weight_string  args( ( CHAR_DATA *ch ) );
int	how_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
char    *skill_string   args( ( int value ) );
int     int_lang        args( ( CHAR_DATA *ch, char *lang ) );
bool    has_skill       args( ( CHAR_DATA *ch, int sn ) );
bool    remove_blindness args(( CHAR_DATA *ch ) );
bool    remove_poison   args( ( CHAR_DATA *ch ) );
void    remove_affects  args( ( CHAR_DATA *ch, BITVECT_DATA bvect ) );
bool    can_move        args( ( CHAR_DATA *ch ) );
bool    has_free_will   args( ( CHAR_DATA *ch ) );
int     get_race        args( ( CHAR_DATA *ch ) );
void    update_innate_timers args( ( CHAR_DATA *ch ) );
void    purge_innate_timers args( ( CHAR_DATA *ch ) );
void    add_innate_timer args ( ( CHAR_DATA *ch, int type, int timer ) );
void    remove_innate_timer args ( ( CHAR_DATA *ch , INNATE_TIMER_DATA *itd ) );
bool    has_innate_timer args ( ( CHAR_DATA *ch , int type ) );
void    reset_stats      args( ( CHAR_DATA *ch ) );


/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );
int     find_door       args( ( CHAR_DATA *ch, char *arg ) );
int     find_exit       args( ( CHAR_DATA *ch, char *arg ) );
RID*    find_room       args( ( CHAR_DATA *ch, char *arg ) );
void    check_fall      args( ( ROOM_INDEX_DATA *pRoomIndex, ROOM_INDEX_DATA *target, CHAR_DATA *ch ) );
MOVE_SCRIPT *create_move_script args ( ( void ) );

/* act_obj.c */
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
int     get_cash        args( ( CHAR_DATA *ch ) );
void    spend_cash      args( ( CHAR_DATA *ch, int amount ) );
void    receive_cash    args( ( CHAR_DATA *ch, int amount ) );
void    set_cost        args( ( OBJ_INDEX_DATA *obj ) );
char    *coin_string    args( ( int cost ) );
bool    check_quest     args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int money ) );
bool    insult_artifact args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int     get_cost        args( ( CHAR_DATA *customer, CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );

/* act_wiz.c */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );
char *                  random_sentence args( ( void ) ) ;

/* act_clan.c */
bool			is_clan		args( ( CHAR_DATA *ch ) );

/* comm.c */
void    add_languages    args( ( CHAR_DATA *ch ) );
void	close_socket	 args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	 args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
char *  condition_string args( ( CHAR_DATA *ch ) );
char *  condition_meter  args( ( CHAR_DATA *ch ) );
char *  condition_string2 args( ( CHAR_DATA *ch ) );
char *  condition_meter2  args( ( CHAR_DATA *ch ) );
void    send_to_room     args( ( const char *txt, ROOM_INDEX_DATA *room ) );
void    send_to_all_char args( ( const char *text ) );
void	send_to_char	 args( ( const char *txt, CHAR_DATA *ch ) );
void	printf_to_char	 args( ( CHAR_DATA *ch, char *fmt, ... ) );
void    show_string      args( ( DESCRIPTOR_DATA *d, char *input ) );
void	act	         args( ( const char *format, CHAR_DATA *ch,
				const void *arg1, const void *arg2,
				int type ) );
char *  text_number      args( ( int number ) );
void    newbie_eq_char   args( ( CHAR_DATA *ch ) );
bool	check_parse_name args( ( char *name ) );

/*
 * Color stuff by Lope of Loping Through The MUD
 */
int	colour		args( ( const char *type, CHAR_DATA *ch, char *string ) );
void	colourconv	args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void	send_to_char_bw	args( ( const char *txt, CHAR_DATA *ch ) );

/* db.c */
void    make_arti_owner args( ( char * const owner, const int vnum ) );
bool    is_artifact     args( ( int vnum ) );
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	new_character	args( ( bool player ) );
AD *	new_affect	args( ( void ) );
OD *	new_object	args( ( void ) );
ED *	new_extra_descr args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
void	clone_mobile	args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clone_object	args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
QID *   create_quest    args( ( void ) );
QD  *   create_quest_data       args( ( void ) );
TD  *   create_quest_message    args( ( void ) );
QI  *   create_quest_item       args( ( void ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp, int *status ) );
char *	fread_string	args( ( FILE *fp, int *status ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp, int *status ) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
void	free_string	args( ( char *pstr ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int     number_race     args( ( int type ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
char *  all_capitalize  args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void    warning args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );

void	bugf		args( ( char *fmt, ... ) );
void	logff		args( ( char *fmt, ... ) );

void	log_clan	args( ( const char *str ) );
void	temp_fread_string	args( ( FILE *fp, char *str ) );
void	save_sysdata	args( ( void ) );
int     class_convert   args( ( int class ) );

/* event.c */
EVD *   create_event    	args( ( int type, int time, void *arg1,
                                void *arg2, int var ) ); 
void    delete_attached_events	args( ( CHAR_DATA *ch ) );
void    event_update    	args( ( void ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt, int wpn, int dam_type ) );
void	spell_damage    args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt, int dam_type ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    death_cry	args( ( CHAR_DATA *ch ) );
void    stop_med	args( ( CHAR_DATA *ch ) ); 
void    stop_mem	args( ( CHAR_DATA *ch ) ); 
void    make_vis	args( ( CHAR_DATA *ch ) ); 
void    stop_hunting    args( ( CHAR_DATA *ch ) );
void    stop_hating     args( ( CHAR_DATA *ch ) );
void    stop_fearing    args( ( CHAR_DATA *ch ) );
bool    check_aggressive args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_hunting   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_hating    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_fearing   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_grudge    args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool ranged ) );
bool    is_hunting      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_hating       args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_fearing      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int     check_trophy    args( ( CHAR_DATA *ch, CHAR_DATA *victim, int members ) );
CD *    check_guarding  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    backstab        args( ( CHAR_DATA *ch, CHAR_DATA *victim ));
void    kill_ch         args( ( CHAR_DATA *ch, CHAR_DATA *victim ));
bool    check_riposte   args( ( CHAR_DATA *ch, CHAR_DATA *victim ));
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ));
bool    check_vicious   args( ( CHAR_DATA *ch, CHAR_DATA *victim ));
bool    check_tumble    args( ( CHAR_DATA *ch ) );

/* handler.c */
void last_fun args( ( const char *name    ) );
void last_com args( ( const char *name ) );
void    print_last_funs args( ( void ) );
void    print_last_coms args( ( void ) );
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int     get_curr_agi    args( ( CHAR_DATA *ch ) );
int     get_curr_cha    args( ( CHAR_DATA *ch ) );
int     get_curr_pow    args( ( CHAR_DATA *ch ) );
int     get_curr_luk    args( ( CHAR_DATA *ch ) );
int     get_curr_race   args( ( CHAR_DATA *ch ) );
int     get_orig_race   args( ( CHAR_DATA *ch ) );
int	get_max_str	args( ( CHAR_DATA *ch ) );
int	get_max_int	args( ( CHAR_DATA *ch ) );
int	get_max_wis	args( ( CHAR_DATA *ch ) );
int	get_max_dex	args( ( CHAR_DATA *ch ) );
int	get_max_con	args( ( CHAR_DATA *ch ) );
int     get_max_agi     args( ( CHAR_DATA *ch ) );
int     get_max_cha     args( ( CHAR_DATA *ch ) );
int     get_max_pow     args( ( CHAR_DATA *ch ) );
int     get_max_luk     args( ( CHAR_DATA *ch ) );
int     get_max_hit     args( ( CHAR_DATA *ch ) );
int     get_hitroll     args( ( CHAR_DATA *ch, int wpn ) );
int     get_damroll     args( ( CHAR_DATA *ch, int wpn ) );
char    *get_invader_string args( ( int justice ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
bool	is_name_prefix	args( ( const char *str, char *namelist ) );
AD      create_affect   args( ( int skill, int spell, int song, int duration, int location,
                                int modifier, BITVECT_DATA bitvector ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_to_obj   args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int skl, int spl, int song ) );
void    apply_poison    args( ( CHAR_DATA *ch ) );
bool    is_affected     args( ( CHAR_DATA *ch, BITVECT_DATA bitvector ) );
bool	has_affect      args( ( CHAR_DATA *ch, int skl, int spl, int song ) );
bool    has_affect_obj  args( ( OBJ_DATA *obj, int skl, int spl, int song ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void  affect_remove_obj args (( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_strip_obj args( ( OBJ_DATA *obj, int skl, int spl, int song ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void    invoke          args( ( CHAR_DATA *ch, OBJ_DATA *obj ) ); 
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_at     args( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch, char *argument ) );
CD *	get_char_area	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			       OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_room    args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int copper, int silver, int gold, int platinum ) );
RID *   get_random_room args( ( ) );
RID *   get_random_map_room args( ( int map ) );
bool    fill_coin_data  args( ( COIN_DATA *dest, char *argument, COIN_DATA *src ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
int     get_sector      args( ( ROOM_INDEX_DATA *room ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_fly_room     args( ( ROOM_INDEX_DATA *rm ) );
bool    is_water_room   args( ( ROOM_INDEX_DATA *room ) );
bool    is_midair_room  args( ( ROOM_INDEX_DATA *room ) );
bool    can_fly         args( ( CHAR_DATA *ch ) );
bool    is_aggro        args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	how_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	parts_bit_name	args( ( int vector ) );
char *	ris_bit_name	args( ( int vector ) );
char *  wall_decay_msg  args( ( int vnum ) );
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized      args( ( CHAR_DATA *ch, char *skllnm ) );
void    end_of_game     args( ( void ) );
int	size_lookup	args( ( const char *size ) ); /* Govno get size by name*/
int     race_lookup     args( ( const char *race ) );
int     race_key_lookup args( ( const char *race ) );
int     skill_affect_lookup   args( ( const char *race ) );
int     spell_affect_lookup   args( ( const char *race ) );
int     song_affect_lookup   args( ( const char *race ) );
int	advatoi		args( ( const char *s ) );
CLD  *	clan_lookup	args( ( const char *name ) );
int	race_full_lookup args( ( const char *race ) );
int	check_ris	args( ( CHAR_DATA *ch, int dam_type ) );
int     get_ac          args( ( CHAR_DATA *ch ) );
bool    is_underground  args( ( CHAR_DATA *ch ) );
CD *    create_char_from_obj args( ( OBJ_DATA *obj ) );

/* interp.c */
void    substitute_alias args( ( DESCRIPTOR_DATA *d ) );
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );
bool    check_command   args( ( char *argument, int number ));

/* justice.c */
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_thief	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    check_invader   args( ( CHAR_DATA *ch ) );
CRD *   create_crime args(( CHAR_DATA *ch, CHAR_DATA *victim, int type ));
void    save_crimes     args( ( void ) );
void    load_crimes     args( ( void ) );
void    start_invasion  args( (CHAR_DATA *ch ) );

/* magic.c */
void do_area_spell_damage(CHAR_DATA *, int, int, int, int);
void affect_refresh (CHAR_DATA *, int, int, int, int );
void affect_cumulate(CHAR_DATA *, int, int, int, int, int );

int	skill_lookup	args( ( const char *name ) );
int	monk_skill_lookup args( ( const char *name ) );
int	spell_lookup	args( ( const char *name ) );
int	song_lookup	args( ( const char *name ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim, int dam_type ) );
bool	saves_breath	args( ( int level, CHAR_DATA *victim, int dam_type ) );
bool    disbelieve      args( ( int level, CHAR_DATA *victim, CHAR_DATA *ch ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj ) );
int     calc_memtime    args( ( CHAR_DATA *ch, int sn ) );
MD *    create_memdata  args( ( CHAR_DATA *ch, int sn ) );
void    memorize_update args( ( void ) );
void    forget_all      args( ( CHAR_DATA *ch ) );
void    say_spell       args( ( CHAR_DATA *ch, int sn ) );
void    finish_spell    args( ( CHAR_DATA *ch, int sn, void *vo, bool song ) );
void    finish_song     args( ( CHAR_DATA *ch, int sn, void *vo ) );
bool    has_spell_consent args( ( CHAR_DATA *ch, CHAR_DATA *victim ));

/* progs.c */
#ifdef DUNNO_STRSTR
char *  strstr                  args ( (const char *s1, const char *s2 ) );
#endif

void    prog_wordlist_check    args ( ( char * arg, CHAR_DATA *mob,
                                        CHAR_DATA* actor, OBJ_DATA* object,
                                        void* vo, int type ) );
void    prog_percent_check     args ( ( CHAR_DATA *mob, CHAR_DATA* actor,
                                        OBJ_DATA* object, void* vo,
                                        int type ) );
void    prog_act_trigger       args ( ( char* buf, CHAR_DATA* mob,
                                        CHAR_DATA* ch, OBJ_DATA* obj,
                                        void* vo ) );
void    prog_bribe_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                                        int amount ) );
void    prog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void    prog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                                        OBJ_DATA* obj ) );
void    prog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    prog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    prog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    prog_death_trigger     args ( ( CHAR_DATA* mob ) );
void    prog_random_trigger    args ( ( CHAR_DATA* mob ) );
void    prog_speech_trigger    args ( ( char* txt, CHAR_DATA* mob ) );

/* save.c */
void	backup_char_obj	args( ( CHAR_DATA *ch ) ); /* Zen was here :) */
void	delete_char_obj	args( ( CHAR_DATA *ch ) ); /* Zen was here :) */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );
void    save_corpses    args( ( void ) );
void    load_corpses    args( ( void ) );

/* special.c */
MF *	spec_mob_lookup	args( ( const char *name ) );
MF *    envy_spec_mob_convert args( ( const char *name ) );
OF *	spec_obj_lookup	args( ( const char *name ) );

/* spells.c */

/* tables.c */
void	clear_social	args( ( SOC_INDEX_DATA *soc ) );
void	extract_social	args( ( SOC_INDEX_DATA *soc ) );
SID *	new_social	args( ( void ) );
void	free_social	args( ( SOC_INDEX_DATA *soc ) );

void	load_socials	args( ( void ) );
void	save_socials	args( ( void ) );
void	load_classes	args( ( void ) );
void	save_classes	args( ( void ) );
void	save_clan	args( ( CLAN_DATA *clan ) );
void	save_clan_list  args( ( void ) );
void	load_clans	args( ( void ) );
void    set_skills      args( ( CHAR_DATA *ch ) );
void    set_spells      args( ( CHAR_DATA *ch ) );
void    skill_practice  args( ( CHAR_DATA *ch, int sn ) );
void    spell_practice  args( ( CHAR_DATA *ch, int sn ) );
void    lang_practice   args( ( CHAR_DATA *ch, int sn ) );
CLAN_DATA * get_clan	args( ( const char *name ) );
char    * size_string   args( ( int size ) );
char    * position_string args ( ( int position ) );
char    * align_string  args( ( CHAR_DATA *ch ) );
char    * stance_name   args( ( int stance ) );

/* traps.c */
void    setoff_trap     args( ( CHAR_DATA *ch, OBJ_DATA *obj ));

/* vehicle.c */
void    load_vehicles   args( ( FILE *fp ) );
VD *	new_vehicle	args( ( void ) );

/* track.c */
void    found_prey      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    hunt_victim     args( ( CHAR_DATA *ch) );
void    return_to_load  args( ( CHAR_DATA *ch) );

/* gamble.c */
GF *    game_lookup     args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch, bool skills ) );
void	demote_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void    ban_update      args( ( void ) );
void    heartbeat_update args( ( void ) );
void    char_update     args( ( void ) );
void    mobile_update   args( ( void ) );
void    list_update     args( ( void ) );
void    obj_update      args( ( void ) );
void    obj_spec_update args( ( void ) );
void    aggr_update     args( ( void ) );
void    weather_update  args( ( void ) );
void    time_update     args( ( void ) );
void    room_update     args( ( void ) );
void    hit_update      args( ( void ) );
void    mana_update     args( ( void ) );
void    move_update     args( ( void ) );
bool 	camp_update     args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );

/* wiznet.c */
void	wiznet		args ( ( CHAR_DATA *ch, int chan,
				int level, const char * string ) );

/* olc_save.c */
char *  mprog_type_to_name      args ( ( int type ) );
char *	fix_string		args ( ( const char *str ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	MF
#undef	OF
#undef	SID
#undef	GF
#undef	CLD
#undef  EVD
#undef  VD

/***************************************************************************
 *                                                                         *
 *                                    OLC                                  *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * This structure is used in spec_*.c to lookup spec funs and
 * also in olc_act.c to display listings of spec funs.
 */
struct spec_mob_type
{
    char *	spec_name;
    MOB_FUN *	spec_fun;
};

struct spec_obj_type
{
    char *	spec_name;
    OBJ_FUN *	spec_fun;
};

struct game_type
{
    char *      game_name;
    GAME_FUN *  game_fun;
};


/* This structure is used in bit.c to lookup flags and stats */
struct flag_type
{
    char *	name;
    int		bit;
    bool	settable;
};

// This structure is used in bit.c to lookup flags and stats
// for bitvector-based values.
struct bitvector_flag_type
{
    char *	name;
    const BITVECT_DATA * bit;
    bool	settable;
};


/*
 * Area flags.
 */
#define AREA_NONE	     	      0
#define AREA_CHANGED		   BV00		/* Area has been modified    */
#define AREA_ADDED		   BV01		/* Area has been added to    */
#define AREA_LOADING		   BV02		/* Used for counting in db.c */
#define AREA_VERBOSE		   BV03

#define NO_FLAG			    -99		/* Must not be used in flags or stats */


/*
 * Command functions.
 * Defined in olc.c
 */
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_asave        );
DECLARE_DO_FUN( do_alist        );
DECLARE_DO_FUN( do_resets       );


/*
 * Interpreter prototypes.
 */
void    aedit   args( ( CHAR_DATA * ch, char *argument ) );
void    redit   args( ( CHAR_DATA * ch, char *argument ) );
void    medit   args( ( CHAR_DATA * ch, char *argument ) );
void    oedit   args( ( CHAR_DATA * ch, char *argument ) );
void    mpedit  args( ( CHAR_DATA * ch, char *argument ) );

/*
 * Global Constants
 */
extern  char *  const			dir_name        [ ];
extern  const   int			rev_dir         [ ];
extern  char *  const                   dir_rev         [ ];
extern  const   struct  spec_mob_type	spec_mob_table	[ ];
extern  const   struct  spec_obj_type	spec_obj_table	[ ];
extern  const   struct  game_type       game_table      [ ];
extern  const   int     class_avail     [MAX_PC_RACE][MAX_CLASS];
extern  const   int     repop_point     [MAX_PC_RACE][MAX_CLASS];
extern  const   struct  exptable        exp_table[MAX_MOB_LEVEL + 1];
extern  const   struct  monk_skill      monk_skills[MAX_MONK_SKILL];
extern  const   struct  monk_stance     monk_stances[MAX_STANCE];

/*
 * Global variables
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          SHOP_DATA *             shop_last;
extern          GAME_DATA *             game_last;

extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern          int                     top_obj_index;
extern          int                     top_reset;
extern          int                     top_room;
extern          int                     top_shop;
extern          int                     top_game;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

extern          char                    str_empty       [1];

extern		MOB_INDEX_DATA    *	mob_index_hash	[ MAX_KEY_HASH ];
extern		OBJ_INDEX_DATA    *	obj_index_hash	[ MAX_KEY_HASH ];
extern		ROOM_INDEX_DATA   *	room_index_hash [ MAX_KEY_HASH ];
extern		SOC_INDEX_DATA    *	soc_index_hash	[ MAX_WORD_HASH ];
extern		ACT_PROG_DATA     *	mob_act_list;

/* db.c */
void    reset_area      args( ( AREA_DATA * pArea ) );
void    reset_room      args( ( ROOM_INDEX_DATA *pRoom , int not_initial ) );
void    do_abort        args( ( void ) );
void    save_names      args( ( void ) );

/* fraglist.c */
void    check_frag      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    load_fraglist   args( ( void ) );
void    save_fraglist   args( ( void ) );
void    resort_fraglist args( ( CHAR_DATA *ch, FRAGGER_DATA list[] ) );


/* string.c */
void    string_edit     args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *  string_replace  args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *  string_unpad    args( ( char * argument ) );
char *  string_proper   args( ( char * argument ) );

/* olc.c */
bool    run_olc_editor  args( ( DESCRIPTOR_DATA *d ) );
char    *olc_ed_name    args( ( CHAR_DATA *ch ) );
char    *olc_ed_vnum    args( ( CHAR_DATA *ch ) );

/* spec_mob.c */
char *  spec_mob_string	args( ( MOB_FUN *fun ) );
bool    combat_skill_check args ( ( CHAR_DATA *ch ) );

/* spec_obj.c */
char *  spec_obj_string	args( ( OBJ_FUN *fun ) );

/* gamble.c */
char *  game_string     args( ( GAME_FUN *fun ) );

/* bit.c */
extern	const	struct	flag_type	area_flags		[ ];
extern	const	struct	flag_type	sex_flags		[ ];
extern	const	struct	flag_type	exit_flags		[ ];
extern	const	struct	flag_type	door_resets		[ ];
extern	const	struct	flag_type	sector_flags		[ ];
extern	const	struct	flag_type	type_flags		[ ];
extern	const	struct	flag_type	extra_flags		[ ];
extern	const	struct	flag_type	wear_flags		[ ];
extern	const	struct	flag_type	mob_act_flags		[ ];
extern	const	struct	flag_type	plr_act_flags		[ ];
extern	const	struct	flag_type	apply_flags		[ ];
extern	const	struct	flag_type	wear_loc_strings	[ ];
extern	const	struct	flag_type	wear_loc_flags		[ ];
extern	const	struct	flag_type	weapon_flags		[ ];
extern	const	struct	flag_type	container_flags		[ ];
extern	const	struct	flag_type	liquid_flags		[ ];
extern  const   struct  flag_type       material_flags          [ ];
extern	const	struct	flag_type	mprog_type_flags	[ ];
extern	const	struct	flag_type	portal_door_flags	[ ];
extern	const	struct	flag_type	portal_flags		[ ];
extern	const	struct	flag_type	mana_flags		[ ];
extern	const	struct	flag_type	clan_flags		[ ];
extern	const	struct	flag_type	rank_flags		[ ];
extern	struct	bitvector_flag_type	room_flags		[ ];
extern  struct  bitvector_flag_type	affect_flags            [ ];
extern  struct  bitvector_flag_type	item_flags            [ ];
char    *item_string    args( ( OBJ_DATA *obj ));
char    *room_string    args( ( ROOM_INDEX_DATA *room ));
char    *affect_string  args( ( int vector[NUM_AFFECT_VECTORS], bool mortal ) );
char    *event_string   args( ( EVENT_DATA *event ) );
void    set_bitvector   args( ( AFFECT_DATA *af, BITVECT_DATA bvect ) );
bool    get_bitvector   args( ( AFFECT_DATA *af, BITVECT_DATA bvect ) );
bool    IS_AFFECTED     args( ( CHAR_DATA *ch, BITVECT_DATA bvect ) );
void    SET_AFF_BIT     args( ( CHAR_DATA *ch, BITVECT_DATA bvect ) );
void    REMOVE_AFF_BIT  args( ( CHAR_DATA *ch, BITVECT_DATA bvect ) );
void    TOGGLE_AFF_BIT  args( ( CHAR_DATA *ch, BITVECT_DATA bvect ) );
bool    IS_OBJ_STAT     args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
void    SET_OBJ_STAT    args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
void    REMOVE_OBJ_STAT args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
bool    IS_OBJ_AFF      args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
void    SET_OBJ_AFF     args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
void    REMOVE_OBJ_AFF  args( ( OBJ_DATA *obj, BITVECT_DATA bvect ) );
bool    IS_ROOM         args( ( ROOM_INDEX_DATA *room, BITVECT_DATA bvect ) );
void    REMOVE_ROOM_BIT args( ( ROOM_INDEX_DATA *room, BITVECT_DATA bvect ) );
void    SET_ROOM_BIT    args( ( ROOM_INDEX_DATA *room, BITVECT_DATA bvect ) );
/***************************************************************************
 *                                                                         *
 *                                    OLC                                  *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/


#endif
