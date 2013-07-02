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
 *                                                                         *
 *  This code was written by Jason Dinkel and inspired by Russ Taylor,     *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

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
// Note that many of these may be removed as the editor changes.
const BITVECT_DATA AFF_NONE 			= {0,    0};
const BITVECT_DATA AFF_BLIND			= {0, BV00};
const BITVECT_DATA AFF_INVISIBLE		= {0, BV01};
const BITVECT_DATA AFF_FARSEE			= {0, BV02};
const BITVECT_DATA AFF_DETECT_INVIS	 	= {0, BV03};
const BITVECT_DATA AFF_HASTE			= {0, BV04};
const BITVECT_DATA AFF_SENSE_LIFE		= {0, BV05};
const BITVECT_DATA AFF_MINOR_GLOBE		= {0, BV06};
const BITVECT_DATA AFF_STONESKIN		= {0, BV07};
const BITVECT_DATA AFF_UNDERDARK_VIS		= {0, BV08}; // Not imped
const BITVECT_DATA AFF_SHADOW			= {0, BV09}; // Not imped
const BITVECT_DATA AFF_GHOUL			= {0, BV10}; // wraithform
const BITVECT_DATA AFF_GILLS			= {0, BV11}; // waterbreath
const BITVECT_DATA AFF_KO			= {0, BV12}; // Not imped
const BITVECT_DATA AFF_PROTECT_EVIL	  	= {0, BV13};
const BITVECT_DATA AFF_BOUND			= {0, BV14};
const BITVECT_DATA AFF_SLOW_POISON		= {0, BV15};
const BITVECT_DATA AFF_PROTECT_GOOD		= {0, BV16};
const BITVECT_DATA AFF_SLEEP		   	= {0, BV17};
const BITVECT_DATA AFF_SKL_AWARE		= {0, BV18};
const BITVECT_DATA AFF_SNEAK			= {0, BV19};
const BITVECT_DATA AFF_HIDE			= {0, BV20};
const BITVECT_DATA AFF_FEAR			= {0, BV21};
const BITVECT_DATA AFF_CHARM			= {0, BV22};
const BITVECT_DATA AFF_MEDITATE			= {0, BV23};
const BITVECT_DATA AFF_BARKSKIN			= {0, BV24};
const BITVECT_DATA AFF_INFRARED			= {0, BV25}; // infravision
const BITVECT_DATA AFF_LEVITATE			= {0, BV26};
const BITVECT_DATA AFF_FLYING			= {0, BV27};
const BITVECT_DATA AFF_AWARE 			= {0, BV28};
const BITVECT_DATA AFF_PROTECT_FIRE		= {0, BV29};
const BITVECT_DATA AFF_CAMPING	                = {0, BV30};
const BITVECT_DATA AFF_FLAMING			= {1, BV00}; // fireshield
const BITVECT_DATA AFF_ULTRAVISION		= {1, BV01};
const BITVECT_DATA AFF_DETECT_EVIL		= {1, BV02};
const BITVECT_DATA AFF_DETECT_GOOD		= {1, BV03};
const BITVECT_DATA AFF_DETECT_MAGIC		= {1, BV04};
const BITVECT_DATA AFF_MAJOR_PHYSICAL		= {1, BV05};
const BITVECT_DATA AFF_PROTECT_COLD		= {1, BV06};
const BITVECT_DATA AFF_PROTECT_LIGHTNING	= {1, BV07};
const BITVECT_DATA AFF_MINOR_PARA		= {1, BV08};
const BITVECT_DATA AFF_HOLD			= {1, BV09}; // major para
const BITVECT_DATA AFF_SLOWNESS			= {1, BV10};
const BITVECT_DATA AFF_MAJOR_GLOBE		= {1, BV11};
const BITVECT_DATA AFF_PROTECT_GAS		= {1, BV12};
const BITVECT_DATA AFF_PROTECT_ACID		= {1, BV13};
const BITVECT_DATA AFF_POISON			= {1, BV14};
const BITVECT_DATA AFF_SOULSHIELD		= {1, BV15};
const BITVECT_DATA AFF_DUERGAR_HIDE		= {1, BV16};
const BITVECT_DATA AFF_MINOR_INVIS		= {1, BV17};
const BITVECT_DATA AFF_VAMP_TOUCH		= {1, BV18};
const BITVECT_DATA AFF_STUNNED			= {1, BV19};
const BITVECT_DATA AFF_DROPPED_PRIMARY		= {1, BV20};
const BITVECT_DATA AFF_DROPPED_SECOND		= {1, BV21};
const BITVECT_DATA AFF_FUMBLED_PRIMARY		= {1, BV22};
const BITVECT_DATA AFF_FUMBLED_SECOND		= {1, BV23};
const BITVECT_DATA AFF_HOLDING_BREATH		= {1, BV24};
const BITVECT_DATA AFF_MEMORIZING		= {1, BV25};
const BITVECT_DATA AFF_DROWNING			= {1, BV26};
const BITVECT_DATA AFF_PASS_DOOR		= {1, BV27};
const BITVECT_DATA AFF_DRAINING			= {1, BV28};
const BITVECT_DATA AFF_CASTING			= {1, BV29};
const BITVECT_DATA AFF_SCRIBING			= {1, BV30};
const BITVECT_DATA AFF_TENSORS_DISC		= {2, BV00};
const BITVECT_DATA AFF_TRACKING			= {2, BV01};
const BITVECT_DATA AFF_SINGING			= {2, BV02};
const BITVECT_DATA AFF_ECTOPLASMIC		= {2, BV03};
const BITVECT_DATA AFF_ABSORBING		= {2, BV04};
const BITVECT_DATA AFF_VAMP_BITE		= {2, BV05};
const BITVECT_DATA AFF_SPIRIT_WARD		= {2, BV06};
const BITVECT_DATA AFF_GREATER_SPIRIT_WARD	= {2, BV07};
const BITVECT_DATA AFF_NON_DETECTION		= {2, BV08};
const BITVECT_DATA AFF_SILVER			= {2, BV09};
const BITVECT_DATA AFF_PLUS_ONE			= {2, BV10};
const BITVECT_DATA AFF_PLUS_TWO			= {2, BV11};
const BITVECT_DATA AFF_PLUS_THREE		= {2, BV12};
const BITVECT_DATA AFF_PLUS_FOUR		= {2, BV13};
const BITVECT_DATA AFF_PLUS_FIVE		= {2, BV14};
const BITVECT_DATA AFF_ENLARGED			= {2, BV15};
const BITVECT_DATA AFF_REDUCED			= {2, BV16};
const BITVECT_DATA AFF_COVER			= {2, BV17};
const BITVECT_DATA AFF_FOUR_ARMS		= {2, BV18};
const BITVECT_DATA AFF_INERTIAL_BARRIER		= {2, BV19};
const BITVECT_DATA AFF_INTELLECT_FORTRESS	= {2, BV20};
const BITVECT_DATA AFF_COLDSHIELD		= {2, BV21};
const BITVECT_DATA AFF_CANNIBALIZING		= {2, BV22};
const BITVECT_DATA AFF_SWIMMING			= {2, BV23};
const BITVECT_DATA AFF_TOWER_OF_IRON_WILL	= {2, BV24};
const BITVECT_DATA AFF_UNDERWATER		= {2, BV25};
const BITVECT_DATA AFF_BLUR			= {2, BV26};
const BITVECT_DATA AFF_NECKBITING		= {2, BV27};
const BITVECT_DATA AFF_ELEMENTAL_FORM		= {2, BV28};
const BITVECT_DATA AFF_PASS_WITHOUT_TRACE	= {2, BV29};
const BITVECT_DATA AFF_PALADIN_AURA		= {2, BV30};
const BITVECT_DATA AFF_LOOTER			= {3, BV00};
const BITVECT_DATA AFF_PLAGUE			= {3, BV01}; // diseased
const BITVECT_DATA AFF_SACKING			= {3, BV02};
const BITVECT_DATA AFF_SENSE_FOLLOWER		= {3, BV03};
const BITVECT_DATA AFF_STORNOG_SPHERES		= {3, BV04};
const BITVECT_DATA AFF_GREATER_SPHERES		= {3, BV05};
const BITVECT_DATA AFF_VAMPIRE_FORM		= {3, BV06};
const BITVECT_DATA AFF_NO_UNMORPH		= {3, BV07};
const BITVECT_DATA AFF_HOLY_SACRIFICE		= {3, BV08};
const BITVECT_DATA AFF_BATTLE_ECSTASY		= {3, BV09};
const BITVECT_DATA AFF_DAZZLE			= {3, BV10};
const BITVECT_DATA AFF_DAZZLED			= {3, BV11};
const BITVECT_DATA AFF_THROAT_CRUSH		= {3, BV12};
const BITVECT_DATA AFF_REGENERATION		= {3, BV13};
const BITVECT_DATA AFF_BEARHUG			= {3, BV14};
const BITVECT_DATA AFF_GRAPPLING		= {3, BV15};
const BITVECT_DATA AFF_GRAPPLED			= {3, BV16};
const BITVECT_DATA AFF_MAGE_FLAME		= {3, BV17};
const BITVECT_DATA AFF_NO_IMMOLATE		= {3, BV18};
const BITVECT_DATA AFF_MULTICLASS		= {3, BV19};
const BITVECT_DATA AFF_DETECT_UNDEAD		= {3, BV20};
// Internal-only values.  Whether intentional or just not yet
// added to DikuEdit, they are only settable within the game.
// - Veygoth
const BITVECT_DATA AFF_IS_FLEEING       	= {4, BV00};
const BITVECT_DATA AFF_HUNTING          	= {4, BV01};
const BITVECT_DATA AFF_BIOFEEDBACK              = {4, BV02};
const BITVECT_DATA AFF_FAMINE			= {4, BV03};
const BITVECT_DATA AFF_MUTE                     = {4, BV04};
const BITVECT_DATA AFF_FAERIE_FIRE              = {4, BV05};
const BITVECT_DATA AFF_SANCTUARY                = {4, BV06};
const BITVECT_DATA AFF_CHANGE_SEX               = {4, BV07};
const BITVECT_DATA AFF_CURSE                    = {4, BV08};
const BITVECT_DATA AFF_DETECT_HIDDEN		= {4, BV09};
const BITVECT_DATA AFF_POLYMORPH                = {4, BV10};
const BITVECT_DATA AFF_COMP_LANG                = {4, BV11};
const BITVECT_DATA AFF_DENY_EARTH               = {4, BV12};
const BITVECT_DATA AFF_DENY_AIR                 = {4, BV13};
const BITVECT_DATA AFF_DENY_FIRE                = {4, BV14};
const BITVECT_DATA AFF_DENY_WATER               = {4, BV15};
const BITVECT_DATA AFF_TRACK                    = {4, BV16};
const BITVECT_DATA AFF_JUSTICE_TRACKER          = {4, BV17};
const BITVECT_DATA AFF_LAYHANDS_TIMER           = {4, BV18};
const BITVECT_DATA AFF_ELEM_SIGHT               = {4, BV19};
// BV20 unused.  Was a duplicate of dazzle
const BITVECT_DATA AFF_MISDIRECTION             = {4, BV21};
const BITVECT_DATA AFF_VACANCY                  = {4, BV22};
const BITVECT_DATA AFF_CHANGE_SELF              = {4, BV23};
const BITVECT_DATA AFF_PROWESS                  = {4, BV24};
const BITVECT_DATA AFF_SUMMON_MOUNT_TIMER       = {4, BV25};
const BITVECT_DATA AFF_INCOMPETENCE             = {4, BV26};
const BITVECT_DATA AFF_CLIMBING                 = {4, BV27};

