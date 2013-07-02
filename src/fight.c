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

char lbuf[MAX_STRING_LENGTH * 2];

/*
 * Local functions.
 */
bool	check_shout	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_shield_block   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				    int dt, int wpn, bool immune ) );
void	spl_dam_message      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				    int dt, bool immune ) );
void	death_cry	     args( ( CHAR_DATA *ch ) );
void	group_gain	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	     args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
int     is_wielding_poisoned args( ( CHAR_DATA *ch, int wpn ) );
void	make_corpse	     args( ( CHAR_DATA *ch ) );
bool	leaves_no_corpse     args( ( CHAR_DATA *ch ) );//Govno corpse yes/no.
void	one_hit		     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt,
				    int wpn ) );
void	raw_kill	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	set_fighting	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_boss              args( ( CHAR_DATA *ch ) );

bool    check_race_special   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    use_magical_item     args( ( CHAR_DATA *ch ) );

void 	blur_attack	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    stop_not_vicious     args( ( CHAR_DATA *victim                ) );
void    inflict_poison       args( ( int gsn, int level, int type, CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	kill_ch		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Slightly less efficient than Merc 2.2.  Takes 10% of 
 *  total CPU time.
 */
/*
 * This function also handles various spell effects such as
 * haste, blur, fear, and slowness
 *
 * This function actually now takes about 17-20% of total CPU
 * time.  If we were really concerned about CPU usage, we
 * would revamp it.  But we're not, since the MUD uses aout
 * 4% of the CPU or less on an AMD K6-2 450MHz
 *
 * -- Veygoth
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    static bool slowround;
    static bool hasteround;

    last_fun( "violence_update");
    // Hasteround and slowround are used to make hasted and slowed
    // people hit faster or slower (between rounds or every other
    // round)
    if( !hasteround )
       {
         hasteround = TRUE;
       }
    else if( hasteround )
       {
       hasteround = FALSE;

       if( !slowround )
         slowround = TRUE;
       else
         slowround = FALSE;
       }

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( !ch->in_room || ch->deleted )
	    continue;

        if( hasteround && !is_affected( ch, AFF_HASTE ))
            continue;

	if ( ( victim = ch->fighting ) )
	{
	    if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room
		&& !victim->deleted )
	    {
                if ( !check_vicious( ch, victim ) )
                {
                    stop_fighting( ch, FALSE );
                    continue;
                }

                if( !hasteround )
                {
	            prog_hitprcnt_trigger( ch, victim );
	            prog_fight_trigger( ch, victim );
                }

	        /* Ok here we test for switch if victim is charmed */
	        if ( is_affected( victim, AFF_CHARM )
                    && !is_affected( victim, AFF_BLIND )
		    && victim->master
		    && victim->in_room == victim->master->in_room
		    && ch != victim
		    && number_percent( ) > 40 )
		{
		    stop_fighting( ch, FALSE );
                    if( is_affected( ch, AFF_SLOWNESS ) && !slowround )
                      continue;
		    blur_attack (ch, victim);
                    multi_hit( ch, victim, TYPE_UNDEFINED );
		}
		else
		{
                    if( is_affected( ch, AFF_SLOWNESS ) && !slowround )
                      continue;
		    blur_attack (ch, victim);
                    multi_hit( ch, victim, TYPE_UNDEFINED );
		}
                if( is_affected( victim, AFF_FEAR ) && number_percent() < 25 )
                {
                  send_to_char( "You are afraid to continue the battle any longer!\n\r", victim );
                  do_flee( victim, "" );
                }
	    }
	    else
	    {
	        stop_fighting( ch, FALSE );
	    }
	    continue;
	}

	if ( is_affected( ch, AFF_BLIND )
	    || ( IS_NPC( ch ) && ch->pIndexData->pShop )
	    || ( IS_NPC( ch ) && ch->pIndexData->pGame ) )
	    continue;

	/*
	 * Ok. So ch is not fighting anyone.
	 * Is there a fight going on?
	 */

        // Assist code - Veygoth
	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
            bool assist = FALSE;
	    if ( rch->deleted
		|| !IS_AWAKE( rch )
		|| !( victim = rch->fighting ) )
	        continue;

            // Removed the autoassist groupmembers stuff

            // IF a character is fighting a  mobile, then we can check for assist.
            // protector mobs will only assist their own race, a group member,
            // or a follower/leader
            if ( IS_NPC ( ch ) && IS_SET( ch->act, ACT_PROTECTOR )
                 && !ch->fighting && ch->position > POS_SLEEPING) {
                if ( rch->race == ch->race )
                    assist = TRUE;
                if ( is_same_group( ch, rch ) )
                    assist = TRUE;
                if ( (ch == rch->master && IS_NPC( rch )) || rch == ch->master )
                    assist = TRUE;
                if ( is_same_group( ch, rch->fighting ) )
                    assist = FALSE;
                if ( ch->master && is_same_group( ch->master, rch->fighting ) )
                    assist = FALSE;
                if ( rch->race == ch->race && victim->race == ch->race
                    && !IS_NPC( rch ) )
                    assist = FALSE;

            }

            if ( assist && ch->wait <= 0 )
	    {

                    // Lower level mobs will take longer to assist, 26% for a level one
                    // and 100% for a 50.  Newbie guards could concievably ignore a whole
                    // fight.  I guess that's why newbie guards are rare.

                    if ( can_see( ch, victim ) && number_percent() < (( ch->level * 3 / 2) + 25))
                    {
                      act( "$n&n assists $N&n...", ch, NULL, rch, TO_ROOM );
                      one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND );
		      continue;
                    }
	    }

	}
        // End of assist code

	if ( !victim || !rch )
	    continue;

        if( is_affected( ch, AFF_SLOWNESS ) && !slowround )
           continue;

        blur_attack (ch, victim);
        multi_hit( ch, victim, TYPE_UNDEFINED );

        if( is_affected( victim, AFF_FEAR ) && number_percent() < 25 )
        {
           send_to_char( "You are afraid to continue the battle any longer!\n\r", victim );
           do_flee( victim, "" );
        }
    }

    return;
}

/*
 * Do one round of attacks for one character.
 * Note: This is a round, not a single attack!
 * Note: This function is not PLR_VICIOUS or victim death compliant!
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA	*wield;
    int		 chance;

    last_fun( "multi_hit");
    if ( ch == NULL || ch->deleted )
	return;

    if ( victim == NULL || victim->deleted )
	return;

    /* No casting/being para'd and hitting at the same time. */
    if ( (is_affected( ch, AFF_CASTING ))
         || is_affected( ch, AFF_MINOR_PARA ) 
         || is_affected( ch, AFF_HOLD ) )
	return;

    /* I don't know how a dead person can hit someone/be hit. */
    if ( victim->position == POS_DEAD  || victim->hit < -10 )
    {
	stop_fighting( victim, TRUE );
	return;
    }

    /*
     * Set the fighting fields now.
     */
    if ( victim->position > POS_STUNNED )
    {
	if ( !victim->fighting )
	    set_fighting( victim, ch );
        // Can't have bashed/prone people just automatically be standing.
        if( victim->position == POS_STANDING )
            victim->position = POS_FIGHTING;
    }

    if(    IS_NPC( ch ) && ch->pIndexData 
        && ( ch->pIndexData->vnum == 9316 || ch->pIndexData->vnum == 9748 )
        && number_percent() < 20 )
        check_shout( ch, victim );
    make_vis( ch );

    // Everyone gets at least one swing/breath in battle.
    // This handles breathing, roaring, etc.
    if ( !check_race_special( ch, victim, dt ) )
     	one_hit( ch, victim, dt, WEAR_HAND );

    // Thrikreen primary hand extra attack, only thri extra attack that is
    // given to non-warriors in addition to warriors
    if( ch->race == RACE_THRIKREEN && number_percent() < ch->level )
    {
        if( ch->class == CLASS_WARRIOR )
        {
         switch( number_range( 1, 4 ))
         {
          case 1:
            if( get_eq_char( ch, WEAR_HAND ))
              one_hit( ch, victim, dt, WEAR_HAND );
            break;
          case 2:
            if( get_eq_char( ch, WEAR_HAND_2 ))
              one_hit( ch, victim, dt, WEAR_HAND_2 );
            break;
          case 3:
            if( get_eq_char( ch, WEAR_HAND_3 ))
              one_hit( ch, victim, dt, WEAR_HAND_3 );
            break;
          case 4:
            if( get_eq_char( ch, WEAR_HAND_4 ))
              one_hit( ch, victim, dt, WEAR_HAND_4 );
            break;
         }
        }
        else
        {
              one_hit( ch, victim, dt, WEAR_HAND );
              if( number_percent() < ch->level / 2)
                one_hit( ch, victim, dt, WEAR_HAND );
        }
    }

    // Don't hurt a corpse.
    if ( victim->position == POS_DEAD || victim->hit < -10 )
    {
	stop_fighting ( ch, FALSE );
	return;
    }

    // For NPCs we assume they have max skill value for their level.
    // When checking combat skills we only practice them on a successful
    // check in order to make them go up slower.  If they go up too slow
    // we can always practice them before they check - Veygoth

    // Check for second attack
    if( IS_NPC( ch ))
    {
        if( ch->level < skills_table[gsn_second_attack].skill_level[ch->class] )
            chance = 0;
        else
            chance = ((ch->level - skills_table[gsn_second_attack].skill_level[ch->class] ) * 2 + 25) * 3 / 4;
    }
    else
    {
        chance = ch->pcdata->skl_lrn[gsn_second_attack] * 3 / 4;
    }

    if( chance > 95 ) chance = 95;

    if ( number_percent( ) < chance )
    {
        skill_practice( ch, gsn_second_attack );
	one_hit( ch, victim, dt, WEAR_HAND );
	if ( ch->fighting != victim )
	    return;
    }

    // Check for Thri-Kreen arm #3
    if( ch->race == RACE_THRIKREEN && (wield = get_eq_char( ch, WEAR_HAND_3)))
    {
      if ( CAN_WEAR( wield, ITEM_WIELD) )
      {
        if( IS_NPC( ch ))
        {
          if( ch->level < skills_table[gsn_second_attack].skill_level[ch->class] )
            chance = ch->level / 5;  // Up to 10% chance of third arm for psis and
                                     // other miscellaneous thris
          else
            chance = ((ch->level - skills_table[gsn_second_attack].skill_level[ch->class] ) * 2 + 25);
        }
        else
        {
          chance = ch->pcdata->skl_lrn[gsn_second_attack];
        }
    
        if( chance > 95 ) chance = 95;

        if ( number_percent( ) < chance )
        {
          skill_practice( ch, gsn_second_attack );
    	  one_hit( ch, victim, dt, WEAR_HAND_3 );
 	  if ( ch->fighting != victim )
	    return;
        }
      }
    }

    // Check for third attack
    if( IS_NPC( ch ))
    {
       if( ch->level < skills_table[gsn_third_attack].skill_level[ch->class] )
            chance = 0;
       else
            chance = ((ch->level - skills_table[gsn_third_attack].skill_level[ch->class] ) * 2 + 25 ) * 3 / 8;
    }
    else
    {
        chance = ch->pcdata->skl_lrn[gsn_third_attack] * 3 / 8;
    }

    if( chance > 95 ) chance = 95;

    if ( number_percent( ) < chance )
    {
        skill_practice( ch, gsn_third_attack );
	one_hit( ch, victim, dt, WEAR_HAND );
	if ( ch->fighting != victim )
	    return;
    }

    // Check for fourth attack
    if( IS_NPC( ch ))
    {
       if( ch->level < skills_table[gsn_fourth_attack].skill_level[ch->class] )
            chance = 0;
       else
            chance = ((ch->level - skills_table[gsn_fourth_attack].skill_level[ch->class] ) * 2 + 25 ) / 4;
    }
    else
    {
        chance = ch->pcdata->skl_lrn[gsn_fourth_attack] / 4;
    }

    if( chance > 95 ) chance = 95;
    if ( number_percent( ) < chance )
    {
        skill_practice( ch, gsn_fourth_attack );
	one_hit( ch, victim, dt, WEAR_HAND );
	if ( ch->fighting != victim )
	    return;
    }

    // Check for dual wield.  May want to allow a second swing when dual wielding.
    // We'll wait and see what combat looks like before we decide - Veygoth
    if ( ( wield = get_eq_char( ch, WEAR_HAND_2 ) ) )
    {
	if ( CAN_WEAR( wield, ITEM_WIELD) )
        {
          skill_practice( ch, gsn_dual );
          chance = IS_NPC( ch ) ? ch->level : ch->pcdata->skl_lrn[gsn_dual] * 2 / 3;
          chance += ch->class == CLASS_RANGER ? 10 : 0;
          if ( number_percent( ) < chance )
            one_hit( ch, victim, dt, WEAR_HAND_2 );
        }
	if ( ch->fighting != victim )
	    return;
    }

    // Check for fourth arm on thrikreen
    if ( ch->race == RACE_THRIKREEN && ( wield = get_eq_char( ch, WEAR_HAND_4 ) ) )
    {
	if ( CAN_WEAR( wield, ITEM_WIELD) )
        {
          skill_practice( ch, gsn_dual );
          chance = IS_NPC( ch ) ? (ch->level * 3 / 2 + 20) : ch->pcdata->skl_lrn[gsn_dual];
          if( chance > 95 )
            chance = 95;
          if ( number_percent( ) < chance )
            one_hit( ch, victim, dt, WEAR_HAND_4 );
        }
	if ( ch->fighting != victim )
	    return;
    }

    // Don't hurt a corpse.
    if ( victim->position == POS_DEAD
	 || victim->hit < -10 )
    {
	stop_fighting ( ch, FALSE );
	return;
    }

    return;
}

