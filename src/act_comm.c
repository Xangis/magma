/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
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

char lbuf[MAX_STRING_LENGTH];

/*
 * Local functions.
 */
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );
bool	check_group	args( ( CHAR_DATA *ch ) );
void	door_trigger	args( ( char *arg, CHAR_DATA *ch ) );


bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( IS_IMMORTAL( ch ) && (   is_name( "immortal",  pnote->to_list )
			       || is_name( "immortals", pnote->to_list )
			       || is_name( "imm",       pnote->to_list )
			       || is_name( "immort",    pnote->to_list ) ) )
	return TRUE;

    if ( is_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}

void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    last_fun( "note_attach");
    if ( ch->pnote )
	return;

    if ( !note_free )
    {
	pnote	  = alloc_perm( sizeof( *ch->pnote ) );
        top_note++;
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    ch->pnote		= pnote;
    return;
}

void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    FILE      *fp;
    NOTE_DATA *prev;
    char      *to_list;
    char       to_new  [ MAX_INPUT_LENGTH ];
    char       to_one  [ MAX_INPUT_LENGTH ];
    char       strsave [ MAX_INPUT_LENGTH ];

    last_fun( "note_remove");
    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " "    );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for ( prev = note_list; prev; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( !prev )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->next	= note_free;
    note_free	= pnote;

    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, NOTE_FILE );

    if ( !( fp = fopen( strsave, "w" ) ) )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender			);
	    fprintf( fp, "Date    %s~\n", pnote->date			);
	    fprintf( fp, "Stamp   %ld\n", (unlong) pnote->date_stamp	);
	    fprintf( fp, "To      %s~\n", pnote->to_list		);
	    fprintf( fp, "Subject %s~\n", pnote->subject		);
	    fprintf( fp, "Text\n%s~\n\n", fix_string( pnote->text )	);
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/* Date stamp idea comes from Alander of ROM */
void do_note( CHAR_DATA *ch, char *argument )
{
    NOTE_DATA *pnote;
    char       buf     [ MAX_STRING_LENGTH   ];
    char       buf1    [ MAX_STRING_LENGTH*7 ];
    char       arg     [ MAX_INPUT_LENGTH    ];
    char       strsave [ MAX_INPUT_LENGTH    ];
    int        vnum;
    int        anum;

    last_fun( "do_note");
    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[0] == '\0' )
    {
	do_note( ch, "read" );
	return;
    }

    if ( !str_cmp( arg, "list" ) )
    {
	vnum    = 0;
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "[%3d%s] %s: %s\n\r",
			vnum,
			( pnote->date_stamp > ch->last_note
			 && str_cmp( pnote->sender, ch->name ) ) ? "N" : " ",
			pnote->sender, pnote->subject );
		strcat( buf1, buf );
		vnum++;
	    }
	}
	send_to_char( buf1, ch );
	return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( argument[0] == '\0' || !str_prefix( argument, "next" ) )
	  /* read next unread note */
	{
	    vnum    = 0;
	    buf1[0] = '\0';
	    for ( pnote = note_list; pnote; pnote = pnote->next )
	    {
		if ( is_note_to( ch, pnote )
		    && str_cmp( ch->name, pnote->sender )
		    && ch->last_note < pnote->date_stamp )
		{
		    break;
		}
		else
		{
		    if ( is_note_to( ch, pnote ) )
		        vnum++;
		}
	    }
	    if ( pnote )
	    {
		sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
			vnum,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list );
		strcat( buf1, buf );
		strcat( buf1, pnote->text );
		ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		send_to_char( buf1, ch );
		return;
	    }
	    send_to_char( "You have no unread notes.\n\r", ch );
	    return;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\n\r", ch );
	    return;
	}

	vnum    = 0;
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
	        if ( vnum == anum || fAll )
		{
		    sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
			    vnum,
			    pnote->sender,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list );
		    strcat( buf1, buf );
		    strcat( buf1, pnote->text );
		    if ( !fAll )
		      send_to_char( buf1, ch );
		    else
		      strcat( buf1, "\n\r" );
		    ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		    if ( !fAll )
		      return;
		}
		vnum++;
	    }
	}

	if ( !fAll )
	    send_to_char( "No such note.\n\r", ch );
	else
	    send_to_char( buf1, ch );
	return;
    }

    if ( !str_cmp( arg, "edit" ) )
    {
	note_attach( ch );
        ch->pcdata->editing = EDIT_NOTE;
	string_append( ch, &ch->pnote->text );
	return;
    }

    if ( !str_cmp( arg, "subject" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "to" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
		ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_cmp( arg, "post" ) || !str_prefix( arg, "send" ) )
    {
	FILE *fp;
	char *strtime;

	if ( !ch->pnote )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->to_list, "" ) )
	{
	    send_to_char(
	      "You need to provide a recipient (name, all, or immortal).\n\r",
			 ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->subject, "" ) )
	{
	    send_to_char( "You need to provide a subject.\n\r", ch );
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	free_string( ch->pnote->date );
	ch->pnote->date			= str_dup( strtime );
	ch->pnote->date_stamp           = current_time;

	if ( !note_list )
	{
	    note_list	= ch->pnote;
	}
	else
	{
	    for ( pnote = note_list; pnote->next; pnote = pnote->next )
		;
	    pnote->next	= ch->pnote;
	}
	pnote		= ch->pnote;
	ch->pnote       = NULL;

	fclose( fpReserve );

	sprintf( strsave, "%s%s", SYSTEM_DIR, NOTE_FILE );

	if ( !( fp = fopen( strsave, "a" ) ) )
	{
	    perror( NOTE_FILE );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender			);
	    fprintf( fp, "Date    %s~\n", pnote->date			);
	    fprintf( fp, "Stamp   %ld\n", (unlong) pnote->date_stamp	);
	    fprintf( fp, "To      %s~\n", pnote->to_list		);
	    fprintf( fp, "Subject %s~\n", pnote->subject		);
	    fprintf( fp, "Text\n%s~\n\n", fix_string( pnote->text )	);
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
	if ( !is_number( argument ) )
	{
            send_to_char( "Note Remove which number?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && vnum++ == anum )
	    {
		note_remove( ch, pnote );
		send_to_char( "Ok.\n\r", ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    send_to_char( "Huh?  Type '&+LHelp Note&n' for usage.\n\r", ch );
    return;
}

/*
 * How to make a string look drunk by Apex <robink@htsa.hva.nl>
 * Modified and enhanced by Maniac from Mythran
 */
char *makedrunk( char *string, CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    char temp;
    int  randomnum;
    int  drunkpos;
    int  drunklevel = 0;
    int  pos        = 0;

    /* Check how drunk a person is... */
    if ( !IS_NPC( ch ) )
    {
        if ( ( drunklevel = ch->pcdata->condition[ COND_DRUNK ] ) > 0 )
	{
	    do {
	        temp     = UPPER( *string );
		drunkpos = temp - 'A';

		if ( ( temp >= 'A' ) && ( temp <= 'Z' ) )
		{
		    if ( drunklevel > drunk[ drunkpos ].min_drunk_level )
		    {
			randomnum =
			  number_range( 0, drunk[ drunkpos ].number_of_rep );
			strcpy( &buf[ pos ],
			       drunk[ drunkpos ].replacement[ randomnum ] );
			pos +=
			  strlen( drunk[ drunkpos ].replacement[ randomnum ] );
		    }
		    else
		        buf[ pos++ ] = *string;
		}
		else
		{
		    if ( ( temp >= '0' ) && ( temp <= '9' ) )
		    {
			temp         = '0' + number_range( 0, 9 );
			buf[ pos++ ] = temp;
		    }
		    else
		        buf[ pos++ ] = *string;
		}
	    }
	    while ( *string++ );
	    buf[ pos ] = '\0';
	    strcpy( string, buf );
	}
    }
    return( string );
}

// Function is ready for tweaking. - Lohrr
char *translate( const char *string, CHAR_DATA *ch, CHAR_DATA *victim )
{
    static const char *alphabet  = "ecdfighjaklmnpyqrstvowxzub ";
    static const char *numbers   = "1234567890";
    static       char  buf       [ MAX_STRING_LENGTH   ];
    int                i;
    int                lang = IS_NPC(ch) ? race_table[ch->race].language
                                         : ch->pcdata->speaking;

    /* Step 1: Copy string. */
    for (i = 0; string[i] != '\0'; i++ )
        buf[i] = string[i];
    buf[i] = '\0';

    /* Step 2: Immortals have perfect speech & NPCs have perfect ears &
     *   nobody misunderstands themselves.
     */
    if( IS_IMMORTAL( victim ) || IS_IMMORTAL ( ch ) || IS_NPC( victim )   
          || ( ch == victim ) || IS_NPC( ch ) )
        return buf;

    /* Step 3: Check to see if ch says everything right.
     * NPC's never say anything wrong.
     */
    if( !IS_NPC( ch ) )
    for( i = 0; buf[i] != '\0'; i++ )
    {
        if( number_percent() > ch->pcdata->lang_lrn[lang]
            && number_percent() < 50  )
        {
            if ( isalpha(buf[i]) )
            {
                if ( islower( buf[i] ) )
                    buf[i] = alphabet[buf[i] - 'a'];
                else
                    buf[i] = UPPER(alphabet[buf[i] - 'A']);
            }
            else if ( isdigit(buf[i] ) )
                buf[i] = numbers[buf[i] - '0'];
            else
                buf[i] = buf[i];
        }
    }

    /* Step 4: Check for comp.  If so, victim understands perfectly. */
    if( is_affected( victim, AFF_COMP_LANG ) || ( lang == LANG_UNKNOWN ) )
        return buf;

    lang_practice( victim, lang );

    /* Step 5: Check to see if victim hears everything right. */
    for( i = 0; buf[i] != '\0'; i++ )
    {
        if( number_percent() > victim->pcdata->lang_lrn[lang]
            && number_percent() < 50  )
        {
            if ( isalpha(buf[i]) )
            {
                if ( islower( buf[i] ) )
                    buf[i] = alphabet[buf[i] - 'a'];
                else
                    buf[i] = UPPER(alphabet[buf[i] - 'A']);
            }
            else if ( isdigit(buf[i] ) )
                buf[i] = numbers[buf[i] - '0'];
            else
                buf[i] = buf[i];
        }
    }

    /* Step 6: return the (probably messed up if it got this far) string. */
    return buf;
}

/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel,
		  const char *verb )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    int              position;

    last_fun( "talk_channel");
    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\n\r", verb );
	buf[0] = UPPER( buf[0] );
	return;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	sprintf( buf, "You can't %s\n\r", verb );
	send_to_char( buf, ch );
	return;
    }

    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    REMOVE_BIT( ch->deaf, channel );

    /*
     * Make the words look drunk if needed...
     */
    argument = makedrunk( argument, ch );

    switch ( channel )
    {
    default:
	sprintf( buf, "You %s '%s'\n\r", verb, argument );
	send_to_char( buf, ch );
	sprintf( buf, "$n&n %ss '$t'",     verb );
	break;
 
     case CHANNEL_SHOUT:
 	sprintf( buf, "You %s '%s'\n\r", verb, argument );
 	send_to_char( buf, ch );
 	sprintf( buf, "$n&n %ss '$t'",     verb );
 	break;
 
     case CHANNEL_YELL:
 	sprintf( buf, "You %s '%s'\n\r", verb, argument );
 	send_to_char( buf, ch );
 	sprintf( buf, "$n&n %ss '$t'",     verb );
 	break;
 
     case CHANNEL_CLANTALK:
        sprintf( buf, "&+L[&+C$n&+L]&n&+c $t&n" );
	act( buf, ch, argument, NULL, TO_CHAR );
 	break;

    case CHANNEL_IMMTALK:
        sprintf( buf, "&+L[&+r$n&+L]&n $t" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	    && vch != ch
	    && !IS_SET( och->deaf, channel )
            && !IS_ROOM( och->in_room, ROOM_SILENT ) )
	{
	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( och ) )
		continue;
	    if ( channel == CHANNEL_CLANTALK
	        && ( !is_clan( och )
	        || !is_same_clan( ch, och ) ) )
		continue;
	    if ( channel == CHANNEL_SHOUT
		&& vch->in_room->area != ch->in_room->area && !IS_IMMORTAL( vch ) )
	        continue;

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
            if( channel == CHANNEL_SHOUT || channel == CHANNEL_YELL )
            {
              if( ( IS_NPC( ch ) && 
                    ( ch->pIndexData->vnum == 9316 
                      || ch->pIndexData->vnum == 9748 ) )
                  || ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_SHOUT ) ) )
              {
                 // Add foreign language filter
                 if ( !IS_NPC( ch ) )
                     act( buf, ch, translate( argument, ch, och ), vch, TO_VICT );
                 else
                     act( buf, ch, argument, vch, TO_VICT);
              }
            }
            else
            {
                act( buf, ch, argument, vch, TO_VICT );
            }
	    vch->position	= position;
	}
    }

    return;
}

