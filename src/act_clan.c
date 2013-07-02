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

/*
    This clans code was made for the HomeCooked release, it was completely
    built from the ground up, though got ideas from here and there...
    Painful! -- Zen --

    This file was written by Zen <vasc@camoes.rnl.ist.utl.pt>
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
#include "olc.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                  field = fread_string( fp, &stat); \
                    fMatch = TRUE;                  \
                    break;                          \
                }



CLAN_DATA *             clan_first = NULL;
CLAN_DATA *             clan_last  = NULL;


bool is_clan( CHAR_DATA *ch )
{

    if ( !IS_NPC( ch )
	&& ( ch->pcdata->clan != NULL )
	&& ( ch->pcdata->rank > RANK_EXILED) )
	return TRUE;

    return FALSE;
}


void remove_from_clan( CHAR_DATA *ch )
{
    CLAN_DATA *clan;
    int count;

    last_fun( "remove_from_clan");

    if ( !is_clan( ch ) )
	return;

    clan = ch->pcdata->clan;

    for( count = 0; count < MAX_CLAN; count++ )
    {
      if( !str_cmp( clan->member[count]->name, ch->name ))
        clan->member[count]->filled = FALSE;
    }

    switch ( ch->pcdata->rank )
    {
      default:								  break;
      case RANK_LEADER:  free_string( clan->overlord );			  break;
    }

    clan->members--;
    ch->pcdata->clan = NULL;
    free_string( ch->pcdata->title );
    set_title( ch, "&n" );

    return;    
}

void do_initiate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    OBJ_DATA  *ring;
    bool found = FALSE;
    int        count;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "do_initiate");

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Initiate whom?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if( IS_NPC( victim ))
    {
        send_to_char( "Very funny trying to enroll an NPC.\n\r", ch );
        return;
    }

    if ( !is_clan( ch ) || ch->pcdata->rank < RANK_OFFICER )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( victim == ch )
        return;

    if( clan->applicant != victim )
    {
       send_to_char( "They have not applied for membership to your guild.\n\r", ch );
       return;
    }

    if( is_clan( victim ))
    {
       send_to_char( "They are already in another guild.\n\r", ch );
       return;
    }

    if( clan->members >= 30 )
    {
       send_to_char( "Your guild is full.\n\r", ch );
       return;
    }

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
       send_to_char( "There are no more openings in your organization.\n\r", ch );
       return;
    }

/*
    if( victim->pcdata->rank == RANK_EXILED )
    {
       send_to_char( "They are an exile and may not be guilded.\n\r", ch );
       send_to_char( "You are an exile and cannot be accepted into a guild.\n\r", victim );
       return;
    }
*/

    if( victim->level < 25 )
    {
        if( clan->clan_type == CLAN_CLAN && ch->level >= 15 )
        {
        }
        else if( clan->clan_type == CLAN_GUILD && ch->level >=20 )
        {
        }
        else
        {
          act( "$N&n is too low level to be initiated.",
	     ch, NULL, victim, TO_CHAR );
          act( "You are too weak to  be initiated to $t!",
	     ch, clan->who_name, victim, TO_VICT ); 
          return;
        }
    }

    if ( clan->clan_type == CLAN_GUILD && clan->class != victim->class )
	send_to_char( "You may only initiate those of your class into a guild.\n\r", ch );

    victim->pcdata->clan      = clan;
    victim->pcdata->rank      = RANK_NORMAL;
    clan->members++;

    set_title( victim, clan->rank_name[victim->pcdata->rank] );

    clan->member[count]->name = str_dup( victim->name );
    clan->member[count]->rank = victim->pcdata->rank;
    clan->member[count]->join_time = (int)current_time;
    clan->member[count]->filled = TRUE;

    // reset so others can apply
    clan->applicant = NULL;

    if( get_obj_index( clan->clanobj1 ))
    {
      ring = create_object( get_obj_index( clan->clanobj1 ), victim->level );
      
      if ( ring )
    	obj_to_char( ring, victim );
    }

    sprintf( buf, "Log %s: initiated %s to %s",
	    ch->name,
	    victim->name,
	    clan->name  );
    log_clan( buf );

    sprintf( buf, "'I %s %s, hereby declare you %s a member of %s!'\n\r"
		  "Forever remember our motto: \"%s\"\n\r",
		  flag_string( rank_flags, ch->pcdata->rank ),
		  ch->name,
		  victim->name,
		  clan->who_name,
		  clan->motto );

    send_to_char( buf, victim );

    act( "$N&n has been initiated to $t!", ch, clan->who_name, victim, TO_ROOM );
    act( "You have initiated $N&n to $t!", ch, clan->who_name, victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}


/*
 * "exil" command is a trap to the "exile" command.
 */
void do_exil( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_exil");

    send_to_char( "If you want to EXILE, you have to spell it out.\n\r", ch );

    return;
}


void do_exile( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "do_exile");

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Exile whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank != RANK_LEADER )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) || victim == ch || victim->level > ch->level )
        return;

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N isn't even from $t.", ch, clan->who_name, victim, TO_CHAR );
        return;
    }

    sprintf( buf, "Log %s: exiling %s from %s",
	    ch->name,
	    victim->name,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    // this function handles resetting member data
    remove_from_clan( victim );
    victim->pcdata->rank = RANK_EXILED;

    sprintf( buf,
	    "The grand Overlord of %s %s says:\n\r\n\r"
	    "'Then so be done, you %s shall be exiled from %s!'\n\r"
	    "You hear a thundering sound...\n\r\n\r"
	    "A booming voice says: 'You have been exiled. Only the gods can allow you\n\r"
	    "to join another clan, order or guild!'\n\r",
	    clan->who_name,
	    clan->overlord,
	    victim->name,
	    clan->who_name );

    send_to_char( buf, victim );

    act( "You have exiled $N&n from $t!", ch, clan->who_name, victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}

void do_kickout( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *victDD;
    CHAR_DATA       *wch;
    CLAN_DATA       *clan;
    char             arg [ MAX_INPUT_LENGTH ];
    char             buf [ MAX_STRING_LENGTH ];
    bool             logged_in;

    last_fun( "do_kickout");

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Kick out whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch ) || ch->pcdata->rank < RANK_OFFICER )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    victim = NULL;
    for ( wch = char_list; wch ; wch = wch->next )
    {
        if( wch->deleted || IS_NPC( wch ) )
            continue;
        if( is_name( arg, wch->name ) )
        {
            victim = wch;
            logged_in = TRUE;
            break;
        }
    }

    if ( victim == NULL )
    {
        if ( !( load_char_obj ( ( victDD = malloc( sizeof( DESCRIPTOR_DATA) ) ), arg ) ) )
        {
            send_to_char( "Who's that?!\n\r", ch );
            return;
        }
        else
        {
            victim = victDD->original ? victDD->original : victDD->character;
            logged_in = FALSE;
        }
    }

    if ( IS_NPC( victim ) )
        return;

    if ( victim == ch )
    {
        send_to_char( "Try &+rsociety secede&n!\n\r", ch );
        return;
    }

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N&n isn't even from $t.", ch, clan->who_name, victim, TO_CHAR );
        return;
    }

    if( ch->pcdata->rank <= victim->pcdata->rank )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    sprintf( buf, "Log %s: kicking %s from %s", ch->name,
            victim->name, ch->pcdata->clan->name  );
    log_clan( buf );

    // this function resets member data
    remove_from_clan( victim );
    victim->pcdata->rank = RANK_NORMAL;

    if ( logged_in )
        send_to_char( "&+RYou have been booted from your guild!&n\n\r", victim );

    send_to_char( "You have kicked them out of the guild.\n\r", ch );    

    save_char_obj( victim );
    save_clan( clan );

    /* Close rented chars pfile. */
    if ( !logged_in )
        free( victDD );

    return;
}

