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

char lbuf[MAX_STRING_LENGTH * 2];

bool spellup( CHAR_DATA *ch );
bool spellup_others( CHAR_DATA *ch );
bool heal_self( CHAR_DATA *ch );
bool check_vigorize( CHAR_DATA *ch );
bool offensive_spell( CHAR_DATA *ch, CHAR_DATA *victim );
bool area_offensive( CHAR_DATA *ch, CHAR_DATA *victim );
bool check_offensive( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int percent );
bool check_defensive( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int percent );
bool check_spellup( CHAR_DATA *ch, int sn, int percent );
bool shout_and_hunt( CHAR_DATA *ch, CHAR_DATA *victim, char *msg, int *helpers);
void move_xorn( CHAR_DATA *ch, ROOM_INDEX_DATA *room, int type );
CHAR_DATA *transform_mob( CHAR_DATA **pch, int vnum, char *msg );

#define XORN_MOVE_PHASE 0
#define XORN_MOVE_EARTH 1

/*
 * The following special functions are available for mobiles.
 */
DECLARE_MOB_FUN(        spec_breath_any         );
DECLARE_MOB_FUN(        spec_breath_acid        );
DECLARE_MOB_FUN(        spec_breath_fire        );
DECLARE_MOB_FUN(        spec_breath_frost       );
DECLARE_MOB_FUN(        spec_breath_gas         );
DECLARE_MOB_FUN(        spec_breath_water       );
DECLARE_MOB_FUN(        spec_breath_lightning   );
DECLARE_MOB_FUN(        spec_cast_adept         );
DECLARE_MOB_FUN(        spec_cast_cleric        );
DECLARE_MOB_FUN(        spec_cast_druid         );
DECLARE_MOB_FUN(        spec_cast_shaman        );
DECLARE_MOB_FUN(        spec_cast_ghost         );
DECLARE_MOB_FUN(        spec_cast_judge         );
DECLARE_MOB_FUN(        spec_cast_conjurer      );
DECLARE_MOB_FUN(        spec_cast_necromancer   );
DECLARE_MOB_FUN(        spec_cast_sorcerer      );
DECLARE_MOB_FUN(        spec_cast_psionicist    );
DECLARE_MOB_FUN(        spec_cast_vampire       );
DECLARE_MOB_FUN(        spec_cast_undead        );
DECLARE_MOB_FUN(        spec_cast_illusionist   );
DECLARE_MOB_FUN(        spec_cast_earth_ele     );
DECLARE_MOB_FUN(        spec_cast_air_ele       );
DECLARE_MOB_FUN(        spec_cast_fire_ele      );
DECLARE_MOB_FUN(        spec_cast_water_ele     );
DECLARE_MOB_FUN(        spec_cast_ranger        );
DECLARE_MOB_FUN(        spec_cast_antipaladin   );
DECLARE_MOB_FUN(        spec_cast_paladin       );
DECLARE_MOB_FUN(        spec_cast_hunter        );
DECLARE_MOB_FUN(        spec_warrior            );
DECLARE_MOB_FUN(        spec_earth_elemental    );
DECLARE_MOB_FUN(        spec_air_elemental      );
DECLARE_MOB_FUN(        spec_fire_elemental     );
DECLARE_MOB_FUN(        spec_water_elemental    );
DECLARE_MOB_FUN(        spec_monk               );
DECLARE_MOB_FUN(        spec_executioner        );
DECLARE_MOB_FUN(        spec_fido               );
DECLARE_MOB_FUN(        spec_guard              );
DECLARE_MOB_FUN(        spec_janitor            );
DECLARE_MOB_FUN(        spec_mayor              );
DECLARE_MOB_FUN(        spec_poison             );
DECLARE_MOB_FUN(        spec_repairman          );
DECLARE_MOB_FUN(        spec_thief              );
DECLARE_MOB_FUN(        spec_assassin           );
DECLARE_MOB_FUN(        spec_mercenary          );
DECLARE_MOB_FUN(        spec_bard               );
DECLARE_MOB_FUN(        spec_justice_guard      );
DECLARE_MOB_FUN(        spec_justice_tracker    );
DECLARE_MOB_FUN(        spec_lost_girl          );
DECLARE_MOB_FUN(        spec_grumbar_shout      );
DECLARE_MOB_FUN(        death_proc_generic      );
DECLARE_MOB_FUN(        spec_xorn               );
DECLARE_MOB_FUN(        spec_lil_critter        );


MOB_FUN *spec_mob_lookup( const char *name )
{
    int cmd;

    last_fun( "spec_mob_lookup" );

    for ( cmd = 0; *spec_mob_table[cmd].spec_name; cmd++ )
	if ( !str_cmp( name, spec_mob_table[cmd].spec_name ) )
	    return spec_mob_table[cmd].spec_fun;

    return NULL;
}

// Translate Envy specs into Magma specs.
MOB_FUN *envy_spec_mob_convert( const char *name )
{
    if( !str_cmp( name, "spec_cast_mage" ))
    {
        return spec_mob_lookup( "spec_cast_sorcerer" );
    }

    return NULL;
}

char *spec_mob_string( MOB_FUN *fun )
{
    int cmd;

    last_fun( "spec_mob_string" );

    for ( cmd = 0; *spec_mob_table[cmd].spec_fun; cmd++ )
	if ( fun == spec_mob_table[cmd].spec_fun )
	    return spec_mob_table[cmd].spec_name;

    return 0;
}

/*
 * Special function commands.
 */
const   struct  spec_mob_type   spec_mob_table  [ ]     =
{
    { "spec_breath_any",        spec_breath_any         },
    { "spec_breath_acid",       spec_breath_acid        },
    { "spec_breath_fire",       spec_breath_fire        },
    { "spec_breath_frost",      spec_breath_frost       },
    { "spec_breath_gas",        spec_breath_gas         },
    { "spec_breath_water",      spec_breath_water       },
    { "spec_breath_lightning",  spec_breath_lightning   },
    { "spec_cast_adept",        spec_cast_adept         },
    { "spec_cast_cleric",       spec_cast_cleric        },
    { "spec_cast_druid",        spec_cast_druid         },
    { "spec_cast_shaman",       spec_cast_shaman        },
    { "spec_cast_ghost",        spec_cast_ghost         },
    { "spec_cast_judge",        spec_cast_judge         },
    { "spec_cast_air_ele",      spec_cast_air_ele       },
    { "spec_cast_earth_ele",    spec_cast_earth_ele     },
    { "spec_cast_fire_ele",     spec_cast_fire_ele      },
    { "spec_cast_water_ele",    spec_cast_water_ele     },
    { "spec_cast_ranger",       spec_cast_ranger        },
    { "spec_cast_hunter",       spec_cast_hunter        },
    { "spec_cast_paladin",      spec_cast_paladin       },
    { "spec_cast_antipaladin",  spec_cast_antipaladin   },
    { "spec_cast_illusionist",  spec_cast_illusionist   },
    { "spec_cast_conjurer",     spec_cast_conjurer      },
    { "spec_cast_necromancer",  spec_cast_necromancer   },
    { "spec_cast_sorcerer",     spec_cast_sorcerer      },
    { "spec_cast_psionicist",   spec_cast_psionicist    },
    { "spec_cast_vampire",      spec_cast_vampire       },
    { "spec_cast_undead",       spec_cast_undead        },
    { "spec_executioner",       spec_executioner        },
    { "spec_fido",              spec_fido               },
    { "spec_guard",             spec_guard              },
    { "spec_janitor",           spec_janitor            },
    { "spec_mayor",             spec_mayor              },
    { "spec_poison",            spec_poison             },
    { "spec_repairman",         spec_repairman          },
    { "spec_thief",             spec_thief              },
    { "spec_assassin",          spec_assassin           },
    { "spec_mercenary",         spec_mercenary          },
    { "spec_warrior",           spec_warrior            },
    { "spec_earth_elemental",   spec_earth_elemental    },
    { "spec_air_elemental",     spec_air_elemental      },
    { "spec_fire_elemental",    spec_fire_elemental     },
    { "spec_water_elemental",   spec_water_elemental    },
    { "spec_monk",              spec_monk               },
    { "spec_bard",              spec_bard               },
    { "spec_justice_guard",     spec_justice_guard      },
    { "spec_justice_tracker",   spec_justice_tracker    },
    { "spec_lost_girl",         spec_lost_girl          },
    { "spec_grumbar_shout",     spec_grumbar_shout      },
    { "spec_xorn",              spec_xorn               },
    { "spec_lil_critter",       spec_lil_critter        },
    { "",                       0                       }
};  

