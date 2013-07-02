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

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


int     last_fun_count = 0;
bool    last_funs_init = FALSE;
char    *last_funs[MAX_LAST_FUNS];
int     last_com_count = 0;
bool    last_coms_init = FALSE;
char    *last_coms[MAX_LAST_COMS];
char    lbuf[100];

/*
 * Local functions.
 */
void    affect_modify		args( ( CHAR_DATA *ch, AFFECT_DATA *paf,
				      bool fAdd ) );
void 	move_light 		args( ( CHAR_DATA *ch, bool entering ) );



/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{

    if ( ch == NULL )
    {
        bug( "Get_trust: NULL ch", 0 );
        return -1;
    }

//    sprintf( lbuf, "get_trust for %s", ch->name );
//    last_fun( lbuf );

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

//    sprintf( lbuf, "get_trust for %s, #1", ch->name );
//    last_fun( lbuf );

    if ( ch->trust != 0 )
	return ch->trust;

    if ( IS_NPC( ch ) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;
    else {
//    sprintf( lbuf, "get_trust for %s, #2", ch->name );
//    last_fun( lbuf );


	return ch->level;
    }
}

/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{

    last_fun( "get_age");
    if( IS_NPC( ch ))
      return race_table[ch->race].base_age + ( ch->played + (int) ( current_time - ch->logon ) ) / 428400;
    else
      return (int)(race_table[ch->race].base_age + (( current_time - ch->pcdata->created) / 428400 ));

    /* 428400 assumes 30 secs/mud hour * 24 hours/day * 35 days/month *
       17 months/year - Kahn */
}

/*
 * Retrieve character's current strength.
 */
int get_curr_str( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_str");
    mod  = race_table[ch->race].str_mod;
    plus = ch->mod_str;
    max  = race_table[ch->race].str_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_str;
      plus += ch->pcdata->maxmod_str;
      max += (ch->pcdata->racemod_str + ch->pcdata->maxmod_str);
    }

    return URANGE( 1, (ch->perm_str * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum strength.
 */
int get_max_str( CHAR_DATA *ch )
{
    last_fun( "get_max_str");
    
    if( !IS_NPC( ch ))
      return race_table[ch->race].str_mod + ch->pcdata->racemod_str + ch->pcdata->maxmod_str;
    else
      return race_table[ch->race].str_mod;
}

/*
 * Retrieve character's current intelligence.
 */
int get_curr_int( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_int");
    mod  = race_table[ch->race].int_mod;
    plus = ch->mod_int;
    max  = race_table[ch->race].int_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_int;
      plus += ch->pcdata->maxmod_int;
      max += (ch->pcdata->racemod_int + ch->pcdata->maxmod_int);
    }

    return URANGE( 1, (ch->perm_int * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum intelligence.
 */
int get_max_int( CHAR_DATA *ch )
{
    last_fun( "get_max_int");

    if( !IS_NPC( ch ))
      return race_table[ch->race].int_mod + ch->pcdata->racemod_int + ch->pcdata->maxmod_int;
    else
      return race_table[ch->race].int_mod;
}

/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_wis");
    mod  = race_table[ch->race].wis_mod;
    plus = ch->mod_wis;
    max  = race_table[ch->race].wis_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_wis;
      plus += ch->pcdata->maxmod_wis;
      max += (ch->pcdata->racemod_wis + ch->pcdata->maxmod_wis);
    }

    return URANGE( 1, (ch->perm_wis * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum wisdom.
 */
int get_max_wis( CHAR_DATA *ch )
{
    last_fun( "get_max_wis");

    if( !IS_NPC( ch ))
      return race_table[ch->race].wis_mod + ch->pcdata->racemod_wis + ch->pcdata->maxmod_wis;
    else
      return race_table[ch->race].wis_mod;
}

/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_dex");
    mod  = race_table[ch->race].dex_mod;
    plus = ch->mod_dex;
    max  = race_table[ch->race].dex_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_dex;
      plus += ch->pcdata->maxmod_dex;
      max += (ch->pcdata->racemod_dex + ch->pcdata->maxmod_dex);
    }

    return URANGE( 1, (ch->perm_dex * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum dexterity.
 */
int get_max_dex( CHAR_DATA *ch )
{
    last_fun( "get_max_dex");

    if( !IS_NPC( ch ))
      return race_table[ch->race].dex_mod + ch->pcdata->racemod_dex + ch->pcdata->maxmod_dex;
    else
      return race_table[ch->race].dex_mod;
}

/*
 * Retrieve character's current constitution
 */
int get_curr_con( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_con");

    if( !ch )
    {
        bug( "get_curr_con: NULL ch ", 0 );
        return 0;
    }

    if( ch->deleted )
    {
        bug( "get_curr_con: deleted ch (%d) ", (int)ch );
        return 0;
    }

    mod  = race_table[ch->race].con_mod;
    plus = ch->mod_con;
    max  = race_table[ch->race].con_mod;

    if( !IS_NPC( ch ) && ch->pcdata )
    {
      mod += ch->pcdata->racemod_con;
      plus += ch->pcdata->maxmod_con;
      max += (ch->pcdata->racemod_con + ch->pcdata->maxmod_con);
    }

    return URANGE( 1, (ch->perm_con * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum constitution
 */
int get_max_con( CHAR_DATA *ch )
{
    last_fun( "get_max_con");

    if( !IS_NPC( ch ))
      return race_table[ch->race].con_mod + ch->pcdata->racemod_con + ch->pcdata->maxmod_con;
    else
      return race_table[ch->race].con_mod;
}

/*
 * Retrieve character's current agility - Veygoth.
 */
int get_curr_agi( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_agi");
    mod  = race_table[ch->race].agi_mod;
    plus = ch->mod_agi;
    max  = race_table[ch->race].agi_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_agi;
      plus += ch->pcdata->maxmod_agi;
      max += (ch->pcdata->racemod_agi + ch->pcdata->maxmod_agi);
    }

    return URANGE( 1, (ch->perm_agi * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum agility - Veygoth.
 */
int get_max_agi( CHAR_DATA *ch )
{
    last_fun( "get_max_agi");

    if( !IS_NPC( ch ))
      return race_table[ch->race].agi_mod + ch->pcdata->racemod_agi + ch->pcdata->maxmod_agi;
    else
      return race_table[ch->race].agi_mod;
}

/*
 * Retrieve character's current charisma - Veygoth.
 */
int get_curr_cha( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_cha");
    mod  = race_table[ch->race].cha_mod;
    plus = ch->mod_cha;
    max  = race_table[ch->race].cha_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_cha;
      plus += ch->pcdata->maxmod_cha;
      max += (ch->pcdata->racemod_cha + ch->pcdata->maxmod_cha);
    }

    return URANGE( 1, (ch->perm_cha * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum charisma - Veygoth.
 */
int get_max_cha( CHAR_DATA *ch )
{
    last_fun( "get_max_cha");

    if( !IS_NPC( ch ))
      return race_table[ch->race].cha_mod + ch->pcdata->racemod_cha + ch->pcdata->maxmod_cha;
    else
      return race_table[ch->race].cha_mod;
}

/*
 * Retrieve character's current power - Veygoth.
 */
int get_curr_pow( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_pow");
    mod  = race_table[ch->race].pow_mod;
    plus = ch->mod_pow;
    max  = race_table[ch->race].pow_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_pow;
      plus += ch->pcdata->maxmod_pow;
      max += (ch->pcdata->racemod_pow + ch->pcdata->maxmod_pow);
    }

    return URANGE( 1, (ch->perm_pow * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum power - Veygoth.
 */
int get_max_pow( CHAR_DATA *ch )
{
    last_fun( "get_max_pow");

    if( !IS_NPC( ch ))
      return race_table[ch->race].pow_mod + ch->pcdata->racemod_pow + ch->pcdata->maxmod_pow;
    else
      return race_table[ch->race].pow_mod;
}

/*
 * Retrieve character's current luck - Veygoth.
 */
int get_curr_luk( CHAR_DATA *ch )
{
    int mod;
    int plus;
    int max;

    last_fun( "get_curr_luk");
    mod  = race_table[ch->race].luk_mod;
    plus = ch->mod_luk;
    max  = race_table[ch->race].luk_mod;

    if( !IS_NPC( ch ))
    {
      mod += ch->pcdata->racemod_luk;
      plus += ch->pcdata->maxmod_luk;
      max += (ch->pcdata->racemod_luk + ch->pcdata->maxmod_luk);
    }

    return URANGE( 1, (ch->perm_luk * mod / 100 ) + plus, max );
}

/*
 * Retrieve character's maximum luck - Veygoth.
 */
int get_max_luk( CHAR_DATA *ch )
{
    last_fun( "get_max_luk");

    if( !IS_NPC( ch ))
      return race_table[ch->race].luk_mod + ch->pcdata->racemod_luk + ch->pcdata->maxmod_luk;
    else
      return race_table[ch->race].luk_mod;
}

/*
 * Retrieve character's current hitroll for given weapon location
 */
int get_hitroll( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA    *wield;
    OBJ_DATA    *other_wield;
    AFFECT_DATA *paf;
    int          other_wpn;
    int          hitroll;

    last_fun( "get_hitroll");
    if ( wpn == WEAR_HAND)
        other_wpn = WEAR_HAND_2;
    else if ( wpn == WEAR_HAND_2 )
        other_wpn = WEAR_HAND;
    else
    {
        char buf [ MAX_STRING_LENGTH ];
        sprintf( buf, "get_hitroll: Invalid weapon location %d on %s.",
		wpn, ch->name );
        bug( buf, 0 );
        return 0;
    }

    hitroll = ch->hitroll + str_app[get_curr_str( ch )].tohit;

    if( ch->class == CLASS_MONK || ch->class == CLASS_MYSTIC )
    {
      int count;
      hitroll += monk_stances[ch->pcdata->stance].hit;
      for( count = 0; count < 5; count++ )
      {
        if( ch->level >= monk_stances[ch->pcdata->stance].hitplus[count] )
          hitroll++;
      }
    }

    wield = get_eq_char( ch, wpn );
    other_wield = get_eq_char( ch, other_wpn );

    if( ch->class == CLASS_ANTIPALADIN && wield && wield->item_type == TYPE_WEAPON
        && IS_OBJ_STAT( wield, ITEM_TWOHANDED ) )
           hitroll += ch->level / 9;

    if( ch->class == CLASS_PALADIN && wield && wield->item_type == TYPE_WEAPON
        && IS_OBJ_STAT( wield, ITEM_TWOHANDED ) )
           hitroll += ch->level / 9;

    if ( ch->class == CLASS_RANGER && wield && wield->item_type == TYPE_WEAPON
        && other_wield && other_wield->item_type == TYPE_WEAPON)
        hitroll += ch->level / 9;

    if ( !( other_wield = get_eq_char( ch, other_wpn ) ) )
        return UMIN( hitroll, ch->level );

    if( other_wield->item_type != TYPE_WEAPON )
        return UMIN( hitroll, ch->level );

    for( paf = other_wield->pIndexData->affected; paf; paf = paf->next )
        if ( paf->location == APPLY_HITROLL )
            hitroll -= paf->modifier;
    for( paf = other_wield->affected; paf; paf = paf->next )
        if ( paf->location == APPLY_HITROLL )
            hitroll -= paf->modifier;

    return UMIN( hitroll, ch->level );
}



/*
 * Retrieve character's current damroll for given weapon location
 */
int get_damroll( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA    *wield;
    OBJ_DATA    *other_wield;
    AFFECT_DATA *paf;
    int          other_wpn;
    int          damroll;

    last_fun( "get_damroll");
    if ( wpn == WEAR_HAND)
        other_wpn = WEAR_HAND_2;
    else if ( wpn == WEAR_HAND_2 )
        other_wpn = WEAR_HAND;
    else
    {
        char buf [ MAX_STRING_LENGTH ];
        sprintf( buf, "get_damroll: Invalid weapon location %d on %s.",
		wpn, ch->name );
        bug( buf, 0 );
        return 0;
    }

    damroll = ch->damroll + str_app[get_curr_str( ch )].todam;

    if( ch->class == CLASS_MONK || ch->class == CLASS_MYSTIC )
    {
      int count;
      damroll += monk_stances[ch->pcdata->stance].dam;
      for( count = 0; count < 5; count++ )
      {
        if( ch->level >= monk_stances[ch->pcdata->stance].damplus[count] )
          damroll++;
      }
    }

    wield = get_eq_char( ch, wpn );

    if( ch->class == CLASS_ANTIPALADIN && wield && wield->item_type == TYPE_WEAPON
        && IS_OBJ_STAT( wield, ITEM_TWOHANDED ) && wield->value[3] == DAM_SLASH )
           damroll += ch->level / 9;

    if( ch->class == CLASS_PALADIN && wield && wield->item_type == TYPE_WEAPON
        && IS_OBJ_STAT( wield, ITEM_TWOHANDED ) && wield->value[3] == DAM_SLASH )
           damroll += ch->level / 9;

    if ( !( other_wield = get_eq_char( ch, other_wpn ) ) )
        return UMIN( damroll, ch->level );

    if( other_wield->item_type != TYPE_WEAPON )
        return UMIN( damroll, ch->level );

    for( paf = other_wield->pIndexData->affected; paf; paf = paf->next )
        if ( paf->location == APPLY_DAMROLL )
            damroll -= paf->modifier;
    for( paf = other_wield->affected; paf; paf = paf->next )
        if ( paf->location == APPLY_DAMROLL )
            damroll -= paf->modifier;

    return UMIN( damroll, ch->level );
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    last_fun( "can_carry_n");
    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return 1000;

//    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
//	return 0;

    return get_curr_dex( ch ) / 11 + 3;
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    last_fun( "can_carry_w");
//    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
//	return 1000000;

// Let the pets carry stuff!
//    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
//	return 0;

    return str_app[get_curr_str( ch )].carry;
}



/*
 * See if a string is one of the names of an object.
 * New is_name sent in by Alander.
 */
bool is_name( const char *str, char *namelist )
{
    char name [MAX_INPUT_LENGTH * 4];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}


/*
 * See if a string is prefix of one of the names of an object.
 */
bool is_name_prefix( const char *str, char *namelist )
{
    char name [ MAX_INPUT_LENGTH ];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_prefix( str, name ) )
	    return TRUE;
    }
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    OBJ_DATA *wield2;
    char      buf [ MAX_STRING_LENGTH ];
    int       mod;
//    int       count;

    last_fun( "affect_modify");
    mod = paf->modifier;

/*
 * Testing if we can use affect pointer without setting affected_by in ch
* Lithos
    if ( fAdd )
    {
        for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
        {
	SET_BIT ( ch->affected_by[count], paf->bitvector[count] );
        }
    }
    else
    {
        for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
        {
	REMOVE_BIT ( ch->affected_by[count], paf->bitvector[count] );
        }
	switch ( paf->location )
	{
	case APPLY_RESISTANT:	REMOVE_BIT( ch->resistant, mod );	return;
	case APPLY_IMMUNE:	REMOVE_BIT( ch->immune, mod );		return;
	case APPLY_SUSCEPTIBLE:	REMOVE_BIT( ch->susceptible, mod );	return;
	}
	mod = 0 - mod;
    }
*/
    if ( !fAdd )
        mod = 0 - mod;
    switch ( paf->location )
    {
    default:
        sprintf( buf, "Affect_modify: unknown location %d on %s.",
		paf->location, ch->name );
	bug ( buf, 0 );
        /* Changed this to break from return.  - Lohrr */
	break;
    case APPLY_NONE:					break;
    case APPLY_STR:
	    ch->mod_str += mod;                         break;
    case APPLY_DEX:
	    ch->mod_dex += mod;                         break;
    case APPLY_INT:
	    ch->mod_int += mod;                         break;
    case APPLY_WIS:
	    ch->mod_wis += mod;                         break;
    case APPLY_CON:
    {
        int hit;
        hit = get_max_hit( ch );
        ch->mod_con += mod;
//        ch->hit = UMIN( get_max_hit( ch ), ch->hit + get_max_hit( ch ) - hit );
        ch->hit += get_max_hit( ch ) - hit;
        if (ch->hit > get_max_hit( ch ) ) ch->hit = get_max_hit( ch );
        break;
    }
    case APPLY_AGI:
	    ch->mod_agi += mod;                         break;
    case APPLY_CHA:
	    ch->mod_cha += mod;                         break;
    case APPLY_POW:
	    ch->mod_pow += mod;                         break;
    case APPLY_LUK:
	    ch->mod_luk += mod;                         break;
    case APPLY_SIZE:          ch->size                  += mod; break;
    case APPLY_SEX:           ch->sex                   += mod; break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->max_mana              += mod; break;
    case APPLY_HIT:
      if(!IS_NPC( ch ))
      {
           ch->pcdata->mod_hit               += mod;
      }
      else
      {
           ch->max_hit += mod;
      }
      ch->hit                   += mod;
      break;
    case APPLY_MOVE:          ch->max_move              += mod; break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor                 += mod; break;
    case APPLY_HITROLL:       ch->hitroll               += mod; break;
    case APPLY_DAMROLL:       ch->damroll               += mod; break;
    case APPLY_SAVING_PARA:   ch->saving_throw[0]       += mod; break;
    case APPLY_SAVING_ROD:    ch->saving_throw[1]       += mod; break;
    case APPLY_SAVING_PETRI:  ch->saving_throw[2]       += mod; break;
    case APPLY_SAVING_BREATH: ch->saving_throw[3]       += mod; break;
    case APPLY_SAVING_SPELL:  ch->saving_throw[4]       += mod; break;
    case APPLY_FIRE_PROT:  break;
    case APPLY_MAX_STR:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_str += mod;
         else
            ch->mod_str += mod;
         break;
    case APPLY_MAX_DEX:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_dex += mod;
         else
            ch->mod_dex += mod;
         break;
    case APPLY_MAX_INT:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_int += mod;
         else
            ch->mod_int += mod;
         break;
    case APPLY_MAX_WIS:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_wis += mod;
         else
            ch->mod_wis += mod;
         break;
    case APPLY_MAX_CON:
         if( !IS_NPC( ch )) {
            int hit;
            hit = get_max_hit( ch );
            ch->pcdata->maxmod_con += mod;
            ch->hit +=  get_max_hit( ch )- hit;
            if ( ch->hit > get_max_hit( ch ) ) ch->hit = get_max_hit( ch );
         }
         else
         {
            int hit;
            hit = get_max_hit( ch );
            ch->mod_con += mod;
            ch->hit +=  get_max_hit( ch )- hit;
            if ( ch->hit > get_max_hit( ch ) ) ch->hit = get_max_hit( ch );
            break;
         }
         break;
    case APPLY_MAX_AGI:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_agi += mod;
         else
            ch->mod_agi += mod;
         break;
    case APPLY_MAX_POW:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_pow += mod;
         else
            ch->mod_pow += mod;
         break;
    case APPLY_MAX_CHA:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_cha += mod;
         else
            ch->mod_cha += mod;
         break;
    case APPLY_MAX_LUK:
         if( !IS_NPC( ch ))
            ch->pcdata->maxmod_luk += mod;
         else
            ch->mod_luk += mod;
         break;
    case APPLY_RACE_STR:
    case APPLY_RACE_DEX:
    case APPLY_RACE_INT:
    case APPLY_RACE_WIS:
    case APPLY_RACE_CON:
    case APPLY_RACE_AGI:
    case APPLY_RACE_POW:
    case APPLY_RACE_CHA:
    case APPLY_RACE_LUK:
    case APPLY_CURSE:						break;
    case APPLY_RESISTANT:     SET_BIT( ch->resistant, mod );    break;
    case APPLY_IMMUNE:        SET_BIT( ch->immune, mod );       break;
    case APPLY_SUSCEPTIBLE:   SET_BIT( ch->susceptible, mod );  break;
    case APPLY_RACE:
         // Veygoth - added size adjustment so when you change races higher or lower
         // you are adjusted proportionally.  We don't just set the race to default
         // size because the person may be enlarged or reduced and will be hosed when
         // the enlarge or reduce affect wears off.
         ch->size -= (race_table[ch->race].size - race_table[(ch->race + mod)].size );
         ch->race                  += mod;
         break;
    }

    /* Remove the excess general stats */
    // took out class check, because evryone can go over max with Holy Sac - Lithos
//    if( ch->class != CLASS_ANTIPALADIN )
//      ch->hit  = UMIN( ch->hit, get_max_hit( ch ) );
//   else
    ch->hit  = UMIN( ch->hit, (get_max_hit( ch ) * 3 / 2 ) );
    ch->mana = UMIN( ch->mana, ch->max_mana );
    ch->move = UMIN( ch->move, ch->max_move );

    if ( IS_NPC( ch ) )
        return;

    /*
     * Check for PC weapon wielding.
     * Guard against recursion (for weapons with affects).
     * If more than one weapon, drop weapon 2 first, then recheck.
     * And yes, it does work.  :)  --- Thelonius (Monk)
     */
    if ( ( wield  = get_eq_char( ch, WEAR_HAND ) ) )
    {
	if ( ( wield2 = get_eq_char( ch, WEAR_HAND_2 ) ) )
	{
	    if ( ( ( get_obj_weight( wield ) + get_obj_weight( wield2 ) )
		  > str_app[get_curr_str( ch )].wield )
		|| !IS_SET( race_table[ ch->race ].race_abilities,
			   RACE_WEAPON_WIELD ) )
	    {
		static int depth;

		if ( depth == 0 )
		{
		    depth++;
		    act( "You drop $p&n because it is too heavy.", ch, wield2, NULL, TO_CHAR );
		    act( "$n&n drops $p&n.", ch, wield2, NULL, TO_ROOM );
		    obj_from_char( wield2 );
		    obj_to_room( wield2, ch->in_room );
		    depth--;
		}

	    }
	}
	else
	if ( ( get_obj_weight( wield ) > str_app[get_curr_str( ch )].wield )
	    || !IS_SET( race_table[ ch->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    static int depth;

	    if ( depth == 0 )
	    {
		depth++;
		act( "You drop $p&n because it is too heavy.", ch, wield, NULL, TO_CHAR );
		act( "$n&N drops $p&n.", ch, wield, NULL, TO_ROOM );
		obj_from_char( wield );
		obj_to_room( wield, ch->in_room );
		depth--;
	    }

	}
    }
    else if ( ( wield2 = get_eq_char( ch, WEAR_HAND_2 ) )
             && ( get_obj_weight( wield2 ) > str_app[get_curr_str( ch )].wield
		 || !IS_SET( race_table[ ch->race ].race_abilities,
			    RACE_WEAPON_WIELD ) ) )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act( "You drop $p&n because it is too heavy.", ch, wield2, NULL, TO_CHAR );
	    act( "$n&n drops $p&n.", ch, wield2, NULL, TO_ROOM );
	    obj_from_char( wield2 );
	    obj_to_room( wield2, ch->in_room );
	    depth--;
	}

    }

    return;
}

/*
 * Create an affect and set its parameters.
 *
 * This function was created to eliminate the use of 6 lines of code to initialize
 * an affect.  Parts of the affect could remain unitialized, thus causing potential
 * problems.  Calls to this functions require all necessary parameters and are
 * thus somewhat safer.
 *
 * -- Veygoth
 */
AFFECT_DATA create_affect( int skill, int spell, int song, int duration, int location, int modifier,
                    BITVECT_DATA bitvector )
{
    AFFECT_DATA af;

    last_fun( "create_affect");
    if( skill >= MAX_SKILL )
        skill = 0;
    if( spell >= MAX_SPELL )
        spell = 0;
    if( song >= MAX_SONG )
        song = 0;
    if( ( skill != 0 && ( spell != 0 || song != 0 ) )
		 || ( spell != 0 && song != 0 ) )
    {
        bug( "Create_affect called trying to create an affect that is both skill and spell for skill %d", skill );
        bug( " and spell %d.", spell );
        bug( "  or song %d.", song );
        skill = 0;
        spell = 0;
        song = 0;
    }

    if( location >= APPLY_HIGHEST )
    {
        bug( "Create_affect called with an affect location %d higher than max.", location );
        location = APPLY_NONE;
    }

    af.skill = skill;
    af.spell = spell;
    af.song = song;
    af.duration = duration;
    af.location = location;
    af.modifier = modifier;
    set_bitvector( &af, bitvector );

    return af;
}

/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    last_fun( "affect_to_char");
    paf_new		= new_affect();

    *paf_new		= *paf;
    paf_new->deleted    = FALSE;
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}

/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    last_fun( "affect_remove");
    if ( !ch->affected )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    affect_modify( ch, paf, FALSE );

    paf->deleted = TRUE;

    return;
}

/*
 * Strip all affects of a given sn.
 */
// This is now designed to take two arguments, a skill and a spell
// argument, in that order.  One of the values must be zero.
// -- Veygoth
void affect_strip( CHAR_DATA *ch, int skl, int spl, int song )
{
    AFFECT_DATA *paf;
    int sn;

    last_fun( "affect_strip");
    if(    ( skl  != 0 && !( spl == 0 && song == 0 ) ) 
        || ( spl  != 0 && !( skl == 0 && song == 0 ) ) 
        || ( song != 0 && !( skl == 0 && spl  == 0 ) ) )
    {
       bug( "Invalid call to affect_strip!  Must pass two null values!", 0 );
       return;
    }

    if( skl )
    {
        sn = skl;
    }
    else if( spl )
    {
        sn = spl;
    }
    else if( song )
    {
        sn = song;
    }
    else
    {
       bug( "Invalid call to affect_strip!  Must pass one non-null value!", 0 );
       return;
    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	if (   ( paf->skill == sn && skl  )
	    || ( paf->spell == sn && spl  )
	    || ( paf->song  == sn && song ) )
	{
	    affect_remove( ch, paf );
/* NEW: Added check to make sure paf is marked for deletion. */   
	    if ( !paf->deleted )
	    {
		bug( "Affect_strip: paf not deleted when it should be!", 0 );
        	paf->deleted = TRUE;
	    }
	}
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool has_affect( CHAR_DATA *ch, int skl, int spl, int song )
{
    AFFECT_DATA *paf;

    if( ( skl != 0 && ( spl != 0 || song != 0 ) )
	|| ( spl != 0 && song != 0 ) )
    {
       bug( "Invalid call to has_affect!  Must pass two null value!", 0 );
       return FALSE;
    }

    if( !(skl > 0) && !(spl > 0) && !(song > 0) )
    {
       bug( "Invalid call to has_affect!  Must pass one positive non-null value!", 0 );
       return FALSE;
    }

    if( skl )
    {
      for ( paf = ch->affected; paf; paf = paf->next )
      {
        if ( paf->deleted )
	    continue;
	if ( paf->skill == skl )
	    return TRUE;
      }
    }
    else if( spl )
    {
      for ( paf = ch->affected; paf; paf = paf->next )
      {
        if ( paf->deleted )
	    continue;
	if ( paf->spell == spl )
	    return TRUE;
      }
    }
    else
    {
      for ( paf = ch->affected; paf; paf = paf->next )
      {
        if ( paf->deleted )
	    continue;
	if ( paf->song == song )
	    return TRUE;
      }
    }

    return FALSE;
}



/*
 * Add or enhance an affect.
 */
// Checks to see if both skills and spells match.
// hopefully if one set matches nonzero values the
// other set will be zeros. - Veygoth
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;

    if ( !ch )
    {
        bug( "Affect_join: NULL ch", 0 );
        return;
    }

    last_fun( "affect_join");
    for ( paf_old = ch->affected; paf_old; paf_old = paf_old->next )
    {
        if ( paf_old->deleted )
	    continue;
	if ( paf_old->skill == paf->skill && paf_old->spell == paf->spell
               && paf_old->song == paf->song )
	{
	    paf->duration += paf_old->duration;
	    paf->modifier = UMAX( paf->modifier, paf_old->modifier);
	    affect_remove( ch, paf_old );
	    break;
	}
    }

    affect_to_char( ch, paf );
    return;
}



/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{

    last_fun( "char_from_room");
    if ( !ch->in_room )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if ( !IS_NPC( ch ) )
	--ch->in_room->area->nplayer;

    move_light(ch, FALSE);
    last_fun( "char_from_room:1");

    if ( ch == ch->in_room->people )
    {
	ch->in_room->people = ch->next_in_room;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
	{
	    if ( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }
	}

	if ( !prev )
	    bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;

    if ( ch->riding )
	char_from_room( ch->riding );
    return;
}



/*
 * Move a char into a room.
 *
 * Something about this function causes segmentation faults
 * in the line of code that says pRoomIndex->people = ch;
 *
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    last_fun( "char_to_room");
    if ( pRoomIndex == NULL)
    {
	bug( "Char_to_room: NULL pRoomIndex.", 0 );
	return;
    }

    if( ch == NULL )
    {
        bug( "Char_to_room: NULL ch.", 0 );
        return;
    }

    if( ch->deleted )
    {
        bug( "Called char_to_room with deleted CH!", 0 );
        return;
    }

    ch->in_room		= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    if( !ch->in_room->area )
    {
        bug( "Char_to_room: Room not in any area!", 0 );
        return;
    }

    if ( !IS_NPC( ch ) )
	++ch->in_room->area->nplayer;

    move_light(ch, TRUE);
    last_fun( "char_to_room");

    if ( ch->riding )
	char_to_room( ch->riding, pRoomIndex );

    return;
}

/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    last_fun( "obj_to_char");

    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    ch->carry_number	+= 1;
    ch->carry_weight	+= get_obj_weight( obj );

    if ( IS_OBJ_STAT ( obj, ITEM_LIT ) )
	ch->in_room->light++;
}

/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    last_fun( "obj_from_char");
    if ( !( ch = obj->carried_by ) )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
	ch->carrying = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by      = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= 1;
    ch->carry_weight	-= get_obj_weight( obj );

    if ( IS_OBJ_STAT ( obj, ITEM_LIT ) )
	ch->in_room->light--;
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    last_fun( "apply_ac");

    if ( obj->item_type != TYPE_ARMOR )
	return 0;

    if( obj->item_type == TYPE_SHIELD && ((iWear == WEAR_HAND) 
        || (iWear == WEAR_HAND_2)))
    {
	return     obj->value[0];
    }

    switch ( iWear )
    {
    case WEAR_BODY:     return     obj->value[0];
    case WEAR_HEAD:	return     obj->value[0];
    case WEAR_LEGS:	return     obj->value[0];
    case WEAR_FEET:	return     obj->value[0];
    case WEAR_HANDS:	return     obj->value[0];
    case WEAR_ARMS:	return     obj->value[0];
    case WEAR_FINGER_L:	return     obj->value[0];
    case WEAR_FINGER_R: return     obj->value[0];
    case WEAR_NECK_1:	return     obj->value[0];
    case WEAR_NECK_2:	return     obj->value[0];
    case WEAR_ABOUT:	return     obj->value[0];
    case WEAR_WAIST:	return     obj->value[0];
    case WEAR_WRIST_L:	return     obj->value[0];
    case WEAR_WRIST_R:	return     obj->value[0];
    case WEAR_EYES:     return     obj->value[0];
    case WEAR_FACE:     return     obj->value[0];
    case WEAR_HORNS:    return     obj->value[0];
    case WEAR_TAIL:     return     obj->value[0];
    case WEAR_EAR_L:    return     obj->value[0];
    case WEAR_EAR_R:	return     obj->value[0];
    case WEAR_BADGE:    return     obj->value[0];
    case WEAR_QUIVER:   return     obj->value[0];
    case WEAR_ONBACK:   return     obj->value[0];
    default:		return	   0;
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    last_fun( "get_eq_char");
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->deleted )
	    continue;
	if ( obj->wear_loc == iWear )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];
    int          aff;

    last_fun( "equip_char");

    if ( get_eq_char( ch, iWear ) )
    {
        sprintf( buf, "Equip_char: %s already equipped at %d.",
		ch->name, iWear );
	    warning( buf, 0 );
	    return;
    }

    if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
	|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
	|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	act( "You are zapped by $p&n and drop it.", ch, obj, NULL, TO_CHAR );
	act( "$n&n is zapped by $p&n and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	return;
    }

    ch->armor      	-= apply_ac( obj, iWear );
    obj->wear_loc	 = iWear;
    ch->carry_number--;

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf; paf = paf->next )
	affect_modify( ch, paf, TRUE );

    if ( obj->item_type == TYPE_LIGHT
	&& obj->value[2] != 0
	&& ch->in_room )
	++ch->in_room->light;
//dddd
    for( aff = 0;aff < NUM_AFFECT_VECTORS; aff++ )
        ch->affected_by[aff] = ch->affected_by[aff] | obj->affected_by[aff];

    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];
    int          aff;

    last_fun( "unequip_char");
    if ( obj->wear_loc == WEAR_NONE )
    {
        sprintf( buf, "Unequip_char: %s already unequipped with %d.",
		ch->name, obj->pIndexData->vnum );
	bug( buf, 0 );
	return;
    }

    ch->armor		+= apply_ac( obj, obj->wear_loc );
    obj->wear_loc	 = WEAR_NONE;
    ch->carry_number++;

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	affect_modify( ch, paf, FALSE );
    for ( paf = obj->affected; paf; paf = paf->next )
	affect_modify( ch, paf, FALSE );

    if ( obj->item_type == TYPE_LIGHT
	&& obj->value[2] != 0
	&& ch->in_room
	&& ch->in_room->light > 0 )
	--ch->in_room->light;

    for( aff = 0;aff < NUM_AFFECT_VECTORS; aff++ )
    {
        OBJ_DATA    *objw;
        /* Remove all affects of object being removed. */
        ch->affected_by[aff] = ch->affected_by[aff] & ~obj->affected_by[aff];
        /* Add any affects that are on the object being removed and
            another object that is worn. */
        for ( objw = ch->carrying; objw != NULL; objw = objw->next_content )
        {
            if ( !objw->deleted && objw->wear_loc != WEAR_NONE )
                ch->affected_by[aff] = ch->affected_by[aff] | objw->affected_by[aff];
        }
    }

    return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int       nMatch;

    last_fun( "count_obj_list");
    nMatch = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
        if ( obj->deleted )
	    continue;
	if ( obj->pIndexData == pObjIndex )
	    nMatch++;
    }

    return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;

    last_fun( "obj_from_room");
    if ( !( in_room = obj->in_room ) )
    {
	bug( "obj_from_room: Object not in a room.", 0 );
	return;
    }

    if ( obj == in_room->contents )
    {
	in_room->contents = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = in_room->contents; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }

    if ( IS_OBJ_STAT ( obj, ITEM_LIT ) )
	obj->in_room->light--;
    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    last_fun( "obj_to_room");
    if ( !pRoomIndex ) {
        bug("Obj_to_room: null pRoomIndex", 0);
        return;
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_MONEY_ONE 
         || obj->pIndexData->vnum == OBJ_VNUM_MONEY_SOME )
    {
	OBJ_DATA 	*obj2, *obj_next;
	for ( obj2 = pRoomIndex->contents; obj2; obj2 = obj_next )
	{
	    obj_next = obj2->next_content;

	    if ( obj2->deleted )
	        continue;

	    switch ( obj2->pIndexData->vnum )
	    {
	    case OBJ_VNUM_MONEY_ONE:
	    case OBJ_VNUM_MONEY_SOME:
	    	obj->pIndexData = get_obj_index( OBJ_VNUM_MONEY_SOME );
    		obj->name           = str_dup( obj->pIndexData->name );
    		obj->short_descr    = str_dup( obj->pIndexData->short_descr );
    		obj->description    = str_dup( obj->pIndexData->description );
		obj->value[0] += obj2->value[0];
		obj->value[1] += obj2->value[1];
		obj->value[2] += obj2->value[2];
		obj->value[3] += obj2->value[3];
		extract_obj( obj2 );
		break;
	    }
	}
    }

    obj->next_content		= pRoomIndex->contents;
    pRoomIndex->contents	= obj;
    obj->in_room		= pRoomIndex;
    obj->carried_by		= NULL;
    obj->in_obj			= NULL;
    if ( IS_OBJ_STAT ( obj, ITEM_LIT ) )
	obj->in_room->light++;

    return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    last_fun( "obj_to_obj");
    if ( obj_to->deleted )
    {
	bug( "Obj_to_obj:  Obj_to already deleted", 0 );
        return;
    }

    obj->next_content		= obj_to->contains;
    obj_to->contains		= obj;
    obj->in_obj			= obj_to;
    obj->in_room		= NULL;
    obj->carried_by		= NULL;

    for ( ; obj_to; obj_to = obj_to->in_obj )
    {
        if ( obj_to->deleted )
	    continue;
	if ( obj_to->carried_by )
	{
	    obj_to->carried_by->carry_weight += get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    last_fun( "obj_from_obj");
    if ( !( obj_from = obj->in_obj ) )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    if ( obj == obj_from->contains )
    {
	obj_from->contains = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from; obj_from = obj_from->in_obj )
    {
        if ( obj_from->deleted )
	    continue;
	if ( obj_from->carried_by )
	{
	    obj_from->carried_by->carry_weight -= get_obj_weight( obj );
	}
    }

    return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;
    extern bool      delete_obj;

    last_fun( "extract_obj");

    if ( obj->deleted )
    {
	bug( "Extract_obj:  Obj already deleted", 0 );
	return;
    }

    // Decrement global object count.
    obj->pIndexData->count--;

    if ( obj->in_room    )
	obj_from_room( obj );
    else if ( obj->carried_by )
	obj_from_char( obj );
    else if ( obj->in_obj     )
	obj_from_obj( obj  );

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
        obj_next = obj_content->next_content;
	if( obj_content->deleted )
	    continue;
	extract_obj( obj_content );
    }

    obj->deleted = TRUE;

    delete_obj   = TRUE;
    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    extern bool       delete_char;

    last_fun( "extract_char");

    if( !ch )
    {
        bug( "Extract_char: NULL ch.", 0 );
        return;
    }

    if ( !ch->in_room )
    {
	bug( "Extract_char: NULL in_room.", 0 );
	return;
    }

    if ( ch->fighting )
        stop_fighting( ch, TRUE );

    forget_all( ch );

    delete_attached_events( ch );

    /* Remove casting bit.  Dunno where this goes. - Lohrr */
    REMOVE_AFF_BIT( ch, AFF_CASTING );
    REMOVE_AFF_BIT( ch, AFF_SINGING );

    // Meaning they're dead for good or have left the game.
    if ( fPull )
    {
	char* name;

	if ( IS_NPC ( ch ) )
	    name = ch->short_descr;
	else
	    name = ch->name;

	die_follower( ch, name );
        if( ch->group_leader || ch->next_in_group )
            remove_from_group( ch, ch );
 
	/* Get rid of weapons _first_ 
	   - from Erwin Andreasen <erwin@pip.dknet.dk> */

	{
	    OBJ_DATA *obj, *obj2;

	    obj  = get_eq_char( ch, WEAR_HAND   );
	    obj2 = get_eq_char( ch, WEAR_HAND_2 );

	    if ( obj )
	        extract_obj( obj );

	    /* Now kill obj2 if it exists no matter if on body or floor */
	    if ( obj2 )
	        extract_obj( obj2 );

	}

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	      continue;
	    extract_obj( obj );
	}
     }
    
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch->reply == ch )
	    wch->reply = NULL;
        if ( !IS_NPC( wch ) && wch->pcdata->consent == ch )
        {
            wch->pcdata->consent = NULL;
            act( "You stop consenting $N&n.", wch, NULL, ch, TO_CHAR );
        }
        if ( !IS_NPC( wch ) && wch->pcdata->ignore == ch )
        {
            wch->pcdata->ignore = NULL;
            act( "You stop ignoring $N&n.", wch, NULL, ch, TO_CHAR );
        }
        if ( !IS_NPC( wch ) && wch->pcdata->guarding == ch )
        {
            wch->pcdata->guarding = NULL;
            act( "You stop guarding $N&n.", wch, NULL, ch, TO_CHAR );
        }
        if( wch->hating && wch->hating->who == ch ) 
           stop_hating( wch );
        if( wch->hunting && wch->hunting->who == ch ) 
           stop_hunting( wch );
        if( wch->fearing && wch->fearing->who == ch ) 
           stop_fearing( wch );
    }

    // This was placed *after* the act strings to be safe.
    for( wch = ch->in_room->people; wch; wch = wch->next_in_room )
    {
        if( wch == ch )
        {
            char_from_room( ch );
            break;
        }
    }

    if ( !fPull )
    {
        ROOM_INDEX_DATA *location;

        if( ch->level < 5 )
          newbie_eq_char( ch );

        if( !IS_NPC( ch ) && (ch->race < MAX_PC_RACE) && (ch->class < MAX_CLASS))
        {
          if( !(location = get_room_index( ch->pcdata->currhome )))
          {
            if ( !( location = get_room_index( repop_point[ch->race][ch->class] ) ) )
	    {
	      bug( "Starting room does not exist for class %d of player's race!  Calling char_to_room for altar.", ch->class );
  	      char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
	    }
          }
          else
          {
            char_to_room( ch, location );
          }
        }
	else
        {
	  if ( !( location = get_room_index( ROOM_VNUM_START ) ) )
	  {
	    bug( "Starting room %d does not exist!  Calling char_to_room for altar.", ROOM_VNUM_START );
	    char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
	  }
          else
	    char_to_room( ch, get_room_index( ROOM_VNUM_START) );
        }
	return;
    }

    if ( IS_NPC( ch ) )
    {
	--ch->pIndexData->count;
    }
    else
    {
        ch->pcdata->ignore = NULL;
        ch->pcdata->consent = NULL;
        ch->pcdata->guarding = NULL;
        ch->pcdata->condition[COND_FULL] = 48;
        ch->pcdata->condition[COND_THIRST] = 48;
        ch->pcdata->condition[COND_DRUNK] = 0;
    }

    if ( ch->desc && ch->desc->original )
	do_return( ch, "" );

    ch->deleted = TRUE;

    if ( ch->desc )
	ch->desc->character = NULL;

    delete_char = TRUE;
    return;
}

/*
 * Find a char in the room.
 */
// Veygoth - made "me" another alias for "self"
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "self" ) || !str_cmp( arg, "me" ) )
	return ch;
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) || ch->fly_level != rch->fly_level)
	    continue;

        // Targeting by race by Lohrr, NPC fix added by Veygoth
        // added a check for ch being NPC - Lithos
        if( !IS_NPC( rch ) && (race_table[ch->race].racewar_side !=
            race_table[rch->race].racewar_side) && !IS_IMMORTAL( ch )
	    && !IS_IMMORTAL ( rch ) && !IS_NPC( ch ) )
	{
	    if ( !is_name( arg, race_table[ rch->race ].name ) )
		continue;
	}
	else
        {
	    if ( !is_name( arg, rch->name ) )
		continue;
        }

	if ( ++count == number )
	    return rch;
    }

    return NULL;
}

/*
 * Find a char in an area.
 */
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *ach;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    if ( ( ach = get_char_room( ch, argument ) ) )
	return ach;

    number = number_argument( argument, arg );
    count  = 0;
    for ( ach = char_list; ach ; ach = ach->next )
    {
        if ( ach->deleted ) continue;
	if ( !ach->in_room || !ch->in_room
	    || ach->in_room->area != ch->in_room->area
	    || !can_see( ch, ach ))
                continue;

        // Targeting by race by Lohrr, NPC fix added by Veygoth
        if( !IS_NPC( ach ) && (race_table[ch->race].racewar_side !=
            race_table[ach->race].racewar_side) && !IS_IMMORTAL( ch ))
	{
	    if ( !is_name( arg, race_table[ ach->race ].name )  && 
                  !RACEWAR(ch, ach))
		continue;
	}
	else
        {
	    if ( !is_name( arg, ach->name ) )
		continue;
        }
	if ( ++count == number )
	    return ach;
    }

    return NULL;
}

/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    last_fun( "get_char_world");

    if ( ( wch = get_char_room( ch, argument ) ) )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = char_list; wch ; wch = wch->next )
    {
         if ( wch->deleted ) continue;
        // look only at PCs first
        if( IS_NPC( wch ) ) continue;
        if (RACEWAR(ch, wch))
          continue;
        if( !can_see( ch, wch ))
          continue;

        // Targeting by race by Lohrr, NPC fix added by Veygoth
        if( !IS_NPC( wch ) && (race_table[ch->race].racewar_side !=
            race_table[wch->race].racewar_side) && !IS_IMMORTAL( ch )
	    && !IS_IMMORTAL( wch ) )
	{
	    if ( !is_name( arg, race_table[ wch->race ].name ) )
		continue;
	}
	else
        {
	    if ( !is_name( arg, wch->name ) )
		continue;
        }

	if ( ++count == number )
	    return wch;
    }

   //now loop for all the NPCs
    for(wch = char_list; wch; wch = wch->next ) {
        if (!IS_NPC(wch) || wch->deleted) continue;
        if ( !can_see( ch, wch )) continue;
        if (is_name( arg, wch->name ) )
            return wch;
    }

    return NULL;
}