/*
 * The object extra_flags have exceeded a single bitvector and
 * have also been converted to BITVECT_DATA type.
 *
 * - Veygoth
 */
const BITVECT_DATA ITEM_NONE 			= {0,    0};
const BITVECT_DATA ITEM_GLOW                  	= {0, BV00};
const BITVECT_DATA ITEM_NOSHOW                	= {0, BV01}; // new, was ITEM_HUM = 1, 25
const BITVECT_DATA ITEM_BURIED                	= {0, BV02}; // new, was ITEM_DARK (gone)
const BITVECT_DATA ITEM_NOSELL                	= {0, BV03}; // new, was ITEM_LOCK (gone)
const BITVECT_DATA ITEM_EVIL                  	= {0, BV04};
const BITVECT_DATA ITEM_INVIS                 	= {0, BV05};
const BITVECT_DATA ITEM_MAGIC                 	= {0, BV06};
const BITVECT_DATA ITEM_NODROP                	= {0, BV07};
const BITVECT_DATA ITEM_BLESS                 	= {0, BV08};
const BITVECT_DATA ITEM_ANTI_GOOD             	= {0, BV09};
const BITVECT_DATA ITEM_ANTI_EVIL             	= {0, BV10};
const BITVECT_DATA ITEM_ANTI_NEUTRAL          	= {0, BV11};
const BITVECT_DATA ITEM_SECRET                	= {0, BV12}; // was ITEM_NOREMOVE = 2, 18
const BITVECT_DATA ITEM_FLOAT                 	= {0, BV13}; // was ITEM_INVENTORY = 1, 27
const BITVECT_DATA ITEM_NOBURN                	= {0, BV14}; // was ITEM_POISONED = 2, 17
const BITVECT_DATA ITEM_NOLOCATE              	= {0, BV15}; // was ITEM_VAMPIRE_BANE (gone)
const BITVECT_DATA ITEM_NOID                  	= {0, BV16}; // was ITEM_HOLY = 2, 19
const BITVECT_DATA ITEM_NOSUMMON              	= {0, BV17}; // was ITEM_VIS_DEATH (gone)
const BITVECT_DATA ITEM_LIT             	= {0, BV18}; 
const BITVECT_DATA ITEM_TRANSIENT             	= {0, BV19};
const BITVECT_DATA ITEM_NOSLEEP               	= {0, BV20}; 
const BITVECT_DATA ITEM_NOCHARM               	= {0, BV21};
const BITVECT_DATA ITEM_TWOHANDED            	= {0, BV22};
const BITVECT_DATA ITEM_NORENT                	= {0, BV23};
const BITVECT_DATA ITEM_GOODONLY              	= {0, BV24};
const BITVECT_DATA ITEM_HUM                   	= {0, BV25};
const BITVECT_DATA ITEM_LEVITATES             	= {0, BV26};
const BITVECT_DATA ITEM_INVENTORY             	= {0, BV27};
const BITVECT_DATA ITEM_WAS_DISARMED          	= {0, BV28};
const BITVECT_DATA ITEM_WHOLEBODY             	= {0, BV29};
const BITVECT_DATA ITEM_WHOLEHEAD             	= {0, BV30};
const BITVECT_DATA ITEM_SILVER                  = {1, BV00};
const BITVECT_DATA ITEM_THROW_RETURN            = {1, BV06};
const BITVECT_DATA ITEM_THROW_ONEROOM           = {1, BV07};
const BITVECT_DATA ITEM_THROW_TWOROOMS          = {1, BV08};
const BITVECT_DATA ITEM_ARTIFACT                = {1, BV09};
const BITVECT_DATA ITEM_ENLARGED                = {1, BV16};
const BITVECT_DATA ITEM_REDUCED                 = {1, BV17};
const BITVECT_DATA ITEM_POISONED                = {1, BV18};

/*
 * The room flags have exceeded a single bitvector and
 * have also been converted to BITVECT_DATA type.
 *
 * - Veygoth
 */
const BITVECT_DATA ROOM_NONE 			= {0,    0};
const BITVECT_DATA ROOM_DARK                    = {0, BV00};
const BITVECT_DATA ROOM_LIMITSCAN               = {0, BV01};
const BITVECT_DATA ROOM_NO_MOB                  = {0, BV02};
const BITVECT_DATA ROOM_INDOORS                 = {0, BV03};
const BITVECT_DATA ROOM_SILENT                  = {0, BV04};
const BITVECT_DATA ROOM_UNDERWATER              = {0, BV05}; 
const BITVECT_DATA ROOM_NO_RECALL               = {0, BV06};
const BITVECT_DATA ROOM_NO_MAGIC                = {0, BV07};
const BITVECT_DATA ROOM_TUNNEL                  = {0, BV08};
const BITVECT_DATA ROOM_PRIVATE                 = {0, BV09};
// BV10 Unused in DE.
const BITVECT_DATA ROOM_SAFE                    = {0, BV11};
const BITVECT_DATA ROOM_NO_PRECIP               = {0, BV12};
const BITVECT_DATA ROOM_SINGLE_FILE             = {0, BV13};
const BITVECT_DATA ROOM_JAIL                    = {0, BV14};
const BITVECT_DATA ROOM_NO_TELEPORT             = {0, BV15};
const BITVECT_DATA ROOM_SOLITARY                = {0, BV16};
const BITVECT_DATA ROOM_HEAL                    = {0, BV17}; 
const BITVECT_DATA ROOM_NO_HEAL                 = {0, BV18};
const BITVECT_DATA ROOM_INN                     = {0, BV19};
const BITVECT_DATA ROOM_DOCKABLE                = {0, BV20};
const BITVECT_DATA ROOM_MAGICDARK               = {0, BV21};
const BITVECT_DATA ROOM_MAGICLIGHT              = {0, BV22};
const BITVECT_DATA ROOM_NO_SUMMON               = {0, BV23};
const BITVECT_DATA ROOM_GUILDROOM               = {0, BV24};
const BITVECT_DATA ROOM_TWILIGHT                = {0, BV25};
const BITVECT_DATA ROOM_NO_PSIONICS             = {0, BV26};
const BITVECT_DATA ROOM_NO_GATE                 = {0, BV27};
const BITVECT_DATA ROOM_BANK                    = {0, BV28};
const BITVECT_DATA ROOM_PET_SHOP                = {0, BV29}; // Unused in editor.
const BITVECT_DATA ROOM_NO_SCAN                 = {0, BV30};
const BITVECT_DATA ROOM_EARTHEN_STARSHELL       = {1, BV00};
const BITVECT_DATA ROOM_AIRY_STARSHELL          = {1, BV01};
const BITVECT_DATA ROOM_FIERY_STARSHELL         = {1, BV02};
const BITVECT_DATA ROOM_WATERY_STARSHELL        = {1, BV03};
const BITVECT_DATA ROOM_HYPNOTIC_PATTERN        = {1, BV04};

/*
 * The code below uses a table lookup system that is based on suggestions
 * from Russ Taylor.  There are many routines in handler.c that would benefit
 * with the use of tables.  You may consider simplifying your code base by
 * implementing a system like below with such functions - Jason Dinkel
 */

struct flag_stat_type
{
    const struct flag_type	*structure;
    bool			 stat;
};

struct bitvector_stat_type
{
    const struct bitvector_flag_type	*structure;
    bool				 stat;
};


