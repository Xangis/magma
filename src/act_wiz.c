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
#include "olc.h"

/* Conversion of Immortal powers to Immortal skills done by Thelonius */
void restore( CHAR_DATA *ch, CHAR_DATA *victim );

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];
    int        cmd;
    int        col;

    last_fun( "do_wizhelp");
    rch = get_char( ch );
    
    if ( !authorized( rch, "wizhelp" ) )
        return;

    buf1[0] = '\0';
    col     = 0;

    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level < LEVEL_HERO
	    || str_infix( cmd_table[cmd].name, rch->pcdata->immskll ) )
	    continue;

	sprintf( buf, "%-10s", cmd_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 8 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 8 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    last_fun("do_bamfin");
    rch = get_char( ch );

    if ( !authorized( rch, "bamfin" ) )
        return;

    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    last_fun("do_bamfout");
    rch = get_char( ch );

    if ( !authorized( rch, "bamfout" ) )
        return;

    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun("do_deny");
    rch = get_char( ch );

    if ( !authorized( rch, "deny" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT( victim->act, PLR_DENY );
    send_to_char( "You are denied access!\n\r", victim );
    send_to_char( "OK.\n\r", ch );
    if ( victim->level <= 1 )
    do_quit( victim, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    last_fun("do_disconnect");
    rch = get_char( ch );

    if ( !authorized( rch, "disconnect" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
//    char       buf  [ MAX_STRING_LENGTH ];

    last_fun("do_pardon");
    rch = get_char( ch );

    if ( !authorized( rch, "pardon" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    send_to_char( "Justice system not complete, pardon command inoperable.\n\r", ch );

    return;
}



void do_echo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    last_fun("do_echo");    
    rch = get_char( ch );

    if ( !authorized( rch, "echo" ) )
        return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Echo what?\n\r", ch );
	return;
    }

    strcat( argument, "\n\r" );
    send_to_all_char( argument );

    return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;

    last_fun("do_recho");    
    rch = get_char( ch );

    if ( !authorized( rch, "recho" ) )
        return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Recho what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	    && d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}



ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;

    if ( is_number( arg ) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) )
	return obj->in_room;

    return NULL;
}

void do_transfer( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_transfer");
    rch = get_char( ch );

    if ( !authorized( rch, "transfer" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character ) )
	    {
		char buf [ MAX_STRING_LENGTH ];

		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
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
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->in_room )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );
    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL,   TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    last_fun( "do_transfer");
    act( "$n arrives from a puff of smoke.",   victim, NULL, NULL,   TO_ROOM );
    if ( ch != victim )
	act( "$n has transferred you.",        ch,     NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    last_fun( "do_transfer");
    send_to_char( "Ok.\n\r", ch );
}

void do_at( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *wch;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char             arg [ MAX_INPUT_LENGTH ];
    
    last_fun( "do_at");
    rch = get_char( ch );

    if ( !authorized( rch, "at" ) )
        return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    last_fun( "do_at");
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}

void do_goto( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    char             arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_goto");
    rch = get_char( ch );

    if ( !authorized( rch, "goto" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( ch->fighting )
	stop_fighting( ch, TRUE );
    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
	    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
    }

    char_from_room( ch );
    char_to_room( ch, location );
    last_fun( "do_goto");

    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
	    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
    }

    do_look( ch, "auto" );
    return;
}

void do_stat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];

    last_fun( "do_stat");
    rch = get_char( ch );

    if ( !authorized( rch, "stat" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Try statting a MOBILE, OBJECT, ROOM, SKILL, SPELL, AREA, CLAN,\n\r", ch );
        send_to_char( "CLASS, MEMDATA, SHOP, QUEST, CASTLIST, TRAPS, HUNT, HATE,\n\r", ch );
        send_to_char( "CRIME, VEHICLE, MONK, RACE, or FEAR\n\r", ch );
	return;
    }

    if( !str_prefix( arg1, "mobile" ))
       do_mstat( ch, argument );
    else if( !str_prefix( arg1, "object" ))
       do_ostat( ch, argument );
    else if( !str_prefix( arg1, "room" ))
       do_rstat( ch, argument );
    else if( !str_prefix( arg1, "skill" ))
       do_slookup( ch, argument );
    else if( !str_prefix( arg1, "spell" ))
       do_splookup( ch, argument );
    else if( !str_prefix( arg1, "area" ))
       do_astat( ch, argument );
    else if( !str_prefix( arg1, "zone" ))
       do_astat( ch, argument );
    else if( !str_prefix( arg1, "clan" ))
       do_showclan( ch, argument );
    else if( !str_prefix( arg1, "class" ))
       do_clookup( ch, argument );
    else if( !str_prefix( arg1, "quest" ))
       do_qstat( ch, argument );
    else if( !str_prefix( arg1, "memdata" ))
       do_memdata( ch, argument );
    else if( !str_prefix( arg1, "shop" ))
       do_shopstat( ch, argument );
    else if( !str_prefix( arg1, "desc" ))
       do_showdesc( ch, argument );
    else if( !str_prefix( arg1, "castlist" ))
       do_castlist( ch, argument );
    else if( !str_prefix( arg1, "traps" ))
       do_traps( ch, argument );
    else if( !str_prefix( arg1, "hunt" ))
       do_stathunt( ch, argument );
    else if( !str_prefix( arg1, "hate" ))
       do_stathate( ch, argument );
    else if( !str_prefix( arg1, "crime" ))
       do_statcrime( ch, argument );
    else if( !str_prefix( arg1, "vehicle" ))
       do_statvehicle( ch, argument );
    else if( !str_prefix( arg1, "fear" ))
       do_statfear( ch, argument );
    else if( !str_prefix( arg1, "monk" ))
       do_monkstat( ch, argument );
    else if( !str_prefix( arg1, "group" ) )
        do_gstat( ch, argument );
    else if( !str_prefix( arg1, "race" ))
        do_racestat( ch, argument );
    else
    {
	send_to_char( "Try statting a MOBILE, OBJECT, ROOM, SKILL, SPELL, AREA, CLAN,\n\r", ch );
        send_to_char( "CLASS, MEMDATA, SHOP, QUEST, CASTLIST, TRAPS, HUNT, HATE,\n\r", ch );
        send_to_char( "CRIME, VEHICLE, MONK, RACE, or FEAR\n\r", ch );
    }

    return;
}

void do_castlist( CHAR_DATA *ch, char *argument )
{
    CAST_DATA *caster;
    int total = 0;
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_castlist");
    for( caster = cast_list; caster; caster = caster->next )
    {
       total++;
    }
    sprintf( buf, "%d total in the casting list.\n\r", total );
    send_to_char( buf, ch );
}

// This function written purely for future debugging purposes
// It shows the char_list in exactly the order that the game runs through
// it.
void do_charlist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA* wch;
    int count = 0;

    last_fun( "do_charlist");
    for( wch = char_list; wch; wch = wch->next )
    {
       count++;
       sprintf( buf, "%d: %s\n\r", count, wch->name );
       send_to_char( buf, ch );
    }

    return;
}

void do_stathunt( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH * 3];
    CHAR_DATA* wch;
    int count = 0;

    last_fun( "do_stathunt");
    buf1[0] = '\0';

    for( wch = char_list; wch; wch = wch->next )
    {
       if( wch->deleted || !IS_NPC( wch ) || !wch->hunting )
         continue;
       count++;
       if( count < 100 )
       {
         sprintf( buf, "%d: %s&n is hunting %s&n.\n\r", count,
            PERS(wch, ch), PERS(ch, wch) );
         strcat( buf1, buf );
       }
    }
    sprintf( buf, "Found %d total hunting mobs.\n\r", count );
    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}

void do_stathate( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH * 3];
    CHAR_DATA* wch;
    int count = 0;

    last_fun( "do_stathate");
    buf1[0] = '\0';

    for( wch = char_list; wch; wch = wch->next )
    {
       if( wch->deleted || !IS_NPC( wch ) || !wch->hating )
         continue;
       count++;
       if( count < 100 )
       {
         sprintf( buf, "%d: %s&n is hating %s&n.\n\r", count,
            PERS(wch, ch), PERS(wch->hating->who, wch) );
         strcat( buf1, buf );
       }
    }
    sprintf( buf, "Found %d total hating mobs.\n\r", count );
    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}

void do_statfear( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH * 3];
    CHAR_DATA* wch;
    int count = 0;

    buf1[0] = '\0';

    for( wch = char_list; wch; wch = wch->next )
    {
       if( wch->deleted || !IS_NPC( wch ) || !wch->fearing )
         continue;
       count++;
       if( count < 100 )
       {
         sprintf( buf, "%d: %s&n is fearing %s&n.\n\r", count,
            PERS(wch, ch), PERS(ch, wch) );
         strcat( buf1, buf );
       }
    }
    sprintf( buf, "Found %d total fearing mobs.\n\r", count );
    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}

void do_traps( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA * obj;
    int count = 0;

    for( obj = object_list; obj; obj = obj->next )
    {
       if( !obj->trap )
         continue;
       count++;
       if( count < 100 )
       {
         sprintf( buf, "%d: %d trig,%d dmg,%d charges,%d lvl,%d percent\n\r", obj->pIndexData->vnum,
           obj->trap->trigger, obj->trap->damage, obj->trap->charges,
           obj->trap->level, obj->trap->percent );
         send_to_char( buf, ch );
       }
    }
    sprintf( buf, "There are %d total traps in the game.\n\r", count );
    send_to_char( buf, ch );

    return;
}

void do_find( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "lookup" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Try looking up a MOBILE or OBJECT.\n\r", ch );
	return;
    }

    if( !str_prefix( arg1, "mobile" ))
       do_mfind( ch, argument );
    else if( !str_prefix( arg1, "object" ))
       do_ofind( ch, argument );
    else if( !str_prefix( arg1, "room" ))
       do_rfind( ch, argument );
    else
	send_to_char( "Try looking up a MOBILE, OBJECT, or ROOM.\n\r", ch );

    return;
}

void do_where( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "where" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Try where-ing a MOBILE, CHAR, or OBJECT, or WHERE ALL for all players.\n\r", ch );
	return;
    }

    if( !str_prefix( arg1, "mobile" ))
       do_mwhere( ch, argument );
    else if( !str_cmp( arg1, "char" ) )
       do_pwhere( ch, argument );
    else if( !str_prefix( arg1, "object" ))
       do_owhere( ch, argument );
    else if ( !str_prefix( arg1, "all" ))
       do_pwhere( ch, "all" );
    else
	send_to_char( "Try where-ing a MOBILE, CHAR, or OBJECT, or WHERE ALL for all players.\n\r", ch );

    return;
}

void do_set( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "set" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "&+cTry setting an OBJECT, MOBILE, ROOM, SKILL, SPELL, MONKSKILL, or CLAN.\n\r", ch );
	return;
    }

    if( !str_prefix( arg1, "object" ))
       do_oset( ch, argument );
    else if( !str_prefix( arg1, "mobile" ))
       do_mset( ch, argument );
    else if( !str_prefix( arg1, "room" ))
       do_rset( ch, argument );
    else if( !str_prefix( arg1, "skill" ))
       do_sset( ch, argument );
    else if( !str_prefix( arg1, "language" ))
       do_lset( ch, argument );
    else if( !str_prefix( arg1, "spell" ))
       do_splset( ch, argument );
    else if( !str_prefix( arg1, "monkskill" ))
       do_monk_sset( ch, argument );
    else if( !str_prefix( arg1, "clan" ) || !str_prefix( arg1, "guild" ))
       do_setclan( ch, argument );
    else
	send_to_char( "Try setting an OBJECT, MOBILE, ROOM, SKILL, SPELL, MONKSKILL, or CLAN.\n\r", ch );

    return;
}

void do_load( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "load" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Try loading a MOBILE or OBJECT.\n\r", ch );
	return;
    }

    if( !str_prefix( arg1, "mobile" ))
       do_mload( ch, argument );
    else if( !str_prefix( arg1, "object" ))
       do_oload( ch, argument );
    else
	send_to_char( "Try loading a MOBILE or OBJECT.\n\r", ch );

    return;
}

void do_astat( CHAR_DATA *ch, char *argument )
{
    AREA_DATA       *pArea;
    ROOM_INDEX_DATA *room;  
    OBJ_DATA        *obj;  
    EXIT_DATA       *pexit;  
    char             buf [ MAX_STRING_LENGTH ];
    int		     lower, upper, count, door;

    smash_tilde( argument );

    pArea = ch->in_room->area;

    if ( pArea == NULL )
    {
        bug("do_astat: char not in a valid area.", 0);
	send_to_char("You are not in a valid area!", ch);
	return;
    }

    sprintf( buf, "Name:     [%5d] '%s'.\n\r",
	    pArea->vnum, pArea->name );
    send_to_char( buf, ch );

    sprintf( buf, "Range:    {%2d %2d}  Author:   %s.\n\r",
	    pArea->llv, pArea->ulv, pArea->author );
    send_to_char( buf, ch );

    sprintf( buf, "File:     %s.\n\r",
	    pArea->filename );
    send_to_char( buf, ch );

    sprintf( buf, "Age:      [%5d]  Players: [%5d]  Security: [%5d]\n\r",
	    pArea->age, pArea->nplayer, pArea->security );
    send_to_char( buf, ch );

    sprintf( buf, "Vnums:    %d - %d\n\r",
	    pArea->lvnum, pArea->uvnum );
    send_to_char( buf, ch );

    sprintf( buf, "Rooms:    %d - %d Objs: %d - %d Mobs: %d - %d\n\r",
	    pArea->low_r_vnum, pArea->hi_r_vnum, pArea->low_o_vnum,
	    pArea->hi_o_vnum, pArea->low_m_vnum, pArea->hi_m_vnum );
    send_to_char( buf, ch );

    sprintf( buf, "Recall:   [%5d] '%s'.&n\n\r", pArea->recall,
	    get_room_index( pArea->recall )
	    ? get_room_index( pArea->recall )->name
	    : "none" );
    send_to_char( buf, ch );

    sprintf( buf, "Flags:    %s          Resetmode: %d\n\r",
	    flag_string( area_flags, pArea->area_flags ), pArea->resetmode );
    send_to_char( buf, ch );

    sprintf( buf, "Reset:    %s\n\r", pArea->resetmsg );
    send_to_char( buf, ch );

    sprintf( buf, "Reset lifespan: %d to %d minutes      Time since reset: %d\n\r",
        pArea->llifespan*5, pArea->ulifespan*5, pArea->age*5);
    send_to_char( buf, ch );

    sprintf( buf, "Builders: %s\n\r", pArea->builders );
    send_to_char( buf, ch );

    if( pArea->justice )
      send_to_char( "This area is controlled by justice.\n\r", ch );

    if( pArea->defender )
      sprintf( buf, "Defender Vnum: %d  Number Spawned During Invasion: %d\n\r", pArea->defender, pArea->defender_num );
    send_to_char( buf, ch );

    sprintf( buf, "Judge Room: %d  Jail Room: %d  Barracks: %d  Defenders Dispatched: %d/%d (%d squads)\n\r",
                  pArea->judge, pArea->jail, pArea->barracks, pArea->dispatched,
                  (pArea->defender_num * 5), pArea->squads );
    send_to_char( buf, ch );

/* This code checks for exits from zone and bad exits. */  
    lower = pArea->low_r_vnum;  
    upper = pArea->hi_r_vnum;  
    for (count = lower;count <= upper;count++) 
    { 
    if ( ( room = get_room_index(count) ) ) 
    { 
	for (door = 0;door < MAX_DIR;door++) 
	{ 
	if ( ( pexit = room->exit[door] ) ) 
	{ 
	    int vn = pexit->to_room ? pexit->to_room->vnum : -1; 
	    if (vn == -1) 
	    { 
		// Bad exit  
		sprintf(buf, "Exit %s in room %d leads to nothing.\n\r",   
		        capitalize(dir_name[door]),  
			room->vnum ? room->vnum : -1); 
		send_to_char(buf, ch); 
	    } 
	    else
	    {
		// Check to see outta zone 
		if (vn < lower || vn > upper)
		{
		sprintf(buf, "Exit %s in room %d leads to zone %s&n(%d).\n\r",   
		    capitalize(dir_name[door]), room->vnum,
		    pexit->to_room->area->name, pexit->to_room->area->vnum);
		send_to_char(buf, ch);
		}
	    }
	}
	}
    }
    }

/* Adding check through objects to check for portals. */
    lower = pArea->low_o_vnum;
    upper = pArea->hi_o_vnum;
    for (count = lower;count <= upper;count++)
    {
    if ( ( obj = get_obj_type(get_obj_index(count)) ) )
    {
	if ( obj->item_type == TYPE_PORTAL &&
	     ( room = get_room_index(obj->value[0]) ) )
	{
	    if ( obj->value[0] < pArea->low_r_vnum || 
		 obj->value[0] > pArea->hi_r_vnum )
	    {   /* Display portal destn. room etc. */
	    	if ( obj->in_room == NULL)
		{
		if ( obj->pIndexData )
		{
		    bug("do_astat: Object %d is portal w/out a room", 
				obj->pIndexData->vnum );
		    continue;
		}
		else
		{
		    bug("do_astat: Object is portal w/out a room", 0 );
		    continue;
		}
		}
		if ( room->area == NULL )
		{
		    bug("do_astat: Room %d is not in any area!", room->vnum);
		    continue;
		}
		sprintf(buf, "Portal %s&n in room %s&n(%d) leads to zone %s&n(%d).\n\r",   
		        obj->name, obj->in_room->name, obj->in_room->vnum,
			room->area->name, room->area->vnum); 
		send_to_char(buf, ch); 
	    }
	}
    }
    }
    return;
}