/*
 * Hit one guy once.
 *
 * Hitroll is now done on a 200-sided die rather than a 20-sided die
 * This allows for more dynamic modifiers to hitroll.
 * i.e. a couple extra points of strength and whatnot _may_ make the
 * difference between a hit and a miss rather than incrementing something
 * every 10-20 points of an ability we can modify it every 1-2 points.
 * - Veygoth
 *
 * ch == attacker, victim == getting attacked, 
 * dt == damage type, wpn == wear location of weapon.
 * returns TRUE iff victim dies.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int wpn )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       victim_ac;
    int       thac0;
    int       thac0_00;
    int       thac0_47;
    int       dam;
    int       chance;
    int       diceroll;
    int       wpn_gsn;
    int       dam_type;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */

    last_fun( "one_hit");

    if ( victim->position == POS_DEAD || victim->hit < -10 )
    {
	sprintf( buf, "one_hit: ch %s fighting dead victim %s.",
		ch->name, victim->name );
	bug( buf, 0 );
	ch->fighting = NULL;
        if( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	return;
    }
    if ( ch->in_room != victim->in_room )
    {
	sprintf( buf, "one_hit: ch %s not with victim %s.",
		ch->name, victim->name );
	bug( buf, 0 );
	ch->fighting = NULL;
        if( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	return;
    }

    /* No casting/being para'd and hitting at the same time. */
    if ( ( is_affected( ch, AFF_CASTING ))
         || is_affected( ch, AFF_MINOR_PARA )
         || is_affected( ch, AFF_HOLD ) )
	return;

    // Inertial barrier will prevent some attacks.  At the following levels a person
    // affected by inertial barrier will be able to avoid this percentage of attacks:
    // 1 = 7%  5 = 10%  10 = 13%  20 = 20%  30 = 27%  40 = 33%  50 = 39%  51 = 40%
    // -- Veygoth
    if( is_affected( victim, AFF_INERTIAL_BARRIER ) && number_percent() > (victim->level * 2 / 3 + 7) )
        return;

    // Keep in mind that check_riposte returns a boolean.
    //
    // Riposte was pretty buggy, so this was redone -- Veygoth
    if( dt != gsn_kick && dt != gsn_backstab && dt != gsn_circle 
	&& check_riposte( ch, victim ))
    {
        one_hit( victim, ch, TYPE_UNDEFINED, WEAR_HAND );
        return;
    }
    if ( check_parry( ch, victim ) && dt != gsn_backstab && dt != gsn_circle )
	return;
    if ( check_shield_block( ch, victim ) && dt != gsn_backstab && dt != gsn_circle )
        return;
    if ( check_dodge( ch, victim ) && dt != gsn_backstab && dt != gsn_circle )
        return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, wpn );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == TYPE_WEAPON )
	    dt += wield->value[3];
    }

    /*
     * Weapon proficiencies.
     */
    wpn_gsn	= gsn_hit;
    dam_type	= DAM_BASH;
    if ( wield && wield->item_type == TYPE_WEAPON )
    {
	if ( wield->value[3] >= 0 && wield->value[3] < MAX_ATTACK )
	{
	    wpn_gsn	= (*attack_table[wield->value[3]].wpn_gsn);
	    dam_type	= attack_table[wield->value[3]].dam_type;
	}
	else
	{
	    sprintf( buf, "one_hit: bad weapon dt %d caused by %s (%d).",
		    dt, wield->name, wield->pIndexData ?
			wield->pIndexData->vnum : -1 );
	    bug( buf, 0 );
	    wield->value[3] = 0;
	}
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    thac0_00 = class_table[ch->class]->thac0_00;
    thac0_47 = class_table[ch->class]->thac0_47;

    /* Weapon-specific hitroll and damroll */

    thac0     = interpolate( ch->level, thac0_00, thac0_47 )
              - (get_hitroll( ch, wpn ) * 3);
    victim_ac = UMAX( -100, get_ac( victim ) );

    // Added blindfighting skill - Veygoth
    if ( !can_see( ch, victim ) )
    {
      if( ch->level >= skills_table[gsn_blindfighting].skill_level[ch->class] )
      {
         if( IS_NPC( ch ))
         {
            chance = ((ch->level * 3) / 2 + 15);
         }
         else
         {
            chance = ch->pcdata->skl_lrn[gsn_blindfighting];
            skill_practice( ch, gsn_blindfighting );
         }
         if( number_percent() > chance )
            victim_ac -= 40;
         else
            victim_ac -= 5;
      }
      else
      {
	victim_ac -= 40;
      }
    }

    /* Weapon proficiencies *
     *
     * The twohanded version of a weapon proficiency *MUST* follow the onehanded
     * version in the const.c definitions
     */
    if ( wield && wield->item_type == TYPE_WEAPON )
    {
        if( IS_NPC( ch ))
        {
          chance = ((ch->level * 3) / 2) + 20;
          if( number_percent() < chance )
          {
            victim_ac += 20;
          }
        }
        else if( !IS_OBJ_STAT( wield, ITEM_TWOHANDED ))
        {
          chance = ch->pcdata->skl_lrn[wpn_gsn];
          if( number_percent() < chance )
          {
            victim_ac += 20;
            skill_practice( ch, wpn_gsn );
          }
        }
        else
        {
          chance = ch->pcdata->skl_lrn[(wpn_gsn + 1)];
          if( number_percent() < chance )
          {
            victim_ac += 20;
            skill_practice( ch, (wpn_gsn + 1) );
          }
        }
    }
    else
    {
       if( IS_NPC( ch ))
        {
          chance = ((ch->level * 3) / 2) + 20;
          if( number_percent() < chance )
          {
            victim_ac += 20;
          }
        }
        else
        {
          chance = ch->pcdata->skl_lrn[gsn_hit] + 2;
          if( number_percent() < chance )
          {
            skill_practice( ch, gsn_hit );
            victim_ac += 20;
          }
        }
    }

    /*
     * The moment of excitement!
     */
    diceroll = number_range( 0, 199 );

    // Give them a small bonus if they can make a successful luck check.
    if( number_percent() <= get_curr_luk( ch ) )
        diceroll += 5;

    /* Made really lucky chars get saved by the godz. - Lohrr */
    if (     diceroll == 0
	|| ( diceroll <= 196 && diceroll < thac0 - victim_ac )
        || ( number_percent() < get_curr_luk( victim ) / 40 ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, wpn, dam_type );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     *
     * NPCs are more badass barehanded than players.  If they weren't
     * the game would be too damned easy since mobs almost never have
     * weapons.
     *
     * Increased mob damage by about 1/6 -- Veygoth
     * It was previously level/2 to level*3/2 (25-75 at 50, average 50)
     * It is now level*3/5 to level*10/6      (30-87 at 50, average 59)
     *
     * Added the + ch->level - 1 'cause mobs still not hittin' hard enough
     *   - Lohrr
     *
     */
    if ( IS_NPC( ch ) )
    {
	dam = number_range( (ch->level * 3 / 5), (ch->level * 14 / 8) )
              + (ch->level - 1);
	if ( wield )
	    dam += dice( wield->value[1], wield->value[2] );
        else if( ch->level >= skills_table[gsn_unarmed].skill_level[ch->class] )
        {
            chance = ((ch->level * 3) / 2 + 15);
            if( number_percent() < chance )
              dam += number_range( 1, (chance / 12) );
        }
    }
    else
    {
	if ( wield )
	    dam = dice( wield->value[1], wield->value[2] );
	else
        {
          if( ch->class != CLASS_MONK )
          {
	    dam = number_range( 1, (2 + ch->size / 3) );
            if( ch->level >= skills_table[gsn_unarmed].skill_level[ch->class] )
            {
              skill_practice( ch, gsn_unarmed );
              chance = ch->pcdata->skl_lrn[gsn_unarmed];
              if( number_percent() < chance )
                dam += number_range( 1, (chance / 12) );
            }
          }
          else
          {
            int min;
            // monk barehanded damage - Veygoth
            skill_practice( ch, gsn_unarmed );
            if( ch->pcdata->skl_lrn[gsn_hit] < 13 )
            {
               min = 1;
            }
            else
            {
               min = ch->pcdata->skl_lrn[gsn_hit] / 13;
            }
            // at max skill for barehanded and unarmed, a monk will get
            // a damage of 7-38, an average of 22.5 damage per hit before
            // modifiers.  This is slightly better than a 6d6 weapon (average of 21 dmg)
            // this is slightly worse than a 6d7 weapon (average of 24 dmg)
            // -- Veygoth
            dam = number_range( min, ((ch->pcdata->skl_lrn[gsn_unarmed] / 3) + min) );
          }
        }
	if (( wield && dam > 1000 ) && ch->level < L_IMM ) // Govno
	{ 
	    sprintf( buf, "One_hit dam range > 1000 from %d to %d",
		    wield->value[1], wield->value[2] );
	    bug( buf, 0 );
	}
    }

    /*
     * Played a character with an armor class of 126 (awful agility).
     * Wasn't getting pounded much at all.  Added a damage bonus applied
     * when the target's ac is worse than 100.
     *
     * This also means that someone who makes their weapon proficiency
     * check against someone with an ac of 81 or higher will also get a
     * damage bonus.
     *
     * This applies to mobs too, so if a mob has a terrible AC it will
     * get whacked harder.  I call this the "soft as a pudding" code.
     * -- Veygoth
     */
    if( victim_ac > 100 )
      dam += (dam / 5);

    /*
     * Bonuses.
     */
    dam += get_damroll( ch, wpn );

    // Ya know we might want to check whether they are resistant to poison
    // before we do this math.
    //
    // Yep...its done already in damage(), so commenting out.  -Lithos
/*
    if ( wield && IS_OBJ_STAT( wield, ITEM_POISONED ) )
        dam += dam / 4;
*/

    /* Weapon proficiencies */
    /* Up to 50% increase based on weapon skill */
    if ( wield && !IS_NPC( ch ) && ch->pcdata->skl_lrn[wpn_gsn] > 0 )
	dam += dam * ch->pcdata->skl_lrn[wpn_gsn] / 180;
    /* Up to 33% for offense skill */
    /* This means someone that has mastered a weapon and offense
       automatically does double damage in combat */
    if ( !IS_NPC( ch ) && ch->pcdata->skl_lrn[gsn_enhanced_damage] > 0 )
    {
	dam += dam * ch->pcdata->skl_lrn[gsn_enhanced_damage] / 270;
        skill_practice( ch, gsn_enhanced_damage );
    }

    /* Bad idea to get caught napping in a fight */
    if ( !IS_AWAKE( victim ) )
	dam *= 2;

    /* Backstab: 2 + one per 9 levels, 7x damage at 50 - Veygoth */
    if ( dt == gsn_backstab ) {
        // Cap was previously too low.  It has been raised because a merc that was previously
        // stabbing for 180 now stabs for 64.  Assassins will still be able to stab for
        // 175 and mercs for 116 with this revised cap.  Keep in mind that a sorc can easily
        // fist for 250.
        // -- Veygoth
        int cap;
        cap = 100 + 12 * ch->level;
        if (ch->class == CLASS_MERCENARY || ch->class == CLASS_BARD)
            cap = cap * 2 / 3;
	dam *= (2 + (ch->level / 9));
        /* damage cap applied here  - Lithos */
        dam = UMIN( dam, cap );
    }
    else if ( dt == gsn_circle )              /* 150% to 200% at lev. 50 */
	dam += dam / 2 + ( dam * ch->level ) / 100;

    if ( dam <= 0 )
	dam = 1;

    if ( wield && wield->item_type == TYPE_WEAPON
	&& attack_table[wield->value[3]].hit_fun
	&& (*attack_table[wield->value[3]].hit_fun)(ch, victim, diceroll, dam) )
    {
        if ( !victim || victim->position == POS_DEAD
             || ch->in_room != victim->in_room || victim->hit < -10 )
            return;
        else
            return;
    }

    damage( ch, victim, dam, dt, wpn, dam_type );
    tail_chain( );

/*    if ( !victim || victim->position == POS_DEAD
         || ch->in_room != victim->in_room || victim->hit < -10 )
    return;
*/

    return;
}

/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int wpn,
	    int dam_type )
{
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    bool critical = FALSE;
    bool immune;
    char buf[MAX_STRING_LENGTH];

    last_fun( "damage");
    if ( victim->position == POS_DEAD
	 || victim->hit < -10 )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if ( ( dam > 1276 ) && ch->level < L_IMM ) // Govno
    {
        char buf [ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) && ch->desc )
	    sprintf( buf,
		    "Damage: %d from %s by %s: > 1276 points with %d dt!",
		    dam, ch->name, ch->desc->original->name, dt );
	else
	    sprintf( buf,
		    "Damage: %d from %s: > 1276 points with %d dt!",
		    dam, IS_NPC( ch ) ? ch->short_descr : ch->name, dt );

	bug( buf, 0 );
	dam = 1276;
    }

    // Remove memorization and meditation bits - Veygoth
    stop_med( victim );
    stop_mem( victim );
    if( is_affected( victim, AFF_MINOR_PARA ))
    {
        act( "$n&n disrupts the magic preventing $N&n from moving.", ch, NULL, victim, TO_ROOM_VICT );
        act( "You disrupt the magic preventing $N&n from moving.", ch, NULL, victim, TO_CHAR );
        act( "&+YYou can move again.&n", ch, NULL, victim, TO_VICT );
        REMOVE_AFF_BIT( victim, AFF_MINOR_PARA );
        remove_affects( victim, AFF_MINOR_PARA );
    }

    immune = FALSE;
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
        victim = check_guarding( ch, victim );
	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;
        check_killer( ch, victim );
	if ( victim->position > POS_STUNNED )
	{
	    if ( !victim->fighting )
		set_fighting( victim, ch );
            // Can't have prone people automatically stand
            if( victim->position == POS_STANDING )
                victim->position = POS_FIGHTING;

	    if ( !ch->fighting )
		set_fighting( ch, victim );

	    /*
	     * If NPC victim is following, ch might attack victim's master.
	     * No charm check here because charm would be dispelled from
	     * tanking mobile when combat ensues thus ensuring PC charmer is
	     * not harmed.
	     * Check for is_same_group wont work as following mobile is not
	     * always grouped with PC charmer - Kahn
             *
             * Added a check for whether ch has switch skill.  If not,
             * much lower chancing of retargetting - Lithos
	     */
	    if (   IS_NPC( ch )
		&& IS_NPC( victim )
		&& victim->master
		&& victim->master->in_room == ch->in_room
		&& number_bits( 2 ) == 0 )
	    {
              start_grudge(ch, victim->master, FALSE);
              /* start_grudge() combines start_hating and start_hunting
               * This way mob will be hating master even if it cannot switch
              if (has_skill(ch, gsn_switch) || number_percent() < 5) {
                  stop_fighting( ch, FALSE );
                  set_fighting( ch, victim->master );
                  return;
              }
          * Just took this stuff out, it's a carry-over from Envy
          * Maybe add some AI stuff in spec_warrior to bash sorc, etc
          * - Lithos
               */
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

        make_vis(ch);

	/*
	 * Hunting stuff...
	 */
	if ( dam && IS_NPC( victim ) )
	{
            /* start_grudge is combined start_hating and start_hunting */
            start_grudge( victim, ch, FALSE );
	}

	/*
	 * Damage modifiers.
	 */

        // Critical hits for double damage -- Veygoth
        // Average of 5% for those that have average luck
        // Gnomes could concievably have 10%
        if( number_percent() < (2 + (get_curr_luk( ch ) / 18 )) && dam > 0 )
        {
          send_to_char( "&+WYou score a CRITICAL HIT!&n\n\r", ch );
          dam *= 2;
          critical = TRUE;
        }

	if( is_affected( victim, AFF_SANCTUARY ))
	    dam /= 2;

	if( is_affected( victim, AFF_PROTECT_EVIL ) && IS_EVIL( ch ))
	    dam -= dam / 8;
	else if( is_affected( victim, AFF_PROTECT_GOOD ) && IS_GOOD( ch ))
	    dam -= dam / 8;

        // Check stoneskin.  People not affected by a stoneskin affect
        // cannot lose their stoneskin for any reason.  This should mean
        // that mobs will keep their stoneskin and players should always
        // have a chance to lose it, since no player should ever be
        // setbit stoneskin.
        //
        // The bool value of found is used so that we can have them
        // take full damage when their stoneskin shatters, but get the
        // damage reduction if they are either a mob or their stoneskin
        // wears off that round.
        //
        // - Veygoth.
	/* Yeah, yeah.. so maybe backstabs shouldn't be aff'd. */
        // Actually they should be affected, but they should have a much
        // higher chance of getting through (say 30-70%). -- Veygoth
        //
        // Critical hits will now go through stoneskin
        // automatically
	if ( !critical && is_affected( victim, AFF_STONESKIN ) && 
		( dt != gsn_backstab || number_percent() < (25 + ch->level) ) )
        {
            bool found = FALSE;
            for( paf = victim->affected; paf; paf = paf->next )
            {
              if( paf->deleted )
                continue;
              if( get_bitvector( paf, AFF_STONESKIN ))
              {
                  // Small chance of shattering the stoneskin on a good hit.
                  // Reduced chance by about 20% -- Veygoth 5-4-00
                  if( dam >= 25 && number_percent() <= ( dam / 12 ) )
                  {
                    send_to_char( "&+LYour stoneskin is shattered by the massive blow!&n\n\r", victim );
                    act( "$n&n's massive blow shatters $N&n's stoneskin!", ch, NULL, victim, TO_NOTVICT );
                    act( "Your massive blow shatters $N&n's stoneskin!", ch, NULL, victim, TO_CHAR );
                    affect_remove( victim, paf );
                    found = TRUE;
                  }
                  else if (dam > 0) //added check for actual damage - Lithos
                  {
                    paf->modifier--;
                    if( paf->modifier < 1 )
                    {
                      affect_remove( victim, paf );
                      send_to_char( "&+LYou feel your skin soften and return to normal.&n\n\r", victim );
                    }
                    dam /= 15;
                    found = TRUE;
                  }
              }
            }
            // This means they're AFF_STONESKIN as an innate/permenant.
            // We will still allow it to shatter, but it will refresh itself
            // upon a mob update.  Because of this, we make it easier to shatter.
            // No damage reduction when it shatters.
            // -- Veygoth
            if( !found )
            {
                if( dam >= 8 && number_percent() <= ( dam / 8 ) )
                {
                  send_to_char( "&+LYour stoneskin is shattered by the massive blow!&n\n\r", victim );
                  act( "$n&n's massive blow shatters $N&n's stoneskin!", ch, NULL, victim, TO_NOTVICT );
                  act( "Your massive blow shatters $N&n's stoneskin!", ch, NULL, victim, TO_CHAR );
                  REMOVE_AFF_BIT( victim, AFF_STONESKIN );
                }                
                else
                {
                  dam = dam/15 ? dam/15 : 1;
                }
            }

        }

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, dodge and shield block.
	 */
	if ( dt >= TYPE_HIT || dt == gsn_kick )
	{
            // Trip and disarm removed because those should be handled
            // by each individual mob's special function.
	    if ( IS_NPC( ch )
		&& IS_SET( race_table[ ch->race ].race_abilities,
			  RACE_WEAPON_WIELD )
		&& number_percent( ) < UMIN( 25, UMAX( 10, ch->level ) )
		&& !IS_NPC( victim ) )
	        use_magical_item( ch );
	}
    }

    switch( check_ris( victim, dam_type ) )
    {
    case IS_RESISTANT:		dam -= dam / 3;				break;
    case IS_IMMUNE:		immune = TRUE; dam = 0;			break;
    case IS_SUSCEPTIBLE:	dam += dam / 2;				break;
    default:
		break;
    }

    if( (dam_type == DAM_WIND || dam_type == DAM_GAS || dam_type == DAM_ASPHYXIATION )
         && is_affected( victim, AFF_DENY_AIR ))
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+CYou deny the damage.&n\n\r", ch );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( dam_type == DAM_FIRE && is_affected( victim, AFF_DENY_FIRE ))
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+rYou deny the damage.&n\n\r", ch );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( (dam_type == DAM_EARTH || dam_type == DAM_CRUSHING )
        && is_affected( victim, AFF_DENY_EARTH ))
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+yYou deny the damage.&n\n\r", ch );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( (dam_type == DAM_WATER || dam_type == DAM_ACID || dam_type == DAM_DROWNING )
        && is_affected( victim, AFF_DENY_WATER ))
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+bYou deny the damage.&n\n\r", ch );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }

    // Check for protection spells that give 25% damage reduction - Veygoth
    if( dam_type == DAM_FIRE && is_affected( victim, AFF_PROTECT_FIRE ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_COLD && is_affected( victim, AFF_PROTECT_COLD ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_ACID && is_affected( victim, AFF_PROTECT_ACID ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_GAS && is_affected( victim, AFF_PROTECT_GAS ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_ELECTRICITY && is_affected( victim, AFF_PROTECT_LIGHTNING ))
        dam = (dam * 3) / 4;

    // Barkskin protects from 8% of slash and 12% of pierce damage.
    if( is_affected( victim, AFF_BARKSKIN ))
    {
        if( dt == DAM_SLASH )
            dam = dam * 11 / 12;
        else if( dt == DAM_PIERCE )
            dam = dam * 7 / 8;
    }

    // Check for vampiric touch for anti-paladins and vampires
    if( wpn == WEAR_HAND || wpn == WEAR_HAND_2 || wpn == WEAR_HAND_3
      || wpn == WEAR_HAND_4 )
      if( ( (ch->class == CLASS_ANTIPALADIN || ch->race == RACE_VAMPIRE)
          && dt == TYPE_HIT && !get_eq_char( ch, wpn ) )
        || ( IS_AFFECTED( ch, AFF_VAMP_TOUCH ) 
             && ( !(obj = get_eq_char( ch, wpn ))
                 || IS_OBJ_AFF( obj, AFF_VAMP_TOUCH ) ) ) )
    {
        ch->hit += dam / 3;
        if( ch->hit > (get_max_hit( ch ) + 50 + ch->level * 5 ))
            ch->hit = get_max_hit( ch ) + 50 + ch->level * 5;
    }

    /*  PC to PC damage quartered.
     *  NPC to PC damage divided by 3.  - Lohrr
     */
    if( dam > 0 && !IS_NPC( victim ) && victim != ch )
    {
        if ( !IS_NPC( ch ) )
            dam /= 4;
        else
            dam /= 3;
    }

    /*
     * Just a check for anything that is excessive damage
     * Send a log message, keeping the imms on their toes
     * Changed this from 300 to 250 'cause hitters get more than one
     *  attack/round and w/haste that's 1000 possible in the time one fist
     *  goes off.  That's more than the fist might do and it has to be
     *  memmed.  - Lohrr
     */
    if ( dam > 250 && dt != gsn_backstab)
    {
        char buf [ MAX_STRING_LENGTH ];
        sprintf( buf, "Excessive damage: %s attacking %s for %d, gsn = %s(%d).",
            ch->name, victim->name, dam, skills_table[dt].noun_damage, dt);
        log_string( buf );
    }

    /*
     * We moved dam_message out of the victim != ch if above
     * so self damage would show.  Other valid type_undefined
     * damage is ok to avoid like mortally wounded damage - Kahn
     */
    if ( dt != TYPE_UNDEFINED )
        dam_message( ch, victim, dam, dt, wpn, immune );

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
//    if (ch != victim) {
//        sprintf(lbuf,"Damage: %s does %d damage to %s", ch->name,
//            dam, victim->name);
//        wiznet( 0, WIZ_LSPAM, 0, lbuf);
//    }
    victim->hit -= dam;

    /* Check for HOLY_SACRFICE and BATTLE_ECSTASY  - Lithos */
    if ( dam > 0 && victim != ch ) {
        CHAR_DATA *gch;
        if ( is_affected( victim, AFF_HOLY_SACRIFICE ) && victim->group_leader ) {
            for ( gch = victim->group_leader; gch; gch = gch->next_in_group ) {
                if (gch->deleted || gch == victim || gch->in_room != ch->in_room)
                    continue;
                gch->hit += dam / 5;
                if ( gch->hit  > get_max_hit( gch ) + 50 + gch->level * 5 )
                    gch->hit = get_max_hit( gch ) + 50 + gch->level * 5;
            } //end for loop
        } //end if holy sac
        if ( ch->group_leader ) {
            for ( gch = ch->group_leader; gch; gch = gch->next_in_group ) {
                if (gch->deleted || gch == victim || gch->in_room != ch->in_room)
                    continue;
                if ( is_affected( gch, AFF_BATTLE_ECSTASY ) ) {
                    gch->hit += dam / 20;
                    if ( gch->hit  > get_max_hit( gch ) + 50 + gch->level * 5 )
                        gch->hit = get_max_hit( gch ) + 50 + gch->level * 5;
                } // end if battle ecstasy
            } //end for loop
        } //end if grouped
    } //end if
             
    // Make sure if they got an instant kill roll that the victim dies - Veygoth
    if( dt == gsn_instant_kill )
    {
      if( victim->race != RACE_DEVIL
       && victim->race != RACE_DEMON
       && victim->race != RACE_GOD )
         victim->hit = -20;
    }

    /* Added damage exp!  - Lohrr */
    // chance added by Veygoth because people level faster and faster as they get higher level...
    // to be worked out when exp is redone.
    // you can now only get damage exp on mobs that con easy or better
    // and there's only a 25% chance per hit of you evern being eligible for damage exp.
    if( number_percent() < 25 && victim->level >= ( ch->level - 3 ) )
      gain_exp( ch, UMAX( 1, dam / 20 ) );

    if ( !IS_NPC( victim )
	&& victim->level >= LEVEL_IMMORTAL
	&& victim->hit < 1 )
	victim->hit = 1;

    /*
     * Magic shields that retaliate
     *
     * Apparently two people with the same sort of shield do not
     * take damage from each other
     */
    if ( ( dam > 1 ) && victim != ch )
    {
	/* For compatibility with old Envy2.2 areas & pfiles */
	if ( is_affected( victim, AFF_FLAMING )
	    && !is_affected( ch, AFF_FLAMING ) )
	    spell_damage( victim, ch, dam/2, spl_flame_shield, DAM_FIRE );

	if ( has_affect( victim, 0, spl_frost_shield, 0  )
	    && !has_affect( ch, 0, spl_frost_shield, 0 ) )
	    spell_damage( victim, ch, dam/2, spl_frost_shield, DAM_COLD );

	if ( has_affect( victim, 0, spl_shock_shield, 0  )
	    && !has_affect( ch, 0, spl_shock_shield, 0  ) )
	    spell_damage( victim, ch, dam/2, spl_shock_shield, DAM_ELECTRICITY );

        /* Soulshield is a complex one.  If the attacker and victim are of
         * opposite alignment, the shield retaliates with 1/2 damage just like
         * any other shield.  If the victim is neutral and the attacker is
         * not, the shield retaliates with 1/4 damage.  If the victim is good
         * or evil and the attacker is neutral, the shield retaliates with
         * 1/8 damage.  If the attacker and victim are of same alignment,
         * the shield does nothing.
         *
         * -- Veygoth
         */
        if ( has_affect( victim, 0, spl_soulshield, 0  )
            && !has_affect( ch, 0, spl_soulshield, 0  ))
            {
              if( IS_EVIL( victim ) && IS_GOOD( ch ))
                spell_damage( victim, ch, dam/2, spl_soulshield, DAM_HARM );
              else if( IS_GOOD( victim ) && IS_EVIL( ch ))
                spell_damage( victim, ch, dam/2, spl_soulshield, DAM_HARM );
              else if( IS_NEUTRAL( victim ) && ( IS_EVIL( ch ) || IS_GOOD( ch )) )
                spell_damage( victim, ch, dam/4, spl_soulshield, DAM_HARM );
              else if( IS_GOOD( victim ) && IS_NEUTRAL( ch ))
                spell_damage( victim, ch, dam/8, spl_soulshield, DAM_HARM );
              else if( IS_EVIL( victim ) && IS_NEUTRAL( ch ))
                spell_damage( victim, ch, dam/8, spl_soulshield, DAM_HARM );
            } 
    }

    if ( has_affect( victim, gsn_berserk, 0, 0  )
	&& victim->position <= POS_STUNNED )
        affect_strip( victim, gsn_berserk, 0, 0  );

    if ( dam > 0 && dt > TYPE_HIT
	&& is_wielding_poisoned( ch, wpn ) > 0
	&& !saves_spell( ch->level, victim, DAM_POISON ) )
    {
        inflict_poison( gsn_poison_weapon, ch->level, is_wielding_poisoned( ch, wpn ), ch, victim );
        act("$n&n suffers from the &+Gpoison&n inflicted upon $m.", victim, NULL, NULL, TO_ROOM);
        affect_strip_obj( get_eq_char(ch,wpn), gsn_poison_weapon, 0, 0);
    }

    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	send_to_char( 
            "&+LYou are &+Rmo&n&+rr&+Rt&n&+ral&+Rl&n&+ry&+L wounded, and will die soon, if not aided.&n\n\r",
	    victim );
        act( "$n&+L is &+Rmo&n&+rr&+Rt&n&+ral&+Rl&n&+ry&+L wounded, and will die soon, if not aided.&n",
	    victim, NULL, NULL, TO_ROOM );
        stop_not_vicious( victim );
	break;

    case POS_INCAP:
	send_to_char(
            "&+LYou are incapacitated and will &n&+rbl&+Re&n&+re&+Rd&+L to death, if not aided.&n\n\r",
	    victim );
        act( "$n&+L is incapacitated and will slowly &n&+rbl&+Re&n&+re&+Rd&+L to death, if not aided.&n",
	    victim, NULL, NULL, TO_ROOM );
        stop_not_vicious( victim );
	break;

    case POS_STUNNED:
        send_to_char("&+LYou are stunned, but will probably recover.&n\n\r",
	    victim );
        act( "$n&+L is stunned, but will probably recover.&n",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_DEAD:
        if( victim == ch )
        {
          send_to_char( "&+LYou have been &+Rsl&n&+ra&+Ri&n&+rn&+L!&n\n\r\n\r", victim );
        }
        else
        {
          sprintf( buf, "&+LYou have been &+Rsl&n&+ra&+Ri&n&+rn&+L by&n %s&+L!&n\n\r\n\r",
            PERS( ch, victim ) );
          send_to_char( buf, victim );
        }
	/* Added this to stop a bug. */
	stop_fighting ( victim, TRUE );
        act( "$n&+L is &n&+rDEAD&+L!!&n", victim, NULL, NULL, TO_ROOM );
	break;

    default:
	if ( dam > get_max_hit( victim ) / 5 )
            send_to_char( "That really did &+RHURT&n!\n\r", victim );
	if ( victim->hit < get_max_hit(victim) / 10 )
            send_to_char( "You sure are &n&+rBL&+RE&n&+rE&+RDI&n&+rN&+RG&n!\n\r", victim );
	break;
    }

    // Check for weapon procs
    if ( (obj = get_eq_char( ch, wpn )) && POS_DEAD != victim->position )
    {
      if ( obj->spec_fun != 0 )
            ( *obj->spec_fun ) ( obj, obj->carried_by, TRUE );
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE( victim ) )		/* lets make NPC's not slaughter PC's */
    {
	if ( victim->fighting
	    && victim->fighting->hunting
	    && victim->fighting->hunting->who == victim )
	    stop_hunting( victim->fighting );

	if ( victim->fighting
	    && victim->fighting->hating
	    && victim->fighting->hating->who == victim )
	    stop_hating( victim->fighting );

        if ( victim->fighting
	    && !IS_NPC( victim )
	    && IS_NPC( ch ) )
          stop_fighting( victim, TRUE );
        else
          stop_fighting( victim, FALSE );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
        // Done in attempt to squelch the combat continuation bug
        stop_fighting( victim, TRUE );

        if(!IS_SET( victim->act, ACT_NOEXP ) || !IS_NPC( victim ) )
  	  group_gain( ch, victim );

        if ( IS_NPC(ch) ) {
            if (ch->hunting) {
                if (ch->hunting->who == victim)
                    stop_hunting( ch );
            }
            if (ch->hating) {
                if (ch->hating->who == victim)
                    stop_hating( ch );
            }
        }

	if ( !IS_NPC( victim ) )
	{
		if ( IS_NPC( ch ) )
		{
		    victim->pcdata->mdeaths++;
		    if ( is_clan( victim ) )
			victim->pcdata->clan->mdeaths++;
		}
		else
		{
		    ch->pcdata->pkills++;
		    victim->pcdata->pdeaths++;

		    if (   is_clan( ch )
			&& is_clan( victim )
			&& ch->pcdata->clan != victim->pcdata->clan )
		    {
			ch->pcdata->clan->pkills++;
			victim->pcdata->clan->pdeaths++;
		    }
	    }

	    sprintf( log_buf, "%s&n killed by %s&n at %d",
		    victim->name,
		    ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
		    victim->in_room->vnum );
	    log_string( log_buf );
            wiznet( ch, WIZ_DEATHS, L_AVA, log_buf );

	    /*
	     * Dying penalty:
	     * 
             * At level 1 you lose 12.5% of a level.
             * At level 50 you lose 37.5% of a level.
             * Higher levels lose bigger
             * -- Veygoth.
	     */
//           gain_exp( victim, (0 - (((25 + victim->level) * exp_table[victim->level].lvl_exp ) / 250 )));
            /* Exp damage was deemed too severe
             * Lessened exp loss at higher levels, at 50 you lose 25% of a level
             *
             * Lithos 5/31/2000
             */
           // Made it so people level 5 and under lose no exp from death.
           if( ch->level > 5 )
             gain_exp( victim, (0 - (((50 + victim->level) * exp_table[victim->level].lvl_exp ) / 400 )));
           if( victim->level < 2 && victim->exp < 1 )
               victim->exp = 1;	
	
	}
	else
	{
	    if ( !IS_NPC( ch ))
	    {
		ch->pcdata->mkills++;
		if ( is_clan( ch ) )
		    ch->pcdata->clan->mkills++;
	    }
	}
	raw_kill( ch, victim );
        last_fun( "damage: return from raw kill");
        // Keep in mind after this point the character is not in the
        // char_list, not in any room, and is at the menu.  Don't do
        // anything that would cause a segmentation fault - Veygoth

	if (   (!IS_NPC( ch ) && is_clan( ch ))
	    && (!IS_NPC( victim ) && is_clan( victim ))
	    && ch->pcdata->clan != victim->pcdata->clan )
	{
	    ch->pcdata->clan->score += 20;
	}

	/* 
	 * Ok, now we want to remove the deleted flag from the
	 * PC victim.
	 */
	if ( !IS_NPC( victim ) )
	    victim->deleted = FALSE;

        return;
    }

    if ( victim == ch )
    {
	return;
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC( victim ) && dam > 0 )
    {
	if ( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
	      && victim->hit < get_max_hit( victim ) / 5 )
	    || ( is_affected( victim, AFF_CHARM ) && victim->master
		&& victim->master->in_room != victim->in_room ) )
	    {
		start_fearing( victim, ch );
		stop_hunting( victim );
                do_flee( victim, "\0" );
	    }
    }

    if ( !IS_NPC( victim )
	&& victim->hit   > 0
	&& victim->hit  <= victim->wimpy )
	do_flee( victim, "\0" );

    tail_chain( );

    return;
}

/*
 * Inflict damage from a spell.
 */
// By Veygoth, based on damage() function, but customized for spells
void spell_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type )
{
    bool immune;
    char buf[MAX_STRING_LENGTH];

    last_fun( "spell_damage");
    if ( victim->position == POS_DEAD )
	return;

    // Remove memorization and meditation bits - Veygoth
    // And invis.  - Lohrr
    make_vis( ch );
    stop_med( victim );
    stop_mem( victim );

    if ( check_shrug( ch, victim ) )
	return;

    if( victim->position == POS_SLEEPING 
        && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
        && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
        && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
        && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
    {
        act( "$n&n has a rude awakening!", victim, NULL, NULL, TO_ROOM );
        victim->position = POS_RESTING;
        if (ch->in_room == victim->in_room && ch->fly_level == victim->fly_level)
            set_fighting( victim, ch );
    }

    // Check for globe spells.  See also finish_spell under TAR_CHAR_OFFENSIVE
    // in merc.h.  This check here is just to prevent area effect spells from
    // doing damage if too low level.  The check for direct spells is in
    // magic.c
        if( is_affected( victim, AFF_MAJOR_GLOBE )
                && ( spells_table[dt].spell_circle[ch->class] <= 6
                       || dt == spl_flame_shield 
                       || dt == spl_shock_shield 
                       || dt == spl_soulshield 
                       || dt == spl_frost_shield ) )
        {
            act( "&+RThe globe around $N&+R's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+RYour globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+R$N&+R's globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_ROOM_VICT );
            return;
        }
        if( is_affected( victim, AFF_GREATER_SPIRIT_WARD ) && spells_table[dt].spell_circle[ch->class] <= 5 )
        {
            act( "&+WThe aura around $N&+W's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+WYour globe absorbs $n&+W's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+W$N&+W's aura absorbs $n&+W's attack!&n", ch, NULL, victim, TO_ROOM_VICT );
            return;
        }
        if( is_affected( victim, AFF_MINOR_GLOBE ) && spells_table[dt].spell_circle[ch->class] <= 4 )
        {
            act( "&+RThe globe around $N&+R's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+RYour globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+R$N&+R's globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_ROOM_VICT );
            return;
        }
        if( is_affected( victim, AFF_SPIRIT_WARD ) && spells_table[dt].spell_circle[ch->class] <= 3 )
        {
            act( "&+WThe aura around $N&+W's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+WYour globe absorbs $n&+W's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+W$N&+W's aura absorbs $n&+W's attack!&n", ch, NULL, victim, TO_VICT );
            return;
        }


    /*
     * Stop up any residual loopholes.
     */
    // 1275 is average damage from Akiaurn's Power Word 
    // I changed this to reflect that.
    if (( dam > 1275 ) && ch->level < L_IMM && ch->race != RACE_DRAGON ) // Govno
    {
        char buf [ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) && ch->desc )
	    sprintf( buf,
		    "Spell_Damage: %d from %s by %s: > 1000 points with %d dt!",
		    dam, ch->name, ch->desc->original->name, dt );
	else
	    sprintf( buf,
		    "Spell_Damage: %d from %s: > 1275 points with %d dt!",
		    dam, IS_NPC( ch ) ? ch->short_descr : ch->name, dt );

	bug( buf, 0 );
	dam = 1275;
    }

    if( is_affected( victim, AFF_MINOR_PARA )
        && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
        && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
        && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
        && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
    {
        act( "$n&n disrupts the magic preventing $N&n from moving.", ch, NULL, victim, TO_ROOM_VICT );
        act( "You disrupt the magic preventing $N&n from moving.", ch, NULL, victim, TO_CHAR );
        act( "&+YYou can move again.&n", ch, NULL, victim, TO_VICT );
        REMOVE_AFF_BIT( victim, AFF_MINOR_PARA );
        affect_strip( victim, 0, spl_earthen_grasp, 0 );
        affect_strip( victim, 0, spl_gr_earthen_grasp, 0 );
        affect_strip( victim, 0, spl_minor_para, 0  );
    }

    immune = FALSE;
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;
	  check_killer( ch, victim );

	if ( victim->position > POS_STUNNED
            && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
            && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
            && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
            && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
	{
	    // Offensive spells engage victim if not fighting, and
	    //   caster only if neither are fighting.
	    if ( !victim->fighting  && victim->in_room == ch->in_room
                && victim->fly_level == ch->fly_level )
	    {
		set_fighting( victim, ch );
	    	if ( !ch->fighting )
		    set_fighting( ch, victim );
            // Can't have prone people automaticaly stand.
            if( victim->position == POS_STANDING )
                victim->position = POS_FIGHTING;
            }
	    /*
	     * If NPC victim is following, ch might attack victim's master.
	     * No charm check here because charm would be dispelled from
	     * tanking mobile when combat ensues thus ensuring PC charmer is
	     * not harmed.
	     * Check for is_same_group wont work as following mobile is not
	     * always grouped with PC charmer - Kahn
	     */
	    if (   IS_NPC( ch )
		&& IS_NPC( victim )
		&& victim->master
		&& victim->master->in_room == ch->in_room
		&& number_bits( 2 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		set_fighting( ch, victim->master );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch
            && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
            && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
            && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
            && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
	    stop_follower( victim );

	/*
	 * Hunting stuff...
	 */
	if ( dam && IS_NPC( victim )
            && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
            && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
            && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
            && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
	{
            start_grudge( victim, ch, FALSE );
	}

	/*
	 * Damage modifiers.
	 */
	if( is_affected( victim, AFF_SANCTUARY))
	    dam /= 2;

	if ( (   is_affected( victim, AFF_PROTECT_EVIL )) && IS_EVIL( ch ))
	    dam -= dam / 8;
	else if ( (   is_affected( victim, AFF_PROTECT_GOOD )) && IS_GOOD( ch ))
	    dam -= dam / 8;

	if ( dam < 0 )
	    dam = 0;
    }

    switch( check_ris( victim, dam_type ) )
    {
    case IS_RESISTANT:		dam -= dam / 3;				break;
    case IS_IMMUNE:		immune = TRUE; dam = 0;			break;
    case IS_SUSCEPTIBLE:	dam += dam / 2;				break;
    }

    if( (dam_type == DAM_WIND || dam_type == DAM_GAS || dam_type == DAM_ASPHYXIATION )
         && is_affected( victim, AFF_DENY_AIR ) )
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+CYou deny the damage.&n\n\r", victim );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( dam_type == DAM_FIRE && is_affected( victim, AFF_DENY_FIRE ) )
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+rYou deny the damage.&n\n\r", victim );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( (dam_type == DAM_EARTH || dam_type == DAM_CRUSHING )
         && is_affected( victim, AFF_DENY_EARTH ) )
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+yYou deny the damage.&n\n\r", victim );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }
    if( (dam_type == DAM_WATER || dam_type == DAM_ACID || dam_type == DAM_DROWNING )
        && is_affected( victim, AFF_DENY_WATER ) )
    {
       if( number_percent() < 50 )
       {
         send_to_char( "&+bYou deny the damage.&n\n\r", victim );
         immune = TRUE;
         dam = 0;
       }
       else
         dam -= dam / 5;
    }

    // Check for protection spells that give 25% damage reduction - Veygoth
    if( dam_type == DAM_FIRE && is_affected( victim, AFF_PROTECT_FIRE ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_COLD && is_affected( victim, AFF_PROTECT_COLD ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_ACID && is_affected( victim, AFF_PROTECT_ACID ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_GAS && is_affected( victim, AFF_PROTECT_GAS ))
        dam = (dam * 3) / 4;
    else if( dam_type == DAM_ELECTRICITY && is_affected( victim, AFF_PROTECT_LIGHTNING ))
        dam = (dam * 3) / 4;

    /*
     * We moved dam_message out of the victim != ch if above
     * so self damage would show.  Other valid type_undefined
     * damage is ok to avoid like mortally wounded damage - Kahn
     */
    if ( dt != TYPE_UNDEFINED
        && !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
        && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
        && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
        && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
        spl_dam_message( ch, victim, dam, dt, immune );

    /*  PC to PC damage quartered.
     *  NPC to PC damage divided by 3.  - Lohrr
     */
    if( dam > 0 && !IS_NPC( victim ) && victim != ch )
    {
        if ( !IS_NPC( ch ) )
            dam /= 4;
        else
            dam /= 3;
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    if(    !( victim->race == RACE_ELEM_FIRE  && dam_type == DAM_FIRE )
        && !( victim->race == RACE_ELEM_WATER && dam_type == DAM_WATER )
        && !( victim->race == RACE_ELEM_EARTH && dam_type == DAM_EARTH )
        && !( victim->race == RACE_ELEM_AIR   && dam_type == DAM_WIND ) )
    {
        /* Added damage exp!  - Lohrr */
        // chance added by Veygoth because people level faster and faster as they get higher level...
        // you can now only get damage exp on mobs that con easy or better
        // and there's only a 25% chance per hit of you evern being eligible for damage exp.
        if( number_percent() < 25 && victim->level >= ( ch->level - 3 ) )
          gain_exp( ch, UMAX( 1, dam / 20 ) );
        victim->hit -= dam;
    }
    else
    {
        const  char         *attack;

	if ( dt >= 0 && dt < MAX_SPELL )
	    attack	= spells_table[dt].name;
        else
            attack      = "it";

        act( "$N&n absorbs your $t!", ch, attack, victim, TO_CHAR );
        act( "You absorb $n&n's $t!", ch, attack, victim, TO_VICT );
        act( "$N&n absorbs $n&n's $t", ch, attack, victim, TO_ROOM_VICT );
        if( IS_IMMORTAL( ch ) )
        {
            char buf [100];
            sprintf( buf, "You healed %d damage.",
                get_max_hit( victim ) >= dam + victim->hit ? dam : get_max_hit( victim ) - victim->hit );
            send_to_char( buf, ch );
        }

        victim->hit = UMIN( get_max_hit( victim ), victim->hit + dam );

        return;
    }

    if ( !IS_NPC( victim )
	&& victim->level >= LEVEL_IMMORTAL
	&& victim->hit < 1 )
	victim->hit = 1;

    if ( has_affect( victim, gsn_berserk, 0, 0  )
	&& victim->position <= POS_STUNNED )
        affect_strip( victim, gsn_berserk, 0, 0  );

    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	send_to_char( 
            "&+LYou are &+Rmo&n&+rr&+Rt&n&+ral&+Rl&n&+ry&+L wounded, and will die soon, if not aided.&n\n\r",
	    victim );
        act( "$n&+L is &+Rmo&n&+rr&+Rt&n&+ral&+Rl&n&+ry&+L wounded, and will die soon, if not aided.&n",
	    victim, NULL, NULL, TO_ROOM );
        stop_not_vicious( victim );
	break;

    case POS_INCAP:
	send_to_char(
            "&+LYou are incapacitated and will slowly &n&+rbl&+Re&n&+re&+Rd&+L to death, if not aided.\n\r",
	    victim );
        act( "$n&+L is incapacitated and will slowly &n&+rbl&+Re&n&+re&+Rd&+L to death, if not aided.&n",
	    victim, NULL, NULL, TO_ROOM );
        stop_not_vicious( victim );
	break;

    case POS_STUNNED:
        send_to_char("&+LYou are stunned, but will probably recover.&n\n\r",
	    victim );
        act( "$n&+L is stunned, but will probably recover.&n",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_DEAD:
        act( spells_table[dt].msg_kill, ch, NULL, victim, TO_ROOM_VICT );
        act( spells_table[dt].msg_kill, ch, NULL, victim, TO_CHAR );
        if( victim == ch )
        {
          send_to_char( "&+LYou have been &+Rsl&n&+ra&+Ri&n&+rn&+L!&n\n\r\n\r", victim );
        }
        else
        {
          sprintf( buf, "&+LYou have been &+Rsl&n&+ra&+Ri&n&+rn&+L by&n %s&+L!&n\n\r\n\r",
            PERS( ch, victim ) );
          send_to_char( buf, victim );
        }
	stop_fighting ( victim, TRUE );
        act( "$n&+L is &n&+rDEAD&+L!!&n", victim, NULL, NULL, TO_ROOM );
	break;

    default:
	if ( dam > get_max_hit( victim ) / 5 )
            send_to_char( "That really did &+RHURT&n!\n\r", victim );
	if ( victim->hit < get_max_hit( victim ) / 10 )
            send_to_char( "You sure are &n&+rBL&+RE&n&+rE&+RDI&n&+rN&+RG&n!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE( victim ) )		/* lets make NPC's not slaughter PC's */
    {
	if ( victim->fighting
	    && victim->fighting->hunting
	    && victim->fighting->hunting->who == victim )
	    stop_hunting( victim->fighting );

	if ( victim->fighting
	    && victim->fighting->hating
	    && victim->fighting->hating->who == victim )
	    stop_hating( victim->fighting );

        if ( victim->fighting
	    && !IS_NPC( victim )
	    && IS_NPC( ch ) )
          stop_fighting( victim, TRUE );
        else
          stop_fighting( victim, FALSE );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
        stop_fighting( ch, FALSE );

        if(!IS_SET( victim->act, ACT_NOEXP ) || !IS_NPC( victim ) )
	  group_gain( ch, victim );

	if ( !IS_NPC( victim ) )
	{
		if ( IS_NPC( ch ) )
		{
		    victim->pcdata->mdeaths++;
		    if ( is_clan( victim ) )
			victim->pcdata->clan->mdeaths++;
		}
		else
		{
		    ch->pcdata->pkills++;
		    victim->pcdata->pdeaths++;

		    if (   is_clan( ch )
			&& is_clan( victim )
			&& ch->pcdata->clan != victim->pcdata->clan )
		    {
			ch->pcdata->clan->pkills++;
			victim->pcdata->clan->pdeaths++;
		    }
                }

	    sprintf( log_buf, "%s&n killed by %s&n at %d",
		    victim->name,
		    ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
		    victim->in_room->vnum );
	    log_string( log_buf );
            wiznet( ch, WIZ_DEATHS, L_AVA, log_buf );

	    /*
	     * Dying penalty:
	     * 1/2 way back to previous 2 levels.
	     */
           // This was missed when exp loss was reduced.  Newbies no longer lose exp from death.
           // -- Veygoth
           if( ch->level > 5 )
             gain_exp( victim, (0 - (((50 + victim->level) * exp_table[victim->level].lvl_exp ) / 400 )));
           if( victim->level < 2 && victim->exp < 1 )
               victim->exp = 1;	
	}
	else
	{
	    if ( !IS_NPC( ch ))
	    {
		ch->pcdata->mkills++;
		if ( is_clan( ch ) )
		    ch->pcdata->clan->mkills++;
	    }
	}

	raw_kill( ch, victim );
        last_fun( "spell_damage");
        // Keep in mind after this point the character is not in the
        // char_list, not in any room, and is at the menu.  Don't do
        // anything that would cause a segmentation fault - Veygoth

	if (   is_clan( ch )
	    && is_clan( victim )
	    && ch->pcdata->clan != victim->pcdata->clan )
	{
	    ch->pcdata->clan->score += 20;
	}

	/* 
	 * Ok, now we want to remove the deleted flag from the
	 * PC victim.
	 */
	if ( !IS_NPC( victim ) )
	    victim->deleted = FALSE;

	return;
    }

    if ( victim == ch )
	return;

    /*
     * Wimp out?
     */
    if ( IS_NPC( victim ) && dam > 0 )
    {
	if ( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
	      && victim->hit < get_max_hit( victim ) / 5 )
	    || ( is_affected( victim, AFF_CHARM ) && victim->master
		&& victim->master->in_room != victim->in_room ) )
	    {
		start_fearing( victim, ch );
		stop_hunting( victim );
                do_flee( victim, "\0" );
	    }
    }

    if ( !IS_NPC( victim )
	&& victim->hit   > 0
	&& victim->hit  <= victim->wimpy )
	do_flee( victim, "\0" );

    tail_chain( );
    return;
}

CHAR_DATA *check_guarding( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *guard;

    for( guard = victim->in_room->people; guard; guard = guard->next_in_room )
    {
        if( !IS_NPC( guard ) && (guard->pcdata->guarding == victim) && (guard != victim)
            && (guard != ch))
        {
          skill_practice( guard, gsn_guard );
          if(number_percent() < ( ( guard->level + guard->pcdata->skl_lrn[gsn_guard] ) / 4 ) )
          {
            act( "$n&n bravely jumps in front of you!.", guard, NULL, victim, TO_VICT );
            act( "$n&n bravely jumps in front of $N&n.", guard, NULL, victim, TO_ROOM_VICT );
            act( "You heriocally jump in front of $N&n's attacker.", guard, NULL, victim, TO_CHAR );
            if( ch->fighting == victim )
                ch->fighting = guard;
            return guard;
          }
          else
          {
            act( "$n&n watches helplessly as you are attacked.", guard, NULL, victim, TO_VICT );
            act( "$n&n unsuccessfully tries to protect $N&n.", guard, NULL, victim, TO_ROOM_VICT );
            act( "You heroically step aside for $N&n's attacker.", guard, NULL, victim, TO_CHAR );
          }
       }
    }

    return victim;
}

bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !IS_NPC( ch ) && is_affected( ch, AFF_GHOUL ) )
    {
	send_to_char( "&+LYou may not participate in combat while in &+Cgh&n&+co&+Cu&n&+cl&+L form.&n\n\r", ch );
	return TRUE;
    }

    if( !IS_NPC( victim ) && IS_SET( victim->act, PLR_JUST_DIED ))
    {
      
        send_to_char( "&+cThey just died, try in a few seconds...&n\n\r", ch );
        return TRUE;
    }

    if ( !IS_NPC( victim ) && is_affected( victim, AFF_GHOUL ) )
    {
	act( "Your attack passes through $N.",  ch, NULL, victim, TO_CHAR    );
	act( "$n's attack passes through $N.",  ch, NULL, victim, TO_ROOM_VICT );
	act( "$n's attack passes through you.", ch, NULL, victim, TO_VICT    );
	return TRUE;
    }

    // removed restriction allowing only registered players to pkill

    if ( victim->level > LEVEL_HERO && victim != ch && !IS_NPC( victim ) )
    {
        act( "$N&n is an IMMORTAL and is automatically safe.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    if ( victim->fighting )
        return FALSE;

    if ( IS_ROOM( victim->in_room, ROOM_SAFE ) )
    {
	act( "$N&n is in a safe room.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    if ( is_boss( victim ) ) {
        //keep fighting, but allow the boss to invoke his special function
        // ensures the function will be called
        sprintf( lbuf, "%s is a boss", victim->short_descr );
        log_string( lbuf );
        victim->fighting = ch;
        if ( victim->spec_fun )
            (*victim->spec_fun) ( victim, PROC_NORMAL );
        return FALSE;
    }

    if ( IS_NPC( ch ) || IS_NPC( victim ) )
        return FALSE;

    return FALSE;
}

/*
 * Check to see if weapon is poisoned.
 */
int is_wielding_poisoned( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA *obj;
    AFFECT_DATA *paf;

    last_fun( "is_wielding_poisoned" );
/*
    if ( ( obj = get_eq_char( ch, wpn ) )
	&& IS_OBJ_STAT( obj, ITEM_POISONED ) )
        return TRUE;
*/
    obj = get_eq_char( ch, wpn );
    if ( !obj ) return 0;
    for (paf = obj->affected; paf; paf = paf->next) {
        if (paf->skill == gsn_poison_weapon)
            return paf->modifier;
    }

    return FALSE;

}


/*
 * Hunting, Hating and Fearing code. -Thoric
 */
bool is_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hunting || ch->hunting->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hating )
        return FALSE;
    if ( ch->hating->who != victim )
        return FALSE;
    
    return TRUE;    
}

bool is_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->fearing || ch->fearing->who != victim )
      return FALSE;
    
    return TRUE;    
}

void stop_hunting( CHAR_DATA *ch )
{
    last_fun( "stop_hunting");
    if ( ch->hunting )
    {
        sprintf(lbuf,"%s&n has stopped hunting %s.",ch->short_descr, ch->hunting->name );
        wiznet(0, WIZ_HUNTING, 0, lbuf);

	free_string( ch->hunting->name );
	free_mem( ch->hunting, sizeof( HHF_DATA ) );
	ch->hunting = NULL;

    }
    stop_hating( ch );
    return;
}

void stop_hating( CHAR_DATA *ch )
{
    last_fun( "stop_hating");
    if ( ch->hating )
    {
      sprintf(lbuf,"%s&n has stopped hating %s.",ch->short_descr, ch->hating->name);
          wiznet(0, WIZ_HATING, 0, lbuf);
      free_string( ch->hating->name );
      free_mem( ch->hating, sizeof( HHF_DATA ) );
      ch->hating = NULL;
    }
    return;
}

void stop_fearing( CHAR_DATA *ch )
{
    last_fun( "stop_fearing");
    if ( ch->fearing )
    {
	free_string( ch->fearing->name );
	free_mem( ch->fearing, sizeof( HHF_DATA ) );
	ch->fearing = NULL;
    }
    return;
}

void start_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    last_fun( "start_hunting");
    if ( ch->hunting ) {
        if ( ch->hunting->who == victim )
            return;
        stop_hunting( ch );
    }
    ch->hunting = alloc_mem( sizeof( HHF_DATA ) );
    ch->hunting->name = str_dup( victim->name );
    ch->hunting->who  = victim;
    sprintf(lbuf,"%s&n has started hunting %s.",ch->short_descr, ch->hunting->name);
    wiznet(0, WIZ_HUNTING, 0, lbuf);
    return;
}

void start_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{

    last_fun( "start_hating");
    if ( ch->hating ) {
        if ( ch->hating->who == victim )
            return;
        stop_hating( ch );
    }

    ch->hating = alloc_mem( sizeof( HHF_DATA ) );
    ch->hating->name = str_dup( victim->name );
    ch->hating->who  = victim;
    sprintf(lbuf,"%s&n has started hating %s.",ch->short_descr, ch->hating->name);
    wiznet(0, WIZ_HATING, 0, lbuf);
    return;
}

void start_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    last_fun( "start_fearing");
    if( !IS_SET( ch->act, ACT_MEMORY ))
      return;

    if ( ch->fearing )
      stop_fearing( ch );

    ch->fearing = alloc_mem( sizeof( HHF_DATA ) );
    ch->fearing->name = str_dup( victim->name );
    ch->fearing->who  = victim;
    return;
}

/* A function to set a mob to being aggro and/or hunting a PC 
 * The parameter "ranged" is used if player ranged a spell or weapon
 * at the mobile.  Otherwise its a simple call of start_hating
 * and start_hunting.  - Lithos
 */
void start_grudge( CHAR_DATA *ch, CHAR_DATA *victim, bool ranged )
{
    last_fun( "start_grudge");
    if ( !IS_NPC(ch) || IS_NPC(victim) )
      return;
    if( (IS_SET( ch->act, ACT_HUNTER ) && IS_SET(ch->act, ACT_MEMORY)) || ranged )
      start_hunting( ch, victim );
    if( IS_SET( ch->act, ACT_MEMORY ) || ranged )
      start_hating( ch, victim );
    return;
}



/*
 * Check for victim parrying ch.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) || victim->position < POS_RECLINING )
        return FALSE;

    if( is_affected( ch, AFF_DAZZLE ))
        return FALSE;

    if( victim->level < skills_table[gsn_parry].skill_level[victim->class] )
        return FALSE;

    if (  IS_NPC( victim ) )
    {
        // Adjusted by Veygoth
        chance  = victim->level + 4;
        // Mobs more often than not don't have weapons
        // so they should get bonuses for actually
        // having them
        if ( !get_eq_char( victim, WEAR_HAND ) )
        {
            if ( !get_eq_char( victim, WEAR_HAND_2 ) )
                chance -= 5;
        }
        else
            chance += 5;
    }
    else
    {
        if ( !get_eq_char( victim, WEAR_HAND ) )
        {
            if ( !get_eq_char( victim, WEAR_HAND_2 ) )
                return FALSE;
            chance = victim->pcdata->skl_lrn[gsn_parry] / 2;
        }
        else
            chance = victim->pcdata->skl_lrn[gsn_parry];

        skill_practice( victim, gsn_parry );
    }

    if ( number_percent( ) >= ( chance - ch->level ) / 2 )
        return FALSE;

    act( "$N&n skillfully parries your attack.", ch, NULL, victim, TO_CHAR );
    act( "You parry $n&n's fierce attack.",  ch, NULL, victim, TO_VICT );
    act( "$N&n parries $n&n's attack.", ch, NULL, victim, TO_ROOM_VICT );
    if ( ch->fighting == NULL )
   	set_fighting( ch, victim );
    if ( victim->fighting == NULL )
   	set_fighting( victim, ch );
    return TRUE;
}

/*
 * Check for riposte.
 * Returns FALSE if failed, TRUE if successful.
 */
bool check_riposte( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) || victim->position < POS_RECLINING )
        return FALSE;

    if( is_affected( ch, AFF_DAZZLE ) )
        return FALSE;

    if( is_affected( ch, AFF_BLIND ) )
        return FALSE;

    if( is_affected( ch, AFF_CASTING ) )
        return FALSE;

    if( victim->level < skills_table[gsn_riposte].skill_level[victim->class] )
        return FALSE;

    if (  IS_NPC( victim ) )
    {
        // Adjusted by Veygoth, 20% at 50.
        chance  = ((victim->level ) / 3) + 4;
        // Mobs more often than not don't have weapons
        // so they should get bonuses for actually
        // having them
        if ( get_eq_char( victim, WEAR_HAND ) )
        {
                chance += 2;
        }
    }
    else
    {
        if ( !get_eq_char( victim, WEAR_HAND ) )
        {
            chance = 0;
        }
        else
            chance = victim->pcdata->skl_lrn[gsn_riposte] / 5;

        skill_practice( victim, gsn_riposte );
    }

    if ( number_percent( ) >= (chance - ch->level / 3) )
        return FALSE;

    act( "$N&n deflects your blow and strikes back at YOU!", ch, NULL, victim, TO_CHAR );
    act( "You deflect $n&n's attack and strike back at $m.",  ch, NULL, victim, TO_VICT );
    act( "$N&n deflects $n&n's attack and strikes back at $m.", ch, NULL, victim, TO_ROOM_VICT );
    return TRUE;
}

/*
 * Check for block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    int chance;
 
    if( victim->level < skills_table[gsn_shield_block].skill_level[victim->class] )
        return FALSE;

    if( is_affected( ch, AFF_DAZZLE ))
        return FALSE;

    if ( !IS_AWAKE( victim ) || victim->position < POS_RECLINING )
        return FALSE;
 
    if ( !(obj = get_eq_char( victim, WEAR_HAND ))
        || (obj && obj->item_type != TYPE_SHIELD) )
    {
        if ( !(obj = get_eq_char( victim, WEAR_HAND_2 )) )
          return FALSE;
        else if( obj->item_type != TYPE_SHIELD )
          return FALSE;
    }
    if ( obj->item_type != TYPE_SHIELD )
    {
      if ( !(obj = get_eq_char( victim, WEAR_HAND_2 )) )
        return FALSE;
      else if( obj->item_type != TYPE_SHIELD )
        return FALSE;
    }

    if ( IS_NPC( victim ) )
	/* Veygoth was here.  :) */
        chance  = victim->level + 13;
    else
    {
	chance  = ( victim->pcdata->skl_lrn[gsn_shield_block] * 2 ) / 3;
        skill_practice( victim, gsn_shield_block );
    }

    if ( number_percent( ) >= chance - ch->level )
        return FALSE;
 

    switch( number_range( 1, 2 ))
    {
       case 1:
	    act( "You block $n&n's attack with your shield.", ch, NULL, victim, TO_VICT );
	    act( "$N&n blocks your attack with a shield.",    ch, NULL, victim, TO_CHAR    );
	    act( "$N&n blocks $n&n's attack with a shield.",  ch, NULL, victim, TO_ROOM_VICT );
            break;
       case 2:
            // If we were really smart we would check to see whether both the shield
            // and weapon were made of metal before we gave a sparks message...
	    act( "&+CS&n&+cp&+Car&n&+ck&+Cs&n fly off your shield as you block $n&n's attack.", ch, NULL, victim, TO_VICT );
	    act( "$N&n defends against your attack with a shield.",    ch, NULL, victim, TO_CHAR    );
	    act( "$N&n deflects $n&n's attack with a shield.",  ch, NULL, victim, TO_ROOM_VICT );
            break;
       default:
            break;
    }

    if ( ch->fighting == NULL )
   	set_fighting( ch, victim );
    if ( victim->fighting == NULL )
   	set_fighting( victim, ch );

    return TRUE;
}

/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) || victim->position < POS_RECLINING )
	return FALSE;

    if( is_affected( ch, AFF_DAZZLE ))
        return FALSE;

    if( victim->level < skills_table[gsn_dodge].skill_level[victim->class] )
        return FALSE;

    if ( IS_NPC( victim ) )
	/* Veygoth was here.  :) */
        chance  = ((victim->level * 2) / 3) + 15;
    else
        chance  = ( victim->pcdata->skl_lrn[gsn_dodge] * 2 ) / 3;

    // Size difference bonus for dodge for halflings - they get 2% dodge
    // bonus per size difference between them and the attacker. -- Veygoth
    // Drow get a flat 15% bonus.
    if( victim->race == RACE_HALFLING )
    {
        if( ch->size > victim->size )
        {
           chance += 3 * (ch->size - victim->size );
        }
    }
    else if( victim->race == RACE_DROW )
    {
        chance += 15;
    }
    else if( victim->race == RACE_OGRE )
    {
        chance -= 3;
    }

    // Bashed mobs/creatures have a hard time dodging
    if( victim->position < POS_FIGHTING )
    {
        chance -= 25;
    }

    // Leap is 16% max at level 50.  Considering crappy thri hitpoints it's necessary.
    if( victim->race == RACE_THRIKREEN && number_percent() <= (victim->level / 3) )
    {
      act( "$N&n leaps over your attack.", ch, NULL, victim, TO_CHAR );
      act( "You leap over $n&n's attack.", ch, NULL, victim, TO_VICT );
      act( "$N&n leaps over $n&n's attack.", ch, NULL, victim, TO_ROOM_VICT );
      return TRUE;
    }

    skill_practice( victim, gsn_dodge );

    if ( number_percent( ) >= chance - ch->level )
        return FALSE;

    switch( number_range( 1, 2 ))
    {
       case 1:
	    act( "$N&n dodges your attack.", ch, NULL, victim, TO_CHAR );
	    act( "You dodge $n&n's attack.", ch, NULL, victim, TO_VICT );
	    act( "$N&n dodges $n&n's attack.", ch, NULL, victim, TO_ROOM_VICT );
	    break;
       case 2:
	    act( "$N&n sidesteps your attack.", ch, NULL, victim, TO_CHAR );
	    act( "You narrowly dodge $n&n's attack.", ch, NULL, victim, TO_VICT );
	    act( "$N&n avoids $n&n's attack.", ch, NULL, victim, TO_ROOM_VICT );
	    break;
      default:
	    break;
    }

    if ( ch->fighting == NULL )
   	set_fighting( ch, victim );
    if ( victim->fighting == NULL )
   	set_fighting( victim, ch );

    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    CHAR_DATA *ch = victim;

    if ( victim->deleted )
        return;

    if ( !victim ) {
        bug( "Update_pos: null victim.", 0 );
        return;
    }

    if ( IS_NPC( victim ) )
        sprintf( lbuf, "update_pos: %s (%d)", victim->name, victim->pIndexData->vnum );
    else 
        sprintf( lbuf, "update_pos: %s", victim->name);
    last_fun( lbuf );

    if ( victim->hit < -10 && !victim->deleted && victim->in_room )
    {
        if ( victim->riding && victim->in_room == victim->riding->in_room )
	{
            act( "$n&n unceremoniously falls from $N&n.", victim, NULL, victim->riding, TO_ROOM );
	    victim->riding->rider = NULL;
	    victim->riding        = NULL;
	}

	victim->position = POS_DEAD;
        last_fun( "update_pos: *victim is dead");
	return;
    }

    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_RESTING;
    }
    else
    {
	     if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
        else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
        else                          victim->position = POS_STUNNED;

        if ( victim->riding )
        {
	    act( "$n&n falls unconscious from $N&n.", victim, NULL, victim->riding, TO_ROOM );
	    victim->riding->rider = NULL;
            victim->riding        = NULL;
        }
    }
    // If is an NPC just return now. - Lithos
    if ( IS_NPC( victim ) )
        return;

     // Passing out.
    if (  !IS_NPC( ch ) && !IS_IMMORTAL( ch ) 
         && ( get_curr_con (ch) - victim->pcdata->condition[COND_DRUNK] < number_percent() )
	 && ( ( victim->class != CLASS_BARD && ch->pcdata->condition[COND_DRUNK] > 22 )
	      || ( victim->class == CLASS_BARD && ch->pcdata->condition[COND_DRUNK] > 32 ) ) )
    {
    	WAIT_STATE( ch, 5 * PULSE_PER_SECOND );
        victim->pcdata->condition[COND_DRUNK] -= 3;
	if ( victim->position > POS_SLEEPING )
	{
	    send_to_char( "&+WOh, you &n&+wfeel &+Lreally&N&+l bad...&n\n\r", ch );
	    victim->position = POS_SLEEPING;
	    act( "$n&n falls flat on $s face, immobile&n.",
	    		ch, NULL, NULL, TO_ROOM );
	}
    }
    if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && victim->mana < 0 ) 
    {
        sprintf(lbuf, "Update_pos: %s is in mana-debt", victim->name);
        wiznet( 0, WIZ_LSPAM, 0, lbuf );
    	WAIT_STATE( ch, 5 * PULSE_PER_SECOND );
	if ( victim->position > POS_SLEEPING )
	{
	    send_to_char( "&+WYou &n&+wfeel &+Lexhau&n&+lsted...&n\n\r", ch );
	    victim->position = POS_STANDING;
            victim->fighting = NULL;
	    act( "$n&n collapses from exhaustion&n.",
	    		ch, NULL, NULL, TO_ROOM );
            do_sleep( ch, NULL );
	}
    }

    return;
}