/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    last_fun( "get_obj_type");
    for ( obj = object_list; obj; obj = obj->next )
    {
        if ( obj->deleted )
	    continue;

	if ( obj->pIndexData == pObjIndex )
	    return obj;
    }

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    last_fun( "get_obj_list");
    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
         if ( obj->deleted ) continue;
        // fly_level added by Lithos 3/18/2000
        if(obj->fly_level != ch->fly_level) continue;

	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    count  = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
        if ( obj->deleted) continue;
        if ( ch->fly_level != obj->fly_level 
            && obj->item_type != TYPE_WALL) continue;
	if ( can_see_obj( ch, obj ) && is_name_prefix( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    last_fun( "get_obj_carry");
    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	    && can_see_obj( ch, obj )
	    && (   is_name( arg, obj->name ) 
		|| get_extra_descr( arg, obj->extra_descr )
		|| get_extra_descr( arg, obj->pIndexData->extra_descr ) ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name_prefix( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    last_fun( "get_obj_wear");
    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc != WEAR_NONE
	    && can_see_obj( ch, obj )
	    && ( is_name( arg, obj->name )
		|| get_extra_descr( arg, obj->extra_descr )
		|| get_extra_descr( arg, obj->pIndexData->extra_descr ) ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc != WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name_prefix( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}


/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    last_fun( "get_obj_here");
    if ( ( obj = get_obj_carry( ch, argument ) ) )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) )
	return obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj )
	return obj;

    return NULL;
}

/*
 * Find an obj in the room, but not in inventory.
 */
OBJ_DATA *get_obj_room( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    last_fun( "get_obj_room");
    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj )
	return obj;

    return NULL;
}