void do_promote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *victDD;
    CHAR_DATA       *wch;
    CLAN_DATA       *clan;
    int              count;
    char             arg [ MAX_INPUT_LENGTH ];
    char             buf [ MAX_STRING_LENGTH ];
    int              newrank;
    bool             logged_in;

    last_fun( "do_promote" );

    if( IS_NPC( ch ) )
      return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Promote whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch ) || ch->pcdata->rank != RANK_LEADER )
    {
        send_to_char( "You don't have the power to do this.  Only a leader may promote someone.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    victim = NULL;
    for ( wch = char_list; wch ; wch = wch->next )
    {
        if( wch->deleted || IS_NPC( wch ) )
            continue;
        if( is_name( arg, wch->name ) )
        {
            victim = wch;
            logged_in = TRUE;
            break;
        }
    }

    if ( victim == NULL )
    {
        if ( !( load_char_obj ( ( victDD = malloc( sizeof( DESCRIPTOR_DATA) ) ), arg ) ) )
        {
            send_to_char( "Who's that?!\n\r", ch );
            return;
        }
        else
        {
            victim = victDD->original ? victDD->original : victDD->character;
            logged_in = FALSE;
        }
    }

    if ( IS_NPC( victim ) || victim == ch )
        return;

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N&n isn't even from $t.", ch, clan->who_name, victim, TO_CHAR );
        return;
    }

    // This is technically where we would give them their guild badges.
    switch ( victim->pcdata->rank )
    {
    default:
	return;
    case RANK_NORMAL:
    case RANK_OFFICER:
    case RANK_SENIOR:
    case RANK_DEPUTY:
	break;
    case RANK_LEADER:
	send_to_char( "You may not promote that person any further.\n\r", ch );
	return;
	break;
    }

    victim->pcdata->rank++;
    newrank = victim->pcdata->rank;

    set_title( victim, clan->rank_name[victim->pcdata->rank] );

    sprintf( buf, "Log %s: promoting %s to %d in clan %s",
	    ch->name,
	    victim->name,
            newrank,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    sprintf( buf,
	    "The grand Overlord %s says:\n\r\n\r"
	    "'I hereby promote you %s to %s!'\n\r",
	    ch->name,
	    victim->name,
	    flag_string( rank_flags, newrank ) );

    if ( logged_in )
        send_to_char( buf, victim );

    act( "You have promoted $N&n to $t.",
	ch, flag_string( rank_flags, newrank ), victim, TO_CHAR );

    for( count = 0; count < MAX_CLAN; count++ )
    {
      if( !str_cmp( clan->member[count]->name, victim->name ))
      {
        clan->member[count]->rank = victim->pcdata->rank;
      }
    }

    save_char_obj( victim );
    save_clan( clan );
    
    /* Close rented chars pfile. */
    if ( !logged_in )
        free( victDD );

    return;
}


void do_demote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *victDD;
    CHAR_DATA       *wch;
    CLAN_DATA       *clan;
    char             arg [ MAX_INPUT_LENGTH ];
    char             buf [ MAX_STRING_LENGTH ];
    char             *arg2;
    int              newrank;
    int              count;
    bool             logged_in;

    last_fun( "do_demote");

    arg2 = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Demote whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank != RANK_LEADER )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    victim = NULL;
    for ( wch = char_list; wch ; wch = wch->next )
    {
        if( wch->deleted || IS_NPC( wch ) )
            continue;
        if( is_name( arg, wch->name ) )
        {
            victim = wch;
            logged_in = TRUE;
            break;
        }
    }

    if ( victim == NULL )
    {
        if ( !( load_char_obj ( ( victDD = malloc( sizeof( DESCRIPTOR_DATA) ) ), arg ) ) )
        {
            send_to_char( "Who's that?!\n\r", ch );
            return;
        }
        else
        {
            victim = victDD->original ? victDD->original : victDD->character;
            logged_in = FALSE;
        }
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "NPCs are not guildable!\n\r", ch );
        return;
    }

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N&n isn't even from $t.", ch, clan->who_name, victim, TO_CHAR );
        return;
    }

    switch ( victim->pcdata->rank )
    {
    default:
	send_to_char( "You can't demote this person.\n\r", ch );
	return;
	break;
    case RANK_NORMAL:
	send_to_char( "You can't demote them any further.\n\r", ch );
	return;
	break;
    case RANK_SENIOR:
    case RANK_OFFICER:
    case RANK_DEPUTY:
	break;
    case RANK_LEADER:
        if ( strcmp( arg2, "confirm" ) ) {
	    send_to_char( "Try 'demote leadername confirm' to demote a leader.\n\r", ch );
	    return;
        }
	break;
    }

    victim->pcdata->rank--;
    newrank = victim->pcdata->rank;

    set_title( victim, clan->rank_name[victim->pcdata->rank] );

    sprintf( buf, "Log %s: demoting %s to %s",
	    ch->name,
	    victim->name,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    sprintf( buf,
	    "The grand Overlord %s says:\n\r\n\r"
	    "'I hereby demote you %s to %s!!!'\n\r"
	    "You should make more efforts to improve!",
	    ch->name,
	    victim->name,
	    flag_string( rank_flags, newrank ) );

    if ( logged_in )
        send_to_char( buf, victim );

    act( "You have demoted $N to $t.",
	ch, flag_string( rank_flags, newrank ), victim, TO_CHAR );

    for( count = 0; count < MAX_CLAN; count++ )
    {
      if( !str_cmp( clan->member[count]->name, victim->name ))
      {
        clan->member[count]->rank = victim->pcdata->rank;
      }
    }

    save_char_obj( victim );
    save_clan( clan );

    /* Close rented chars pfile. */
    if ( !logged_in )
        free( victDD );

    return;
}

