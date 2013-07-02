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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/*
 * Event code written by Veygoth.  Events are used to time things so that
 * they don't have to happen on an exact tick.
 *
 * Ticks are also handled by this code.
 */

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

char lbuf[MAX_STRING_LENGTH];
EVENT_DATA * event_first = NULL;
EVENT_DATA * event_free = NULL;

// Event creation function - Veygoth
// Once it is finished, deleted events should be recycled.
EVENT_DATA * create_event( int type, int time, void * arg1, void * arg2, int var )
{
        EVENT_DATA *event;

        last_fun( "create_event");

        if( event_free )
        {
          event = event_free;
          event_free = event->next;
          event->next = NULL;
        }
        else
        {
	  event = alloc_perm( sizeof( EVENT_DATA ) );
          top_event++;
        }

        memset( event, 0, sizeof( EVENT_DATA ) );

        event->type = type;
        event->time = time;
        event->arg1 = arg1;
        event->arg2 = arg2;
        event->var = var;
        event->deleted = FALSE;
        event->next = event_first;
        event_first = event;        

	return event;
}

// This is called once per pulse - Veygoth
//
// The event system uses a shitload of void pointers
// and is prone to accident.  Scrutinize carefully.
void event_update( )
{
    ROOM_INDEX_DATA *room;
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    EVENT_DATA *event = NULL;
    EVENT_DATA *prev = NULL;
    EVENT_DATA *next = NULL;
    int count;
    char buf[MAX_STRING_LENGTH];

    last_fun( "event_update");

    // Handle all the events
    for( event = event_first; event; event = next )
    {
        next = event->next;
        // We use nextevent so we can remove events from the list without losting our place
        if( event->deleted )
        {
          if( event_first == event )
            event_first = event->next; 
          if( prev )
            prev->next = event->next;
          event->next = event_free;      
          event_free = event;
          continue;
        }
        else
          prev = event;
		  event->time -= 1;
        // Be sure and send them the stars for spellcast ticks. (Veygoth)
        if( event->type == EVENT_SPELL_CAST ) {
            if ( is_affected( (CHAR_DATA *)event->arg1, AFF_CASTING) ) {
                if ( !IS_NPC( (CHAR_DATA *)event->arg1 )
                    && IS_SET( ((CHAR_DATA *)event->arg1 )->act, PLR_CAST_TICK )
                    && event->time > 0
                    && event->time % PULSE_PER_SECOND == 0 )
                {
                  // Snikt - Added Colors to Cast_Meter ... Purely Cosmetic
                  sprintf( buf, "&+LCasting &n%s&n&+r:&+R ", spells_table[event->var].name );
                  for( count = 0; count < (event->time / PULSE_PER_SECOND);
                     count++ )
                          strcat( buf, "*" );
                  strcat( buf, "&n\n\r" );
                  send_to_char( buf, (CHAR_DATA *)event->arg1 );
                }
            }
            else {
                //abort the spell
                event->time = 0;
            }
        }
        if( event->time <= 0 ) 
        {
          sprintf( lbuf, "Event command, event type %s", event_string( event ) );
            last_com( lbuf );
          switch( event->type )
          {
            case EVENT_SPELL_CAST:
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1
               // We stored the sn in var
               // We stored the target argument in arg2
               finish_spell( event->arg1, event->var, event->arg2, FALSE );
               break;
            case EVENT_SONG:
               event->deleted = TRUE;
               ch = (CHAR_DATA *)event->arg1;
               if( !ch || ch->deleted )
                  break;
               if( ch->mana < spells_table[event->var].min_mana )
			   {
				   act( "$n&n chokes and falls silent.", ch, NULL, NULL, TO_ROOM );
				   break;
			   }
               ch->mana -= spells_table[event->var].min_mana;
               // We stored the CHAR_DATA in arg1
               // We stored the sn in var
               // We stored the target argument in arg2
               finish_spell( event->arg1, event->var, event->arg2, TRUE );
               ch = NULL;
               break;
            case EVENT_ACID_ARROW:
               log_string( "Event acid arrow" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               if( !(CHAR_DATA *)event->arg2 || !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted )
                  break;
               spell_damage( (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2, event->var, spl_acid_arrow, DAM_ACID );
               break;
            case EVENT_IMMOLATE:
               log_string( "Event immolate" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               if( !(CHAR_DATA *)event->arg2 || !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted )
                  break;
               spell_damage( (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2, event->var, spl_immolate, DAM_FIRE );
               break;
            // Veygoth - This is bound to be buggy as hell, because it's multiple effects in an event,
            // and the sleep spell could take place after the player dies.
            case EVENT_NIGHTMARES:
               log_string( "Event nightmares" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               if( !(CHAR_DATA *)event->arg2 || !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted )
                  break;
               spell_damage( (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2, event->var, spl_nightmares, DAM_MENTAL );
               spell_sleep( spl_sleep, ((CHAR_DATA *)event->arg1)->level, (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2 );
               break;
            case EVENT_WIERD:
               log_string( "Event wierd" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               if( !(CHAR_DATA *)event->arg2 || !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted )
                  break;
               spell_damage( (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2, event->var, spl_wierd, DAM_MENTAL );
               break;
            case EVENT_CONFLAGURATION:
               log_string( "Event conflaguration" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               if( !(CHAR_DATA *)event->arg2 || !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted )
                  break;
               spell_damage( (CHAR_DATA *)event->arg1, (CHAR_DATA *)event->arg2, event->var, spl_conflaguration, DAM_FIRE );
               break;
            case EVENT_REMOVE_JUSTDIED:
               log_string( "Event remove justdied bit" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg1 for ch
               // We stored the CHAR_DATA in arg2 for victim
               // We stored the damage in var
               log_string( "checking" );
               if( !(CHAR_DATA *)event->arg1 || ((CHAR_DATA *)(event->arg1))->deleted || IS_NPC((CHAR_DATA *)event->arg1))
                  break;
               log_string( "removing" );
               REMOVE_BIT( ((CHAR_DATA *)event->arg1)->act, PLR_JUST_DIED );
               log_string( "done" );
               break;
            case EVENT_CREEPING_DOOM:
               log_string( "Event creeping doom" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg2 for room
               // We stored the CHAR_DATA in arg1 for ch (caster)
               // We stored the damage in var
               ch = NULL;
               victim = NULL;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg2) || !(ch = (CHAR_DATA *)event->arg1 ))
                  break;
               for( victim = room->people; victim; victim = victim->next_in_room )
               {
                 if( victim->deleted )
                   continue;
                 if( ch && is_same_group( ch, victim ))
                   continue;
                   
                 spell_damage( ch, victim, event->var, spl_creeping_doom, DAM_HARM );                 
               }
               ch = NULL;
               victim = NULL;
               room = NULL;
               break;
            case EVENT_BOMBARD:
               log_string( "Event bombard" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg2 for room
               // We stored the CHAR_DATA in arg1 for ch (caster)
               // We stored the damage in var
               ch = NULL;
               victim = NULL;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg2) || !(ch = (CHAR_DATA *)event->arg1 ))
                  break;
               for( victim = room->people; victim; victim = victim->next_in_room )
               {
                 if( victim->deleted )
                   continue;
                 if( ch && is_same_group( ch, victim ))
                   continue;
                   
                 spell_damage( ch, victim, event->var, spl_bombard, DAM_HARM );                 
               }
               ch = NULL;
               victim = NULL;
               room = NULL;
               break;
            case EVENT_INCENDIARY_CLOUD:
               log_string( "Event incendiary cloud" );
               event->deleted = TRUE;
               // We stored the CHAR_DATA in arg2 for room
               // We stored the CHAR_DATA in arg1 for ch (caster)
               // We stored the damage in var
               ch = NULL;
               victim = NULL;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg2) || !(ch = (CHAR_DATA *)event->arg1 ))
                  break;
               for( victim = room->people; victim; victim = victim->next_in_room )
               {
                 if( victim->deleted )
                   continue;
                 if( ch && is_same_group( ch, victim ))
                   continue;
                   
                 spell_damage( ch, victim, event->var, spl_incendiary_cloud, DAM_FIRE );                 
               }
               ch = NULL;
               victim = NULL;
               room = NULL;
               break;
            case EVENT_LIGHT_END:                             //**Saeven 00-8-15
               log_string( "Event light end" );               //  Continual light event
               event->deleted = TRUE;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg1 ))
                  break;
               if( IS_ROOM( room, ROOM_MAGICLIGHT ))
               {
                  ch = NULL;
                  REMOVE_ROOM_BIT( room, ROOM_MAGICLIGHT );
                  for( ch = room->people; ch; ch = ch->next_in_room )
                  {
                    send_to_char( "&+WThe magical light subsides.\n\r", ch );
                  }
               }
               break;
			case EVENT_DARKNESS_END:
               log_string( "Event darkness end" );
               event->deleted = TRUE;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg1 ))
                  break;
               if( IS_ROOM( room, ROOM_MAGICDARK ))
               {
                  ch = NULL;
                  REMOVE_ROOM_BIT( room, ROOM_MAGICDARK );
                  for( ch = room->people; ch; ch = ch->next_in_room )
                  {
                    send_to_char( "&+LThe darkness&n seems to lift a bit.\n\r", ch );
                  }
               }
               break;
            case EVENT_STARSHELL_END:
               log_string( "Event starshell end" );
               event->deleted = TRUE;
               room = NULL;
               if( !(room = (ROOM_INDEX_DATA *)event->arg1 ))
                  break;
               if( IS_ROOM( room, ROOM_EARTHEN_STARSHELL )
		|| IS_ROOM( room, ROOM_FIERY_STARSHELL )
		|| IS_ROOM( room, ROOM_AIRY_STARSHELL )
		|| IS_ROOM( room, ROOM_WATERY_STARSHELL ) )
               {
                  ch = NULL;
                  REMOVE_ROOM_BIT( room, ROOM_EARTHEN_STARSHELL );
                  REMOVE_ROOM_BIT( room, ROOM_AIRY_STARSHELL );
                  REMOVE_ROOM_BIT( room, ROOM_FIERY_STARSHELL );
                  REMOVE_ROOM_BIT( room, ROOM_WATERY_STARSHELL );
                  for( ch = room->people; ch; ch = ch->next_in_room )
                  {
                    send_to_char( "The &+Ystarshell&n shatters and fades away.\n\r", ch );
                  }
               }
               if( IS_ROOM( room, ROOM_HYPNOTIC_PATTERN ) )
               {
                  REMOVE_ROOM_BIT( room, ROOM_HYPNOTIC_PATTERN );
                  for( ch = room->people; ch; ch = ch->next_in_room )
                  {
                    send_to_char( "&+CThe pa&+cttern &+Bfades a&+bway.&n\n\r", ch );
                  }
               }
               break;
            case EVENT_HIT_GAIN:
               hit_update( );
               event->time = PULSE_HITGAIN;
               break;
            case EVENT_MANA_GAIN:
               mana_update( );
               event->time = PULSE_MANAGAIN;
               break;
            case EVENT_MOVE_GAIN:
               move_update( );
               event->time = PULSE_MOVEGAIN;
               break;
            // This particular event could cause things to get pretty hairy if not used right.
            case EVENT_EXTRACT_CHAR:
               log_string( "Event extract char" );
               event->deleted = TRUE;
               ch = NULL;
               if( !( ch = (CHAR_DATA *)event->arg1) || ch->deleted )
                 break;
               stop_fighting( ch, TRUE );
               if( ch->in_room )
                 act( "$n&n disappears.", ch, NULL, NULL, TO_ROOM );
               extract_char( ch, TRUE );
               last_fun( "event_update");
               break;
	    case EVENT_CAMP:
               // Camp_update returns TRUE if they're still camping.
               if( !camp_update( (CHAR_DATA *)event->arg1, (ROOM_INDEX_DATA *)event->arg2 ) )
               {
                 event->deleted = TRUE;
                 REMOVE_BIT( ((CHAR_DATA *)event->arg1)->act, PLR_CAMPING);
               }
               else if( (--event->var) == 0 )
               {
                 do_quit( (CHAR_DATA *)event->arg1, "" );
                 event->deleted = TRUE;
               }
               else
               {
                 event->time = PULSE_CAMP;
               }
               break;
            case EVENT_MEMORIZE_UPDATE:
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Memorize update event" );
               memorize_update( );
               event->time = PULSE_MEMORIZE;
               break;
            case EVENT_AGGR_UPDATE:
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Aggression event" );
               aggr_update( );
               event->time = PULSE_AGGRESS;
               break;
	    case EVENT_HEARTBEAT:
	       wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Heartbeat event" );
	       heartbeat_update( );
	       event->time = PULSE_WEATHER;
	       break;
            case EVENT_WEATHER_UPDATE:
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Weather update event" );
			   weather_update  ( );
               event->time = PULSE_WEATHER;
               break;
            case EVENT_CHAR_UPDATE:
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Char update event" );
               char_update     ( );
               event->time = PULSE_CHAR_UPDATE;
               break;
            case EVENT_OBJECT_SPECIAL:
               // Object specials only.
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Object special event" );
               obj_spec_update      ( );
               event->time = PULSE_OBJECT;
               break;
            case EVENT_LIST_UPDATE:
               wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "List update event" );
	           list_update     ( );
               event->time = PULSE_LIST;
               break;
            case EVENT_AREA_UPDATE:
               wiznet( NULL, WIZ_TICKS, L_APP, "Area update event" );
               area_update	( );
               event->time = PULSE_AREA;
               break;
            case EVENT_ROOM_UPDATE:
               wiznet( NULL, WIZ_TICKS, L_APP, "Room update event" );
               room_update     ( );
               event->time = PULSE_ROOM;
               break;
            case EVENT_MOBILE_UPDATE:
               wiznet( NULL, WIZ_TICKS, L_APP, "Mobile update event" );
               mobile_update   ( );
               event->time = PULSE_MOBILE;
               break;
            case EVENT_OBJECT_UPDATE:
               // Object timers only
               wiznet( NULL, WIZ_TICKS, L_APP, "Object update event" );
	           obj_update   ( );
               event->time = PULSE_OBJ_UPDATE;
               break;
            case EVENT_VIOLENCE_UPDATE:
               wiznet( NULL, WIZ_TICKS, L_APP, "Violence update pulse" );
	           violence_update ( );
               event->time = PULSE_VIOLENCE_UP;
               break;
            case EVENT_SAVE_SYSDATA:
               wiznet( NULL, WIZ_TICKS, L_APP, "Save sysdata event" );
               save_sysdata( );
               event->time = PULSE_SAVE_SYSDATA;
               break;
            case EVENT_SAVE_CORPSES:
               wiznet( NULL, WIZ_TICKS, L_APP, "Save corpse event" );
               save_corpses( );
               event->time = PULSE_SAVE_CORPSES;
               break;
            default:
               bug( "Unsupported Event Type %d", event->type );
               break;
          } // end switch
       } // end if
    }

    time_update( );
    tail_chain( );
    return;
}

// This function is used to deleted all of the events attached to
// a character when they die or leave the game.
//
// Accessing a terminated character is big time bad news, since
// pointers will get way outta whack, the wrong characters will
// be effected, effects will persist after death possibly causing
// multiple deaths, and the mud may crash randomly and seemingly
// without reason.
//
// Affects like immolate and such still need to be resolved
// (if the caster dies, should the spell persist?)
//
// -- Veygoth
void delete_attached_events( CHAR_DATA *ch )
{
	EVENT_DATA *event;
	CHAR_DATA *ech;

        last_fun( "delete_attached_events");

	for( event = event_first; event; event = event->next )
	{
		if( event->deleted )
			continue;
		switch( event->type )
		{
		case EVENT_CAMP:
			ech = (CHAR_DATA *)event->arg1;
			if( !ech || ech->deleted || ech == ch )
				event->deleted = TRUE;
			break;
		case EVENT_EXTRACT_CHAR:
			// Does not delete event upom caster's death.
			// This may be a problem.
			// However, it would be really smooth if the caster
			// was able to kill someone after their death.
			// This could cause problems if say, a shadow monster
			// was killed before it was supposed to disappear.
			// This would cause a corpse to be left for something
			// that should not leave a corpse.
			// However, the shadow monsters and such should be flagged
			// race illusion and not leave corpses.
			ech = (CHAR_DATA *)event->arg1;
			if( !ech || ech->deleted || ech == ch )
				event->deleted = TRUE;
			break;
		case EVENT_IMMOLATE:
			// Does not delete event upom caster's death.
			// This may be a problem.
			// However, it would be really smooth if the caster
			// was able to kill someone after their death.
			ech = (CHAR_DATA *)event->arg2;
			if( !ech || ech->deleted || ech == ch )
				event->deleted = TRUE;
			break;
		case EVENT_BOMBARD:
			// Room specific event, should be deleted only if it
			// should not continue if the caster is dead.
			break;
		case EVENT_CREEPING_DOOM:
			// Room specific event, should be deleted only if it
			// should not continue if the caster is dead.
			break;
		case EVENT_INCENDIARY_CLOUD:
			// Room specific event, should be deleted only if it
			// should not continue if the caster is dead.
			break;
		case EVENT_CONFLAGURATION:
			// Does not delete event upom caster's death.
			// This may be a problem.
			// However, it would be really smooth if the caster
			// was able to kill someone after their death.
			ech = (CHAR_DATA *)event->arg2;
			if( !ech || ech->deleted || ech == ch )
				event->deleted = TRUE;
			break;
		case EVENT_ACID_ARROW:
			// Does not delete event upom caster's death.
			// This may be a problem.
			// However, it would be really smooth if the caster
			// was able to kill someone after their death.
			ech = (CHAR_DATA *)event->arg2;
			if( !ech || ech->deleted || ech == ch )
				event->deleted = TRUE;
			break;
		case EVENT_SPELL_CAST:
			// Does not delete event upon nonexistance of target.
			// This may be a problem.
			// Ideally we should only have to worry about the caster
			// and the validity of the target is checked in finish_spell()
			ech = (CHAR_DATA *)event->arg1;
			if( !ech || ech->deleted || ech == ch )
			{
				act( "$n&n stops chanting abruptly!", ch, NULL, NULL, TO_ROOM );
				event->deleted = TRUE;
			}
			break;
		case EVENT_SONG:
			// All bard songs are area effects and not victim-dependent
			ech = (CHAR_DATA *)event->arg1;
			if( !ech || ech->deleted || ech == ch )
			{
				act( "$n&n gasps and falls silent.", ch, NULL, NULL, TO_ROOM );
				event->deleted = TRUE;
			}
			break;
		}
	}

	return;

}

char *event_string( EVENT_DATA *event ) {

    last_fun( "event_string " );

    if ( !event ) 
        return "null event";
    switch( event->type ) {
    case EVENT_NONE:
        return "none"; break;
    case EVENT_SAVE_SYSDATA:
        return "save_sysdata"; break;
    case EVENT_SAVE_CORPSES:
        return "save_corpses"; break;
    case EVENT_HIT_GAIN:
        return "hit_gain"; break;
    case EVENT_MANA_GAIN:
        return "mana_gain"; break;
    case EVENT_MOVE_GAIN:
        return "move_gain"; break;
    case EVENT_RESET_AREA:
        return "reset_area"; break;
    case EVENT_MEMORIZE_UPDATE:
        return "memorize_update"; break;
    case EVENT_WEATHER_UPDATE:
        return "weather_update"; break;
    case EVENT_MOBILE_UPDATE:
        return "mobile_update"; break;
    case EVENT_OBJECT_UPDATE:
        return "object_update"; break;
    case EVENT_SLOW_CHAR_UPDATE:
        return "slow_char_update"; break;
    case EVENT_CHAR_UPDATE:
        return "char_update"; break;
    case EVENT_LIST_UPDATE:
        return "list_update"; break;
    case EVENT_VIOLENCE_UPDATE:
        return "violence_update"; break;
    case EVENT_AGGR_UPDATE:
        return "aggr_update"; break;
    case EVENT_HUNGER:
        return "hunger"; break;
    case EVENT_PULSE_REGENERATE:
        return "pulse_regenerate"; break;
    case EVENT_CAST_TICK:
        return "cast_tick"; break;
    case EVENT_ROOM_AFF_ADD:
        return "room_aff_add"; break;
    case EVENT_ROOM_AFF_REMOVE:
        return "room_aff_remove"; break;
    case EVENT_ROOM_PROC:
        return "room_proc"; break;
    case EVENT_MOB_PROC:
        return "mob_proc"; break;
    case EVENT_OBJ_PROC:
        return "obj_proc"; break;
    case EVENT_ZONE_PROC:
        return "zone_proc"; break;
    case EVENT_QUEST_PROC:
        return "quest_proc"; break;
    case EVENT_SHIP_PROC:
        return "ship_proc"; break;
    case EVENT_ROOM_DMG_SPELL:
        return "room_dmg_spell"; break;
    case EVENT_PERSON_DMG_SPELL:
        return "person_dmg_spell"; break;
    case EVENT_SPELL_CAST:
        return "spell_cast"; break;
    case EVENT_CAMP:
        return "camp"; break;
    case EVENT_DELAYED_COMMAND:
        return "delayed_command"; break;
    case EVENT_EXTRACT_CHAR:
        return "extract_char"; break;
    case EVENT_BOMBARD:
        return "bombard"; break;
    case EVENT_OBJ_DECAY:
        return "obj_decay"; break;
    case EVENT_WIERD:
        return "wierd"; break;
    case EVENT_SHIP_MOVE:
        return "ship_move"; break;
    case EVENT_FIRE_PLANE:
        return "fire_plane"; break;
    case EVENT_AUTOSAVE:
        return "autosave"; break;
    case EVENT_TRACK:
        return "track"; break;
    case EVENT_SPELL_SCRIBING:
        return "spell_scribing"; break;
    case EVENT_NIGHTMARES:
        return "nightmares"; break;
    case EVENT_REMOVE_JUSTDIED:
        return "remove_justdied"; break;
    case EVENT_SONG:
        return "song"; break;
    case EVENT_BARD_EFFECTDECAY:
        return "bard_effectdecay"; break;
    case EVENT_STUNNED:
        return "stunned"; break;
    case EVENT_KO:
        return "ko"; break;
    case EVENT_BRAIN_DRAIN:
        return "brain_drain"; break;
    case EVENT_CONFLAGURATION:
        return "conflaguration"; break;
    case EVENT_BERSERK:
        return "berserk"; break;
    case EVENT_MOB_HUNT:
        return "mob_hunt"; break;
    case EVENT_UNDERWATER:
        return "underwater"; break;
    case EVENT_SWIMMING:
        return "swimming"; break;
    case EVENT_ROOM_UPDATE:
        return "room_update"; break;
    case EVENT_AREA_UPDATE:
        return "area_update"; break;
    case EVENT_OBJECT_SPECIAL:
        return "object_special"; break;
    case EVENT_ACID_ARROW:
        return "acid_arrow"; break;
    case EVENT_LIGHT_END:
        return "light_end"; break;
    case EVENT_DARKNESS_END:
        return "darkness_end"; break;
    case EVENT_IMMOLATE:
        return "immolate"; break;
    case EVENT_CREEPING_DOOM:
        return "creeping_doom"; break;
    case EVENT_INCENDIARY_CLOUD:
        return "incendiary_cloud"; break;
    case EVENT_HEARTBEAT:
        return "heartbeat"; break;
    case EVENT_STARSHELL_END:
        return "starshell_end"; break;
    default:
        return "unknown";
    }
}