void do_shout( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_shout");
    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}

void do_yell( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_yell");
    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
  
    last_fun( "do_immtalk");
    rch = get_char( ch );

    if ( !authorized( rch, "wiz" ) )
        return;

    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}

void do_wartalk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
  
    last_fun( "do_wartalk");
    rch = get_char( ch );

    if ( IS_NPC( rch ) || !is_clan( rch ) )
    {
        send_to_char( "You aren't a &+Lclansman&n!\n\r", ch );
        return;
    }

    talk_channel( ch, argument, CHANNEL_CLANTALK, "clantalk" );
    return;
}

void do_say( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char  buf [ MAX_STRING_LENGTH ];

    last_fun( "do_say");
    if ( argument[0] == '\0' )
    {
 	send_to_char_bw( "Say what?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    argument = makedrunk( argument, ch );

    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;
        if ( och == ch || IS_NPC( och ) )
            continue;

        if ( IS_NPC( ch ) || (!IS_NPC( ch ) && 
          ( ch->pcdata->speaking == LANG_GOD ||
          ch->pcdata->speaking == LANG_UNKNOWN ) ) )
            sprintf( buf, "%s says '&n$T&n'", PERS( ch, och ) );
        else
            sprintf( buf, "%s says in %s '&n$T&n'", PERS( ch, och ),
                IS_NPC( ch ) ? lang_table[race_table[ch->race].language]
                             : lang_table[ch->pcdata->speaking] );

        // Add foreign language filter.
        act( buf, och, NULL, translate( argument, ch, och ), TO_CHAR );
    }

    MOBtrigger = FALSE;
    act( "You say '&n$T&n'", ch, NULL, argument, TO_CHAR );
    prog_speech_trigger( argument, ch );
    door_trigger( argument, ch );
    return;
}

void do_tell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char              arg [ MAX_INPUT_LENGTH ];
    char              buf [ MAX_STRING_LENGTH ];
    QUEST_INDEX_DATA *quest;
    TALK_DATA        *message;
    int               position;
    int               lang;

    last_fun( "do_tell");

    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     *
     * get PC target first, if fails then get NPC
     * -- Lithos
     */
    
    if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if( victim == ch )
    {
	send_to_char( "You try to tell yourself something.\n\r", ch );
	return;
    }

    if( ( race_table[victim->race].racewar_side !=
            race_table[ch->race].racewar_side ) 
        && ( !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim) )
        && ( ch->in_room != victim->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /* Can tell to other side of racewar iff the opponent is in the same
	room or one of the people are Immortals. */
    if ( ( !IS_IMMORTAL( ch ) && !IS_IMMORTAL( victim ) )
	 && ( race_table[ch->race].racewar_side !=
	      race_table[victim->race].racewar_side )
	 && ( victim->in_room != ch->in_room )		  )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ( !IS_NPC( ch ) && (      IS_SET(     ch->act, PLR_SILENCE )
                             ||   !IS_SET(     ch->act, PLR_TELL ) 
                             || ( !IS_NPC( victim ) && 
                                  !IS_SET( victim->act, PLR_TELL ) ) ) )
        || IS_ROOM( victim->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your message didn't get through.\n\r", ch );
        return;
    }

    if ( !victim->desc && !IS_NPC( victim ) )
    {
        act( "$N&n is &+Llinkdead&n.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC( victim ) && victim->pcdata->ignore == ch )
    {
	act( "$E is ignoring you.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = makedrunk( argument, ch );

    position		= victim->position;
    victim->position	= POS_STANDING;
    lang = IS_NPC( ch ) ? race_table[ch->race].language :
                          ch->pcdata->speaking;
    if ( lang == LANG_GOD || lang == LANG_UNKNOWN )
        sprintf( buf, "&+WYou tell $N&+W '$t&+W'&n");
    else
        sprintf( buf, "&+WYou tell $N&+W in %s '$t&+W'&n", lang_table[lang] );

    act( buf, ch, argument, victim, TO_CHAR );
    if ( lang == LANG_GOD || lang == LANG_UNKNOWN )
        sprintf( buf, "&+W$n&+W tells you '$t&+W'&n");
    else
        sprintf( buf, "&+W$n&+W tells you in %s '$t&+W'&n", lang_table[lang] );
    act( buf, ch, translate( argument, ch, victim ), victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    if ( IS_SET( victim->act, PLR_AFK ) )
        act( "Just so you know, $E is &+RAFK&n.", ch, NULL, victim, TO_CHAR );

    // players can't have talk files -- go home!  Quest stuff.
    if( !IS_NPC( victim ))
      return;

    for( quest = quest_first; quest; quest = quest->next )
    {
         bool isquest = (IS_IMMORTAL(ch) && !str_cmp(argument, "quest")) ? TRUE : FALSE;

         if( !quest->messages || (quest->vnum != victim->pIndexData->vnum ))
            continue;
         for( message = quest->messages; message; message = message->next )
         {
           if( is_name( argument, message->keywords ) || isquest)
            {
              send_to_char( "\n\r", ch );
              send_to_char( message->message, ch );
            }
         }
    }

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int        position;

    last_fun( "do_reply");
    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    if ( !( victim = ch->reply ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ( !IS_NPC( ch ) && (      IS_SET(     ch->act, PLR_SILENCE )
                             ||   !IS_SET(     ch->act, PLR_TELL )
                             || ( !IS_NPC( victim ) && 
                                  !IS_SET( victim->act, PLR_TELL ) ) ) )
        || IS_ROOM( victim->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your message didn't get through.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Reply what?\n\r", ch );
        return;
    }

    if ( !victim->desc )
    {
        act( "$N is &+Llinkdead&n.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( !IS_NPC( victim ) && victim->pcdata->ignore == ch )
    {
	act( "$E is ignoring you.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = makedrunk( argument, ch );

    act( "&+WYou tell $N&+W '$t&+W'&n", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "&+W$n&+W tells you '$t&+W'&n", ch, translate( argument, ch, victim ), victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    if ( IS_SET( victim->act, PLR_AFK ) )
        act( "Just so you know, $E is &+RAFK&n.", ch, NULL, victim, TO_CHAR );

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char  buf [ MAX_STRING_LENGTH ];
    char *plast;

    last_fun( "do_emote");
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "You can't show your emotions.\n\r", ch );
	return;
    }

    if( ch->position == POS_FIGHTING || ch->fighting )
    {
        send_to_char( "You can't emote in combat.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\n\r", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha( plast[-1] ) )
	strcat( buf, "." );

    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;

        if ( och->deleted || och == ch )
	    continue;

        act( "$n $t", ch, translate( buf, ch, och), och, TO_VICT );
    }
    MOBtrigger = FALSE;
    act( "$n $T", ch, NULL, buf, TO_CHAR );
    return;
}

void do_crashbug( CHAR_DATA *ch, char *argument )
{
    char strsave [ MAX_INPUT_LENGTH ];
    char buf [ MAX_STRING_LENGTH ];
    char *strtime;

    last_fun( "do_crashbug");
    strtime                    = ctime( &current_time );
    strtime[strlen( strtime )-1] = '\0';

    sprintf( strsave, "%s%s", SYSTEM_DIR, CRASHBUG_FILE );

    if ( argument[0] == '\0' )
    {
        send_to_char( "The Implementors look at you quizzically.\n\r", ch );
        return;
    }

    sprintf( buf, "%s crashbugged at %s", argument, strtime );

    append_file( ch, strsave,  buf );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}


void do_bug( CHAR_DATA *ch, char *argument )
{
    char strsave [ MAX_INPUT_LENGTH ];
    char buf [ MAX_STRING_LENGTH ];
    char *strtime;
 
    last_fun( "do_bug");
    strtime                    = ctime( &current_time );
    strtime[strlen( strtime )-1] = '\0';

    sprintf( strsave, "%s%s", SYSTEM_DIR, BUG_FILE );

    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\n\r", ch );
	return;
    }

    sprintf( buf, "%s bugged at %s", argument, strtime ); 

    append_file( ch, strsave,  buf );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    char strsave [ MAX_INPUT_LENGTH ];

    last_fun( "do_idea");
    sprintf( strsave, "%s%s", SYSTEM_DIR, IDEA_FILE );

    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\n\r", ch );
	return;
    }

    append_file( ch, strsave,  argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    char strsave [ MAX_INPUT_LENGTH ];

    last_fun( "do_typo");
    sprintf( strsave, "%s%s", SYSTEM_DIR, TYPO_FILE );

    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\n\r", ch );
	return;
    }

    append_file( ch, strsave,  argument );
    send_to_char( "Ok.  Thanks.\n\r", ch );
    return;
}

void do_qui( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_qui");
    send_to_char( "If you want to &+LQUIT&n, you have to spell it out.\n\r", ch );
    return;
}


// This code is basically a hack, and I'm adding a boatload of log
// messages until I am satisfied that this code works and is stable.
// When messing with code like this, it is easy to have players that
// don't enter or leave the game completely, causing ghost images,
// player duplicates, weird dangling pointers, etc., so we have to
// be especially careful.  If anyone happens upon this and has a
// suggestion for how to handle anything better than I have it will
// be greatly appreciated. - Veygoth
void do_rent( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *room = NULL;
    CHAR_DATA *wch = NULL;

    last_fun( "do_rent");
    if ( IS_NPC( ch ) )
	return;

    if( !IS_ROOM( ch->in_room, ROOM_INN ))
    {
        send_to_char( "You must be within an inn to rent.\n\r", ch );
        return;
    }

    if ( ch->position == POS_FIGHTING || ch->fighting  )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
        send_to_char( "You're not &+RD&n&+rE&+RA&n&+rD&n yet.\n\r", ch );
	return;
    }

    send_to_char( "The innkeeper grabs a &+Lkey&n from the &n&+yrack&n, and shows you to your room.\n\r\n\r", ch );
    act( "$n&n has left the realm.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has rented.", ch->name );
    log_string( log_buf );

    wiznet ( ch, WIZ_LOGINS, get_trust( ch ), log_buf );

    /*
     * After extract_char the ch is no longer valid
     * that is why we aren't extracting the character but rather
     * sending them to our version of the "menu". - Veygoth
     */

    // I know we checked for position fighting, but I'm paranoid...
    if( ch->fighting )
        stop_fighting( ch, TRUE );

    die_follower( ch, ch->name );

    // I can't see any reason why ch would not have an ->in_room, but that
    // may just be shortsighted of me - Veygoth
    if( ch->in_room )
    {
        room = ch->in_room;
    }

    char_from_room( ch );
    if( room )
        ch->in_room = room;

    // Put them in the correct body
    if( ch->desc && ch->desc->original )
    {
      do_return( ch, "" );
    }

    for( wch = char_list; wch; wch = wch->next )
         if( wch->reply == ch )
             wch->reply = NULL;

    REMOVE_BIT( ch->act, PLR_CAMPING );
    ch->pcdata->lastrent = ch->in_room->vnum;
    save_char_obj( ch );

    // Now the part that is easy to screw up... we remove them from
    // the character list - Veygoth
    for( wch = char_list; wch; wch = wch->next )
    {
        // This should only happen if they are at the start of the char_list
        if( wch == ch )
        {
            char_list = ch->next;
        }
        // This should happen if they're on any other part of the list...
        else if( wch->next && wch->next == ch )
        {
            // Unnecessary statement, but it makes me feel better.
            if( ch->next )
            {
              wch->next = ch->next;
            }
            else
            {
              wch->next = NULL;
            }
        }
    }

    do_help( ch, "_login_menu" );

    ch->desc->connected = CON_MENU; // Con_menu is when they enter
                                         // the game... this shows menu
                                         // before they enter the game

    //This was the original code - Veygoth
    //d = ch->desc;
    //extract_char( ch, TRUE );
    //if ( d )
    //close_socket( d );

    return;
}

// This code is basically a hack, and I'm adding a boatload of log
// messages until I am satisfied that this code works and is stable.
// When messing with code like this, it is easy to have players that
// don't enter or leave the game completely, causing ghost images,
// player duplicates, weird dangling pointers, etc., so we have to
// be especially careful.  If anyone happens upon this and has a
// suggestion for how to handle anything better than I have it will
// be greatly appreciated. - Veygoth
void do_quit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *room = NULL;
    CHAR_DATA *wch = NULL;
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_quit");
    if ( IS_NPC( ch ) )
	return;

    if ( ch->position == POS_FIGHTING || ch->fighting )
    {
        if( IS_SET( ch->act, PLR_CAMPING ))
        {
          send_to_char( "You're not gifted enough to make camp and fight at the same time.\n\r", ch );
          REMOVE_BIT( ch->act, PLR_CAMPING );
          return;
        }
        else
	  send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position < POS_STUNNED  )
    {
      if( number_range( 1, 2 ) == 1 )
      {
        send_to_char( "You're not &+RD&n&+rE&+RA&n&+rD&n yet.\n\r", ch );
	return;
      }
      else
      {
        send_to_char( "Just lie still and finish &+RBle&+reding&n!\n\r", ch );
        return;
      }
    }

    if( IS_SET( ch->act, PLR_CAMPING ))
    {
        REMOVE_BIT( ch->act, PLR_CAMPING );
        act( "You climb into your bedroll and leave the realm.", ch, NULL, NULL, TO_CHAR );
	if ( ch->sex == 1 )
    	    act( "$n&n climbs into his bedroll and leaves the realm.", ch, NULL, NULL, TO_ROOM );
	else if ( ch->sex == 2 )
    	    act( "$n&n climbs into her bedroll and leaves the realm.", ch, NULL, NULL, TO_ROOM );
	else
    	    act( "$n&n climbs into its bedroll and leaves the realm.", ch, NULL, NULL, TO_ROOM );

        sprintf( buf, "%s has camped out.", ch->name );
        log_string( buf );
        wiznet( ch, WIZ_LOGINS, get_trust( ch ), buf );
    }
    else
    {
        send_to_char( "You leave the realm.\n\r\n\r", ch );
        act( "$n&n has left the realm.", ch, NULL, NULL, TO_ROOM );
        sprintf( log_buf, "%s has camped out.", ch->name );
        log_string( log_buf );
        wiznet ( ch, WIZ_LOGINS, get_trust( ch ), log_buf );
    }
 
    /*
     * After extract_char the ch is no longer valid
     * that is why we aren't extracting the character but rather
     * sending them to our version of the "menu". - Veygoth
     */

    // I know we checked for position fighting, but I'm paranoid...
    if( ch->fighting )
        stop_fighting( ch, TRUE );

    die_follower( ch, ch->name );

    // I can't see any reason why ch would not have an ->in_room, but that
    // may just be shortsighted of me - Veygoth
    if( ch->in_room )
    {
        room = ch->in_room;
    }

    char_from_room( ch );
    if( room )
        ch->in_room = room;

    // Put them in the correct body
    if( ch->desc && ch->desc->original )
    {
      do_return( ch, "" );
    }

    for( wch = char_list; wch; wch = wch->next )
         if( wch->reply == ch )
             wch->reply = NULL;
 
    ch->pcdata->lastrent = ch->in_room->vnum;
    save_char_obj( ch );

    // Now the part that is easy to screw up... we remove them from
    // the character list - Veygoth
    for( wch = char_list; wch; wch = wch->next )
    {
        // This should only happen if they are at the start of the char_list
        if( wch == ch )
        {
            char_list = ch->next;
        }
        // This should happen if they're on any other part of the list...
        else if( wch->next && wch->next == ch )
        {
            // Unnecessary statement, but it makes me feel better.
            if( ch->next )
            {
              wch->next = ch->next;
            }
            else
            {
              wch->next = NULL;
            }
        }
    }

    do_help( ch, "_login_menu" );

/* Govno .. added this to try to alleviate crashes */
// Veygoth - added the descriptor check too.

    if ( ch && ch->desc )
    {
      ch->desc->connected = CON_MENU; // Con_menu is when they enter
    }                                    // the game... this shows menu
                                         // before they enter the game

    return;
}

// The camp function now simply creates a camp event.
// The do_quit function handles quitters and campers, based
// on the camping bit.  The only goofy side effect of this is
// that an immortal who is camping can quit and get the
// "you roll up in your bedroll" message.
void do_camp( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;

    last_fun( "do_camp");
    if ( ch->position == POS_FIGHTING || ch->fighting )
    {
        send_to_char( "You're not gifted enough to make camp and fight at the same time.\n\r", ch );
	REMOVE_BIT( ch->act, PLR_CAMPING );
	return;
    }

    if (ch->fly_level) {
        send_to_char("Perhaps it would be more comfortable on the ground.\n\r", ch);
        return;
    }

    if( IS_SET( ch->act, PLR_CAMPING ))
    {
        send_to_char( "Your preparations are not quite complete.\n\r", ch );
        return;
    }    


    if ( ch->position < POS_STUNNED )
    {
        send_to_char( "Just lie still and finish &+RBle&+reding&n!\n\r", ch );
	return;
    }
    
    act( "$n&n starts to set up camp.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You start to set up camp.\n\r", ch);
    SET_BIT( ch->act, PLR_CAMPING );
    // Pass the character, the room they started camping in, and the
    // number of cycles to camp for
    // Pulse camp is 5 seconds, so make them wait for 1.5 minutes
    create_event( EVENT_CAMP, PULSE_CAMP, ch, ch->in_room, 18 );

    return;
}


void do_save( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_save");
    if ( IS_NPC( ch ) )
	return;

    send_to_char( "Saving.\n\r", ch );
    save_char_obj( ch );
    return;
}

void do_follow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_follow");
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) && ch->master )
    {
	act( "But you'd rather follow $N&n!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim->master == ch )
    {
        send_to_char( "No following in loops!\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    PERSON_DATA *follower;  

    last_fun( "add_follower");
    if( !master )
    {
        bug( "Add_follower function called with no argument for master.\n\r", 0 );
        return;
    }

    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    follower = alloc_mem( sizeof( PERSON_DATA ));

    ch->master        = master;

    // Put the follower at the top of the list and set the
    // previous of the old one to point to it
    follower->prev    = NULL;
    follower->next    = master->followers;
    follower->who     = ch;
    if( master->followers )
      master->followers->prev = follower;
    master->followers = follower;

    if ( can_see( master, ch ) )
	act( "$n&n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N&n.",  ch, NULL, master, TO_CHAR );

    return;
}

void add_follower_nomsg( CHAR_DATA *ch, CHAR_DATA *master )
{
    PERSON_DATA *follower;
  
    last_fun( "add_follower_nomsg");
    if( !master )
    {
        bug( "Add_follower function called with no argument for master.\n\r", 0 );
        return;
    }

    if ( ch->master )
    {
	bug( "Add_follower_nomsg: non-null master.", 0 );
	return;
    }

    follower = alloc_mem( sizeof( PERSON_DATA ));

    ch->master        = master;
    ch->group_leader  = NULL;

    // Put the follower at the top of the list and set the
    // previous of the old one to point to it
    follower->prev    = NULL;
    follower->next    = master->followers;
    follower->who     = ch;
    if( master->followers )
      master->followers->prev = follower;
    master->followers = follower;

    return;
}

void stop_follower( CHAR_DATA *ch )
{
    PERSON_DATA *follower;

    last_fun( "stop_follower");
    if( !ch )
    {
       bug( "Stop_follower called with no CH argument.\n\r", 0 );
       return;
    }

    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
    {
	REMOVE_AFF_BIT( ch, AFF_CHARM );
	affect_strip( ch, 0, spl_charm_person, 0 );
	affect_strip( ch, 0, spl_domination, 0   );
    }

    if ( ch->master != ch && can_see( ch->master, ch ) && ch->master->in_room )
	act( "$n&n stops following you.", ch, NULL, ch->master, TO_VICT );
    if( ch->in_room )
      act( "You stop following $N&n.", ch, NULL, ch->master, TO_CHAR );

    // Remove the follower from the list of followers
    for( follower = ch->master->followers; follower; follower = follower->next )
    {
      if( follower->who == ch )
      {
        if( follower->next != NULL )
        {
          // In the middle of the list
          if( follower->prev != NULL )
          {
             follower->prev->next = follower->next;
             follower->next->prev = follower->prev;
          }
          // At the top of the list
          else
          {
             ch->master->followers = follower->next;
             if( follower->next->prev != NULL )
             {
                 follower->next->prev = NULL;
             }
          }
        }
        // At the end of the list
        else if( follower->prev )
        {
          follower->prev->next = NULL;
        }
        // Only one on the list
        else
        {
          ch->master->followers = NULL;
        }
        free_mem( follower, sizeof( PERSON_DATA) );
        // No need to continue loop after they're found.
        break;
      }
    }

    ch->master = NULL;

    return;
}

void die_follower( CHAR_DATA *ch, char *name )
{
    CHAR_DATA *fch;

    last_fun( "die_follower");
    if ( ch->master )
	stop_follower( ch );

    if ( ch->group_leader ) {
        remove_from_group( ch, ch );
    }

    for ( fch = char_list; fch; fch = fch->next )
    {
        if ( fch->deleted )
	    continue;
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->group_leader == ch )
	    fch->group_leader = NULL;
    }

    return;
}

void do_order( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char       arg [ MAX_INPUT_LENGTH ];
    bool       found;
    bool       fAll;

    last_fun( "do_order");
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_CHARM ) )
    {
        send_to_char( "You feel like taking, not giving, orders, slave!\n\r", ch );
	return;
    }

    if( !str_cmp( arg, "ship" ))
    {
        do_ordership( ch, argument );
        return;
    }

    if ( !str_cmp( arg, "all" ) || !str_prefix( arg, "followers" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( !is_affected( victim, AFF_CHARM ) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;

        if ( och->deleted )
	    continue;

	if ( is_affected( och, AFF_CHARM )
	    && och->master == ch
	    && ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "$n&n orders you to '$t'.", ch, argument, och, TO_VICT );
            if ( och->wait > 0 )
                act( "$N&n seems to be busy at the moment.", ch, NULL, och, TO_CHAR );
            else
                interpret( och, argument );
	}
    }

    if ( found )
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}

/*
 * 'Split' originally by Gnort, God of Chaos.
 *
 * This modified version for the 4-type coin system by Veygoth
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];
    char       *args;
    int        members = 0;
    int        share;
    int        extra;
    int        coinage;
    bool       success = FALSE;
    COIN_DATA  coin;

    last_fun( "do_split");
    args = argument;
    argument = one_argument( argument, arg );

    if ( !ch->group_leader ) {
        send_to_char("But you aren't in a group silly!\n\r", ch);
        return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }

    if ( !fill_coin_data(&coin, args, &(ch->money) ) ) {
        send_to_char( "Try 'split <number/all> <coin type>' or 'split all.coins' \n\r", ch );
        return;
    }
    if ( !coin.copper && !coin.silver && !coin.gold && !coin.platinum ) {
        send_to_char( "Try sharing some actual coins!\n\r", ch );
        return;
    }
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
        if ( gch->deleted )
	        continue;
	    if ( is_same_group( gch, ch ) )
	        members++;
    }

    if ( members < 2 ) {
 	    send_to_char( "Just keep it all.\n\r", ch );
        bug("do_split: managed to find a group of 1 person\n\r", 0);
	    return;
    }
    for (coinage = 0; coinage < 4; coinage++ ) {
        switch (coinage) {
            case 0: //copper
                if (coin.copper <= 0) continue; //quietly ignore errors
                if (coin.copper <= ch->money.copper) {
                    share = coin.copper / members;
                    extra = coin.copper % members;
                    if ( share == 0 ) continue;
                    ch->money.copper -= coin.copper;
                    sprintf( buf,
                    	"You split %d &+ycopper&n.  Your share is %d coins.\n\r",
                    	coin.copper, share + extra );
                    send_to_char( buf, ch );
                    sprintf( buf, "$n splits some &+ycopper&n.  Your share is %d coins.",
                         share );
                    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
                    	if ( gch->deleted )
                    	    continue;
                    	if ( gch != ch && is_same_group( gch, ch ) )
                    	{
                    	    act( buf, ch, NULL, gch, TO_VICT );
                    	    gch->money.copper += share;
                    	}
                        else if( gch == ch )
                           gch->money.copper += share + extra;
                    }
                    success = TRUE;
                    continue;
                }
                else
                    send_to_char("You don't have that much &+ycopper&n coin!\n\r", ch);
                break;
            case 1: //silver
                if (coin.silver <= 0) continue; //quietly ignore errors
                if (coin.silver <= ch->money.silver) {
                    share = coin.silver / members;
                    extra = coin.silver % members;
                    if ( share == 0 ) continue;
                    ch->money.silver -= coin.silver;
                    sprintf( buf,
                    	"You split %d &+wsilver&n.  Your share is %d coins.\n\r",
                    	coin.silver, share + extra );
                    send_to_char( buf, ch );
                    sprintf( buf, "$n splits some &+wsilver&n.  Your share is %d coins.",
                         share );
                    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
                    	if ( gch->deleted )
                    	    continue;
                    	if ( gch != ch && is_same_group( gch, ch ) )
                    	{
                    	    act( buf, ch, NULL, gch, TO_VICT );
                    	    gch->money.silver += share;
                    	}
                        else if( gch == ch )
                           gch->money.silver += share + extra;
                    }
                    success = TRUE;
                    continue;
                }
                else
                    send_to_char("You don't have that much &+wsilver&n coin!\n\r", ch);
                break;
            case 2: //gold
                if (coin.gold <= 0) continue; //quietly ignore errors
                if (coin.gold <= ch->money.gold) {
                    share = coin.gold / members;
                    extra = coin.gold % members;
                    if ( share == 0 ) continue;
                    ch->money.gold -= coin.gold;
                    sprintf( buf,
                    	"You split %d &+Ygold&n.  Your share is %d coins.\n\r",
                    	coin.gold, share + extra );
                    send_to_char( buf, ch );
                    sprintf( buf, "$n splits some &+Ygold&n.  Your share is %d coins.",
                         share );
                    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
                    	if ( gch->deleted )
                    	    continue;
                    	if ( gch != ch && is_same_group( gch, ch ) )
                    	{
                    	    act( buf, ch, NULL, gch, TO_VICT );
                    	    gch->money.gold += share;
                    	}
                        else if( gch == ch )
                           gch->money.gold += share + extra;
                    }
                    success = TRUE;
                    continue;
                }
                else
                    send_to_char("You don't have that much &+Ygold&n coin!\n\r", ch);
                break;
            case 3: //platinum
                if (coin.platinum <= 0) continue; //quietly ignore errors
                if (coin.platinum <= ch->money.platinum) {
                    share = coin.platinum / members;
                    extra = coin.platinum % members;
                    if ( share == 0 ) continue;
                    ch->money.platinum -= coin.platinum;
                    sprintf( buf,
                    	"You split %d &+Wplatinum&n.  Your share is %d coins.\n\r",
                    	coin.platinum, share + extra );
                    send_to_char( buf, ch );
                    sprintf( buf, "$n splits some &+Wplatinum&n.  Your share is %d coins.",
                         share );
                    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room ) {
                    	if ( gch->deleted )
                    	    continue;
                    	if ( gch != ch && is_same_group( gch, ch ) )
                    	{
                    	    act( buf, ch, NULL, gch, TO_VICT );
                    	    gch->money.platinum += share;
                    	}
                        else if( gch == ch )
                           gch->money.platinum += share + extra;
                    }
                    success = TRUE;
                    continue;
                }
                else
                    send_to_char("You don't have that much &+Wplatinum&n coin!\n\r", ch);
                break;
            } //end switch
        } //end for
    if (!success) {
        send_to_char("You didn't seem to split anything.\n\r", ch);
    }
    return;
}

void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "do_gtell");
    // No arguments, no leader, no chance. - Lithos
    if ( !ch->group_leader )
    {
        send_to_char( "Tell who, yourself? You are not in a group!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    argument = makedrunk( argument, ch );

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    
    for ( gch = char_list; gch; gch = gch->next )
    {
	if ( gch == ch)
	{
	    sprintf( buf, "&+GYou tell the group '&n&+G%s&+G'.&n\n\r", argument );
	    send_to_char( buf, ch );
	    continue;
	}
        if ( is_same_group( gch, ch )
            && !IS_ROOM( gch->in_room, ROOM_SILENT )
            && !IS_SET( race_table[gch->race].race_abilities, RACE_MUTE )
            && !is_affected( gch, AFF_MUTE )
            && IS_AWAKE( gch ))
        {
            sprintf( buf, "&+G%s&n&+G tells the group '&n&+G%s&+G'.&n\n\r",
                PERS( ch, gch ), translate( argument, ch, gch) );
            buf[3] = UPPER( buf[3] );
            send_to_char( buf, gch );
        }
    }

    return;
}


/* Sent in by Judson Knott <jek@conga.oit.unc.edu> */

//Added OR to prevent cross-racewar beeping  **Saeven
void do_beep( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_beep");
    if ( IS_NPC( ch ) )
        return;

    argument = one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Beep who?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_world( ch, arg ) )
	   || (race_table[victim->race].racewar_side != 
		   race_table[ch->race].racewar_side ) )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "They are not beepable.\n\r", ch );
	return;
    }

    if( ch != victim )
    {
        sprintf( buf, "You beep %s.\n\r", victim->name );
        send_to_char( buf, ch );
        sprintf( buf, "\a\a%s has beeped you.\n\r", ch->name );
        send_to_char( buf, victim );
    }
    else
    {
        sprintf( buf, "\aYou beep yourself.\a\n\r" );
        send_to_char( buf, ch );
    }

    return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->deleted || bch->deleted )
        return FALSE;

    if ( ach == bch )
	return TRUE;

    if ( ach->group_leader == NULL || bch->group_leader == NULL )
        return FALSE;

    if ( ach->group_leader ) ach = ach->group_leader;
    if ( bch->group_leader ) bch = bch->group_leader;
    return ach == bch;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_clan( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->deleted || bch->deleted )
        return FALSE;

    if ( is_clan( ach ) && is_clan( bch ) )
	return ach->pcdata->clan == bch->pcdata->clan;
    else
	return FALSE;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void do_color( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_STRING_LENGTH ];

    last_fun( "do_color");
    argument = one_argument( argument, arg );

    if ( !*arg )
    {
	if( !IS_SET( ch->act, PLR_COLOUR ) )
	{
	    SET_BIT( ch->act, PLR_COLOUR );
            send_to_char( "&+LThe world becomes more &n&+mco&+Ml&+Wor&+Cf&n&+cul&+L.&n\n\r", ch );
	}
	else
	{
            send_to_char_bw( "The color drains.\n\r", ch );
	    REMOVE_BIT( ch->act, PLR_COLOUR );
	}
	return;
    }
    else
    {
        send_to_char_bw( "Color Configuration is unavailable in this\n\r", ch );
        send_to_char_bw( "version of color, sorry.\n\r", ch );
    }

    return;
}


/*
 * "retir" command is a trap to the "retire" command. (Zen)
 */
void do_retir( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_retir");
    send_to_char( "If you want to &+LRETIRE&n, you have to spell it out.\n\r", ch );
 
    return;
}
 
 
/* 
 * Made by Zen :)
 * Some inspiration from bits here and there in the Envy code...
 */
void do_retire( CHAR_DATA *ch, char *argument )
{
    extern const char echo_off_str [ ];

    last_fun( "do_retire");
    if ( IS_NPC( ch ) )
      return;

    if ( ch->position == POS_FIGHTING || ch->fighting )
    {
      send_to_char( "No way! You are fighting.\n\r", ch );
      return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
      send_to_char( "You're not &+RD&n&+rE&+RA&n&+rD&n yet.\n\r", ch );
      return;
    }

    /* If player does not have level 1 he should not have a file... */
    if ( ch->level < 1 )
    {
      send_to_char( "You haven't logged in yet and you want to retire? I think not.\n\r", ch );
      return;
    }

    write_to_buffer( ch->desc, "Password: ", 0 );
    write_to_buffer( ch->desc, echo_off_str, 0 );
    ch->desc->connected = CON_RETIRE_GET_PASSWORD;

    return;
}


void send_ansi_title( DESCRIPTOR_DATA *d )
{
    FILE *titlefile;
    char  buf     [ MAX_STRING_LENGTH*2 ];
    char  strsave [ MAX_INPUT_LENGTH    ];
    int   num;

    last_fun( "send_ansi_title");
    num = 0;

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, ANSI_TITLE_FILE );

    if ( ( titlefile = fopen( strsave, "r" ) ) )
    {
	while ( ( buf[num] = fgetc( titlefile ) ) != EOF )
	    num++;
	fclose( titlefile );
	buf[num] = '\0';
	write_to_buffer( d, buf, num );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void send_ascii_title( DESCRIPTOR_DATA *d )
{
    FILE *titlefile;
    char  buf     [ MAX_STRING_LENGTH*2 ];
    char  strsave [ MAX_INPUT_LENGTH    ];
    int   num;

    last_fun( "send_ascii_title");
    num = 0;

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, ANSI_TITLE_FILE );

    if ( ( titlefile = fopen( strsave, "r" ) ) )
    {
	while ( ( buf[num] = fgetc( titlefile ) ) != EOF )
	    num++;
	fclose( titlefile );
	buf[num] = '\0';
	write_to_buffer( d, buf, num );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

// Mainly used for quest files, but can be used in conversation
// - Veygoth
void do_ask( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    QUEST_INDEX_DATA *quest;
    TALK_DATA *message;

    last_fun( "do_ask");
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ask who?\n\r", ch );
	return;
    }

    if( !(victim = get_char_room( ch, arg )))
    {
        send_to_char( "They're not here.\n\r", ch );
        return;
    }

    if( argument[0] == '\0' )
    {
        send_to_char( "Ask them what?\n\r", ch );
        return;
    }

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but nothing escapes their bounds.\n\r", ch );
	return;
    }

    argument = makedrunk( argument, ch );

    act( "$n&n asks $N&n a question.", ch, NULL, victim, TO_NOTVICT );
    sprintf( buf, "&n&+c$n&n&+c asks you '&+c%s&n&+c'&n", argument );
    act( buf, ch, NULL, victim, TO_VICT );
    sprintf( buf, "&n&+cYou ask $N&n&+c '%s&n&+c'&n", argument );
    act( buf, ch, NULL, victim, TO_CHAR );

    // players can't have talk files -- go home!
    if( !IS_NPC( victim ))
      return;

    for( quest = quest_first; quest; quest = quest->next )
    {
         if( !quest->messages || (quest->vnum != victim->pIndexData->vnum ))
            continue;
         for( message = quest->messages; message; message = message->next )
         {
           if( is_name( argument, message->keywords ))
            {
              send_to_char( "\n\r", ch );
              send_to_char( message->message, ch );
            }
         }
    }
    return;
}

// For players to talk to the gods - Veygoth
void do_petition( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];

    last_fun( "do_petition");
    if( argument[0] == '\0' )
    {
    	send_to_char( "Petition what?\n\r", ch );
        return;
    }
    sprintf( buf, "&+r%s petitions '%s'&n", ch->name, argument );
    wiznet( ch, WIZ_PETITION, L_AVA, buf );
    sprintf( buf, "&+rYou petition '%s'&n\n\r", argument );
    send_to_char( buf, ch );
    return;
}

