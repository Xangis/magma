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

void apply_affect_mods( CHAR_DATA *ch, AFFECT_DATA *paf);
/* Govno Following list should match merc.h Equipment wear loc list
 *
 * However, any unused locations need to be left in as placeholders
 * - Veygoth
 */
char *	const	where_name	[] = 
{
    "&+y<invalid location>  &n", // 0
    "&+y<worn on finger>    &n", // 1
    "&+y<worn on finger>    &n", // 2
    "&+y<worn around neck>  &n", // 3
    "&+y<worn around neck>  &n", // 4
    "&+y<worn on body>      &n", // 5
    "&+y<worn on head>      &n", // 6
    "&+y<worn on legs>      &n", // 7
    "&+y<worn on feet>      &n", // 8
    "&+y<worn on hands>     &n", // 9
    "&+y<worn on arms>      &n", // 10
    "&+y<worn as shield>    &n", // 11 /* placeholder */
    "&+y<worn about body>   &n", // 12
    "&+y<worn about waist>  &n", // 13
    "&+y<worn around wrist> &n", // 14
    "&+y<worn around wrist> &n", // 15
    "&+y<primary hand>      &n", // 16
    "&+y<held>              &n", // 17 /* placeholder */
    "&+y<secondary hand>    &n", // 18
    "&+y<worn on eyes>      &n", // 19
    "&+y<worn on face>      &n", // 20
    "&+y<worn in left ear>  &n", // 21
    "&+y<worn in right ear> &n", // 22
    "&+y<worn as badge>     &n", // 23
    "&+y<worn on back>      &n", // 24
    "&+y<attached to belt>  &n", // 25
    "&+y<attached to belt>  &n", // 26
    "&+y<attached to belt>  &n", // 27
    "&+y<placeholder>       &n", // 28 /* placeholder */
    "&+y<worn as quiver>    &n", // 29
    "&+y<worn on tail>      &n", // 30
    "&+y<worn on horse body>&n", // 31
    "&+y<worn on horns>     &n", // 32
    "&+y<worn in nose>      &n", // 33
    "&+y<third hand>        &n", // 34
    "&+y<fourth hand>       &n", // 35
    "&+y<lower arms>        &n", // 36
    "&+y<lower hands>       &n", // 37
    "&+y<lower wrist>       &n", // 38
    "&+y<lower wrist>       &n", // 39
};

const int topdown_eq [MAX_WEAR] =
{ 32, 0, 23, 33, 6, 19, 21, 22, 20, 3, 4, 5, 24, 29, 12, 13, 30, 25, 26, 27, \
  31, 10, 36, 14, 15, 38, 39, 9, 37, 1, 2, 11, 16, 18, 34, 35, 17, 7, 8 }; 

const int WAIT_SEARCH = 18;

/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				       bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				       bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
char *  race_space              args( ( int race ) );
char *  class_space             args( ( int class ) );
char *  rem_end                 args( ( const char *str ) );
void	show_aff_lines		args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool	can_speak		args( ( CHAR_DATA *ch, int lang ) );

char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf [ MAX_STRING_LENGTH ];

    last_fun( "format_obj_to_char");
    buf[0] = '\0';
    if ( fShort )
    {
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
	strcat( buf, " ");
    }

    if( IS_OBJ_STAT( obj, ITEM_NOSHOW ) && IS_IMMORTAL( ch ) )
       strcat( buf, "&n(&+LNoshow&n) " );
    else if( IS_OBJ_STAT( obj, ITEM_NOSHOW ) )
	 {
             return buf;
	 }
    if ( IS_OBJ_STAT( obj, ITEM_INVIS)     )   strcat( buf, "&n(&+LInvis&n) " );
    if ( ( is_affected( ch, AFF_DETECT_EVIL  )
	  || IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_ALIGN)
          || ch->class == CLASS_ANTIPALADIN
          || ch->class == CLASS_PALADIN )
        && IS_OBJ_STAT( obj, ITEM_EVIL )   )   strcat( buf, "&n(&+rBlood &+LAura&n) " );
    if ( ( is_affected( ch, AFF_DETECT_GOOD  )
	  || IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_ALIGN)
	  || ch->class == CLASS_ANTIPALADIN 
          || ch->class == CLASS_PALADIN )
        && IS_OBJ_STAT( obj, ITEM_BLESS )  )   strcat( buf, "&n(&+WLight&+L Aura&n) " );
    if ( is_affected( ch, AFF_DETECT_MAGIC )
        && IS_OBJ_STAT( obj, ITEM_MAGIC )  )   strcat( buf, "&n(&+BMagic&n) " );
    if ( IS_OBJ_STAT( obj, ITEM_GLOW )     )   strcat( buf, "&n(&+CGlowing&n) " );
    if ( IS_OBJ_STAT( obj, ITEM_LIT )      )   strcat( buf, "&n(&+WIlluminating&n) " );
    if ( IS_OBJ_STAT( obj, ITEM_SECRET )
	&& is_affected ( ch, AFF_DETECT_HIDDEN) )   strcat( buf, "&n(&+yHidden&n) " );
    strcat ( buf, "");
    
    if (!fShort)
    {
	if ( obj->description )
	    strcat( buf, obj->description );
    }

    strcat ( buf, "&n");

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    OBJ_DATA  *obj;
    char       buf [ MAX_STRING_LENGTH ];
    char       buf1[ MAX_STRING_LENGTH * 4 ];
    char     **prgpstrShow;
    char      *pstrShow;
    int       *prgnShow;
    int        nShow;
    int        iShow;
    int        count;
    bool       fCombine;

    last_fun( "show_list_to_char");
    if ( !ch->desc || !list )
	return;

    buf1[0] = '\0';
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	count++;
    }

    prgpstrShow	= alloc_mem( count * sizeof( char * ) );
    prgnShow    = alloc_mem( count * sizeof( int )    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    { 
      if (!obj->carried_by && !obj->in_obj ) { 
        if (ch->fly_level != obj->fly_level && obj->item_type != TYPE_WALL)
            continue;
      }
      if ( !IS_OBJ_STAT( obj, ITEM_NOSHOW ) || \
         ( IS_OBJ_STAT( obj, ITEM_NOSHOW) && IS_IMMORTAL( ch ) ) )
      {
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
      } // end NOWSHOW parsing addendum Govno
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%d) ", prgnShow[iShow] );
		strcat( buf1, buf );
	    }
	}
	strcat( buf1, prgpstrShow[iShow] );
	strcat( buf1, "\n\r" );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	    strcat( buf1, "     " );
	strcat( buf1, "Nothing.\n\r" );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof( char * ) );
    free_mem( prgnShow,    count * sizeof( int )    );

    send_to_char( buf1, ch );
    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];
    char posbuf [80];
    buf[0] = '\0';

    last_fun( "show_char_to_char_0");
    if ( !victim || !ch )
	return;

    if ( victim->rider
	&& victim->rider->in_room == ch->in_room )
        return;

    // If invis, show char invis symbol first.
    if ( is_affected( victim, AFF_INVISIBLE )   )
    	strcat( buf, "&+L*&n " );

    // Show the player's long description.
    // PC's do not have mob index datas. Quick fix attempt? -Lohrr
    if ( ( (!IS_NPC(ch) && victim->position == POS_STANDING)
         || (IS_NPC(victim) && victim->pIndexData
			&& victim->position == victim->pIndexData->position))
         && (victim->long_descr[0] != '\0')
	 && !victim->riding )
    {
        /* Added long description w/out the stupid \n\r */  
	strcat( buf, rem_end(victim->long_descr));
        strcat( buf, "&n" );
    }
    else
    {
	// Show the player's name.
        strcat( buf, PERS( victim, ch ) );

        strcat( buf, "&n" );

	// Show the player's title.
        // Show the player's race, only if PC. -- Veygoth
	// And on the same side of the racewar or a god.
        if ( !IS_NPC( victim ) )
     	{
	    if ( str_cmp(victim->pcdata->title, " &n" ) )
            	strcat( buf, victim->pcdata->title );

            if( is_clan( victim ))
            {
                strcat( buf, " " );
                strcat( buf, victim->pcdata->clan->who_name );
            }

	    if ( race_table[ ch->race ].racewar_side ==
	 	 race_table[ victim->race ].racewar_side
		 || IS_IMMORTAL ( victim ) || IS_IMMORTAL ( ch ) )
	    {
           	strcat( buf, " (" );
           	strcat( buf, race_table[ get_race(victim) ].colorname );
           	strcat( buf, ")" );
	    }
        }
	
	// Show the player's condition.
        strcpy( posbuf, position_string(victim->position) );
        if (victim->position == POS_STANDING && can_fly( victim) )
            strcpy( posbuf, "flying" );
        strcat( buf, " is " );
        strcat( buf, posbuf );
        strcat( buf, " here");
        if ( victim->fighting )
        {
            strcat( buf, "&n fighting " );
            if( victim->fighting == ch )
                strcat( buf, "&nYOU!" );
	    else if ( victim->in_room == victim->fighting->in_room )
	        strcat( buf, PERS( victim->fighting, ch ) );
            else
                strcat( buf, "&nsomeone who left??" );
        }

    	if ( victim->riding
 	    && victim->riding->in_room == victim->in_room )
    	{
            char buf1 [ MAX_STRING_LENGTH ];

            sprintf( buf1, "&n, mounted on %s&n.", PERS( victim->riding, ch ) );
            strcat( buf, buf1 );
    	}
    	else 
            strcat( buf, "&n." );
    }

    if ( is_affected( victim, AFF_CASTING ) ) {
        strcat( buf, "&n&+y (casting)&n");
    }

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( victim ) <= get_trust( ch ) )
                        strcat( buf, " &n&+g*&n" );
    if ( is_affected( victim, AFF_HIDE ) &&
       ( is_affected( ch, AFF_DETECT_HIDDEN ) ||
         IS_SET(race_table[ch->race].race_abilities, RACE_DETECT_HIDDEN )))
                        strcat( buf, " &n(&+LHiding&n)" );
    if ( is_affected( victim, AFF_CHARM ) && IS_SET( ch->act, PLR_HOLYLIGHT ))
                        strcat( buf, " &n(&n&+mCharmed&n)" );
    if ( ( is_affected( victim, AFF_PASS_DOOR ) 
	  || IS_SET( race_table[ victim->race ].race_abilities, RACE_PASSDOOR ) )
        && IS_SET( ch->act, PLR_HOLYLIGHT ) )
                        strcat( buf, " &n(&+WTranslucent&n)" );
    if( ( victim->race == RACE_UNDEAD || victim->race == RACE_VAMPIRE )
        && ( is_affected( ch, AFF_DETECT_UNDEAD ) || IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
                        strcat( buf, " &n(&+WPale&n)" );
    if ( is_affected( victim, AFF_FAERIE_FIRE ) )
                        strcat( buf, " &n(&n&+mFa&+Me&n&+mr&+Mie&+L Aura&n)" );
    if ( IS_EVIL( victim )
	&& ( is_affected( ch, AFF_DETECT_EVIL )
	    || IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_ALIGN )
            || ch->class == CLASS_PALADIN 
	    || ch->class == CLASS_ANTIPALADIN ))
                        strcat( buf, " &n(&+rBlood&+L Aura&n)" );
    if ( IS_GOOD( victim )
	&& ( is_affected( ch, AFF_DETECT_GOOD )
	    || IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_ALIGN)
            || ch->class == CLASS_PALADIN 
            || ch->class == CLASS_ANTIPALADIN ))
                        strcat( buf, " &n(&+CLight&+L Aura&n)" );
    if ( is_affected( victim, AFF_SANCTUARY ))
                        strcat( buf, " &n(&+WWhite&+L Aura&n)" );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK )  )
                        strcat( buf, " &n&+b(&+RAFK&n&+b)&n" );

    strcat( buf, "\n\r" );
    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char      buf  [ MAX_STRING_LENGTH ];
    char      buf1 [ MAX_STRING_LENGTH ];
    int       iWear;
    int       percent;
    bool      found;

    last_fun( "show_char_to_char_1");
    if ( can_see( victim, ch ) )
    {
        act( "$n&n looks at you.", ch, NULL, victim, TO_VICT    );
        if( victim != ch )
          act( "$n&n looks at $N&n.",  ch, NULL, victim, TO_NOTVICT );
        else
          act( "$n&n looks at $mself.", ch, NULL, victim, TO_NOTVICT );
    }

    buf[0] = '\0';
    if ( victim->riding )
    {
        sprintf( buf1, "&nMounted on %s, ", PERS( victim->riding, ch ) );
	strcat( buf, buf1 );
    }
    else
    if ( victim->rider )
    {
        sprintf( buf1, "&nRidden by %s, ", PERS( victim->rider, ch ) );
	strcat( buf, buf1 );
    }

    if ( !IS_NPC( victim ) && is_clan( victim  ) )
    {
        sprintf( buf1, "&n%s of %s.\n\r", 
		flag_string( rank_flags, victim->pcdata->rank ),
		victim->pcdata->clan->name );
	buf1[0] = UPPER( buf1[0] );
	strcat( buf, buf1 );
    }
    
    act( buf, ch, NULL, victim, TO_CHAR );

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
        act( "&nYou see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( get_max_hit( victim ) > 0 )
	percent = ( 100 * victim->hit ) / get_max_hit( victim );
    else
	percent = -1;

    strcpy( buf, PERS( victim, ch ) );

         if ( percent >= 100 ) strcat( buf, " &nis in perfect &n&+ghealth&n.\n\r" );
    else if ( percent >=  90 ) strcat( buf, " &nis slightly &n&+yscratched&n.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " &nhas a &+yfew bruises&n.\n\r" );
    else if ( percent >=  70 ) strcat( buf, " &nhas &+Ysome cuts&n.\n\r" );
    else if ( percent >=  60 ) strcat( buf, " &nhas &+Mseveral wounds&n.\n\r" );
    else if ( percent >=  50 ) strcat( buf, " &nhas &+mmany nasty wounds&n.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " &nis &+Rbleeding freely&n.\n\r" );
    else if ( percent >=  30 ) strcat( buf, " &nis &+Rcovered in blood&n.\n\r" );
    else if ( percent >=  20 ) strcat( buf, " &nis &+rleaking guts&n.\n\r" );
    else if ( percent >=  10 ) strcat( buf, " &nis &+ralmost dead&n.\n\r" );
    else                       strcat( buf, " &nis &+rDYING&n.\n\r"  );

    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );

/* Govno show size on look at someone */
    sprintf( buf, "%s&n is size %s.\n\r", PERS( victim, ch ), 
                                 size_string( victim->size ));
    send_to_char( buf, ch );

    show_aff_lines( ch, victim );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, topdown_eq[iWear] ) )
	    && can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
                act( "&n$E is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    // Added wielding & shield - Lohrr.
	    if ( obj->item_type == TYPE_WEAPON
		 && ( topdown_eq[iWear] == WEAR_HAND 
			|| topdown_eq[iWear] == WEAR_HAND_3
			|| topdown_eq[iWear] == WEAR_HAND_4
			|| topdown_eq[iWear] == WEAR_HAND_2 )
		 && CAN_WEAR(obj, ITEM_WIELD) ) {
                if ( IS_OBJ_STAT( obj, ITEM_TWOHANDED ) &&
                     !IS_NPC( ch ) && ch->pcdata->twohanded ) 
                    send_to_char ( "&+y<wielding twohanded>&n", ch);
                else
                    send_to_char ( "&+y<wielding>          &n", ch);
            }
	    else
	    {
	    if ( obj->item_type == TYPE_SHIELD
		 && ( topdown_eq[iWear] == WEAR_HAND 
			|| topdown_eq[iWear] == WEAR_HAND_3
			|| topdown_eq[iWear] == WEAR_HAND_4
			|| topdown_eq[iWear] == WEAR_HAND_2 )
		 && CAN_WEAR ( obj, ITEM_WEAR_SHIELD ) )
		send_to_char ( "&+y<worn as shield>    &n", ch );
	    else
		send_to_char( where_name[topdown_eq[iWear]], ch );
	    }
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    // Keep in mind that players can spam looking at someone in order
    // to increase their skill in peek -- this will need to be fixed
    // - Veygoth
    if ( ( victim != ch
	 && !IS_NPC( ch ) 
	 && ( number_percent( ) < ch->pcdata->skl_lrn[gsn_peek]
	      || ch->level >= LEVEL_IMMORTAL ) )
	|| ch->riding == victim 
	|| ch->rider  == victim )
    {
        send_to_char( "\n\r&nYou peek at the inventory:\n\r", ch );
        skill_practice( ch, gsn_peek );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}

void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;
    char       buf  [ MAX_STRING_LENGTH ];
    int sight;

    last_fun( "show_char_to_char");
    if ( !list )
        return;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
        if ( rch->deleted || rch == ch )
	    continue;
        if ( rch->fly_level != ch->fly_level )
            continue;

	if ( !IS_NPC( rch )
	    && IS_SET( rch->act, PLR_WIZINVIS )
	    && get_trust( ch ) < get_trust( rch ) )
	    continue;

        sight = how_see( ch, rch );

	// Added function.
	if ( sight == SEE_VISIBLE )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if( sight == SEE_SENSE_INFRA )
	{
	    sprintf(buf, "&+rYou see the red shape of a %s living being here.&n\n\r"
			, size_string(rch->size) );
            send_to_char( buf, ch );
	}
        else if( sight == SEE_SENSE_HIDDEN )
        {
            send_to_char( "&+LYou sense a lifeform nearby.&n\n\r", ch );
        }
	else if( sight == SEE_INVISIBLE && ( rch->riding ) 
		 && how_see( ch, rch->riding ) != SEE_INVISIBLE )
	{  
	    rch->riding->rider = NULL;
	    show_char_to_char_0( rch->riding, ch );
	    rch->riding->rider = rch->riding;
	}

    }

    return;
} 