/*
 * If a spell casting mob is hating someone... try and summon them. -Toric (?)
 *
 * Veygoth - Need to add code to also gate to the person if they can't be summoned
 */
void summon_if_hating( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char       name [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    bool       found;

    last_fun( "summon_if_hating");
    if ( ch->fighting
	|| ch->fearing
	|| !ch->hating
	|| IS_ROOM( ch->in_room, ROOM_SAFE ) )
	return;

    /* If player is close enough to hunt... don't summon. */
    if ( ch->hunting )
	return;

    one_argument( ch->hating->name, name );

    found = FALSE;

    /* Make sure the char exists; works even if player quits. */
    for ( victim = char_list; victim; victim = victim->next )
    {
	if ( !str_cmp( ch->hating->name, victim->name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return;

    if ( ch->in_room == victim->in_room )
	return;

    if( (ch->level * 5 - 4) >= spells_table[spl_vortex_lift].spell_circle[ch->class] )
    {
      if ( !IS_NPC( victim ) )
	sprintf( buf, "relocate 0.%s", name );
      else
	sprintf( buf, "relocate %s",   name );
    }
    else if( (ch->level * 5 - 4) >= spells_table[spl_summon].spell_circle[ch->class] )
    {
      if ( !IS_NPC( victim ) )
	sprintf( buf, "summon 0.%s", name );
      else
	sprintf( buf, "summon %s",   name );
    }
    else if( (ch->level * 5 - 4) >= spells_table[spl_spirit_jump].spell_circle[ch->class] )
    {
      if ( !IS_NPC( victim ) )
	sprintf( buf, "spirit jump 0.%s", name );
      else
	sprintf( buf, "spirit jump %s",   name );
    }

    do_cast( ch, buf );
    return;
}

/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, int sn )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;

    last_fun( "dragon");

    if ( !(victim = ch->fighting) )
	return FALSE;

    // use dragon roar 1/2 half the time
    if ( number_percent() > 50 ) {
        send_to_char( "You roar like the enraged beast you are.\n\r", ch );
        act( "$n&n &+WROARS&n, sending you into a panic!",
            ch, NULL, NULL, TO_ROOM);
        for ( victim = ch->in_room->people; victim; victim = victim_next) {
            victim_next = victim->next_in_room;
            if ( victim == ch || victim->deleted ) continue;
            if ( victim->fly_level != ch->fly_level ) continue;
            if ( IS_NPC( victim ) ) continue;
            if ( saves_breath( ch->level, victim, DAM_SOUND ) ) {
                if ( saves_breath( ch->level, victim, DAM_SOUND ) ) {
                    send_to_char("You control the urge to flee and stand your ground.\n\r", victim );
                } else {
                    send_to_char("You are bowled over by the powerful blast!\n\r", victim);
                    act("$N&n is floored by $n&n's roar.", ch, NULL, victim, TO_NOTVICT);
                    victim->position = POS_SITTING;
                }
            } else {
                send_to_char("Run away! Run away!\n\r", victim);
                act("$n&n attempts to flee from the room!", victim, NULL, NULL, TO_ROOM);
                do_flee( victim, NULL );
            }
        } //end for
        return TRUE;
    }
    send_to_char( "You feel the urge to &+Gburp&n!\n\r", ch );
    act( "$n&n breathes!", ch, NULL, NULL, TO_ROOM );

    if( sn > 0 && sn < MAX_SPELL )
    {
        sprintf(lbuf,"Dragon (%s) breathing %s.", ch->short_descr,
            spells_table[sn].name );
        wiznet( 0, WIZ_LSPAM, 0, lbuf);
    }

    if( sn > 0 && sn < MAX_SPELL )
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

	/* NPC's do not hit NPC's! (charmies?) */
	if ( IS_NPC( victim ) && IS_NPC( ch ) )
	    continue;

        (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    }

    return TRUE;
}

/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_any");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !ch->fighting )
	return FALSE;

    switch ( number_range( 0, 7 ))
    {
    case 0: return spec_breath_frost            ( ch, PROC_NORMAL );
    case 1: return spec_breath_water            ( ch, PROC_NORMAL );
    case 2: return spec_breath_lightning        ( ch, PROC_NORMAL );
    case 3: return spec_breath_gas              ( ch, PROC_NORMAL );
    case 4: return spec_breath_acid             ( ch, PROC_NORMAL );
    case 5:
    case 6:
    case 7: return spec_breath_fire             ( ch, PROC_NORMAL );
    }

    return FALSE;
}

bool spec_breath_acid( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_acid");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_acid_breath );
}

bool spec_breath_fire( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_fire");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_fire_breath );
}

bool spec_breath_frost( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_frost");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_frost_breath );
}

bool spec_breath_water( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_water");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_water_breath );
}

bool spec_breath_gas( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_gas" );
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_gas_breath );
}

bool spec_breath_lightning( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_breath_lightning");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    return dragon( ch, spl_lightning_breath );
}