void do_qstat( CHAR_DATA *ch, char *argument )
{
    QUEST_DATA *quests;
    TALK_DATA *talk;
    QUEST_ITEM *item;
    QUEST_INDEX_DATA *quest;
    char buf[MAX_STRING_LENGTH];
    int number;

    if( ch->level < L_JUN )
    {
       send_to_char( "You are too low level to stat quests.\n\r", ch );
       return;
    }

    if( !quest_first )
    {
       send_to_char( "No quests to stat.\n\r", ch );
       return;
    }

    if( argument[0] == '\0' )
    {
      for( quest = quest_first; quest; quest = quest->next )
      {
         sprintf( buf, "Quest for mob %d has %s and %s", quest->vnum,
                 quest->quests ? "quest data" : "no quest data",
                 quest->messages ? "message data\n\r" : "no message data\n\r" );
         send_to_char( buf, ch );
      }
    }
    else if( !is_number( argument ))  
    {
      send_to_char( "That's not a number.\n\r", ch );
      return;
    }
    else
    {
      number = atoi( argument );
      if( number < 1 )
      {
         send_to_char( "That's not a valid number.\n\r", ch );
         return;
      }
      for( quest = quest_first; quest; quest = quest->next )
      {
         if( quest->vnum != number )
           continue;
         sprintf( buf, "Quest for mob %d:\n\r", quest->vnum );
         send_to_char( buf, ch );
         for( talk = quest->messages; talk; talk = talk->next )
         {
           sprintf( buf, "Keywords: %s\n\rMessage:\n\r%s\n\r",
              talk->keywords, talk->message );
           send_to_char( buf, ch );
         }
         for( quests = quest->quests; quests; quests = quests->next )
         {
           send_to_char( "Subquest data:\n\r  Give items to mob:\n\r", ch );
           for( item = quests->receive; item; item = item->next )
           {
              sprintf( buf, "  Type: %d  Number:  %d  Complete: %d\n\r",
                item->type, item->value, item->complete );
              send_to_char( buf, ch );
           }
           send_to_char( "  Give items to player:\n\r", ch );
           for( item = quests->give; item; item = item->next )
           {
              sprintf( buf, "  Type: %d  Number:  %d  Complete: %d\n\r",
                item->type, item->value, item->complete );
              send_to_char( buf, ch );
           }
           sprintf( buf,"  Completion message: \n\r%s&n\n\r  Disappearance message: %s&n\n\r",
              quests->complete, quests->disappear );
           send_to_char( buf, ch );
         }
      }
    }

    return;
}

void do_memdata( CHAR_DATA *ch, char *argument )
{
	MEM_DATA *mem;
        int count = 0;
        char buf[MAX_STRING_LENGTH];
 
        for( mem = memdata_free; mem; mem = mem->next )
        {
           count++;
        }

        sprintf( buf, "There are %d free memorization data of %d total.\n\r",
                 count, top_memdata );

        send_to_char( buf, ch );

        if( ch->pcdata->memorized )
        {
        for( mem = ch->pcdata->memorized; mem; mem = mem->next )
        {
          sprintf( buf, "Sn: %d.  Name: %s. Memtime: %d pulses.\n\r  Full Memtime: %d pulses.  Circle: %d.  Memmed: %d.\n\r",
                   mem->sn,
                   spells_table[mem->sn].name,
                   mem->memtime,
                   mem->full_memtime,
                   mem->circle,
                   mem->memmed );
          send_to_char( buf, ch );
        }
        }
        else
          send_to_char( "You have no associated memorization data.\n\r", ch );

        return;
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA        *obj;
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    char             buf  [ MAX_STRING_LENGTH ];
    char             buf1 [ MAX_STRING_LENGTH ];
    char             arg  [ MAX_INPUT_LENGTH  ];
    int              door;

    rch = get_char( ch );

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    buf1[0] = '\0';
    sprintf( buf, "Name: '%s&N'.\n\rArea: [%5d] '%s&n'.\n\r",
	    location->name,
	    location->area->vnum,
	    location->area->name );
    strcat( buf1, buf );

    sprintf( buf,
	    "Vnum: %d.  Light: %d.  Sector: %s.\n\r",
	    location->vnum,
	    location->light,
	    flag_string( sector_flags, location->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "Temp Room flags: %d %d: %s.\n\r",
            location->room_flags[0], location->room_flags[1],
	    room_string( location ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "Orig Room flags: %d.\n\rDescription:\n\r%s&n",
            location->orig_room_flags,
	    location->description );
    strcat( buf1, buf );

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "'.\n\r" );
    }

    strcat( buf1, "Characters:" );
    /* Yes, we are reusing the variable rch.  - Kahn */
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
      /* Heh.  Thanks to Zavod for this little security fix */ 
      if ( can_see( ch, rch ) )
	{
	  strcat( buf1, " " );			 
	  one_argument( rch->name, buf );	 
	  strcat( buf1, buf );			 
	}
    }

    strcat( buf1, ".\n\rObjects:   " );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	strcat( buf1, " " );
	one_argument( obj->name, buf );
	strcat( buf1, buf );
    }
    strcat( buf1, ".\n\r" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) )
	{
	    sprintf( buf,
		    "%-5s:  To: %d.  Key: %d.  Exit flags: %s (%d).\n\r",
		    capitalize( dir_name[door] ),
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    flag_string( exit_flags, pexit->exit_info ),
                    pexit->exit_info );
	    strcat( buf1, buf );
	    sprintf( buf,
		    "Keyword: '%s'.  Description: %s",
		    pexit->keyword,
		    pexit->description[0] != '\0' ? pexit->description
		                                  : "(none).\n\r" );
	    strcat( buf1, buf );
	}
    }

/* Added display of percent chance of falling. */
    if (location->exit[DIR_DOWN]) 
    { 
        sprintf( buf, "Percent chance of falling: %d.\n\r", 
                location->fall_chance ); 
        strcat( buf1, buf ); 
    } 
	sprintf( buf, "Extra Sector Info:    %d.\n\r",
		location->extra_sector_info);
	strcat( buf1, buf );
        if ( location->vnum > SURFACE_MAP1_START_VNUM ) {
            //dddd
          sprintf( buf, "Extended sector: %d\n\r%s%s\n\rMain sector: %d  Map char: %s    Visibility: %d\n\r",
          map_info[location->extra_sector_info].extended_sector,
          map_info[location->extra_sector_info].room_title,
          map_info[location->extra_sector_info].room_description,
          map_info[location->extra_sector_info].main_sector,
          map_info[location->extra_sector_info].map_char,
          map_info[location->extra_sector_info].visibility);
          strcat( buf1, buf );

        }

    send_to_char( buf1, ch );
    return;
}

void do_ostat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA       *obj;
    AFFECT_DATA    *paf;
    OBJ_INDEX_DATA *pObjIndex;
    int             value;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf1 [ MAX_STRING_LENGTH ];
    char            arg  [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ostat what?\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    if ( !( obj = get_obj_world( ch, arg ) ) && !( value = atoi( arg ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }
    if ( !obj )
    {
        if ( !( pObjIndex = get_obj_index( value ) ) )
        {
            send_to_char( "Invalid object vnum.\n\r", ch);
	    return;
        }
        if ( !( obj = get_obj_type( pObjIndex ) ) )
        {
            send_to_char( "None of those in game.\n\r", ch);
            return;
        }
    }

    sprintf( buf, "Name: %s.\n\r",
	    obj->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Type: %d - %s.\n\r",
	    obj->pIndexData->vnum, obj->item_type, item_type_name( obj ) );
    strcat( buf1, buf );

    sprintf( buf, "Short description: %s&n.\n\rLong description: %s&n\n\r",
	    obj->short_descr, obj->description );
    strcat( buf1, buf );

    sprintf( buf, "Wear bits: %d.  Extra bits: %s (%d %d).\n\r",
	    obj->wear_flags, item_string( obj ), obj->extra_flags[0],
            obj->extra_flags[1] );
    strcat( buf1, buf );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.  Size: %d.  Volume: %d.\n\r",
	    1,           get_obj_number( obj ),
	    obj->weight, get_obj_weight( obj ),
            obj->size, obj->volume );

    sprintf( buf, "Material Type: %d. (%s).  Craftsmanship: %d.\n",
             obj->material,
	     flag_string( material_flags, obj->material ),
             obj->craftsmanship );
    strcat( buf1, buf );

    sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.  Number Loaded: %d.\n\r",
	    obj->cost, obj->timer, obj->level, obj->pIndexData->count );
    strcat( buf1, buf );

/* Added wear_location in text, not int format. */
    sprintf( buf,
	    "In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %s.\n\r",
	    obj->in_room    ? obj->in_room->vnum : 0,
	    obj->in_obj     ? obj->in_obj->short_descr : "(none)",
	    obj->carried_by ? obj->carried_by->name : "(none)",
	    flag_string( wear_loc_strings, obj->wear_loc )); // Had to change this - V
    strcat( buf1, buf );

/* Added all eight values. */
    sprintf( buf, "Values: %d %d %d %d %d %d %d %d.\n\r",
	    obj->value[0], obj->value[1], obj->value[2], 
	    obj->value[3], obj->value[4], obj->value[5],
	    obj->value[6], obj->value[7] );
    strcat( buf1, buf );

    if ( obj->extra_descr || obj->pIndexData->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: " );

	for ( ed = obj->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}

	for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}

	strcat( buf1, ".\n\r" );
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d, Bits %s.\n\r",
		affect_loc_name( paf->location ), paf->modifier,
		affect_string( paf->bitvector, FALSE ) );
	strcat( buf1, buf );
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Index Affects %s by %d, Bits %s.\n\r",
		affect_loc_name( paf->location ), paf->modifier,
		affect_string( paf->bitvector, FALSE ) );
	strcat( buf1, buf );
    }

/* Added a marked for deletion check. (debugging) */
    if ( obj->deleted )
    {
        sprintf( buf, "Object is marked for deletion.\n\r" );
	strcat( buf1, buf );
    }

/* Added display of object's condition. */
/* 	This will need to be modified from int to a char string, once
	  a table exists for it.  */
    sprintf( buf, "Object is in %d condition.\n\r", obj->condition);
    strcat( buf1, buf );

/* Added display of anti_flags. */
/* 	This will need to be modified from an int to a char string, once
	  a table exists for it.  */
    if ( obj->anti_flags != 0 )
        sprintf( buf, "Object anti_flags: %d\n\r", obj->anti_flags);
    else
	sprintf(buf, "Object has no anti_flags.\n\r");
    strcat( buf1, buf );

/* Added display of anti_flags2. */
/* 	This will need to be modified from an int to a char string, once
 *	  a table exists for it.  */
    if ( obj->anti_flags2 != 0 )
        sprintf( buf, "Object anti_flags2: %d\n\r", obj->anti_flags2);
    else
	sprintf(buf, "Object has no anti_flags2.\n\r");
    strcat( buf1, buf );

    if ( obj->spec_fun )
    {
       sprintf( buf, "Special function: %s.\n\r", 
            spec_obj_string( obj->spec_fun ) );
       strcat( buf1, buf );
    }

    if( obj->pIndexData->objprogs )
    {
        sprintf( buf, "Object has an objprog.\n\r" );
        strcat( buf1, buf );
    }

/* Added display of affected_by array. */
    sprintf( buf, "Affected by: %s.\n\r",
	    affect_string( obj->affected_by, FALSE ) );
    strcat( buf1, buf );

    send_to_char( buf1, ch );
    return;
}

void do_shopstat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA     *obj;
    CHAR_DATA    *keeper;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];
    int          count;

    one_argument( argument, arg );

    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
        if ( IS_NPC( keeper ) && ( keeper->pIndexData->pShop ) )
            break;
    }
 
    if( !keeper )
    {
        send_to_char( "There is no shop in this room.\n\r", ch );
        return;
    }

    sprintf( buf, "Keeper: %d.  Name: %s&n\n\r",
        keeper->pIndexData->vnum,
        keeper->short_descr );
    
    sprintf( buf1, "Profit Buy: %d.  Profit Sell: %d.\n\r",
        keeper->pIndexData->pShop->profit_buy,
        keeper->pIndexData->pShop->profit_sell );
    strcat( buf, buf1 );

    sprintf( buf1, "Open Hour: %d.   Close Hour: %d.\n\r",
        keeper->pIndexData->pShop->open_hour,
        keeper->pIndexData->pShop->close_hour );
    strcat( buf, buf1 );

    strcat( buf, "Types of item shop buys: " );
    for( count = 0; count < MAX_BUY; count++ )
    {
        if( keeper->pIndexData->pShop->buy_type[count] == 0 )
          break;
        sprintf( buf1, " %d", keeper->pIndexData->pShop->buy_type[count] );
        strcat( buf, buf1 );
    }
    strcat( buf, ".\n\r" );

    strcat( buf, "Permanent items shop sells:\n\r" );
    for( count = 0; count < MAX_SELL; count++ )
    {
        if( keeper->pIndexData->pShop->sell_item[count] == 0 )
          break;
        sprintf( buf1, " %d.\n\r", keeper->pIndexData->pShop->sell_item[count] );
        strcat( buf, buf1 );
    }
    strcat( buf, ".\n\r" );

    strcat( buf, "Temporary items shop is selling:\n\r" );
    for ( obj = keeper->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc != WEAR_NONE )
            continue;
        sprintf( buf1, "%s (%d).\n\r",
            obj->short_descr, obj->pIndexData->vnum );
        strcat( buf, buf1 );
    }
 
    send_to_char( buf, ch );
}

void do_gstat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *rch;
    CHAR_DATA   *victim, *gch, *leader;
    char         buf  [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];
    char         buf1 [ MAX_STRING_LENGTH * 3 ];

    last_fun( "do_gstat" );
    rch = get_char( ch );

    one_argument( argument, arg );

    if ( arg[0]  )
    {
        victim = get_char_room( rch, arg );
        if ( !victim ) victim = get_char_world( rch, arg );
        if ( !victim ) {
            send_to_char( "You can't find that person anywhere.\n\r", rch );
            return;
        }
        if ( !victim->group_leader ) {
            send_to_char( "They aren't in a group.\n\r", rch );
            return;
        }
        leader = victim->group_leader;
        sprintf( buf1, "&+G%s's group:&n\n\r", leader->name);
        for( gch = leader; gch; gch = gch->next_in_group) {
            sprintf( buf, "     &+G%s&n\n\r", gch->name);
            strcat( buf1, buf );
        }
        send_to_char( buf1, rch );
        return;
    }
    buf1[0] = 0;
    for ( victim = char_list; victim; victim = victim->next) {
        if ( victim->deleted || victim != victim->group_leader)
            continue;
        leader = victim->group_leader;
        sprintf( buf, "&+G%s's group:&n\n\r", leader->name);
        strcat(buf1, buf);
        for( gch = leader; gch; gch = gch->next_in_group) {
            sprintf( buf, "     &+G%s&n\n\r", gch->name);
            strcat( buf1, buf );
        }
    }
    send_to_char( buf1, rch );
    return;
}


