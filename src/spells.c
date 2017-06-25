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

char lbuf[MAX_STRING_LENGTH];
OBJ_DATA * make_wall(int sn, int level, CHAR_DATA *ch, void *vo, int vnum);
void summon_elem(int sn, int level, CHAR_DATA *ch, int vnum);
void make_portal( int sn, int level, CHAR_DATA *ch, void *vo, int vnum );
int dragon_breath_damage( CHAR_DATA *ch, int percent );

/*
 * Spell functions.
 */

// Acid Blast - level 25 cap.  (2 * level)d3
// At level 25 or more: 50 minimum damage, 150 maximum, 100 average

void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_acid_blast");
    if( level > 25 ) level = 25;

    dam = dice( (2 * level), 3 );

    if ( saves_spell( level, victim, DAM_ACID ) )
       dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_ACID );

    return;
}

// Bird of Prey - level 25 cap.  (3/2 * level)d3

void spell_bird_prey( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_bird_of_prey");
    if( level > 25 )
        level = 25;

    dam = dice( ( 3 / 2 * level ), 3 );
    if ( saves_spell( level, victim, DAM_SLASH ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_SLASH );

    return;
}

// Bonethunder - 20-80 damage
// Item proc
// Maybe you should actually have it deal the damage to someone... -V

void spell_bonethunder( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_bonethunder");

    dam = dice( 1, 4 ) * 20;
    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_armor");
    if ( has_affect( victim, 0, sn, 0 ) ) {
        affect_refresh( victim, 0, sn, 0, 5 * level );
       return;
    }

    af = create_affect( 0, sn, 0, 5 * level, APPLY_AC, -20, AFF_NONE );
    affect_to_char( victim, &af );

    send_to_char( "You feel someone protecting you.\n\r", victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    
    return;
}


void spell_dazzle( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_dazzle");
    if ( has_affect( victim, 0, sn, 0 ) )
       return;

    af = create_affect( 0, sn, 0,
         level/10 + ((ch->level > 50) ? ch->level - 48 : 2), 0, 0, AFF_DAZZLE );
    affect_to_char( victim, &af );

    send_to_char( "&+cAn arc of &n&+Csilver &n&+Wlight &n&+ccoruscates between your fingertips.\n\r", victim );
    act( "&+cLights begin to coruscate about $n&+c and $m eyes take a &+Cbright &+csilver glow.&n",
            victim, NULL, NULL, TO_ROOM );


    return;
}

void spell_darkness( int sn, int level, CHAR_DATA *ch, void *vo )
{

    last_fun( "spell_darkness");
    
	//** Saeven - If the room is dark, it doesn't do anything
	
	if ( IS_ROOM( ch->in_room, ROOM_MAGICDARK) )
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    //** Saeven - If the room is bright, it goes neutral
	if ( IS_ROOM( ch->in_room, ROOM_MAGICLIGHT) )
    {
		REMOVE_ROOM_BIT( ch->in_room, ROOM_MAGICLIGHT );
		act( "&+WThe bright&nnes&+Ls fades.&n", ch, NULL, NULL, TO_ALL );
        return;
    }

	//** Saeven - Doesn't work in twilight areas

	if ( IS_ROOM( ch->in_room, ROOM_TWILIGHT) )
    {
		send_to_char( "&+LThe &n&+cdarkness&n&+L fades quicker than expected here.\n\r", ch );
	    return;
    }

	//** Saeven - If the room is neutral, it goes dark
    
    SET_ROOM_BIT( ch->in_room, ROOM_MAGICDARK );
    act( "&+LThe room goes dark!&n", ch, NULL, NULL, TO_ALL );

    //This lasts 12 seconds per level, max of 10 minutes at level 50

    create_event( EVENT_DARKNESS_END, (ch->level * 48), ch->in_room, NULL, 0 );
    return;
}

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_haste");
    // Removes slowness, takes two castings to make a slowed person hasted

    if( has_affect( victim, 0, spl_slowness, 0) || is_affected( victim, AFF_SLOWNESS ))
    {
        affect_strip( victim, 0, spl_slowness, 0);
        REMOVE_AFF_BIT( victim, AFF_SLOWNESS );
        return;
    }

    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_HASTE ))
       return;

    af = create_affect( 0, sn, 0, 6 + level/8, APPLY_AC, -10, AFF_HASTE );
    affect_to_char( victim, &af );

    act( "&+R$n&+R starts to move with uncanny speed!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+RYou feel yourself move much faster.&n\n\r", victim );
    return;
}

void spell_enlarge( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_enlarge");
    if( has_affect( victim, 0, spl_enlarge, 0))
    {
        send_to_char( "You failed!\n\r", ch );
        return;
    }
    if( has_affect( victim, 0, spl_reduce, 0))
    {
        affect_strip( victim, 0, spl_reduce, 0);
        act( "$n&n returns to $s normal size!&n", victim, NULL, NULL, TO_ROOM );
        send_to_char( "You return to your normal size!&n\n\r", victim );
        return;
    } 

    af = create_affect( 0, sn, 0, 12 + level/4, APPLY_SIZE, 1, AFF_NONE );
    affect_to_char( victim, &af );

    af.location  = APPLY_CON;
    af.modifier  = 8;
    affect_to_char( victim, &af );


    act( "$n&n grows to almost twice $s normal size!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You grow to almost twice your normal size!&n\n\r", victim );
    return;
}

void spell_reduce( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_reduce");
    if( has_affect( victim, 0, spl_reduce, 0))
    {
        send_to_char( "You failed!\n\r", ch );
        return;
    }
    if( has_affect( victim, 0, spl_enlarge, 0))
    {
        affect_strip( victim, 0, spl_enlarge, 0);
        act( "$n&n returns to $s normal size!&n", victim, NULL, NULL, TO_ROOM );
        send_to_char( "You return to your normal size!&n\n\r", victim );
        return;
    } 

    af = create_affect( 0, sn, 0, 6 + level/5, APPLY_SIZE, -1, AFF_NONE );
    affect_to_char( victim, &af );

    af.location  = APPLY_CON;
    af.modifier  = -4;
    affect_to_char( victim, &af );

    act( "$n&n shrinks to almost half $s normal size!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You shrink to almost half your normal size!&n\n\r", victim );
    return;
}

void spell_blur( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_blur");
    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_BLUR ))
       return;

    af = create_affect( 0, sn, 0, 6 + level/10, APPLY_NONE, 0, AFF_BLUR );
    set_bitvector( &af, AFF_BLUR );
    affect_to_char( victim, &af );

    act( "&+R$n&+R starts to move with a BLUR of speed!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+BYour skin crackles with lightning and you become a&n &+Wblur&n &+Bof motion.&n\n\r", victim );
    return;
}

void spell_slowness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_slowness");
    if (saves_spell( level, victim, DAM_OTHER ) )
    {
        send_to_char( "You failed!\n\r", ch );
        return;
    }

    // Removes haste, takes two castings to make a hasted person slowed

    if( has_affect( victim, 0, spl_haste, 0 ) || is_affected( victim, AFF_HASTE ))
    {
        affect_strip( victim, 0, spl_haste, 0 );
        REMOVE_AFF_BIT( victim, AFF_HASTE );
        return;
    }

    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_SLOWNESS ))
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level / 8;
    af.location  = APPLY_AC;
    af.modifier  = 10;
    set_bitvector( &af, AFF_SLOWNESS);
    affect_to_char( victim, &af );

    act( "&+R$n&+R moves much more slowly.&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+RYou feel yourself slowing down.&n\n\r", victim );

    return;
}

void spell_barkskin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_barkskin");
    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_BARKSKIN) )
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = UMIN ( UMAX( 15, level) , 50 );
    af.location  = APPLY_AC;
    af.modifier  = -10;
    set_bitvector( &af, AFF_BARKSKIN);
    affect_to_char( victim, &af );

    send_to_char( "&+yYour skin gains the texture and toughness of bark.&n\n\r", victim );
    
    return;
}

void spell_spirit_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_spirit_armor");
    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 18 + level;
    af.location  = APPLY_AC;
    af.modifier  = -10;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 12;
    affect_to_char( victim, &af );

    send_to_char( "&+mYou feel spirits protecting you.&n\n\r", victim );
    return;
}

void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_bless");
    if ( ch->position == POS_FIGHTING || has_affect( victim, 0, sn, 0 ) )
//    if ( has_affect( victim, 0, sn, 0 ) )
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 11 + 1;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    affect_to_char( victim, &af );

    send_to_char( "You feel righteous.\n\r", victim );
    return;
}

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_blindness");
    if ( is_affected( victim, AFF_BLIND ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
       send_to_char( "You failed to blind your target.\n\r", ch );
       return;
    }

    af.skill     = 0;
    af.spell      = sn;
    af.song      = 0;
    af.duration  = 1 + (level / 5);

    if ( IS_NPC( victim ) ) af.duration = 3 + level / 4;

    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    set_bitvector( &af, AFF_BLIND);
    affect_to_char( victim, &af );

    act( "$N&n is blinded!", ch, NULL, victim, TO_CHAR    );
    send_to_char( "You are blinded!\n\r", victim );
    act( "$N&n is blinded!", ch, NULL, victim, TO_NOTVICT );
    return;
}

void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_fear");
    if ( is_affected( victim, AFF_FEAR ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
       send_to_char( "You have failed.\n\r", ch );
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 3 + (level / 5);
    af.location  = 0;
    af.modifier  = 0;
    set_bitvector( &af, AFF_FEAR);
    affect_to_char( victim, &af );

    act( "$N&n is scared!", ch, NULL, victim, TO_CHAR    );
    send_to_char( "You are scared!\n\r", victim );
    act( "$N&n is scared!", ch, NULL, victim, TO_NOTVICT );

    do_flee( victim, "" );

    if( IS_NPC( victim ))
      start_fearing( victim, ch );

    return;
}

// Sunray - Level 45 cap.  (7 * level)d3
// In true druid fashion, it does less damage to neutrals.

void spell_sunray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_sunray");
    if (level > 45) level = 45;

    if ( is_affected( victim, AFF_BLIND ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
       spell_damage( ch, victim, dice( (3 * level), 3), sn, DAM_LIGHT );
    }
    else
    {
       af.skill     = 0;
       af.spell      = sn;
       af.song      = 0;
       af.duration  = 2 + (level / 5);

       if ( IS_NPC( victim ) ) af.duration = 4 + level / 4;

       af.location  = APPLY_HITROLL;
       af.modifier  = -4;
       set_bitvector( &af, AFF_BLIND);
       affect_to_char( victim, &af );

       act( "$N&n is blinded!", ch, NULL, victim, TO_CHAR    );
       send_to_char( "You are blinded!\n\r", victim );
       act( "$N&n is blinded!", ch, NULL, victim, TO_NOTVICT );

       if ( IS_UNDEAD( victim ))
       {
           spell_damage( ch, victim, dice( (8 * level), 3 ), sn, DAM_LIGHT );
       }
       else if( race_table[victim->race].racewar_side == RACEWAR_EVIL )
       {
           spell_damage( ch, victim, dice( (7 * level), 3 ), sn, DAM_LIGHT );
       }
       else if( race_table[victim->race].racewar_side == RACEWAR_GOOD )
       {
           spell_damage( ch, victim, dice( (6 * level), 3 ), sn, DAM_LIGHT );
       }
       else
       {
           spell_damage( ch, victim, dice( (4 * level), 3 ), sn, DAM_LIGHT );
       }

    }

    return;
}

// Sunburst - No level cap.  Area.  (3 * level)d3.  Chance for blind.
// Does less to neutrals.
// Does more to undead (3dlevel)d3 + 50.
// At level 50, damage vs. good/evil:  150-450 average 300.
// At level 50, damage vs. neutral:    100-300 average 200.
// At level 50, damage vs. undead:     200-500 average 350.
void spell_sunburst( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    CHAR_DATA  *victim_next;
    AFFECT_DATA af;

    last_fun( "spell_sunburst");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;

       if( victim->deleted || is_same_group( victim, ch ))
			continue;
       if ( victim->fly_level != ch->fly_level) continue;
 
       if ( is_affected( victim, AFF_BLIND ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
       {
           spell_damage( ch, victim, dice( (2 * level), 2), sn, DAM_LIGHT );
       }
       else
       {
           af.skill     = 0;
           af.spell      = sn;
           af.song      = 0;
           af.duration  = 2 + (level / 5);

           if ( IS_NPC( victim ) ) af.duration = 4 + level / 4;

           af.location  = APPLY_HITROLL;
           af.modifier  = -4;
           set_bitvector( &af, AFF_BLIND);
           affect_to_char( victim, &af );

           act( "$N&n is blinded!", ch, NULL, victim, TO_CHAR    );
           send_to_char( "You are blinded!\n\r", victim );
           act( "$N&n is blinded!", ch, NULL, victim, TO_NOTVICT );

           if ( IS_UNDEAD( victim ))
           {
               spell_damage( ch, victim, (dice( (3 * level), 3) + 50), sn, DAM_LIGHT );
           }
           else if( race_table[victim->race].racewar_side == RACEWAR_EVIL )
           {
               spell_damage( ch, victim, dice( (3 * level), 3), sn, DAM_LIGHT );
           }
           else if( race_table[victim->race].racewar_side == RACEWAR_GOOD )
           {
               spell_damage( ch, victim, dice( (3 * level), 3), sn, DAM_LIGHT );
           }
           else
           {
               spell_damage( ch, victim, dice( (2 * level), 3), sn, DAM_LIGHT );
           }
       }
    }
    return;
}

void spell_molevision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_molevision");
    if ( has_affect( victim, 0, sn, 0 ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
       send_to_char( "You have failed.\n\r", ch );
       return;
    }

    affect_strip( victim, 0, spl_hawk, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 3 + (level / 4);
    af.location  = APPLY_HITROLL;
    af.modifier  = 0 - (level / 11 + 3);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    act( "$N&n squints and blinks a few times.", ch, NULL, victim, TO_CHAR    );
    send_to_char( "Everything you see is suddenly blurry.\n\r", victim );
    act( "$N&n squints and blinks a few times.", ch, NULL, victim, TO_NOTVICT );

    return;
}

// Just the blind spell with a higher duration and harder to save against

void spell_power_word_blind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_power_word_blind");
    if ( is_affected( victim, AFF_BLIND ) || saves_spell( (level + 10), victim, DAM_BLACK_MANA ) )
    {
       send_to_char( "You have failed.\n\r", ch );
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + (level / 4);

    if ( IS_NPC( victim ) ) af.duration += 3;

    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    set_bitvector( &af, AFF_BLIND);
    affect_to_char( victim, &af );

    act( "$N&n is blinded by a word of power!", ch, NULL, victim, TO_CHAR    );
    send_to_char( "You are blinded by a word of power!\n\r", victim );
    act( "$N&n is blinded by a word of power!", ch, NULL, victim, TO_NOTVICT );

    return;
}

void spell_water_breathing( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_water_breathing");
    if ( is_affected( victim, AFF_GILLS ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_GILLS);
    affect_to_char( victim, &af );

    act( "$N grows small gills in his neck.", ch, NULL, victim, TO_CHAR );
    send_to_char( "You grow small gills in your neck and can now breathe underwater.\n\r", victim );
    act( "Small gills appear in the neck of $N.", ch, NULL, victim, TO_NOTVICT );

    return;
}

// Burning Hands - Level 20 cap.  4d3 + (level * 2)
// At level 20 or more: 44 minimum damage, 52 max, 48 average

void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_burning_hands");
    if( level > 20 ) level = 20;

    dam = dice( 4, 3 ) + (level * 2);

    if ( saves_spell( level, victim, DAM_FIRE ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Call Lightning - Level 30 cap.  (2 * level)d4

void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    int        dam;

    last_fun( "spell_call_lightning");
    if ( !IS_OUTSIDE( ch ) )
    {
       send_to_char( "You must be outdoors.\n\r", ch );
       return;
    }

    if( level > 30) level = 30;

    dam = dice( (2 * level), 3 );

    send_to_char( "&+YThe wrath of the storm strikes lighting upon your foes!&N\n\r", ch );
    act( "&+Y$n calls Nature's wrath as lightning strikes $s foes!&n", ch, NULL, NULL, TO_ROOM );

    if (ch->fly_level < MAX_FLY)
        act( "&+YA fierce lighning storm appears below you.&n",ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0 )
        act( "&+YA fierce lighning storm appears above you.&n",ch, NULL, NULL, TO_BELOW);

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
           continue;

        if ( vch->in_room == ch->in_room && vch->fly_level == ch->fly_level)
        {
           if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
           {
               spell_damage( ch, vch, saves_spell( level, vch, DAM_ELECTRICITY ) ? dam/2 : dam, sn, DAM_ELECTRICITY );
           }
           continue;
        }

        if ( vch->in_room->area == ch->in_room->area && IS_OUTSIDE( vch ) && IS_AWAKE( vch ) )
           send_to_char( "&+LA storm gathers, and &+Wlightning&+L crashes across the &N&+mhorizon&+L!&n\n\r", vch );
    }

    return;
}

// Cause Light - No level cap.  1d8 + (level/3)

void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_cause_light");
    dam = dice( 1, 8 ) + (level / 3);

    if ( saves_spell( level, victim, DAM_HARM ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Cause Critical - No level cap.  3d8 + level

void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_cause_critical");
    dam = dice( 3, 8 ) + level;

    if ( saves_spell( level, victim, DAM_HARM ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Cause Serious - No level cap.  2d8 + (level/2)

void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_cause_serious");
    dam = dice( 2, 8 ) + (level / 2);

    if ( saves_spell( level, victim, DAM_HARM ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Ravenous Vines - No level cap.  3d6 + (level/2)

void spell_ravenous_vines( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_ravenous_vines");
    dam = dice( 3, 6 ) + (level / 2);

    if ( saves_spell( level, victim, DAM_ASPHYXIATION ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_ASPHYXIATION );

    return;
}

void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_change_sex");
    if ( is_affected( victim, AFF_CHANGE_SEX ) )
    {
       act( "$E is already changed.", ch, NULL, victim, TO_CHAR );
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 * level;
    af.location  = APPLY_SEX;

    do
    {
       af.modifier  = number_range( 0, 2 ) - victim->sex;
    }
    while ( af.modifier == 0 );

    set_bitvector( &af, AFF_CHANGE_SEX);
    affect_to_char( victim, &af );

    send_to_char( "You feel strangely different.\n\r", victim );

    return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];

    last_fun( "spell_charm_person");

    if ( victim == ch )
    {
       send_to_char( "You like yourself even better!\n\r", ch );
       return;
    }

    if ( get_trust( ch ) > LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
       send_to_char( "There is no need for you to have a charmed mobile\n\r", ch );
       sprintf( buf, "%s charming", ch->name );
       log_string( buf );
       return;
    }

    if( is_affected( victim, AFF_CHARM ) || is_affected( ch, AFF_CHARM ) || level < victim->level || saves_spell( level, victim, DAM_CHARM ) )
       return;

    if ( victim->master )
       stop_follower( victim );

    add_follower( victim, ch );
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 3;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_CHARM);
    affect_to_char( victim, &af );

    if( victim->fighting == ch )
    {
      stop_fighting( ch, TRUE );
    }

    act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
    return;
}

// Chill Touch - Level 20 cap.  4d3 + (level * 3/2)
// -5 STR for 1 minute.
// At level 20 or more: 34 minimum damage, 42 max, 38 average

void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;
    AFFECT_DATA af;

    last_fun( "spell_chill_touch");
    if( level > 20 ) level = 20;

    dam = dice( 4, 3 ) + (level * 3 / 2);

    if ( !saves_spell( level, victim, DAM_COLD ) )
    {
       af.skill     = 0;
       af.spell     = sn;
       af.song      = 0;
       af.duration  = 2;
       af.location  = APPLY_STR;
       af.modifier  = 0 - 5;
       set_bitvector( &af, AFF_NONE);
       affect_join( victim, &af );
       send_to_char( "The chill seems to sap some of the strength from your body.\n\r", victim);
    }
    else
    {
        dam /= 2;
    }

    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

// Prismatic Orb - level 20 cap - 4d3 + (level * 2)
// At level 20 - Average 48, minimum 44, maximum 52

void spell_prismatic_orb( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim       = (CHAR_DATA *) vo;
    int         dam;

    last_fun( "spell_prismatic_orb");
    if( level > 20 ) level = 20;
    dam = dice( 4, 3 ) + (level * 2);

    if( disbelieve( level, victim, ch ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       spell_damage( ch, victim, ( dam / 2 ), sn, DAM_LIGHT );
    }
    else
    {
       spell_damage( ch, victim, dam, sn, DAM_LIGHT );
    }

    return;
}

// Shadow Magic - level 30 cap - (2 * level)d4 - 20
// At level 30 - Average 130, mimumum 40, maximum 130
// Disbelieve for 1/3rd damage
void spell_shadow_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim       = (CHAR_DATA *) vo;
    int         dam;

    last_fun( "spell_shadow_magic");
    if( level > 35 )
      level = 35;

    dam = dice( (2 * level), 4) - 20;

    if( disbelieve( level, victim, ch  ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       spell_damage( ch, victim, ( dam / 3 ), sn, DAM_LIGHT );
    }
    else
    {
       spell_damage( ch, victim, dam, sn, DAM_LIGHT );
    }

    return;
}

/*
 * Caps at level 45
 * level 45 damage: min 345, avg 660, max 975
 */
void spell_demi_shadow_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim       = (CHAR_DATA *) vo;
    int         dam;

    last_fun( "spell_demi_shadow_magic");
    if( level > 45 )
      level = 45;

    dam = dice( (level * 7), 3) + 30;

    if( disbelieve( level, victim, ch ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       spell_damage( ch, victim, ( dam / 3 ), sn, DAM_LIGHT );
    }
    else
    {
       spell_damage( ch, victim, dam, sn, DAM_LIGHT );
    }

    return;
}

// Color (not colour) Spray.  No cap.  (4*level)d2
// At level 50: 200 minimum, 400 maximum, 300 average

void spell_color_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_color_spray");
    dam = dice( (4 * level), 2 );

    if ( saves_spell( level, victim, DAM_LIGHT ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_LIGHT );

    return;
}

void spell_prismatic_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;
    int        color;

    last_fun( "spell_prismatic_spray");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
       if( victim->deleted || is_same_group( victim, ch ) )
          continue;
      if ( victim->fly_level != ch->fly_level ) continue;
      color = number_range( 1, 8 );

      switch( color )
      {
         case 1:
            // Yellow
            act( "$n&n is struck by a shaft of &+Yyellow&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+Yyellow&n light!", victim, NULL, NULL, TO_CHAR );
            dam = 300;
            break;
        case 2:
            // Orange
            act( "$n&n is struck by a shaft of &+yorange&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+yorange&n light!", victim, NULL, NULL, TO_CHAR );
            dam = 400;
            break;
        case 3:
            // Red
            act( "$n&n is struck by a shaft of &+rred&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+rred&n light!", victim, NULL, NULL, TO_CHAR );
            dam = 600;
            break;
        case 4:
            // Blue
            act( "$n&n is struck by a shaft of &+Bblue&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+Bblue&n light!", victim, NULL, NULL, TO_CHAR );
            spell_minor_para( spl_minor_para, level, ch, victim );
            continue;
            break;
        case 5:
            // Indigo
            act( "$n&n is struck by a shaft of &+bin&+mdi&+bgo&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+bin&+mdi&+bgo&n light!", victim, NULL, NULL, TO_CHAR );
            spell_feeblemind( spl_feeblemind, level, ch, victim );
            continue;
            break;
        case 6:
            // Green
            act( "$n&n is struck by a shaft of &+ggreen&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+ggreen&n light!", victim, NULL, NULL, TO_CHAR );
            spell_poison( spl_poison, level, ch, victim );
            continue;
            break;
        case 7:
            // Violet
            act( "$n&n is struck by a shaft of &+Mviolet&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+Mviolet&n light!", victim, NULL, NULL, TO_CHAR );
            spell_dispel_magic( spl_dispel_magic, level, ch, victim );
            continue;
            break;
        case 8:
            // Azure
            act( "$n&n is struck by a shaft of &+ba&+Bzu&+bre&n light!", victim, NULL, NULL, TO_ROOM );
            act( "You are struck by a shaft of &+ba&+Bzu&+bre&n light!", victim, NULL, NULL, TO_CHAR );
            spell_blindness( spl_blindness, level, ch, victim );
            continue;
            break;
      }

      if ( saves_spell( level, victim, DAM_LIGHT ) )
         dam /= 2;

      spell_damage( ch, victim, dam, sn, DAM_LIGHT );
    }

    return;
}

void spell_cone_of_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *pRoomIndex;

    last_fun( "spell_cone_of_silence");
    if ( !( pRoomIndex = ch->in_room ) )
        return;

    if ( IS_ROOM( pRoomIndex, ROOM_SAFE ) )
    {
    send_to_char( "You can't do that.\n\r", ch );
    return;
    }

    if ( !IS_ROOM( pRoomIndex, ROOM_SILENT ) )
    {
        SET_ROOM_BIT( pRoomIndex, ROOM_SILENT );
        send_to_char( "You have created a cone of silence!\n\r", ch );
        act( "$n has created a cone of silence!", ch, NULL, NULL, TO_ROOM );
    }

    return;
}

void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj;
    char *target_name = (char *) vo;

    last_fun( "spell_continual_light");
    if( target_name[0] == '\0' )
    {
      
		//**Saeven - If room is bright, nothing happens.
		if( IS_ROOM( ch->in_room, ROOM_MAGICLIGHT ))
		{
			send_to_char( "Nothing happens.\n\r", ch );
			return;
		}

		//**Saeven - If room is dark, it goes neutral.
		if ( IS_ROOM( ch->in_room, ROOM_MAGICDARK) )
		{
			REMOVE_ROOM_BIT( ch->in_room, ROOM_MAGICDARK );
			act( "&+LThe darkn&nes&+Ws lifts.&n", ch, NULL, NULL, TO_ALL );
			return;
		}

		//**Saeven - If room is twilight, doesn't work.
		if ( IS_ROOM( ch->in_room, ROOM_TWILIGHT) )
		{
			send_to_char( "&+cThe &+Clight&n&+c fades more quickly than expected here.\n\r", ch );
			return;
		}

		//**Saeven - If room is neutral, room goes bright.

			SET_ROOM_BIT( ch->in_room, ROOM_MAGICLIGHT );
			act( "&+WThe room brightens considerably!&n", ch, NULL, NULL, TO_ALL );


    // This lasts 12 seconds per level, max of 10 minutes at level 50

		create_event( EVENT_LIGHT_END, (ch->level * 48), ch->in_room, NULL, 0 );
		return;
	}

    else if( !(obj = get_obj_carry( ch, target_name )))
    {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
    }

    if( IS_OBJ_STAT( obj, ITEM_LIT ))
    {
      send_to_char( "Nothing happens.\n\r", ch );
      return;
    }

    act( "&+W$p&+W glows brightly.&n", ch, obj, NULL, TO_CHAR );
    act( "&+W$p&+W glows brightly.&n", ch, obj, NULL, TO_ROOM );
    SET_OBJ_STAT( obj, ITEM_LIT );

    ch->in_room->light++;

    return;

}

void spell_twilight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_twilight");
    if( IS_ROOM( ch->in_room, ROOM_TWILIGHT ) &&
        !IS_ROOM( ch->in_room, ROOM_MAGICDARK ) &&
        !IS_ROOM( ch->in_room, ROOM_MAGICLIGHT ))
    {
      send_to_char( "Nothing happens.\n\r", ch );
      return;
    }

    act( "&+bA gleam of &+Btwilight&n fills the room.", ch, NULL, NULL, TO_CHAR );
    act( "&+bA gleam of &+Btwilight&n fills the room.", ch, NULL, NULL, TO_ROOM );

    SET_ROOM_BIT( ch->in_room, ROOM_TWILIGHT );
    REMOVE_ROOM_BIT( ch->in_room, ROOM_MAGICLIGHT );
    REMOVE_ROOM_BIT( ch->in_room, ROOM_MAGICDARK );

    return;
}


void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo )
{
    char *target_name = (char *) vo;

    last_fun( "spell_control_weather");
    if ( !str_cmp( target_name, "better" ) )
       weather_info.change += dice( level / 3, 4 );
    else if ( !str_cmp( target_name, "worse" ) )
       weather_info.change -= dice( level / 3, 4 );
    else
       send_to_char ( "Do you want it to get better or worse?\n\r", ch );

    return;
}

void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;

    last_fun( "spell_create_food");
    mushroom = create_object( get_obj_index( OBJ_VNUM_IRON_RATION ), 0 );
    mushroom->value[0] = 5 + level;
    obj_to_room( mushroom, ch->in_room );
    mushroom->cost = 0;
    SET_OBJ_STAT( mushroom, ITEM_NOSELL );

    act( "$p&n suddenly appears.", ch, mushroom, NULL, TO_CHAR );
    act( "$p&n suddenly appears.", ch, mushroom, NULL, TO_ROOM );
    return;
}

void spell_celestial_sword( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *sword;

    last_fun( "spell_celestial_sword");
    sword = create_object( get_obj_index( OBJ_VNUM_CELESTIAL_SWORD ), 0 );
    obj_to_char( sword, ch );
    sword->cost = 0;
    sword->value[1] = (level - 2 )/10 + 1;
    SET_OBJ_STAT( sword, ITEM_NOSELL );
    sword->timer = ch->level + 5;

    act( "$p&n suddenly appears.", ch, sword, NULL, TO_CHAR );
    act( "$p&n suddenly appears.", ch, sword, NULL, TO_ROOM );
    return;
}

void spell_conjure_windsaber( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *sword;

    last_fun( "spell_conjure_windsaber");
    sword = create_object( get_obj_index( OBJ_VNUM_WINDSABER ), 0 );
    obj_to_char( sword, ch );
    sword->cost = 0;
    sword->value[1] = (level - 2 )/10 + 1;
    SET_OBJ_STAT( sword, ITEM_NOSELL );
    sword->timer = ch->level + 5;

    act( "$p&n suddenly appears.", ch, sword, NULL, TO_CHAR );
    act( "$p&n suddenly appears.", ch, sword, NULL, TO_ROOM );
    return;
}

void spell_create_buffet( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;
    int       num;

    last_fun( "spell_create_buffet");
    for ( num = 0; num < level / 5 + 1; num++ )
    {
    mushroom = create_object( get_obj_index( OBJ_VNUM_IRON_RATION ), 0 );
    mushroom->value[0] = 5 + level;
    obj_to_room( mushroom, ch->in_room );
        mushroom->cost = 0;
        SET_OBJ_STAT( mushroom, ITEM_NOSELL );

    act( "$p&n suddenly appears.", ch, mushroom, NULL, TO_CHAR );
    act( "$p&n suddenly appears.", ch, mushroom, NULL, TO_ROOM );
    }

    return;
}

void spell_create_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    last_fun( "spell_create_skin");
    spring = create_object( get_obj_index( OBJ_VNUM_WATERSKIN ), 0 );
    obj_to_room( spring, ch->in_room );

    act( "You create $p&n.", ch, spring, NULL, TO_CHAR );
    act( "$n&n creates $p&n.", ch, spring, NULL, TO_ROOM );
    spring->cost = 0;
    SET_OBJ_STAT( spring, ITEM_NOSELL );
    return;
}

void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    last_fun( "spell_create_spring");
    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
    obj_to_room( spring, ch->in_room );

    spring->timer = dice( 4, (ch->level / 2) );

    act( "You create $p&n.", ch, spring, NULL, TO_CHAR );
    act( "$n&n creates $p&n.", ch, spring, NULL, TO_ROOM );
    return;
}

void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj   = (OBJ_DATA *) vo;
    int       water;

    last_fun( "spell_create_water");
    if ( obj->item_type != TYPE_DRINK_CON )
    {
       send_to_char( "It is unable to hold water.\n\r", ch );
       return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
       send_to_char( "It contains some other liquid.\n\r", ch );
       return;
    }

    water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 ), obj->value[0] - obj->value[1] );
  
    if ( water > 0 )
    {
       obj->value[2] = LIQ_WATER;
       obj->value[1] += water;
       if ( !is_name( "water", obj->name ) )
       {
          char buf [ MAX_STRING_LENGTH ];

          sprintf( buf, "%s water", obj->name );
          free_string( obj->name );
          obj->name = str_dup( buf );
       }
    
    act( "$p is filled.", ch, obj, NULL, TO_CHAR );
    }

    return;
}

void spell_purify_spirit( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_purify_spirit");

    remove_blindness( victim );
    affect_strip( victim, 0, spl_malison, 0 );
    affect_strip( victim, 0, spl_curse, 0 );
    send_to_char( "A warm feeling runs through your body.\n\r", victim );
    act( "$N&n looks better.", ch, NULL, victim, TO_NOTVICT );

    return;
}

void spell_purify( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_purify");

    remove_blindness( victim );
    remove_poison( victim );
    send_to_char( "A warm feeling runs through your body.\n\r", victim );
    act( "$N&n looks better.", ch, NULL, victim, TO_NOTVICT );

    return;
}

void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_cure_blindness");
    if ( remove_blindness( victim ) )
        send_to_char( "&+WYour vision returns!&n\n\r", victim );
    return;
}

void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    last_fun( "spell_cure_critical");
    heal = dice( 2, 10 ) + 10 + level / 3;
    if( victim->hit < get_max_hit( victim ) )
      victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );
    update_pos( victim );

    gain_exp( ch, 2 );

    if ( ch != victim )
        act( "$N&n looks better.", ch, NULL, victim, TO_CHAR );

    send_to_char( "You feel much better!\n\r", victim );
    return;
}

void spell_mending( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    last_fun( "spell_mending");
    heal = dice( 9, 8 ) + level / 3;
    if( victim->hit < get_max_hit( victim ) )
       victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );

    update_pos( victim );

    gain_exp( ch, 2 );

    send_to_char( "You feel spirits tending your wounds.\n\r", victim );
    return;
}

void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    last_fun( "spell_cure_light");
    heal = dice( 1, 8 ) + level / 5;
    if( victim->hit < get_max_hit( victim ) )
       victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );

    update_pos( victim );

    send_to_char( "You feel a little better!\n\r", victim );
    return;
}

void spell_sustenance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        amount;

    last_fun( "spell_sustenance");
    if( IS_NPC( victim ))
    {
        send_to_char( "NPCs do not need to eat or drink.\n\r", ch );
        return;
    }

    amount = dice( 2, 8 ) + ch->level / 3;

    if( victim->pcdata->condition[COND_FULL] < amount )
        victim->pcdata->condition[COND_FULL] = amount;
    if( victim->pcdata->condition[COND_THIRST] < 8 )
        victim->pcdata->condition[COND_THIRST] = 8;  

    send_to_char( "You feel comfortably sated.\n\r", victim );
    return;
}

void spell_ls_nourishment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        amount;

    last_fun( "spell_ls_nourishment");
    if( IS_NPC( victim ))
    {
        send_to_char( "NPCs do not need to eat or drink.\n\r", ch );
        return;
    }

    amount = dice( 2, 4 ) + (ch->level / 10);

    if( victim->pcdata->condition[COND_FULL] < amount )
        victim->pcdata->condition[COND_FULL] = amount;
    if( victim->pcdata->condition[COND_THIRST] < 3 )
        victim->pcdata->condition[COND_THIRST] = 3;  

    send_to_char( "You feel partially sated.\n\r", victim );
    return;
}

void spell_gr_sustenance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA  *victim_next;
    int        amount;

    last_fun( "spell_gr_sustenance");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
       if( victim->deleted || IS_NPC( victim ))
           continue;
       if ( victim->fly_level != ch->fly_level) continue;

       amount = dice( 2, 11 ) + ch->level / 2;

       if( victim->pcdata->condition[COND_FULL] < amount )
           victim->pcdata->condition[COND_FULL] = amount;
       if( victim->pcdata->condition[COND_THIRST] < (amount * 2 / 3) )
           victim->pcdata->condition[COND_THIRST] = (amount * 2 / 3);  

       send_to_char( "You feel comfortably sated.\n\r", victim );
    }

    if ( ch != victim )
       send_to_char( "You sate the hunger of everyone in the room.\n\r", ch );
    return;
}

void spell_rain_maker( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA  *victim_next;
    int        amount;

    last_fun( "spell_rain_maker");
    if( IS_ROOM( ch->in_room, ROOM_NO_PRECIP ))
    {
       send_to_char( "You seem to be unable to conjure precipitation in this area.\n\r", ch );
       return;
    }

    send_to_char( "&+bYou conjure a refreshing rainstorm to sate everyone's thirst.&n\n\r", ch );
    act("$n&n conjures a refreshing rainstorm.&n", ch, NULL, NULL, TO_ROOM);

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
       if( victim->deleted || IS_NPC( victim ))
           continue;
       if (victim->fly_level != ch->fly_level)
           continue;

       amount = ch->level - 2;

       if( victim->pcdata->condition[COND_FULL] < 1 )
           victim->pcdata->condition[COND_FULL] = 1;
       if( victim->pcdata->condition[COND_THIRST] < amount )
           victim->pcdata->condition[COND_THIRST] = amount;  

       send_to_char( "You feel comfortably sated.\n\r", victim );
    }
    return;
}

void spell_remove_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_remove_poison");
    if ( remove_poison( victim ) ) {
        send_to_char( "A warm feeling runs through your body.\n\r", victim );    
        act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );
    }
    return;
}

void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    last_fun( "spell_cure_serious");
    heal = dice( 2, 9 ) + level / 4 ;
    if( victim->hit < get_max_hit( victim ) )
      victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );
    update_pos( victim );

    gain_exp( ch, 1 );

    send_to_char( "You feel better!\n\r", victim );
    return;
}