// For gods to respond to the players - Veygoth
void do_ptell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        position;
    char       buf [MAX_STRING_LENGTH];

    last_fun( "do_ptell");
    if( !authorized( ch, "ptell" ) )
    {
       send_to_char( "&+LYou cannot respond to &+Bpetitions&+L.&n\n\r", ch );
       return;
    }

    one_argument( argument, arg );

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
        act( "$N&n is &+Llinkdead&n.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Ptell whom what?\n\r", ch );
	return;
    }

    argument = makedrunk( argument, ch );

    act( "&n&+rYou tell $N&n&+r '&+R$t&n&+r'&n", ch, argument, victim, TO_CHAR );
    position            = victim->position;
    victim->position    = POS_STANDING;
    act( "&n&+r$n&n&+r responds to your petition with '&+R$t&n&+r'&n", ch, argument, victim, TO_VICT );
    sprintf( buf, "&+r%s responds to %s's petition with '&+R%s&n&+r'&n", ch->name, victim->name, argument );
    wiznet( ch, WIZ_PETITION, L_AVA, buf );
    victim->position    = position;
    victim->reply       = ch;

    if ( IS_SET( victim->act, PLR_AFK ) )
	act( "Just so you know, $E is &+RAFK&n.", ch, NULL, victim, TO_CHAR );

    return;
}

