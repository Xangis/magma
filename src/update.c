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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

char lbuf[MAX_STRING_LENGTH];

/*
 * Externals
 */ 
extern  bool            merc_down;

/*
 * Globals
 */
bool    delete_obj;
bool    delete_char;

/*
 * Local functions.
 */
int	hit_gain        args( ( CHAR_DATA *ch ) );
int	mana_gain       args( ( CHAR_DATA *ch ) );
int	move_gain       args( ( CHAR_DATA *ch ) );
void	room_update	args( ( void ) );
void	mobile_update   args( ( void ) );
void	obj_update	args( ( void ) );
void	weather_update  args( ( void ) );
void	char_update     args( ( void ) );
void	obj_spec_update args( ( void ) );
void	aggr_update     args( ( void ) );
void	heartbeat_update args(( void ) );


/*
 * Advancement stuff.
 */
// Non-bards and non-psionicists will still gain mana,
//  we just won't tell them about it. -- Veygoth
void advance_level( CHAR_DATA *ch, bool skills )
{
    char buf [ MAX_STRING_LENGTH ];
    int  count;
    int  add_hp;
    int  add_mana;

    last_fun( "advance_level");
    send_to_char( "&+WYou raise a level!&n\n\r", ch );

    /* Knock down a level of exp so they don't spamlevel. */
    ch->exp -= exp_table[ch->level].lvl_exp;
    if( ch->exp < 0 )
      ch->exp = 0;
    ch->level += 1;

    if( !IS_NPC( ch ) && ch->pcdata->lost_hp != 0 )
    {
      if( number_percent() < ( get_curr_luk( ch ) + get_curr_con( ch ) ) / 3 )
          add_hp = ch->pcdata->lost_hp;
      else
          add_hp = ch->pcdata->lost_hp - 1;
      ch->pcdata->lost_hp = 0;
    }
    else
    {
      add_hp      = number_range( class_table[ch->class]->hp_min,
		                  class_table[ch->class]->hp_max);
    }

    add_mana    = class_table[ch->class]->fMana
        ? dice(2, (( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 30 ) )
	: 0;

    // Removed con bonus - This will be handled by get_max_hit()

    if( ch->class != CLASS_BARD )
    {
      add_mana = (add_mana * get_curr_pow( ch )) / 100;
    }
    else
    {
      add_mana = (add_mana * get_curr_cha( ch )) / 100;
      if( ch->race == RACE_HALFELF )
        add_mana += 1; // Bonus for half-elven bards
    }

    add_hp               = UMAX(  1, add_hp   );
    add_mana             = UMAX(  0, add_mana );

    // Don't touch max_hit except for PERMENANT changes - Veygoth
    ch->max_hit 	+= add_hp;

    ch->hit             += add_hp;

    ch->max_mana	+= add_mana;
    ch->mana            += add_mana;

    // People getting hits rerolled shouldn't get automatic skill increases
    // -- Veygoth
    if( skills )
    {
      if( ch->class == CLASS_MONK || ch->class == CLASS_MYSTIC )
         ch->pcdata->train += (1 + ch->level / 10);
      set_skills( ch );
    }

    /*  Clerics and psionicists should automatically get new spells
     *  every five levels and be notified what they learned.
     *  And shamans too.
     */
    if( ch->class == CLASS_CLERIC || ch->class == CLASS_PSIONICIST
	|| ch->class == CLASS_SHAMAN || ch->class == CLASS_PALADIN
        || ch->class == CLASS_ANTIPALADIN || ch->class == CLASS_DRUID )
    {
       if( ch->level %5 == 1 )
       {
           send_to_char( "You learn new spells!\n\r", ch );
           for( count = 0; count < MAX_SPELL; count++ )
           {
              if( (spells_table[count].spell_circle[ch->class] <=
                  ((ch->level + 4) / 5)) 
                  && (ch->pcdata->spl_lrn[count] < BASE_SPELL_ADEPT 
						+ get_curr_wis( ch ) / 9 )
                  && spells_table[count].can_scribe )
                  {
                     sprintf( buf, "&+cYou learn %s.&n\n\r", spells_table[count].name );
                     send_to_char( buf, ch );
                     ch->pcdata->spl_lrn[count] = BASE_SPELL_ADEPT  
						+ get_curr_wis( ch ) / 9;
                  }
           }
       }
    }

    if( ch->class == CLASS_PSIONICIST || ch->class == CLASS_BARD )
    {
      sprintf( buf,
	    "Your gain is: %d/%d hp, %d/%d mana.\n\r",
	    (add_hp * get_curr_con( ch ) / 100),     get_max_hit( ch ),
	    add_mana,	ch->max_mana );
      send_to_char( buf, ch );
    }
    else
    {
      sprintf( buf,
	    "Your gain is: %d/%d hp.\n\r",
	    (add_hp * get_curr_con( ch ) / 100),	get_max_hit( ch ) );
      send_to_char( buf, ch );
    }

    save_char_obj( ch );

    return;
}   

/*
 * Demote stuff.
 */
void demote_level( CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];
    int  add_hp;
    int  add_mana;

    last_fun( "demote_level");
    if ( ch->level == 1 )
        return;

    add_hp      = class_table[ch->class]->hp_max;

    add_mana    = class_table[ch->class]->fMana
        ? number_range(2, (( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 16 ) )
	: 0;

    if( ch->class != CLASS_BARD )
    {
      add_mana = (add_mana * get_curr_pow( ch )) / 100;
    }
    else
    {
      add_mana = (add_mana * get_curr_cha( ch )) / 100;
      if( ch->race == RACE_HALFELF )
        add_mana += 1; // Bonus for half-elven bards
    }

    add_hp               = UMAX(  1, add_hp   );
    add_mana             = UMAX(  0, add_mana );

    ch->max_hit 	-= add_hp;
    ch->hit             -= add_hp;
    ch->max_mana	-= add_mana;
    ch->mana            -= add_mana;

    if( !IS_NPC( ch ))
       ch->pcdata->lost_hp = add_hp;

    ch->level -= 1;
    if( ch->class == CLASS_PSIONICIST || ch->class == CLASS_BARD )
    {
      sprintf( buf,
	    "Your loss is: %d/%d hp, %d/%d mana.\n\r",
	    (add_hp * get_curr_con( ch ) / 100),	get_max_hit( ch ),
	    add_mana,	ch->max_mana );
    }
    else
    {
      sprintf( buf,
	    "Your loss is: %d/%d hp.\n\r",
	    (add_hp * get_curr_con( ch ) / 100),	get_max_hit( ch ) );
    }
    send_to_char( buf, ch );
    return;
}   

void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf [ MAX_STRING_LENGTH ];

    last_fun( "gain_exp");
    if ( IS_NPC( ch ) )
	return;

    /* Cap for exp on any kill. Equivalent of ress exp. - Lohrr */
    if( gain > ( (50 + ch->level) * exp_table[ch->level].lvl_exp ) / 500 )
        gain = ( (50 + ch->level) * exp_table[ch->level].lvl_exp ) / 500;

    // Human experience bonus of 10% as a Human innate -- Veygoth
    if( ch->race == RACE_HUMAN )
    {
       gain = (gain * 11) / 10;
    }

    ch->exp += gain;

    /* Allow 50+ chars to continue gaining exp, but do not let them gain
     *   any levels.  That's the job of 51/52/etc. potions. - Lohrr */

    if( (ch->exp >= exp_table[ch->level].lvl_exp) && (ch->level < MAX_ADVANCE_LEVEL))
    {
        /* Increase a level. Better than just stealing all their exp
         * past 1 level; allows someone needing 1 exp to level to get more
         * than two exp.  Also allows them to only get 1.
         */
	advance_level( ch, TRUE );
	sprintf( buf, "%s has levelled and is now level %d.",
		ch->name, ch->level );
	wiznet( ch, WIZ_LEVELS, get_trust( ch ), buf );
        if ( IS_SET( sysdata.act, MUD_TURBOLEVEL))
            {
                while ( ch->level % 10 != 0) {
                   gain_exp( ch,exp_table[ch->level].lvl_exp );
                }
            }
    }
    /* Lvl 1 chars. can't lose lvl. - Lohrr */
    else if( ch->exp < 0 && ch->level > 1 && !IS_IMMORTAL( ch ) )
    {
        ch->exp = exp_table[(ch->level - 1)].lvl_exp + ch->exp;
        send_to_char( "&+RYou lose a level!&n\n\r", ch );
        demote_level( ch );
	sprintf( buf, "%s has de-levelled and is now level %d.",
		ch->name, ch->level );
	wiznet( ch, WIZ_LEVELS, get_trust( ch ), buf );
    }

    return;
}

void hit_update( )
{
    CHAR_DATA *ch;

    last_fun( "hit_update");
    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( ch->deleted )
	    continue;

	if ( ch->position >= POS_INCAP )
	{
	    if ( ch->hit  < get_max_hit( ch )  )
		ch->hit  += hit_gain( ch );
            else if( ch->hit > get_max_hit( ch ))
                ch->hit--;

	}
        update_pos( ch );
    }
}