/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{

    char buf [ MAX_STRING_LENGTH ];

    last_fun( "set_fighting");
    if ( ch == victim )
        return;

    if ( ch->fighting )
    {
	bug( "Set_fighting: already fighting", 0 );
	sprintf( buf, "...%s attacking %s at %d", 
		( IS_NPC( ch )     ? ch->short_descr     : ch->name     ),
		( IS_NPC( victim ) ? victim->short_descr : victim->name ),
		victim->in_room->vnum );
	bug( buf, 0 );
	return;
    }

    if ( is_affected( ch, AFF_SLEEP ) )
	affect_strip( ch, 0, spl_sleep, 0  );

    if(ch->fly_level != victim->fly_level) {
        start_grudge( ch, victim, FALSE);
        return;
    }

    ch->fighting = victim;
    if( ch->position == POS_STANDING )
      ch->position = POS_FIGHTING;

    return;
}

/*
 * Stop ch from fighting.  fBoth implies stop everyone from killing ch.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    last_fun( "stop_fighting");
    /* If this is true, something is _bad_. */
    if ( ch == NULL )
    {
	bug ( "stop_fighting: NULL character as argument!", 0 );
	return;
    }

    /* Always stop the character from fighting. */
    ch->fighting = NULL;
    if ( ch->position == POS_FIGHTING )
	ch->position = POS_STANDING;

    /* Taking this out of the for loop is MUCH faster. */
    if ( fBoth )
    for ( fch = char_list; fch; fch = fch->next )
    {
	if ( fch->fighting == ch )
	{
	    fch->fighting = NULL;
            if( fch->position == POS_FIGHTING )
                fch->position = POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    OBJ_DATA        *corpse;
    OBJ_DATA        *obj;
    char            *name;
    char             buf [ MAX_STRING_LENGTH ];
    int              corpse_vnum;
    int              timer;
    int		     level; // corpse level Govno
    OBJ_DATA        *obj_next;

    last_fun( "make_corpse");
    // Need a bailout here if undead corpses are supposed to poof.
    // Govno
    if( leaves_no_corpse( ch ))
    {
	return;
    }

    // Different corpse settings for players and mobs - Veygoth
    if( IS_NPC( ch ) )
    {
        corpse_vnum 	= OBJ_VNUM_CORPSE_NPC;
        name            = ch->short_descr;
	timer	 	= number_range( 15, 30 );
        level		= ch->level;  // Corpse level Govno
    }
    else
    {
        corpse_vnum 	= OBJ_VNUM_CORPSE_PC;
        name          	= ch->name;
	timer 		= number_range( 180, 240 ) + (ch->level * 2);
        level           = ch->level; // Corpse level Govno
    }

    /*
     * This longwinded corpse creation routine comes about because
     * we dont want anything created AFTER a corpse to be placed  
     * INSIDE a corpse.  This had caused crashes from obj_update()
     * in extract_obj() when the updating list got shifted from
     * object_list to obj_free.          --- Thelonius (Monk)
     */

    if ( get_cash( ch ) > 0 )
    {
	OBJ_DATA * coins;
	coins         = create_money( ch->money.copper, ch->money.silver,
                                      ch->money.gold, ch->money.platinum );
	corpse        = create_object( get_obj_index( corpse_vnum ), 0 );
	obj_to_obj( coins, corpse );
        ch->money.copper   = 0;
        ch->money.silver   = 0;
	ch->money.gold     = 0;
        ch->money.platinum = 0;
    }
    else
    {
        corpse        = create_object( get_obj_index( corpse_vnum ), 0 );
    }

    corpse->timer = timer;
    corpse->value[0] = race_table[ch->race].parts;

    corpse->level = level; // corpse level Govno

    sprintf( buf, "corpse of %s", name );
    free_string( corpse->name );
    corpse->name = str_dup( buf );

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );
	
    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;

	if ( obj->deleted )
	  continue;

	/*
	 * Remove item inventories from all corpses.
	 * Includes licenses to kill
	 */
//	if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
//	{
//	    extract_obj( obj );
//	}
//	else
//	{
            if ( IS_NPC(ch)  && ch->pIndexData->pShop
              && obj->wear_loc == WEAR_NONE )
            {
          	obj_from_char( obj );
                extract_obj( obj );
            }
            else
            {
          	obj_from_char( obj );
                obj_to_obj( obj, corpse );
            }
//	}
    }

    obj_to_room( corpse, ch->in_room );
    if ( !corpse->in_room ) {
        bug("Make_corpse: corpse sent to null room, extracting", 0 );
        extract_obj(corpse);
    }

    return;
}

/*
 * Pad out first if() clause for elementals, etc. that
 * shouldn't leave corpses with appropriate messages. Govno
 */
bool leaves_no_corpse( CHAR_DATA *ch )
{
    char        msg[ MAX_STRING_LENGTH ];
    bool        no_corpse = 0;
    OBJ_DATA    *obj;
    OBJ_DATA    *obj_next;

    if( IS_UNDEAD( ch ))
    {
        no_corpse = TRUE;
	sprintf( msg, "$n&N crumbles to dust." );
    }
    else if ( IS_ELEMENTAL( ch )) {
        no_corpse = TRUE;
        sprintf( msg, "$n&n returns to the elements from which it formed.");
    }

    if( no_corpse )
    {
        act( msg, ch, NULL, NULL, TO_ROOM );
        if( get_cash( ch ) > 0 )
        {
            OBJ_DATA *coins;
            coins = create_money( ch->money.copper, ch->money.silver, \
                                  ch->money.gold,   ch->money.platinum );
            obj_to_room( coins, ch->in_room );
        }
        for ( obj = ch->carrying; obj; obj = obj_next )
        {
            obj_next = obj->next_content;
            if( obj->deleted )
               continue;
            obj_from_char( obj );
            obj_to_room( obj, ch->in_room );
        }
    }

    return no_corpse;
}

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char            *msg;
    char             mesg [ MAX_STRING_LENGTH ];
    int              vnum;
    int              door;
    int              parts;

    last_fun( "death_cry");
    vnum = 0;
    
    msg   = "You hear the animal shriek of $n&n's death cry.";
    parts = race_table[ch->race].parts;

    if( !ch )
    {
      bug( "Death cry: death cry called with no arguments for CHAR_DATA!", 0 );
      return;
    }
    if (!ch->in_room) {
      bug("Death cry called with ch in no room.", 0);
      return;
    }

    switch ( number_bits( 6 ) )
    {
    default: msg  = "You hear the animal shriek of $n&n's death cry.";      break;
    case  0: msg  = "$n&n pitches toward the &n&+yground&n ... &+LDEAD&n."; break;
    case  1: msg  = "$n&n splatters &n&+rbl&+Ro&n&+ro&+Rd&n all over you."; break;
    case  4: if ( IS_SET( parts, PART_HEAD ) )
	     {
                msg  = "$n&n's &+Rse&n&+rv&+Rer&n&+re&+Rd&n head bounces across the &n&+yground&n.";
		vnum = OBJ_VNUM_SEVERED_HEAD;				
	     }								break;
    case  6: if ( IS_SET( parts, PART_ARMS ) )
	     {
                msg  = "$n&n's arm is sliced from $s &+Ldead&n body.";
		vnum = OBJ_VNUM_SLICED_ARM;				
	     }								break;
    case  7: if ( IS_SET( parts, PART_LEGS ) )
	     {
                msg  = "$n&n's leg is seperated from $s &+Lcorpse&n.";
		vnum = OBJ_VNUM_SLICED_LEG;				
	     }								break;
    }

    sprintf( mesg, "%s", msg );
    act( mesg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
	OBJ_DATA *obj;
	char     *name;
	char      buf [ MAX_STRING_LENGTH ];

	name		= IS_NPC( ch ) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	if ( is_affected( ch, AFF_POISON ) )
	    obj->value[3] = 1;

	obj_to_room( obj, ch->in_room );
    }

      if ( IS_NPC( ch ) )
        msg = "&+lA pained and gutteral scream assaults your ears.&n";
      else
        msg = "&+lYou hear someone's death lamentation.&n";

    was_in_room = ch->in_room;
    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] )
	    && pexit->to_room
	    && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}