void do_mstat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    AFFECT_DATA *paf;
    OBJ_DATA    *obj;
    MOB_INDEX_DATA *pMobIndex;
    int          count;
    int          value;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];

    last_fun( "do_mstat" );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mstat whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) && !(value = atoi( arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !victim )
    {
        if ( !( pMobIndex = get_mob_index( value ) ) )
        {
            send_to_char( "No mob has that vnum.\n\r", ch );
            return;
        }
        for ( victim = char_list; victim; victim = victim->next )
            if ( victim->pIndexData == pMobIndex )
                break;
        if ( !victim )
        {
            send_to_char( "No mob has that vnum.\n\r", ch );
            return;
        }
    }

    buf1[0] = '\0';

    sprintf( buf, "Name: %s.\n\r",
	    victim->name );
    strcat( buf1, buf );

    sprintf( buf, "Race: %s (%d).\n\r", race_table[victim->race].colorname,
            victim->race );
    strcat( buf1, buf );

    sprintf( buf, "Parts: %s.\n\r",
	    parts_bit_name( race_table[victim->race].parts ) );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Sex: %s.  Room: %d.  Load room: %d.\n\r",
	    IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
	    victim->sex == SEX_MALE    ? "male"   :
	    victim->sex == SEX_FEMALE  ? "female" : "neuter",
	    !victim->in_room           ?        0 : victim->in_room->vnum,
            victim->load_room_vnum );
    strcat( buf1, buf );

    sprintf( buf, "&+WBase Stats&n  Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\n\r",
	    victim->perm_str,
	    victim->perm_int,
	    victim->perm_wis,
	    victim->perm_dex,
	    victim->perm_con );
    strcat( buf1, buf );

    sprintf( buf, "            Agi: %d.  Cha: %d.  Pow: %d.  Luk: %d.\n\r",
	    victim->perm_agi,
	    victim->perm_cha,
	    victim->perm_pow,
	    victim->perm_luk );
    strcat( buf1, buf );

    sprintf( buf, "&+WActual Stats&n Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\n\r",
	    get_curr_str( victim ),
	    get_curr_int( victim ),
	    get_curr_wis( victim ),
	    get_curr_dex( victim ),
	    get_curr_con( victim ) );
    strcat( buf1, buf );

    sprintf( buf, "            Agi: %d.  Cha: %d.  Pow: %d.  Luk: %d.\n\r",
	    get_curr_agi( victim ),
	    get_curr_cha( victim ),
	    get_curr_pow( victim ),
	    get_curr_luk( victim ) );
    strcat( buf1, buf );


    sprintf( buf, "Hp: %d/%d (%d base).  Mana: %d/%d.  Move: %d/%d.\n\r",
	    victim->hit,         get_max_hit( victim ),  victim->max_hit,
	    victim->mana,        victim->max_mana,
	    victim->move,        victim->max_move );
    strcat( buf1, buf );
	
    sprintf( buf,
	"Lv: %d.  Class: %d (%s)&n.  Align: %d.  AC: %d.\n\r",
	    victim->level, victim->class,
	    class_table[victim->class]->who_name, 
	    victim->alignment, get_ac( victim ) );
    strcat( buf1, buf );

/* Govno: show current and default size */
    sprintf( buf, "Current Size:  %s (%d)  Default Size: %s (%d)\n\r",
                   size_string( victim->size ), victim->size,
                   size_string( race_table[ victim->race].size ), race_table[victim->race].size );
    strcat( buf1, buf );

/* Added position in text. */
    sprintf( buf, "Position: %s (%d).  Wimpy: %d.\n\r  Exp: %d/%d.  Coins: %d.",
             position_string( victim->position ),
             victim->position,
             victim->wimpy,
	     victim->exp, exp_table[victim->level].lvl_exp, get_cash( victim) );
    strcat( buf1, buf );

/* Always displays the Hit/Dam bonus for the char's primary weapon. */
    strcat ( buf1, "Primary Weapon: " );
    sprintf( buf, "Hitroll: %d  Damroll: %d.\n\r",
            get_hitroll( victim, WEAR_HAND ),
            get_damroll( victim, WEAR_HAND ) );
    strcat( buf1, buf );

/* Changed location of the if statement. */
/* If char has a weapon in second hand and has dual wield.. */
    if ( ( IS_NPC( victim )
        || victim->level >= skills_table[gsn_dual].skill_level[victim->class] )
    && ( (obj = get_eq_char( victim, WEAR_HAND_2 )) && (obj->item_type ==
          TYPE_WEAPON)) )
    {
	sprintf( buf, " Second Weapon Hitroll: %d  Damroll: %d.\n\r",
		get_hitroll( victim, WEAR_HAND_2 ),
		get_damroll( victim, WEAR_HAND_2 ) );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( victim ) && ch->pcdata )
    {
        sprintf( buf, "Currhome: %d, Orighome: %d, Lastrent: %d\n\r",
            victim->pcdata->currhome,victim->pcdata->orighome,victim->pcdata->lastrent);
        strcat( buf1, buf );
	sprintf( buf, "Page Lines: %d.\n\rLanguages learned:", victim->pcdata->pagelen );
	strcat( buf1, buf );
        for( count = 0; count < MAX_LANG; count ++ )
        {
          if( ch->pcdata->lang_lrn[count] > 0 )
          {
            sprintf( buf, " %s=%d", lang_table[count], victim->pcdata->lang_lrn[count] );
            strcat( buf1, buf );
          }
        }
        strcat( buf1, "\n\r" );
    }

    sprintf( buf, "Fighting: %s.\n\r",
	    victim->fighting ? victim->fighting->name : "(none)" );
    strcat( buf1, buf );

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
		"Thirst: %d.  Full: %d.  Drunk: %d.  Saving throw: %d %d %d %d %d.\n\r",
		victim->pcdata->condition[COND_THIRST],
		victim->pcdata->condition[COND_FULL  ],
		victim->pcdata->condition[COND_DRUNK ],
		victim->saving_throw[0],
		victim->saving_throw[1],
		victim->saving_throw[2],
		victim->saving_throw[3],
		victim->saving_throw[4]);
	strcat( buf1, buf );
	if ( is_clan( victim ) )
	{
	    sprintf( buf,
		    "Clan: %s.  Rank: %s.\n\r",
		    victim->pcdata->clan->name,
		    flag_string( rank_flags, victim->pcdata->rank ) );
	    strcat( buf1, buf );
	}
    }

    sprintf( buf, "Carry number: %d.  Carry weight: %d.\n\r",
	    victim->carry_number, victim->carry_weight );
    strcat( buf1, buf );

    sprintf( buf, "Age: %d.  Played: %d.  Timer: %d.\n\r",
	    get_age( victim ),
	    victim->played,
	    victim->timer );
    strcat( buf1, buf );

    if( IS_NPC( victim ))
    {
      sprintf( buf,"Act flags: %s (%d).\n\r",
	    flag_string( mob_act_flags, victim->act ), victim->act );
    }
    else
    {
      sprintf( buf,"Act flags: %s (%d).\n\r",
	    flag_string( plr_act_flags, victim->act ), victim->act );
    }
    strcat( buf1, buf );

    sprintf( buf, "Riding:  %s.  Rider:  %s.\n\r",
	    victim->riding      ? victim->riding->name   : "(none)",
	    victim->rider       ? victim->rider->name    : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "Master:  %s.  Leader: %s.\n\r",
	    victim->master      ? victim->master->name   : "(none)",
	    victim->group_leader? victim->group_leader->name   : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "Hunting: %s.  Hating: %s.  Fearing: %s.\n\r",
	    victim->hunting     ? victim->hunting->name  : "(none)",
	    victim->hating      ? victim->hating->name   : "(none)",
	    victim->fearing     ? victim->fearing->name  : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "Resistant:   %s.\n\r",
	    ris_bit_name( victim->resistant ) );
    strcat( buf1, buf );

    sprintf( buf, "Immune:      %s.\n\r",
	    ris_bit_name( victim->immune ) );
    strcat( buf1, buf );

    sprintf( buf, "Susceptible: %s.\n\r",
	    ris_bit_name( victim->susceptible ) );
    strcat( buf1, buf );

    sprintf( buf, "Wait state: %d (%d seconds)\n\r",
            victim->wait, (victim->wait / PULSE_PER_SECOND) );
    strcat( buf1, buf );

    sprintf( buf, "Affected by: %s.\n\r", affect_string( victim->affected_by, FALSE ) );
    strcat( buf1, buf );

    if ( !IS_NPC( victim ) )
    {
        sprintf( buf, "Security: %d.\n\r",
		victim->pcdata->security );
    }
    else
    {
        sprintf( buf, "Start Position: %s (%d).\n\r",
                position_string( victim->pIndexData->position ),
		victim->pIndexData->position );
    }
    strcat( buf1, buf );

    sprintf( buf, "Short description: %s&n.\n\rLong description: %s&n\n\r",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ? victim->long_descr
	                                  : "(none)." );
    strcat( buf1, buf );

/* Displays mob specials */
    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
    {
	sprintf( buf, "Mobile has %s.\n\r",
		spec_mob_string( victim->spec_fun ) );
        strcat( buf1, buf );
    }
    if( IS_NPC( victim ) && victim->spec_fun2 != 0 )
    {
        sprintf( buf, "Mobile has %s.\n\r",
                spec_mob_string( victim->spec_fun2 ) );
        strcat( buf1, buf );
    }
    if( IS_NPC( victim ) && victim->pIndexData->death_fun != 0 )
    {
        sprintf( buf, "Mobile has %s.\n\r",
                spec_mob_string( victim->pIndexData->death_fun ) );
        strcat( buf1, buf );
    }

    if( IS_NPC( victim ) && victim->pIndexData->mobprogs )
    {
        sprintf( buf, "Mobile has a mobprog.\n\r" );
        strcat( buf1, buf );
    }

/* Display for gambling. */
    if ( IS_NPC( victim )
	&& victim->pIndexData
	&& victim->pIndexData->pGame
	&& victim->pIndexData->pGame->game_fun != 0 )
    {
	sprintf( buf, "Mobile has %s.\n\r",
		game_string( victim->pIndexData->pGame->game_fun ) );
        strcat( buf1, buf );
    }

/* Displays skills/spells currently on victim. */
    for ( paf = victim->affected; paf; paf = paf->next )
    {
        // note that with this code if an affect has both a skill
        // and spell affect, they will both show up as individual affects.
        if ( paf->deleted )
	    continue;
        if( paf->skill )
        {
           sprintf( buf,
		"Skill: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
		skills_table[paf->skill].name,
		affect_loc_name( paf->location ),
		paf->modifier,
		paf->duration,
		affect_string( paf->bitvector, FALSE ) );
	   strcat( buf1, buf );
        }
        if( paf->spell )
        {
	   sprintf( buf,
		"Spell: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
		spells_table[paf->spell].name,
		affect_loc_name( paf->location ),
		paf->modifier,
		paf->duration,
		affect_string( paf->bitvector, FALSE ) );
	   strcat( buf1, buf );
        }
        if( paf->song )
        {
	   sprintf( buf,
		"Song: '%s' modifies %s by %d for %d hours with bits %s.\n\r",
		songs_table[paf->song].name,
		affect_loc_name( paf->location ),
		paf->modifier,
		paf->duration,
		affect_string( paf->bitvector, FALSE ) );
	   strcat( buf1, buf );
        }
        if ( !paf->skill && !paf->spell && !paf->song  ) {
            sprintf( buf, 
                "Unknown: modifies %s by %d for %d hours with bits %s.\n\r",
                affect_loc_name( paf->location ),
                paf->modifier,
                paf->duration,
                affect_string( paf->bitvector, FALSE ) );
            strcat( buf1, buf );
        }
    }

/* Added a marked for deletion check. (debugging) */
    if ( victim->deleted )
    {
        sprintf( buf, "Character is marked for deletion.\n\r" );
	strcat( buf1, buf );
    }

/* ADD deaf in text, once table is written for it. */
    sprintf( buf, "Deaf to: %d. ", victim->deaf);
    strcat( buf1, buf );
/* Added trust level. */
    sprintf( buf, "Trust level: %d.\n\r",
            victim->trust);
    strcat( buf1, buf );

/* Added # mobs killed, and # times killed by a mob. */
    if ( !IS_NPC( victim ) )
    {
    sprintf( buf, "Mobs killed: %d. Times killed by a mob: %d.\n\r",
            victim->pcdata->mkills, victim->pcdata->mdeaths);
    strcat( buf1, buf );
    }

/* Added # players killed, and # times killed by a player. */
    if ( !IS_NPC( victim ) )
    {
    sprintf( buf, "Players killed: %d. Times killed by a player: %d.\n\r",
            victim->pcdata->pkills, victim->pcdata->pdeaths);
    strcat( buf1, buf );
    }

/* Added # frags and illegal pkills. */
    if ( !IS_NPC( victim ) )
    {
    sprintf( buf, "Frags: %d. Illegal pkills: %d.  NewHP: %d\n\r",
            victim->pcdata->frags, victim->pcdata->illegal_pk, victim->pcdata->new_hitpoints);
    strcat( buf1, buf );
    }