void do_consent( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   CHAR_DATA *wch;
   char arg[ MAX_INPUT_LENGTH ];

    last_fun( "do_consent");
   if( IS_NPC( ch ) || ch->pcdata == NULL )
   {
      send_to_char( "No consenting for mobs!", ch);
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
       send_to_char( "Consent who?\n\r", ch );
       return;
   }

   if( !str_cmp( arg, "who" ))
   {
     if( ch->pcdata->consent == NULL )
        send_to_char( "You are not consenting anyone.\n\r", ch );
     else
        act( "You are consenting $N&n.", ch, NULL, ch->pcdata->consent, TO_CHAR );
     for( wch = char_list; wch; wch = wch->next )
     {
         if( IS_NPC( wch ) )
              continue;
         if( wch->pcdata->consent && wch->pcdata->consent == ch )
              act( "$N&n is consenting you.", ch, NULL, wch, TO_CHAR );
     }
     return;
   }

   if( !(victim = get_char_world( ch, arg )) ||
        (race_table[victim->race].racewar_side !=
        race_table[ch->race].racewar_side ))
   {
         send_to_char( "That person doesn't exist.\n\r", ch );
	 return;
   }

   if( victim == ch )
   {
         send_to_char( "You no longer give consent to anyone.\n\r", ch );
         if( ch->pcdata->consent )
         {
           act( "$N&n stops consenting you.", ch->pcdata->consent, NULL, ch, TO_CHAR );
           act( "You stop consenting $N&n.", ch, NULL, ch->pcdata->consent, TO_CHAR );
         }
   	 ch->pcdata->consent = NULL;
         return;
   }

   if( ch->pcdata->consent )
   {
     act( "$N&n stops consenting you.", ch->pcdata->consent, NULL, ch, TO_CHAR );
     act( "You stop consenting $N&n.", ch, NULL, ch->pcdata->consent, TO_CHAR );
   }

   ch->pcdata->consent = victim;
   act( "You now give consent to $N&n.", ch, NULL, victim, TO_CHAR );
   act( "$n&n has given you $s consent.", ch, NULL, victim, TO_VICT );
   return;
}

