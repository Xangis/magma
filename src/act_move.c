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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"

MOVE_SCRIPT * mvscript_first = NULL;
MOVE_SCRIPT * mvscript_free = NULL;

int scan_this_room(CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *buf, int fly_level);

char *	const	dir_name	[ ]		=
{
    "north", "east", "south", "west", "up", "down",
    "northwest", "southwest", "northeast", "southeast"
};

char *  const   dir_rev		[ ]		=
{
    "the south", "the west", "the north", "the east", "below", "above",
    "the southeast", "the northeast", "the southwest", "the southwest"
};

const   int     trap_dir_flag   [ ]             =
{
   TRIG_NORTH, TRIG_EAST, TRIG_SOUTH, TRIG_WEST, TRIG_UP, TRIG_DOWN
};

const	int	rev_dir		[ ]		=
{
    2, 3, 0, 1, 5, 4, 9, 8, 7, 6
};

// Original Data - values were reduced by Ski per orders
// from Thendar.  All hail Thendar!
/*
{
    1, 2, 2, 3, 4, 6, 4, 1, 2, 10, // Sector types 0-9
    8, 6, 8, 3, 2, 1, 4, 10,2, 6,  // Sector types 10-19
    6, 6, 6, 6, 5, 5, 6, 1, 600, 8    // Sector types 20-28
};
*/
// New Data
const	int	movement_loss	[ SECT_MAX ]	=
{
    1, 1, 1, 2, 3, 4, 3, 1, 2, 5, // Sector types 0-9
    5, 2, 5, 2, 1, 1, 3, 5, 1, 2,  // Sector types 10-19
    2, 2, 2, 2, 2, 2, 4, 1, 600, 8    // Sector types 20-28
};


/*
 * Local functions.
 */
#define OD      OBJ_DATA

int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
OD*	has_key		args( ( CHAR_DATA *ch, int key ) );

#undef OD

