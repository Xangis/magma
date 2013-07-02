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

// This file created by Veygoth for use with bard songs

void song_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    send_to_char( "You sing a song of protection.\n\r", ch );

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {

      if ( victim->deleted || has_affect( victim, 0, spl_armor, 0 ))
          continue;

      af = create_affect( 0, 0, sn, 4, APPLY_AC, (0 - (10 + (level / 5))), AFF_NONE );
      affect_join( victim, &af );

      send_to_char( "You feel someone protecting you.\n\r", victim );
    }

    return;
}

void song_flight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( is_affected( victim, AFF_FLYING ) )
        return;

      af = create_affect( 0, 0, sn, (level / 6), APPLY_NONE, 0, AFF_FLYING );
      affect_to_char( victim, &af );

      send_to_char( "&+WYour feet rise off the ground.&n\n\r", victim );
      act( "$n&n rises off the ground.", victim, NULL, NULL, TO_ROOM );
    }
    return;
}

void song_babble( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_calming( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if( victim->deleted )
        continue;
      stop_fighting( victim, FALSE );
      WAIT_STATE( victim, 2 );
    }
    return;
}

void song_chaos( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_clumsiness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_corruption( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_cowardice( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( !is_same_group( victim, ch ) )
      {
      af = create_affect( 0, 0, sn, (level / 6 + 1), APPLY_HITROLL, (0 - (level / 3)), AFF_NONE );
      affect_to_char( victim, &af );

      act( "$n&n looks unsure of $mself.", victim, NULL, NULL, TO_ROOM );
      send_to_char( "You feel less confident about your battle skills.\n\r", victim );
      }
    }
    return;
}

void song_dragons( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_feasting( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    int amount;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
       if( victim->deleted || IS_NPC(victim))
           continue;
  
       amount = level / 7 + 1;

       if( victim->pcdata->condition[COND_FULL] < 48 )
           victim->pcdata->condition[COND_FULL] += amount;
       if( victim->pcdata->condition[COND_THIRST] < 48 )
           victim->pcdata->condition[COND_THIRST] += amount; 

       send_to_char( "You stomach feels fuller.\n\r", victim );
    }
    return;
}

void song_forgetfulness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_harming( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_healing( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    int heal;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if( victim->deleted || !is_same_group( ch, victim ) )
          continue;

        heal = dice( 4, (level / 3)) + 1;

        if( victim->hit < get_max_hit( victim ) )
          victim->hit = UMIN( victim->hit + heal, get_max_hit( victim ) );
        update_pos( victim );

        send_to_char( "You feel better!\n\r", victim );
     }
    return;
}

void song_heroism( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( victim->deleted )
        continue;

      af = create_affect( 0, 0, sn, (level / 8), APPLY_HITROLL, (level / 6 + 1), AFF_NONE );
      affect_to_char( victim, &af );
      af = create_affect( 0, 0, sn, (level / 8), APPLY_DAMROLL, (level / 11 + 1), AFF_NONE );
      affect_to_char( victim, &af );

      act( "$n&n looks more courageous.", victim, NULL, NULL, TO_ROOM );;
      send_to_char( "You feel righteous.\n\r", victim );
    }
    return;
}

void song_invisibility( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;
    OBJ_DATA *obj;
    int total = 0;
    int max;

    max = level / 8;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( !is_same_group( victim, ch ) || is_affected( victim, AFF_INVISIBLE ) )
            continue;

        if( total >= max )
            return;

        send_to_char( "You slowly fade out of existence.\n\r", victim );
        act( "$n&n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM );

        af = create_affect( 0, 0, sn, (level / 6), APPLY_NONE, 0, AFF_INVISIBLE );
        affect_to_char( victim, &af );
        total++;
    }
    for( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
        if( obj->deleted )
          continue;
        if( IS_OBJ_STAT( obj, ITEM_INVIS ))
          continue;

        if( total >= max )
          return;

        act( "&+L$p&+L turns invisible.", ch, obj, NULL, TO_ROOM );
        act( "&+L$p&+L turns invisible.", ch, obj, NULL, TO_CHAR );
        SET_OBJ_STAT( obj, ITEM_INVIS );
        total++;
    }
    return;
}

void song_idiocy( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( has_affect( victim, 0, sn, 0 )
        || saves_spell( level, victim, DAM_BLACK_MANA ) )
           {
              send_to_char( "You failed!\n\r", ch );
              continue;
           };

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = level / 9;
      af.location  = APPLY_INT;
      af.modifier  = 0 - ( level + 15 );
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_INT;
      af.modifier  = 0 - ( level + 5 );
      affect_to_char( victim, &af );

      act( "A dumb look crosses $n&n's face.", victim, NULL, NULL, TO_ROOM );
      send_to_char( "You feel _REALLY_ dumb.\n\r", victim );
    }
    return;
}

void song_nightmares( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if( victim->deleted )
          continue;

      if ( is_affected( victim, AFF_FEAR ) || saves_spell( level, victim,
          DAM_BLACK_MANA ) )
      {
        send_to_char( "You have failed.\n\r", ch );
        send_to_char( "You resist the urge to panic.\n\r", ch );
        continue;
      }

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = 1 + (level / 7);
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
    }
    return;
}

void song_obscurement( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if( is_affected( victim, AFF_MINOR_INVIS ) || is_affected( victim, AFF_INVISIBLE ))
        return;

      act( "$n&n fades out of existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You vanish.\n\r", ch );

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = level / 6;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_MINOR_INVIS);
      affect_to_char( victim, &af );
    }
    return;
}