void do_cdonate( CHAR_DATA *ch, char *argument )
{
    char      arg [ MAX_INPUT_LENGTH ];
    CLAN_DATA *clan;
    COIN_DATA  coin;
    int        value = 0;
    int        count = 0;
    int        coinage;
    bool       success = FALSE;

    last_fun( "do_cdonate");

    if( IS_NPC( ch ))
        return;

	arg[0] = '\0';
	if( !( clan = ch->pcdata->clan ))
    {
        send_to_char( "You're not in a guild!\n\r", ch );
        return;
    }
    if ( arg[0] == '\0' ) {
        send_to_char( "Deposit what?\n\r", ch );
        return;
    }
    if ( !fill_coin_data(&coin, argument, &(ch->money) ) ) {
        send_to_char( "&+LSyntax: &+RSoc deposit &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
        return;
    }
    if ( !coin.copper && !coin.silver && !coin.gold && !coin.platinum ) {
        send_to_char( "You have none of that type of &+Lcoin&n yet.\n\r", ch );
        return;
    }
    for (coinage = 0; coinage < 4; coinage++ ) {
        switch (coinage) {
            case 0: //copper
                if (coin.copper < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.copper > ch->money.copper) {
                    send_to_char("You don't have that much &+ycopper&n coin!\n\r", ch);
                    continue;
                }
                if (coin.copper == 0) continue;
                ch->money.copper -= coin.copper;
                success = TRUE;
                break;
            case 1: //silver
                if (coin.silver < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.silver > ch->money.silver) {
                    send_to_char("You don't have that much &+wsilver&n coin!\n\r",
ch);
                    continue;
                }
                if (coin.silver == 0) continue;
                ch->money.silver -= coin.silver;
                success = TRUE;
                break;

            case 2: //gold
                if (coin.gold < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.gold > ch->money.gold) {
                    send_to_char("You don't have that much &+Ygold&n coin!\n\r", ch
);
                    continue;
                }
                if (coin.gold == 0) continue;
                ch->money.gold -= coin.gold;
                success = TRUE;
                break;
            case 3: //platinum
                if (coin.platinum < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.platinum > ch->money.platinum) {
                    send_to_char("You don't have that much &+Wplatinum&n coin!\n\r"
, ch);
                    continue;
                }
                if (coin.platinum == 0) continue;
                ch->money.platinum -= coin.platinum;
                success = TRUE;
                break;
            } //end switch
        } //end for
    if (success) {
        value = coin.copper + 10*coin.silver + 100*coin.gold + 1000*coin.platinum;
        for( count = 0; count < MAX_CLAN; count++ )
        {
          if( !str_cmp( clan->member[count]->name, ch->name ))
          {
            clan->member[count]->fine -= value;
            if( clan->member[count]->fine < 0 ) {
                clan->member[count]->fine = 0;
                send_to_char("&+WThank you for the donation!&n\n\r", ch);
            } else
                send_to_char("You reduce your fine.\n\r", ch);
          }
        }
        clan->copper += coin.copper;
        clan->silver += coin.silver;
        clan->gold += coin.gold;
        clan->platinum += coin.platinum;
        save_clan( clan );
        save_char_obj( ch );
    }
    else {
        send_to_char( "&+LSyntax:  &+RSoc deposit &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
    }
    return;
}

/*
 * "leav" command is a trap to the "leave" command.
 */
void do_leav( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_leav");

    send_to_char( "If you want to LEAVE, you have to spell it out.\n\r", ch );

    return;
}

void do_leave( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "do_leave");

    if ( !is_clan( ch )  )
    {
        send_to_char( "You aren't a clansman.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( ch->pcdata->rank == RANK_LEADER )
    {
        send_to_char( "Huh? An Overlord shouldn't leave his clan!  Get demoted first!\n\r", ch );
        return;
    }

    remove_from_clan( ch );
    ch->pcdata->rank = RANK_NORMAL;

    send_to_char( buf, ch );

    act( "You have left clan $t.", ch, clan->who_name, ch, TO_CHAR );
    save_char_obj( ch );
    save_clan( clan );
    
    return;
}

void do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf1 [ MAX_STRING_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        found;

    last_fun( "do_clans");

    buf1[0] = '\0';

    sprintf( buf, "&+BClan            Members Pkills   Pdeaths  Mkills   Mdeaths  Frags&n\n\r" );
    strcat( buf1, buf );

    for ( clan = clan_first, found = 0; clan; clan = clan->next )
    {
	sprintf( buf, "%-15.15s %5d    %5d    %5d    %5d    %5d    &+W%5d&n\n\r",
		clan->name,
                clan->members,
		clan->pkills,
                clan->pdeaths,
                clan->mkills,
                clan->mdeaths,
                clan->frags );
	strcat( buf1, buf );
	found++;
    }

    if ( !found )
	sprintf( buf, "There are no Guilds currently formed.\n\r" );
    else
	sprintf( buf, "You see %d clan%s in the game.\n\r",
		found,
		found == 1 ? "" : "s" );

    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}

void do_claninfo( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];
    int        count;

    last_fun( "do_claninfo");

    if ( !is_clan( ch )  )
    {
        send_to_char( "You aren't a clansman.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    buf1[0] = '\0';

    sprintf( buf, "%s\n\r", clan->name );
    strcat( buf1, buf );

    sprintf( buf, "Motto: \"%s\"\n\r", clan->motto );
    strcat( buf1, buf );

    strcat( buf1, "-----------------------------------------------------------------------------\n\r" );

    sprintf( buf, "%s\n\r", clan->description );
    strcat( buf1, buf );
    
    sprintf( buf, "Leader:  %s\n\r",
	    clan->overlord );
    strcat( buf1, buf );

    sprintf( buf, "Members:   %5d\n\rFrags: &+W%5d&n\n\r",
	    clan->members, clan->frags );
    strcat( buf1, buf );

    sprintf( buf, "Vault:     %d &+Wplatinum&n, %d &+Ygold&n, %d silver, %d &+ycopper&n\n\r",
            clan->platinum, clan->gold, clan->silver, clan->copper );
    strcat( buf1, buf );

    sprintf( buf, "Current Applicant: %s\n\r", clan->applicant ? clan->applicant->name : "(none)" );
    strcat( buf1, buf );

    if( ch->pcdata->rank == RANK_LEADER )
    {
       sprintf( buf, "\n\r&+cRank Titles:&n\n\r  Exile: %s\n\r  Parole: %s\n\r  Normal: %s\n\r  Senior: %s\n\r"
                     "  Officer: %s\n\r  Deputy: %s\n\r  Leader: %s\n\r",
       clan->rank_name[RANK_EXILED],
       clan->rank_name[RANK_PAROLE],
       clan->rank_name[RANK_NORMAL],
       clan->rank_name[RANK_SENIOR],
       clan->rank_name[RANK_OFFICER],
       clan->rank_name[RANK_DEPUTY],
       clan->rank_name[RANK_LEADER]);
       strcat( buf1, buf );
    }

    sprintf( buf, "\n\rMember            Rank  Fine:\n\r" );
    strcat( buf1, buf );

    for( count = 0; count < MAX_CLAN; count++ )
    {
      if( clan->member[count]->filled )
      {
        sprintf( buf, " &+r%c&n%-15s %s %8d&+yc&n\n\r",
          (get_char_world( ch, clan->member[count]->name ) ? 'o' : ' '),
          clan->member[count]->name,
          clan->rank_name[clan->member[count]->rank],
          clan->member[count]->fine );
        strcat( buf1, buf );
      }
    }

    if( clan->ostra )
    {
      sprintf( buf, "Ostracized: %s\n\r", clan->ostra );
      strcat( buf1, buf );
    }

    send_to_char( buf1, ch );

    return;
}

void do_setclan( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CLAN_DATA *clan;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_setclan");

    rch = get_char( ch );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: setclan <clan> <sedit>                     \n\r", ch );
        send_to_char( "or:     setclan <clan> <field>            <value>  \n\r", ch );
        send_to_char( "or:     setclan <clan> <string>           <value>  \n\r", ch );
        send_to_char( "or:     setclan <clan> overlord <player>           \n\r", ch );
        send_to_char( "\n\rField being one of:                            \n\r", ch );
        send_to_char( " mkills mdeaths pkills pdeaths illegalpk type      \n\r", ch );
        send_to_char( " members                                           \n\r", ch );
        send_to_char( " recall donation class score                       \n\r", ch );
        send_to_char( " obj1 obj2 obj3                                    \n\r", ch );
        send_to_char( "String being one of:                               \n\r", ch );
        send_to_char( " name whoname filename motto ostra                 \n\r", ch );
        send_to_char( "Sedit being one of:                                \n\r", ch );
        send_to_char( " desc                                              \n\r", ch );
        return;
    }

    if ( !( clan = clan_lookup( arg1 ) ) )
    {
        send_to_char( "No such clan.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "overlord" ) || !str_cmp( arg2, "leader" ) )
    {
        free_string( clan->overlord );
        clan->overlord = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "members" ) )
    {
        clan->members = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "mkills" ) )
    {
        clan->mkills = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "mdeaths" ) )
    {
        clan->mdeaths = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "pkills" ) )
    {
        clan->pkills = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "pdeaths" ) )
    {
        clan->pdeaths = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "illegalpk" ) )
    {
        clan->illegal_pk = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "recall" ) )
    {
        clan->recall = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "donation" ) )
    {
        clan->donation = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj1" ) )
    {
        clan->clanobj1 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj2" ) )
    {
        clan->clanobj2 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj3" ) )
    {
        clan->clanobj3 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "type" ) )
    {
	int value = 0;

	if ( value > 0 && value < 4 )
	{
	    clan->clan_type = value;
	    send_to_char( "Done.\n\r", ch );
	    save_clan( clan );
	    return;
	}
	else
	    send_to_char( "Unknown clan type.\n\r1=Clan 2=Order 3=Guild\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "class" ) )
    {
        clan->class = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "score" ) )
    {
        clan->score = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
        free_string( clan->name );
        clan->name = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "ostra" ) )
    {
        free_string( clan->ostra );
        clan->ostra = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "whoname" ) )
    {
        free_string( clan->who_name );
        clan->who_name = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "filename" ) )
    {
        free_string( clan->filename );
        clan->filename = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        save_clan_list( );
        return;
    }
    if ( !str_cmp( arg2, "motto" ) )
    {
        free_string( clan->motto );
        clan->motto = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "desc" ) )
    {
        ch->pcdata->editing = EDIT_CLAN_DESC;
	string_append( ch, &clan->description );
        save_clan( clan );
        return;
    }

    return;
}