void spell_lesser_mending( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    last_fun( "spell_lesser_mending");
    heal = dice( 2, 9 ) + level / 4 ;
    if( victim->hit < get_max_hit( victim ) )
       victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );

    update_pos( victim );

    gain_exp( ch, 1 );

    send_to_char( "You feel better!\n\r", victim );

    return;
}

void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = NULL;
    OBJ_DATA *obj = NULL;
    AFFECT_DATA af;
    int chance;

    last_fun( "spell_curse");
    if ( ((OBJ_CHAR *)vo)->type == TYPE_OBJ_DATA )
    {
        obj = ((OBJ_CHAR *)vo)->pointer.obj;
        if (!obj) 
        {
            bug("spell_curse: NULL obj in *vo", 0);
            return;
        }
        if (obj->carried_by || obj->in_room != ch->in_room ) 
        {
            send_to_char("That isn't in the room!\n\r", ch );
            return;
        }
        if(    IS_OBJ_STAT( obj, ITEM_NODROP ) 
           &&  IS_OBJ_STAT( obj, ITEM_EVIL   ) 
           && !IS_OBJ_STAT( obj, ITEM_BLESS  ) )
        {
            send_to_char("It seems to have no effect.\n\r", ch);
            return;
        }

        REMOVE_OBJ_STAT( obj, ITEM_BLESS );
        SET_OBJ_STAT( obj, ITEM_NODROP );
        SET_OBJ_STAT( obj, ITEM_EVIL );
        act("$p shows a &+rred aura&n.", ch, obj, NULL, TO_CHAR);
        act("$p shows a &+rred aura&n.", ch, obj, NULL, TO_ROOM);
        return;
    }
    if ( ((OBJ_CHAR *)vo)->type == TYPE_CHAR_DATA )
    {
        victim = ((OBJ_CHAR *)vo)->pointer.ch;
    }
    if (victim == NULL)
    {
        bug("spell_curse: NULL ch in *vo", 0);
        return;
    }
    if ( ch->in_room != victim->in_room )
    {
        send_to_char("They aren't here anymore.\n\r", ch);
        return;
    }
    if ( is_affected( victim, AFF_CURSE ) )
    {
        act("$N is already cursed!", ch, NULL, victim, TO_CHAR);
        return;
    }

    if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
        /* have a chance of victim being aggro'd */
        chance = 2 * ch->level + get_curr_wis(ch) / 2 - victim->level;
        if (chance > 95 ) chance = 95;
        if ( number_percent() > chance ) {
            //yep its mad now
            set_fighting( victim, ch );
        }
        else
        {
            act("$N&n resists your spell.", ch, NULL, victim, TO_CHAR );
        }
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 2 * level;
    af.location  = APPLY_HITROLL;
    af.modifier  = - (ch->level / 15 + 1 );
    set_bitvector( &af, AFF_CURSE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 2;
    affect_to_char( victim, &af );

    af.location  = APPLY_CHA;
    af.modifier  = 0 - (ch->level / 3);
    affect_to_char( victim, &af );

    send_to_char( "You feel unclean.\n\r", victim );
    act("$n shows a &+rred glow&n.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_hex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_hex");
    if ( has_affect( victim, 0, sn, 0 ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
    send_to_char( "You have failed.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 2 * level;
    af.location  = APPLY_HITROLL;
    af.modifier  = - (ch->level / 7);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "A hex is cast upon you!\n\r", victim );
    return;
}

void spell_malison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_malison");
    if ( is_affected( victim, AFF_CURSE ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
    send_to_char( "You have failed.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 3;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1;
    set_bitvector( &af, AFF_CURSE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = (ch->level / 4 + 3);
    affect_to_char( victim, &af );

    af.location  = APPLY_CHA;
    af.modifier  = 0 - (ch->level / 3);
    affect_to_char( victim, &af );

    if ( ch != victim )
       act( "&+L$N&n briefly fades from sight.&n", ch, NULL, victim, TO_NOTVICT );

    send_to_char( "You suddenly feel more vulnerable.\n\r", victim );
    return;
}

void spell_destroy_cursed( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        yesno  = 0;

    last_fun( "spell_destroy_cursed");
    for ( obj = victim->carrying; obj; obj = obj_next )
    {
       obj_next = obj->next_content;

       if ( IS_OBJ_STAT( obj, ITEM_NODROP ) && obj->wear_loc == WEAR_NONE )
       {
          act( "You convulse as you toss $p to the ground, destroying it.", victim, obj, NULL, TO_CHAR );
          act( "$n convulses as $e tosses $p to the ground, destroying it.", victim, obj, NULL, TO_ROOM );

          if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_UNCURSE ))
          {
             setoff_trap( ch, obj );
             obj_from_room( obj );
             extract_obj( obj );
             if( ch->deleted || ch->position == POS_DEAD )
                return;
          }
          else
             extract_obj( obj );

          yesno = 1;
       }
    }

    if ( ch != victim && yesno )
       send_to_char( "Ok.\n\r", ch );

    return;
}

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_detect_evil");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 6+level);
        return;
    }

    af = create_affect( 0, sn, 0, 6 + level, APPLY_NONE, 0, AFF_DETECT_EVIL );
    affect_to_char( victim, &af );

    send_to_char( "&+rYour eyes tingle.\n\r", victim );
    return;
}

void spell_detect_undead( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_detect_undead");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 6+level);
        return;
    }

    af = create_affect( 0, sn, 0, 6 + level, APPLY_NONE, 0, AFF_DETECT_UNDEAD );
    affect_to_char( victim, &af );

    send_to_char( "&+WYour eyes tingle.\n\r", victim );
    return;
}

void spell_sense_life( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_sense_life");
    if ( is_affected( victim, AFF_SENSE_LIFE ) )
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SENSE_LIFE);
    affect_to_char( victim, &af );

    send_to_char( "Your awareness improves.\n\r", victim );
    return;
}

void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_detect_invis");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 6+level);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_INVIS);
    affect_to_char( victim, &af );

    send_to_char( "&+lYour eyes tingle.\n\r", victim );
    return;
}

void spell_elemental_sight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_elemental_sight");
    if (victim != ch ) {
        send_to_char("This spell can only be cast upon yourself.\n\r", ch);
        return;
    }

    if ( has_affect( victim, 0, spl_elemental_sight, 0 ) )
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_ELEM_SIGHT);
    affect_to_char( victim, &af );

    send_to_char( "&+cYour eyes tingle.&n\n\r", victim );
    return;
}

void spell_comp_lang( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_comp_lang");
    if ( is_affected( victim, AFF_COMP_LANG ) )
       return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_COMP_LANG);
    affect_to_char( victim, &af );

    send_to_char( "Your feel your understanding of the languages of Magma improve!\n\r", victim );
    return;
}

void spell_spirit_sight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_spirit_sight");
    if(  has_affect( victim, 0, sn, 0 ) ) {
        affect_refresh( victim, 0, sn, 0, 4 + level / 2);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_MAGIC);

    if( !is_affected( victim, AFF_DETECT_MAGIC ))
       affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_GOOD);

    if( !is_affected( victim, AFF_DETECT_GOOD ))
       affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_EVIL);

    if( !is_affected( victim, AFF_DETECT_EVIL ))
       affect_to_char( victim, &af );

    send_to_char( "Your sight deepens and you now sense auras.\n\r", victim );
    return;
}

void spell_analyze_balance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_analyze_balance");
    if( is_affected( victim, AFF_DETECT_GOOD ) &&
        is_affected( victim, AFF_DETECT_EVIL ))
           return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_EVIL);

    if( !is_affected( victim, AFF_DETECT_GOOD ))
       affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_GOOD);

    if( !is_affected( victim, AFF_DETECT_EVIL ))
       affect_to_char( victim, &af );

    send_to_char( "You feel more in tune with the balance of things.\n\r", victim );
    return;
}

void spell_gr_spirit_sight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_gr_spirit_sight");
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_MAGIC);

    if( !is_affected( victim, AFF_DETECT_MAGIC ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_GOOD);

    if( !is_affected( victim, AFF_DETECT_GOOD ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_EVIL);

    if( !is_affected( victim, AFF_DETECT_EVIL ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_INVIS);

    if( !is_affected( victim, AFF_DETECT_INVIS ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_SENSE_LIFE );

    if( !is_affected( victim, AFF_SENSE_LIFE ))
      affect_to_char( victim, &af );

    send_to_char( "Your sight deepens and you now sense auras and beings.\n\r", victim );
    return;
}

void spell_true_seeing( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_tru_seeng");
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_MAGIC);

    if( !is_affected( victim, AFF_DETECT_MAGIC ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_GOOD);

    if( !is_affected( victim, AFF_DETECT_GOOD ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_EVIL);

    if( !is_affected( victim, AFF_DETECT_EVIL ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_INVIS);

    if( !is_affected( victim, AFF_DETECT_INVIS ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_DETECT_HIDDEN );

    if( !is_affected( victim, AFF_DETECT_HIDDEN ))
      affect_to_char( victim, &af );

    send_to_char( "Your sight deepens and you now sense auras and beings.\n\r", victim );
    return;
}

void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_detect_magic");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 6+level);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + ( level * 3 ) / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_MAGIC);
    affect_to_char( victim, &af );

    send_to_char( "&+bYour eyes tingle.\n\r", victim );
    return;
}

void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    last_fun( "spell_detect_poison");
    if ( obj->item_type == TYPE_DRINK_CON || obj->item_type == TYPE_FOOD )
    {
    if ( obj->value[3] != 0 )
        send_to_char( "You smell poisonous fumes.\n\r", ch );
    else
        send_to_char( "It looks very delicious.\n\r", ch );
    }
    else
    {
    send_to_char( "It doesn't look poisoned.\n\r", ch );
    }

    return;
}

// Dispel Evil - level 30+ slows down.  (2 * level)d4
void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
  
    last_fun( "dispel_evil");
    if( level > 30 ) level -= (level - 30)/2;

    if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
    {
    send_to_char( "You are too EVIL to cast this.\n\r", ch );
    return;
    }
  
    if ( IS_GOOD( victim ) )
    {
    act( "$N is protected by $S goodness.", ch, NULL, victim, TO_ROOM );
    act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if ( IS_NEUTRAL( victim ) )
    {
    act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
    return;
    }

    dam = dice( (2 * level), 4 );
    if ( saves_spell( level, victim, DAM_WHITE_MANA ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_WHITE_MANA );

    return;
}

/* Dispel Magic recoded by Thelonius for EnvyMud */
void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA   *victim, *ch2;
    OBJ_DATA    *obj, *pwall;
    AFFECT_DATA *paf;
    EXIT_DATA   *pexit, *pexit2;
    ROOM_INDEX_DATA *proom;
    int         aff_list[50], flag_list[10];
    int         aff_count, flag_count, aff, paf_count;
    bool        is_imm = FALSE;
    bool        defensive = FALSE;
	const BITVECT_DATA* aff_array[] = { &AFF_AWARE, &AFF_BARKSKIN, &AFF_BLUR, 
        &AFF_COLDSHIELD, &AFF_DENY_EARTH, &AFF_DENY_AIR, &AFF_DENY_FIRE,
        &AFF_DENY_WATER, &AFF_DETECT_EVIL, &AFF_DETECT_GOOD, &AFF_DETECT_HIDDEN,
        &AFF_DETECT_INVIS, &AFF_DETECT_MAGIC, &AFF_FARSEE, &AFF_FLYING, &AFF_GILLS,
        &AFF_HASTE, &AFF_INFRARED, &AFF_LEVITATE, &AFF_MAJOR_GLOBE, &AFF_MINOR_GLOBE,
        &AFF_MINOR_INVIS, &AFF_PROTECT_ACID, &AFF_PROTECT_COLD, &AFF_PROTECT_EVIL,
        &AFF_PROTECT_FIRE, &AFF_PROTECT_GAS, &AFF_PROTECT_GOOD,
        &AFF_PROTECT_LIGHTNING, &AFF_SENSE_LIFE, &AFF_SLOW_POISON, &AFF_SLOWNESS,
        &AFF_SNEAK, &AFF_SOULSHIELD, &AFF_SPIRIT_WARD, &AFF_GREATER_SPIRIT_WARD,
		&AFF_STONESKIN, &AFF_ULTRAVISION, &AFF_INVISIBLE };
    const BITVECT_DATA* flag_array[] = 
	{ &ITEM_LEVITATES, &ITEM_NOSUMMON, &ITEM_NOSLEEP,
	&ITEM_GLOW, &ITEM_LIT, &ITEM_BLESS, &ITEM_HUM };
    int aff_size = 39, flag_size = 7;
    int i;


    last_fun( "spell_dispel_magic");
    if (ch->level >= LEVEL_IMMORTAL) is_imm = TRUE;
    if ( ((OBJ_CHAR *)vo)->type == TYPE_CHAR_DATA)
    {
        victim = ((OBJ_CHAR *)vo)->pointer.ch;
        obj = NULL;
    }
    else if ( ((OBJ_CHAR *)vo)->type == TYPE_OBJ_DATA )
    {
        victim = NULL;
        obj = ((OBJ_CHAR *)vo)->pointer.obj;
    }
    else
    {
        bug( "OBJ_CHAR bad type (%d)!", ((OBJ_CHAR *)vo)->type );
        send_to_char( "You lost your target.\n\r", ch );
        return;
    }

    if ( obj != NULL && ( obj->deleted 
           || ( obj->carried_by != ch && obj->in_room != ch->in_room ) ) )
    {
        send_to_char( "It seems to have gotten away.\n\r", ch );
        return;
    }
    else if ( victim != NULL && ( victim->deleted 
                                || ( victim->in_room != ch->in_room ) ) )
    {
        send_to_char( "Your prey seems to have gotten away.\n\r", ch );
        return;
    }

/* redundant code, checked for in cast() with TAR_OBJ_CHAR
    if ( arg2[0] == '\0' ) { //check for target in combat
        if (ch->fighting) {
            victim = ch->fighting;
        }
        else {
            send_to_char("Cast dispel magic on who or what?\n\r", ch);
            return;
        }
    }
*/

    if ( victim != NULL)  
    {
    if ( !IS_NPC(victim) ) {
        // does caster have consent?
        if ( victim->pcdata->consent == ch)
            defensive = TRUE;
    }
    else  {
        if ( is_affected( victim, AFF_CHARM ) && victim->master 
         && is_same_group( ch, victim->master )  ) 
            defensive = TRUE;
    }
    if ( defensive || is_imm  || victim == ch )
    {   /* Defensive spell - remove ALL effects */
    for ( paf = victim->affected; paf; paf = paf->next )
    {
        // cannot remove skill affects, just spell and song affects
        if ( paf->deleted )
            continue;
        if ( !paf->song && !paf->spell && !is_imm )
            continue;
        if ( paf->spell == spl_polymorph && !is_imm )
            continue;
        if ( paf->spell == gsn_vampiric_bite && !is_imm)
            continue;
        if ( paf->spell == spl_curse && !is_imm)
            continue;
        if ( paf->spell == spl_poison && !is_imm )
            continue;
        affect_remove( victim, paf );
    }

    if ( victim == ch )
    {
        act( "You have removed all magic effects from yourself.",
            ch, NULL, NULL, TO_CHAR );
        act( "$n has removed all magic effects from $mself.",
            ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        act( "You have removed all magic effects from $N&n.",
            ch, NULL, victim, TO_CHAR );
        act( "$n has removed all magic effects from you.",
            ch, NULL, victim, TO_VICT );
        act( "$n has removed all magic effects from $N&n.",
            ch, NULL, victim, TO_NOTVICT );
    }
    return;
    }
    else
    {   /* Offensive spell - enforced by kill_ch whether succeeds or fails */
        /* Immortals always cast it defensively */
   
    for ( paf = victim->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
            continue;
            if ( !paf->spell )
                continue;
        if ( !saves_spell( level, victim, DAM_OTHER ) )
        {
        send_to_char( spells_table[paf->spell].msg_off, victim );
        send_to_char( "\n\r", victim );
        act( "$n&n is no longer affected by '$t'.",
            victim, spells_table[paf->spell].name, NULL, TO_ROOM );
        affect_strip( victim, 0, paf->spell, 0 );
        break;
        }
    }
    /* ALWAYS give a shot at removing sanctuary */
    if ( is_affected( victim, AFF_SANCTUARY)
        && !saves_spell( level, victim, DAM_OTHER ) )
    {
        REMOVE_AFF_BIT( victim, AFF_SANCTUARY );
        send_to_char( "The white aura around your body fades.\n\r",
                                victim );
        act( "The white aura around $n's body fades.",
            victim, NULL, NULL, TO_ROOM );
    }
    if ( !victim->fighting || !is_same_group( ch, victim->fighting ) )
      kill_ch( victim, ch );
    }
    }
    else
    {
        if ( obj == NULL )
            obj = get_obj_carry( ch, vo );
        if ( obj == NULL )
            send_to_char( "Your prey has left.\n\r", ch );
        else
        { /* Dispel object here */
//            AFFECT_DATA    *paf;
//            AFFECT_DATA    *paf2;
        if (obj->item_type == TYPE_WALL) {
            if (number_percent() > get_curr_int(ch)/2 + ch->level - obj->value[2] ) return;
                pexit = ch->in_room->exit[obj->value[0]];
                proom = pexit->to_room;
                pexit2 = proom->exit[rev_dir[obj->value[0]]];
                if ( pexit2 ) {
                  for (pwall = proom->contents; pwall; pwall = pwall->next_content) {
                    if (pwall->deleted || pwall->item_type != TYPE_WALL) continue;
                    if (pwall->in_room != proom) continue;
                    if (proom->exit[pwall->value[0]] == pexit2) {
                        //Tada!
                        if((ch2 = proom->people) ) {
                            //let them know wall came down
                            act(wall_decay_msg(pwall->pIndexData->vnum), ch2, pwall, NULL, TO_ALL);
                        }
                        obj_from_room( pwall );
                        extract_obj(pwall);
                        REMOVE_BIT(pexit2->exit_info, EX_WALLED);
                        break;
                    }
                  }
                  //  bug("dispel magic: can't find matching wall", 0);
                }
              act(wall_decay_msg(obj->pIndexData->vnum), ch, obj, NULL, TO_ALL);
              REMOVE_BIT(pexit->exit_info, EX_WALLED);
              obj_from_room( obj );
              extract_obj( obj );
              return;
            }
            else
            {
                if ( insult_artifact( ch, obj ) )
                    return;
                // OK, now lets see about removing an affect or flag
                // disabled affects, seems to be buggy
                flag_count = 0;
                aff_count = 0;
                paf_count = 0;
                // count the number of affected_by flags 
                for( i = 0; i < aff_size; i++) {
                    if ( IS_OBJ_AFF( obj, *aff_array[i]) ) {
                        aff_list[aff_count] = i;
                        aff_count++;
                    }
                }
                //count the number of ITEM_ flags
                for(i=0; i < flag_size; i++) {
                    if ( IS_OBJ_STAT( obj, *flag_array[i]) ) {
                        flag_list[flag_count] = i;
                        flag_count++;
                    }
                }
                //count the number of removable affects
                for (paf = obj->affected; paf; paf = paf->next) {
                    if (paf->deleted) continue;
                    paf_count++;
                }
                if (!flag_count && !aff_count && !paf_count ) {
                   send_to_char("It seems to have no effect.\n\r", ch);
                   return;
                }
                aff = number_range(1, flag_count + aff_count + paf_count);
                if ( aff > flag_count + aff_count ) {
                    aff = aff - flag_count - aff_count - 1;
                    i = 0;
                    for (paf = obj->affected; paf; paf = paf->next) {
                        if (paf->deleted) continue;
                        if ( i == aff) {
                            affect_remove_obj( obj, paf );
                            break;
                        }
                        i++;
                    }
                }
                else if (aff <= aff_count) {
                    REMOVE_OBJ_AFF( obj, *aff_array[aff_list[aff-1]] );
                }
                else {
                    aff = aff - aff_count - paf_count - 1;
                    REMOVE_OBJ_STAT( obj, *flag_array[flag_list[aff]] );
                }
            act("$p&n's magical aura lessens slightly.", ch, obj, NULL, TO_ROOM);
            act("$p&n's magical aura lessens slightly.", ch, obj, NULL, TO_CHAR);
            }
        }
    }
    return;
}

void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    last_fun( "spell_earthquake");
    send_to_char( "&+yThe earth trembles beneath your feet!&n\n\r", ch );
    act( "$n&+y makes the earth tremble and shiver.&n", ch, NULL, NULL, TO_ROOM );

    // no messages to those flying in room above fly_level 0

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
           continue;
        if (vch->fly_level != ch->fly_level)
           continue;

        if ( vch->in_room == ch->in_room )
        {
           if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : !is_same_group( ch, vch ) ) )
           {
              if ( number_percent() > get_curr_agi( vch ) )
              {
                  vch->position = POS_SITTING;
                  send_to_char( "&+YYou crash to the ground!&n\n\r", vch );
              }

              spell_damage( ch, vch, level + dice( 2, 8 ), sn, DAM_CRUSHING );
           }

           continue;
        }

//        if ( vch->in_room->area == ch->in_room->area )
//           send_to_char( "&+yThe earth trembles and shivers.&n\n\r", vch );
    }

    return;
}

void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    last_fun( "spell_enchant_weapon");
    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_ENCHANT ))
    {
      setoff_trap( ch, obj );
      return;
    }

    if ( obj->item_type != TYPE_WEAPON
         || IS_OBJ_STAT( obj, ITEM_MAGIC )
         || obj->affected )
    {
       send_to_char( "That item cannot be enchanted.\n\r", ch );
       return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );

    paf            = new_affect();

    paf->skill          = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_HITROLL;
    paf->modifier    = level / 5;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;

    paf            = new_affect();

    paf->skill          = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_DAMROLL;
    paf->modifier    = level / 10;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;
    obj->level        = number_fuzzy( ch->level - 5 );

    if ( IS_GOOD( ch ) )
    {
    SET_OBJ_STAT( obj, ITEM_ANTI_EVIL);
    act( "$p&n glows blue.",   ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL( ch ) )
    {
    SET_OBJ_STAT( obj, ITEM_ANTI_GOOD );
    act( "$p&n glows red.",    ch, obj, NULL, TO_CHAR );
    }
    else
    {
    SET_OBJ_STAT( obj, ITEM_ANTI_EVIL );
    SET_OBJ_STAT( obj, ITEM_ANTI_GOOD );
    act( "$p&n glows yellow.", ch, obj, NULL, TO_CHAR );
    }

    return;
}


// Energy Drain.  Drains mana and HP.  Caster gain HP.
// No cap.  10d5 + level.
// Level 50: Minimum damage 60, maximum 100, average 80

void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_energy_drain");
    if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
    {
    send_to_char( "Nothing seems to happen.\n\r", ch );
    return;
    }

    ch->alignment = UMAX(-1000, ch->alignment - 75 );
    if ( victim->level <= 2 )
    {
    dam         = ch->hit + 1;
    }
    else
    {
        gain_exp( victim, 0 - number_range( level / 2, level ) );
        victim->mana = (victim->mana * 9) / 10;
        dam          = dice( 10, 5 ) + level;
        ch->hit     += (dam / 2);
    }

    spell_damage( ch, victim, dam, sn, DAM_BLACK_MANA );

    return;
}

// Fireball - level 40 cap.  (2*level)d8
// At level 40: 80 minimum damage, 640 maximum, 360 average

void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_fireball");
    if( level > 40 ) level = 40;

    dam = dice( (2 * level), 8 );

    if ( saves_spell( level, victim, DAM_FIRE ) )
      dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Bigby's Clenched Fist - Level 45 cap.  (11 * level)d3
// At level 45 or more: 495 minimum damage, 1485 maximum, 990 average
void spell_bigby_fist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_bigby_fist");
    if( level > 45 ) level = 45;

    dam = dice( (11 * level), 3 );

    if ( saves_spell( level, victim, DAM_CRUSHING ) )
        dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_CRUSHING );

    return;
}

// Akiaurn's Power Word - Level 51 cap.  (10 * level)d4
// At level 51 or more: 510 minimum, 2040 maximum, 1275 average
// Chance for stun or blind.

void spell_akiaurn_word( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int        dam;

    last_fun( "spell_akiaurn_word");
    if( level > 51 ) level = 51;

    dam = dice( (10 * level), 4 ) ;

    if ( saves_spell( level, victim, DAM_CRUSHING ) )
        dam /= 2;

    if( victim->hit < dam )
    {
       send_to_char( "&+WWith but a word, your soul is rended from your body.&N", victim );
       act( "$n&n&+W has had their soul rended from their body with but a word.&N", victim, NULL, NULL, TO_ROOM );
       spell_damage( ch, victim, 1000, 0, DAM_CRUSHING );
    }
    else
    {
       if ( is_affected( victim, AFF_BLIND ) || saves_spell( (level), victim,
          DAM_BLACK_MANA ) )
       {
          return;
       }

       af.skill     = 0;
       af.spell     = sn;
       af.song      = 0;
       af.duration  = 2 + level / 5;
       if ( IS_NPC( victim ) ) af.duration = 4 + level / 4;
       af.location  = APPLY_HITROLL;
       af.modifier  = -2;
       set_bitvector( &af, AFF_BLIND);
       affect_to_char( victim, &af );

       act( "$N&n is blinded by a word of power!", ch, NULL, victim, TO_CHAR    );
       send_to_char( "You are blinded by a word of power!\n\r", victim );
       act( "$N&n is blinded by a word of power!", ch, NULL, victim, TO_NOTVICT );

       if ( !saves_spell( level, victim, DAM_SOUND ) || victim->hit < 50 )
       {
          act( "You stun $N&n.", ch, NULL, victim, TO_CHAR );
          act( "You are stunned!", ch, NULL, victim, TO_VICT );
          act( "$N&n is stunned!", ch, NULL, victim, TO_NOTVICT );
          WAIT_STATE( victim, 40);
          stop_fighting( victim, FALSE );
       }

       spell_damage( ch, victim, dam, sn, DAM_CRUSHING );

       return;
    }

    return;
}

// Arieks Shattering Iceball - 9 * level d3 - level 45 cap
// At level 45: minimum 405, maximum 1215, average 810

void spell_iceball( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_iceball");
    dam = dice( (9 * level), 3);

    if ( saves_spell( level, victim, DAM_COLD ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

// Flamestrike - Level 40 cap.  (3 * level)d3

void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if( level > 40 ) level = 40;

    dam = dice( (3 * level), 3 );
    if ( saves_spell( level, victim, DAM_FIRE ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Chill of the Windsaber - no level cap - 200 + 1d100
// At all levels: 201 minimum, 300 maximum, 251 average
// Chance for minor paralysis

void spell_windsaber( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *fch;
    int        dam;
    AFFECT_DATA af;

    last_fun( "spell_windsaber");
    dam = 200 + dice( 1, 100 );
    if ( saves_spell( level, victim, DAM_COLD ) )
    {
        dam /= 2;
        spell_damage( ch, victim, dam, sn, DAM_COLD );
    }
    else
    {
      spell_damage( ch, victim, dam, sn, DAM_COLD );
      if( ch->position == POS_DEAD || ch->hit < -9 )
        return;
      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = level / 4;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_MINOR_PARA );
      affect_join( victim, &af );
      send_to_char( "You are paralyzed by the chill touch of the windsaber!\n\r", victim );
      act( "$n&n is paralyzed by the windsaber's icy force!", victim, NULL, NULL, TO_ROOM );
      if ( ch->fighting || victim->position == POS_FIGHTING )
         stop_fighting( victim, FALSE );
      for (fch = victim->in_room->people; fch; fch = fch->next_in_room ) {
        if ( fch->deleted || fch == victim ) continue;
        if ( fch->fighting == victim && !IS_SET( fch->act, PLR_VICIOUS) )
          stop_fighting( fch, FALSE );
      }
    }

    return;
}

// Soul Disturbance - 3/2 * level d3 - level cap 40
// At level 40: minimum 60, maximum 180, average 120

// Scathing Wind - 3 * level d3 - level cap 35 - area effect
// At level 35: minimum 135, maximum 405, average 270

void spell_scathing_wind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_scathing_wind");
    if( level > 45 ) level = 45;

    send_to_char( "You release a scathing wind from your fingertips.\n\r", ch );
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;

      dam = dice( (3 * level), 3);
      if ( saves_spell( level, victim, DAM_WIND ) )
          spell_damage( ch, victim, dam/2, sn, DAM_WIND );
      else
          spell_damage( ch, victim, dam, sn, DAM_WIND );

    }

    return;
}
// At level 20: 11 minimum,  56 maximum, 33.5 average
// At level 50: 21 minimum, 141 maximum, 81   average
void spell_tidal_wave( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_tidal_wave");
    send_to_char( "&+bYou conjure a gigantic wall of water that crashes violently into your foes!&n\n\r", ch );
    act("$n&n cackles gleefully as a &+Bgigantic wall of water&n comes crashing down.", ch, NULL, NULL, TO_ROOM);
    if (ch->fly_level < MAX_FLY)
      act("&nA &+Bgigantic wave&n drenches the room beneath you.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("A &+Bhuge wave of water&n crashes in above you, drenching everything.",ch, NULL, NULL, TO_BELOW);
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;

      dam = dice( 5, (level / 2) ) + ( level / 3 );
      if ( saves_spell( level, victim, DAM_DROWNING ) )
          spell_damage( ch, victim, dam/2, sn, DAM_DROWNING );
      else
      {
          if( victim->position > POS_SITTING )
              victim->position = POS_SITTING;
          spell_damage( ch, victim, dam, sn, DAM_DROWNING );
      }
    }

    return;
}

// Cyclone - Level 40 cap.  Area.

void spell_cyclone( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_cyclone");
    send_to_char( "&+CYou conjure a powerful cyclone!&N\n\r", ch );
    act("$n&n&+C conjures a powerful cyclone!&n", ch, NULL, NULL, TO_ROOM);
    if (ch->fly_level < MAX_FLY)
      act("&+CA powerful cyclone appears below you&n.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+CA powerful cyclone appears above you&n.", ch, NULL, NULL, TO_BELOW);
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (ch->fly_level != victim->fly_level)
          continue;

      // Spell_damage is used in each condition just because we want the message
      // to appear before the bash message

      if (level > 40) level = 40;

      dam = dice( level, 5 ) + 20;
      if ( saves_spell( level, victim, DAM_WIND ) )
      {   
         dam /= 2;
         spell_damage( ch, victim, dam, sn, DAM_WIND );
      }
      else if( victim->position > POS_SITTING && !saves_spell( level, victim, DAM_WIND ))
      {
         // if they fail two saves they get knocked over...
         send_to_char( "&+cYou are knocked down by the powerful winds!&N\n\r", victim );
         act( "$n&n&+c is knocked over by the powerful winds!&N", victim, NULL, NULL, TO_ROOM );
         victim->position = POS_SITTING;
         spell_damage( ch, victim, dam, sn, DAM_WIND );
         WAIT_STATE( victim, 4 );
      }
      else
      {
         spell_damage( ch, victim, dam, sn, DAM_WIND );
      }
    }

    return;
}

// Ice Storm - Level 35 cap.  (3 * level)d3.  Area effect.
// Level 35 or more: 105 minimum, 315 maximum, 210 average

void spell_ice_storm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_icestorm");
    if( level > 35 ) level = 35;

    send_to_char( "&+WYou call forth a rain of hail and snow!&n\n\r", ch );
    act("$n&n&+W calls forth a rain of hail and snow!&n", ch, NULL, NULL, TO_ROOM);
    if (ch->fly_level < MAX_FLY)
      act("&+WHail and snow starts pelting the area below you&n.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+WHail and snow come raining down from above you&n.", ch, NULL, NULL, TO_BELOW);
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (ch->fly_level != victim->fly_level)
        continue;

      dam = dice( (3 * level), 3 );
      if ( saves_spell( level, victim, DAM_COLD ) )
          spell_damage( ch, victim, dam/2, sn, DAM_COLD );
      else
          spell_damage( ch, victim, dam, sn, DAM_COLD );
    }

    return;
}

// Inferno - level 35 cap.  (3 * level)d3.  Area effect.
// Level 35 or more: 105 minimum, 315 maximum, 210 average

void spell_inferno( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_inferno");
    if( level > 35 ) level -= (level - 35)/2;

    send_to_char( "&+rYou call forth a raging inferno!&n\n\r", ch );
    act("$n&n&+r calls forth a raging inferno to engulf the area!&n", ch, NULL, NULL, TO_ROOM);
    if (ch->fly_level < MAX_FLY)
      act("&+rA roaring blaze appears below you!&n", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+rThe sky above you bursts into flames!&n", ch, NULL, NULL, TO_BELOW);

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;

      dam = dice( (3 * level), 3);
      if ( saves_spell( level, victim, DAM_FIRE ) )
          spell_damage( ch, victim, dam/2, sn, DAM_FIRE );
      else
          spell_damage( ch, victim, dam, sn, DAM_FIRE );
    }

    return;
}

//Light and Dark spell, level 35 cap, spell resembles Ariek's Iceball
//with lower cap.
//** Saeven											

void spell_lightanddark( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_lightanddark");
    if( level > 45 ) level = 45;

    if (ch->fly_level < MAX_FLY)
      act("&+W bright flash of light surges from below you!&n", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+WThe sky above glows with intermittent flashes of light!&n", ch, NULL, NULL, TO_BELOW);

    
      dam = dice( (7 * level), 3);
      if ( saves_spell( level, victim, DAM_HARM ) )
          spell_damage( ch, victim, dam/2, sn, DAM_HARM );
      else
          spell_damage( ch, victim, dam, sn, DAM_HARM );
    

    return;
}

//Proc spell for Trident of the Tides from WaterPlane, assassin 
//subclass stabber.
//** Saeven											

void spell_tridenttides( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_tridenttides");


      dam = dice( (4 * level), 3);
      if ( saves_spell( level, victim, DAM_HARM ) )
          spell_damage( ch, victim, dam/2, sn, DAM_HARM );
      else
          spell_damage( ch, victim, dam, sn, DAM_HARM );
    

    return;
}
// Firestorm - No level cap.  Leveld5.  Area.

void spell_firestorm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    int        dam;

    last_fun( "spell_firestorm");
    send_to_char( "&+rYou call forth a storm of fire that engulfs the area!&n\n\r", ch );
//    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    for( victim = char_list; victim; victim = victim->next )
    {
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch  || ch->in_room != victim->in_room )
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;
      dam = dice( level, 5) ;
      if ( saves_spell( level, victim, DAM_FIRE ) )
          spell_damage( ch, victim, dam/2, sn, DAM_FIRE );
      else
          spell_damage( ch, victim, dam, sn, DAM_FIRE );
    }

    return;
}

// Earthen Rain - 3 * level d3 + 2 * level - Level cap 50
// At level 50: minimum 250, maximum 550, average 400
// Earthquake style knockdown effect
void spell_earthen_rain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;
    last_fun( "spell_earthen_rain");

    if( level > 50 ) level = 50;

    if( !IS_OUTSIDE( ch ) || is_underground( ch ) )
    {
        send_to_char( "You failed!  Try casting this spell *outdoors* next time!\n\r", ch );
        return;
    }

    send_to_char( "You cause a rain of stones to fall from the sky.\n\r", ch );
    act( "$n calls forth a rain of stones to fall from the sky!", ch, NULL, NULL, TO_ROOM );

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;

      dam = dice( (3 * level), 3) + 2 * level;
      if ( number_percent() + ch->level > victim->level + get_curr_agi( victim ) )
      {   
         victim->position = POS_SITTING;
         act( "You are knocked to the ground by the falling stones!", ch, NULL, victim, TO_VICT );
         act( "$n&n falls to the ground!", victim, NULL, ch, TO_ROOM );
      }
      if ( saves_spell( level, victim, DAM_CRUSHING ) )
          spell_damage( ch, victim, dam/2, sn, DAM_CRUSHING );
      else
          spell_damage( ch, victim, dam, sn, DAM_CRUSHING );

    }

    return;
}

// Flameburst - 9dlevel - level cap 10
// At level 10: minimum 9, maximum 90, average 49.5