/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    last_fun( "get_obj_room");
    if ( ( obj = get_obj_here( ch, argument ) ) )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    count  = 0;
    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( can_see_obj( ch, obj ) && is_name_prefix( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int copper, int silver, int gold, int platinum )
{
    OBJ_DATA *obj;
    int amount;

    last_fun( "create_money");

    amount = copper + silver + gold + platinum;

    if ( copper < 0 || silver < 0 || gold < 0 || platinum < 0 || amount == 0)
    {
	bug( "Create_money: zero or negative money %d.", amount );
        copper = 1;
        silver = 0;
        gold = 0;
        platinum = 0;
        amount = 1;
    }

    if ( amount == 1 )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    }
    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
    }

    obj->value[0]		= copper;
    obj->value[1]               = silver;
    obj->value[2]               = gold;
    obj->value[3]               = platinum;
    return obj;
}



/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    int number;

    last_fun( "get_obj_number");
    number = 0;
    if ( obj->item_type == TYPE_CONTAINER )
        for ( obj = obj->contains; obj; obj = obj->next_content )
	{
	    if ( obj->deleted )
	        continue;
	    number += get_obj_number( obj );
	}
    else
	number = 1;

    return number;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;

    last_fun( "get_obj_weight");
    weight = obj->weight;
    for ( obj = obj->contains; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	weight += get_obj_weight( obj );
    }

    return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    last_fun( "room_is_dark");
    if( !pRoomIndex )
    {
        bug( "Error in room_is_dark: Null pRoomIndex", 0 );
        return FALSE;
    }

    // Magic light overcomes magic darkness.  May want to cause
    // these flags to cancel each other out if both are set - Veygoth
    if ( IS_ROOM( pRoomIndex, ROOM_MAGICLIGHT ) )
	return FALSE;

    // Magical darkness should overcome infravision and twilight
    if ( IS_ROOM( pRoomIndex, ROOM_MAGICDARK ) )
	return TRUE;

    // You can always see in a twilight room - Veygoth
    if ( IS_ROOM( pRoomIndex, ROOM_TWILIGHT ) )
	return FALSE;

    if ( pRoomIndex->light > 0 )
	return FALSE;

    for ( obj = pRoomIndex->contents; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	if ( obj->item_type == TYPE_LIGHT && obj->value[2] != 0 )
	    return FALSE;
        if( IS_OBJ_STAT( obj, ITEM_LIT ))
            return FALSE;
    }

    if ( IS_ROOM( pRoomIndex, ROOM_DARK ))
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
	|| pRoomIndex->sector_type == SECT_CITY
	|| pRoomIndex->sector_type == SECT_ARCTIC
	|| pRoomIndex->sector_type == SECT_FIELD
	|| pRoomIndex->sector_type == SECT_FOREST
	|| pRoomIndex->sector_type == SECT_HILLS
	|| pRoomIndex->sector_type == SECT_MOUNTAIN
	|| pRoomIndex->sector_type == SECT_DESERT
	|| pRoomIndex->sector_type == SECT_UNDERG_CITY )
	return FALSE;

    if ( weather_info.sunlight == SUN_SET
	|| weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}