void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA *paf;
    CHAR_DATA *wch;
    EVENT_DATA *event;
    ROOM_INDEX_DATA *room;
    bool no_corpse = FALSE;

    last_fun( "raw_kill");

    stop_fighting( victim, TRUE );

    if ( victim->group_leader || victim->next_in_group )
        remove_from_group( victim, victim );
    if ( ch != victim )
    {
        if( IS_NPC( victim ) && victim->pIndexData->death_fun )
        {
          ( *victim->pIndexData->death_fun ) ( victim, PROC_NORMAL );
        }
	prog_death_trigger( victim );
        last_fun( "raw_kill");
    }
    if ( IS_NPC( victim ) && victim->spec_fun ) {
        no_corpse = (*victim->spec_fun) ( victim, PROC_DEATH );
    }
    if ( !no_corpse )
        make_corpse( victim );
    last_fun( "raw_kill");

    /* Strip all event-spells from victim! */
    for( event = event_first; event; event = event->next ) 
    {
    	if ( event->type == EVENT_IMMOLATE 
	   || event->type == EVENT_ACID_ARROW )
	    if( (CHAR_DATA *)event->arg2 == victim ) 
		event->deleted = TRUE;
    }

    if ( victim->rider )
    {
        act( "$n&n dies suddenly, and you topple to the &n&+yground&n.", 
	    victim, NULL, victim->rider, TO_VICT );
        victim->rider->riding	  = NULL;
        victim->rider->position	  = POS_RESTING;
        victim->rider		  = NULL;
    }

    if ( victim->riding )
    {
        act( "$n&n topples from you, &+Ldead&n.", 
	    victim, NULL, victim->riding, TO_VICT );
        victim->riding->rider	   = NULL;
        victim->riding		   = NULL;
    }

    if ( !IS_NPC( victim ) && is_affected( victim, AFF_VAMP_BITE ) )
        victim->race = RACE_VAMPIRE;

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	/* Keep the ghoul affect */
	if ( !IS_NPC( victim ) && is_affected( victim, AFF_GHOUL ) )
	    continue;

        affect_remove( victim, paf );
    }

    if ( IS_NPC( victim ) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE( 0, victim->level, MAX_LEVEL-1 )].killed++;
	extract_char( victim, TRUE );
	return;
    }
    else
        //save corpses, don't wait til next save_corpse event
        save_corpses();
    // Character has died in combat, extract them to repop point and put
    // them at the menu.
    victim->pcdata->lastrent = 0;
    extract_char( victim, FALSE );
    victim->armor        = 100;
    victim->position     = POS_STANDING;
    victim->hit	         = UMAX( 1, victim->hit  );
    victim->mana         = UMAX( 1, victim->mana );
    victim->move         = UMAX( 1, victim->move );
    if (!IS_NPC (victim) )
    {
    	victim->pcdata->condition[COND_FULL  ] = 20;
    	victim->pcdata->condition[COND_THIRST] = 20;
    	victim->pcdata->condition[COND_DRUNK]  = 00;
        victim->pcdata->mod_hit = 0;
    }
    victim->group_leader    = NULL;
    victim->next_in_group   = NULL;
    victim->hitroll         = 0;
    victim->damroll         = 0;
    victim->saving_throw[0] = 0;
    victim->saving_throw[1] = 0;
    victim->saving_throw[2] = 0;
    victim->saving_throw[3] = 0;
    victim->saving_throw[4] = 0;
    victim->mod_str = 0;
    victim->mod_int = 0;
    victim->mod_wis = 0;
    victim->mod_dex = 0;
    victim->mod_con = 0;
    victim->mod_agi = 0;
    victim->mod_cha = 0;
    victim->mod_pow = 0;
    victim->mod_luk = 0;
    if ( !IS_NPC( victim ) ) {
        victim->pcdata->maxmod_str = 0;
        victim->pcdata->maxmod_int = 0;
        victim->pcdata->maxmod_wis = 0;
        victim->pcdata->maxmod_dex = 0;
        victim->pcdata->maxmod_con = 0;
        victim->pcdata->maxmod_agi = 0;
        victim->pcdata->maxmod_cha = 0;
        victim->pcdata->maxmod_pow = 0;
        victim->pcdata->maxmod_luk = 0;
    }



    /*
     * Pardon crimes once justice system is complete - Veygoth
     */

    if ( victim->race == RACE_VAMPIRE )
    {
	victim->pcdata->condition[COND_FULL  ] = 0;
	victim->pcdata->condition[COND_THIRST] = 0;
    }

    // This is where we send them to the menu.
    die_follower( victim, victim->name );

    if( victim->in_room )
    {
        room = victim->in_room;
    }
    else
    {
        room = get_room_index( repop_point[victim->race][victim->class] );
        if( !room )
        {
          send_to_char( "The repop point for your race/class does not exist.  Please bug this.\n\r", victim );
          room = get_room_index( ROOM_VNUM_START );
        }
        if( !IS_NPC( victim ) && get_room_index( victim->pcdata->currhome ) )
        {
          room = get_room_index( victim->pcdata->currhome );
        }
    }
    char_from_room( victim );
    if( room )
        victim->in_room = room;

    // Put them in the correct body
    if( victim->desc && victim->desc->original )
    {
        do_return( victim, "\0" );
    }

    // Reset reply pointers
    for( wch = char_list; wch; wch = wch->next )
      if( wch->reply == victim )
          wch->reply = NULL;

    save_char_obj( victim );

    for( wch = char_list; wch; wch = wch->next )
    {
	if( wch == victim )
	{
           char_list = victim->next;
        }
        else if( wch->next && wch->next == victim )
        {
 	   if( victim->next )
           {
              wch->next = victim->next;
           }
           else
           {
              wch->next = NULL;
           }
        }
    }

    do_help( victim, "_login_menu" );

    if( victim->desc )
      victim->desc->connected = CON_MENU;
    else
      victim->deleted = TRUE;

    // Just died flag used for safe time after re-login -- Veygoth
    SET_BIT( victim->act, PLR_JUST_DIED );

    /* Dunno why, but this keeps the MUD alive. */
    tail_chain( );

    return;
}

void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *gch;
    char       buf[ MAX_STRING_LENGTH ];
    int        members;
    int        xp;
    int        highest = 1;

    last_fun( "group_gain");

    /*
     * Monsters will now eat a share of the experience in a group if it gets the kill.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
	return;
    
    members = 0;

    // Check for highest level group member and number of members in room
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
        if( gch->level > highest )
            highest = gch->level;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: %d members.", members );
	members = 1;
    }

    if( !IS_NPC( victim ) && !IS_NPC( ch ))
      check_frag( ch, victim );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
        int factor;

	if ( !is_same_group( gch, ch ) || IS_NPC( gch ))
	    continue;

        // Previously factor was set to be 110% - 10% per group member.  This caused people in groups
        // to get insane high amounts of experience.  This was originally added because players were
        // getting seemingly too little exp in groups larger than two.  This method gives them 94% +
        // 6% per group member of original exp which is then split among the members.
        // This will cause groups with the following number of members to get a certain percentage as
        // compared to the solo person:
        //  1 = 100%   2 = 53%   3 = 37%   4 = 29%   5 = 24%   6 = 21%   7 = 19%   8 = 17%   9 = 16%
        // 10 = 15%   11 = 14%  12 = 13%  13 = 13%  14 = 13%  15 = 13%  16 = 12%  17 = 11%  18 = 11%
        // 19 = 10%   20 = 10%
        // Previously the total amount of experience was split among the group.  This yielded percentages
        // of:
        //  1 = 100%   2 = 50%   3 = 33%   4 = 25%   5 = 20%   6 = 16%   7 = 14%   8 = 12%   9 = 11%
        // 10 = 10%   11 =  9%  12 =  8%  13 =  7%  14 =  7%  15 =  6%  16 =  6%  17 =  5%  18 =  5%
        // 19 =  5%   20 =  5%
        //
        // While this may not seem like a big difference, 3 to 4 person groups have basically gotten a
        // 10-15% increase, while 5-7 person groups have basically gotten a 20-35% increase.
        //
        // This should be quite a bit better, but depending on how the actual results are, this equation
        // may need to be changed so that it may be as high as 90% + 10% per group member of original
        // exp.  However, instead of making a large change, if things don't seem right, try a change
        // of just 1% at first and gauge the results.
        //
        // Since experience is one of the most delicate and precise factors of the MUD, it needs to be
        // changed and tweaked in very small increments.  Please contact Veygoth (veygoth@stax.net)
        // if you plan on making any changes
        //
        // -- Veygoth 5-24-00
        //
        // After making a few observations on the math of the experience, I've decided to finalize it at
        // 91 + members * 9 / members.
        //
        // The chart for this is as follows:
        //  1 = 100%   2 = 54%   3 = 39%   4 = 31%   5 = 27%   6 = 24%   7 = 22%   8 = 20%   9 = 19%
        //
        // However we use a different curve for those in groups of 10 or more.  We don't like large
        // groups.
        //
        // We use the curve we were previously using, 94 + members * 6 / members.
        //
        // 10 = 15%   11 = 14%  12 = 13%  13 = 13%  14 = 13%  15 = 13%  16 = 12%  17 = 11%  18 = 11%
        // 19 = 10%   20 = 10%
        //
        // The overall bonus impact of this is that compared to two days ago people will get this
        // percentage more experience:
        //
        // 1 = 0%  2 = 1.8%  3 = 5.4%  4 = 6.9%  5 = 12.5%  6 = 14.3%  7 = 15.8%  8 = 17.6%  9 = 18.8% 
        //
        // Groups 10 or larger will not have any difference from 5-24.
        //
        // -- Veygoth 5-26-00
        //
        // It seems that people didn't have the skills to handle our exp curve.  Wusses.  Well
        // i've made it quite a bit easier on people in groups of 2-4 and a slight bit easier on
        // groups of 5-9 by using 
        //
        // The chart for this is as follows:
        //  1 = 100%   2 = 65%   3 = 53%   4 = 40%   5 = 36%   6 = 28%   7 = 26%   8 = 20%   9 = 19%
        //
        // The overall increases are:
        // 2 = 20.4% 3 = 35.9% 4 = 29.0% 5 = 33.3% 6 = 16.7% 7 = 18.2%

        if( members < 4 )
          factor = (70 + (members * 30)) / members;
        else if( members < 6 )
          factor = (80 + (members * 20)) / members;
        else if( members < 8 )
          factor = (80 + (members * 15)) / members;
        else if( members < 10 )
          factor = (90 + (members * 10)) / members;
        else
          factor = (94 + (members * 6)) / members;

	xp = xp_compute( gch, victim ) * factor / 100;

        // Prevent total cheese leveling with unbalanced groups.  Well, not
        // prevent, but reduce.  A level 10 grouped with a 30 will end up
        // getting 80% of experience.  A level 1 with a 50 will end up
        // getting 51%.  Not enough of a reduction IMHO, but a little might
        // be all that we need.
        /* I increased this 150% as plvling is too easy. - Lohrr */
        if( highest > (ch->level + 10))
        {
           xp = (xp * (100 - (3*(highest - ch->level)) / 2 )) / 100;
        }

        // Only check trophy for NPC as victim, killer over level 5,
        // and victim less than 20 levels lower than killer.
        // -- Veygoth

        if( IS_NPC( victim ) && !IS_NPC( gch ) && (gch->level > 4) && ((ch->level - victim->level) < 20))
           xp = ( xp * check_trophy( gch, victim, members ) ) / 100;

        if( xp == 0 )
          continue;

	if ( ( race_table[ch->race].racewar_side != race_table[gch->race].racewar_side )
	     && ( !IS_NPC( ch ) && !IS_NPC( gch ) ) )
	{
            send_to_char( "You lost half of the experience by grouping with scum.\n\r", ch );
	    xp /= 2;
	}

        if( xp > 0 )
          sprintf( buf, "You receive your portion of the experience.\n\r" );
        else
          sprintf( buf, "You lose your portion of the experience.\n\r" );
	send_to_char( buf, gch );
        if( !IS_NPC( gch ))
          gain_exp( gch, xp );

	for ( obj = gch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL    )
		    && IS_EVIL   ( gch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD    )
		    && IS_GOOD   ( gch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL )
		    && IS_NEUTRAL( gch ) ) )
	    {
                act( "&+LYou are wracked with &n&+rp&+Ra&n&+ri&+Rn&+L by&n $p&+L.&n", gch, obj, NULL, TO_CHAR );
                act( "$n&+L convulses in &+Cp&n&+ca&+Ci&n&+cn&+L from&n $p&+L.&n",   gch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, gch->in_room );
	    }
	}
    }

    return;
}

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    int       percent = 100;
    int       xp;
    int sign;
    int align_dir;
    int chance;

    // Veygoth - exp tables completely redone since stock code.
    // UltraEnvy used a flat line table, and Magma
    // uses a semi-exponential table called exp_table
    // which is defined in const.c

    // no exp is awarded for anything 20 levels below you.
    // exp is reduced by 10% per level of the creature below you
    // for the first 9 levels, and then another 1% for the next 9 levels.
    // It actually counts down starting at 91%.
       /* If victim is lower level */
    if( victim->level < gch->level )
    {
       /* If victim is less than 10 levels below */
      if( gch->level - victim->level < 10 )
      {
         percent = 101 - (( gch->level - victim->level ) * 10);
      }
       /* If victim is less than 20 levels below */
      else if( gch->level - victim->level < 20 )
      {
         percent = 20 - ( gch->level - victim->level );
      }
      else
      {
         percent = 0;
      }
    }
       /* If victim is over 10 levels over */
    else if( victim->level > (gch->level + 10) && gch->level <= 20 )
    {
      // Experience penalty for killing stuff way higher than you, 33 level difference and you
      // get about nothing.
      // Tweaked this slightly, 96 % at 10 levels above, 96% at 10 levels, etc
      percent = 129 - ((victim->level - gch->level) * 4);
      if( percent < 2 )
        percent = 2;
    }
    else
        percent += (victim->level - gch->level);

/*
 *  trying new align change formula - Lithos
    // Alignment adjustment -- Veygoth
    if( number_percent( ) < 50 - gch->level + victim->level )
    {
        if( gch->alignment > victim->alignment && gch->alignment > -1000 )
          gch->alignment++;
        else
        {
          gch->alignment--;
          if( gch->alignment == -1000 )
              send_to_char( "&+rYou give in to the &+ldark&+r side.\n\r", gch )
        }
    }

*/
    if ( gch->alignment > 0 ) 
        sign = 1;
    else
        sign = -1;
    if ( victim->alignment > 0 )
        align_dir  = -1;
    else
        align_dir = 1;
    chance = abs(gch->alignment - victim->alignment) - sign * gch->alignment;
    chance /= 10;
    if ( chance < 0 ) chance *= -1;
    if ( gch->level > victim->level)
        chance -= (gch->level - victim->level);
    chance = URANGE ( 0, chance, 100 );

    sprintf(lbuf, "Xp_compute: %s has a %d chance of gaining %d align.",
        gch->name, chance, align_dir );
    wiznet( 0, WIZ_LSPAM, 0, lbuf);
    if ( number_percent() < chance && gch->alignment != -1000 )
    {
         gch->alignment += align_dir;
         if( gch->alignment <= -1000 )
             send_to_char( "&+RThe d&+rarkside t&+lakes over...&n\n\r", gch );
    }

    gch->alignment = URANGE( -1000, gch->alignment, 1000 );

    // 25% bonus for sactuary
    if ( is_affected( victim, AFF_SANCTUARY ))
    {
	percent = ( percent * 5 ) / 4;
    }

    // 10% bonus for fireshield
    if ( is_affected( victim, AFF_FLAMING ) )
    {
	percent = ( percent * 11 ) / 10;
    }

    // 12.5% bonus for an armed opponent
    if ( ( obj = get_eq_char( victim, WEAR_HAND ) ) )
    {
	percent = ( percent * 9 ) / 8;
    }
    
    // 8.25% bonus for secondary weapon
    if ( ( obj = get_eq_char( victim, WEAR_HAND_2 ) ) )
    {
	percent = ( percent * 13 ) / 12;
    }

    // 10% bonus for racially hated mobs
    if ( !str_infix( race_table[victim->race].name,
		    race_table[gch->race].hate ) )
    {
	percent = ( percent * 11 ) / 10;
    }
    
    // 10% penalty for killing same race
    if ( victim->race == gch->race )
    {
	percent = ( percent * 9 ) / 10;
    }

    // Lowbie experience bonus was eliminated since the exp_table
    // made it no longer necessary

    if ( IS_NPC( victim ) )
    {
        // 5% bonus for aggros
	if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
	{
   	  percent = ( percent * 21 ) / 20;
	}

        // 50% penalty for killing shopkeepers
	if ( victim->pIndexData->pShop != 0 )
   	  percent = percent / 2;

        // no bonus for special function #1 because they get that by having a class

        // 10% bonus for special function #2
        if( victim->spec_fun2 != 0 )
        {
   	  percent = ( percent * 11 ) / 10;
        }
    }
    else
    {
        // Player-vs-player experience
        // no exp for under level 5, 1/5 exp for level 6-10, double exp
        // for non-newbies (20+)
        // Changed by Veygoth on 6-9-00 so that killing a level 1-5 makes you lose
        // experience, and you get no experience for anyone under level 10.
        // Those 11-20 are worth half experience.
        //
        // Took out exp penalty for killing a newbie 6/26/00  - Lithos
        if( victim->level < 6)
        {
            send_to_char( "You killed a newbie!  You feel like a twink!\n\r", gch );
            return 0;
        }
        else if( victim->level < 11)
        {
            return 0;
        }
        else if( victim->level < 20)
        {
            percent /= 5;
        }
        else
        {
            percent *= 2;
        }

        if( race_table[victim->race].racewar_side == race_table[gch->race].racewar_side )
        {
            send_to_char( "You gain no experience for killing your own side.\n\r", gch );
            return 0;
        }
    }


    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to - 50%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     *
     * Removed by Veygoth because this code was obsoleted by
     * trophy code.
     */

    xp = ( percent * exp_table[victim->level].mob_exp ) / 100;

    xp     = UMAX( 0, xp );

    return xp;
}

void spl_dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt,
		 bool immune )
{
    const  char         *attack;
           char          buf1           [ 256 ];
           char          buf2           [ 256 ];
           char          buf3           [ 256 ];
           char          buf4           [ 256 ];
           char          buf5           [ 256 ];

    last_fun( "spl_dam_message");
        if( dam < 0 )
        {
            bug( "Spell_damage:  Negative damage from dt %d", dt );
            dam = 0;
        }

	if ( dt >= 0 && dt < MAX_SPELL )
	    attack	= spells_table[dt].name;

        if ( immune )
	{
	    sprintf( buf1, "$N&n seems unaffected by your %s!",	 attack );
	    sprintf( buf2, "$n&n's %s seems powerless against you.", attack );
	    sprintf( buf3, "$N&n seems unaffected by $n&n's %s!", attack );
	    sprintf( buf4, "Luckily, you are immune to your %s.", attack );
	    sprintf( buf5, "$n&n seems unaffected by $s own %s.", attack );
	}
        else if( dam == 0 )
        {
	    sprintf( buf1, "Your %s misses $N&n.",	 attack );
	    sprintf( buf2, "$n&n's %s misses you.", attack );
	    sprintf( buf3, "$N&n avoids being hit by $n&n's %s!", attack );
	    sprintf( buf5, "Luckily, you are not hit by your %s.", attack );
	    sprintf( buf4, "$n&n fails to hit $mself with $s own %s.", attack );
        }
	else
	{
		sprintf( buf1, spells_table[dt].msg_damage );
		sprintf( buf2, spells_table[dt].msg_damage_vict );
		sprintf( buf3, spells_table[dt].msg_damage_room );
		sprintf( buf4, spells_table[dt].msg_damage_self );
		sprintf( buf5, spells_table[dt].msg_damage_self_rm );
	}

    if ( victim != ch )
    {
  	act( buf1, ch, NULL, victim, TO_CHAR    );
	act( buf2, ch, NULL, victim, TO_VICT    );
	act( buf3, ch, NULL, victim, TO_ROOM_VICT );
    }
    else
    {
	act( buf4, ch, NULL, victim, TO_CHAR    );
	act( buf5, ch, NULL, victim, TO_ROOM    );
    }
      
    return;
}

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt,
		 int wpn, bool immune )
{
    const  char         *vp1;
    const  char         *vp2;
    const  char         *adj;
    const  char         *attack;
           char          buf            [ MAX_STRING_LENGTH ];
           char          buf1           [ 256 ];
           char          buf2           [ 256 ];
           char          buf3           [ 256 ];
           char          buf4           [ 256 ];
           char          buf5           [ 256 ];
           char          punct;

    last_fun( "dam_message");
    if( victim->position == POS_SLEEPING )
    {
        act( "$n&n has a rude awakening!", victim, NULL, NULL, TO_ROOM );
        victim->position = POS_RESTING;
        set_fighting( victim, ch );
    }

    if ( dt == gsn_bash )
    {
        act( "You send $N&n crashing to the &n&+yground&n with your powerful bash.", ch, NULL, victim, TO_CHAR );
        act( "$n&n's powerful bash sends you sprawling!", ch, NULL, victim, TO_VICT );
        act( "$n&n sends $N&n sprawling with a powerful bash!", ch, NULL, victim, TO_ROOM_VICT );
	return;
    }

    if ( dt == gsn_headbutt)
    {
        if ( ch != victim && dam > victim->hit + 10 )
        {
            // a killing blow needs some nice verbage
            act("You swiftly split $N&n's skull with your forehead, sending &+Rblood&n&+r and brains&n flying!",
                ch, NULL, victim, TO_CHAR);
            act("$n&n rears back and splits $N&n's skull with $s forehead!", ch, NULL, victim, TO_NOTVICT);
            act("&+RBlood&n&+r and brains&n splatter everywhere as $N&n goes limp and collapses.",
                ch, NULL, victim, TO_NOTVICT);
            act("$n&n's forehead smashes into you, crushing your skull and bringing on a wave of &+Lblackness&n.",
                ch, NULL, victim, TO_VICT);
            //deal some decent damage
        }
        else if ( ch != victim && dam > 100 ) {
            // a pretty nasty headbutt
            act("Your headbutt smashes into $N&n's skull.", ch, NULL, victim, TO_CHAR);
            act("$n&n's headbutt smashes into $N&n's skull.", ch, NULL, victim, TO_NOTVICT);
            act("$n&n's head smashes into you, sending you reeling.", ch, NULL, victim, TO_VICT);
            //deal some decent damage
        }
        else if (ch != victim ){
            act("$n&n's headbutt leaves a red welt on $N&n's forehead.", ch,
                NULL, victim, TO_NOTVICT);
            act("Your headbutt leaves a red welt on $N&n's forehead.", ch,
                NULL, victim, TO_CHAR);
            act("$n&n's headbutt leaves a red welt on your forehead.", ch,
                NULL, victim, TO_VICT);
        }

     /* left the damage to self messages in the do_headbutt() function
      * because they are a little too tricky to implement through
      * this function */
        return;
    }
    if ( dt == gsn_bodyslam && ch != victim )
    {
        act( "You bodyslam $N&n!", ch, NULL, victim, TO_CHAR );
        act( "$n&n bodyslams you!\n\rYou are stunned!", ch, NULL, victim, TO_VICT );
        act( "$n&n bodyslams $N&n.", ch, NULL, victim, TO_ROOM_VICT );
    }

    if( dt == gsn_instant_kill )
    {
      switch( number_range( 1, 3 ))
      {
       case 1:
       case 2:
        act( "You place your weapon in the back of $N&n, resulting in some strange noises, some blood, and a corpse!", ch, NULL, victim, TO_CHAR );
        act( "You realize you should have kept your vital organs somewhere safe as $n&n stabs you to death.", ch, NULL, victim, TO_VICT );
        act( "$n&n places $s weapon in the back of $N&n, resulting in some strange noises, some blood, and a corpse!", ch, NULL, victim, TO_ROOM_VICT );
        break;
       case 3:
        act( "You place your weapon in the back of $N&n with such force that it comes out the other side!", ch, NULL, victim, TO_CHAR );
        act( "$n&n ends your life with a well-placed backstab.", ch, NULL, victim, TO_VICT );
        act( "$n&n places $s weapon in the back of $N&n with such force that it comes out the other side!", ch, NULL, victim, TO_ROOM_VICT);
        break;
      }
      return;
    }

    // Multiple backstab messages, feel free to add more.
    if ( dt == gsn_backstab && dam > 0 )
    {
      switch( number_range( 1, 4 ))
      {
        case 1:
          act( "$N&n howls in agony as you pierce $S backbone!", ch, NULL, victim, TO_CHAR );
          act( "You howl in agony as you feel &+rpain&n in your back!", ch, NULL, victim, TO_VICT );
          act( "$N&N howls in agony as $n&n pierces $S backbone!", ch, NULL, victim, TO_ROOM_VICT );
          break;
        case 2:
          act( "You place your $p&n silently and skillfully through the spine of $N&n.", ch, get_eq_char( ch, wpn ), victim, TO_CHAR );
          act( "Your spine feels $p&n neatly slicing through it.", ch, get_eq_char( ch, wpn ), victim, TO_VICT );
          act( "$n&n places $s $p&n into $N&n's back!", ch, get_eq_char( ch, wpn ), victim, TO_ROOM_VICT );
          break;
        case 3:
          act( "Blood flies everywhere as you stab $N&n in the back!", ch, NULL, victim, TO_CHAR );
          act( "You feel a sharp stabbing sensation in your back!", ch, NULL, victim, TO_VICT );
          act( "Blood flies everywhere as $n&n places $s $p&n into $N&n's back!", ch, get_eq_char( ch, wpn ), victim, TO_ROOM_VICT );
          break;
        case 4:
          act("You smile with perverse pleasure as your $p&n plunges through $N&n's soft tissue, piercing vital organs.", ch, get_eq_char( ch, wpn ),victim, TO_CHAR);
          act("$n&n smiles with perverse pleasure as $s $p&n plunges through $N&n's soft tissue, piercing vital organs.", ch, get_eq_char( ch, wpn),victim, TO_VICT);
          act("$n&n smile with perverse pleasure as $s $p&n plunges through $N&n's soft tissue, piercing vital organs.", ch, get_eq_char( ch, wpn ),victim, TO_ROOM_VICT);

      }
      return;
    }
    if ( dt == gsn_poison_weapon  || dt == gsn_poison
        || dt == gsn_poison_bite )
        return;

    // Adjectives based on amount of damage done.
    adj = "\0";
         if( dam > 100 ) { adj = " godly"; }
    else if( dam > 75  ) { adj = " devastating"; }
    else if( dam > 55 )  { adj = " mighty"; }
    else if( dam > 40 )  { adj = " awesome"; }
    else if( dam > 25 )  { adj = " powerful"; }
    else if( dam > 4   ) { adj = "\0"; }  // no message modifier for normal hits
    else if( dam > 2   ) { adj = " mediocre"; }
    else if( dam > 0   ) { adj = " feeble"; }

    vp2 = "\0";
    if ( dam ==   0 ) { vp1 = "misses";vp2 = "\0";  }
    else
    {
        dam *= 100;
	if ( victim->hit > 0 )
	    dam /= victim->hit;

             if ( dam <=   1 ) { vp1 = "scratches";             }
	else if ( dam <=   2 ) { vp1 = "grazes";                }
	else if ( dam <=   3 ) { vp1 = "hits";                  }
	else if ( dam <=   4 ) { vp1 = "hits";
				 vp2 = " hard";                 }
	else if ( dam <=   5 ) { vp1 = "hits";
				 vp2 = " very hard";            } 
	else if ( dam <=  10 ) { vp1 = "mauls";                 }
	else if ( dam <=  15 ) { vp1 = "decimates";             }
	else if ( dam <=  20 ) { vp1 = "makes";
                                 vp2 = " stagger in pain";      }
	else if ( dam <=  25 ) { vp1 = "maims";                 }
	else if ( dam <=  30 ) { vp1 = "mutilates";             }
	else if ( dam <=  40 ) { vp1 = "disembowels";           }
	else if ( dam <=  50 ) { vp1 = "eviscerates";           }
	else if ( dam <=  75 ) { vp1 = "enshrouds";
				 vp2 = " in a mist of blood";   }
	else                   { vp1 = "beats the crap out of"; }
    }

    punct   = ( dam <= 40 ) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
        if ( ch->race > MAX_RACE )
	{
	    bug( "Dam_message:  %d invalid race", ch->race );
	    ch->race = 0;
	}

        attack = race_table[ch->race].dmg_message;

	sprintf( buf1, "Your%s %s %s $N&n%s%c",    adj, attack, vp1, vp2, punct );
	sprintf( buf2, "$n&n's%s %s %s you%s%c",   adj, attack, vp1, vp2, punct );
	sprintf( buf3, "$n&n's%s %s %s $N&n%s%c",  adj, attack, vp1, vp2, punct );
	sprintf( buf4, "You%s %s %s yourself%s%c", adj, attack, vp1, vp2, punct );
	sprintf( buf5, "$n&n's%s %s %s $m%s%c",    adj, attack, vp1, vp2, punct );
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skills_table[dt].noun_damage;
	else if (   dt >= TYPE_HIT
		 && dt  < TYPE_HIT + MAX_ATTACK )
	    attack	= attack_table[dt - TYPE_HIT].name;
	else
	{
	    sprintf( buf, "Dam_message: bad dt %d for %d damage caused by %s to %s with weapon %d.",
                    dt,
		    dam,
                    ch->name,
                    victim->name,
                    wpn );
	    bug( buf, 0 );
	    dt      = TYPE_HIT;
	    attack  = attack_table[0].name;
	}

	if ( immune )
	{
	    sprintf( buf1, "$N&n seems unaffected by your %s!",	 attack );
	    sprintf( buf2, "$n&n's %s seems powerless against you.", attack );
	    sprintf( buf3, "$N&n seems unaffected by $n&n's %s!", attack );
	    sprintf( buf4, "Luckily, you seem immune to %s.",		 attack );
	    sprintf( buf5, "$n&n seems unaffected by $s own %s.", attack );
	}
	else
	{
	    if ( dt > TYPE_HIT && is_wielding_poisoned( ch, wpn ) )
	    {
		sprintf( buf1, "Your poisoned %s %s $N&n%s%c",  attack, vp1, vp2, punct );
		sprintf( buf2, "$n&n's poisoned %s %s you%s%c", attack, vp1, vp2, punct );
		sprintf( buf3, "$n&n's poisoned %s %s $N&n%s%c",attack, vp1, vp2, punct );
		sprintf( buf4, "Your poisoned %s %s you%s%c", 	attack, vp1, vp2, punct );
		sprintf( buf5, "$n&n's poisoned %s %s $m%s%c",	attack, vp1, vp2, punct );
	    }
	    else
	    {
   	        sprintf( buf1, "Your%s %s %s $N&n%s%c",    adj, attack, vp1, vp2, punct );
		sprintf( buf2, "$n&n's%s %s %s you%s%c",   adj, attack, vp1, vp2, punct );
		sprintf( buf3, "$n&n's%s %s %s $N&n%s%c",  adj, attack, vp1, vp2, punct );
		sprintf( buf4, "You%s %s %s yourself%s%c", adj, attack, vp1, vp2, punct );
		sprintf( buf5, "$n&n's%s %s %s $m%s%c",    adj, attack, vp1, vp2, punct );
	    }
	}
    }

    if ( victim != ch )
    {
	act( buf1, ch, NULL, victim, TO_CHAR    );
	act( buf2, ch, NULL, victim, TO_VICT    );
	act( buf3, ch, NULL, victim, TO_ROOM_VICT );
    }
    else
    {
	act( buf4, ch, NULL, victim, TO_CHAR    );
	act( buf5, ch, NULL, victim, TO_ROOM    );
    }
      
    return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    last_fun( "disarm");
    if ( ( ch->size - victim->size ) < -2 )
        return;

    if ( !( obj = get_eq_char( victim, WEAR_HAND ) ) )
	if ( !( obj = get_eq_char( victim, WEAR_HAND_2 ) ) )
	    return;

    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
	return;

    if (   !get_eq_char( ch, WEAR_HAND   )
	&& !get_eq_char( ch, WEAR_HAND_2 )
	&& number_bits( 1 ) == 0 )
	return;

    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
    act( "$n DISARMS $N!",  ch, NULL, victim, TO_ROOM_VICT );

    obj_from_char( obj );
   // obj_to_room( obj, victim->in_room ); //Jolle removed drop in room
	obj_to_char( obj, victim ); // put it in inventory instead
    sprintf( lbuf, "Disarm: %s disarmed by %s.", victim->name, ch->name );
    wiznet( 0, WIZ_LSPAM, 0, lbuf );