void spell_flameburst( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_flameburst");
    if( level > 10 ) level = 10;

    dam = dice( 9, level);
    if ( saves_spell( level, victim, DAM_FIRE ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Scalding Blast - 3/2 * level d3 - level cap 25
// At level 25: minimum 37.5, maximum 112.5, average 75
//
// Since flameburst does more damage when scalding blast is first gotten,
// I've compressed the curve to max out at level 20 -- Veygoth
// This gives at level 11: 19-57, average 38
// and at level        20: 38-114, average 76
// This still is not as good or better than flameburst until level 14-15,
// which does seem about right.
void spell_scalding_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_scalding_blast");
    /* Note here: Water elementalists get this at 5th, not 3rd circle. */
    if( level > 20 && ch->class != CLASS_ELEMENT_WATER ) level = 20;
    /* Level decreases 1 for every 2 lvls above 30
     * (i.e. 32 -> 31, 50 -> 40, etc ).  Note this is for Water Elem's
     * only, as others have max at 20. - Lohrr
     */
    if( level > 30 ) level -= (level - 30 ) / 2;

    dam = dice( ( ((3 * level) / 2) + 8), 3 );
    if ( saves_spell( level, victim, DAM_FIRE ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Molten Spray - 2 * level d4 - level 35 cap
// At level 35: minimum 70, maximum 280, average 175

void spell_molten_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_molten_spray");
    if( level > 35 ) level = 35;

    dam = dice( (2 * level), 4);
    if ( saves_spell( level, victim, DAM_FIRE ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Scorching Touch - 3/2 * level d4 - level cap 30
// At level 30: minimum 45, maximum 180, average 112.5

void spell_scorching_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_scorching_touch");
    if( level > 30 ) level = 35;

    dam = dice( (3 / 2 * level), 4 );
       if ( saves_spell( level, victim, DAM_FIRE ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Soul Disturbance - 3/2 * level d3 - level cap 40
// At level 40: minimum 60, maximum 180, average 120

// NOTE: We probably want to put in a check for mob types
// with "souls", and not plants, inanimate objects, etc.
// This is not done yet.

void spell_soul_dist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_soul_disturbance");
    if( level > 40 ) level = 40;

    dam = dice( (3 / 2 * level), 3 );
    if ( saves_spell( level, victim, DAM_HARM ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Greater Soul Disturbance - 5/2 * level d5 - level cap 40
// At level 40: minimum 100, maximum 500, average 300

// NOTE: We probably want to put in a check for mob types
// with "souls", and not plants, inanimate objects, etc.
// This is not done yet.

void spell_gr_soul_dist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_gr_soul_dist");
    if( level > 40 ) level = 40;

    dam = dice( ( 5 / 2 * level), 5);
    if ( saves_spell( level, victim, DAM_HARM ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Spirit Anguish - 2 * level d5 - level cap 40
// At level 40: minimum 80, maxmium 400, average 240

// Should probably be modified in the future to only work on creatures
// with souls (no plants, inanimate objects, etc).

void spell_spirit_anguish( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_spirit_anguish");
    if( level > 40 ) level = 40;

    dam = dice( (2 * level), 5);
    if ( saves_spell( level, victim, DAM_HARM ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int         effect;

    last_fun( "spell_faerie_fire");
    if ( is_affected( victim, AFF_FAERIE_FIRE ) )
    return;

    effect       = UMIN( 2 * level, 35 );
    if ( saves_spell( level, victim, DAM_OTHER ) )
        effect  /= 2;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_AC;
    af.modifier  = effect;
    set_bitvector( &af, AFF_FAERIE_FIRE);

    affect_to_char( victim, &af );

    send_to_char( "&+mYo&+Mu a&N&+mre su&+Mrr&n&+mou&+Mnde&N&+md by a p&+Min&N&+mk out&+Mli&N&+mne.&N\n\r", victim );
    act( "$n&n &+mis &+Msur&n&+mr&+Mo&N&+mun&+Mde&N&+md by a &+Mpink &N&+mout&+Mli&N&+mne.&N", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_vitality( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int duration;

    last_fun( "spell_vitality");
    duration = 2 + (level / 3);
    if (level > 50)
        duration += (level - 49) * 3;
    if ( has_affect( victim, 0, sn, 0 ) )
    {
        affect_refresh( victim, 0, sn, 0, duration );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = duration;
    af.location  = APPLY_HIT;
    af.modifier  = 4 * level;
    set_bitvector( &af, AFF_NONE);

    affect_to_char( victim, &af );

    update_pos( victim );

    send_to_char( "&+BYou feel vitalized!&N\n\r", victim );
    if (victim != ch) send_to_char( "&+bYou vitalize them.&N\n\r", ch );
    return;
}

// Should not be cumulative with pantherspeed.
void spell_wolfspeed( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_wolfspeed");
    // 1.5% chance per level of the caster to remove snailspeed.
    if( number_percent() < ( level * 3 / 2 ) )
      affect_strip( victim, 0, spl_snail, 0);

    if ( has_affect( victim, 0, sn, 0 ) || has_affect( victim, 0, spl_panther, 0 ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + (level / 3);
    af.location  = APPLY_MOVE;
    af.modifier  = dice( 2, 11 );
    set_bitvector( &af, AFF_NONE);

    affect_to_char( victim, &af );

    send_to_char( "&+yYou feel the &+wsp&+Wir&N&+wit &+yof the &+Lwolf&N&+y enter your body.&n\n\r", victim );
    send_to_char( "&+yYou spread the &N&+wsp&+Wir&N&+wit &n&+yof the &+Lwolf&N&+y to another.&N\n\r", ch );
    return;
}

// Should not be cumulative with wolfspeed
void spell_pantherspeed( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_pantherspeed");
    // Always cures snailspeed.
    affect_strip( victim, 0, spl_snail, 0);

    if ( has_affect( victim, 0, sn, 0 ) || has_affect( victim, 0, spl_wolf, 0 ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + (level / 3);
    af.location  = APPLY_MOVE;
    af.modifier  = dice( 7, 13 );
    set_bitvector( &af, AFF_NONE);

    affect_to_char( victim, &af );

    send_to_char( "&+yYou feel the spirit of the panther enter your body.&n\n\r", victim );
    return;
}

// Always Removes panterhspeed and wolfspeed
void spell_snailspeed( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int amount;

    last_fun( "spell_snailspeed");
    affect_strip( victim, 0, spl_wolf, 0);
    affect_strip( victim, 0, spl_panther, 0);

    if ( has_affect( victim, 0, sn, 0 ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + (level / 4);
    af.location  = APPLY_MOVE;
    amount       = dice( 6, 12 );
    if( amount >= victim->max_move )
        amount = victim->max_move - 1;
    af.modifier  = 0 - amount;
    set_bitvector( &af, AFF_NONE);

    affect_to_char( victim, &af );

    send_to_char( "&+yYou feel the spirit of the snail enter your body.&n\n\r", victim );
    return;
}

void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *ich;

    last_fun( "spell_faerie_fog");
    send_to_char( "You conjure a cloud of purple smoke.\n\r", ch );
    act( "$n&n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );

    for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
    {
    if ( !IS_NPC( ich ) && IS_SET( ich->act, PLR_WIZINVIS ) )
        continue;
      if (ich->deleted)
          continue;
      if (ich->fly_level != ch->fly_level)
          continue;

    if ( ich == ch || saves_spell( level, ich, DAM_OTHER ) )
        continue;

    affect_strip ( ich, 0, spl_invis, 0        );
    affect_strip ( ich, 0, spl_mass_invis, 0    );
    affect_strip ( ich, gsn_sneak, 0, 0        );
    affect_strip ( ich, gsn_shadow, 0, 0        );
    REMOVE_AFF_BIT   ( ich, AFF_HIDE    );
    REMOVE_AFF_BIT   ( ich, AFF_INVISIBLE    );

    act( "$n is revealed!", ich, NULL, NULL, TO_ROOM );
    send_to_char( "You are revealed!\n\r", ich );
    }

    return;
}

void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int duration;

    last_fun( "spell_fly");
    duration = 10 + 2 * ch->level;
    if (ch->level > 50)
        duration += 10 * (ch->level - 49);
    if ( has_affect( victim, 0, sn, 0 ) ) {
        affect_refresh( victim, 0, sn, 0, duration );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = duration;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_FLYING);
    affect_to_char( victim, &af );

    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_ravenflight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_ravenflight");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, level - 5 );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = (level - 5);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_FLYING);
    affect_to_char( victim, &af );

    send_to_char( "&+LYou begin to fly like the raven.&n\n\r", victim );
    act( "$n&n rises off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_gr_ravenflight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    CHAR_DATA  *victim_next;
    AFFECT_DATA af;

    last_fun( "spell_gr_ravenflight");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if (victim->deleted || !is_same_group( victim, ch ))
          continue;
      if (victim->fly_level != ch->fly_level )
          continue;
      if ( has_affect( victim, 0, spl_ravenflight, 0 ) ) {
          affect_refresh( victim, 0, spl_ravenflight, 0, level - 3 );
          continue;
      }

      af.skill     = 0;
      af.spell     = sn;
    af.song      = 0;
      af.duration  = (level - 1);
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_FLYING);
      affect_to_char( victim, &af );

      send_to_char( "&+LYour begin to fly like the raven.&n\n\r", victim );
      act( "$n&n rises off the ground.", victim, NULL, NULL, TO_ROOM );
    }
    return;
}

void spell_reveal_true_form( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    CHAR_DATA  *victim_next;

    last_fun( "spell_reveal_true_form");
    act( "$n&n calls on the spirits to reveal all presences.", ch, NULL, NULL, TO_ROOM );
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if (victim->deleted)
          continue;
      if (victim->fly_level != ch->fly_level)
          continue;
    
    affect_strip ( victim, 0, spl_invis, 0        );
    affect_strip ( victim, 0, spl_mass_invis, 0    );
    affect_strip ( victim, gsn_shadow, 0, 0        );
    REMOVE_AFF_BIT( victim, AFF_HIDE    );
    REMOVE_AFF_BIT( victim, AFF_INVISIBLE    );
    act( "$n is revealed!", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are revealed!\n\r", victim );
    }
    return;
}

void spell_dispel_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    OBJ_DATA   *obj;
    char *target_name = (char *) vo;

    last_fun( "spell_dispel_invis");
    if( (target_name != NULL) && (obj = get_obj_carry( ch, target_name )))
    {
      act( "$n&n casts a spell at $p&n.", ch, obj, NULL, TO_ROOM );
      act( "You cast a spell at $p&n.", ch, obj, NULL, TO_CHAR );
      if( IS_OBJ_STAT( obj, ITEM_INVIS ))
      {
          REMOVE_OBJ_STAT( obj, ITEM_INVIS );
          act( "$p&n turns visible.", ch, obj, NULL, TO_ROOM );
          act( "$p&n turns visible.", ch, obj, NULL, TO_CHAR );
      }
      return;
    }
    else
    {
      act( "$n&n dispels invisibility.", ch, NULL, NULL, TO_ROOM );
      for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
      {
        if( is_affected( victim, AFF_INVISIBLE ))
        {
          act( "$n&n is revealed!", victim, NULL, NULL, TO_ROOM );
          send_to_char( "You are now visible.\n\r", victim );
        }
    affect_strip ( victim, 0, spl_invis, 0        );
    affect_strip ( victim, 0, spl_mass_invis, 0    );
    REMOVE_AFF_BIT( victim, AFF_INVISIBLE    );
      }
    }
    return;
}

void spell_negate_veil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    last_fun( "spell_negate_veil");
    act( "$n&n casts a spell at $p&n.", ch, obj, NULL, TO_ROOM );
    act( "You cast a spell at $p&n.", ch, obj, NULL, TO_CHAR );
    if( IS_OBJ_STAT( obj, ITEM_INVIS ))
    {
        REMOVE_OBJ_STAT( obj, ITEM_INVIS );
        act( "$p&n turns visible.", ch, obj, NULL, TO_ROOM );
        act( "$p&n turns visible.", ch, obj, NULL, TO_CHAR );
    }
    else
        send_to_char( "Nothing happens.\n\r", ch );

    return;
}

void spell_negate_luster( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    last_fun( "spell_negate_luster");
    act( "$n&n casts a spell at $p&n.", ch, obj, NULL, TO_ROOM );
    act( "You cast a spell at $p&n.", ch, obj, NULL, TO_CHAR );
    if( IS_OBJ_STAT( obj, ITEM_LIT ))
    {
        REMOVE_OBJ_STAT( obj, ITEM_LIT );
        act( "$p&n turns dark.", ch, obj, NULL, TO_ROOM );
        act( "$p&n turns dark.", ch, obj, NULL, TO_CHAR );
        --ch->in_room->light;
    }
    else
        send_to_char( "Nothing happens.\n\r", ch );

    return;
}

void spell_illumination( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    last_fun( "spell_illumination");
    act( "$n&n casts a spell at $p&n.", ch, obj, NULL, TO_ROOM );
    act( "You cast a spell at $p&n.", ch, obj, NULL, TO_CHAR );
    if( !IS_OBJ_STAT( obj, ITEM_LIT ))
    {
        SET_OBJ_STAT( obj, ITEM_LIT );
        act( "$p&n glows brightly.", ch, obj, NULL, TO_ROOM );
        act( "$p&n glows brightly.", ch, obj, NULL, TO_CHAR );
        --ch->in_room->light;
    }
    else
        send_to_char( "Nothing happens.\n\r", ch );

    return;
}

void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char *argument = (char *)vo;
    OBJ_DATA *portal;
    int count;
    int dest_lower;
    int dest_upper;

    last_fun( "spell_gate");
    original = ch->in_room;

    if (ch->fly_level > 0) {
        send_to_char("You cannot create that while flying!\n\r", ch);
        return;
    }

    if ( IS_ROOM( original, ROOM_NO_GATE ) )
    {
       send_to_char( "Strange forces prevent you from opening a gate.\n\r", ch );
       return;
    }

    if ( !str_prefix( argument, "astral" ) )
    {
       dest_lower = ROOM_VNUM_ASTRAL_LOW;
       dest_upper = ROOM_VNUM_ASTRAL_HIGH;
       if( ch->in_room
           && ch->in_room->vnum >= ROOM_VNUM_ASTRAL_LOW
           && ch->in_room->vnum <= ROOM_VNUM_ASTRAL_HIGH  )
       {
           if( argument[0] != '\0' )
           {
               send_to_char( "You're already on the astral plane!\n\r", ch); 
               return;
           }
           else /* They didn't specify a target, so let them gate back
                 *  to the prime plane. - Lohrr
                 */
               switch( number_range( 1, 5 ) )
               {
               default:
               case 1:
                   dest_lower = SURFACE_MAP1_START_VNUM;
                   dest_upper = SURFACE_MAP1_END_VNUM;
               break;
               case 2:
                   dest_lower = SURFACE_MAP2_START_VNUM;
                   dest_upper = SURFACE_MAP2_END_VNUM;
               break;
               case 3:
                   dest_lower = UD_MAP1_START_VNUM;
                   dest_upper = UD_MAP1_END_VNUM;
               break;
               case 4:
                   dest_lower = UD_MAP2_START_VNUM;
                   dest_upper = UD_MAP2_END_VNUM;
               break;
               case 5:
                   dest_lower = UD_MAP3_START_VNUM;
                   dest_upper = UD_MAP3_END_VNUM;
               break;
               }
       }
    }
    else if ( !str_prefix( argument, "prime" ) )
    {
       switch( number_range( 1, 5 ) )
       {
           default:
           case 1:
               dest_lower = SURFACE_MAP1_START_VNUM;
               dest_upper = SURFACE_MAP1_END_VNUM;
           break;
           case 2:
               dest_lower = SURFACE_MAP2_START_VNUM;
               dest_upper = SURFACE_MAP2_END_VNUM;
           break;
           case 3:
               dest_lower = UD_MAP1_START_VNUM;
               dest_upper = UD_MAP1_END_VNUM;
           break;
           case 4:
               dest_lower = UD_MAP2_START_VNUM;
               dest_upper = UD_MAP2_END_VNUM;
           break;
           case 5:
               dest_lower = UD_MAP3_START_VNUM;
               dest_upper = UD_MAP3_END_VNUM;
           break;
       }
       if( !(  ch->in_room
            && ch->in_room->vnum >= ROOM_VNUM_ASTRAL_LOW
            && ch->in_room->vnum <= ROOM_VNUM_ASTRAL_HIGH ) )
       {
           send_to_char( "You're already off the astral plane!\n\r", ch);
           return;
       }
    }
    else
    {
       send_to_char( "What &+Rp&+Yl&+Ga&+Bn&+Ce&n did you want to gate to?\n\r", ch );
       return;
    }

    // If they can't find a valid target in 10 tries, give up.
    for ( count = 0; count < 10; count++ )
    {
        location = get_room_index( number_range( dest_lower, dest_upper ) );
        if ( location )
        {
          if (   !IS_ROOM( location, ROOM_PRIVATE  )
             && !IS_ROOM( location, ROOM_SOLITARY )
             && !IS_ROOM( location, ROOM_NO_GATE)
             && ( ch->in_room->area != location->area ) )
          {
            break;
          }
          else location = NULL;
        }
    }

    if( location == NULL )
    {
        send_to_char( "You failed!\n\r", ch );
        return;
    }


    portal = create_object( get_obj_index( OBJ_VNUM_GATE ), 0 );

    portal->timer = level / 15;
    portal->value[2] = level / 7;
    portal->value[0] = location->vnum;
 
    obj_to_room( portal, original );

    portal = create_object( get_obj_index( OBJ_VNUM_GATE ), 0 );
    portal->timer = level / 15;
    portal->value[2] = level / 7;
    portal->value[0] = original->vnum;

    obj_to_room( portal, location );
    send_to_char( "&+bA &+Bgate&N&+b to other realms rises from a column of mist...&n\n\r", ch );
    act( "&+bA &+Bgate&N&+b to other realms rises before you.&n", ch, NULL, NULL, TO_ROOM );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_general_purpose");
    dam = number_range( 25, 100 );
    if ( saves_spell( level, victim, DAM_PIERCE ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_PIERCE );

    return;
}

void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_giant_strength");

    send_to_char( "You feel stronger.\n\r", victim );

    if ( has_affect( victim, 0, sn, 0 ) ) {
       affect_cumulate( victim, 0, sn, 0, level, level / 10 );
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 2 + level / 8;
    set_bitvector( &af, AFF_NONE);
    affect_join( victim, &af );

    return;
}

void spell_wither( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_wither");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;
    if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_STR;
    af.modifier  = 0 - (ch->level / 2 + 5);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_CON;
    af.modifier  = 0 - (ch->level / 4);
    affect_to_char( victim, &af );

    af.location  = APPLY_AGI;
    af.modifier  = 0 - (ch->level / 6);
    affect_to_char( victim, &af );

    af.location  = APPLY_DEX;
    af.modifier  = 0 - (ch->level / 6);
    affect_to_char( victim, &af );

    af.location  = APPLY_CHA;
    af.modifier  = 0 - (ch->level / 6);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "&+LYou have withered your opponent!&n\n\r", ch );
    act( "$n&+L has been withered!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+LYou feel very weak and powerless.&n\n\r", victim );
    return;
}

void spell_mousestrength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_mousestrength");
    if ( has_affect( victim, 0, sn, 0 ) || saves_spell( level, victim, DAM_BLACK_MANA ) )
    return;

    affect_strip( victim, 0, spl_bear, 0);
    affect_strip( victim, 0, spl_elephant, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_STR;
    af.modifier  = 0 - (ch->level / 2 + 5);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    send_to_char( "You feel as weak as a mouse.\n\r", victim );
    return;
}

void spell_bearstrength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_bearstrength");

    if ( has_affect( victim, 0, sn, 0 ) || has_affect( victim, 0, spl_elephant, 0 ))
      return;

    affect_strip( victim, 0, spl_mouse, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_STR;
    af.modifier  = (ch->level / 4 + 5);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    af.location  = APPLY_DEX;
    af.modifier  = -(ch->level / 4 + 5);
    affect_to_char( victim, &af );
    send_to_char( "You feel as strong as a bear!\n\r", victim );
    return;
}

void spell_elephantstrength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_elephantstrength");
    if ( has_affect( victim, 0, sn, 0 ) || has_affect( victim, 0, spl_bear, 0 ))
      return;

    affect_strip( victim, 0, spl_mouse, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_STR;
    af.modifier  = (ch->level / 2 + 10);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    af.location  = APPLY_CON;
    affect_to_char( victim, &af );
    af.modifier  = (ch->level / 5);
    af.location  = APPLY_MAX_STR;
    affect_to_char( victim, &af );
    af.modifier  *= -1;
    af.location  = APPLY_DEX;
    affect_to_char( victim, &af );
    af.location  = APPLY_AGI;
    affect_to_char( victim, &af );
    send_to_char( "You feel as strong as an elephant!\n\r", victim );
    return;
}

void spell_lionrage( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_lionrage");
    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    affect_strip( victim, 0, spl_shrew, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 7 + ( level * 4 ) / 9;
    af.location  = APPLY_HITROLL;
    af.modifier  = (ch->level / 11 + 2);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = (ch->level / 9 + 3);
    affect_to_char( victim, &af );

    af.location  = APPLY_AGI;
    af.modifier  = 0 - (ch->level / 6 + 4);
    affect_to_char( victim, &af );
    send_to_char( "You feel the rage of the lion!\n\r", victim );
    return;
}

void spell_hawkvision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_hawkvision");

    if ( has_affect( victim, 0, sn, 0 ) )
      return;

    affect_strip( victim, 0, spl_mole, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level / 4 + level / 10;
    af.location  = APPLY_HITROLL;
    af.modifier  = (ch->level / 11 + 2);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    send_to_char( "&+yYour vision sharpens with deadly accuracy.&n\n\r", victim );
    return;
}

void spell_shrewtameness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_shrewtameness");
    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    affect_strip( victim, 0, spl_lion, 0);

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = APPLY_HITROLL;
    af.modifier  = 0 - (ch->level / 11 + 2);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = 0 - (ch->level / 9 + 3);
    affect_to_char( victim, &af );
    send_to_char( "You feel much less angry.\n\r", victim );
    return;
}

void spell_dexterity( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_dexterity");

    if ( get_curr_dex( victim ) >= get_max_dex( victim ) )
    {
        send_to_char("It seems to have no effect.\n\r", ch); 
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_DEX;
    af.modifier  = 2 + number_range(1,3);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    send_to_char( "You feel more dexterous.\n\r", victim );
    return;
}

// Harm - No cap.  150 points unsaved.  75 saved.
// The spell cannot kill it's victim.

void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_harm");
    dam = 200;
    if ( saves_spell( level, victim, DAM_HARM ) )
        dam /= 2;
    if( victim->hit < dam )
        dam = victim->hit < 4 ? 0 : victim->hit - dice( 1,4 );

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

void spell_fullharm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_fullharm");
    dam = 300;
    if ( saves_spell( level, victim, DAM_HARM ) )
        dam /= 2;
    if ( victim->hit < dam )
        dam = victim->hit < 4 ? 1 : victim->hit - dice( 1,4 );

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_heal");

    /* If over 100 hps down. */
    if( victim->hit < get_max_hit( victim ) - 100 )
      victim->hit += 100;
    else
      victim->hit = UMAX( get_max_hit( victim ) - dice( 1, 4), victim->hit );
    update_pos( victim );
    send_to_char( "A warm feeling fills your body.\n\r", victim );
    if( ch != victim )
        act( "&+WYou heal $N&n.", ch, NULL, victim, TO_CHAR );
    act( "&+W$n&+W heals $N&n.", ch, NULL, victim, TO_NOTVICT );

    gain_exp( ch, 3 );

    /* Added by Lohrr. */
    /* added by Lithos because there are too many affects with blind */
    if ( remove_blindness( victim ) )
        send_to_char( "&+WYour vision returns!&n\n\r", victim );

    return;
}

void spell_fullheal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_fullheal");
    if( victim->hit < get_max_hit( victim ) - 300 )
      victim->hit += 300;
    else
      victim->hit = UMAX( get_max_hit( victim ) - dice( 1, 4), victim->hit );
    update_pos( victim );
    send_to_char( "You feel FULLY HEALED!\n\r", victim );
    if( ch != victim )
        act( "&+WYou full heal $N&n.", ch, NULL, victim, TO_CHAR );
    act( "&+W$n&+W full heals $N&n.", ch, NULL, victim, TO_NOTVICT );

    gain_exp( ch, 5 );

    /* added by Lithos because there are too many affects with blind */
    if ( remove_blindness( victim ) )
        send_to_char( "&+WYour vision returns!&n\n\r", victim );

    return;
}

// Group Heal - acts just like a full heal spell on the entire group
// Code by Ski, so it won't work right.

void spell_group_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;

    last_fun( "spell_group_heal");
    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
       if( victim->deleted || !is_same_group( victim, ch) )
           continue;
       if( ch->fly_level != victim-> fly_level )
           continue;

       if( victim->hit < get_max_hit( victim ) )
           victim->hit = UMIN( victim->hit + 300, get_max_hit( victim ) );
           update_pos( victim );

       if ( ch != victim )
           send_to_char( "You feel FULLY HEALED!\n\r", victim );
       if ( remove_blindness( victim ) )
           send_to_char("You can see again.\n\r", victim );
       gain_exp( ch, 4 );
     }

    return;
}

// Aid - 10th Circle Paladin - Heal + Bless
// Code by Ski, so it won't work right.
void spell_aid( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
//    AFFECT_DATA af;

    last_fun( "spell_aid");
    // heal effects
/*
    if( victim->hit < get_max_hit( victim ) )
      victim->hit = UMIN( victim->hit + 100, get_max_hit( victim ) );
    update_pos( victim );

    if ( ch != victim )
    send_to_char( "You finish the spell.\n\r", ch );
    send_to_char( "A warm feeling fills your body.\n\r", victim );

    gain_exp( ch, 3 );

    if ( remove_blindness( victim ) )
        send_to_char( "&+WYour vision returns!&n\n\r", victim );
*/
    spell_heal( spl_heal, level, ch, (void *)victim );

    // Bless effects

    if ( victim->position == POS_FIGHTING || has_affect( victim, 0, sn, 0 ) )
       return;
/*
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 11 + 1;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    affect_to_char( victim, &af );
    send_to_char( "You feel righteous.\n\r", victim );
*/
    spell_bless(  spl_bless, level, ch, (void *)victim );

    return;
}

void spell_greater_mending( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int amount;

    last_fun( "spell_greater_mending");
    amount = 90 + dice( 3, 9 );

    if( victim->hit < get_max_hit( victim ) )
      victim->hit = UMIN( victim->hit + amount, get_max_hit( victim ) );
    update_pos( victim );

    gain_exp( ch, 3 );
    send_to_char( "You feel powerful spirits tending to your wounds.\n\r", victim );
    return;
}

/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    last_fun( "spell_high_explosive");
    dam = number_range( 30, 120 );
    if ( saves_spell( level, victim, DAM_PIERCE ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_PIERCE );

    return;
}

void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];

    last_fun( "spell_identify");
    if ( IS_OBJ_STAT( obj, ITEM_NOID ) )
    {
        /* Lvl 50+ can id !id items */
        if ( ch->level < 50 )
        {
            sprintf( buf, "Object '%s' evades your magic.\n\r", obj->name );
            send_to_char( buf, ch );
            return;
        }
    }

    sprintf( buf,
        "Object '%s' is type %s, extra flags %s.\n\r",
        obj->short_descr,
        item_type_name( obj ),
        item_string( obj ) );
    send_to_char( buf, ch );
    sprintf( buf,
        "Weight is %d, value is %d, level is %d.\n\r",
        obj->weight,
        obj->cost,
        obj->level );
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case TYPE_PILL:  
    case TYPE_SCROLL: 
    case TYPE_POTION:
    sprintf( buf, "Level %d spells of:", obj->value[0] );
    send_to_char( buf, ch );

    if ( obj->value[1] >= 0 && obj->value[1] < MAX_SPELL )
    {
        send_to_char( " '", ch );
        send_to_char( spells_table[obj->value[1]].name, ch );
        send_to_char( "'", ch );
    }

    if ( obj->value[2] >= 0 && obj->value[2] < MAX_SPELL )
    {
        send_to_char( " '", ch );
        send_to_char( spells_table[obj->value[2]].name, ch );
        send_to_char( "'", ch );
    }

    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SPELL )
    {
        send_to_char( " '", ch );
        send_to_char( spells_table[obj->value[3]].name, ch );
        send_to_char( "'", ch );
    }

    send_to_char( ".\n\r", ch );
    break;

    case TYPE_WAND: 
    case TYPE_STAFF: 
    sprintf( buf, "Has %d(%d) charges of level %d",
        obj->value[1], obj->value[2], obj->value[0] );
    send_to_char( buf, ch );
      
    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SPELL )
    {
        send_to_char( " '", ch );
        send_to_char( spells_table[obj->value[3]].name, ch );
        send_to_char( "'", ch );
    }

    send_to_char( ".\n\r", ch );
    break;
      
    case TYPE_WEAPON:
    sprintf( buf, "Damage is %dd%d.\n\r",
        obj->value[1], obj->value[2] );
    send_to_char( buf, ch );
    break;

    case TYPE_ARMOR:
    sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
    send_to_char( buf, ch );
    break;
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
        sprintf( buf, "Affects %s by %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier );
        send_to_char( buf, ch );
        if ( paf->bitvector )
        {
        sprintf( buf, "Enables %s.\n\r",
            affect_string( paf->bitvector, !IS_IMMORTAL( ch ) ) );
        send_to_char( buf, ch );
        }
    }
    else
        if ( paf->bitvector )
        {
        sprintf( buf, "Enables %s.\n\r",
            affect_string( paf->bitvector, !IS_IMMORTAL( ch ) ) );
        send_to_char( buf, ch );
        }
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
        sprintf( buf, "Affects %s by %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier );
        send_to_char( buf, ch );
        if ( paf->bitvector )
        {
        sprintf( buf, "Enables %s.\n\r",
            affect_string( paf->bitvector, !IS_IMMORTAL( ch ) ) );
        send_to_char( buf, ch );
        }
    }
    else
        if ( paf->bitvector )
        {
        sprintf( buf, "Enables %s.\n\r",
            affect_string( paf->bitvector, !IS_IMMORTAL( ch ) ) );
        send_to_char( buf, ch );
        }
    }

    return;
}



void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_infravision");
    if ( has_affect( victim, 0, sn, 0 ) ) {
        affect_refresh( victim, 0, sn, 0, 2 * level );
        return;
    }
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 2 * level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_INFRARED );
    affect_to_char( victim, &af );

    send_to_char( "Your eyes glow red.\n\r", victim );
    act( "$n's eyes glow red.", ch, NULL, NULL, TO_ROOM );
    return;
}

void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_invisibility");
    if ( has_affect( victim, 0, sn, 0)) {
        affect_refresh( victim, 0, sn, 0, 24 + level / 5);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_INVISIBLE);

    send_to_char( "You fade out of existence.\n\r", victim );
    act( "$n&n fades out of existence.", victim, NULL, NULL, TO_ROOM );

    /* This was moved so people can see who's disappearing. -Lohrr */
    affect_to_char( victim, &af );

    return;
}

void spell_vacancy( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_vacancy" );
    if ( has_affect( victim, 0, sn, 0))
    {
        affect_refresh( victim, 0, sn, 0, 24 + level / 5);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_VACANCY);

    send_to_char( "You become part of your surroundings.\n\r", victim );
    act( "$n&n fades from view.", victim, NULL, NULL, TO_ROOM );

    /* This was moved so people can see who's disappearing. -Lohrr */
    affect_to_char( victim, &af );
    SET_AFF_BIT( victim, AFF_HIDE );

    return;
}

void spell_mirage_arcana( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    last_fun( "spell_mirage_arcana" );

    for( victim = ch->in_room->people; victim; victim = victim->next )
    {
      if( victim->deleted || !is_same_group( victim, ch ))
        continue;

      if ( has_affect( victim, 0, spl_vacancy, 0 ))
      {
        affect_refresh( victim, 0, spl_vacancy, 0, 24 + level / 5);
        continue;
      }

      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = 24 + level / 5;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_VACANCY);

      send_to_char( "You become part of your surroundings.\n\r", victim );
      act( "$n&n fades from view.", victim, NULL, NULL, TO_ROOM );

      /* This was moved so people can see who's disappearing. -Lohrr */
      affect_to_char( victim, &af );
      SET_AFF_BIT( victim, AFF_HIDE );
    }

    return;
}

void spell_farsee( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_farsee");
    if ( is_affected( victim, AFF_FARSEE ) || has_affect( victim, 0, sn, 0))
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 12 + level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_FARSEE);
    affect_to_char( victim, &af );
    send_to_char( "&+MYour vision extends outward.&n\n\r", victim );
    return;
}

void spell_reveal_spirit_essence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    last_fun( "spell_reveal_spirit");
    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N&n has a very pure spirit.";
    else if ( ap >  350 ) msg = "$N&n has a relatively pure spirit.";
    else if ( ap >  100 ) msg = "$N&n has a slightly tarnished but basically good spirit.";
    else if ( ap > -100 ) msg = "$N&n has an uncommitted spirit.";
    else if ( ap > -350 ) msg = "$N&n has a dark spirit.";
    else if ( ap > -700 ) msg = "$N&n has a blackened spirit.";
    else msg = "$N&n has a heart of pure evil!";

    act( msg, ch, NULL, victim, TO_CHAR );

    if( IS_UNDEAD( victim ))
         send_to_char( "They are of &+Wundead&n origin.\n\r", ch );
    else if( victim->race == RACE_DEMON || victim->race == RACE_DEVIL )
         send_to_char( "They are of &+rdemonic&n origin.\n\r", ch);

    return;
}

void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    last_fun( "spell_know_alignment");
    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "$N's slash DISEMBOWELS you!";
    else msg = "I'd rather just not say anything at all about $N.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}

// Lightning Bolt - level 30 cap.  (2 * level)d4
// At level 30 or more: 60 minimum damage, 240 maximum, 150 average

void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_lightning_bolt");
    if( level > 30 ) level = 30;

    dam = dice( (2 * level), 4 );

    if ( saves_spell( level, victim, DAM_ELECTRICITY ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );

    return;
}

// Touch of Nightshade - level 30 cap.  (3/2 * level)d4

void spell_touch_nightshade( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int dam;

    last_fun( "spell_touch_of_nightshade");
    if( level > 30 )
        level = 30;

    dam = dice( ( 3 / 2 * level), 4);

    if ( saves_spell( level, victim, DAM_HARM ) )
       dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_HARM );

    return;
}

// Cone of Cold - level 35 cap.  (3 * level)d3
// At level 35 or more: 105 minimum damage, 315 maximum, 210 average

void spell_cone_of_cold( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_cone_of_cold");
    if( level > 35 ) level = 35;

    dam = dice( (3 * level), 3 );

    if ( saves_spell( level, victim, DAM_COLD ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    char      buf [ MAX_INPUT_LENGTH ];
    bool      found;
    char *target_name = (char *) vo;

    last_fun( "spell_locate_object");
    found = FALSE;
    for ( obj = object_list; obj; obj = obj->next )
    {
    if ( !can_see_obj( ch, obj ) || !is_name( target_name, obj->name ) )
        continue;

    found = TRUE;

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
        ;

    if ( in_obj->carried_by )
    {
        sprintf( buf, "%s carried by %s.\n\r",
            obj->short_descr, PERS( in_obj->carried_by, ch ) );
    }
    else
    {
        sprintf( buf, "%s in %s.\n\r",
            obj->short_descr, !in_obj->in_room
            ? "somewhere" : in_obj->in_room->name );
    }

    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );
    }

    if ( !found )
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}

void spell_minor_creation( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj;
    char      buf [ MAX_INPUT_LENGTH ];
    int       vnum;
    char *target_name = (char *) vo;

    last_fun( "spell_minor_creation");
    vnum = 0;

    if (IS_IMMORTAL(ch) ) {
        sprintf(buf, "casting minor creation for a %s.", target_name);
        wiznet( 0, WIZ_LSPAM, 0, buf );
    }
    sprintf( buf, "Minor creation: argument %s", target_name );

    if( !str_cmp( target_name, "iron" ) || !str_cmp( target_name, "food" )
    || !str_cmp( target_name, "ration" ) )
        vnum = OBJ_VNUM_IRON_RATION;
    else if( !str_cmp( target_name, "skin" ) || !str_cmp( target_name, "waterskin" ))
        vnum = OBJ_VNUM_WATERSKIN;
    else if( !str_cmp( target_name, "quill" ))
        vnum = OBJ_VNUM_QUILL;
    else if( !str_cmp( target_name, "backpack" ) || !str_cmp( target_name, "bp" ))
        vnum = OBJ_VNUM_NEWBIE_BACKPACK;
    else if( !str_cmp( target_name, "torch" ))
        vnum = OBJ_VNUM_TORCH;
    else if( !str_cmp( target_name, "jerkin" ))
        vnum = OBJ_VNUM_NEWBIE_VEST;
    else if( !str_cmp( target_name, "shabby" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 1;
    else if( !str_cmp( target_name, "worn" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 2;
    else if( !str_cmp( target_name, "dark" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 3;
    else if( !str_cmp( target_name, "alligator" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 4;
    else if( !str_cmp( target_name, "vest" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 5;
    else if( !str_cmp( target_name, "armor" ) || !str_cmp( target_name, "suit" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 6;
    else if( !str_cmp( target_name, "shirt" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 7;
    else if( !str_cmp( target_name, "hide" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 8;
    else if( !str_cmp( target_name, "ogre" ))
        vnum = OBJ_VNUM_NEWBIE_VEST + 9;
    else if( !str_cmp( target_name, "cap" ))
        vnum = OBJ_VNUM_NEWBIE_HELM;
    else if( !str_cmp( target_name, "sallet" ))
        vnum = OBJ_VNUM_NEWBIE_HELM + 1;
    else if( !str_cmp( target_name, "tattered" ))
        vnum = OBJ_VNUM_NEWBIE_HELM + 2;
    else if( !str_cmp( target_name, "cowl" ))
        vnum = OBJ_VNUM_NEWBIE_HELM + 3;
    else if( !str_cmp( target_name, "helm" ))
        vnum = OBJ_VNUM_NEWBIE_HELM + 4;
    else if( !str_cmp( target_name, "sleeves" ))
        vnum = OBJ_VNUM_NEWBIE_SLEEVES;
    else if( !str_cmp( target_name, "cloth" ))
        vnum = OBJ_VNUM_NEWBIE_SLEEVES;
    else if( !str_cmp( target_name, "pants" ))
        vnum = OBJ_VNUM_NEWBIE_PANTS;
    else if( !str_cmp( target_name, "boots" ))
        vnum = OBJ_VNUM_NEWBIE_BOOTS;
    else if( !str_cmp( target_name, "cloak" ))
        vnum = OBJ_VNUM_NEWBIE_CLOAK;
    else if( !str_cmp( target_name, "sword" ))
        vnum = OBJ_VNUM_NEWBIE_SWORD;
    else if( !str_cmp( target_name, "dagger" ))
        vnum = OBJ_VNUM_NEWBIE_DAGGER;
    else if( !str_cmp( target_name, "two" ))
        vnum = OBJ_VNUM_NEWBIE_2HSWORD;
    else if( !str_cmp( target_name, "knife" ))
        vnum = OBJ_VNUM_NEWBIE_KNIFE;
    else if( !str_cmp( target_name, "short" ))
        vnum = OBJ_VNUM_NEWBIE_SHORTSWORD;
    else if( !str_cmp( target_name, "mace" ))
        vnum = OBJ_VNUM_NEWBIE_MACE;
    else if( !str_cmp( target_name, "staff" ))
        vnum = OBJ_VNUM_NEWBIE_STAFF;
    
    if( vnum == 0 )
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    obj = create_object( get_obj_index( vnum ), 0 );
    if( !obj )
    {
       sprintf( buf, "Minor creation spell tried to create an object number %d which does not exist. Please report this as a bug.\n\r", vnum );
       send_to_char( buf, ch );
       return;
    }
    obj->cost = 0;
    SET_OBJ_STAT( obj, ITEM_NOSELL );
    obj->fly_level = ch->fly_level;
    act( "$p&n appears.", ch, obj, NULL, TO_ROOM );
    act( "$p&n appears.", ch, obj, NULL, TO_CHAR );
    obj_to_room( obj, ch->in_room );

    return;
}

// Magic Missile - Level/3 missiles, up to 6.
// Each does 1d6+1 damage.
void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int number;
    int count;
    int dam;

    last_fun( "spell_magic_missile");
    number = UMIN( (ch->level / 4), 6 );
    number = UMAX( number, 1 );

    dam = dice( 2, 4 ) + 1;

    for( count = 0; count < number && victim->position > POS_DEAD; count++ )
        spell_damage( ch, victim, dam, sn, DAM_ENERGY );

    return;
}

// Ice Missile - Level/3 missiles, up to 5.
// Each does 1d6+1 damage.
// This spell was apparently never modified to take into consideration
// the /4 PvP damage.  Doubled the damage.
void spell_ice_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int number;
    int count;
    int dam;

    last_fun( "spell_ice_missile");
    number = UMIN( (ch->level / 3), 5 );
    number = UMAX( number, 1 );

    dam = dice( 2, 4 ) + 1;

    for( count = 0; count < number && victim->position > POS_DEAD; count++ )
        spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

// Ultra Death Ray - 20 missiles
// Each does 1d6+1 damage.

void spell_ultra_death_ray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int number;
    int count;
    int dam;

    last_fun( "spell_ultra_death_ray");
    number = 20;

    dam = dice( 2, 4) + 1;

    for( count = 0; count < number && victim->position > POS_DEAD; count++ )
        spell_damage( ch, victim, dam, sn, DAM_ENERGY );

    return;
}


// Melf's Acid Arrow - Level 20 cap.  3d3 + level
// Does damage over three rounds
// At level 20 or more: 23 minimum damage, 29 max, 26 average
// First round does +4 damage.

void spell_acid_arrow( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int       dam;

    last_fun( "spell_acid_arrow");
    if( level > 20 ) level = 20;

    dam = dice( 3, 3) + level;

    if ( saves_spell( level, victim, DAM_ACID ) )
    dam /= 2;

    act( "Your &+gacid arrow&n strikes $N&n solidly.", ch, NULL, victim, TO_CHAR );
    act( "You are hit by an &+gacid arrow&n from $n&n!", ch, NULL, victim, TO_VICT );
    act( "$N&n is hit by an &+gacid arrow&n from $n&n.", ch, NULL, victim, TO_ROOM );
   
    spell_damage( ch, victim, (dam + 4), sn, DAM_ACID );
    if (victim->position == POS_DEAD )
      return;
    create_event( EVENT_ACID_ARROW, PULSE_VIOLENCE, ch, victim, dam );
    create_event( EVENT_ACID_ARROW, (PULSE_VIOLENCE * 2), ch, victim, dam );
    if( level > 30 )
        create_event( EVENT_ACID_ARROW, (PULSE_VIOLENCE * 3), ch, victim, dam );
    if( level > 40 )
        create_event( EVENT_ACID_ARROW, (PULSE_VIOLENCE * 3), ch, victim, dam );

    return;
}

// Immolate - No cap.  (level d3) +100.  Damage over time.
// Level 50: 150 minimum, 250 maximum, 200 average.

void spell_immolate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int       dam;

    last_fun( "spell_immolate");
    dam = dice( level, 3) + 100;

    if ( saves_spell( level, victim, DAM_FIRE ) )
    dam /= 2;

    act( "$N&n is covered in &+rburning flames!&n", ch, NULL, victim, TO_CHAR );
    act( "You are covered in &+rburning flames!&n", ch, NULL, victim, TO_VICT );
    act( "$N&n is covered in &+rburning flames!&n", ch, NULL, victim, TO_ROOM );
   
    spell_damage( ch, victim, (dam + 4), sn, DAM_FIRE );
    if (victim->position == POS_DEAD )
      return;
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE + 6), ch, victim, dam );
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE * 2 + 12), ch, victim, dam );
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE * 3 + 18), ch, victim, dam );

    return;
}

// Conflaguration - no cap.  (level 3d) + 100.  Damage over time.
// Level 50: 150 minimum, 250 maximum, 200 average.
// burns faster than immolate.

void spell_conflaguration( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int       dam;

    last_fun( "spell_conflaguration");
    dam = dice( level, 3 ) + 100;

    if ( saves_spell( level, victim, DAM_FIRE ) )
    dam /= 2;

    act( "$N&n is covered in a &+rfiery conflaguration!&n", ch, NULL, victim, TO_CHAR );
    act( "You are covered in a &+rfiery conflaguration!&n", ch, NULL, victim, TO_VICT );
    act( "$N&n is covered in a &+rfiery conflaguration!&n", ch, NULL, victim, TO_ROOM );
   
    spell_damage( ch, victim, (dam + 4), sn, DAM_FIRE );
    if (victim->position == POS_DEAD )
      return;
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE + 4), ch, victim, dam );
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE * 2 + 8), ch, victim, dam );
    create_event( EVENT_IMMOLATE, (PULSE_VIOLENCE * 3 + 12), ch, victim, dam );

    return;
}

void spell_creeping_doom( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int       dam;

    last_fun( "spell_creeping_doom");
    dam = dice( level, 4 );

    act( "&+LYou conjure a &+Rplague&+L of &+ginsects&+L which ravages the area.&n", ch, NULL, NULL, TO_CHAR );
    act( "&+L$n&+L conjures a &+Rplague&+L of &+ginsects&+L which ravages the area.&n", ch, NULL, NULL, TO_ROOM );
    if (ch->fly_level < MAX_FLY)
      act("&+LA &+Rplague&+L of &n&+ginsects&+L appears below you&n.", ch,
         NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+LA &+Rplague&+L of &n&+ginsects&+L appears above you&n.", ch,
         NULL, NULL, TO_BELOW);

    create_event( EVENT_CREEPING_DOOM, 1, ch, ch->in_room, dam );
    create_event( EVENT_CREEPING_DOOM, (PULSE_VIOLENCE + 3), ch, ch->in_room, dam );
    create_event( EVENT_CREEPING_DOOM, (PULSE_VIOLENCE * 2 + 5), ch, ch->in_room, dam );
    create_event( EVENT_CREEPING_DOOM, (PULSE_VIOLENCE * 3 + 7), ch, ch->in_room, dam );

    return;
}

// Incendiary Cloud.  (level*4)d3.  Area effect.
// Level 50: 200 minimum, 600 maximum, 400 average

void spell_incendiary_cloud( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    int        dam;

    last_fun( "spell_incendiary_cloud");
    if( level > 50 ) level = 50;

    act( "&+rYou conjure a cloud of noxious gas which fills the area and ignites!&n", ch, NULL, NULL, TO_CHAR );
    act( "&+r$n&+r conjures a cloud of noxious gas which fills the area and ignites!&n", ch, NULL, NULL, TO_ROOM );
    if ( ch->fly_level  < MAX_FLY )
        act( "&+rA cloud of noxious gas appears below you and &+Rignites!&n", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0 )
        act( "&+rA cloud of noxious gas appears above you and &+Rignites!&n", ch, NULL, NULL, TO_BELOW );

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if ( victim->fly_level != ch->fly_level ) continue;
      dam = dice( (4 * level), 3 );
      if ( saves_spell( level, victim, DAM_FIRE ) )
          spell_damage( ch, victim, dam/2, sn, DAM_FIRE );
      else
          spell_damage( ch, victim, dam, sn, DAM_FIRE );
    }

    return;
}

void spell_mass_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;

    last_fun( "spell_mass_heal");
    for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if (gch->fly_level != ch->fly_level || gch->deleted)
            continue;

        /* Don't heal ppl that aren't hurt. */
        if( gch->hit >= get_max_hit( gch ) )
            continue;
        if( gch->hit < get_max_hit( gch ) - 100 )
          gch->hit = UMIN( gch->hit + 100, get_max_hit( gch ) );
        else
          gch->hit = UMAX( get_max_hit( gch ) - dice( 1, 4), gch->hit );

        gain_exp( ch, 3 );
        update_pos( gch );
        send_to_char( "You feel better!\n\r", gch );
        if ( remove_blindness( gch ) )
            send_to_char( "You can see again.\n\r", gch);
    }

    return;
}




void spell_wellness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;
    int        heal;

    last_fun( "spell_wellness");
    for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
    if ( !is_same_group( gch, ch ) )
        continue;
      if (gch->fly_level != ch->fly_level)
          continue;

    heal = dice( 2, 14 ) + dice(2, level / 2);
        if( gch->hit < get_max_hit( gch ) )
          gch->hit = UMIN( gch->hit + heal, get_max_hit( gch ) );
    update_pos( gch );
    
    send_to_char( "You feel better!\n\r", gch );
    }

    gain_exp( ch, 2 );

    return;
}

void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    OBJ_DATA   *obj;
    AFFECT_DATA af;

    last_fun( "spell_mass_invis");

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_INVISIBLE);
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
    if ( !is_same_group( gch, ch ) || is_affected( gch, AFF_INVISIBLE )
            || gch->fly_level != ch->fly_level)
        continue;

    send_to_char( "You slowly fade out of existence.\n\r", gch );
    act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );
    affect_to_char( gch, &af );
    }
    for( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
        if( obj->deleted  || obj->fly_level != ch->fly_level)
          continue;
        if( IS_OBJ_STAT( obj, ITEM_INVIS ))
          continue;
        if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) ) continue;
        act( "&+L$p&+L turns invisible.", ch, obj, NULL, TO_ROOM );
        act( "&+L$p&+L turns invisible.", ch, obj, NULL, TO_CHAR );
        SET_OBJ_STAT( obj, ITEM_INVIS );
    }

    return;
}