// All in all, this code is absolutely HUGE and nearly unreadable.  It
// could use a little cleanup - Veygoth
void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA       *fch;
    CHAR_DATA       *fch_next;
    CHAR_DATA       *watch_ch;
    EXIT_DATA       *pexit;
    OBJ_DATA        *obj;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *target;
    int              moved = BV17; /* Matches ACT & PLR bits */
    int		     visibility;
    char             buf[MAX_STRING_LENGTH];
    char             buf1[MAX_STRING_LENGTH];
    bool	         riding;
    bool             disbelief = FALSE;
    bool             illusion = FALSE;
    int              dchance;
    int              move;

    last_fun("move_char");

    if ( door < 0 || door >= MAX_DIR )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    /*
     * Prevents infinite move loop in
     * maze zone when group has 2 leaders - Kahn
     */
    if ( IS_SET( ch->act, moved ) )
        return;

    if ( is_affected( ch, AFF_MINOR_PARA )
      || is_affected( ch, AFF_BOUND )
      || is_affected( ch, AFF_HOLD ) ) 
    {
	send_to_char( "You can't move!\n\r", ch );
	return;
    }

    if ( ch->riding && (is_affected( ch->riding, AFF_BOUND ) || 
                        is_affected( ch->riding, AFF_HOLD ) ||
                        is_affected( ch->riding, AFF_MINOR_PARA )))
    {
        send_to_char( "Your ride can't move.\n\r", ch );
        return;
    }

    // More likely to go NSEW than any other dirs
    if( is_affected( ch, AFF_MISDIRECTION ) )
    {
      if( number_range( 1, 2 ) == 1 )  
        door = number_range( 0, 3 );
      else
        door = number_door();
    }

    in_room = ch->in_room;

    REMOVE_AFF_BIT( ch, AFF_HIDE );

    if ( !( pexit = in_room->exit[door] ) || !( to_room = pexit->to_room )
         || ( to_room->sector_type == SECT_UNDERG_IMPASSABLE )
         || ( to_room->extra_sector_info == EXT_ZONEMARKER )
	 || ( ch->level < LEVEL_IMMORTAL 
         && ( IS_SET( in_room->exit[door]->exit_info, EX_SECRET )
         ||   IS_SET( in_room->exit[door]->exit_info, EX_BLOCKED ) ) ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if (ch->fly_level && !is_fly_room( to_room ) )
    {
        send_to_char( "Alas, you cannot fly there.\n\r", ch );
        return;
    }

    // For trapped objects in the room that trigger on certain actions
    for( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
        if( !obj->trap || obj->trap->charges == 0)
           continue;
        if( IS_SET( obj->trap->trigger, TRIG_MOVE ))
        {
           setoff_trap( ch, obj );
           if( ch->deleted || ch->position == POS_DEAD )
             return;
        }
        else if( door < 6 )
        {
            if( IS_SET( obj->trap->trigger, trap_dir_flag[door] ))
            {
               setoff_trap( ch, obj );
               if( ch->deleted || ch->position == POS_DEAD )
                 return;
            }
        }
    }

    // Being walled doesen't mean they can't go there, it just means that we
    // need to check for wall functions.
    if( !is_affected( ch, AFF_CLIMBING) && IS_SET( in_room->exit[door]->exit_info, EX_WALLED ))
    {
        OBJ_DATA *wall;

        for( wall = ch->in_room->contents; wall; wall = wall->next_content )
        {
          if( wall->deleted || wall->item_type != TYPE_WALL || wall->value[0] != door )
            continue;
          if( wall->pIndexData->vnum == OBJ_VNUM_WALL_ILLUSION)
          {
              illusion = TRUE;
              //test for disbelief
              dchance = get_curr_int( ch ) / 2 + ch->level - wall->value[2];
              if (ch->fighting) dchance /= 2;
              if (ch->race == RACE_OGRE || ch->race == RACE_CENTAUR ) dchance /= 4;
              if ( number_percent() < dchance )
              {
                  // disbelief
                  act("You disbelieve $p&n!", ch, obj, NULL, TO_CHAR);
                  disbelief = TRUE;
              }
          }

          if( wall->value[1] == FALSE && !disbelief)
          {  
            if (!illusion || (illusion && can_see_obj( ch, obj )) ) {
              act( "You bump into $p&n.", ch, wall, NULL, TO_CHAR );
              act( "$n&n bumps into $p&n.", ch, wall, NULL, TO_ROOM );
              if( wall->pIndexData->vnum == OBJ_VNUM_WALL_ICE )
              {
                 spell_damage( ch, ch, dice( 2, (wall->value[2] / 2) ), spl_wall_of_ice, DAM_COLD );
                 if( ch->deleted || ch->position == POS_DEAD )
                   return;
              }
              else if( wall->pIndexData->vnum == OBJ_VNUM_WALL_FORCE )
              {
                 ch->move -= dice( 2, 15 );
                 send_to_char( "You feel drained of energy!\n\r", ch );
                 send_to_char( "The force of the wall knocks you down!\n\r", ch );
                 ch->position = POS_SITTING;
                 WAIT_STATE( ch, dice( 2, 9 ));
              }
            } //end if !illusion
            else {
                send_to_char("Alas, you cannot go that way.\n\r", ch);
            }
	    return;
          }
          else if (disbelief) {
              act( wall_decay_msg(OBJ_VNUM_WALL_ILLUSION), ch, obj, NULL, TO_ALL );
              extract_obj( wall );
              REMOVE_BIT( in_room->exit[door]->exit_info, EX_WALLED );
          }
          else if ( !illusion )
          {
            act( "You just walked through $p&n.", ch, wall, NULL, TO_CHAR );
            act( "$n&n just walked through $p&n.", ch, wall, NULL, TO_ROOM );
            if( wall->pIndexData->vnum == OBJ_VNUM_WALL_FIRE )
            {
               spell_damage( ch, ch, dice( wall->value[2], 5 ), spl_wall_of_fire, DAM_FIRE );
               if( ch->deleted || ch->position == POS_DEAD )
                 return;
            }
            else if( wall->pIndexData->vnum == OBJ_VNUM_WALL_SPARKS )
            {
               spell_damage( ch, ch, dice( 3, wall->value[2] ), spl_wall_of_sparks, DAM_FIRE );
               if( ch->deleted || ch->position == POS_DEAD )
                 return;
            }
            else if( wall->pIndexData->vnum == OBJ_VNUM_LIGHTNING_CURTAIN )
            {
               spell_damage( ch, ch, dice( wall->value[2], 5 ), spl_lightning_curtain, DAM_ELECTRICITY );
               if( ch->deleted || ch->position == POS_DEAD )
                 return;
            }
            if( wall->pIndexData->vnum == OBJ_VNUM_WALL_MIST )
            {
               if( !saves_spell( wall->value[2], ch, DAM_POISON ))
               {
		    AFFECT_DATA af;
                    af = create_affect( 0, spl_poison, 0, wall->value[2], APPLY_STR, (0 - 2), AFF_POISON );
		    affect_join( ch, &af );
	    	    send_to_char( "You feel very sick.\n\r", ch );
               }
               spell_damage( ch, ch, dice( 4, wall->value[2] ), spl_wall_of_mist, DAM_POISON );
               if( ch->deleted || ch->position == POS_DEAD )
                 return;
            }
          }
        }
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
	if ( ch->riding )
	{
	    if (   !is_affected( ch->riding, AFF_PASS_DOOR )
		&& !IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_PASSDOOR )
		&& !IS_IMMORTAL ( ch->riding ) )
	    {
		act( "The $d is closed so your mount is unable to pass.",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }

	    if ( IS_SET( pexit->exit_info, EX_PASSPROOF )
		&& !IS_IMMORTAL( ch->riding ) )
	    {
		act( "Your mount is unable to pass through the $d.  Ouch!",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	}
	else
	{
	    if (   !is_affected( ch, AFF_PASS_DOOR )
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_PASSDOOR )
		&& !IS_IMMORTAL ( ch ) )
	    {
		act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	
	    if ( IS_SET( pexit->exit_info, EX_PASSPROOF )
		&& !IS_IMMORTAL( ch ) )
	    {
		act( "You are unable to pass through the $d.  Ouch!",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	}
    }

    if ( ch->riding && ch->riding->position < POS_STANDING )
    {
        send_to_char( "Your ride doesn't want to move right now.\n\r", ch );
        return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    // RACE_SWIM means you're an aquatic race... and fish can't walk on land.
    if ( ch->riding )
    {
	if ( !(is_water_room( to_room )) && IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_SWIM ) )
	{
	    send_to_char( "Your mount flaps around but can't move!\n\r", ch );
	    return;
        }
    }
    else
    {
	if ( !(is_water_room( to_room )) && IS_SET( race_table[ ch->race ].race_abilities, RACE_SWIM ) )
	{
	    send_to_char( "You flap around but you can't move!\n\r", ch );
	    return;
	}
    }

    // allow all mobs to go wherever they want?  not allowing them to would really
    // screw up the track code -- Veygoth
    if ( !IS_NPC( ch ) )
    {

	if ( (is_midair_room( ch->in_room ) && !ch->in_room->fall_chance)
             || (is_midair_room( to_room ) && !to_room->fall_chance) )
	{
            if( ch->in_room->fall_chance )
                check_fall( ch->in_room, to_room, ch );
	    if ( ch->riding )
	    {
		if ( !can_fly( ch->riding ) && door != DIR_DOWN )
		{
		    send_to_char( "Your mount can't fly.\n\r", ch );
		    return;
		}
	    }
	    else
	    {
                // Changed to allow going down always in an air sector - Veygoth
		if ( !can_fly( ch ) && door != DIR_DOWN )
		{
		    send_to_char( "You can't fly.\n\r", ch );
		    return;
		}
	    }
	}

        // Water terrain code, check for boats and swimming.  Flyers don't need to swim
	if ( (( ch->riding && !can_fly( ch->riding )) || (!ch->riding && !can_fly( ch )))
             && ( is_water_room( to_room ) || is_water_room( in_room )))
	  {
	    OBJ_DATA *obj;
	    bool      found = FALSE;

            if( ch->riding && !IS_NPC( ch->riding ))
            {
               if( ch->riding->pcdata->skl_lrn[gsn_swim] )
               {
                  found = TRUE;
                  skill_practice( ch->riding, gsn_swim );
                  skill_practice( ch->riding, gsn_swim );
                  skill_practice( ch->riding, gsn_swim );
               }
            }
            else if( !IS_NPC( ch ))
            {
               if( ch->pcdata->skl_lrn[gsn_swim] )
               {
                  found = TRUE;
                  skill_practice( ch, gsn_swim );
                  skill_practice( ch, gsn_swim );
                  skill_practice( ch, gsn_swim );
               }
            }


            // Check for a boat first, uses less effort than swimming.
	    for ( obj = ch->carrying; obj; obj = obj->next_content )
	    {
		if ( obj->item_type == TYPE_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }

	    if ( ch->riding && ( IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_WATERWALK )
		    || IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_SWIM ) ) )
            {
	        found = TRUE;
            }

	    if ( IS_SET( race_table[ ch->race ].race_abilities, RACE_WATERWALK )
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_SWIM ))
            {
	        found = TRUE;
            }

	    if ( !found )
	    {
		send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
        }

	if ( ch->riding )
	{
	    if ( ( in_room->sector_type == SECT_UNDERWATER
		|| to_room->sector_type == SECT_UNDERWATER )
		&& !IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_SWIM )
		&& ( !IS_NPC( ch->riding )
		    && !ch->pcdata->skl_lrn[gsn_swim] ) )
	    {
		send_to_char( "Your mount needs to be able to swim better to go there.\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( ( in_room->sector_type == SECT_UNDERWATER
		|| to_room->sector_type == SECT_UNDERWATER )
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_SWIM )
		&& !ch->pcdata->skl_lrn[gsn_swim] )
	    {
		send_to_char( "You need to be able to swim better to go there.\n\r", ch );
                skill_practice( ch, gsn_swim );
                skill_practice( ch, gsn_swim );
		return;
	    }
	}

        if( in_room->sector_type < SECT_MAX && to_room->sector_type < SECT_MAX &&
            in_room->sector_type > -1 && to_room->sector_type > -1 )
        {
  	  move = (movement_loss[in_room->sector_type] + movement_loss[to_room->sector_type ]) / 2;
        }
        else
        {
          move = 4;
        }

	/* Flying persons lose constant minimum movement. */
	if ( can_fly( ch ) || is_affected( ch, AFF_LEVITATE ) )
          move = 1;

	if ( ( ch->riding ) )
        {
            if ( ch != ch->riding->rider )
            {
                char buf[MAX_STRING_LENGTH];

                sprintf( buf, "%s riding %s, who is not mounted!",
                    ch->name, ch->riding->name );
                bug( buf, 0 );
            }
            else
            {
                ch = ch->riding;
                riding = TRUE;
            }
        }

	if (( ch->move < move ) && ( !IS_IMMORTAL( ch ))) // Govno
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
            if ( ( ch->rider) && riding )
            {
                send_to_char( "Your mount is too exhausted.\n\r", ch->rider );
            }
	    return;
	}

	WAIT_STATE( ch, 1 );
	ch->move -= move;

	if ( ( ch->rider) && riding )
        {
            ch = ch->rider;
        }
    }

    if (   !is_affected( ch, AFF_SNEAK )
        && !(  has_affect( ch, gsn_sneak, 0, 0 ) 
            && !IS_NPC( ch ) && ch->pcdata
            && number_percent() < ch->pcdata->skl_lrn[gsn_sneak] )
	&& !IS_SET( ch->act, PLR_WIZINVIS )
        && !is_affected( ch, AFF_IS_FLEEING ) )
    {
        if ( ( (     in_room->sector_type == SECT_WATER_SWIM )
		|| ( in_room->sector_type == SECT_UNDERWATER ) )
	    && (   ( to_room->sector_type == SECT_WATER_SWIM )
		|| ( to_room->sector_type == SECT_UNDERWATER ) ) )
	{
	    act( "$n&n swims $T.",  ch, NULL, dir_name[door], TO_ROOM );
	}
	else
	{
	    for ( watch_ch = ch->in_room->people; watch_ch;
	          watch_ch = watch_ch->next_in_room )
	    {
		if (watch_ch == ch || watch_ch->fly_level != ch->fly_level)
		    continue;
	    	visibility = how_see(watch_ch, ch);
		switch ( visibility )
	 	{
		case SEE_VISIBLE:
 		    if ( !ch->riding )
		    {
 			sprintf( buf1, "%s&n %s %s.\n\r",
			    	 PERS( ch, watch_ch ), 
			         race_table[ch->race].walk_message, dir_name[door]);
			send_to_char( buf1, watch_ch);
		    }
		    else
		    {
		        sprintf( buf1, "%s&n rides %s&n %s.\n\r", PERS(ch, watch_ch),
				 PERS( ch->riding , watch_ch), dir_name[door] );
			send_to_char( buf1, watch_ch);
		    }
 		    break;
		case SEE_SENSE_INFRA:
		    sprintf( buf1, "&+LA &n&+rred shape &+Lleaves %s in the &+Wd&n&+war&+Lkness.\n\r&n",
			     dir_name[door]);
		    send_to_char( buf1, watch_ch);
		    break;
		case SEE_INVISIBLE:
 		    if ( ch->riding )
		    {
			if ( how_see(watch_ch, ch->riding) == SEE_VISIBLE )
			{
   			    sprintf( buf1, "%s&n %s %s.\n\r", 
				PERS( ch->riding, watch_ch ),
			        race_table[ch->riding->race].walk_message,
				dir_name[door]);
			    send_to_char( buf1, watch_ch);
			}
		    }
		default:
		    break;
		}
	    }
	}
    }

    char_from_room( ch );
    char_to_room( ch, to_room );

    if ( is_affected( ch, AFF_IS_FLEEING ) ) {
        sprintf(buf, "move_char: %s has fled to room %d.", ch->name, ch->in_room->vnum);
        wiznet( 0, WIZ_LSPAM, 0 , buf);
    }

    if (   !IS_AFFECTED( ch, AFF_SNEAK )
        && !( has_affect( ch, gsn_sneak, 0, 0 ) 
            && !IS_NPC( ch ) && ch->pcdata
            && number_percent() < ch->pcdata->skl_lrn[gsn_sneak] )
	&& ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
    {
	if ( ch->riding )
        {
          sprintf( buf, "$n&n enters from %s, riding $N&n.",
             dir_rev[door] );
          act( buf, ch, NULL, ch->riding, TO_ROOM );
        }
	else if ( !ch->rider )
        {
            int sight;
            // Changed: Don't show invis people entering room.-Lohrr
	    for ( watch_ch = ch->in_room->people; watch_ch;
		  watch_ch = watch_ch->next_in_room )
            {
		if ((sight = how_see( watch_ch, ch )) && watch_ch != ch 
                    && watch_ch->fly_level == ch->fly_level)
		{   
                    // If visible to char (& not char), show msg.
		    if ( sight == SEE_VISIBLE )
		    {
                        // If they can see the person enter clearly..
		        sprintf( buf1, "%s&n %s in from %s.\n\r",
			    	 PERS( ch, watch_ch ), 
	        		 race_table[ch->race].walk_message,
				 dir_rev[door] ); 
			send_to_char( buf1, watch_ch);
		    }
		    else if( sight == SEE_SENSE_INFRA )
		    {
                        // Otherwise, show a vague message..
 		        sprintf( buf1, "&+LA &n&+rred shape &+Lenters from %s.\n\r&n",
			    	 dir_rev[door]);
			        send_to_char( buf1, watch_ch);
		    }
		}
            }
        }
    }

    /* Because of the addition of the deleted flag, we can do this -Kahn */
    if ( !IS_IMMORTAL( ch ) && ch->race == RACE_VAMPIRE
	&& to_room->sector_type == SECT_UNDERWATER )
    {
	send_to_char( "Arrgh!  Large body of water!\n\r", ch );
	act( "$n&n thrashes underwater!", ch, NULL, NULL, TO_ROOM );
	damage( ch, ch, 20, TYPE_UNDEFINED, WEAR_NONE, DAM_DROWNING );
    }
    else if ( !IS_IMMORTAL( ch )
	     && ( to_room->sector_type == SECT_UNDERWATER
		 && !is_affected( ch, AFF_GILLS )
		 && !IS_SET( race_table[ ch->race ].race_abilities,
			    RACE_WATERBREATH ) ) )
    {
	send_to_char( "You can't breathe!\n\r", ch );
	act( "$n&n sputters and chokes!", ch, NULL, NULL, TO_ROOM );
	damage( ch, ch, 2, TYPE_UNDEFINED, WEAR_NONE, DAM_DROWNING );
    }

    /*
     * Suggested by D'Sai from A Moment in Tyme Mud.  Why have mobiles
     * see the room?  -Kahn
     */
    if ( ch->desc )
    {
	do_look( ch, "auto" );
    }

    SET_BIT( ch->act, moved );

    for ( fch = in_room->people; fch; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if ( fch->deleted )
	    continue;
      
	if ( fch->master == ch && fch->position == POS_STANDING && fch->wait == 0 
            && fch->fly_level == ch->fly_level && can_see( fch, ch ) )
	{
	    act( "You follow $N&n.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door );
	}
        else if ( fch == ch->rider )
	    move_char( fch, door );
    }

    REMOVE_BIT( ch->act, moved );

    // okay, now that they're in to room, check to see if they've just invaded a
    // hometown
    check_invader( ch );

    // Check to see if a person falls out of this room
    if( ch->in_room->exit[5] && (target = ch->in_room->exit[5]->to_room ))
    {
      if( ch->in_room->sector_type == SECT_AIR ||
          ch->in_room->sector_type == SECT_PLANE_AIR ||
          ch->in_room->sector_type == SECT_UNDERG_NOGROUND ||
          ch->in_room->fall_chance )
             check_fall( ch->in_room, target, ch );
    }

   /*
    * Testing this here, it can take too long if waiting for aggr_update
    *  - Lithos
    */
   // Its spam crashing on this code, commented out -- Veygoth
    /*
    for (fch = ch->in_room->people; fch; fch = fch->next_in_room) {
        if (fch->deleted || !IS_NPC( fch ) ) continue;
        check_aggressive( ch, fch );
    }*/


    prog_entry_trigger( ch );
    prog_greet_trigger( ch );
    return;
}

void do_north( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_north");
    move_char( ch, DIR_NORTH );
    return;
}

void do_northeast( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_northeast");
    move_char( ch, DIR_NORTHEAST );
    return;
}

void do_northwest( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_northwest");
    move_char( ch, DIR_NORTHWEST );
    return;
}

void do_east( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_east");
    move_char( ch, DIR_EAST );
    return;
}

void do_south( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_south");
    move_char( ch, DIR_SOUTH );
    return;
}

void do_southeast( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_southeast");
    move_char( ch, DIR_SOUTHEAST );
    return;
}

void do_southwest( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_southwest");
    move_char( ch, DIR_SOUTHWEST );
    return;
}

void do_west( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_west");
    move_char( ch, DIR_WEST );
    return;
}

void do_up( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_up");
    move_char( ch, DIR_UP );
    return;
}

void do_down( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_down");
    move_char( ch, DIR_DOWN );
    return;
}

// Use find_exit if you only need to get an exit, no door
// required.  This should _not_ tell the character anything;
// it is an internal function.
int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA 	*pexit;
    int        	 door;

	 if ( !str_prefix( arg, "north" ) ) door = 0;
    else if ( !str_prefix( arg, "east"  ) ) door = 1;
    else if ( !str_prefix( arg, "south" ) ) door = 2;
    else if ( !str_prefix( arg, "west"  ) ) door = 3;
    else if ( !str_prefix( arg, "up"    ) ) door = 4;
    else if ( !str_prefix( arg, "down"  ) ) door = 5;
    else if ( !str_prefix( arg, "northwest" ) ) door = 6;
    else if ( !str_prefix( arg, "southwest" ) ) door = 7;
    else if ( !str_prefix( arg, "northeast" ) ) door = 8;
    else if ( !str_prefix( arg, "southeast" ) ) door = 9;
    else if ( !str_cmp( arg, "nw" ) ) door = 6;
    else if ( !str_cmp( arg, "sw" ) ) door = 7;
    else if ( !str_cmp( arg, "ne" ) ) door = 8;
    else if ( !str_cmp( arg, "se" ) ) door = 9;
    else
    {
	for ( door = 0; door < MAX_DIR; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] )
		&& IS_SET( pexit->exit_info, EX_ISDOOR )
    	        && !( ch->level < LEVEL_IMMORTAL 
		      && pexit->exit_info
		      && (IS_SET( pexit->exit_info, EX_SECRET ) ||
                          IS_SET( pexit->exit_info, EX_BLOCKED ) ))
		&& pexit->keyword
		&& ( !str_cmp( arg, "door" )
			 || is_name( arg, pexit->keyword ) ) )
		return door;
	}
	return -1;
    }

    if ( !( pexit = ch->in_room->exit[door] ) )
    {
	return -1;
    }

    if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	return -1;
    }

    return door;
}