void song_purity( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_quagmire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
       if( victim->deleted )
         continue;

       if( victim->move < 0 )
         continue;

       victim->move -= dice( 2, (level / 2)) + 5;

       if( victim->move < 0 )
         victim->move = 0;

       send_to_char( "Your feet feel mired to the ground.\n\r", victim );
    }
    return;
}

void song_revelation( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if( victim->deleted )
          continue;

        affect_strip ( victim, 0, spl_invis, 0             );
        affect_strip ( victim, 0, spl_mass_invis, 0        );
        affect_strip ( victim, gsn_shadow, 0, 0            );
        REMOVE_AFF_BIT( victim, AFF_HIDE        );
        REMOVE_AFF_BIT( victim, AFF_INVISIBLE   );
        act( "$n&n is revealed!", victim, NULL, NULL, TO_ROOM );
        send_to_char( "You are revealed!\n\r", victim );
    }
    return;
}

void song_skylark( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    }
    return;
}

void song_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA   *victim;
    AFFECT_DATA  af;

    for( victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room )
    {
        if ( is_affected( victim, AFF_SLEEP )
            || saves_spell( level, victim, DAM_CHARM )
            || victim->race == RACE_VAMPIRE 
	    || is_same_group( ch, victim )
	    || victim->deleted )
        {
            continue;
        }

        af = create_affect( 0, 0, sn, level/8, APPLY_NONE, 0, AFF_SLEEP );
        affect_join( victim, &af );

        if ( IS_AWAKE( victim ) )
        {
            send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", victim );
            if ( ch->fighting || victim->position == POS_FIGHTING )
                stop_fighting( victim, FALSE );
            do_sleep( victim, "" );
        }

    }

    return;
}

void song_slowness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if (saves_spell( level, victim, DAM_OTHER ) )
      {
        send_to_char( "You failed!\n\r", ch );
        continue;
      }

      // Removes haste, takes two castings to make a hasted person slowed
      if( has_affect( victim, 0, spl_haste, 0 ) || is_affected( victim, AFF_HASTE ))
      {
        affect_strip( victim, 0, spl_haste, 0 );
        REMOVE_AFF_BIT( victim, AFF_HASTE );
        send_to_char( "You slow to your normal speed.\n\r", victim );
        continue;
      }

      if ( is_affected( victim, AFF_SLOWNESS ))
        continue;

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = 6;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      set_bitvector( &af, AFF_SLOWNESS);
      affect_to_char( victim, &af );

      act( "&+R$n&+R moves much more slowly.&n", victim, NULL, NULL, TO_ROOM );
      send_to_char( "&+RYou feel yourself slowing down.&n\n\r", victim );
  }
  return;
}

void song_susceptibility( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if( victim->deleted )
        continue;

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = level / 7;
      af.modifier  = (level / 5) + 1;
      af.location  = APPLY_SAVING_SPELL;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_PARA;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_PETRI;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_ROD;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_BREATH;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );
    }
    return;
}

void song_warding( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if( victim->deleted )
        continue;

      af.skill     = 0;
      af.spell     = sn;
      af.duration  = level / 7;
      af.modifier  = 0 - ((level / 5) + 1);
      af.location  = APPLY_SAVING_SPELL;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_PARA;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_PETRI;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_ROD;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      af.location  = APPLY_SAVING_BREATH;
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );
     }
    return;
}

void song_weakling( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim;
    AFFECT_DATA af;

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
    if ( has_affect( victim, 0, sn, 0 )
      || saves_spell( level, victim, DAM_BLACK_MANA ) )
        continue;

      af.skill     = 0;
      af.spell     = 0;
      af.song      = sn;
      af.duration  = level / 7;
      af.location  = APPLY_STR;
      af.modifier  = -(level / 2);
      set_bitvector( &af, AFF_NONE);
      affect_to_char( victim, &af );

      if( level > 25 )
      {
        af.location = APPLY_DAMROLL;
        af.modifier = 0 - ( level / 7 );
        affect_to_char( victim, &af );
      }

      act( "$n&n looks weaker.", victim, NULL, NULL, TO_ROOM );
      send_to_char( "You feel weaker.\n\r", victim );
    }
    return;
}

void do_songs ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH * 3 ];
    char buf1 [ MAX_STRING_LENGTH * 3 ];
    int  sn;
    int  level;
    bool pSong;

    if ( IS_NPC( ch ) || ch->class != CLASS_BARD )
    {
       send_to_char ( "&nYou do not need any stinking songs!\n\r", ch );
       return;
    }

    buf1[0] = '\0';

    strcat ( buf1, "&n&+rALL songs available for your class.&n\n\r" );
    strcat ( buf1, "&+RLv      Song&n\n\r" );

    for ( level = 1; level < 56; level++ )
    {
      pSong = TRUE;

      for ( sn = 0; sn < MAX_SONG; sn++ )
      {
        if ( !songs_table[sn].name )
          break;
        if ( songs_table[sn].spell_circle[ch->class] != level )
          continue;

        if ( pSong )
        {
          sprintf ( buf, "&+Y%2d&+y:&n", level );
          strcat ( buf1, buf );
          pSong = FALSE;
        }
        else
          strcat( buf1, "   " );

        /* format fix by Koala */
        strcat ( buf1, "     " );

        sprintf ( buf, "&n&+c%-21s  &+Y%d&n", songs_table[sn].name,
           ch->pcdata->sng_lrn[sn] );
        strcat ( buf1, buf );

        strcat ( buf1, "\n\r" );

      }

    }

    send_to_char( buf1, ch );
    return;

}