bool check_blind( CHAR_DATA *ch )
{
    last_fun( "check_blind");
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( is_affected( ch, AFF_BLIND ) )
    {
        send_to_char( "&+LYou can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}

void do_look( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  	*obj;
    CHAR_DATA 	*victim;
    EXIT_DATA 	*pexit;
    char       	 buf  	[ MAX_STRING_LENGTH ];
    char       	 arg1 	[ MAX_INPUT_LENGTH  ];
    char       	 arg2 	[ MAX_INPUT_LENGTH  ];
    char      	*pdesc;
    int        	 door;
    int		 number = 0;
    int		 count 		= 0;

    last_fun( "do_look");
    /* If it's a mob that isn't switched */
    if ( !ch->desc ) 
	return;

    if ( ch->position < POS_SLEEPING )
    {
        send_to_char( "&nYou can't see anything but &+Ystars&n!  See how pretty!\n\r",
                     ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "&nYou can't see anything, you're &+Lsleeping&n!  Zzz.\n\r",
		     ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    // Look panel for ships - Veygoth
    if( !str_cmp( arg1, "panel" ))
    {
       do_lookpanel( ch, "" );
       return;
    }
    // look out for ships - veygoth
    if( !str_cmp( arg1, "out" ))
    {
       do_lookout( ch, "" );
       return;
    }
    //added 'look room' - Lithos
    /* 'look' or 'look auto' or 'look room'*/
    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) || !str_cmp( arg1, "room") )
    {
        if( ch->fly_level > 0 )
        {
           switch( ch->fly_level )
           {
             case 1:
               send_to_char( "Hovering above ", ch );
               break;
             case 2:
               send_to_char( "Flying above ", ch );
               break;
             case 3:
               send_to_char( "Flying high above ", ch );
               break;
           }
        }
        if( !IS_SET( ch->act, PLR_HOLYLIGHT ) 
            && room_is_dark( ch->in_room )
            && !IS_SET( race_table[ch->race].race_abilities, RACE_ULTRAVISION )
            && !is_affected( ch, AFF_ULTRAVISION ) )
        {
            send_to_char( "&+lSomewhere\n\r", ch );
        }
        else if( !IS_SET( ch->act, PLR_HOLYLIGHT ) )
        {
            // Added support for both manual and automatic descriptions on the worldmap - Veygoth
            if( ch->in_room->vnum < SURFACE_MAP1_START_VNUM || strlen(ch->in_room->name) > 1 )
              sprintf ( buf, "%s&n\n\r", ch->in_room->name );
            else if( ch->in_room->extra_sector_info < MAX_EXTENDED_TERRAIN )
              sprintf( buf, "%s", map_info[ch->in_room->extra_sector_info].room_title );
            else
              sprintf( buf, "No room title." );
            send_to_char( buf, ch );
        }
	else
	{
	    show_room_info( ch, ch->in_room );
	}

        if( ch->in_room->vnum >= SURFACE_MAP1_START_VNUM && !IS_NPC( ch ) )
        {
            if( IS_SET( ch->act, PLR_MAP ) )
                do_worldmap( ch, "" );
            else
                send_to_char( "\n\r", ch );
        }

        if ( !IS_NPC( ch )
            && !IS_SET( ch->act, PLR_HOLYLIGHT )
            && room_is_dark( ch->in_room )
            && !IS_SET( race_table[ch->race].race_abilities, RACE_ULTRAVISION )
            && !is_affected( ch, AFF_ULTRAVISION ) )
        {
            send_to_char( "&+LIt is pitch black...&n \n\r", ch );
            show_char_to_char( ch->in_room->people, ch );
            return;
        }

        /* The str_cmp cannot be replaced by str_prefix since a simple
         *   "look" should be affected by whether the player has brief on.
         *   i.e. look + brief => !room desc. - Lohrr
         */
	if ( !IS_NPC( ch ) && ( !str_cmp( arg1, "room" )
          || !IS_SET( ch->act, PLR_BRIEF ) ) ) 
	{
	    // Added support for both manual and automatic descriptions on the worldmap - Veygoth
            if( ch->in_room->vnum < SURFACE_MAP1_START_VNUM || strlen(ch->in_room->description) > 1)
              sprintf ( buf, "%s&n", ch->in_room->description );
            else if( ch->in_room->extra_sector_info < MAX_EXTENDED_TERRAIN )
              sprintf( buf, "%s", map_info[ch->in_room->extra_sector_info].room_description );
            else
              sprintf( buf, "No room description." );
	    send_to_char( buf, ch );
	}

        if ( IS_ROOM( ch->in_room, ROOM_SILENT ) )
            send_to_char( "&nIt seems preternaturally quiet.\n\r", ch );

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
	    do_exits( ch, "auto" );

        show_room_affects( ch, ch->in_room );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    /* 'look direction' */
    door = -1;
         if ( !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "down"  ) ) door = 5;
    else if ( !str_prefix( arg1, "northwest" ) ) door = 6;
    else if ( !str_prefix( arg1, "southwest" ) ) door = 7;
    else if ( !str_prefix( arg1, "northeast" ) ) door = 8;
    else if ( !str_prefix( arg1, "southeast" ) ) door = 9;
    else if ( !str_prefix( arg1, "nw" ) ) door = 6;
    else if ( !str_prefix( arg1, "sw" ) ) door = 7;
    else if ( !str_prefix( arg1, "ne" ) ) door = 8;
    else if ( !str_prefix( arg1, "se" ) ) door = 9;
    if ( door != -1 )
    {
    /* If no exit data, then return. */
    if ( !( pexit = ch->in_room->exit[door] ) )
    {
        send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if( IS_SET( pexit->exit_info, EX_WALLED ))
    {
        send_to_char( "There's a wall in the way.\n\r", ch );
        return;
    }

    // Check for farsee 
    if( (is_affected( ch, AFF_FARSEE ) || IS_SET( ch->act, PLR_HOLYLIGHT ) )
        && !IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
      if( pexit->to_room )
      {
        ROOM_INDEX_DATA *room;
        room = ch->in_room;
        char_from_room( ch );
        last_fun( "do_look");
        char_to_room( ch, pexit->to_room );
        do_look( ch, "" );
        last_fun( "do_look");
        char_from_room( ch );
        last_fun( "do_look");
        char_to_room( ch, room );
        last_fun( "do_look");
        return;
      }
      else
        send_to_char( "Nothing special there.\n\r", ch );
    }    

    if ( pexit->description && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
        send_to_char( "Nothing special there.\n\r", ch );

    if (   pexit->keyword
	&& pexit->keyword[0] != '\0'
	&& pexit->keyword[0] != ' ' )
    {
        if ( IS_SET( pexit->exit_info, EX_BASHED ) )
            act( "The $d has been bashed from its &n&+whinges&n.",
		    ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
            act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_SECRET ) )
            act( "The $d is secret.",   ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_BLOCKED ) )
            act( "The $d is blocked.",   ch, NULL, pexit->keyword, TO_CHAR );
    }
    else
    {
        if ( IS_SET( pexit->exit_info, EX_BASHED ) )
            act( "The door has been bashed from its &n&+whinges&n.",
		    ch, NULL, NULL, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
            act( "The door is closed.", ch, NULL, NULL, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
            act( "The door is open.",   ch, NULL, NULL, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_SECRET ) )
            act( "The door is secret.",   ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_BLOCKED ) )
            act( "The $d is blocked.",   ch, NULL, pexit->keyword, TO_CHAR );
    }
    /* If exit found, don't keep looking. */
    return;
    }

    /* Look inside something ('look in'). */
    if ( !str_prefix( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
            send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_here( ch, arg2 ) ) )
	{
            send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
            send_to_char( "That is not a container.\n\r", ch );
	    break;

	case TYPE_DRINK_CON:
	    if ( obj->value[1] == -1 )
 	    {
                send_to_char( "It is full.\n\r", ch );
		break;
	    }

	    if ( obj->value[1] <= 0 )
 	    {
                send_to_char( "It is empty.\n\r", ch );
		break;
	    }

            sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half"     : 
		obj->value[1] < obj->value[0]
		    ? "more than half" : "completely",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

	case TYPE_QUIVER:
	case TYPE_CONTAINER:
	case TYPE_CORPSE_NPC:
	case TYPE_CORPSE_PC:
	    if ( IS_SET( obj->value[1], CONT_CLOSED ) )
	    {
                send_to_char( "It is closed.\n\r", ch );
		break;
	    }

            act( "$p&n contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
        case TYPE_PORTAL:
            act("A $p&n leads to:", ch, obj, NULL, TO_CHAR);
            sprintf(buf, "%s\n\r",  get_room_index( obj->value[0] )->name ); 
            strcat( buf, get_room_index( obj->value[0] )->description );
            strcat( buf, "\n\r" );
            send_to_char( buf, ch );
            break;
	}
	return;
    }

    /* Look at another char. */
    if ( ( victim = get_char_room( ch, arg1 ) ) )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    /* Check inventory. */
    obj = get_obj_carry( ch, arg1 );    
    /* If not in inventory, check eq. */
    if ( obj == NULL )
    	obj = get_obj_wear( ch, arg1 );    
    /* If not on character, check room. */
    if ( obj == NULL )
	obj = get_obj_list( ch, arg1, ch->in_room->contents );
    /* Strip the number from name ( 2.necklace -> necklace ) */
    number_argument (arg1, arg1);
    /* If object found, show it to the char. */
    if ( obj != NULL )
    {
        pdesc = get_extra_descr( arg1, obj->extra_descr );
        if ( pdesc != NULL )
        {
            send_to_char( pdesc, ch );
        }
        else if( (pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr ) ) )
        {
            send_to_char( pdesc, ch );
        }
        else if ( obj->description != NULL )
        {
            send_to_char( obj->description, ch );
            send_to_char( "\n\r", ch );
        }
        if( has_affect_obj( obj, gsn_poison_weapon, 0, 0 ) ) {
            if ( ch->class == CLASS_THIEF
                || ch->class == CLASS_ASSASSIN
                || number_percent() < get_curr_int( ch)  / 2 )
                send_to_char("It has a &+Gsickly &+Lcolored&n hue.\n\r", ch );
        }
    return;
    }

/*
 * Original code for looking at an object in your inventory.
 *
 *   for ( obj = ch->carrying; obj; obj = obj->next_content )
 *   {
 *	if ( can_see_obj( ch, obj ) )
 *	{
 *	    pdesc = get_extra_descr( arg1, obj->extra_descr );
 *	    if ( pdesc )
 *	    {
 *		send_to_char( pdesc, ch );
 *		return;
 *	    }
 *
 *	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
 *	    if ( pdesc )
 *	    {
 *		send_to_char( pdesc, ch );
 *		return;
 *	    }
 *	}
 *
 *	if ( is_name( arg1, obj->name ) )
 *	{
 *	    send_to_char( obj->description, ch );
 *	    send_to_char( "\n\r", ch );
 *	    return;
 *	}
 *   }
 *
 * End of original code for looking at an object in your inventory.
 */

    count = 0;
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( ( pdesc = get_extra_descr( arg1, obj->extra_descr ) ) )
	    {
		if ( ++count == number )
		{
	 	    send_to_char( pdesc, ch );
		    return;
		}
		else
		    continue;
	    }
	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc )
	    {
		if ( ++count == number )
		{
		    send_to_char( pdesc, ch );
		    return;
		}
		else
		    continue;
	    }
	    if ( is_name( arg1, obj->name ) )
	    {
		if ( ++count == number )
		{
	    	    send_to_char( obj->description, ch );
	    	    send_to_char( "\n\r", ch );
	    	    return;
		}
		else
		    continue;
	    }
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc )
    {
	send_to_char( pdesc, ch );
	return;
    }

    else
    {
        send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    return;
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      buf [ MAX_STRING_LENGTH ];
    char      arg [ MAX_INPUT_LENGTH  ];

    last_fun( "do_examine");
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "&nExamine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	switch ( obj->item_type )
	{
	default:
	    break;
	case TYPE_DRINK_CON:
	case TYPE_CONTAINER:
	// Quiver added by Lohrr.
	case TYPE_QUIVER:
	case TYPE_CORPSE_NPC:
	case TYPE_CORPSE_PC:
            send_to_char( "&nWhen you look inside, you see:\n\r", ch );
            sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
           EXIT_DATA       *pexit;
    extern char *    const  dir_name [ ];
           char             buf      [ MAX_STRING_LENGTH ];
           int              door;
           bool             found;
           bool             fAuto;
    char doorname[160];

    last_fun( "do_exits");
    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "&+cExits: " : "&+cObvious exits:\n\r" );

    found = FALSE;

    // Veygoth - made it show the direction with an asterisk if it
    // has a closed door, and in exits command it shows up as a
    // closed <whatever>.  It was silly not to show that a room had
    // doors and such.
    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] )
	    && pexit->to_room )
	{
	    if ( IS_SET( pexit->exit_info, EX_SECRET ) )
	    {
		/* Mortals do not see secret exits. */
		if ( ch->level < LEVEL_IMMORTAL )
		    continue;
		/* Mark secret exits for immortals. */
		else
		    strcat( buf, "&+LS&n&+c" );
	    }
            if( IS_SET( pexit->exit_info, EX_BLOCKED ) )
	    {
		/* Mortals do not see secret exits. */
		if ( ch->level < LEVEL_IMMORTAL )
		    continue;
		/* Mark secret exits for immortals. */
		else
		    strcat( buf, "&+yB&n&+c" );
	    }
	    found = TRUE;
	    if ( fAuto )
	    {
                if( IS_SET( pexit->exit_info, EX_WALLED ))
                  continue;
                if(IS_SET( pexit->exit_info, EX_CLOSED ))
                  strcat( buf, "&n&+y#&n&+c" );
                // Don't show exits to impassable rooms - Veygoth
                // show a yellow exclamation point to those with holylight on though
                if( pexit->to_room && pexit->to_room->sector_type == SECT_UNDERG_IMPASSABLE )
                {
                  if( !IS_SET( ch->act, PLR_HOLYLIGHT ))
                    continue;
                  else( strcat( buf, "&+Y!&n" ));
                }
		strcat( buf, dir_name[door] );
		strcat( buf, " " );
	    }
	    else
	    {
                if( IS_SET( pexit->exit_info, EX_WALLED ))
                {
                  sprintf( buf + strlen( buf ), "&n%-5s - (Walled)&n\n\r",
		    capitalize( dir_name[door] ) );
                }
                else if(!IS_SET( pexit->exit_info, EX_CLOSED ))
		{
                  // gods with holylight on can go to useless exits.
                  if( pexit->to_room && pexit->to_room->sector_type == SECT_UNDERG_IMPASSABLE )
                  {
                    if( !IS_SET( ch->act, PLR_HOLYLIGHT ))
                      continue;
                    else( strcat( buf, "&+Y!&n" ));
                  }
                  sprintf( buf + strlen( buf ), "&n%-5s - %s&n\n\r",
		    capitalize( dir_name[door] ),
			( !IS_SET( race_table[ch->race].race_abilities, RACE_ULTRAVISION )
		    	    && room_is_dark( pexit->to_room ) )
			? "&nToo dark to tell" : pexit->to_room->name );
                }
                else
                {
                  one_argument( pexit->keyword, doorname );
                  sprintf( buf + strlen( buf ), "%-5s - closed %s\n\r",
		    capitalize( dir_name[door] ), doorname);
                }
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "&nNone.\n\r" );

    if ( fAuto )
        strcat( buf, "&n\n\r" );

    send_to_char( buf, ch );
    return;
}

/*
 * Enhanced version of do_exits() done by Zen.
 */