void spell_pythonsting( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_pythonsting");
    if ( !saves_spell( level, victim, DAM_POISON ) )
    {
      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = level / 2;
      af.location  = APPLY_STR;
      af.modifier  = -4;
      set_bitvector( &af, AFF_POISON);
      affect_refresh( victim, 0, sn, 0, level/2 );
//      affect_join( victim, &af );
      act( "$n&n shivers slightly.", victim, NULL, NULL, TO_ROOM );
      send_to_char( "You feel very sick.\n\r", victim );
      spell_damage( ch, victim, dice( 2, 8 ), sn, DAM_POISON );
    }
    else
      spell_damage( ch, victim, dice( 2, 4 ), sn, DAM_POISON );

    return;
}

void spell_gr_pythonsting( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA  *victim_next;
    AFFECT_DATA af;

    last_fun( "spell_gr_pythonsting");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
        if( victim->deleted || victim == ch || is_same_group( victim, ch )
            || victim->fly_level != ch->fly_level)
          continue;

        if ( !saves_spell( level, victim, DAM_POISON ) )
        {
          af.skill     = 0;
          af.spell     = sn;
          af.song      = 0;
          af.duration  = level / 2;
          af.location  = APPLY_STR;
          af.modifier  = -4;
          set_bitvector( &af, AFF_POISON);
//          affect_join( victim, &af );
          affect_refresh( victim, 0, sn, 0, level/2 );
          act( "$n&n shivers slightly.", victim, NULL, NULL, TO_ROOM );
          send_to_char( "You feel very sick.\n\r", victim );
          spell_damage( ch, victim, dice( 4, 8 ), sn, DAM_POISON );
        }
        else
            spell_damage( ch, victim, dice( 4, 4 ), sn, DAM_POISON );
    }

    return;
}

void spell_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{

    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_silence");
    if ( is_affected( victim, AFF_MUTE )
    || saves_spell( level, victim, DAM_BLACK_MANA ) )
    return;
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 4 + level / 4;
    af.location  = 0;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MUTE);
    affect_to_char( victim, &af );
    
    act( "You have silenced $N!", ch, NULL, victim, TO_CHAR    );
    act( "$n has silenced you!",  ch, NULL, victim, TO_VICT    );
    act( "$n has silenced $N!",   ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_pass_door");
    if ( is_affected(victim, AFF_PASS_DOOR) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PASS_DOOR);
    affect_to_char( victim, &af );

    send_to_char( "You turn translucent.\n\r", victim );
    act( "$n&n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_plague");
    if ( saves_spell( level, victim, DAM_DISEASE ) )
    {
        if ( ch == victim )
        send_to_char( "You feel momentarily ill, but it passes.\n\r", ch );
        else
        act( "$N seems to be unaffected.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af = create_affect( 0, sn, 0, level, APPLY_STR, -5, AFF_PLAGUE );
    affect_join( victim, &af );

    send_to_char( "You scream in agony as plague sores erupt from your skin.\n\r", victim );
    act( "$n&n screams in agony as plague sores erupt from $s skin.", victim, NULL, NULL, TO_ROOM );
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_poison");
    if ( saves_spell( level, victim, DAM_POISON ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    set_bitvector( &af, AFF_POISON);
    affect_join( victim, &af );

    if ( ch != victim )
    send_to_char( "You have poisoned your victim.\n\r", ch );
    send_to_char( "You feel very sick.\n\r", victim );
    return;
}

void spell_call_wild( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];

    last_fun( "spell_call_wild");
    if ( is_affected( victim, AFF_POLYMORPH ) )
    {
    act( "$E is already changed.", ch, NULL, victim, TO_CHAR );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = dice( 2, level );
    af.location  = APPLY_RACE;

    do
    {
       // Veygoth - changed race selection to be a non-coin carrying
       // race
//       af.modifier = number_range( (MAX_COIN_RACE - victim->race),
//                                   (MAX_RACE - victim->race ));
        //using number_race() instead  - Lithos
        af.modifier = number_race(RACE_RANGE_ANIMAL) - victim->race ;
    }
    while ( af.modifier == 0 );

    set_bitvector( &af, AFF_POLYMORPH);
    affect_to_char( victim, &af );
    sprintf( buf, "&+WYou have been turned into a &n%s&n&+W!\n\r&n", 
        race_table[victim->race].colorname );
    send_to_char( buf, victim );
    sprintf( buf, "$n&n screams in pain as $e violently transforms into a %s&n!&n", 
        race_table[victim->race].colorname );
    act( buf, victim, NULL, ch, TO_ROOM );

    if ( get_trust( ch ) > LEVEL_HERO )
    {
      sprintf( buf, "Log %s: casting Call of the Wild", ch->name );
      log_string( buf );
    }

    return;

}

void spell_polymorph_other( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];

    last_fun( "spell_polymorph_other");
    if ( is_affected( victim, AFF_POLYMORPH ) )
    {
    act( "$E is already changed.", ch, NULL, victim, TO_CHAR );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = dice( 10, level );
    af.location  = APPLY_RACE;

    do
    {
    af.modifier  = number_range( 0, MAX_RACE - 1 ) - victim->race;
    }
    while ( af.modifier == 0 );

    set_bitvector( &af, AFF_POLYMORPH);
    affect_to_char( victim, &af );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel different.\n\r", victim );

    if ( get_trust( ch ) > LEVEL_HERO )
    {
      sprintf( buf, "Log %s: casting Polymorph other", ch->name );
      log_string( buf );
    }

    return;

}

void spell_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_protection_evil");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_EVIL);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel holy and pure.\n\r", victim );
    return;
}

void spell_harbor_balance( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_harbor_balance");
    if ( is_affected( victim, AFF_PROTECT_EVIL )
        && is_affected( victim, AFF_PROTECT_GOOD ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;

    set_bitvector( &af, AFF_PROTECT_EVIL);
    if( !is_affected( victim, AFF_PROTECT_EVIL ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_PROTECT_GOOD);
    if( !is_affected( victim, AFF_PROTECT_GOOD ))
      affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel protected from the unbalance and chaos of the world.\n\r", victim );
    return;
}

void spell_minor_globe( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_minor_globe");
    if ( is_affected( victim, AFF_MINOR_GLOBE )
        || is_affected( victim, AFF_MAJOR_GLOBE ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level/10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_GLOBE);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+RYou are surrounded by a shimmering globe.&n\n\r", victim );
    return;
}

void spell_major_globe( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_major_globe");
    if ( is_affected( victim, AFF_MINOR_GLOBE )
        || is_affected( victim, AFF_MAJOR_GLOBE ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level/10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MAJOR_GLOBE);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+RYou are surrounded by a shimmering globe.&n\n\r", victim );
    return;
}

void spell_group_globe( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    last_fun( "spell_group_globe");
    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
       if( victim->deleted || !is_same_group( ch, victim ))
         continue;
       if ( is_affected( victim, AFF_MINOR_GLOBE )
         || is_affected( victim, AFF_MAJOR_GLOBE ) )
         continue;

       af.skill     = 0;
       af.spell     = sn;
       af.song      = 0;
       af.duration  = 6 + level/10;
       af.location  = APPLY_NONE;
       af.modifier  = 0;
       set_bitvector( &af, AFF_MAJOR_GLOBE);
       affect_to_char( victim, &af );
       send_to_char( "&+RYou are surrounded by a shimmering globe.&n\n\r", victim );
       act( "&+R$n&+r is surrounded by a shimmering globe.&n", victim, NULL, NULL, TO_ROOM );
    }

    return;
}

void spell_gr_spirit_ward( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_gr_spirit_ward");
    if ( is_affected( victim, AFF_SPIRIT_WARD )
        || is_affected( victim, AFF_GREATER_SPIRIT_WARD) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level/10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_GREATER_SPIRIT_WARD);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+WYou are surrounded by a protective aura.&n\n\r", victim );
    return;
}

void spell_spirit_ward( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_spirit_ward");
    if ( is_affected( victim, AFF_SPIRIT_WARD )
        || is_affected( victim, AFF_GREATER_SPIRIT_WARD) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level/10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SPIRIT_WARD);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+WYou are surrounded by a protective aura.&n\n\r", victim );
    return;
}

void spell_protection_good( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_protection_good");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_GOOD);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel aligned with darkness.\n\r", victim );
    return;
}



void spell_recharge_item ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj= (OBJ_DATA *) vo;

    last_fun( "spell_recharge_item");
    if (   obj->item_type != TYPE_WAND
    && obj->item_type != TYPE_STAFF )
    {
    send_to_char( "That item cannot be recharged.\n\r", ch );
    return;
    }

    if ( number_percent( ) <
    ch->pcdata->spl_lrn[sn] / 2 + 3 * ( ch->level - obj->value[0] )
    && obj->value[2] < obj->value[1] )
    {
    obj->value[2] = obj->value[1];
    obj->cost     = 1;
    act( "$p shines brightly, then returns to normal.",
        ch, obj, NULL, TO_CHAR );
    act( "$p shines brightly, then returns to normal.",
        ch, obj, NULL, TO_ROOM );
    }
    else
    {
    act( "$p shines brightly, then explodes into fragments!",
        ch, obj, NULL, TO_CHAR );
    act( "$p shines brightly, then explodes into fragments!",
        ch, obj, NULL, TO_ROOM );
    obj_from_char( obj );
    extract_obj( obj );

    spell_damage( ch, ch, get_max_hit( ch ) / 16, sn, DAM_ENERGY );
    }

    return;
}

// Spell by Veygoth
// Rejuvenate can either add or remove 1-6 years based on whether the
// spell was successful or not.  However, failure removes one point of
// constitution from the victim *permenantly*.  Each casting ages the
// caster two weeks in addition.
//
// I suspect that the rejuvenation failure penalties aren't harsh enough,
// but they'll do for now.
//
// -- Veygoth
void spell_rejuvenate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_rejuvenate");
    if( IS_NPC( ch ))
        return;

    if( IS_NPC( victim ))
    {
        send_to_char( "This spell affects only players.\n\r", ch );
        return;
    }

    if( current_time - victim->pcdata->created < 424800 )
    {
        send_to_char( "You cannot make someone younger than adult age.\n\r", ch );
        return;
    }

    // Age the caster two weeks for casting it.
    ch->pcdata->created -= 10080;

    if( ch->pcdata->created < 1 ) ch->pcdata->created = 1;

    if( number_percent() < ch->pcdata->spl_lrn[spl_rejuvenate])
    {
        send_to_char( "Success!\n\r", ch );
        send_to_char( "You feel younger!\n\r", ch );
        victim->pcdata->created += number_range( 1, 6 ) * 424800;
        if( victim->pcdata->created > current_time ) victim->pcdata->created = (int)current_time;
    }
    else
    {
        send_to_char( "You withered them!\n\r", ch );
        send_to_char( "You feel older and weaker!\n\r", ch );
        victim->pcdata->created -= number_range( 1, 6 ) * 424800;
        victim->perm_con -= 1;
        if( victim->pcdata->created < 1 ) victim->pcdata->created = 1;
    }

    return;
}

void spell_vigorize_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int amount;

    last_fun( "spell_vigorize_light");
    amount = dice( 2, 6 ) + ( level / 25 );

    victim->move = UMIN( victim->move + amount, victim->max_move );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel less tired.\n\r", victim );
    return;
}

void spell_vigorize_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int amount;

    last_fun( "spell_vigorize_serious");
    amount = dice( 3, 7 ) + ( level / 6 );

    victim->move = UMIN( victim->move + amount, victim->max_move );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel much less tired.\n\r", victim );
    return;
}

void spell_vigorize_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int amount;

    last_fun( "spell_vigorize_critical");
    amount = dice( 4, 8 ) + ( level / 4 );

    victim->move = UMIN( victim->move + amount, victim->max_move );

    if ( ch != victim )
    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel energized.\n\r", victim );
    return;
}

/* Remove Alignment by Thelonius for EnvyMud */
void spell_remove_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    last_fun( "spell_remove_alignment");
    if ( !IS_OBJ_STAT( obj, ITEM_EVIL        )
    &&   !IS_OBJ_STAT( obj, ITEM_ANTI_GOOD    )
    &&   !IS_OBJ_STAT( obj, ITEM_ANTI_EVIL    )
    &&   !IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL    ) )
    {
    send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    if ( number_percent( ) < ch->pcdata->spl_lrn[sn] *
     ( 33 +
      ( 33 * ( ch->level - obj->level ) / (float)LEVEL_HERO ) ) / 100.0 )
    {
    REMOVE_OBJ_STAT( obj, ITEM_EVIL        );
    REMOVE_OBJ_STAT( obj, ITEM_ANTI_GOOD    );
    REMOVE_OBJ_STAT( obj, ITEM_ANTI_EVIL    );
    REMOVE_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL    );
    act( "$p&n hums briefly, then lies quiet.", ch, obj, NULL, TO_CHAR );
    act( "$p&n hums briefly, then lies quiet.", ch, obj, NULL, TO_ROOM );
    return;
    }

    SET_OBJ_STAT( obj, ITEM_NODROP );
    obj->wear_flags = 1;            /* Useless   */
    obj->cost       = 0;            /* Worthless */
    act( "$p&n blazes brightly, then turns grey.", ch, obj, NULL, TO_CHAR );
    act( "$p&n blazes brightly, then turns grey.", ch, obj, NULL, TO_ROOM );

    return;

}

/* by Veygoth */
void spell_preserve( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_STRING_LENGTH];
    int time;

    last_fun( "spell_preserve");
    if ( (obj->item_type != TYPE_CORPSE_NPC)
      && (obj->item_type != TYPE_CORPSE_PC) )
    {
    send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    time = dice( 3, (ch->level / 2) );

    // NPC corpses get preserved half as long as PC corpses.
    if( obj->item_type == TYPE_CORPSE_NPC )
      time /= 2;

    obj->timer += time;

    sprintf( buf, "$p&n will last %d more minutes before decaying.\n\r", time / 2 );
    act( buf, ch, obj, NULL, TO_CHAR );

    act( "$p&n glows briefly.", ch, obj, NULL, TO_ROOM );

    return;
}

/* Expulsion of ITEM_NOREMOVE addition by Katrina */
/* Added drop cursed items in inventory. - Lohrr */
/* Converted to TAR_OBJ_CHAR -Lithos */
void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim;
    int        iWear;
    int        yesno  = 0;
    bool        hasobj = FALSE;

    last_fun( "spell_remove_curse");
    if (!vo) return;
    if ( ((OBJ_CHAR *)vo)->type == TYPE_OBJ_DATA) {
    /* remove curse on an object */
        obj = ((OBJ_CHAR *)vo)->pointer.obj;
        if (!obj) {
            bug( "spell_remove_curse: NULL obj pointer in(OBJ_CHAR *)vo", 0);
            return;
        }
        /* check and see if obj is in inventory */
        for(obj_next = ch->carrying; obj_next; obj_next = obj_next->next) {
            if (obj == obj_next) {
                hasobj = TRUE;
                break;
            }
        }
        if (!hasobj) {
            send_to_char("You don't seem to have that item in your inventory.", ch);
            return;
        }
        if ( IS_OBJ_STAT( obj, ITEM_NODROP ) ) {
            act("$p&n&+B flashes blue&n for a moment.", ch, obj, NULL, TO_CHAR);
            act("$p&n&+B flashes blue&n for a moment.", ch, obj, NULL, TO_ROOM);
            //turn off the NODROP bit
            REMOVE_OBJ_STAT( obj, ITEM_NODROP );
            return;
        }
        if ( IS_OBJ_STAT( obj, ITEM_EVIL ) ) {
            act("$p&n loses its &+rred aura&n.", ch, obj, NULL, TO_CHAR);
            act("$p&n loses its &+rred aura&n.", ch, obj, NULL, TO_ROOM);
            REMOVE_OBJ_STAT( obj, ITEM_EVIL );
            return;
        }
        else {
            act("It seems to have no effect on $p&n.", ch, obj, NULL, TO_CHAR);
        }
    }
    else if ( ((OBJ_CHAR *)vo)->type == TYPE_CHAR_DATA ) {
    /* remove cursed equipped items. */
    victim = ((OBJ_CHAR *)vo)->pointer.ch;
    if(!victim) {
        bug("spell_remove_curse: NULL ch pointer in (OBJ_CHAR *)vo", 0);
        return;
    }
    if (victim->in_room != ch->in_room) {
        send_to_char("They are not here.", ch);
        return;
    }
    for ( iWear = 0; iWear < MAX_WEAR; iWear ++ )
    {
    if ( !( obj = get_eq_char( victim, iWear ) ) )
        continue;

    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
        unequip_char( victim, obj );
        obj_from_char( obj );
        obj_to_room( obj, victim->in_room );
        act( "Your $p&n &+Bflashes blue&n and falls to the ground.",  victim, obj, NULL, TO_CHAR );
        act( "$n&n's $p&n &+Bflashes blue&n and falls to the ground.", victim, obj, NULL, TO_ROOM );
            if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_UNCURSE ))
            {
              setoff_trap( ch, obj );
              if( ch->deleted || ch->position == POS_DEAD ) {
                return;
              }
            }
        yesno = 1;
    }
    }

    /* remove cursed items in inventory. */
    for ( obj = victim->carrying; obj; obj = obj_next )
    {
    obj_next = obj->next_content;
    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
//        unequip_char( victim, obj );
        obj_from_char( obj );
        obj_to_room( obj, victim->in_room );
        act( "Your $p&n &+Bflashes blue&n and falls to the ground.",  victim, obj, NULL, TO_CHAR );
        act( "$n&n's $p&n &+Bflashes blue&n and falls to the ground.", victim, obj, NULL, TO_ROOM );
            if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_UNCURSE ))
            {
              setoff_trap( ch, obj );
              if( ch->deleted || ch->position == POS_DEAD ) {
                return;
              }
            }
        yesno = 1;
    }
    }

    /* victim cursed. */
    if ( has_affect( victim, 0, spl_curse, 0 ) || has_affect( victim, 0, spl_hex, 0 )
         || has_affect( victim, 0, spl_malison, 0 ))
    {
    affect_strip( victim, 0, spl_curse, 0 );
    affect_strip( victim, 0, spl_malison, 0 );
    affect_strip( victim, 0, spl_hex, 0 );
    send_to_char( "You feel better.\n\r", victim );
        act("$n loses $s &+rred glow&n.",ch, NULL, NULL, TO_ROOM);
    yesno = 1;
    }
    
    if ( ch != victim && yesno )
        send_to_char( "Ok.\n\r", ch );
    } //end if(TYPE_DATA_CHAR)
    return;
}

void spell_negate_hex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_negate_hex");
    /* victim cursed. */
    if ( has_affect( victim, 0, spl_curse, 0 ) || has_affect( victim,
         0, spl_hex, 0) || has_affect( victim, 0,
         spl_malison, 0) )
    {
    affect_strip( victim, 0, spl_curse, 0 );
    affect_strip( victim, 0, spl_malison, 0 );
    affect_strip( victim, 0, spl_hex, 0 );
    send_to_char( "You feel better.\n\r", victim );
    }
    
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_remove_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_remove_silence");
    /*
     *  Note: because this spell is declared as TAR_CHAR_DEFENSIVE,
     *  do_cast will set vo = ch with no argument.  Since it is
     *  impossible for a silenced character to cast anyway, if
     *  victim == ch, that means there was no argument supplied
     *  (or the spellcaster tried to specify themself), i.e., trying
     *  to remove a "Cone of Silence" from ch->in_room.
     *  This might seem pointless, but it will work within obj_cast_spell.
     *  Thelonius (Monk) 5/94
     */
    if ( victim == ch )
    {
    ROOM_INDEX_DATA *pRoomIndex;
    bool             DidSomething;

    pRoomIndex   = ch->in_room;
    DidSomething = FALSE;

        if ( IS_ROOM( pRoomIndex, ROOM_SILENT ) )
        {
            REMOVE_ROOM_BIT( pRoomIndex, ROOM_SILENT );
            send_to_char( "You have lifted the cone of silence!\n\r", ch );
            act( "$n has lifted the cone of silence!",
                ch, NULL, NULL,   TO_ROOM );
            DidSomething = TRUE;
        }

    if ( is_affected( victim, AFF_MUTE ) )
        {
        affect_strip( victim, 0, spl_silence, 0 );
        send_to_char( "You lift the veil of silence from yourself.\n\r",
             ch );
        act( "$n lifts the veil of silence from $mself.",
        ch, NULL, victim, TO_VICT );
        DidSomething = TRUE;
    }

    if ( !DidSomething )
        send_to_char( "Fzzzzzzzzz...\n\r", ch );

    return;
    }

    if ( is_affected( victim, AFF_MUTE ) )
    {
    affect_strip( victim, 0, spl_silence, 0 );
    act( "You lift the veil of silence from $N.",
                    ch, NULL, victim, TO_CHAR    );
    act( "$n lifts the veil of silence from you.",
                    ch, NULL, victim, TO_VICT    );
    act( "$n lifts the veil of silence from $N.",
                    ch, NULL, victim, TO_NOTVICT );
    }
    else
    act( "$N is not silenced.",     ch, NULL, victim, TO_CHAR    );

    return;
}



void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_sanctuary");
    if ( is_affected( victim, AFF_SANCTUARY ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SANCTUARY);
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a white aura.\n\r", victim );
    act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_shield");
    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 8 + level;
    af.location  = APPLY_AC;
    af.modifier  = -10;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_DEX;
    af.modifier  = -10;
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a bulky force shield.\n\r", victim );
    act( "$n&n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_shadow_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_shadow_shield");
    if ( has_affect( victim, 0, sn, 0 ) )
      return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 10 + level / 2;
    af.location  = APPLY_AC;
    af.modifier  = -20;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    send_to_char( "&+LYou are surrounded by a shadowy shield.&n\n\r", victim );
    act( "&+L$n&+L is surrounded by a shadowy shield.&n", victim, NULL, NULL, TO_ROOM );
    return;
}

// Shocking Grasp - level 25 cap.  (2 * level)d3
// At level 25 or more: 50 minimum damage, 150 maximum, 100 average
// At level 11: 22d3, damage 22-66, average 44.
void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_shocking_grasp");
    if( level > 25 ) level = 25;

    dam = dice( (2 * level), 3 );

    if ( saves_spell( level, victim, DAM_ELECTRICITY ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );

    return;
}

void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  
    last_fun( "spell_sleep");
    if ( is_affected( victim, AFF_SLEEP )
    || saves_spell( level, victim, DAM_CHARM )
    || victim->race == RACE_VAMPIRE )
    {
      send_to_char( "You failed to put your victim to sleep.\n\r", ch );
      return;
    }

    af = create_affect( 0, spl_sleep, 0, level / 2, APPLY_NONE, 0, AFF_SLEEP );
    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
    send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", victim );
    if ( ch->fighting || victim->position == POS_FIGHTING )
       stop_fighting( victim, FALSE );
    do_sleep( victim, "" );
    }

    return;
}

void spell_minor_para( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  
    last_fun( "spell_minor_para");
    if ( is_affected( victim, AFF_MINOR_PARA )
    || saves_spell( level, victim, DAM_CHARM ))
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_PARA );

    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
    send_to_char( "&+YYou are paralyzed!&n\n\r", victim );
    if ( victim->position == POS_FIGHTING )
       stop_fighting( victim, TRUE );
    }

    return;
}

void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
//    AFFECT_DATA af;

    last_fun( "spell_entangle");

    spell_damage( ch, victim, dice( 2, 8 ), sn, DAM_ASPHYXIATION );
    if ( victim->position == POS_DEAD )
        return;
    if ( !saves_spell( level, victim, DAM_CHARM ))
    {
     victim->move = (int)(victim->move * 0.10);
    }
    else
    {
     victim->move = (int)(victim->move * 0.75);
    }

//    if ( !is_affected( victim, AFF_MINOR_PARA )
//    && !saves_spell( level, victim, DAM_CHARM ))
//    {
//      af.skill     = 0;
//     af.spell     = sn;
//      af.song      = 0;
//      af.duration  = level / 6;
//      af.location  = APPLY_NONE;
//      af.modifier  = 0;
//      set_bitvector( &af, AFF_MINOR_PARA );
//      affect_join( victim, &af );

//      if ( IS_AWAKE( victim ) )
//      {
//      send_to_char( "&+YYou are paralyzed!&n\n\r", victim );  
//      if ( victim->position == POS_FIGHTING )
//        stop_fighting( victim, TRUE );
//      }
//    }

    return;
}

// Illusionist spell... was previously the first version of  -- Veygoth
void spell_enthrall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    EVENT_DATA *event;
    CHAR_DATA *victim;
    AFFECT_DATA af;
  
    last_fun( "spell_enthrall");

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if (victim->fly_level != ch->fly_level || victim == ch
            || is_same_group( victim, ch ) )
          continue;
      if ( victim == ch || is_affected( victim, AFF_MINOR_PARA )
      || saves_spell( level, victim, DAM_CHARM ))
      {
        continue;
      }

    if ( IS_AWAKE( victim ) )
    {
      send_to_char( "&+YYou are enthralled!&n\n\r", victim );
        act( "&+Y$N&+Y is dazzled by the lights shooting from $n&+Y's fingertips!&n", ch, NULL, victim, TO_ROOM );
        act( "&+Y$N&+Y is dazzled by the lights shooting from $n&+Y's fingertips!&n", ch, NULL, victim, TO_CHAR );
      if ( victim->position == POS_FIGHTING )
         stop_fighting( victim, TRUE );
    }

      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = 1;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_MINOR_PARA );
      affect_join( victim, &af );

      for( event = event_first; event; event = event->next )
      {
         if( event->deleted )
             continue;
         if( event->type != EVENT_SPELL_CAST )
             continue;
         if( (CHAR_DATA *)event->arg1 != victim )
             continue;
         send_to_char( "You lose your concentration!\n\r", victim );
         act( "$n&n stops casting abruptly!", victim, NULL, NULL, TO_ROOM );
         event->deleted = TRUE;
      }

    }

    return;
}

void spell_earthen_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  
    last_fun( "spell_earthen_grasp");
    if ( is_affected( victim, AFF_MINOR_PARA )
    || saves_spell( level, victim, DAM_CRUSHING ))
    {
        spell_damage( ch, victim, level / 2, sn, DAM_CRUSHING );
        return;
    }

    spell_damage( ch, victim, level, sn, DAM_CRUSHING );
    if( victim->position == POS_DEAD || victim->deleted )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_PARA );
    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
    send_to_char( "&+yYou are paralyzed!&n\n\r", victim );
    act("$n is too busy worrying about their nervous system to fight!",
        victim, NULL, NULL, TO_ROOM);
    if ( victim->position == POS_FIGHTING )
       stop_fighting( victim, TRUE );
    }

    return;
}