bool spec_cast_adept( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_adept");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( !victim || IS_NPC( victim ) )
	return FALSE;

    if ( victim->level > 10 )
    {
	return FALSE;
    }

    switch ( number_bits( 3 ) )
    {
    case 0:
	act( "$n&n utters the word 'tehctah'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( spl_armor, ch->level, ch, victim );
	return TRUE;

    case 1:
	act( "$n&n utters the word 'nhak'.",    ch, NULL, NULL, TO_ROOM );
	spell_bless( spl_bless, ch->level, ch, victim );
	return TRUE;

    case 2:
	act( "$n&n utters the word 'yeruf'.",   ch, NULL, NULL, TO_ROOM );
	spell_cure_blindness( spl_cure_blind, ch->level, ch, victim );
	return TRUE;

    case 3:
	act( "$n&n utters the word 'garf'.",    ch, NULL, NULL, TO_ROOM );
	spell_cure_light( spl_cure_light, ch->level, ch, victim );
	return TRUE;

    case 4:
	act( "$n&n utters the words 'rozar'.",  ch, NULL, NULL, TO_ROOM );
	spell_remove_poison( spl_remove_poison, ch->level, ch, victim );
	return TRUE;

    case 5:
	act( "$n&n utters the words 'nadroj'.", ch, NULL, NULL, TO_ROOM );
	spell_vigorize_light( spl_stamina, ch->level, ch, victim );
	return TRUE;

    case 6:
	act( "$n&n utters the words 'suinoleht'.", ch, NULL, NULL, TO_ROOM );
	spell_combat_mind( spl_combat_mind, ch->level, ch, victim );
	return TRUE;
    }

    return FALSE;
}

bool spec_cast_cleric( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_cleric");
    if ( cmd == PROC_DEATH )
        return FALSE;
        

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( ch->hit < (get_max_hit( ch ) - 10) )
            if( heal_self( ch ))
              return TRUE;
          if( check_vigorize( ch ))
              return TRUE;
          if( spellup_others( ch ))
              return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_druid( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_druid");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
               return TRUE;
          if( ch->hit < (get_max_hit( ch ) - 10) )
            if( heal_self( ch ))
              return TRUE;
          if( check_vigorize( ch ))
              return TRUE;
          if( spellup_others( ch ))
              return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
	return FALSE;

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_shaman( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_shaman");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
               return TRUE;
          if( ch->hit < (get_max_hit( ch ) - 10) )
            if( heal_self( ch ))
              return TRUE;
          if( check_vigorize( ch ))
              return TRUE;
          if( spellup_others( ch ))
              return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_judge( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_cast_judge");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    spell = "high explosive";
    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_cast_conjurer( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_cast_conjurer");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
            if( spellup( ch ))
               return TRUE;
            if( spellup_others( ch ))
               return TRUE;
        }
	return FALSE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 5 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "color spray";   break;
	case  5: min_level = 12; spell = "sleep";     break;
	case  6: min_level = 13; spell = "energy drain";   break;
	case  7: min_level =  6; spell = "burning hands";  break;
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	case 12: min_level = 16; spell = "shocking grasp"; break;
	case 13: min_level = 26; spell = "polymorph other";break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_cast_necromancer( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_cast_necromancer");
    if ( cmd == PROC_DEATH )
        return FALSE;

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
            if( spellup( ch ))
               return TRUE;
            if( spellup_others( ch ))
               return TRUE;
        }
	return FALSE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 5 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "color spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "energy drain";   break;
	case  7:
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	case 12: min_level = 16; spell = "polymorph other";break;
	case 13: min_level = 16; spell = "polymorph other";break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_cast_sorcerer( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_sorcerer");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
               return TRUE;
          if( spellup_others( ch ))
               return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
	return FALSE;

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_air_ele( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_air_ele");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_earth_ele( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_earth_ele");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_fire_ele( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_fire_ele");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_water_ele( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_water_ele");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_ranger( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_ranger");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( ch->hit < (get_max_hit( ch ) - 10) )
            if( heal_self( ch ))
              return TRUE;
          if( spellup_others( ch ))
              return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_hunter( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_hunter");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_paladin( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_cast_paladin");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;


    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( ch->hit < (get_max_hit( ch ) - 10) )
            if( heal_self( ch ))
              return TRUE;
          if( check_vigorize( ch ))
              return TRUE;
          if( spellup_others( ch ))
              return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_antipaladin( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    last_fun( "spec_cast_antipaladin");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( check_vigorize( ch ))
              return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_cast_illusionist( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    last_fun( "spec_cast_illusionist");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
          if( spellup( ch ))
            return TRUE;
          if( spellup_others( ch ))
            return TRUE;
        }
	return FALSE;
    }

    victim = ch->fighting;

    if ( !victim )
      return FALSE;

    // Not sure whether being able to see the victim should be required...

    // Give a 1/3 chance of picking an area spell over a direct damage spell
    // later on, I plan to code it so that it makes an intelligence check
    // based on the number of enemies in the room. -- Veygoth
    if( number_range( 1, 3 ) == 3 )
    {
      if( area_offensive( ch, victim ))
        return TRUE;
      else if( offensive_spell( ch, victim ))
        return TRUE;
    }
    else
    {
      if( offensive_spell( ch, victim ))
        return TRUE;
      else if( area_offensive( ch, victim ))
        return TRUE;
    }

    return FALSE;
}

bool spec_assassin( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_assassin");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( ch->position != POS_STANDING )
    {
      if( ch->position == POS_FIGHTING && ch->fighting
        && ch->fighting->fighting == ch )
      {
        start_grudge(ch, ch->fighting, FALSE);
        do_flee( ch, "" );
        do_hide( ch, "" );
      }
      return FALSE;
    }

    if( ch->hating )
    {
      if( !ch->fighting && ch->position != POS_FIGHTING && ch->hating->who->in_room == ch->in_room )
        if( backstab( ch, ch->hating->who ))
          return TRUE;
    }

    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_warrior( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_warrior");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_fire_elemental( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_fire_elemental");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_earth_elemental( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_earth_elemental");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_air_elemental( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_air_elemental");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_water_elemental( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_water_elemental");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_monk( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_monk");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_mercenary( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_mercenary");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    if( ch->hating )
    {
      if( !ch->fighting && ch->position != POS_FIGHTING && ch->hating->who->in_room == ch->in_room )
        if( backstab( ch, ch->hating->who ))
          return TRUE;
    }

    return FALSE;
}

bool spec_bard( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_bard");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    return FALSE;
}

bool spec_cast_undead( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_undead");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 4 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
//	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	case  8:
	case  9:
	case 10: if ( ch->race == RACE_VAMPIRE )
		 {
		     min_level = 24;
		     spell = "vampiric bite";  break;
		 }
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_cast_vampire( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    CHAR_DATA *gch;

    if( !( victim = ch->fighting ))
      return FALSE;

    last_fun( "spec_cast_vampire");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if( victim && ch->position >= POS_FIGHTING )
    {
       // Allow a vampire to gaze in addition to its other abilities.
       if( number_percent() < 40 )
       {
         act( "$n&n turns $s gaze upon you.", ch, NULL, victim, TO_VICT );
         stop_fighting( ch, FALSE );
         for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
         {
            if( is_same_group( ch, gch ) && gch != ch )
            {
               act( "$n&n switches targets.", victim, NULL, NULL, TO_ROOM );
               set_fighting( ch, gch );
               break;
            }
         }
       }
    }

    return FALSE;
}

bool spec_executioner( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *crime;
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "spec_executionaer");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    crime = "pretty annoying person";
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted )
	    continue;

//      if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
//          { crime = "KILLER"; break; }

//      if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  ) )
//          { crime = "THIEF"; break; }
    }

    if ( !victim )
	return FALSE;

    sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
	    victim->name, crime );
    do_shout( ch, buf );
    kill_ch( ch, victim );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		 ch->in_room );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		 ch->in_room );
    return TRUE;
}

bool spec_fido( CHAR_DATA *ch, int cmd )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *corpse;
    OBJ_DATA *corpse_next;

    last_fun( "spec_fido");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !IS_AWAKE( ch ) )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse; corpse = corpse_next )
    {
	corpse_next = corpse->next_content;
	if ( corpse->deleted )
	    continue;
	if ( corpse->item_type != TYPE_CORPSE_NPC )
	    continue;

	act( "$n&n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
		continue;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}

// recoded so mobs will only assist own race, and will do so 90% of the time
// when function is checked -- Veygoth
bool spec_guard( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    CHAR_DATA *ech;

    last_fun( "spec_guard");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    ech      = NULL;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted )
	    continue;

        if ( IS_NPC(victim) && victim->race == ch->race )
            continue;

	if ( victim->fighting && (victim->fighting != ch) && (victim->fighting->race == ch->race) && (number_percent() < 90) )
	  {
	    ech = victim;
	    continue;
	  }
    }

    if ( ech )
    {
	act( "$n&n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
	    ch, NULL, NULL, TO_ROOM );
	kill_ch( ch, ech );
	return TRUE;
    }

    return FALSE;
}

