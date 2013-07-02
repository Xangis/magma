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
 *                                                                         *
 ***************************************************************************/

// This file will contain all functions related
// to ships and other mobile vehicles
// - Veygoth

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

/*
Trap code

Trap functions are actually spell functions, but with the prefix
"trap" before them.  They are always objects nailing the character
with nasty stuff, but they are exactly like spell functions in every
other respect.

-- Veygoth
*/

void setoff_trap( CHAR_DATA *ch, OBJ_DATA *obj )
{
   char buf [ MAX_STRING_LENGTH ];

   if( !obj || !obj->trap || !ch )
   {
     bug( "Setoff_trap called with no trap on object, no object, or no ch.", 0 );
     return;
   }

   if( number_percent() < obj->trap->percent )
     send_to_char( "You set off a trap!\n\r", ch );
   else
     return;

   if( obj->trap->charges > -1 )
       obj->trap->charges--;

   sprintf( buf, "%s set off trap %s in room %d.", ch->name, obj->name,
       obj->in_room->vnum );
   log_string( buf );

   switch( obj->trap->damage )
   {
     default:
       send_to_char( "Lucky for you the trap malfunctioned!\n\r", ch );
     case TRAP_SLEEP:
       trap_sleep( trp_sleep, obj->trap->level, ch, obj );
       break;
     case TRAP_TELEPORT:
       trap_teleport( trp_teleport, obj->trap->level, ch, obj );
       break;
     case TRAP_FIRE:
       trap_fire( trp_fire, obj->trap->level, ch, obj );
       break;
     case TRAP_COLD:
       trap_cold( trp_cold, obj->trap->level, ch, obj );
       break;
     case TRAP_ACID:
       trap_acid( trp_acid, obj->trap->level, ch, obj );
       break;
     case TRAP_ENERGY:
       trap_energy( trp_energy, obj->trap->level, ch, obj );
       break;
     case TRAP_BLUNT:
       trap_bash( trp_bash, obj->trap->level, ch, obj );
       break;
     case TRAP_PIERCING:
       trap_pierce( trp_pierce, obj->trap->level, ch, obj );
       break;
     case TRAP_SLASHING:
       trap_slash( trp_slash, obj->trap->level, ch, obj );
       break;
     case TRAP_DISPEL:
       trap_dispel( trp_dispel, obj->trap->level, ch, obj );
       break;
     case TRAP_GATE:
       trap_gate( trp_gate, obj->trap->level, ch, obj );
       break;
     case TRAP_SUMMON:
       trap_summon( trp_summon, obj->trap->level, ch, obj );
       break;
     case TRAP_WITHER:
       trap_wither( trp_wither, obj->trap->level, ch, obj );
       break;
     case TRAP_HARM:
       trap_harm( trp_harm, obj->trap->level, ch, obj );
       break;
     case TRAP_POISON:
       trap_poison( trp_poison, obj->trap->level, ch, obj );
       break;
     case TRAP_PARALYSIS:
       trap_para( trp_para, obj->trap->level, ch, obj );
       break;
     case TRAP_STUN:
       trap_stun( trp_stun, obj->trap->level, ch, obj );
       break;
     case TRAP_DISEASE:
       trap_disease( trp_disease, obj->trap->level, ch, obj );
       break;
   }

   return;
}

void trap_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *)vo;
    AFFECT_DATA af;

    if ( is_affected( ch, AFF_SLEEP )
        || level < ch->level
        || saves_spell( level, ch, DAM_CHARM )
        || ch->race == RACE_VAMPIRE )
    {
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.duration  = 4 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_SLEEP);
    affect_join( ch, &af );

    if ( IS_AWAKE( ch ) )
    {
        act( "$p&n hums lightly and you feel very sleepy ... zzzzzz.", ch, obj, NULL, TO_CHAR );
        if ( ch->position == POS_FIGHTING )
           stop_fighting( ch, TRUE );
        do_sleep( ch, "" );
    }

    return;
}

void trap_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_FIRE ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_FIRE );

    return;
}

void trap_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_HARM ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_HARM );

    return;
}

void trap_energy( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_ENERGY ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_ENERGY );

    return;
}

void trap_cold( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_COLD ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_COLD );

    return;
}

void trap_acid( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_ACID ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_ACID );

    return;
}

void trap_bash( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_BASH ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_BASH );

    return;
}

void trap_slash( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_SLASH ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_SLASH );

    return;
}

void trap_pierce( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int        dam;

    dam = dice( 6, level ) + 15;

    if ( saves_spell( level, ch, DAM_PIERCE ) )
        dam /= 2;

    spell_damage( ch, ch, dam, sn, DAM_PIERCE );

    return;
}