// Use find_door if you are looking for a door.
int find_exit( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA 	*pexit;
    int        	 door;

	 if ( !str_prefix( arg, "north" ) ) door = 0;
    else if ( !str_prefix( arg, "east"  ) ) door = 1;
    else if ( !str_prefix( arg, "south" ) ) door = 2;
    else if ( !str_prefix( arg, "west"  ) ) door = 3;
    else if ( !str_prefix( arg, "up"    ) ) door = 4;
    else if ( !str_prefix( arg, "down"  ) ) door = 5;
    else if ( !str_prefix( arg, "northwest" ) ) door = 6;
    else if ( !str_prefix( arg, "southwest" ) ) door = 7;
    else if ( !str_prefix( arg, "northeast" ) ) door = 8;
    else if ( !str_prefix( arg, "southeast" ) ) door = 9;
    else if ( !str_cmp( arg, "nw" ) ) door = 6;
    else if ( !str_cmp( arg, "sw" ) ) door = 7;
    else if ( !str_cmp( arg, "ne" ) ) door = 8;
    else if ( !str_cmp( arg, "se" ) ) door = 9;
    else
    {
	for ( door = 0; door < MAX_DIR; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] )
		&& IS_SET( pexit->exit_info, EX_ISDOOR )
    	        && !( ch->level < LEVEL_IMMORTAL 
		      && pexit->exit_info
		      && (IS_SET( pexit->exit_info, EX_SECRET ) ||
                          IS_SET( pexit->exit_info, EX_BLOCKED )))
		&& pexit->keyword
		&& ( !str_cmp( arg, "door" )
			 || is_name( arg, pexit->keyword ) ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !( pexit = ch->in_room->exit[door] ) )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    return door;
}

void do_open( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg  [ MAX_INPUT_LENGTH ];
    char      arg1 [ MAX_INPUT_LENGTH ];
    char     *rest;
    int       door;

    last_fun( "do_open");

    rest = one_argument( argument, arg );
    one_argument( rest, arg1 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( !str_cmp(arg, "door") && arg1 && strlen( arg1 ) > 0 )
	door = find_door( ch, arg1 );
    else
	door = find_door( ch, arg );

    if ( door >= 0 && !( ch->level < LEVEL_IMMORTAL 
	     && ch->in_room->exit[door] && ch->in_room->exit[door]->exit_info
	     && IS_SET( ch->in_room->exit[door]->exit_info, EX_SECRET ) ) )
    {
	/* 'open door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

        if (ch->fly_level > 0) {
            send_to_char("You see no doors this high up!\n\r", ch);
            return;
        }
	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )  )
	    { send_to_char( "It's already open.\n\r",     ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED )  )
	    { send_to_char( "It's locked.\n\r",           ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	act( "$n&n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
            REMOVE_BIT( pexit_rev->exit_info, EX_SECRET );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'open portal' */
	if ( obj->item_type == TYPE_PORTAL )
	{
	    if ( !IS_SET( obj->value[3], PORTAL_CLOSEABLE ) )
		{ send_to_char( "You can't do that.\n\r",      ch ); return; }
	    if ( !IS_SET( obj->value[3], PORTAL_CLOSED )    )
		{ send_to_char( "It's already open.\n\r",      ch ); return; }
	    if (  IS_SET( obj->value[3], PORTAL_LOCKED )    )
		{ send_to_char( "It's locked.\n\r",            ch ); return; }

	    REMOVE_BIT( obj->value[3], PORTAL_CLOSED );
	    send_to_char( "Ok.\n\r", ch );
	    act( "$n&n opens $p&n.", ch, obj, NULL, TO_ROOM );
	    return;
        }

	/* 'open object' */
	if ( obj->item_type != TYPE_CONTAINER &&
		 obj->item_type != TYPE_QUIVER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n&n opens $p&n.", ch, obj, NULL, TO_ROOM );
        if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_OPEN ))
        {
          setoff_trap( ch, obj );
          if( ch->deleted || ch->position == POS_DEAD )
             return;
        }
 	return;
    }

    send_to_char( "Open what?\n\r", ch );
    return;
}