void show_room_info( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
    char buf	[ MAX_STRING_LENGTH ];
    char buf1	[ MAX_STRING_LENGTH ];

    if ( !IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return;

    buf[0] = '\0';
    sprintf( buf, "[%d] %s (%s)\n\r",
	    room->vnum,
	    room->name,
	    flag_string( sector_flags, room->sector_type )
    );
    buf1[0] = '\0';
    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}

void show_room_affects( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{

    if( IS_ROOM( room, ROOM_EARTHEN_STARSHELL ) )
        send_to_char( "&+yEarth moves about this room.&n\n\r", ch );
    if( IS_ROOM( room, ROOM_FIERY_STARSHELL   ) )
        send_to_char( "&+rFire burns in the air here.&n\n\r", ch );
    if( IS_ROOM( room, ROOM_AIRY_STARSHELL    ) )
        send_to_char( "&+cAir blows around viciously here.&n\n\r", ch );
    if( IS_ROOM( room, ROOM_WATERY_STARSHELL  ) )
        send_to_char( "&+bWater floats about in this room.&n\n\r", ch );
    if( IS_ROOM( room, ROOM_HYPNOTIC_PATTERN  ) )
        send_to_char( "&+mA &+bbe&+mau&+bt&+mifu&+bl pa&+mtter&+bn  floa&+mts her&+be..&n\n\r", ch );

}

void do_score( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *prev;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];

    buf1[0] = '\0';

/*    if ( IS_NPC( ch ) ) // NPCs don't have same coin structure as
    {                   // players, so do_score will crash the mud
        return;         // if an NPC types score (e.g. a switched imm)
    }
*/
    sprintf( buf,
	    "&+WName: &+G%s&n%s\n\r",
	    ch->name,
	    IS_NPC( ch ) ? "" : ch->pcdata->title );
    strcat( buf1, buf );


    sprintf( buf,
	    "&+WRace:&n %s%s&+WClass:&n %-32.32s&n\n\r",
	    race_table[ get_race( ch ) ].colorname,
	    race_space( ch->race ),
            class_table[ch->class]->who_name );
    strcat( buf1, buf );

    sprintf( buf,
	    "&+WLevel: %-5d          Played: %d hours       &+WSex: %s\n\r",
	    ch->level,
	    ( ch->played / 3600 ),
	    capitalize( get_sex( ch )));
    strcat( buf1, buf );

    if ( ch->fighting == NULL )
    {
    	sprintf( buf, "&+WExperience: &+B%s&n\n\r\n\r", get_exp_string( ch ) );
    	strcat( buf1, buf );
    }

    sprintf( buf,
            "&+WCurrent/Max Health: [&n&+g%4d&+W / &n&+g%-4d&+W]     Coins:     Carried     In Bank\n\r",
	    ch->hit,  get_max_hit( ch ) );
    strcat( buf1, buf );

    sprintf( buf,
            "&+WCurrent/Max Moves:  [&n&+g%4d&+W / &n&+g%-4d&+W]     &+WPlatinum      %4d        %4d\n\r",
	    ch->move, ch->max_move,
            ch->money.platinum, IS_NPC(ch) ? 0 : ch->pcdata->bank.platinum );
    strcat( buf1, buf );

    sprintf( buf,
            "Current/Max Mana:   [&n&+g%4d&+W / &n&+g%-4d&+W]     &+YGold          %4d        %4d\n\r",
	    ch->mana, ch->max_mana,
            ch->money.gold, IS_NPC(ch) ? 0 : ch->pcdata->bank.gold );
    strcat( buf1, buf );

    sprintf( buf,
            "                                      &n&+wSilver        %4d        %4d\n\r",
            ch->money.silver, IS_NPC(ch) ? 0 : ch->pcdata->bank.silver );
    strcat( buf1, buf );

    sprintf( buf,
            "&+WFrags: &+W%-3d&n                            &n&+yCopper        %4d        %4d\n\r",
	    IS_NPC(ch) ? 0 : ch->pcdata->frags,
            ch->money.copper, IS_NPC(ch) ? 0 : ch->pcdata->bank.copper );
    strcat( buf1, buf );

    if( !IS_NPC( ch ))
    {
      sprintf( buf,
            "&+WTotal Deaths: &+W%-4d&n\n\r\n\r",
            (IS_NPC(ch) ? 0 : ch->pcdata->mdeaths + ch->pcdata->pdeaths ) );
      strcat( buf1, buf );
    }

    if( ch->class == CLASS_MONK || ch->class == CLASS_MYSTIC )
    {
      sprintf( buf, "&+WTraining Points: &+B%d&n\n\r", IS_NPC(ch) ? 0 : ch->pcdata->train );
      strcat( buf1, buf );
    }

    if( ch->followers )
    {
      PERSON_DATA *follower;
      strcat( buf1, "&+BFollowers:&n\n\r" );
      for( follower = ch->followers; follower; follower = follower->next )
      {
        if( !follower->who || follower->who->deleted )
          continue;
        sprintf( buf, "%s &n\n\r", PERS( follower->who, ch ));
        strcat( buf1, buf );
      }
      strcat( buf1, "\n\r" );
    }
    if ( is_affected( ch, AFF_POISON ) )
        strcat( buf1, "&+GYou are poisoned.&n\n\r" );

    if ( ( is_affected( ch, AFF_DETECT_MAGIC ) || IS_IMMORTAL( ch) )
          && strcmp( affect_string( ch->affected_by, TRUE ), "none" ) ) {
        strcat( buf1, "&+BEnchantments: &+W");
        strcat( buf1, affect_string( ch->affected_by, TRUE ) );
        strcat( buf1, "&n\n\r\n\r");
    }

    if ( ch->affected )
    {
        bool printed = FALSE;
        prev = NULL;
	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;

	    if ( !printed )
	    {
		strcat( buf1, "&+BActive Spells:&+W\n\r" );
		printed = TRUE;
	    }

            /* Show only new affects to mortals. */
            if (prev != NULL && prev->skill == paf->skill
                && prev->spell == paf->spell
                && prev->song == paf->song && !IS_IMMORTAL( ch ) ) {
                prev = paf;
                continue;
            }
            prev = paf;

            /* Log paf's w/multiple non-zero values. */
            if( ( paf->skill && paf->spell )
             || ( paf->song  && paf->spell )
             || ( paf->skill && paf->song ) )
                log_string( "Possible bug - affect with 2 of spell, skill, song type defined" );

            if( paf->skill != 0 && IS_IMMORTAL( ch ) )
            {
	      sprintf( buf, "%s", skills_table[paf->skill].name );
	      strcat( buf1, buf );
            }
            else if( paf->skill != 0 )
                continue;
            else if( paf->spell != 0 )
            {
	      sprintf( buf, "%s", spells_table[paf->spell].name );
	      strcat( buf1, buf );
            }
            else if( paf->song != 0 )
            {
	      sprintf( buf, "%s", songs_table[paf->song].fullname );
	      strcat( buf1, buf );
            }
            else
            {
              sprintf( buf, "something");
              log_string( "affect without spell or skill associated. possible bug" );
            }

	    if ( IS_IMMORTAL( ch ) )
	    {
		sprintf( buf,
			" modifies %s by %d for %d hours.\n\r",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		strcat( buf1, buf );
	    }
	    else
	    {
		if ( paf->duration == 0 
		     && is_affected( ch, AFF_DETECT_MAGIC ) )
			strcat ( buf1, " (fading rapidly)\n\r" );
		else if ( paf->duration == 1 
		     && is_affected( ch, AFF_DETECT_MAGIC ) )
			strcat ( buf1, " (fading)\n\r" );
                else
                        strcat( buf1, "\n\r" );
	    }
	}
    }
    strcat( buf1, "&n" );
    send_to_char( buf1, ch );
    return;
}

// Code by Veygoth, design by Ski
// Modified to show attributes for others to immortals
void do_attributes( CHAR_DATA *ch, char *argument )
{
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    int time;
    int days;
    int hours;
    int minutes;
    CHAR_DATA *wch;

    if ( IS_NPC( ch ) )
    {
       send_to_char ( "&nYour attributes are as would be expected for an NPC.\n\r", ch); 
       return;
    }

    wch = ch;

    if( IS_IMMORTAL( ch ) && argument[0] != '\0' )
    {
      wch = get_char_world( ch, argument );
      if( !wch )
      {
        send_to_char( "No such person.\n\r", ch );
        return;
      }
      if ( IS_NPC( wch ) )
      {
        send_to_char ( "NPCs don't have skills!\n\r", ch );
        return;
      }
    }


    buf1[0] = '\0';

    sprintf( buf,
	    "&+WName: &+G%-17s&n    &+WLevel: %d&n\n\r",
	    ch->name,
	    ch->level );
    strcat( buf1, buf );

    sprintf( buf,
	    "&+WRace:&n %s%s     &+WClass:&n %s%s&n&+WSex:&n %s\n\r",
	    race_table[ get_race( ch ) ].colorname,
	    race_space( ch->race ),
            class_table[ch->class]->who_name,
	    class_space( ch->class ),
	    capitalize( get_sex( ch )));
    strcat( buf1, buf );

    // Break a player's size into strings when we get around to it
    // -- Veygoth
    if( !IS_NPC( ch))
    {
       sprintf( buf,
	    "&+WHeight:&n %3d inches         &+WWeight:&n %5d pounds    &+WSize:&n %s\n\r",
	    ch->pcdata->height,
            ch->pcdata->weight,
            size_string( ch->size ));
    }
    else
    {
       sprintf( buf,
	    "&+WSize:&n  %3d\n\r",
            ch->size );
    }
    strcat( buf1, buf );

    time = ch->played;
    days = time / 86400;
    time -= (days * 86400);
    hours = time / 3600;
    time -= (hours * 3600);
    minutes = time / 60;

    // Age is a hack until we get it coded - Veygoth
    sprintf( buf,
            "\n\r&+BAge:&n %3d years.            &+BPlaying Time:&n %d days %d hours %d minutes.\n\r",
            get_age( ch ),
            days, hours, minutes );
    strcat( buf1, buf );

    // Need to create a function to display character status strings
    sprintf( buf, "&+BStatus:&n  %s",capitalize( position_string(ch->position)) );
    if( !IS_NPC( ch ) && has_affect( ch, gsn_berserk, 0, 0 ) ) {
       strcat( buf, ", &+RBerserk&n" );
    }
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    {
       strcat( buf, ", Memorizing" );
    }
    if( is_affected( ch, AFF_CASTING ))
    {
       strcat( buf, ", Casting" );
    }
    if( is_affected( ch, AFF_SINGING ))
    {
       strcat( buf, ", Singing" );
    }
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEDITATING ))
    {
       strcat( buf, ", Meditating" );
    }
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_CAMPING ))
    { /* Govno This is ugly and should be moved to its own function */
       strcat( buf, ", Camping" );
    }
    strcat( buf, ".\n\r\n\r" );
    strcat( buf1, buf );

    // We want players to see tha same stats for levels 1-50. -- Veygoth

    // Should create string converters so that we have no decimals displayed
    // below this point.
    sprintf( buf, "     &+cSTR:&n  %-15s      &+cArmor Class:&n %d.\n\r",
             stat_string( get_curr_str(ch) ), get_ac( ch ) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cAGI:&n  %-15s      &+cHitroll:&n     %s\n\r",
             stat_string( get_curr_agi(ch) ), bonus_string(get_hitroll( ch, WEAR_HAND )) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cDEX:&n  %-15s      &+cDamroll:&n     %s\n\r",
             stat_string( get_curr_dex(ch) ), bonus_string(get_damroll( ch, WEAR_HAND )) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cCON:&n  %-15s      &+cAlignment:&n   %s\n\r",
             stat_string( get_curr_con(ch) ), align_string( ch ));
    strcat( buf1, buf );
    sprintf( buf, "     &n&+cINT:&n  %-15s\n\r", stat_string(get_curr_int(ch)) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cWIS:&n  %-15s      &+BSaving Throws&n\n\r",
             stat_string( get_curr_wis(ch) ) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cPOW:&n  %-15s      &+cParalysis:&n %s\n\r",
             stat_string( get_curr_pow(ch) ), bonus_string(-ch->saving_throw[0]) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cCHA:&n  %-15s      &+cRod:&n       %s\n\r",
             stat_string( get_curr_cha(ch) ), bonus_string(-ch->saving_throw[1]) );
    strcat( buf1, buf );
    sprintf( buf, "     &+cLUK:&n  %-15s      &+cPetrify:&n   %s\n\r",
             stat_string( get_curr_luk(ch) ), bonus_string(-ch->saving_throw[2]) );
    strcat( buf1, buf );

    sprintf( buf, "                                &+cBreath:&n    %s\n\r",
             bonus_string(-ch->saving_throw[3]) );
    strcat( buf1, buf );

    sprintf( buf, "&+BWimpy:        &n%-4d              &+cSpell:&n     %s\n\r",
             ch->wimpy, bonus_string(-ch->saving_throw[4]) );
    strcat( buf1, buf );

    sprintf( buf, "&+BLoad Carried: &n%-3d pounds (%s)\n\r",
             ch->carry_weight, weight_string( ch ) );
    strcat( buf1, buf );

    send_to_char( buf1, ch );
    return;
}

void do_exp( CHAR_DATA *ch, char *arg )
{
    char buf[MAX_STRING_LENGTH];

    if( ch->fighting || (ch->position == POS_FIGHTING) )
    {
        send_to_char( "&nEnd the fight first!\n\r", ch );
        return;
    }

    sprintf( buf, "&+B%s&n\n\r",
	    get_exp_string( ch ));
    send_to_char( buf, ch );
}

char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
           char  buf           [ MAX_STRING_LENGTH ];
    extern char  str_boot_time[];
           char *suf;
           int   day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
            "&nIt is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r&nThe year is %d.\n\r",
	    ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
	    time_info.hour >= 12 ? "pm" : "am",
	    day_name[day % 7],
	    day, suf,
	    month_name[time_info.month],
            time_info.year );
    send_to_char( buf, ch );
    sprintf( buf,
            "&n&+rMagma MUD&n started up %s\rThe system time is %s\r",
	    str_boot_time,
	    (char *) ctime( &current_time ) );
    send_to_char( buf, ch );
    if ( down_time > 0 )
    {
        sprintf( buf, "&n%s will be at %s\r", Reboot ? "Reboot" : "Shutdown",
		(char *) ctime( &down_time ) );
	send_to_char( buf, ch );
    }
    return;
}

void do_weather( CHAR_DATA *ch, char *argument )
{
           char         buf     [ MAX_STRING_LENGTH ];
           char         descr   [ MAX_STRING_LENGTH ];
           int          wind;
    static char * const sky_look[ 8 ] =
    {
	"cloudless",
        "&+Wcloudy&n",
        "&+Brainy&n",
        "lit by flashes of &+Cli&n&+cg&+Ch&n&+ctn&+Ci&n&+cn&+Cg&n",
        "slightly &n&+wovercast&n",
        "scattered with a few &+Wflakes&n",
        "filled with &+Wflakes&n",
        "a &+Wblizzard&n of white"
    };

    if ( !IS_OUTSIDE( ch ) )
    {
        send_to_char( "&nYou can't see the weather indoors.\n\r", ch );
	return;
    }

    wind = weather_info.windspeed;
    descr[0] = '\0';
    if( !is_underground( ch ))
    {
      sprintf( buf, "&nThe sky is %s",
	    (time_info.month <= 4 || time_info.month >= 15)
	    ? sky_look[weather_info.sky + 4]
	    : sky_look[weather_info.sky] );
      strcat( descr, buf );
      if( weather_info.sunlight == MOON_RISE )
      {
         switch( weather_info.moonphase )
         {
            default:
              break;
            case MOON_NEW:
              break;
            case MOON_QUARTER:
              sprintf( buf, ", there is a quarter moon," );
              strcat( descr, buf );
              break;
            case MOON_HALF:
              sprintf( buf, ", there is a half moon," );
              strcat( descr, buf );
              break;
            case MOON_THREEQUARTER:
              sprintf( buf, ", the moon is in the third quarter," );
              strcat( descr, buf );
              break;
            case MOON_FULL:
              sprintf( buf, ", the moon is full," );
              strcat( descr, buf );
              break;
         }
      }
      sprintf( buf, "&n and a %s %sward %s blows.\n\r",
	    ( weather_info.temperature < 35 ) ? "cold" :
	    ( weather_info.temperature < 60 ) ? "cool" :
	    ( weather_info.temperature < 90 ) ? "warm" : "hot",
	    dir_name[abs( weather_info.winddir ) % 3],
	    wind <= 20 ? "breeze" :
	    wind <= 50 ? "wind" :
	    wind <= 80 ? "gust" :
			 "torrent" );
      strcat( descr, buf );
    }
    else
    {
      sprintf( descr, "There isn't a whole lot of weather underground.\n\r" );    
    }

    send_to_char( descr, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    bool       hfound = FALSE; /* modification to show multiple helps
				  by Robin Goodall from merc mailing list */

    last_fun( "do_help");

    if ( argument[0] == '\0' )
	argument = "summary";

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

/* changed is_name to is_name_prefix for Ski.  Govno
 * changed it to is_name_prefix again, dunno who messed w/it. - Lohrr
 */
	if ( is_name_prefix( argument, pHelp->keyword ) )
	{
	    if ( hfound )
	        send_to_char( "\n\r----------------------------------------------------------------------------\n\r\n\r", ch );

/* Govno Following adjustment changed to print keywords (titles)        */
/* regardless of pHelp->level.  Just stripped motd and imotd keywords.*/
/* Add further topics that shouldn't be displayed to the list, heh.   */
/* Govno	    if ( pHelp->level >= 0 && str_cmp( argument, "_imotd" ) )  */
	    if ( str_cmp( argument, "_imotd" ) &&
                 str_cmp( argument, "_motd"  ) &&
                 str_cmp( argument, "_login_menu" ) )
	    {
		send_to_char( pHelp->keyword, ch );
		send_to_char( "\n\r", ch );
	    } 

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    hfound = TRUE;
	}
    }
    
    if ( !hfound )
    {
    	char strsave [ MAX_INPUT_LENGTH ];
    	char message [ MAX_STRING_LENGTH ];

// This should never happen.
//    	if ( argument[0] == '\0' )
//	    return;

    	sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, IDEA_FILE );
	sprintf( message, " could not find help on '%s'.",
				 argument );

    	append_file( ch, strsave, message );

        send_to_char( "&nNo help on that word.\n\r", ch );
    }
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 *
 * 'who good' and 'who evil' added by Veygoth
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char             buf      	[ MAX_STRING_LENGTH*3 ];
    char	    *name;
    int              temp;
    int              num_players;
    int              iClass;
    int              iRace;
    int              iLevelLower;
    int              iLevelUpper;
    int              nNumber;
    int              nMatch;
    int              sorted = 0; /* for sorted list */
    int		     rws; // for RaceWarSide wholist
    bool             rgfClass [ MAX_CLASS ];
    bool             rgfRace [ MAX_PC_RACE ];
    bool             fClassRestrict;
    bool             fRaceRestrict;
    bool             fImmortalOnly;

    WHO_DATA *curr_who		= NULL;
    WHO_DATA *next_who		= NULL;
    WHO_DATA *first_mortal	= NULL;
    WHO_DATA *first_imm		= NULL;

    if( ch->fighting || (ch->position == POS_FIGHTING) )
    {
        send_to_char( "&nEnd the fight first!\n\r", ch );
        return;
    }

    /*
     * Set default arguments.
     */
    iLevelLower         = 0;
    iLevelUpper         = MAX_LEVEL;