/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int        count;

    count = 0;
    for ( rch = pRoomIndex->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted )
	    continue;

	count++;
    }

    if ( IS_ROOM( pRoomIndex, ROOM_PRIVATE  ) && count >= 2 )
	return TRUE;

    if ( IS_ROOM( pRoomIndex, ROOM_SOLITARY ) && count >= 1 )
	return TRUE;

    return FALSE;
}



/*
 * True if char can see victim.
 *
 * This is only a straightford all-or-none vision checker.
 *
 * If you need more versatility of function, use how_see which returns an
 * int based on the level of visibility but otherwise functions similarly.
 * how_see can be found in act_info.c
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->deleted )
        return FALSE;

    if ( ch == victim )
	return TRUE;

    /* All mobiles cannot see wizinvised immorts */
    if ( IS_NPC( ch )
	&& !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS ) )
        return FALSE;
	
    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->level )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( is_affected( ch, AFF_BLIND ) )
	return FALSE;

    if ( room_is_dark( ch->in_room )
	&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_ULTRAVISION )
	&& !is_affected( ch, AFF_ULTRAVISION )
	&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_INFRAVISION )
	&& !is_affected( ch, AFF_INFRARED ) )
	return FALSE;

    if ( victim->position == POS_DEAD )
        return TRUE;

    if ( (is_affected( victim, AFF_INVISIBLE ) || is_affected( victim, AFF_MINOR_INVIS ))
	&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_INVIS )
	&& !is_affected( ch, AFF_DETECT_INVIS ) 
        && !( is_affected( ch, AFF_ELEM_SIGHT) &&
          ( victim->race == RACE_ELEM_AIR || victim->race == RACE_ELEM_WATER
          || victim->race == RACE_ELEM_FIRE || victim->race == RACE_ELEM_EARTH) ) )
	return FALSE;

    if ( is_affected( victim, AFF_HIDE )
	&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_HIDDEN )
	&& !is_affected( ch, AFF_DETECT_HIDDEN )
	&& !victim->fighting )
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    last_fun( "can_see_obj");
    if( !obj )
    {
        bug( "Calling CAN_SEE_OBJ with no obj data!", 0 );
        return FALSE;
    }

    if( !ch )
    {
        bug( "Calling CAN_SEE_OBJ with no char_data!", 0 );
        return FALSE;
    }

    if ( obj->deleted )
        return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( is_affected( ch, AFF_BLIND ) || ch->position <= POS_SLEEPING )
	return FALSE;

    if ( obj->item_type == TYPE_LIGHT && obj->value[2] != 0 )
	return TRUE;

    if ( IS_OBJ_STAT( obj, ITEM_SECRET )
	&& !(IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_HIDDEN )
             || is_affected ( ch, AFF_DETECT_HIDDEN) ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_INVIS )
	&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_INVIS )
	&& !is_affected( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    if( IS_OBJ_STAT( obj, ITEM_LIT ))
        return TRUE;

    if ( room_is_dark( ch->in_room )
	&& !(IS_SET( race_table[ ch->race ].race_abilities, RACE_ULTRAVISION )
	     || is_affected( ch, AFF_ULTRAVISION ) ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    last_fun( "can_drop_obj");
    if ( !IS_OBJ_STAT( obj, ITEM_NODROP ) )
	return TRUE;

    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;

    return FALSE;
}



/*
 * Return ascii name of an item type.
 */
char *item_type_name( OBJ_DATA *obj )
{
    OBJ_DATA *in_obj;
    char      buf [ MAX_STRING_LENGTH ];

    last_fun( "item_type_name");
    switch ( obj->item_type )
    {
    case TYPE_LIGHT:		return "light";
    case TYPE_SCROLL:		return "scroll";
    case TYPE_WAND:		return "wand";
    case TYPE_STAFF:		return "staff";
    case TYPE_WEAPON:		return "weapon";
    case TYPE_RANGED_WEAPON:	return "ranged weapon";
    case TYPE_MISSILEWEAP:	return "missile weapon";
    case TYPE_TREASURE:		return "treasure";
    case TYPE_ARMOR:		return "armor";
    case TYPE_POTION:		return "potion";
    case TYPE_CLOTHING:		return "clothing";
    case TYPE_OTHER:		return "other";
    case TYPE_TRASH:		return "trash";
    case TYPE_TRAP:		return "trap";
    case TYPE_CONTAINER:	return "container";
    case TYPE_NOTE:		return "note";
    case TYPE_DRINK_CON:	return "drink container";
    case TYPE_KEY:		return "key";
    case TYPE_FOOD:		return "food";
    case TYPE_MONEY:		return "money";
    case TYPE_PEN:		return "pen";
    case TYPE_BOAT:		return "boat";
    case TYPE_BATTERY:		return "battery";
    case TYPE_PORTAL:		return "portal";
    case TYPE_TIMER:		return "timer";
    case TYPE_VEHICLE:		return "vehicle";
    case TYPE_SHIP:		return "ship";
    case TYPE_SWITCH:		return "switch";
    case TYPE_QUIVER:		return "quiver";
    case TYPE_LOCKPICK:		return "lockpick";
    case TYPE_INSTRUMENT:	return "instrument";
    case TYPE_SPELLBOOK:	return "spellbook";
    case TYPE_TOTEM:		return "totem";
    case TYPE_STORAGECHEST:	return "storage chest";
    case TYPE_SCABBARD:		return "scabbard";
    case TYPE_SHIELD:		return "shield";
    case TYPE_CORPSE_NPC:	return "npc corpse";
    case TYPE_CORPSE_PC:        return "pc corpse";
    case TYPE_PILL:		return "pill";
    case TYPE_MESSAGE_BOARD:	return "message board";
    case TYPE_WANTED_LIST:	return "wanted list";
    case TYPE_WALL:		return "wall";
    case TYPE_AMMO:		return "ammo";
    }

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
      ;

    if ( in_obj->carried_by )
      sprintf( buf, "Item_type_name: unknown type %d from %s owned by %s.",
	      obj->item_type, obj->name, obj->carried_by->name );
    else
      sprintf( buf,
	      "Item_type_name: unknown type %d from %s owned by (unknown).",
	      obj->item_type, obj->name );

    bug( buf, 0 );
    return "(unknown)";
}



/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:		return "none";
    case APPLY_STR:		return "strength";
    case APPLY_DEX:		return "dexterity";
    case APPLY_INT:		return "intelligence";
    case APPLY_WIS:		return "wisdom";
    case APPLY_CON:		return "constitution";
    case APPLY_SEX:		return "sex";
    case APPLY_CLASS:		return "class";
    case APPLY_LEVEL:		return "level";
    case APPLY_AGE:		return "age";
    case APPLY_HEIGHT:          return "height";
    case APPLY_WEIGHT:          return "weight";
    case APPLY_SIZE:		return "size";
    case APPLY_MANA:		return "mana";
    case APPLY_HIT:		return "hp";
    case APPLY_MOVE:		return "moves";
    case APPLY_GOLD:		return "gold";
    case APPLY_EXP:		return "experience";
    case APPLY_AC:		return "armor class";
    case APPLY_HITROLL:		return "hit roll";
    case APPLY_DAMROLL:		return "damage roll";
    case APPLY_SAVING_PARA:	return "save vs paralysis";
    case APPLY_SAVING_ROD:	return "save vs rod";
    case APPLY_SAVING_PETRI:	return "save vs petrification";
    case APPLY_SAVING_BREATH:	return "save vs breath";
    case APPLY_SAVING_SPELL:	return "save vs spell";
    case APPLY_FIRE_PROT:	return "fire protection";
    case APPLY_AGI:             return "agility";
    case APPLY_CHA:             return "charisma";
    case APPLY_POW:             return "power";
    case APPLY_LUK:             return "luck";
    case APPLY_MAX_STR:		return "max strength";
    case APPLY_MAX_DEX:		return "max dexterity";
    case APPLY_MAX_INT:		return "max intelligence";
    case APPLY_MAX_WIS:		return "max wisdom";
    case APPLY_MAX_CON:		return "max constitution";
    case APPLY_MAX_AGI:		return "max agility";
    case APPLY_MAX_POW:		return "max power";
    case APPLY_MAX_CHA:		return "max charisma";
    case APPLY_MAX_LUK:		return "max luck";
    case APPLY_RACE_STR:	return "racial strength";
    case APPLY_RACE_DEX:	return "racial dexterity";
    case APPLY_RACE_INT:	return "racial intelligence";
    case APPLY_RACE_WIS:	return "racial wisdom";
    case APPLY_RACE_CON:	return "racial constitution";
    case APPLY_RACE_AGI:	return "racial agility";
    case APPLY_RACE_POW:	return "racial power";
    case APPLY_RACE_CHA:	return "racial charisma";
    case APPLY_RACE_LUK:	return "racial luck";
    case APPLY_CURSE:		return "curse";
    case APPLY_RESISTANT:	return "resistant";
    case APPLY_IMMUNE:		return "immune";
    case APPLY_SUSCEPTIBLE:	return "susceptible";
    case APPLY_RACE:		return "race";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";
}

/*
 * Return ascii name of parts vector.
 */
char *parts_bit_name( int vector )
{
    static char buf [ 512 ];

    buf[0] = '\0';
    if ( vector & PART_HEAD         ) strcat( buf, " head"           );
    if ( vector & PART_ARMS         ) strcat( buf, " arms"           );
    if ( vector & PART_LEGS         ) strcat( buf, " legs"           );
    if ( vector & PART_HEART        ) strcat( buf, " heart"          );
    if ( vector & PART_BRAINS       ) strcat( buf, " brains"         );
    if ( vector & PART_GUTS         ) strcat( buf, " guts"           );
    if ( vector & PART_HANDS        ) strcat( buf, " hands"          );
    if ( vector & PART_FEET         ) strcat( buf, " feet"           );
    if ( vector & PART_FINGERS      ) strcat( buf, " fingers"        );
    if ( vector & PART_EAR          ) strcat( buf, " ears"           );
    if ( vector & PART_EYE          ) strcat( buf, " eyes"           );
    if ( vector & PART_LONG_TONGUE  ) strcat( buf, " tongue"    );
    if ( vector & PART_EYESTALKS    ) strcat( buf, " eyestalks"      );
    if ( vector & PART_TENTACLES    ) strcat( buf, " tentacles"      );
    if ( vector & PART_FINS         ) strcat( buf, " fins"           );
    if ( vector & PART_WINGS        ) strcat( buf, " wings"          );
    if ( vector & PART_TAIL         ) strcat( buf, " tail"           );
    if ( vector & PART_CLAWS        ) strcat( buf, " claws"          );
    if ( vector & PART_FANGS        ) strcat( buf, " fangs"          );
    if ( vector & PART_HORNS        ) strcat( buf, " horns"          );
    if ( vector & PART_SCALES       ) strcat( buf, " scales"         );
    if ( vector & PART_TUSKS        ) strcat( buf, " tusks"          );
    if ( vector & PART_LEAVES       ) strcat( buf, " leaves"         );
    if ( vector & PART_BRANCHES     ) strcat( buf, " branches"       );
    if ( vector & PART_BARK         ) strcat( buf, " bark"           );
    if ( vector & PART_TRUNK        ) strcat( buf, " trunk"          );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/*
 * Return ascii name of a ris vector.
 */
char *ris_bit_name( int vector )
{
    static char buf [ 512 ];

    last_fun( "ris_bit_name" );
    buf[0] = '\0';
    if ( vector & RIS_FIRE          ) strcat( buf, " fire"           );
    if ( vector & RIS_COLD          ) strcat( buf, " cold"           );
    if ( vector & RIS_ELECTRICITY   ) strcat( buf, " electricity"    );
    if ( vector & RIS_ENERGY        ) strcat( buf, " energy"         );
    if ( vector & RIS_ACID          ) strcat( buf, " acid"           );
    if ( vector & RIS_POISON        ) strcat( buf, " poison"         );
    if ( vector & RIS_CHARM         ) strcat( buf, " charm"          );
    if ( vector & RIS_MENTAL        ) strcat( buf, " mental"         );
    if ( vector & RIS_WHITE_MANA    ) strcat( buf, " white_mana"     );
    if ( vector & RIS_BLACK_MANA    ) strcat( buf, " black_mana"     );
    if ( vector & RIS_DISEASE       ) strcat( buf, " disease"        );
    if ( vector & RIS_DROWNING      ) strcat( buf, " drowning"       );
    if ( vector & RIS_LIGHT         ) strcat( buf, " light"          );
    if ( vector & RIS_SOUND         ) strcat( buf, " sound"          );
    if ( vector & RIS_MAGIC         ) strcat( buf, " magic"          );
    if ( vector & RIS_NONMAGIC      ) strcat( buf, " nonmagic"       );
    if ( vector & RIS_SILVER        ) strcat( buf, " silver"         );
    if ( vector & RIS_IRON          ) strcat( buf, " iron"           );
    if ( vector & RIS_WOOD          ) strcat( buf, " wood"           );
    if ( vector & RIS_WEAPON        ) strcat( buf, " weapon"         );
    if ( vector & RIS_BASH          ) strcat( buf, " bash"           );
    if ( vector & RIS_PIERCE        ) strcat( buf, " pierce"         );
    if ( vector & RIS_SLASH         ) strcat( buf, " slash"          );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}



CHAR_DATA *get_char( CHAR_DATA *ch )
{
    if ( !ch->pcdata )
        return ch->desc->original;
    else
        return ch;
}

bool longstring( CHAR_DATA *ch, char *argument )
{
    if ( strlen( argument) > 60 )
    {
	send_to_char( "No more than 60 characters in this field.\n\r", ch );
	return TRUE;
    }
    else
        return FALSE;
}

bool authorized( CHAR_DATA *ch, char *skllnm )
{

    char buf [ MAX_STRING_LENGTH ];

    if ( ( !IS_NPC( ch ) && str_infix( skllnm, ch->pcdata->immskll ) )
	||  IS_NPC( ch ) )
    {
        sprintf( buf, "Sorry, you are not authorized to use %s.\n\r", skllnm );
	send_to_char( buf, ch );
	return FALSE;
    }

    return TRUE;

}

void end_of_game( void )
{
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *d_next;

    last_fun( "end_of_game");
    for ( d = descriptor_list; d; d = d_next )
    {
	d_next = d->next;
	if ( d->connected == CON_PLAYING )
	{
	    if ( d->character->position == POS_FIGHTING )
	      interpret( d->character, "save" );
	    else
	      do_quit( d->character, "" );
	}
	else
	    close_socket( d );
    }
    save_corpses();
    return;

}

int race_lookup( const char *race )
{
    int index;

    for ( index = 0; index < MAX_RACE; index++ )
        if ( !str_prefix( race, race_table[index].name ) )
	    return index;

    return -1;

}

/* Govno */
int size_lookup( const char *size )
{
    int index;
    for ( index = 0; index < MAX_SIZE; index++ )
        if ( !str_prefix( size, size_string(index) ) )
	    return index;
    
    return -1;

}

int race_key_lookup( const char *race )
{
    int index;

    for ( index = 0; index < MAX_RACE; index++ )
        if ( !str_cmp( race, race_table[index].key ) )
	    return index;

    return -1;

}

int race_full_lookup( const char *race )
{
    int index;

    for ( index = 0; index < MAX_RACE; index++ )
        if ( !str_cmp( race, race_table[index].name ) )
            return index;

    return NO_FLAG;

}

int skill_affect_lookup( const char *affectname )
{
    int index;

    for ( index = 0; index < MAX_SKILL; index++ )
	if ( !str_cmp( affectname, skills_table[index].name ) )
	    return index;

    return -1;

}

int spell_affect_lookup( const char *affectname )
{
    int index;

    for ( index = 0; index < MAX_SPELL; index++ )
	if ( !str_cmp( affectname, spells_table[index].name ) )
	    return index;

    return -1;

}

int song_affect_lookup( const char *affectname )
{
    int index;

    for ( index = 0; index < MAX_SONG; index++ )
	if ( !str_cmp( affectname, songs_table[index].name ) )
	    return index;

    return -1;

}

/*
 * Lookup a clan by name.
 */
CLAN_DATA *clan_lookup( const char *name )
{
    CLAN_DATA *clan;

    for ( clan = clan_first ; clan; clan = clan->next )
    {
	if ( !clan->name )
	    break;
	if ( LOWER( name[0] ) == LOWER( clan->name[0] )
	    && !str_prefix( name, clan->name ) )
	    return clan;
    }

    return NULL;
}

/*
  14k42 = 14 * 1000 + 14 * 100 + 2 * 10 = 14420

  Of course, it only pays off to use that notation when you can skip many 0's.
  There is not much point in writing 66k666 instead of 66666, except maybe
  when you want to make sure that you get 66,666.

  More than 3 (in case of 'k') or 6 ('m') digits after 'k'/'m' are automatically
  disregarded. Example:

  14k1234 = 14,123

  If the number contains any other characters than digits, 'k' or 'm', the
  function returns 0. It also returns 0 if 'k' or 'm' appear more than
  once.

  the pointer to buffer stuff is not really necessary, but originally I
  modified the buffer, so I had to make a copy of it. What the hell, it
  works :) (read: it seems to work :)
*/
int advatoi( const char *s )
{
    int number		= 0;
    int multiplier	= 0;

    /*
     * as long as the current character is a digit add to current number.
     */
    while ( isdigit( s[0] ) )
        number = ( number * 10 ) + ( *s++ - '0' );

    switch (UPPER(s[0]))
    {
        case 'K'  : number *= ( multiplier = 1000 );      ++s; break;
        case 'M'  : number *= ( multiplier = 1000000 );   ++s; break;
        case '\0' : break;
        default   : return 0; /* not k nor m nor NULL - return 0! */
    }

    /* if any digits follow k/m, add those too */
    while ( isdigit( s[0] ) && ( multiplier > 1 ) )
    {
        /* the further we get to right, the less the digit 'worth' */
        multiplier /= 10;
        number = number + ( ( *s++ - '0' ) * multiplier );
    }

    /* return 0 if non-digit character was found, other than NULL */
    if ( s[0] != '\0' && !isdigit( s[0] ) )
        return 0;

    /* anything left is likely extra digits (ie: 14k4443  -> 3 is extra) */

    return number;
}

/*
 * For resistant, immune, and susceptible.
 * The 'globals' (magic and weapons) may be overriden.
 * Three other cases -- wood, silver, and iron -- will be checked in fight.c
 * RIS was mostly based from ROM2.4 code by Russ Taylor <rtaylor@efn.org>
 */
int check_ris( CHAR_DATA *ch, int dam_type )
{
    int ris;
    int def;
    int bit;
    int resistant;
    int immune;
    int susceptible;


    ris = -1;
    def = IS_NORMAL;

    if ( dam_type == DAM_NONE )
        return ris;

    resistant   = ch->resistant   | race_table[ ch->race ].resistant;
    immune      = ch->immune      | race_table[ ch->race ].immune;
    susceptible = ch->susceptible | race_table[ ch->race ].susceptible;

    if ( dam_type < DAM_OTHER )
    {
	     if ( IS_SET( resistant, RIS_WEAPON ) )
	    def = IS_RESISTANT;
	else if ( IS_SET( immune, RIS_WEAPON ) )
	    def = IS_IMMUNE;
	else if ( IS_SET( susceptible, RIS_WEAPON ) )
	    def = IS_SUSCEPTIBLE;
    }
    else
    {
	     if ( IS_SET( resistant, RIS_MAGIC ) )
	    def = IS_RESISTANT;
	else if ( IS_SET( immune, RIS_MAGIC ) )
	    def = IS_IMMUNE;
	else if ( IS_SET( susceptible, RIS_MAGIC ) )
	    def = IS_SUSCEPTIBLE;
    }

    switch ( dam_type )
    {
    default:			return def;
    case DAM_FIRE:		bit = RIS_FIRE;		break;
    case DAM_COLD:		bit = RIS_COLD;		break;
    case DAM_ELECTRICITY:	bit = RIS_ELECTRICITY;	break;
    case DAM_ACID:		bit = RIS_ACID;		break;
    case DAM_POISON:		bit = RIS_POISON;	break;
    case DAM_CHARM:		bit = RIS_CHARM;	break;
    case DAM_MENTAL:		bit = RIS_MENTAL;	break;
    case DAM_ENERGY:		bit = RIS_ENERGY;	break;
    case DAM_WHITE_MANA:	bit = RIS_WHITE_MANA;	break;
    case DAM_BLACK_MANA:	bit = RIS_BLACK_MANA;	break;
    case DAM_DISEASE:		bit = RIS_DISEASE;	break;
    case DAM_DROWNING:		bit = RIS_DROWNING;	break;
    case DAM_LIGHT:		bit = RIS_LIGHT;	break;
    case DAM_SOUND:		bit = RIS_SOUND;	break;
    case DAM_BASH:		bit = RIS_BASH;		break;
    case DAM_PIERCE:		bit = RIS_PIERCE;	break;
    case DAM_SLASH:		bit = RIS_SLASH;	break;
    case DAM_GAS:               bit = RIS_GAS;          break;
    }

         if ( IS_SET( immune, bit ) )
	ris = IS_IMMUNE;
    else if ( IS_SET( resistant, bit ) && ris != IS_IMMUNE )
	ris = IS_RESISTANT;
    else if ( IS_SET( susceptible, bit ) )
    {
	switch ( ris )
	{
	default:		ris = IS_SUSCEPTIBLE;	break;
	case IS_IMMUNE:		ris = IS_RESISTANT;	break;
	case IS_RESISTANT:	ris = IS_NORMAL;	break;
	}
    }

    if ( ris == -1 )
        return def;
    else
        return ris;
}

int get_ac( CHAR_DATA *ch )
{
    int ac;

    ac = ch->armor;

    if( !IS_NPC( ch ) && ch->class == CLASS_MONK )
      ac -= (100 - monk_stances[ch->pcdata->stance].ac );

    /* Make it harder to get to -100. */
    // Was this here before? -- Veygoth
    // Yes, people with an agility of 18 or less actually
    // have a little *better* armor class while asleep.
    // I'm not gonna worry about it. - Veygoth

    if( IS_AWAKE( ch ))
      ac += agi_app[get_curr_agi(ch)].defensive;

    switch( ch->position )
    {
      case POS_SLEEPING:
      case POS_INCAP:
      case POS_STUNNED:
      case POS_DEAD:
      case POS_MORTAL:
         ac += 30;
         break;
      case POS_RECLINING:
         ac += 25;
         break;
      case POS_RESTING:
         ac += 20;
         break;
      case POS_SITTING:
         ac += 15;
         break;
      case POS_KNEELING:
         ac += 10;
         break;
      case POS_FIGHTING:
      case POS_STANDING:
         break;
    }

    if ( ac < 0 )
	ac /=2;

    if ( ac < -100 )
	ac = -100;

    return ac;
}

void move_light ( CHAR_DATA *ch, bool entering )
{
    OBJ_DATA *obj;
    last_fun( "move_light");
    if ( ch->in_room == NULL )
    {
	bug( "Move_light: CH not in a room!", 0 );
	return;
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->deleted )
	    continue;
	if ( IS_OBJ_STAT( obj, ITEM_LIT ) )
        {
	    if ( entering )
	    	ch->in_room->light++;
	    else
	    	ch->in_room->light--;
        }
    }


	/* If they're holding a light source. */
    if (  ( obj = get_eq_char( ch, WEAR_HAND ) )
	 && obj->item_type == TYPE_LIGHT && obj->value[2] != 0 )
    {
	if (entering)
	    ch->in_room->light++;
	else
	{
	    if ( ch->in_room->light > 0 )
	    	--ch->in_room->light;
	    else
	    {
   	    	char error [MAX_STRING_LENGTH];
	    	sprintf(error, "Trying to decrement 0-value light in room %d.",
				 ch->in_room->vnum );
  	    	bug(error, 0);
	    }
	}
    }

	/* If they're holding a light source in 2ndary hand. */
    if ( ( obj = get_eq_char( ch, WEAR_HAND_2 ) )
	 && obj->item_type == TYPE_LIGHT && obj->value[2] != 0 )
    {
	if (entering)
	    ch->in_room->light++;
	else
	{
	    if ( ch->in_room->light > 0 )
	    	--ch->in_room->light;
	    else
	    {
   	    	char error [MAX_STRING_LENGTH];
	    	sprintf(error, "Trying to decrement 0-value light in room %d.",
				 ch->in_room->vnum );
  	    	bug(error, 0);
	    }
	}
    }
}

bool is_underground( CHAR_DATA *ch )
{

   if( !ch->in_room )
     return FALSE;

   if( ch->in_room->sector_type == SECT_UNDERG_WILD
       || ch->in_room->sector_type == SECT_UNDERG_CITY
       || ch->in_room->sector_type == SECT_UNDERG_INSIDE
       || ch->in_room->sector_type == SECT_UNDERG_WATER
       || ch->in_room->sector_type == SECT_UNDERG_WATER_NOSWIM
       || ch->in_room->sector_type == SECT_UNDERG_NOGROUND )
     return TRUE;

   return FALSE;
}

bool is_water_room( ROOM_INDEX_DATA *room )
{
    if( !room )
    {
        bug ("is_water_room: NULL room pointer for target room", 0);
        return FALSE;
    }

    switch( room->sector_type )
    {
        case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
	case SECT_OCEAN:
	case SECT_UNDERWATER_GROUND:
	case SECT_UNDERG_WATER:
	case SECT_UNDERG_WATER_NOSWIM:
	case SECT_PLANE_WATER:
          return TRUE;
          break;
        default:
          return FALSE;
          break;
    }

    return FALSE;
}

bool is_midair_room( ROOM_INDEX_DATA *room )
{
    if( !room )
    {
        bug ("is_water_room: NULL room pointer for target room", 0);
        return FALSE;
    }

    switch( room->sector_type )
    {
	case SECT_AIR:
	case SECT_PLANE_AIR:
	case SECT_UNDERG_NOGROUND:
          return TRUE;
          break;
        default:
          return FALSE;
          break;
    }

    return FALSE;
}

bool is_fly_room( ROOM_INDEX_DATA *rm)
{
    if (!rm )
    {
        bug ("is_fly_room: NULL room pointer for target room", 0);
        return FALSE;
    }
    if ( rm->sector_type == SECT_UNDERG_CITY
        || rm->sector_type == SECT_UNDERG_INSIDE
        || rm->sector_type == SECT_UNDERG_WATER
        || rm->sector_type == SECT_UNDERG_WATER_NOSWIM
        || rm->sector_type == SECT_UNDERG_NOGROUND
        || rm->sector_type == SECT_INSIDE
        || rm->sector_type == SECT_UNDERWATER
        || rm->sector_type == SECT_UNDERWATER_GROUND
        || rm->sector_type == SECT_PLANE_EARTH
        || rm->sector_type == SECT_PLANE_FIRE
        || rm->sector_type == SECT_PLANE_WATER
        || rm->sector_type == SECT_PLANE_ASTRAL
        || rm->sector_type == SECT_PLANE_ETHEREAL )

      return FALSE;
    if (IS_ROOM( rm, ROOM_INDOORS))
      return FALSE;

    return TRUE;
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
    AFFECT_DATA *prev = NULL;

    last_fun( "affect_remove_obj");
    for (prev = obj->affected; prev; prev = prev->next )
    {
    	if (prev == paf) {
            obj->affected = prev->next;
            paf->deleted = TRUE;
        } 
        else if (prev && prev->next == paf) {
            if (paf->next) {
                prev->next = paf->next;
                paf->deleted = TRUE;
            }
            else { //end of list
                prev->next = NULL;
                paf->deleted = TRUE;
            }
        }
    }
    return;
}

// Mob hitpoints are not based on their con *except* when they have modifiers
// Farly complex math on con-modified mobs
int get_max_hit( CHAR_DATA *ch )
{
   int value;

   if( !IS_NPC( ch ))
     value = ( (ch->max_hit * get_curr_con( ch )) / 100 ) + ch->pcdata->mod_hit;
   else if( ch->mod_con == 0 )
     value = ch->max_hit;
   else
/* commented out for now, getting WAY buggy max_hits for mobs  - Lithos
     value = ch->max_hit + (( ch->max_hit * (ch->mod_con * race_table[ch->race].con_mod) / 100) * ch->max_hit);
 */
//     value = ch->max_hit + (( ch->max_hit * (ch->mod_con + race_table[ch->race].con_mod) / 100) );
    value = ch->max_hit * ( 100 + ch->mod_con  ) / 100;

   if( value < 1 )
     value = 1;

   return value;
}

/*
 * Find a char in an arbitrary room.
 */
CHAR_DATA *get_char_at( ROOM_INDEX_DATA *room, CHAR_DATA *ch,
          char *argument)
{
    CHAR_DATA *ach;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;


    number = number_argument( argument, arg );
    count  = 0;
    for ( ach = room->people; ach ; ach = ach->next_in_room )
    {
	if ( ach->deleted || ch->fly_level != ach->fly_level)
                continue;

        // Targeting by race by Lohrr, NPC fix added by Veygoth
        if( !IS_NPC( ach ) && (race_table[ch->race].racewar_side !=
            race_table[ach->race].racewar_side) && !IS_IMMORTAL( ch ))
	{
	    if ( !is_name( arg, race_table[ ach->race ].name ) )
		continue;
	}
	else
        {
	    if ( !is_name( arg, ach->name ) )
		continue;
        }
	if ( ++count == number )
	    return ach;
    }

    return NULL;
}

/*
 * Give an affect to an obj.
 *
 * Shameless clone of affect_to_char  - Lithos
 * Doesn't set any bits on the object
 */
void affect_to_obj( OBJ_DATA *obj, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;
    int i;

    last_fun( "affect_to_obj");
    paf_new = new_affect();
    paf_new->skill = paf->skill;
    paf_new->spell = paf->spell;
    paf_new->song = paf->song;
    paf_new->duration = paf->duration;
    paf_new->location = paf->location;
    paf_new->modifier = paf->modifier;
    for(i = 0; i < NUM_AFFECT_VECTORS; i++) {
        paf_new->bitvector[i] = paf->bitvector[i];
    }
    paf_new->deleted    = FALSE;
    paf_new->next	= obj->affected;
    obj->affected	= paf_new;
    
    return;
}

bool has_affect_obj( OBJ_DATA *obj, int skl, int spl, int song) {
    AFFECT_DATA *paf;

    for (paf = obj->affected; paf; paf = paf->next) {
        if (paf->skill == skl && paf->spell == spl && paf->song == song)
            return TRUE;
    }
    return FALSE;
}


CHAR_DATA *create_char_from_obj( OBJ_DATA *obj )
{
    CHAR_DATA *mob;
    MOB_INDEX_DATA *pMobIndex;
    int count;

    last_fun( "create_char_from_obj");
    mob = new_character( FALSE );

    if ( !(pMobIndex = get_mob_index( MOB_VNUM_OBJECT )))
    {
        bug( "Create_char_from_obj: NULL pMobIndex.", 0 );
        return NULL;
    }

    mob                 = new_character( FALSE );

    mob->pIndexData     = pMobIndex;
    mob->followers      = NULL;
    mob->name           = str_dup( obj->name );
    mob->short_descr    = str_dup( obj->short_descr );
    mob->long_descr     = str_dup( obj->description  );
    mob->description    = str_dup( obj->description );
    mob->spec_fun       = NULL;
    mob->spec_fun2      = NULL;
    mob->class          = pMobIndex->class;
    mob->level          = UMAX( obj->level, 1 );
    mob->act            = pMobIndex->act;
    mob->position       = pMobIndex->position;
    for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
    {
        mob->affected_by[count] = pMobIndex->affected_by[count];
    }
    mob->alignment      = pMobIndex->alignment;
    mob->sex            = pMobIndex->sex;
    mob->race           = pMobIndex->race;
    mob->size           = race_table[mob->race].size;
    if( IS_SET( mob->act, ACT_SIZEMINUS ))
      mob->size--;
    if( IS_SET( mob->act, ACT_SIZEPLUS ))
      mob->size++;

    mob->cast_spl       = 0;
    mob->cast_time      = 0;
    mob->perm_str       = 55;
    mob->perm_int       = 55;
    mob->perm_wis       = 55;
    mob->perm_dex       = 55;
    mob->perm_con       = 55;
    mob->perm_agi       = 55;
    mob->perm_cha       = 55;
    mob->perm_pow       = 55;
    mob->perm_luk       = 55;
    mob->mod_str        = 0;
    mob->mod_int        = 0;
    mob->mod_wis        = 0;
    mob->mod_dex        = 0;
    mob->mod_con        = 0;
    mob->mod_agi        = 0;
    mob->mod_cha        = 0;
    mob->mod_pow        = 0;
    mob->mod_luk        = 0;
    mob->resistant      = pMobIndex->resistant;
    mob->immune         = pMobIndex->immune;
    mob->susceptible    = pMobIndex->susceptible;
    mob->money.copper = 0;
    mob->money.silver = 0;
    mob->money.gold = 0;
    mob->money.platinum = 0;
    mob->armor          = interpolate( mob->level, 100, -100 );

    // * MOB HITPOINTS *
    //
    // Was level d 8, upped it to level d 13
    // considering mobs *still* won't have as many hitpoints as some players until
    // at least lvl 10, this shouldn't be too big an upgrade.
    //
    // Mob hitpoints are not based on constitution *unless* they have a
    // constitution modifier from an item, spell, or other affect
    //
    // -- Veygoth

    mob->max_hit        = mob->level * 100;
    mob->hit            = get_max_hit( mob );

    /*
     * Insert in list.
     */
    mob->next           = char_list;
    char_list           = mob;
    pMobIndex->count++;
    char_to_room( mob, obj->in_room );
    return mob;
}


/*  Cloned from affect_strip()  -- Lithos
 * Strip all affects of a given sn.
 */
// This is now designed to take two arguments, a skill and a spell
// argument, in that order.  One of the values must be zero.
// -- Veygoth
void affect_strip_obj( OBJ_DATA *obj, int skl, int spl, int song )
{
    AFFECT_DATA *paf;
    int sn;

    last_fun( "affect_strip_obj");
    if( skl != 0 && spl != 0 )
    {
       bug( "Invalid call to affect_strip_obj!  Must pass one null value!", 0
);
       return;
    }

    if( skl )
    {
        sn = skl;
    }
    else if( spl )
    {
        sn = spl;
    }
    else if( song )
    {
        sn = song;
    }
    else
    {
       bug( "Invalid call to affect_strip_obj!  Must pass one non-null value!", 0 );
       return;
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	if (   ( paf->skill == sn && skl  )
	    || ( paf->spell == sn && spl  )
	    || ( paf->song  == sn && song ) )
	{
	    affect_remove_obj( obj, paf );
/* NEW: Added check to make sure paf is marked for deletion. */   
	    if ( !paf->deleted )
	    {
		bug( "Affect_strip_obj: paf not deleted when it should be!",
0 );
        	paf->deleted = TRUE;
	    }
	}
    }

    return;
}

// Moved print_last_fun to another function, and made the function count a global
// to speed things up (this function will get called billions of times, we want
// it fast -- Veygoth
void last_fun( const char *name )
{
    if ( !IS_SET( sysdata.act, MUD_DEBUG ))
      return;

    if( last_fun_count >  MAX_LAST_FUNS - 1 )
      last_fun_count = 0;
    if( last_fun_count == 0 || strcmp( last_funs[last_fun_count-1], name ) )
    {
        strcpy( last_funs[last_fun_count], name );
        last_fun_count++;
    }
}

void last_com( const char *com )
{
    if ( !IS_SET( sysdata.act, MUD_DEBUG ))
      return;

    if ( last_com_count > MAX_LAST_COMS - 1 )
        last_com_count = 0;
    strcpy(last_coms[last_com_count], com );
    last_com_count++;
}

void print_last_funs( )
{
    int count;
    int i = last_fun_count - 1;

    if( !IS_SET( sysdata.act, MUD_DEBUG ))
    {
      log_string( "MUD_DEBUG is not turned on, not printing last 10 functions." );
      return;
    }

    log_string( "Last functions, oldest to newest:" );
    for( count = 0; count < MAX_LAST_FUNS; count++ )
    {
      i++;
      if( i > MAX_LAST_FUNS - 1 )
        i = 0;
      log_string( last_funs[i] );
    }
    log_string( "Done printing last functions." );
}

void print_last_coms( )
{
    int count;
    int i = last_com_count - 1;

    if ( !IS_SET( sysdata.act, MUD_DEBUG ))
    {
        log_string( "MUD_DEBUG is not turned on, not printing last commands." );
        return;
    }
    log_string( "Last commands, oldest to newest." );
    for( count = 0; count < MAX_LAST_COMS; count++ )
    {
        i++;
         if ( i > MAX_LAST_COMS - 1 )
            i = 0;
        log_string( last_coms[i] );
    }
    log_string( "Done printing last commands." );
}

void do_crashmud( CHAR_DATA *ch, char *argument ) {
    CHAR_DATA *empty = NULL;
    int i;

    last_fun( "do_crashmud" );
    if ( !IS_IMMORTAL( ch ) ) {
        send_to_char( "Howzat?\n\r", ch );
        return;
    }
    if ( !strcmp( argument, "confirm" ) )
        i = empty->wait;
    else {
        send_to_char( "Usage: crashmud confirm\n\r", ch);
        return;
    }
}

void do_loopmud( CHAR_DATA *ch, char *argument ) {
//    CHAR_DATA *empty = NULL;
    int i;

    last_fun( "do_loopmud" );
    if ( !IS_IMMORTAL( ch ) ) {
        send_to_char( "Howzat?\n\r", ch );
        return;
    }
    if ( !strcmp( argument, "confirm" ) ) {
        send_to_char("As you wish.  The mud is now in an endless loop.\n\r", ch );
        for( ; ; ) {
            i = 1;
        }
    }
    else {
        send_to_char( "Usage: loopmud confirm\n\r", ch);
        return;
    }
}