const	struct	flag_stat_type	flag_stat_table	[ ]	=
{
    {	area_flags, 		FALSE	},
    {	sex_flags, 		TRUE	},
    {	exit_flags, 		FALSE	},
    {	door_resets, 		TRUE	},
    {	sector_flags, 		TRUE	},
    {	type_flags, 		TRUE	},
    {	wear_flags, 		FALSE	},
    {	mob_act_flags, 		FALSE	},
    {	plr_act_flags, 		FALSE	},
    {	apply_flags, 		TRUE	},
    {	wear_loc_flags, 	TRUE	},
    {	wear_loc_strings, 	TRUE	},
    {	weapon_flags, 		TRUE	},
    {	container_flags, 	FALSE	},
    {	liquid_flags, 		TRUE	},
    {   material_flags,         TRUE    },
    {	mprog_type_flags, 	TRUE	},
    {	portal_door_flags, 	FALSE	},
    {	portal_flags,	 	FALSE	},
    {	mana_flags,	 	FALSE	},
    {	rank_flags, 		TRUE	},
    {	clan_flags, 		TRUE	},
    {	0, 			0	}
};

bool is_stat( const struct flag_type *flag_table )
{
    int flag;

    last_fun( "is_stat");

    for ( flag = 0; flag_stat_table[flag].structure; flag++ )
    {
        if ( flag_stat_table[flag].structure == flag_table
            && flag_stat_table[flag].stat )
            return TRUE;
    }
    return FALSE;
}

/*
 * This function is Russ Taylor's creation.  Thanks Russ!
 * all code copyright (C) Russ Taylor, permission to use and/or distribute
 * has NOT been granted.  Use only in this OLC package has been granted.
 */
int flag_lookup( const char *name, const struct flag_type *flag_table )
{
    int flag;

    last_fun( "flag_lookup");

    for ( flag = 0; *flag_table[flag].name; flag++ )
    {
	if ( !str_cmp( name, flag_table[flag].name )
	    && flag_table[flag].settable )
	    return flag_table[flag].bit;
    }

    return NO_FLAG;
}


int flag_value( const struct flag_type *flag_table, char *argument )
{
    char word [ MAX_INPUT_LENGTH ];
    int  bit;
    int  marked	= 0;
    bool found	= FALSE;

    last_fun( "flag_value");

    if ( is_stat( flag_table ) )
    {
	one_argument( argument, word );

	if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
	    return bit;
	else
	    return NO_FLAG;
    }

    for ( ; ; )
    {
	argument = one_argument( argument, word );

	if ( word[0] == '\0' )
	    break;

	if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
	{
	    SET_BIT( marked, bit );
	    found = TRUE;
	}
    }

    if ( found )
	return marked;
    else
	return NO_FLAG;
}

