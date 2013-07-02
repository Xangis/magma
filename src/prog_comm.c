/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  The MOBprograms have been contributed by N'Atas-ha.  Any support for   *
 *  these routines should not be expected from Merc Industries.  However,  *
 *  under no circumstances should the blame for bugs, etc be placed on     *
 *  Merc Industries.  They are not guaranteed to work on all systems due   *
 *  to their frequent use of strxxx functions.  They are also not the most *
 *  efficient way to perform their tasks, but hopefully should be in the   *
 *  easiest possible way to install and begin using. Documentation for     *
 *  such installation can be found in INSTALL.  Enjoy........    N'Atas-Ha *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

/*
 * Local functions.
 */
char *			prog_type_to_name	args( ( int type ) );
bool prog_move_script( CHAR_DATA *ch );

/*
 * This routine transfers between alpha and numeric forms of the
 * mob_prog bitvector types. It allows the words to show up in mpstat to
 * make it just a hair bit easier to see what a mob should be doing.
 */
char *prog_type_to_name( int type )
{
    switch ( type )
    {
    case IN_FILE_PROG:          return "in_file_prog";
    case ACT_PROG:              return "act_prog";
    case SPEECH_PROG:           return "speech_prog";
    case RAND_PROG:             return "rand_prog";
    case FIGHT_PROG:            return "fight_prog";
    case HITPRCNT_PROG:         return "hitprcnt_prog";
    case DEATH_PROG:            return "death_prog";
    case ENTRY_PROG:            return "entry_prog";
    case GREET_PROG:            return "greet_prog";
    case ALL_GREET_PROG:        return "all_greet_prog";
    case GIVE_PROG:             return "give_prog";
    case MOVE_PROG:             return "move_prog";
    case BRIBE_PROG:            return "bribe_prog";
    default:                    return "ERROR_PROG";
    }
}

/* 
 * A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MOBprograms which are set.
 */
void do_mpstat( CHAR_DATA *ch, char *argument )
{
    char        buf[ MAX_STRING_LENGTH ];
    char        arg[ MAX_INPUT_LENGTH  ];
    MPROG_DATA *mprg;
    CHAR_DATA  *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "MobProg stat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Only mobiles can have programs!\n\r", ch);
	return;
    }

    if ( !( victim->pIndexData->progtypes ) )
    {
	send_to_char( "That mobile has no programs set.\n\r", ch);
	return;
    }

    sprintf( buf, "Name: %s.  Vnum: %d.\n\r",
	victim->name, victim->pIndexData->vnum );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong  description: %s",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ?
	    victim->long_descr : "(none).\n\r" );
    send_to_char( buf, ch );

    sprintf( buf, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d.\n\r",
	victim->hit,         get_max_hit( victim ),
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move );
    send_to_char( buf, ch );

    sprintf( buf,
	"Lv: %d.  Class: %d.  Align: %d.  AC: %d.  Coins: %d.  Exp: %d.\n\r",
	victim->level,       victim->class,        victim->alignment,
	get_ac( victim ),    get_cash( victim ),   victim->exp );
    send_to_char( buf, ch );

    for ( mprg = victim->pIndexData->mobprogs; mprg != NULL;
	 mprg = mprg->next )
    {
      sprintf( buf, ">%s %s\n\r%s\n\r",
	      prog_type_to_name( mprg->type ),
	      mprg->arglist,
	      mprg->comlist );
      send_to_char( buf, ch );
    }

    return;

}