void do_ignore( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   CHAR_DATA *wch;
   char arg[ MAX_INPUT_LENGTH ];

   last_fun( "do_ignore");

   if( IS_NPC( ch ) || ch->pcdata == NULL )
   {
      send_to_char( "No ignoring for mobs!", ch);
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
       send_to_char( "Ignore who?\n\r", ch );
       return;
   }

   if( !str_cmp( arg, "who" ))
   {
     if( ch->pcdata->ignore == NULL )
        send_to_char( "You are not ignoring anyone.\n\r", ch );
     else
        act( "You are ignoring $N&n.", ch, NULL, ch->pcdata->ignore, TO_CHAR );
     for( wch = char_list; wch; wch = wch->next )
     {
         if( IS_NPC( wch ) )
              continue;
         if( wch->pcdata->ignore && wch->pcdata->ignore == ch )
              act( "$N&n is ignoring you.", ch, NULL, wch, TO_CHAR );
     }
     return;
   }

   if( !(victim = get_char_world( ch, arg )) ||
        (race_table[victim->race].racewar_side !=
        race_table[ch->race].racewar_side ))
   {
         send_to_char( "That person doesn't exist.\n\r", ch );
	 return;
   }

   if( victim == ch )
   {
         send_to_char( "You no longer give ignore anyone.\n\r", ch );
         if( ch->pcdata->ignore )
         {
           act( "$N&n stops ignoring you.", ch->pcdata->ignore, NULL, ch, TO_CHAR );
           act( "You stop ignoring $N&n.", ch, NULL, ch->pcdata->ignore, TO_CHAR );
         }
   	 ch->pcdata->ignore = NULL;
         return;
   }

   if( ch->pcdata->ignore )
   {
     act( "$N&n stops ignoring you.", ch->pcdata->ignore, NULL, ch, TO_CHAR );
     act( "You stop ignoring $N&n.", ch, NULL, ch->pcdata->ignore, TO_CHAR );
   }

   ch->pcdata->ignore = victim;
   act( "You now ignore $N&n.", ch, NULL, victim, TO_CHAR );
   act( "$n&n is now ignoring you.", ch, NULL, victim, TO_VICT );
   return;
}