/* Added list of follower */
/* Made it check target instead of imm Govno*/
    if( victim->followers )
    {
      PERSON_DATA *follower;
      strcat( buf1, "Followers: " );
      for( follower = victim->followers; follower; follower = follower->next )
      {
        if( !follower->who || follower->who->deleted )
          continue;
        sprintf( buf, "%s&n", PERS( follower->who, victim ));
        strcat( buf1, buf );
      }
      strcat( buf1, "\n\r" );
    }

    send_to_char( buf1, ch );
    return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
           MOB_INDEX_DATA *pMobIndex;
           AREA_DATA      *pArea;
           char            buf  [ MAX_STRING_LENGTH   ];
           char            arg  [ MAX_INPUT_LENGTH    ];
           char            arg1 [ MAX_INPUT_LENGTH    ];
    extern int             top_vnum_mob;
           int             vnum;
	   int             bottom;
	   int             top;
	   bool            fAll;
	   bool            fWorld;
	   bool            found;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' ||
	( str_cmp( arg, "world" ) && str_cmp( arg, "area" ) ) )
    {
	send_to_char( "Syntax: lookup m world <mobile>|all\n\r", ch );
	send_to_char( "or:     lookup m area  <mobile>|all\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Lookup what mob?\n\r", ch );
	return;
    }

    fAll    = !str_cmp( arg1, "all" );
    found   = FALSE;
    pArea   = ch->in_room->area;
    fWorld  = !str_cmp( arg, "world" );

    if ( fWorld )
    {
	bottom	= 0;
	top	= top_vnum_mob;
    }
    else
    {
	bottom	= pArea->low_m_vnum;
	top	= pArea->hi_m_vnum + 1;
    } 

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = bottom; vnum < top; vnum++ )
    {
	if ( !( pMobIndex = get_mob_index( vnum ) ) )
	    continue;

	if ( !fWorld && pArea != pMobIndex->area )
	    continue;

	if ( fAll || is_name( arg1, pMobIndex->player_name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %s&n\n\r",
		pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
	    send_to_char( buf, ch );
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    return;
}

void do_rfind( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Not yet implemented. And this is a hard one.\n\r", ch);
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    AREA_DATA      *pArea;
    char            buf  [ MAX_STRING_LENGTH * 8 ];
    char            arg  [ MAX_INPUT_LENGTH    ];
    char            arg1 [ MAX_INPUT_LENGTH    ];
    extern int             top_vnum_obj;
    int             vnum, bottom, top, type;
    bool            fBit, found, fType, fArea;
    const BITVECT_DATA *bvect = NULL;
    int count, total = 0;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: find o world <object>\n\r", ch );
	send_to_char( "or:     find o area  <object>\n\r", ch );
	send_to_char( "or:     find o bit   <bit name>\n\r", ch );
	send_to_char( "or:     find o type  <type number>\n\r", ch );
	return;
    }
    if ( str_cmp( arg, "world" ) && str_cmp( arg, "area" ) && str_cmp( arg, "bit" ) && str_cmp( arg, "type" )) 
    {
	one_argument(arg, arg1);
	one_argument("world", arg);
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Lookup what object?\n\r", ch );
	return;
    }

    found   = FALSE;
    pArea   = ch->in_room->area;
    fBit    = !str_cmp( arg, "bit" );
    fType   = !str_cmp( arg, "type" );
    fArea   = !str_cmp( arg, "area" );

    if( fBit )
    {
      for( count = 0; item_flags[count].bit; count++ )
      {
         if( !str_cmp( arg1, item_flags[count].name ))
         {
           bvect = item_flags[count].bit;
           break;
         }
      }
      if( !bvect )
      {
        send_to_char( "No such item flag.\n\r", ch );
        return;
      }
    }
    
    if( fType )
    {
      type = atoi( arg1 );
      if( type < 1 || type > MAX_ITEM_TYPE )
      {
         send_to_char( "Invalid object type!\n\r", ch );
         return;
      }
      sprintf( buf, "Looking for object type %d:\n\r", type );
      send_to_char( buf, ch );
    }

    if ( fArea )
    {
	bottom	= pArea->low_o_vnum;
	top	= pArea->hi_o_vnum + 1;
    } 
    else
    {
	bottom	= 0;
	top	= top_vnum_obj;
    }

    for ( vnum = bottom; vnum < top; vnum++ )
    {
        if( total > 300 )
        {
           send_to_char( "Greater than 300 items found.  List truncated.\n\r", ch );
           break;
        }

	if ( !( pObjIndex = get_obj_index( vnum ) ) )
	    continue;

	if ( fArea && pArea != pObjIndex->area )
	    continue;

        if( fBit )
        {
  	  if ( IS_SET( pObjIndex->extra_flags[bvect->group], bvect->vector ))
	  {
      	    found = TRUE;
	    sprintf( buf, "[%5d] %s&n\n\r",
		pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	    send_to_char( buf, ch );
            total++;
	  }
        }
        else if( fType )
        {
  	  if ( pObjIndex->item_type == type )
	  {
      	    found = TRUE;
	    sprintf( buf, "[%5d] %s&n\n\r",
		pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	    send_to_char( buf, ch );
            total++;
	  }
        }
        else
        {
  	  if ( is_name( arg1, pObjIndex->name ) )
	  {
	    found = TRUE;
	    sprintf( buf, "[%5d] %s&n\n\r",
		pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	    send_to_char( buf, ch );
            total++;
	  }
        }
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    return;
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*5 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        count;
    int        vnum;

    one_argument( argument, arg );
    vnum = atoi( arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\n\r", ch );
	return;
    }

    buf1[0] = '\0';
    count = 0;
    for ( victim = char_list; victim; victim = victim->next )
    {
	if ( IS_NPC( victim )
	    && victim->in_room
	    && ( is_name( arg, victim->name ) 
               || vnum == victim->pIndexData->vnum ) )
	{
            count++;
            if( count < 150 )
            {
  	      sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
		    victim->pIndexData->vnum,
		    victim->short_descr,
		    victim->in_room->vnum,
		    victim->in_room->name );
	      strcat( buf1, buf );
            }
	}
    }

    if ( !count )
    {
	act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
	return;
    }
    else
    {
        sprintf( buf, "You found %d results.  Displayed no more than the first 150.\n\r", count );
        strcat( buf1, buf );
    }

    send_to_char( buf1, ch );
    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "reboot" ) )
        return;

    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;

    rch = get_char( ch );

    if ( !authorized( rch, "reboot" ) )
        return;

    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );

    print_last_coms( );
    print_last_funs( );
    end_of_game( );

    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "shutdown" ) )
        return;

    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;

    rch = get_char( ch );

    if ( !authorized( rch, "shutdown" ) )
        return;

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    do_echo( ch, buf );

    end_of_game( );

    merc_down = TRUE;
    return;
}

void do_snoop( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];
    char             buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "snoop" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) && str_cmp( ch->name, "Veygoth" ))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc )
    {
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "%s is snooping %s", ch->name, victim->name );
    wiznet( ch, WIZ_SNOOPS, get_trust( rch ), buf );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "switch" ) )
        return;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( !ch->desc )
	return;
    
    if ( ch->desc->original )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
        send_to_char( "You cannot switch into a player!\n\r", ch );
	return;
    }

    if ( victim->desc )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    ch->pcdata->switched  = TRUE;
    ch->desc->character   = victim;
    ch->desc->original    = ch;
    victim->desc          = ch->desc;
    ch->desc              = NULL;
    send_to_char( "Ok.\n\r", victim );
    sprintf( buf, "%s switched into %s", rch->name, victim->short_descr );
    wiznet( ch, WIZ_SWITCHES, get_trust( rch ), buf );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !ch->desc )
	return;

    if ( !ch->desc->original )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

/* Note that we dont check for immortal ability to have return here.
 * We assume we will automatically allow immortals with switch to return.
 * Dont want to have our immortals stuck in a mobile's body do we?  :)
 * -Kahn */

    send_to_char( "You return to your original body.\n\r", ch );
    ch->desc->original->pcdata->switched = FALSE;
    ch->desc->character                  = ch->desc->original;
    ch->desc->original                   = NULL;
    ch->desc->character->desc            = ch->desc; 
    ch->desc                             = NULL;

    sprintf( buf, "%s returns to his original body", rch->name );
    wiznet( rch, WIZ_SWITCHES, get_trust( rch ), buf );
    return;
}


/* Govno Cloning codebits - Ensures containers are properly cloned.  */
void recurs_clone( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone )
{
    OBJ_DATA *c_obj, *t_obj;

    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content )
    {
        t_obj = create_object( c_obj->pIndexData,0 );
	clone_object( c_obj, t_obj );
	obj_to_obj( t_obj, clone );
	recurs_clone( ch, c_obj, t_obj );
    }
}

/* Govno Cloning codebits - Make a clone of something */
void do_clone( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA  *obj;

    rest = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Clone what?\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "object" ) )
    {
        mob = NULL;
	obj = get_obj_here( ch, rest );
	if ( obj == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }
    else if (!str_prefix(arg,"mobile") || (!str_prefix(arg,"character")))
    {
        obj = NULL;
	mob = get_char_room( ch, rest );
	if ( mob == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }
    else
    {
        mob = get_char_room( ch, argument );
	obj = get_obj_here( ch, argument );
	if ( mob == NULL && obj == NULL )
	{
	    send_to_char( "You don't see that here.\n\r", ch );
	    return;
	}
    }

    /* clone object */
    if ( obj != NULL )
    {
        OBJ_DATA *clone;

	clone = create_object( obj->pIndexData, 0 );
	clone_object( obj, clone );
	if ( obj->carried_by != NULL )
	   obj_to_char( clone, ch );
        else
	   obj_to_room( clone, ch->in_room);
	recurs_clone( ch, obj, clone );

	act("$n has created $p.", ch, clone, NULL, TO_ROOM );
	act("You clone $p.", ch, clone, NULL, TO_CHAR );
	sprintf( buf, "%s clones %s.", ch->name, clone->short_descr);
        wiznet( ch, WIZ_LOAD, get_trust( ch ), buf );
	return;
    }
    else if (mob != NULL )
    {
        CHAR_DATA *clone;
	OBJ_DATA *new_obj;

	if ( !IS_NPC( mob ) )
	{
	    send_to_char( "You can only clone mobiles.\n\r", ch );
	    return;
	}

	clone = create_mobile( mob->pIndexData );
	clone_mobile( mob, clone );

	for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
	{
	    new_obj = create_object( obj->pIndexData, 0 );
	    clone_object( obj, new_obj );
	    recurs_clone( ch, obj, new_obj );
	    obj_to_char( new_obj, clone );
	    new_obj->wear_loc = obj->wear_loc;
	}

	char_to_room( clone, ch->in_room );
	act( "$n has created $N.", ch, NULL, clone, TO_ROOM );
	act( "You clone $N.", ch, NULL, clone, TO_CHAR );
	sprintf( buf, "%s clones %s.", ch->name, clone->short_descr);
        wiznet( ch, WIZ_LOAD, get_trust( ch ), buf );
	return;
    }
}
/* Govno end clone code */



void do_mload( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *rch;
    CHAR_DATA      *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg [ MAX_INPUT_LENGTH ];
    char            buf [ MAX_STRING_LENGTH ];
    
    rch = get_char( ch );

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    send_to_char( "Ok.\n\r", ch );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    sprintf( buf, "%s has mloaded %s at %s [%d]", ch->name, victim->short_descr,
	    ch->in_room->name, ch->in_room->vnum );
    wiznet( ch, WIZ_LOAD, get_trust( rch ), buf );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA       *obj;
    CHAR_DATA      *rch;
    OBJ_INDEX_DATA *pObjIndex;
    char            arg1 [ MAX_INPUT_LENGTH ];
    char            arg2 [ MAX_INPUT_LENGTH ];
    int             level;
    char            buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
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
	    send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
        }
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR( obj, ITEM_TAKE ) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
	act( "$n&n has created $p&n!", ch, obj, NULL, TO_ROOM );
    }
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "%s has loaded %s at %s [%d]", ch->name, obj->short_descr,
	    ch->in_room->name, ch->in_room->vnum );
    wiznet( ch, WIZ_LOAD, get_trust( rch ), buf );
    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_purge");
    rch = get_char( ch );

    if ( !authorized( rch, "purge" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
        OBJ_DATA  *obj_next;
	CHAR_DATA *vnext;

	for ( victim = ch->in_room->people; victim; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( victim->deleted )
	        continue;

	    if ( IS_NPC( victim ) && victim != ch )
            {
		extract_char( victim, TRUE );
                last_fun( "do_purge");
            }
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    extract_obj( obj );
	}

	send_to_char( "Ok.\n\r", ch );
	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
	return;
    }

    act( "$n&n purges $N&n.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    last_fun( "do_purge");
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        level;
    int        iLevel;

    rch = get_char( ch );

    if ( !authorized( rch, "advance" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    
    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Advance within range 1 to %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r",    victim );
	victim->level    = 1;
        // Max_hit should only be accessed for PERMENANT changes - Veygoth
	victim->max_hit  = 20;
	victim->max_mana = 50;
        if( ch->class == CLASS_PSIONICIST )
        {
          // Mana bonuses for newbies -- Veygoth
          victim->max_mana += (get_curr_int( victim ) / 10);
          victim->max_mana += (get_curr_wis( victim ) / 10);
          victim->max_mana += (get_curr_wis( victim ) / 10);
        }
	victim->max_move = 150;
        // removed resetting of skills.
	victim->hit      = get_max_hit( victim );
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	sprintf( buf, "%s has been demoted to level %d by %s", victim->name,
		level, ch->name );
	wiznet( victim, WIZ_LEVELS, get_trust( rch ), buf );
        if( !victim->pcdata->new_hitpoints )
          victim->pcdata->new_hitpoints = 1;
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
	sprintf( buf, "%s has been advanced to level %d by %s", victim->name,
		level, ch->name );
	wiznet( victim, WIZ_LEVELS, get_trust( rch ), buf );
    }

    // Do not advance skills -- rerolling someone will auto-master
    // their skills with no effort from the player... so we advance
    // them with skills set to FALSE -- Veygoth
    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	advance_level( victim, FALSE );
    }
    victim->exp   = 1;
    victim->trust = 0;

    return;
}

void do_trust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        level;

    rch = get_char( ch );

    if ( !authorized( rch, "trust" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\n\r", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Trust within range 1 to %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }

    sprintf( buf, "%s has been trusted at level %d by %s", victim->name,
	    level, ch->name );
    wiznet( ch, WIZ_LEVELS, get_trust( rch ), buf );

    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *vch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "restore" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, "room" ) )
    {
        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted || IS_NPC( vch ))
	        continue;
            restore( ch, vch );
	}
        sprintf( buf, "%s has restored room %d.",
		rch->name, ch->in_room->vnum );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Room restored.\n\r", ch );
	return;
    }

    /* Restore All feature coded by Katrina */
    if ( !str_cmp( arg, "all" ) )
    {
        for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( victim->deleted || !victim->in_room || IS_NPC( victim ))
	        continue;
            restore( ch, victim );
	}
        sprintf( buf, "%s has restored the whole mud.", ch->name );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Aww...how sweet :)...Done.\n\r", ch );
    }
    else
    {
	if ( !( victim = get_char_world( ch, arg ) ) )
	  {
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	  }
        restore( ch, victim );
        sprintf( buf, "%s has restored %s.", ch->name, victim->name );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



void do_freeze( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "freeze" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_FREEZE ) )
    {
	REMOVE_BIT( victim->act, PLR_FREEZE );
	send_to_char( "FREEZE removed.\n\r",     ch     );
	send_to_char( "You can play again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_FREEZE );
	send_to_char( "FREEZE set.\n\r",            ch     );
	send_to_char( "You can't do ANYthing!\n\r", victim );
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "log" ) )
        return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        one_argument( argument, arg );
        if( str_cmp( arg, "confirm" ) && !fLogAll )
        {
            send_to_char( "&+rType log all confirm to log everything everyone does!\n\r", ch );
            return;
        }
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "&+RLog ALL on. Logging everything!\n\r",  ch );
	}
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET( victim->act, PLR_LOG ) )
    {
	REMOVE_BIT( victim->act, PLR_LOG );
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(    victim->act, PLR_LOG );
	send_to_char( "LOG set.\n\r",     ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "noemote" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_EMOTE );
	send_to_char( "NO_EMOTE removed.\n\r",    ch     );
	send_to_char( "You can emote again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_EMOTE );
	send_to_char( "You can't emote!\n\r",    victim );
	send_to_char( "NO_EMOTE set.\n\r",       ch     );
    }

    return;
}

void do_silence( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "silence" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_SILENCE ) )
    {
	REMOVE_BIT( victim->act, PLR_SILENCE );
	send_to_char( "You can use channels again.\n\r", victim );
	send_to_char( "SILENCE removed.\n\r",            ch     );
    }
    else
    {
	SET_BIT(    victim->act, PLR_SILENCE );
	send_to_char( "You can't use channels!\n\r",     victim );
	send_to_char( "SILENCE set.\n\r",                ch     );
    }

    return;
}


void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "peace" ) )
        return;

    /* Yes, we are reusing rch.  -Kahn */
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted )
	    continue;

	if ( rch->fighting )
	    stop_fighting( rch, TRUE );

	stop_hating( rch );
	stop_hunting( rch );
	stop_fearing( rch );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



BAN_DATA *		ban_free;
BAN_DATA *		ban_list;

void do_ban( CHAR_DATA *ch, char *argument )
{
    BAN_DATA  *pban;
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    if ( IS_NPC( ch ) )
	return;

    rch = get_char( ch );

    if ( !authorized( rch, "ban" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	strcpy( buf, "Banned sites:\n\r" );
	for ( pban = ban_list; pban; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );
	return;
    }

    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( !str_cmp( arg, pban->name ) )
	{
	    send_to_char( "That site is already banned!\n\r", ch );
	    return;
	}
    }

    if ( !ban_free )
    {
	pban		= alloc_perm( sizeof( *pban ) );
        top_ban++;
    }
    else
    {
	pban		= ban_free;
	ban_free	= ban_free->next;
    }

    pban->name	= str_dup( arg );
    pban->next	= ban_list;
    ban_list	= pban;
    send_to_char( "Ok.\n\r", ch );
    ban_update( );
    return;
}



void do_allow( CHAR_DATA *ch, char *argument )
{
    BAN_DATA  *prev;
    BAN_DATA  *curr;
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "allow" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove which site from the ban list?\n\r", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr; prev = curr, curr = curr->next )
    {
	if ( !str_cmp( arg, curr->name ) )
	{
	    if ( !prev )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    curr->next	= ban_free;
	    ban_free	= curr;
	    send_to_char( "Ok.\n\r", ch );
	    ban_update( );
	    return;
	}
    }

    send_to_char( "Site is not banned.\n\r", ch );
    return;
}