void mana_update( )
{
    CHAR_DATA *ch;

    last_fun( "mana_update");
    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( ch->deleted )
	    continue;

	if ( ch->position >= POS_INCAP )
	{
	    if ( ch->mana < ch->max_mana  )
		ch->mana += mana_gain( ch );
	}
    }
}

void move_update( )
{
    CHAR_DATA *ch;

    last_fun( "move_update");

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( ch->deleted )
	    continue;

	if ( ch->move < ch->max_move )
	{
	    if ( ch->position > POS_INCAP  )
		ch->move  += move_gain( ch );
	}
    }
}

/*
 * Camping timer tick-tocking away
 * Ugly, but it should work.   Govno
 */
/*
 * Rewritten to use a single event for each person that
 * is actively camping.
 */
bool camp_update( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
    if( ch->deleted )
      return FALSE;
    if ( ch->position <= POS_INCAP )
      return FALSE;
    if( !IS_SET( ch->act, PLR_CAMPING ))
      return FALSE;
    if( ch->position == POS_FIGHTING || ch->fighting ||
        ch->in_room != room )
    {
       send_to_char( "So much for that camping effort.\n\r", ch );
       REMOVE_BIT( ch->act, PLR_CAMPING );
       return FALSE;
    }
    return TRUE;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int percent;
    
    // Veygoth - they aren't going to gain hits in a no heal room...
    if( IS_ROOM( ch->in_room, ROOM_NO_HEAL ) )
        return 0;

    if ( IS_NPC( ch ) )
    {
	gain = 1;
    }
    else
    {
	gain = 1;
        percent = 100;

	if ( number_percent() < ch->pcdata->skl_lrn[gsn_fast_healing])
	     gain += 1;

        if( number_percent() < 3 )
             skill_practice( ch, gsn_fast_healing );

	switch ( ch->position )
	{
	case POS_SLEEPING: 	gain += 3;	break;
	case POS_RECLINING:  	gain += 2;  	break;
	case POS_RESTING:  	gain += 1;  	break;
	case POS_FIGHTING:  	gain  = 0;  	break;
	}

        if( ch->race == RACE_TROLL )
        {
          gain+= 2;
          percent += 60;
        }

        if ( ch->pcdata->condition[COND_FULL  ] == 0 )
        {
            gain /= 2;
            gain -= 1;
            percent -= 25;
            send_to_char( "&nYou double over from &+Rhunger pains&n!\n\r",  ch);
        }

        if ( ch->pcdata->condition[COND_THIRST] == 0 )
        {
            gain /= 2;
            gain -= 1;
            percent -= 25;
            send_to_char( "&nYou suffer from severe &+cth&+Ci&n&+cr&+Cst&n!\n\r", ch);
        }

    }

    if ( is_affected( ch, AFF_POISON ) )
    {
	gain /= 4;
        percent /= 2;
    }

    if( gain == 0 )
       if( number_percent() < percent )
           gain = 1;

    // Veygoth - heal rooms heal you a little quicker
    if ( IS_ROOM( ch->in_room, ROOM_HEAL ))
        gain += UMAX( 1, gain / 2 );

    if( (  ch->in_room->sector_type != SECT_UNDERWATER
        && ch->in_room->sector_type != SECT_WATER_NOSWIM
        && ch->in_room->sector_type != SECT_WATER_SWIM
        && IS_SET( race_table[ ch->race ].race_abilities,  RACE_WATERBREATH )
        && str_cmp( race_table[ch->race].name, "Object" ) 
        && ch->race != RACE_GOD )
      || ( ch->in_room->sector_type == SECT_UNDERWATER
          && ( !IS_IMMORTAL( ch ) && !is_affected( ch, AFF_GILLS )
              && !IS_SET( race_table[ ch->race ].race_abilities, RACE_WATERBREATH ) ) ) )
        gain = 0;

    return UMIN( gain, get_max_hit( ch ) - ch->hit );
}

int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int number;
    int percent = 0;

    if ( IS_NPC( ch ) )
    {
	gain = 4 + ch->level / 10;
    }
    else
    {
        /* at 17 gain == base 5, at 34 gain == base 6, at 51 gain == base 7 */
        gain = 4 + ch->level / 17;
        percent = 100;

	switch ( ch->position )
	{
	case POS_SLEEPING: percent += 100;
  			   break;
	case POS_RESTING:  percent += 50;
                           if( IS_SET( ch->act, PLR_MEDITATING ))
                           {
                             percent += 25;
                             number = number_percent();
	                     if (number < ch->pcdata->skl_lrn[gsn_meditate])
             		       percent += 150;
	                     if (number + 50 < ch->pcdata->skl_lrn[gsn_meditate])
             		       percent += 100;
                             skill_practice( ch, gsn_meditate );
                           }
  			   break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
        {
	    percent -= 50;
        }
	if ( ch->pcdata->condition[COND_THIRST] == 0 )
        {
            percent -= 50;
        }

    }

    if( percent < 0 )
      percent = 0;

    gain = (gain * percent) / 100;
    if( ch->class != CLASS_BARD )
        gain = (gain * get_curr_pow( ch ) ) / 100;
    else
        gain = (gain * get_curr_cha( ch ) ) / 100;

    return UMIN( gain, ch->max_mana - ch->mana );
}

int move_gain( CHAR_DATA *ch )
{
    int gain;
    int percent = 0;

    if ( IS_NPC( ch ) )
    {
	gain = 4;
    }
    else
    {
        /* Change this back before pwipe
	gain = 2;
        */
        //gain = 10;

        // Veygoth -- People like fast move regen, but 10 is too fast.
        gain = 6;

	switch ( ch->position )
	{
 	  case POS_SLEEPING: gain += 4;	break;
	  case POS_RESTING:  gain += 2;  	break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
        {
	    gain /= 2;
            percent -= 30;
        }

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
        {
	    gain /= 2;
            percent -= 30;
        }
       if ( is_affected( ch, AFF_POISON ) )
       {
 	    gain /= 3;
            percent /= 2;
       }
       if( gain == 0 )
          if( number_percent() < percent )
            gain = 1;
    }


    return UMIN( gain, ch->max_move - ch->move );
}

// Cause bards to gain mana by drinking alcohol.
void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    last_fun( "gain_condition");
    if ( value == 0 || IS_NPC( ch ) || ch->level >= LEVEL_HERO )
	return;

    // Tentatively giving bards 3 mana per drunken unit
    if( value > 0 && iCond == COND_DRUNK && ch->class == CLASS_BARD )
      ch->mana += value * 3;

    if( value > 0 && iCond == COND_DRUNK && ch->level >= skills_table[gsn_alcohol_tolerance].skill_level[ch->class] )
    {
       if( number_percent() < ch->pcdata->skl_lrn[gsn_alcohol_tolerance] )
       {
          value /= 2;
          skill_practice( ch, gsn_alcohol_tolerance );
          send_to_char( "That drink wasn't nearly as strong as you had hoped.\n\r", ch );
       }
    }

    condition				= ch->pcdata->condition[ iCond ];
    ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );

    // Tentatively giving bards 3 mana per drunken unit
    if( iCond == COND_DRUNK && ch->class == CLASS_BARD )
      ch->mana += value * 3;

    if ( ch->pcdata->condition[iCond] == 0 )
    {
        switch ( iCond )
        {
        case COND_FULL:
            send_to_char( "&nYou are &+Rhungry&n.\n\r",    ch );
            break;

        case COND_THIRST:
            send_to_char( "&nYou are &+Cthirsty&n.\n\r",   ch );
            break;

        case COND_DRUNK:
            if ( condition != 0 )
                send_to_char( "You begin to sober.\n\r", ch );
            break;
        }
    }
    else if ( ch->pcdata->condition[iCond] <= 3 )
    {
        switch ( iCond )
        {
        case COND_FULL:
            send_to_char( "&nYou are getting &n&+rhungry&n.\n\r",    ch );
            break;

        case COND_THIRST:
            send_to_char( "&nYou are getting &n&+cthirsty&n.\n\r",   ch );
            break;
        }
    }
    else if ( ch->pcdata->condition[iCond] <= 10 )
    {
        switch ( iCond )
        {
        case COND_FULL:
            if ( number_bits( 2 ) == 0 )
                send_to_char( "&nYour stomach rumbles.\n\r",    ch );
            break;

        case COND_THIRST:
            if ( number_bits( 2 ) == 0 )
                send_to_char( "&nYour mouth is feeling parched.\n\r",   ch );
            break;
        }
    }

}