bool spec_janitor( CHAR_DATA *ch, int cmd )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    last_fun( "spec_janitor");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !IS_AWAKE( ch ) )
	return FALSE;

    for ( trash = ch->in_room->contents; trash; trash = trash_next )
    {
	trash_next = trash->next_content;
	if ( trash->deleted )
	    continue;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if (   trash->item_type == TYPE_DRINK_CON
	    || trash->item_type == TYPE_TRASH
	    || trash->cost < 10 )
	{
	    act( "$n&n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}

bool spec_mayor( CHAR_DATA *ch, int cmd )
{
    static const char *path;
    static const char  open_path  [ ] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";
    static const char  close_path [ ] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";
    static       int   pos;
    static       bool  move;

    last_fun( "spec_mayor");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
	    path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting )
	return spec_cast_cleric( ch, PROC_NORMAL );
    if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0' );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act( "$n&n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act( "$n&n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'a':
	act( "$n&n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'b':
	act( "$n&n says 'What a view!  I must do something about that dump!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'c':
	act( "$n&n says 'Vandals!  Youngsters have no respect for anything!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'd':
	act( "$n&n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'e':
	act( "$n&n says 'I hereby declare the city of Midgaard open!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'E':
	act( "$n&n says 'I hereby declare the city of Midgaard closed!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'O':
	do_unlock( ch, "gate" );
	do_open  ( ch, "gate" );
	break;

    case 'C':
	do_close ( ch, "gate" );
	do_lock  ( ch, "gate" );
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}

bool spec_poison( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;

    last_fun( "spec_poison");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !ch->fighting
	|| !( victim = ch->fighting )
	|| number_percent( ) > 2 * ch->level )
	return FALSE;

    act( "You bite $N&n!",  ch, NULL, victim, TO_CHAR    );
    act( "$n&n bites you!", ch, NULL, victim, TO_VICT    );
    act( "$n&n bites $N&n!",  ch, NULL, victim, TO_NOTVICT );
    spell_poison( spl_poison, ch->level, ch, victim );
    return TRUE;
}

bool spec_thief( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    int        gold;

    last_fun( "spec_thief");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( ch->position != POS_STANDING )
    {
      if( ch->position == POS_FIGHTING && ch->fighting 
         && ch->fighting->fighting == ch)
      {
        start_grudge(ch, ch->fighting, FALSE);
        do_flee( ch, "" );
        do_hide( ch, "" );
      }
      return FALSE;
    }

    if( ch->hating )
    {
      if( !ch->fighting && ch->position != POS_FIGHTING && ch->hating->who->in_room == ch->in_room )
        if( backstab( ch, ch->hating->who ))
          return TRUE;
    }

    if( ch->fighting && ch->position == POS_FIGHTING )
    {
       if( combat_skill_check( ch ) )
         return TRUE;
    }

    for ( victim = ch->in_room->people; victim;
	 victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	    || victim->level >= LEVEL_IMMORTAL
            || victim->fly_level != ch->fly_level
	    || number_bits( 3 ) != 0
	    || !can_see( ch, victim ) ) /* Thx Glop */
	    continue;

	if ( IS_AWAKE( victim ) && victim->level > 5
	    && number_percent( ) + ch->level - victim->level >= 33 )
	{
	    act( "You discover $n&n's hands in your wallet!",
		ch, NULL, victim, TO_VICT );
	    act( "$N&n discovers $n&n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    gold = victim->money.gold * number_range( 1, 20 ) / 100;
	    ch->money.gold     += 3 * gold / 4;
	    victim->money.gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}

/*
 * Psionicist spec_fun by Thelonius for EnvyMud.
 */
bool spec_cast_psionicist( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_cast_psionicist");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    if ( ch->position != POS_FIGHTING )
    {
        if( ch->position == POS_STANDING )
        {
            if( spellup( ch ))
               return TRUE;
            if( spellup_others( ch ))
               return TRUE;
        }
	return FALSE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "mind thrust";          break;
	case  1: min_level =  4; spell = "psychic drain";        break;
	case  2: min_level =  6; spell = "agitation";            break;
	case  3: min_level =  8; spell = "psychic crush";        break;
	case  4: min_level =  9; spell = "project force";        break;
	case  5: min_level = 13; spell = "ego whip";             break;
	case  6: min_level = 14; spell = "energy drain";         break;
	case  7:
	case  8: min_level = 17; spell = "psionic blast";        break;
	case  9: min_level = 31; spell = "detonate";             break;
	case 10: min_level = 27; spell = "disintegrate";         break;
        case 11: min_level = 41; spell = "neural fragmentation"; break;
        case 12: min_level = 21; spell = "inflict pain";         break;
	default: min_level = 25; spell = "ultrablast";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}

bool spec_cast_ghost( CHAR_DATA *ch, int cmd )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    last_fun( "spec_cast_ghost");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    summon_if_hating( ch );

    if  ( weather_info.sunlight == SUN_LIGHT  )
    {

      if ( !ch->in_room )
      {
	  bug( "Spec_cast_ghost: NULL in_room.", 0 );
	  return FALSE;
      }

      if ( ch->fighting )
	  stop_fighting( ch, TRUE );

      act( "A beam of sunlight strikes $n&n, destroying $m.",
	  ch, NULL, NULL, TO_ROOM);

      extract_char( ch, TRUE );
      last_fun( "spec_cast_ghost");
      return TRUE;

    }

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted )
	    continue;
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
//	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = spell_lookup( spell ) ) < 0 )
	return FALSE;
    (*spells_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}


/*
 * spec_fun to repair bashed doors by Thelonius for EnvyMud.
 */
bool spec_repairman( CHAR_DATA *ch, int cmd )
{
		 EXIT_DATA       *pexit;
		 EXIT_DATA       *pexit_rev;
		 ROOM_INDEX_DATA *to_room;
    extern const int              rev_dir [ ];
		 int              door;

    last_fun( "spec_repairman");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    if ( !IS_AWAKE( ch ) )
	return FALSE;

    door = number_range( 0, (MAX_DIR - 1) );
    /*
     *  Could search through all doors randomly, but deathtraps would 
     *  freeze the game!  And I'd prefer not to go through from 1 to 6...
     *  too boring.  Instead, just check one direction at a time.  There's
     *  a 51% chance they'll find the door within 4 tries anyway.
     *  -- Thelonius (Monk)
     */
    if ( !( pexit = ch->in_room->exit[door] ) )
	return FALSE;

    if ( IS_SET( pexit->exit_info, EX_BASHED ) )
    {
	REMOVE_BIT( pexit->exit_info, EX_BASHED );
	act( "You repair the $d.", ch, NULL, pexit->keyword, TO_CHAR );
	act( "$n&n repairs the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* Don't forget the other side! */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_BASHED );

	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
		act( "The $d is set back on its hinges.",
		    rch, NULL, pexit_rev->keyword, TO_CHAR );
	}

	return TRUE;
    }

    return FALSE;
}

// Guards spawned during an invasion.  This causes them to unspawn if the victim is gone.
bool spec_justice_guard( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_justice_guard");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    // if the victim is gone or dead, then get rid of the guard.
    if( !ch->hunting && !ch->hating )
    {
        if( ch->in_room && ch->in_room->area->dispatched > 0 )
          ch->in_room->area->dispatched--;
        extract_char( ch, TRUE );
        last_fun( "spec_justice_guard");
        return TRUE;
    }

    // if they leave the zone, the guards need to dissipate after a bit
    // or the town will be left helpless because the guards will want to kill
    // someone who is long gone and not spawn to defend the town.
    // since they stop hunting someone who has left the zone, they should stop
    // hating them in order to dissipate.  To keep someone from popping in and
    // out to distract the guards, it should take a bit for them to dissipate.
    // this can be done by using a random chance of rehunting or stop hating
    if( ch->hating )
    {
      if( number_percent() < 50 )
      {
         stop_hating( ch );
         return TRUE;
      }
      else
      {
         start_hunting( ch, ch->hating->who );
         return TRUE;
      }
    }

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    return FALSE;
}

// Guard spawned when a crime is committed and the criminal needs to be captured
bool spec_justice_tracker( CHAR_DATA *ch, int cmd )
{
    last_fun( "spec_justice_tracker");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    // if the victim is gone or dead, then get rid of the guard.
    if( !ch->hunting )
    {
        extract_char( ch, TRUE );
        last_fun( "spec_justice_tracker");
        return TRUE;
    }

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    return FALSE;
}

// Checks for a mob spelling itself up.  Certain spells have higher
// priority and percentage chances.
//
// The spell_lookup function is horribly inefficient and anywhere possible
// should be replaced with gsn's (spl_armor and whatever) - Veygoth
//
// The commented out spells have not been written yet.
//
// The check_spellup function automatically checks to see whether they
// are effected by the particular spell, so unless the spell has an
// associated bitvector, there is no need for extra logic.
//
// Spells with associated bitvectors will need the "is_affected" stuff
// before the if( check_spellup( ... ) ) for the particular spell
bool spellup( CHAR_DATA *ch )
{
    if( has_affect( ch, 0, spl_blindness, 0 ))
    {
      if( check_spellup( ch, spl_cure_blind, 50 )) return TRUE;
      if( check_spellup( ch, spl_purify_spirit, 35 )) return TRUE;
    }

    if( !is_affected( ch, AFF_BLUR ))
      if( check_spellup( ch, spl_blur, 45 )) return TRUE;

    if( !is_affected( ch, AFF_STONESKIN ))
      if( check_spellup( ch, spl_stoneskin, 60 )) return TRUE;

    if( !is_affected( ch, AFF_DAZZLE ))
      if( check_spellup( ch, spl_dazzle, 60 )) return TRUE;

    if( !is_affected( ch, AFF_MAJOR_GLOBE ))
      if( check_spellup( ch, spl_major_globe, 60 )) return TRUE;

    if( !is_affected( ch, AFF_GREATER_SPIRIT_WARD ) && !is_affected( ch, AFF_SPIRIT_WARD ))
      if( check_spellup( ch, spl_gr_spirit_ward, 65 )) return TRUE;

    if( !is_affected( ch, AFF_MINOR_GLOBE ))
      if( check_spellup( ch, spl_minor_globe, 25 )) return TRUE;

    if( !is_affected( ch, AFF_SPIRIT_WARD ) && !is_affected( ch, AFF_GREATER_SPIRIT_WARD ))
      if( check_spellup( ch, spl_spirit_ward, 35 )) return TRUE;

    if( !is_affected( ch, AFF_SOULSHIELD ))
      if( check_spellup( ch, spl_soulshield, 30 )) return TRUE;

    /* Fireshield and Frostshield cancel eachother!  So, we don't want
     * that to happen.  Easiest way is to give a chance to try either.
     * Odds are, if they only have one of the two, that they'll take twice
     * as long to cast it on themselves. Yes, this is a quick fix.  Lohrr
     */
    if( !is_affected( ch, AFF_FLAMING ) && !is_affected( ch, AFF_COLDSHIELD ))
    {
      if( number_percent() < 50 )
      {
        if( check_spellup( ch, spl_flame_shield, 30 )) return TRUE;
      }
      else
        if( check_spellup( ch, spl_frost_shield, 30 )) return TRUE;
    }

    if( check_spellup( ch, spl_vitality, 55 )) return TRUE;

    if( check_spellup( ch, spl_true_seeing, 45 )) return TRUE;
    if( check_spellup( ch, spl_gr_spirit_sight, 45 )) return TRUE;

    /* Govno If mob is hurt (and not a teacher), invis. Otherwise don't. */
    if(( !is_affected( ch, AFF_INVISIBLE )) && 
          !(IS_SET( ch->act, ACT_TEACHER )))
      if( (ch->hit < get_max_hit( ch )) && check_spellup( ch, spl_invis, 40 ) )
      {
          return TRUE;
      }

/* Govno if mob is hurt (and not a teacher), conceal, otherwise don't. */
    if(( !is_affected( ch, AFF_MINOR_INVIS )) &&
          !(IS_SET( ch->act, ACT_TEACHER )))
      if( (ch->hit < get_max_hit( ch )) && check_spellup( ch, spl_concealment, 35 ) )
      {
          return TRUE;
      }

    if( !has_affect( ch, 0, spl_enlarge, 0) && !has_affect( ch, 0, spl_reduce, 0))
    {
      if( check_spellup( ch, spl_enlarge, 60 )) return TRUE;
      if( check_spellup( ch, spl_reduce, 20 )) return TRUE;
    }

    if( !is_affected( ch, AFF_HASTE ))
      if( check_spellup( ch, spl_haste, 45 )) return TRUE;

    if ( !has_affect( ch, 0, spl_bear, 0 ))
      if( check_spellup( ch, spl_elephant, 60 )) return TRUE;

    if( check_spellup( ch, spl_lion, 40 )) return TRUE;

    if ( !has_affect( ch, 0, spl_elephant, 0 ))
      if( check_spellup( ch, spl_bear, 30 )) return TRUE;

    if( check_spellup( ch, spl_hawk, 40 )) return TRUE;

    if( !is_affected( ch, AFF_BARKSKIN ))
      if( check_spellup( ch, spl_barkskin, 60 )) return TRUE;

    if( check_spellup( ch, spl_shield, 50 )) return TRUE;

    if( check_spellup( ch, spl_armor, 50 )) return TRUE;

    if( check_spellup( ch, spl_shadow_shield, 50 )) return TRUE;

    if( check_spellup( ch, spl_spirit_armor, 50 )) return TRUE;

    if( !is_affected( ch, AFF_DETECT_INVIS ))
      if( check_spellup( ch, spl_detect_invis, 50)) return TRUE;

    if( check_spellup( ch, spl_bless, 55 )) return TRUE;

    if( !is_affected( ch, AFF_SENSE_LIFE ))
    {
      if( check_spellup( ch, spl_sense_life, 35 )) return TRUE;
      if( check_spellup( ch, spl_sense_spirit, 35 )) return TRUE;
    }

    if( !can_fly( ch ))
    {
      if( check_spellup( ch, spl_fly, 35 )) return TRUE;
      if( check_spellup( ch, spl_gr_ravenflight, 55 )) return TRUE;
      if( check_spellup( ch, spl_ravenflight, 35 )) return TRUE;
    }

    if( !is_affected( ch, AFF_LEVITATE ))
      if( check_spellup( ch, spl_levitation, 25)) return TRUE;

    if ( has_affect( ch, 0, spl_curse, 0 ) || has_affect( ch, 0, spl_hex, 0) || has_affect( ch, 0, spl_malison, 0) )
    {
//      if( check_spellup( ch, spl_remove_curse, 40 )) return TRUE;
      if( check_spellup( ch, spl_negate_hex, 40 )) return TRUE;
    }

    if( check_spellup( ch, spl_giant_strength, 40 )) return TRUE;
    if( check_spellup( ch, spl_dexterity, 20 )) return TRUE;

    if( !is_affected( ch, AFF_PROTECT_EVIL ))
    {
      if( check_spellup( ch, spl_prot_evil, 25 )) return TRUE;
      if( check_spellup( ch, spl_harbor_balance, 25 )) return TRUE;
    }

    if( !is_affected( ch, AFF_PROTECT_GOOD ))
    {
      if( check_spellup( ch, spl_protection_good, 25 )) return TRUE;
      if( check_spellup( ch, spl_harbor_balance, 25 )) return TRUE;
    }

    if( !is_affected( ch, AFF_PROTECT_FIRE ))
    {
      if( check_spellup( ch, spl_adaptation, 45 )) return TRUE;
      if( check_spellup( ch, spl_protect_fire, 25 )) return TRUE;
      if( check_spellup( ch, spl_fire_ward, 30 )) return TRUE;
    }

    if( !is_affected( ch, AFF_PROTECT_COLD ))
    {
      if( check_spellup( ch, spl_protect_cold, 25 )) return TRUE;
      if( check_spellup( ch, spl_cold_ward, 30 )) return TRUE;
    }

    if( !is_affected( ch, AFF_PROTECT_LIGHTNING ))
      if( check_spellup( ch, spl_protect_lightning, 25 )) return TRUE;

    if( !is_affected( ch, AFF_PROTECT_ACID ))
      if( check_spellup( ch, spl_protect_acid, 25 )) return TRUE;

    if( !is_affected( ch, AFF_PROTECT_GAS ))
      if( check_spellup( ch, spl_protect_gas, 25 )) return TRUE;

    if( !is_affected( ch, AFF_DETECT_MAGIC ))
    {
      if( check_spellup( ch, spl_detect_magic, 9 )) return TRUE;
      if( check_spellup( ch, spl_spirit_sight, 22 )) return TRUE;
    }

    if( !is_affected( ch, AFF_DETECT_GOOD ))
    {
      if( check_spellup( ch, spl_detect_good, 7 )) return TRUE;
      if( check_spellup( ch, spl_spirit_sight, 22 )) return TRUE;
      if( check_spellup( ch, spl_analyze_balance, 20 )) return TRUE;
    }

    if( !is_affected( ch, AFF_DETECT_EVIL ))
    {
      if( check_spellup( ch, spl_detect_evil, 7 )) return TRUE;
      if( check_spellup( ch, spl_spirit_sight, 22 )) return TRUE;
      if( check_spellup( ch, spl_analyze_balance, 20 )) return TRUE;
    }

    if( !is_affected( ch, AFF_INFRARED ))
      if( check_spellup( ch, spl_infravision, 5 )) return TRUE;

    if ( !has_affect( ch, 0, spl_wolf, 0 ))
      if( check_spellup( ch, spl_panther, 70 )) return TRUE;

    if ( !has_affect( ch, 0, spl_panther, 0 ))
      if( check_spellup( ch, spl_wolf, 30 )) return TRUE;

    if( !is_affected( ch, AFF_PROWESS ) && !is_affected( ch, AFF_INCOMPETENCE ))
    {
      if( check_spellup( ch, spl_prowess, 8 )) return TRUE;
      if( check_spellup( ch, spl_incompetence, 6 )) return TRUE;
    }

    return FALSE;
}

bool offensive_spell( CHAR_DATA *ch, CHAR_DATA *victim )
{

    last_fun( "offensive_spell" );

    if( check_offensive( ch, victim, spl_bigby_fist, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_akiaurn_word, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_iceball, 60 ))
      return TRUE;
//    if( check_offensive( ch, victim, spl_call_wild, 4 ))
//      return TRUE;
    if( check_offensive( ch, victim, spl_immolate, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_full_harm, 65 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_gr_soul_dist, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_flamestrike, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_fireball, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_sunray, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_harm, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_cone_of_cold, 50 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_color_spray, 40 ))
      return TRUE;
    if ( IS_UNDEAD( victim ))
    {
      if( check_offensive( ch, victim, spl_destroy_undead, 60 ))
        return TRUE;
      if( check_offensive( ch, victim, spl_turn_undead, 25 ))
        return TRUE;
    }
    if( check_offensive( ch, victim, spl_demi_shad_magic, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_spirit_anguish, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_lightning_bolt, 55 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_molten, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_soul_disturbance, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_scorching, 30 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_shadow_magic, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_energy_drain, 45 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_shocking_grasp, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_acid_blast, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_touch_nightshade, 50 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_scalding, 55 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_sunburst, 20 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_power_word_blind, 10 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_plague, 25 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_poison, 15 ))
      return TRUE;
    if( IS_GOOD( ch ) && IS_EVIL( victim ))
    {
      if( check_offensive( ch, victim, spl_dispel_evil, 45 )) return TRUE;
      if( check_offensive( ch, victim, spl_gleam_dawn, 45 )) return TRUE;
    }
    else if( IS_EVIL( ch ) && IS_GOOD( ch ))
    {
      if( check_offensive( ch, victim, spl_dispel_good, 45 )) return TRUE;
      if( check_offensive( ch, victim, spl_gleam_dusk, 45 )) return TRUE;
    }
    if( check_offensive( ch, victim, spl_pythonsting, 17 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_earthen_grasp, 16 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_entangle, 14 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_blindness, 20 ))
      return TRUE;
    if( victim->class != CLASS_WARRIOR && victim->class != CLASS_MERCENARY &&
        victim->class != CLASS_ASSASSIN && victim->class != CLASS_THIEF )
    {
      if( check_offensive( ch, victim, spl_feeblemind, 25 ))
        return TRUE;
    }
    if( !is_affected( victim, AFF_CURSE ))
    {
      if( check_offensive( ch, victim, spl_malison, 15 ))
        return TRUE;
//      if( check_offensive( ch, victim, spl_curse, 5 ))
//        return TRUE;
    }
    if( check_offensive( ch, victim, spl_slowness, 20 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_shrew, 15 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_fear, 10 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_wither, 15 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_mouse, 12 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_enfeeblement, 8 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_enthrall, 45 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_minor_para, 20 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_sleep, 6 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_cause_critical, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_gr_agony, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_bird_prey, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_burning_hands, 65 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_chill_touch, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_prismatic_orb, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_cause_serious, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_agony, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_acid_arrow, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_azure_flame, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_gust_of_frost, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_water_bolt, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_spark, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_dirt_cloud, 30 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_cutting_breeze, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_ravenous_vines, 20 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_flameburst, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_acid_spray, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_fire_bolt, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_ice_bolt, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_dust_blast, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_pebble, 40 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_magic_missile, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_ice_missile, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_cause_light, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_m_agony, 60 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_faerie_fire, 15 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_mole, 45 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_hex, 45 ))
      return TRUE;
    if( check_offensive( ch, victim, spl_snail, 45 ))
      return TRUE;

    return FALSE;
}

// Keep in mind that any healing spells that give affects to the mob
// will only be able to be cast if they are not affected by them because
// the check_spellup function checks to see if they are affected by
// that spell, so a heal spell that heals *and* blesses will not work
// properly and we'll need to write a new function for it -- Veygoth
bool heal_self( CHAR_DATA *ch )
{
    if( check_spellup( ch, spl_fullheal, 75 )) return TRUE;
    if( check_spellup( ch, spl_aid, 75 )) return TRUE;
    if( check_spellup( ch, spl_gr_mending, 75 )) return TRUE;
    if( check_spellup( ch, spl_heal, 75 )) return TRUE;
    if( check_spellup( ch, spl_mending, 75 )) return TRUE;
    if( check_spellup( ch, spl_soothe, 75 )) return TRUE;
    if( has_affect( ch, 0, spl_poison, 0 ))
    {
      if( check_spellup( ch, spl_remove_poison, 50 )) return TRUE;
      if( check_spellup( ch, spl_purify_spirit, 35 )) return TRUE;
      if( check_spellup( ch, spl_slow_poison, 25 )) return TRUE;
    }
    if( has_affect( ch, 0, spl_plague, 0 ))
    {
      if( check_spellup( ch, spl_cure_disease, 55 )) return TRUE;
    }
    if( check_spellup( ch, spl_cure_critical, 75 )) return TRUE;
    if( check_spellup( ch, spl_herbal_remedy, 75 )) return TRUE;
    if( check_spellup( ch, spl_ls_mending, 75 )) return TRUE;
    if( check_spellup( ch, spl_cure_light, 75 )) return TRUE;
    if( check_spellup( ch, spl_ls_herbal_remedy, 75 )) return TRUE;

    return FALSE;
}

bool check_vigorize( CHAR_DATA *ch )
{
   if( ch->move > 30 )
     return FALSE;

   if( check_spellup( ch, spl_vig_critical, 40 )) return TRUE;
   if( check_spellup( ch, spl_gr_stamina, 35 )) return TRUE;
   if( check_spellup( ch, spl_vig_serious, 35 )) return TRUE;
   if( check_spellup( ch, spl_stamina, 35 )) return TRUE;
   if( check_spellup( ch, spl_vig_light, 30 )) return TRUE;

   return FALSE;
}

bool area_offensive( CHAR_DATA *ch, CHAR_DATA *victim )
{
    last_fun( "area_offensive" );

    if( IS_OUTSIDE( ch ))
    {
      if( check_offensive( ch, victim, spl_meteor, 50 )) return TRUE;
      if( !is_underground( ch ))
        if( check_offensive( ch, victim, spl_earthen_rain, 55 )) return TRUE;
    }
    if( check_offensive( ch, victim, spl_incendiary_cloud, 45 )) return TRUE;
    if( check_offensive( ch, victim, spl_sunburst, 40 )) return TRUE;
    if( check_offensive( ch, victim, spl_creeping_doom, 55 )) return TRUE;
    if( check_offensive( ch, victim, spl_gr_earthen_grasp, 38 )) return TRUE;
    if( IS_GOOD( ch ) && IS_EVIL( victim ))
    {
        if( check_offensive( ch, victim, spl_judgement, 55 )) return TRUE;
        if( check_offensive( ch, victim, spl_holy_word, 40 )) return TRUE;
    }
    else if( IS_EVIL( ch ) && IS_GOOD( victim ))
    {
        if( check_offensive( ch, victim, spl_apocalypse, 55 )) return TRUE;
        if( check_offensive( ch, victim, spl_unholy_word, 40 )) return TRUE;
    }
    if( check_offensive( ch, victim, spl_gr_pythonsting, 38 )) return TRUE;
    if( check_offensive( ch, victim, spl_prismatic_spray, 50 )) return TRUE;
    if( check_offensive( ch, victim, spl_chain_lightning, 50 )) return TRUE;
    if( check_offensive( ch, victim, spl_scathing_wind, 50 )) return TRUE;
    if( check_offensive( ch, victim, spl_firestorm, 50 )) return TRUE;
    if( IS_OUTSIDE( ch ))
    {
      if( check_offensive( ch, victim, spl_call_lightning, 25 )) return TRUE;
    }
    if( check_offensive( ch, victim, spl_ice_storm, 25 )) return TRUE;
    if( check_offensive( ch, victim, spl_earthquake, 50 )) return TRUE;

    return FALSE;
}

// Direct targeted spells (tar_char_offensive)
bool check_offensive( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int percent )
{
    char buf [100];
    sprintf( buf, "check_offensive: spell (%d)'%s'", sn, spells_table[sn].name );
    last_fun( buf );

       if( sn <= 0 ) return FALSE;

       if( ( ch->level >=
          (spells_table[sn].spell_circle[ch->class] * 5 - 4))
          && ( number_percent() < percent ))
          {
             CAST_DATA *caster = NULL;
#ifdef HARDCORE_DEBUG
             bug( "Mob casting spell %d.", sn );
#endif

             if( spells_table[sn].target != TAR_CHAR_OFFENSIVE &&
                 spells_table[sn].target != TAR_IGNORE &&
                 spells_table[sn].target != TAR_CHAR_RANGE )
               bug( "Check_spellup:  Mob casting spell %d which is neither target offensive nor ignore.a", sn );

             act( "$n&n starts casting...", ch, NULL, NULL, TO_ROOM );
	     SET_AFF_BIT( ch, AFF_CASTING );
             caster = alloc_mem( sizeof( CAST_DATA ));
             caster->who = ch;
             caster->next = cast_list;
             caster->event = create_event( EVENT_SPELL_CAST, spells_table[sn].beats, ch, victim, sn );
             cast_list = caster;
             return TRUE;
          }

       return FALSE;
}

// Defensive targeted spells (tar_char_defensive)
// Mainly spellups that can be cast on others
bool check_defensive( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int percent )
{
       if( sn <= 0 ) return FALSE;

       if( ( ch->level >=
          (spells_table[sn].spell_circle[ch->class] * 5 - 4))
          && ( number_percent() < percent ))
          {
             CAST_DATA *caster = NULL;
#ifdef HARDCORE_DEBUG
             bug( "Mob casting spell %d.", sn );
#endif

             if( spells_table[sn].target != TAR_CHAR_DEFENSIVE &&
                 spells_table[sn].target != TAR_IGNORE )
               bug( "Check_defensive:  Mob casting spell %d which is neither target defensive nor ignore.b", sn );

             act( "$n&n starts casting...", ch, NULL, NULL, TO_ROOM );
	     SET_AFF_BIT( ch, AFF_CASTING );
             caster = alloc_mem( sizeof( CAST_DATA ));
             caster->who = ch;
             caster->next = cast_list;
             caster->event = create_event( EVENT_SPELL_CAST, spells_table[sn].beats, ch, victim, sn );
             cast_list = caster;
             return TRUE;
          }

       return FALSE;
}

// This function should prevent a mob from spamming
// spellups on itself if it is already affected by the spell.
// in the spellup() function, checks for AFF_WHATEVER
// still need to be done. - Veygoth
bool check_spellup( CHAR_DATA *ch, int sn, int percent )
{
       if( sn <= 0 ) return FALSE;

       // Keep mobs from casting spells they are affected by
       if( has_affect( ch, 0, sn, 0 ))
           return FALSE;

       if( ( ch->level >=
          (spells_table[sn].spell_circle[ch->class] * 5 - 4))
          && ( number_percent() < percent ))
          {
             CAST_DATA *caster = NULL;
#ifdef HARDCORE_DEBUG
             bug( "Mob casting spell %d.", sn );
#endif
             if( spells_table[sn].target != TAR_CHAR_DEFENSIVE &&
                 spells_table[sn].target != TAR_CHAR_SELF )
               bug( "Check_spellup:  Mob casting spell %d which is neither target self nor target defensive.c", sn );

             act( "$n&n starts casting...", ch, NULL, NULL, TO_ROOM );
	     SET_AFF_BIT( ch, AFF_CASTING );
             caster = alloc_mem( sizeof( CAST_DATA ));
             caster->who = ch;
             caster->next = cast_list;
             caster->event = create_event( EVENT_SPELL_CAST, spells_table[sn].beats, ch, ch, sn );
             cast_list = caster;
             return TRUE;
          }

       return FALSE;
}

// Checks melee combat skills for mobs - Veygoth
bool combat_skill_check( CHAR_DATA *ch )
{
   if( ch->level >= skills_table[gsn_kick].skill_level[ch->class] && number_percent() < 60 )
   {
      do_kick( ch, "" );
      return TRUE;
   }
   
   if( ch->level >= skills_table[gsn_bash].skill_level[ch->class] && number_percent() < 40
     && ch->fighting != NULL && !(ch->fighting->deleted)
     && ch->size >= ch->fighting->size && ch->size - 2 <= ch->fighting->size )
   {
      do_bash( ch, "" );
      return TRUE;
   }

   if( ch->level >= skills_table[gsn_springleap].skill_level[ch->class] && number_percent() < 50 )
   {
      do_springleap( ch, "" );
      return TRUE;
   }

   if( ch->level >= skills_table[gsn_headbutt].skill_level[ch->class] && number_percent() < 40 )
   {
      do_headbutt( ch, "" );
      return TRUE;
   }

   if( ch->level >= skills_table[gsn_circle].skill_level[ch->class] && number_percent() < 40 )
   {
      do_circle( ch, "" );
      return TRUE;
   }

   if( ch->level >= skills_table[gsn_trip].skill_level[ch->class] && number_percent() < 35 )
   {
      do_trip( ch, "" );
      return TRUE;
   }

   if( ch->level >= skills_table[gsn_dirt].skill_level[ch->class] && number_percent() < 60 )
   {
      do_dirt( ch, "" );
      return TRUE;
   }


   return FALSE;
}

bool spellup_others( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if (IS_ROOM( ch->in_room, ROOM_NO_MAGIC) )
        return FALSE;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    if ( is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
	|| IS_ROOM( ch->in_room, ROOM_SILENT ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 && IS_NPC( victim ))
	    break;
    }

    if ( !victim )
	return FALSE;

    if( victim->hit < (get_max_hit( victim ) - 10))
    {
      if( check_defensive( ch, victim, spl_fullheal, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_gr_mending, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_aid, 60 )) return TRUE;
      if( check_defensive( ch, victim, spl_heal, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_mending, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_soothe, 75 )) return TRUE;
      if( has_affect( victim, 0, spl_poison, 0 ))
        if( check_defensive( ch, victim, spl_remove_poison, 50 )) return TRUE;
      if( has_affect( victim, 0, spl_blindness, 0 ))
        if( check_defensive( ch, victim, spl_cure_blind, 50 )) return TRUE;
      if( check_defensive( ch, victim, spl_cure_critical, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_herbal_remedy, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_ls_mending, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_cure_light, 75 )) return TRUE;
      if( check_defensive( ch, victim, spl_ls_herbal_remedy, 75 )) return TRUE;
    }

    if( !is_affected( victim, AFF_STONESKIN ))
      if( check_defensive( ch, victim, spl_stoneskin, 60 )) return TRUE;

    if( !is_affected( victim, AFF_MAJOR_GLOBE ))
      if( check_defensive( ch, victim, spl_major_globe, 60 )) return TRUE;

    if( !is_affected( victim, AFF_GREATER_SPIRIT_WARD ) && !is_affected( victim, AFF_SPIRIT_WARD ))
      if( check_defensive( ch, victim, spl_gr_spirit_ward, 65 )) return TRUE;

    if( !is_affected( victim, AFF_SPIRIT_WARD ) && !is_affected( victim, AFF_GREATER_SPIRIT_WARD ))
      if( check_defensive( ch, victim, spl_spirit_ward, 35 )) return TRUE;

    if( !is_affected( victim, AFF_MINOR_GLOBE ))
      if( check_defensive( ch, victim, spl_minor_globe, 25 )) return TRUE;

    if( !is_affected( victim, AFF_HASTE ))
      if( check_defensive( ch, victim, spl_haste, 45 )) return TRUE;

    if( !has_affect( victim, 0, spl_enlarge, 0) && !has_affect( victim, 0, spl_reduce, 0) )
      if( check_defensive( ch, victim, spl_enlarge, 65 )) return TRUE;
  
    if( !has_affect( victim, 0, spl_enlarge, 0) && !has_affect( victim, 0, spl_reduce, 0) )
      if( check_defensive( ch, victim, spl_reduce, 15 )) return TRUE;

    if( !is_affected( victim, AFF_BARKSKIN ))
      if( check_defensive( ch, victim, spl_barkskin, 60 )) return TRUE;

    if( !is_affected( victim, AFF_DETECT_INVIS ))
      if( check_defensive( ch, victim, spl_detect_invis, 50)) return TRUE;

    if( !can_fly( victim ))
    {
      if( check_defensive( ch, victim, spl_fly, 35 )) return TRUE;
      if( check_defensive( ch, victim, spl_ravenflight, 35 )) return TRUE;
    }

    if( !is_affected( victim, AFF_PROTECT_ACID ))
      if( check_defensive( ch, victim, spl_protect_acid, 35 )) return TRUE;

    if( !is_affected( victim, AFF_PROTECT_COLD ))
      if( check_defensive( ch, victim, spl_protect_cold, 35 )) return TRUE;

    if( !is_affected( victim, AFF_PROTECT_GAS ))
      if( check_defensive( ch, victim, spl_protect_gas, 35 )) return TRUE;

    if( !is_affected( victim, AFF_PROTECT_FIRE ))
      if( check_defensive( ch, victim, spl_protect_fire, 35 )) return TRUE;

    if( !is_affected( victim, AFF_PROTECT_LIGHTNING ))
      if( check_defensive( ch, victim, spl_protect_lightning, 35 )) return TRUE;

    return FALSE;
}

bool spec_lost_girl( CHAR_DATA *ch, int cmd )
{
  CHAR_DATA *demon, *i, *i_next;
  CHAR_DATA *was_fighting = NULL;
    int vnum = 20531;


  if (!ch || !IS_AWAKE(ch))
    return FALSE;

  if ( ch->fighting ) {
      do_say(ch, "I would not do such things if I were you!");

      demon = transform_mob( &ch, vnum, NULL );
      if (!demon) {
        return FALSE;
      }
      do_say(demon, "Now you shall pay!");
      //call for assistance (sets helpers hating whoever is fighting)
      shout_and_hunt( demon, was_fighting, "Come, my demon brethren!", NULL );
//      shout_and_hunt( demon, was_fighting, "Come, my demon brethren!", helpers );

      last_fun( "lost_girl, return from shout" );
      // set the lil critters aggro, so they can transform
      for (i = char_list; i; i = i_next) {
        i_next = i->next;
        if ( i->deleted || !i->pIndexData ) continue;
        if (IS_NPC(i) && ((i->pIndexData->vnum == 20524) ||
                 (i->pIndexData->vnum == 20525) ||
                 (i->pIndexData->vnum == 20526))
            && i->in_room && i->in_room->area == ch->in_room->area ) {
          act("$n shivers.", i, 0, 0, TO_ROOM);
          i->hit = i->max_hit = 5000;
          SET_BIT(i->act, ACT_AGGRESSIVE);
        } //end if
      } //end for
      // set the trees aggro too
      for (i = char_list; i; i = i_next) {
        i_next = i->next;
        if ( i->deleted ) continue;
        if (IS_NPC(i) && ((i->pIndexData->vnum == 20528) || 
                 (i->pIndexData->vnum == 20529)) &&
            i->in_room && i->in_room->area == ch->in_room->area ) {
          act("$n shivers.", i, 0, 0, TO_ROOM);
          SET_BIT(i->act, ACT_AGGRESSIVE);
        } //end if
      } //end for
      extract_char( ch, TRUE );
      ch = NULL;
      return TRUE;
  }

  return FALSE;
}

bool shout_and_hunt( CHAR_DATA *ch, CHAR_DATA *victim, char *msg, int *helpers) {
    int i;
//    int j;
    int count = 0;
    CHAR_DATA *vch;
    bool is_helper;
    char buf[MAX_STRING_LENGTH];
//    char *pbuf;

    last_fun( "shout_and_hunt" );

    //send the shout message
    sprintf( buf, msg, victim->name );
    do_shout( ch, buf );

    if ( !helpers ) return FALSE;
    //loop through all chars
    for( vch = char_list; vch; vch = vch->next ) {
        if ( vch->deleted ) continue;
        if ( !vch->in_room ) continue;
        if (!vch->pIndexData ) continue;
        if ( !IS_NPC( vch ) || vch->in_room->area != ch->in_room->area )
            continue;
        is_helper = FALSE;
        for(i = 0; helpers[i] > 0; i++) {
            if ( vch->pIndexData->vnum == helpers[i] )
                is_helper = TRUE;
        }
        if ( !is_helper )
            continue;
//        sprintf( lbuf, "Processing shout_and_hunt for vch %d: ", (int)vch);
//        if ( vch->short_descr )
//            strcat( lbuf, vch->short_descr );
//        log_string( lbuf );
         start_grudge( vch, victim, TRUE );
last_fun( "shout_and_hunt: return from start_grudge" );
        count++;
    }
    if (count > 0) return TRUE;
    return FALSE;
}

bool spec_grumbar_shout( CHAR_DATA *ch, int cmd )
  {
  int helpers[] = {9500, 9501, 9502, 9503, 9504, 9505, 9506, 9507, 
         9508, 9509, 9510, 0};
  static CHAR_DATA *was_fighting;

    last_fun( "spec_grumbar_shout" );

    if ( cmd == PROC_DEATH )
        return FALSE;
  if (!ch || !IS_AWAKE(ch))
    return FALSE;

  if ( !ch->fighting ) {
    was_fighting = NULL;
    return FALSE;
  }

  if ( ch->fighting == was_fighting )
    return FALSE;

  was_fighting = ch->fighting;
  return shout_and_hunt(ch, ch->fighting, "&+LDenizens of the Earth plane, %s has trespassed upon us!", helpers );
}

bool spec_xorn( CHAR_DATA *ch, int cmd ) {
    CHAR_DATA *vict;

    last_fun( "spec_xorn");
    if ( cmd == PROC_DEATH )
        return FALSE;
        
    vict = ch->fighting;
    if ( ch->fighting ) {
        if ( number_percent() < 50 ) {
            // make sure they are pissed off
            start_grudge( ch, ch->fighting, TRUE );
            // teleport  out
            ch->fighting->fighting = NULL;
            ch->fighting = NULL;
            move_xorn( ch, get_room_index( 9676 ), XORN_MOVE_EARTH );
            return TRUE;
        }
        return FALSE;
    }
    if ( ch->hating || ch->hunting ) {
//        if ( number_percent() > 50 ) return FALSE;
        vict = ch->hating->who;
        if ( !vict ) 
            vict = ch->hunting->who;
        if ( !vict ) {
            bug( "spec_xorn: mob is hating or hunting, but who is null", 0);
            return FALSE;
        }
        if ( !vict->in_room || vict->in_room->area != ch->in_room->area ) {
            // vict not in EP
            return FALSE;
        }
        move_xorn( ch, vict->in_room, XORN_MOVE_EARTH );
        kill_ch( ch, vict );
        return TRUE;
    }

    if ( number_percent() < 15 ) {
        // phase in/out of existance
        if ( ch->in_room && ch->in_room->vnum == 9676 )
        {
            move_xorn( ch, NULL, XORN_MOVE_PHASE );
        }
        else
        {
            move_xorn( ch, get_room_index( 9676 ), XORN_MOVE_PHASE );
        }
    return TRUE;
    }
        
    return FALSE;
}

void move_xorn( CHAR_DATA *ch, ROOM_INDEX_DATA *to_room, int type ) {
    ROOM_INDEX_DATA *room = to_room;
    int min, max;
    
    last_fun( "move_xorn" );
    if ( room == NULL ) {
        min = ch->in_room->area->low_r_vnum;
        max = ch->in_room->area->hi_r_vnum;
        while ( !room ) {
            room = get_room_index( number_range( min, max ) );
        }
    }
    if ( type == XORN_MOVE_EARTH)
        act( "$n&n&+L sinks into the ground and dissappears from sight!&n",
            ch, NULL, NULL, TO_ROOM );
    else
        act( "$n&n phases out of existence.", ch, NULL, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, room );
    if (type == XORN_MOVE_EARTH )
        act( "$n&n&+L rises out of the earth right beside you.&n",
            ch, NULL, NULL, TO_ROOM );
    else
        act( "$n&n phases into existence.", ch, NULL, NULL, TO_ROOM );
    return;
}

CHAR_DATA *transform_mob( CHAR_DATA **pch, int vnum, char *msg ) {
    CHAR_DATA *new_ch, *was_fighting, *ch;
    OBJ_DATA *item, *next_item;
    int pos;
    
    last_fun( "transform_mob");
    ch  = *pch;
    if ( ch->fighting ) {
      was_fighting = ch->fighting;
      stop_fighting(ch, FALSE);
    }

      new_ch = create_mobile( get_mob_index(vnum) );

      if (!new_ch) {
        log_string( "assert: mob load failed in transform_mob()" );
        return NULL;
      }
      char_to_room(new_ch, ch->in_room);

      for (item = ch->carrying; item; item = next_item) {
        next_item = item->next_content;
        obj_from_char(item);
        obj_to_char(item, new_ch);
      }

      for (pos = 0; pos < MAX_WEAR; pos++) {
          item = get_eq_char(ch, pos);
          if ( !item ) continue;
          unequip_char( ch, item );
          obj_from_char( item );
          obj_to_char( item, new_ch );
          equip_char(new_ch, item, pos);
      }
    new_ch->position = ch->position;
    new_ch->act = ch->act;
    REMOVE_BIT( new_ch->act, PLR_WIZINVIS );
    new_ch->hating = ch->hating;
    ch->hating = NULL;
    new_ch->hunting = ch->hunting;
    ch->hunting = NULL;
    new_ch->fearing = ch->fearing;
    ch->fearing = NULL;

/*
    // replace ch with new_ch on any spell events
    for( event = event_first; event; event = event->next )
    {
        if ( event->deleted ) continue;
        if ( event->type == EVENT_SPELL_CAST ) {
            if ( ( CHAR_DATA *)event->arg2 == ch ) {
                sprintf( lbuf, "spell %d was targetting %s, now targetting %s",
                  event->var, ((CHAR_DATA *)event->arg2)->short_descr, 
                  ch->short_descr);
                log_string( lbuf );
                event->arg2 = (void *)new_ch;
            }
        }
    }

*/

    if ( msg )
      act( msg, ch, NULL, new_ch, TO_ROOM );
    else
      act("$n&n suddenly changes into $N!&n", ch, NULL, new_ch, TO_ROOM);

    if (was_fighting)
      set_fighting(new_ch, was_fighting);
    tail_chain();
    return new_ch;

}

bool spec_lil_critter( CHAR_DATA *ch , int cmd ) {
    CHAR_DATA *demon, *new_ch;
    int vnum = 20530;

    last_fun( "spec_lil_critter" );
    if ( cmd == PROC_DEATH ) {
        act( "The corpse of $n&n &+Wglows&n with a strange light.",
            ch, NULL, NULL, TO_ROOM );
        new_ch = create_mobile( get_mob_index( ch->pIndexData->vnum) );
        char_to_room( new_ch, ch->in_room );
        new_ch->act = ch->act;
        REMOVE_BIT( new_ch->act, PLR_WIZINVIS );
        act( "$p&n comes back to life!", ch, NULL, NULL, TO_ROOM );
        if ( IS_SET( ch->act, ACT_AGGRESSIVE ) ) {
            ch->hit = ch->max_hit = 5000;
        }
        return TRUE; //make no corpse
    }
    if ( ch->fighting && IS_SET( ch->act, ACT_AGGRESSIVE ) ) {
        // transform into a demon
        demon = transform_mob( &ch, vnum, NULL );
        if (!demon) {
            return FALSE;
        }
        /* get rid of the mob */
        extract_char(ch, TRUE);
        ch = (CHAR_DATA *)NULL;

        return TRUE;
    }
    return FALSE;
}