void do_wizlock( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern bool       wizlock;

    rch = get_char( ch );

    if ( !authorized( rch, "wizlock" ) )
        return;

    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );

    return;
}

void do_slookup( CHAR_DATA *ch, char *argument )
{
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH*12];
    char       arg  [ MAX_INPUT_LENGTH ];
    int        sn;
    int        count;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
    {/* This is buggy for skills 59 through MAX_SKILL-1.  Dunno why. - Lohrr */
        buf1[0] = '\0';
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skills_table[sn].name )
		continue;
            if( strlen( buf1 ) > MAX_STRING_LENGTH*12 - 500 )
            {
                send_to_char( "&+WToo many skills to display!!\n\r", ch );
                break;
            }
            sprintf( buf, "Sn: %4d Skill: '%s' Lag: %d\n\r",
		    sn, skills_table[sn].name, skills_table[sn].beats );
            strcat( buf1, buf );
            for( count = 0; count < MAX_CLASS; count++ )
            {
              sprintf( buf, "%18s: %d ", class_table[count]->name,
                skills_table[sn].skill_level[count] );
              if( count % 3 == 2 )
                strcat( buf1, "\n\r" );
              strcat( buf1, buf );
            }
            strcat( buf1, "\n\r" );
	}
	send_to_char( buf1, ch );
        return;
    }
    else
    {
	if ( is_number( arg ) )
        {
	    sn = atoi( arg );
	    if (   sn >= 0
		&& sn  < MAX_SKILL
		&& skills_table[sn].name )
	    {
                buf1[0] = '\0';
	        sprintf( buf, "Sn: %4d Skill: '%s' Lag: %d\n\r",
		    sn, skills_table[sn].name, skills_table[sn].beats );
                strcat( buf1, buf );
                strcat( buf1, "\n\rClass Availability:\n\r" );
                for( count = 0; count < MAX_CLASS; count++ )
                {
                  sprintf( buf, "   %15s: %d", class_table[count]->name,
                    skills_table[sn].skill_level[count] );            
                  strcat( buf1, buf );
                  if( count % 3 == 2 )
                    strcat( buf1, "\n\r" );       
                }
                strcat( buf1, "\n\r" );
		send_to_char( buf1, ch );
		return;
            }
	}

        if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill.\n\r", ch );
	    return;
	}

        buf1[0] = '\0';
        sprintf( buf, "Sn: %4d Skill: '%s' Lag: %d\n\r",
	    sn, skills_table[sn].name, skills_table[sn].beats );
        strcat( buf1, buf );
        strcat( buf1, "\n\rClass Availability:\n\r" );
        for( count = 0; count < MAX_CLASS; count++ )
        {
          sprintf( buf, "   %15s: %d", class_table[count]->name,
            skills_table[sn].skill_level[count] );            
          strcat( buf1, buf );
          if( count % 3 == 2 )
            strcat( buf1, "\n\r" );       
        }
        strcat( buf1, "\n\r" );
	send_to_char( buf1, ch );
    }

    return;
}

// spell lookup - Veygoth
void do_splookup( CHAR_DATA *ch, char *argument )
{
    char       buf  [ MAX_STRING_LENGTH  ];
    char       buf1 [ MAX_STRING_LENGTH*5];
    char       arg  [ MAX_INPUT_LENGTH ];
    int        sn;
    int        count;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat what spell?\n\rYou can use 'stat spell all', 'stat spell offensive', and 'stat spell ignore' also.", ch );
	return;
    }

    /* Added castable in battle and minimum amount of mana. */
    // Changed to lag time - Veygoth
    if ( !str_cmp( arg, "all" ) || !str_cmp( arg, "offensive") || !str_cmp( arg, "ignore" ))
    {
        int type;
        if( !str_cmp( arg, "offensive" )) type = TAR_CHAR_OFFENSIVE;
        else if( !str_cmp( arg, "ignore" )) type = TAR_IGNORE;
        else type = -1;
	for ( sn = 0; sn < MAX_SPELL; sn++ )
	{
	    if ( !spells_table[sn].name )
		continue;

            if( type != -1 )
                if( spells_table[sn].target != type )
                   continue;

    	    buf1[0] = '\0';

            sprintf( buf, "Sn: %4d. Spell: '%s'. ",
		    sn, spells_table[sn].name );
            strcat( buf1, buf );
            sprintf( buf, "Lag: %d. Castable in battle: %s.\n\r",
		    spells_table[sn].beats,
		    spells_table[sn].cast_combat ? "Yes" : "No" );
            strcat( buf1, buf );

	    send_to_char( buf1, ch );
	}
    }
    else
/* Added castable in battle, minimum amount of mana, possible
	  targets, and mana type. */
    {
	if ( is_number( arg ) )
        {
	    sn = atoi( arg );
	    if (   sn >= 0
		&& sn  < MAX_SPELL
		&& spells_table[sn].name )
	    {
	        sprintf( buf1, "Sn: %4d Spell: '%s' ",
		    sn, spells_table[sn].name );
                sprintf( buf, "Minumum mana: %d Castable in battle: %s\n\r",
    		    spells_table[sn].min_mana,
		    spells_table[sn].cast_combat ? "Yes" : "No" );
                strcat( buf1, buf );
		sprintf( buf, "Targets: %d Mana Type: %d\n\r",
		    spells_table[sn].target, spells_table[sn].mana_type);
                strcat( buf1, buf );
		sprintf( buf, "Msg To Room: %s\n\rMsg To Vict: %s\n\r",
		    spells_table[sn].msg_damage_room, spells_table[sn].msg_damage_vict);
                strcat( buf1, buf );
		sprintf( buf, "Msg To Room: %s\n\rMsg To Self: %s\n\r",
		    spells_table[sn].msg_damage_self_rm, spells_table[sn].msg_damage_self);
                strcat( buf1, buf );
		sprintf( buf, "Damage: %s\n\rKill Msg: %s\n\r",
		    spells_table[sn].msg_damage, spells_table[sn].msg_kill);
                strcat( buf1, buf );
		sprintf( buf, "Wear Off Msg: %s\n\r",
		    spells_table[sn].msg_off );
                strcat( buf1, buf );
                strcat( buf1, "\n\rClass Availability:\n\r" );
                for( count = 0; count < MAX_CLASS; count++ )
                {
                  sprintf( buf, "   %17s: %3d", class_table[count]->name,
                    spells_table[sn].spell_circle[count] );            
                  strcat( buf1, buf );
                  if( count % 3 == 2 )
                    strcat( buf1, "\n\r" );       
                }
                strcat( buf1, "\n\r" );
	        send_to_char( buf1, ch );
		return;
            }
	}

        if ( ( sn = spell_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such spell.\n\r", ch );
	    return;
	}

/* Added castable in battle, minimum amount of mana, possible
  	targets, and mana type - Lohrr */
	        sprintf( buf1, "Sn: %4d Spell: '%s' ",
		    sn, spells_table[sn].name );
                sprintf( buf, "Minumum mana: %d Castable in battle: %s\n\r",
    		    spells_table[sn].min_mana,
		    spells_table[sn].cast_combat ? "Yes" : "No" );
                strcat( buf1, buf );
		sprintf( buf, "Targets: %d Mana Type: %d\n\r",
		    spells_table[sn].target, spells_table[sn].mana_type);
                strcat( buf1, buf );
		sprintf( buf, "Msg To Room: %s\n\rMsg To Vict: %s\n\r",
		    spells_table[sn].msg_damage_room, spells_table[sn].msg_damage_vict);
                strcat( buf1, buf );
		sprintf( buf, "Msg To Room: %s\n\rMsg To Self: %s\n\r",
		    spells_table[sn].msg_damage_self_rm, spells_table[sn].msg_damage_self);
                strcat( buf1, buf );
		sprintf( buf, "Damage: %s\n\rKill Msg: %s",
		    spells_table[sn].msg_damage, spells_table[sn].msg_kill);
                strcat( buf1, buf );
		sprintf( buf, "Wear Off Msg: %s\n\r",
		    spells_table[sn].msg_off );
                strcat( buf1, buf );
                strcat( buf1, "\n\rClass Availability:\n\r" );
                for( count = 0; count < MAX_CLASS; count++ )
                {
                  sprintf( buf, "   %15s: %d", class_table[count]->name,
                    spells_table[sn].spell_circle[count] );            
                  strcat( buf1, buf );
                  if( count % 3 == 2 )
                    strcat( buf1, "\n\r" );       
                }
	        send_to_char( buf1, ch );
    }

    return;
}

void do_sset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set skill <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     set skill <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch->level <= victim->level && ch != victim && str_cmp( ch->name, "Veygoth" ))
    {
	send_to_char( "You may not set your peer nor your superior.\n\r",
		     ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > MAX_SKILL_ADEPT )
    {
	send_to_char( "Value range is 0 to 95.\n\r", ch );
	return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Only Directors may set all.\n\r", ch );
	    return;
	}
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skills_table[sn].name )
            {
		if ( skills_table[sn].skill_level[victim->class] <= LEVEL_HERO
		    || IS_IMMORTAL( victim ) )
		    victim->pcdata->skl_lrn[sn] = value;
            }
	    else
            {
		if ( skills_table[sn].skill_level[victim->class]
		    <= get_trust( victim ) )
		    victim->pcdata->skl_lrn[sn] = 1;
            }
	}
    }
    else
    {
        victim->pcdata->skl_lrn[sn] = value;
    }

    return;
}