void do_close( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg  [ MAX_INPUT_LENGTH ];
    char      arg1 [ MAX_INPUT_LENGTH ];
    char     *rest;
    int       door;

    last_fun( "do_close");

    rest = one_argument( argument, arg );
    one_argument( rest, arg1 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( !str_cmp(arg, "door") && arg1 && strlen( arg1 ) > 0 )
	door = find_door( ch, arg1 );
    else
	door = find_door( ch, arg );

    if ( door >= 0 )
    {
	/* 'close door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

        if(ch->fly_level > 0) {
            send_to_char("You can't reach that from up here!\n\r", ch);
            return;
        }
    	if ( ch->position == POS_FIGHTING )
    	{
	    send_to_char( "You can't close doors while fighting!.\n\r", ch );
	    return;
    	}

	pexit	= ch->in_room->exit[door];
	if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "It's already closed.\n\r",    ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	{
	    act( "The $d has been bashed open and cannot be closed.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}

	SET_BIT( pexit->exit_info, EX_CLOSED );
	act( "$n&n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'close portal' */
	if ( obj->item_type == TYPE_PORTAL )
	{
	    if (  IS_SET( obj->value[3], PORTAL_CLOSED )    )
		{ send_to_char( "It's already closed.\n\r",    ch ); return; }
	    if ( !IS_SET( obj->value[3], PORTAL_CLOSEABLE ) )
		{ send_to_char( "You can't do that.\n\r",      ch ); return; }

	    SET_BIT( obj->value[3], PORTAL_CLOSED );
	    send_to_char( "Ok.\n\r", ch );
	    act( "$n&n closes $p&n.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'close object' */
	if ( obj->item_type != TYPE_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if (  IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT( obj->value[1], CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n&n closes $p&n.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



OBJ_DATA* has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return obj;
    }

    return NULL;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    last_fun( "do_lock");

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 && !IS_IMMORTAL( ch ))
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key ) && !IS_IMMORTAL( ch ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'lock portal' */
	if ( obj->item_type == TYPE_PORTAL )
	{
	    if ( !IS_SET( obj->value[3], PORTAL_CLOSED ) )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[4] < 0 )
		{ send_to_char( "It can't be locked.\n\r",     ch ); return; }
	    if ( !has_key( ch, obj->value[4] ) )
		{ send_to_char( "You lack the key.\n\r",       ch ); return; }
	    if (  IS_SET( obj->value[3], PORTAL_LOCKED ) )
		{ send_to_char( "It's already locked.\n\r",    ch ); return; }

	    SET_BIT( obj->value[3], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n&n locks $p&n.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'lock object' */
	if ( obj->item_type != TYPE_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 && !IS_IMMORTAL( ch ) )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) && !IS_IMMORTAL( ch ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n locks $p&n.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    char      arg1 [ MAX_INPUT_LENGTH ];
    char     *rest;
    int       door;

    last_fun( "do_unlock");

    rest = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    one_argument( rest, arg1 );
    if ( !str_cmp(arg, "door") && arg1 )
	door = find_door( ch, arg1 );
    else
	door = find_door( ch, arg );

    if ( door >= 0 && !( ch->level < LEVEL_IMMORTAL 
	     && ch->in_room->exit[door] && ch->in_room->exit[door]->exit_info
	     && IS_SET( ch->in_room->exit[door]->exit_info, EX_SECRET ) ) )
    {
	/* 'unlock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key ) && !IS_IMMORTAL( ch ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	if ( IS_SET( pexit->exit_info, EX_EAT_KEY ) )
	{
	    extract_obj( has_key( ch, pexit->key ) );
	    act( "The $d eats the key!", ch, NULL, pexit->keyword, TO_CHAR );
	    act( "The $d eats the key!", ch, NULL, pexit->keyword, TO_ROOM );
	}

	/* unlock the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'unlock portal' */
	if ( obj->item_type == TYPE_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSED ) )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[4] < 0 )
		{ send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	    if ( !has_key( ch, obj->value[4] ) )
		{ send_to_char( "You lack the key.\n\r",       ch ); return; }
	    if ( !IS_SET( obj->value[3], PORTAL_LOCKED ) )
		{ send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	    REMOVE_BIT( obj->value[3], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n&n unlocks $p&n.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'unlock object' */
	if ( obj->item_type != TYPE_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n unlocks $p&n.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *gch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        door;

    last_fun( "do_pick");

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skills_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( IS_NPC( gch ) && IS_AWAKE( gch ) && ch->level + 5 < gch->level )
	{
	    act( "$N&n is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    /* Check skill roll for player-char, make sure mob isn't charmed */
    if ( (  !IS_NPC( ch )
	  && number_percent( ) > ch->pcdata->skl_lrn[gsn_pick_lock] )
	|| ( IS_NPC( ch ) && is_affected( ch, AFF_CHARM ) ) )
    {
        skill_practice( ch, gsn_pick_lock );
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )    )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED )    )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_PICKPROOF ) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'pick portal' */
	if ( obj->item_type == TYPE_PORTAL )
	{
	    if ( !IS_SET( obj->value[3], PORTAL_CLOSED )    )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[4] < 0 )
		{ send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	    if ( !IS_SET( obj->value[3], PORTAL_LOCKED )    )
		{ send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	    if (  IS_SET( obj->value[3], PORTAL_PICKPROOF ) )
		{ send_to_char( "You failed.\n\r",             ch ); return; }

	    REMOVE_BIT( obj->value[3], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n&n picks $p&n.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'pick object' */
	if ( obj->item_type != TYPE_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( obj->value[1], CONT_PICKPROOF ) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n&n picks $p&n.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_stand");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You're paralyzed!\n\r", ch );
        return;
    }

    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    {
        REMOVE_BIT( ch->act, PLR_MEMORIZING );
        send_to_char( "You abandon your studies.\n\r", ch );
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( is_affected( ch, AFF_SLEEP ) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n&n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_SITTING:
    case POS_KNEELING:
    case POS_RECLINING:
    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n&n stands up.", ch, NULL, NULL, TO_ROOM );
        if( ch->fighting )
            ch->position = POS_FIGHTING;
        else
	    ch->position = POS_STANDING;
	break;

    case POS_FIGHTING:
    case POS_STANDING:
	send_to_char( "You are already standing.\n\r",  ch );
	break;
    }

    return;
}

void do_rest( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_rest");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You're paralyzed!\n\r", ch );
        return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r",  ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r",   ch );
	break;

    case POS_FIGHTING:
	send_to_char( "Not while you're fighting!\n\r", ch );
	break;

    case POS_SITTING:
    case POS_KNEELING:
    case POS_RECLINING:
    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n&n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;
    }

    return;
}

void do_recline( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_recline");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You're paralyzed!\n\r", ch );
        return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",  ch );
	break;

    case POS_RECLINING:
	send_to_char( "You are already reclining.\n\r",   ch );
	break;

    case POS_FIGHTING:
	send_to_char( "Not while you're fighting!\n\r", ch );
	break;

    case POS_SITTING:
    case POS_KNEELING:
    case POS_RESTING:
    case POS_STANDING:
    	if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    	{
            REMOVE_BIT( ch->act, PLR_MEMORIZING );
            send_to_char( "You abandon your studies.\n\r", ch );
    	}
	send_to_char( "You recline.\n\r", ch );
	act( "$n&n reclines.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RECLINING;
	break;
    }

    return;
}

void do_sit( CHAR_DATA *ch, char *argument )
{
    last_fun( "o_sit");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You're paralyzed!\n\r", ch );
        return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You can't do that while you are asleep.\n\r",  ch );
	break;

    case POS_RESTING:
	send_to_char( "You sit up.\n\r",   ch );
	act( "$n&n sits up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;

    case POS_KNEELING:
    case POS_RECLINING:
	send_to_char( "You sit up.\n\r", ch );
	act( "$n&n sits up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;
    case POS_FIGHTING:
    case POS_STANDING:
	send_to_char( "You sit down.\n\r", ch );
	act( "$n&n sits down.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	break;
    }

    return;    
}

void do_kneel( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_kneel");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You're paralyzed!\n\r", ch );
        return;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You can't do that while you are asleep.\n\r",  ch );
	break;

    case POS_RECLINING:
    case POS_RESTING:
    case POS_SITTING:
    	if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    	{
            REMOVE_BIT( ch->act, PLR_MEMORIZING );
            send_to_char( "You abandon your studies.\n\r", ch );
    	}
	send_to_char( "You swing up to your knees.\n\r",   ch );
	act( "$n&n swings up to $s knees.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_KNEELING;
	break;

    case POS_FIGHTING:
    case POS_STANDING:
	send_to_char( "You kneel.\n\r", ch );
	act( "$n&n drops to $s knees.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_KNEELING;
	break;
    }

    return;    
}

void do_sleep( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_sleep");

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r",  ch );
	break;

    case POS_RECLINING:
    case POS_SITTING:
    case POS_KNEELING:
    case POS_RESTING:
    case POS_STANDING: 

    	if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    	{
            REMOVE_BIT( ch->act, PLR_MEMORIZING );
            send_to_char( "You abandon your studies.\n\r", ch );
    	}
	send_to_char( "You sleep.\n\r", ch );
	act( "$n&n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "Not while you're fighting!\n\r", ch );
	break;
    }

    return;
}

void do_wake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];


    last_fun( "do_wake");
    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->mana > 0 )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE( ch ) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( !( victim = get_char_room( ch, arg ) ) )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE( victim ) )
	{ act( "$N&n is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( is_affected( victim, AFF_SLEEP ) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR ); return; }

    victim->position = POS_RESTING;
    act( "You wake $M.",  ch, NULL, victim, TO_CHAR );
    act( "$n&n wakes you.", ch, NULL, victim, TO_VICT );
    return;
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    last_fun( "do_sneak");

    /* Don't allow charmed mobs to do this, check player's skill */
    if ( ( ch->level < skills_table[gsn_sneak].skill_level[ch->class] ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    if( !strcmp( argument, "off" ) )
    {
        if( !has_affect( ch, gsn_sneak, 0, 0 ) )
        {
            send_to_char( "You're not sneaking.\n\r", ch );
        }
        else
        {
            send_to_char( "You stop sneaking around.\n\r", ch );
            affect_strip( ch, gsn_sneak, 0, 0 );
        }
        return;
    }

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak, 0, 0 );

    /* Check skill knowledge when moving only. */
    af = create_affect( gsn_sneak, 0, 0, -1, APPLY_NONE, 0, AFF_SNEAK );
    affect_to_char( ch, &af );

    skill_practice( ch, gsn_sneak );
    WAIT_STATE( ch, 10 );
    return;
}

void do_hide( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_hide");

    /* Check player's skill */
    if ( ch->level < skills_table[gsn_hide].skill_level[ch->class] ||
         (argument[0] != '\0') )
    {
       do_hide_item( ch, argument );
       return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    if (ch->position <= POS_SLEEPING) return;
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( is_affected( ch, AFF_HIDE ) )
	REMOVE_AFF_BIT( ch, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->skl_lrn[gsn_hide] )
	SET_AFF_BIT( ch, AFF_HIDE );

    skill_practice( ch, gsn_hide );
    WAIT_STATE( ch, 12 );

    return;
}

/*
 * Contributed by Alander.
 *
 * This function no longer needs to exist, so it's a null function.
 * The going vis stuff is handled by interp.c
 * since this function is flagged snapvis and unhide.
 * -- Veygoth
 * This function needs to get re-written so players can make themselves
 *   visible if they wanna be. - Lohrr
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    return;
}

void do_train( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_train");
    send_to_char( "You do not train on this MUD.  Go out and use your skills!\n\r", ch );
    return;
}

void do_chameleon ( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_chameleon");
    if ( !IS_NPC( ch )
	&& ch->level < skills_table[gsn_chameleon].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to blend in with your surroundings.\n\r", ch);

    if ( is_affected( ch, AFF_HIDE ) )
        REMOVE_AFF_BIT( ch, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->skl_lrn[gsn_chameleon] )
        SET_AFF_BIT( ch, AFF_HIDE );

    skill_practice( ch, gsn_chameleon );

    return;
}

/*
===========================================================================
This snippet was written by Erwin S. Andreasen, erwin@pip.dknet.dk. You may 
use this code freely, as long as you retain my name in all of the files. You
also have to mail me telling that you are using it.

All my snippets are publically available at: http://pip.dknet.dk/~pip1773/

The classic SCAN command, shows the mobs surrounding the character.

===========================================================================
*/
int scan_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *buf,
		 int distance, int dir )
{
    CHAR_DATA *target		= room->people;
    int        number_found	= 0;
    int        visibility	= 0;
    char       distance_msg[MAX_STRING_LENGTH];

    if (dir < 0 || dir >= MAX_DIR)
    {
	bug( "scan_room: direction %d out of bounds!", dir );
	send_to_char( "Bug while scanning, direction out of bounds!\n\r", ch );
	return 0;
    }

    // Not going to find anything in a room that is unscannable - Veygoth
    if( IS_ROOM( room, ROOM_NO_SCAN ))
      return 0;

    switch ( distance )
    {
	case 1:
	    sprintf(distance_msg, "&n who is close by to the " );
	    break;
	case 2:
	    sprintf(distance_msg, "&n who is not far off to the " );
	    break;
	case 3:
	    sprintf(distance_msg, "&n who is a brief walk away to the " );
	    break;
	default:
	    sprintf(distance_msg, "&n who is an unknown distance to the " );
	    break;
    }

    while ( target )
    {
        if (target->deleted) continue;
        if (ch->fly_level == target->fly_level) {
	visibility = how_see(ch, target);	
        switch ( visibility )
        {
	case SEE_SENSE_HIDDEN:
	case SEE_INVISIBLE:
	case SEE_TOO_DARK:
	default:
	    break;
	case SEE_VISIBLE:
            strcat ( buf, PERS( target, ch ) );
            strcat ( buf, distance_msg );
            strcat ( buf, dir_name[dir] );
            strcat ( buf, ".&n\n\r");
            number_found++;
	    break;
	case SEE_SENSE_INFRA:
	    strcat ( buf, "&+rYou sense a being within the darkness" );
            strcat ( buf, distance_msg );
            strcat ( buf, dir_name[dir] );
            strcat ( buf, ".&n\n\r");
            number_found++;
	    break;
        }
        } //end if(ch->fly_level == target->fly_level)
        target = target->next_in_room;
    }
    return number_found;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *room;
    char             buf[ MAX_STRING_LENGTH ];
    int              dir;
    int              distance;
    int              fly_level;

    last_fun( "do_scan");

    send_to_char( "You scan the area.\n\r", ch );
    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        room = ch->in_room;

        for ( distance = 1 ; distance <= 3; distance++ )
        {
            pexit = room->exit[dir];
            if ( !pexit
		|| !pexit->to_room
		|| IS_SET( pexit->exit_info, EX_CLOSED )
		|| ( (  IS_SET( pexit->exit_info, EX_SECRET)
		     || IS_SET( pexit->exit_info, EX_BLOCKED)
                     || IS_SET( pexit->exit_info, EX_WALLED) )
			 && !IS_IMMORTAL(ch) ) )
                break;

 	    buf[0] = '\0';
            if ( scan_room( ch, pexit->to_room, buf, distance, dir ) )
                send_to_char ( buf, ch );

            room = pexit->to_room;
        }
    }
    for( fly_level = 0; fly_level <= MAX_FLY; fly_level++) {
        if(ch->fly_level == fly_level)
            continue;
        if ( scan_this_room(ch, ch->in_room, buf, fly_level) )
            send_to_char( buf, ch );
    }
    return;

}

void do_heighten ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    last_fun( "do_heighten");
    if ( !IS_NPC( ch )
	&& ch->level < skills_table[gsn_heighten].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( has_affect( ch, gsn_heighten, 0, 0 ) )
        return;

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->skl_lrn[gsn_heighten] )
    {
	af.skill     = gsn_heighten;
        af.spell     = 0;
        af.song      = 0;
	af.duration  = 24;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	set_bitvector( &af, AFF_DETECT_INVIS );
	affect_to_char( ch, &af );

	set_bitvector( &af, AFF_SENSE_LIFE );
	affect_to_char( ch, &af );
	
	set_bitvector( &af, AFF_INFRARED );
	affect_to_char( ch, &af );
	
	send_to_char( "Your senses are heightened.\n\r", ch );
    }

    skill_practice( ch, gsn_heighten );

    return;

}

void do_shadow ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    last_fun( "do_shadow");
    if ( !IS_NPC( ch )
	&& ch->level < skills_table[gsn_shadow].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to move in the shadows.\n\r", ch );
    affect_strip( ch, gsn_shadow, 0, 0 );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->skl_lrn[gsn_shadow] )
    {
	af.skill     = gsn_shadow;
        af.spell     = 0;
        af.song      = 0;
	af.duration  = ch->level;
	af.modifier  = APPLY_NONE;
	af.location  = 0;
	set_bitvector( &af, AFF_SNEAK );
	affect_to_char( ch, &af );
    }

    skill_practice( ch, gsn_shadow );
    WAIT_STATE( ch, 10 );

    return;

}

void do_shift ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    AREA_DATA *area;

    if (ch->race != race_githyanki && !IS_IMMORTAL( ch ) ) {
        send_to_char( "You lack that abillity!\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Shift to where?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "You can't break off your fight.\n\r", ch );
        return;
    }
    if( ( has_innate_timer( ch, INNATE_SHIFT_ASTRAL )
          && !str_prefix( arg, "astral" ) )
     || ( has_innate_timer( ch, INNATE_SHIFT_PRIME ) 
          && !str_prefix( arg, "prime" ) ) )
    {
        send_to_char( "You need to rest a bit first.\n\r", ch);
        return;
    }

    area = ch->in_room->area;
    spell_plane_shift( spl_plane_shift, ch->level, ch, (void *)arg );
    // if it failed, don't lag or add a timer
    if ( area == ch->in_room->area ) return;
    if (!IS_IMMORTAL( ch ) )
    {
        if( !str_prefix( arg, "astral" ) )
            add_innate_timer( ch, INNATE_SHIFT_ASTRAL, 8 );
        else if( !str_prefix( arg, "prime" ) )
            add_innate_timer( ch, INNATE_SHIFT_PRIME, 8 );
    }
    WAIT_STATE( ch, 14 );

}

/*
 * Bash code by Thelonius for EnvyMud (originally bash_door)
 * Damage modified using Morpheus's code
 * Message for bashproof doors by that wacky guy Kahn
 */
void do_doorbash( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        door;
    ROOM_INDEX_DATA *to_room;

    last_fun( "do_doorbash");

    if ( IS_NPC( ch )
	|| ( ch->level < skills_table[gsn_doorbash].skill_level[ch->class]
	   && !IS_SET( race_table[ch->race].race_abilities, RACE_DOORBASH ) ) )
    {
	send_to_char( "You don't feel massive enough!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Doorbash what?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "You can't break off your fight.\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	int              chance;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "Calm down.  It is already open.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, skills_table[gsn_doorbash].beats );

	if ( IS_NPC( ch ) )
	  chance = 0;
	else if (ch->level < skills_table[gsn_doorbash].skill_level[ch->class] )
	  chance = 20;
	else
	  chance = ch->pcdata->skl_lrn[gsn_doorbash]/2;

	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	    chance /= 2;

	if ( IS_SET( pexit->exit_info, EX_BASHPROOF )
            && !IS_IMMORTAL(ch) )
	{
	    act( "WHAAAAM!!!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "WHAAAAM!!!  $n&n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( get_max_hit( ch ) /  20 ), gsn_doorbash, WEAR_NONE, DAM_BASH );
	    return;
	}

	if ( ( get_curr_str( ch ) >= 20 )
	    && number_percent( ) <
	        ( chance + 4 * ( get_curr_str( ch ) - 20 ) ) )
	{
	    /* Success */

	    REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	    if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	        REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	    
	    SET_BIT( pexit->exit_info, EX_BASHED );

	    act( "Crash!  You bashed open the $d!",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "$n&n bashes open the $d!",
		ch, NULL, pexit->keyword, TO_ROOM );

	    damage( ch, ch, ( get_max_hit( ch ) / 30 ), gsn_doorbash, WEAR_NONE, DAM_BASH );

	    /* Bash through the other side */
	    if (   ( to_room   = pexit->to_room               )
		&& ( pexit_rev = to_room->exit[rev_dir[door]] )
		&& pexit_rev->to_room == ch->in_room        )
	    {
		CHAR_DATA *rch;

		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		if ( IS_SET( pexit_rev->exit_info, EX_LOCKED ) )
		    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );

		SET_BIT( pexit_rev->exit_info, EX_BASHED );

		for ( rch = to_room->people; rch; rch = rch->next_in_room )
		{
		    if ( rch->deleted )
		        continue;
		    act( "The $d crashes open!",
			rch, NULL, pexit_rev->keyword, TO_CHAR );
		}

    		// Have any aggro mobs on the other side come after the player -- Veygoth
    		for ( gch = to_room->people; gch; gch = gch->next_in_room )
    		{
        	  if ( !gch->deleted
            		&& gch != ch
            		&& ( IS_NPC( gch ) && !is_affected( gch, AFF_CHARM ) )
            		&& is_aggro( gch, ch )
            		&& IS_AWAKE( gch )
            		&& can_see( gch, ch )
            		&& !gch->fighting )
            		{
            		  start_hating( gch, ch );
            		  start_hunting( gch, ch );
            		}
    		}

	    }
	}
	else
	{
	    /* Failure */
	    
	    act( "OW!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "$n&n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( get_max_hit( ch ) / 10 ), gsn_doorbash, WEAR_NONE, DAM_BASH );
	}
    }

    /*
     * Check for "guards"... anyone bashing a door is considered as
     * a potential aggressor, and there's a 25% chance that mobs
     * will do unto before being done unto.
     * But first...let's make sure ch ain't dead?  That'd be a pain.
     */

    if ( ch->deleted || ch->hit <= 1 )
        return;

    skill_practice( ch, gsn_doorbash );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !gch->deleted
	    && gch != ch
            && IS_SET( gch->act, ACT_PROTECTOR )
	    && ( IS_NPC( gch ) && !is_affected( gch, AFF_CHARM ) )
	    && IS_AWAKE( gch )
	    && can_see( gch, ch )
	    && !gch->fighting
	    && number_bits( 2 ) == 0 )
            {
              act( "$n&n is very unhappy about you trying to destroy the door.", gch, NULL, ch, TO_VICT );
  	      kill_ch( gch, ch );
            }
    }

    return;

}

/* Capture skill by Binky for EnvyMud */
void do_capture( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    OBJ_DATA    *rope;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_capture");

    /* Check player's level and class, allow mobs to do this too */
    if ( ( ch->level < skills_table[gsn_capture].skill_level[ch->class] ) )
    {
        send_to_char( "You couldn't capture a dead rat.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !( victim = ch->fighting ) )
	{
	    send_to_char( "Capture whom?\n\r", ch );
	    return;
	}
    }
    else				/* argument supplied */
    {
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
 	}
    }
    if ( ch->fighting && ch->fighting != victim )
    {		
        send_to_char( "Take care of the person you are fighting first!\n\r", ch );
        return;
    }                             
    if( !IS_IMMORTAL( ch ) )
        WAIT_STATE( ch, skills_table[gsn_capture].beats );

    rope = get_eq_char( ch, WEAR_HAND );
    if( !rope || rope->item_type != TYPE_ROPE )
    {
        rope = get_eq_char( ch, WEAR_HAND_2 );
        if( !rope || rope->item_type != TYPE_ROPE )
        {
            rope = get_eq_char( ch, WEAR_HAND_3 );
            if( !rope || rope->item_type != TYPE_ROPE )
            {
                rope = get_eq_char( ch, WEAR_HAND_4 );
                if( !rope || rope->item_type != TYPE_ROPE )
                    rope = NULL;
            }
        }
    }
    if( !rope )
    {
        send_to_char( "You must have some rope to tie someone up!\n\r", ch );
        return;
    }
    else
        extract_obj( rope );

    /* only appropriately skilled PCs and uncharmed mobs */
    if (   (  IS_NPC( ch ) && !is_affected( ch, AFF_CHARM ) )
        || ( !IS_NPC( ch ) && number_percent( ) <
                       ch->pcdata->skl_lrn[gsn_capture] / 4 ) )
    {
	affect_strip( victim, gsn_capture, 0, 0 );  

	af.skill     = gsn_capture;
        af.spell     = 0;
        af.song      = 0;
	af.duration  = 3 + ( (ch->level ) / 8 );
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	set_bitvector( &af, AFF_BOUND );

	affect_to_char( victim, &af );

	act( "You have captured $M!", ch, NULL, victim, TO_CHAR    );
	act( "$n&n has captured you!",  ch, NULL, victim, TO_VICT    );
	act( "$n&n has captured $N&n.",   ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	act( "You failed to capture $M.  Uh oh!",
	    ch, NULL, victim, TO_CHAR    );
	act( "$n&n tried to capture you!  Get $m!",
	    ch, NULL, victim, TO_VICT    );
	act( "$n&n attempted to capture $N&n, but failed!",
	    ch, NULL, victim, TO_NOTVICT );
    }

    /* go for the one who wanted to fight :) */
    if ( IS_NPC( ch ) && is_affected( ch, AFF_CHARM ) && !victim->fighting )
    {
	kill_ch( victim, ch->master );
    }
    else if ( !victim->fighting )
    {
	kill_ch( victim, ch );
    }

    return;
}



/* Untangle by Thelonius for EnvyMud */
void do_untangle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    char         arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_untangle");

    if ( !IS_NPC( ch )
	&& ch->level < skills_table[gsn_untangle].skill_level[ch->class] )
    {
	send_to_char( "You aren't nimble enough.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	victim = ch;
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    }

    if ( !has_affect( victim, gsn_capture, 0, 0 ) )
	return;

    if ( ( IS_NPC( ch ) && !is_affected( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch ) 
	    && number_percent( ) < ch->pcdata->skl_lrn[gsn_untangle] ) )
    {
	affect_strip( victim, gsn_capture, 0, 0 );

	if ( victim != ch )
        {
	    act( "You untangle $N&n.",  ch, NULL, victim, TO_CHAR    );
	    act( "$n&n untangles you.", ch, NULL, victim, TO_VICT    );
	    act( "$n&n untangles $n&n.",  ch, NULL, victim, TO_NOTVICT );
        }
	else
        {
	    send_to_char( "You untangle yourself.\n\r", ch );
	    act( "$n&n untangles $mself.", ch, NULL, NULL, TO_ROOM );
        }

        skill_practice( ch, gsn_untangle );

	return;
    }
}



/* 
 * Random room generation function.
 */
ROOM_INDEX_DATA  *get_random_room( )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for ( ; ; )
    {
        pRoomIndex = get_room_index( number_range( 0, 65535 ) );
        if ( pRoomIndex )
            if (   !IS_ROOM( pRoomIndex, ROOM_PRIVATE  )
                && !IS_ROOM( pRoomIndex, ROOM_SOLITARY ) )
            break;
    }

    return pRoomIndex;
}

/*
 * Returns a random room on one of the maps
 *
 * Argument: 0 = any map
 *           1 = Surface map 1
 *           2 = Surface map 2
 *           3 = Underdark map 1
 *           4 = Underdark map 2
 *           5 = Underdark map 3
 *
 * - Lithos
 */
ROOM_INDEX_DATA  *get_random_map_room( int map )
{
    ROOM_INDEX_DATA *pRoomIndex;
    int key, rval;
    
    if ( map > 5 ) {
        bug ( "get_random_map_room: map index out of range", 0);
        return NULL;
    }
    if ( !map )
        key = number_range( 1, 5 );
    else
        key = map;
    switch ( key ) {
    case 1:
        rval = number_range( SURFACE_MAP1_START_VNUM, SURFACE_MAP1_END_VNUM ); 
        break;
    case 2:
        rval = number_range( SURFACE_MAP2_START_VNUM, SURFACE_MAP2_END_VNUM );
        break;
    case 3:
        rval = number_range( UD_MAP1_START_VNUM, UD_MAP1_END_VNUM );
        break;
    case 4:
        rval = number_range( UD_MAP2_START_VNUM, UD_MAP2_END_VNUM );
        break;
    case 5:
        rval = number_range( UD_MAP3_START_VNUM, UD_MAP3_END_VNUM );
        break;
    default:
        break;
    }
    pRoomIndex = get_room_index( rval );
    if( !pRoomIndex ||
      pRoomIndex->extra_sector_info == 92 ||
      pRoomIndex->extra_sector_info == 101 ||
      pRoomIndex->extra_sector_info == 102 ||
      pRoomIndex->extra_sector_info == 116 ||
      pRoomIndex->extra_sector_info == 130 ||
      pRoomIndex->extra_sector_info == 131 ||
      pRoomIndex->extra_sector_info == 132 ||
      pRoomIndex->extra_sector_info == 136 ||
      pRoomIndex->extra_sector_info == 137 ||
      IS_ROOM( pRoomIndex, ROOM_PRIVATE  ) ||
      IS_ROOM( pRoomIndex, ROOM_SOLITARY ) ||
      IS_ROOM( pRoomIndex, ROOM_NO_TELEPORT) ) 
        pRoomIndex = get_random_map_room( key );
    return pRoomIndex;
}

void do_enter( CHAR_DATA * ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA        *portal;
    CHAR_DATA       *fch;
    bool             ship = FALSE;

    last_fun( "do_enter");

    if ( argument[0] == '\0' )
    {
	send_to_char( "Enter what?\n\r", ch );
	return;
    }

    if ( ch->fighting ) {
        send_to_char( "Not while you're fighting!\n\r", ch );
        return;
    }

    original	= ch->in_room;
    portal	= get_obj_list( ch, argument, ch->in_room->contents );

    if ( !portal )
    {
	act( "There is no $t in here.", ch, argument, NULL, TO_CHAR );
	return;
    }

    if ( portal->item_type != TYPE_PORTAL )
    {
        if( portal->item_type != TYPE_SHIP && portal->item_type != TYPE_VEHICLE)
        {
          send_to_char( "That's not a portal.\n\r", ch );
          return;
        }
        else
        {
          ship = TRUE;
        }
    }

    if( !ship )
    {
      if( IS_SET( portal->value[3], PORTAL_CLOSED ) )
      {
	send_to_char( "That portal is closed.\n\r", ch );
	return;
      }

      if ( portal->value[2] == 0 )
      {
	act( "$p&n has no power left.", ch, portal, NULL, TO_CHAR );
	return;
      }

      if ( !IS_SET( portal->value[3], PORTAL_NO_CURSED )
  	&& ( is_affected( ch, AFF_CURSE )
      /* Govno removed.  If room has a portal in it, no_recall shouldn't matter.
	    || IS_ROOM( original, ROOM_NO_RECALL ) Govno*/ ) ) 
      {
  	send_to_char( "God has forsaken you.\n\r", ch );
  	return;
      }
      location = get_room_index( portal->value[0] );

      if ( IS_SET( portal->value[3], PORTAL_BUGGY ) && number_percent() < 5 )
	location = get_random_room();

      if ( IS_SET( portal->value[3], PORTAL_RANDOM ) )
      {
	location		= get_random_room();
	portal->value[0]	= location->vnum;
      }

      if ( !location )
      {
        send_to_char( "The target room for this portal does not exist.\n\r", ch );
        return;
      }

      if( location == original )
      {
	act( "$p&n doesn't seem to go anywhere.", ch, portal, NULL, TO_CHAR );
	return;
      }

    }
    else
    {
      location = get_room_index( portal->value[1] );
      if( !location )
      {
        send_to_char( "That boat is broken.  You may not board it.\n\r", ch );
        return;
      }
    }

    if( room_is_private( location ) )
    {
        send_to_char( "There is no room for you on the other side.\n\r", ch );
        return;
    }

    if( !ship )
    {
    act( "$n&n steps into $p&n.", ch, portal, NULL, TO_ROOM );
    if ( IS_SET(portal->value[3], PORTAL_RANDOM )
	|| IS_SET(portal->value[3], PORTAL_BUGGY ) )
	act( "You walk through $p&n and find yourself somewhere else...",
	    ch, portal, NULL, TO_CHAR );
    else
	act( "You enter $p&n.", ch, portal, NULL, TO_CHAR );
    }
    else
    {
      act( "$n&n boards $p&n.", ch, portal, NULL, TO_ROOM );
      act( "You board $p&n.", ch, portal, NULL, TO_CHAR );
    } 

    char_from_room( ch );
    char_to_room( ch, location );
    last_fun( "do_enter");

    if( !ship )
    {
      if ( IS_SET( portal->value[3], PORTAL_RANDOM )
	|| IS_SET( portal->value[3], PORTAL_BUGGY ) )
	act( "$n&n has arrived.", ch, portal, NULL, TO_ROOM );
      else
	act( "$n&n steps out of $p&n.", ch, portal, NULL, TO_ROOM );
    }
    else
    {
	act( "$n&n has boarded.", ch, portal, NULL, TO_ROOM );
    }
      
    do_look( ch, "auto" );
    last_fun( "do_enter");

    if( !ship )
    {
      if ( portal->value[2] > 0 )	/*
				 * This way i prevent an underflow 
				 */
	portal->value[2]--;

      if ( portal->value[2] == 0 )/*
				 * If there are no more charges; remove 
				 */
      {
	act( "$p&n fades out of existence.", ch, portal, NULL, TO_CHAR );
	act( "$p&n fades out of existence.", ch, portal, NULL, TO_ROOM );
	extract_obj( portal );
	return;
      }

      if ( IS_SET( portal->value[3], PORTAL_GO_WITH ) )
      {
	obj_from_room( portal );
	obj_to_room( portal, location );
        last_fun( "do_enter");
      }
    }

    for ( fch = original->people; fch; fch = fch->next_in_room )
    {
	if ( fch->deleted
	    || !is_affected( fch, AFF_CHARM )
	    || fch->master != ch )
	    continue;

	if ( fch->position < POS_STANDING )
        {
	    do_stand( fch, "" );
            last_fun( "do_enter");
        }

	if ( fch->position == POS_STANDING && fch->wait == 0 )
	{
	    act( "You follow $N&n.", fch, NULL, ch, TO_CHAR );
	    do_enter( fch, argument );
	}
    }

    return;
}



void do_mount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    last_fun( "do_mount");

    if ( !ch )
    {
        bug( "do_mount: no ch!", 0 );
        return;
    }

    if ( ch->riding )
    {
	send_to_char( "You're already mounted!\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, argument ) ) )
    {
	send_to_char( "You can't find that here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim )
	|| !IS_SET( victim->act, ACT_MOUNT ) )
    {
	send_to_char( "You can't mount that!\n\r", ch );
	return;
    }

    if ( ch->rider )
    {
        send_to_char( "You are being ridden by someone else!\n\r", ch );
        return;
    }

    if ( victim->rider )
    {
	send_to_char( "That mount already has a rider.\n\r", ch );
	return;
    }

    if ( victim->position < POS_STANDING )
    {
	send_to_char( "Your mount must be standing.\n\r", ch );
	return;
    }

    if ( victim->position == POS_FIGHTING
	|| victim->fighting )
    {
	send_to_char( "Your mount is moving around too much.\n\r", ch );
	return;
    }

    /* I up'd this since ppl were complaining. -Lohrr */
    skill_practice( ch, gsn_mount );
    skill_practice( ch, gsn_mount );
    skill_practice( ch, gsn_mount );

    WAIT_STATE( ch, skills_table[gsn_mount].beats );
    if ( IS_NPC( ch ) || IS_IMMORTAL( ch )
	|| number_percent( ) < ch->pcdata->skl_lrn[gsn_mount] )
    {
	victim->rider = ch;
	ch->riding    = victim;
	act( "You mount $N&n.", ch, NULL, victim, TO_CHAR );
	act( "$n&n skillfully mounts $N&n.", ch, NULL, victim, TO_NOTVICT );
	act( "$n&n mounts you.", ch, NULL, victim, TO_VICT );
    }
    else
    {
	act( "You unsuccessfully try to mount $N&n.", ch, NULL, victim, TO_CHAR );
	act( "$n&n unsuccessfully attempts to mount $N&n.", ch, NULL, victim, TO_NOTVICT );
	act( "$n&n tries to mount you.", ch, NULL, victim, TO_VICT );
    }

    return;
}



void do_dismount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    last_fun( "do_dismount");

    if ( !( victim = ch->riding ) )
    {
	send_to_char( "You're not mounted.\n\r", ch );
	return;	
    }

    if (ch->riding->deleted || ch->riding->in_room != ch->in_room) {
        send_to_char( "Weird! You dismount, but your mount isn't here.", ch);
        if (ch->riding->rider && ch->riding->rider == ch )
            ch->riding->rider = NULL;
        ch->riding = NULL;
        return;
    }

    WAIT_STATE( ch, skills_table[gsn_mount].beats );
    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->skl_lrn[gsn_mount] )
    {
	act( "You dismount $N&n.",            ch, NULL, victim, TO_CHAR );
	act( "$n&n skillfully dismounts $N&n.", ch, NULL, victim, TO_NOTVICT );
	act( "$n&n dismounts you.  Whew!",    ch, NULL, victim, TO_VICT );
	victim->rider = NULL;
	ch->riding    = NULL;
	ch->position  = POS_STANDING;
    }
    else
    {
	act( "You fall off while dismounting $N&n.  Ouch!", ch, NULL, victim, TO_CHAR );
	act( "$n&n falls off of $N&n while dismounting.", ch, NULL, victim, TO_NOTVICT );
	act( "$n&n falls off your back.", ch, NULL, victim, TO_VICT );
	victim->rider = NULL;
	ch->riding    = NULL;
	ch->position  = POS_RESTING;
	damage( ch, ch, 1, TYPE_UNDEFINED, WEAR_NONE, DAM_BASH );
    }

    skill_practice( ch, gsn_mount );

    return;
}

void do_buck( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_buck");

    if ( !ch->rider )
    {
        send_to_char( "There is no one riding you.\n\r", ch );
        return;
    }

    act( "$n&n bucks $N&n!",             ch, NULL, ch->rider, TO_NOTVICT );
    act( "You buck $M!",             ch, NULL, ch->rider, TO_CHAR );
    act( "$n&n bucks you from $m!",    ch, NULL, ch->rider, TO_VICT );

    ch->rider->riding   = NULL;
    ch->rider->position = POS_RESTING;
    ch->rider           = NULL;
    return;
}

void do_firstaid( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_firstaid");
    if( IS_NPC( ch ))
       return;

    if (ch->fighting || ch->position == POS_FIGHTING )
    {
        send_to_char( "You can't do that while fighting!\n\r", ch );
	return;
    }

    if( ch->pcdata->firstaid > 0 )
    {
       send_to_char( "You can only first aid once per day.\n\r", ch );
       return;
    }

    if( ch->hit < get_max_hit( ch ) )
    {
      ch->hit += dice( 3, (ch->level / 2)) + 1;
      if( ch->pcdata->skl_lrn[gsn_bandage] > 9 )
      {
          ch->hit += ch->pcdata->skl_lrn[gsn_bandage] / 8;
          skill_practice( ch, gsn_bandage );
      }
      if( ch->hit > get_max_hit( ch ) )
        ch->hit = get_max_hit( ch );
    }

    ch->pcdata->firstaid = 24;

    send_to_char( "You attempt to render first aid unto yourself.\n\r", ch );

    return;
}

void do_carve( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char *name2;
    OBJ_DATA *corpse;
    OBJ_DATA *part;
    OBJ_DATA *weap;
    int value;

    last_fun( "do_carve");

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if( arg[0] == '\0' )
    {
       send_to_char( "What do you want to carve?\n\r", ch );
       return;
    }

    if( !(corpse = get_obj_here( ch, arg )))
    {
       send_to_char( "I don't see that around here.\n\r", ch );
       return;
    }

    if( corpse->item_type != TYPE_CORPSE_PC )
    {
       send_to_char( "That's not a player corpse!\n\r", ch );
       return;
    }

    if( arg2[0] == '\0' )
    {
       send_to_char( "Parts of that corpse that are left:\n\r", ch );
       if( corpse->value[0] == 0 )
         send_to_char( "None.\n\r", ch );
       else
         send_to_char( parts_bit_name( corpse->value[0] ), ch );
       return;
    }

    // 11 is for piercing eapons
    if( !(weap = get_eq_char( ch, WEAR_HAND )) || weap->value[3] != 11)
    {
       send_to_char( "You need some sort of stabbing weapon for carving.\n\r", ch );
       return;
    }

    if( !str_cmp( arg2, "head" ))
       value = PART_HEAD;
    else if( !str_cmp( arg2, "arms" ))
       value = PART_ARMS;
    else if( !str_cmp( arg2, "legs" ))
       value = PART_LEGS;
    else if( !str_cmp( arg2, "heart" ))
       value = PART_HEART;
    else if( !str_cmp( arg2, "brains" ))
       value = PART_BRAINS;
    else if( !str_cmp( arg2, "guts" ))
       value = PART_GUTS;
    else if( !str_cmp( arg2, "hands" ))
       value = PART_HANDS;
    else if( !str_cmp( arg2, "feet" ))
       value = PART_FEET;
    else if( !str_cmp( arg2, "fingers" ))
       value = PART_FINGERS;
    else if( !str_cmp( arg2, "ears" ))
       value = PART_EAR;
    else if( !str_cmp( arg2, "eyes" ))
       value = PART_EYE;
    else if( !str_cmp( arg2, "tongue" ))
       value = PART_LONG_TONGUE;
    else if( !str_cmp( arg2, "eyestalks" ))
       value = PART_EYESTALKS;
    else if( !str_cmp( arg2, "tentacles" ))
       value = PART_TENTACLES;
    else if( !str_cmp( arg2, "fins" ))
       value = PART_FINS;
    else if( !str_cmp( arg2, "wings" ))
       value = PART_WINGS;
    else if( !str_cmp( arg2, "tail" ))
       value = PART_TAIL;
    else if( !str_cmp( arg2, "claws" ))
       value = PART_CLAWS;
    else if( !str_cmp( arg2, "fangs" ))
       value = PART_FANGS;
    else if( !str_cmp( arg2, "scales" ))
       value = PART_SCALES;
    else if( !str_cmp( arg2, "horns" ))
       value = PART_HORNS;
    else if( !str_cmp( arg2, "tusks" ))
       value = PART_TUSKS;
    else
    {
       send_to_char( "No such part.\n\r", ch );
       return;
    }

    if( !IS_SET( corpse->value[0], value ))
    {
       send_to_char( "That part has either already been removed or never existed.\n\r", ch );
       return;
    }

    REMOVE_BIT( corpse->value[0], value );
    if( number_percent( ) < ch->pcdata->skl_lrn[gsn_carve] )
    {
       send_to_char( "You managed to pry it loose!\n\r", ch );
    }
    else
    {
       send_to_char( "You mangled that part badly.\n\r", ch );
       return;
    }

    /* corpse of %s   - Lohrr*/
    name2 = one_argument( corpse->name, name );
    name2 = one_argument( name2, name );
    one_argument( name2, name );
    // This is where we would normally create the object...
    part = create_object( get_obj_index(OBJ_VNUM_SEVERED_HEAD), 0 );
    free_string( part->name );
    sprintf( buf, "%s %s", parts_bit_name( value ), name);
    part->name = str_dup( buf );
    free_string( part->short_descr );
    sprintf( buf, "the %s of %s", parts_bit_name( value ), capitalize(name));
    part->short_descr = str_dup( buf );
    free_string( part->description );
    sprintf( buf, "The %s of %s&n is lying here.", parts_bit_name( value ), capitalize(name));
    part->description = str_dup( buf );
    part->cost = 0;
    part->level = corpse->level;
    part->wear_flags = 1;
    part->item_type = TYPE_TRASH;
    part->timer = dice( 5, 20 );
    SET_OBJ_STAT( part, ITEM_NOSELL );
    obj_to_obj( part, corpse );

}

void do_bandage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int chance;
    int change; 

    last_fun( "do_bandage");
    if( IS_NPC( ch ) || ch->level < skills_table[gsn_bandage].skill_level[ch->class] )
    {
        send_to_char( "You don't know how to bandage!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Bandage whom?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_room( ch, argument ) ) )
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }
 
    if(victim->hit > 0 )
    {
        send_to_char( "They do not need your help.\n\r", ch );
        return;
    }

    chance = ch->pcdata->skl_lrn[gsn_bandage];

    if( ch->class == CLASS_CLERIC )
      chance += 4;
    else if( ch->class == CLASS_ANTIPALADIN )
      chance -= 4;

    /* Don't allow someone doing more than 1 pt. of damage with bandage. */
    change = (UMAX( chance - number_percent(), -1) / 20) + 1;

    // Bandage is rarely used, make it likely to increase
    skill_practice( ch, gsn_bandage );
    skill_practice( ch, gsn_bandage );
    skill_practice( ch, gsn_bandage );

    WAIT_STATE( ch, skills_table[gsn_bandage].beats );

    if( change < 0 )
    {
       send_to_char( "You just made the problem worse!\n\r", ch );
       act( "$n&n tries bandage you but your condition only worsens.", ch, NULL, victim, TO_VICT );
       act( "$n&n tries bandage $N&n but $S condition only worsens.", ch, NULL, victim, TO_NOTVICT );
    }
    else if( change > 0 )
    {
       send_to_char( "You manage to fix them up a bit.\n\r", ch );
       act( "$n&n bandages you.", ch, NULL, victim, TO_VICT );
       act( "$n&n bandages $N&n.", ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
       send_to_char( "Your bandaging attempt had no effect.\n\r", ch );
       act( "$n&n tries to bandage you but the wounds are too great.", ch, NULL, victim, TO_VICT );
       act( "$n&n tries to bandage $N&n but is unable to have any effect.", ch, NULL, victim, TO_NOTVICT );
    }

    victim->hit += change;

    update_pos( victim );

    return;
}


void do_fly( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA* troom;
    EXIT_DATA* texit;
    CHAR_DATA *fch;
    int lower, higher, i, iroom;

    last_fun( "do_fly");

    if( ch->fighting || ch->position == POS_FIGHTING )
    {
        send_to_char( "&+cNot &+Wwhile &+cyou&+Wr fi&+cghting...&n\n\r", ch );
        return;
    }

    if ( ch->riding )
    {
        if ( !can_fly( ch->riding ) ) 
        {
            send_to_char( "Your mount can't fly.\n\r", ch );
            return;
        }
    }

    if( argument[0] == '\0' && can_fly( ch ) )
    {
         send_to_char( "Yep, you're flying.  Maybe you want to fly up or fly down.", ch );
         return;
    }
    if ( argument[0] == '\0' && !can_fly(ch ) ){
        send_to_char("Alas you are landbound.\n\r", ch );
        return;
    }

      if( !str_prefix( argument, "up" ))
      {
          if ( !can_fly(ch) ) {
            send_to_char("You cannot fly.\n\r", ch);
            return;
          }
        if ( !is_fly_room(ch->in_room) ) {
            send_to_char( "You cannot fly here.\n\r", ch);
            return;
        }
        if( ch->in_room->sector_type == SECT_PLANE_AIR ) {
            send_to_char( "That's not really an option here.\n\r", ch);
            return;
        }
         if( ch->fly_level < MAX_FLY )
         {
             act( "$n&n flies up higher.", ch, NULL, NULL, TO_ROOM );
             ch->fly_level++;
             act( "$n&n flies in from below.", ch, NULL, NULL, TO_ROOM );
             send_to_char( "You fly up.\n\r", ch );
             do_look( ch, "auto" );
             for (fch = ch->in_room->people; fch; fch = fch->next_in_room) {
                 if(fch->deleted || fch == ch) continue;
                 if (fch->master == ch && can_fly(fch)
                     && fch->fly_level == ch->fly_level - 1 ) {
                     if (fch->fly_level < MAX_FLY) {
                       act("$n&n flies up.",fch,NULL,NULL,TO_ROOM);
                       fch->fly_level++;
                       send_to_char("You fly up.\n\r", fch);
                       act("$n&n flies in from below.",fch,NULL,NULL,TO_ROOM);
                     }
                 }
             }
         }
         else
         {
             //if in a zone then fly out
         if (ch->in_room->vnum < SURFACE_MAP1_START_VNUM) {
//                 send_to_char("Trying to get you out of here, honest!\n\r", ch); //glitch
                 lower = ch->in_room->area->low_r_vnum;
                 higher = ch->in_room->area->hi_r_vnum;
                 // there has to be a better way than checking every room in zone
                 for (iroom = lower; iroom <= higher; iroom++) {
                     troom = get_room_index(iroom);
                     if (!troom) continue;
                     //now check all exits
                     for (i = 0; i < MAX_DIR; i++) {
                         texit = troom->exit[i];
                         if (!texit || !(texit->to_room) ) continue;
                         if (texit->to_room->vnum >= SURFACE_MAP1_START_VNUM) {
                             //wheee! found a way out
                             //should check for exit zone being flyable
                            send_to_char( "You fly up.\n\r", ch );
                            act( "$n&n flies up higher.", ch, NULL, NULL, TO_ROOM );
                            char_from_room(ch);
                            char_to_room(ch, texit->to_room);
                            last_fun( "do_fly");
                            ch->fly_level = 1;
                            do_look( ch, "auto" );
                            return;
                         }
                     }
                 }
                 send_to_char("Sorry pal, couldn't find any zone exits.\n\r", ch); //glitch
             }
             send_to_char( "If you were any higher you'd be Bob Marley.\n\r", ch );
         }
      }
      else if( !str_prefix( argument, "down" ))
      {
         if( ch->fly_level > 0 )
         {
             act( "$n&n flies down lower.", ch, NULL, NULL, TO_ROOM );
             ch->fly_level--;
             act( "$n&n flies in from above.", ch, NULL, NULL, TO_ROOM );
             if( ch->fly_level == 0 )
               send_to_char( "You fly down to the ground.\n\r", ch );
             else
               send_to_char( "You fly down.\n\r", ch );
             do_look( ch, "auto" );
             for (fch = ch->in_room->people; fch; fch = fch->next_in_room) {
                 if(fch->deleted || fch == ch) continue;
                 if (fch->master == ch && can_fly(fch) ) {
                     act("$n&n flies down.",fch,NULL,NULL,TO_ROOM);
                     fch->fly_level--;
                     send_to_char("You fly down.\n\r", fch);
                     act("$n&n flies in from above.",fch,NULL,NULL,TO_ROOM);
                 }
             }
         }
         else
         {
             send_to_char( "You're already on the ground.\n\r", ch );
         }
      }
      else if( !str_prefix( argument, "land" ))
      {
         if( ch->fly_level > 0 )
         {
             int oldfly;
             act( "$n&n flies down lower.", ch, NULL, NULL, TO_ROOM );
             oldfly = ch->fly_level;
             for( ; ch->fly_level > 0; ch->fly_level--) {
                act("$n&n flies down lower.",ch, NULL, NULL, TO_ROOM );
             }
             ch->fly_level = 0;
             act( "$n&n flies in from above.", ch, NULL, NULL, TO_ROOM );
             send_to_char( "You fly down to the ground.\n\r", ch );
             do_look( ch, "auto" );
             for (fch = ch->in_room->people; fch; fch = fch->next_in_room) {
                 if(fch->deleted || fch == ch) continue;
                 if (fch->master == ch && can_fly( fch )
                   && fch->fly_level == oldfly ) {
                     for( ; fch->fly_level > 0; fch->fly_level-- ) {
                         act("$n&n flies down lower.",fch,NULL,NULL,TO_ROOM);
                     }
                     fch->fly_level = 0;
                     send_to_char("You fly down to the ground.\n\r", fch);
                     act("$n&n flies in from above.",fch,NULL,NULL,TO_ROOM);
                 }
             }
         }
         else
         {
             send_to_char( "You're already on the ground.\n\r", ch );
         }
      }
      else
          send_to_char( "Fly where?\n\r", ch );
 
      return;
}



int scan_this_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *buf,
		 int fly_level)
{
    CHAR_DATA *target		= room->people;
    int        number_found	= 0;
    int        visibility	= 0;
    char       level_msg[MAX_STRING_LENGTH];
	int        diff;


	diff = fly_level - ch->fly_level;
    switch ( diff )
    {
	case -3:
	    sprintf(level_msg, "&n who is very far beneath you." );
	    break;
	case -2:
	    sprintf(level_msg, "&n who is not far beneath you." );
	    break;
	case -1:
	    sprintf(level_msg, "&n who is close by beneath you." );
	    break;
	case 1:
	    sprintf(level_msg, "&n who is flying close by above you." );
	    break;
	case 2:
	    sprintf(level_msg, "&n who is flying not far above you." );
	    break;
	case 3:
	    sprintf(level_msg, "&n who is flying very far above you." );
	    break;
	default:
	    sprintf(level_msg, "&n who is an unknown distance away from you." );
	    break;
    }

    while ( target )
    {
        if (target->deleted) continue;
        if ( target->fly_level == fly_level) {
	visibility = how_see(ch, target);	
        switch ( visibility )
        {
	case SEE_SENSE_HIDDEN:
	case SEE_INVISIBLE:
	case SEE_TOO_DARK:
	default:
	    break;
	case SEE_VISIBLE:
            strcat ( buf, PERS( target, ch ) );
            strcat ( buf, level_msg );
            strcat ( buf, "&n\n\r");
            number_found++;
	    break;
	case SEE_SENSE_INFRA:
	    strcat ( buf, "&+rYou sense a being within the darkness" );
            strcat ( buf, level_msg );
            strcat ( buf, "&n\n\r");
            number_found++;
	    break;
        }
        } //end if (target->fly_level == fly_level)
        target = target->next_in_room;
    }
    return number_found;
}

void do_suicide( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_suicide");
    if( ch->fighting )
    {
      send_to_char( "Suicide!?  Just let your opponent finish you off...\n\r", ch );
      return;
    }
    if( ch->level > 20 && !IS_IMMORTAL(ch) )
    {
      send_to_char( "Suicide is for low level characters only!\n\r", ch );
      return;
    }

    if ( is_affected( ch, AFF_CHARM ) ) {
        send_to_char( "Only those with free will can suicide.\n\r", ch);
        if (ch->master)
            send_to_char( "Shame on you for taking advantage like that!\n\r", ch->master);
        return;
    }
    if( str_cmp( argument, "confirm" ))
    {
      send_to_char( "If you really want to kill yourself, try 'suicide confirm'\n\r", ch );
      return;
    }

    // Specifically allowing suicide while bleeding to death -- Veygoth
    if( ch->position < POS_STUNNED )
    {
       send_to_char( "You give up the will to live.\n\r", ch );
    }
    else
    {
       act( "With a look of despair in $s eyes, $n&n takes $s own life.", ch, NULL, NULL, TO_ROOM );
       send_to_char( "You take your own life.\n\r", ch );
    }

    raw_kill(ch, ch );
    last_fun( "do_suicide");

    // Exp loss is that of a normal death
    gain_exp( ch, (0 - (((25 + ch->level) * exp_table[ch->level].lvl_exp ) / 200 )));
    if( ch->level < 2 && ch->exp < 1 )
      ch->exp = 1;

    return;
}

ROOM_INDEX_DATA *find_room( CHAR_DATA *ch, char *argument)
{
    ROOM_INDEX_DATA *room;
    int dir;

    dir = find_exit( ch, argument );
    if ( dir == -1 ) {
        return NULL;
    }
    if ( ch->in_room->exit[dir] ) {
        room = ch->in_room->exit[dir]->to_room;
        return room;
    }
    return NULL;
}

// Event creation function - Veygoth
// Once it is finished, deleted events should be recycled.
MOVE_SCRIPT * create_move_script( )
{
        MOVE_SCRIPT *script;

        if( mvscript_free )
        {
          script = mvscript_free;
          mvscript_free = script->next;
          script->next = NULL;
        }
        else
        {
          script = alloc_perm( sizeof( MOVE_SCRIPT ) );
          top_mvscript++;
        }

        free_string( script->script );

        memset( script, 0, sizeof( MOVE_SCRIPT ) );

        return script;
}

void do_climb( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA *obj;
        int chance;

        if( ch->level < skills_table[gsn_climb].skill_level[ch->class] )
        {
           send_to_char( "You lack the skills to climb anything.\n\r", ch );
           return;
        }

        if( !(obj = get_obj_here( ch, argument ) ))
        {
           send_to_char( "Uhh... what exactly did you want to climb!?\n\r", ch );
           return;
        }

        if( obj->item_type != TYPE_WALL )
        {
           send_to_char( "That wasn't exactly designed for climbing.\n\r", ch );
           return;
        }

        if( IS_NPC( ch ))
           chance = ch->level * 3 / 2 + 20;
        else
           chance = ch->pcdata->skl_lrn[gsn_climb];

        // Agility helps.
        chance += get_curr_agi( ch ) / 10;

        switch( obj->pIndexData->vnum )
        {
           case OBJ_VNUM_WALL_STONE:
             chance += 5;
             break;
           case OBJ_VNUM_WALL_IRON:
             chance -= 15;
             break;
           default:
             send_to_char( "That wasn't exactly designed for climbing.\n\r", ch );
             return;
             break;
        }

        // Maximum chance of 98%
        if( chance > 98 )
          chance = 98;

        if( number_percent() >= chance )
        {
           send_to_char( "You try to climb it, but you fall on your ass!\n\r", ch );
           ch->position = POS_SITTING;
           WAIT_STATE( ch, 5 );
           return;
        }

        send_to_char( "With great skill, you scale the wall!\n\r", ch );

        // Value 0 of a wall object is the direction that has been walled...
        // This means that they should move in that direction.  We leave it up to
        // move_char to make sure that there is actually an exit in that direction.
        // we use the climbing bit to allow them to pass the walls in move_char.
        // -- Veygoth
        SET_AFF_BIT( ch, AFF_CLIMBING );
        move_char( ch, obj->value[0] );
        REMOVE_AFF_BIT( ch, AFF_CLIMBING );

        return;
}