/* L_DIR; Used to be Max_level.. Why change this? - Lohrr */
    fClassRestrict      = FALSE;
    fRaceRestrict       = FALSE;
    fImmortalOnly       = FALSE;
    name = NULL;
    if ( IS_IMMORTAL ( ch ) )
	rws	        = RACEWAR_NONE;
    else
    	rws	        = race_table[ch->race].racewar_side;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
	rgfRace[iRace] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg  [ MAX_STRING_LENGTH ];

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
                send_to_char( "&nOnly two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    int iClass;

	    /*
	     * Look for classes to turn on.
	     */
	    if ( !str_prefix( arg, "immortals" ) )
	    {
		fImmortalOnly = TRUE;
	    } else
            if (!str_prefix(arg, "sort")) {
                sorted = TRUE;
            }
            else if( IS_IMMORTAL( ch ) && !str_prefix( arg, "good" ) )
            {
                rws = RACEWAR_GOOD;
            }
            else if( IS_IMMORTAL( ch ) && !str_prefix( arg, "evil" ) )
            {
                rws = RACEWAR_EVIL;
            }
	    else
	    {
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( !str_prefix( arg, class_table[iClass]->name ) )
		    {
			rgfClass[iClass] = TRUE;
			fClassRestrict = TRUE;
			break;
		    }
		}

		for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
		{
		    if ( !str_prefix( arg, race_table[iRace].name ) )
		    {
			rgfRace[iRace] = TRUE;
			fRaceRestrict = TRUE;
			break;
		    }
		}

		if ( iClass == MAX_CLASS && iRace == MAX_PC_RACE )
                    name = str_dup (arg);
	    }
	}
    }

    /*
     * Now show matching chars.
     */
   nMatch = 0;
   for (temp = 0; temp <= MAX_LEVEL ; temp++) { /* for sorted list */
    for ( d = descriptor_list; d; d = d->next ) 
    {
	CHAR_DATA       *wch;
	char      const *class;

	wch   = ( d->original ) ? d->original : d->character; 

	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */
	if (  d->connected != CON_PLAYING || !can_see( ch, wch )
		|| wch->deleted )  
	    continue; 

	if ( name == NULL ) 
	{
	/* Outside level/class restrictions. */
	if ( ( wch->level < iLevelLower ) || ( wch->level > iLevelUpper ) 
 	     || ( fClassRestrict && !rgfClass[wch->class] )
 	     || ( fRaceRestrict && !rgfRace[wch->race] ) )
	    continue;
        if (sorted && (wch->level != temp)) 
           continue;
	/* Imm only -> skip non-immortals. */
	if ( fImmortalOnly  && !IS_IMMORTAL( wch ) )
	    continue;
	}
	else if ( !is_name ( wch->name, name ) )
	    continue;

	/* Opposite racewar sides, and both chars are mortals. */
	if ( race_table[wch->race].racewar_side != rws 
                 && rws != RACEWAR_NONE && !IS_IMMORTAL ( wch ) )
	    continue;

	nMatch++;
	buf[0] = '\0';

	/*
	 * Figure out what to print for class.
	 */
	class = class_table[wch->class]->who_name;

	if ( IS_IMMORTAL ( wch ) )
	{
	    switch ( wch->level )
	      {
	      default:    class = "&+yUnknown    &n";   break;
              case L_DIR: class = "&+r Overlord  &n";   break;
	      case L_SEN: class = "&+LImplementor&n";	break;
              case L_JUN: class = "&+mGreater God&n";   break;
	      case L_APP: class = "&+MLesser God &n";	break;
              case L_DEM: class = "&+y  Demigod  &n";   break;
              case L_IMM: class = "&+c Immortal  &n";   break;
              case L_AVA: class = "&+R  Avatar   &n";   break;
              case L_BUI: class = "&+LBu&n&+wil&+Wder&n";   break;
	      }
        }

	/*
	 * Format it up.
	 */
	if ( !IS_IMMORTAL ( wch ) )
	{
	    char buf1 [ MAX_STRING_LENGTH ];

	    sprintf( buf1, "%s%s%s%s %s (%s)",
		    IS_SET( wch->act, PLR_AFK      ) ? "[AFK] "    : "",
                    is_affected( wch, AFF_INVISIBLE) ? "*" : "",
		    wch->name,
		    wch->pcdata->title,
                    !is_clan( wch ) ? "" : wch->pcdata->clan->who_name,
		    race_table[ get_race( wch )].colorname );
	    sprintf( buf + strlen( buf ), "&+L[&n%-2d %-13s&+L]&n %s\n\r",
		    wch->level,
		    class,
		    buf1 );
	}
	else
	{
	    char buf1 [ MAX_STRING_LENGTH ];
	    char buf2 [ MAX_STRING_LENGTH ];

	    sprintf( buf1, "%s%s%s%s",
		    IS_SET( wch->act, PLR_WIZINVIS ) ? "(WIZINVIS) " : "", 
		    IS_SET( wch->act, PLR_AFK      ) ? "[AFK] "    : "",
		    wch->name,
		    wch->pcdata->title );
	    sprintf( buf2, " %s", !is_clan( wch ) ? "" : wch->pcdata->clan->who_name );
	    sprintf( buf + strlen( buf ), "&+L[&n%-15s&+L]&n %s%s\n\r",
		    class,
		    buf1,
		    buf2 );
	}

	curr_who	= alloc_mem( sizeof( WHO_DATA ) );
	curr_who->text	= str_dup( buf );

	if ( !IS_IMMORTAL ( wch ) )
	{
	    curr_who->next	= first_mortal;
	    first_mortal	= curr_who;
	}
	else
	{
	    curr_who->next	= first_imm;
	    first_imm		= curr_who;
	}
    }
    if (!sorted) 
      break;  /* only do loop once for !sorted */
   }
    // Veygoth - made immortals show up above mortals because immortals
    // are more important.
    send_to_char( "\n\r", ch );

    if ( first_imm )
	send_to_char( "&+b-----------------------------------[ &+BIMMORTALS&+b ]------------------------------&n\n\r", ch );

    for ( curr_who = first_imm; curr_who; curr_who = next_who )
    {
	send_to_char( curr_who->text, ch );
	next_who = curr_who->next;
	free_string( curr_who->text );
	free_mem( curr_who, sizeof( WHO_DATA ) ); 
    }

    if ( first_mortal )
        send_to_char( "&n&+b------------------------------------[ &+BMORTALS&n&+b ]-------------------------------&n\n\r", ch );

    for ( curr_who = first_mortal; curr_who; curr_who = next_who )
    {
	send_to_char( curr_who->text, ch );
	next_who = curr_who->next;
	free_string( curr_who->text );
	free_mem( curr_who, sizeof( WHO_DATA ) ); 
    }

    send_to_char( "&n&+b------------------------------------------------------------------------------&n\n\r", ch );


    for ( d = descriptor_list, num_players = 0; d; d = d->next )
        if( d->connected == CON_PLAYING )
            num_players++;

    sprintf( buf, "&nYou see %d of %d player%s in the game.\n\r",
	    nMatch, num_players, num_players == 1 ? "" : "s" );
    send_to_char( buf, ch );
    return;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "&nYou are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int       iWear;
    bool      found;
    last_fun( "do_equipment");
    send_to_char( "&nYou are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( !( obj = get_eq_char( ch, topdown_eq[iWear] ) ) )
	    continue;

	// Added wielding - Lohrr.
	if ( obj->item_type == TYPE_WEAPON
	     && ( topdown_eq[iWear] == WEAR_HAND 
		    || topdown_eq[iWear] == WEAR_HAND_4
		    || topdown_eq[iWear] == WEAR_HAND_3
		    || topdown_eq[iWear] == WEAR_HAND_2 )
	     && CAN_WEAR(obj, ITEM_WIELD) ) {
            if ( IS_OBJ_STAT( obj, ITEM_TWOHANDED ) 
                && !IS_NPC( ch ) && ch->pcdata->twohanded )
                send_to_char ( "&+y<wielding twohanded>&n", ch);
            else
                send_to_char ( "&+y<wielding>          &n", ch);
        }
	else
	{
	if ( obj->item_type == TYPE_SHIELD
	     && ( topdown_eq[iWear] == WEAR_HAND 
		    || topdown_eq[iWear] == WEAR_HAND_4
		    || topdown_eq[iWear] == WEAR_HAND_3
		    || topdown_eq[iWear] == WEAR_HAND_2 )
	     && CAN_WEAR ( obj, ITEM_WEAR_SHIELD ) )
	    send_to_char ( "&+y<worn as shield>    &n", ch );
	else
	    send_to_char( where_name[topdown_eq[iWear]], ch );
	}
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
            send_to_char( "&nsomething.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
        send_to_char( "&nNothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    char     *msg;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    int       value1;
    int       value2;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( IS_NPC( ch ) ) return;

    if ( arg1[0] == '\0' )
    {
        send_to_char( "&nCompare what to what?\n\r", ch );
	return;
    }

    if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
    {
        send_to_char( "&nYou do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
		&& can_see_obj( ch, obj2 )
		&& obj1->item_type == obj2->item_type
		&& ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
            send_to_char( "&nYou aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
	{
	    char new_arg2 [ MAX_INPUT_LENGTH ];
	    int  number;

	    /*  Strip off number argument, subtract one, paste it together */
	    number = number_argument( arg2, arg2 );
	    if ( number > 1 )  number--;
	    sprintf( new_arg2, "%d.%s", number, arg2 );

	    if ( !( obj2 = get_obj_wear( ch, new_arg2 ) ) )
	    {
                send_to_char( "&nYou do not have that item.\n\r", ch );
		return;
	    }
	
	    if ( ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) == 0 )
	    {
                send_to_char( "&nThey are not comparable items.\n\r", ch );
		return;
	    }

	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
        msg = "You compare $p&n to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
        msg = "You can't compare $p&n and $P&n.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
            msg = "You can't compare $p&n and $P&n.";
	    break;

	case TYPE_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case TYPE_CLOTHING:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case TYPE_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( !msg )
    {
        if ( obj2->wear_loc != WEAR_NONE )
        {
                 if ( value1 == value2 )
                     msg = "$p&n and $P&n (equipped) look about the same.";
           else if ( value1  > value2 )
                     msg = "$p&n looks better than $P&n (equipped).";
            else
                     msg = "$p&n looks worse than $P&n (equipped).";
        }
        else
        {
             if ( value1 == value2 ) msg = "$p&n and $P&n look about the same.";
        else if ( value1  > value2 ) msg = "$p&n looks better than $P&n.";
        else                         msg = "$p&n looks worse than $P&n.";
        }

    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_pwhere( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    char             arg [ MAX_INPUT_LENGTH  ];
    bool             found;

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "&nPlayers near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& ( victim = d->character )
		&& !IS_NPC( victim )
		&& victim->in_room
		&& victim->in_room->area == ch->in_room->area
		&& can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
			victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else if ( !str_prefix(arg, "all") )
    {
        send_to_char( "&nPlayer locations:\n\r", ch );
        found = FALSE;
        for ( d = descriptor_list; d; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && ( victim = d->character )
                && !IS_NPC( victim )
                && victim->in_room
                && can_see( ch, victim ) )
            {
                found = TRUE;
                sprintf( buf, "%-28s [%7d]%s&n\n\r",
                        victim->name, victim->in_room->vnum, victim->in_room->name );
                send_to_char( buf, ch );
            }
        }
        if ( !found )
            send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( victim->in_room->area == ch->in_room->area
		&& can_see( ch, victim )
		&& is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
			PERS( victim, ch ), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
            act( "&nYou didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}

void do_consider( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      *msg;
    char       arg [ MAX_INPUT_LENGTH ];
    int        diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "&nConsider killing whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "&nThey're not here.\n\r", ch );
	return;
    }

    // Veygoth - removed code that would not let you consider players.
    // The Mighty Ski - changed messages back to old style due to player demand
    //   and added color (2/14/00)

    diff = victim->level - ch->level;

    if( is_affected( victim, AFF_PROWESS))
      diff += (5 + victim->level / 5);

    if( is_affected( victim, AFF_INCOMPETENCE))
      diff -= (5 + victim->level / 5);

    if( IS_SET( ch->act, PLR_COLOR_CON ))
    {
         if ( diff <= -10 ) msg = "&n&+bNow where did that chicken go?&n";
      else if ( diff <=  -5 ) msg = "&n&+bYou could do it with a needle!&n";
      else if ( diff <=  -2 ) msg = "&n&+BEasy.&n";
      else if ( diff <=  -1 ) msg = "&n&+gFairly easy.&n";
      else if ( diff <=   0 ) msg = "&n&+GThe perfect match!&n";
      else if ( diff <=   1 ) msg = "&n&+gYou would need some luck.&n";
      else if ( diff <=   2 ) msg = "&n&+yYou would need a lot of luck!&n";
      else if ( diff <=   3 ) msg = "&n&+YYou would need a lot of luck and great equipment!&n";
      else if ( diff <=   5 ) msg = "&n&+YDo you feel lucky, punk?&n";
      else if ( diff <=  10 ) msg = "&n&+RAre you mad!?&n";
      else if ( diff <=  15 ) msg = "&n&+RYou ARE mad!&n";
      else if ( diff <=  20 ) msg = "&n&+rWhy don't you just lie down and pretend you're dead instead?&n";
      else if ( diff <=  25 ) msg = "&n&+rWhat do you want your epitaph to say?!?&n";
      else                    msg = "&n&+LThis thing will kill you so fast, it's not EVEN funny!&n";
    }
    else
    {
         if ( diff <= -10 ) msg = "&nNow where did that chicken go?&n";
      else if ( diff <=  -5 ) msg = "&nYou could do it with a needle!&n";
      else if ( diff <=  -2 ) msg = "&nEasy.&n";
      else if ( diff <=  -1 ) msg = "&nFairly easy.&n";
      else if ( diff <=   0 ) msg = "&nThe perfect match!&n";
      else if ( diff <=   1 ) msg = "&nYou would need some luck.&n";
      else if ( diff <=   2 ) msg = "&nYou would need a lot of luck!&n";
      else if ( diff <=   3 ) msg = "&nYou would need a lot of luck and great equipment!&n";
      else if ( diff <=   5 ) msg = "&nDo you feel lucky, punk?&n";
      else if ( diff <=  10 ) msg = "&nAre you mad!?&n";
      else if ( diff <=  15 ) msg = "&nYou ARE mad!&n";
      else if ( diff <=  20 ) msg = "&nWhy don't you just lie down and pretend you're dead instead?&n";
      else if ( diff <=  25 ) msg = "&nWhat do you want your epitaph to say?!?&n";
      else                    msg = "&nThis thing will kill you so fast, it's not EVEN funny!&n";
    }

    act( msg, ch, NULL, victim, TO_CHAR );

    /* Removed health comparison messages - Veygoth */

    return;
}

void do_glance( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       msg[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        send_to_char( "&nGlance at whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, argument ) ) )
    {
        send_to_char( "&nThey're not here.\n\r", ch );
	return;
    }

    sprintf( msg, "&nYou glance at $N&n.\n\r$N&n is %s.", condition_string( victim ) );

    act( msg, ch, NULL, victim, TO_CHAR );
    show_aff_lines( ch, victim );

    return;
}

void set_title( CHAR_DATA *ch, char *title )
{
    char buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    buf[0] = '\0';

    sprintf( buf, " " );

    strcat( buf, title );

    strcat( buf, "&n" );

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}

// Changing title length could screw up formatting in lots of different places.
void do_title( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA	*victim;
    char	*point;
    char	 arg [ MAX_INPUT_LENGTH ];
    int		 length;

    if ( IS_NPC( ch ) )
        return;

    one_argument( argument, arg );

    if ( argument[0] == '\0' )
    {
        send_to_char( "&nChange whose title to what?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
        send_to_char( "&nThat person isn't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) 
        || ( ( ( ch->pcdata->rank < RANK_DEPUTY && ch != victim )
        || (ch->pcdata->clan != victim->pcdata->clan ) )
        && !IS_IMMORTAL(ch) ) )
    {
        send_to_char( "&nThey might not appreciate that.\n\r", ch );
        return;
    }

    if (ch == victim && ch->pcdata->rank < RANK_OFFICER && !IS_IMMORTAL(ch)) 
    {
        send_to_char( "You can't do that at your rank.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    length = 0;
    for( point = argument ; *point ; point++ )
	if ( *point == '&' )
	{	/* Don't count color codes. */
	    point++;
		/* Skip the &n's. */
	    if ( !( *point == 'N' || *point == 'n' ) )
	    {
    		/* Skip the &+'s and &-'s. */
	    	if ( *point == '+' || *point == '-' )
		    point++;
		else
		    if ( ++length >= 60 )
		    {
		        *point = '\0';
		    	break;
		    }
	    }
	}
	else
	    if ( ++length >= 60 )
	    {
		*point = '\0';
		break;
	    }
    
    smash_tilde( argument );
    set_title( victim, argument );
    send_to_char( "&nOk.\n\r", ch );
}

void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;

    ch->pcdata->editing = EDIT_DESCRIPTION;
    string_append( ch, &ch->description );
    return;
}

void do_report( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_INPUT_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH ];

    if ( is_affected( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }


    if (argument[0] == '\0') {
    sprintf( buf,
            "&nYou report: %d/%d hp %d/%d mana %d/%d mv.\n\r",
	    ch->hit,  get_max_hit( ch ),
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move );

    send_to_char( buf, ch );

    sprintf( buf,
	    "$n&n reports: %d/%d hp %d/%d mana %d/%d mv.",
	    ch->hit,  get_max_hit( ch ),
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move );

    act( buf, ch, NULL, NULL, TO_ROOM );
    return;
    }
    //report is essentially a tell, why not use that code? -Lithos
    one_argument(argument, arg);
    sprintf( buf,
	    "%s Reporting: %d/%d hp %d/%d mana %d/%d mv.",
	    arg,
	    ch->hit,  get_max_hit( ch ),
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move );
    do_tell(ch, buf);
    return;
}

void do_practice( CHAR_DATA *ch, char *argument )
{
    send_to_char(
    "&n&+rMagma&n MUD does not use practice.  Go use your skills instead!\n\r",
    ch );
    return;
}

/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->wimpy;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
        send_to_char( "&nYour courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > get_max_hit( ch ) )
    {
        send_to_char( "&nSuch cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "&nWimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    extern const char echo_off_str [ ];

    if ( IS_NPC( ch ) )
	return;

    write_to_buffer( ch->desc, "Password: ", 0 );
    write_to_buffer( ch->desc, echo_off_str, 0 );
    ch->desc->connected = CON_PASSWD_GET_OLD;
    return;
}


// Lists socials.
void do_socials( CHAR_DATA *ch, char *argument )
{
    SOC_INDEX_DATA *social;
    char            buf  [ MAX_STRING_LENGTH * 2];
    char            buf1 [ MAX_STRING_LENGTH * 2 ];
    int             col;
    int             x;
    

    buf1[0] = '\0';
    col = 0;

    for ( x = 0; x < MAX_WORD_HASH; x++ )
    {
	for ( social = soc_index_hash[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
		continue;

	sprintf( buf, "%-12s", social->name );
	strcat( buf1, buf );
	if ( ++col % 6 == 0 )
	    strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 6 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  cmd;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if (   cmd_table[cmd].level <  LEVEL_HERO
	    && cmd_table[cmd].level <= get_trust( ch )
	    && cmd_table[cmd].show )
	{
		sprintf( buf, "%-16s", cmd_table[cmd].name );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
		    strcat( buf1, "\n\r" );
	}
    }

    if ( col % 5 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
	{
            send_to_char( "&nYou are silenced, cur.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_SHOUT    )
		     ? " +SHOUT"
		     : " -shout",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_YELL     )
		     ? " +YELL"
		     : " -yell",
		     ch );

	if ( is_clan( ch ) )
	{
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_CLANTALK )
			 ? " +CLANTALK"
			 : " -clantalk",
			 ch );
	}


	if ( IS_HERO( ch ) )
	{
	    send_to_char( "\n\r", ch );
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_IMMTALK )
			 ? " +IMMTALK"
			 : " -immtalk",
			 ch );
	}

	send_to_char( ".\n\r", ch );
    }
    else
    {
	int  bit;
	bool fClear;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
            send_to_char( "&nChannels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "clantalk" ) ) bit = CHANNEL_CLANTALK;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else
	{
            send_to_char( "&nSet or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT ( ch->deaf, bit );
	else
	    SET_BIT    ( ch->deaf, bit );

        send_to_char( "&nOk.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH ];

//    if ( IS_NPC( ch ) )
//	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "&+L[&n&+m Keyword&+L  ]&n&+r Option&n\n\r", ch );

	send_to_char(  IS_SET( ch->act, PLR_AUTOEXIT  )
            ? "&n[+AUTOEXIT ] &+cYou automatically see exits.\n\r"
            : "&n[-autoexit ] &+cYou don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_BLANK     )
            ? "&n[+BLANK    ] &+cYou have a blank line before your prompt.\n\r"
            : "&n[-blank    ] &+cYou have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_BRIEF     )
            ? "&n[+BRIEF    ] &+cYou see brief descriptions.\n\r"
            : "&n[-brief    ] &+cYou see long descriptions.\n\r"
	    , ch );

        send_to_char(  IS_SET( ch->act, PLR_CAST_TICK )
            ? "&n[+CASTTICK ] &+cYou see your casting ticks.\n\r"
            : "&n[-CASTTICK ] &+cFor spam's sake, no casting ticks!\n\r"
            , ch );
         
	send_to_char(  IS_SET( ch->act, PLR_COMBINE   )
            ? "&n[+COMBINE  ] &+cYou see object lists in combined format.\n\r"
            : "&n[-combine  ] &+cYou see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_COLOUR    )
            ? "&n[+COLOR    ] &+cYou see ANSI colors.\n\r"
            : "&n[-color    ] &+cYou don't see ANSI colors.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_COLOR_CON    )
            ? "&n[+COLORCON ] &+cYou see ANSI colors in consider messages.\n\r"
            : "&n[-colorcon ] &+cYou don't see ANSI colors in consider messages.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_PAGER     )
            ? "&n[+PAGER    ] &+cText is shown in separate pages.\n\r"
            : "&n[-pager    ] &+cText is shown all-at-once with no paging.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_PROMPT    )
            ? "&n[+PROMPT   ] &+cYou have a prompt.\n\r"
            : "&n[-prompt   ] &+cYou don't have a prompt.\n\r"
	    , ch );

        send_to_char(  IS_SET( ch->act, PLR_SHOUT     )
            ? "&n[+SHOUT    ] &+cYou can hear shouts.\n\r"
            : "&n[-shout    ] &+cYou cover your ears when someone is yelling.\n\r"
            , ch );

	send_to_char(  IS_SET( ch->act, PLR_TELNET_GA )
            ? "&n[+TELNETGA ] &+cYou receive a telnet GA sequence.\n\r"
            : "&n[-telnetga ] &+cYou don't receive a telnet GA sequence.\n\r"
	    , ch );

        send_to_char(  IS_SET( ch->act, PLR_TELL   )
            ? "&n[+TELL     ] &+cYou can hear tells.\n\r"
            : "&n[-tell     ] &+cYou are ignoring tells.\n\r"
            , ch );

        send_to_char(  IS_SET( ch->act, PLR_VICIOUS   )
            ? "&n[+VICIOUS  ] &+cYou are vicious and will kill mortally wounded victims.\n\r"
            : "&n[-vicious  ] &+cYou are not vicious and will spare a mortally wounded victim.\n\r", ch );
        send_to_char(  IS_SET( ch->act, PLR_MAP   )
            ? "&n[+MAP      ] &+cYou see the maps.\n\r"
            : "&n[-map      ] &+cYou do not see maps.\n\r", ch );

	send_to_char(  IS_SET( ch->act, PLR_SILENCE   )
            ? "&n[+SILENCE  ] &+cYou are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET( ch->act, PLR_NO_EMOTE  )
	    ? "&n"
            : "&n[-emote    ] &+cYou can't emote.&n\n\r"
	    , ch );
    }
    else
    {
	char  buf [ MAX_STRING_LENGTH ];
	char *point;
	int   bit;
	int   fSet;

	point = arg;
	point++;
	     if ( arg[0] == '-' ) fSet = 0;
	else if ( arg[0] == '+' ) fSet = 1;
	else
	{
	    fSet = 2;
	    point--;
// These two lines if the above doesn't work!
//	    for ( point = buf[0] ; point != '\0'; point ++ )
//		{}  // Go to end of string, then copy over 1.
	}

             if ( !str_cmp( point, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( point, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( point, "brief"    ) ) bit = PLR_BRIEF;
        else if ( !str_cmp( point, "casttick" ) ) bit = PLR_CAST_TICK;
	else if ( !str_cmp( point, "combine"  ) ) bit = PLR_COMBINE;
	else if ( !str_cmp( point, "color"   ) ) bit = PLR_COLOUR;
        else if ( !str_cmp( point, "colorcon" ) ) bit = PLR_COLOR_CON;
        else if ( !str_cmp( point, "pager"    ) ) bit = PLR_PAGER;
        else if ( !str_cmp( point, "shout"    ) ) bit = PLR_SHOUT;
        else if ( !str_cmp( point, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( point, "telnetga" ) ) bit = PLR_TELNET_GA;
        else if ( !str_cmp( point, "compact"  ) ) bit = PLR_BLANK;
        else if ( !str_cmp( point, "tell"     ) ) bit = PLR_TELL;
        else if ( !str_cmp( point, "vicious"  ) ) bit = PLR_VICIOUS;
        else if ( !str_cmp( point, "map"      ) ) bit = PLR_MAP;
	else
	{
            send_to_char( "&nConfig which option?\n\r", ch );
	    return;
	}

        if ( ch->class == CLASS_PALADIN && bit == PLR_VICIOUS )
	{
            send_to_char( "Paladins may not toggle vicious.\n\r", ch );
            /* Just to make sure they don't have it toggled on. */
	    REMOVE_BIT    ( ch->act, bit );
	    return;
	}

	if ( fSet == 1 )
	{
	    SET_BIT    ( ch->act, bit );
            sprintf( buf, "&n%s is now ON.\n\r", point );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	if ( fSet == 0 )
	{
	    REMOVE_BIT ( ch->act, bit );
            sprintf( buf, "&n%s is now OFF.\n\r", point );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	if ( fSet == 2 )
	{
	    TOGGLE_BIT ( ch->act, bit );
	    if ( IS_SET ( ch->act, bit ) )
            	sprintf( buf, "&n%s is now ON.\n\r", point );
	    else
            	sprintf( buf, "&n%s is now OFF.\n\r", point );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	    bug ( "This should never be seen in act_info.c do_config", 0 );

    }

    return;
}

void do_news ( CHAR_DATA *ch, char *argument )
{

    do_help ( ch, "news" );
    return;

}

void do_wizlist ( CHAR_DATA *ch, char *argument )
{

    do_help ( ch, "wizlist" );
    return;

}

// reformatted - Veygoth
// Immortals can also see a player's skill list now.
void do_slist ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    CHAR_DATA *wch;
    int  sn;
    int  level;
    bool pSpell;

    if ( IS_NPC( ch ) )
    {  
       send_to_char ( "&nYou do not need any stinking skills!\n\r", ch );
       return;
    }

    wch = ch;

    if( IS_IMMORTAL( ch ) && argument[0] != '\0' )
    {
      wch = get_char_world( ch, argument );
      if( !wch )
      {
        send_to_char( "No such person.\n\r", ch );
        return;
      }
      if ( IS_NPC( wch ) )
      {  
        send_to_char ( "NPCs don't have skills!\n\r", ch );
        return;
      }
    }

    buf1[0] = '\0';

    strcat ( buf1, "&n&+rALL Abilities available for your class.&n\n\r" );
    strcat ( buf1, "&n&+RLv      Abilities&n\n\r" );

    for ( level = 1; level <= LEVEL_HERO; level++ )
    {

      pSpell = TRUE;

      for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
	if ( !skills_table[sn].name )
	  break;
	if ( skills_table[sn].skill_level[wch->class] != level )
	  continue;

	if ( pSpell )
	{
	  sprintf ( buf, "&+Y%2d&+y:&n", level );
	  strcat ( buf1, buf );
	  pSpell = FALSE;
	}
        else
          strcat( buf1, "   " );

	/* format fix by Koala */ 
        strcat ( buf1, "     " );

        // Show skills as words rather than numbers for non-immortals
        if( !IS_IMMORTAL( ch ))
          sprintf ( buf, "&n&+c%-20s  &+Y%s&n", skills_table[sn].name,
            skill_string( wch->pcdata->skl_lrn[sn] ));
        else
          sprintf ( buf, "&n&+c%-20s  &+Y%d&n", skills_table[sn].name,
            wch->pcdata->skl_lrn[sn] );
	strcat ( buf1, buf );

        strcat ( buf1, "\n\r" );

      }

    }

    send_to_char( buf1, ch );
    return;

}

void do_spells ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH * 3 ];
    char buf1 [ MAX_STRING_LENGTH * 3 ];
    int  sn;
    int  circle;
    bool pSpell;

    if ( IS_NPC( ch ) )
    {  
       send_to_char ( "&nYou do not need any stinking spells!\n\r", ch );
       return;
    }

    buf1[0] = '\0';

    strcat ( buf1, "&n&+rALL spells available for your class.&n\n\r" );
    strcat ( buf1, "&+RLv      Spell&n\n\r" );

    // Don't show 12th circle spells...
    for ( circle = 1; circle < MAX_CIRCLE; circle++ )
    {

      pSpell = TRUE;

      for ( sn = 0; sn < MAX_SPELL; sn++ )
      {
	if ( !spells_table[sn].name )
	  break;
	if ( spells_table[sn].spell_circle[ch->class] != circle )
	  continue;

	if ( pSpell )
	{
	  sprintf ( buf, "&+Y%2d&+y:&n", circle );
	  strcat ( buf1, buf );
	  pSpell = FALSE;
	}
        else
          strcat( buf1, "   " );

	/* format fix by Koala */ 
        strcat ( buf1, "     " );

        if( ch->pcdata->spl_lrn[sn] > 0 )
        {
          sprintf ( buf, "&n&+c%-24s  &+Y%d&n", spells_table[sn].name,
            ch->pcdata->spl_lrn[sn] );
	  strcat ( buf1, buf );
        }
        else
        {
          sprintf ( buf, "&n&+c%-24s  &+Ynot learned&n", spells_table[sn].name );
	  strcat ( buf1, buf );
        }

        strcat ( buf1, "\n\r" );

      }

    }

    send_to_char( buf1, ch );
    return;

}

/* bypassing the config command - Kahn */
void do_autoexit ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOEXIT )
     ? sprintf( buf, "-autoexit" )
     : sprintf( buf, "+autoexit" ) );

    do_config( ch, buf );

    return;

}


void do_blank ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BLANK )
     ? sprintf( buf, "-blank" )
     : sprintf( buf, "+blank" ) );

    do_config( ch, buf );

    return;

}

void do_brief ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BRIEF )
     ? sprintf( buf, "-brief" )
     : sprintf( buf, "+brief" ) ) ; 

    do_config( ch, buf );

    return;

}

void do_combine ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_COMBINE )
     ? sprintf( buf, "-combine" )
     : sprintf( buf, "+combine" ) );

    do_config( ch, buf );

    return;

}
 
void do_pagelen ( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  lines;

    if ( IS_NPC( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	lines = 20;
    else
	lines = atoi( arg );

    if ( lines < 19 )
    {
        send_to_char( "&nLess than 19 lines of paged text is not allowed.\n\r", ch );
	return;
    }

    if ( lines > 60 )
    {
	send_to_char(
                "&nI don't know of a screen that is larger than 60 lines!\n\r",
		     ch );
	lines = 60;
    }

    ch->pcdata->pagelen = lines;
    sprintf( buf, "&nPage pause set to %d lines.\n\r", lines );
    send_to_char( buf, ch );
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
// Updates by Veygoth and Sniktiorg
void do_prompt( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   buf[0] = '\0';

   /* Unswitched NPC's get kicked out */
   if ( !ch->desc )
       return;

   /* Will always have a pc ch after this */
   ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

   if ( argument[0] == '\0' )
   {
       ( IS_SET ( ch->act, PLR_PROMPT )
	? sprintf( buf, "-prompt" )
	: sprintf( buf, "+prompt" ) );

       do_config( ch, buf );

       return;
   }

   if( !strcmp( argument, "raw" ) || !strcmp( argument, "RAW" ) )
      strcat( buf, "&n&+g<%h&n&+g/%H&n&+ghp %mm %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%D&n %B&+g>&n ");
   else if( !strcmp( argument, "dark" ) || !strcmp( argument, "DARK" ) )
      strcat( buf, "&+L<&n&+m%h&+L/&n&+m%H&+Lhp &n&+m%v&+L/&n&+m%V&+Lmv>\n\r&+L<&n%T&n %E&+L>&n ");
   else if( !strcmp( argument, "meter" ) || !strcmp( argument, "METER" ) )
      	if ( ch->class == CLASS_PSIONICIST || ch->class == CLASS_BARD )
            strcat( buf, "&n&+g<%h&n&+g/%H&n&+ghp %mm/%MM %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%d&n %b&+g>&n ");
	else
            strcat( buf, "&n&+g<%h&n&+g/%H&n&+ghp %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%d&n %b&+g>&n ");
   else if( !strcmp( argument, "chains" ) || !strcmp( argument, "CHAINS" ) )
      strcat( buf, "&+L<&n&+m%h&+L/&n&+m%H&+Lhp &n&+m%v&+L/&n&+m%V&+Lmv>\n\r&+L<&n%t&n %e&+L>&n ");
    else if( !strcmp( argument, "status" ) || !strcmp( argument, "STATUS" ) )
      strcat( buf, "&n&+g<%l&+g:%h&n&+g/%H&n&+ghp %mm %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%D&n %B&+g>&n ");
   else if( !strcmp( argument, "all" ) || !strcmp( argument, "ALL" ) )
      	if ( ch->class == CLASS_PSIONICIST || ch->class == CLASS_BARD )
   	    strcat( buf, "&n&+g<%h&n&+g/%H&n&+ghp %m/%Mm %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%D&n %B&+g>&n ");
	else
   	    strcat( buf, "&n&+g<%h&n&+g/%H&n&+ghp %v&n&+g/%V&n&+gmv>\n\r&n&+g<&n%D&n %B&+g>&n ");
   else
   {
      if ( strlen( argument ) > 70 )
	  argument[70] = '\0';
      smash_tilde( argument );
      strcat( buf, argument );
   }

   free_string( ch->pcdata->prompt );
   ch->pcdata->prompt = str_dup( buf );
   send_to_char( "&nOk.\n\r", ch );
   return;
}

void do_auto( CHAR_DATA *ch, char *argument )
{

    do_config( ch, "" );
    return;

}

void do_afk( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;

    if ( IS_SET( ch->act, PLR_AFK ) )
    {
	REMOVE_BIT( ch->act, PLR_AFK );
        send_to_char( "&nYou are back at your keyboard.\n\r", ch        );
	act( "$n&n has returned to $s keyboard.", ch, NULL, ch, TO_ROOM );
    }
    else
    {
	SET_BIT( ch->act, PLR_AFK );
        send_to_char( "&nYou are away from keyboard.\n\r", ch       );
	act( "$n&n has left $s keyboard.", ch, NULL, ch, TO_ROOM        );
    }
    
    return;
}

void do_trophy( CHAR_DATA *ch, char *argument )
{  
    int count;  
    CHAR_DATA *rch;
    char buf[MAX_STRING_LENGTH];  

    if( argument[0] == '\0' || !IS_IMMORTAL( ch ))
      rch = ch;
    else if( !(rch = get_char_world( ch, argument )))
    {
      send_to_char( "No such person.\n\r", ch );
      return;
    }

    // Stop meditating was specifically removed by Veygoth.

    if( IS_NPC( rch ))  
    {
       send_to_char( "Mobs don't have trophy.\n\r", ch );
       return;  
    }

    send_to_char( "&+BTrophy data:\n\r", ch );   
    for( count = 0; count < MAX_LEVEL; count++ ) 
    { 
        if( rch->pcdata->trophy[count].vnum == 0 ) 
            continue; 
        if( rch->pcdata->trophy[count].number == 0 ) 
            continue;  
        /* Added else for mobs which are removed from game. (Earlier, trophy would
           crash the mud on no-longer existant mobs.) */
	if ( get_mob_index ( rch->pcdata->trophy[count].vnum ) != NULL ) 
            sprintf( buf, "   &n&+b(&+y%4.2f&+b)&n %s&n\n\r", 
                ((float)rch->pcdata->trophy[count].number / (float)100), 
	        (get_mob_index(rch->pcdata->trophy[count].vnum))->short_descr);
	else if( rch->pcdata->trophy[count].vnum )
            sprintf( buf, "   &n&+b(&+y%4.2f&+b)&n (%d) \n\r",   
                ((float)rch->pcdata->trophy[count].number / (float)100),
                rch->pcdata->trophy[count].vnum );
	else
            sprintf( buf, "   &n&+b(&+y%4.2f&+b)&n (null) \n\r",   
                ((float)rch->pcdata->trophy[count].number / (float)100));
        send_to_char( buf, ch );
    }     
    send_to_char( "\n\r", ch ); 
    return; 
}

// Returns the text string of someone's sex - Veygoth
char *get_sex( CHAR_DATA *ch )
{
   switch( ch->sex )
   {
      case 0:
         return "neuter";
         break;
      case 1:
         return "male";
         break;
      case 2:
         return "female";
         break;
      default:
         return "mutant";
         break;
   }
}

// Returns how far a character is from their next level as a text string
// -- Veygoth
char *get_exp_string( CHAR_DATA *ch )
{
    int exp;

    // get percent of way to next level
    exp = (100 * ch->exp) / exp_table[ch->level].lvl_exp;

    if( exp < 10 )
      return "You have just begun the trek to your next level!";
    if( exp < 20 )
      return "You are still a very long way from your next level.";
    if( exp < 30 )
      return "You have gained some progress but still have a long way to your next level.";
    if( exp < 40 )
      return "You have gained some progress and are nearing the halfway point.";
    if( exp < 47 )
      return "You are close to the halfway point in the journey to your next level.";
    if( exp < 53 )
      return "You are at the halfway point.";
    if( exp < 60 )
      return "You have just passed the halfway point.";
    if( exp < 70 )
      return "You are well on your way to the next level.";
    if( exp < 80 )
      return "You are three quarters the way to your next level.";
    if( exp < 90 )
      return "You are almost ready to attain your next level.";
    else
      return "You should level anytime now!";
}

void do_innate( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    AFFECT_DATA af;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    // Use innates here
    if ( arg1[0] != '\0' )
    {
	if ( !str_prefix( arg1, "strength" ))
        {
  	  if( IS_SET( race_table[ch->race].race_abilities, RACE_STRENGTH ) )
	  {
	    if ( has_affect( ch, 0, spl_giant_strength, 0 ) )
	    {
		send_to_char( "You are already affected by strength.", ch );
		return;
	    }
            if ( has_innate_timer( ch, INNATE_STRENGTH ) ) {
                send_to_char( "You need to rest a bit first.\n\r", ch);
                return;
            }

	    af.skill     = 0;
	    af.spell     = spl_giant_strength; 
	    af.song      = 0;
	    af.duration  = dice( 8, 5 );
	    af.location  = APPLY_STR;
	    af.modifier  = dice(2, 4) + 15;
	    set_bitvector( &af, AFF_NONE);
	    affect_to_char( ch, &af );
	    send_to_char( "You feel stronger.\n\r", ch );
            add_innate_timer( ch, INNATE_STRENGTH, 24 );

            WAIT_STATE( ch, 14 );

	  }
          else
          {
            send_to_char( "You don't know how to do that.\n\r", ch );
            return;
          }
        }
	else if ( !str_prefix( arg1, "levitate" ))
        {
	  if(IS_SET( race_table[ch->race].race_abilities, RACE_LEVITATE ) )
	  {
	    if ( has_affect( ch, 0, spl_levitation, 0 ) )
	    {
		send_to_char( "You are already levitating.", ch );
		return;
	    }
            if ( has_innate_timer( ch, INNATE_LEVITATE ) ) {
                send_to_char( "You need to rest a bit first.\n\r", ch);
                return;
            }


	    af.skill     = 0;
	    af.spell     = spl_levitation;
	    af.song      = 0;
	    af.duration  = dice( 8, 5 );
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    set_bitvector( &af, AFF_LEVITATE );
	    affect_to_char( ch, &af );
	    send_to_char( "Your feet rise off the ground.\n\r", ch );
            add_innate_timer( ch, INNATE_LEVITATE, 24 );

            WAIT_STATE( ch, 6 );
	  }
          else
          {
            send_to_char( "You don't know how to do that.\n\r", ch );
            return;
          }
        }
	else if ( !str_prefix( arg1, "faerie" ))
        {
          if( IS_SET( race_table[ch->race].race_abilities, RACE_FAERIE_FIRE ))
          {
            CHAR_DATA *victim = get_char_room( ch, arg2 );

            if ( victim == NULL )
                send_to_char( "You do not see them here.", ch ); 
            else
                spell_faerie_fire( spl_faerie_fire,
                    URANGE( 1, ch->level, LEVEL_HERO ), ch,
                    (void *) victim );
	  }
          else
          {
            send_to_char( "You don't know how to do that.\n\r", ch );
            return;
          }
        }
	else if ( !str_prefix( arg1, "invisibility" ))
        {
	  if( IS_SET( race_table[ch->race].race_abilities, RACE_INVIS ) )
	  {
	    if ( is_affected( ch, AFF_INVISIBLE )
	       || has_affect( ch, 0, spl_invis, 0 ) )
	    {
		send_to_char( "You are already invisible!\n\r", ch );
		return;
	    }
            if ( has_innate_timer( ch, INNATE_INVIS ) ) {
                send_to_char( "You need to rest a bit first.\n\r", ch);
                return;
            }

	    af.skill     = 0;
	    af.spell     = spl_invis;
	    af.song      = 0;
	    af.duration  = dice( 2, 8 ) + 4;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    set_bitvector( &af, AFF_INVISIBLE);

	    act( "$n&n fades out of existence.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You vanish.\n\r", ch );

	    affect_to_char( ch, &af );
            add_innate_timer( ch, INNATE_INVIS, 24 );

            WAIT_STATE( ch, 24 );
	  }
          else
          {
            send_to_char( "You don't know how to do that.\n\r", ch );
            return;
          }
        }
	else if ( !str_prefix( arg1, "enlarge" ))
        {
	  if(IS_SET( race_table[ch->race].race_abilities, RACE_ENLARGE ) )
  	  {
	    if ( has_affect( ch, 0, spl_enlarge, 0 ) )
	    {
	    	send_to_char( "You are already enlarged.\n\r", ch);
	    	return;
	    }
            if ( has_innate_timer( ch, INNATE_ENLARGE ) ) {
                send_to_char("You need to rest a bit first.\n\r", ch );
                return;
            }

	    af.skill     = 0;
	    af.spell     = spl_enlarge;
	    af.song      = 0;
	    af.duration  = dice( 8, 5 );
	    af.location  = APPLY_SIZE;
	    af.modifier  = 1;
	    set_bitvector( &af, AFF_NONE);
	    affect_to_char( ch, &af );

	    send_to_char( "&nYou grow to almost twice your normal size!\n\r", ch);
	    act( "$n&n grows to almost twice $s normal size!&n", ch, NULL, NULL, TO_ROOM );
            add_innate_timer( ch, INNATE_ENLARGE, 24 );

            WAIT_STATE( ch, 10 );
          }
          else
          {
            send_to_char( "You don't know how to do that.\n\r", ch );
            return;
          }
	}
	else if ( !str_prefix( arg1, "shift" ))
        {
          if( !str_prefix( arg2, "astral" ))
          {
            if(IS_SET( race_table[ch->race].race_abilities, RACE_SHIFT_ASTRAL ) )
            {
              if ( has_innate_timer( ch, INNATE_SHIFT_ASTRAL ) ) {
                send_to_char("You need to rest a bit first.\n\r", ch );
                return;
              }

              spell_plane_shift( spl_plane_shift, ch->level, ch, (void *)arg2 );
              add_innate_timer( ch, INNATE_SHIFT_ASTRAL, 8 );

              WAIT_STATE( ch, 10 );
            }
            else
            {
              send_to_char( "You don't know how to do that.\n\r", ch );
              return;
            }
	  }
          else if( !str_prefix( arg2, "prime" ))
          {
            if(IS_SET( race_table[ch->race].race_abilities, RACE_SHIFT_PRIME ) )
            {
              if ( has_innate_timer( ch, INNATE_SHIFT_PRIME ) ) {
                send_to_char("You need to rest a bit first.\n\r", ch );
                return;
              }

              spell_plane_shift( spl_plane_shift, ch->level, ch, (void *)arg2 );
              add_innate_timer( ch, INNATE_SHIFT_PRIME, 8 );

              WAIT_STATE( ch, 10 );
            }
            else
            {
              send_to_char( "You don't know how to do that.\n\r", ch );
              return;
            }
          }
	}
	else
	    send_to_char( "That's not implemented yet. :(\n\r", ch);
	return;
    }

    sprintf( buf, "&+BInnate abilities available to your race and class:&n\n\r" );

    if( IS_SET( race_table[ch->race].race_abilities, RACE_BODYSLAM ))
      strcat( buf, "&n  bodyslam\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_CHARGE ))
      strcat( buf, "&n  charge\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_FLY ))
      strcat( buf, "&n  fly\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_PASSDOOR ))
      strcat( buf, "&n  pass door\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_SWIM ))
      strcat( buf, "&n  swim\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_WATERBREATH ))
      strcat( buf, "&n  water breathing\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_INFRAVISION ))
      strcat( buf, "&n  infravision\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_ULTRAVISION ))
      strcat( buf, "&n  ultravision\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_DETECT_ALIGN)
        || ch->class == CLASS_ANTIPALADIN
        || ch->class == CLASS_PALADIN )
      strcat( buf, "&n  detect align\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_DETECT_INVIS ))
      strcat( buf, "&n  detect invis\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_DETECT_HIDDEN ))
      strcat( buf, "&n  detect hidden\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_MUTE ))
      strcat( buf, "&n  mute\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_DOORBASH ))
      strcat( buf, "&n  doorbash\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_SHRUG ))
      strcat( buf, "&n  shrug\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_ODSNEAK ))
      strcat( buf, "&n  outdoor sneak\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_UDSNEAK ))
      strcat( buf, "&n  underdark sneak\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_STRENGTH ))
      strcat( buf, "&n  strength\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_FAERIE_FIRE ))
      strcat( buf, "&n  faerie fire\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_STEAL ))
      strcat( buf, "&n  steal\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_ENLARGE ))
      strcat( buf, "&n  enlarge\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_INVIS ))
      strcat( buf, "&n  invisibility\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_SUMMON_HOARDE ))
      strcat( buf, "&n  summon hoarde\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_SHIFT_PRIME ))
      strcat( buf, "&n  shift prime\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_SHIFT_ASTRAL ))
      strcat( buf, "&n  shift astral\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_LEVITATE ))
      strcat( buf, "&n  levitate\n\r" );
    if( IS_SET( race_table[ch->race].race_abilities, RACE_BITE ))
      strcat( buf, "&n  bite\n\r" );

    send_to_char( buf, ch );
    
    return;
}


/*
 * Returns a string with the end removed..
 */
char *rem_end( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
           int  i;

    for ( i = 0; str[i] != '\n'; i++ )
	strcap[i] = str[i];
    strcap[i] = '\0';
    return strcap;
}

/* Semi-infomative display for lvls 1-57. */
char * stat_string( int value )
{
         if( value > 100 ) return "quite excellent";
    else if( value > 90 ) return "excellent";
    else if( value > 80 ) return "very good";
    else if( value > 70 ) return "good";
    else if( value > 60 ) return "above average";
    else if( value > 50 ) return "average";
    else if( value > 40 ) return "below average";
    else if( value > 30 ) return "bad";
    else if( value > 20 ) return "very bad";
    else if( value > 10 ) return "awful";
    else if( value > 0  ) return "incredibly lame";
    else return "buggy";
}

char * bonus_string( int value )
{
         if( value > 50 ) return "buggy";
    else if( value > 30 ) return "phenomenal";
    else if( value > 23 ) return "awesome";
    else if( value > 16 ) return "excellent";
    else if( value > 10 ) return "very good";
    else if( value > 5  ) return "good";
    else if( value > 1  ) return "above average";
    else if( value > -2 ) return "average";
    else if( value > -6 ) return "below average";
    else if( value > -10 ) return "bad";
    else if( value > -15 ) return "horrible";
    else return "incredibly lame";
}

char * weight_string( CHAR_DATA *ch )
{
    int value = ( ch->carry_weight * 100 ) / can_carry_w( ch );

         if( value >  100 ) return "&+ROVERLOADED&n";
    else if( value == 100 ) return "&+YFully Loaded&n";
    else if( value >   90 ) return "&+YExtremely Heavy&n";
    else if( value >   80 ) return "&+rReally Heavy&n";
    else if( value >   70 ) return "&+mHeavy&n";
    else if( value >   60 ) return "&+mModerately Heavy&n";
    else if( value >   50 ) return "&+yModerate&n";
    else if( value >   40 ) return "&+bModerately Light&n";
    else if( value >   30 ) return "&+bLight&n";
    else if( value >   20 ) return "&+BReally Light&n";
    else if( value >   10 ) return "&+cExtremely Light&n";
    else if( value >   00 ) return "&+CAlmost Nothing&n";
    else if( value ==   0 ) return "&+WNothing&n";
    else return "&+Rbuggy - too light&n";
}

/*
 * Returns an int based on how well ch can see victim.
 */
int how_see ( CHAR_DATA *ch, CHAR_DATA *victim )
{
    // Victim no longer exists.
    if ( victim->deleted )
        return SEE_INVISIBLE;

    // Char almost dead, or asleep.
    if ( ch->position <= POS_SLEEPING )
        return SEE_INVISIBLE;

    /* All mobiles cannot see wizinvised immorts */
    if ( IS_NPC( ch )
	&& !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS ) )
        return SEE_INVISIBLE;

    // Handles Immortal Invis.
    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->level )
	return SEE_INVISIBLE;
 
    // Handles Immmortal sight.
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
 	return SEE_VISIBLE;

    // Handles blindness.
    if ( is_affected( ch, AFF_BLIND ) )
	return SEE_INVISIBLE;

    // Handles regular invisibility.
    if (( is_affected( victim, AFF_INVISIBLE ) || is_affected( victim, AFF_MINOR_INVIS ))) {
    
        if ( IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_INVIS )
	|| is_affected( ch, AFF_DETECT_INVIS ) 
        || ( is_affected( ch, AFF_ELEM_SIGHT) &&
          ( victim->race == RACE_ELEM_AIR || victim->race == RACE_ELEM_WATER
          || victim->race == RACE_ELEM_FIRE || victim->race == RACE_ELEM_EARTH) ) )
	return SEE_VISIBLE;
        else  if( is_affected( ch, AFF_SENSE_LIFE ) )
	    return SEE_SENSE_HIDDEN;
        else
            return SEE_INVISIBLE;
    }

    // Handles dark rooms. Jolle added ultracheck.
    if ( room_is_dark( victim->in_room ) )
    {
	if (IS_SET( race_table[ ch->race ].race_abilities, RACE_ULTRAVISION )
		|| IS_AFFECTED( ch, AFF_ULTRAVISION ) )
	    return SEE_VISIBLE;	

	if ( (IS_SET( race_table[ ch->race ].race_abilities, RACE_INFRAVISION )
		|| is_affected( ch, AFF_INFRARED )
		) && !IS_ROOM( victim->in_room, ROOM_UNDERWATER ) )
	    return SEE_SENSE_INFRA;
	if ( !(IS_SET( race_table[ ch->race ].race_abilities,
                       RACE_ULTRAVISION )
	       || is_affected( ch, AFF_ULTRAVISION ) ) )
	    return SEE_TOO_DARK;
    }

    // Handles hidden people.
    if ( is_affected( victim, AFF_HIDE ) )
    {
	if ( is_affected( ch, AFF_DETECT_HIDDEN ) )
	    return SEE_VISIBLE;
	if( IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_HIDDEN )
	    || is_affected( ch, AFF_SENSE_LIFE ) )
	    return SEE_SENSE_HIDDEN;
	else
	    return SEE_INVISIBLE;
    }

    return SEE_VISIBLE;
}

void do_search( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*to_room;
    CHAR_DATA           *vch;
    EXIT_DATA		*pexit_rev;
    EXIT_DATA 		*pexit;
    OBJ_DATA  		*obj;
    bool		 is_arg;
    int 		 door;
    int 		 chance;

    /* Calculate the probability that ch finds something. */
    chance = get_curr_int ( ch ) * 35 / 100;
    chance += get_curr_wis ( ch ) * 35 / 100;
    chance += get_curr_luk ( ch ) * 10 / 100;

    // Dwarves, being natural architects, can search better - Veygoth
    if( ch->race == RACE_DWARF )
       chance += 25;

    if ( chance > 95 )
	chance = 95;
    if ( chance < 10 )
	chance = 10;

    /* Searching with arguments. */
    if ( argument[0] !=  '\0' )
    {
	is_arg = TRUE;
	obj = get_obj_here ( ch, argument );
	if ( obj == NULL )
	{
	    send_to_char ( "You don't see that object here.\n\r", ch );
	    return;
	}
        switch ( obj->item_type )
	{
	    case TYPE_QUIVER:
	    case TYPE_CONTAINER:
	    case TYPE_CORPSE_NPC:
	    case TYPE_CORPSE_PC:
                if ( IS_SET(obj->value[1], CONT_CLOSED) ) {
                    act("$p&n is closed.", ch, obj, NULL, TO_CHAR);
                    return;
                }
		break;
	    default:
	    	send_to_char ( "That's not a container!\n\r", ch );
	    	return;
	}
    }
    else
    {
    	is_arg = FALSE;
	obj = NULL;
    }

    /* Lag ch from searching. */
    if ( ch->level < LEVEL_IMMORTAL )
    	WAIT_STATE ( ch, number_fuzzy( WAIT_SEARCH ) );

    if ( obj != NULL )
	obj = obj->contains;
    else
	obj = ch->in_room->contents;

    while ( obj != NULL )
    {
        if ( IS_OBJ_STAT( obj, ITEM_SECRET ) && chance > number_percent( )  
             && obj->fly_level == ch->fly_level)
        {
	    	REMOVE_OBJ_STAT ( obj, ITEM_SECRET );
	        act( "You find $p&n.",    ch, obj, NULL, TO_CHAR );
	    	act( "$n&n points out $p&n!", ch, obj, NULL, TO_ROOM );
                return;
        }
	obj = obj->next_content;
    }

    /* Look for a hidden exit. */
    for ( door = 0; door <= MAX_DIR; door++ )
    {
        /* If all exits have been searched and none are secret. */
	if ( door == MAX_DIR )
	{
	    break;
	}
        /* If there's a secret exit that leads to a room. */
	if ( ( pexit = ch->in_room->exit[door] )
	       && IS_SET( pexit->exit_info, EX_SECRET )
	       && pexit->to_room )
	    break;
    }

    if ( is_arg )
	door = MAX_DIR;
    if ( door < MAX_DIR )
    {
    	if ( pexit == NULL )
    	{
	    bug( "do_search: NULL exit found", 0 );
	    return;
    	}
    	if ( pexit->to_room == NULL )
    	{
	    bug( "do_search: exit to NULL room found", 0 );
	    return;
    	}
    	if ( number_percent () < chance )
    	{
            if (ch->fly_level > 0) {
                send_to_char("You're too high to be sure of what you saw.\n\r", ch);
                return;
            }
	    send_to_char ("You found a hidden exit!\n\r", ch);
	    act( "$n&n points out a secret exit!", ch, NULL, NULL, TO_ROOM );

	    /* Search out one side. */
	    REMOVE_BIT ( pexit->exit_info, EX_SECRET );
	    /* And the other side, if it exists. */
	    if (  ( to_room   = pexit->to_room 		     )
	       && ( pexit_rev = to_room->exit[rev_dir[door]] )
	       && ( pexit_rev->to_room == ch->in_room 	     ) )
	    	REMOVE_BIT ( pexit_rev->exit_info, EX_SECRET );
	    return;
    	}
    }


    if ( !is_arg )
    // Chance of finding people hiding in the room
    for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
       if( !is_affected( vch, AFF_HIDE ) || vch->fly_level != ch->fly_level )
         continue;
       if( number_percent() < chance )
       {
         affect_strip( vch, gsn_hide, 0, 0 );
         REMOVE_AFF_BIT( vch, AFF_HIDE );
         /* People without DI can't search out invis hiding pplz. */
         if ( can_see( ch, vch ) )
         {
             act( "$n&n points out $N&n lurking here!", ch, NULL, vch, TO_ROOM );
             act( "You point out $N&n lurking here!", ch, NULL, vch, TO_CHAR );
             act( "$n&n reveals your hiding spot!", ch, NULL, vch, TO_VICT );
             return;
         }
         else 
             SET_AFF_BIT( vch, AFF_HIDE );
       }
    }

    send_to_char ( "You find nothing new.\n\r", ch );
    return;
}

void show_aff_lines( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    if (is_affected( victim, AFF_GILLS ) ) // Govno
    {
        act( "&+c$E has little gills in $s neck.&n", ch, 0, victim, TO_CHAR );
    }
    if (is_affected(victim,AFF_SOULSHIELD) // Govno
        || has_affect( victim, 0, spl_soulshield, 0 ) ) // Lohrr
    {
       if ( victim->alignment > 0 )
       {
          act("&+W$E is surrounded by a holy aura.&n", ch, 0, victim, TO_CHAR);
       }
       else
       {
          act("&+r$E is surrounded by an unholy aura.&n", ch, 0, victim, TO_CHAR);
       }
    }
    if ( is_affected( victim, AFF_STONESKIN ) ) // Govno
    {
        act( "&+L$S skin appears to be made from stone.&n", ch, 0, victim, TO_CHAR );
    }
    if ( is_affected( victim, AFF_BARKSKIN ) ) // Govno
    {
        act( "&+y$S skin resembles the bark of a tree.&n", ch, 0, victim, TO_CHAR );
    }
    if ( is_affected( victim, AFF_MAJOR_GLOBE ) || \
         is_affected( victim, AFF_MINOR_GLOBE ) ) // Govno
    {
        act( "&+R$E is surrounded by a shimmering globe.&n", ch, 0, victim, TO_CHAR );
    }
    if ( is_affected( victim, AFF_GREATER_SPIRIT_WARD ) ) // Govno
    {
        act( "&+W$E is surrounded by a diffuse globe of light!&n", ch, 0, victim, TO_CHAR );
    }
    if ( is_affected( victim, AFF_COLDSHIELD ) ) //Govno
    {
        act( "&+B$E is surrounded by a killing frost!&n", ch, 0, victim, TO_CHAR );
    }
    if ( is_affected( victim, AFF_FLAMING) )
    {
	act( "&+r$E is surrounded by a flaming shield.&n", ch, 0, victim, TO_CHAR );
    }
    if (  victim->class == CLASS_MONK || victim->class == CLASS_MYSTIC ) {
        int stance = 0;
        if ( IS_IMMORTAL( ch ) || 
            ch->class == CLASS_MONK || ch->class == CLASS_MYSTIC ) {
            if ( !IS_NPC( victim ) ) stance = victim->pcdata->stance;
            if (stance ) {
                sprintf( buf, "&+y$E has assumed the %s stance.&n", monk_stances[stance].name );
            } else {
                sprintf( buf, "&+y$E stands at ease.&n" );
            }
            act( buf, ch, 0, victim, TO_CHAR );
        }
    }
    if ( is_affected( victim, AFF_BOUND ) ) {
        act( "&+y$E is bound and cannot move!&n", ch, 0, victim, TO_CHAR );
    }

}

// By veygoth.  Currently only lists known languages
// Can change current language too. - Lohrr
void do_speak( CHAR_DATA *ch, char *argument )
{
    int count;
    char buf[MAX_STRING_LENGTH];
    char arg1[ MAX_INPUT_LENGTH  ];

    if( IS_NPC( ch ))
      return;

    one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        for( count = 0; count < MAX_LANG; count++ )
        {
            if( count == LANG_UNKNOWN )
                continue;
            if( ch->pcdata->lang_lrn[count] > 20 )
            {
                if ( ch->pcdata->lang_lrn[count] < 40 )
                    sprintf( buf, "You have a basic understanding of %s.\n\r",
                        lang_table[count] );
                else if ( ch->pcdata->lang_lrn[count] < 60 )
                    sprintf( buf, "You comprehend %s.\n\r",
                        lang_table[count] );
                else if ( ch->pcdata->lang_lrn[count] < 90 )
                    sprintf( buf, "You are quite fluent in %s.\n\r",
                        lang_table[count] );
                else 
                    sprintf( buf, "You are a master of %s.\n\r", 
                        lang_table[count] );

                send_to_char( buf, ch );
            }
        }
        if( ch->pcdata->speaking == LANG_UNKNOWN )
            ch->pcdata->speaking = race_table[ch->race].language;
        sprintf( buf, "Currently speaking " );
        strcat( buf, lang_table[ch->pcdata->speaking] );
        strcat( buf, ".\n\r" );
        send_to_char( buf, ch );
    }
    else
    {
        int lang = int_lang( ch, arg1 );

        if ( !ch->pcdata )
            return;
        if ( can_speak( ch, lang ) )
        {
            static char buf [ MAX_STRING_LENGTH ];

            sprintf( buf, "You begin speaking %s.\n\r", lang_table[lang] );
            ch->pcdata->speaking = lang;
            send_to_char( buf, ch );
        }
        else
            send_to_char( "You don't know that language!\n\r", ch);
    }
}

char * skill_string( int value )
{
   if( value == 0 )
     return "unlearned";
   else if( value < 30 )
     return "just learning";
   else if( value < 38 )
     return "below average";
   else if( value < 46 )
     return "slightly below average";
   else if( value < 54 )
     return "average";
   else if( value < 62 )
     return "slightly above average";
   else if( value < 70 )
     return "above average";
   else if( value < 78 )
     return "good";
   else if ( value < 86 )
     return "very good";
   else if ( value < 94 )
     return "excellent";
   else if ( value < 96 )
     return "master";
   else
     return "grand master";

  // Players should never get to grand master skill level.
}

/* amount of spaces to put after class in att/score. */
char * class_space( int class )
{
    static char buf [ MAX_STRING_LENGTH ];
    int i;
    int len = 16 - strlen( class_table[class]->name ) / 2;

    buf[0] = '\0';
    for ( i = 0; i < len; i++ )
    	strcat( buf, " " );

    return "     ";
}

/* amount of spaces to put after race in att/score. */
char * race_space( int race )
{
    static char buf [ MAX_STRING_LENGTH ];
    int i;
    int len = 16 - strlen( race_table[race].name );

    buf[0] = '\0';
    for ( i = 0; i < len; i++ )
    	strcat( buf, " " );

    return buf;
}

// Innate for antipals and paladins.
void do_summon_mount ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    bool       is_anti;
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mount;
    AFFECT_DATA af;

    if ( ch->class == CLASS_ANTIPALADIN )
	is_anti = TRUE;
    else if ( ch->class == CLASS_PALADIN )
	is_anti = FALSE;
    else
    {
	send_to_char( "You scream and yell for a mount.\n\r", ch );
	return;
    }
    if ( is_affected( ch, AFF_SUMMON_MOUNT_TIMER) )
    {
	send_to_char("&nIt is too soon to accomplish that!\n\r", ch);
	return;
    }


    // Look to see if they already have a mount.
    for ( gch = char_list; gch; gch = gch->next )
    {
        if ( gch->master == ch 
          && IS_NPC( gch ) 
          && ( gch->pIndexData != NULL )
          && ( ( gch->pIndexData->vnum == 264 && is_anti ) 
             || ( gch->pIndexData->vnum == 265 && !is_anti ) ) )
        {
            send_to_char( "You already have a mount!\n\r", ch);
            return;
        }
    }

    // If not let them summon one. :)
    if ( is_anti )
        pMobIndex = get_mob_index( 264 );
    else
        pMobIndex = get_mob_index( 265 );

    if ( pMobIndex == NULL )
    {
        bug( "do_summon_mount: phukt pMobIndex!", 0);
        return;
    }

    mount = create_mobile( pMobIndex );

    /* Must simulate the poor horse running across the world.
     * They arrive with between 303 and 126 moves out of 294-310.
     */
    mount->move -= dice(2, 40) + dice(1, 80) + dice(4, 2);
    add_follower( mount, ch );
    SET_AFF_BIT( mount, AFF_CHARM );
    SET_BIT( mount->act, ACT_NOEXP );
    char_to_room( mount, ch->in_room);

    WAIT_STATE ( ch, number_fuzzy( skills_table[gsn_summon_mount].beats ));

    act( "$n&n trots up to you.", mount, NULL, ch, TO_VICT    );
    act( "$n&n trots up to $N&n.",  mount, NULL, ch, TO_NOTVICT );

    if( IS_IMMORTAL( ch ) )
       return;
    af.skill     = gsn_summon_mount;
    af.spell     = 0;
    af.song      = 0;
    af.duration  = 48;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SUMMON_MOUNT_TIMER);
    affect_to_char( ch, &af );

    return;
}

int int_lang( CHAR_DATA *ch, char *lang )
{

    if( IS_IMMORTAL(ch) && is_number(lang) )
    {
        int ilang = atoi(lang);
        if ( ilang >= 0 && ilang < MAX_LANG )
            return ilang;
        else
        {
            static char buf [ MAX_STRING_LENGTH ];

            sprintf( buf, "%d is not within valid language range(0 to %d)!\n\r",
                ilang, ( MAX_LANG - 1 ) );
            send_to_char( buf, ch );
            return -1;
        }
    }

    if( !str_prefix( lang, "unknown" ) )
        return LANG_UNKNOWN;
    if( !str_prefix( lang, "common" ) )
        return LANG_COMMON;
    if( !str_prefix( lang, "elven" ) )
        return LANG_ELVEN;
    if( !str_prefix( lang, "dwarven" ) )
        return LANG_DWARVEN;
    if( !str_prefix( lang, "centaur" ) )
        return LANG_CENTAUR;
    if( !str_prefix( lang, "ogre" ) )
        return LANG_OGRE;
    if( !str_prefix( lang, "orc" ) )
        return LANG_ORC;
    if( !str_prefix( lang, "troll" ) )
        return LANG_TROLL;
    if( !str_prefix( lang, "aquatic elf" ) )
        return LANG_AQUA_ELF;
    if( !str_prefix( lang, "saurial" ) )
        return LANG_SAURIAL;
    if( !str_prefix( lang, "thri-kreen" ) )
        return LANG_THRI_KREEN;
    if( !str_prefix( lang, "dragon" ) )
        return LANG_DRAGON;
    if( !str_prefix( lang, "magic" ) )
        return LANG_MAGICAL;
    if( !str_prefix( lang, "goblin" ) )
        return LANG_GOBLIN;
    if( !str_prefix( lang, "god" ) )
        return LANG_GOD;
    if( !str_prefix( lang, "halfling" ) )
        return LANG_HALFLING;
    if( !str_prefix( lang, "githyanki" ) )
        return LANG_GITHYANKI;
    if( !str_prefix( lang, "drow" ) )
        return LANG_DROW;
    if( !str_prefix( lang, "kobold" ) )
        return LANG_KOBOLD;
    if( !str_prefix( lang, "gnome" ) )
        return LANG_GNOME;
    if( !str_prefix( lang, "animal" ) )
        return LANG_ANIMAL;
    if( !str_prefix( lang, "duergar" ) )
        return LANG_DUERGAR;
    if( !str_prefix( lang, "githzerai" ) )
        return LANG_GITHZERAI;
    if( !str_prefix( lang, "flind" ) )
        return LANG_FLIND;
    if( !str_prefix( lang, "rakshasa" ) )
        return LANG_RAKSHASA;
    if( !str_prefix( lang, "minotaur" ) )
        return LANG_MINOTAUR;
    if( !str_prefix( lang, "gnoll" ) )
        return LANG_GNOLL;
    if( !str_prefix( lang, "barbarian" ) )
        return LANG_BARBARIAN;

    return -1;
}

bool can_speak( CHAR_DATA *ch, int lang )
{
    if ( lang < 0 || lang > MAX_LANG )
        return FALSE;
    if ( lang == LANG_UNKNOWN && !IS_IMMORTAL( ch ) && !IS_NPC(ch) )
        return FALSE;
    if ( IS_NPC( ch ) )
        return FALSE;
    if ( ch->pcdata->lang_lrn[lang] > 19 )
        return TRUE;
    return FALSE;
}

void do_justice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    if( !ch->in_room || !ch->in_room->area )
      return;

    if( ch->in_room->area->justice == 0 )
    {
      send_to_char( "&+BYou are not in an area controlled by justice.&n\n\r", ch );
      return;
    }
    else
    {
      send_to_char( "&+BYou are in an area controlled by justice.&n\n\r", ch );
    }
    sprintf(buf,  "&+B%s&n\n\r", get_invader_string( ch->in_room->area->justice ) );
    send_to_char( buf, ch );
    send_to_char( "\n\r&+BPossible crimes are:\n\r&n", ch );

    send_to_char( "&+bAttempted Murder&n\n\r", ch );
    send_to_char( "&+bTheft&n\n\r", ch );

    return;
}

bool is_same_room( CHAR_DATA *ach, CHAR_DATA *bch )
{
	if ( ach->deleted || bch->deleted ) return FALSE;
	if ( ach->in_room == bch->in_room && ach->fly_level == bch->fly_level)
		return TRUE;
	return FALSE;
}


void do_layhands( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char       arg [ MAX_INPUT_LENGTH ];

	if (ch->level <= LEVEL_IMMORTAL && ch->class != CLASS_PALADIN) {
		send_to_char( "&nYou aren't holy enough to do that!\n\r", ch );
		return;
	}

	if ( is_affected( ch, AFF_LAYHANDS_TIMER) ) {
		send_to_char("&nIt is too soon to accomplish that!\n\r", ch);
		return;
	}

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "&nLayhands on whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "&nThey're not here.\n\r", ch );
		return;
	}

	if (victim != ch && ch->fighting) {
		send_to_char( "&nYou can only layhands on yourself while fighting.\n\r", ch );
		return;
	}

    if( victim->hit < get_max_hit( victim ) )
        victim->hit = UMIN( victim->hit + 300, get_max_hit( victim) );
    update_pos( victim );

    if ( ch != victim ) {
	act( "You lay your hands upon $N.", ch, NULL, victim, TO_CHAR );
        act( "$n&n lays $s hands upon you.", ch, NULL, victim, TO_VICT );
        act( "$n&n lays $s hands upon $N&n.",  ch, NULL, victim, TO_NOTVICT );
    }
    else {
        send_to_char("&nYou layhands upon yourself.\n\r", ch);
        act( "$n&n lays hands upon $mself&n.",  ch, NULL, NULL, TO_ROOM ); 
    }
    send_to_char("&+WYou feel a warm glow!&n\n\r", victim );
    if (ch->level >= LEVEL_IMMORTAL)
       return;
    af.skill     = gsn_layhands_timer;
    af.spell     = 0;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_LAYHANDS_TIMER);
    affect_to_char( ch, &af );
}

/* A function to process a char string for money amounts and
   fill the associated coin data.  Acceptable input formats
   would be:
      all.coins
      10 platinum
      1 p
      1 p 5 g 2 gold all copper
      all g
      all copper

   If the word 'all' appears in the argument string then the
   function fills the COIN_DATA strusture with a -1 for that
   particular coin.

  -- Lithos 3/18/2000
*/
bool fill_coin_data( COIN_DATA *money, char *argument, COIN_DATA *src)
{
    char arg1[MAX_INPUT_LENGTH];
    bool coinnext = FALSE;
    int count = 0;

    money->copper = 0;
    money->silver = 0;
    money->gold = 0;
    money->platinum = 0;
    if (!argument) {
        bug("fill_money: NULL argument string\n", 0);
        return FALSE;
    }
    if (strstr(argument, "all.coin") ) {
        if ( !src) {
            money->copper = -1;
            money->silver = -1;
            money->gold = -1;
            money->platinum = -1;
        } else {
            money->copper = src->copper;
            money->silver = src->silver;
            money->gold = src->gold;
            money->platinum = src->platinum;
        }
        return TRUE;
    }
    while( argument ) {
        if(argument[0] == '\0') break;
        argument = one_argument(argument, arg1);
        if (is_number(arg1)) {
            //process int value
            count = atoi(arg1);
            coinnext = TRUE;
        }
        if (strcmp(arg1, "all") == 0) {
            count = -1;
            coinnext = TRUE;
        }
        if (!coinnext) 
            return FALSE; //choked on an argument
        argument = one_argument(argument, arg1);
        if (arg1[0] == '\0') break; //end of string
        coinnext = FALSE;
        //now process arg1 for coin type
        if (!str_prefix(arg1,"copper"))  {
            if (count == -1 && src)
                money->copper = src->copper;
            else
                money->copper = count;
            continue;
        }
        if ( !str_prefix(arg1,"silver")) {
            if (count == -1 && src)
                money->silver = src->silver;
            else 
                money->silver = count;
            continue;
        }
        if ( !str_prefix(arg1,"gold")) {
            if (count == -1 && src)
                money->gold = src->gold;
            else
                money->gold = count;
           continue;
        }
        if ( !str_prefix(arg1,"platinum") ) {
            if (count == -1 && src)
                money->platinum = src->platinum;
            else
                money->platinum = count;
            continue;
        }
    } //end while
    return TRUE;
}


char * wall_decay_msg(int vnum) {
  char *message;
  switch (vnum) {
      default: message = "$p&n vanishes."; break;
      case OBJ_VNUM_WALL_IRON: 
          message = "$p&n melts to &+Lslag&n and vanishes.";
          break;
      case OBJ_VNUM_WALL_STONE: 
          message = "$p&n crumbles to nothingness.";
          break;
      case OBJ_VNUM_WALL_FIRE: 
          message = "$p&n consumes itself with a blazing roar.";
          break;
      case OBJ_VNUM_WALL_ILLUSION: 
          message = "$p&n shudders into reality.";
          break;
      case OBJ_VNUM_WALL_FORCE: 
          message = "$p&n weakens away.";
          break;
      case OBJ_VNUM_LIGHTNING_CURTAIN: 
          message = "$p&n shorts itself to ground.";
          break;
      case OBJ_VNUM_WALL_SPARKS: 
          message = "$p&n cools and crumbles.";
          break;
      case OBJ_VNUM_WALL_MIST: 
          message = "$p&n disperses into a hazy cloud.";
          break;
      }
    return message;
}

bool has_skill( CHAR_DATA *ch, int sn ) {
    if ( ch->class == 0 )
        return FALSE;
    if ( ch->level < skills_table[sn].skill_level[ch->class] )
        return FALSE;
    return TRUE;
}

bool remove_blindness( CHAR_DATA *ch ) {
    AFFECT_DATA *paf;
    bool retval;
    
    retval = FALSE;
    if ( !ch ) return FALSE;
    if ( is_affected( ch, AFF_BLIND ) ) retval = TRUE;
    REMOVE_AFF_BIT( ch, AFF_BLIND );
    for( paf = ch->affected; paf; paf = paf->next) {
        if (get_bitvector( paf, AFF_BLIND) ) {
            // strip the affect
            affect_remove( ch, paf );
        }
    }
    return retval;
}

bool remove_poison( CHAR_DATA *ch ) {
    AFFECT_DATA *paf;
    bool retval;

    retval = FALSE;
    if ( !ch ) return FALSE;
    if ( is_affected( ch, AFF_POISON ) ) retval = TRUE;
    REMOVE_AFF_BIT( ch, AFF_POISON );
    for( paf = ch->affected; paf; paf = paf->next) {
        if (get_bitvector( paf, AFF_POISON) ) {
            // strip the affect
            affect_remove( ch, paf );
        }
    }
    return retval;
}

bool can_move( CHAR_DATA *ch ) {
    if ( is_affected( ch, AFF_HOLD ) )
        return FALSE;
    if ( is_affected( ch, AFF_MINOR_PARA ) )
        return FALSE;
    if ( is_affected( ch, AFF_BOUND ) )
        return FALSE;
    return TRUE;
}

bool has_free_will( CHAR_DATA *ch ) {
    if(  IS_SET( ch->act, ACT_PET) && ch->master
        && !ch->master->deleted )
        return FALSE;
    if ( has_affect(ch, 0, spl_charm_person, 0 ) )
        return FALSE;
    return TRUE;
}

void do_forage(CHAR_DATA *ch, char *argument) {
    int sector;
    int fvnum;
    OBJ_DATA *obj = NULL;
    OBJ_INDEX_DATA *pObjIndex;
    int chance;
    char buf[MAX_STRING_LENGTH];
    int flist[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 25};

    if (ch->fly_level) {
        send_to_char("Right, you're going to find something way up here.\n\r", ch);
        return;
    }
    chance = 20;
    if ( ch->class == CLASS_RANGER || ch->class == CLASS_HUNTER || IS_IMMORTAL(ch) )
        chance = 90;
    if ( chance < number_percent() ) {
        ch->wait += 10;
        return;
    }
    ch->wait += 15;
    sector = ch->in_room->sector_type;
    switch (sector) {
        default:
            send_to_char("Nothing edible could be growing here.\n\r", ch);
            return;
        case SECT_FIELD:
            flist[0] = 80;
            flist[1] = 84;
            flist[2] = 86;
            flist[6] = 91;
            flist[7] = 80;
            break;
        case SECT_HILLS:
            flist[0] = 82748;
            flist[1] = 86;
            flist[2] = 92;
            flist[6] = 94;
            break;
        case SECT_UNDERG_WILD:
            flist[0] = 3352;
            flist[5] = 3352;
            flist[6] = 7711;
            flist[1] = 85;
            flist[2] = 88;
            flist[7] = 82;
            flist[8] = 83;
            break;
        case SECT_SWAMP:
            flist[0] = 3352;
            flist[1] = 88;
            flist[5] = 94;
            flist[6] = 83;
            flist[7] = 89;
            break;
        case SECT_FOREST:
            flist[0] = 2057;
            flist[1] = 81651;
            flist[2] = 90;
            flist[3] = 93;
            flist[4] = 92;
            flist[5] = 90;
            flist[6] = 87;
            flist[7] = 84;
            break;
    } //end switch
    if (ch->class == CLASS_RANGER || ch->class == CLASS_HUNTER)
        fvnum = flist[number_range(0,9)];
    else
        fvnum = flist[number_range(0,4)];
    if ( !fvnum ) {
        send_to_char("You find nothing edible.\n\r", ch);
        return;
    }
     sprintf(buf, "forage: %s found vnum %d in room %d.", ch->name, fvnum, ch->in_room->vnum);
      wiznet( 0, WIZ_LSPAM, 0, buf);
/*
    if (fvnum == OBJ_VNUM_SPRING) {
        //don't allow endless springs
        for ( obj = ch->in_room->contents; obj; obj = obj->next_content ) {
            if (obj->pIndexData->vnum == fvnum) {
                act("You notice the $p&n.", ch, obj, NULL, TO_CHAR);
                return;
            }
        }
    }
*/
    pObjIndex = get_obj_index( fvnum );
    if ( !obj ) {
        bug ( "forage: invalid vnum", 0 );
        return;
    }
    obj = create_object( pObjIndex, 1 );
    obj_to_room( obj, ch->in_room );
    obj->fly_level = 0;
    if (fvnum == OBJ_VNUM_SPRING ) // give spring a timer;
        obj->timer = 10 + number_range(1,20);
    act("You find $p&n.", ch, obj, NULL, TO_CHAR);
    act("$n&n forages around and finds $p&n.", ch, obj, NULL, TO_ROOM);
    return;
}

bool can_fly( CHAR_DATA *ch ) {
    if ( is_affected( ch, AFF_FLYING )
        || has_affect( ch, 0, spl_fly, 0 )
        || has_affect( ch, 0, spl_ravenflight, 0 )
        || IS_SET( race_table[ ch->race ].race_abilities, RACE_FLY ) )
       return TRUE;
    return FALSE;
}

bool is_aggro( CHAR_DATA *ch, CHAR_DATA *victim )
{

    if (ch == victim ) return FALSE;
    if ( !str_infix( race_table[victim->race].name, race_table[ch->race].hate ) )
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGROGOOD) && IS_GOOD( victim )  )
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGROEVIL) && IS_EVIL(victim) )
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGRONEUT) && IS_NEUTRAL(victim) )
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGROEVILRACE)
        && race_table[victim->race].racewar_side == RACEWAR_EVIL)
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGROGOODRACE) 
        && race_table[victim->race].racewar_side == RACEWAR_GOOD)
        return TRUE;
    if ( IS_SET( ch->act, ACT_AGGRESSIVE )
        && !IS_SET( ch->act, ACT_AGGROGOOD)
        && !IS_SET( ch->act, ACT_AGGROEVIL)
        && !IS_SET( ch->act, ACT_AGGRONEUT)
        && !IS_SET( ch->act, ACT_AGGROGOODRACE)
        && !IS_SET( ch->act, ACT_AGGROEVILRACE) )

        return TRUE;

    return is_hating( ch, victim );
}

void do_aggressive( CHAR_DATA *ch, char *argument )
{
    int aggr;  
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) ) return;
    if ( ch->class == CLASS_PALADIN ) {
        send_to_char("Your beliefs prevent you from acting in such a manner.\n\r", ch);
        return;
    }
    argument = one_argument( argument, arg1 );
    if ( arg1[0] ) {
    if ( !str_infix( "off", arg1 ) || !str_infix( "none", arg1 ) )
    {
        ch->pcdata->aggr_level = -1;
    }
    else {
        aggr = atoi( arg1 );
        if (aggr < -1 || aggr > get_max_hit( ch ) ) {
            send_to_char("Value out of range.\n\r", ch);
            return;
        }
        else {
            ch->pcdata->aggr_level = aggr;
        }
    }
    } //end if (arg1[0])
    if (ch->pcdata->aggr_level == -1 )
         sprintf(buf, "You are not aggressive.\n\r");
    else
        sprintf(buf, "You are aggressive to creatures if hit points above %d.\n\r", ch->pcdata->aggr_level);
    send_to_char( buf, ch );
    return;
}

// This is a function that returns a characters *percieved* race, allowing those
// affected by change self, and possibly disguise/doppleganger to appear as a race
// other than their own without actually changing their race.
//
// -- Veygoth
int get_race( CHAR_DATA *ch )
{
   AFFECT_DATA * paf;

    last_fun("get_race");
   if( !is_affected( ch, AFF_CHANGE_SELF ))
     return ch->race;

   // Check for change self
   for ( paf = ch->affected; paf; paf = paf->next )
   {
     if( paf->spell == spl_change_self )
     {
       return paf->modifier;
     }
   }

   return ch->race;
}

void add_innate_timer ( CHAR_DATA *ch, int type, int timer ) {
    INNATE_TIMER_DATA *itd;

    last_fun( "add_innate_timer");
    if ( IS_NPC( ch ) ) return;
    if ( ch->deleted ) return;
    if ( has_innate_timer( ch, type ) ) return;
    itd = (INNATE_TIMER_DATA *) malloc( sizeof( INNATE_TIMER_DATA ) );
    if ( !itd ) {
        bug( "add_innate_timer: unable to allocate memory", 0);
        return;
    }
    itd->type = type;
    itd->timer = timer;
    itd->who = ch;
    itd->next = NULL;
    if (ch->pcdata->innate_timers) {
        itd->next = ch->pcdata->innate_timers;
    }
    ch->pcdata->innate_timers = itd;
}

void remove_innate_timer ( CHAR_DATA *ch , INNATE_TIMER_DATA *itd ) {
    INNATE_TIMER_DATA *prev, *next;

    last_fun("remove_innate_timer");
    if ( ch->pcdata->innate_timers == NULL ) return;
    if ( !itd ) return;
    if ( ch->pcdata->innate_timers == itd ) {
        ch->pcdata->innate_timers = itd->next;
        free( itd );
        return;
    }
    for ( prev = ch->pcdata->innate_timers; prev; prev = next ) {
        next = prev->next;
        if ( next == itd ) {
            prev->next = next->next;
            free( itd );
            return;
        }
    }
}


bool has_innate_timer ( CHAR_DATA *ch , int type ) {
    INNATE_TIMER_DATA *itd;

    last_fun( "has_innate_timer" );
    if ( IS_NPC( ch ) ) return FALSE;
    for ( itd = ch->pcdata->innate_timers; itd; itd = itd->next ) {
        if (itd->type == type) return TRUE;
    }
    return FALSE;
}

void update_innate_timers ( CHAR_DATA *ch ) {
    INNATE_TIMER_DATA *itd, *itd_next;

    last_fun( "update_innate_timers");
    if ( IS_NPC( ch ) ) return;

    for ( itd = ch->pcdata->innate_timers; itd; itd = itd_next ) {
        itd_next = itd->next;
        itd->timer--;
        if ( itd->timer <= 0 ) {
            remove_innate_timer( ch, itd );
        }
    }
    return;
}

void purge_innate_timers( CHAR_DATA *ch ) {
    INNATE_TIMER_DATA *itd;

    last_fun( "purge_innate_timers");
    if ( IS_NPC( ch ) ) return;
    itd = ch->pcdata->innate_timers;
    while ( itd ) {
        remove_innate_timer( ch, itd );
        itd = ch->pcdata->innate_timers;
    }
    return;
}

int get_orig_race( CHAR_DATA *ch ) {
    AFFECT_DATA *paf;
    int mod = 0;

    last_fun( "get_race" );
    if ( !ch ) return -1;
    for (paf = ch->affected; paf; paf = paf->next) {
        if ( paf->location == APPLY_RACE )
            mod += paf->modifier;
    }
    return ch->race - mod;
}

int get_curr_race( CHAR_DATA *ch ) {

    last_fun( "get_curr_race" );
    if ( !ch ) return -1;
    return ch->race;
}

/*
 * return a race index based on input type
 *
 * calls itself recursively if race value isn't permitted,
 * such as RACE_GOD.
 *
 */

int number_race( int type ) {
    int race;
    int animals[] = { race_worm, race_bird, race_herbivore, race_rat,
        race_fish, race_bat, race_snake, race_boar, race_bear, race_horse,
        race_primate, race_rat, race_worm, race_insect};

    last_fun( "number_race" );
    switch( type ) {
    default:
        bug("number_race: invalid type", 0);
        return -1;
    case RACE_RANGE_ANY:
        race = number_range( 0, MAX_RACE-1 );
        break;
    case RACE_RANGE_PLAYER:
        race = number_range( 0, MAX_PC_RACE - 1 );
        break;
    case RACE_RANGE_HUMANOID:
        race = number_range( race_human, MAX_COIN_RACE - 1);
        break;
    case RACE_RANGE_ANIMAL:
        race  = animals[number_range( 0, 13 )];
    } //end switch
    if ( race == RACE_GOD ) race = number_race( type );
    return race;
}

void do_dice( CHAR_DATA *ch, char *argument )
{
    int number, sides;
    char       	 arg1 	[ MAX_INPUT_LENGTH  ];
    char       	 arg2 	[ MAX_INPUT_LENGTH  ];
    char         buf[MAX_STRING_LENGTH];

    if( argument[0] == '\0' )
    {
      send_to_char( "You roll a zero-sided die.  Strangely enough you get no result.\n\r", ch );
      return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if( arg1[0] == '\0' )
    {
      send_to_char( "You roll a zero-sided die.  Strangely enough you get no result.\n\r", ch );
      return;
    }

    if( arg2[0] == '\0' )
    {
      send_to_char( "You roll a zero-sided die.  Strangely enough you get no result.\n\r", ch );
      return;
    }

    if( !is_number( arg1 ) || !is_number( arg2 ))
    {
      send_to_char( "Usage:  dice <number of dice> <number of sides>\n\r", ch );
      return;
    }

    number = atoi( arg1 );
    sides = atoi( arg2 );

    if ( number <= 0 || number > 1000 || sides <= 0 || sides > 1000 ) {
        send_to_char("C'mon! Gimme some numbers I can work with here.\n\r", ch );
        return;
    }
    if( number == 1 )
        sprintf( buf, "&+WWith stylish flair, $n&n&+W rolls a %d-sided die, resulting in the number %d&n", sides, dice( number, sides ) );
    else
        sprintf( buf, "&n&+WWith a wink, $n&n&+W rolls %d dice with %d sides, resulting in the number %d.&n", number, sides, dice( number, sides ) );
    act( buf, ch, NULL, NULL, TO_ALL );
}

/* This function fixes buggy stats.  It's called whenever someone enters
 *    the game or when an IMM "reset"s a PC. It was written by Lithos. - Lohrr
 */
void reset_stats( CHAR_DATA *ch ) {
    int pos, count;
    OBJ_DATA *obj;
    AFFECT_DATA *paf;

    ch->group_leader    = NULL;
    ch->next_in_group   = NULL;
    ch->hitroll         = 0;
    ch->damroll         = 0;
    ch->armor           = 100;
    ch->saving_throw[0] = 0;
    ch->saving_throw[1] = 0;
    ch->saving_throw[2] = 0;
    ch->saving_throw[3] = 0;
    ch->saving_throw[4] = 0;
    ch->mod_str = 0;
    ch->mod_int = 0;
    ch->mod_wis = 0;
    ch->mod_dex = 0;
    ch->mod_con = 0;
    ch->mod_agi = 0;
    ch->mod_cha = 0;
    ch->mod_pow = 0;
    ch->mod_luk = 0;
    ch->resistant   = 0;
    ch->immune      = 0;
    ch->susceptible = 0;
    if ( !IS_NPC( ch ) ) {
        ch->pcdata->maxmod_str = 0;
        ch->pcdata->maxmod_int = 0;
        ch->pcdata->maxmod_wis = 0;
        ch->pcdata->maxmod_dex = 0;
        ch->pcdata->maxmod_con = 0;
        ch->pcdata->maxmod_agi = 0;
        ch->pcdata->maxmod_cha = 0;
        ch->pcdata->maxmod_pow = 0;
        ch->pcdata->maxmod_luk = 0;
        ch->pcdata->mod_hit = 0;
    }
    for ( count = 0; count < NUM_AFFECT_VECTORS; count++ ) {
        ch->affected_by[count] = 0;
    }
    ch->carry_number = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE && !obj->deleted )
            ch->carry_number++;
    }
    for( pos = 0; pos < MAX_WEAR; pos++ ) {
        obj = get_eq_char( ch, pos );
        if ( !obj )
            continue;
        ch->armor -= apply_ac( obj, pos );
        for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
        {
            SET_BIT ( ch->affected_by[count], obj->affected_by[count] );
        }
        for ( paf = obj->pIndexData->affected; paf; paf = paf->next ) {
            for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
            {
                SET_BIT ( ch->affected_by[count], paf->bitvector[count] );
            }
            apply_affect_mods( ch, paf );
        }
        for ( paf = obj->affected; paf; paf = paf->next ) {
            for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
            {
                SET_BIT ( ch->affected_by[count], paf->bitvector[count] );
            }
            apply_affect_mods( ch, paf );
        }
    }
    for (paf = ch->affected; paf; paf = paf->next ) {
        apply_affect_mods( ch, paf );
    } //end for
}

void apply_affect_mods( CHAR_DATA *ch, AFFECT_DATA *paf ) {
    int mod;

    mod = paf->modifier;
    switch ( paf->location ) {
    default:
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
        ch->mod_con += mod;
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
//    case APPLY_SIZE:          ch->size                  += mod; break;
//    case APPLY_SEX:           ch->sex                   += mod; break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
//    case APPLY_MANA:          ch->max_mana              += mod; break;
    case APPLY_HIT:
      if(!IS_NPC( ch ))
      {
           ch->pcdata->mod_hit               += mod;
      }
      else
      {
           ch->max_hit += mod;
      }
      break;
//    case APPLY_MOVE:          ch->max_move              += mod; break;
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
            ch->pcdata->maxmod_con += mod;
         }
         else
         {
            ch->mod_con += mod;
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
    } //end switch
    return;
}

/*
 * A utility function to remove all affects and perm affects from a char
 *
 * Lithos
 */
void remove_affects( CHAR_DATA *ch, BITVECT_DATA bvect ) {
    AFFECT_DATA *paf, *paf_next;

    last_fun( "remove_affects" );
    REMOVE_AFF_BIT( ch, bvect );
    for( paf = ch->affected; paf; paf = paf_next ) {
        paf_next = paf->next;
        if ( paf->deleted )
            continue;
        if ( get_bitvector( paf, bvect ) )
            affect_remove( ch, paf );
    }
}
