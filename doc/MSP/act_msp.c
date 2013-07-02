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
#include <time.h>
#include "merc.h"

/* merc.h */
#define MSP_DEFAULT		    -99
#define PLR_SOUND                  BV29
#define PLR_MUSIC                  BV30

void sound_to_char( const char *fname, int vol, int repeats, int priority,
			const char *type, CHAR_DATA *ch )
{
    char buf [MAX_STRING_LENGTH];

    if ( !IS_SET( ch->act, PLR_SOUND ) )
	return;

    if ( vol == MSP_DEFAULT )
	vol = 100;
    if ( repeats == MSP_DEFAULT )
	repeats = 1;
    if ( priority == MSP_DEFAULT )
	priority = 50;

    sprintf( buf, "!!SOUND(%s/%s V=%d L=%d P=%d T=%s)\n\r",
	    type, fname, vol, repeats, priority, type );
    send_to_char( buf, ch );

    return;
}


void music_to_char( const char *fname, int vol, int repeats, bool continue,
			const char *type, CHAR_DATA *ch )
{
    char buf [MAX_STRING_LENGTH];

    if ( !IS_SET( ch->act, PLR_MUSIC ) )
	return;

    if ( vol == MSP_DEFAULT )
	vol = 100;
    if ( repeats == MSP_DEFAULT )
	repeats = 1;
    if ( continue == MSP_DEFAULT )
	continue = 1;

    sprintf( buf, "!!MUSIC(%s/%s V=%d L=%d C=%d T=%s)\n\r",
	    type, fname, vol, repeats, continue, type );
    send_to_char( buf, ch );

    return;
}


void reset_sound( CHAR_DATA *ch )
{
    if ( IS_SET( ch->act, PLR_SOUND ) )
	send_to_char( "!!SOUND(Off)\n\r",ch );

    return;
}


void reset_music( CHAR_DATA *ch )
{
    if ( IS_SET( ch->act, PLR_MUSIC ) )
	send_to_char( "!!MUSIC(Off)\n\r",ch );

    return;
}