/*
 * Mob autonomous action.
 * This function takes 25% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch, *pch;
    EXIT_DATA *pexit;
    int        door;
    char       buf [ MAX_STRING_LENGTH ];
    static int pulse = 0;

    last_fun( "mobile_update");
    pulse = ((pulse + 1) % NUM_MOB_PULSES );

    /* Examine all mobs. */
    for ( ch = char_list; ch; ch = ch->next )
    {
        int rnum;

        if ( ch->deleted )
	    continue;

	if ( !ch->in_room )
	    continue;

	if ( !IS_NPC( ch ))
            continue;

        // Mobs may have to wait for lag too
        if ( ch->wait > 0)
        {
            ch->wait -= PULSE_MOBILE;
            if( ch->wait < 0 )
            {
              ch->wait = 0;
            }
            continue;
        }

        // Mobs bashed or knocked down will try to get back up...
        if( ch->position < POS_FIGHTING && ch->position > POS_SLEEPING 
            && (ch->position != ch->pIndexData->position || ch->fighting) )
        {
            do_stand( ch, "" );
            continue;
        }

        if( ch->position == POS_SLEEPING && !has_affect(ch, 0, spl_sleep, 0) ) {
            //if there is a fight going on have a chance to waken
            for( pch = ch->in_room->people; pch; pch = pch->next_in_room) {
                if (pch->deleted) continue;
                if (pch->fighting &&  number_bits(3) == 0&& !has_affect( ch, 0, spl_sleep, 0) ) {
                    act("$n awakens from $s slumber.",ch, NULL, NULL, TO_ROOM);
                    ch->position = POS_RECLINING;
                    break;
                }
            }
        }

        if (ch->position == POS_SLEEPING)
            continue;

        // If we're just updating lag, we have no need at all for any
        // mob movement or special function code
        // every mob pulse-check they'll just check for stand and such, but they
        // won't check special functions
        // -- Veygoth
        if( pulse != 0 )
          continue;

        // Only thing charmies will do is autostand
        if( is_affected( ch, AFF_CHARM ) )
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( ( *ch->spec_fun ) ( ch, PROC_NORMAL ) )
		continue;
	}
        if ( ch->spec_fun2 != 0 )
        {
            if ( ( *ch->spec_fun2 ) ( ch, PROC_NORMAL ) )
                continue;
        }
        // check if spec_fun wiped out ch
        if ( !ch )
            continue;
        if ( !ch->fighting
            && ch->hunting
            && ch->position == POS_STANDING  )
        {
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            hunt_victim( ch );
            continue;
        }


	/* That's all for sleeping / busy monster */
	if ( ch->position < POS_STANDING )
	    continue;

        if ( ch->rider )
        {
            if ( IS_SET( ch->act, ACT_AGGRESSIVE ) )
                do_emote( ch, "snarls and growls." );
            continue;
        }

        if ( IS_ROOM( ch->in_room, ROOM_SAFE )
	    && IS_SET( ch->act, ACT_AGGRESSIVE ) )
	    do_emote( ch, "glares around and snarls." );

        /* MOBprogram random trigger */
        if ( ch->in_room->area->nplayer > 0 )
        {
            prog_random_trigger( ch );
                                                /* If ch dies or changes
                                                position due to it's random
                                                trigger, continue - Kahn */
            if ( ch->position < POS_STANDING )
                continue;
        }

	/* Scavenge */
	if ( IS_SET( ch->act, ACT_SCAVENGER )
	    && ch->in_room->contents
	    && number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int       max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR( obj, ITEM_TAKE )
		    && obj->cost > max
		    && can_see_obj( ch, obj ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n&n gets $p&n.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

        // Added by Veygoth for movement scripts
        if( ch->pIndexData->move_script )
          if( prog_move_script( ch ))
            continue;

	/* Wander or Flee  - modifications by Jason Dinkel */
	if ( ch->hit < get_max_hit( ch ) / 5 )
	    rnum = 3;
	else
	    rnum = 5;

	if ( !IS_SET( ch->act, ACT_SENTINEL )
	    && ( door = number_bits( rnum ) ) < MAX_DIR
	    && ( pexit = ch->in_room->exit[door] )
	    &&   pexit->to_room
	    &&   !IS_SET( pexit->exit_info, EX_CLOSED )
	    &&   !IS_ROOM( pexit->to_room, ROOM_NO_MOB )
	    && ( !IS_SET( ch->act, ACT_STAY_AREA )
		||   pexit->to_room->area == ch->in_room->area )
            && !( !(is_water_room( pexit->to_room )) && IS_SET( race_table[ ch->race].race_abilities, RACE_SWIM ) ) )
	{
	    /* Give message if hurt */
	    if ( rnum == 3 )
	        act( "$n&n wanders off terrified!", ch, NULL, NULL, TO_ROOM );
		  
	    move_char( ch, door );
	                                        /* If ch changes
	                                        position due
                                                to it's or someother mob's
                                                movement via MOBProgs,
                                                continue - Kahn */
            if ( ch->position < POS_STANDING )
                continue;

	}

	/* If people are in the room, then flee. */
	if ( rnum == 3
	    && !IS_SET( ch->act, ACT_SENTINEL ) )
	{
	    CHAR_DATA *rch;

	    for ( rch  = ch->in_room->people; rch; rch  = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;

		/* If NPC can't see PC it shouldn't feel fear - Zen */
		if ( !IS_NPC( rch ) && can_see( ch, rch ) && !IS_IMMORTAL( rch) )
		{
		    int direction;

		    door = -1;

		    /* SMAUG had this. It makes some nice FX - Zen */
		    if ( is_fearing( ch, rch ) )
		    {
			switch( number_bits( 5 ) )
			{
			default: break; 
			case  0: sprintf( buf, "Get away from me, %s!",
					 rch->name );
                                 do_yell( ch, buf );
                                 break;
			case  1: sprintf( buf, "Leave me be, %s!",
					    rch->name );
                                 do_yell( ch, buf );
                                 break;
			case  2: sprintf( buf, "%s is trying to kill me! Help!",
					 rch->name );
                                 do_yell( ch, buf );
                                 break;
			case  3: sprintf( buf, "Someone save me from %s!",
					    rch->name );
                                 do_yell( ch, buf );
                                 break;
			}
		    }

		    act( "$n&n attempts to flee...", ch, NULL, NULL, TO_ROOM );
                    if( is_affected( ch, AFF_HOLD ) ||
                        is_affected( ch, AFF_MINOR_PARA ) ||
                        is_affected( ch, AFF_BOUND ))
                    {
                        act( "$n&n cannot move!", ch, NULL, NULL, TO_ROOM );
                        break;
                    }


		    /* Find an exit giving each one an equal chance */
		    for ( direction = 0; direction < MAX_DIR; direction++ )
		    {
			if ( ch->in_room->exit[direction]
			    && number_range( 0, direction ) == 0 )
			    door = direction;
		    }

		    /* If no exit, attack.  Else flee! */
		    if ( door == -1 )
                        ;
/* Commented this out for now...too many bugs at the moment  - Lithos
			kill_ch( ch, rch );
*/
		    else
			move_char( ch, door );
		    break;
		}
	    }
	}

        /* do this last, low priority  - Lithos */
        if ( !ch->fighting && !ch->hunting
          && ch->position == POS_STANDING
          && IS_SET( ch->act, ACT_SENTINEL )
          && ch->load_room_vnum
          && ch->load_room_vnum != ch->in_room->vnum ) {
            return_to_load( ch );
        }

    }
    return;
}



/*
 * Update the weather.
 */