void trap_disease( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;

    if ( saves_spell( level, ch, DAM_DISEASE ) )
    {
        send_to_char( "You feel momentarily ill, but it passes.\n\r", ch );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.level     = level * 3 / 4;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -( level / 3 );
    set_bitvector( &af, AFF_PLAGUE);
    affect_join( ch, &af );

    send_to_char( "You scream in agony as plague sores erupt from your skin.\n\r", ch );
    act( "$n&n screams in agony as plague sores erupt from $s skin.", ch, NULL, NULL, TO_ROOM );
}

void trap_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;

    if ( saves_spell( level, ch, DAM_POISON ) )
    {
        send_to_char( "Luckily you managed to avoid the poison.\n\r", ch );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    set_bitvector( &af, AFF_POISON);
    affect_join( ch, &af );

    send_to_char( "You feel very sick.\n\r", ch );
    return;
}

void trap_para( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;

    if ( is_affected( ch, AFF_MINOR_PARA )
        || saves_spell( level, ch, DAM_CHARM ))
    {
        send_to_char( "You avoided the trap!\n\r", ch );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.duration  = level / 8;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    set_bitvector( &af, AFF_MINOR_PARA );
    affect_join( ch, &af );

    if ( IS_AWAKE( ch ) )
    {
        send_to_char( "&+YYou are paralyzed!&n\n\r", ch );
        if ( ch->position == POS_FIGHTING )
           stop_fighting( ch, TRUE );
    }

    return;
}

void trap_dispel( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *)vo;
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf; paf = paf->next )
    {
       // Doesen't remove the icky stuff
       if ( paf->deleted )
           continue;
       if ( paf->spell == spl_polymorph )
           continue;
       if ( paf->spell == gsn_vampiric_bite )
           continue;
       if ( paf->spell == spl_plague )
           continue;
       if ( paf->spell == spl_poison )
           continue;
       if ( paf->spell == spl_curse )
           continue;
       if ( paf->spell == spl_blindness )
           continue;
       affect_remove( ch, paf );

       act( "$p&n has dispelled your magic!", ch, obj, NULL, TO_CHAR );
       act( "$p&n has removed all magic effects from $n&n.", ch, obj, NULL, TO_ROOM );
    }

    return;
}
   
void trap_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *)vo;
    ROOM_INDEX_DATA *pRoomIndex;

    last_fun( "trap_teleport");
    if ( !ch->in_room || IS_ROOM( ch->in_room, ROOM_NO_TELEPORT ))
    {
        act( "$p&n failed to teleport you.", ch, obj, NULL, TO_CHAR );
        return;
    }

    for ( ; ; )
    {
        pRoomIndex = get_room_index( number_range( 0, top_vnum_room ) );
        if ( pRoomIndex )
            if (   !IS_ROOM( pRoomIndex, ROOM_PRIVATE  )
                && !IS_ROOM( pRoomIndex, ROOM_SOLITARY )
                && !IS_ROOM( pRoomIndex, ROOM_NO_TELEPORT ))
            break;
    }

    if ( ch->fighting )
        stop_fighting( ch, TRUE );
    act( "$n&n slowly fades out of existence.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, pRoomIndex );
    act( "$n&n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    return;
}

void trap_wither( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *)vo;
    AFFECT_DATA af;

    if ( has_affect( ch, 0, sn, 0 ) || saves_spell( (level + 5), ch, DAM_BLACK_MANA ) )
    {
        send_to_char( "You avoided the trap!\n\r", ch );
        return;
    }

    af.skill     = 0;
    af.spell     = sn;
    af.duration  = level / 2;
    af.location  = APPLY_STR;
    af.modifier  = 0 - (level / 2 + 10);
    set_bitvector( &af, AFF_NONE);
    affect_to_char( ch, &af );

    af.location  = APPLY_CON;
    af.modifier  = 0 - (level / 3 + 2);
    affect_to_char( ch, &af );

    af.location  = APPLY_AGI;
    af.modifier  = 0 - (level / 4 + 3);
    affect_to_char( ch, &af );

    af.location  = APPLY_DEX;
    af.modifier  = 0 - (level / 5 + 3);
    affect_to_char( ch, &af );

    af.location  = APPLY_CHA;
    af.modifier  = 0 - (level / 3 + 5);
    affect_to_char( ch, &af );

    af.location  = APPLY_INT;
    af.modifier  = 0 - (level / 5);
    affect_to_char( ch, &af );

    af.location  = APPLY_WIS;
    af.modifier  = 0 - (level / 7);
    affect_to_char( ch, &af );

    act( "$N&+L has been withered by $p&+L!&n", ch, obj, NULL, TO_ROOM );
    send_to_char( "&+LYou feel _very_ weak and powerless.&n\n\r", ch );
    return;
}

void trap_stun( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *)vo;

    act( "$n&n is knocked to the ground, stunned!", ch, NULL, NULL, TO_ROOM );
    act( "You see &+Ystars&n as you are knocked to the ground by the stunning force of $p&n!", ch, obj, NULL, TO_CHAR );

    ch->hit -= level;
    update_pos( ch );

    if( ch->position > POS_STUNNED )
        ch->position = POS_STUNNED;

    WAIT_STATE( ch, ( level / 2 + 1 ));

    return;
}

void trap_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "This type of trap is not enabled yet.  Lucky!\n\r", ch );
    return;
}

void trap_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "This type of trap is not enabled yet.  Lucky!\n\r", ch );
    return;
}