void spell_gr_earthen_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    CHAR_DATA  *victim_next;
    AFFECT_DATA af;
   
    last_fun( "spell_gr_earthen_grasp");
    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;
      if (victim->fly_level != ch->fly_level || victim == ch )
          continue;
      if ( is_affected( victim, AFF_MINOR_PARA )
         || saves_spell( level, victim, DAM_CRUSHING ))
      {
          spell_damage( ch, victim, level, sn, DAM_CRUSHING );
          continue;
      }

      spell_damage( ch, victim, level, sn, DAM_CRUSHING );
      if( victim->position == POS_DEAD || victim->deleted )
          return;

      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = level / 6;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_MINOR_PARA );
      affect_join( victim, &af );

      if ( IS_AWAKE( victim ) )
      {
      send_to_char( "&+yYou are paralyzed!&n\n\r", victim );
      act("$n is too busy worrying about their nervous system to fight!",
          victim, NULL, NULL, TO_ROOM);
      if ( victim->position == POS_FIGHTING )
         stop_fighting( victim, TRUE );
      }
  }

    return;
}

void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_stone_skin");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    gain_exp( ch, 2 );

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + (level / 5);
    af.location  = 0;
    af.modifier  = (level / 4 + number_range( 1, 4 ));
    set_bitvector( &af, AFF_STONESKIN );
    affect_to_char( victim, &af );

    send_to_char( "&+LYour skin turns to stone.&n\n\r", victim );
    act( "$n&+L's skin turns to stone.&n", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    last_fun( "spell_summon");
    if ( !( victim = get_char_world( ch, target_name ) )
    || victim == ch
    || !victim->in_room
    || IS_ROOM( victim->in_room, ROOM_NO_SUMMON )
    || ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
    || victim->fighting
    || victim->in_room->area != ch->in_room->area
    || ( IS_NPC( victim ) && saves_spell( level, victim, DAM_OTHER ) ) )
    {
      send_to_char( "You failed.\n\r", ch );
      return;
    }

    if( !has_spell_consent( ch, victim ) )
    {
        return;
    }

    act( "$n&n disappears suddenly.", victim, NULL, NULL,     TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n&n has summoned you!",    ch,     NULL, victim,   TO_VICT );
    act( "$n&n arrives suddenly.",    victim, NULL, NULL,     TO_ROOM );
    do_look( victim, "auto" );
    return;
}
/*  Caster always has the possibility of ending up in same room */

void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    last_fun( "spell_teleport");

    if (victim != ch)
    {
        bug( "teleport: target is NULL or !caster", 0);
        victim = ch;
    }

    if( IS_ROOM( victim->in_room, ROOM_NO_TELEPORT))
    {
        send_to_char( "The magic of this room prevents you from leaving.\n\r", victim );
        return;
    }

    for ( ; ; )
    {
      pRoomIndex = get_room_index( number_range( 0, top_vnum_room ) );
      if ( pRoomIndex )
      {
        // Impassable rock on the UD map
        if( pRoomIndex->extra_sector_info == 92 ||
            pRoomIndex->extra_sector_info == 101 ||
            pRoomIndex->extra_sector_info == 102 ||
            pRoomIndex->extra_sector_info == 116 ||
            pRoomIndex->extra_sector_info == 130 ||
            pRoomIndex->extra_sector_info == 131 ||
            pRoomIndex->extra_sector_info == 132 ||
            pRoomIndex->extra_sector_info == 136 ||
            pRoomIndex->extra_sector_info == 137 )
        {
            continue;
        }
        if (   !IS_ROOM( pRoomIndex, ROOM_PRIVATE  )
        && !IS_ROOM( pRoomIndex, ROOM_SOLITARY )
        && !IS_ROOM( pRoomIndex, ROOM_NO_TELEPORT)
        && ( victim->in_room->area == pRoomIndex->area ) )
        {
            break;
        }
      }
    }

    if ( victim->fighting )
        stop_fighting( victim, TRUE );
    act( "$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n slowly fades into existence.",   victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
    return;
}

void spell_shadow_veil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    last_fun( "spell_shadow_veil");
    if ( !victim->in_room
    || ( !IS_NPC( ch ) && victim->fighting ) 
    || ( victim != ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if( IS_ROOM( victim->in_room, ROOM_NO_TELEPORT))
    {
        send_to_char( "The magic of this room prevents you from leaving.\n\r", victim );
        return;
    }

    for ( ; ; )
    {
      pRoomIndex = get_room_index( number_range( 0, top_vnum_room ) );
      if ( pRoomIndex )
      {
        /* If it's private/solitary/!tele, find another room. */
        if( !(   !IS_ROOM( pRoomIndex, ROOM_PRIVATE  )
              && !IS_ROOM( pRoomIndex, ROOM_SOLITARY ) 
              && !IS_ROOM( pRoomIndex, ROOM_NO_TELEPORT ) ) )
            continue;
        /* If it's in a hometown, find another room. */
        if(  pRoomIndex->sector_type == SECT_CITY 
          || pRoomIndex->sector_type == SECT_UNDERG_CITY )
            continue;
        /* Otherwise, it's a good room. */
        break;
      }
    }

    if ( victim->fighting )
        stop_fighting( victim, TRUE );
    act( "$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    spell_concealment( spl_concealment, level, ch, ch );
    do_look( victim, "auto" );
    return;
}

void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       buf2    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];
    char *target_name = (char *) vo;

    last_fun( "spell_ventriloquate");
    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER( buf1[0] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
    if ( !is_name( speaker, vch->name ) )
        send_to_char( saves_spell( level, vch, DAM_SOUND ) ? buf2 : buf1, vch );
    }

    return;
}

void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_weaken");
    if ( has_affect( victim, 0, sn, 0 )
      || saves_spell( level, victim, DAM_BLACK_MANA ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 3;
    af.location  = APPLY_STR;
    af.modifier  = -(5 + level / 3);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    if ( ch != victim )
    act( "$N&n looks a bit weaker.", ch, NULL, victim, TO_CHAR );
    send_to_char( "You feel weaker.\n\r", victim );
    return;
}

void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_feeblemind");
    if ( has_affect( victim, 0, sn, 0 )
      || saves_spell( level, victim, DAM_BLACK_MANA ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 4;
    af.location  = APPLY_INT;
    af.modifier  = 0 - ( level + 15 );
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_INT;
    af.modifier  = 0 - ( level + 5 );
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel _REALLY_ dumb.\n\r", victim );
    act("$n&n suddenly looks _REALLY_ dumb.", victim, NULL, NULL, TO_ROOM);
    return;
}

/*
 * This is for muds that want scrolls of recall.
 */
/* A character's chance of succeeding with the recall spell is based
 * on their skill with the spell and is reduced by 25% if they are
 * are in combat.  A successful recall costs the caster half of his
 * or her moves.  This spell should be a little less cheesy than on
 * most pkill MUDs.
 *
 * -- Veygoth
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *location;
    char             buf [ MAX_STRING_LENGTH ];
    int              place;
    int chance;

    last_fun( "spell_word_of_recall");
    act( "$n&n prays for transportation!", ch, NULL, NULL, TO_ROOM );

    if( IS_NPC( ch ))
        chance = (ch->level * 3 / 2) + 20;
    else
        chance = ch->pcdata->spl_lrn[spl_recall];

    // Subtle effects from other spells...
    if( is_affected( ch, AFF_CURSE ))
        chance -= 15;
    if( has_affect( ch, 0, spl_bless, 0 ))
        chance += 3;

    if ( IS_ROOM( ch->in_room, ROOM_NO_RECALL )
	|| is_affected( ch, AFF_CURSE ) )
    {
	send_to_char( "Your god has forsaken you.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) ) {
        place = ch->pcdata->currhome;
    }
    if ( IS_NPC( ch ) )
        place = repop_point[ch->race][ch->class];

    if ( !( location = get_room_index( place ) ) )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
    {
        send_to_char( "You're already home!\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) )
    {
        chance -= 25;

        if( chance < 5 ) chance = 10;

	if ( number_percent() > chance  )
	{
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed!\n\r" );
	    send_to_char( buf, ch );
	    return;
	}

	sprintf( buf, "You recall from combat!  Wuss!\n\r" );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }
    else if ( number_percent() > chance  )
    {
        WAIT_STATE( ch, 4 );
        sprintf( buf, "You failed!\n\r" );
        send_to_char( buf, ch );
        return;
    }

    ch->move /= 2;
    act( "$n&n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n&n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}

void spell_group_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *location;
    char             buf [ MAX_STRING_LENGTH ];
    int              place;
    int chance, chance2;
    CHAR_DATA *temp;

    last_fun( "spell_group_recall");
    act( "$n&n prays for transportation!", ch, NULL, NULL, TO_ROOM );

    if( IS_NPC( ch ))
        chance = (ch->level * 3 / 2) + 20;
    else
        chance = ch->pcdata->spl_lrn[sn];

    // Subtle effects from other spells...
    if( is_affected( ch, AFF_CURSE ))
        chance -= 15;
    if( has_affect( ch, 0, spl_bless, 0 ))
        chance += 3;

    if ( IS_ROOM( ch->in_room, ROOM_NO_RECALL )
	|| is_affected( ch, AFF_CURSE ) )
    {
	send_to_char( "Your god has forsaken you.\n\r", ch );
	return;
    }

    place = repop_point[ch->race][ch->class];

    if ( !( location = get_room_index( place ) ) )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
    {
        send_to_char( "You're already home!\n\r", ch );
	return;
    }

    // We use temp because removing the person from the room would totally FUBAR
    // the list of people in the room.
    for( victim = ch->in_room->people; victim; )
    {
      if( !is_same_group( victim, ch ))
      {
        victim = victim->next_in_room;
        continue;
      }

      chance2 = chance;
      if ( ( victim->fighting ) )
      {
        chance2 -= 25;
      }

      if( chance2 < 10 ) chance2 = 10;

      if ( number_percent() > chance2  )
      {
	sprintf( buf, "You failed to recall %s!\n\r", victim->name );
	send_to_char( buf, ch );
        send_to_char( "You feel a brief wrenching sensation.\n\r", victim );
        victim = victim->next_in_room;
        continue;
      }

      if( victim->fighting )
      {
        sprintf( buf, "You recall from combat!  Wuss!\n\r" );
        send_to_char( buf, victim );
      }

      stop_fighting( victim, TRUE );
      act( "$n&n disappears.", victim, NULL, NULL, TO_ROOM );
      temp = victim;
      victim = victim->next_in_room;
      char_from_room( temp );
      char_to_room( temp, location );
      act( "$n&n appears in the room.", temp, NULL, NULL, TO_ROOM );
      do_look( temp, "auto" );
    }

    ch->move /= 2;

    return;
}

/*
 * NPC spells.
 */

/*
 * Damage with 80K hitpoints, min is 2000 hp, max is 3000 hp
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_acid_breath");
    log_string("Spell acid_breath");
    if (  number_percent( ) < (2 * level) && !saves_breath( level, victim, DAM_ACID ) )
    {
      for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
      {
        int iWear;

        obj_next = obj_lose->next_content;
        if ( obj_lose->deleted )
            continue;
        if ( IS_OBJ_STAT( obj_lose, ITEM_MAGIC ) )
            continue;
        if ( is_artifact( obj_lose->pIndexData->vnum ) )
            continue;
        if ( !is_affected( victim, AFF_PROTECT_ACID ) && number_percent() < 50 )
            continue;

        if ( number_bits( 2 ) != 0 )
        continue;

        switch ( obj_lose->item_type )
        {
        case TYPE_ARMOR:
        if ( obj_lose->value[0] > 0 )
        {
            /* Correction on math by IvoryTiger */ 
            act( "$p&n is pitted and etched!",
            victim, obj_lose, NULL, TO_CHAR );
            if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
            victim->armor += apply_ac( obj_lose, iWear );
            obj_lose->value[0] -= 1;
            obj_lose->cost      = 0;
            if ( iWear != WEAR_NONE )
            victim->armor -= apply_ac( obj_lose, iWear );
        }
        break;

        case TYPE_CONTAINER:
        case TYPE_OTHER:
        act( "$p&n fumes and dissolves!",
            victim, obj_lose, NULL, TO_CHAR );
        if (obj_lose->wear_loc != WEAR_NONE)
            unequip_char( victim, obj_lose );
        obj_from_char( obj_lose );
        if( obj_lose->item_type == TYPE_CONTAINER )
        {
            OBJ_DATA *contents = obj_lose->contains;
            while( contents != NULL )
            {
                obj_from_obj( contents );
                act( "$p&n falls to the ground.", ch, obj_lose, NULL, TO_ROOM );
                act( "$p&n falls to the ground.", ch, obj_lose, NULL, TO_CHAR );
                obj_to_room( contents, ch->in_room );
                contents = obj_lose->contains;
            }
        }
        extract_obj( obj_lose );
        break;
        }
      }
    }

    dam = dragon_breath_damage( ch, 75 );

    if( saves_breath( level, victim, DAM_ACID ))
        spell_damage( ch, victim, dam / 2, sn, DAM_ACID );
    else
        spell_damage( ch, victim, dam, sn, DAM_ACID );

    return;
}

/*
 * At 80k hitpoints, min is 2400, max is 3600
 */
void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_fire_breath");
    log_string( "spell fire breath");
    if ( number_percent( ) < 2 * level && !saves_breath( level, victim, DAM_FIRE ) )
    {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
        char *msg;
        bool  burnable;

        obj_next = obj_lose->next_content;
        if ( obj_lose->deleted )
            continue;
        if ( IS_OBJ_STAT( obj_lose, ITEM_MAGIC ) )
            continue;
        if ( is_artifact( obj_lose->pIndexData->vnum ) )
            continue;
        if ( !is_affected( victim, AFF_PROTECT_FIRE ) && number_percent() < 50 )
            continue;

        burnable = FALSE;
        switch( obj_lose->material )
        {
            case MAT_CLOTH:
            case MAT_BARK:
            case MAT_SOFTWOOD:
            case MAT_HARDWOOD:
            case MAT_FLESH:
            case MAT_HIDE:
            case MAT_LEATHER:
            case MAT_CUREDLEATHER:
            case MAT_PAPER:
            case MAT_PARCHMENT:
            case MAT_LEAVES:
            case MAT_BAMBOO:
            case MAT_REEDS:
            case MAT_HEMP:
            case MAT_EGGSHELL:
            case MAT_GENERICFOOD:
            case MAT_FEATHER:
            case MAT_WAX:
            case MAT_SILK:
            case MAT_SPIDERSILK:
                burnable = TRUE;
            default:
            break;
        }
        if ( !burnable )
           continue;

        if ( IS_OBJ_STAT( obj_lose, ITEM_NOBURN ) || number_bits( 2 ) != 0 )
        continue;

        switch ( obj_lose->item_type )
        {
        default:             continue;
        case TYPE_CONTAINER: msg = "$p&n ignites and burns!";   break;
        case TYPE_POTION:    msg = "$p&n bubbles and boils!";   break;
        case TYPE_SCROLL:    msg = "$p&n crackles and burns!";  break;
        case TYPE_STAFF:     msg = "$p&n smokes and chars!";    break;
        case TYPE_WAND:      msg = "$p&n sparks and sputters!"; break;
        case TYPE_FOOD:      msg = "$p&n blackens and crisps!"; break;
        case TYPE_PILL:      msg = "$p&n melts and drips!";     break;
        case TYPE_OTHER:     msg = "$p&n crackles and burns!";  break;
        }

        act( msg, victim, obj_lose, NULL, TO_CHAR );
        if ( obj_lose->wear_loc != WEAR_NONE )
            unequip_char( victim, obj_lose );
        obj_from_char( obj_lose );
        if( obj_lose->item_type == TYPE_CONTAINER )
        {
            OBJ_DATA *contents = obj_lose->contains;
            while( contents != NULL )
            {
                obj_from_obj( contents );
                act( "$p&n falls to the ground.", ch, contents, NULL, TO_ROOM );
                act( "$p&n falls to the ground.", ch, contents, NULL, TO_CHAR );
                obj_to_room( contents, ch->in_room );
                contents = obj_lose->contains;
            }
        }
        extract_obj( obj_lose );
    }
    }

      dam = dragon_breath_damage( ch, 100 );

    if ( saves_breath( level, victim, DAM_FIRE ) )
      dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}


/*
 * At 80k hitpoints, damage is min 2400 and max 3600
 */
void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_frost_breath");
    log_string( "spell frost breath");
    if ( number_percent( ) < 2 * level && !saves_breath( level, victim, DAM_COLD ) )
    {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
        char *msg;

        obj_next = obj_lose->next_content;
        if ( obj_lose->deleted )
            continue;
        if ( number_bits( 2 ) != 0 )
        continue;
        if ( is_artifact( obj_lose->pIndexData->vnum ) )
            continue;
        if ( !is_affected( victim, AFF_PROTECT_COLD ) && number_percent() < 50 )
            continue;

        switch ( obj_lose->item_type )
        {
        default:            continue;
        case TYPE_CONTAINER:
        case TYPE_DRINK_CON:
        case TYPE_POTION:   msg = "$p&n freezes and shatters!"; break;
        }

        act( msg, victim, obj_lose, NULL, TO_CHAR );
        if (obj_lose->wear_loc != WEAR_NONE)
            unequip_char( victim, obj_lose );
        obj_from_char( obj_lose );
        if( obj_lose->item_type == TYPE_CONTAINER )
        {
            OBJ_DATA *contents = obj_lose->contains;
            while( contents != NULL )
            {
                obj_from_obj( contents );
                act( "$p&n falls to the ground.", ch, contents, NULL, TO_ROOM );
                act( "$p&n falls to the ground.", ch, contents, NULL, TO_CHAR );
                obj_to_room( contents, ch->in_room );
                contents = obj_lose->contains;
            }
        }
        extract_obj( obj_lose );
    }
    }

      dam = dragon_breath_damage( ch, 100 );

    if ( saves_spell( level, victim, DAM_COLD ) ) {
      dam /= 2;
      sprintf( lbuf, "%s made a save against %s&n's breath", victim->name, ch->short_descr);
    }
    else
      sprintf( lbuf, "%s failed a save against %s&n's breath", victim->name, ch->short_descr);
    log_string( lbuf );
    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

/*
 * At 80k hitpoints, damage is min 2000 and max 3000
 */
void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    int        dam;

    last_fun( "spell_gas_breath");
    log_string( "spell gas breath" );

      dam = dragon_breath_damage( ch, 75 );

    if ( saves_breath( level, victim, DAM_GAS ) )
      dam /= 2;
    spell_poison( spl_poison, level, ch, victim );
    spell_damage( ch, victim, dam, sn, DAM_GAS );

    return;
}

/*
 * At 80k hitpoints, damage is min 2400 and max 3600
 */
void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_lightning_breath");
    log_string( "spell lightning breath ");

      dam = dragon_breath_damage( ch, 100 );

    if ( saves_breath( level, victim, DAM_ELECTRICITY ) )
      dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );

    return;
}

/*
 * At 80k hitpoints, PC damage is min 640 and max 960
 */
void spell_water_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_water_breath");
    log_string(" spell water breath ");

      dam = dragon_breath_damage( ch, 100 );

    if ( saves_breath( level, victim, DAM_WATER ) )
      dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_WATER );

    return;
}

/*
 * Code for Psionicist spells/skills by Thelonius
 */
void spell_adrenaline_control ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_adrenaline_control");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill        = 0;
    af.spell        = sn;
    af.song         = 0;
    af.duration     = (level / 2) + 1;
    af.location     = APPLY_DEX;
    af.modifier     = 4 + (level / 7);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location    = APPLY_CON;
    affect_to_char( victim, &af );

    send_to_char( "You have given yourself an adrenaline rush!\n\r", ch );
    act( "$n&n looks more alert and ready for action.", ch, NULL, NULL, TO_ROOM );
   
    return;
}

// Agitation - (2 * level)d2

void spell_agitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_agitation");
    dam = dice( (2 * level), 2 );

    if ( saves_spell( level, victim, DAM_MENTAL ) )
      dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_MENTAL );

    return;
}


//Saeven 8-00
// Modified by Lohrr
void spell_aura_sight ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_aura_sight");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 8+level);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 8 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_EVIL);
    affect_to_char( victim, &af );
	set_bitvector( &af, AFF_DETECT_GOOD);
    affect_to_char( victim, &af );
    if( level > 20 )
    {
	set_bitvector( &af, AFF_DETECT_MAGIC );
        affect_to_char( victim, &af );
    }
    if( level > 50 )
    {
	set_bitvector( &af, AFF_SENSE_LIFE );
        affect_to_char( victim, &af );
    }
/* Just a tad too much - Lithos
    if( level > 40 )
    {
	set_bitvector( &af, AFF_DETECT_INVIS );
        affect_to_char( victim, &af );
    }
    if( level > 45 )
    {
	set_bitvector( &af, AFF_ULTRAVISION );
        affect_to_char( victim, &af );
    }
*/

    send_to_char( "&+LYour eyes are endowed with a strange &+Mdark &+mhued &+Lglow.\n\r", victim );

    return;
}

void spell_awe ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun( "spell_awe");
    if ( victim->fighting == ch && !saves_spell( level, victim, DAM_MENTAL ) )
    {
        stop_fighting ( victim, TRUE);
        if ( victim->hating->who == ch )
            stop_hating ( victim );
        act( "$N&n is in AWE of you!", ch, NULL, victim, TO_CHAR    );
        act( "You are in AWE of $n!",ch, NULL, victim, TO_VICT    );
        act( "$N&n is in AWE of $n!",  ch, NULL, victim, TO_NOTVICT );
    }
    return;
}

// Ballistic Attack
// Damage 4d2 + 2*lvl (min: 24, max: 28,avg: 26 at lvl 10)
void spell_ballistic_attack ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int        dam;

    last_fun( "spell_ballistic_attack");
    if( level > 10 )
      level = 10;

    dam = dice( 4, 2 ) + (level * 2);
    
    if ( saves_spell( level, victim, DAM_BASH ) )
      dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_BASH );

    return;
}


/* This needs to be fixed */
void spell_biofeedback ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_biofeedback");

    (*spells_table[ spl_sanctuary ].spell_fun) ( sn, level, ch, vo );

    return;
}

void spell_cell_adjustment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
   
    last_fun("spell_cell_adjustment");
    if ( has_affect( victim, 0, spl_poison, 0 ) )
    {
      affect_strip( victim, 0, spl_poison, 0 );
      send_to_char( "A warm feeling runs through your body.\n\r", victim );
      act( "$N&n looks better.", ch, NULL, victim, TO_NOTVICT );
    }
    if ( has_affect( victim, 0, spl_curse, 0  ) && (number_percent() < ch->level * 3 ))
    {
      affect_strip( victim, 0, spl_curse, 0  );
      send_to_char( "You feel better.\n\r", victim );
    }    
    if ( has_affect( victim, 0, spl_malison, 0  ) && (number_percent() < ch->level * 3 ))
    {
      affect_strip( victim, 0, spl_malison, 0  );
      send_to_char( "You feel better.\n\r", victim );
    }    
    if ( has_affect( victim, 0, spl_hex, 0  ) && (number_percent() < ch->level * 3 ))
    {
      affect_strip( victim, 0, spl_hex, 0  );
      send_to_char( "You feel better.\n\r", victim );
    }    
    if ( has_affect( victim, 0, spl_plague, 0 ) && (number_percent() < ch->level * 2 ))
    {
      affect_strip( victim, 0, spl_plague, 0 );
      send_to_char( "Your plague sores seem to disappear.\n\r", victim );
    }
    send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_combat_mind ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_combat_mind");
    if ( has_affect( victim, 0, sn, 0 ) )
    {
    if ( victim == ch )
      send_to_char( "You already understand battle tactics.\n\r",
               victim );
    else
      act( "$N already understands battle tactics.",
          ch, NULL, victim, TO_CHAR );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level + 6;
    af.location     = APPLY_HITROLL;
    af.modifier     = 1 + level / 7;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location     = APPLY_DAMROLL;
    af.modifier     = 2 + level / 25;
    affect_to_char( victim, &af );

    if ( victim != ch )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You gain a keen understanding of battle tactics.\n\r",
         victim );
    return;
}



void spell_complete_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    last_fun("spell_complete_healing");
    if( victim->hit < get_max_hit( victim ) )
      victim->hit = get_max_hit( victim );
    update_pos( victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "Ahhhhhh...You are completely healed!\n\r", victim );
    return;
}


void spell_control_flames ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun("spell_control_flames");
    send_to_char("That spell has been disabled.\n\r", ch);
    return;
/*
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    OBJ_DATA *obj;
    static const int        dam_each [ ] = 
    {
     0,
     0,  0,  0,  0,  0,       0,  0, 16, 20, 24,
    28, 32, 35, 38, 40,      42, 44, 45, 45, 45,
    46, 46, 46, 47, 47,      47, 48, 48, 48, 49,
    49, 49, 50, 50, 50,      51, 51, 51, 52, 52,
    52, 53, 53, 53, 54,      54, 54, 55, 55, 55
    };
         int        dam;

    if ( !(obj = get_eq_char( ch, WEAR_HAND )) && obj->item_type !=
         TYPE_LIGHT )
    {
    send_to_char( "You must be carrying a light source.\n\r", ch );
    return;
    }

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam         = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim, DAM_FIRE ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
*/
}

void spell_create_sound ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch, *victim;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];
    char *arg = (char *) vo;

    last_fun( "spell_create_sound" );

    arg = one_argument( arg, speaker );

    if( !( victim = get_char_room( ch, speaker ) ) )
    {
        send_to_char( "You don't see that person here.\n\r", ch );
        return;
    }

    if ( saves_spell( level, victim, DAM_SOUND ) )
        sprintf( buf1, "Someone makes %s say '&n$T&n'.\n\r", IS_NPC( victim ) ?
                 victim->short_descr : victim->name );
    else
        sprintf( buf1, "%s says '&n$T&n'", IS_NPC( victim ) ? 
                 victim->short_descr : victim->name );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
        if ( vch == victim )
        {
            act( "Someone makes you say '&n$T&n'", vch, NULL, arg, TO_CHAR );
        }
        else if ( vch == ch )
        {
            act( "You make $N say '&n$t&n'", vch, arg, victim, TO_CHAR );
        }
        else
            act( buf1, vch, NULL, arg, TO_CHAR );
    }

    return;
}

void spell_death_field ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;

    last_fun("spell_death_field");
    send_to_char( "&+LYou send out a wave of psionic death!&n\n\r", ch );
    act ( "$n sends out a wave of psionic death!&n!", ch, NULL, ch, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( vch->deleted || is_same_group( ch, vch ))
          continue;

        if (vch->fly_level != ch->fly_level) continue;

        dam = dice( (6 * level), 2 );

        if ( saves_spell( level, vch, DAM_BLACK_MANA ))
        {
          dam /= 2;
        }

        send_to_char( "The haze envelops you!\n\r", vch );
        spell_damage( ch, vch, dam, sn, DAM_BLACK_MANA );
    }

    for ( vch = ch->in_room->people; vch; vch = vch_next ) {
        if (vch->deleted || !vch->in_room || IS_NPC( vch ) ) continue;
            if ( vch->in_room->area == ch->in_room->area && IS_AWAKE( vch ) )
            send_to_char( "&+LYour brain hurts as a black haze fills the sky!&n\n\r", vch );
    }
    return;
}

// Detonate - level 40 cap.  (7 * level)d2 + 5 * level

void spell_detonate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int dam;

    last_fun("spell_detonate");
    dam = dice( (7 * level), 2 ) + 5 * level;

    if ( saves_spell( level, victim, DAM_ENERGY ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_ENERGY );
    return;
}

// Neural Fragmentation

// Cap of 900, average of 600 at level 50, minus victim's int
// damage is reduced by 1 per int of victim.
// likely damage will be 130-150 against players
void spell_neurofrag ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_neurofrag");
    if (level > 50)
        level = 50;

    dam = dice( 100, 5 ) + (level * 6);

    dam -= get_curr_int( ch );

    if ( saves_spell( level, victim, DAM_ENERGY ) )
        dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_ENERGY );
    return;
}

// Disintegrate - no level cap - (6 * (level-5))d2
// At level 50: 270 minimum, 540 maximum, 405 average

void spell_disintegrate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    int dam;

    last_fun("spell_disintegrate");
    if ( number_percent( ) < level - 10
    && !saves_spell( level, victim, DAM_ENERGY ) )
      for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
      {
      obj_next = obj_lose->next_content;
      if ( obj_lose->deleted )
          continue;

      if ( number_bits( 2 ) != 0 )
          continue;

      if ( insult_artifact( ch, obj_lose ) )
          return;
      act( "$p&+G disintegrates!&n", victim, obj_lose, NULL, TO_CHAR );
      act( "$p&+G disintegrates!&n", victim, obj_lose, NULL, TO_ROOM );
        if (obj_lose->wear_loc != WEAR_NONE)
            unequip_char( victim, obj_lose );
        obj_from_char( obj_lose );
      extract_obj( obj_lose ) ;
      }

    dam = dice( (6 * (level-5)), 2);
    if ( !saves_spell( level, victim, DAM_ENERGY ) )
    {
        dam /= 2;
    }
    spell_damage( ch, victim, dam, sn, DAM_DISINTEGRATION );
    return;
}

void spell_displacement ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_displacement");
    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level;
    af.location     = APPLY_AC;
    af.modifier     = - (level / 2);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "Your form shimmers, and you appear displaced.\n\r",
         victim );
    act( "$N shimmers and appears in a different location.",
    ch, NULL, victim, TO_NOTVICT );
    return;
}



void spell_domination ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    CHAR_DATA *vch;

    last_fun("spell_domination");
    if( level > 25 )
      level = 25;

    if ( is_affected( ch, AFF_CHARM) ) {
        send_to_char("You cannot force your will when it isnt free.\n\r", ch);
        return;
    }

    if ( victim == ch ) {
        send_to_char( "Dominate yourself?  You're weird.\n\r", ch );
        return;
    }

    for (vch = char_list; vch; vch = vch->next) {
        if (vch->deleted) continue;
        if (vch->master == ch && is_affected(vch, AFF_CHARM) ) {
            send_to_char("You cannot control more than one creature at a time!\n\r", ch);
            return;
        }
    }

    if (   is_affected( victim, AFF_CHARM ) ) {
        send_to_char("That creature has eyes for another.\n\r", ch);
        return;
    }
  
    if (level < victim->level
      || saves_spell( level, victim, DAM_CHARM ) ) {
        send_to_char("Uh oh, you made them mad now!\n\r", ch);
        if (can_see( victim, ch))
            set_fighting( victim, ch );
        else
            start_grudge( victim, ch, FALSE );
        return;
    }

    if ( victim->master )
        stop_follower( victim );
    add_follower( victim, ch );

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = number_fuzzy( level / 4 );
    af.location     = APPLY_NONE;
    af.modifier     = 0;
    set_bitvector( &af, AFF_CHARM);
    affect_to_char( victim, &af );

    act( "Your will dominates $N!", ch, NULL, victim, TO_CHAR );
    act( "Your will is dominated by $n!", ch, NULL, victim, TO_VICT );
    return;
}

void spell_ectoplasmic_form ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_ectoplasmic_form");
    if ( is_affected( victim, AFF_PASS_DOOR ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = number_fuzzy( level / 4 );
    af.location     = APPLY_NONE;
    af.modifier     = 0;
    set_bitvector( &af, AFF_PASS_DOOR);
    affect_to_char( victim, &af );

    send_to_char( "You turn translucent.\n\r", victim );
    act( "$n&n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}

// Ego Whip - 1d8+1 damage

void spell_ego_whip ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_ego_whip");
    dam = dice( 1, 8 ) + 1;

    if ( saves_spell( level, victim, DAM_MENTAL ) )
       dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_MENTAL );

    return;
}

void spell_energy_containment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_energy_containment");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level / 2 + 7;
    af.modifier     = -(level / 4);
    af.location  = APPLY_SAVING_SPELL;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "You can now absorb some forms of energy.\n\r", ch );
    return;
}

void spell_enhance_armor (int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    last_fun("spell_enhance_armor");
    if ( obj->item_type != TYPE_ARMOR
    || IS_OBJ_STAT( obj, ITEM_MAGIC )
    || obj->affected )
    {
        send_to_char( "That item cannot be enhanced.\n\r", ch );
        return;
    }

    paf            = new_affect();

    paf->spell          = 0;
    paf->skill        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_AC;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;

    if ( number_percent() < ch->pcdata->spl_lrn[sn]/2
    + 3 * ( ch->level - obj->level ) )

    /* Good enhancement */
    {
    paf->modifier   = -level / 12;

    if ( IS_GOOD( ch ) )
    {
        SET_OBJ_STAT( obj, ITEM_ANTI_EVIL );
        act( "$p&n glows blue.",   ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL( ch ) )
    {
        SET_OBJ_STAT( obj, ITEM_ANTI_GOOD );
        act( "$p&n glows red.",    ch, obj, NULL, TO_CHAR );
    }
    else
    {
        SET_OBJ_STAT( obj, ITEM_ANTI_EVIL );
        SET_OBJ_STAT( obj, ITEM_ANTI_GOOD );
        act( "$p&n glows yellow.", ch, obj, NULL, TO_CHAR );
    }
       
    send_to_char( "Ok.\n\r", ch );
    }
    else
    /* Bad Enhancement ... opps! :) */
    {
    paf->modifier   = level / 12;
    obj->cost       = 0;

    SET_OBJ_STAT( obj, ITEM_NODROP );
    act( "$p&n &+Lturns black.&n", ch, obj, NULL, TO_CHAR );
    }

    return;
}

void spell_enhanced_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_enhanced_strength");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = (level / 2) + 8;
    af.location     = APPLY_STR;
    af.modifier     = 5 + ( level / 6 );
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "Strength expands outward from your mind and encompasses your whole body.\n\r", victim );
    return;
}

void spell_flesh_armor ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_flesh_armor");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level;
    af.location     = APPLY_AC;
    af.modifier     = -(((level-1)/5) + 14);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "Your flesh turns to steel.\n\r", victim );
    act( "$N's flesh turns to steel.", ch, NULL, victim, TO_NOTVICT);
    return;
}

void spell_inertial_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;

    last_fun("spell_inertial_barrior");
   
    if (has_affect( ch, 0, sn, 0) ) return; 
    send_to_char( "An inertial barrier forms around you.\n\r", ch );

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 6;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    set_bitvector( &af, AFF_INERTIAL_BARRIER );
    affect_to_char( ch, &af );
    return;
}

// Inflict Pain - (3 * level)d4 + 80
void spell_inflict_pain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_inflict_pain");
    if( level > 40 ) level = 40;

    dam = dice( (3 * level), 4 ) + 80;

    if ( saves_spell( level, victim, DAM_MENTAL ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_MENTAL );

    return;

}

void spell_intellect_fortress ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_intellect_fortress");

    send_to_char( "A virtual fortress forms around you.\n\r", gch );
    act( "A virtual fortress forms around $N&n.", gch, NULL, gch, TO_ROOM );

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 15 + level / 6;
    af.location  = APPLY_AC;
    af.modifier  = - (get_curr_int( ch ) / 5);
    set_bitvector( &af, AFF_NONE);

    affect_to_char( gch, &af );

    return;
}

void spell_transfer_wellness ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        hpch;

    last_fun("spell_transfer_wellness");
    if ( ch == victim )
    {
    send_to_char( "You transfer your health to yourself.  Waste of a spell.\n\r", ch );
    return;
    }

    hpch = UMIN( 40, get_max_hit( victim ) - victim->hit );
    {
    act( "Nice thought, but $N&n doesn't need healing.", ch, NULL,
        victim, TO_CHAR );
    return;
    }
    if ( ch->hit-hpch < 40 )
    {
        send_to_char( "You aren't well enough yourself!\n\r", ch );
        return;
    }

    victim->hit += hpch;
    ch->hit     -= hpch;
    update_pos( victim );
    update_pos( ch );

    act( "You transfer some of your lifeforce to $N&n.", ch, NULL, victim, TO_CHAR );
    act( "$n&n lends you some of $s lifeforce.",     ch, NULL, victim, TO_VICT );

    return;
}

void spell_lend_health ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        hpch;

    last_fun("spell_lend_health");
    if ( ch == victim )
    {
        send_to_char( "Lend health to yourself?  What a weirdo.\n\r", ch );
        return;
    }
    hpch = UMIN( 50, get_max_hit( victim ) - victim->hit );
    if ( hpch == 0 )
    {
    act( "Nice thought, but $N doesn't need healing.", ch, NULL,
        victim, TO_CHAR );
    return;
    }
    if ( ch->hit-hpch < 50 )
    {
        send_to_char( "You aren't healthy enough yourself!\n\r", ch );
        return;
    }
    victim->hit += hpch;
    ch->hit     -= hpch;
    update_pos( victim );
    update_pos( ch );

    act( "You lend some of your health to $N.", ch, NULL, victim, TO_CHAR );
    act( "$n lends you some of $s health.",     ch, NULL, victim, TO_VICT );

    return;
}