// Also updates the game hour every pulse point and ticks
// off time till next firstaid for characters
void weather_update( void )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    int              diff;

    last_fun( "weather_update");
    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  4:
	weather_info.sunlight = MOON_SET;
        if( weather_info.moonphase != MOON_NEW )
  	  strcat( buf, "&+cThe &+Cmoon&n&+c slowly &+Lvanishes&n&+c from the horizon.&n\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "&+cThe first &+Clights&n&+c of &+md&+Maw&n&+mn &n&+cilluminate the&+C sky&n.\n\r");
	weather_info.temperature += number_fuzzy( 10 );
	break;

    case  7:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "&+MA new day has begun as the &+Ysun&+M rises&n.&n\n\r" );
	if ( time_info.month <= 4 || time_info.month >= 15 )
	    weather_info.temperature = number_fuzzy( 20 );
	else
	    weather_info.temperature = number_fuzzy( 50 );
	break;

    case  12:
	strcat( buf, "&+cThe &+Ysun&n &+cnow hangs at high noon&n.\n\r" );
	weather_info.temperature += number_fuzzy( 20 );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "&+mThe &+Ysun&n&+m slowly slips off the &+Mhorizon&n.\n\r" );
	weather_info.temperature -= number_fuzzy( 20 );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "&+LThe night begins as darkness settles across the lands&n.\n\r" );
	weather_info.temperature -= number_fuzzy( 10 );
	break;

    case 24:
	weather_info.sunlight = MOON_RISE;
	weather_info.temperature -= number_fuzzy( 10 );
	time_info.hour = 0;
	time_info.day++;

        // Moon stuff
        weather_info.moonday++;

        if( weather_info.moonday >= LUNAR_CYCLE_DAYS )
        {
          weather_info.moonphase = MOON_NEW;
          weather_info.moonday = 0;
        }
        else
        {
          // Waxing moon
          if( weather_info.moonday < LUNAR_CYCLE_DAYS / 2 )
          {
            // The actual number of mmon phases total waxming and waning is MAX_MOON_PHASE x 2
            // so we divide the total days in a cycle by the total phases to get the number of
            // days in each phase
            weather_info.moonphase = weather_info.moonday / (LUNAR_CYCLE_DAYS / (MAX_MOON_PHASE * 2));
          }
          // Waning moon
          else
          {
            // Since it's waning we count back down from MAX_MOON_PHASE to zero.
            weather_info.moonphase = (MAX_MOON_PHASE * 2) - (weather_info.moonday / (LUNAR_CYCLE_DAYS / (MAX_MOON_PHASE * 2)));
          }          

        }

        switch( weather_info.moonphase )
        {
           default:
             break;
           case MOON_NEW:
             strcat( buf, "&+LThe night sky is overshadowed by an uncommon darkness.&n\n\r" );
             break;
           case MOON_FULL:
             strcat( buf, "&n&+LThe &+Cmoon&n&+L rises full, casting a &n&+wsi&+Wlv&n&+wer &n&+cglow&+L across the entire sky.&n\n\r" );
             break;
           case MOON_THREEQUARTER:
             strcat( buf, "&n&+LThe &+Cmoon&n&+L ascends, a small &n&+csliver&+L absent against the night sky.&n\n\r" );
             break;
           case MOON_HALF:
             strcat( buf, "&n&+LA giant half-circle, the &+Cmoon&n&+L rises against the blanket of night.&n\n\r" );
             break;
           case MOON_QUARTER:
             strcat( buf, "&n&+LThe &+Cmoon&n&+L rises, a &n&+wsi&+Wlv&n&+wer &+csliver&n&+L against the dark firmament.&n\n\r" );
             break;
        }

	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    weather_info.winddir += number_range( 0, 2 ) - 1;

    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice( 1, 4 ) + dice( 2, 6 ) - dice( 2, 6 );
    weather_info.change    = UMAX( weather_info.change, -12 );
    weather_info.change    = UMIN( weather_info.change,  12 );

    weather_info.mmhg     += weather_info.change;
    weather_info.mmhg      = UMAX( weather_info.mmhg,  960 );
    weather_info.mmhg      = UMIN( weather_info.mmhg, 1040 );

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if (     weather_info.mmhg <  990
	    || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wA few &+Wf&n&+wl&+Wa&n&+wk&+We&n&+ws of &+Ws&n&+wn&+Wo&n&+ww&+w are falling&n.\n\r" );
		weather_info.temperature -= 10;
	    }
	    else
		strcat( buf, "&+LStorm clouds &n&+mthunder&+L in the distance&n.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	    weather_info.windspeed += 10;
	}
	break;

    case SKY_CLOUDY:
	if (     weather_info.mmhg <  970
	    || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{

	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wThe &+Wharsh s&n&+wn&+Wo&n&+ww-&+Lstorm&n&+w makes visibility difficult&n.\n\r");
		weather_info.temperature -= 10;
	    }
	    else
		strcat( buf, "&+cSmall drops of &+Crain&n&+w m&+Wis&n&+wt the air&n.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    weather_info.windspeed += 10;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wThe &+Wsnow&n&+w-&+Lstorm&n&+w seems to settle&n.\n\r" );
		weather_info.temperature += 10;
	    }
	    else
		strcat( buf, "&+cThe &+Cclouds&n&+c disappear from the skyline&n.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	    weather_info.windspeed -= 10;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wThe &+Wsnow-&+cstorm&n&+w has evolved into a full &+Cblizzard&n.\n\r" );
		weather_info.temperature -= 30;
	    }
	    else
		strcat( buf, "&+WLightning flashes across the sky&n.\n\r");
	    weather_info.sky = SKY_LIGHTNING;
	    weather_info.windspeed += 10;
	}

	if (     weather_info.mmhg > 1030
	    || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wThe &+Ws&n&+wn&+Wo&n&+ww seems to be letting up&n.\n\r" );
		weather_info.temperature += 30;
	    }
	    else
		strcat( buf, "&+cThe &+Crain&n&+c slows to a drizzle then quits&n.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	    weather_info.windspeed -= 10;
	}
	break;

    case SKY_LIGHTNING:
	if (     weather_info.mmhg > 1010
	    || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "&+wThe &+Wblizzard&n&+w subsides&n.\n\r" );
		weather_info.temperature += 10;
	    }
	    else
		strcat( buf, "&n&+wThe &+Lthunder &N&+wand &+Clightning&n&+w has stopped&N.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    weather_info.windspeed -= 10;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& IS_OUTSIDE( d->character )
                && !is_underground( d->character )
		&& IS_AWAKE  ( d->character )
                && !IS_ROOM( d->character->in_room, ROOM_NO_PRECIP ) )
		send_to_char( buf, d->character );
	}
    }

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( ( d->connected == CON_PLAYING ) &&
                  !IS_NPC( d->character) )
               if( d->character->pcdata->firstaid > 0 )
                  d->character->pcdata->firstaid -= 1;
	}

    return;
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    time_t     save_time;
    static int hunger = 0;

    last_fun( "char_update");
    hunger = ((hunger + 1) % 16 );

    ch_save	= NULL;
    ch_quit	= NULL;
    save_time	= current_time;

    for ( ch = char_list; ch; ch = ch_next )
    {
	AFFECT_DATA *paf;

        ch_next = ch->next;

	if ( ch->deleted || !ch->in_room )
	    continue;

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

        if( ch->position == POS_DEAD )
        {
            send_to_char( "&+lYour soul finally leaves your body.&n\n\r", ch );
            act( "&+l$n&+l's corpse grows &+bcold&+l.", ch, NULL, NULL, TO_ROOM );
            raw_kill( ch, ch );
            continue;
        }

        if (ch->position == POS_FIGHTING && !ch->fighting)
            ch->position = POS_STANDING;

        if ( !IS_NPC( ch ) ) update_innate_timers( ch );

	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		; /* perm affect; not on a timer. */
	    else
	    {
		if ( !paf->next
		    || (paf->next->skill != paf->skill
                    && paf->next->spell != paf->spell)
		    || paf->next->duration > 0 )
		{
		    if ( paf->skill > 0 && skills_table[paf->skill].msg_off )
		    {
			send_to_char( skills_table[paf->skill].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		    if ( paf->spell > 0 && spells_table[paf->spell].msg_off )
		    {
			send_to_char( spells_table[paf->spell].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		    if ( paf->song > 0 && songs_table[paf->song].msg_off )
		    {
			send_to_char( songs_table[paf->song].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}

		if ( paf->skill == gsn_vampiric_bite )
		    ch->race = RACE_VAMPIRE;

		affect_remove( ch, paf );
	    }
	}

        if( is_affected( ch, AFF_VACANCY ) && !is_affected( ch, AFF_HIDE ))
        {
           send_to_char( "You become part of your surroundings.\n\r", ch );
           act( "$n&n fades from view.", ch, NULL, NULL, TO_ROOM );
           SET_AFF_BIT( ch, AFF_HIDE );
        }

        /*
         * Careful with the damages here,
         *   MUST NOT refer to ch after damage taken,
         *   as it may be lethal damage (on NPC).
         */
	if ( has_affect( ch, 0, spl_plague, 0 ) )
	{
	    AFFECT_DATA *af;
	    CHAR_DATA   *vch;
	    AFFECT_DATA  plague;
	    int          save;
	    int          dam;

	    act( "$n&n writhes in agony as plague sores erupt from $s skin.",
		ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You writhe in agony from the plague.\n\r", ch );
	    for ( af = ch->affected; af; af = af->next )
                if ( af->spell == spl_plague ) break;

	    if ( !af )
	    {
                REMOVE_AFF_BIT( ch, AFF_PLAGUE );
                continue;
	    }

	    if ( af->level == 1 )
		continue;

            plague.skill        = 0;
	    plague.spell	= spl_plague;
	    plague.level	= af->level - 1;
	    plague.duration	= number_range( 1, 2 * plague.level );
	    plague.location	= APPLY_STR;
	    plague.modifier	= -5;
	    set_bitvector( &plague, AFF_PLAGUE);
	    save		= plague.level;

	    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	    {
		if ( vch->deleted )
		    continue;

		if ( save && !saves_spell( save, vch, DAM_DISEASE )
		    && !IS_IMMORTAL( vch )
		    && !is_affected( vch, AFF_PLAGUE )
		    && number_bits( 4 ) == 0 )
		{
		    send_to_char( "You feel hot and feverish.\n\r", vch );
		    act( "$n&n shivers and looks very ill.", vch, NULL, NULL,
			TO_ROOM );
		    affect_join( vch, &plague );
                }
	    }

	    dam = UMIN( ch->level, 5 );
	    ch->mana -= dam;
	    ch->move -= dam;
	    spell_damage( ch, ch, dam, spl_plague, DAM_DISEASE );
        }

	if ( ( time_info.hour > 5 && time_info.hour < 21 )
	    && CHECK_SUS( ch, RIS_LIGHT )
            && ( FALSE ) // used to disable this code - Veygoth
	    && !IS_ROOM( ch->in_room, ROOM_DARK ) )
	{
	    int dmg = 0;

            // Shouldn't this mess be a switch statement? - Veygoth
            if( is_underground( ch ))
            {
                dmg = 0;
            }
	    if ( ch->in_room->sector_type == SECT_INSIDE )
	    {
	        dmg = 1;
	    }
	    else
	    {
		if ( ch->in_room->sector_type == SECT_FOREST ||
                     ch->in_room->sector_type == SECT_SWAMP )
		{
		    dmg = 2;
		}
		else
		{
		    dmg = 4;
		}
	    }
	    
	    if ( weather_info.sky == SKY_CLOUDY )
	        dmg /= 2;
	    if ( weather_info.sky == SKY_RAINING )
	      {
		dmg *= 3;
	        dmg /= 4;
	      }

            send_to_char( "&+RThe heat of the sun feels terrible!&n\n\r", ch );
	    spell_damage( ch, ch, dmg, spl_poison, DAM_LIGHT );
	}
	else if (   ch->in_room->sector_type == SECT_UNDERWATER
	        && ( !IS_IMMORTAL( ch ) && !is_affected( ch, AFF_GILLS )
                    && !CHECK_IMM( ch, RIS_DROWNING )
		    && !IS_SET( race_table[ ch->race ].race_abilities, RACE_WATERBREATH ) ) )
        {
            send_to_char( "You can't breathe!\n\r", ch );
            act( "$n&n sputters and chokes!", ch, NULL, NULL, TO_ROOM );
            ch->hit -= 5;
            update_pos( ch );
        }
        else if (  ch->in_room->sector_type != SECT_UNDERWATER
                && ch->in_room->sector_type != SECT_WATER_NOSWIM
                && ch->in_room->sector_type != SECT_WATER_SWIM
                && IS_SET( race_table[ ch->race ].race_abilities,  RACE_WATERBREATH )
                && str_cmp( race_table[ch->race].name, "Object" ) 
                && ch->race != RACE_GOD )
        {
            send_to_char( "You can't breathe!\n\r", ch );
            act( "$n&n sputters and gurgles!", ch, NULL, NULL, TO_ROOM );
            ch->hit -= 5;
            update_pos( ch );
        }
        else if ( is_affected( ch, AFF_POISON ) )
        {
            apply_poison( ch );
        }
        else if ( ch->position == POS_INCAP )
        {
            sprintf(lbuf,"char_update: %s&n is incapacitated.",ch->name);
            wiznet( 0, WIZ_LSPAM, 0, lbuf );
	    if ( ch->race == RACE_TROLL && ch->hit < get_max_hit( ch ) )
		ch->hit++;
	    else if( !IS_NPC( ch ))
            {
            	damage( ch, ch, 1, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
            }
            else if( number_percent() < 50 )
               ch->hit--;
        }
        else if ( ch->position == POS_MORTAL )
        {
            sprintf(lbuf,"char_update: %s&n is mortally wounded.",ch->name);
            wiznet( 0, WIZ_LSPAM, 0, lbuf );
	    if ( ch->race == RACE_TROLL && ch->hit < get_max_hit( ch ))
            {
 		 ch->hit++;
            }
	    else if( !IS_NPC( ch ))
            {
                damage( ch, ch, 2, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
            }
            else
            {
                damage( ch, ch, 1, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
            }
        }
        update_pos( ch );

        // Refresh stoneskin on perm stone mobs.
        if( IS_NPC( ch ) && IS_SET( ch->pIndexData->affected_by[AFF_STONESKIN.group], AFF_STONESKIN.vector ) &&
            !is_affected( ch, AFF_STONESKIN ))
        {
            SET_AFF_BIT( ch, AFF_STONESKIN );
            act( "$n&+L's skin turns to stone.&n", ch, NULL, NULL, TO_ROOM );
        }

	/* Thats all for mobs */
        if ( IS_NPC( ch ) )
	    continue;
     
        if ( ch->position == POS_DEAD )
        {
            raw_kill( ch, ch );
            continue;
        }

	/*
	 * Find dude with oldest save time.
	 */
	if (   ( !ch->desc || ch->desc->connected == CON_PLAYING )
	    &&   ch->level >= 2
	    &&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if ( ( ch->level < LEVEL_IMMORTAL
	    || ( !ch->desc && !IS_SWITCHED( ch ) ) ) )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_HAND ) )
		&& obj->item_type == TYPE_LIGHT
		&& obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room )
		{
		    --ch->in_room->light;
		    act( "$p&n goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p&n goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if ( ch->timer > 15 && !IS_SWITCHED( ch ) )
		ch_quit = ch;

            // This is so that hunger updates once every 16 char_updates,
            // This is so that thirst updates once every 16 char_updates,
            // This is so that drunkeness updates once every 2
            //   char_updates, all at different intervals. - Lohrr
            if( ( hunger % 2 ) == 1 )
                gain_condition( ch, COND_DRUNK,  -1 );
            if( hunger == 4 )
                gain_condition( ch, COND_FULL, -1 );
            if( hunger == 8 )
                gain_condition( ch, COND_THIRST, -1 );
            if( has_affect( ch, 0, spl_thirst, 0 ) )
                gain_condition( ch, COND_THIRST, -2 );
            if( ( ch->hit - get_max_hit( ch ) ) >  50 
                && ( hunger % 5 == 0 ) )
                ch->hit--;
            else if( ( ch->hit - get_max_hit( ch ) ) > 100 
                && ( hunger % 4 == 0 ) )
                ch->hit--;
            else if( ( ch->hit - get_max_hit( ch ) ) > 150 )
                ch->hit--;
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save || ch_quit )
    {
	for ( ch = char_list; ch; ch = ch->next )
	{
	    if ( ch->deleted )
	        continue;
	    if ( ch == ch_save )
            {
                send_to_char( "Autosaving...\n\r", ch );
		save_char_obj( ch );
            }
	    if ( ch == ch_quit )
		do_camp( ch, "" );
	}
    }

    return;
}



/*
 * Update all rooms.
 * This function is performance sensitive.
 *
 * So far we only check fall chance and
 * fire plane flags during a room update
 * - Veygoth
 *
 * Also check fall from losing fly, and do track updates
 * - Lithos
 */
void room_update( void )
{   
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;
    CHAR_DATA       *rch;
    CHAR_DATA       *rch_next;
    ROOM_INDEX_DATA *new_room;
    ROOM_INDEX_DATA *pRoomIndex;
    int              iHash;
    int              ris;

    last_fun( "room_update");
    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoomIndex  = room_index_hash[iHash];
              pRoomIndex;
              pRoomIndex  = pRoomIndex->next )
        {
            if( pRoomIndex->sector_type == SECT_PLANE_FIRE )
            {
  	      for ( rch = pRoomIndex->people; rch; rch = rch_next )
              {
		rch_next = rch->next_in_room;

                if( rch->deleted )
                  continue;
              if( (!IS_NPC(rch) && rch->level >= LEVEL_IMMORTAL)
                 || is_affected( rch, AFF_DENY_FIRE )
                 || rch->race == RACE_ELEM_FIRE )
                  continue;
                ris = check_ris( rch, DAM_FIRE );
                if( ris == IS_SUSCEPTIBLE )
                {
                  rch->hit -= 6;
                }
                if( ris == IS_NORMAL )
                {
                  rch->hit -= 4;
                }
                if( ris == IS_RESISTANT )
                {
                  rch->hit -= 2;
                }
                if( ris == IS_IMMUNE )
                {
                  continue;
                }
                send_to_char( "&+rYou get burned from the heat in the room.\n\r", rch );
                update_pos( rch );
		if ( rch->position == POS_DEAD )
                {
                    raw_kill( rch, rch );
                    last_fun( "room_update");
                }
              }
            }
            if( pRoomIndex->sector_type == SECT_PLANE_AIR 
                && number_percent() < 20 )
            {
  	      for ( rch = pRoomIndex->people; rch; rch = rch_next )
              {
		rch_next = rch->next_in_room;

                if( rch->deleted )
                  continue;
              if( (!IS_NPC(rch) && rch->level >= LEVEL_IMMORTAL)
                 || is_affected( rch, AFF_DENY_AIR )
                 || rch->race == RACE_ELEM_AIR 
                 || check_shrug( rch, rch ) )
                  continue;
                ris = check_ris( rch, DAM_WIND );
                if( ris == IS_SUSCEPTIBLE )
                {
                  rch->hit -= 6;
                }
                if( ris == IS_NORMAL )
                {
                  rch->hit -= 4;
                }
                if( ris == IS_RESISTANT )
                {
                  rch->hit -= 2;
                }
                if( ris == IS_IMMUNE )
                {
                  continue;
                }
                send_to_char( "&+CYou're hit by a bolt of lightning!\n\r", rch );
                update_pos( rch );
		if ( rch->position == POS_DEAD )
                {
                    raw_kill( rch, rch );
                    last_fun( "room_update");
                }
              }
            }


/*  Check for falling char and objects from fly_level > 0
    -- Lithos
*/
           if ( is_fly_room( pRoomIndex ) ) {
                for ( rch = pRoomIndex->people; rch; rch = rch->next_in_room) {
                    if (rch->deleted || rch->fly_level == 0)
                        continue;
                    if (rch->fly_level < 0) {
                        bug ("Mob has a fly_level less than 0", 0);
                        rch->fly_level = 0;
                        continue;
                    }
                    if ( can_fly( rch ) 
                      || is_affected( rch, AFF_LEVITATE)
                      || ( !IS_NPC(rch) && rch->level >= LEVEL_IMMORTAL ) )
                        continue;
                    // ouch, gonna fall!
                    if ( !IS_NPC( rch ) ) send_to_char("You fall tumbling down!\n\r", rch);
                    act("$n&n falls away.", rch, NULL, rch, TO_ROOM);
                    rch->position = POS_SITTING;
	//                    damage(rch, rch, dice(rch->fly_level, 10), NULL, DAM_OTHER);
                    for(rch->fly_level--; rch->fly_level; rch->fly_level-- ) {
                        act("$n&n falls past from above.", rch, NULL, NULL, TO_ROOM);
                    }
                    rch->fly_level = 0;
                    act("$n&n falls from above.", rch, NULL, rch, TO_ROOM);
                } //end of people falling from fly

                for (obj = pRoomIndex->contents; obj; obj = obj_next ) {
                    obj_next = obj->next_content;
                    if (obj->deleted || obj->fly_level == 0) continue;
                    if ( !IS_SET(obj->wear_flags, ITEM_TAKE) ) continue;
                    if ( IS_OBJ_STAT( obj, ITEM_LEVITATES) ) continue;
                    //object falls
                    //cant use act() withou knowing who is in room
                    for(rch = pRoomIndex->people; rch; rch = rch->next_in_room) {
                        if (rch->deleted) continue;
                        if (rch->fly_level == obj->fly_level)
                            act("$p&n falls away.",rch,obj,rch,TO_CHAR);
                        if (rch->fly_level && rch->fly_level < obj->fly_level)
                            act("$p&n falls past you from above.",rch,obj,rch,TO_CHAR);
                        if (rch->fly_level == 0)
                            act("$p&n falls from above.",rch,obj,rch,TO_CHAR);
                    }
                    obj->fly_level = 0;
                }
            }
//end of fall from fly_level

// do track update here
            for (rch = pRoomIndex->people; rch; rch = rch_next ) {
                rch_next = rch->next_in_room;
                if ( rch->deleted ) continue;
                if ( rch->hunting && rch->wait <= 0 ) {
                    if ( IS_NPC( rch ) )
                        WAIT_STATE( rch, 2 * PULSE_VIOLENCE );
                    hunt_victim( rch );
                }
            }

//now check for falling from room
            if( !pRoomIndex->exit[5] || !(new_room = pRoomIndex->exit[5]->to_room ))
                continue;
            if ( IS_SET( pRoomIndex->exit[5]->exit_info, EX_BLOCKED )
                || IS_SET( pRoomIndex->exit[5]->exit_info, EX_CLOSED ) )
                continue;

            if( pRoomIndex->sector_type != SECT_AIR &&
                pRoomIndex->sector_type != SECT_PLANE_AIR &&
                pRoomIndex->sector_type != SECT_UNDERG_NOGROUND )
            {
  	      if ( !pRoomIndex->fall_chance )
		continue;
            }

	    for ( obj = pRoomIndex->contents; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( obj->deleted )
		    continue;

		if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) )
		    continue;

                if ( IS_OBJ_STAT(obj, ITEM_LEVITATES) )
                    continue;

		if ( ( rch = obj->in_room->people ) )
		{
		    act( "$p&n falls away.", rch, obj, NULL, TO_ROOM );
		    act( "$p&n falls away.", rch, obj, NULL, TO_CHAR );
		}
	
		obj_from_room( obj );
		obj_to_room( obj, new_room );
	
		if ( ( rch = obj->in_room->people ) )
		{
		    act( "$p&n falls by.", rch, obj, NULL, TO_ROOM );
		    act( "$p&n falls by.", rch, obj, NULL, TO_CHAR );
		}
	    }

	    for ( rch = pRoomIndex->people; rch; rch = rch_next )
	    {
		rch_next = rch->next_in_room;

		if ( rch->deleted )
		    continue;

                check_fall( pRoomIndex, new_room, rch );
	    }
        }
    }

    return;
}

// By Veygoth so we can check for fall in both movement and in room update.
void check_fall( ROOM_INDEX_DATA *pRoomIndex, ROOM_INDEX_DATA *target, CHAR_DATA *ch )

{
      int chance;

      last_fun( "check_fall");
      if( !pRoomIndex || !target || !ch || ch->deleted )
          return;
      if( pRoomIndex->sector_type != SECT_AIR &&
          pRoomIndex->sector_type != SECT_PLANE_AIR &&
          pRoomIndex->sector_type != SECT_UNDERG_NOGROUND )
      {
        if ( number_percent() > pRoomIndex->fall_chance )
          return;
      }

      if ( can_fly( ch ) || is_affected( ch, AFF_LEVITATE ))
          return;

      if ( ch->in_room->people )
      {
          act( "You are falling down!", ch, NULL, NULL, TO_CHAR );
          act( "$n&n falls away.",      ch, NULL, NULL, TO_ROOM );
      }

      char_from_room( ch );
// What the hell is this? NPC's disappear from the MUD when they fall??
// Umm.. no!  - Lohrr
//      if ( ! IS_NPC ( ch ) )
        char_to_room( ch, target );

      if( ch->level < skills_table[gsn_safe_fall].skill_level[ch->class] )
        chance = 0;
      else if( IS_NPC( ch ))
        chance = ((ch->level * 3) / 2) + 15;
      else
        chance = ch->pcdata->skl_lrn[gsn_safe_fall];

      // People with high agility have a small chance to safe fall, and those with
      // the skill already get a bonus.
      chance += (get_curr_agi( ch ) / 20);

      // Minimum 1% chance of a bad fall.
      if( chance > 99 )
        chance = 99;

      // Safe fall added by Veygoth
      if ( !target->fall_chance
          || !target->exit[5]
          || !target->exit[5]->to_room )
      {
          if( number_percent() < chance )
          {
            // Decent chance of skill increase -- people don't fall very often.
            skill_practice( ch, gsn_safe_fall );
            skill_practice( ch, gsn_safe_fall );
            skill_practice( ch, gsn_safe_fall );
            skill_practice( ch, gsn_safe_fall );
            send_to_char ( "You fall to the ground!\n\r", ch);

            if( number_percent() < chance )
            {
                  act( "$n&n falls from above and lands gracefully.", ch, NULL, NULL, TO_ROOM);
                  send_to_char ( "You land gracefully, avoiding any injury.\n\r", ch);
            }
            else
            {
              act( "$n&n falls from above and lands on $s arse.", ch, NULL, NULL, TO_ROOM);
              if (MAX_SIZE > 0  && !IS_NPC( ch ) )
              {
                  damage( ch, ch, number_range( 2, 4 ), TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
                  ch->position = POS_SITTING;
                  WAIT_STATE( ch, 3 );
              }
            }
          }
          else
          {
            send_to_char ( "You slam into the ground!\n\r", ch);
            ch->position = POS_SITTING;
            WAIT_STATE( ch, 8 );
            act( "$n&n comes crashing in from above.", ch, NULL, NULL, TO_ROOM);
            if (MAX_SIZE > 0  && !IS_NPC( ch ) )
            {
                damage( ch, ch, (( number_percent () * ch->size ) / MAX_SIZE)
                      , TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
            }
          }
      }
      else if ( ch && ch->in_room )
      {
         if ( ch->in_room->people )
         act( "$n&n falls by.", ch, NULL, NULL, TO_ROOM );
      }

      return;
}


/*
 * Update all objects.
 * This is only used for object special functions.
 * This function is performance sensitive.
 */
void obj_spec_update( void )
{   
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;

    last_fun( "obj_spec_update");
    for ( obj = object_list; obj; obj = obj_next )
    {
	obj_next = obj->next;

	if ( obj->deleted )
	    continue;

	/* Examine call for special procedure */
	if ( obj->spec_fun != 0 )
	{
	    if ( ( *obj->spec_fun ) ( obj, obj->carried_by, FALSE ) )
		continue;
	}
    }

    return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 * This function just decrements object timers.
 */
void obj_update( void )
{   
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    OBJ_DATA  *obj_prev;
    OBJ_DATA  *c_obj;
    OBJ_DATA  *c_obj_next;

    CHAR_DATA *rch;
    char      *message;

    last_fun( "obj_update");
    for ( obj = object_list, obj_prev = NULL; obj; obj = obj_next )
    {
	obj_next = obj->next;

	if ( obj->deleted )
	{
	    obj_prev = obj;
	    continue;
	}

	if ( obj->timer < -1 )
	    obj->timer = -1;

	if ( obj->timer < 0 )
	{
	    obj_prev = obj;
	    continue;
	}

	/*
	 *  Bug fix:  used to shift to obj_free if an object whose
	 *  timer ran out contained obj->next.  Bug was reported
	 *  only when the object(s) inside also had timer run out
	 *  during the same tick.     --Thelonius (Monk)
	 */
	if ( --obj->timer == 0 )
	{
	    switch ( obj->item_type )
	    {
	    	default:              message = "$p&n vanishes.";         break;
    		case TYPE_DRINK_CON:  message = "$p&n dries up."; break;
    		case TYPE_CORPSE_NPC: message = "$p&n decays into dust."; break;
    		case TYPE_CORPSE_PC:  message = "$p&n decays into dust."; break;
    		case TYPE_FOOD:       message = "$p&n decomposes."; break;
            case TYPE_PORTAL:     message = "$p&n fades out of existence."; 
				break;
            case TYPE_WALL:
                message = wall_decay_msg( obj->pIndexData->vnum);
              if( obj->in_room && obj->in_room->exit[obj->value[0]] )
                REMOVE_BIT( obj->in_room->exit[obj->value[0]]->exit_info, EX_WALLED );
              break;
	    }
    
	    if ( obj->carried_by )
	    {
	        act( message, obj->carried_by, obj, NULL, TO_CHAR );
	    }
	    else
	      if ( obj->in_room
		  && ( rch = obj->in_room->people ) )
	      {
		  act( message, rch, obj, NULL, TO_ROOM );
		  act( message, rch, obj, NULL, TO_CHAR );
	      }
    
	    /* Govno - Don't nuke contents of containers upon decay  */
	    if ( obj->item_type == TYPE_CORPSE_PC  || \
                 obj->item_type == TYPE_CORPSE_NPC || \
		 obj->item_type == TYPE_CONTAINER     )
	    {
		 for ( c_obj = obj->contains; c_obj; c_obj = c_obj_next )
		 {
		     c_obj_next = c_obj->next_content;

		     obj_from_obj( c_obj );
		     obj_to_room( c_obj, obj->in_room );
		 }
	    }

	    if ( obj == object_list )
	    {
	        extract_obj( obj );
   
	        obj_next = object_list;
	        obj_prev = NULL;
	        continue;
	    }

					    /* (obj != object_list) */
	    if ( !obj_prev )  /* Can't see how, but... */
		bug( "Obj_update: obj %d no longer in object_list",
		    obj->pIndexData->vnum );

	    extract_obj( obj );
    
	    obj_next = obj_prev->next;
	}

	obj_prev = obj;
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes .2% of total CPU time.
 *
 * -Kahn
 */
void aggr_update( void )
{
    CHAR_DATA       *ch;
    CHAR_DATA       *mch;
    CHAR_DATA       *vch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ACT_PROG_DATA   *apdtmp;

    last_fun( "aggr_update");

    while ( ( apdtmp = mob_act_list ) )
    {
        mch = mob_act_list->vo;
        if ( !mch->deleted && mch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST *tmp_act;

	    while ( ( tmp_act = mch->mpact ) )
            {
		if ( tmp_act->obj && tmp_act->obj->deleted )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !tmp_act->ch->deleted )
		  prog_wordlist_check( tmp_act->buf, tmp_act->ch, mch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		mch->mpact = tmp_act->next;
		free_string( tmp_act->buf );
		tmp_act->buf = NULL;
		free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
	    }
	    mch->mpactnum = 0;
	    mch->mpact    = NULL;
	}
        mob_act_list = apdtmp->next;
        free_mem( apdtmp, sizeof( ACT_PROG_DATA ) );
    }

    /*
     * Let's not worry about link dead characters. -Kahn
     */
    for ( d = descriptor_list; d; d = d->next )
    {
	ch = d->character;

	if ( d->connected != CON_PLAYING
	    || (ch->level >= LEVEL_HERO && IS_SET( ch->act, PLR_FOG ))
	    || !ch->in_room )
	    continue;

	/* mch wont get hurt */
	for ( mch = ch->in_room->people; mch; mch = mch->next_in_room )
	{
	    int count;
	    bool hate = FALSE;

	    if ( !IS_NPC( mch )
		|| mch->deleted
		|| mch->fighting
		|| is_affected( mch, AFF_CHARM )
		|| !IS_AWAKE( mch )
		|| ( IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )
                || ch->fly_level != mch->fly_level
                || IS_IMMORTAL( ch )
		|| !can_see( mch, ch ) ) {
		continue; //next mch
            }
            hate = is_aggro( mch, ch );
            if ( !hate ) continue;

            if ( is_hating( mch, ch ) )
            {
                found_prey( mch, ch );
                continue;
            }

	    /*
	     * Ok we have a 'ch' player character and a 'mch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count  = 0;
	    victim = NULL;
	    for ( vch = mch->in_room->people; vch; vch = vch->next_in_room )
	    {
	        if ( IS_NPC( vch )
		    || vch->deleted
		    || vch->level >= LEVEL_IMMORTAL
                    || vch->fly_level != mch->fly_level
                    || !can_see( mch, vch )
                    || !is_aggro( mch, vch )  )
		    continue;
		if ( ( !IS_SET( mch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) )
		    && can_see( mch, vch ) )
		{
		        if ( number_range( 0, count ) == 0 )
			    victim = vch;
			count++;
		}
	    } /* vch loop */

	    if ( !victim )
	        continue; //next mch
            if ( !check_aggressive( victim, mch ) ) {
                if ( mch->position == POS_STANDING )
                    kill_ch( mch, victim );
                else if ( mch->position >= POS_RECLINING ) {
                    act("$n scrambles to an upright position.", mch, NULL, NULL, TO_ROOM);
                    mch->position = POS_STANDING;
                }
            }

	} /* mch loop */

    } /* descriptor loop */

    return;
}

/* Heartbeat file update    -Govno-             */
void heartbeat_update( void )
{
    FILE	*fp;
//    char	buf [ MAX_STRING_LENGTH ];

    if ( !( fp = fopen( HEARTBEAT_FILE, "w" ) ) )
    {
        perror( HEARTBEAT_FILE );
	bug( "Could not open the Heartbeat file!", 0 );
    }
    else
    {
        fclose( fp );
    }
}

/* Update the check on time for autoshutdown */
void time_update( void )
{
    FILE            *fp;
    char             buf [ MAX_STRING_LENGTH ];
    
    last_fun( "time_update");
    if ( down_time <= 0 )
        return;
    if ( current_time > warning1 && warning1 > 0 )
    {
	sprintf( buf, "First Warning!\n\r%s in %d minutes or %d seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown",
		(int) ( down_time - current_time ) / 60,
		(int) ( down_time - current_time ) );
	send_to_all_char( buf );
	warning1 = 0;
    }
    if ( current_time > warning2 && warning2 > 0 )
    {
	sprintf( buf, "Second Warning!\n\r%s in %d minutes or %d seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown",
		(int) ( down_time - current_time ) / 60,
		(int) ( down_time - current_time ) );
	send_to_all_char( buf );
	warning2 = 0;
    }
    if ( current_time + 10 > down_time && warning2 == 0 )
    {
	sprintf( buf, "Final Warning!\n\r%s in 10 seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown" );
	send_to_all_char( buf );
	warning2--;
    }
    if ( current_time > down_time )
    {
        sprintf( buf, "%s by system.\n\r", Reboot ? "Reboot" : "Shutdown" );
	send_to_all_char( buf );
	log_string( buf );

	end_of_game( );

	if ( !Reboot )
	{
            char strsave[MAX_STRING_LENGTH];
            sprintf( strsave, "%s%s%s", data_directory, SYSTEM_DIR, SHUTDOWN_FILE );
	    fclose( fpReserve );
	    if ( !( fp = fopen( strsave, "a" ) ) )
	      {
		perror( strsave );
		bug( "Could not open the Shutdown file!", 0 );
	      }
	    else
	      {
		fprintf( fp, "Shutdown by System\n" );
		fclose ( fp );
	      }
	    fpReserve = fopen ( NULL_FILE, "r" );
	}
	merc_down = TRUE;
    }

    return;

}

/*
 * Remove deleted EXTRA_DESCR_DATA from objects.
 * Remove deleted AFFECT_DATA from chars and objects.
 * Remove deleted CHAR_DATA and OBJ_DATA from char_list and object_list.
 */
void list_update( void )
{
            CHAR_DATA   *ch;
            CHAR_DATA   *ch_next;
            OBJ_DATA    *obj;
            OBJ_DATA    *obj_next;
    extern  bool         delete_obj;
    extern  bool         delete_char;

    last_fun( "list_update");
    if ( delete_char )
        for ( ch = char_list; ch; ch = ch_next )
	  {
	    AFFECT_DATA *paf;
	    AFFECT_DATA *paf_next;
	    
	    for ( paf = ch->affected; paf; paf = paf_next )
	      {
		paf_next = paf->next;
		
		if ( paf->deleted || ch->deleted )
		  {
		    if ( ch->affected == paf )
		      {
			ch->affected = paf->next;
		      }
		    else
		      {
			AFFECT_DATA *prev;
			
			for ( prev = ch->affected; prev; prev = prev->next )
			  {
			    if ( prev->next == paf )
			      {
				prev->next = paf->next;
				break;
			      }
			  }
			
			if ( !prev )
			  {
			    bug( "List_update: cannot find paf on ch.", 0 );
			    continue;
			  }
		      }
		    
		    paf->next   = affect_free;
		    affect_free = paf;
		  }
	      }

	    ch_next = ch->next;
	    
	    if ( ch->deleted )
	      {
		if ( ch == char_list )
		  {
		    char_list = ch->next;
		  }
		else
		  {
		    CHAR_DATA *prev;

		    for ( prev = char_list; prev; prev = prev->next )
		      {
			if ( prev->next == ch )
			  {
			    prev->next = ch->next;
			    break;
			  }
		      }
		    
		    if ( !prev )
		      {
			char buf [ MAX_STRING_LENGTH ];
			
			sprintf( buf, "List_update: char %s not found.",
				ch->name );
			bug( buf, 0 );
			continue;
		      }
		  }
		
		free_char( ch );
	      }
	  }

    if ( delete_obj )
      for ( obj = object_list; obj; obj = obj_next )
	{
	  AFFECT_DATA      *paf;
	  AFFECT_DATA      *paf_next;
	  EXTRA_DESCR_DATA *ed;
	  EXTRA_DESCR_DATA *ed_next;

	  for ( ed = obj->extra_descr; ed; ed = ed_next )
	    {
	      ed_next = ed->next;
	      
	      if ( obj->deleted )
		{
		  free_string( ed->description );
		  free_string( ed->keyword     );
		  ed->next         = extra_descr_free;
		  extra_descr_free = ed;
		}
	    }

	  for ( paf = obj->affected; paf; paf = paf_next )
	    {
	      paf_next = paf->next;
	      
	      if ( obj->deleted )
		{
		  if ( obj->affected == paf )
		    {
		      obj->affected = paf->next;
		    }
		  else
		    {
		      AFFECT_DATA *prev;
		      
		      for ( prev = obj->affected; prev; prev = prev->next )
			{
			  if ( prev->next == paf )
			    {
			      prev->next = paf->next;
			      break;
			    }
			}

		      if ( !prev )
			{
			  bug( "List_update: cannot find paf on obj.", 0 );
			  continue;
			}
		    }
		  
		  paf->next   = affect_free;
		  affect_free = paf;
		}
	    }

	  obj_next = obj->next;

	  if ( obj->deleted )
	    {
	      if ( obj == object_list )
		{
		  object_list = obj->next;
		}
	      else
		{
		  OBJ_DATA *prev;
		  
		  for ( prev = object_list; prev; prev = prev->next )
		    {
		      if ( prev->next == obj )
			{
			  prev->next = obj->next;
			  break;
			}
		    }
		  
		  if ( !prev )
		    {
		      bug( "List_update: obj %d not found.",
			  obj->pIndexData->vnum );
		      continue;
		    }
		}


	      free_string( obj->name        );
	      free_string( obj->description );
	      free_string( obj->short_descr );
	      --obj->pIndexData->count;

	      obj->next	= obj_free;
	      obj_free	= obj;
	    }
	}

    delete_obj		= FALSE;
    delete_char		= FALSE;
    return;
}

/*
 * Update the ban file upon call.
 * Written by Tre of EnvyMud and modified by Kahn
 */
void ban_update( void )
{
    FILE      *fp;
    BAN_DATA  *pban;
    char       strsave [ MAX_INPUT_LENGTH ];

    last_fun( "ban_update");
    fclose( fpReserve );

    sprintf( strsave, "%s%s%s", data_directory, SYSTEM_DIR, BAN_FILE );

    if ( !( fp = fopen ( strsave, "w" ) ) )
    {
	bug( "Ban_update:  fopen of BAN_FILE failed", 0 );
    }
    else
    {
	for ( pban = ban_list; pban; pban = pban->next )
	    fprintf( fp, "%s~\n", pban->name );

	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return;

}

void apply_poison( CHAR_DATA *ch ) {
    int poison_type = 0, dam;
    AFFECT_DATA *paf;
    AFFECT_DATA af;
    bool is_spell = FALSE;

    last_fun( "apply_poison" );
    for( paf = ch->affected; paf; paf = paf->next ) {
      if (paf->deleted) continue;
      if ( paf->spell == spl_poison ) {
        is_spell = TRUE;
      }
      else if ( ( paf->skill == gsn_poison_weapon || paf->skill == gsn_poison_bite)
          && paf->location == APPLY_NONE) {
        if ( is_affected( ch, AFF_SLOW_POISON )  && number_bits(1) == 0 )
          continue;
        poison_type = paf->modifier;
        switch( poison_type ) {
          case POISON_DAMAGE:
            act("$n&n goes into a brief siezure as the poison courses through $s body.", ch, NULL, NULL, TO_ROOM);
            send_to_char("Your muscles twitch randomly as the poison courses through your body.\n\r", ch);
            dam = dice( 1, 10);
            if ( !saves_spell( paf->level, ch, DAM_POISON ) )
              damage( ch, ch, dam, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
            else
              damage( ch, ch, dam/2, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
            return;
            break;
          case POISON_ATTRIBUTES:
            if ( !saves_spell( paf->level, ch, DAM_POISON ) ) {
                int lev;
                lev = paf->level;
                affect_strip( ch, gsn_poison_weapon, 0, 0 );
                af.spell = 0;
                af.skill = gsn_poison;
                af.song = 0;
                af.modifier = 0 - dice( 1, 20);
                af.duration = lev / 4;
                af.level = lev;
                af.location = APPLY_STR;
                set_bitvector( &af, AFF_POISON );
                affect_to_char( ch, &af );

                af.location = APPLY_DEX;
                af.modifier = 0 - dice( 1, 20 );
                affect_to_char( ch, &af );

                af.location = APPLY_AGI;
                af.modifier = 0 - dice( 1, 20 );
                affect_to_char( ch, &af );

                af.location = APPLY_CON;
                af.modifier = 0 - dice( 1, 20 );
                affect_to_char( ch, &af );
                send_to_char("You suddenly feel quite weak as the poison is distributed through your body.&n\n\r", ch);
                act("$n&n pales visibly and looks much weaker.", ch, NULL, NULL, TO_ROOM);
                return;
            }
            else {
                send_to_char("You feel the poison working its way into your system.\n\r", ch);
                damage( ch, ch, 2, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
            }
            break;
          case POISON_DAMAGE_MAJOR:
            dam = dice( 10, 10 );
            act("$n&n screams in agony as the poison courses through $s body.", ch, NULL, NULL, TO_ROOM);
            send_to_char("&+RYour blood is on fire!&n\n\r", ch);

            if ( !saves_spell( paf->level, ch, DAM_POISON ) )
              damage( ch, ch, dam, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
            else
              damage( ch, ch, dam/2, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
            return;
            break;
          case POISON_MINOR_PARA:
            if ( !saves_spell( paf->level, ch, DAM_POISON ) ) {
                affect_strip( ch, gsn_poison_weapon, 0, 0 );
                af.skill = gsn_poison;
                af.song = 0;
                af.spell = 0;
                af.location = APPLY_NONE;
                af.duration = number_range(1, 10);
                af.modifier = 0;
                set_bitvector( &af, AFF_MINOR_PARA );
                affect_to_char( ch, &af );
                send_to_char("&+YYou are paralyzed!&n\n\r", ch);
                stop_fighting( ch, FALSE );
                act("$n&n&+y is suddenly overcome with rigor and cannot move.&n",
                    ch, NULL, NULL, TO_ROOM);
            }
            break;
          case POISON_MINOR_PARA_LONG:
            break;
          case POISON_MAJOR_PARA:
            break;
          case POISON_MAJOR_PARA_LONG:
            break;
          case POISON_PERM_CON:
            break;
          case POISON_PERM_HP:
            break;
          case POISON_NEAR_DEATH:
            break;
          default:
			  break;
        } //end switch
        
      } //end if
    } //end paf loop
    if ( is_spell ) {
        //normal poison from the spell 
        send_to_char( "You shiver and suffer.\n\r", ch );
        act( "$n&n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
        if( !is_affected( ch, AFF_SLOW_POISON ) )
        {
          spell_damage( ch, ch, 2, spl_poison, DAM_POISON );
        }
        else
        {
          // Slow poison gives them a 20% chance of avoiding damage
          // and does half damage when it does hit them, giving
          // them 40% as much damage overall as a non-slowed person
          if( number_percent() < 80 )
            spell_damage( ch, ch, 1, spl_poison, DAM_POISON );
        }
    }
    else 
    {
        //normal poison from a bite or weapon
        send_to_char( "You shiver and suffer.\n\r", ch );
        act( "$n&n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
        if( !is_affected( ch, AFF_SLOW_POISON ) )
        {
            damage( ch, ch, 2, gsn_poison, WEAR_NONE, DAM_POISON );
        }
        else
        {
          // Slow poison gives them a 20% chance of avoiding damage
          // and does half damage when it does hit them, giving
          // them 40% as much damage overall as a non-slowed person
          if( number_percent() < 80 )
            damage( ch, ch, 1, gsn_poison, WEAR_NONE, DAM_POISON );
        }
    }

}