char *flag_string( const struct flag_type *flag_table, int bits )
{
    static char buf [ MAX_STRING_LENGTH ];
    int         flag;

    last_fun( "flag_string");

    buf[0] = '\0';

    for ( flag = 0; *flag_table[flag].name; flag++ )
    {
        if ( !is_stat( flag_table ) && IS_SET( bits, flag_table[flag].bit ) )
        {
            strcat( buf, " " );
            strcat( buf, flag_table[flag].name );
        }
        else if ( flag_table[flag].bit == bits )
        {
            strcat( buf, " " );
            strcat( buf, flag_table[flag].name );
            break;
        }
    }
    return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

char *item_string( OBJ_DATA *obj )
{
   static char buf[MAX_STRING_LENGTH];
   int count;

   last_fun( "item_string");

   buf[0] = '\0';

   for( count = 0; item_flags[count].bit; count ++ )
   {
      if( IS_OBJ_STAT( obj, *item_flags[count].bit ))
      {
        strcat( buf, " " );
        strcat( buf, item_flags[count].name );
      }
   }

   return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

char *room_string( ROOM_INDEX_DATA *room )
{
   static char buf[MAX_STRING_LENGTH];
   int count;

   last_fun( "room_string");

   buf[0] = '\0';

   for( count = 0; room_flags[count].bit; count ++ )
   {
      if( IS_ROOM( room, *room_flags[count].bit ))
      {
        strcat( buf, " " );
        strcat( buf, room_flags[count].name );
      }
   }

   return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

char *affect_string( int vector[NUM_AFFECT_VECTORS], bool mortal )
{
   static char buf[MAX_STRING_LENGTH];
   int count;

   last_fun( "affect_string");

   buf[0] = '\0';

   for( count = 0; affect_flags[count].bit; count ++ )
   {
      if( IS_SET( vector[(affect_flags[count].bit->group)], affect_flags[count].bit->vector ))
      {
        if ( mortal ) {
          if ( !strcmp( affect_flags[count].name, "awareskill" ) )  continue;
//          if ( !strcmp( affect_flags[count].name, "bound" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "camping" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "casting" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "duergarhide" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "fourarms" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "hide" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "justicetracker" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "layhandstimer" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "meditating" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "poison" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "singing" ) )  continue;
//          if ( !strcmp( affect_flags[count].name, "sneak" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "summoned" ) )  continue;
          if ( !strcmp( affect_flags[count].name, "summonmounttimer" ) )  continue;
        }
        strcat( buf, " " );
        strcat( buf, affect_flags[count].name );
      }
   }

   return ( buf[0] != '\0' ) ? buf + 1 : "none";
}

const	struct	flag_type	area_flags	[ ]	=
{
    {	"none", 		AREA_NONE, 	FALSE	},
    {	"changed", 		AREA_CHANGED,	FALSE	},
    {	"added", 		AREA_ADDED, 	FALSE	},
    {	"loading", 		AREA_LOADING, 	FALSE	},
    {	"verbose", 		AREA_VERBOSE, 	FALSE	},
    {	"", 			0, 		0	}
};


const	struct	flag_type	sex_flags	[ ]	=
{
    {	"male", 		SEX_MALE, 	TRUE	},
    {	"female", 		SEX_FEMALE, 	TRUE	},
    {	"neutral", 		SEX_NEUTRAL, 	TRUE	},
    {	"", 			0, 		0	}
};


const	struct	flag_type	exit_flags	[ ]	=
{
    {	"door", 		EX_ISDOOR, 	TRUE	},
    {	"closed", 		EX_CLOSED, 	TRUE	},
    {	"locked", 		EX_LOCKED, 	TRUE	},
    {	"bashed", 		EX_BASHED, 	FALSE	},
    {   "secret",               EX_SECRET,      TRUE    },
    {   "blocked",              EX_BLOCKED,     TRUE    },
    {   "walled",               EX_WALLED,      TRUE    },
    {	"bashproof", 		EX_BASHPROOF, 	TRUE	},
    {	"pickproof", 		EX_PICKPROOF, 	TRUE	},
    {	"passproof", 		EX_PASSPROOF, 	TRUE	},
    {	"eat_key", 		EX_EAT_KEY, 	TRUE	},
    {	"", 			0, 		0	}
};


const	struct	flag_type	door_resets	[ ]	=
{
    {	"open and unlocked", 	0, 		TRUE	},
    {	"closed and unlocked", 	1, 		TRUE	},
    {	"closed and locked", 	2, 		TRUE	},
    {	"", 			0, 		0	}
};


const	struct	flag_type	sector_flags	[ ]	=
{
    {	"inside", 		SECT_INSIDE, 		TRUE	},
    {	"city", 		SECT_CITY, 		TRUE	},
    {	"field", 		SECT_FIELD, 		TRUE	},
    {	"forest", 		SECT_FOREST, 		TRUE	},
    {	"hills", 		SECT_HILLS, 		TRUE	},
    {	"mountain", 		SECT_MOUNTAIN, 		TRUE	},
    {	"water_swim", 		SECT_WATER_SWIM, 	TRUE	},
    {	"water_noswim", 	SECT_WATER_NOSWIM, 	TRUE	},
    {	"underwater", 		SECT_UNDERWATER, 	TRUE	},
    {	"air", 			SECT_AIR, 		TRUE	},
    {	"desert", 		SECT_DESERT, 		TRUE	},
    {	"plane of fire",	SECT_PLANE_FIRE,	TRUE	},
    {	"plane of water",	SECT_PLANE_WATER,	TRUE	},
    {	"plane of earth",	SECT_PLANE_EARTH,	TRUE	},
    {	"plane of air",		SECT_PLANE_AIR,		TRUE	},
    {   "astral plane",         SECT_PLANE_ASTRAL,      TRUE    },
    {   "ethereal plane",       SECT_PLANE_ETHEREAL,    TRUE    },
    {   "arctic",               SECT_ARCTIC,            TRUE    },
    {   "swamp",                SECT_SWAMP,             TRUE    },
    {   "ocean",                SECT_OCEAN,             TRUE    },
    {   "underwater-ground",    SECT_UNDERWATER_GROUND, TRUE    },
    {   "underworld - city",    SECT_UNDERG_CITY,       TRUE    },
    {   "underworld - wild",    SECT_UNDERG_WILD,       TRUE    },
    {   "underworld - inside",  SECT_UNDERG_INSIDE,     TRUE    },
    {   "underworld - water",   SECT_UNDERG_WATER,      TRUE    },
    {   "underworld - noswim water", SECT_UNDERG_WATER_NOSWIM, TRUE },
    {   "underworld - noground",SECT_UNDERG_NOGROUND,   TRUE    },
    {	"", 			0, 			0	}
};


const	struct	flag_type	type_flags	[ ]	=
{
    {	"light", 		TYPE_LIGHT, 		TRUE	},
    {	"scroll", 		TYPE_SCROLL, 		TRUE	},
    {	"wand", 		TYPE_WAND, 		TRUE	},
    {	"staff", 		TYPE_STAFF, 		TRUE	},
    {	"weapon", 		TYPE_WEAPON, 		TRUE	},
    {	"treasure", 		TYPE_TREASURE, 		TRUE	},
    {	"armor", 		TYPE_ARMOR, 		TRUE	},
    {	"potion", 		TYPE_POTION, 		TRUE	},
    {	"other", 		TYPE_OTHER, 		TRUE	},
    {	"trash", 		TYPE_TRASH, 		TRUE	},
    {	"container", 		TYPE_CONTAINER, 	TRUE	},
    {	"drink-container", 	TYPE_DRINK_CON, 	TRUE	},
    {	"key", 			TYPE_KEY, 		TRUE	},
    {	"food", 		TYPE_FOOD, 		TRUE	},
    {	"money", 		TYPE_MONEY, 		TRUE	},
    {	"boat", 		TYPE_BOAT, 		TRUE	},
    {	"npc_corpse", 		TYPE_CORPSE_NPC, 	TRUE	},
    {	"pc_corpse", 		TYPE_CORPSE_PC, 	FALSE	},
    {	"pill", 		TYPE_PILL, 		TRUE	},
    {	"portal", 		TYPE_PORTAL, 		TRUE	},
    {	"clothing", 		TYPE_CLOTHING, 		TRUE	},
    {	"", 			0, 			0	}
};


const	struct	flag_type	wear_flags	[ ]	=
{
    {	"take", 		ITEM_TAKE, 		TRUE	},
    {	"finger", 		ITEM_WEAR_FINGER, 	TRUE	},
    {	"neck", 		ITEM_WEAR_NECK, 	TRUE	},
    {	"body", 		ITEM_WEAR_BODY, 	TRUE	},
    {	"head", 		ITEM_WEAR_HEAD, 	TRUE	},
    {	"legs", 		ITEM_WEAR_LEGS, 	TRUE	},
    {	"feet",			ITEM_WEAR_FEET, 	TRUE	},
    {	"hands", 		ITEM_WEAR_HANDS, 	TRUE	},
    {	"arms", 		ITEM_WEAR_ARMS, 	TRUE	},
    {	"shield", 		ITEM_WEAR_SHIELD, 	TRUE	},
    {	"about", 		ITEM_WEAR_ABOUT, 	TRUE	},
    {	"waist", 		ITEM_WEAR_WAIST, 	TRUE	},
    {	"wrist", 		ITEM_WEAR_WRIST, 	TRUE	},
    {	"wield", 		ITEM_WIELD, 		TRUE	},
    {	"hold", 		ITEM_HOLD, 		TRUE	},
    {   "eyes",                 ITEM_WEAR_EYES,         TRUE    },
    {   "face",                 ITEM_WEAR_FACE,         TRUE    },
    {   "ear",                  ITEM_WEAR_EAR,          TRUE    },
    {   "quiver",               ITEM_QUIVER,            TRUE    },
    {   "badge",                ITEM_BADGE,             TRUE    },
    {   "on back",              ITEM_WEAR_ONBACK,       TRUE    },
    {   "attached to belt",     ITEM_ATTACH_BELT,       TRUE    },
    {   "tail",                 ITEM_TAIL,              TRUE    },
    {   "horns",                ITEM_HORNS,             TRUE    },
    {	"", 			0, 			0	}
};

// Not fully updated to show all new flags - Veygoth
const	struct	flag_type	mob_act_flags	[ ]	=
{
    {   "no_telekinesis",       ACT_CANT_TK,            TRUE    },
    {   "nice_thief",           ACT_NICE_THIEF,         TRUE    },
    {   "outlaw",               ACT_OUTLAW,             TRUE    },
    {   "witness",              ACT_WITNESS,            TRUE    },
    {   "can_fly",              ACT_CANFLY,             TRUE    },
    {   "can_swim",             ACT_CANSWIM,            TRUE    },
    {	"npc", 			ACT_IS_NPC, 		FALSE	},
    {	"moved", 		ACT_MOVED, 		FALSE	},
    {   "sizeplusone",          ACT_SIZEPLUS,           TRUE    },
    {   "sizeminusone",         ACT_SIZEMINUS,          TRUE    },
    {   "no_exp",               ACT_NOEXP,              TRUE    },
    {   "no_bash",              ACT_NOBASH,             TRUE    },
    {   "no_charm",             ACT_NOCHARM,            TRUE    },
    {   "no_para",              ACT_NOPARA,             TRUE    },
    {   "no_summon",            ACT_NOSUMMON,           TRUE    },
    {   "memory",               ACT_MEMORY,             TRUE    },
    {   "hunter",               ACT_HUNTER,             TRUE    },
    {	"sentinel", 		ACT_SENTINEL, 		TRUE	},
    {	"scavenger", 		ACT_SCAVENGER, 		TRUE	},
    {	"aggressive", 		ACT_AGGRESSIVE, 	TRUE	},
    {	"aggr_good", 		ACT_AGGROGOOD,	 	TRUE	},
    {	"aggr_evil", 		ACT_AGGROEVIL,	 	TRUE	},
    {	"aggr_neut", 		ACT_AGGRONEUT,	 	TRUE	},
    {	"aggr_goodrace",	ACT_AGGROGOODRACE, 	TRUE	},
    {	"aggr_evilrace",	ACT_AGGROEVILRACE, 	TRUE	},
    {	"stay_area", 		ACT_STAY_AREA, 		TRUE	},
    {	"wimpy", 		ACT_WIMPY, 		TRUE	},
    {	"pet", 			ACT_PET, 		TRUE	},
    {	"teacher", 		ACT_TEACHER, 		TRUE	},
    {	"protector", 		ACT_PROTECTOR, 		TRUE	},
    {	"mountable", 		ACT_MOUNT, 		TRUE	},
    {	"", 			0, 			0	}
};

// Not fully updated to show all new flags - Veygoth
const	struct	flag_type	plr_act_flags	[ ]	=
{
    {	"pager", 		PLR_PAGER, 		TRUE	},
    {   "tell",                 PLR_TELL,               TRUE    },
    {   "log",                  PLR_LOG,                TRUE    },
    {   "deny",                 PLR_DENY,               TRUE    },
    {   "camping",              PLR_CAMPING,            TRUE    },
    {   "prompt",               PLR_PROMPT,             TRUE    },
    {   "blank",                PLR_BLANK,              TRUE    },
    {   "combine",              PLR_COMBINE,            TRUE    },
    {   "shout",                PLR_SHOUT,              TRUE    },
    {   "silence",              PLR_SILENCE,            TRUE    },
    {   "noemote",              PLR_NO_EMOTE,           TRUE    },
    {   "moved",                PLR_MOVED,              TRUE    },
    {   "autoexit",             PLR_AUTOEXIT,           TRUE    },
    {   "brief",                PLR_BRIEF,              TRUE    },
    {   "wizinvis",             PLR_WIZINVIS,           TRUE    },
    {   "wizbit",               PLR_WIZBIT,             TRUE    },
    {   "vicious",              PLR_VICIOUS,            TRUE    },
    {   "holylight",            PLR_HOLYLIGHT,          TRUE    },
    {   "memorizing",           PLR_MEMORIZING,         TRUE    },
    {   "meditating",           PLR_MEDITATING,         TRUE    },
    {   "just_died",            PLR_JUST_DIED,          TRUE    },
    {   "telnet_ga",            PLR_TELNET_GA,          TRUE    },
/* Add these
#define PLR_FREEZE                 BV21
#define PLR_COLOR_CON              BV22 // players see multicolored con messages
#define PLR_MAP                    BV23 // Added by Lohrr.
#define PLR_CAST_TICK              BV24
#define PLR_AFK                    BV25
#define PLR_COLOUR                 BV26
#define PLR_PAGER                  BV28
#define PLR_MOUNTABLE              BV29 // Can be mounted       
#define PLR_FOG                    BV30
*/
    {	"",
0,
0 }
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const	struct	flag_type	apply_flags	[ ]	=
{
    {	"none", 		APPLY_NONE, 		TRUE	},
    {	"strength", 		APPLY_STR, 		TRUE	},
    {	"dexterity", 		APPLY_DEX, 		TRUE	},
    {	"intelligence", 	APPLY_INT, 		TRUE	},
    {	"wisdom", 		APPLY_WIS, 		TRUE	},
    {	"constitution", 	APPLY_CON, 		TRUE	},
    {	"sex", 			APPLY_SEX, 		TRUE	},
    {	"class", 		APPLY_CLASS, 		TRUE	},
    {	"level", 		APPLY_LEVEL, 		TRUE	},
    {	"age", 			APPLY_AGE, 		TRUE	},
    {	"height", 		APPLY_HEIGHT, 		TRUE	},
    {	"weight", 		APPLY_WEIGHT, 		TRUE	},
    {	"mana", 		APPLY_MANA, 		TRUE	},
    {	"hp", 			APPLY_HIT, 		TRUE	},
    {	"move", 		APPLY_MOVE, 		TRUE	},
    {	"gold", 		APPLY_GOLD, 		TRUE	},
    {	"experience", 		APPLY_EXP, 		TRUE	},
    {	"ac", 			APPLY_AC, 		TRUE	},
    {	"hitroll", 		APPLY_HITROLL, 		TRUE	},
    {	"damroll", 		APPLY_DAMROLL, 		TRUE	},
    {	"saving-para", 		APPLY_SAVING_PARA, 	TRUE	},
    {	"saving-rod", 		APPLY_SAVING_ROD, 	TRUE	},
    {	"saving-petri", 	APPLY_SAVING_PETRI, 	TRUE	},
    {	"saving-breath", 	APPLY_SAVING_BREATH, 	TRUE	},
    {	"saving-spell", 	APPLY_SAVING_SPELL, 	TRUE	},
    {	"resistant", 		APPLY_RESISTANT, 	TRUE	},
    {	"immune", 		APPLY_IMMUNE, 		TRUE	},
    {	"susceptible", 		APPLY_SUSCEPTIBLE,	TRUE	},
    {   "agility",              APPLY_AGI,              TRUE    },
    {   "charisma",             APPLY_CHA,              TRUE    },
    {   "power",                APPLY_POW,              TRUE    },
    {   "luck",                 APPLY_LUK,              TRUE    },
    {	"", 			0, 			0	}
};


/*
 * What is seen.
 */
const	struct	flag_type	wear_loc_strings	[ ]	=
{
    {	"in the inventory", 	WEAR_NONE, 		TRUE	},
    {	"on the left finger", 	WEAR_FINGER_L, 		TRUE	},
    {	"on the right finger", 	WEAR_FINGER_R, 		TRUE	},
    {	"around the neck (1)", 	WEAR_NECK_1, 		TRUE	},
    {	"around the neck (2)", 	WEAR_NECK_2, 		TRUE	},
    {	"on the body", 		WEAR_BODY, 		TRUE	},
    {	"over the head", 	WEAR_HEAD, 		TRUE	},
    {	"on the legs", 		WEAR_LEGS, 		TRUE	},
    {	"on the feet", 		WEAR_FEET, 		TRUE	},
    {	"on the hands", 	WEAR_HANDS, 		TRUE	},
    {	"on the arms", 		WEAR_ARMS, 		TRUE	},
    {	"about the shoulders", 	WEAR_ABOUT, 		TRUE	},
    {	"around the waist", 	WEAR_WAIST, 		TRUE	},
    {	"on the left wrist", 	WEAR_WRIST_L, 		TRUE	},
    {	"on the right wrist", 	WEAR_WRIST_R, 		TRUE	},
    {	"primary wield", 	WEAR_HAND, 		TRUE	},
    {	"second wield", 	WEAR_HAND_2, 		TRUE	},
    {   "on the eyes",          WEAR_EYES,              TRUE    },
    {   "on the face",          WEAR_FACE,              TRUE    },
    {   "in the left ear",      WEAR_EAR_L,             TRUE    },
    {   "in the right ear",     WEAR_EAR_R,             TRUE    },
    {   "as a badge",           WEAR_BADGE,             TRUE    },
    {   "on the back",          WEAR_ONBACK,            TRUE    },
    {   "attached to belt (1)", WEAR_BELT_1,            TRUE    },
    {   "attached to belt (2)", WEAR_BELT_2,            TRUE    },
    {   "attached to belt (3)", WEAR_BELT_3,            TRUE    },
    {	"", 			0,			0	}
};


/*
 * What is typed.
 * neck2 should not be settable for loaded mobiles.
 */
const	struct	flag_type	wear_loc_flags	[ ]	=
{
    {	"none", 		WEAR_NONE, 		TRUE	},
    {	"left finger", 		WEAR_FINGER_L, 		TRUE	},
    {	"right finger", 	WEAR_FINGER_R, 		TRUE	},
    {	"neck", 		WEAR_NECK_1, 		TRUE	},
    {	"neck2", 		WEAR_NECK_2, 		TRUE	},
    {	"body", 		WEAR_BODY, 		TRUE	},
    {	"head", 		WEAR_HEAD, 		TRUE	},
    {	"legs", 		WEAR_LEGS, 		TRUE	},
    {	"feet", 		WEAR_FEET, 		TRUE	},
    {	"hands", 		WEAR_HANDS, 		TRUE	},
    {	"arms", 		WEAR_ARMS, 		TRUE	},
    {	"about", 		WEAR_ABOUT, 		TRUE	},
    {	"waist", 		WEAR_WAIST, 		TRUE	},
    {	"left wrist", 		WEAR_WRIST_L, 		TRUE	},
    {	"right wrist", 		WEAR_WRIST_R, 		TRUE	},
    {	"primary hand", 	WEAR_HAND, 		TRUE	},
    {	"second hand", 		WEAR_HAND_2, 		TRUE	},
    {	"third hand", 		WEAR_HAND_3, 		TRUE	},
    {	"fourth hand", 		WEAR_HAND_4, 		TRUE	},
    {   "eyes",			WEAR_EYES,              TRUE    },
    {   "face",          	WEAR_FACE,              TRUE    },
    {   "left ear",      	WEAR_EAR_L,             TRUE    },
    {   "right ear",     	WEAR_EAR_R,             TRUE    },
    {   "badge",           	WEAR_BADGE,             TRUE    },
    {   "back",          	WEAR_ONBACK,            TRUE    },
    {   "belt1", 		WEAR_BELT_1,            TRUE    },
    {   "belt2", 		WEAR_BELT_2,            TRUE    },
    {   "belt3", 		WEAR_BELT_3,            TRUE    },
    {	"", 			0, 			0	}
};


const	struct	flag_type	weapon_flags	[ ]	=
{
    {	"hit", 			0, 		TRUE	},
    {	"slice", 		1, 		TRUE	},
    {	"stab", 		2, 		TRUE	},
    {	"slash", 		3, 		TRUE	},
    {	"whip", 		4, 		TRUE	},
    {	"claw", 		5, 		TRUE	},
    {	"blast", 		6, 		TRUE	},
    {	"pound", 		7, 		TRUE	},
    {	"crush", 		8, 		TRUE	},
    {	"grep", 		9, 		TRUE	},
    {	"bite", 		10, 		TRUE	},
    {	"pierce", 		11, 		TRUE	},
    {	"suction", 		12, 		TRUE	},
    {	"chop", 		13, 		TRUE	},
    {	"vorpal", 		14, 		TRUE	},
    {	"cleave", 		15, 		TRUE	},
    {	"wail", 		16, 		TRUE	},
    {	"", 			0, 		0	}
};


const	struct	flag_type	container_flags	[ ]	=
{
    {	"closeable", 		1, 		TRUE	},
    {	"pickproof", 		2, 		TRUE	},
    {	"closed", 		4, 		TRUE	},
    {	"locked", 		8, 		TRUE	},
    {	"", 			0, 		0	}
};


const struct flag_type portal_door_flags	[ ] =
{
    {	"closeable", 		1, 		TRUE	},
    {	"pickproof", 		2, 		TRUE	},
    {	"closed", 		4, 		TRUE	},
    {	"locked", 		8, 		TRUE	},
    {	"", 			0, 		0	}
};

const	struct	flag_type	portal_flags	[ ]	=
{
    {	"nocursed", 		1, 		TRUE	},
    {	"gowithchar", 		2, 		TRUE	},
    {	"random", 		4, 		TRUE	},
    {	"buggy", 		8, 		TRUE	},
    {	"", 			0, 		0	}
};

const	struct	flag_type	mana_flags	[ ]	=
{
    {	"mana_earth", 		MANA_EARTH, 	TRUE	},
    {	"mana_air", 		MANA_AIR, 	TRUE	},
    {	"mana_fire", 		MANA_FIRE, 	TRUE	},
    {	"mana_water", 		MANA_WATER, 	TRUE	},
    {	"", 			0, 		0	}
};

const	struct	flag_type	liquid_flags	[ ]	=
{
    {	"water", 		0, 		TRUE	},
    {	"beer", 		1, 		TRUE	},
    {	"wine", 		2, 		TRUE	},
    {	"ale", 			3, 		TRUE	},
    {	"dark-ale", 		4, 		TRUE	},
    {	"whisky", 		5, 		TRUE	},
    {	"lemonade", 		6, 		TRUE	},
    {	"firebreather", 	7, 		TRUE	},
    {	"local-specialty", 	8, 		TRUE	},
    {	"slime-mold-juice", 	9, 		TRUE	},
    {	"milk", 		10, 		TRUE	},
    {	"tea", 			11, 		TRUE	},
    {	"coffee", 		12, 		TRUE	},
    {	"blood", 		13, 		TRUE	},
    {	"salt-water", 		14, 		TRUE	},
    {	"cola", 		15, 		TRUE	},
    {	"white wine", 		16, 		TRUE	},
    {	"root beer", 		17, 		TRUE	},
    {	"champagne", 		18, 		TRUE	},
    {	"vodka", 		19, 		TRUE	},
    {	"absinth", 		20, 		TRUE	},
    {	"brandy", 		21, 		TRUE	},
    {	"schnapps", 		22, 		TRUE	},
    {	"orange juice", 	23, 		TRUE	},
    {	"sherry", 		24, 		TRUE	},
    {	"rum", 			25, 		TRUE	},
    {	"port", 		26, 		TRUE	},
    {	"holy water",		27,		TRUE	},
    {	"unholy water",		28,		TRUE	},
    {	"", 			0, 		0	}
};

// Veygoth - material types.  Would be better code if the
// MAT_WHATEVER definitions were used rather than integers.
const   struct  flag_type       material_flags          [ ]     =
{
    {	"undefined", 			0, 		TRUE	},
    {	"nonsubstantial", 		1, 		TRUE	},
    {	"flesh", 			2, 		TRUE	},
    {	"cloth", 			3, 		TRUE	},
    {	"bark", 			4, 		TRUE	},
    {	"soft wood", 			5, 		TRUE	},
    {	"hard wood", 			6, 		TRUE	},
    {	"glass", 			7, 		TRUE	},
    {	"crystal", 			8, 		TRUE	},
    {	"ceramic", 			9, 		TRUE	},
    {	"bone", 			10, 		TRUE	},
    {	"stone", 			11, 		TRUE	},
    {	"hide", 			12, 		TRUE	},
    {	"leather", 			13, 		TRUE	},
    {	"cured leather", 		14, 		TRUE	},
    {	"iron",		 		15, 		TRUE	},
    {	"steel", 			16, 		TRUE	},
    {	"brass",	 		17, 		TRUE	},
    {	"mithril", 			18, 		TRUE	},
    {	"adamantium", 			19, 		TRUE	},
    {	"bronze", 			20, 		TRUE	},
    {	"copper", 			21, 		TRUE	},
    {   "silver",                       22,		TRUE    },
    {	"electrum", 			23, 		TRUE	},
    {	"gold", 			24, 		TRUE	},
    {	"platinum", 			25, 		TRUE	},
    {	"gem", 				26, 		TRUE	},
    {	"diamond",			27, 		TRUE	},
    {	"paper",			28, 		TRUE	},
    {	"parchment", 			29, 		TRUE	},
    {	"leaves", 			30, 		TRUE	},
    {	"ruby", 			31, 		TRUE	},
    {	"emerald", 			32, 		TRUE	},
    {	"sapphire", 			33, 		TRUE	},
    {	"ivory", 			34, 		TRUE	},
    {	"dragonscale", 			35, 		TRUE	},
    {	"obsidian", 			36, 		TRUE	},
    {	"granite", 			37, 		TRUE	},
    {	"marble", 			38, 		TRUE	},
    {	"limestone", 			39, 		TRUE	},
    {	"liquid", 			40, 		TRUE	},
    {	"bamboo", 			41, 		TRUE	},
    {	"reeds", 			42, 		TRUE	},
    {	"hemp", 			43, 		TRUE	},
    {	"glassteel", 			44, 		TRUE	},
    {	"eggshell", 			45, 		TRUE	},
    {	"chitinous", 			46, 		TRUE	},
    {	"reptile scale",		47, 		TRUE	},
    {	"generic food",			48, 		TRUE	},
    {	"rubber", 			49, 		TRUE	},
    {	"feather", 			50, 		TRUE	},
    {	"", 				0,		0	}
};

const	struct	flag_type	mprog_type_flags	[ ]	=
{
    {	"in_file_prog", 	IN_FILE_PROG, 		TRUE	},
    {	"act_prog", 		ACT_PROG, 		TRUE	},
    {	"speech_prog", 		SPEECH_PROG, 		TRUE	},
    {	"rand_prog", 		RAND_PROG, 		TRUE	},
    {	"fight_prog", 		FIGHT_PROG, 		TRUE	},
    {	"hitprcnt_prog", 	HITPRCNT_PROG, 		TRUE	},
    {	"death_prog", 		DEATH_PROG, 		TRUE	},
    {	"entry_prog", 		ENTRY_PROG, 		TRUE	},
    {	"greet_prog", 		GREET_PROG, 		TRUE	},
    {	"all_greet_prog", 	ALL_GREET_PROG, 	TRUE	},
    {	"give_prog", 		GIVE_PROG, 		TRUE	},
    {	"bribe_prog", 		BRIBE_PROG, 		TRUE	},
    {	"", 			0, 			0	}
};


const	struct	flag_type	clan_flags	[ ]	=
{
    {	"clan",			CLAN_CLAN,	TRUE	},
    {	"order",		CLAN_ORDER,	TRUE	},
    {	"guild",		CLAN_GUILD,	TRUE	},
    {	"",			0,		0	}
};


const	struct	flag_type	rank_flags	[ ]	=
{
    {	"exiled",		RANK_EXILED,	TRUE	},
    {	"parole",		RANK_PAROLE,	TRUE	},
    {	"normal",		RANK_NORMAL,	TRUE	},
    {	"senior",		RANK_SENIOR,	TRUE	},
    {	"officer",		RANK_OFFICER,	TRUE	},
    {	"deputy",		RANK_DEPUTY,	TRUE	},
    {	"leader",		RANK_LEADER,	TRUE	},
    {	"",			0,		0	}
};

// Converted to bitvectors -- Veygoth
struct	bitvector_flag_type	room_flags	[ ]	=
{
    {   "airystarshell",	&ROOM_AIRY_STARSHELL,   TRUE    },
    {	"bank",	 		&ROOM_BANK, 		TRUE	},
    {   "dockable", 		&ROOM_DOCKABLE,         TRUE    },
    {   "dark",                 &ROOM_DARK,             TRUE    },
    {   "earthenstarshell",	&ROOM_EARTHEN_STARSHELL,TRUE    },
    {   "fierystarshell",  	&ROOM_FIERY_STARSHELL,  TRUE    },
    {   "guildroom",		&ROOM_GUILDROOM,  	TRUE    },
    {   "heal",                 &ROOM_HEAL,             TRUE    },
    {   "hypnoticpattern",	&ROOM_HYPNOTIC_PATTERN, TRUE    },
    {	"indoors", 		&ROOM_INDOORS, 		TRUE	},
    {	"inn",	 		&ROOM_INN, 		TRUE	},
    {   "jail",			&ROOM_JAIL,             TRUE    },
    {   "magicdark",	       	&ROOM_MAGICDARK,        TRUE    },
    {   "magiclight",   	&ROOM_MAGICLIGHT,       TRUE    },
    {	"nomob", 		&ROOM_NO_MOB, 		TRUE	},
    {   "noheal",       	&ROOM_NO_HEAL,          TRUE    },
    {	"norecall", 		&ROOM_NO_RECALL, 	TRUE	},
    {   "noscan",           	&ROOM_NO_SCAN,          TRUE    },
    {   "nomagic",          	&ROOM_NO_MAGIC,         TRUE    },
    {   "nogate",         	&ROOM_NO_GATE,          TRUE    },
    {   "nosummon",		&ROOM_NO_SUMMON,        TRUE    },
    {   "noteleport",      	&ROOM_NO_TELEPORT,      TRUE    },
    {   "noprecipitation",	&ROOM_NO_PRECIP,        TRUE    },
    {	"private", 		&ROOM_PRIVATE, 		TRUE	},
    {	"pet_shop", 		&ROOM_PET_SHOP, 	TRUE	},
    {	"safe", 		&ROOM_SAFE, 		TRUE	},
    {	"solitary", 		&ROOM_SOLITARY, 	TRUE	},
    {	"silence", 		&ROOM_SILENT,	 	TRUE	},
    {   "singlefile",		&ROOM_SINGLE_FILE, 	TRUE    },
    {   "tunnel",   		&ROOM_TUNNEL,           TRUE    },
    {   "twilight",             &ROOM_TWILIGHT,         TRUE    },
    {   "underwater",		&ROOM_UNDERWATER,   	TRUE    },
    {   "waterystarshell",  	&ROOM_WATERY_STARSHELL, TRUE    },
    {	"", 			0,			0	}
};

// Converted to bitvectors -- Veygoth
struct	bitvector_flag_type	affect_flags	[ ]	=
{
    {   "absorbing",		&AFF_ABSORBING,		TRUE},
    {   "aware",		&AFF_AWARE,		TRUE},
    {   "awareskill",		&AFF_SKL_AWARE,		TRUE},
    {   "barkskin",             &AFF_BARKSKIN,          TRUE},
    {   "battle ecstasy",      	&AFF_BATTLE_ECSTASY,	TRUE},
    {	"bearhug", 		&AFF_BEARHUG, 		TRUE},
    {	"biofeedback", 		&AFF_BIOFEEDBACK,	TRUE},
    {	"blind", 		&AFF_BLIND, 		TRUE},
    {   "blur",                 &AFF_BLUR,		TRUE},
    {	"bound", 		&AFF_BOUND, 		TRUE},
    {   "camping",		&AFF_CAMPING,           TRUE},
    {	"cannibalizing", 	&AFF_CANNIBALIZING, 	TRUE},
    {   "casting",		&AFF_CASTING,           TRUE},
    {	"changeself", 		&AFF_CHANGE_SELF,	TRUE},
    {	"changesex", 		&AFF_CHANGE_SEX,	TRUE},
    {	"charm", 		&AFF_CHARM, 		TRUE},
    {   "climbing",             &AFF_CLIMBING,          TRUE},
    {	"coldshield", 		&AFF_COLDSHIELD,	TRUE},
    {	"complang", 		&AFF_COMP_LANG,		TRUE},
    {   "concealed",            &AFF_MINOR_INVIS,       TRUE},
    {   "cover",                &AFF_COVER,             TRUE},
    {	"curse", 		&AFF_CURSE, 		TRUE},
    {	"dazzle", 		&AFF_DAZZLE,		TRUE},
    {	"dazzled", 		&AFF_DAZZLED,		TRUE},
    {	"denyair",		&AFF_DENY_AIR,		TRUE},
    {	"denyearth",		&AFF_DENY_EARTH,	TRUE},
    {	"denyfire",		&AFF_DENY_FIRE,		TRUE},
    {	"denywater",		&AFF_DENY_WATER,	TRUE},
    {	"detectevil", 		&AFF_DETECT_EVIL, 	TRUE},
    {	"detecthidden", 	&AFF_DETECT_HIDDEN, 	TRUE},
    {	"detectinvis", 		&AFF_DETECT_INVIS,	TRUE},
    {	"detectmagic", 		&AFF_DETECT_MAGIC, 	TRUE},
    {	"detectundead", 	&AFF_DETECT_UNDEAD,	TRUE},
    {	"detectgood", 		&AFF_DETECT_GOOD, 	TRUE},
    {   "draining",		&AFF_DRAINING,          TRUE},
    {   "droppedprimary",	&AFF_DROPPED_PRIMARY,	TRUE},
    {   "droppedsecondary",	&AFF_DROPPED_SECOND,	TRUE},
    {   "drowning",		&AFF_DROWNING,          TRUE},
    {   "duergarhide",		&AFF_DUERGAR_HIDE,	TRUE},
    {   "ectoplasmic",  	&AFF_ECTOPLASMIC,	TRUE},
    {   "elementalsight",	&AFF_ELEM_SIGHT,	TRUE},
    {   "elementalform",	&AFF_ELEMENTAL_FORM,	TRUE},
    {   "enlarge",              &AFF_ENLARGED,		TRUE},
    {	"faeriefire", 		&AFF_FAERIE_FIRE, 	TRUE},
    {	"famine", 		&AFF_FAMINE, 		TRUE},
    {	"farsee", 		&AFF_FARSEE, 		TRUE},
    {	"flaming", 		&AFF_FLAMING, 		TRUE},
    {	"flying", 		&AFF_FLYING, 		TRUE},
    {   "fourarms",             &AFF_FOUR_ARMS,         TRUE},
    {	"fumbledprimary", 	&AFF_FUMBLED_PRIMARY,	TRUE},
    {	"fumbledsecondary",	&AFF_FUMBLED_SECOND,	TRUE},
    {	"ghoul", 		&AFF_GHOUL, 		TRUE},
    {	"gills", 		&AFF_GILLS, 		TRUE},
    {	"grappling", 		&AFF_GRAPPLING,		TRUE},
    {	"grappled", 		&AFF_GRAPPLED, 		TRUE},
    {	"grspiritward", 	&AFF_GREATER_SPIRIT_WARD,TRUE},
    {	"grstornogs", 		&AFF_GREATER_SPHERES,	TRUE},
    {   "haste",                &AFF_HASTE,             TRUE},
    {	"hide", 		&AFF_HIDE, 		TRUE},
    {	"hold", 		&AFF_HOLD, 		TRUE},
    {	"holy sacrifice", 	&AFF_HOLY_SACRIFICE,	TRUE},
    {	"hunting", 		&AFF_HUNTING, 		TRUE},
    {	"holdingbreath", 	&AFF_HOLDING_BREATH,	TRUE},
    {	"incompetence", 	&AFF_INCOMPETENCE,	TRUE},
    {	"inertial", 		&AFF_INFRARED, 		TRUE},
    {	"infrared", 		&AFF_INERTIAL_BARRIER,	TRUE},
    {	"intellectfort", 	&AFF_INTELLECT_FORTRESS,TRUE},
    {	"invisible", 		&AFF_INVISIBLE, 	TRUE},
    {   "isfleeing",       	&AFF_IS_FLEEING,	TRUE},
    {   "justicetracker",    	&AFF_JUSTICE_TRACKER,	TRUE},
    {   "ko",                   &AFF_KO,                TRUE},
    {   "layhandstimer",    	&AFF_LAYHANDS_TIMER,	TRUE},
    {   "levitate",             &AFF_LEVITATE,          TRUE},
    {   "looter",          	&AFF_LOOTER, 		TRUE},
    {   "mageflame",  		&AFF_MAGE_FLAME,	TRUE},
    {   "misdirection",		&AFF_MISDIRECTION,	TRUE},
    {   "majorglobe",    	&AFF_MAJOR_GLOBE,	TRUE},
    {   "majorphysical",        &AFF_MAJOR_PHYSICAL,    TRUE},
    {   "meditate",             &AFF_MEDITATE,          TRUE},
    {   "memorizing",           &AFF_MEMORIZING,        TRUE},
    {   "minorglobe",           &AFF_MINOR_GLOBE,       TRUE},
    {   "minorpara",		&AFF_MINOR_PARA,	TRUE},
    {	"multiclass", 		&AFF_MULTICLASS,	TRUE},
    {	"mute", 		&AFF_MUTE, 		TRUE},
    {	"neckbiting", 		&AFF_NECKBITING,	TRUE},
    {	"noimmolate", 		&AFF_NO_IMMOLATE,	TRUE},
    {	"nounmorph", 		&AFF_NO_UNMORPH,	TRUE},
    {	"nondetection",		&AFF_NON_DETECTION,	TRUE},
    {	"paladinaura", 		&AFF_PALADIN_AURA, 	TRUE},
    {	"passdoor", 		&AFF_PASS_DOOR, 	TRUE},
    {	"passwithouttrace",	&AFF_PASS_WITHOUT_TRACE,TRUE},
    {	"protacid",		&AFF_PROTECT_ACID, 	TRUE},
    {	"protcold",		&AFF_PROTECT_COLD, 	TRUE},
    {	"protevil",		&AFF_PROTECT_EVIL, 	TRUE},
    {	"protfire",		&AFF_PROTECT_FIRE, 	TRUE},
    {	"protgas",		&AFF_PROTECT_GAS, 	TRUE},
    {	"protgood",		&AFF_PROTECT_GOOD, 	TRUE},
    {	"protlightning",	&AFF_PROTECT_LIGHTNING,	TRUE},
    {	"prowess", 		&AFF_PROWESS,	 	TRUE},
    {	"plague",		&AFF_PLAGUE, 		TRUE},
    {	"plusone",		&AFF_PLUS_ONE, 		TRUE},
    {	"plustwo",		&AFF_PLUS_TWO, 		TRUE},
    {	"plusthree",		&AFF_PLUS_THREE,	TRUE},
    {	"plusfour",		&AFF_PLUS_FOUR,		TRUE},
    {	"plusfive",		&AFF_PLUS_FIVE,		TRUE},
    {	"poison", 		&AFF_POISON, 		TRUE},
    {	"polymorph", 		&AFF_POLYMORPH,		TRUE},
    {   "reduce",               &AFF_REDUCED,		TRUE},
    {	"regeneration", 	&AFF_REGENERATION, 	TRUE},
    {   "sacking",              &AFF_SACKING,           TRUE},
    {   "silver",               &AFF_SILVER,            TRUE},
    {   "scribing",             &AFF_SCRIBING,          TRUE},
    {	"sensefollower", 	&AFF_SENSE_FOLLOWER, 	TRUE},
    {	"sanctuary", 		&AFF_SANCTUARY, 	TRUE},
    {   "senselife",            &AFF_SENSE_LIFE,        TRUE},
    {   "shadow",               &AFF_SHADOW,            TRUE},
    {   "singing",              &AFF_SINGING,           TRUE},
    {	"sleep", 		&AFF_SLEEP, 		TRUE},
    {   "slowness",             &AFF_SLOWNESS,          TRUE},
    {   "slowpoison",           &AFF_SLOW_POISON,       TRUE},
    {	"sneak", 		&AFF_SNEAK, 		TRUE},
    {   "soulshield",           &AFF_SOULSHIELD,        TRUE},
    {   "spiritward",           &AFF_SPIRIT_WARD,       TRUE},
    {   "stoneskin",            &AFF_STONESKIN,         TRUE},
    {	"stornogspheres", 	&AFF_STORNOG_SPHERES, 	TRUE},
    {   "stunned",              &AFF_STUNNED,           TRUE},
    {	"summonmounttimer",	&AFF_SUMMON_MOUNT_TIMER,TRUE},
    {   "swimming",             &AFF_SWIMMING,          TRUE},
    {   "tensersdisc",          &AFF_TENSORS_DISC,      TRUE},
    {   "throatcrush",       	&AFF_THROAT_CRUSH,	TRUE},
    {   "towerironwill",    	&AFF_TOWER_OF_IRON_WILL,TRUE},
    {   "track",            	&AFF_TRACK, 		TRUE},
    {   "tracking",             &AFF_TRACKING,          TRUE},
    {   "ultravision",		&AFF_ULTRAVISION,	TRUE},
    {   "underdarkvis",         &AFF_UNDERDARK_VIS,     TRUE},
    {   "underwater",		&AFF_UNDERWATER,	TRUE},
    {	"vacancy", 		&AFF_VACANCY,		TRUE},
    {	"vampbite", 		&AFF_VAMP_BITE, 	TRUE},
    {	"vampform", 		&AFF_VAMPIRE_FORM, 	TRUE},
    {	"vamptouch", 		&AFF_VAMP_TOUCH, 	TRUE},
    {   "",                     0,                      FALSE},
};

struct	bitvector_flag_type	item_flags	[ ]	=
{
    {	"antigood",		&ITEM_ANTI_GOOD,	TRUE},
    {	"antievil",		&ITEM_ANTI_EVIL,	TRUE},
    {	"antineutral",		&ITEM_ANTI_NEUTRAL,	TRUE},
//    {	"artifact",		&ITEM_ARTIFACT,		TRUE},
    {	"bless", 		&ITEM_BLESS, 		TRUE},
    {   "buried",               &ITEM_BURIED,           TRUE},
    {	"disarmed",		&ITEM_WAS_DISARMED,	TRUE},
    {	"evil", 		&ITEM_EVIL, 		TRUE},
    {	"enlarged", 		&ITEM_ENLARGED,		TRUE},
    {	"float",		&ITEM_FLOAT, 		TRUE},
    {	"glow", 		&ITEM_GLOW, 		TRUE},
    {	"goodonly", 		&ITEM_GOODONLY,		TRUE},
    {	"hum", 			&ITEM_HUM, 		TRUE},
    {	"inventory", 		&ITEM_INVENTORY,	TRUE},
    {	"invis", 		&ITEM_INVIS, 		TRUE},
    {	"levitates", 		&ITEM_LEVITATES,	TRUE},
    {	"lit", 			&ITEM_LIT, 		TRUE},
    {	"magic", 		&ITEM_MAGIC, 		TRUE},
    {	"nodrop",		&ITEM_NODROP, 		TRUE},
    {   "noshow",               &ITEM_NOSHOW,           TRUE},
    {   "nosell",               &ITEM_NOSELL,           TRUE},
    {	"noburn",		&ITEM_NOBURN, 		TRUE},
    {	"nolocate",		&ITEM_NOLOCATE,		TRUE},
    {	"noid",			&ITEM_NOID, 		TRUE},
    {	"norent", 		&ITEM_NORENT, 		TRUE},
    {	"nosummon",		&ITEM_NOSUMMON,		TRUE},
    {	"nosleep", 		&ITEM_NOSLEEP, 		TRUE},
    {	"nocharm", 		&ITEM_NOCHARM, 		TRUE},
    {	"reduced", 		&ITEM_REDUCED, 		TRUE},
    {	"secret",		&ITEM_SECRET, 		TRUE},
    {	"silver",		&ITEM_SILVER, 		TRUE},
    {	"poisoned",		&ITEM_POISONED, 	TRUE},
    {	"throwreturn",		&ITEM_THROW_RETURN,	TRUE},
    {	"throwoneroom",		&ITEM_THROW_ONEROOM,	TRUE},
    {	"throwtworooms",	&ITEM_THROW_TWOROOMS,	TRUE},
    {	"transient",		&ITEM_TRANSIENT,	TRUE},
    {	"twohanded", 		&ITEM_TWOHANDED,	TRUE},
    {	"wholebody",		&ITEM_WHOLEBODY,	TRUE},
    {	"wholehead", 		&ITEM_WHOLEHEAD, 	TRUE},
    {   "",                     0,                      FALSE},
};

struct wear_type
{
    int	wear_loc;
    int	wear_bit;
};

const	struct	wear_type	wear_table_olc	[ ]	=
{
    {	WEAR_NONE, 		ITEM_TAKE		},
    {	WEAR_FINGER_L, 		ITEM_WEAR_FINGER	},
    {	WEAR_FINGER_R, 		ITEM_WEAR_FINGER	},
    {	WEAR_NECK_1, 		ITEM_WEAR_NECK		},
    {	WEAR_NECK_2, 		ITEM_WEAR_NECK		},
    {	WEAR_BODY, 		ITEM_WEAR_BODY		},
    {	WEAR_HEAD, 		ITEM_WEAR_HEAD		},
    {	WEAR_LEGS, 		ITEM_WEAR_LEGS		},
    {	WEAR_FEET, 		ITEM_WEAR_FEET		},
    {	WEAR_HANDS, 		ITEM_WEAR_HANDS		},
    {	WEAR_ARMS, 		ITEM_WEAR_ARMS		},
    {	WEAR_ABOUT, 		ITEM_WEAR_ABOUT		},
    {	WEAR_WAIST, 		ITEM_WEAR_WAIST		},
    {	WEAR_WRIST_L, 		ITEM_WEAR_WRIST		},
    {	WEAR_WRIST_R, 		ITEM_WEAR_WRIST		},
    {	WEAR_HAND, 		ITEM_WIELD		},
    {   WEAR_EYES,              ITEM_WEAR_EYES          },
    {   WEAR_FACE,              ITEM_WEAR_FACE          },
    {   WEAR_BADGE,             ITEM_BADGE              },
    {   WEAR_QUIVER,            ITEM_QUIVER             },
    {   WEAR_EAR_L,             ITEM_WEAR_EAR           },
    {   WEAR_EAR_R,             ITEM_WEAR_EAR           },
    {   WEAR_ONBACK,            ITEM_WEAR_ONBACK        },
    {   WEAR_BELT_1,            ITEM_ATTACH_BELT        },
    {   WEAR_BELT_2,            ITEM_ATTACH_BELT        },
    {   WEAR_TAIL,              ITEM_TAIL               },
    {   WEAR_HORNS,             ITEM_HORNS              },
    {	NO_FLAG, 		NO_FLAG			}
};

int wear_loc( int bits, int count )
{
    int flag;

    last_fun( "wear_loc");

    for ( flag = 0; wear_table_olc[flag].wear_bit != NO_FLAG; flag++ )
	if ( IS_SET( bits, wear_table_olc[flag].wear_bit ) && --count < 1 )
	    return wear_table_olc[flag].wear_loc;

    return NO_FLAG;
}


int wear_bit( int loc )
{
    int flag;

    last_fun( "wear_bit");

    for ( flag = 0; wear_table_olc[flag].wear_loc != NO_FLAG; flag++ )
	if ( loc == wear_table_olc[flag].wear_loc )
	    return wear_table_olc[flag].wear_bit;

    return 0;
}

// only allows one bitvector per affect data.
// this prevents us from adding wacky multiple affects.
// automatically sets all other vector values to 0. -- Veygoth
void set_bitvector( AFFECT_DATA *af, BITVECT_DATA bvect )
{
    int count;

    last_fun( "set_bitvector");

    for( count = 0;count < NUM_AFFECT_VECTORS; count++ )
    {
      if( count == bvect.group )
        af->bitvector[count] = bvect.vector;
      else
        af->bitvector[count] = 0;
    }

    return;
}

// Checks to see if a bitvector is set
bool get_bitvector( AFFECT_DATA *af, BITVECT_DATA bvect )
{
    last_fun( "get_bitvector");

    if( IS_SET(af->bitvector[bvect.group], bvect.vector ))
      return TRUE;

    return FALSE;
}

bool IS_AFFECTED( CHAR_DATA *ch, BITVECT_DATA bvect )
{
//    last_fun( "IS_AFFECTED");
//   This is commented out 'cause it's too spammy... I don't see how
// it could crash the mud either. - Lohrr

    if( !ch || ch->deleted )
        return FALSE;

    if( IS_SET( ch->affected_by[bvect.group], bvect.vector))
        return TRUE;

    return FALSE;
}

bool is_affected( CHAR_DATA *ch, BITVECT_DATA bvect )
{
    AFFECT_DATA *paf;

    last_fun( "is_affected");
    if ( IS_AFFECTED( ch, bvect ) )
            return TRUE;
    for ( paf = ch->affected; paf; paf = paf->next ) {
        if( paf->deleted )
            continue;
        if ( IS_SET( paf->bitvector[bvect.group], bvect.vector))
            return TRUE;
    }
    return FALSE;
}

// These three functions are only used for affect manipulation - Veygoth
void SET_AFF_BIT( CHAR_DATA *ch, BITVECT_DATA bvect )
{
    last_fun( "SET_AFF_BIT");

    if ( !ch ) {
        bug(" SET_AFF_BIT: Null ch", 0);
        return;
    }
        ch->affected_by[bvect.group] |= bvect.vector;
        return;
}

void REMOVE_AFF_BIT( CHAR_DATA *ch, BITVECT_DATA bvect )
{
    last_fun( "REMOVE_AFF_BIT");

        ch->affected_by[bvect.group] &= ~(bvect.vector);
        return;
}

void TOGGLE_AFF_BIT( CHAR_DATA *ch, BITVECT_DATA bvect )
{
    last_fun( "TOGGLE_AFF_BIT");

        ch->affected_by[bvect.group] ^= bvect.vector;
        return;
}

bool IS_OBJ_STAT( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "IS_OBJ_STAT");

	if ( IS_SET( obj->extra_flags[bvect.group], bvect.vector ) )
            return TRUE;
        return FALSE;
}

void SET_OBJ_STAT( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "SET_OBJ_STAT");

        SET_BIT( obj->extra_flags[bvect.group], bvect.vector );
        return;
}

void REMOVE_OBJ_STAT( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "REMOVE_OBJ_STAT");

        REMOVE_BIT( obj->extra_flags[bvect.group], bvect.vector );
        return;
}

bool IS_OBJ_AFF( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "IS_OBJ_AFF");

    if ( IS_SET( obj->affected_by[bvect.group], bvect.vector ) )
        return TRUE;

    return FALSE;
}