return;
}

/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{

    last_fun( "trip");
    if ( can_fly( victim ))
        return;

    if ( victim->riding )
    {
	if( can_fly( victim->riding ))
  	  return;

        act( "$n trips your mount and you crash to the &n&+yground&n!", ch, NULL, victim, TO_VICT    );
        act( "You trip $N's mount and $N is thrown off!", ch, NULL, victim, TO_CHAR    );
        act( "$n trips $N's mount and $N falls off!", ch, NULL, victim, TO_ROOM_VICT );
	victim->riding->rider = NULL;
        victim->riding        = NULL;

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
        return;         
    }

    if ( victim->wait == 0 )
    {
	act( "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
        act( "$n trips you and you topple like a pile of bricks!", ch, NULL, victim, TO_VICT    );
        act( "$n trips $N and $N's face meets the &nearth&n!", ch, NULL, victim, TO_ROOM_VICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}

void do_kill( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_kill");
    one_argument( argument, arg );

    if( is_affected( ch, AFF_HOLD ) ||
      is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You can't move!\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Don't be a moron.\n\r", ch );
	return;
    }

    kill_ch( ch, victim );

}

void kill_ch( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int        chance;

    victim = check_guarding( ch, victim );
    if ( is_safe( ch, victim ) )
    {
	return;
    }
    // is_safe could wipe out victim, as it calls procs if a boss
    // check and see that victim is still valid
    if ( !victim )
        return;

    if ( ch->position == POS_FIGHTING || ch->fighting )
    {
      if( victim == ch->fighting )
      {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
      }
      else
      {
        if( IS_NPC( ch ))
        {
          chance = (ch->level * 3 / 2 + 15);
        }
        else if( ch->level >= skills_table[gsn_switch].skill_level[ch->class] )
        {
          chance = ch->pcdata->skl_lrn[gsn_switch];
          skill_practice( ch, gsn_switch );
        }
        else
        {
          chance = ch->level / 2;
        }
        if( number_percent() < chance )
        {
          send_to_char( "You switch opponents!\n\r", ch );
          act( "$n&n switches targets...", ch, NULL, victim, TO_CHAR );
          act( "$n&n switches targets...", ch, NULL, victim, TO_ROOM_VICT );
          ch->fighting = victim;
          WAIT_STATE( ch, skills_table[gsn_switch].beats );
          return;
        }
        else
        {
          send_to_char( "You can't seem to break away from your current opponent.\n\r", ch );
          stop_fighting( ch, FALSE );
          WAIT_STATE( ch, skills_table[gsn_switch].beats );
          return;
        }
      }
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND );
    return;
}

/*
 * I'm only allowing backstabbing with the primary weapon...immortals
 * who wield two weapons, with the first not being a dagger, will be
 * unable to backstab or circle.  Tough cookie.  --- Thelonius
 * Added double backstab. The cookie tasted good. - Lohrr
 * Backstab was removed from multihit as it can initiate battle.
 */
void do_backstab( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_backstab");
    if ( ch->level < skills_table[gsn_backstab].skill_level[ch->class] )
    {
	send_to_char(
            "You better leave the assassination trade to those more skilled.\n\r", ch );
	return;
    }
 
    if ( !check_blind( ch ) )
        return;

    if ( ch->riding )
    {
        send_to_char( "You can't get close enough while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }
    
    backstab( ch, victim );
    return;
}

/* The backstab function was cloned from do_backstab for use by mobile AI code.
 * the difference is that a mob that wants to backstab already knows their
 * victim, so no need for the argument stuff
 * -- Veygoth
 *
 * Modified by to allow auto-stab for backstabbers with aggr_level set.  Called
 * from do_backstab()
 * -- Lithos
 * 
 * Added ability to stab if piercer is secondary. Useful for mercs. - Lithos
 *
 * Downgraded damage considerably. Reduced damage for bards and mercs
 */
bool backstab( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA  *obj, *obj2;
    int chance = 0;
    int stab_chance;
    int hand;

    // Can't stab if lacking the skill
    last_fun( "backstab");
    if ( ch->level < skills_table[gsn_backstab].skill_level[ch->class] ) {
        send_to_char("Leave backstabbing to the assassins!\n\r", ch);
        return FALSE;
    }

    // Can't stab if blind
    if ( !check_blind( ch ) )
        return FALSE;

    // Can't stab on horseback
    if ( ch->riding ) {
        send_to_char( "You can't get close enough while mounted.\n\r", ch );
        return FALSE;
    }

    // Can't stab yourself
    if ( victim == ch )
	return FALSE;

    victim = check_guarding( ch, victim );

    if ( is_safe( ch, victim ) )
      return FALSE;
    // is_safe could wipe out victim, as it calls procs if a boss
    // check and see that victim is still valid
    if ( !victim )
        return FALSE;

    /* Check size of ch vs. victim. */
    if ( victim->position > POS_SLEEPING )
    {
    /* If ch is more than 2 sizes smaller it's too small. */
    if( (ch->size - 2) > victim->size   && victim->position >= POS_STUNNED
      && !(IS_NPC(ch) && ch->pIndexData->vnum == 10165) )
    {
        send_to_char( "Such tiny beings evade your skills.\n\r", ch );
        return FALSE;
    }
    /* Ch 2 or more sizes larger than victim => bad! */
    if ( (ch->size + 2) < victim->size && victim->position >= POS_STUNNED ) {
        send_to_char( "It is rather ineffective to stab someone in the calf.\n\r", ch );
        return FALSE;
    }
    }

    hand = 0;
    if ( ( obj = get_eq_char( ch, WEAR_HAND ) )
        && obj->item_type == TYPE_WEAPON
	&&  (*attack_table[obj->value[3]].wpn_gsn) == gsn_pierce )  
        hand = WEAR_HAND;
    if ( !hand && ( obj2 = get_eq_char( ch, WEAR_HAND_2 ) )
        && obj2->item_type == TYPE_WEAPON
        && (*attack_table[obj2->value[3]].wpn_gsn) == gsn_pierce )
        hand = WEAR_HAND_2; 
    if ( !hand )
    {
        send_to_char( "You need to wield a piercing weapon.\n\r", ch );
        return FALSE;
    }

    if ( !IS_NPC(ch) ) skill_practice( ch, gsn_backstab );

    check_killer( ch, victim );

    WAIT_STATE( ch, skills_table[gsn_backstab].beats );
    if ( IS_NPC( ch ) )
    {
        stab_chance = 2 * ch->level;
    }
    else 
    {
         stab_chance = ch->pcdata->skl_lrn[gsn_backstab];
    }
    if ( abs(ch->size - victim->size) == 2 ) stab_chance -= 10;
    switch ( victim->position )
    {
         default: break;
        case POS_MORTAL:
        case POS_INCAP:
        case POS_UNCONSCIOUS:
            stab_chance += 80;
            break;
        case POS_STUNNED:  
        case POS_SLEEPING:
            stab_chance += 40;
            break;
        case POS_RECLINING:  
            stab_chance += 20;
            break;
        case POS_RESTING:
        case POS_SITTING:
            stab_chance += 10;
            break;
    } //end switch
    // Half as likely to succeed on those that are aware
    if( is_affected( victim, AFF_AWARE ) || is_affected( victim, AFF_SKL_AWARE ))
    {
        chance /= 2;
    }
    sprintf(lbuf,"do_backstab: %s is attempting with a %d%% chance.", ch->name, stab_chance);
    wiznet(0, WIZ_LSPAM, 0, lbuf);
    if (  number_percent( ) < stab_chance )
    {
        /* First hit on backstab.  Check for instant kill. - Veygoth */
	if( ch->level >= skills_table[gsn_instant_kill].skill_level[ch->class] )
        {
             if( !IS_NPC( ch ))
               chance = ch->pcdata->skl_lrn[gsn_instant_kill];
             else
               chance = (ch->level * 3) / 2 + 20;

             // People over level 50 get a bonus, equates to about 1-2%
             if( ch->level > 50 )
               chance += 25;

             chance += ( ch->level - victim->level );

             // Immortals will get a bonus too
             if( IS_IMMORTAL( ch ))
               chance *= 4;

             // Half as likely to succeed on those that are aware
	     if( is_affected( victim, AFF_AWARE ) || is_affected( victim, AFF_SKL_AWARE ))
    	     {
        	chance /= 2;
    	     }

             if( number_range( 1, 20000 ) < chance )
             {
               if (!IS_NPC(ch) ) skill_practice( ch, gsn_instant_kill );
               {
                   sprintf(lbuf,"backstab: %s hit an instakill on %s with a %d chance in hundredths of a percent.",
                     ch->name, victim->name, (chance / 30));
                   wiznet( 0, WIZ_LSPAM, 0, lbuf);
                   log_string( lbuf );
                   WAIT_STATE( ch, 15 );
	           one_hit( ch, victim, gsn_instant_kill, hand );
                   return TRUE;
               }
             }
        }
    	
        one_hit( ch, victim, gsn_backstab, hand );
        /* No double stabs when the first stab kills'm. -Lohrr */
        if( victim->position == POS_DEAD )
            return TRUE;

        /* Case of thieves/assassins doing a double backstab. */
        obj = get_eq_char( ch, WEAR_HAND_2);
    	/* Stop 2nd-hand non-pierce backstabs. */
	if ( !IS_NPC(ch) && ( obj != NULL ) && ( hand != WEAR_HAND_2 )
              && obj->item_type == TYPE_WEAPON
 	      && (*attack_table[obj->value[3]].wpn_gsn) == gsn_pierce )
            {	
        /* Thieves get 1/2 chance at double, assassins get 2/3. */
        // Veygoth - removed double stab for thieves 6-9-00
	if ((ch->class == CLASS_ASSASSIN ) && (number_percent( ) < ch->pcdata->skl_lrn[gsn_backstab] * 2 / 3 )) 
                {
sprintf(lbuf,"backstab: %s hit a double backstab.",ch->name);
wiznet(0, WIZ_LSPAM, 0, lbuf);
	         one_hit( ch, victim, gsn_backstab, WEAR_HAND_2 );
                }
            }
    }
    else	/* Send a "you miss your backstab" messge & engage. */
	damage( ch, victim, 0, gsn_backstab, WEAR_HAND, DAM_PIERCE );
    return TRUE;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_circle");
    /* Verify that ch can circle. */
    if ( !IS_NPC( ch )
	&& ch->level < skills_table[gsn_circle].skill_level[ch->class] )
    {
	send_to_char(
            "You'd better leave the assassination trade to those more skilled.\n\r", ch );
	return;
    }

    /* No charmies or NPC's are allowed to circle. */
    if ( IS_NPC( ch ) && is_affected( ch, AFF_CHARM ) )
	return;

    /* Yeah, gallop around them without them noticing. */
    if ( ch->riding )
    {
        send_to_char( "You can't circle while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );
    
    /* Find the unlucky soul. */
    if ( arg[0] == '\0' )
	victim = ch->fighting;
    else
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

    /* No target. */
    if ( !victim )
    {
    	send_to_char( "Circle who?\n\r", ch );
    	return;
    }

    /* Run around yourself? Ok. */
    if ( victim == ch )
    {
	send_to_char( "You spin around in a circle.  Whee!\n\r", ch );
	return;
    }

    /* Check for protection of victim. */
    victim = check_guarding( ch, victim );
    if ( is_safe( ch, victim ) )
      return;
    // is_safe could wipe out victim, as it calls procs if a boss
    // check and see that victim is still valid
    if ( !victim )
        return;

    /* Check if someone is attacking ch. */
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( rch->fighting == ch )
	    break;
    if ( rch )
    {
	send_to_char( "You're too busy being hit right now.\n\r", ch );
	return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_HAND ) )
	|| obj->value[3] != 11 )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    act( "You circle around behind $N&n...", ch, NULL, victim, TO_CHAR    );
    act( "$n&n circles around behind $N&n...", ch, NULL, victim, TO_ROOM_VICT );
    
    check_killer( ch, victim );
    WAIT_STATE( ch, skills_table[gsn_circle].beats );

    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->skl_lrn[gsn_circle] +
             get_curr_agi( ch ) - get_curr_agi( victim ) )
      {
	/* Don't always switch. */
	if (number_percent ( ) < 40 )
	    stop_fighting( victim, FALSE );
	one_hit( ch, victim, gsn_circle, WEAR_HAND );
      }
    else
        act( "You failed to get around $M!", ch, NULL, victim, TO_CHAR );

    skill_practice( ch, gsn_circle );

    return;
}

void do_flee( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *now_in;
    ROOM_INDEX_DATA *was_in;
    int              attempt;
    int              chances;
    char             buf[MAX_STRING_LENGTH];

    if ( ch->position < POS_RECLINING || ch->wait > 0 )
    {
	return;
    }

    // Remove memorization and meditation bits - Veygoth
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEDITATING ))
    {
        act( "$N&n is disrupted from meditation.", ch, NULL, NULL, TO_ROOM );
        act( "Your meditation is disrupted.", ch, NULL, NULL, TO_CHAR );
        REMOVE_BIT( ch->act, PLR_MEDITATING );
    }
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    {
        act( "$N&n abandons $S studies.", ch, NULL, NULL, TO_ROOM );
        act( "You abandon your studies.", ch, NULL, NULL, TO_CHAR );
        REMOVE_BIT( ch->act, PLR_MEMORIZING );
    }
 
    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "You scramble madly to your feet!\n\r", ch );
        act( "$n&n scrambles madly to $s feet!",
	    ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	return;
    }

    // Panicked people can flee when not fighting - Veygoth
    if ( !( victim = ch->fighting ) )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
    }

    if ( has_affect( ch, gsn_berserk, 0, 0 ) )
    {
        send_to_char( "You can't flee, you're in a &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n!!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_BOUND ) ) 
    {
        send_to_char( "You are bound!  You can't move!\n\r", ch );
        act( "$n&n tries to flee, but is tied up!",
	    ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HOLD ) ||
         IS_AFFECTED( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You can't move!\n\r", ch );
        act( "$n&n tries to flee, but $e can't move!",
	    ch, NULL, NULL, TO_ROOM );
	return;
    }

    // You should almost always be able to flee when not fighting.
    if ( ch->position == POS_STANDING )
	chances = 30;
    else
	chances = 6;

    was_in = ch->in_room;
    for ( attempt = 0; attempt < chances; attempt++ )
    {
	EXIT_DATA *pexit;
	int        door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == NULL
	    ||   !pexit->to_room
            ||   pexit->to_room == was_in
	    ||   IS_SET( pexit->exit_info, EX_CLOSED )
	    || ( IS_NPC( ch )
		&& ( IS_ROOM( pexit->to_room, ROOM_NO_MOB )
		    || ( IS_SET( ch->act, ACT_STAY_AREA )
			&& pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	if ( ch->riding && ch->riding->fighting )
	    stop_fighting( ch->riding, TRUE );

// This shouldn't happen anymore
//    ROOM_INDEX_DATA *now_in;
//	if ( ( now_in = ch->in_room ) == was_in )
//	    continue;

        // Just to keep the damned messages from being wacky...
        SET_AFF_BIT( ch, AFF_IS_FLEEING );
	move_char( ch, door );
        REMOVE_AFF_BIT( ch, AFF_IS_FLEEING );
        if( ch->in_room == was_in )
            break;
        now_in = ch->in_room;
        ch->in_room = was_in;

        act( "$n&n panics and attempts to flee...", ch, NULL, NULL, TO_ROOM );
        if( IS_AFFECTED( ch, AFF_SNEAK ))
        {
            act( "$n&n has fled!", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            sprintf( buf, "$n&n flees %sward.", dir_name[door] );
            act( buf, ch, NULL, NULL, TO_ROOM );
        }
        ch->in_room = now_in;

        sprintf( buf, "You flee %sward!\n\r", dir_name[door] );
	send_to_char( buf, ch );

	stop_fighting( ch, TRUE );
	return;
    }

    act( "$n&n tries to flee but can't make it out of here!", ch, NULL, NULL, TO_ROOM );
    send_to_char( "&+LPANIC!&n You cannot escape!\n\r", ch );
    return;
}

void do_berserk( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    last_fun( "do_berserk");
    /* Don't allow charmed mobs to do this, check player's level */
    if ( ( IS_NPC( ch ) && is_affected( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skills_table[gsn_berserk].skill_level[ch->class] ) )
    {
        send_to_char( "You're not enough of a warrior to enter a &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n.\n\r",
		     ch );
	return;
    }
    if( IS_NPC( ch ) )
    {
        send_to_char( "This has been disabled until someone figures out why"
            " it's crashing the mud.", ch );
        return;
    }

    if ( has_affect( ch, gsn_berserk, 0, 0  ) )
    {
	if ( number_percent() + 20 > ch->pcdata->skl_lrn[gsn_berserk] ) 
        {
	    send_to_char( "You failed to calm yourself down!\n\r", ch);
	    WAIT_STATE( ch, skills_table[gsn_berserk].beats );
	    return;
        
        }
	else
	{
            send_to_char( "You no longer see targets everywhere.\n\r", ch);
            affect_strip( ch, gsn_berserk, 0, 0 );
            WAIT_STATE( ch, skills_table[gsn_berserk].beats );
	    return;
        }
    }
	//Jolle -serk when ever.
    //if ( !ch->fighting )                                         
    //{                                   
    //    send_to_char( "You aren't fighting anyone.\n\r", ch );
    //    return;                                                      
    //}                                                                       

    send_to_char(
                 "Your slam your weapon into your foe and &+Rbl&n&+ro&+Ro&n&+rd&n splatters all over!\n\r",
		 ch );
    send_to_char( "The sight of &+Rbl&n&+ro&+Ro&n&+rd&n begins to drive you crazy!\n\r",
		 ch );

    if ( !IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->skl_lrn[gsn_berserk] )
    {
	af.skill     = gsn_berserk;
        af.spell     = 0;
        af.song      = 0;
	af.duration  = dice(1, 2);
	af.location  = APPLY_HITROLL;
	af.modifier  = UMIN( ch->level / 6, 2 );
	set_bitvector( &af, AFF_NONE);
	affect_to_char( ch, &af );

	af.location  = APPLY_DAMROLL;
	affect_to_char( ch, &af );

	af.location  = APPLY_AC;
	af.modifier  = ch->level;
	affect_to_char( ch, &af );

	af.location  = APPLY_MAX_CON;
	af.modifier  = dice( 5, 9 );
	affect_to_char( ch, &af );

	af.location  = APPLY_AGI;
	af.modifier  = 0 - dice( 5, 9 );
	affect_to_char( ch, &af );

	af.location  = APPLY_MAX_STR;
	af.modifier  = dice( 5, 9 );
	affect_to_char( ch, &af );

        send_to_char( "You are overcome by &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n!!\n\r", ch );
        act( "$n has slipped into a &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n!!", ch, NULL, NULL, TO_ROOM );

	return;
    }
    send_to_char( "You force the madness into submission.\n\r", ch );

    skill_practice( ch, gsn_berserk );

    return;
}

void do_rescue( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        count;
    last_fun( "do_rescue");

    /* Don't allow the unskilled to rescue */
    if ( ( ch->level < skills_table[gsn_rescue].skill_level[ch->class] ) )
    {
	send_to_char( "You'd better leave the heroic acts to warriors.\n\r", ch );
	return;
    }

    if ( has_affect( ch, gsn_berserk, 0, 0  ) )
    {
        send_to_char( "You can't rescue anyone, you're in a &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n!!\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

/* No reason you shouldn't be able to rescue mobs.
    if ( !IS_NPC( ch ) && IS_NPC( victim ) )
    {
	send_to_char( "They don't need your help!\n\r", ch );
	return;
    }
*/

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( !victim->fighting )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( !check_blind ( ch ) )
        return;

    WAIT_STATE( ch, skills_table[gsn_rescue].beats );

    count = 0;
    for ( fch = victim->in_room->people; fch; fch = fch->next_in_room )
    {

	if ( fch->deleted )
	    continue;

	if ( fch->fighting == victim )
	{
	    if ( number_range( 0, count ) == 0 )
	        break;
	    count++;
	}
    }

    skill_practice( ch, gsn_rescue );

    if ( !fch
	|| ( !IS_NPC( ch )
	    && number_percent( ) > ch->pcdata->skl_lrn[gsn_rescue] ) )
    {
        act( "$n&n fails miserably in $s attempt to rescue $N&n.", ch, NULL, victim, TO_ROOM_VICT );
        act( "$n&n fails miserably in $s attempt to rescue you.", ch, NULL, victim, TO_VICT );
        act( "You fail in your attempt to rescue $N&n.", ch, NULL, victim, TO_CHAR );
	return;
    }

    act( "&+WYou leap in front of $N&n!",  ch, NULL, victim, TO_CHAR    );
    act( "&+W$n&+W rescues you!", ch, NULL, victim, TO_VICT    );
    act( "&+w$n&+w leaps in front of $N&+w taking his place in battle!&n", ch, NULL, victim, TO_ROOM_VICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( fch, ch );
    if( !ch->fighting )
        set_fighting( ch, fch );
    return;
}

void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA 		*victim;
    ROOM_INDEX_DATA	*kicked_into;
    char       		 arg [ MAX_INPUT_LENGTH ];
    char       		 arg1 [ MAX_INPUT_LENGTH ];
    char                 buf[MAX_STRING_LENGTH];
    int	       		 door;
    int                  chance;
    int                  wallkickchance;

    last_fun( "do_kick");

    /* Check player's level and class, allow mobs to do this too */
    if ( ( ch->level < skills_table[gsn_kick].skill_level[ch->class] ) )
    {
	send_to_char(
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) && !ch->fighting )
        return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) 
	     || victim->deleted || victim->position == POS_DEAD )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }
    else
    {
        if( !victim || victim->deleted || victim->position == POS_DEAD )
        {
	  send_to_char( "You aren't fighting anyone.\n\r", ch );
	  return;
        }
    }

    if( victim == ch )
    {
       send_to_char( "You kick yourself for being a dummy.\n\r", ch );
       return;
    }

    WAIT_STATE( ch, number_fuzzy(skills_table[gsn_kick].beats) );

    if( !IS_NPC( ch ))
    {
      chance = ch->pcdata->skl_lrn[gsn_kick];
      skill_practice( ch, gsn_kick );
    }
    else
      chance = ch->level * 3 / 2 + 20;
    // It's much harder to kick really tiny things; imagine trying to kick
    //  a fly.
    if( ch->size > victim->size + 5 )
      chance -= (ch->size - victim->size) * 5;
    // It's harder to kick things that move faster than you.
    chance += get_curr_agi( ch ) - get_curr_agi( victim );
    
    // damned high penalty for kicking blind
    if ( is_affected( ch, AFF_BLIND) && !is_affected( victim, AFF_BLIND ) )
        chance /= 10;
    if ( is_affected( ch, AFF_BLIND) && is_affected( victim, AFF_BLIND ) )
        chance /= 4;

    // If the victim is two or more sizes smaller than the kicker give them a chance
    // to be kicked into a wall or out of the room.
    //
    // Chance of 5% per size class difference, no maximum
    // (wall/room kick is 50% at a difference of 10 sizes)
    if( victim->size - 1 >= ch->size )
      wallkickchance = 0;
    else
      wallkickchance = ((ch->size - victim->size) * 5) - 5;

    // Check for kick success
    if ( number_percent( ) < chance )
    {
	/* Check for wall kick. */
        /* to be kicked out of the room (random direction). */
	if ( number_percent( ) < wallkickchance )
	{
            // Let immortals choose the direction to kick the victim
	    if ( IS_IMMORTAL (ch) && atoi(arg1) >= 0 && atoi (arg1) < MAX_DIR )
		door = atoi (arg1);
            else
            // Changed to number_door to be safe.
	        door = number_door();

	    kicked_into = NULL;

            // Check for valid room stuff on victim
	    if ( victim && victim->in_room && victim->in_room->exit
		 && victim->in_room->exit[door]
		 && victim->in_room->exit[door]->to_room
		 && victim->in_room->exit[door]->exit_info
                 && !IS_SET( victim->in_room->exit[door]->exit_info, EX_SECRET ) 
                 && !IS_SET( victim->in_room->exit[door]->exit_info, EX_BLOCKED )
                 && !IS_SET( victim->in_room->exit[door]->exit_info, EX_WALLED )
                 && !IS_SET( victim->in_room->exit[door]->exit_info, EX_CLOSED )
                 && !room_is_private( victim->in_room->exit[door]->to_room )
                 && victim->in_room->exit[door]->to_room->sector_type != SECT_UNDERG_IMPASSABLE
		 && ( kicked_into = victim->in_room->exit[door]->to_room ) )
	    {
		stop_fighting( victim, TRUE );
                sprintf( buf, "$N&n is sent flying out of the room %sward by $n&n's mighty kick.", dir_name[door] );
		act( buf, ch, NULL, victim, TO_ROOM_VICT );
                sprintf( buf, "$N&n is sent flying out of the room %sward by your mighty kick.", dir_name[door] );
                act( buf, ch, NULL, victim, TO_CHAR );
                act( "You are sent flying out of the room by $n's mighty kick!", ch, NULL, victim, TO_VICT );
		char_from_room( victim );
		char_to_room( victim, kicked_into );
                last_fun( "do_kick");
            	act( "$n&n is stunned!", victim, NULL, NULL, TO_ROOM );
            	send_to_char( "You are stunned!\n\r", victim );
	        WAIT_STATE( victim, ( skills_table[gsn_kick].beats * 9 ) / 10 );
	        if ( victim->position > POS_RESTING )
	            victim->position = POS_RESTING;
	    }
	    else
	    {
                // If no exit in our chosen direction, must be a wall.
                act( "$N&n is sent flying into the wall by $n&n's mighty kick.", ch, NULL, victim, TO_ROOM_VICT );
                act( "$N&n is sent flying into the wall by your mighty kick.", ch, NULL, victim, TO_CHAR );
                act( "You are smacked into the wall by $n's mighty kick!", ch, NULL, victim, TO_VICT );
	        if ( victim->position > POS_RESTING )
	            victim->position = POS_RESTING;
                // At least a two second stun
                WAIT_STATE( victim, 8 );
	        /* Check for stunning victim. */
	        if ((number_percent( ) * 2) > get_curr_agi( victim ) )
	        {
            	    act( "$N&n is stunned!", ch, NULL, victim, TO_ROOM_VICT );
            	    send_to_char( "You are stunned!\n\r", victim );
	            WAIT_STATE( victim, ( skills_table[gsn_kick].beats * 9 ) / 10 );
	        }
                else
                {
                    WAIT_STATE( victim, 1 );
                }
	    }

            // Do excessive damage compared to a normal kick
	    damage( ch, victim, dice( 2, ch->level), TYPE_UNDEFINED, WEAR_NONE, DAM_BASH );

	} // Check for wall kick (execute regular kick)
	else
        {
	    damage( ch, victim, number_range( 1, ch->level ), gsn_kick, WEAR_NONE, DAM_BASH );
        }

    } // Check for successful kick (missed kick)
    else
    {
	damage( ch, victim, 0, gsn_kick, WEAR_NONE, DAM_BASH );
    }

    return;
}

/*
 * Bash by Veygoth
 * Usable to initiate combat and during combat
 */
void do_bash( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    int        chance;
    CHAR_DATA *victim;
    char       arg  [ MAX_INPUT_LENGTH ];
    char       lbuf [ MAX_INPUT_LENGTH ];

    last_fun( "do_bash");
    /* Check player's level and class, mobs can use this skill */
    if ( (ch->level < skills_table[gsn_bash].skill_level[ch->class] ) )
    {
	send_to_char(
            "You'd better leave that to those with more skills.\n\r", ch );
	return;
    }
    one_argument( argument, arg );

    if ( !check_blind( ch ) && !ch->fighting )
        return;

    /* Verify a target. */
    victim = ch->fighting;
    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) 
	   || victim->deleted || victim->position == POS_DEAD )
	{
            send_to_char( "They aren't anywhere to be found.\n\r", ch );
	    return;
	}
    }
///dddd
    else
    {
        if( !victim || victim->deleted || victim->position == POS_DEAD )
        {
	  send_to_char( "You aren't fighting anyone.\n\r", ch );
	  return;
        }
    }

    /* Bash self? Ok!  Govno */
    // Toned down the damage cuz you can't really charge into yourself
    // like you could with someone else.  - Veygoth
    if ( victim == ch )
    {
        send_to_char( "You throw yourself to the ground!\n\r", ch );
        act( "$N&n knocks $mself to the ground.", ch, NULL, victim, TO_ROOM_VICT);
        ch->position = POS_KNEELING;
        WAIT_STATE( ch, ( skills_table[gsn_bash].beats * 8 ) / 10 );
        damage( ch, ch, number_range( 1, 3 ), gsn_bash, WEAR_NONE,
                DAM_BASH );
        return;
    }

    /* Check size of ch vs. victim. */
    /* If ch is too small. */
    if ( ch->size < victim->size )
    {
        act( "$N&n is too big for you to bash!", ch, NULL, victim, TO_CHAR );
	return;
    }
    /* Ch 2 or more sizes larger than victim => bad! */
    if ( ch->size - 2 > victim->size )
    {
        act( "You nearly topple over as you try to bash $N&n.", ch, NULL, victim, TO_CHAR );
        act( "$n&n nearly topples over as $e attempts to bash you.", ch, NULL, victim, TO_VICT );
        act( "$n&n nearly topples over as $e attempts to bash $N&n.", ch, NULL, victim, TO_ROOM_VICT );
        WAIT_STATE( ch, ( skills_table[gsn_bash].beats ));
        ch->position = POS_KNEELING;
	if ( victim->fighting == NULL )
	   set_fighting( victim, ch );
	return;
    }

    /* Lag to basher from bash. Pets get more lag because pets are cheesy */
    if( !IS_NPC( ch ))
      WAIT_STATE( ch, number_fuzzy( skills_table[gsn_bash].beats ));
    else
      WAIT_STATE( ch, (skills_table[gsn_bash].beats * 6 / 5));

    /* Base chance to bash, followed by chance modifications. */
    if( IS_NPC( ch ) )
        chance = (ch->level * 3) / 2 + 15;
    else
        chance = ch->pcdata->skl_lrn[gsn_bash] - 5;

    if( victim->position < POS_FIGHTING )
        chance /= 5; //used to be 0 - Lithos
    else
        chance +=  get_curr_agi( ch ) - get_curr_agi( victim );

    if( chance > 95 )
        chance = 95;

    if ( !(obj = get_eq_char( ch, WEAR_HAND )) )
    {            /* No primary item. */
      if ( !(obj = get_eq_char( ch, WEAR_HAND_2 )) )
      {          /* No items in hand. */
        if( ch->class != CLASS_PALADIN && ch->class != CLASS_ANTIPALADIN )
        {
           if( ch->class != CLASS_WARRIOR )
             chance -= 25;
           else
             chance -= 20;
           send_to_char( "You lower your shoulder and attempt to bash without a shield...\n\r", ch );
        }
        else
        {
           chance -=3; // Hidden penalty for not having a shield
        }
      }
      else if( obj->item_type != TYPE_SHIELD )
      {          /* Secondary item isn't a shield, no primary. */
        if( ch->class != CLASS_PALADIN && ch->class != CLASS_ANTIPALADIN )
        {
           if( ch->class != CLASS_WARRIOR )
             chance -= 25;
           else
             chance -= 20;
           send_to_char( "Bashing without a shield is tough, but you try anyway...\n\r", ch );
        }
        else
        {
           chance -= 5; // Small hidden penalty for not having a shield
        }
      }          /* Secondary item is a shield, no primary. */
      else if( ch->class == CLASS_PALADIN || ch->class == CLASS_ANTIPALADIN )
      {
        chance += 3; // Small hidden bonus for having a shield
      }
    }
    else if ( obj->item_type != TYPE_SHIELD )
    {            /* Primary item isn't a shield. */
      if ( !(obj = get_eq_char( ch, WEAR_HAND_2 )) )
      {          /* No secondary. */
        if( ch->class != CLASS_PALADIN && ch->class != CLASS_ANTIPALADIN )
        {
           if( ch->class != CLASS_WARRIOR )
             chance -= 25;
           else
             chance -= 20;
           send_to_char( "Without a shield, bashing is a wistful thought, but you try anyway...\n\r", ch );
        }
        else
        {
           chance -=5; // Hidden penalty for not having a shield
        }
      }
      else if( obj->item_type != TYPE_SHIELD )
      {          /* Secondary item is not a shield. */
        if( ch->class != CLASS_PALADIN && ch->class != CLASS_ANTIPALADIN )
        {
           if( ch->class != CLASS_WARRIOR )
             chance -= 25;
           else
             chance -= 20;
           send_to_char( "Without a shield, your shoulder bash is but wishful thinking...\n\r", ch );
        }
        else
        {
           chance -=5; // Hidden penalty for not having a shield
        }
      }
      else if( ch->class == CLASS_PALADIN || ch->class == CLASS_ANTIPALADIN )
      {          /* Secondary is a shield. */
        chance += 3; // Small hidden bonus for having a shield
      }
      else if( ch->class == CLASS_RANGER )
      {
        chance -= 8;
      }
      else if( ch->class != CLASS_WARRIOR )
      {
        chance -= 5;
      }
    }

    // Centaurs are awful damned hard to bash -- Veygoth
    if( victim->race == RACE_CENTAUR )
    {
      chance -= 25;
    }

    // damned high penalty for bashing blind
    if ( is_affected( ch, AFF_BLIND) && !is_affected( victim, AFF_BLIND ) )
        chance /= 10;
    if ( is_affected( ch, AFF_BLIND) && is_affected( victim, AFF_BLIND ) )
        chance /= 4;

    skill_practice( ch, gsn_bash );

    /* Start a fight if not already in one. */
    if( ch != victim )
    {
        if( !ch->fighting )
            set_fighting( ch, victim );
        if( !victim->fighting )
            set_fighting( victim, ch );
    }

    sprintf( lbuf,"Bash: %s bashing %s with %d chance.", ch->name,
        victim->name, chance );
    wiznet( 0, WIZ_LSPAM, 0, lbuf);

    /* Do the bash, deal the damage. */
    if ( number_percent( ) < chance )
    {
        /* Hit the bash. */
        if( is_affected( victim, AFF_SINGING ))
        {
            REMOVE_AFF_BIT( victim, AFF_SINGING );
            act( "$n&n chokes on a note and falls silent as $e slams into the ground!", victim, NULL, NULL, TO_ROOM );
            send_to_char( "You abort your singing!\n\r", victim );
        }
        if( is_affected( victim, AFF_CASTING ))
        {
            REMOVE_AFF_BIT( victim, AFF_CASTING );
            act( "$n&n's eyes roll back in $s head and $e forgets all about $s spell.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Being knocked over so forcefully makes it hard to cast.\n\r", victim );
        }
        if( !check_tumble( victim ))
        {
          WAIT_STATE( victim, ( ( skills_table[gsn_bash].beats * 5 ) / 6 ) );
  	  if ( victim->position > POS_KNEELING )
             victim->position = POS_KNEELING;
          send_to_char("You are knocked to the ground!\n\r", victim );
          damage( ch, victim, number_range( 1, ch->level ), gsn_bash, WEAR_NONE, DAM_BASH );
        }
        else
        {
          damage( ch, victim, number_range( 1, (ch->level / 3)), gsn_bash, WEAR_NONE, DAM_BASH );
          send_to_char( "You roll with the blow, finally landing on your feet.\n\r", victim );
          act( "$n&n rolls with the blow, finally landing on $s feet.", victim, NULL, NULL, TO_ROOM );
        }
    }
    else
    { /* Miss the bash. */
        act( "As $N&n avoids your bash, you topple to the &n&+yground&n with a loud crash.", ch, NULL, victim, TO_CHAR );
        act( "$n&n crashes to the &n&+yground&n as you sidestep $s bash.", ch, NULL, victim, TO_VICT );
        act( "$n&n misses $s bash at $N&n and is driven to the &n&+yground&n.", ch, NULL, victim, TO_ROOM_VICT );
        ch->position = POS_KNEELING;
    }

    return;
}

/*
 * Trip by Veygoth
 * Usable to initiate combat and during combat
 */
void do_trip( CHAR_DATA *ch, char *argument )
{
    int chance;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_trip");
    /* Check player's level and class, mobs can use this skill */
    if ( (ch->level < skills_table[gsn_trip].skill_level[ch->class] ) )
    {
	send_to_char(
            "You would just fall over if you tried to trip someone.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
            send_to_char( "You look around, unable to find them.\n\r", ch );
	    return;
	}
    }
    else
    {
        if( !victim )
        {
	  send_to_char( "You aren't fighting anyone.\n\r", ch );
	  return;
        }
    }

    /* Govno anti 'trip me' code */
    if ( victim == ch )
    {
        send_to_char( "You don't think you're clumsy enough already?\n\r", ch );
        return;
    }

    WAIT_STATE( ch, number_fuzzy(skills_table[gsn_trip].beats) );
    skill_practice( ch, gsn_trip );

    if( IS_NPC( ch ))
        chance = (ch->level * 3) / 2 + 10;
    else
        chance = ch->pcdata->skl_lrn[gsn_trip];

    if( chance > 90 )
        chance = 90;

    if ( (!ch->fighting) && ( ch != victim ) )
      set_fighting( ch, victim );
    if ( (!victim->fighting) && ( victim != ch ) )
      set_fighting( victim, ch );

    /* Mobs do NOT auto-trip! */
    if ( number_percent( ) < chance )
    {
        WAIT_STATE( victim, ( skills_table[gsn_trip].beats * 5 / 6 ));
        act( "You trip $N&n and send $M sprawling to the &n&+yearth&n!", ch, NULL, victim, TO_CHAR    );
	act( "$n&n trips you and you go down!", ch, NULL, victim, TO_VICT    );
        act( "$n&n trips $N&n and $E falls face-first to the &n&+yground&n!", ch, NULL, victim, TO_ROOM_VICT );
        if( victim->position > POS_RECLINING )
           victim->position = POS_RECLINING;
    }
    else
    {
	act( "You try to trip $N&n and fall down!", ch, NULL, victim, TO_CHAR    );
        act( "$n&n attempts to knock you from your feet, and falls down $sself!", ch, NULL, victim, TO_VICT    );
        act( "$n&n tries to trip $N&n and tumbles to the &n&+yground&n!", ch, NULL, victim, TO_ROOM_VICT );
        ch->position = POS_RECLINING;
    }

    return;
}

/*
 * Springleap by Veygoth
 * Usable to initiate combat and during combat
 */
void do_springleap( CHAR_DATA *ch, char *argument )
{
    int chance;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_springleap");
    /* Check player's level and class, mobs can use this skill */
    if ( (ch->level < skills_table[gsn_springleap].skill_level[ch->class] ) )
    {
	send_to_char(
            "You'd better leave the martial arts to Bruce Lee.\n\r", ch );
	return;
    }

    if( ch->race == RACE_CENTAUR )
    {
        send_to_char( "Your anatomy prevents you from springleaping.\n\r", ch );
        return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) 
	     || victim->deleted || victim->position == POS_DEAD )
	{
            send_to_char( "You don't see them, here.\n\r", ch );
	    return;
	}
    }
    else
    {
        if( !victim || victim->deleted || victim->position == POS_DEAD )
        {
	  send_to_char( "You aren't fighting anyone.\n\r", ch );
	  return;
        }
    }

    /* Govno springleap self */
    if ( ch == victim )
    {
        send_to_char( "You can't quite figure out how to do that.\n\r", ch );
        return;
    }

    /* Check size of ch vs. victim. */
    /* If ch is too small. */
    if ( ch->size - 2 > victim->size  )
    {
        act( "Your acrobatic maneuver cannot accurately leap into such a small being.", ch, NULL, victim, TO_CHAR );
	return;
    }
    /* Ch 2 or more sizes larger than victim => bad! */
    if ( ch->size + 2 < victim->size )
    {
        act( "Your acrobatic maneuver does not seem to work on someone so large.", ch, NULL, victim, TO_CHAR );
        act( "$n&n jumps into you, and slides down your leg.", ch, NULL,victim, TO_VICT );
        act( "$n&n jumps into $N&n and slides down $S leg.", ch, NULL, victim, TO_ROOM_VICT );

        WAIT_STATE( ch, skills_table[gsn_springleap].beats );
        ch->position = POS_RECLINING;
        if ( victim->fighting == NULL )
            set_fighting( victim, ch );
	return;
    }

    WAIT_STATE( ch, number_fuzzy(skills_table[gsn_springleap].beats) );
    skill_practice( ch, gsn_springleap );


    if( IS_NPC( ch ))
        chance = (ch->level * 3) / 2 + 15;
    else
        chance = ch->pcdata->skl_lrn[gsn_springleap] - 5;

    if( chance > 95 )
        chance = 95;

    if( victim->position < POS_FIGHTING )
      chance /= 4;

    if ( number_percent( ) < chance )
    {
        ch->position = POS_FIGHTING;
        act("&+WYour springleap knocks $N&n&+W on $S butt.&n", ch, NULL, victim, TO_CHAR);
        act("&+W$n&n&+W leaps gracefully at $N&n&+W, sending $M to the ground.&n", ch, NULL, victim, TO_ROOM);
        act("&+W$n&n&+W leaps at you, knocking you to the ground!&n", ch, NULL, victim, TO_VICT);
        if( is_affected( victim, AFF_SINGING ))
        {
            REMOVE_AFF_BIT( victim, AFF_SINGING );
            act( "$n&n gasps and falls silent as $e falls over backward!", victim, NULL, NULL, TO_ROOM );
            send_to_char( "You abort your singing!\n\r", victim );
        }
        if( is_affected( victim, AFF_CASTING ))
        {
            REMOVE_AFF_BIT( victim, AFF_CASTING );
            act( "$n&n no longer has any idea what $e was casting.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Lying on the ground, you realize you have no idea what you were just casting.\n\r", victim );
        }

        if (!ch->fighting)
            set_fighting( ch, victim );
        if(!victim->fighting)
            set_fighting( victim, ch );
        if( !check_tumble( victim ))
        {
          WAIT_STATE( victim, ( skills_table[gsn_springleap].beats * 5 / 6 ));
          if( victim->position > POS_SITTING )
            victim->position = POS_SITTING;
          damage( ch, victim, number_range( 1, ch->level ), gsn_springleap, WEAR_NONE, DAM_BASH );
        }
        else
        {
          damage( ch, victim, number_range( 1, (ch->level / 3) ), gsn_springleap, WEAR_NONE, DAM_BASH );
          send_to_char( "You roll with the blow, finally landing on your feet.\n\r", ch );
          act( "$n&n rolls with the blow, finally landing on $s feet.", victim, NULL, NULL, TO_ROOM );
        }
     }
    else
    {
        bool pissed_off = FALSE;
        if (ch->fighting == victim || get_curr_int( victim ) > number_percent() )
            pissed_off = TRUE;
        if ( pissed_off ) {
            act( "As $N&n avoids your leap you crash to the ground.", ch, NULL, victim, TO_CHAR );
            act( "$n&n crashes to the ground as you avoid $s springleap.", ch, NULL, victim, TO_VICT );
            if (!ch->fighting) 
                set_fighting( ch, victim );
            if(!victim->fighting) 
                set_fighting( victim, ch );
        }
        else {
            act("You ungracefully leap at $N and miss, landing head first!", ch, NULL, victim, TO_CHAR);
        }
        act( "$n&n misses a springleap and falls awkwardly to the ground.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_RECLINING;
        damage( ch, ch, number_range( 1, 4 ), gsn_springleap, 
               WEAR_NONE,  DAM_BASH );
    }

    return;
}

/*
 * Bodyslam by Veygoth
 * Usable to initiate combat
 */
void do_bodyslam( CHAR_DATA *ch, char *argument )
{
    int       chance;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_bodyslam");

    /* Check player's level and class, mobs can use this skill */
    if ( !IS_SET( race_table[ch->race].race_abilities, RACE_BODYSLAM ) )
    {
	send_to_char(
            "You don't feel massive enough to manhandle that.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;
    if ( ch->riding ) {
        send_to_char("You can't do that while mounted.\n\r", ch);
        return;
    }
    one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) )
	     || victim->deleted || victim->position == POS_DEAD )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }
    else
    {
        send_to_char( "Bodyslam who?\n\r", ch );
        return;
    }

    // Added size restrictions -- Veygoth
    if( victim->size > ch->size )
    {
        if( ch->race == RACE_CENTAUR && victim->race == RACE_OGRE )
        {
          // allowing centaurs to slam one size up if it's an ogre
          if( victim->size > (ch->size + 1) )
          {
            send_to_char( "You can't bodyslam something that much bigger than you.\n\r", ch );
            return;
          }
        }
        else
        {
          send_to_char( "You can't bodyslam something bigger than you.\n\r", ch );
          return;
        }
    }

    if( (ch->size > victim->size) && ((ch->size - victim->size) > 3) )
    {
        send_to_char( "They're too small to slam.\n\r", ch );
        return;
    }

    /* Bodyslam self?  Ok! Govno */
    // Toned down the damage - Veygoth
    if ( ch == victim )
    {
	send_to_char( "You slam yourself to the ground!\n\r", ch );
        act( "$n&n throws $mself to the ground in a fit of clumsiness.", 
              ch, NULL, victim, TO_ROOM_VICT );
        WAIT_STATE( ch, (skills_table[gsn_bodyslam].beats / 2) );
        ch->position = POS_RECLINING;
	damage( ch, ch, number_range( 1, 6 ), gsn_bodyslam,
	       WEAR_NONE, DAM_BASH );
	return;
    }
        
    WAIT_STATE( ch, skills_table[gsn_bodyslam].beats );
    skill_practice( ch, gsn_bodyslam );

    chance = (ch->level * 3) / 2 + 15;
    chance += get_curr_agi( ch ) - get_curr_agi( victim );
    chance -= (victim->level - ch->level );

    switch( victim->position )
    {
        case POS_DEAD:
            return;
        case POS_MORTAL:
            chance += 15; break;
        case POS_INCAP:
            chance += 10; break;
        case POS_UNCONSCIOUS:
            chance +=  5; break;
        case POS_STUNNED:
            chance +=  3; break;
        case POS_SLEEPING:
            chance +=  2; break;
        case POS_RECLINING:
            chance -= 45; break;
        case POS_RESTING:
            chance -= 30; break;
        case POS_SITTING:
            chance -= 20; break;
        case POS_KNEELING:
            chance -= 15; break;
        case POS_FIGHTING:
        case POS_STANDING:
        default:
            break;
    }

    // Small penalty for the small buggers -- Veygoth
    if( victim->size < ( ch->size - 1 ))
    {
      chance -= 15;
    }

    if( chance > 90 )
        chance = 90;

    // Shaman bodyslam penalty - Veygoth
    if( ch->class == CLASS_SHAMAN || ch->class == CLASS_DRUID )
        chance = (chance * 2) / 3;

    if( is_affected( victim, AFF_AWARE ))
    {
        chance -= 15;
    }
    else if( is_affected( victim, AFF_SKL_AWARE ))
    {
      if( ch->level >= skills_table[gsn_aware].skill_level[ch->class] )
      {
        if( IS_NPC( ch ))
        {
          if( number_percent() < ((ch->level * 3) / 2 + 15))
          {
             chance -= 15;
          }
        }
        else if( number_percent() < ch->pcdata->skl_lrn[gsn_aware] )
        {
          skill_practice( ch, gsn_aware );
          chance -= 15;
        }
        else
        {
          skill_practice( ch, gsn_aware );
        }
      }
    }

    if (!ch->fighting)
      set_fighting( ch, victim );
    if(!victim->fighting)
      set_fighting( victim, ch );

    if ( IS_NPC( ch ) || number_percent( ) < chance )
    {
        if( is_affected( victim, AFF_SINGING ))
        {
            REMOVE_AFF_BIT( victim, AFF_SINGING );
            act( "$n&n suddenly loses track of the key $e was singing in.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "You get the wind knocked out of you!\n\r", victim );
        }
        if( is_affected( victim, AFF_CASTING ))
        {
            REMOVE_AFF_BIT( victim, AFF_CASTING );
            act( "$n&n's thoughts of casting are scattered about as $e is slammed into the ground.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Your brain slamming against your skull disrupts your concentration.\n\r", victim );
        }
        // Veygoth - moved damage to bottom because it would crash when a person died, because
        // it still tried to access them as a valid character.  Also added tumble check for
        // thieves.
        if( !check_tumble( victim ))
        {
          WAIT_STATE( victim, skills_table[gsn_bodyslam].beats );
          victim->position = POS_RECLINING;
  	  damage( ch, victim, number_range( 1, ch->level ), gsn_bodyslam, WEAR_NONE, DAM_BASH );
        }
        else
        {
  	  damage( ch, victim, number_range( 1, (ch->level / 3) ), gsn_bodyslam, WEAR_NONE, DAM_BASH );
          send_to_char( "You roll with the blow, finally landing on your feet.\n\r", ch );
          act( "$n&n rolls with the blow, finally landing on $s feet.", ch, NULL, NULL, TO_ROOM );
        }
     }
    else
    {
        ch->hit -= number_range( 1, 5 );
        act( "As $N&n avoids your slam, you smack headfirst into the &n&+yground&n.", ch, NULL, victim, TO_CHAR );
        act( "$n&n throws $mself to the &n&+yground&n in a fit of clumsiness.", ch, NULL, victim, TO_VICT );
        act( "$n&n misses a bodyslam on $N&n and slams $s head into the &n&+yground&n.", ch, NULL, victim, TO_ROOM_VICT );
        ch->position = POS_RECLINING;
    }

    return;
}

/*
 * Headbutt by Veygoth
 * Usable to initiate combat and during combat
 *
 * Modified by Lithos to up the damage and allow for a 
 * chance to stun victim or self
 *   damage = (level) d2, for an average of 75 hp at level 50
 *   stun damage = (level) d3, for an average of 100 hp at level 50
 * Player vs player damage is reduced in damage()
 */
void do_headbutt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int        dam;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance;
    int        ko;
    char        buf[MAX_STRING_LENGTH];

    last_fun( "do_headbutt");
    /* Check player's level and class, mobs can use this skill */
    if ( (ch->level < skills_table[gsn_headbutt].skill_level[ch->class] ) )
    {
        send_to_char( "You'd best leave the headbutting to the mercenaries.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) )
	     || victim->deleted || victim->position == POS_DEAD )
	{
            send_to_char( "They are nowhere to be seen.\n\r", ch );
	    return;
	}
    }
    else
    {
        if( !victim || victim->deleted || victim->position == POS_DEAD )
        {
	  send_to_char( "You aren't fighting anyone.\n\r", ch );
	  return;
        }
    }

    /* Govno anti headbutt me code */
    if ( ch == victim )
    {
	send_to_char( 
           "You get dizzy as you ponder the mechanics of headbutting yourself.\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING ) {
        send_to_char( "You need to stand up to do that.\n\r", ch);
        return;
    }
    /* Check size of ch vs. victim. */
    /* If ch is too small. */
    /* Made it 2 sizes  - Lithos */
    if ( ch->size - 2 > victim->size  )
    {
        act( "You would crush such a small and delicate being with your mass.", ch, NULL, victim, TO_CHAR );
	return;
    }
    /* Ch 2 or more sizes larger than victim => bad! */
    if ( ch->size + 1 < victim->size )
    {
        act( "You can't reach their head!", ch, NULL, victim, TO_CHAR );
        act( "$n&n slams $s head into your thigh.", ch, NULL,victim, TO_VICT );
        act( "$n&n slams $s head into $N's thigh.", ch, NULL, victim, TO_ROOM_VICT );

        WAIT_STATE( ch, ( skills_table[gsn_headbutt].beats * 9 ) / 10 );
        if ( victim->fighting == NULL )
            set_fighting( victim, ch );
	return;
    }

    WAIT_STATE( ch, number_fuzzy(skills_table[gsn_headbutt].beats) );
    skill_practice( ch, gsn_headbutt );

    if (!ch->fighting)
      set_fighting( ch, victim );
    if(!victim->fighting)
      set_fighting( victim, ch );

/* Added a PC skill level 
 *
 * Lithos
 */
    // Chance was over-downgraded at some point.  Was skill level - 5%, 
    // then it was changed to skill level / 2 giving a level 50 a headbutt
    // success rate of about 47%.  I've upped it to 4/5 of skill level,
    // meaning that a level 50 has a success rate of 76%, which is a good
    // target success rate.  Keep in mind minotaur will have a success rate of
    // about 83%.
    // -- Veygoth
    if ( IS_NPC( ch ) ) 
         chance = 50 + ch->level;
    else
        chance =  ch->pcdata->skl_lrn[gsn_headbutt] * 4 / 5;

    // Minotaur headbutt bonus -- Veygoth
    if( ch->race == RACE_MINOTAUR ) 
        chance += 7;

    if ( victim->position < POS_FIGHTING )
        chance /= 3;

    if ( number_percent() < chance )
    /*  Headbutt successful, possible KO */
    {
        /* First give the victim a chance to dodge */
        if ( check_dodge( ch, victim ) )
            return;
        /* OK, lets settle for stun right now 
         * a skill level of 100% has a 20% chance of stun
         * a skill level of 50% has a 2.5% chance of stun
         * a skill level of 23% has a 1% chance of stun
         * immortals get a 15% bonus
         */
        // The stun math was bad.  Stun was way more often that it should have
        // been.  Now we have a flat /4 chance, meaning a the following stun chances
        // at each skill level:
        // 25 = 5%  50 = 10 %  75 = 15%  95 = 19%
        ko  = chance / 4;
        if (IS_IMMORTAL(ch)) ko += 15;
        sprintf(buf,"do_headbutt: %s&n attempting a KO with %d%% chance.",ch->name, ko);
        wiznet( 0, WIZ_LSPAM, 0, buf);
        if ( number_percent() < ko + 1 )
        {
            // deal some decent damage
            // This was previously level d 3 which was fairly pathetic.
            // Level d 8 = 50 min, 400 max at level 50 with an average of 225
            // PvP damage is quartered, so headbutt will do about 56 against a player.
            // - Veygoth
            if( ch->race != RACE_MINOTAUR )
              damage( ch, victim, dice( ch->level, 8 ), gsn_headbutt, WEAR_NONE, DAM_BASH );
            else
              damage( ch, victim, dice( ch->level, 9 ), gsn_headbutt, WEAR_NONE, DAM_BASH );
            if (victim->position > POS_STUNNED)
            {
                act("$n&n staggers about, then collapses in a heap.", victim, NULL, NULL, TO_ROOM);
                send_to_char("You fall to the ground &+Wstunned&n.\n\r", victim);
                act("$n&n is &+Wstunned!&n", victim, NULL, NULL, TO_ROOM);
                victim->position = POS_STUNNED;
                WAIT_STATE( victim, (skills_table[gsn_headbutt].beats ) );
                sprintf(buf,"do_headbutt: %s&n stunned %s&n.",ch->name, victim->name);
                wiznet( 0, WIZ_LSPAM, 0, buf);
            }
        }
        else
        {
            // just your ordinary headbutt damage
            // This was previously level d 2 which was fairly pathetic.
            // Level d 6 = 50 min, 300 max at level 50 with an average of 175
            // PvP damage is quartered, so headbutt will do about 43 against a player.
            // -- Veygoth
            if( ch->race != RACE_MINOTAUR )
              damage( ch, victim, dice( ch->level, 6 ), gsn_headbutt, WEAR_NONE, DAM_BASH );
            else
              damage( ch, victim, dice( ch->level, 7 ), gsn_headbutt, WEAR_NONE, DAM_BASH );
            // Someone blasts you in the head it'll definitely stun you for 3/4 of a second.
            // -- Veygoth
            WAIT_STATE( victim, 3 );
        }
    }
    else /* Headbutt failed, possible damgage to self, possible KO of self */
    {    /* Don't allow char to kill self, just mort self */
         /* Give them a chance to not take any damage */
        // Checking chance instead of player's level.  Since this should be based on skill
        // this should be about right (average of 24% chance of screwing yourself at level 50
        // instead of 50%, 17% chance for minos).
        // - Veygoth
        if (number_percent() < chance )
        {
            send_to_char("Your headbutt fails to strike its target.\n\r", ch);
            act("$n&n tries to headbutt $N&n but can't quite connect.", ch, NULL,
                victim, TO_NOTVICT);
            act("$n&n bobs around you in a feeble attempt at a headbutt.", ch,
                NULL, victim, TO_VICT);
            return;
        }
        act("You bang your head against the brick wall of $N&n.", ch, NULL,
            victim, TO_CHAR);
        act("$n&n tries a headbutt but $N&n gets the best of $m.", ch, NULL,
            victim, TO_NOTVICT);
        act("$n&n bangs into you in a feeble attempt at a headbutt.", ch,
            NULL, victim, TO_VICT);
        // KO chance of 24% for normals, 17% for minos.
        // You have to fail three checks to get your ass kicked by KO, one for the actual skill check,
        // one for the damage check and finally one for the KO check.
        // keep in mind this KO does damage of about 100 to self at level 50, which is a hell of a lot
        // for a failed skill.
        // The chance of ko'ing yourself at each skill level is as follows: (after all 3 checks)
        // Skill 25 = 59.2%  Skill 50 = 29.6%  Skill 75 = 14.4%  Skill 95 = 9.38%
        ko = 108 - chance;
        if ( number_percent() < ko )
        {
            // doh! This is gonna hurt
            //deal some decent damage...to self!
            if( ch->race != RACE_MINOTAUR )
              dam = dice(ch->level, 3);
            else
              dam = dice(ch->level, 2);
            if ( dam > ch->hit ) 
                dam = ch->hit +1;
            damage( ch, ch, dam, gsn_headbutt, WEAR_NONE, DAM_BASH );
            if (ch->position <= POS_INCAP)
                return;
            act("$n&n staggers about, then collapses in a heap.", ch, NULL,
                NULL, TO_ROOM);
            send_to_char("You fall to the ground stunned.\n\r", ch);
            act("$n&n is stunned!", ch, NULL, NULL, TO_ROOM);
            ch->position = POS_STUNNED;
            WAIT_STATE( ch, (skills_table[gsn_headbutt].beats * 2) );
                sprintf(buf,"do_headbutt: %s&n stunned self.",ch->name);
                wiznet( 0, WIZ_LSPAM, 0, buf);
        }
        else
        {
            // Was previously level d 2, which averaged 30 damage at 20 and 75 at 50.  PC to PC
            // damage is not quartered when it is done to yourself, so this it kind of high at the
            // upper levels.  This has been reduced by level / 5, so the damage at 50 averages 65
            // instead of 65.
            // Keep in mind that the real penalties come from KO and comparitively someone that
            // fails a bash doesen't take insane damage.
            // -- Veygoth
            dam = dice(ch->level, 2) - ch->level / 5;
            if ( dam > ch->hit )
                dam = ch->hit +1;
            damage( ch, ch, dam, gsn_headbutt, WEAR_NONE, DAM_BASH );
        }
    } //end if() headbutt failed
    return;
}

/*
 * Charge by Veygoth
 * Usable to initiate combat
 */
void do_charge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int   chance;

    last_fun( "do_charge");
    /* Check player's level and class, mobs can use this skill */
    if ( !IS_SET( race_table[ch->race].race_abilities, RACE_CHARGE ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) 
	     || victim->deleted || victim->position == POS_DEAD )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }
    else
    {
        send_to_char( "Charge who?\n\r", ch );
        return;
    }

    /* charge me trap Govno */
    if ( ch == victim )
    {
	send_to_char( "You chase your tail for a while.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, number_fuzzy( skills_table[gsn_charge].beats ));

    if (!ch->fighting)
      set_fighting( ch, victim );
    if(!victim->fighting)
      set_fighting( victim, ch );

    // Chance is based on level of charger and victim -- Veygoth
    chance = (ch->level * 2) - victim->level + 25;
    if( chance > 95 ) chance = 95;

    if ( IS_NPC( ch ) || number_percent( ) < chance )
	damage( ch, victim, number_range( 1, ch->level ), gsn_charge,
	       WEAR_NONE, DAM_BASH );
    else
	damage( ch, victim, 0, gsn_charge, WEAR_NONE, DAM_BASH );

    return;
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        percent;

    last_fun( "do_dirt");
    /* Don't allow the unskilled to do this, check player's level */
    if ( ch->level < skills_table[gsn_dirt].skill_level[ch->class] )
    {
        send_to_char( "You get your feet dirty.\n\r", ch );
        return;
    }

    if ( !check_blind( ch ) )
    {
        send_to_char( "You can't see anything!\n\r", ch );
        return;
    }

    if (ch->fly_level)
    {
        send_to_char("Perhaps you should land first matey.\n\r", ch);
        return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) )
	     || victim->deleted || victim->position == POS_DEAD )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }
    }
    if ( !victim || victim->deleted || victim->position == POS_DEAD )
    {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
    }


    if ( victim == ch )
    {
	send_to_char( "Very funny.\n\r", ch );
	return;
    }

    if ( is_affected( victim, AFF_BLIND ) )
    {
        act( "$E's already been &+Lblinded&n.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if( !IS_NPC( ch ))
      percent = ch->pcdata->skl_lrn[gsn_dirt];
    else
      percent = (ch->level * 3) / 2 + 25;

    percent += ( ch->level - victim->level ) * 2;

    percent += (get_curr_dex( ch ) / 10);

    percent -= (get_curr_dex( victim ) / 10);

    percent -= (get_curr_agi( victim ) / 10);

    // Why waste time listing sectors with no modifier? - Veygoth
    switch( ch->in_room->sector_type )
    {
	case SECT_INSIDE:
        case SECT_ARCTIC:
        case SECT_SWAMP:
          percent -= 20;
          break;
	case SECT_CITY:
        case SECT_MOUNTAIN:
	  percent -= 10;
          break;
        case SECT_PLANE_FIRE:
        case SECT_PLANE_AIR:
        case SECT_PLANE_WATER:
        case SECT_PLANE_ASTRAL:
        case SECT_PLANE_ETHEREAL:
        case SECT_UNDERWATER:
        case SECT_UNDERWATER_GROUND:
        case SECT_WATER_SWIM:
        case SECT_WATER_NOSWIM:
        case SECT_AIR:
        case SECT_OCEAN:
        case SECT_UNDERG_WATER:
        case SECT_UNDERG_WATER_NOSWIM:
          percent = 0;
          break;
	case SECT_FIELD:
	  percent +=  5;
          break;
	case SECT_DESERT:
          percent += 10;
          break;
        case SECT_PLANE_EARTH:
          percent += 15;
	default:
          break;
    }

    if ( percent > 75 )
	percent = 75;
    else if ( percent < 5 )
	percent = 5;

    if ( percent <= 0 )
    {
        send_to_char( "There isn't any &n&+ydirt&n to kick.\n\r", ch );
	return;
    }

    skill_practice( ch, gsn_dirt );

    if ( percent < number_percent( ) )
    {
	AFFECT_DATA af;

        act( "$n is &+Lblinded&n by the &n&+ydirt&n in $s eyes!", victim, NULL, NULL,
								    TO_ROOM );
        act( "$n kicks &n&+ydirt&n into your eyes!", ch, NULL, victim, TO_VICT );

        send_to_char( "&+LYou can't see a thing!&n\n\r", victim );

	af.skill     = gsn_dirt;
        af.spell     = 0;
        af.song      = 0;
	af.duration  = number_range( 1, 2 );
	af.location  = APPLY_HITROLL;
	af.modifier  = -4;
	set_bitvector( &af, AFF_BLIND);
	affect_to_char( victim, &af );

    }
    else
    {
        send_to_char( "&+LYou kick dirt at your target!&n\n\r", ch );
    }

    set_fighting( victim, ch );

    WAIT_STATE( ch, skills_table[gsn_dirt].beats );

    return;
}

// This function needs to be rewritten to take args!
void do_whirlwind( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *pChar;
    CHAR_DATA *pChar_next;
    OBJ_DATA  *wield;
    bool       found = FALSE;

    last_fun( "do_whirlwind");
    if ( !IS_NPC( ch ) 
	&& ch->level < skills_table[gsn_whirlwind].skill_level[ch->class] )
    {
	send_to_char( "You don't know how to do that...\n\r", ch );
	return;
    }

    if ( !( wield = get_eq_char( ch, WEAR_HAND ) )
         || wield->item_type != TYPE_WEAPON )
    {
        send_to_char( "You need to wield a weapon first.\n\r", ch );
	return;
    }

    act( "$n&n holds $p&n firmly, and starts spinning round...", ch, wield, NULL,
	TO_ROOM );
    act( "You hold $p&n firmly, and start spinning round...",  ch, wield, NULL,
	TO_CHAR );

    pChar_next = NULL;   
    for ( pChar = ch->in_room->people; pChar; pChar = pChar_next )
    {
	pChar_next = pChar->next_in_room;

	if( ( IS_NPC( pChar ) || ( race_table[pChar->race].racewar_side !=
            race_table[ch->race].racewar_side && !IS_IMMORTAL( pChar ) ) )
            && can_see( pChar, ch ) )
	{
	    found = TRUE;
	    act( "$n&n turns towards YOU!", ch, NULL, pChar, TO_VICT );
	    one_hit( ch, pChar, gsn_whirlwind, WEAR_HAND );
            // Added small amount of lag per target hit
            WAIT_STATE( ch, 3 );
	}
    }

    if ( !found )
    {
	act( "$n&n looks dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
	    TO_ROOM );
	act( "You feel dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
	    TO_CHAR );
    }

    WAIT_STATE( ch, skills_table[gsn_whirlwind].beats );

    skill_practice( ch, gsn_whirlwind );

    if ( !found
	&& number_percent( ) < 25 )
    {
        act( "$n&n loses $s balance and colapses into a heap.",  ch, NULL, NULL,
	    TO_ROOM );
	act( "You lose your balance and fall into a heap.", ch, NULL, NULL,
	    TO_CHAR );
	ch->position = POS_STUNNED;
    }

   return;
}      


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        odds;

    last_fun( "do_disarm");
    /* Don't allow charmed mobiles to do this, check player's level */
    if ( ( IS_NPC( ch ) && is_affected( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skills_table[gsn_disarm].skill_level[ch->class] ) )
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if (   !get_eq_char( ch, WEAR_HAND   )
	&& !get_eq_char( ch, WEAR_HAND_2 ) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

    if ( victim->fighting != ch && ch->fighting != victim )
    {
	act( "$E is not fighting you!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (   !get_eq_char( victim, WEAR_HAND   )
	&& !get_eq_char( victim, WEAR_HAND_2 ) )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    if( victim->level > ch->level + 10 )
    {
        send_to_char( "They are much too clever for such a clumsy attempt at that maneuver.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skills_table[gsn_disarm].beats );
    skill_practice( ch, gsn_disarm );
    if ( IS_NPC( ch ) )
        odds = ch->level;
    else 
        odds = ch->pcdata->skl_lrn[gsn_disarm] / 2;
    if ( IS_NPC( victim ) )
        odds += 2 * (ch->level - victim->level);
    else
        /* Offense skill helps prevent disarms */
        odds -= victim->pcdata->skl_lrn[gsn_enhanced_damage] / 4;
    if ( !get_eq_char( ch, WEAR_HAND ) )
	odds /= 2;		/* 1/2 as likely with only 2nd weapon */
    odds = UMIN( odds, 98 );
    odds = UMAX( odds, 2 );
    sprintf( lbuf, "Disarm: %s attempting with %d%% chance.", ch->name, odds );
    wiznet( 0, WIZ_LSPAM, 0, lbuf );
    if ( number_percent() < odds )
	disarm( ch, victim );
    else if( number_percent() > 80 )
    {
        send_to_char( "Expertly countering your maneuver, they dislodge your weapon and send it flying.\n\r", ch );
        disarm( victim, ch );
    }
    else
        send_to_char( "You failed in your attempt.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    last_fun( "do_sla");
    rch = get_char( ch );

    if ( !authorized( rch, "slay" ) )
        return;

    send_to_char( "If you want to &+LSLAY&n, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_slay");
    rch = get_char( ch );

    if ( !authorized( rch, "slay" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
        send_to_char( "They aren't even here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
        send_to_char( "Suicide is a mortal sin.\n\rYou will burn!\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "immolate" ) )
    {
      act( "Your &n&+rfi&+Rr&n&+re&+Rba&n&+rll&n turns $N into a blazing &n&+rinf&+Rer&+Yn&+Wo&n.",
						ch, NULL, victim, TO_CHAR    );
      act( "$n releases a searing &n&+rfi&+Rr&n&+re&+Rba&n&+rll&n in your direction.",
						ch, NULL, victim, TO_VICT    );
      act( "$n points at $N, who bursts into a flaming &n&+rinf&+Rer&+Yn&+Wo&n.",
						ch, NULL, victim, TO_ROOM_VICT );
    }
    else if ( !str_prefix( arg2, "pounce" ) && get_trust( ch ) >= L_DIR )
    {
      act( "Leaping upon $N with bared &+Wfangs&n, you tear open $S throat and toss the &+Lcorpse&n to the &n&+yground&n...",
						ch, NULL, victim, TO_CHAR    );
      act( "In a heartbeat, $n rips $s &+Wfangs&n through your throat!  Your &+Rbl&n&+ro&+Ro&n&+rd&n sprays and pours to the &n&+yground&n as your life ends...",
						ch, NULL, victim, TO_VICT    );
      act( "Leaping suddenly, $n sinks $s &+Wfangs&n into $N's throat.  As &+Rbl&n&+ro&+Ro&n&+rd&n sprays and gushes to the &n&+yground&n, $n tosses $N's dying body away.",
						ch, NULL, victim, TO_ROOM_VICT );
    }
    else if ( !str_prefix( arg2, "shatter" ) )
    {
      act( "You freeze $N with a glance and shatter the frozen &+Lcorpse&n into tiny &+Wsh&+Ca&+Wr&+Cds&n.",
						ch, NULL, victim, TO_CHAR    );
      act( "$n freezes you with a glance and shatters your frozen body into tiny &+Wsh&+Ca&+Wr&+Cds&n.",
						ch, NULL, victim, TO_VICT    );
      act( "$n freezes $N with a simple look and shatters the frozen body into tiny &+Wsh&+Ca&+Wr&+Cds&n.",
						ch, NULL, victim, TO_ROOM_VICT );
    } 
    else if ( !str_prefix( arg2, "slit" ) && get_trust( ch ) >= L_DIR )
    {
      act( "You calmly &+Lslit&n $N's throat.",
						ch, NULL, victim, TO_CHAR    );
      act( "$n reaches out with a clawwed finger and calmly &+Lslits&n your throat.",
						ch, NULL, victim, TO_VICT    );
      act( "$n calmly &+Lslits&n $N's throat.",
						ch, NULL, victim, TO_ROOM_VICT );
    }
    else if ( !str_prefix( arg2, "squeeze" ) )
    {
        act( "You grasp $S head and squeeze it until it explodes in a bubble of &+Rbl&n&+ro&+Ro&n&+r&n!",
						ch, NULL, victim, TO_CHAR    );
        act( "$n grasps your head and squeezes until the &+Wskull&n colapses!",
						ch, NULL, victim, TO_VICT    );
	act( "$n grasps $N's head and squeezes until it explodes!",
						ch, NULL, victim, TO_ROOM_VICT );
    }
    else
    {
        act( "You &+Lslay&n $M in cold &+Rbl&n&+ro&+Ro&n&+rd&n!",  ch, NULL, victim, TO_CHAR    );
        act( "$n &+Lslays&n you in cold &+Rbl&n&+ro&+Ro&n&+rd&n!", ch, NULL, victim, TO_VICT    );
        act( "$n &+Lslays&n $N in cold &+Rbl&n&+ro&+Ro&n&+rd&n!",  ch, NULL, victim, TO_ROOM_VICT );
    }

    raw_kill( ch, victim );
    last_fun( "do_slay");
    return;
}

/* This code is for PC's who polymorph into dragons.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Breathe on friend and enemy alike.
 * -Kahn
 */

void pc_breathe( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    last_fun( "pc_breathe");

    if( IS_NPC( ch ) )
    {
        bug( "pc_breathe: ch is a NPC instead of a PC!", 0 );
        return;
    }

    send_to_char( "You feel the urge to &+Gburp&n!\n\r", ch );
    act( "$n&n breathes!", ch, NULL, NULL, TO_ROOM );

    switch( number_range( 1, 7 ) )
    {
        default:
        case 1:
            sn = spl_fire_breath;
        break;
        case 2:
            sn = spl_acid_breath;
        break;
        case 3:
            sn = spl_lightning_breath;
        break;
        case 4:
            sn = spl_frost_breath;
        break;
        case 5:
            sn = spl_gas_breath;
        break;
        case 6:
            sn = spl_water_breath;
        break;
        case 7:
            /* THis needs to be fixed when shadow breath is addeD! */
            sn = spl_fire_breath;
        break;
    }

    if( sn > 0 && sn < MAX_SPELL )
    {
        sprintf(lbuf,"Dragon (%s) breathing %s.", ch->name,
            spells_table[sn].name );
        wiznet( 0, WIZ_LSPAM, 0, lbuf);
    }

    if( sn > 0 && sn < MAX_SPELL )
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

	/* NPC's do not hit NPC's! (charmies?) */
	if ( IS_NPC( victim ) && IS_NPC( ch ) )
	    continue;

        (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    }

    return;
}

/* This code is for PC's who polymorph into harpies.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Scream into the ears of enemy and friend alike.
 * -Kahn
 */

void pc_screech( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    last_fun( "pc_screech");
    send_to_char( "You feel the urge to scream!\n\r", ch );
    interpret( ch, "scream" );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;
	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

        act( "Your ears begin to &n&+rbl&+Re&n&+re&+Rd&n from $n's scream.  Ouch!", ch, NULL, victim,
	    TO_VICT );
	sn = spl_agitation;
	(*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    }

    return;
}

void pc_spit( CHAR_DATA *ch )
{
    CHAR_DATA *victim =  ch->fighting;

    last_fun( "pc_spit");
    if( !victim || victim->deleted )
        return;

    send_to_char( "You feel the urge to bite!\n\r", ch );
    act( "$n&n bites $N&n!", ch, NULL, victim, TO_NOTVICT );
    act( "You are bitten by $n&n!", ch, NULL, victim, TO_VICT );
//	(*spells_table[spl_poison].spell_fun) ( spl_poison, ch->level, ch, victim );
        inflict_poison( gsn_poison_bite, ch->level, 0, ch, victim );
	damage( ch, victim, number_range( 1, ch->level ),
	       gsn_poison_bite, WEAR_NONE, DAM_POISON );

    return;
}


bool check_race_special( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    if ( !str_cmp( race_table[ch->race].name, "Earth Elemental" ) )
    {
     	one_hit( ch, victim, dt, WEAR_HAND );
	if ( number_percent( ) < 9 )
	{
          spell_dust_blast( spl_dust_blast, ch->level, ch, victim );
	}
        else if( number_percent( ) < 6 )
        {
          spell_pebble( spl_pebble, ch->level, ch, victim );
        }
        else if( number_percent( ) < 3 )
        {
          spell_dirt_cloud( spl_dirt_cloud, ch->level, ch, victim );
        }
        return TRUE;
    }

    if ( !str_cmp( race_table[ch->race].name, "Fire Elemental" ) )
    {
     	one_hit( ch, victim, dt, WEAR_HAND );
	if ( number_percent( ) < 9 )
	{
          spell_fire_bolt( spl_fire_bolt, ch->level, ch, victim );
	}
        else if( number_percent( ) < 6 )
        {
          spell_burning_hands( spl_burning_hands, ch->level, ch, victim );
        }
        else if( number_percent( ) < 3 )
        {
          spell_spark( spl_spark, ch->level, ch, victim );
        }
        return TRUE;
    }

    if ( !str_cmp( race_table[ch->race].name, "Air Elemental" ) )
    {
     	one_hit( ch, victim, dt, WEAR_HAND );
	if ( number_percent( ) < 9 )
	{
          spell_ice_bolt( spl_ice_bolt, ch->level, ch, victim );
	}
        else if( number_percent( ) < 6 )
        {
          spell_cutting_breeze( spl_cutting_breeze, ch->level, ch, victim );
        }
        else if( number_percent( ) < 3 )
        {
          spell_cutting_breeze( spl_cutting_breeze, ch->level, ch, victim );
        }
        return TRUE;
    }

    if ( !str_cmp( race_table[ch->race].name, "Water Elemental" ) )
    {
     	one_hit( ch, victim, dt, WEAR_HAND );
	if ( number_percent( ) < 9 )
	{
          spell_water_bolt( spl_water_bolt, ch->level, ch, victim );
	}
        else if( number_percent( ) < 6 )
        {
          spell_chill_touch( spl_chill_touch, ch->level, ch, victim );
        }
        else if( number_percent( ) < 3 )
        {
          spell_water_blast( spl_water_blast, ch->level, ch, victim );
        }
        return TRUE;
    }

    if ( !str_cmp( race_table[ch->race].name, "Dragon" ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    return TRUE;
	}
    }

    if ( !str_cmp( race_table[ch->race].name, "Harpy" ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    pc_screech( ch );
	    return TRUE;
	}
    }

    if (   !str_cmp( race_table[ch->race].name, "Arachnid" )
	|| !str_cmp( race_table[ch->race].name, "Snake"    ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    pc_spit( ch );
	    return TRUE;
	}
    }

    return FALSE;
}

/* 
 * Mobs using magical items by Spitfire from merc mailing list
 * Modified to give every magical item an equal chance of being used plus
 * eating pills -Kahn
 */
void use_magical_item( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *cobj     = NULL;
    int       number   = 0;
    int       i        = 0;
    int       sn       = 0;
    char      buf[ MAX_INPUT_LENGTH ];

    last_fun( "use_magical_item");
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( (   obj->item_type == TYPE_SCROLL
	      || obj->item_type == TYPE_WAND
	      || obj->item_type == TYPE_STAFF
	      || obj->item_type == TYPE_PILL )
	    && number_range( 0, number ) == 0 )
	{
	    cobj = obj;
	    number++;
	}
    }

    if ( !cobj )
        return;

   /*
    * Modified so mobs don't cause damage to themselves and
    * don't aid players to help stop HEAVY player cheating -Zen
    */
    switch( cobj->item_type )
    {
    case TYPE_SCROLL:	for ( i = 1; i < 5; i++ )
			{
			    sn = cobj->value[i];

			    if (   sn <= 0
				|| sn >= MAX_SPELL )
			    {
				extract_obj( cobj );
				return;
			    }

			    if ( spells_table[sn].target == TAR_CHAR_DEFENSIVE )
			    {
				act( "$n discards a $p.", ch, cobj, NULL, TO_ROOM );
				extract_obj( cobj );
				return;
			    }
			}
                 	break;
    case TYPE_POTION:
    case TYPE_PILL:	for ( i = 1; i < 5; i++ )
			{
			    sn = cobj->value[i];

			    if (   sn <= 0
				|| sn >= MAX_SPELL )
			    {
				extract_obj( cobj );
				return;
			    }

			    if ( spells_table[sn].target == TAR_CHAR_OFFENSIVE )
			    {
				act( "$n discards a $p.", ch, cobj, NULL, TO_ROOM );
				extract_obj( cobj );
				return;
			    }
			}
                 	break;
    }

    switch( cobj->item_type )
    {
        case TYPE_SCROLL: do_recite( ch, "scroll" );
	                  break;
	case TYPE_WAND:   if ( cobj->wear_loc == WEAR_HAND )
	                      do_zap( ch, "" );
	                  break;
	case TYPE_STAFF:  if ( cobj->wear_loc == WEAR_HAND )
			      do_brandish( ch, "" );
	                  break;
	case TYPE_POTION: do_quaff( ch, "potion" );
	                  break;
	case TYPE_PILL:   sprintf( buf, "%s", cobj->name );
	                  do_eat( ch, buf );
                 	  break;
    }
    return;

}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 */    
bool hit_suck_disarm( CHAR_DATA *ch, CHAR_DATA *victim, int hit, int dam )
{
    OBJ_DATA *obj;

    if ( number_percent( ) <= ch->level )
    {
	if ( ( ch->size	- victim->size ) < -2 )
        return FALSE;

	if ( !( obj = get_eq_char( victim, WEAR_HAND ) ) )
	    if ( !( obj = get_eq_char( victim, WEAR_HAND_2 ) ) )
		return FALSE;

	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
	    return FALSE;

	if ( !get_eq_char( ch, WEAR_HAND   )
	    && !get_eq_char( ch, WEAR_HAND_2 )
	    && number_bits( 1 ) == 0 )
	    return FALSE;

        act( "You suck $N's weapon right out of $S hand!",
	    ch, NULL, victim, TO_CHAR );
        act( "$n sucks your weapon right out of your hand!",
	    ch, NULL, victim, TO_VICT );
        act( "$n sucks $N's weapon right out of $S hand!",
	    ch, NULL, victim, TO_ROOM_VICT );

	obj_from_char( obj );
	obj_to_char( obj, ch );

        return TRUE;
    }

    return FALSE;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 */    
bool hit_vorpal( CHAR_DATA *ch, CHAR_DATA *victim, int hit, int dam )
{
    OBJ_DATA *obj;
    char     *name;
    char      buf [MAX_STRING_LENGTH];

    last_fun( "hit_vorpal");
    dam *= 100;
    if ( victim->hit > 0 )
	dam /= victim->hit;

    if ( hit >= 17 && dam >= 40 )		/* dam >= 40 is MUTILATES */
    /*
     * Cut victim's head off, generate a corpse, do not
     * give experience for kill.  Needless to say,
     * it would be bad to be a target of this weapon!
     */
    {
        act( "You slice $N's head clean off!",  ch, 0, victim, TO_CHAR );
        act( "$n slices your head clean off!",  ch, 0, victim, TO_VICT );
        act( "$n slices $N's head clean off!",  ch, 0, victim, TO_ROOM_VICT );
        act( "$n's severed head bounces across the &n&+yground&n.", victim, 0, 0, TO_ROOM );
        stop_fighting( victim, TRUE );

	name		= IS_NPC( victim ) ? victim->short_descr : victim->name;
	obj		= create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	if ( is_affected( victim, AFF_POISON ) )
	    obj->value[3] = 1;

        obj_to_room( obj, ch->in_room );
	raw_kill( ch, victim );
        last_fun( "hit_vorpal");

        return TRUE;
    }

    return FALSE;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 * Heavily modified by Zen.
 */    
void do_shoot( CHAR_DATA *ch, char *argument )
{
    extern char *    const  dir_name	[ ];
    extern char *    const  dir_rev	[ ];

    OBJ_DATA		*obj;
    OBJ_DATA		*newobj;
    OBJ_INDEX_DATA	*pObjIndex;
    EXIT_DATA		*pexit;
    ROOM_INDEX_DATA	*to_room;
    CHAR_DATA		*mob;
    char		*msg;
    char		 arg1 [ MAX_INPUT_LENGTH ]; // Target.
    char		 arg2 [ MAX_INPUT_LENGTH ]; // Direction.
    char		 arg3 [ MAX_INPUT_LENGTH ]; // Distance.
    char		 buf  [ MAX_STRING_LENGTH ];
    int			 range = 3;
    int			 dir;
    int			 n;
    bool		 in_battle = FALSE;
//    bool		 grounded = FALSE;

    last_fun( "do_shoot");

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
       return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    /* Target specified => arg2 has direction. */
    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
	if ( dir == MAX_DIR )
	{
	    send_to_char( "Invalid direction.\n\r", ch );
	    return;
	}
	/* If a target was specified as arg1. */
	if ( !str_prefix( arg2, dir_name[dir] ) )
	    break;
    }
    if ( dir == MAX_DIR )
	dir = 0;
    /* If arg2 wasn't a direction, then check if arg1 is. */
    if ( str_prefix( arg2, dir_name[dir] ) )
    for ( dir = 0; dir <= MAX_DIR; dir++ )
    {
	if ( dir == MAX_DIR )
	{
	    send_to_char( "Invalid direction.\n\r", ch );
	    return;
	}
	/* If arg1 is the direction, no target specified. */
	if ( !str_prefix( arg1, dir_name[dir] ) )
	{
	    if ( arg2[0] != 0 )			// If there is a distance,
	    	strcpy(arg3, arg2);  	// Copy distance into arg3.
	    strcpy(arg2, arg1);	 	// Copy direction into arg2.
	    arg1[0] = '\0';	 	// Set target to NULL;
	    break;
	}
    }

    if ( arg2[0] == '\0' || dir < 0 )
    {
         send_to_char( "&+LSyntax:  &+RShoot &+L<&n&+rDirection&+L> [&n&+rRange&+L]&n\n\r", ch );
         return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_HAND_2 ) ) )
    {
	send_to_char( "You aren't using a missile weapon.\n\r", ch );
	return;
    }

    if ( obj->item_type != TYPE_RANGED_WEAPON
	|| obj->value[1] < 0 || obj->value[2] < 0 )
    {
         send_to_char( "You can't do that without a ranged weapon.\n\r", ch);
         return;
    }

    if ( obj->value[4] == 0 )
    {
         send_to_char( "Its payload is empty.\n\r", ch);
         return;
    }

    if ( obj->value[3] == RNG_CATAPULT )
    {
	if ( arg3[0] != '\0' )
	    range = atoi( arg3 );

	if ( range <= 0 )
	{
	    send_to_char( "Try shooting it away from you.\n\r", ch );
	    return;
	}

	if ( range > obj->value[0] )
	{
	    if ( obj->value[0] > 0 )	range = obj->value[0];
	    else			range = 1;
	}
    }

//    while ( obj->value[4] > 0 )
//    {
    switch ( obj->value[3] )
    {
    default:            msg = "You pull back the &n&+ystring&n on %s&n and fire %s&n!\n\r";
									break;
    case RNG_BOW:       msg = "You pull back the &n&+ystring&n on %s&n and fire %s&n!\n\r";
									break;
    case RNG_CROSSBOW:	msg = "You fire %s&n bolt %s&n!\n\r";
									break;
    case RNG_CATAPULT:	msg = "You crank back %s&n, and release it %sward!\n\r";
									break;
    }
    sprintf( buf, msg, obj->short_descr, dir_name[dir] );
    send_to_char( buf, ch );

    switch ( obj->value[3] )
    {
    default:            msg = "%s&n pulls back the &n&+ystring&n on %s&n and fire %s&n!";
									break;
    case RNG_BOW:       msg = "%s&n pulls back the &n&+ystring&n on %s&n and fires %s&n!";
									break;
    case RNG_CROSSBOW:	msg = "%s&n fires %s bolt %s!";
									break;
    case RNG_CATAPULT:	msg = "%s&n cranks back %s, and releases it %sward!";
									break;
    }
    sprintf( buf, msg, ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
	    obj->short_descr, dir_name[dir] );
    act( buf, ch, NULL, NULL, TO_ROOM );

    if ( !(pObjIndex = get_obj_index( obj->value[5] ) ) )
    {
         bug( "Ranged weapon (vnum %d) has invalid ammunition.", 0 );
         return;
    }

    if ( !ch->in_room->exit[dir]
	|| IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
	newobj = create_object( get_obj_index( obj->value[5] ), 0 );
	if ( !newobj )
	    bug( "do_shoot: Missile not created! (1)", 0);
	obj->value[4]--;

	switch ( obj->value[3] )
	{
        default:           msg = "%s&n impales itself into the &n&+yground&n.";  break;
        case RNG_BOW:      msg = "%s&n impales itself into the &n&+yground&n.";  break;
        case RNG_CROSSBOW: msg = "%s&n falls to the &n&+yground&n.";             break;
        case RNG_CATAPULT: msg = "%s&n crashes to the &n&+yground&n.";           break;
	}

	sprintf( buf, msg, newobj->short_descr );

        act( capitalize( buf ), ch, NULL, NULL, TO_ROOM );
        act( capitalize( buf ), ch, NULL, NULL, TO_CHAR );
        obj_to_room( newobj, ch->in_room );
//    grounded = TRUE;
return;
    }
//        continue;
//    }

//    if ( !grounded )
//    while ( obj->value[4] > 0 )
//    {
    newobj = create_object( pObjIndex, 0 );
    if ( !newobj )
	bug( "do_shoot: Missile not created! (2)", 0);
    obj->value[4]--;

    for ( n = 1, pexit = ch->in_room->exit[dir]; pexit && n <= (range+1); n++ )
    {
	if ( IS_SET( pexit->exit_info, EX_CLOSED )
	    || IS_SET( pexit->exit_info, EX_JAMMED ) )
	{
	    switch ( obj->value[3] )
	    {
            default:           msg = "%s&n impales itself into the &n&+yground&n.";
									break;
            case RNG_BOW:      msg = "%s&n impales itself into the &n&+yground&n.";
									break;
            case RNG_CROSSBOW: msg = "%s&n falls to the &n&+yground&n.";
									break;
            case RNG_CATAPULT: msg = "%s&n crashes to the &n&+yground&n.";
									break;
	    }

	    sprintf( buf, msg, newobj->short_descr );

	    if ( pexit->to_room
		&& pexit->to_room->people )
	    {
		act( buf, pexit->to_room->people, NULL, NULL, TO_ROOM );
		act( buf, pexit->to_room->people, NULL, NULL, TO_CHAR );
	    }

	    obj_to_room( newobj, pexit->to_room );
	    act( buf, ch, NULL, NULL, TO_ROOM );
	    continue;
	}

	to_room = pexit->to_room;

	if ( to_room->people )
	{
	    for ( mob = to_room->people; mob; mob = mob->next_in_room )
	    {

		if ( mob->deleted )
		    continue;

	    if ( ( mob->position == POS_STANDING 
		|| mob->position == POS_FIGHTING )
		&& ( check_dodge( ch, mob ) || ch->level > number_percent( ) ) )
	    {
		int dam;

		sprintf( buf, "%s&n streaks into %s&n from %s&n!",
			newobj->short_descr,
			( IS_NPC( mob ) ? mob->short_descr : mob->name ),
			dir_rev[dir] );
		act( buf, mob, NULL, NULL, TO_ROOM );
		n = range;

		sprintf( buf, "%s&n streaks into you from %s&n!",
			newobj->short_descr, dir_rev[dir] );
		act( buf, mob, NULL, NULL, TO_CHAR );

		dam = number_range( obj->value[1], obj->value[2] );
		dam += newobj->value[1];

		/*
		 * Weapon Proficiencies.
		 */
		if ( !IS_NPC( ch ) && ch->pcdata->skl_lrn[gsn_shot] > 0 )
		    dam += dam * ch->pcdata->skl_lrn[gsn_shot] / 150;

	    	if ( ch->position == POS_FIGHTING || ch->fighting )
		    in_battle = TRUE;
		extract_obj( newobj );
		damage( ch, mob, dam, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
		if ( !in_battle )
		    stop_fighting( mob, TRUE );
		else
		    stop_fighting( mob, FALSE );

		if ( IS_NPC( mob ) )
		{
		    start_grudge( mob, ch, FALSE );
		}
             
		if ( mob->position > POS_STUNNED
		    && obj->value[3] == RNG_CATAPULT )
		{
		    act ( "The impact puts $n&n to sleep!",
			 mob, NULL, NULL, TO_ROOM );
		    act ( "The impact puts you to sleep... ZzZzZ",
			 mob, NULL, NULL, TO_CHAR  );
		    mob->position = POS_SLEEPING;
		}

//		continue;
return;
	    }
	    } 
	}

	if ( to_room->people )
	{
	    if ( n <= range )
	    {
		sprintf( buf, "%s&n streaks through the air and continues %s&n.",
			newobj->short_descr, dir_name[dir] );
		act( buf, to_room->people, NULL, NULL, TO_ROOM );
		act( buf, to_room->people, NULL, NULL, TO_CHAR );
	    }
	    else
	    {
                sprintf( buf, "%s&n zooms in and lands on the &n&+yground&n.",
                        newobj->short_descr );
		act( buf, to_room->people, NULL, NULL, TO_ROOM );
		act( buf, to_room->people, NULL, NULL, TO_CHAR );
		obj_to_room( newobj, to_room );
	    }
	}

	pexit = to_room->exit[dir];
//    }
//    }
    }

    return;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 * Heavily modified by Zen.
 * Weapons #'s:			    Ammo #'s:
	0: Range of weapon		0:
	1: Max ammo that can be held	1:
	2: Unused			2:
	3: Type of ammo loadable	3: Type of ammo.
	4: Amount of ammo loaded atm	4:
	5: vnum of ammo loaded atm	5:
 */    
void do_reload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *weapon;
    OBJ_DATA *quiver;
    OBJ_DATA *ammo;
    OBJ_DATA *ammo_next;
    char      buf [MAX_STRING_LENGTH];

    last_fun( "do_reload");
    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
       return;

    if ( !( weapon = get_eq_char( ch, WEAR_HAND_2 ) ) )
    {
        send_to_char( "I don't see that weapon here.\n\r", ch );
        return;
    }

    if ( weapon->item_type != TYPE_RANGED_WEAPON )
    {
        send_to_char( "That is not a missile weapon.\n\r", ch );
        return;
    }

    for ( ammo = ch->carrying; ammo; ammo = ammo_next )
    {
	ammo_next = ammo->next_content;

	if ( ammo->deleted )
	    continue;
	    
	if (   ammo->pIndexData->vnum == weapon->value[5] )
	    break;
	if (   weapon->value[4] == 0  
	    && ammo->item_type == TYPE_MISSILEWEAP
	    && ammo->value[3] == weapon->value[3] )
	    break;
    }

    if (!ammo && ( quiver = get_eq_char( ch, WEAR_QUIVER ) ) )
    {
    for ( ammo = quiver->contains; ammo; ammo = ammo_next )
    {
	if ( ammo->deleted )
	    continue;
	if ( ammo->pIndexData->vnum == weapon->value[5] )
	    break;
	if (   weapon->value[4] == 0  
	    && ammo->item_type == TYPE_MISSILEWEAP
	    && ammo->value[3] == weapon->value[3] )
	    break;
    }
    }

    if ( !ammo )
    {
	if (   weapon->value[4] <= 0 )
            send_to_char( "You do not have ammo for this weapon.\n\r", ch );
	else
	    send_to_char( "You can't load two types of ammo into one weapon.", ch );
        return;
    }

    if ( weapon->value[4] >= weapon->value[1] )
    {
        sprintf( buf, "%s is already fully loaded.\n\r", weapon->short_descr );
        send_to_char( buf, ch );
        return;
    }

    weapon->value[5] = ammo->pIndexData->vnum;
    weapon->value[4]++;

    act( "You get $p&n.", ch, ammo, weapon, TO_CHAR );
    act( "$n&n gets $p&n.", ch, ammo, weapon, TO_ROOM );

    act( "You load $P&n with $p&n.", ch, ammo, weapon, TO_CHAR );
    act( "$n&n loads $P&n with $p&n.", ch, ammo, weapon, TO_ROOM );

    extract_obj( ammo );
    return;
}

// Returns the percent of experience player should get for kill
// based on their trophy and increases the amount of kills on
// the player's trophy - Veygoth
int check_trophy( CHAR_DATA *ch, CHAR_DATA *victim, int members )
{
     int percent;
     int count;
     int vnum;
     int maxlev;
     bool found = FALSE;
     bool found2 = FALSE;

     if( IS_NPC( ch ))
       return 0;

     if( !IS_NPC( victim ))
       return 100;

     if( ch->level < 5 )
       return 100;

     if( ch->level < MAX_LEVEL )
       maxlev = ch->level;
     else
       maxlev = MAX_LEVEL;

     if( !(vnum = victim->pIndexData->vnum) )
     {
        bug( "Mobile without vnum in check_trophy!", 0 );
        return 100;
     }

     // If you've found an entry for the mob already just increase the
     // number of kills.
     for( count = 0; count < maxlev; count++ )
     {
        if( ch->pcdata->trophy[count].vnum == vnum )
        {
           found = TRUE;
           ch->pcdata->trophy[count].number += (100/members);
           break;
        }
     }

     // If it's not on the list yet, see if there's a blank slot we could
     // toss it into.
     if( !found )
     {
        for( count = 0; count < maxlev; count++ )
        {
           if( ch->pcdata->trophy[count].vnum == 0 
               || ch->pcdata->trophy[count].number == 0 )
           {
               ch->pcdata->trophy[count].vnum = vnum;
               ch->pcdata->trophy[count].number = ( 100 / members);
               found2 = TRUE;
               return 100;
           }
        }
     }

     // No blank slot or previous entry, roll the oldest item off of trophy
     // to make room for the newest one.
     // we cycle from the bottom to the top, moving them all up a notch, then
     // we replace the last one with the new entry
     if( !found && !found2 )
     {
       for( count = 0; count < (maxlev -1); count++ )
       {
         ch->pcdata->trophy[count].vnum = ch->pcdata->trophy[count+1].vnum;
         ch->pcdata->trophy[count].number = ch->pcdata->trophy[count+1].number;
       }
       ch->pcdata->trophy[maxlev - 1].vnum = vnum;
       ch->pcdata->trophy[maxlev - 1].number = ( 100 / members );
       return 100;
     }

     percent = 100 - ( ch->pcdata->trophy[count].number / 40 );
     if( percent < 1 )
         percent = 1;
     if( percent <= 50 )
        send_to_char( "What's the point anymore?  It just doesen't seem worth it.\n\r", ch );
     else if( percent <= 80 )
        send_to_char( "This is starting to get dull.\n\r", ch );
     else if( percent <= 90 )
        send_to_char( "You are beginning to learn your victim's weak spots.\n\r", ch );

     return percent;
}

void do_disengage( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_disengage");
     if( !ch->fighting )
     {
         send_to_char( "You're not fighting anyone!\n\r", ch );
         if( ch->position == POS_FIGHTING)
           ch->position = POS_STANDING;
         return;
     }

     if( ch->fighting->fighting && ch->fighting->fighting == ch
         && IS_SET(ch->act, PLR_VICIOUS) )
     {
         send_to_char( "You're a little busy getting beat on at the moment.\n\r", ch );
         return;
     }

     send_to_char( "You disengage from the fight!\n\r", ch );
     WAIT_STATE( ch, 8 );
     stop_fighting( ch, FALSE );

     return;
}

void do_assist( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_assist");

    if ( !check_blind ( ch ) )
        return;

    one_argument( argument, arg );

    if( ch->fighting )
    {
        send_to_char( "You're a bit busy at the moment.\n\r", ch );
        return;
    }

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
    }
    else
    {
	  send_to_char( "Assist who?\n\r", ch );
	  return;
    }

    if( victim == ch )
    {
          send_to_char( "You're too busy assisting yourself to assist yourself.\n\r", ch );
          return;
    }

    if( !victim->fighting )
    {
          send_to_char( "They're not fighting anyone.\n\r", ch );
          return;
    }

    if( victim->fighting == ch )
    {
          send_to_char( "Assist!? They're fighting YOU, fool!\n\r", ch );
          return;
    }

    act( "You scream a battle cry, and assist $N&n heroically.", ch, NULL, victim, TO_CHAR );
    act( "$n&n leaps into the fray, valiantly assisting you.", ch, NULL, victim, TO_VICT );
    act( "$n&n charges into battle and assists $N&n heroically.", ch, NULL, victim, TO_ROOM_VICT );

    one_hit( ch, victim->fighting, TYPE_UNDEFINED, WEAR_HAND );

    return;
}

void do_aware( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    last_fun( "do_aware");
    if( IS_NPC( ch ))
       return;

    if( ch->level < skills_table[gsn_aware].skill_level[ch->class] )
    {
       send_to_char( "Your general obliviousness prevents your use of this skill.\n\r", ch );
       return;
    }

    if( is_affected( ch, AFF_SKL_AWARE ))
    {
       send_to_char( "You are already about as tense as you can get.\n\r", ch );
       return;
    }

    send_to_char( "You try to become more aware of your surroundings.\n\r", ch );

    skill_practice( ch, gsn_aware );

    af.skill     = gsn_aware;
    af.spell     = 0;
    af.song      = 0;
    af.duration  = ( ch->level / 3 ) + 3;
    af.location  = 0;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SKL_AWARE );
    affect_to_char( ch, &af );

    return;
}

void blur_attack( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int num_attacks;
    int count;

    last_fun( "blur_attack");

    if( is_affected( ch, AFF_CASTING) )
        return;

    if( is_affected( ch, AFF_BLUR ) &&  number_percent() < 25 )
    {
            act( "$n&n moves with a BLUR of speed!", ch, NULL, NULL, TO_ROOM );
            act( "You move with a BLUR of speed!", ch, NULL, NULL, TO_CHAR );
            for( count = 0, num_attacks = 4;count < num_attacks && victim->position > POS_DEAD;count++ )
                one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND );
    }
    else
    {
        OBJ_DATA *wield;

        if( number_percent( ) > 10 )
            return;
        if( ch->class == CLASS_HUNTER || ch->class == CLASS_RANGER )
            num_attacks = 2;
        else if( ch->race == RACE_OGRE || ch->race == RACE_CENTAUR )
            num_attacks = 4;
        else
            num_attacks = 9;
        if( number_percent() < get_curr_luk( ch ) )
            num_attacks++;
        if( number_percent() < get_curr_luk( victim ) )
            num_attacks--;

        /* 9716 is the vnum for the dagger of the wind. */
        if( ( wield = get_eq_char( ch, WEAR_HAND ) )
            && wield->pIndexData->vnum == 9716 )
        {
            act( "&+c$n&+c's $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_ROOM );
            act(       "Your $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_CHAR );
            for( count = 0;count < num_attacks && victim->position > POS_DEAD;count++ )
                one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND );
            return;
        }
        if( ( wield = get_eq_char( ch, WEAR_HAND_2 ) )
            && wield->pIndexData->vnum == 9716 )
        {
            act( "&+c$n&+c's $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_ROOM );
            act(       "Your $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_CHAR );
            for( count = 0;count < num_attacks && victim->position > POS_DEAD;count++ )
                one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND_2 );
            return;
        }
        if( ( wield = get_eq_char( ch, WEAR_HAND_3 ) )
            && wield->pIndexData->vnum == 9716 )
        {
            act( "&+c$n&+c's $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_ROOM );
            act(       "Your $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_CHAR );
            for( count = 0;count < num_attacks && victim->position > POS_DEAD;count++ )
                one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND_3 );
            return;
        }
        if( ( wield = get_eq_char( ch, WEAR_HAND_4 ) )
            && wield->pIndexData->vnum == 9716 )
        {
            act( "&+c$n&+c's $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_ROOM );
            act(       "Your $p&n &+cbegins to move with the &+Wspeed&+c of a &+lstorm&+c!&n", ch, wield, NULL, TO_CHAR );
            for( count = 0;count < num_attacks && victim->position > POS_DEAD;count++ )
                one_hit( ch, victim, TYPE_UNDEFINED, WEAR_HAND_4 );
            return;
        }
    }
}

/*
 * Check for shrug.
 */
bool check_shrug( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_SET( race_table[victim->race].race_abilities, RACE_SHRUG ) )
    	return FALSE;

    switch ( victim->race )
    {	// Default at 99 to show races not listed here.
	case RACE_GREYELF:	chance = 25;			break;
	case RACE_GITHZERAI:	chance = 10;			break;
	case RACE_DROW:		chance = 40;			break;
	case RACE_RAKSHASA:	chance = 15;			break;
	case RACE_GITHYANKI:	chance = 20;			break;
			 // Demons, devils, etc.
	default:		chance = 25 + (victim->level)/2;break;
    }

    if ( number_percent() < chance )
    {
        act("&+MYour spell flows around &n$N&+M, leaving $M unharmed!&n", ch, NULL, victim, TO_CHAR);
        act( "&+M$N&+M's spell flows around you, leaving you unharmed!&n", victim, NULL, ch, TO_CHAR );
        act( "&+M$N&+M's spell flows around $n&+M, leaving $m unharmed!&n", victim, NULL, ch, TO_ROOM_VICT );
        return TRUE;
    }
    else
        return FALSE;
}


void stop_med( CHAR_DATA * ch )
{
    last_fun( "stop_med");
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEDITATING ))
    {
        act( "$n&n is disrupted from meditation.", ch, NULL, NULL, TO_ROOM );
        act( "Your meditation is disrupted.",      ch, NULL, NULL, TO_CHAR );
        REMOVE_BIT( ch->act, PLR_MEDITATING );
    }

}  

/*
 * Inviso attacks ... not.
 */
void make_vis( CHAR_DATA * ch )
{
    last_fun( "make_vis");
    if ( is_affected( ch, AFF_INVISIBLE ) ) 
    {
        remove_affects( ch, AFF_INVISIBLE );
        remove_affects( ch, AFF_HIDE );
        remove_affects( ch, AFF_MINOR_INVIS );
        act( "$n&n snaps into visibility.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You snap into visibility.\n\r", ch);
    }
    if ( IS_SET( ch->act, PLR_WIZINVIS ) && !IS_IMMORTAL( ch ) ) {
        REMOVE_BIT( ch->act, PLR_WIZINVIS );
    }

}

void stop_mem( CHAR_DATA * ch )
{
    last_fun( "stop_mem");
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    {
        act( "$n&n abandons $s studies.", ch, NULL, NULL, TO_ROOM );
        act( "You abandon your studies.", ch, NULL, NULL, TO_CHAR );
        REMOVE_BIT( ch->act, PLR_MEMORIZING );
    }
}

void do_stance( CHAR_DATA *ch, char *argument )
{
    int stance;
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_stance");
    if( (ch->class != CLASS_MONK && ch->class != CLASS_MYSTIC) || IS_NPC( ch ))
    {
       send_to_char( "You don't know how to do anything fancier than just stand there.\n\r", ch );
       return;
    }

    if( argument[0] == '\0' )
    {
      sprintf( buf, "Available stances:  none" );

      if( ch->pcdata->monk_skill[STANCE_BEAR] )
        strcat( buf, ", bear" );
      if( ch->pcdata->monk_skill[STANCE_CAT] )
        strcat( buf, ", cat" );
      if( ch->pcdata->monk_skill[STANCE_COBRA] )
        strcat( buf, ", cobra" );
      if( ch->pcdata->monk_skill[STANCE_CRANE] )
        strcat( buf, ", crane" );
      if( ch->pcdata->monk_skill[STANCE_DRAGON] )
        strcat( buf, ", dragon" );
      if( ch->pcdata->monk_skill[STANCE_DRAGONFLY] )
        strcat( buf, ", dragonfly" );
      if( ch->pcdata->monk_skill[STANCE_HAWK] )
        strcat( buf, ", hawk" );
      if( ch->pcdata->monk_skill[STANCE_LEOPARD] )
        strcat( buf, ", leopard" );
      if( ch->pcdata->monk_skill[STANCE_MANTIS] )
        strcat( buf, ", mantis" );
      if( ch->pcdata->monk_skill[STANCE_MONKEY] )
        strcat( buf, ", monkey" );
      if( ch->pcdata->monk_skill[STANCE_SNAKE] )
        strcat( buf, ", snake" );
      if( ch->pcdata->monk_skill[STANCE_TIGER] )
        strcat( buf, ", tiger" );

      strcat( buf, "\n\r" );

      send_to_char( buf, ch );
      return;
    }

    if( !str_cmp( argument, "none" ))
    {
      ch->pcdata->stance = STANCE_NONE;
      act( "You break out of your fighting stance and return to a relaxed position.", ch, NULL, NULL, TO_CHAR );
      act( "$n&n leaves $s fighting stance.", ch, NULL, NULL, TO_ROOM );
      return;
    }

    if( !str_cmp( argument, "bear" ))
      stance = STANCE_BEAR;
    else if( !str_cmp( argument, "cat" ))
      stance = STANCE_CAT;
    else if( !str_cmp( argument, "cobra" ))
      stance = STANCE_COBRA;
    else if( !str_cmp( argument, "crane" ))
      stance = STANCE_CRANE;
    else if( !str_cmp( argument, "dragon" ))
      stance = STANCE_DRAGON;
    else if( !str_cmp( argument, "dragonfly" ))
      stance = STANCE_DRAGONFLY;
    else if( !str_cmp( argument, "hawk" ))
      stance = STANCE_HAWK;
    else if( !str_cmp( argument, "leopard" ))
      stance = STANCE_LEOPARD;
    else if( !str_cmp( argument, "mantis" ))
      stance = STANCE_MANTIS;
    else if( !str_cmp( argument, "monkey" ))
      stance = STANCE_MONKEY;
    else if( !str_cmp( argument, "snake" ))
      stance = STANCE_SNAKE;
    else if( !str_cmp( argument, "tiger" ))
      stance = STANCE_TIGER;

    if( !ch->pcdata->monk_skill[stance] )
    {
      sprintf( buf, "You try to shift into a %s stance and realize you have no idea what you are doing.", stance_name(stance) );
      act( buf, ch, NULL, NULL, TO_CHAR );
      act( "$n&n contorts $s body strangely, nearly tying $mself into a knot.  With a frustrated look, $e returns to a standing position.", ch, NULL, NULL, TO_ROOM );
      ch->pcdata->stance = STANCE_NONE;
      return;
    }

    ch->pcdata->stance = stance;

    sprintf( buf, "You shift your body into a %s stance.", stance_name(stance) );
    act( buf, ch, NULL, NULL, TO_CHAR );
    sprintf( buf, "$n&n changes $s stance, looking much like a %s", stance_name(stance) );
    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}

bool check_vicious( CHAR_DATA *ch, CHAR_DATA *victim ) {
    if ( IS_SET( ch->act, PLR_VICIOUS ) )
        return TRUE;
    if ( victim->position < POS_RECLINING
        && victim->position != POS_STUNNED )
        return FALSE;
    return TRUE;
}

bool check_aggressive( CHAR_DATA *ch, CHAR_DATA *victim ) {
    if ( !is_aggro( victim, ch ) )
        return FALSE;
   if ( !IS_NPC( ch ) && ch->pcdata->aggr_level >= 1
        && ch->pcdata->aggr_level < ch->hit
        && ch->position == POS_STANDING
        && !is_affected( ch, AFF_CASTING) ) {
            if ( victim->hating && victim->hating->who == ch )
               stop_hating( victim );
            send_to_char("You charge aggressively at your foe!\n\r", ch);
            if (ch->class == CLASS_THIEF || ch->class == CLASS_ASSASSIN
            || ch->class == CLASS_BARD || ch->class == CLASS_MERCENARY ) {
                backstab( ch, victim ); 
                set_fighting( victim, ch );
            }
            else
                multi_hit( ch, victim, TYPE_UNDEFINED );
        return TRUE;
    }
    return FALSE;
}

// Added by Veygoth
bool check_tumble( CHAR_DATA *ch )
{
   int chance;

   if( ch->level < skills_table[gsn_tumble].skill_level[ch->class] )
     return FALSE;

   if( IS_NPC( ch ))
     chance = ch->level / 2 + 8;
   else
     chance = ch->pcdata->skl_lrn[gsn_tumble] / 3;

   chance += get_curr_agi( ch ) / 15;

   skill_practice( ch, gsn_tumble );

   if( number_percent() >= chance )
     return FALSE;

   return TRUE;
}

void stop_not_vicious( CHAR_DATA *victim )
{
    CHAR_DATA *nice_ch;

    for( nice_ch = victim->in_room->people; nice_ch; nice_ch = nice_ch->next_in_room )
    {
        if( !IS_SET( nice_ch->act, PLR_VICIOUS )
             && nice_ch->fighting == victim      )
            stop_fighting( nice_ch, FALSE );
    }
}

/* 
 * To be used only with gsn's, not spells.
 * - Lithos
 */
void inflict_poison( int gsn, int level, int type, CHAR_DATA *ch, CHAR_DATA *victim ) {
    AFFECT_DATA af;

    last_fun( "inflict_poison");
    if( saves_spell( level, victim, DAM_POISON ) )
        return;

    af.skill     = gsn;
    af.spell     = 0;
    af.song      = 0;
    af.level     = level;
    af.duration  = level/4;
    af.location  = APPLY_NONE;
    af.modifier  = type;
    set_bitvector( &af, AFF_POISON);
    affect_join( victim, &af );

    if ( ch != victim )
    send_to_char( "You have poisoned your victim.\n\r", ch );
    send_to_char( "You feel very sick.\n\r", victim );
    return;
}

/* Check to see if mob should shout for protection.  This applies to only
 * 2 plane gods atm, but you can add others.  The call for this function
 * should be within an if statement checking IS_NPC and valid vnum of the
 * NPC.  It'll make things a lot faster than calling it too often. - Lohrr
 */
bool check_shout( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char      buf [ MAX_STRING_LENGTH ];
    CHAR_DATA *minion;

    last_fun( "check_shout");

    if( !IS_NPC( ch ) || IS_NPC( victim ) || !ch->pIndexData )
        return FALSE;

    switch ( ch->pIndexData->vnum )
    {
        case 9316:
            sprintf( buf, "Denizens of the Fire Plane, come slay %s!",
                victim->name );
            break;
        case 9748:
            sprintf( buf, "Denizens of the Air Plane, come slay %s!",
                victim->name );
            break;
        default:
            sprintf( buf, "Someone kill %s!", victim->name );
            return FALSE;
            break;
    }

    do_shout( ch, buf );

    for( minion = char_list; minion; minion = minion->next )
    {
        /* Minion must be a NPC from the right plane and on that plane */
        if(     minion->deleted
            || !IS_NPC( minion ) 
            ||  minion->in_room->area != ch->in_room->area
            ||  minion->pIndexData->area != ch->in_room->area )
            continue;
        last_fun( minion->name );
        /* This is a ranged call to start_grudge */
        start_grudge( minion, victim, TRUE );
    }

    return TRUE;
}

bool is_boss( CHAR_DATA *ch ) {
    int i;
    int list[] = {9511, 20527, 20537, 0};

    if ( !IS_NPC( ch ) )
        return FALSE;
    for ( i=0; list[i] > 0; i++ ) {
        if ( list[i] == ch->pIndexData->vnum )
            return TRUE;
    }
    return FALSE;
}

void do_throw( CHAR_DATA *ch, char *argument ) {
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *in_hand, *in_hand2;
    ROOM_INDEX_DATA *room2;
    int dir;
    bool two_rooms;

    last_fun("do_throw");

    if ( TRUE ) {
        send_to_char( "Throwing weapons is not implemented yet.\n\r", ch );
        return;
    }
    // find an object to throw
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' ) {
        send_to_char( "Throw what exactly?\n\r", ch);
        return;
    }
    in_hand = get_eq_char( ch, WEAR_HAND );
    in_hand2 = get_eq_char( ch, WEAR_HAND_2 );
    if ( !in_hand && !in_hand2 ) {
        send_to_char( "You need to be wielding _something_ to throw it!\n\r", ch );
        return;
    }
    obj = NULL;
    if ( in_hand && strstr( arg, in_hand->name ) ) {
        obj = in_hand;
    } else if ( in_hand2 && strstr( arg, in_hand2->name ) ) {
        obj = in_hand2;
    }
    if ( !obj ) {
        send_to_char( "You don't seem to be wielding that.\n\r", ch );
        return;
    }
    // is it throwable?
    two_rooms = !IS_OBJ_STAT( obj, ITEM_THROW_TWOROOMS );
    if ( !IS_OBJ_STAT( obj, ITEM_THROW_ONEROOM ) && two_rooms ) {
        send_to_char(  "That is not throwable.\n\r", ch );
        return;
    }

    // now find a valid target
    argument = one_argument( argument, arg2 );
    if ( arg2[0] == '\0' ) {
        send_to_char( "Throw it at who?\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg3 );
    if ( arg3[0] == '\0' ) {
        if (ch->level >= LEVEL_IMMORTAL) {
            sprintf(lbuf,"Looking for %s in same room.\n\r",arg2);
            send_to_char(lbuf, ch);
        }
        victim = get_char_room( ch, arg2 );
        if ( !victim )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }
    }
    else {
        dir = find_exit( ch, arg3 );
        if (ch->level >= LEVEL_IMMORTAL) {
           sprintf(lbuf,"Looking for %s to the %s.\n\r",arg2,arg3);
           send_to_char(lbuf, ch );
        }
    }
    if ( IS_SET(ch->in_room->exit[dir]->exit_info, EX_WALLED)
         || IS_SET(ch->in_room->exit[dir]->exit_info, EX_BLOCKED)
         || IS_SET(ch->in_room->exit[dir]->exit_info, EX_SECRET)
         || IS_SET(ch->in_room->exit[dir]->exit_info, EX_CLOSED)
         || !ch->in_room->exit[dir]->to_room
         || ch->in_room->area != ch->in_room->exit[dir]->to_room->area )
    {
        send_to_char("You see nothing in that direction.\n\r", ch);
        return;
    }
    room2 = find_room(ch, arg3);
    if (!room2) {
        send_to_char("You see nothing in that direction.\n\r", ch);
        return;
    }
    victim = NULL;
    victim = get_char_at(room2, ch, arg2);
    if (!victim ) {
        send_to_char( "They aren't here.\n\r", ch);
        return;
    }
    return;
}