void do_showclan( CHAR_DATA *ch, char *argument )
{   
    CHAR_DATA *rch;
    CLAN_DATA *clan;

    last_fun( "do_showclan");

    rch = get_char( ch );

    if ( argument[0] == '\0' )
    {
        int found = 0;
        char buf[MAX_STRING_LENGTH];
        char buf1[MAX_STRING_LENGTH];

        send_to_char( "Syntax: stat clan <clan>\n\r\n\r", ch );
        sprintf( buf1, "Clan           Overlord          Keyword\n\r" );

        for ( clan = clan_first, found = 0; clan; clan = clan->next )
        {
  	  sprintf( buf, "%-16.16s  %-12.12s  %11.11s\n\r",
		clan->who_name,
		clan->overlord,
		clan->name );
	  strcat( buf1, buf );
	  found++;
        }

        if ( !found )
	  sprintf( buf, "There are no Clans/Guilds/Orders currently formed.\n\r" );
        else
	  sprintf( buf, "You see %d clan%s in the game.\n\r",
		found,
		found == 1 ? "" : "s" );

        strcat( buf1, buf );
        send_to_char( buf1, ch );

        return;
    }

    clan = clan_lookup( argument );
    if ( !clan )
    {
        send_to_char( "No such clan.\n\r", ch );
        return;
    }

    printf_to_char( ch, "&+cWhoName :&n %s.\n\r",
                        clan->who_name );
    printf_to_char( ch, "&+cName    :&n %s.\n\r",
                        clan->name );
    printf_to_char( ch, "&+cType    :&n [%5.5d]  %s.\n\r",
                        clan->clan_type,
                        flag_string( clan_flags, clan->clan_type ) );
    printf_to_char( ch, "&+cFilename:&n %s\n\r&+cMotto   :&n \"%s\".\n\r",
                        clan->filename,
                        clan->motto );
    printf_to_char( ch, "&+cDescription:&n \n\r%s\n\r&+cOverlord :&n %s.\n\r",
                        clan->description,
                        clan->overlord );
    printf_to_char( ch, "&+cMembers:  &n %5.5d  &+cClass:   &n %5.5d\n\r",
                        clan->members,
                        clan->class );
    printf_to_char( ch, "&+cMKills:   &n %5.5d  &+cMDeaths: &n %5.5d\n\r",
                        clan->mkills,
                        clan->mdeaths );
    printf_to_char( ch, "&+cPKills:   &n %5.5d  &+cPDeaths: &n %5.5d\n\r",
                        clan->pkills,
                        clan->pdeaths );
    printf_to_char( ch, "&+cFrags:    &n %5.5d  &+cScore:   &n %5.5d\n\r",
                        clan->frags,
                        clan->score );
    printf_to_char( ch, "&+cObj1:     &n %5.5d  &+cObj2:    &n %5.5d  &+cObj3:&n %5.5d\n\r",
                        clan->clanobj1,
                        clan->clanobj2,
                        clan->clanobj3 );
    printf_to_char( ch, "&+cRecall:   &n %5.5d  &+cDonation:&n% 5.5d\n\r",
                        clan->recall, clan->donation );
    printf_to_char( ch, "&+cBank Account: &n%d &+Wplatinum&n, %d &+Ygold&n, %d silver, %d &+ycopper&n\n\r",
                        clan->platinum, clan->gold, clan->silver, clan->copper );
    return;
}