void do_splset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set spell <victim> <spell> <value>\n\r",	ch );
	send_to_char( "or:     set spell <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch->level <= victim->level && ch != victim && str_cmp( ch->name, "Veygoth"))
    {
	send_to_char( "You may not set your peer nor your superior.\n\r",
		     ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = spell_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > MAX_SPELL_ADEPT )
    {
	send_to_char( "Value range is 0 to 99.\n\r", ch );
	return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Only Directors may set all.\n\r", ch );
	    return;
	}
	for ( sn = 0; sn < MAX_SPELL; sn++ )
	{
	    if ( spells_table[sn].name )
            {
		if ( spells_table[sn].spell_circle[victim->class] <= 11
		    || IS_IMMORTAL( victim ) )
		    victim->pcdata->spl_lrn[sn] = value;
            }
	    else
            {
		if ( spells_table[sn].spell_circle[victim->class]
		    <= (( get_trust( victim ) + 4 ) / 5 ))
		    victim->pcdata->spl_lrn[sn] = 1;
            }
	}
    }
    else
    {
        victim->pcdata->spl_lrn[sn] = value;
    }

    return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg3 [ MAX_INPUT_LENGTH  ];
    int        count2;
    int        value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set m <victim> <field>  <value>\n\r",	ch );
	send_to_char( "or:     set m <victim> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  str int wis dex con agi cha pow luk size\n\r",	ch );
	send_to_char( "  class hp mana move align wait height weight\n\r", ch );
	send_to_char( "  sex race level thirst drunk full security\n\r", ch );
	send_to_char( "  copper silver gold platinum position exp\n\r", ch );
	send_to_char( "String being one of:\n\r",			 ch );
	send_to_char( "  name short long title spec spec2 clan rank\n\r",ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Strength range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_str = value;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Intelligence range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_int = value;
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Wisdom range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_wis = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Dexterity range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_dex = value;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Constitution range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_con = value;
	return;
    }

    if ( !str_cmp( arg2, "agi" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Agility range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_agi = value;
	return;
    }

    if ( !str_cmp( arg2, "cha" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Charisma range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_cha = value;
	return;
    }

    if ( !str_cmp( arg2, "pow" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Power range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_pow = value;
	return;
    }

    if ( !str_cmp( arg2, "luk" ) )
    {
	if ( value < 1 || value > MAX_BASE_ATTRIBUTE )
	{
	    sprintf( buf, "Luck range is 1 to %d.\n\r", MAX_BASE_ATTRIBUTE );
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_luk = value;
	return;
    }

/* Govno size bit */
    if ( !str_cmp( arg2, "size" ) )
    {
	int newsize;

	if ( !atoi(arg3) )
	    newsize = size_lookup( arg3 );
	else
	    newsize = atoi(arg3);

        if ( newsize < 0 || newsize > MAX_SIZE )
	{
	    sprintf( buf, "Size range is from 0 to %d.\n\r", MAX_SIZE );
	    send_to_char( buf, ch );
	    return;
        }

	victim->size = newsize;
	return;
    }

    if ( !str_cmp( arg2, "position" ) )
    {
        if ( value < 0 || value > POS_STANDING )
	{
	    sprintf( buf, "Position range is from 0 to %d.\n\r", POS_STANDING );
	    send_to_char( buf, ch );
	    return;
        }

	victim->position = value;
	return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
	if ( value < 0 || value >= MAX_CLASS )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
	    send_to_char( buf, ch );
	    return;
	}
	victim->class = value;
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
        if ( is_affected( victim, AFF_CHANGE_SEX ) )
	{
	    send_to_char( "This person is affect by change sex.\n\r", ch );
	    send_to_char( "Try again later.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}

	victim->sex = value;

	return;
    }

    if ( !str_cmp( arg2, "race" ) )
    {
        OBJ_DATA *wield;
	OBJ_DATA *wield2;
	int       race;

	if ( is_affected( victim, AFF_POLYMORPH ) )
	{
	    send_to_char( "This person is affected by polymorph other.\n\r",
			 ch );
	    send_to_char( "Try again later.\n\r", ch );
	    return;
	}

	race = race_lookup( arg3 );

	if ( race < 0 && !(race = atoi( arg3 ) ) )
	{
	    send_to_char( "Invalid race.\n\r", ch );
	    return;
	}

	if (  !IS_SET( race_table[ race ].race_abilities, RACE_PC_AVAIL )
	    && get_trust( ch ) < L_DIR )
	{
	    send_to_char( "You may not set a race not available to PC's.\n\r",
			 ch );
	    return;
	}

	victim->race = race;

	if ( ( wield = get_eq_char( victim, WEAR_HAND ) )
            && wield->item_type == TYPE_WEAPON
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( "You drop $p&n.", victim, wield, NULL, TO_CHAR );
	    act( "$n&n drops $p&n.", victim, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, victim->in_room );
	}

	if ( ( wield2 = get_eq_char( victim, WEAR_HAND_2 ) )
            && wield2->item_type == TYPE_WEAPON
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( "You drop $p&n.", victim, wield2, NULL, TO_CHAR );
	    act( "$n&n drops $p&n.", victim, wield2, NULL, TO_ROOM );
	    obj_from_char( wield2 );
	    obj_to_room( wield2, victim->in_room );
	}

	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Level range is 0 to 50.\n\r", ch );
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_cmp( arg2, "wait" ) )
    {
	if ( value < 0 || value > 120 )
	{
	    send_to_char( "Wait range is 0 to 120.\n\r", ch );
	    return;
	}
	victim->wait = value;
	return;
    }

    if ( !str_cmp( arg2, "copper" ) )
    {
	victim->money.copper = value;
	return;
    }
    if ( !str_cmp( arg2, "silver" ) )
    {
	victim->money.silver = value;
	return;
    }
    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->money.gold = value;
	return;
    }
    if ( !str_cmp( arg2, "platinum" ) )
    {
	victim->money.platinum = value;
	return;
    }

    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	if ( victim->fighting && value < 0 )
	{
	    send_to_char( "You cannot set a fighting person's hp below 0.\n\r",
			 ch );
	    return;
	}

        // Set their perm hitpoints so that their current max hitpoints will be reflected
        // -- Veygoth
        if( !IS_NPC( ch ))
  	  victim->max_hit = ( (value * 100) / get_curr_con( victim )) + 1;
        else
          victim->max_hit = value;

	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }

    if ( !str_cmp( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
	return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Thirst range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Thirst range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if( !str_cmp( arg2, "exp" ))
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Experience range is 0 to 100 (in percent).\n\r", ch );
	    return;
	}

	victim->exp = (value * exp_table[victim->level].lvl_exp) / 100;
	return;
    }

    if( !str_cmp( arg2, "frag" ))
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Frag range is -1000 to 1000.\n\r", ch );
	    return;
	}

	victim->pcdata->frags = value;
	return;
    }


    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Drunk range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }

    if ( !str_cmp( arg2, "weight" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 1 || value > 1000 )
	{
	    send_to_char( "Weight range is 1 to 1000 pounds.\n\r", ch );
	    return;
	}

	victim->pcdata->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "height" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 1 || value > 120 )
	{
	    send_to_char( "Height range is 1 to 120 inches.\n\r", ch );
	    return;
	}

	victim->pcdata->height = value;
	return;
    }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Full range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Full range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->long_descr );
	strcat( arg3, "\n\r" );
	victim->long_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "clan" ) )
    {
	CLAN_DATA *clan;
        CLAN_DATA *clanold;
        int count;
        bool found = FALSE;

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( !( clan = clan_lookup( arg3 ) ) )
	{
	    send_to_char( "That clan doesn't exist.\n\r", ch );
	    return;
	}

        // remember the number of the available slot in the count
        // variable so we can fill that slot at the end of this code
        for( count = 0; count < MAX_CLAN; count++ )
        {
          if( clan->member[count]->filled == FALSE )
          {
            found = TRUE;
            break;
          }
        }

        if( !found )
        {
          send_to_char( "That clan is full.\n\r", ch );
          return;
        }

        if( victim->pcdata->clan )
        {
          clanold = victim->pcdata->clan;
          clanold->members--;

          for( count2 = 0; count2 < MAX_CLAN; count2++ )
          {
            if( !clanold->member[count2]->name || 
              !str_cmp( clanold->member[count2]->name, victim->name ))
            {
              clanold->member[count2]->filled = FALSE;
            }
          }

          save_clan( clanold );
        }

	victim->pcdata->clan = clan;
	victim->pcdata->clan->name = str_dup( clan->name );
        victim->pcdata->clan->members++;

        if( victim->pcdata->rank == 0 )
            victim->pcdata->rank = RANK_NORMAL;

        clan->member[count]->name = str_dup( victim->name );
        clan->member[count]->rank = victim->pcdata->rank;
        clan->member[count]->fine = 0;
        clan->member[count]->join_time = (int)current_time;
        clan->member[count]->filled = TRUE;

        send_to_char( "Clan initiation successful.  Be sure to set rank.\n\r", ch );

        save_clan( clan );

	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_mob_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\n\r", ch );
	    return;
	}

	return;
    }

    if ( !str_cmp( arg2, "spec2" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun2 = spec_mob_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\n\r", ch );
	    return;
	}

	return;
    }

    if ( !str_cmp( arg2, "death" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->pIndexData->death_fun = spec_mob_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such death fun.\n\r", ch );
	    return;
	}

	return;
    }

    if ( !str_cmp( arg2, "rank" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 1 || value > 6 )
	{
	    send_to_char( "No such rank.\n\r", ch );
            send_to_char( "Ranks: 1=Exile, 2=Clansman, 3=Clanhero, 4=Subchief, 5=Chieftain, 6=Overlord\n\r", ch );
	    return;
	}

	victim->pcdata->rank = value;
	return;
    }

    if ( !str_cmp( arg2, "security" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( ( value > ch->pcdata->security && get_trust( ch ) < L_DIR )
	    || value < 0 )
        {
	    if ( ch->pcdata->security > 0 )
	    {
		sprintf( buf, "Valid security is 0-%d.\n\r",
		    ch->pcdata->security );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "Valid security is 0 only.\n\r", ch );
	    }
	    return;
        }
        victim->pcdata->security = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       arg3 [ MAX_INPUT_LENGTH ];
    char       arg4 [ MAX_INPUT_LENGTH ];
    int        value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set o <object> <field>  <value>\n\r",	ch );
	send_to_char( "or:     set o <object> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  value0 value1 value2 value3 value4\n\r",	ch );
	send_to_char( "  value5 value6 value7 type\n\r",		ch );
	send_to_char( "  extra wear level weight cost timer\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long ed\n\r",			ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg1 ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	obj->value[0] = value;
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	return;
    }

    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
	obj->value[4] = value;
	return;
    }

    if ( !str_cmp( arg2, "value5" ) || !str_cmp( arg2, "v5" ) )
    {
	obj->value[5] = value;
	return;
    }

    if ( !str_cmp( arg2, "value6" ) || !str_cmp( arg2, "v6" ) )
    {
	obj->value[6] = value;
	return;
    }

    if ( !str_cmp( arg2, "value7" ) || !str_cmp( arg2, "v7" ) )
    {
	obj->value[7] = value;
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	if ( !is_number( arg3 ) )
	{
            send_to_char( "Extra value must be a number.\n\r", ch );
	    return;
	}

	obj->extra_flags[0] = value;
	send_to_char( "Extra flag is set.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	obj->wear_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	obj->level = value;
	return;
    }

    if ( !str_cmp( arg2, "type" ) )
    {
	obj->item_type = value;
	return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
    {
	if ( obj->carried_by != NULL && !IS_NPC( obj->carried_by ) )
	{
	    send_to_char(
		"You may not modify an item's weight while on a PC.\n\r",
			 ch);
	    return;
	}
	obj->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	return;
    }
	
    if ( !str_cmp( arg2, "name" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->name );
	obj->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->description );
	obj->description = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );
	if ( arg4[0] == '\0' )
	{
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	ed			= new_extra_descr();

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->deleted             = FALSE;
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *person;
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];
    char             arg3 [ MAX_INPUT_LENGTH ];
    int              value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set r <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags sector\n\r",				ch );
	return;
    }

    if ( !( location = find_location( ch, arg1 ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    for ( person = location->people; person; person = person->next_in_room )
	if ( !IS_NPC( person ) && person != ch && person->level >= ch->level && str_cmp( ch->name, "Veygoth" ))
	  {
	    send_to_char(
		"Your superior is in this room, no rsetting now.\n\r", ch );
	    return;
	  }

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
        /*
         * Value-based flagging has been disabled until we have an
         * equivalent function that handles bitvector data.
         * -- Veygoth
	if ( !is_number( arg3 ) )
	{
	    if ( ( value = flag_value( room_flags, arg3 ) ) != NO_FLAG )
	    {
		TOGGLE_BIT( location->room_flags[0], value );
		send_to_char( "Room flag toggled.\n\r", ch );
	    }

	    return;
	}
        */

	location->room_flags[0] = value;
	send_to_char( "Room flags set.\n\r", ch );
	location->room_flags[0]	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	if ( !is_number( arg3 )
	    && ( value = flag_value( sector_flags, arg3 ) ) == NO_FLAG )
		return;

	location->sector_type = value;
	send_to_char( "Sector type set.\n\r", ch );

	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}



void do_users( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch, *victim;
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf2 [ MAX_STRING_LENGTH ];
    char            arg   [MAX_INPUT_LENGTH];
    int             count;

    rch = get_char( ch );

    if ( !authorized( rch, "users" ) )
        return;

    count	= 0;
    buf[0]	= '\0';
    buf2[0]     = '\0';
    argument = one_argument( argument, arg );

    if ( arg[0] )
    {
        victim = get_char_world( rch, arg );
        if ( !victim )
        {
            send_to_char("They don't seem to be online.\n\r", rch);
            return;
        }
        for ( d = descriptor_list; d; d = d->next ) {
            if (d->character != victim) continue;
            sprintf( buf, "[%3d %2d] %s@%s\n\r",
                    d->descriptor,
                    d->connected,
                    d->original  ? d->original->name  :
                    d->character ? d->character->name : "(none)",
                    d->host );
            send_to_char( buf, ch );
        }
        return;
    }
    
    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->character && can_see( ch, d->character ) )
	{
	    count++;
	    sprintf( buf + strlen( buf ), "[%3d %2d] %s@%s\n\r",
		    d->descriptor,
		    d->connected,
		    d->original  ? d->original->name  :
		    d->character ? d->character->name : "(none)",
		    d->host );
	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf, buf2 );
    send_to_char( buf, ch );
    return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        trust;
    int        cmd;

    rch = get_char( ch );

    if ( !authorized( rch, "force" ) )
        return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( argument[0] == cmd_table[cmd].name[0]
	    && !str_prefix( argument, cmd_table[cmd].name )
	    && ( cmd_table[cmd].level > trust ) )
	{
	  send_to_char( "You can't even do that yourself!\n\r", ch );
	  return;
	}
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;

	for ( vch = char_list; vch; vch = vch->next )
	{
	    if ( vch->deleted )
	        continue;

	    if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( !( victim = get_char_world( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "wizinvis" ) )
        return;

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	REMOVE_BIT( ch->act, PLR_WIZINVIS );
	send_to_char( "You slowly fade back into existence.\n\r", ch    );
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	send_to_char( "You slowly vanish into thin air.\n\r",     ch    );
	act( "$n slowly fades into thin air.",  ch, NULL, NULL, TO_ROOM );
	SET_BIT(    ch->act, PLR_WIZINVIS );
    }

    return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "holylight" ) )
        return;

    if ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(    ch->act, PLR_HOLYLIGHT );
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
  
    rch = get_char( ch );

    if ( !authorized( rch, "wizify" ) )
        return;

    one_argument( argument, arg1  );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: wizify <name>\n\r" , ch );
	return;
    }
    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r" , ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
	return;
    }

    
    if ( !IS_SET( victim->act, PLR_WIZBIT ) )
    {
	SET_BIT( victim->act, PLR_WIZBIT );
	act( "$N wizified.",         ch, NULL, victim, TO_CHAR );
	act( "$n has wizified you!", ch, NULL, victim, TO_VICT );
    }
    else
    {
	REMOVE_BIT( victim->act, PLR_WIZBIT );
	act( "$N dewizzed.",         ch, NULL, victim, TO_CHAR );
	act( "$n has dewizzed you!", ch, NULL, victim, TO_VICT ); 
    }

    do_save( victim, "");
    return;
}

/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *in_obj;
    char       buf  [ MAX_STRING_LENGTH*2 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        obj_counter = 1;
    bool       found = FALSE;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  owhere <object>.\n\r", ch );
	return;
    }
    else
    {
        int vnum = atoi( arg );

	for ( obj = object_list; obj; obj = obj->next )
	{
	    if( !can_see_obj( ch, obj ) )
	        continue;
            if( !is_name( arg, obj->name ) && obj->pIndexData
                && obj->pIndexData->vnum != vnum )
	        continue;

	    found = TRUE;

	    for ( in_obj = obj; in_obj->in_obj;
		 in_obj = in_obj->in_obj )
	        ;

	    if ( in_obj->carried_by )
	    {
	        if ( !can_see( ch, in_obj->carried_by ) )
		    continue;
		sprintf( buf, "[%2d] %s&n carried by %s&n at [%4d].\n\r",
			obj_counter, obj->short_descr,
			PERS( in_obj->carried_by, ch ),
			in_obj->carried_by->in_room->vnum );
	    }
	    else
	    {
		sprintf( buf, "[%2d] %s&n in %s&n at [%4d].\n\r", obj_counter,
			obj->short_descr, ( !in_obj->in_room ) ?
			"somewhere" : in_obj->in_room->name,
			( !in_obj->in_room ) ?
			0 : in_obj->in_room->vnum );
	    }
	    
	    obj_counter++;
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );

	    /* Only see the first 101 */
	    if ( obj_counter > 100 )
	        break;
	}
    }

    if ( !found )
	send_to_char(
		"Nothing like that in hell, earth, or heaven.\n\r" , ch );

    return;


}

void do_numlock( CHAR_DATA *ch, char *argument )  /*By Globi*/
{
           CHAR_DATA *rch;
	   char       buf  [ MAX_STRING_LENGTH ];
	   char       arg1 [ MAX_INPUT_LENGTH  ];
    extern int        numlock;
           int        temp;

    rch = get_char( ch );

    if ( !authorized( rch, "numlock" ) )
        return;

    one_argument( argument, arg1 );

    temp = atoi( arg1 );

    if ( arg1[0] == '\0' ) /* Prints out the current value */
    {
	sprintf( buf, "Current numlock setting is:  %d.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( ( temp < 0 ) || ( temp > LEVEL_HERO ) )
    {
	sprintf( buf, "Level must be between 0 and %d.\n\r", LEVEL_HERO );
	send_to_char( buf, ch );
	return;
    }

    numlock = temp;  /* Only set numlock if arg supplied and within range */

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
    }
    else
        send_to_char( "Game now open to all levels.\n\r", ch );

    return;

}

void do_newlock( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern int        numlock;
           char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "newlock" ) )
        return;

    if ( numlock != 0 && get_trust( ch ) < L_SEN )
    {
	send_to_char( "You may not change the current numlock setting\n\r",
		     ch );
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
	send_to_char( "Changing to: ", ch );
    }

    numlock = 1;
    send_to_char( "Game locked to new characters.\n\r", ch );
    return;

}