void spell_levitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_levitation");
    if ( is_affected( victim, AFF_LEVITATE ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level + 3;
    af.location     = APPLY_NONE;
    af.modifier     = 0;
    set_bitvector( &af, AFF_LEVITATE );
    affect_to_char( victim, &af );

    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_mental_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_mental_barrier");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;

    set_bitvector( &af, AFF_PROTECT_COLD);
    if( !is_affected( victim, AFF_PROTECT_COLD ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_PROTECT_FIRE);
    if( !is_affected( victim, AFF_PROTECT_FIRE ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_PROTECT_GAS);
    if( !is_affected( victim, AFF_PROTECT_GAS ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_PROTECT_ACID);
    if( !is_affected( victim, AFF_PROTECT_ACID ))
      affect_to_char( victim, &af );

    set_bitvector( &af, AFF_PROTECT_LIGHTNING);
    if( !is_affected( victim, AFF_PROTECT_LIGHTNING ))
      affect_to_char( victim, &af );

    send_to_char( "You erect a mental barrier around yourself.\n\r",
         victim );
    return;
}

// Mind Thrust - (2 * level)d3 + 40

void spell_mind_thrust ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_mind_thrust");
    if( level > 30 ) level = 30;

    dam = dice( (2 * level), 3 ) + 40;

    if ( saves_spell( level, victim, DAM_MENTAL ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_MENTAL );

    return;
}

// Project Force - same as Inflict Pain, but does bashing damage instead of mental
// (3 * level)d4 + 80

void spell_project_force ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_project_force");
    if( level > 40 ) level = 40;

    dam = dice( (3 * level), 4 ) + 80;

    if ( saves_spell( level, victim, DAM_BASH ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_BASH );

    return;

}

void spell_psionic_blast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    last_fun("spell_psionic_blast");
    if( level > 45 ) level = 45;

    dam = dice( (3 * level), 4 ) + 2 * level + 100;

    if ( saves_spell( level, victim, DAM_MENTAL ) )
    dam /= 2;
    spell_damage( ch, victim, dam, sn, DAM_MENTAL );

    return;

}

// Psychic Crush - Not currently available

void spell_psychic_crush ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun("spell_psychic_crush");
    spell_damage( ch, (CHAR_DATA *) vo, dice( 3, 40 ) + level, sn, 
       DAM_MENTAL );
    return;
}

void spell_psychic_drain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_psychic_drain");
    if ( has_affect( victim, 0, sn, 0 )
      || saves_spell( level, victim, DAM_MENTAL ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level / 2;
    af.location     = APPLY_MANA;
    af.modifier     = - (victim->mana / (number_range( 2, 5 )));
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    ch->mana -= af.modifier;

    send_to_char( "You feel drained.\n\r", victim );
    act( "$n&n appears drained of power.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_psychic_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    last_fun("spell_psychic_healing");
    heal = dice( 3, 6 ) + 2 * level / 3 ;
    if( victim->hit < get_max_hit( victim ) )
      victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );
    update_pos( victim );

    send_to_char( "You feel better!\n\r", victim );
    return;
}

void spell_share_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_shar_strength");
    if ( victim == ch )
    {
    send_to_char( "You can't share strength with yourself.\n\r", ch );
    return;
    }
    if ( has_affect( victim, 0, sn, 0 ) )
    {
    act( "$N&n already shares someone's strength.", ch, NULL, victim,
        TO_CHAR );
    return;
    }
    if ( get_curr_str( ch ) <= 5 )
    {
    send_to_char( "You are too weak to share your strength.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level;
    af.location     = APPLY_STR;
    af.modifier     =  5 + ( level / 4 );
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    
    af.modifier     = -5 - ( level / 4 );
    affect_to_char( ch,     &af );

    act( "You share your strength with $N&n.", ch, NULL, victim, TO_CHAR );
    act( "$n&n shares $s strength with you.",  ch, NULL, victim, TO_VICT );
    return;
}

void spell_thought_shield ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun("spell_thought_shield");
    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration     = level + 2;
    af.location     = APPLY_AC;
    af.modifier     = -10;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "You have created a shield around yourself.\n\r", ch );
    return;
}

void spell_ultrablast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    last_fun("spell_ultrablast");
    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
    if ( vch->deleted )
        continue;

    if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
    {
        hpch = UMAX( 10, ch->hit );
        dam  = number_range( hpch / 8+1, hpch / 4 );
        if ( saves_spell( level, vch, DAM_MENTAL ) )
            dam /= 2;
        spell_damage( ch, vch, dam, sn, DAM_MENTAL );
    }
    }
    return;
}

void spell_vampiric_bite( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    OBJ_DATA   *obj;
    AFFECT_DATA af;
    int         dam;

    last_fun("spell_vampiric_bite");
    dam = dice( 5, level );
    spell_damage( ch, victim, dam, sn, DAM_DISEASE );
    ch->hit = UMIN( get_max_hit( ch ), ch->hit + dam );

    if ( victim->level < 11 || get_age( victim ) < 21 || victim->position  == POS_DEAD )
        return;

    if ( is_affected( victim, AFF_POLYMORPH ) )
        return;

    if ( saves_spell( level, victim, DAM_DISEASE )
    || number_bits( 1 ) == 0 )
    return;

    if ( ( obj = get_eq_char( victim, ITEM_HOLD ) ) )
    {
       if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
       {
            if ( ch->level < 31 )
        {
                    send_to_char( "You are repelled by the holy item in their hand!\n\r", ch );
            return;
        }
        else
        {
            if ( victim->level > ch->level )
                return;
        }
    }
    }
          
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = UMAX( 5, 30 - level );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_VAMP_BITE);
    affect_join( victim, &af );

    if ( ch != victim )
    send_to_char( "Ahh!  Taste the power!\n\r", ch );
    send_to_char( "Your blood begins to burn!\n\r", victim );
    return;
}
/* LLLL */  // What's this??

void spell_turn_undead( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;

    if (   victim->level >= level
    || saves_spell( level, victim, DAM_WHITE_MANA ) )
    {
        send_to_char( "You have failed.\n\r", ch );
    return;
    }

    if ( IS_UNDEAD( victim ))
    {
        spell_fear( spl_fear, (level + 20), ch, victim );
        do_flee( victim, "" );
    }

    return;

}

void spell_destroy_undead( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice( (8 * level), 3);

    if( dam > victim->hit )
        dam = UMAX( 0, victim->hit - dice( 1, 5 ) );

    if ( IS_UNDEAD( victim ))
    {
         if( !saves_spell( level, victim, DAM_WHITE_MANA ))
         {
           spell_damage( ch, victim, dam, sn, DAM_WHITE_MANA );
         }
         else
         {
           spell_damage( ch, victim, dam / 4, sn, DAM_WHITE_MANA );
         }
    }
    else
         send_to_char( "Nothing happens.\n\r", ch );

    return;
}

void spell_exorcise( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !has_affect( victim, gsn_vampiric_bite, 0, 0 ) )
        return;

    /*
     * We actually give the vampiric curse a chance to save
     * at the victim's level
     */
    if ( !saves_spell( level, victim, DAM_DISEASE ) )
        return;

    affect_strip( victim, 0, gsn_vampiric_bite, 0 );

    send_to_char( "Ok.\n\r",                                    ch     );
    send_to_char( "A warm feeling runs through your body.\n\r", victim );
    act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );

    return;

}

/* Flame shield spell from Malice of EnvyMud */
void spell_flaming( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_FLAMING ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_FLAMING);
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a flaming shield.\n\r", victim );
    act( "$n is surrounded by a flaming shield.", victim, NULL, NULL, TO_ROOM );
    return;

}


/*
 * ------------------------- by Zen ----------------------------------
 */


// Meteor Swarm.  No cap.  (level*2)d9.  Area Effect.
// Level 50.  Minimum 100, maximum 900, average 450

void spell_meteor_swarm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = NULL;
    int       dam;

    if ( !IS_OUTSIDE( ch ) )
    {
      send_to_char( "You must be out of doors.\n\r", ch );
      return;
    }


    send_to_char( "&+rMe&+Rt&N&+re&+Ror&N&+rs&N are falling from the sky!\n\r", ch );
    act( "$n summons the wrath of a &+rme&+Rt&N&+re&+Ror&N &+yswarm&N!", ch, NULL, NULL, TO_ROOM );
    if (ch->fly_level < MAX_FLY ) 
        act("A &+rme&+Rt&N&+re&+Ror&N &+yswarm&N starts pelting the area below you.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
        act("A &+rme&+Rt&N&+re&+Ror&N &+yswarm&N starts pelting the area above you.", ch, NULL, NULL, TO_BELOW);
    dam = dice((level * 2), 9);
    if (saves_spell(level, victim, DAM_CRUSHING))
      dam /= 2;
    do_area_spell_damage(ch, sn, 5 + UMAX(0, level - 50), dam, 0);
    return;
}

// Relocate
void spell_vortex_lift( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    if( !(victim = get_char_world( ch, target_name )))
    {
      send_to_char( "You failed.\n\r", ch );
      return;
    }

    if( !has_spell_consent( ch, victim ) )
    {
        return;
    }

    if( IS_ROOM( ch->in_room, ROOM_NO_TELEPORT )
     || IS_ROOM( victim->in_room, ROOM_NO_TELEPORT ))
    {
      send_to_char( "The magic in this area prevents you from leaving.\n\r", ch );
      return;
    }

    if ( victim == ch
    || !victim->in_room
    || ch->in_room == victim->in_room
    || IS_ROOM( victim->in_room, ROOM_SAFE      )
    || IS_ROOM( victim->in_room, ROOM_PRIVATE   )
    || IS_ROOM( victim->in_room, ROOM_SOLITARY  )
    || IS_NPC( victim )
    || ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO ) ) // no relocate to immorts
    {
      send_to_char( "You failed.\n\r", ch );
      return;
    }

    act( "$n&n leaves in a &+Cs&N&+cw&+wi&+Wr&N&+wl&N&+ci&+Cn&N&+cg &+Lv&N&+wo&+Wrt&N&+we&+Lx.&N", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    if ( can_fly( ch ) ) ch->fly_level = victim->fly_level;
    act( "$n&n appears in a &+csw&+Ci&+Wr&N&+Cl&n&+cing &+Lv&N&+wo&+Wrt&N&+we&+Lx&N.", ch, NULL, NULL, TO_ROOM );
    if ( !is_fly_room( ch->in_room ) && ch->fly_level ) {
        send_to_char( "&+CYou come tumbling down!&N", ch );
        act("$n tumbles in from above.", ch, NULL, NULL, TO_ROOM );
        ch->position = number_range( POS_RECLINING, POS_KNEELING );
    }
    WAIT_STATE( ch, 12 );

    do_look( ch, "auto" );
    return;
}

void spell_dimdoor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    if ( !( victim = get_char_area( ch, target_name ) )
    || victim == ch
    || !victim->in_room )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if( !has_spell_consent( ch, victim ) )
    {
        return;
    }

    if( IS_ROOM( ch->in_room, ROOM_NO_TELEPORT )
     || IS_ROOM( victim->in_room, ROOM_NO_TELEPORT ))
    {
      send_to_char( "The magic in this area prevents you from leaving.\n\r", ch );
      return;
    }
/*
    if (IS_ROOM( ch->in_room, ROOM_NO_TELEPORT) ) {
        send_to_char( "You are prevented from casting that spell here!\n\r", ch);
        return;
    }
*/
    if ( IS_ROOM( victim->in_room, ROOM_SAFE      )
    || IS_ROOM( victim->in_room, ROOM_PRIVATE   )
    || IS_ROOM( victim->in_room, ROOM_SOLITARY  ))
    {
        send_to_char( "Something prevents you from going there.\n\r", ch );
        act("$n&n creates a momentary ripple in space.", ch, NULL, NULL, TO_ROOM);
        return;
    }

    act( "$n&n creates a dimensional portal and walks through it.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act( "&+L$n&n&+L steps out of a portal, grinning wickedly.&n", ch, NULL, NULL, TO_ROOM );
    if ( can_fly( ch ) ) ch->fly_level = victim->fly_level;
    if ( !is_fly_room( ch->in_room ) && ch->fly_level ) {
        send_to_char( "You come tumbling down!", ch );
        act("$n tumbles in from above.", ch, NULL, NULL, TO_ROOM );
        ch->position = number_range( POS_RECLINING, POS_KNEELING );
    }
    WAIT_STATE( ch, 8 );

    do_look( ch, "auto" );
    return;
}

void spell_spirit_jump( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    if( IS_ROOM( ch->in_room, ROOM_NO_TELEPORT ))
    {
      send_to_char( "The magic in this area prevents you from leaving.\n\r", ch );
      return;
    }

    if ( !( victim = get_char_area( ch, target_name ) )
    || victim == ch
    || !victim->in_room
    || ch->in_room == victim->in_room
    || IS_ROOM( victim->in_room, ROOM_SAFE      )
    || IS_ROOM( victim->in_room, ROOM_PRIVATE   )
    || IS_ROOM( victim->in_room, ROOM_SOLITARY  )
        || IS_ROOM( victim->in_room, ROOM_NO_TELEPORT))
    {
      send_to_char( "You failed.\n\r", ch );
      return;
    }

    if( !has_spell_consent( ch, victim ) )
    {
        return;
    }


    act( "&+Y$n&+Y vanishes in a bright flash of light.&n", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act( "&+Y$n&+Y appears in a bright flash of light.&n.", ch, NULL, NULL, TO_ROOM );
    WAIT_STATE( ch, 10 );

    do_look( ch, "auto" );
    return;
}

void spell_wizard_eye( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *original;
    CHAR_DATA       *wch;
    CHAR_DATA         *victim;
    char *target_name = (char *) vo;

    if ( !( victim = get_char_world( ch, target_name ) )
    || victim == ch
    || !victim->in_room )

    {
    send_to_char( "You have failed.\n\r", ch );
    return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, victim->in_room );

    do_look( ch, "auto" );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'c 'wizard eye' XXXX quit' case.
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


void spell_mass_vortex_lift( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA        *victim;
    CHAR_DATA          *gch;
    int              number;
    char *target_name = (char *) vo;

    if ( !( victim = get_char_world( ch, target_name ) )
    || victim == ch
    || !victim->in_room
    || ch->in_room == victim->in_room
    || IS_ROOM( victim->in_room, ROOM_SAFE      )
    || IS_ROOM( victim->in_room, ROOM_PRIVATE   )
    || IS_ROOM( victim->in_room, ROOM_SOLITARY  )
    || victim->level >= level + 3
    || ( !IS_NPC( victim ) && is_clan( victim ) 
        && !is_same_clan( ch, victim ) )
    || ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
    if ( !is_same_group( gch, ch ) || !can_see( ch, gch ) || gch==ch )
        continue;

    number = number_percent();
     if ( number < ch->pcdata->spl_lrn[spl_mass_vortex_lift]  )
     {
          act( "$n leaves in a swirling vortex.", gch, NULL, NULL, TO_ROOM );
          char_from_room( gch );
          char_to_room( gch, victim->in_room );
          act( "$n appears in a swirling vortex.", gch, NULL, NULL, TO_ROOM );
          do_look( gch, "auto" );
        }
        else
        {
          act( "You have failed to vortex lift $N.", ch, NULL, gch, TO_CHAR );
          act( "$n has failed to vortex lift you.", ch, NULL, gch, TO_VICT );
        }
    }
 
    act( "$n leaves in a swirling vortex.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act( "$n appears in a swirling vortex.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}

// Chain Lightning.  No cap.  leveld6
// Level 50: minimum 50, maximum 300, average 175 for the first target

void spell_chain_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch;
    CHAR_DATA *vch_last;
    bool found;
    int dam;

    act( "A lightning bolt leaps from $n's hand and arcs to $N.",
                        ch, NULL, victim, TO_ROOM );
    act( "A lightning bolt leaps from your hand and arcs to $N.",
                        ch, NULL, victim, TO_CHAR );
    act( "A lightning bolt leaps from $n's hand and hits you!",
                        ch, NULL, victim, TO_VICT );  

    dam = dice( level, 6 );
    if ( saves_spell( level, victim, DAM_ELECTRICITY ) )
     dam /= 3;
    spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );
    vch_last = victim;
    level -= 4;

    while ( level > 0 )
    {
    found = FALSE;
    for ( vch = char_list; vch && level > 0; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
        continue;
        if ( vch->in_room == ch->in_room )
        {
        if ( !saves_spell( level, vch, DAM_ELECTRICITY ) && vch != vch_last )
        {
            found = TRUE;
            vch_last = vch;
            act( "The bolt arcs to $n!", vch, NULL, NULL, TO_ROOM );
            act( "The bolt hits you!", vch, NULL, NULL, TO_CHAR );
            dam = dice( level, 6 );
            if ( saves_spell( level, vch, DAM_ELECTRICITY ) )
            dam /= 3;
            spell_damage( ch, vch, dam, sn, DAM_ELECTRICITY );
            level -= 4;  /* decrement damage */
        }

            continue;
        }

        if ( vch->in_room->area == ch->in_room->area
        && IS_OUTSIDE( vch )
        && IS_AWAKE( vch ) )
            send_to_char( "Your hear a thundering noise.\n\r", vch );
    }
    
    if ( !found )
    {
        if ( ch->deleted || !ch )
        return;

        if ( vch_last == ch )
        {
        act( "The bolt seems to have fizzled out.",
                        ch, NULL, NULL, TO_ROOM );
        act( "The bolt grounds out through your body.",
                        ch, NULL, NULL, TO_CHAR );
        return;
        }
    
      vch_last = ch;
      act( "The bolt arcs to $n...whoops!", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You are struck by your own lightning!\n\r", ch );
      dam = dice( level, 6 );
      if ( saves_spell( level, ch, DAM_ELECTRICITY ) )
        dam /= 3;
      spell_damage( ch, ch, dam, sn, DAM_ELECTRICITY );
      level -= 4;
      if ( ch->deleted || !ch ) 
        return;
    }
    }
    return;
}

void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "detect good");
    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 6+level);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_GOOD );
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+YYour eyes tingle.\n\r", victim );
    return;
}

void spell_reveal_true_name( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Their true name is %s.\n\r", PERS( ch, victim ) );
    send_to_char( buf, ch );

    return;
}

// Dispel Good - level 30 slows down.  (2 * level)d4
void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
  
    if( level > 30 ) level -= (level - 30)/2;

    if ( !IS_NPC( ch ) && IS_GOOD( ch ) )
    {
    send_to_char( "You are too GOOD to cast this.\n\r", ch );
    return;
    }
  
    if ( IS_EVIL( victim ) )
    {
    act( "$N is protected by $S evil.", ch, NULL, victim, TO_ROOM );
    act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if ( IS_NEUTRAL( victim ) )
    {
    act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
    return;
    }

    dam = dice( (2 * level), 4 );
    if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_BLACK_MANA );

    return;
}

// You can hit good aligned group members.  For now this is intentional.
// -- Veygoth
void spell_unholy_word( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;
  
    if ( !IS_NPC( ch ) && IS_GOOD( ch ) )
    {
        send_to_char( "Your god blasts your for your blasphemy.\n\r", ch );
        spell_damage( ch, ch, 300, sn, DAM_BLACK_MANA );
        return;
    }

    if ( !IS_NPC( ch ) && ch->alignment > -900 )
    {
        send_to_char( "&+rYou don't feel evil enough.\n\r", ch );
        return;
    }

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
      victim_next = victim_next->next_in_room;

      if( victim->deleted || victim == ch || is_same_group( victim, ch )) continue;
      if ( IS_EVIL( victim ) || IS_NEUTRAL( victim ) )
      {
        act( "$N&n does not seem to be affected.", ch, NULL, victim, TO_CHAR );
        continue;
      }
      dam = level * 5;
      if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
    dam /= 2;

      spell_damage( ch, victim, dam, sn, DAM_BLACK_MANA );
    }

    return;
}

// You can hit evil aligned group members.  For now this is intentional.
// -- Veygoth

void spell_holy_word( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;
  
    if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
    {
        send_to_char( "Your god blasts your for your blasphemy.\n\r", ch );
        spell_damage( ch, ch, 300, sn, DAM_WHITE_MANA );
        return;
    }

    if ( !IS_NPC( ch ) && ch->alignment < 900 )
    {
        send_to_char( "&+WYou don't feel good enough.\n\r", ch );
        return;
    }

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
      victim_next = victim_next->next_in_room;

      if( victim->deleted || victim == ch || is_same_group( victim, ch )) continue;
      if ( IS_GOOD( victim ) || IS_NEUTRAL( victim ) )
      {
        act( "$N&n does not seem to be affected.", ch, NULL, victim, TO_CHAR );
        continue;
      }
      dam = level * 5;
      if ( saves_spell( level, victim, DAM_WHITE_MANA ) )
          spell_damage( ch, victim, dam/2, sn, DAM_WHITE_MANA );
      else
          spell_damage( ch, victim, dam, sn, DAM_WHITE_MANA );
    }

    return;
}

// Wormhole
void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo )
{
    make_portal( sn, level, ch, vo, OBJ_VNUM_WORMHOLE);
    return;
}

void spell_etherportal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    make_portal( sn, level, ch, vo, OBJ_VNUM_PORTAL );
    return;
}

void spell_moonwell( int sn, int level, CHAR_DATA *ch, void *vo )
{
    make_portal( sn, level, ch, vo, OBJ_VNUM_MOONWELL );
    return;
}

void make_portal( int sn, int level, CHAR_DATA *ch, void *vo, int vnum )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *portal;
    victim = (CHAR_DATA *)vo;
    original = ch->in_room;
 
    if ( IS_NPC( ch ) ) return;
    if ( !victim || victim->deleted )
    {
        send_to_char("Who exactly is your target?\n\r", ch);
        return;
    }
    if ( IS_NPC( victim ) && !IS_IMMORTAL( ch ) ) return;

    if( !has_spell_consent( ch, victim ) )
    {
        return;
    }

    if ( victim == ch || ch->in_room == victim->in_room )
    {
        send_to_char("Seems like a waste of time.\n\r", ch);
        return;
    }

    if ( !( location = victim->in_room )
    || IS_ROOM( victim->in_room, ROOM_SAFE      )
    || IS_ROOM( victim->in_room, ROOM_PRIVATE   )
    || IS_ROOM( victim->in_room, ROOM_SOLITARY  ))
    {
        send_to_char( "You can't seem to get a fix their location.\n\r", ch );
        return;
    }
    if( !IS_NPC( victim ) && (race_table[ch->race].racewar_side !=
        race_table[victim->race].racewar_side) && !IS_IMMORTAL( ch ) ) {
        send_to_char("Don't you wish it was that easy!\n\r", ch);
        return;
    }

    portal = create_object( get_obj_index( vnum ), 0 );

    if( IS_ROOM( victim->in_room, ROOM_NO_GATE )
      || IS_ROOM( ch->in_room, ROOM_NO_GATE ))
    {
        act( "$p opens for a brief instant and then collapses.&n",ch, portal, NULL, TO_CHAR);
        act( "$p opens for a brief instant and then collapses.&n",ch, portal, NULL, TO_ROOM);
        act( "$p opens for a brief instant and then collapses.&n",victim, portal, NULL, TO_CHAR);
        act( "$p opens for a brief instant and then collapses.&n",victim, portal, NULL, TO_ROOM);
        extract_obj( portal );
        return;
    }

    portal->timer = level / 15;
    portal->value[2] = level / 7;
    portal->value[0] = location->vnum;
 
    obj_to_room( portal, original );

    portal = create_object( get_obj_index( vnum ), 0 );
    portal->timer = level / 15;
    portal->value[2] = level / 7;
    portal->value[0] = original->vnum;

    obj_to_room( portal, location );
 
    switch ( vnum )
    {
    case OBJ_VNUM_PORTAL:
        act( "$p&+Y rises up from the ground.&n", ch, portal, NULL, TO_ROOM );
        act( "$p&+Y rises up before you.&n", ch, portal, NULL, TO_CHAR );

        if ( location->people )
        {
        act( "$p&+Y rises up from the ground.&n", location->people, portal, NULL,
                                      TO_ROOM );
        act( "$p&+Y rises up from the ground.&n", location->people, portal, NULL,
                                      TO_CHAR );
        }
        break;
    case OBJ_VNUM_MOONWELL:
        act( "&+WSilver mists swirl and form into a $p.&n", ch, portal, NULL, TO_ROOM );
        act( "&+WSilver mists swirl and form into a $p.&n", ch, portal, NULL, TO_CHAR );

        if ( location->people )
        {
        act( "&+WSilver mists swirl and form into a $p.&n", location->people, portal, NULL,
                                      TO_ROOM );
        act( "&+WSilver mists swirl and form into a $p.&n", location->people, portal, NULL,
                                      TO_CHAR );
        }
        break;
    case OBJ_VNUM_WORMHOLE:
        act( "$p&+L appears from a warping of space and time.&n", ch, portal, NULL, TO_ROOM );
        act( "$p&+L appears from a warping of space and time.&n", ch, portal, NULL, TO_CHAR );

        if ( location->people )
        {
        act( "$p&+L appears from a warping of space and time.&n", location->people, portal, NULL,
                                      TO_ROOM );
        act( "$p&+L appears from a warping of space and time.&n", location->people, portal, NULL,
                                      TO_CHAR );
        }
        break;
    }

    WAIT_STATE( ch, 8 );

    return;
}


void spell_cure_disease( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !has_affect( victim, 0, spl_plague, 0 ) )
    {
    if ( victim == ch )
        send_to_char( "You aren't ill.\n\r", ch );
    else
        act( "$N&n doesn't appear to be diseased.", ch, NULL, victim,
        TO_CHAR );
    return;
    }

    affect_strip( victim, 0, spl_plague, 0 );
    send_to_char( "Your sores vanish.\n\r", victim );
    act( "$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_frost_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0 ) )
    return;
    
    if ( has_affect( victim, 0, spl_flame_shield, 0 )
    && !saves_spell( level, victim, DAM_COLD ) )
    {
    affect_strip( victim, 0, spl_flame_shield, 0 );
    send_to_char( "Your flaming shield is destroyed.\n\r", victim );
    act( "$n's flaming shield is snuffed out.", victim, NULL, NULL, TO_ROOM );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_RESISTANT;
    af.modifier  = RIS_COLD;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    af.location  = APPLY_SUSCEPTIBLE;
    af.modifier  = RIS_FIRE;
    affect_to_char( victim, &af );
    /* For compatibility with old Envy2.2 areas & pfiles */
    set_bitvector( &af, AFF_COLDSHIELD );
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a frost shield.\n\r", victim );
    act( "$n is surrounded by a frost shield.", victim, NULL, NULL, TO_ROOM );

    return;
}


void spell_flame_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0 ) )
    {
        send_to_char( "You failed!\n\r", ch );
    return;
    }

    if ( has_affect( victim, 0, spl_frost_shield, 0 )
    && !saves_spell( level, victim, DAM_FIRE ) )
    {
    affect_strip( victim, 0, spl_frost_shield, 0 );
    send_to_char( "Your frost shield is destroyed.\n\r", victim );
    act( "$n's frost shield is snuffed out.", victim, NULL, NULL, TO_ROOM );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_RESISTANT;
    af.modifier  = RIS_FIRE;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    af.location  = APPLY_SUSCEPTIBLE;
    af.modifier  = RIS_COLD;
    affect_to_char( victim, &af );
    af.location  = APPLY_NONE;
    /* For compatibility with old Envy2.2 areas & pfiles */
    set_bitvector( &af, AFF_FLAMING);
    affect_to_char( victim, &af );

    send_to_char( "&+rYou are surrounded by a flaming shield.&n\n\r", victim );
    act( "&+r$n&+r is surrounded by a flaming shield.&n", victim, NULL, NULL, TO_ROOM );

    return;
}

void spell_fireskin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    // This is just modified fireshield, we want the same exact affect
    sn = spl_flame_shield;

    if ( has_affect( victim, 0, sn, 0 ))
    {
        send_to_char( "You failed!\n\r", ch );
    return;
    }

    if ( has_affect( victim, 0, spl_frost_shield, 0 )
    && !saves_spell( level, victim, DAM_FIRE ) )
    {
    affect_strip( victim, 0, spl_frost_shield, 0 );
    send_to_char( "Your frost shield is destroyed.\n\r", victim );
    act( "$n's frost shield is snuffed out.", victim, NULL, NULL, TO_ROOM );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = ( level / 9 );
    af.location  = APPLY_RESISTANT;
    af.modifier  = RIS_FIRE;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );
    af.location  = APPLY_HITROLL;
    af.modifier  = - 10;
    affect_to_char( victim, &af );
    af.location  = APPLY_SUSCEPTIBLE;
    af.modifier  = RIS_COLD;
    affect_to_char( victim, &af );
    af.location  = APPLY_NONE;
    /* For compatibility with old Envy2.2 areas & pfiles */
    set_bitvector( &af, AFF_FLAMING);
    affect_to_char( victim, &af );

    send_to_char( "&+rYour skin is covered in flames.  However, your vision blurs.&n\n\r", victim );
    act( "&+r$n&+r's skin is covered in flames.", victim, NULL, NULL, TO_ROOM );

    return;
}

void spell_shock_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration = number_fuzzy( level / 8 );
    af.location  = APPLY_RESISTANT;
    af.modifier  = RIS_ELECTRICITY;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "Torrents of cascading energy form around you.\n\r", victim );
    act( "Torrents of cascading energy form around $n.",
    victim, NULL, NULL, TO_ROOM );

    return;
}

void spell_ethereal_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_RESISTANT;
    af.modifier  = RIS_ENERGY;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    send_to_char( "You fade from the mundane energy continuum.\n\r", victim );
    act( "An ethereal shield divides $n from the mundane energy continuum.",
    victim, NULL, NULL, TO_ROOM );

    return;
}

void spell_power_word_lag( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;

    act( "You lag $N&n.", ch, NULL, victim, TO_CHAR );
    act( "A horrible bout of lag creeps up on you and sucks you into the pit of nothingness.", ch, NULL, victim, TO_VICT );
    act( "$n&n engulfs $N&n in a horrible cloud of LAG!", ch, NULL, victim, TO_NOTVICT );

    WAIT_STATE( victim, dice( 2, ch->level ));

    return;
}

void spell_power_word_stun( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;

    act( "$n&n utters a single word of power...", ch, NULL, NULL, TO_ROOM );

    if( !saves_spell( level, victim, DAM_SOUND ) || victim->hit < 50 )
    {
      act( "You stun $N&n.", ch, NULL, victim, TO_CHAR );
      act( "You are stunned!", ch, NULL, victim, TO_VICT );
      act( "$N&n is stunned!", ch, NULL, victim, TO_NOTVICT );
      WAIT_STATE( victim, (dice( 3, 8 ) + ch->level / 10));
      stop_fighting( victim, FALSE );
    }

    return;
}

// Dust Blast - level 15 cap.  (level/3)d5 + (level/3)
// At level 5: 10 minimum, 30 high, 20 average
// Spell by Xueqin

void spell_dust_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    temp = level / 3;

    if ( temp > 5)
      temp = 5;
    else if( temp < 1 )
      temp = 1;

    dam = dice(temp, 5) + temp;

    spell_damage( ch, victim, dam, sn, DAM_SOUND );

    return;
}

void spell_slow_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_SLOW_POISON ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24 + level / 6;
    af.location  = 0;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SLOW_POISON );
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    if( is_affected( victim, AFF_POISON ))
        send_to_char( "The poison in your body slows its rampage.\n\r", victim );
    return;
}

void spell_protection_acid( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_ACID);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+gYou feel protected from acid.&n\n\r", victim );
    return;
}

void spell_protection_cold( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_COLD);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+WYou feel protected from cold.&n\n\r", victim );
    return;
}

void spell_cold_ward( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_PROTECT_COLD ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level / 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_COLD);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+WYou are now warded against cold.&n\n\r", victim );
    return;
}

void spell_protection_gas( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_GAS);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+YYou feel protected from poisonous gases.&n\n\r", victim );
    return;
}

void spell_protection_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_FIRE);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+rYou feel protected from fire.&n\n\r", victim );
    return;
}

void spell_deny_air( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_DENY_AIR ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DENY_AIR);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+CYou feel protected from air.&n\n\r", victim );
    return;
}

void spell_deny_earth( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_DENY_EARTH ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DENY_EARTH);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+yYou feel protected from earth.&n\n\r", victim );
    return;
}

void spell_deny_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_DENY_FIRE ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DENY_FIRE);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+rYou feel protected from fire.&n\n\r", victim );
    return;
}

void spell_deny_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_DENY_WATER ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DENY_WATER);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+bYou feel protected from water.&n\n\r", victim );
    return;
}

void spell_adaptation( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( !is_affected( victim, AFF_PROTECT_FIRE ))
    {
      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_PROTECT_FIRE);
      affect_to_char( victim, &af );
      send_to_char( "&+rYou feel protected from fire.&n\n\r", victim );
      act( "$n&n is protected from fire.", victim, NULL, NULL, TO_ROOM );
    }

    if ( !is_affected( victim, AFF_GILLS ) )
    {
      af.skill     = 0;
      af.spell      = sn;
      af.song      = 0;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_GILLS);
      affect_to_char( victim, &af );
      send_to_char( "You can now breathe underwater.\n\r", victim );
      act( "$n&n can now breathe underwater.", victim, NULL, NULL, TO_ROOM );
    }

    return;
}

void spell_fire_ward( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_PROTECT_FIRE ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
    return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level / 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_FIRE);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+rYou are now warded against fire.&n\n\r", victim );
    return;
}

void spell_protection_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( has_affect( victim, 0, sn, 0) ) {
        affect_refresh( victim, 0, sn, 0, 24);
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROTECT_LIGHTNING);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "&+BYou feel protected from &+Ylightning.&n\n\r", victim );
    return;
}

void spell_soulshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( !IS_NPC( ch ) && ch->alignment < 950 && ch->alignment > -950 )
    {
        send_to_char( "&+lYour faith is not strong enough.\n\r", ch );
        return;
    }


    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = number_fuzzy( level / 3 ) + 3;
    af.location  = 0;
    af.modifier  = 0;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    if( ch->alignment > 0 )
    {
        send_to_char( "&+WA holy aura forms around you.\n\r", victim );
        act( "&+WA holy aura forms around $n&n.",
            victim, NULL, NULL, TO_ROOM );
    }
    else
    {
        send_to_char( "&+rA unholy aura forms around you.\n\r", victim );
        act( "&+rA unholy aura forms around $n&n.",
            victim, NULL, NULL, TO_ROOM );
    }
    return;
}

// Ice Bolt - level 21 cap.  (level/3)d5 + (level/3)
// At level 21: 14 minimum, 42 maximum, 28 average
/* First circle spell for air elementalists by Xueqin.*/

void spell_ice_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    temp = level / 3;
    if ( temp > 7)
      temp = 7;
    else if( temp < 1 )
      temp = 1;
    
    dam = dice(temp, 5) + temp;

    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

// Fire Bolt - level 15 cap.  (level/3)d5 + (level/3)
// At level 15: 10 minimum, 30 maximum, 20 average
/* First circle spell for Fire Elementalists by Xueqin.*/

void spell_fire_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level / 3;
    if ( temp > 5)
      temp = 5;
    else if( temp < 1 )
      temp = 1;
    
    dam = dice(temp, 5) + temp;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Acid Spray - level 15 cap.  (level/3)d5 + (level/3)
// At level 15: 10 minimum, 30 maximum, 20 average
/* First circle spell for Water Elementalists by Xueqin.*/

void spell_acid_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level / 3;
    if ( temp > 5)
      temp = 5;
    else if( temp < 1 )
      temp = 1;
    
    dam = dice(temp, 5) + temp;

    spell_damage( ch, victim, dam, sn, DAM_ACID );

    return;
}

// Pebble - level 20 cap.  4d3 + (level * 2)
// At level 20: 44 minimum, 52 maximum, 48 average
/* Second Circle spell for Earth Elementalists by Xueqin.*/

void spell_pebble( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level;
    if ( temp > 20)
    temp = 20;
    
    dam = dice( 4, 3 ) + (temp * 2);
       
    if ( saves_spell( level, victim, DAM_CRUSHING ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_CRUSHING );

    return;
}

// Cutting Breeze - level 30 cap - 4d3 + (level*3/2)
// At level 30: 48 minimum, 57 maximum, 52.5 average
/* Second Circle spell for Air Elementalists by Xueqin.*/

void spell_cutting_breeze( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    AFFECT_DATA af;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level;
    if ( temp > 30)
    temp = 30;
    
    dam = dice( 4, 3 ) + (level * 3 / 2);

    if ( !saves_spell( level, victim, DAM_WIND ) )
    {
        af.skill     = 0;
        af.spell     = sn;
        af.song      = 0;
        af.duration  = 2;
        af.location  = APPLY_STR;
        af.modifier  = 0 - 5;
        set_bitvector( &af, AFF_NONE);
        affect_join( victim, &af );
    }
    else
    {
    dam /= 2;
    }

    spell_damage( ch, victim, dam, sn, DAM_WIND );

    return;
}

/* Third Circle Elementalist spell by Xueqin.*/
void spell_minor_blending( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_MINOR_INVIS )
      || is_affected( victim, AFF_INVISIBLE ))
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_INVIS);
    affect_to_char( victim, &af );

    send_to_char( "You blend with your surroundings.\n\r", ch );

    return;
}

/* Concealment by Xueqin.*/
void spell_concealment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_MINOR_INVIS )
       || is_affected( victim, AFF_INVISIBLE ))
        return;

    act( "$n&n fades out of existence.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You vanish.\n\r", ch );

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_INVIS);
    affect_to_char( victim, &af );

    return;
}

/* by Xueqin */
void spell_aura( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_DETECT_EVIL ) )
    return;

    if ( is_affected( victim, AFF_DETECT_GOOD ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_DETECT_EVIL);
    affect_to_char( victim, &af );
    set_bitvector( &af, AFF_DETECT_GOOD);
    affect_to_char( victim, &af );


    return;
}