void SET_OBJ_AFF( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "SET_OBJ_AFF");

    SET_BIT( obj->affected_by[bvect.group], bvect.vector );
    return;
}

void REMOVE_OBJ_AFF( OBJ_DATA *obj, BITVECT_DATA bvect )
{
    last_fun( "REMOVE_OBJ_AFF");

    REMOVE_BIT( obj->affected_by[bvect.group], bvect.vector );
    return;
}

bool IS_ROOM( ROOM_INDEX_DATA *room, BITVECT_DATA bvect )
{
    last_fun( "IS_ROOM");
    if( !room || !room->room_flags )
    {
        bug( "IS_ROOM: NULL room or NULL room flags", 0 );
        return FALSE;
    }
    if ( IS_SET( room->room_flags[bvect.group], bvect.vector ) )
      return TRUE;
    return FALSE;
}

void REMOVE_ROOM_BIT( ROOM_INDEX_DATA *room, BITVECT_DATA bvect )
{
    last_fun( "REMOVE_ROOM_BIT");

    REMOVE_BIT( room->room_flags[bvect.group], bvect.vector );
    return;
}

void SET_ROOM_BIT( ROOM_INDEX_DATA *room, BITVECT_DATA bvect )
{
    last_fun( "SET_ROOM_BIT");

    SET_BIT( room->room_flags[bvect.group], bvect.vector );
    return;
}