/*
 * Get pointer to clan structure from clan name. -Toric
 */
CLAN_DATA *get_clan( const char *name )
{
    CLAN_DATA *clan;

    last_fun( "get_clan");
    
    for ( clan = clan_first; clan; clan = clan->next )
       if ( !str_cmp( name, clan->name ) )
         return clan;
    return NULL;
}

/*
 * New code for loading clans from file.
 */
bool fread_clan( CLAN_DATA *clan, FILE *fp )
{
    char *word;
    bool  fMatch;
    int   stat;

    last_fun( "fread_clan");

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp, &stat );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
            KEY( "Class",        clan->class,     fread_number( fp, &stat ) );
            KEY( "ClanType",     clan->clan_type, fread_number( fp, &stat ) );
            KEY( "ClanObjOne",   clan->clanobj1,  fread_number( fp, &stat ) );
            KEY( "ClanObjTwo",   clan->clanobj2,  fread_number( fp, &stat ) );
            KEY( "ClanObjThree", clan->clanobj3,  fread_number( fp, &stat ) );
            KEY( "Copper",       clan->copper,    fread_number( fp, &stat ) );
	    break;

	case 'D':
            SKEY( "Desc",    clan->description );
            KEY( "Donation", clan->donation,  fread_number( fp, &stat ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return TRUE;
	    break;

        case 'G':
            KEY( "Gold",        clan->gold,        fread_number( fp, &stat ) );
            break;

	case 'I':
            KEY( "IllegalPK",   clan->illegal_pk,  fread_number( fp, &stat ) );
	    break;

	case 'M':
            KEY( "Members",     clan->members, fread_number( fp, &stat ) );
            KEY( "MKills",      clan->mkills,  fread_number( fp, &stat ) );
            KEY( "MDeaths",     clan->mdeaths, fread_number( fp, &stat ) );
            SKEY( "Motto",      clan->motto );
	    break;

	case 'N':
            SKEY( "Name",      clan->name    );
	    break;

	case 'O':
            SKEY( "Ostra", clan->ostra );
            SKEY( "Overlord", clan->overlord );
	    break;

	case 'P':
            KEY( "PKills",  clan->pkills,  fread_number( fp, &stat ) );
            KEY( "PDeaths", clan->pdeaths, fread_number( fp, &stat ) );
            KEY( "Platinum", clan->platinum, fread_number( fp, &stat ) );
	    break;

	case 'R':
            KEY( "RankExile", clan->rank_name[RANK_EXILED],  fread_string( fp, &stat ) );
            KEY( "RankParole", clan->rank_name[RANK_PAROLE],  fread_string( fp, &stat ) );
            KEY( "RankMember", clan->rank_name[RANK_NORMAL],  fread_string( fp, &stat ) );
            KEY( "RankSenior", clan->rank_name[RANK_SENIOR],  fread_string( fp, &stat ) );
            KEY( "RankOfficer", clan->rank_name[RANK_OFFICER],  fread_string( fp, &stat ) );
            KEY( "RankDeputy", clan->rank_name[RANK_DEPUTY],  fread_string( fp, &stat ) );
            KEY( "RankLeader", clan->rank_name[RANK_LEADER],  fread_string( fp, &stat ) );
            KEY( "Recall",  clan->recall,  fread_number( fp, &stat ) );
	    break;

	case 'S':
            KEY( "Score",     clan->score,     fread_number( fp, &stat ) );
            KEY( "Silver",    clan->silver,    fread_number( fp, &stat ) );
	    break;

	case 'W':
            SKEY( "WhoName",   clan->who_name    );
	    break;

	}

	if ( !fMatch )
	{
            bugf( "Load_clan_file: no match: %s", word );
	}
    }

    return FALSE;
}

bool load_clan_file( char *filename )
{
    CLAN_DATA *clan;
    FILE      *fp;
    int        stat;
    int        count = 0;
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "load_clan_file");

    sprintf( buf, "%s%s", CLAN_DIR, filename );
    log_string( filename );

    if ( !( fp = fopen( buf, "r" ) ) )
    {
        perror( buf );
        return FALSE;
    }

    clan = alloc_mem ( sizeof( CLAN_DATA ) );
    for( count = 0; count < MAX_CLAN; count++)
      clan->member[count] = alloc_mem( sizeof( MEMBER_DATA ));
    count = 0;
    clan->filename = str_dup( filename );

    for( count = 0; count < MAX_RANK; count++ )
      clan->rank_name[count] = NULL;

    clan->ostra = NULL;

    for ( ; ; )
    {
	char *word;
	char letter;

	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_clan_file: # not found.", 0 );
	    free_mem( clan, sizeof( CLAN_DATA ) );
	    break;
	}

	word = fread_word( fp, &stat );

	if ( !str_cmp( word, "CLAN" ) )
	{
	    fread_clan( clan, fp );

	    if ( !clan_first )
		clan_first	= clan;
	    else
		clan_last->next	= clan;
	    clan->next		= NULL;
	    clan_last		= clan;

	}
        else if( !str_cmp( word, "MEMBER" ))
        {
            if( count == MAX_CLAN )
            {
              bug( "Too many members in clan file!  Aborting load!", 0 );
              return TRUE;
            }
            clan->member[count]->name = fread_string( fp, &stat );
            clan->member[count]->rank = fread_number( fp, &stat );
            clan->member[count]->fine = fread_number( fp, &stat );
            clan->member[count]->join_time = fread_number( fp, &stat );
            clan->member[count]->filled = TRUE;
            count++;
        }
	else if ( !str_cmp( word, "END"  ) )				break;
	else
	{
	    bugf( "Load_clan_file: bad section: %s.", word );
	    free_mem( clan, sizeof( CLAN_DATA ) );
	    break;
	}
    }
    fclose( fp );

    return TRUE;
}

/*
 * Load in all the clan files.
 */ 
void load_clans( void )
{
    FILE *fpList;
    char *filename;
    char  fname		[ MAX_STRING_LENGTH ];
    char  clanslist	[ MAX_STRING_LENGTH ];
    int   stat;
    
    last_fun( "load_clans");

    clan_first  = NULL;
    clan_last   = NULL;

    log_string( "Loading clans" );

    sprintf( clanslist, "%s%s", CLAN_DIR, CLANS_LIST );
    if ( !( fpList = fopen( clanslist, "r" ) ) )
    {
        perror( clanslist );
        exit( 1 );
    }

    for ( ; ; )
    {
        filename = feof( fpList ) ? "$" : fread_word( fpList, &stat );
	strcpy( fname, filename );
        if ( fname[0] == '$' )
          break;

        if ( !load_clan_file( fname ) )
        {
          bugf( "Cannot load clan file: %s", fname );
        }
    }
    fclose( fpList );

    return;
}