void do_guard( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   CHAR_DATA *wch;
   char arg[ MAX_INPUT_LENGTH ];

   last_fun( "do_guard");

   if( IS_NPC( ch ) || !ch->pcdata )
     return;

   if( ch->level < skills_table[gsn_guard].skill_level[ch->class] )
   {
     send_to_char( "Guard!? You can't even protect yourself!\n\r", ch );
     return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
     if (!ch->pcdata->guarding )
     {
       send_to_char( "Guard who?\n\r", ch );
       return;
     }
     else
     if (!ch->pcdata->guarding )
     {
       char buf[MAX_STRING_LENGTH];
       sprintf( buf, "You are guarding %s.\n\r", IS_NPC(ch->pcdata->guarding ) ?
           ch->pcdata->guarding->short_descr : ch->pcdata->guarding->name );
       send_to_char( buf, ch );
       return;
     }
   }

   if( !str_cmp( arg, "who" ))
   {
     if( !ch->pcdata->guarding )
        send_to_char( "You are not guarding anyone.\n\r", ch );
     else
        act( "You are guarding $N&n.", ch, NULL, ch->pcdata->guarding, TO_CHAR );
     for( wch = char_list; wch; wch = wch->next )
     {
         if( IS_NPC( wch ))
              continue;
         if( wch->pcdata->guarding && wch->pcdata->guarding == ch )
              act( "$N&n is guarding you.", ch, NULL, wch, TO_CHAR );
     }
     return;
   }

   if( !(victim = get_char_room( ch, arg )))
   {
         send_to_char( "You don't see them here.\n\r", ch );
	 return;
   }

   if( victim == ch )
   {
         send_to_char( "You no longer guard anyone.\n\r", ch );
	 ch->pcdata->guarding = NULL;
         return;
   }

   if( ch->class == CLASS_PALADIN && IS_EVIL( victim ))
   {
         send_to_char( "Their blackened soul is hardly worth the effort.\n\r", ch );
         return;
   }

   if( ch->pcdata->guarding )
   {
     act( "$N&n stops guarding you.", ch->pcdata->guarding, NULL, ch, TO_CHAR );
     act( "You stop guarding $N&n.", ch, NULL, ch->pcdata->guarding, TO_CHAR );
   }

   ch->pcdata->guarding = victim;
   act( "You now guard $N&n.", ch, NULL, victim, TO_CHAR );
   act( "$n&n is now guarding you.", ch, NULL, victim, TO_VICT );
   return;
}