void do_sstime( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern time_t     down_time;
    extern time_t     warning1;
    extern time_t     warning2;
    extern bool       Reboot;
           char       buf  [ MAX_STRING_LENGTH ];
           char       arg1 [ MAX_INPUT_LENGTH  ];
	   int        number;

    rch = get_char( ch );

    if ( !authorized( rch, "sstime" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );

    if ( !str_cmp( arg1, "reboot" ) )
    {
        Reboot = !Reboot;
	sprintf( buf, "Reboot is %s.\n\r", Reboot ? "on" : "off" );
	send_to_char( buf, ch );
	return;
    }

    number   = atoi( arg1 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) || number < 0 )
    {
	send_to_char( "Syntax: sstime <value>/reboot\n\r",                ch );
	send_to_char( "\n\r",                                             ch );
	send_to_char( "Value is number of minutes till reboot/shutdown.", ch );
	send_to_char( "  Or 0 to turn off.\n\r",                          ch );
	send_to_char( "Reboot will toggle reboot on or off.\n\r",         ch );
	send_to_char( "\n\r",                                             ch );
	if ( down_time > 0 )
	{
	    sprintf( buf, "1st warning:  %d minutes (%d seconds).\n\r",
		    UMAX( ( (int) warning1 - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) warning1 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "2nd warning:  %d minutes (%d seconds).\n\r",
		    UMAX( ( (int) warning2 - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) warning2 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "%s%d minutes (%d seconds).\n\r",
		    Reboot ? "Reboot:       " : "Shutdown:     ",
		    UMAX( ( (int) down_time - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) down_time - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	}
	else
	    send_to_char( "Automatic reboot/shutdown:  off.\n\r",         ch );
	return;
    }

    /* Set something */

    if ( number > 0 )
    {
        down_time = current_time + ( number * 60 );
        if ( number < 6 )
	{
	    warning2  = down_time - 150;
	    warning1  = warning2  - 75;
	}
	else
	{
	    warning2  = down_time - 150;
	    warning1  = warning2  - 150;
	}
	sprintf( buf, "%s will be in %d minutes (%d seconds).\n\r",
		Reboot ? "Reboot" : "Shutdown",
		( (int) down_time - (int) current_time ) / 60,
		( (int) down_time - (int) current_time ) );
	send_to_char( buf, ch );
	sprintf( buf, "1st Warning will be in %d minutes (%d seconds).\n\r",
		( (int) warning1 - (int) current_time ) / 60,
		( (int) warning1 - (int) current_time ) );
	send_to_char( buf, ch );
	sprintf( buf, "2nd Warning will be in %d minutes (%d seconds).\n\r",
		( (int) warning2 - (int) current_time ) / 60,
		( (int) warning2 - (int) current_time ) );
	send_to_char( buf, ch );
    }
    else
    {
	down_time = 0;
	sprintf( buf, "Auto%s is now off.\n\r",
		Reboot ? "reboot" : "shutdown" );
	send_to_char( buf, ch );
    }

    return;

}

/*
 * Modifications contributed by
 * Canth <phule@xs4all.nl>
 * Maniac <v942346@si.hhs.nl>
 * Vego <v942429@si.hhs.nl>
 */
void do_imtlset( CHAR_DATA *ch, char *argument )
{

    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];
    bool       fAll = FALSE;
    int        cmd;
    int        col  = 0;

    rch = get_char( ch );
    
    if ( !authorized( rch, "imtlset" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: imtlset <victim> +|- <immortal skill>\n\r",ch );
	send_to_char( "or:     imtlset <victim> +|- all\n\r",             ch );
	send_to_char( "or:     imtlset <victim>\n\r",                     ch );
	return;
    }

    if ( !( victim = get_char_world( rch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( rch->level <= victim->level && rch != victim && str_cmp( ch->name, "Veygoth") )
    {
	send_to_char( "You may not imtlset your peer nor your superior.\n\r",
		     ch );
	return;
    }

    if ( argument[0] == '+' || argument[0] == '-' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	
	argument = one_argument( argument, arg1 );

	if ( !str_cmp( argument, "all" ) )
	    fAll = TRUE;

	if ( arg1[0] == '+' )
	{
	    if ( !fAll )
	    {
		if ( victim->pcdata->immskll )
		    strcpy( buf, victim->pcdata->immskll );

		if ( is_name( argument, victim->pcdata->immskll ) )
		{
		    send_to_char( "That skill has already been set.\n\r", ch );
		    return;
		}

	    }

	    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	    {
		if ( cmd_table[cmd].level > get_trust( rch ) )
		    continue;
		if ( fAll )
		{
		    if (   cmd_table[cmd].level <= get_trust( victim )
			&& cmd_table[cmd].level >= LEVEL_HERO
			&& str_infix( cmd_table[cmd].name,
				     "delet reboo sla shutdow :" ) )
		    {
			strcat( buf, cmd_table[cmd].name );
			strcat( buf, " " );
		    }
		} 
		else /* Add only one skill */
		{
		    if ( !str_cmp( argument, cmd_table[cmd].name ) )
		        break;
		}
	    }

	    if ( !fAll )
	    {
		if (   cmd_table[cmd].name[0] == '\0'
		    || is_name( argument, "terminat reboo sla shutdow :" ) )
		{
		    send_to_char( "That is not an immskill.\n\r", ch );
		    return;
		}

		strcat( buf, argument );
		strcat( buf, " " ); /* This line is really not needed but makes
				       pfile look nice - Kahn */
	    }
	}
	else /* arg1[0] == '-' */
	{
	    if ( fAll )
	    {
		free_string( victim->pcdata->immskll );
		victim->pcdata->immskll = str_dup( "" );
		send_to_char( "All Immskills have been deleted.\n\r", ch );
		return;
	    }
	    else /* Remove one skill */
	    {
	        char  buf2[ MAX_STRING_LENGTH ];
		char  arg3[ MAX_INPUT_LENGTH ];
		char  arg2[ MAX_INPUT_LENGTH ];

		argument = one_argument( argument, arg2 );

		strcpy( buf2, victim->pcdata->immskll );

		if ( !is_name( arg2, victim->pcdata->immskll ) )
		{
		    send_to_char( "That Immskill is not set.\n\r", ch );
		    return;
		}

		for ( ; ; )
		{
		    strcpy( buf2, one_argument( buf2, arg3 ) );
		    if ( arg3[0] == '\0' )
			break;
		    if ( str_cmp( arg3, arg2 ) )
		    {
			strcat( buf, arg3 );
			strcat( buf, " " );
		    }
		}
	    }
	}

	free_string( victim->pcdata->immskll );
	victim->pcdata->immskll = str_dup( buf );

    }

    sprintf( buf, "Immortal skills set for %s:\n\r", victim->name );
    send_to_char( buf, ch );
    buf1[0] = '\0';
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level < LEVEL_HERO
	    || !is_name( cmd_table[cmd].name, victim->pcdata->immskll ) )
	    continue;

	sprintf( buf, "%-10s", cmd_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 8 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 8 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );

    return;

}

void do_rename( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*rch;
    CHAR_DATA	*victim;
    char	arg1 [ MAX_INPUT_LENGTH ];
    char	arg2 [ MAX_INPUT_LENGTH ];
    char	buf  [ MAX_INPUT_LENGTH ];
    char	oldname[MAX_INPUT_LENGTH];

    last_fun( "do_rename");
    rch = get_char( ch );

    if ( !authorized( rch, "rename" ) )
        return;

    smash_tilde( argument );

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
    	send_to_char( "Rename whom?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
    	send_to_char( "Rename them to what?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( rch->level <= victim->level )
    {
        send_to_char( "You may not rename your peer nor your superior.\n\r", ch );
	return;
    }
    
    if ( !check_parse_name( arg2 ) )
    {
    	send_to_char( "Illegal name, try again.\n\r", ch );
	return;
    }
    
    if( IS_NPC( victim ))
    {
        send_to_char( "You can't rename NPCs, you fool!", ch );
        return;
    }

    log_string( "Rename: Saving" );
    save_char_obj( victim );
    log_string( "Rename: Backing Up" );
    backup_char_obj( victim ); /* yep, handy function */
    log_string( "Rename: Deleting" );
    delete_char_obj( victim ); /* another handy function */

    log_string( "Rename: Giving New Name to Player" );
    strcpy( oldname, victim->name );
    free_string( victim->name );
    victim->name = str_dup( capitalize( arg2 ) );

    log_string( "Rename: Saving Renamed Player" );
    save_char_obj( victim );
    send_to_char( "Done.\n\r", ch );
    sprintf(buf, "Your new name is %s.\n\r", victim->name );
    send_to_char( buf, victim );

    sprintf( buf, "%s has just renamed %s to %s.", ch->name, oldname, victim->name );
    wiznet( ch, WIZ_SECURE, get_trust( rch ), buf );
    log_string( buf );

    return;
}

void do_terminat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "terminate" ) )
        return;

    send_to_char( "If you want to TERMINATE someone, spell it out.\n\r", ch );
    return;
}

/* delete renamed to terminate Govno */
void do_terminate( CHAR_DATA *ch, char *argument )
{
    /* cloned delete as I didn't know if anyone wanted it to remain Govno */
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_terminate");
    rch = get_char( ch );

    if ( !authorized( rch, "terminate" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Terminate whom?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_world( rch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( !victim->desc )
    {
        act( "$N&n doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
    }

    if ( rch == victim )
    {
        send_to_char( "You may not terminate yourself, use RETIRE.\n\r", ch );
        return;
    }

    if ( rch->level <= victim->level && rch != victim )
    {
        send_to_char( "You may not terminate your peer nor your superior.\n\r", ch );
        return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d == victim->desc )
        {
            /* By saving first i assure i am not removing a non existing file
             * i know it's stupid and probably useless but... Zen.
             */
            send_to_char( "&+RYour soul has been revoked by the gods!&n\n\r", victim );
            act( "&+R$n&+R grabs ahold of $N&+R's soul and tears it to shreds.&n\n\r"
                 "&+R$n&+R then proceeds to jump all over the little pieces while cackling fiendishly.&n", rch, NULL, victim, TO_ROOM );
            log_string( "Terminate: Saving" );
            save_char_obj( victim );
            log_string( "Terminate: Backing Up" );
            backup_char_obj( victim ); /* handy function huh? :) */
            log_string( "Terminate: Deleting" );
            delete_char_obj( victim ); /* handy function huh? :) */
            log_string( "Terminate: Extracting" );
            extract_char( victim, TRUE );
            log_string( "Terminate: Closing Socket" );
            close_socket( d );
            send_to_char( "&+RVic&+rtim des&+Rtro&+ryed&+R!&n\n\r", rch );
            return;
        }
    }

    send_to_char( "Termination target not found!\n\r", ch );
    return;
}

/* 
 * Simple function to let any imm make any player instantly sober.
 * Saw no need for level restrictions on this.
 * Written by Narn, Apr/96 
 */
void do_sober( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "sober" ) )
	return;

    smash_tilde( argument );
    argument = one_argument( argument, arg );

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
	return;    
    }

    if ( victim->pcdata ) 
	victim->pcdata->condition[COND_DRUNK] = 0;

    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel sober again.\n\r", victim );

    return;    
}

void do_clookup( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA         *rch;
    char               buf  [ MAX_STRING_LENGTH * 2 ];
    char               buf1 [ MAX_STRING_LENGTH * 6 ];
    int                num;
    int                circle;
    int                level;
    int                sn;
    int                nNumber;
    int                iLevelLower;
    int                iLevelUpper;
    struct class_type *class;

    rch = get_char( ch );
    num = rch->class;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    class          = class_table[rch->class];

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg [ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		send_to_char( "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    int iClass;

	    if ( strlen( arg ) < 3 )
	    {
		send_to_char( "Classes must be longer than that.\n\r", ch );
		return;
	    }

	    /*
	     * Look for classes to turn on.
	     */
	    arg[3]    = '\0';

	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( !str_prefix( arg, class_table[iClass]->name ) )
		{
		    num = iClass;
		    class = class_table[iClass];
		    break;
		}
	    }
	}
    }

    if ( !class )
    {
        send_to_char( "No such class.\n\r", ch );
        return;
    }

    buf1[0] = '\0';

    if ( iLevelLower < 0 || iLevelUpper < 0 )
	return;

    sprintf( buf, "Class: %s (%s) &n&+W[%d]&n\n\r",
	    class->name, class->who_name, num );
    strcat( buf1, buf );

    sprintf( buf, "Prime Attribute: %-14s  Weapon: %-5d\n\r",
	    affect_loc_name( class->attr_prime ), class->weapon );
    strcat( buf1, buf );

    sprintf( buf, "Max Skill Adept: %-3d  Thac0: %-5d  Thac47: %d\n\r",
    	    class->skill_adept, class->thac0_00, class->thac0_47 );
    strcat( buf1, buf );

    sprintf( buf, "Hp Min/Hp Max: %d/%-2d  Mana: %s\n\r",
    	    class->hp_min, class->hp_max, class->fMana ? "yes" : "no" );
    strcat( buf1, buf );

    strcat ( buf1, "\n\rALL Skills available for this class.\n\r\n\r" );
    strcat ( buf1, "Lv          Abilities\n\r" );

    if ( iLevelUpper >= LEVEL_IMMORTAL )
	iLevelUpper = LEVEL_HERO;

    for ( level = iLevelLower; level <= iLevelUpper; level++ )
    {
      for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
        if ( !skills_table[sn].name )
          break;
        if ( skills_table[sn].skill_level[num] != level )
          continue;

        sprintf ( buf, "&+c%2d:&n %s\n\r", level, skills_table[sn].name );
        strcat ( buf1, buf );

      }
    }

    strcat ( buf1, "\n\rALL Spells available for this class.\n\r\n\r" );
    strcat ( buf1, "Circle      Spells\n\r" );

    for ( circle = 0; circle < MAX_CIRCLE; circle++ )
    {
      for ( sn = 0; sn < MAX_SPELL; sn++ )
      {
          if ( !spells_table[sn].name )
            break;
          if ( spells_table[sn].spell_circle[num] != circle )
            continue;

          sprintf( buf, "&+c%2d:&n %s\n\r", circle, spells_table[sn].name );
          strcat( buf1, buf );
      }
    }

    send_to_char( buf1, rch );
    return;
}

/*
 * New mudconfig immskill by Zen.
 */
void do_mudconfig( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );
    
    if ( !authorized( rch, "world" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[  Keyword   ] Option\n\r", ch );

	send_to_char(  IS_SET( sysdata.act, MUD_AUTOSAVE_DB  )
            ? "[+AUTOSAVEDB ] The mud autosaves the world.\n\r"
	    : "[-autosavedb ] The mud doesn't autosave the world.\n\r"
	    , ch );

        send_to_char(  IS_SET( sysdata.act, MUD_VERBOSE_QUESTLOAD )
            ? "[+QLOADMSGS  ] The mud logs verbose quest load messages.\n\r"
            : "[-qloadmsgs  ] The mud doesen't log verbose quest load messages.\n\r"
            , ch );

        send_to_char(  IS_SET( sysdata.act, MUD_NEWLOCK )
            ? "[+NEWLOCK    ] The mud is locked to new players.\n\r"
            : "[-newlock    ] The mud allows new players.\n\r"
            , ch );

        send_to_char(  IS_SET( sysdata.act, MUD_LOG_PROGDATA )
            ? "[+PROGLOG    ] The mud wastes tons of disk space logging progs.\n\r"
            : "[-proglog    ] The mud doesen't waste tons of disk space logging progs.\n\r"
            , ch );

        send_to_char( IS_SET( sysdata.act, MUD_TURBOLEVEL )
            ? "[+TURBOLEVEL ] Players get up to 10 levels at a time.\n\r"
            : "[-turbolevel ] Players level as normal.\n\r"
            , ch);

        send_to_char( IS_SET( sysdata.act, MUD_DEBUG )
            ? "[+DEBUG      ] The MUD monitors and logs every damned thing that happens.\n\r"
            : "[-debug      ] The MUD is not trying to burn up the processor and drive.\n\r"
            , ch);

        send_to_char( "\n\rworld save to save the sysdata.\n\r", ch );
        send_to_char( "world stat for mud stats.\n\r", ch );

    }
    else
    {
	char buf [ MAX_STRING_LENGTH ];
	int  bit;
	bool fSet;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    if ( !str_cmp( arg, "save" ) )
	    {
		save_sysdata( );
                save_fraglist( );
                save_corpses( );
                save_crimes( );
		return;
	    }

            if( !str_cmp( arg, "mobstat" ) )
            {
                do_mobstat_save( ch, "" );
            }

	    if ( !str_cmp( arg, "stat" ) )
	    {
		int        minutes;
		int        seconds;

		minutes = PULSE_DB_DUMP / 60 / PULSE_PER_SECOND;
		seconds = PULSE_DB_DUMP - minutes * 60 * PULSE_PER_SECOND;

		printf_to_char( ch, "Players: %5d  MaxPlayers : %5d\n\r",
				num_descriptors, sysdata.max_players );
		printf_to_char( ch, "MaxEver: %5d  Recorded at: %s\n\r",
				sysdata.all_time_max, sysdata.time_of_max );

		if ( IS_SET( sysdata.act, MUD_AUTOSAVE_DB ) )
		    printf_to_char( ch, "The server autosaves the db every %d minutes, %d seconds.\n\r",
				    minutes, seconds );
		else
		    send_to_char( "The mud database isn't being autosaved.\n\r", ch );
		return;
	    }

	    send_to_char( "Mudconfig -option or +option?\n\r", ch );
	    send_to_char( "or:        save              \n\r", ch );
	    send_to_char( "or:        stat              \n\r", ch );
	    return;
	}

// Autosave DB disabled because it would FUCK SHIT UP - Veygoth
//	     if ( !str_cmp( arg+1, "autosavedb"	) ) bit = MUD_AUTOSAVE_DB;
	     if ( !str_cmp( arg+1, "qloadmsgs"	) ) bit = MUD_VERBOSE_QUESTLOAD;
        else if ( !str_cmp( arg+1, "newlock"    ) ) bit = MUD_NEWLOCK;
        else if ( !str_cmp( arg+1, "proglog"    ) ) bit = MUD_LOG_PROGDATA;
        else if ( !str_cmp( arg+1, "turbolevel" ) ) bit = MUD_TURBOLEVEL;
        else if ( !str_cmp( arg+1, "debug"      ) ) bit = MUD_DEBUG;
	else
	{
	    send_to_char( "Mudconfig which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT    ( sysdata.act, bit );
	    sprintf( buf, "%s is now ON.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT ( sysdata.act, bit );
	    sprintf( buf, "%s is now OFF.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}

    }

    return;
}

void do_fog( CHAR_DATA *ch, char *argument )
{
    if( IS_NPC( ch ))
       return;
    if( !authorized( ch, "fog" ))
       return;
    TOGGLE_BIT( ch->act, PLR_FOG );
    if( IS_SET( ch->act, PLR_FOG ))
       send_to_char( "Fog is now on.\n\r", ch );
    else
       send_to_char( "Fog is now off.\n\r", ch );
    return;
}

void do_finger( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA		*victim;
    DESCRIPTOR_DATA	*victDD;
    char		 arg [ MAX_INPUT_LENGTH ];
    char		 buf [ MAX_INPUT_LENGTH ];
    bool 		 logged_in;
    int			 x;

    last_fun( "do_finger");

    if( IS_NPC( ch ))
       return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Finger whom?\n\r", ch );
	return;
    }

		/* Make sure name is capitalized correctly. */
    arg[0] = UPPER ( arg[0] );
    for ( x = 1; arg[x] != '\0'; x++)
	arg[x] = LOWER ( arg[x] );

		/* Look for the character logged in. */
    if ( ( victim = get_char_world ( ch, arg ) ) )
    {
        if IS_NPC( victim )
        {
           send_to_char( "Don't finger mobs.\n\r", ch );
           return;
        }
        if ( !victim->desc )
        {
            send_to_char( "Character is linkdead.\n\r", ch );
            load_char_obj ( ( victDD = malloc( sizeof( DESCRIPTOR_DATA) ) ), arg );
	    logged_in = FALSE;
        }
        else
        {
	    logged_in = TRUE;
            send_to_char ( "This character is logged in.\n\r", ch );
            victDD = victim->desc;
        }
    } 	/* If not, then load from save files. */
    else
    {
    	logged_in = FALSE;

		/* Thanks to Govno for allocating the memory. */
        if ( !( load_char_obj ( ( victDD = malloc( sizeof( DESCRIPTOR_DATA) ) ), arg ) ) )
      	{
        	/* If not in save files, they don't exist. */
	    send_to_char ( "This character does not exist.\n\r", ch );
	    return;
    	}
        else
            send_to_char ( "This character is not logged in.\n\r", ch );
        victim = victDD->original ? victDD->original : victDD->character;
    }

    /* Display general character info. */
    sprintf( buf, "Name: %s. Title: %s&n.\n\r",
    victDD->original  ? victDD->original->name  :
    victDD->character ? victDD->character->name : "(none)",
    ( victim->pcdata && victim->pcdata->title )
        ? victim->pcdata->title : "(none)" );
    send_to_char( buf, ch );
    sprintf( buf, "Clan: %s&n. Level: %d&n.\n\r",
    ( victim->pcdata && victim->pcdata->clan )
        ? victim->pcdata->clan->name : "(none)", 
    victim->level );
    send_to_char( buf, ch );
    sprintf( buf, "Class: %s&n. Room: %d&n.  Race: %s&n.\n\r",
	( victim->class < MAX_CLASS && victim->class >=0 ) ?
        class_table[victim->class]->who_name : "(Undefined)",
	victim->in_room ? victim->in_room->vnum : -1, 
	(victim->race < MAX_RACE && victim->race >= 0 ) ?
        race_table[victim->race].colorname : "(Undefined)" );
    send_to_char( buf, ch );

    /* Messages specific to whether a player is logged in. */
    if ( logged_in )
    {
	if ( victim->in_room )
	{
	    sprintf( buf, "In room: %s&n (%d).\n\r", 
		victim->in_room->name ? victim->in_room->name : "(none)", 
		victim->in_room->vnum );
	    send_to_char( buf, ch );
	}
    	sprintf( buf, "On since: %s", ctime( &victim->logon ) );
    }
    else
	sprintf( buf, "Created: %s", ctime( (time_t *) &victim->pcdata->created ) );
    send_to_char( buf, ch );

    if ( logged_in )
    {
    	sprintf( buf, "Last save: %s", ctime( &victim->save_time ) );
    	send_to_char( buf, ch );
    }
    else
    {
        sprintf( buf, "Last logged in: %s", ctime( &victim->save_time ) );
        send_to_char( buf, ch );
    }

    /* Close rented chars pfile (w/out saving!). */
    if ( !logged_in )
        free( victDD );

    return;
}

void	do_zreset( CHAR_DATA *ch, char *argument )
{
        AREA_DATA *pArea;
	ROOM_INDEX_DATA *location;
        char    buf [ MAX_STRING_LENGTH ];
	char	arg [ MAX_STRING_LENGTH ];

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
            pArea = ch->in_room->area;
            reset_area( pArea );
            sprintf( buf, "&+WZone&n %s &+Wreset.&n\r\n", pArea->filename );
	    send_to_char( buf, ch );
	    sprintf( buf, "%s has just been reset by %s.", pArea->filename,
                     ch->name );
	    wiznet( NULL, WIZ_RESETS, L_DIR, buf );
            return;
        }
        else
        {
	    if ( !( location = find_location( ch, arg ) ) )
	    {
		send_to_char( "No such location.\n\r", ch );
		return;
	    }
	    reset_area( location->area );
	    sprintf( buf, "&+WZone&n %s &+Wreset.&n\n\r", location->area->filename );
	    send_to_char( buf, ch );
            sprintf( buf, "%s has just been reset by %s.", 
                     location->area->filename, ch->name   );
            wiznet( NULL, WIZ_RESETS, L_DIR, buf );

	    return;
        }
}

// God command bitvector calculator - Veygoth
void do_bitflag( CHAR_DATA *ch, char *argument )
{
    int number;
    char buf[MAX_STRING_LENGTH];

    if( !is_number( argument ))
    {
       send_to_char( "You can't do math without a number.\n\r", ch );
       return;
    }

    number = atoi( argument );

    sprintf( buf, "The bit value for that number is:" );

    if( IS_SET( number, BV00 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV01 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV02 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV03 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV04 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV05 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV06 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV07 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV08 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV09 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV10 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV11 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV12 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV13 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV14 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV15 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV16 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV17 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV18 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV19 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV20 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV21 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV22 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV23 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV24 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV25 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV26 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV27 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV28 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV29 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );
    if( IS_SET( number, BV30 ))
      strcat( buf, "1" );
    else
      strcat( buf, "0" );

    strcat( buf, "\n\r" );
    send_to_char( buf, ch );
    return;
}

void do_killproc( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA 	*rch;

    rch = get_char( ch );

    if ( get_trust( rch ) < 60 )
    	return;

    if ( !IS_NPC( ch ) )
    {
	system( "kill -9 `ps aux | grep magma | grep 6666 | cut -c 10-14`" );
	send_to_char( "Mud Terminated with extreme prejudice!\n\r", ch );
    }
    return;

}

char *random_sentence( void )
{
    static char       buf  [ MAX_STRING_LENGTH ];
    const  NOUN_TYPE *noun;
    const  VERB_TYPE *verb;

    noun = &noun_table[ number_range( 0, MAX_NOUN-1 ) ];
    verb = &verb_table[ number_range( 0, MAX_VERB-1 ) ];

    while ( noun->animate == FALSE )
        noun = &noun_table[ number_range( 0, MAX_NOUN-1 ) ];

    if ( number_percent() > 50 )
    {
        if ( is_vowel( noun->name ) )
            sprintf( buf, "An " );
        else
            sprintf( buf, "A " );
    }
    else
        sprintf( buf, "The " );

    strcat( buf, noun->name );
    strcat( buf, " " );
    strcat( buf, verb->name );
    if ( verb->dobject != NO_DOBJ )
    {
        noun = &noun_table[ number_range( 0, MAX_NOUN-1 ) ];

        if ( is_vowel( noun->name ) )
            strcat( buf, " an " );
        else
            strcat( buf, " a " );

        strcat( buf, noun->name );
    }

    if ( number_percent() > 80 )
        strcat( buf, "!" );
    else
        strcat( buf, "." );

    return buf;

}

void do_random( CHAR_DATA *ch, char *argument )
{
    send_to_char( "Random sentance: '", ch);
    send_to_char( random_sentence(), ch );
    send_to_char( "'\n\r", ch );
}

void do_add_lang( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( !(victim = get_char_world( ch, argument ) ) )
        send_to_char( "You don't see that person.\n\r", ch );
    else
    {
        char       buf  [ MAX_STRING_LENGTH ];

        if( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPCS!\n\r", ch );
            return;
        }

        add_languages( victim );
        sprintf( buf, "%s has had their languages reset.\n\r", victim->name );
        send_to_char( buf, ch); 

    }
}

void do_lset( CHAR_DATA *ch, char *argument )
{
    char       buf  [ MAX_STRING_LENGTH ];
    char      *buf2;
    CHAR_DATA *victim;
    int        lang;
    int        num;

    if (argument[0] == '\0')
    {
        send_to_char( "Syntax: set language <char> <language> <number>.\n\r", ch );
        return;
    }

    buf2 = one_argument( argument, buf );

    if ( !(victim = get_char_world( ch, buf ) ) )
        send_to_char( "You don't see that person.\n\r", ch );

    buf2 = one_argument( buf2, buf);
    if ( !(lang = int_lang( ch, buf ) ) )
    {
        send_to_char( "Syntax: set language <char> &+r<language>&n <number>.\n\r", ch );
        return;
    }

    buf2 = one_argument( buf2, buf);

    if ( !(num = atoi(buf) ) )
    {
        send_to_char( "Syntax: set language <char> <language> &+r<number>&n\n\r", ch );
        return;
    }

}

void do_mobstat_save( CHAR_DATA *ch, char *argument )
{
    FILE      *fp;
    char       strsave [ MAX_INPUT_LENGTH ];
    int        count;

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, "races.txt" );

    if ( !( fp = fopen( strsave, "w" ) ) )
    {
        bug( "mobstat_save: fopen", 0 );
    }
    else
    {
        /* add any help files to be loaded at startup to this section */

        for ( count = 0; count < MAX_RACE; count++ )
            fprintf( fp, "%24s\t%s\n", race_table[count].name,
               size_string(race_table[count].size) );

        fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_alist( CHAR_DATA * ch, char *argument )
{
    AREA_DATA *pArea;
    char       buf  [MAX_STRING_LENGTH];

    sprintf( buf, "[%3s] [%-27s] [%-5s/%5s] [%-10s] %3s [%-10s]\n\r",
    "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );
    send_to_char( buf, ch );
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        sprintf( buf, "[%3d] %-7.7s %-21.21s &n[%5d/%-5d] %-12.12s [%1.1d] [%-10.10s]&n\n\r",
                pArea->vnum,
                pArea->author,
                pArea->name,
                pArea->lvnum,
                pArea->uvnum,
                pArea->filename,
                pArea->security,
                pArea->builders );
        send_to_char( buf, ch );
    }

    return;
}

void do_monkstat( CHAR_DATA *ch, char *argument )
{
   int count;
   char buf[MAX_STRING_LENGTH];

   for( count = 0; count < MAX_MONK_SKILL; count++)
   {
     sprintf( buf, "Skill:  %30s  Point Costs:  %d\n\r",
       monk_skills[count].skill_name, monk_skills[count].points[0] );
     send_to_char( buf, ch );
   }

   return;
}

// Not modified yet - Veygoth
void do_monk_sset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: set monkskill <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     set monkskill <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any monk skill.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch->level <= victim->level && ch != victim && str_cmp( ch->name, "Veygoth" ))
    {
	send_to_char( "You may not set your peer nor your superior.\n\r",
		     ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = monk_skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such monk skill.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > MAX_SKILL_ADEPT )
    {
	send_to_char( "Value range is 0 to 95.\n\r", ch );
	return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Only Directors may set all.\n\r", ch );
	    return;
	}
	for ( sn = 0; sn < MAX_MONK_SKILL; sn++ )
	{
		victim->pcdata->monk_skill[sn] = value;
	}
    }
    else
    {
        victim->pcdata->monk_skill[sn] = value;
    }

    return;
}

void do_statdump( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    char            buf  [ MAX_STRING_LENGTH   ];
    int             vnum;
    FILE *fp;

    fclose( fpReserve );

    if ( !( fp = fopen( "objects.txt", "w" ) ) )
      return;

    for ( vnum = 100; vnum < 100000; vnum++ )
    {
	if ( !( pObjIndex = get_obj_index( vnum ) ) )
	    continue;

        obj = create_object( pObjIndex, 0 );

        sprintf( buf, "%d, %s, %d, %d, %d, %d, %d, %d\n",
		pObjIndex->vnum, pObjIndex->short_descr,
                pObjIndex->item_type, obj->cost,
                obj->value[0],
                obj->value[1],
                obj->value[2],
                obj->value[3]
                );

        fprintf( fp, "%s", buf );
    }

    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

void do_namesucks( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !authorized( ch, "namesucks" ) )
        return;

    if( is_name( argument, strName ))
    {
        send_to_char( "That name is already banned.\n\r", ch );
        return;
    }

    strcat( strName, argument );
    strcat( strName, " " );

    sprintf( buf, "%s is now considered an illegal name.", argument );
    act( buf, ch, NULL, NULL, TO_CHAR );
    log_string( buf );
     
    save_names();
}

void restore( CHAR_DATA *ch, CHAR_DATA *victim)
{
    last_fun( "restore");
    if (victim->hit < get_max_hit( victim ) )
        victim->hit = get_max_hit( victim );
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    if ( !IS_NPC( victim ) ) {
        victim->pcdata->condition[COND_FULL] = 48;
        victim->pcdata->condition[COND_THIRST] = 48;
        victim->pcdata->condition[COND_DRUNK] = 0;
        purge_innate_timers( victim );
    }
    if ( is_affected( victim, AFF_BLIND) )
        remove_blindness( victim );
    if ( is_affected( victim, AFF_POISON ) )
        remove_poison( victim );
    update_pos( victim );
    act( "$n has restored you.", ch, NULL, victim, TO_VICT );
    return;
}

void do_racestat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA         *rch;
    char               buf  [ MAX_STRING_LENGTH * 2 ];
    char               buf1 [ MAX_STRING_LENGTH * 6 ];
    int                num;
    char arg [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    /*
     * Parse arguments.
     */
	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
        {
            num = rch->class;
        }
        else if ( is_number( arg ) )
	{
	    num = atoi( arg );
        }
        else
        {
            num = race_lookup( arg );
        }

        if( num < 0 || num > MAX_RACE )
        {
   	  send_to_char( "Invalid race number!\n\r", ch );
	  return;
        }

        sprintf( buf1, "&+WRacial stats for race %d&n\n\r\n\r", num );

        sprintf( buf, "Name:  %s  ANSI Name: %s\n\r",
                 race_table[num].name,
                 race_table[num].colorname );
        strcat( buf1, buf );

        sprintf( buf, "Key:  %s  Abilities: %d  Size:  %d\n\r",
                 race_table[num].key,
                 race_table[num].race_abilities,
                 race_table[num].size );
        strcat( buf1, buf );
        
        sprintf( buf, "Height:  %d  Weight:  %d  Base Alignment:  %d\n\r",
                 race_table[num].height,
                 race_table[num].weight,
                 race_table[num].base_align );
        strcat( buf1, buf );

        sprintf( buf, "\n\r&+WStat modifiers: (Max. stats)&n\n\r\n\r" );
        strcat( buf1, buf );

        sprintf( buf, "Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d\n\rAgi: %d  Cha: %d  Pow: %d  Luk: %d\n\r",
                 race_table[num].str_mod,
                 race_table[num].int_mod,
                 race_table[num].wis_mod,
                 race_table[num].dex_mod,
                 race_table[num].con_mod,
                 race_table[num].agi_mod,
                 race_table[num].cha_mod,
                 race_table[num].pow_mod,
                 race_table[num].luk_mod );
        strcat( buf1, buf );

        sprintf( buf, "HP Gain (unused):  %d  Mana Gain: %d  Move Gain(unused): %d\n\r",
                 race_table[num].hp_gain,
                 race_table[num].mana_gain,
                 race_table[num].move_gain );
        strcat( buf1, buf );

        sprintf( buf, "Thirst Mod (unused):  %d  Hunger Mod (unused): %d\n\rDamage Message: %s  Walk Message: %s\n\r",
                 race_table[num].thirst_mod,
                 race_table[num].hunger_mod,
                 race_table[num].dmg_message,
                 race_table[num].walk_message );
        strcat( buf1, buf );

        sprintf( buf, "Hatred:  %s\n\rParts:  %d  Resist:  %d  Immune:  %d  Susceptible: %d\n\r",
                 race_table[num].hate,
                 race_table[num].parts,
                 race_table[num].resistant,
                 race_table[num].immune,
                 race_table[num].susceptible );
        strcat( buf1, buf );

        sprintf( buf, "Racewar Side: %d  Language:  %d  Base Age: %d  Max Age: %d\n\r",
                 race_table[num].racewar_side,
                 race_table[num].language,
                 race_table[num].base_age,
                 race_table[num].max_age );
        strcat( buf1, buf );

    send_to_char( buf1, rch );
    return;
}
void do_reset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch, *victim;

    rch = get_char( ch );

    if ( !authorized( rch, "reset" ) )
        return;

    victim = get_char_world( rch, argument );
    if ( !victim ) {
        send_to_char( "To reset a char's stats type RESET CHARNAME.\n\r", ch);
        return;
    }
    reset_stats( victim );
    return;
}