void save_clan_list( void )
{
    FILE      *fp;
    CLAN_DATA *clan;
    char       clanslist	[ MAX_STRING_LENGTH ];

    last_fun( "save_clan_list");

    sprintf( clanslist, "%s%s", CLAN_DIR, CLANS_LIST );

    fclose( fpReserve );

    if ( !( fp = fopen( clanslist, "w" ) ) )
    {
        bug( "Save_clan_list: fopen", 0 );
        perror( clanslist );
	return;
    }

    for ( clan = clan_first; clan; clan = clan->next )
	fprintf( fp, "%s\n", clan->filename );

    fprintf( fp, "$\n" );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

/*
 * New code for writing a clan to a file.
 */
void save_clan( CLAN_DATA *clan )
{
    FILE                    *fp;
    char                     buf	[ MAX_STRING_LENGTH ];
    int        count = 0;

    last_fun( "save_clan");

    if ( !clan->filename )
	return;
    
    sprintf( buf, "%s%s", CLAN_DIR, clan->filename );

    fclose( fpReserve );

    if ( !( fp = fopen( buf, "w" ) ) )
    {
        bugf( "Cannot open: %s for writing", clan->filename );
    }
    else
    {
	fprintf( fp, "#CLAN\n"						    );
	fprintf( fp, "WhoName       %s~\n",        clan->who_name	    );
	fprintf( fp, "Name          %s~\n",        clan->name		    );
	fprintf( fp, "Motto         %s~\n",        clan->motto		    );
	fprintf( fp, "Desc          %s~\n", fix_string( clan->description ) );
	fprintf( fp, "Overlord      %s~\n",        clan->overlord	    );
	fprintf( fp, "PKills        %d\n",         clan->pkills		    );
	fprintf( fp, "PDeaths       %d\n",         clan->pdeaths	    );
	fprintf( fp, "MKills        %d\n",         clan->mkills		    );
	fprintf( fp, "MDeaths       %d\n",         clan->mdeaths	    );
	fprintf( fp, "IllegalPK     %d\n",         clan->illegal_pk	    );
	fprintf( fp, "Score         %d\n",         clan->score		    );
	fprintf( fp, "ClanType      %d\n",         clan->clan_type	    );
	fprintf( fp, "Members       %d\n",         clan->members	    );
	fprintf( fp, "ClanObjOne    %d\n",         clan->clanobj1	    );
	fprintf( fp, "ClanObjTwo    %d\n",         clan->clanobj2	    );
	fprintf( fp, "ClanObjThree  %d\n",         clan->clanobj3	    );
	fprintf( fp, "Recall        %d\n",         clan->recall 	    );
	fprintf( fp, "Donation      %d\n",         clan->donation	    );
	fprintf( fp, "Class         %d\n",         clan->class  	    );
        fprintf( fp, "Copper        %d\n",         clan->copper             );
        fprintf( fp, "Silver        %d\n",         clan->silver             );
        fprintf( fp, "Gold          %d\n",         clan->gold               );
        fprintf( fp, "Platinum      %d\n",         clan->platinum           );
	fprintf( fp, "Ostra         %s~\n",        clan->ostra		    );
        if( clan->rank_name[RANK_LEADER] )
          fprintf( fp, "RankLeader %s~\n", clan->rank_name[RANK_LEADER] );
        if( clan->rank_name[RANK_DEPUTY] )
          fprintf( fp, "RankDeputy %s~\n", clan->rank_name[RANK_DEPUTY] );
        if( clan->rank_name[RANK_OFFICER] )
          fprintf( fp, "RankOfficer %s~\n", clan->rank_name[RANK_OFFICER] );
        if( clan->rank_name[RANK_SENIOR] )
          fprintf( fp, "RankSenior %s~\n", clan->rank_name[RANK_SENIOR] );
        if( clan->rank_name[RANK_NORMAL] )
          fprintf( fp, "RankMember %s~\n", clan->rank_name[RANK_NORMAL] );
        if( clan->rank_name[RANK_PAROLE] )
          fprintf( fp, "RankParole %s~\n", clan->rank_name[RANK_PAROLE] );
        if( clan->rank_name[RANK_EXILED] )
          fprintf( fp, "RankExile %s~\n", clan->rank_name[RANK_EXILED] );
	fprintf( fp, "End\n"						    );

        for( count = 0; count < MAX_CLAN; count++ )
        {
           if( clan->member[count]->filled == TRUE )
           {
              fprintf( fp, "#MEMBER\n" );
              fprintf( fp, "%s~\n", clan->member[count]->name ); // name
              fprintf( fp, "%d\n", clan->member[count]->rank ); // rank
              fprintf( fp, "%d\n", clan->member[count]->fine ); // fine
              fprintf( fp, "%d\n", clan->member[count]->join_time ); // join_time
           }
        }

	fprintf( fp, "#END\n"						    );

	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

void do_makeclan( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CLAN_DATA *clan;
    char       filename [ MAX_STRING_LENGTH ];
    char       who_name [ MAX_STRING_LENGTH ];
    int        count;

    last_fun( "do_makeclan");

    rch = get_char( ch );

    if ( !authorized( rch, "makeclan" ) )
        return;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: makeclan <clan name>\n\r", ch );
        return;
    }

    one_argument( argument, who_name );
    who_name[14] = '\0';

    sprintf( filename, "%s.clan", who_name );

    clan = alloc_mem ( sizeof( CLAN_DATA ) );
    if ( !clan_first )
	clan_first		= clan;
    else
	clan_last->next		= clan;
	clan->next		= NULL;
	clan_last		= clan;

    clan->filename		= str_dup( filename );
    clan->who_name		= str_dup( who_name );
    clan->name			= str_dup( argument );
    clan->motto			= str_dup( "" );
    clan->description		= str_dup( "" );
    clan->overlord		= str_dup( "" );
    clan->members		= 0;
    clan->recall		= 3001;
    clan->donation		= 0;
    clan->class			= 0;
    clan->mkills		= 0;
    clan->mdeaths		= 0;
    clan->pkills		= 0;
    clan->pdeaths		= 0;
    clan->illegal_pk		= 0;
    clan->score			= 0;
    clan->clan_type		= 1; // Defaults at clan
    clan->clanobj1		= 0;
    clan->clanobj2		= 0;
    clan->clanobj3		= 0;
    clan->rank_name[RANK_EXILED] = str_dup( "Exile of" );
    clan->rank_name[RANK_PAROLE] = str_dup( "Parole of " );
    clan->rank_name[RANK_NORMAL] = str_dup( "Member of " );
    clan->rank_name[RANK_SENIOR] = str_dup( "Senior of" );
    clan->rank_name[RANK_OFFICER] = str_dup( "Officer of" );
    clan->rank_name[RANK_DEPUTY] = str_dup( "Deputy of" );
    clan->rank_name[RANK_LEADER] = str_dup( "Leader of" );


    for( count = 0; count < MAX_CLAN; count++)
      clan->member[count] = alloc_mem( sizeof( MEMBER_DATA ));
    for( count = 0; count < MAX_CLAN; count++ )
    {
      clan->member[count]->name = NULL;
      clan->member[count]->rank = 0;
      clan->member[count]->fine = 0;
      clan->member[count]->join_time = 0;
      clan->member[count]->filled = FALSE;
    }

    send_to_char( "Creation successful.  Now use the command\n\rset clan <clan_name> <variable> <value>\n\r"
                  "to set values for the organization.\n\r\n\rParticularly, you will "
                  "need to set:\n\rwhoname\n\rmotto\n\rdesc\n\roverlord\n\r"
                  "\n\rAfter that, to set the leader, you must use:\n\rset m <name> clan <clan name>\n\r"
                  "and\n\rset m <name> rank <6>\n\r", ch );

    save_clan( clan );
    save_clan_list();

    return;
}

void do_society( CHAR_DATA *ch, char *argument )
{
    char       arg1 [ MAX_INPUT_LENGTH ];

    last_fun( "do_society");

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        do_claninfo( ch, "" );
        return;
    }

    if( !str_prefix( arg1, "kickout" ))
       do_kickout( ch, argument );
    else if( !str_prefix( arg1, "enroll" ))
       do_initiate( ch, argument );
    else if( !str_prefix( arg1, "exile" ))
       do_exile( ch, argument );
    else if( !str_prefix( arg1, "apply" ))
       do_apply( ch, argument );
    else if( !str_prefix( arg1, "secede" ))
       do_leave( ch, argument );
    else if( !str_prefix( arg1, "deposit" ))
       do_cdonate( ch, argument );
    else if( !str_prefix( arg1, "demote" ))
       do_demote( ch, argument );
    else if( !str_prefix( arg1, "decline" ))
       do_decline( ch, argument );
    else if( !str_prefix( arg1, "promote" ))
       do_promote( ch, argument );
    else if( !str_prefix( arg1, "withdraw" ))
       do_cwithdraw( ch, argument );
    else if( !str_prefix( arg1, "fine" ))
       do_fine( ch, argument );
    else if( !str_prefix( arg1, "title" ))
       do_title( ch, argument );
    else if( !str_prefix( arg1, "name" ))
       do_socname( ch, argument );
    else if (!str_prefix( arg1, "ostracize" ))
       do_ostracize( ch, argument );
    else if (!str_prefix( arg1, "hometown" ))
       do_hometown( ch, argument );
    else if( !str_prefix( arg1, "rank" ))
    {
       send_to_char( "Use the promote and demote commands instead of the rank command.\n\r", ch );
       return;
    }
    else
    {
       send_to_char( "That's not a valid society command.\n\r", ch );
       send_to_char( "Commands include: apply, deposit, demote, decline, promote, rank, title, name, and withdrawl.\n\r", ch );
       return;
    }

    return;
}

void do_apply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *leader;

    last_fun( "do_apply");

    if ( ch->level < 25 )
    {
        send_to_char( "You don't have enough experience to join a guild.\n\r", ch );
        return;
    }

    if ( !( leader = get_char_room(ch, argument) ) )
    {
        send_to_char( "You do not see that person here.\n\r", ch );
        return;
    }

    if ( IS_NPC(leader) || !leader->pcdata->clan )
    {
        send_to_char( "Apply to whom?!\n\r", ch );
        return;
    }

    if( leader->pcdata->clan->applicant )
    {
        send_to_char( "That guild is currently reviewing another applicant.  Try again later.\n\r", ch );
        return;
    }

    leader->pcdata->clan->applicant = ch;

    act( "$n&n submits an application to join your guild.", ch, NULL, leader, TO_VICT );
    send_to_char( "You turn in your application.\n\r", ch );

    return;
}