// True if ch is grouped, false otherwise.
bool check_group( CHAR_DATA *ch )
{
    CHAR_DATA *gch;

    // No char/leader => not in group
    if ( !ch || ch->group_leader == NULL )
	return FALSE;

    // Must have 1 other person in group before we can return true.
    for (gch = char_list; gch; gch = gch->next)
    {
	if (is_same_group(gch, ch) && gch != ch)
            return TRUE;
    }

    // Only person in group. :(
    ch->group_leader = NULL;
    return FALSE;
}

void door_trigger( char *arg, CHAR_DATA *ch )
{
    int door;
    int i;

    last_fun( "door_trigger");
    for( door = 0;door < MAX_DIR; door++ )
    {
        if( ch->in_room->exit[door] && IS_SET(ch->in_room->exit[door]->exit_info, EX_CLOSED)
            && !IS_SET( ch->in_room->exit[door]->exit_info, EX_SECRET ) 
            && ch->in_room->exit[door]->key < 0 )
        {
            char       buf     [ MAX_STRING_LENGTH   ];
            char      *buf1;

            /* get last argument from door keywords */
//            buf1 = one_argument( ch->in_room->exit[door]->keyword, buf);
            i = 0;
            strcpy(buf, ch->in_room->exit[door]->keyword );
            buf1 = buf;
            while (buf1[0] != '\0' && i < 9 ) {
                buf1 = one_argument( buf1, buf );
                i++;
            }
          sprintf( lbuf, "Comparing %s against word said: %s", buf, arg );
          wiznet( 0, WIZ_LSPAM, 0, lbuf );
            if( !str_cmp( buf, arg ) )
            {
                char buf2 [MAX_STRING_LENGTH];
                EXIT_DATA *exit = ch->in_room->exit[door]->to_room->exit[rev_dir[door]];
                buf1 = one_argument( ch->in_room->exit[door]->keyword, buf);
                sprintf( buf2, "The %s hums briefly and opens.", buf );
                REMOVE_BIT( ch->in_room->exit[door]->exit_info, EX_CLOSED );
                REMOVE_BIT( exit->exit_info, EX_CLOSED );
                act(buf2, ch, NULL, NULL, TO_ROOM );
                act(buf2, ch, NULL, NULL, TO_CHAR );
            }
        }
    }

    return;
}

// Sign language for Drow only.
void do_sign( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char  buf [ MAX_STRING_LENGTH ];

    last_fun( "do_sign");
    if ( argument[0] == '\0' )
    {
 	send_to_char_bw( "Sign what?\n\r", ch );
	return;
    }

    if ( ch->race != RACE_DROW && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "You wiggle your fingers around.\n\r", ch );
        act( "$n wiggles $s fingers around, looking foolish.", ch, NULL, NULL, TO_ROOM );
        return;
    }

    // We don't want to let them know they're drunk.
    act( "You sign '&n$T&n'", ch, NULL, argument, TO_CHAR );

    argument = makedrunk( argument, ch );

    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;
        if ( och == ch || IS_NPC( och ) )
            continue;

        if ( IS_IMMORTAL( och ) || och->race == RACE_DROW )
            sprintf( buf, "%s signs '&n$T&n'", PERS( ch, och ) );
        else
            sprintf( buf, "%s wiggles %s fingers around.", PERS( ch, och ),
                ch->sex == SEX_MALE   ? "his" :
                ch->sex == SEX_FEMALE ? "her" : "its" );

        act( buf, och, NULL, argument, TO_CHAR );
    }

    MOBtrigger = FALSE;

//    mprog_speech_trigger( argument, ch );
//    door_trigger( argument, ch );

    return;

}

// Cant language for theives only.
void do_cant( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char       buf [ MAX_STRING_LENGTH ];
    char      *random;

    last_fun( "do_cant");
    if ( argument[0] == '\0' )
    {
 	send_to_char_bw( "Cant what?\n\r", ch );
	return;
    }

    if ( ch->class != CLASS_THIEF && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "You speak gibberish.\n\r", ch );
        sprintf( buf, "$n says, '%s'\n\r", random_sentence( ) );
        act( buf, ch, NULL, NULL, TO_ROOM );
        return;
    }

    // We don't want to let them know they're drunk.
    act( "You cant '&n$T&n'", ch, NULL, argument, TO_CHAR );

    argument = makedrunk( argument, ch );
    random = random_sentence();

    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;
        if ( och == ch || IS_NPC( och ) )
            continue;

        if ( IS_IMMORTAL( och ) || och->class == CLASS_THIEF )
            sprintf( buf, "%s cants '&n$T&n'", PERS( ch, och ) );
        else
            sprintf( buf, "%s says, '%s'\n\r", PERS( ch, och ), 
                random );

        act( buf, och, NULL, translate( argument, ch, och ), TO_CHAR );
    }

    return;

}

void remove_from_group( CHAR_DATA *ch, CHAR_DATA *victim )
{

    if( victim == NULL || ch == NULL )
    {
        bug( "remove_from_group: NULL ch or victim", 0 );
        return;
    }
/* TEMP.LOHRR 
    send_to_char( "ch: ", ch );
    send_to_char( ch->name, ch );
    send_to_char( ". victim: ", ch );
    send_to_char( victim->name, ch );
    send_to_char( ".\n\r", ch );
*/
    if( ch->group_leader == NULL )
    {
        bug( "remove_from_group: ch not grouped.", 0 );
/*
        while( ch->next_in_group )
            remove_from_group( ch->next_in_group, ch->next_in_group );
*/
        ch->next_in_group = NULL;
        return;
    }
    if( victim->group_leader == NULL )
    {
        bug( "remove_from_group: victim not grouped.", 0 );
        victim->next_in_group = NULL;
        return;
    }

    if( ch == victim )
    {
        send_to_char( "You leave the group.\n\r", victim );
        if( victim != victim->group_leader )
            act( "$N&n leaves your group.", victim->group_leader, NULL, victim, TO_CHAR );
    }
    else
    {
        act( "You kick $N&n out of your group.", ch, NULL, victim, TO_CHAR );
        act( "$n&n removes you from $s group.",  ch, NULL, victim, TO_VICT );
    }

    if( victim->group_leader != victim )
    {   /* Remove group member from group. */
        CHAR_DATA *prev_gmember = victim->group_leader;

        /* Find previous group member. */
        while( prev_gmember != NULL && prev_gmember->next_in_group != victim )
            prev_gmember = prev_gmember->next_in_group;
        if( prev_gmember == NULL )
        {
            bug( "remove_from_group: ch has leader but not on group list.", 0 );
            victim->group_leader = NULL;
            victim->next_in_group = NULL;
            return;
        }
        /* remove ch from group */
        prev_gmember->next_in_group = victim->next_in_group;
        /* remove group from ch */
        victim->group_leader  = NULL;
        victim->next_in_group = NULL;
        /* Member of a group of 2. */
        if( prev_gmember == prev_gmember->group_leader &&
            prev_gmember->next_in_group == NULL )
        {
            send_to_char( "Your group has been disbanded.\n\r", prev_gmember );
            prev_gmember->group_leader = NULL;
            return;
        }
    }
    else
    {   /* Remove leader from group. */
        CHAR_DATA *new_leader = victim->next_in_group;
        CHAR_DATA *gmembers;

        victim->group_leader  = NULL;
        victim->next_in_group = NULL;

        /* Leader of a group of 1 ?! */
        if( new_leader == NULL )
        {
            bug( "remove_from_group: group leader has no group!", 0 );
            return;
        }

        /* Leader of a group of 2. */
        if( ( gmembers = new_leader->next_in_group ) == NULL )
        {
            send_to_char( "Your group has been disbanded.\n\r", new_leader );
            new_leader->group_leader = NULL;
            return;
        }
        new_leader->group_leader = new_leader;
        while( gmembers != NULL )
        {
            gmembers->group_leader = new_leader;
            gmembers = gmembers->next_in_group;
        }
    }
}