/* prints the argument to all the rooms aroud the mobile */
void do_prgasound( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *was_in_room;
  int              door;

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    if ( argument[0] == '\0' )
    {
        bug( "Mpasound - No argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    was_in_room = ch->in_room;
    for ( door = 0; door < MAX_DIR; door++ )
    {
      EXIT_DATA       *pexit;
      
      if ( ( pexit = was_in_room->exit[door] ) != NULL
	  &&   pexit->to_room != NULL
	  &&   pexit->to_room != was_in_room )
      {
	ch->in_room = pexit->to_room;
	MOBtrigger  = FALSE;
	act( argument, ch, NULL, NULL, TO_ROOM );
      }
    }

  ch->in_room = was_in_room;
  return;

}

/* lets the mobile kill any player or mobile without murder*/
void do_prgkill( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	bug( "MpKill - no argument: vnum %d.",
		ch->pIndexData->vnum );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	bug( "MpKill - Victim not in room: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    if ( victim == ch )
    {
	bug( "MpKill - Bad victim to attack: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) && ch->master == victim )
    {
	bug( "MpKill - Charmed mob attacking master: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {	
	bug( "MpKill - Already fighting: vnum %d",
	    ch->pIndexData->vnum );
	return;
    }

    kill_ch( ch, victim );
    return;
}


/* 
 * lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy 
 * items using all.xxxxx or just plain all of them
 */
void do_prgjunk( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
        bug( "Mpjunk - No argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
      if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
      {
	unequip_char( ch, obj );
	extract_obj( obj );
	return;
      }
      if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	return; 
      extract_obj( obj );
    }
    else
      for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
        {
          if ( obj->wear_loc != WEAR_NONE)
	    unequip_char( ch, obj );
          extract_obj( obj );
        } 
      }

    return;

}

/* prints the message to everyone in the room other than the mob and victim */
void do_prgechoaround( CHAR_DATA *ch, char *argument )
{
  char       arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA *victim;

    if ( !IS_NPC( ch ) )
    {
       send_to_char( "Huh?\n\r", ch );
       return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       bug( "Mpechoaround - No argument:  vnum %d.", ch->pIndexData->vnum );
       return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        bug( "Mpechoaround - victim does not exist: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    act( argument, ch, NULL, victim, TO_NOTVICT );
    return;
}

/* prints the message to only the victim */
void do_prgechoat( CHAR_DATA *ch, char *argument )
{
  char       arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA *victim;

    if ( !IS_NPC( ch ) )
    {
       send_to_char( "Huh?\n\r", ch );
       return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
       bug( "Mpechoat - No argument:  vnum %d.",
	   ch->pIndexData->vnum );
       return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        bug( "Mpechoat - victim does not exist: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    act( argument, ch, NULL, victim, TO_VICT );
    return;
}

/* prints the message to the room at large */
void do_prgecho( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    if ( argument[0] == '\0' )
    {
        bug( "Mpecho - called w/o argument: vnum %d.",
	    ch->pIndexData->vnum );
        return;
    }

    act( argument, ch, NULL, NULL, TO_ROOM );
    return;

}

/* 
 * lets the mobile load an item or mobile.  All items
 * are loaded into inventory.  you can specify a level with
 * the load object portion as well.
 */
void do_prgmload( CHAR_DATA *ch, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	bug( "Mpmload - Bad vnum as arg: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	bug( "Mpmload - Bad mob vnum: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    return;
}

void do_prgoload( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;

    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        bug( "Mpoload - Bad syntax: vnum %d.",
	    ch->pIndexData->vnum );
        return;
    }
 
    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    bug( "Mpoload - Bad syntax: vnum %d.", ch->pIndexData->vnum );
	    return;
        }
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    bug( "Mpoload - Bad level: vnum %d.", ch->pIndexData->vnum );
	    return;
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Mpoload - Bad vnum arg: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
    }

    return;
}

/* 
 * lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room.  It can purge
 * itself, but this had best be the last command in the MOBprogram
 * otherwise ugly stuff will happen
 */
void do_prgpurge( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    last_fun( "do_prgpurge");
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	  vnext = victim->next_in_room;
	  if ( IS_NPC( victim ) && victim != ch )
          {
	    extract_char( victim, TRUE );
            last_fun( "do_prgpurge");
          }
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	  obj_next = obj->next_content;
	  extract_obj( obj );
	}

	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) == 0 )
    {
	if ( ( obj = get_obj_here( ch, arg ) ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    bug( "Mppurge - Bad argument: vnum %d.", ch->pIndexData->vnum );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Mppurge - Purging a PC: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    extract_char( victim, TRUE );
    last_fun( "do_prgpurge");
    return;
}


/* lets the mobile goto any location it wishes that is not private */
void do_prggoto( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    last_fun( "do_prggoto");
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "Mpgoto - No argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpgoto - No such location: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, location );
    last_fun( "do_prggoto");

    return;
}

/* lets the mobile do a command at another location. Very useful */
void do_prgat( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA       *wch;

    last_fun( "do_prgat");
    if ( !IS_NPC( ch ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpat - Bad argument: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpat - No such location: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );
    last_fun( "do_prgat");

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
            last_fun( "do_prgat");
	    break;
	}
    }

    return;
}
 
/* 
 * Lets the mobile transfer people.  the all argument transfers
 * everyone in the current room to the specified location
 */
void do_prgtransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *victim;

    last_fun( "do_prgtransfer");
    if ( !IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mptransfer - Bad syntax: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_prgtransfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( !( location = find_location( ch, arg2 ) ) )
	{
	    bug( "Mptransfer - No such location: vnum %d.",
	        ch->pIndexData->vnum );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    bug( "Mptransfer - Private room: vnum %d.",
		ch->pIndexData->vnum );
	    return;
	}
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	bug( "Mptransfer - No such person: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    if ( !victim->in_room )
    {
	bug( "Mptransfer - Victim in Limbo: vnum %d.",
	    ch->pIndexData->vnum );
	return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );

    char_from_room( victim );
    char_to_room( victim, location );
    last_fun( "do_prgtransfer");

    return;
}

/*
 * lets the mobile force someone to do something.  must be mortal level
 *  and the all argument only affects those in the room with the mobile
 */
void do_prgforce( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpforce - Bad syntax: vnum %d.", ch->pIndexData->vnum );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == ch->in_room
		&& get_trust( vch ) < get_trust( ch ) 
		&& can_see( ch, vch ) )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    bug( "Mpforce - No such victim: vnum %d.",
	  	ch->pIndexData->vnum );
	    return;
	}

	if ( victim == ch )
    	{
	    bug( "Mpforce - Forcing oneself: vnum %d.",
	    	ch->pIndexData->vnum );
	    return;
	}

	interpret( victim, argument );
    }

    return;
}

// Instead of using the percentile value, this code will use that value as the starting
// point for the movement script.  The movement script will be nwseud in one long string
// and I have no idea how in the hell I'm going to make it work.
//
// There should be some provision for the mob to return to its original room if it gets knocked
// out of the script and isn't busy hunting down and killing someone.
//
// It should abort the script upon any event that would take it out of the area it would be walking
// through.
//
// This will be difficult - Veygoth
bool prog_move_script( CHAR_DATA *ch )
{
  MOVE_SCRIPT *script;
  ROOM_INDEX_DATA *room;
  char letter;

  last_fun( "prog_move_script" );

  if( !IS_NPC( ch ) || !(script = ch->pIndexData->move_script) )
  {
    return FALSE;
  }

  if( !script->script || script->script[0] == '\0' )
  {
    return FALSE;
  }

  letter = script->script[script->pos];

  if( letter == '\0' || script->script[script->pos+1] == '\0' )
  {
    script->pos = 0;
  }

  if(!(room = ch->in_room))
  {
    bug( "Ch cannot use a movement script when not in any room.", 0 );
    return FALSE;
  }

  script->pos++;

  switch( letter )
  {
    default:
      bug( "Bad letter in movement script", 0 );
      break;
    case '.':
      return TRUE;
      break;
    case 'n':
      do_north( ch, "" );
      break;
    case 's':
      do_south( ch, "" );
      break;
    case 'e':
      do_east( ch, "" );
      break;
    case 'w':
      do_west( ch, "" );
      break;
    case 'u':
      do_up( ch, "" );
      break;
    case 'd':
      do_down( ch, "" );
      break;
  }

  if( ch->in_room != room )
  {
    return TRUE;
  }
  else
  {
    script->pos = 0;
    script->running = FALSE;
    return FALSE;
  }

  return FALSE;
}