void do_decline( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_decline");

    if( IS_NPC( ch ) || !ch->pcdata->clan || ch->pcdata->rank < RANK_OFFICER )
    {
      send_to_char( "You do not have the power to initiate anyone into a guild.\n\r", ch );
      return;
    }

    if( !ch->pcdata->clan->applicant )
    {
      send_to_char( "Nobody wants to join your silly little guild.\n\r", ch );
      return;
    }

    if( IS_NPC( ch->pcdata->clan->applicant ))
    {
      send_to_char( "How in the heck did a mob apply to your guild!?\n\r", ch );
      ch->pcdata->clan->applicant = NULL;
      return;
    }

    send_to_char( "&+RYour guild application has been declined.&n\n\r", ch->pcdata->clan->applicant );
    send_to_char( "You decline the current guild applicant.\n\r", ch );

    ch->pcdata->clan->applicant = NULL;

    return;
}

// cloned from withdraw code - Veygoth
void do_cwithdraw( CHAR_DATA *ch, char *argument )
{
    char      arg [ MAX_INPUT_LENGTH ];
    CLAN_DATA *clan;

    last_fun( "do_cwithdraw");

    argument = one_argument( argument, arg );

    if( IS_NPC( ch ))
        return;

    if( !( clan = ch->pcdata->clan ))
    {
        send_to_char( "You're not in a guild!\n\r", ch );
        return;
    }

    if( ch->pcdata->rank < RANK_DEPUTY )
    {
        send_to_char( "You'll have to be promoted before you can withdraw from the guild.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Withdraw what?\n\r", ch );
        return;
    }

    if ( is_number( arg ) )
    {
        int       amount;

        amount   = atoi( arg );
        one_argument( argument, arg );

        if ( amount <= 0 )
        {
            send_to_char( "Sorry, you can't do that.\n\r", ch );
            return;
        }

        if( !str_prefix( arg, "copper" ))
        {
          if ( clan->copper < amount )
          {
            send_to_char( "The guild doesen't have that many &n&+ycopper&n coins.\n\r", ch );
            return;
          }
          ch->money.copper += amount;
          clan->copper -= amount;
        }
        else if( !str_prefix( arg, "silver" ))
        {
          if ( clan->silver < amount )
          {
            send_to_char( "The guild doesen't have that many &n&+wsilver&n coins.\n\r", ch );
            return;
          }
          ch->money.silver += amount;
          clan->silver -= amount;
        }
        else if( !str_prefix( arg, "gold" ))
        {
          if ( clan->gold < amount )
          {
            send_to_char( "The guild doesen't have that many &+Ygold&n coins.\n\r", ch );
            return;
          }
          ch->money.gold += amount;
          clan->gold -= amount;
        }
        else if( !str_prefix( arg, "platinum" ))
        {
          if ( clan->platinum < amount )
          {
            send_to_char( "The guild doesen't have that many &+Wplatinum&n coins.\n\r", ch );
            return;
          }
          ch->money.platinum += amount;
          clan->platinum -= amount;
        }
        else
        {
            send_to_char( "You don't have any idea what you are trying to do, do you?\n\r", ch );
            return;
        }

        save_clan( clan );
        save_char_obj( ch );

        send_to_char( "You make a withdrawal.\n\r", ch );
        return;
    }
    else
    {
        send_to_char( "&+LSyntax:  &+RWithdraw &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
        return;
    }

    return;
}

void do_fine( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    int amount;
    int count;

    last_fun( "do_fine");

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Fine whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch ) || ch->pcdata->rank < RANK_DEPUTY )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    amount = atoi( argument );

    if( amount <= 0 )
    {
      send_to_char( "Fine them how much?\n\r", ch );
      return;
    }

    if( !str_cmp( arg, "all" ))
    {
      for( count = 0; count < MAX_CLAN; count++ )
      {
        if( clan->member[count]->filled == TRUE &&
            clan->member[count]->rank < ch->pcdata->rank )
        {
          clan->member[count]->fine += amount;
        }
      }
    }
    else
    {
      bool found = FALSE;
      for( count = 0; count < MAX_CLAN; count++ )
      {
        if( str_cmp( clan->member[count]->name, arg ))
          continue;
        if( clan->member[count]->filled == TRUE &&
            clan->member[count]->rank < ch->pcdata->rank )
        {
          clan->member[count]->fine += amount;
          found = TRUE;
        }
      }
      if( !found )
      {
        send_to_char( "You were not able to fine them.\n\r", ch );
        return;
      }
    }

    send_to_char( "OK.\n\r", ch );    

    save_clan( clan );

    return;
}