/* by Xueqin */
void spell_dirt_cloud( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    act( "&+yYou conjure a cloud of dirt that swirls around &n$N.&n", ch, NULL, victim, TO_CHAR );
    act( "$n &+yconjures a cloud of &+Lbillowing dirt&n&+y right in your face.&N", ch, NULL, victim, TO_VICT );
    act( "$N &+yis engulfed by a cloud of &+Ldirt&N&+y conjured by &n$n.&n", ch, NULL, victim, TO_ROOM );

    if( is_affected( victim, AFF_BLIND ))
    {
        send_to_char( "The cloud appears to have no affect on them.\n\r", ch );
        return;
    }   

    if ( saves_spell( level, victim, DAM_LIGHT ) )
    {
        act( "$N&n can see you through the swirling dirt.", ch, NULL, victim, TO_CHAR );
        return;
    }


    af.skill     = 0;
    af.spell      = sn;
    af.song      = 0;
    af.duration  = number_range( 1, 3 );
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    set_bitvector( &af, AFF_BLIND);
    affect_to_char( victim, &af );

    act( "$N&n &+Lis blinded by a cloud of dirt!&n", ch, NULL, victim, TO_CHAR );
    send_to_char( "&+LYou can't see through the swirling dirt!&n\n\r", victim );
    act( "$N&n &+Lis blinded by swirling dirt!&n", ch, NULL, victim, TO_NOTVICT );
    return;
}

// Spark - level 25 cap.  (2 * level)d3
// At level 25: 50 minimum, 150 maximum, 100 average

void spell_spark( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level;
    if ( temp > 25)
    temp = 25;
    
    dam = dice( (2 * level), 3);

    if ( saves_spell( level, victim, DAM_FIRE ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Water Blast - level 25 cap.  (2 * level)d3
// At leevl 25 or more: 50 minimum, 150 maximum, 100 average
// Chance to reduce dexterity of the target

void spell_water_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    AFFECT_DATA af;
    
    if( level > 25 ) level = 25;
    
    dam = dice( (2 * level), 3 );
    
    if ( !saves_spell( level, victim, DAM_WATER ) )
    {
        af.skill     = 0;
        af.spell     = sn;
        af.song      = 0;
        af.duration  = 2;
        af.location  = APPLY_DEX;
        af.modifier  = 0 - (number_range( 1, 5 ));
        set_bitvector( &af, AFF_NONE);
        affect_join( victim, &af );
    }
    else
    {
    dam /= 2;
    }
    spell_damage( ch, victim, dam, sn, DAM_WATER );
        
    return;
}

// Water Bolt - level 15 cap.  (level/3)d3 + (level/3)
// At level 15: 10 minimum, 30 maximum, 20 average
// Chance to reduce dexterity of target

void spell_water_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int number;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    dam = dice( 2, 3) + 2;
       
    if ( !saves_spell( level, victim, DAM_WATER ) )
    {
        af.skill     = 0;
        af.spell     = sn;
        af.song      = 0;
        af.duration  = 2;
        af.location  = APPLY_DEX;
        af.modifier  = 0 - (number_range( 1, 3 ));
        set_bitvector( &af, AFF_NONE);
        affect_join( victim, &af );
    }
    else
    {
        dam /= 2;
    }
    number = URANGE( 1, (ch->level / 4), 6 );
    for( temp = 0; temp < number && victim->position > POS_DEAD; temp++ )
        spell_damage( ch, victim, dam, sn, DAM_WATER );

    return;
}

/*'Earthen Smith' and 'Fiery Smith' should give +2 dam and 'Aquatic Smith'
and 'Airy Smith' should give +2 hit.  None of the spells give the weapons
enchanted any 'alignment'.
-Xueqin */
void spell_earthen_smith( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_ENCHANT ))
    {
      setoff_trap( ch, obj );
      return;
    }

    if ( obj->item_type != TYPE_WEAPON
    || IS_OBJ_STAT( obj, ITEM_MAGIC )
    || obj->affected )
    {
    send_to_char( "$p&n cannot be enchanted.\n\r", ch );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );


    paf            = new_affect();

    paf->skill          = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_DAMROLL;
    paf->modifier    = 2;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;


    send_to_char( "&+yThe elements respond to your control.&n\n\r", ch );
    return;
}

// by Xueqin
void spell_firey_smith( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_ENCHANT ))
    {
      setoff_trap( ch, obj );
      return;
    }

    if ( obj->item_type != TYPE_WEAPON
    || IS_OBJ_STAT( obj, ITEM_MAGIC )
    || obj->affected )
    {
    send_to_char( "$p&n cannot be enchanted.\n\r", ch );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );


    paf            = new_affect();

    paf->skill          = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_DAMROLL;
    paf->modifier    = 2;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;


    send_to_char( "&+rThe elements respond to your control.&n\n\r", ch );
    return;
}

// by Xueqin
void spell_airy_smith( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_ENCHANT ))
    {
      setoff_trap( ch, obj );
      return;
    }

    if ( obj->item_type != TYPE_WEAPON
    || IS_OBJ_STAT( obj, ITEM_MAGIC )
    || obj->affected )
    {
    send_to_char( "$p&n cannot be enchanted.\n\r", ch );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );


    paf            = new_affect();

    paf->skill      = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_HITROLL;
    paf->modifier    = 2;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;


    send_to_char( "&+CThe elements respond to your control.&n\n\r", ch );
    return;
}

// by Xueqin
void spell_aquatic_smith( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_ENCHANT ))
    {
      setoff_trap( ch, obj );
      return;
    }

    if ( obj->item_type != TYPE_WEAPON
    || IS_OBJ_STAT( obj, ITEM_MAGIC )
    || obj->affected )
    {
    act( "$p&n cannot be enchanted.",   ch, obj, NULL, TO_CHAR );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );


    paf            = new_affect();

    paf->skill      = 0;
    paf->spell        = sn;
    paf->duration    = -1;
    paf->location    = APPLY_HITROLL;
    paf->modifier    = 2;
    set_bitvector( paf, AFF_NONE);
    paf->next        = obj->affected;
    obj->affected    = paf;


    send_to_char( "&+BThe elements respond to your control.&n\n\r", ch );
    return;
}

// by Xueqin
void spell_hover( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_LEVITATE ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_LEVITATE);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You begin to hover above the ground.\n\r", victim );
    return;
}

// Azure Flame - level 30 cap.  (2 * level)d4
// At level 30: 60 minimum, 240 maximum, 150 average

void spell_azure_flame( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level;
    if (temp > 30)
    temp = 30;

    dam = dice( (2 * level), 4 );

    if ( saves_spell( level, victim, DAM_FIRE ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_FIRE );

    return;
}

// Gust of Frost - level 30 cap.  (2 * level)d4
// At level 30 or more: 60 minimum, 240 maximum, 150 average
/* Fourth circle air elementalist damage spell by Xueqin.*/

void spell_gust_of_frost( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int temp;
    int dam;
    CHAR_DATA *victim      = (CHAR_DATA *) vo;

    temp = level;
    if (temp > 30)
    temp = 30;

    dam = dice( (2 * level), 4);
       
    if ( saves_spell( level, victim, DAM_COLD ) )
    dam /= 2;

    spell_damage( ch, victim, dam, sn, DAM_COLD );

    return;
}

// Tide of the Seas - level 20 cap.  4d3 + (level*3/2)
// At level 20: 34 minimum, 42 maximum, 38 average
/* This is supposed to be area effect -Xueqin*/

void spell_tide_seas( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    int dam;
    int temp;

    temp = level;
    if ( temp > 20)
    temp = 20;
    
    send_to_char( "&+bYou conjure a massive blast of water!&n\n\r", ch );
    act( "$n&N &+bconjures a massive blast of water.&n", ch, NULL, NULL, TO_ROOM );

    if (ch->fly_level < MAX_FLY)
      act("&+bA massive blast of water appears below you!&n", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+bThe sky above you fills with a blast of water&n", ch, NULL, NULL, TO_BELOW);

    dam = dice( 4, 3 ) + (temp * 3 / 2);

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || is_same_group( ch, vch ) )
            continue;
        if ( vch != ch && ch->in_room == vch->in_room )
        {
            if ( saves_spell( level, vch, DAM_ACID ) )
                spell_damage( ch, vch, dam/2, sn, DAM_ACID );
            else;
                spell_damage( ch, vch, dam, sn, DAM_ACID );
                continue;
        }

        if ( vch->in_room->area == ch->in_room->area && FALSE)
            send_to_char( "You hear the sound of rushing water.\n\r", vch );
    }

    return;
}

/* Elementalist invisibility spell, supposed to be circle 8.-Xueqin*/
void spell_blending( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, AFF_INVISIBLE ) || has_affect( victim, 0, sn, 0))
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_INVISIBLE);
    affect_to_char( victim, &af );

    send_to_char( "You slowly blend into the surroundings.\n\r", victim );
    act( "$n&n blends into the surroundings.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_mirror_image( int sn, int level, CHAR_DATA *ch, void *vo )
{
    PERSON_DATA *pet;
    CHAR_DATA *image;
    int count, number;
    char buf[MAX_STRING_LENGTH];

    number = level / 5;

    if( number > 4 ) number = 4;

    // Might want to check to see that the pet actually belongs to the caster
    // because if someone else's images were following, they would not be able
    // to cast the spell.
    for( pet = ch->followers; pet; pet = pet->next )
    {
       if( !pet->who || pet->who->deleted )
         continue;
       if( IS_NPC( pet->who ) && pet->who->pIndexData->vnum == MOB_VNUM_MIRROR_IMAGE )
       {
         send_to_char( "Nothing happens.\n\r", ch );
         return;
       }
    }

    for( count = 0; count < number; count++ );
    {
      image = create_mobile( get_mob_index( MOB_VNUM_MIRROR_IMAGE ));
      char_to_room( image, ch->in_room );
      sprintf( buf, "image %s", ch->name );
      free_string( image->name );
      image->name = str_dup( buf );
      if( !IS_NPC( ch ))
        sprintf( buf, "%s", ch->name);
      else
        sprintf( buf, "%s", ch->short_descr );
      free_string( image->short_descr );
      image->short_descr = str_dup( buf );
      if( !IS_NPC( ch ))
        sprintf( buf, "%s%s", ch->name, ch->pcdata->title );
      else
        sprintf( buf, "%s", ch->long_descr );
      free_string( image->long_descr );
      image->long_descr = str_dup( buf );
      image->size = ch->size;
      image->race = ch->race;
      image->money.copper = 0;
      image->money.silver = 0;
      image->money.gold = 0;
      image->money.platinum = 0;
      image->max_hit = 1;
      image->hit = 1;
      act( "$n&n appears.", ch, NULL, NULL, TO_ROOM );
      add_follower( image, ch );
      image->master = ch;
      SET_BIT( image->act, ACT_NOEXP );
      SET_BIT( image->act, ACT_PET );
      SET_AFF_BIT( image, AFF_CHARM );
    }
      
    return;
}

// So far it's just an unmodified copy of mirror image - Veygoth
void spell_clone( int sn, int level, CHAR_DATA *ch, void *vo )
{
    PERSON_DATA *pet;
    CHAR_DATA *clone;
    char buf[MAX_STRING_LENGTH];

    // Might want to check to see that the pet actually belongs to the caster
    // because if someone else's images were following, they would not be able
    // to cast the spell.
    for( pet = ch->followers; pet; pet = pet->next )
    {
       if( !pet->who || pet->who->deleted )
         continue;
       if( IS_NPC( pet->who ) && pet->who->pIndexData->vnum == MOB_VNUM_CLONE )
       {
         send_to_char( "Nothing happens.\n\r", ch );
         return;
       }
    }

      clone = create_mobile( get_mob_index( MOB_VNUM_CLONE ));
      char_to_room( clone, ch->in_room );
      sprintf( buf, "clone %s", ch->name );
      free_string( clone->name );
      clone->name = str_dup( buf );
      if( !IS_NPC( ch ))
        sprintf( buf, "%s", ch->name);
      else
        sprintf( buf, "%s", ch->short_descr );
      free_string( clone->short_descr );
      clone->short_descr = str_dup( buf );
      if( !IS_NPC( ch ))
        sprintf( buf, "%s%s", ch->name, ch->pcdata->title );
      else
        sprintf( buf, "%s", ch->long_descr );
      free_string( clone->long_descr );
      clone->long_descr = str_dup( buf );
      clone->size = ch->size;
      clone->race = ch->race;
      clone->money.copper = 0;
      clone->money.silver = 0;
      clone->money.gold = 0;
      clone->money.platinum = 0;
      clone->level = ch->level / 2;
      SET_BIT( clone->act, ACT_NOEXP );
      if( clone->level > 20 ) clone->level = 20;
      clone->max_hit = dice( clone->level, 14 );
      clone->hit = clone->max_hit;
      act( "$n&n appears.", ch, NULL, NULL, TO_ROOM );
      add_follower( clone, ch );
      clone->master = ch;
      SET_BIT( clone->act, ACT_PET );
      SET_AFF_BIT( clone, AFF_CHARM );
      
    return;
}

void spell_wall_of_iron( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall =  make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_IRON);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_wall_of_stone( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall =  make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_STONE);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

OBJ_DATA *make_wall(int sn, int level, CHAR_DATA *ch, void *vo, int vnum)
{
    OBJ_DATA *wall;
    OBJ_DATA *wall2;
    OBJ_INDEX_DATA *pWall;
    int door;
    EXIT_DATA *pexit;
    char buf[MAX_STRING_LENGTH];
    char *target_name = (char *) vo;

    // If we were really smooth we would just create another target called
    // tar_exit
    if( target_name[0] == '\0' )
    {
       send_to_char( "Specify a direction in which to cast the spell!\n\r", ch );
       return NULL;
    }

    door = find_exit( ch, target_name );

    if( door == -1 )
    {
       send_to_char( "You failed!\n\r", ch );
       return NULL;
    }

    // The exit of the same direction should be flagged EX_WALLED
    if ( !( pexit = ch->in_room->exit[door] ) )
    {
       send_to_char( "You failed!\n\r", ch );
       return NULL;
    }  

    if( IS_SET( pexit->exit_info, EX_WALLED ))
    {
       send_to_char( "There's already a wall there!\n\r", ch );
       return NULL;
    }

    pWall = get_obj_index( vnum );
    if ( !pWall ) {
        sprintf( lbuf, "Make_wall: null wall pointer from get_obj_index( %d ).",
            vnum);
        wiznet( 0, WIZ_LSPAM, 0, lbuf);
        bug( lbuf, 0 );
        send_to_char(" Uh oh, no pIndex for that wall.\n\r", ch);
        return NULL;
    }

    wall = create_object( get_obj_index( vnum ), 0 );
    if ( !wall ) {
        bug( "Make_wall: null wall pointer from create_object", 0);
        send_to_char("Whoops, null objindex.  This is a bug.\n\r", ch );
        return NULL;
    }

    // Value[0] should be the direction that is blocked by the wall.
    wall->value[0] = door;
    // Set the passable value for the wall
    wall->value[1] = FALSE;
    // Set the wall's level
    wall->value[2] = level;

    sprintf( buf, "%s %s exit.&n", wall->description, dir_name[door] );
    free_string( wall->description );
    wall->description = str_dup( buf );

    SET_BIT( pexit->exit_info, EX_WALLED );

    wall->timer = ch->level / 3;
    wall->level = ch->level;
    obj_to_room( wall, ch->in_room );

    // Create the wall on the other side
    if( pexit->to_room )
    {
      if (pexit->to_room->exit[rev_dir[wall->value[0]]]) {
      //we actually have a matching exit
      wall2 = create_object( get_obj_index( vnum ), 0 );

      // Value[0] should be the direction that is blocked by the wall.
      wall2->value[0] = rev_dir[door];
      // Set the passable value for the wall
      wall2->value[1] = FALSE;
      // Set the wall's level
      wall2->value[2] = level;

      sprintf( buf, "%s %s exit.&n", wall2->description, dir_name[rev_dir[door]] );
      free_string( wall2->description );
      wall2->description = str_dup( buf );

      if ( pexit->to_room->exit[rev_dir[door]] )
        SET_BIT( pexit->to_room->exit[rev_dir[door]]->exit_info, EX_WALLED );

      wall2->timer = ch->level / 2;
      wall2->level = ch->level;
      obj_to_room( wall2, pexit->to_room );
      if (pexit->to_room->people) {
      sprintf(buf,"$p&n appears to the %s.", dir_name[wall2->value[0]]);
        act(buf, pexit->to_room->people, wall2, NULL, TO_ALL);
      }
      } //end if matching exit
    }

    return wall;
}

void spell_wall_of_force( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall =  make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_FORCE);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_wall_of_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall =  make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_FIRE);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_wall_of_sparks( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall =  make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_SPARKS);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_lightning_curtain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall = make_wall(sn, level, ch, vo, OBJ_VNUM_LIGHTNING_CURTAIN);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_illusionary_wall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall = make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_ILLUSION);
    if (wall){
        sprintf( buf, "The %s exit disappears.", dir_name[wall->value[0]] );
        act( buf, ch, NULL, NULL, TO_ALL );
    }
    return;
}

void spell_wall_of_ice( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];
    wall = make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_ICE);
    if (wall){
        sprintf( buf, "$p&n appears %s.", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_miners_intuition( int sn, int level, CHAR_DATA *ch, void *vo )
{
    EXIT_DATA           *pexit;
    int                  door;

    /* Look for a hidden exit. */
    for ( door = 0; door < MAX_DIR; door++ )
    {
        if ( ( pexit = ch->in_room->exit[door] )
               && IS_SET( pexit->exit_info, EX_SECRET )
               && pexit->to_room )
        {
           REMOVE_BIT( pexit->exit_info, EX_SECRET );
           send_to_char( "An exit is revealed!\n\r", ch );
           act( "An exit is revealed!", ch, NULL, NULL, TO_ROOM );
           return;
        }
    }

    send_to_char( "Your intuition tells you nothing.\n\r", ch );
}

/* by Xueqin */
// Changed by Veygoth to directly cast stoneskin.
void spell_group_stone( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    last_fun( "spell_group_stone");

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
       if( victim->deleted || !is_same_group( ch, victim ))
           continue;
       if ( is_affected( victim, AFF_STONESKIN ) )
         continue;

       spell_stone_skin( spl_stoneskin, level, ch, victim );
    }

    return;
}

// Hurricane - level 35 cap - (3 * level)d3
// At level 35: 105 minimum, 315 maximum, 210 average
// Original spell by Xueqin

void spell_hurricane( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;

    last_fun( "spell_hurricane");

    if( level > 35 ) level = 35;

    send_to_char( "&+cYou conjure a massive hurricane that tears into your foes!&n\n\r", ch );
    act( "$n&+c conjures a massive hurricane.&n", ch, NULL, NULL, TO_ROOM );
    if (ch->fly_level < MAX_FLY)
      act("&+CThe wind below you becomes dangerously strong!&n.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+CAbove you, a storm of dangerous strength rages!&n.", ch, NULL, NULL, TO_BELOW);

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
      victim_next = victim_next->next_in_room;

      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      dam = dice( (3 * level), 3);
      if ( saves_spell( level, victim, DAM_WIND ) )
          dam /= 2;

      spell_damage( ch, victim, dam, sn, DAM_WIND );
    }

    return;
}

//Fire elementalist haste spell, does 10 hp damage as well.
//Not sure if I put the damage call in the right place-Xueqin.
void spell_flashfire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int        dam;

    last_fun( "spell_flashfire");
  
    // Removes slowness, takes two castings to make a slowed person hasted
    if( has_affect( victim, 0, spl_slowness, 0) || is_affected( victim, AFF_SLOWNESS ))
    {
        affect_strip( victim, 0, spl_slowness, 0);
        REMOVE_AFF_BIT( victim, AFF_SLOWNESS );
        dam = 10;
        spell_damage( ch, victim, dam, sn, DAM_FIRE );

    act( "$n&+R is engulfed by burning sparks.&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+RThe burning sparks dispel the magic that slows you.&n\n\r", victim );

        return;
    }

    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_HASTE ))
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + level / 8;
    af.location  = APPLY_AC;
    af.modifier  = -10;
    set_bitvector( &af, AFF_HASTE);
    affect_to_char( victim, &af );
    
    dam = 10;
    victim->hit -= dam;
    update_pos( victim );

    act( "&+WSparks&N&+R surround $n&+R as they begin to move much faster!&n", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+RThe magical sparks burn you as the world around you slows down.&n\n\r", victim );
    return;
}

// Bombard - level 35 cap - (level)d3 for three rounds - Area Effect
// At level 35: minimum 35, maximum 105, average 70

void spell_bombard( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int       dam;

    last_fun( "spell_bombard");

    if(level > 35) level = 35;

    dam = dice( level, 3 );

    act( "&+yYou bombard the area with &+Lrocks and stones.&n", ch, NULL, NULL, TO_CHAR );
    act( "&+y$n&+y conjures &+Lrocks and stones&n&+y to bombard the area.&n", ch, NULL, NULL, TO_ROOM );

    create_event( EVENT_BOMBARD, 1, ch, ch->in_room, dam );
    create_event( EVENT_BOMBARD, (PULSE_VIOLENCE + 3), ch, ch->in_room, dam );
    create_event( EVENT_BOMBARD, (PULSE_VIOLENCE * 2 + 5), ch, ch->in_room, dam );
    
    return;
}

/* Ok, taking a shot at mass dispel magic.  Copying the offensive part of the present
dispel magic spell and trying to make it area affect.  This spell won't hit group
members. I have no idea if this is even close, but I am kinda tired right now.
The way I have tried to do this makes this spell only offensive.  I can't think
of many situations that your group members will want you to mass dispel them.
-Xueqin*/
void spell_mass_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;

    last_fun( "spell_mass_dispel_magic");

    send_to_char( "&+MYou try to draw the magic out of your foes!&n\n\r", ch );
    act( "&+M$n&+M fills the area with a magical haze.&n", ch, NULL, NULL, TO_ROOM );
    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
    
    for ( paf = victim->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
            continue;
            if ( !paf->spell )
                continue;
        if ( !saves_spell( level, victim, DAM_OTHER ) )
        {
        send_to_char( spells_table[paf->spell].msg_off, victim );
        send_to_char( "\n\r", victim );
        act( "$n&n is no longer affected by '$t'.",
            victim, spells_table[paf->spell].name, NULL, TO_ROOM );
        affect_strip( victim, 0, paf->spell, 0 );
        break;
        }
    }
    /* ALWAYS give a shot at removing sanctuary */
    if ( is_affected( victim, AFF_SANCTUARY)
        && !saves_spell( level, victim, DAM_OTHER ) )
    {
        REMOVE_AFF_BIT( victim, AFF_SANCTUARY );
        send_to_char( "The white aura around your body is drained away.\n\r",
                                victim );
        act( "The white aura around $n's body is drawn into the purple haze.",
            victim, NULL, NULL, TO_ROOM );
    }
    if ( !victim->fighting || !is_same_group( ch, victim->fighting ) )
      kill_ch( victim, ch );
    }
    return;
}

/* This spell is supposed to add strength above the maximum limit.  Not really
sure if it will.  There is nothing to limit it to the racial max so I am 
guessing it should.-Xueqin */
void spell_stonestrength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_stonestrength");

    if ( has_affect( victim, 0, sn, 0 ) )
    return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.song      = 0;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 10 + level / 6;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    if ( ch != victim )
    send_to_char( "&+yYou call upon the elements.&n\n\r", ch );
    send_to_char( "&+yYou feel strong and sturdy as a &+Lrock.&n\n\r", victim );
    
    return;
}

void spell_wall_of_mist( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *wall;
    char buf[MAX_STRING_LENGTH];

    last_fun( "spell_wall_of_mist");

    wall = make_wall(sn, level, ch, vo, OBJ_VNUM_WALL_MIST);
    if (wall) {
        sprintf( buf, "$p&+c coalesces in front of the %s exit.&n", dir_name[wall->value[0]] );
        act( buf, ch, wall, NULL, TO_ALL );
    }
    return;
}

void spell_shadow_monsters( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *image;
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    last_fun( "spell_shadow_monsters");

      image = create_mobile( get_mob_index( MOB_VNUM_SHADOW_MONSTER ));
      char_to_room( image, ch->in_room );

      image->money.copper = 0;
      image->money.silver = 0;
      image->money.gold = 0;
      image->money.platinum = 0;
      image->level = ch->level + (number_range( -1, 4 ));
      if( image->level > 40 )
        image->level = 40;
      image->max_hit = dice( 20, ch->level ) + 3000;
      image->hit = image->max_hit;
      act( "$n&n appears.", ch, NULL, NULL, TO_ROOM );
      add_follower( image, ch );
      image->master = ch;
      SET_BIT( image->act, ACT_NOEXP );
      SET_BIT( image->act, ACT_PET );
      SET_AFF_BIT( image, AFF_CHARM );
      
    if( target_name[0] == '\0' )
    {
      if( ch->fighting )
        set_fighting( image, ch->fighting );
    }
    else
    {
      if( !( victim = get_char_room( ch, target_name )))
      {
        if( ch->fighting )
          set_fighting( image, ch->fighting );
      }
      else
      {
          set_fighting( image, victim );
      }
    }

    // 2.87 rounds plus a tick per level... max of 7 rounds
    create_event( EVENT_EXTRACT_CHAR, ((ch->level) + 34), image, NULL, 0 );

    return;
}

void spell_demi_shadow_monsters( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *image;
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    last_fun( "spell_demi_shadow_monsters");

      image = create_mobile( get_mob_index( MOB_VNUM_DEMI_SHADOW_MONSTER ));
      char_to_room( image, ch->in_room );

      image->money.copper = 0;
      image->money.silver = 0;
      image->money.gold = 0;
      image->money.platinum = 0;
      image->level = ch->level + (number_range( -1, 5 ));
      if( image->level > 45 )
        image->level = 45;
      image->max_hit = dice( 28, ch->level ) + 6000;
      image->hit = image->max_hit;
      act( "$n&n appears.", ch, NULL, NULL, TO_ROOM );
      add_follower( image, ch );
      image->master = ch;
      SET_BIT( image->act, ACT_PET );
      SET_BIT( image->act, ACT_NOEXP );
      SET_AFF_BIT( image, AFF_CHARM );
      
    if( target_name[0] == '\0' )
    {
      if( ch->fighting )
        set_fighting( image, ch->fighting );
    }
    else
    {
      if( !( victim = get_char_room( ch, target_name )))
      {
        if( ch->fighting )
          set_fighting( image, ch->fighting );
      }
      else
      {
          set_fighting( image, victim );
      }
    }

    create_event( EVENT_EXTRACT_CHAR, (ch->level + 52), image, NULL, 0 );

    return;
}

void spell_shades( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *image;
    CHAR_DATA *victim;
    char *target_name = (char *) vo;

    last_fun( "spell_shades");

      image = create_mobile( get_mob_index( MOB_VNUM_SHADE ));
      char_to_room( image, ch->in_room );

      image->money.copper = 0;
      image->money.silver = 0;
      image->money.gold = 0;
      image->money.platinum = 0;
      image->level = ch->level + (number_range( -1, 6 ));
      if( image->level > 50 )
        image->level = 50;
      image->max_hit = dice( 17, ch->level ) + 10000;
      image->hit = image->max_hit;
      act( "$n&n appears.", ch, NULL, NULL, TO_ROOM );
      add_follower( image, ch );
      image->master = ch;
      SET_BIT( image->act, ACT_PET );
      SET_BIT( image->act, ACT_NOEXP );
      SET_AFF_BIT( image, AFF_CHARM );
      
    if( target_name[0] == '\0' )
    {
      if( ch->fighting )
        set_fighting( image, ch->fighting );
    }
    else
    {
      if( !( victim = get_char_room( ch, target_name )))
      {
        if( ch->fighting )
          set_fighting( image, ch->fighting );
      }
      else
      {
          set_fighting( image, victim );
      }
    }

    create_event( EVENT_EXTRACT_CHAR, (ch->level + 64), image, NULL, 0 );

    return;
}

// Testing something.  Lohrr
void do_chill( CHAR_DATA *ch, char *argument )
{

    last_fun( "do_chill");

    if ( !ch->fighting )
    {
        send_to_char ("You must be fighting.\n\r", ch);
        return;
    }

    spell_bigby_fist( spl_bigby_fist, ch->level, ch, ch->fighting );

    return;
}

//  This spell works now. Lohrr
void spell_resurrect( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *corpse = (OBJ_DATA *) vo;
    OBJ_DATA    *obj2;
    OBJ_DATA    *obj_next;
    CHAR_DATA   *victim;
    char         buf[ MAX_STRING_LENGTH ];
    char        *name;

    last_fun( "spell_resurrect");

    if (!corpse) {
       bug("NULL corpse pointer in resurrection spell", 0);
       send_to_char("You can't seem to resurrect that.\n\r", ch);
       return;
    }

    if (corpse->pIndexData->vnum != OBJ_VNUM_CORPSE_PC) {
       send_to_char("That corpse is beyond your capabilities to resurrect.\n\r", ch);
       return;
    }

    /* corpse of %s */
    name = one_argument( corpse->name, buf );
    name = one_argument( name, buf );

    /* Name of corpse is the name of the char to be ressed. */
    if ( !( victim = get_char_world( ch, name ) )
         || ( !victim->in_room )
         || ( victim->deleted )
         || ( IS_NPC( victim ) )
         || ( !( victim->pcdata ) )
         || ( victim->pcdata->consent != ch && !IS_IMMORTAL( ch ) )
         || ( corpse == NULL || corpse->in_room != ch->in_room )
         || ( corpse->value[1] == 1) )
    {
        send_to_char( "You failed.\n\r", ch);
        if( IS_IMMORTAL( ch ) )
        {
          send_to_char( "Name: '", ch );
          send_to_char( name, ch );
          send_to_char( "'.\n\r", ch );
          if( !victim )
            send_to_char( "No victim found!\n\r", ch );
          else if ( !victim->in_room )
            send_to_char( "Victim's room not found!\n\r", ch );
          else if ( victim->deleted )
            send_to_char( "Victim's deleted!\n\r", ch );
          else if ( IS_NPC( victim ) )
            send_to_char( "Victim's a NPC!\n\r", ch );
          else if ( !( victim->pcdata ) )
            send_to_char( "Victim's got no pcdata!\n\r", ch );
          else if ( corpse == NULL || corpse->in_room != ch->in_room )
            send_to_char( "Corpse is broken!\n\r", ch );
          if ( !(victim = get_char_world( ch, name ) ) )
            send_to_char( "Victim still not found!.\n\r", ch );
        }
        return;
    }

    if( number_percent() < 60 - level )
    {
        corpse->value[1] = 1;
        save_corpses();
        send_to_char( "You watch the corpse shrvel.\n\r", ch);
        return;
    }

    /* Drop the victims eq. */
    for ( obj2 = victim->carrying; obj2; obj2 = obj_next )
    {
        obj_next = obj2->next_content;

        if ( obj2->deleted )
          continue;
        obj_from_char( obj2 );

        obj_to_room( obj2, victim->in_room );
        act( "$p falls to the ground.&n", victim, obj2, NULL, TO_ROOM );
    }

    /* Move the victim. */
    act("$n howls in pain as $s body crumbles into dust.",victim, NULL, NULL, TO_ROOM);
    char_from_room( victim );
    char_to_room( victim, ch->in_room );

    /* Put the corpses eq on the char. */
    for ( obj2 = corpse->contains; obj2; obj2 = obj_next )
    {
        obj_next = obj2->next_content;

        obj_from_obj( obj2 );
        if (obj2->item_type == TYPE_MONEY) {
            ch->money.copper += obj2->value[0];
            ch->money.silver += obj2->value[1];
            ch->money.gold += obj2->value[2];
            ch->money.platinum += obj2->value[3];
            extract_obj( obj2 );
        }
        else
            obj_to_char( obj2, victim );
    }

    act( "&+WAn aura of intensely bright light surrounds the $p for a moment.&n", victim, corpse, NULL, TO_ROOM );
    act( "&+W$n&+W comes to life again! Taking a deep breath, $n&+W opens $s eyes!&n", victim, NULL, NULL, TO_ROOM );

    /* Remove the corpse. */
    extract_obj( corpse );

    // Prevent corpse duping -- Veygoth
    save_corpses();

    ch->move = 0;
    sprintf( buf, "&+WYou are extremely tired after resurrecting %s.&n\n\r", name );
    send_to_char( buf, ch);
    victim->move = 0;
    victim->hit = get_max_hit( victim );
    sprintf( buf, "&+WYou are extremely tired after being resurrected.&n\n\r" );
    send_to_char( buf, victim);
    send_to_char( "You are resurrected!\n\r", victim);

    if (!IS_IMMORTAL( ch ) )
        WAIT_STATE( ch, 50 );

    // Since we know they lose 12.5% plus half a percent per level exp, we know
    // how much we can give back in order to give them 76.92% of what they lost.
    // This equation was taken from damage() and spell_damage() in fight.c
    // -- Veygoth
//    gain_exp( victim, (((25 + victim->level) * exp_table[victim->level].lvl_exp ) / 325 ));
/* Modified to reflect lower exp loss at death 
 * Restores 80% of lost exp
 * Lithos 5/31/2000
 */
    // However, since people level 5 and under will no longer lose exp from
    // death, they should no longer gain it from a ress.
    if( victim->level > 5 && !IS_IMMORTAL( ch ) )
      gain_exp( victim, (((50 + victim->level) * exp_table[victim->level].lvl_exp ) / 500 ));
    else if( IS_IMMORTAL( ch ) )
      gain_exp( victim, (((50 + victim->level) * exp_table[victim->level].lvl_exp ) / 400 ));
    save_char_obj( victim );

    return;
}

/* The spell to summon one of the four primary elementals */
void spell_summon_elemental( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *elemental, *pch;
    AFFECT_DATA af;
    char buf[100];
    int eindex, odds, maxpets, numpets;
    int odds_earth, odds_air, odds_fire, odds_water;
    bool pissed_off = FALSE;

    last_fun( "spell_summon_elemental");
    numpets = 0;
    for (pch = char_list; pch; pch = pch->next) {
        if ( pch->deleted )
            continue;
        if (pch != ch && pch->master == ch &&
             IS_NPC( pch ) && IS_SET( pch->act, ACT_PET ) )
             numpets++;
    }
    // just a wild guess as far as number...check for some consistency with necro
    maxpets = ch->level / 20 + get_curr_cha(ch)  / 35;
    if (ch->level >= LEVEL_IMMORTAL) {
        sprintf(buf,"You can summon at most %d pets.\n\r", maxpets);
        send_to_char(buf, ch);
    }
    if (numpets >= maxpets) {
        send_to_char("You cannot handle any more pets!\n\r", ch);
        return;
    }
    //set odds weighted toward char class
    odds_air=20; odds_water=20; odds_fire=20; odds_earth=20;
    switch (ch->class){
    case CLASS_ELEMENT_EARTH:
        odds_earth += 35;
        odds_air -= 15;
        break;
    case CLASS_ELEMENT_AIR:
        odds_air += 35; 
        odds_earth -= 15;
        break;
    case CLASS_ELEMENT_FIRE:
        odds_fire += 35;
        odds_water -= 15;
        break;
    case CLASS_ELEMENT_WATER:
        odds_water += 35; 
        odds_fire -= 15;
        break;
    default:
                if(ch->level >= LEVEL_IMMORTAL) {
                    odds_air += 5;
                    odds_water += 5;
                    odds_earth += 5;
                    odds_fire += 5;
                }
                else {
                   send_to_char("This spell is not available to your class.\n\r", ch);
                   bug("spell_summon_elemental:called by invalid class", 0);
                   return;
                }
    }
    odds = dice(1, 100);
    if (odds <= odds_air) eindex = MOB_VNUM_AIR_ELEMENTAL;
    else if (odds <= odds_water + odds_air) eindex = MOB_VNUM_WATER_ELEMENTAL;
    else if (odds <= odds_water + odds_air + odds_fire) eindex = MOB_VNUM_FIRE_ELEMENTAL;
    else eindex = MOB_VNUM_EARTH_ELEMENTAL;

      elemental = create_mobile( get_mob_index( eindex ));
      char_to_room( elemental, ch->in_room );

      elemental->money.copper = 0;
      elemental->money.silver = 0;
      elemental->money.gold = 0;
      elemental->money.platinum = 0;
// This is in bad taste: mob lvl is directly proportional to it's damage.
//	- Lohrr
//      elemental->level = ch->level + 5 - dice(2, 6);
      elemental->level = 18 + dice( 3, 3);
      elemental->max_hit = 80 + dice( 7, level );
      elemental->hit = elemental->max_hit;
      elemental->armor -= ch->level;
      SET_BIT( elemental->act, ACT_NOEXP );

      switch (eindex) {
      case MOB_VNUM_EARTH_ELEMENTAL:
          act( "$N&n rises from the ground before your eyes.", ch, NULL, elemental, TO_ROOM );
          act( "$N&n rises from the ground before your eyes.", ch, NULL, elemental, TO_CHAR );
          if (ch->class == CLASS_ELEMENT_AIR && ch->level < LEVEL_IMMORTAL) pissed_off = TRUE;
          break;
      case MOB_VNUM_AIR_ELEMENTAL:
          elemental->armor = -200;
          act( "$N&n appears out of thin air.", ch, NULL, elemental, TO_ROOM );
          act( "$N&n appears out of thin air.", ch, NULL, elemental, TO_CHAR );
          if (ch->class == CLASS_ELEMENT_EARTH && ch->level < LEVEL_IMMORTAL) pissed_off = TRUE;
          break;
      case MOB_VNUM_FIRE_ELEMENTAL:
          act( "$N&n bursts into existence in a roaring ball of flame.", ch, NULL, elemental, TO_ROOM );
          act( "$N&n bursts into existence in a roaring ball of flame.", ch, NULL, elemental, TO_CHAR );
          if (ch->class == CLASS_ELEMENT_WATER && ch->level < LEVEL_IMMORTAL) pissed_off = TRUE;
          break;
      case MOB_VNUM_WATER_ELEMENTAL:
          act( "$N&n coalesces into existence.", ch, NULL, elemental, TO_ROOM );
          act( "$N&n coalesces into existence.", ch, NULL, elemental, TO_CHAR );
          elemental->max_hit += 100;
          elemental->hit = elemental->max_hit;
          elemental->level++;
          if (ch->class == CLASS_ELEMENT_FIRE && ch->level < LEVEL_IMMORTAL) pissed_off = TRUE;
          break;
      default: 
          bug("spell_summon_elemental: bad vnum in switch", 0);
          send_to_char("You managed to summon a bad vnum! Shame on you.\n\r", ch);
          //kill the elemental
          extract_char( elemental, TRUE);
          last_fun( "spell_summon_elemental");
          return;
      }
      if (pissed_off) {
          ;
      } else {
        act( "$N&n says 'Your wish is my command $n&n.'", ch, NULL, elemental, TO_ROOM );
        act("$N&n tells you 'Your wish is my command.'", ch, NULL, elemental, TO_CHAR );
        add_follower( elemental, ch );
        elemental->master = ch;
        af.skill     = 0;
        af.spell     = sn;
        af.song      = 0;
        af.duration  = level/2 + dice(4, level/2);
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        set_bitvector( &af, AFF_CHARM);
        affect_to_char( elemental, &af );
        //set the ACT_PET bit as well
        SET_BIT( elemental->act, ACT_PET );
      
      elemental->fly_level = ch->fly_level;
      if( ch->fighting )
        set_fighting( elemental, ch->fighting );
      }
      //no event to delete after char dies; elemental remains
    return;
}