void add_to_group( CHAR_DATA *gmember, CHAR_DATA *ch )
{

    if( gmember == NULL || ch == NULL )
    {
        bug( "add_to_group: NULL gmember or NULL ch (%d)!", gmember == NULL );
        return;
    }
/* TEMP.LOHRR 
    send_to_char( "ch: ", gmember );
    send_to_char( ch->name, gmember );
    send_to_char( ". gmember: ", gmember );
    send_to_char( gmember->name, gmember );
    send_to_char( ".\n\r", gmember );
*/
    /* Only the leader can add to group. */
    if( gmember != gmember->group_leader )
    {
        if( gmember->group_leader != NULL )
        {
            bug( "add_to_group: gmember not group leader!", gmember == NULL );
            return;
        }
        else
            gmember->group_leader = gmember;
    }

    act( "$N&n joins your group.", gmember, NULL, ch, TO_CHAR );
    act( "You join $n&n's group.", gmember, NULL, ch, TO_VICT );

    /* Move to last char in group. */
    while( gmember->next_in_group != NULL )
    {
        gmember->next_in_group->group_leader = gmember->group_leader;
        gmember = gmember->next_in_group;
    }
    /* Add char to group. */
    gmember->next_in_group = ch;
    ch->next_in_group = NULL;
    /* Add group to char. */
    ch->group_leader = gmember->group_leader;
}

void show_group( CHAR_DATA *ch )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];

    // No arguments, no leader, no chance.
    if( !check_group( ch ) )
    {
        send_to_char( "But you're not in a group!\n\r", ch );
        return;
    }

    sprintf( buf, "&+R%s&+L's group:&n\n\r", PERS( ch->group_leader, ch ) );
    send_to_char( buf, ch );
    for ( gch = ch->group_leader; gch; gch = gch->next_in_group )
    {
        if ( gch->deleted )
            continue;
        // Won't show stats of a groupmember not in the room
        if( ch->in_room == gch->in_room )
        {
            sprintf( buf,
                "&+L[&n%2d %s&+L]&n %-16s&n %4d&+L/&n%4d&+L hp&n %4d&+L/&n%4d &+Lmana&n %4d&+L/&n%4d&+L mv&n\n\r",
		gch->level,
                IS_NPC( gch ) ? "Mob         "
		              : (char *)class_table[gch->class]->who_name,
		capitalize( PERS( gch, ch ) ),
		gch->hit,   get_max_hit( gch ),
		gch->mana,  gch->max_mana,
		gch->move,  gch->max_move );
            send_to_char( buf, ch );
        }
        else
        {
            sprintf( buf,
                "&+L[&n%2d %s&+L]&n %-16s&n\n\r",
		gch->level,
                IS_NPC( gch ) ? "Mob         "
		              : (char *)class_table[gch->class]->who_name,
		capitalize( PERS( gch, ch ) ));
            send_to_char( buf, ch );
        }
    }
}



void do_group( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    char       	 buf	[ MAX_STRING_LENGTH ];
    char       	 arg 	[ MAX_INPUT_LENGTH  ];

    last_fun( "do_group");

    one_argument( argument, arg );

    // Group with no arguments should show group staus.
    if( arg[0] == '\0' )
    {
        show_group( ch );
        return;
    }

    if ( !str_prefix( arg, "all" ) ) {
        for( victim = ch->in_room->people; victim; victim = victim->next_in_room) {
            if (victim->deleted || victim == ch
               || victim->fly_level != ch->fly_level
               || race_table[victim->race].racewar_side != race_table[ch->race].racewar_side )
                continue;
            if ( (( IS_NPC(victim) && IS_SET( victim->act, ACT_PET) )
                  || ( !IS_NPC( victim) && HAS_CONSENT( ch, victim )))
                && victim->master == ch && !victim->group_leader )
                add_to_group( ch, victim );
        }
        return;
    }

    if( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ch == victim ) {
        remove_from_group( ch, victim );
         return;
    }
    if( race_table[victim->race].racewar_side !=
        race_table[ch->race].racewar_side && !IS_NPC( victim ) )
    {
        send_to_char( "You can't group with such slime!\n\r", ch );
        return;
    }
    if( is_same_group( ch, victim) )
    {
        if( ch->group_leader == ch ) {
            remove_from_group( ch, victim );
        }
        else {
            send_to_char( "Only the leader of a group may kick someone out.\n\r", ch );
        }
    return;
    }
    if( ch->group_leader == NULL || ch->group_leader == ch )
    {
        if( victim->group_leader == NULL )
        {
            if( ( !IS_NPC( victim ) && !HAS_CONSENT( ch, victim ) )
              || ( IS_NPC( victim ) && victim->master != ch ) )
            {
                sprintf( buf, "%s doesn't want to be in your group.\n\r", PERS( victim, ch ) );
                send_to_char( buf, ch );
            }
            else
            {
                ch->group_leader = ch;
                add_to_group( ch, victim );
            }
        }
        else
        {
            sprintf( buf, "%s is in another group.\n\r", PERS( victim, ch ) );
            send_to_char( buf, ch );
        }
    }
    else {
        send_to_char( "You must be the head of the group to add someone.\n\r", ch);
    }
    return;
}

void do_disband( CHAR_DATA *ch, char *argument ) {
    CHAR_DATA *gch, *gch_next;

    last_fun( "do_disband" );
    if ( ch != ch->group_leader) {
        send_to_char("You must be group leader to disband!\n\r", ch );
        return;
    }
    for (gch = ch; gch; gch = gch_next ) {
        gch_next = gch->next_in_group;
        send_to_char("&+GYour group has been disbanded.&n\n\r", gch );
        gch->group_leader = NULL;
        gch->next_in_group = NULL;
    }
    return;
}

void do_ungroup( CHAR_DATA *ch, char *argument ) {
    CHAR_DATA   *victim;

    last_fun( "do_ungroup" );
    victim = get_char_world( ch, argument );
    if (victim == NULL) {
         send_to_char("They do not seem to exist!\n\r", ch);
        return;
    }
    if ( ch != victim->group_leader && !IS_IMMORTAL( ch ) ) {
        act("You are not $S group leader!", ch, NULL, victim, TO_CHAR);
        return;
    }
    if ( !victim->group_leader ) {
        act("$N is not in a group!", ch, NULL, victim, TO_CHAR);
        victim->next_in_group = NULL;
        return;
    }
    remove_from_group( victim->group_leader, victim );
    return;
}

void do_write( CHAR_DATA *ch, char *argument ) {
}

void do_read( CHAR_DATA *ch, char *argument ) {

}

void do_delete( CHAR_DATA *ch, char *argument ) {

}


void look_at_board( CHAR_DATA *ch, OBJ_DATA *obj ) {
    NOTE_DATA *pnote, *list;
    char       buf     [ MAX_STRING_LENGTH   ];
    char       buf1    [ MAX_STRING_LENGTH*7 ];
    int        vnum;

    last_fun( "look_at_board");
    if ( IS_NPC( ch ) )
        return;
    list = (NOTE_DATA *)obj->value[3];
    if ( !list ) {
        send_to_char( "There are no messages posted.\n\r", ch);
        return;
    }
    vnum    = 1;
    buf1[0] = '\0';
    for ( pnote = note_list; pnote; pnote = pnote->next )
    {
        sprintf( buf, "  [%3d] %s: %s&n\n\r",
                vnum,
                pnote->sender, pnote->subject );
        strcat( buf1, buf );
        vnum++;
    }
    send_to_char( buf1, ch );
}

void add_to_board( CHAR_DATA *ch, NOTE_DATA *note, NOTE_DATA *list ) {
}

void delete_from_board( CHAR_DATA *ch, NOTE_DATA *note, NOTE_DATA *list ) {
}