void do_socname( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    int rank;

    last_fun( "do_socname");

    argument = one_argument( argument, arg );

    if ( !is_clan( ch ) || ch->pcdata->rank < RANK_LEADER )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Reset the title on which rank?\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    rank = atoi( arg );

    if( rank < 0 || rank > RANK_LEADER )
    {
      send_to_char( "That's not a valid rank.\n\r", ch );
      return;
    }

    if( argument[0] == '\0' )
    {
      send_to_char( "Set the rank title to what?\n\r", ch );
      return;
    }

    free_string( clan->rank_name[rank] );
    clan->rank_name[rank] = str_dup( argument );

    send_to_char( "OK.\n\r", ch );    

    save_clan( clan );

    return;
}

void do_ostracize( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_ostracize");

    argument = one_argument( argument, arg );

    if ( !is_clan( ch ) || ch->pcdata->rank < RANK_DEPUTY )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Who do you want to ostracize?\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if( is_name( arg, clan->ostra ))
    {
      send_to_char( "They've already been ostracized.\n\r", ch );
      return;
    }

    sprintf( buf, "%s %s", clan->ostra, arg );


    if ( !str_cmp( arg, "clear" ) )
    {
        buf[0] = '\0';
    }

    free_string( clan->ostra );
    clan->ostra = str_dup( buf );

    send_to_char( "OK.\n\r", ch );    

    save_clan( clan );

    return;
}

void do_hometown( CHAR_DATA *ch, char *argument )
{

    last_fun( "do_hometown");

    if( !ch )
    {
        bug( "do_hometown: No ch!", 0 );
        return;
    }

    if ( !is_clan( ch ) && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    if ( IS_NPC( ch ) )
	return;

    if( !IS_ROOM( ch->in_room, ROOM_INN ))
    {
        send_to_char( "You can't set your hometown here!\n\r", ch );
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

    sprintf( log_buf, "%s is resetting their hometown to %d.", ch->name, ch->in_room->vnum );
    log_string( log_buf );

    wiznet ( ch, WIZ_LOGINS, get_trust( ch ), log_buf );

    // I can't see any reason why ch would not have an ->in_room, but that
    // may just be shortsighted of me - Veygoth
    if( !ch->in_room )
    {
        bug( "do_hometown: ch not in a room!", 0 );
        return;
    }

    // Put them in the correct body
    if( ch->desc && ch->desc->original )
    {
      do_return( ch, "" );
    }

    send_to_char( "You reset your hometown.\n\r", ch );
    ch->pcdata->currhome = ch->in_room->vnum;

    save_char_obj( ch );

    return;
}

void do_killclan( CHAR_DATA *ch, char *argument ) {
    CLAN_DATA       *clan, *vclan, *pclan;
    CHAR_DATA *vch;
//    int i;
    char            buf[MAX_STRING_LENGTH];
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    // get clan pointer
    clan = get_clan( arg1 );
    if ( !clan ) {
        send_to_char( "That clan doesn't exist!\n\r", ch );
        return;
    }
    // check if they really mean it
    if (strcmp( arg2, "confirm" ) ) {
        send_to_char( "Type 'killclan clanname confirm' to disband a clan.\n\r", ch);
        return;
    }
    sprintf( buf, "You want to kill clan %s.\n\r", clan->who_name );
    send_to_char( buf, ch );

    //warn existing clan members
    for( vch = char_list; vch; vch = vch->next ) {
        if ( vch->deleted )
            continue;
        if ( IS_NPC( vch ) || !vch->pcdata )
            continue;
        if ( vch->pcdata->clan && vch->pcdata->clan == clan ) {
            send_to_char("&+RYour clan has been disbanded!&n\n\r", vch );
            vch->pcdata->clan = 0;
            strcpy(vch->pcdata->title, "");
        }
    }

    pclan = NULL;
send_to_char( "Checking clan list.\n\r", ch );
    for( vclan = clan_first; vclan; vclan = vclan->next ) {
        if (clan == vclan) {
send_to_char( "Clan found on list.\n\r", ch );
                //remove it from the list
            if ( !pclan ) {
                // clan is first on the list
                clan_first = clan->next;
            }
            else {
                pclan->next = clan->next;
            }
                //dddd
        }
        pclan = vclan;
    }
send_to_char( "Clan removed from list.\n\r", ch );

/*  Freeing memory is crashing the mud.  I guess just let it sit there til reboot
 * or until someone fixes it :P
 * Lithos
            //free up memory
            if (clan->filename) free( clan->filename);
            if (clan->who_name) free( clan->who_name);
            if (clan->name) free( clan->name);
            if (clan->motto) free( clan->motto);
            if (clan->description) free( clan->description);
            if (clan->overlord) free( clan->overlord);
            for ( i=0; i < clan->members && clan->member; i++) {
                if (clan->member[i]->name) free( clan->member[i]->name );
                if (clan->member[i]) free( clan->member[i] );
            }
            if (clan->member) free( clan->member );
            free( clan );
*/
    save_clan_list();
    return;
}