/* At this time I'm just implementing the spell, I hope to make it
 * so the elemental summoned is related to the caster's focus plane
 *
 * Lithos
 */
void spell_summon_gr_elem( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *elemental, *pch;
    AFFECT_DATA af;
    char buf[100];
    int eindex, maxpets, numpets;
    bool aggro = FALSE;
    int odds[] = {40, 50, 60, 70, 80, 88, 96, 100};
    int i, j, percent;
    int class_lookup[4][8] = {
        /* Earth Elem earth,magma,ooze,mineral,dust, fire,water, air */
        {              306, 208,  207,  215,    204,  304 , 305, 307} ,
        /* Fire Elem fire,smoke, magma,plasma, ash, earth, air, water */
        {              304, 205,   208,  209,    210, 306,  307, 305},
        /* Air elem  air, smoke, ice, lightning,vacuum,fire,water,earth */
        {              307,  205,  206,     213,  214,  304,  305,  306},
        /* Water elem  water, ice, ooze, steam, salt, earth, air, fire */
        {               305,  206,  207,  211,   212,  306,  307,  304}  
        };
    int god_lookup[] = {204, 205, 206, 207, 208, 209, 210, 212, 213,
                         304, 305, 306, 307};

    last_fun( "spell_summon_gr_elem");

    eindex = 0;
    if ( IS_IMMORTAL( ch )  ) {
        eindex = god_lookup[ number_range( 0, 11 ) ];
    } else {
        percent = number_percent();
        for ( i = 0; i < 8; i++ ) {
            if ( percent < odds[i] ) break;
        }
        if ( i >= 8 ) {
            send_to_char("Whoops! Unable to find an elemental for you. Bug it\n\r", ch);
            bug("summon_gr_elem: invalid index in class_lookup.", 0);
            return;
        }
        switch ( ch->class ) {
            case CLASS_ELEMENT_EARTH: j = 0; break;
            case CLASS_ELEMENT_FIRE: j = 1; break;
            case CLASS_ELEMENT_AIR: j = 2; break;
            case CLASS_ELEMENT_WATER: j = 3; break;
            default: 
                j=-1;
                return;
                break;
            }
        eindex = class_lookup[j][i];
        if ( i == 7 ) aggro = TRUE;
    }
    numpets = 0;
    for (pch = char_list; pch; pch = pch->next) {
        if ( pch->deleted )
            continue;
        if (pch != ch && pch->master == ch &&
             IS_NPC( pch ) && IS_SET( pch->act, ACT_PET ) )
             numpets++;
    }
    //just a WAG as far as number...check for some consistency with necro
    maxpets = ch->level / 20 + get_curr_cha(ch)  / 35;
    if (ch->level >= LEVEL_IMMORTAL) {
        sprintf(buf,"You can summon at most %d pets.\n\r", maxpets);
        send_to_char(buf, ch);
    }
    if (numpets >= maxpets) {
        send_to_char("You cannot handle any more pets!\n\r", ch);
        return;
    }
//    eindex = number_range(0,13) + MOB_VNUM_DUST_ELEMENTAL;
      elemental = create_mobile( get_mob_index( eindex ));
      char_to_room( elemental, ch->in_room );

      elemental->money.copper = 0;
      elemental->money.silver = 0;
      elemental->money.gold = 0;
      elemental->money.platinum = 0;
// This is in bad taste: mob lvl is directly proportional to it's damage.
//	- Lohrr
//      elemental->level = ch->level + 3 - dice(2, 5);
      elemental->level = 38 + dice( 5, 2);
      elemental->max_hit = 750 + dice( 15, level/2 );
      elemental->hit = elemental->max_hit;
      elemental->armor -= (ch->level * 2);
      SET_BIT( elemental->act, ACT_NOEXP );

        act( "$n&n calls upon the elements to bring $N&n into existence.", ch, NULL, elemental, TO_ROOM );
        act( "You call upon the elements to bring $N&n into existence.", ch, NULL, elemental, TO_CHAR );
        act( "$N&n says 'Your wish is my command $n&n.'", ch, NULL, elemental, TO_ROOM );
        act("$N&n tells you 'Your wish is my command.'", ch, NULL, elemental, TO_CHAR );
        if ( !aggro ) {
        add_follower( elemental, ch );
        elemental->master = ch;
        af.skill     = 0;
        af.spell     = sn;
        af.song      = 0;
        af.duration  = level/2 + dice(4, level/2);
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        set_bitvector( &af, AFF_CHARM);
        affect_to_char( elemental, &af );
        //set the ACT_PET bit as well
        SET_BIT( elemental->act, ACT_PET );
      
      elemental->fly_level = ch->fly_level;
      if( ch->fighting )
        set_fighting( elemental, ch->fighting );
        } else {
        kill_ch( elemental, ch );
        }
      //no event to delete after char dies; elemental remains
    return;
}


void spell_summon_serpent( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_summon_serpent");

    summon_elem( sn, level, ch, MOB_VNUM_FIRE_SERPENT);
    return;
}
void spell_summon_nerneid( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_summon_nerneid");

    summon_elem( sn, level, ch, MOB_VNUM_WATER_NERNEID);
    return;
}

void spell_summon_pech( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_summon_pech");

    summon_elem( sn, level, ch, MOB_VNUM_EARTH_PECH);
    return;
}

void spell_summon_slyph( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_summon_slyph");

    summon_elem( sn, level, ch, MOB_VNUM_AIR_SLYPH);
    return;
}

void summon_elem(int sn, int level, CHAR_DATA *ch, int vnum)
{
    CHAR_DATA *elemental, *pch;
    AFFECT_DATA af;
    char buf[100];
    int maxpets, numpets;

    last_fun( "summon_elem");

    numpets = 0;
    for (pch = char_list; pch; pch = pch->next) {
        if ( pch->deleted )
            continue;
        if (pch != ch && pch->master == ch &&
             IS_NPC( pch ) && IS_SET( pch->act, ACT_PET ) )
             numpets++;
    }

    //just a WAG as far as number...check for some consistency with necro
    maxpets = ch->level / 20 +get_curr_cha(ch)  / 35;
    if (ch->level >= LEVEL_IMMORTAL) {
        sprintf(buf,"You can summon at most %d pets.\n\r", maxpets);
        send_to_char(buf, ch);
    }
    if (numpets >= maxpets) {
        send_to_char("You cannot handle any more pets!\n\r", ch);
        return;
    }
      elemental = create_mobile( get_mob_index( vnum ));
      char_to_room( elemental, ch->in_room );

      elemental->money.copper = 0;
      elemental->money.silver = 0;
      elemental->money.gold = 0;
      elemental->money.platinum = 0;
// This is in bad taste: mob lvl is directly proportional to it's damage.
//	- Lohrr
//      elemental->level = ch->level + 3 - dice(2, 5);
      elemental->level = 27 + dice( 2, 5);
      elemental->max_hit = 150 + dice( 16, level/2 );
      elemental->hit = elemental->max_hit;
      elemental->armor -= (ch->level * 3 / 2 );
      SET_BIT( elemental->act, ACT_NOEXP );

      switch (vnum) {
      case MOB_VNUM_EARTH_PECH:
          act( "$n&n rises from the ground before your eyes.", elemental,
            NULL, NULL, TO_ROOM );
          break;
      case MOB_VNUM_AIR_SLYPH:
          act( "$n&n appears out of thin air.", elemental, NULL, NULL, TO_ROOM );
          break;
      case MOB_VNUM_FIRE_SERPENT:
          act( "$n&n bursts into existence in a roaring ball of flame.",
            elemental, NULL, NULL, TO_ROOM );
          break;
      case MOB_VNUM_WATER_NERNEID:
          act( "$n&n coalesces into existence.", elemental, NULL, NULL, TO_ROOM );
          elemental->max_hit += 100;
          elemental->hit = elemental->max_hit;
          break;
      default:
          bug("spell_summon_elemental: bad vnum in switch", 0);
          send_to_char("You managed to summon a bad vnum! Shame on you.\n\r", ch);
    }
    act( "$N&n says 'Your wish is my command $n&n.'", ch, NULL, elemental, TO_ROOM );
    act("$N&n tells you 'Your wish is my command.'", ch, NULL, elemental, TO_CHAR );
    add_follower( elemental, ch );
    elemental->master = ch;
    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level/2 + dice(4, level/2);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_CHARM);
    affect_to_char( elemental, &af );
    //set the ACT_PET bit as well
    SET_BIT( elemental->act, ACT_PET );

    elemental->fly_level = ch->fly_level;

    if( ch->fighting )
        set_fighting( elemental, ch->fighting );

    return;
}


void spell_dismissal( int sn, int level, CHAR_DATA *ch, void *vo ) {
    CHAR_DATA *victim = (CHAR_DATA *)vo;

    last_fun( "spell_dismissal");

    if (ch == victim) {
        send_to_char("You must find another way to leave this plane.\n\r", ch );
        return;
    }
    if ( victim->race != RACE_ELEM_FIRE && victim->race != RACE_ELEM_AIR
        && victim->race != RACE_ELEM_EARTH && victim->race != RACE_ELEM_WATER )
    {
        send_to_char("They are not from the elemental planes!\n\r", ch);
        return;
    }
    if ( !victim->master || victim->master != ch ) {
        act("You seem to have no influence over $N&n.", ch, NULL, victim, TO_CHAR);
        return;
    }
    act( "$N&n&+Y cackles with anticipation as it disappears with a flash&n.",
        ch, NULL, victim, TO_CHAR );
    act( "$n&n &+Ysend &n$N$n&+Y back to the plane of its origin&n.",
        ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    last_fun( "spell_dismissal");
    return;
}

void spell_plane_shift( int sn, int level, CHAR_DATA *ch, void *vo )
{
//    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    char *arg = (char *)vo;
//    char *arg;
    ROOM_INDEX_DATA *pRoomIndex;
    int count;
    bool dest_astral = FALSE;

    last_fun( "spell_plane_shift");

//    argument = one_argument( argument, arg );
    if ( !strcmp( arg, "astral" ) ) {
        dest_astral = TRUE;
        if ( ch->in_room->sector_type == SECT_PLANE_ASTRAL ) {
            send_to_char( "You are already on the astral plane.\n\r", ch);
            return;
        }
        if ( ch->in_room->sector_type == SECT_PLANE_FIRE ||
           ch->in_room->sector_type == SECT_PLANE_AIR ||
           ch->in_room->sector_type == SECT_PLANE_EARTH ||
           ch->in_room->sector_type == SECT_PLANE_WATER ||
           ch->in_room->sector_type == SECT_PLANE_ETHEREAL
           ) {
            send_to_char("You can only shift there from the prime plane.\n\r", ch);
            return;
        }

    }
    else if ( !strcmp( arg, "prime" ) ) {
        if (ch->in_room->sector_type != SECT_PLANE_ASTRAL ) {
            send_to_char("You can only shift there from the astral plane.\n\r", ch);
            return;
        }
    }
    else if ( !strcmp( arg, "ethereal") || !strcmp( arg, "earth") 
        || !strcmp( arg, "air" ) || !strcmp( arg, "water" ) || !strcmp( arg, "fire" ) )
    {
        send_to_char( "You cannot shift to that plane!\n\r", ch );
        return;
    }
    else {
        send_to_char( "Exactly WHICH plane do you want to shift to?\n\r", ch );
        return;
    }

    if( IS_ROOM( ch->in_room, ROOM_NO_GATE))
    {
        send_to_char( "The magic of this room prevents you from leaving.\n\r", ch );
        return;
    }

    // If they can't find a valid target in 10 tries, give up.
    for ( count = 0; count < 10; count++ )
    {
        if ( dest_astral )
            pRoomIndex = get_room_index( number_range( ROOM_VNUM_ASTRAL_LOW, ROOM_VNUM_ASTRAL_HIGH ) );
        else 
            pRoomIndex = get_random_map_room( 0 );
        if ( pRoomIndex )
        {
          if (   !IS_ROOM( pRoomIndex, ROOM_PRIVATE  )
             && !IS_ROOM( pRoomIndex, ROOM_SOLITARY )
             && !IS_ROOM( pRoomIndex, ROOM_NO_GATE)
             && ( ch->in_room->area == pRoomIndex->area ) )
          {
            break;
          }
        }
    }

    if( !pRoomIndex )
    {
       send_to_char( "You failed.\n\r", ch );
       return;
    }

    if ( ch->fighting )
        stop_fighting( ch, TRUE );
    act( "$n&n slowly fades out of existence.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, pRoomIndex );
    act( "$n&n slowly fades into existence.",   ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    return;
}

// You can hit evil aligned group members.  For now this is intentional.
// -- Veygoth
void spell_judgement( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;
  
    last_fun( "spell_judgement");

    if ( !IS_NPC( ch ) && !IS_GOOD( ch ) )
    {
    send_to_char( "Your god blasts your for your blasphemy.\n\r", ch );
        spell_damage( ch, ch, 800, sn, DAM_WHITE_MANA );
    return;
    }

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;

      if( victim->deleted || victim == ch || is_same_group( victim, ch )) continue;
      if ( IS_GOOD( victim ) )
      {
        spell_bless( spl_bless, level, ch, victim );
        if( victim->alignment > 750 )
        {
          spell_armor( spl_armor, level, ch, victim );
        }
        continue;
      }

      dam = level * 8;

      if ( saves_spell( level, victim, DAM_WHITE_MANA ) )
         dam /= 2;

      spell_damage( ch, victim, dam, sn, DAM_WHITE_MANA );
      if( victim->position == POS_DEAD )
        continue;
      if( !saves_spell( level, victim, DAM_WHITE_MANA ))
      {
        send_to_char( "You are thrown to the ground!\n\r", victim );
        act( "$n&n is thrown to the ground!", victim, NULL, NULL, TO_ROOM );
        if( victim->position > POS_SITTING )
          victim->position = POS_SITTING;
        WAIT_STATE( victim, 12 );
      }

      if( victim->alignment < -750 )
      {
        spell_blindness( spl_blindness, level, ch, victim );
      }

    }

    return;
}

// You can hit good aligned group members.  For now this is intentional.
// -- Veygoth
void spell_apocalypse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;
  
    last_fun( "spell_apocalypse");

    if ( !IS_NPC( ch ) && !IS_EVIL( ch ) )
    {
    send_to_char( "Your god blasts your for your blasphemy.\n\r", ch );
        spell_damage( ch, ch, 800, sn, DAM_BLACK_MANA );
    return;
    }

    for( victim = victim_next = ch->in_room->people; victim; victim = victim_next )
    {
       victim_next = victim_next->next_in_room;

      if( victim->deleted || victim == ch || is_same_group( victim, ch )) continue;
      if ( IS_EVIL( victim ) )
      {
        spell_bless( spl_bless, level, ch, victim );
        if( victim->alignment < -750 )
        {
          spell_armor( spl_armor, level, ch, victim );
        }
        continue;
      }

      dam = level * 8;

      if ( saves_spell( level, victim, DAM_BLACK_MANA ) )
         dam /= 2;

      spell_damage( ch, victim, dam, sn, DAM_BLACK_MANA );
      if( victim->position == POS_DEAD )
         continue;
      if( !saves_spell( level, victim, DAM_BLACK_MANA ))
      {
        spell_curse( spl_curse, level, ch, victim );
      }

      if( !saves_spell( level, victim, DAM_BLACK_MANA ))
      {
        spell_wither( spl_wither, level, ch, victim );
      }

      if( victim->alignment > 750 )
      {
        spell_poison( spl_poison, level, ch, victim );
      }

      if( victim->alignment > 980 )
      {
        spell_plague( spl_plague, level, ch, victim );
      }

    }

    return;
}

// This spell will probably be difficult -- Veygoth
void spell_airy_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_airy_water");
    return;
}

// 5th circle air elementalist spell
// Not very powerful at all it seems.
// 56 - 356 damage at lvl 51, average of 206, 51 vs. players
// direct clone of cyclone but with more damage.
void spell_whirlwind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    int        dam;
    CHAR_DATA *victim_next;

    last_fun( "spell_whirlwind");

    send_to_char( "&+CYou conjure a powerful whirlwind!&N\n\r", ch );
    act("$n&n&+C conjures a powerful whirlwind!&n", ch, NULL, NULL, TO_ROOM);
    if (ch->fly_level < MAX_FLY)
      act("&+CA powerful whirlwind appears below you&n.", ch, NULL, NULL, TO_ABOVE);
    if (ch->fly_level > 0)
      act("&+CA powerful whirlwind appears above you&n.", ch, NULL, NULL, TO_BELOW);
    for( victim = ch->in_room->people; victim; victim = victim_next )
    {
        victim_next = victim->next_in_room;
      if( victim->deleted || is_same_group( victim, ch ) || victim == ch )
          continue;
      if (ch->fly_level != victim->fly_level)
          continue;

      // Spell_damage is used in each condition just because we want the message
      // to appear before the bash message

      dam = dice( 6, level ) + 50;
      if ( saves_spell( level, victim, DAM_WIND ) )
      {   
         spell_damage( ch, victim, dam/2, sn, DAM_WIND );
      }
      else if( victim->position > POS_SITTING && !saves_spell( level, victim, DAM_WIND ))
      {
         // if they fail two saves they get knocked over...
         spell_damage( ch, victim, dam, sn, DAM_WIND );
         if( victim->position == POS_DEAD )
             return;
         send_to_char( "&+cYou are knocked down by the powerful winds!&N\n\r", victim );
         act( "$n&n&+c is knocked over by the powerful winds!&N", victim, NULL, NULL, TO_ROOM );
         victim->position = POS_SITTING;
         WAIT_STATE( victim, 4 );
      }
      else
      {
         spell_damage( ch, victim, dam, sn, DAM_WIND );
      }
    }

    return;
}

void spell_misdirection( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_misdirection");

    if ( has_affect( victim, 0, sn, 0 ) || is_affected( victim, AFF_MISDIRECTION ))
       return;

    if( !saves_spell( level, victim, DAM_MENTAL ))
    {
      af.skill     = 0;
      af.spell     = sn;
      af.song      = 0;
      af.duration  = 6 + level / 8;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_MISDIRECTION);
      affect_to_char( victim, &af );

      act( "&+R$n&+R starts to move in an awkward and confused manner!&n", victim, NULL, NULL, TO_ROOM );
      send_to_char( "&+RYou're not very sure exactly where you're going.&n\n\r", victim );
    }
    else
    {
      send_to_char( "You feel disoriented for a moment, but it passes.\n\r", victim );
      send_to_char( "Nothing happens.\n\r", ch );
    }
    return;
}

void spell_change_self( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;
    int modifier = 0;
    char *arg = (char *) vo;

    last_fun( "spell_change_self");

    if ( is_affected( ch, AFF_CHANGE_SELF ) )
    {
      act( "You are already changed.", ch, NULL, NULL, TO_CHAR );
      return;
    }

    if( race_table[ch->race].racewar_side == RACEWAR_EVIL )
    {
      if( !str_cmp( arg, "ogre" ))
        modifier = RACE_OGRE;
      else if( !str_cmp( arg, "troll" ))
        modifier = RACE_TROLL;
      else if( !str_cmp( arg, "drow" ))
        modifier = RACE_DROW;
      else if( !str_cmp( arg, "goblin" ))
        modifier = RACE_GOBLIN;
      else if( !str_cmp( arg, "duergar" ))
        modifier = RACE_DUERGAR;
      else if( !str_cmp( arg, "orc" ))
        modifier = RACE_ORC;
      else if( !str_cmp( arg, "minotaur" ))
        modifier = RACE_MINOTAUR;
      else if( !str_cmp( arg, "rakshasa" ))
        modifier = RACE_RAKSHASA;
      else if( !str_cmp( arg, "flind" ))
        modifier = RACE_FLIND;
      else if( !str_cmp( arg, "githyanki" ))
        modifier = RACE_GITHYANKI;
    }
    else if( race_table[ch->race].racewar_side == RACEWAR_GOOD )
    {
      if( !str_cmp( arg, "human" ))
        modifier = RACE_HUMAN;
      else if( !str_cmp( arg, "gnome" ))
        modifier = RACE_GNOME;
      else if( !str_cmp( arg, "dwarf" ))
        modifier = RACE_DWARF;
      else if( !str_cmp( arg, "halfling" ))
        modifier = RACE_HALFLING;
      else if( !str_cmp( arg, "barbarian" ))
        modifier = RACE_BARBARIAN;
      else if( !str_cmp( arg, "elf" ))
        modifier = RACE_GREYELF;
      else if( !str_cmp( arg, "half-elf" ))
        modifier = RACE_HALFELF;
      else if( !str_cmp( arg, "centaur" ))
        modifier = RACE_CENTAUR;
      else if( !str_cmp( arg, "githzerai" ))
        modifier = RACE_GITHZERAI;
    }
    else
    {
      send_to_char( "This spell will not work on you.\n\r", ch );
      return;
    }

    if( !modifier )
    {
      send_to_char( "Nothing happens.\n\r", ch );
      return;
    }

    if( modifier == ch->race )
    {
      send_to_char( "That change is pointless.\n\r", ch );
      return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = dice( 3, level );
    af.location  = 0;
    af.modifier  = modifier;
    set_bitvector( &af, AFF_CHANGE_SELF);
    affect_to_char( ch, &af );

    send_to_char( "You feel different.\n\r", ch );

    return;

}

void spell_phantasmal_killer( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim       = (CHAR_DATA *) vo;
    int         dam;

    last_fun( "spell_shadow_magic");
    if( level > 40 )
       level = 40;

    dam = dice( (2 * level), 7 );

    if( disbelieve( level, victim, ch ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       spell_damage( ch, victim, ( dam / 2 ), sn, DAM_PIERCE );
    }
    else
    {
       spell_damage( ch, victim, dam, sn, DAM_PIERCE );
    }

    return;
}

// Three hits of damage of average 56 at level 50 (168 total)
// nightmares makes the victim fall asleep on a failed save for each damage hit.
// Save reduces damage by 60% instaed of the usual half.

// Veygoth - I upped the spell damage by a factor of 4 because of the
// mob to PC damage code.  I also changed the spell save to one
// based on disbelieve. -- Ski

void spell_nightmares( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int       dam;

    last_fun( "spell_nightmares");

    dam = dice( 8, (level / 2)) + 120;

//    if ( saves_spell( level, victim, DAM_MENTAL ) )
//      dam = (dam * 4) / 10;

    if( disbelieve( level, victim, ch ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       dam = dam / 4;
    }

    create_event( EVENT_NIGHTMARES, 6, ch, victim, dam );
    create_event( EVENT_NIGHTMARES, (PULSE_VIOLENCE * 2), ch, victim, dam );
    create_event( EVENT_NIGHTMARES, (PULSE_VIOLENCE * 3 + 6), ch, victim, dam );

    return;
}

// 5 hits over 3 rounds
// 32-130 damage per hit at lvl 50
// total of 160-650 damage, average of 405 unsaved
// As is traditional for illusionists, damage is greatly reduced if the victim saves.

// Veygoth - I upped the spell damage by a factor of 4 because of the
// mob to PC damage code.  I also changed the spell save to one
// based on disbelieve. -- Ski

void spell_wierd( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim      = (CHAR_DATA *) vo;
    int       dam;

    last_fun( "spell_wierd");

    dam = dice( 8, level ) + 30;

//    if ( saves_spell( level, victim, DAM_MENTAL ) )
//      dam = (dam * 4)/10;

    if( disbelieve( level, victim, ch ))
    {
       send_to_char( "They disbelieve!\n\r", ch );
       dam = dam / 4;
    }

    create_event( EVENT_WIERD, 1, ch, victim, dam );
    create_event( EVENT_WIERD, 10, ch, victim, dam );
    create_event( EVENT_WIERD, 19, ch, victim, dam );
    create_event( EVENT_WIERD, 28, ch, victim, dam );
    create_event( EVENT_WIERD, 37, ch, victim, dam );

    return;
}

// Illusionist debuff spell.  At level 50 it'll

void spell_cowardice( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_cowardice");

    if ( has_affect( victim, 0, sn, 0 ) || saves_spell( level, victim, DAM_MENTAL ) )
    {
      send_to_char( "You have failed.\n\r", ch );
      return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level / 3;
    af.location  = APPLY_HITROLL;
    af.modifier  = -2;
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = (ch->level / 4);
    affect_to_char( victim, &af );

    af.location  = APPLY_SUSCEPTIBLE;
    af.modifier  = RIS_MENTAL;
    affect_to_char( victim, &af );

    if ( ch != victim )
       act( "&+c$N&+c suddenly looks a lot less sure of $Mself.&n", ch, NULL, victim, TO_NOTVICT );

    send_to_char( "You feel very insecure.\n\r", victim );
    return;
}

void spell_illusion_prowess( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_illusion_prowess");

    af.skill     = 0;
    af.spell      = sn;
    af.song      = 0;
    af.duration  = 6 + (level);

    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_PROWESS);
    affect_to_char( victim, &af );

    send_to_char( "You appear more skillful than you really are.\n\r", victim );
    act( "$n&n looks more intimidating.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_illusion_incompetence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_illusion_incon");

    af.skill     = 0;
    af.spell      = sn;
    af.song      = 0;
    af.duration  = 6 + (level);

    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_INCOMPETENCE);
    affect_to_char( victim, &af );

    send_to_char( "You appear less skillful than you really are.\n\r", victim );
    act( "$n&n looks less intimidating.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_magnetism( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_magnetism");

    if ( get_curr_cha( victim ) >= get_max_cha( victim ) )
    {
        send_to_char("It seems to have no effect.\n\r", ch); 
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 12 + level;
    af.location  = APPLY_CHA;
    af.modifier  = level / 7 + dice( 2, 6 );
    set_bitvector( &af, AFF_NONE);
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel more attractive.\n\r", victim );
    return;
}

// Ball lightning
/*
 * Caps at level 40.
 * Damage at level 40 is min 80, max 640 per ball.  If victim fails
 * all saves, total min is 400, avg 1800, max 3200
 * If victim saves against first ball, min 40, avg 180, max 320
 */
void spell_ball_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam, i;

    last_fun( "spell_ball_lightning");
    if ( level > 40 ) level = 40;
    send_to_char("You feel the hairs on the back of your neck rise as your are covered with &+Bcrac&+Cling &n&+belec&+Wtricity!&n\n\r", ch );
    for (i = 0; i <= level / 10; i++ ) {
        dam = dice( (2 * level), 8 );

        if ( saves_spell( level, victim, DAM_ELECTRICITY ) ) {
            dam /= 2;
            act("&+WA ball of lightning zaps $N&n&+W and shorts out.&n", ch, NULL, victim, TO_CHAR);
            act("&+WA huge ball of electricity gives you quite a charge!&n", ch, NULL, victim, TO_VICT);
            act("&+WA ball of lightning leaps from $n's&n&+W hands, finding $N to be the shortest path to ground.&n", ch, NULL, victim, TO_ROOM_VICT);
            spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );
            return;
        }
        else {
/*
            act("&+WA ball of lightning jumps from your fingertips, blasting $N&n.", ch, NULL, victim, TO_CHAR);
            act("&+WA ball of lightning jumps from $n's&n&+W fingertips, blasting you.&n", ch, NULL, victim, TO_VICT);
            act("&+WA ball of lightning jumps from $n's&n&+W fingertiogertips, blasting $N&n.", ch, NULL, victim, TO_ROOM_VICT);
*/
            spell_damage( ch, victim, dam, sn, DAM_ELECTRICITY );
        }
    }

    return;

}

void spell_stonestorm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *gch;
    char *arg = (char *) vo;
    int        door;
    OBJ_DATA *obj;

    last_fun( "spell_battering_stonestorm" );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Doorbash what?\n\r", ch );
        return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
        EXIT_DATA       *pexit;
        EXIT_DATA       *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
        {
            send_to_char( "Your spell fizzles!  The door is already open!\n\r", ch );
            return;
        }

        obj = create_object( get_obj_index( OBJ_VNUM_STONES ), level );
        obj_to_room( obj, ch->in_room );        

        if ( IS_SET( pexit->exit_info, EX_BASHPROOF )
            && !IS_IMMORTAL(ch) )
        {
            act( "&+LWHAAAAM!!!  You bombard the $d with a hail of giant stones, but it doesn't budge.&n",
                ch, NULL, pexit->keyword, TO_CHAR );
            act( "&+LWHAAAAM!!!  $n&n bombards the $d with giant stones, but it holds strong.&n",
                ch, NULL, pexit->keyword, TO_ROOM );
            return;
        }

        REMOVE_BIT( pexit->exit_info, EX_CLOSED );
        if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
          REMOVE_BIT( pexit->exit_info, EX_LOCKED );

        SET_BIT( pexit->exit_info, EX_BASHED );

        act( "&+LYou hurl a hail of giant stones at the $d, forcefully bashing it open!&n",
             ch, NULL, pexit->keyword, TO_CHAR );
        act( "&+L$n&+L conjures a hail of giant storms which bashes open the $d!&n",
             ch, NULL, pexit->keyword, TO_ROOM );

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
                    act( "&+BWith a rumble and a hail of stones, the $d crashes open!&n",
                        rch, NULL, pexit_rev->keyword, TO_CHAR );
                }

                obj = create_object( get_obj_index( OBJ_VNUM_STONES ), level);
                obj_to_room( obj, to_room );        

	    	// Have any aggro mobs on the other side come after the player
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
        send_to_char( "There is no door in that direction.\n\r", ch );
        return;
    }

    /*
     * Check for "guards"... anyone bashing a door is considered as
     * a potential aggressor, and there's a 25% chance that mobs
     * will do unto before being done unto.
     */

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

// Each of the starshell spells tentatively lasts 2 seconds per level of caster -- Veygoth
void spell_earthen_starshell( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_earthen_starshell");
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL)
      || IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    SET_ROOM_BIT( ch->in_room, ROOM_EARTHEN_STARSHELL );

    act( "&+yAn earthen starshell forms over the area!&n", ch, NULL, NULL, TO_ALL );

    create_event( EVENT_STARSHELL_END, (ch->level * 8), ch->in_room, NULL, 0 );
    return;
}

// Each of the starshell spells tentatively lasts 2 seconds per level of caster -- Veygoth
void spell_airy_starshell( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_airy_starshell");
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL)
      || IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    SET_ROOM_BIT( ch->in_room, ROOM_AIRY_STARSHELL );

    act( "&+CAn airy starshell forms over the area!&n", ch, NULL, NULL, TO_ALL );

    create_event( EVENT_STARSHELL_END, (ch->level * 8), ch->in_room, NULL, 0 );
    return;
}

// Each of the starshell spells tentatively lasts 2 seconds per level of caster -- Veygoth
void spell_fiery_starshell( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_fiery_starshell");
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL)
      || IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    SET_ROOM_BIT( ch->in_room, ROOM_FIERY_STARSHELL );

    act( "&+rA fiery starshell forms over the area!&n", ch, NULL, NULL, TO_ALL );

    create_event( EVENT_STARSHELL_END, (ch->level * 8), ch->in_room, NULL, 0 );
    return;
}

// Each of the starshell spells tentatively lasts 2 seconds per level of caster -- Veygoth
void spell_watery_starshell( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_darkness");
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL)
      || IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    SET_ROOM_BIT( ch->in_room, ROOM_WATERY_STARSHELL );

    act( "&+BA watery starshell forms over the area!&n", ch, NULL, NULL, TO_ALL );

    create_event( EVENT_STARSHELL_END, (ch->level * 8), ch->in_room, NULL, 0 );
    return;
}

// Hypnotic pattern lasts 1 second per level of caster -- Veygoth
void spell_hypnotic_pattern( int sn, int level, CHAR_DATA *ch, void *vo )
{
    last_fun( "spell_hypnotic_pattern");
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL)
      || IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL )
      || IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ))
    {
        send_to_char( "Nothing happens.\n\r", ch );
        return;
    }

    SET_ROOM_BIT( ch->in_room, ROOM_HYPNOTIC_PATTERN );
    act( "&+mA &+bbe&+mau&+bt&+mifu&+bl pa&+mtter&+bn app&+mears.", ch, NULL, NULL, TO_ALL );

    create_event( EVENT_STARSHELL_END, (ch->level * 4), ch->in_room, NULL, 0 );
    return;
}

void spell_holy_sacrifice( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_holy_sacrifice");

    if (victim != ch) return;
    if ( has_affect( victim, 0, sn, 0 ) ) {
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level/5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = 0;
    set_bitvector( &af, AFF_HOLY_SACRIFICE);
    affect_to_char( victim, &af );

    send_to_char( "&+WA glimmering aura of holy energy wraps about you.&n\n\r", ch );
}

void spell_battle_ecstasy( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_battle_ecstasy");

    if (victim != ch) return;
    if ( has_affect( victim, 0, sn, 0 ) ) {
       return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = level/5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = 0;
    set_bitvector( &af, AFF_BATTLE_ECSTASY);
    affect_to_char( victim, &af );

    send_to_char( "&+LYour pulse quickens as you are overcome with battle ecstasy.&n\n\r", ch);
}

void spell_mermaids_kiss( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;

    for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if( gch->deleted || !is_same_group( gch, ch) )
            continue;
        if( ch->fly_level != gch->fly_level )
            continue;
        if( !IS_NPC( gch ) )
        {
            if( gch->pcdata->condition[COND_FULL] < 1 )
                gch->pcdata->condition[COND_FULL] = 1;
            if( gch->pcdata->condition[COND_THIRST] < level / 2 )
                gch->pcdata->condition[COND_THIRST] = level / 2;  
        }
        spell_adaptation( spl_adaptation, level, ch, (void *) gch );
    }

    return;
}

void spell_thirst( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    last_fun( "spell_thirst");

    if ( !saves_spell( level, victim, DAM_WATER ) )
    {
        victim->move = (victim->move * 2 ) / 3;
        if( !IS_NPC( victim ) )
        {
            victim->pcdata->condition[COND_THIRST] = UMAX( 0, 
		victim->pcdata->condition[COND_THIRST] - level/2 );  
        }
        if ( !has_affect( victim, 0, sn, 0 ) )
        {
            af.skill     = 0;
            af.spell     = sn;
            af.song      = 0;
            af.duration  = UMAX( 1, level / 2 - 7 );
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            set_bitvector( &af, AFF_NONE );
            affect_join( victim, &af );
        }
        spell_damage( ch, victim, level / 10, sn, DAM_WATER );
    }
    else
    {
        send_to_char( "&+cYo&+Bu fee&+cl mo&+Bre &+Cth&+cir&+Bst&+cy.\n\r", victim );
        victim->move = (victim->move * 5 ) / 6;
        if( !IS_NPC( victim ) )
            victim->pcdata->condition[COND_THIRST]--;
    }

    return;
}

/*
 * At 100% and 80k hps, does 600 minimum, 900 maximum
 *
 */
int dragon_breath_damage( CHAR_DATA *ch, int percent ) {
    int dam;
    if (percent < 10) percent = 10;
    if (percent > 100 ) percent = 100;
    dam = ch->hit/267 + number_range(ch->hit/267, ch->hit/133);
    dam = dam * percent / 100;
//    if ( dam > 900 ) dam = 900;
    sprintf( lbuf, "%s&n breathing for %d base damage", ch->short_descr, dam );
    log_string( lbuf );

    return dam;
}

/* These last spells might all have bad target types: please check const.c
before you code them. Thanx. - Lohrr */

void spell_stornogs_spheres( int sn, int level, CHAR_DATA *ch, void *vo )
{

    last_fun( "spell_stornogs_spheres");

    send_to_char( "Under construction.\n\r", ch );
}

void spell_elemental_form( int sn, int level, CHAR_DATA *ch, void *vo )
{

    last_fun( "spell_elemental_form");

    send_to_char( "Under construction.\n\r", ch );
}

void spell_earthen_tomb( int sn, int level, CHAR_DATA *ch, void *vo )
{

    last_fun( "spell_earthen_tomb");

    send_to_char( "Under construction.\n\r", ch );
}

void spell_telekinesis( int sn, int level, CHAR_DATA *ch, void *vo )
{

    last_fun( "spell_telekinesis");

    send_to_char( "Under construction.\n\r", ch );
}


