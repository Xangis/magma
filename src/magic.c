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

MEM_DATA *mem_first = NULL;
MEM_DATA *memdata_free = NULL;

char lbuf[MAX_STRING_LENGTH];

void affect_refresh(CHAR_DATA *ch, int skl, int spl, int song, int duration);
void memorize( CHAR_DATA *ch, char *argument, bool pray, bool lesser );
void cast( CHAR_DATA *ch, char *argument );

/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
    int sn;

    last_fun( "skill_lookup");

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skills_table[sn].name )
	    break;
	if ( LOWER( name[0] ) == LOWER( skills_table[sn].name[0] )
	    && !str_prefix( name, skills_table[sn].name ) )
	    return sn;
    }

    return -1;
}

int monk_skill_lookup( const char *name )
{
    int sn;

    last_fun( "monk_skill_lookup");

    for ( sn = 0; sn < MAX_MONK_SKILL; sn++ )
    {
	if ( !monk_skills[sn].skill_name )
	    break;
	if ( LOWER( name[0] ) == LOWER( monk_skills[sn].skill_name[0] )
	    && !str_prefix( name, monk_skills[sn].skill_name ) )
	    return sn;
    }

    return -1;
}

/*
 * Lookup a spell by name.
 */
int spell_lookup( const char *name )
{
    int sn;

    last_fun( "spell_lookup");

    if ( !name ) {
       return -1;
    }

    sprintf( lbuf, "spell_lookup: %s", name );
    last_fun( lbuf );

    for ( sn = 0; sn < MAX_SPELL; sn++ )
    {
	if ( !spells_table[sn].name )
	    break;
	if ( LOWER( name[0] ) == LOWER( spells_table[sn].name[0] )
	    && !str_prefix( name, spells_table[sn].name ) )
	    return sn;
    }

    return -1;
}

/*
 * Lookup a spell by name.
 */
int song_lookup( const char *name )
{
    int sn;

    last_fun( "song_lookup");

    for ( sn = 0; sn < MAX_SONG; sn++ )
    {
	if ( !songs_table[sn].name )
	    break;
	if ( LOWER( name[0] ) == LOWER( songs_table[sn].name[0] )
	    && !str_prefix( name, songs_table[sn].name ) )
	    return sn;
    }

    return -1;
}

void say_song( CHAR_DATA *ch, int sn )
{
    return;
}

/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
                        CHAR_DATA *rch;
			char      *pName;
			char       buf       [ MAX_STRING_LENGTH ];
			char       buf2      [ MAX_STRING_LENGTH ];
			int        iSyl;
			int        length;

	       	 struct syl_type
	         {
		        char *	   old;
		        char *	   new;
		 };

    static const struct syl_type   syl_table [ ] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    last_fun( "say_spell");

    buf[0]	= '\0';
    for ( pName = spells_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0;
	     ( length = strlen( syl_table[iSyl].old ) ) != 0;
	     iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n&n utters the words, '%s'.", buf );
    sprintf( buf,  "$n&n utters the words, '%s'.", spells_table[sn].name );

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
        if( rch->fly_level != ch->fly_level || rch->deleted )
            continue;
	if ( rch != ch && (( rch->class == ch->class) || IS_IMMORTAL( ch ) || is_affected( ch, AFF_COMP_LANG )))
        {
	    act( buf, ch, NULL, rch, TO_VICT );
        }
        else if( rch != ch )
        {
	    act( buf2, ch, NULL, rch, TO_VICT );
        }
    }

    return;
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
// Veygoth - tweaked the math a bit
bool saves_spell( int level, CHAR_DATA *victim, int dam_type )
{
    int save;
    int base = 50;
    char buf[MAX_STRING_LENGTH];
    char lfbuf[400];

    sprintf( lfbuf, "saves_spell: dt: %d, level: %d", dam_type, level );
    last_fun( lfbuf );

//    if( !victim || victim->deleted )
//  The above keeps the mud alive, but makes the SSM real mad!  Too tired
//to figure it out.  Check log 1394.log for details. - Lohrr
    if( !victim )
    {
        bug( "Saves_spell called without CHAR_DATA argument for victim.", 0 );
        return FALSE;
    }
    if ( victim->deleted ) {
        bug( "Saves_spell called for victim that is deleted.", 0 );
        return FALSE;
    }

    sprintf(buf, "saves_spell: %s", victim->name );
    last_fun( buf );

    if( is_affected( victim, AFF_DENY_WATER ) && dam_type == DAM_WATER )
        return TRUE;
    if( is_affected( victim, AFF_DENY_FIRE ) && dam_type == DAM_FIRE )
        return TRUE;
    if( is_affected( victim, AFF_DENY_EARTH ) && dam_type == DAM_EARTH )
        return TRUE;
    if( is_affected( victim, AFF_DENY_AIR ) && dam_type == DAM_WIND )
        return TRUE;

    // Keep in mind that *negative* saving throw improves the chance.
    // positive saving throw is a bad thing - Veygoth
    /* Thus, we want a - save to increase the save chance, not decrease
     * it.  So, we subtract the saving throw.  - Lohrr
     */
    save = base + ( victim->level - level - victim->saving_throw[4] ) * 2;

    if( IS_NPC( victim ) && victim->level > 55 )
        base += 20;

    switch( check_ris( victim, dam_type ) )
    {
    case IS_RESISTANT:		save += 12;		break;
    case IS_IMMUNE:		return TRUE;
    case IS_SUSCEPTIBLE:	save -= 12;		break;
    }

    /* Note that protection spells aren't quite as good as a natural resistance
     * ( +10% save -25% damage as opposed to +12% save -33% damage), but they
     * are cumulative, so a natural resistance and a protection spell will give
     * +22% save and -50% damage overall.
     *
     * -- Veygoth
     */

    if( dam_type == DAM_FIRE && is_affected( victim, AFF_PROTECT_FIRE ))
        save += 10;
    else if( dam_type == DAM_COLD && is_affected( victim, AFF_PROTECT_COLD ))
        save += 10;
    else if( dam_type == DAM_GAS && is_affected( victim, AFF_PROTECT_GAS ))
        save += 10;
    else if( dam_type == DAM_ACID && is_affected( victim, AFF_PROTECT_ACID ))
        save += 10;
    else if( dam_type == DAM_ELECTRICITY && is_affected( victim, AFF_PROTECT_LIGHTNING ))
        save += 10;

    save = URANGE( 5, save, 95 );
    if ( number_percent( ) < save )
        return TRUE;
    else
        return FALSE;
}

/*
 * Compute a saving throw versus breath weapon (saving_throw[3]).
 * Negative apply's make saving throw better.
 *
 * This is basically identical to saves_spell, except that it uses
 * a different saving throw as its base.
 */
bool saves_breath( int level, CHAR_DATA *victim, int dam_type )
{
    int save;
    int base = 50;
    char buf[MAX_STRING_LENGTH];
    char lfbuf[400];

    sprintf( lfbuf, "saves_spell: dt: %d, level: %d", dam_type, level );
    last_fun( lfbuf );

    if( !victim )
    {
        bug( "Saves_breath called without CHAR_DATA argument for victim.", 0 );
        return FALSE;
    }
    if ( victim->deleted )
    {
        bug( "Saves_breath called for victim that is deleted.", 0 );
        return FALSE;
    }

    sprintf(buf, "saves_breath: %s", victim->name );
    last_fun( buf );

    save = base + ( victim->level - level - victim->saving_throw[3] ) * 2;

    switch( check_ris( victim, dam_type ) )
    {
      case IS_RESISTANT:	save += 12;		break;
      case IS_IMMUNE:		return TRUE;
      case IS_SUSCEPTIBLE:	save -= 12;		break;
    }

    /* Note that protection spells aren't quite as good as a natural resistance
     * ( +10% save -25% damage as opposed to +12% save -33% damage), but they
     * are cumulative, so a natural resistance and a protection spell will give
     * +22% save and -50% damage overall.
     *
     * -- Veygoth
     */

    if( dam_type == DAM_FIRE && is_affected( victim, AFF_PROTECT_FIRE ))
        save += 10;
    else if( dam_type == DAM_COLD && is_affected( victim, AFF_PROTECT_COLD ))
        save += 10;
    else if( dam_type == DAM_GAS && is_affected( victim, AFF_PROTECT_GAS ))
        save += 10;
    else if( dam_type == DAM_ACID && is_affected( victim, AFF_PROTECT_ACID ))
        save += 10;
    else if( dam_type == DAM_ELECTRICITY && is_affected( victim, AFF_PROTECT_LIGHTNING ))
        save += 10;

    save = URANGE( 5, save, 95 );
    if ( number_percent( ) < save )
        return TRUE;
    else
        return FALSE;
}

// Save against illusionist spells, intelligence-based
bool disbelieve( int level, CHAR_DATA *victim, CHAR_DATA *ch )
{
    int save;
    int base = 50;

    last_fun( "disbelieve");

    if( !victim )
    {
        bug( "disbelieve called without CHAR_DATA argument for victim.", 0 );
        return FALSE;
    }

    // Level has more effect on illusions.
    save = base + ( victim->level - level ) * 4;

    // Will cause a 110 int player to get a 33% bonus against a 10 int mob.
    save += (get_curr_int( victim ) / 3 );
    save -= (get_curr_int( ch ) / 3);

    /* Extra 15% bonus for exp mobs. */
    if ( IS_NPC( victim ) && victim->level - ch->level < 5 )
        save -= 15;

    /* Note that protection spells are not factored in at all and an
     * illusion save gets no bonuses other than intelligence
     * -- Veygoth
     */

    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}

int area_ok(CHAR_DATA *ch, CHAR_DATA *tch) {
  if (tch->deleted || is_same_group(tch, ch)  || ch == tch)
    return FALSE;
  if (tch->fly_level != ch->fly_level)
    return FALSE;
  return TRUE;
}

CHAR_DATA *find_first_area_target(CHAR_DATA *ch, int num_targets) {
  CHAR_DATA *tch;
  int num_in_room = 0;
  int temp = 0;
  int first_target = 0;

  for (tch = ch->in_room->people; tch; tch = tch->next_in_room)
     if (area_ok(ch, tch))
       num_in_room ++;
  if (num_targets < num_in_room) 
     first_target = dice(1, num_in_room - num_targets);
  else first_target = dice(1, num_in_room);
  for (tch = ch->in_room->people; tch; tch = tch->next_in_room)
     if (area_ok(ch, tch)) {
        temp++;
        if (temp == first_target)
           break;
     }
   return tch;
}

void do_area_spell_damage(CHAR_DATA *ch, int spellnum, int num_targets,
                          int damage, int damtype) {
  CHAR_DATA *tch, *next;
  int tmp_dam;
  int temp = 0;

  last_fun( "area_spell_damage");
  
  tch = find_first_area_target(ch, num_targets);
  while ((temp != num_targets) && tch) {
    next = tch->next_in_room;
    if ( !is_same_group( ch, tch ) && ch != tch 
            && ch->fly_level == tch->fly_level ) {
        temp++;
        tmp_dam = damage;
        /* handle special damage types (chain lightning, quake, etc) here */
        /* by modifying tmp_dam, or setting stuff on chars */
        if (tmp_dam > 0)
         spell_damage(ch, tch, damage, spellnum, damtype);
    } //end if
    tch = next;
  }
}

// Check proper class, and check totems for Shamans.
// -- Veygoth
void do_cast( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_cast");

    if( ch->class == CLASS_PSIONICIST && !IS_IMMORTAL(ch) )
    {
        send_to_char( "Psionicists use the WILL command to invoke their powers.\n\r", ch );
        return;
    }

    if( ch->class == CLASS_BARD && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "Bards use the SING or PLAY commands to invoke their powers.\n\r", ch );
        return;
    }

    if ( ch->riding && ch->in_room == ch->riding->in_room ) {
        send_to_char( "You cannot cast while mounted!\n\r", ch );
        return;
    }

    if( is_affected( ch, AFF_MINOR_PARA ) ||
        is_affected( ch, AFF_HOLD ))
    {
        send_to_char( "You can't cast when you're paralyzed!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Cast which what where?\n\r", ch );
	return;
    }

    cast( ch, argument );
}

// By all rights psionics should be instantaneous and exempt from
// the casting stuff, and suffer lag afterward - Veygoth
void do_will( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_will");

    if( ch->class != CLASS_PSIONICIST  && !IS_IMMORTAL(ch) )
    {
        send_to_char( "Your mind is much too puny for you to focus your will.\n\r", ch );
        return;
    }

    if ( ch->riding && ch->riding->in_room == ch->in_room ) {
        send_to_char( "It's too hard to concentrate! Dismount.\n\r", ch );
        return;
    }

    // Psis should be able to will stuff when paralyzed, thus no check for para.

    if ( argument[0] == '\0' )
    {
	send_to_char( "Will which what where?\n\r", ch );
	return;
    }

    cast( ch, argument );
}

/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim,
		    OBJ_DATA *obj )
{
    void *vo;

    last_fun( "obj_cast_spell");

    if ( sn <= 0 )
	return;

    if ( sn >= MAX_SPELL || spells_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    switch ( spells_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
        vo = NULL;
	break;

    case TAR_TRAP:
        return;

    case TAR_CHAR_OFFENSIVE:
	if ( !victim )
	    victim = ch->fighting;
	if ( !victim )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;

          check_killer( ch, victim );

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( !victim )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( !obj )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;

    case TAR_OBJ_HERE:
	if ( !obj )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;

    case TAR_OBJ_NECRO: /* Might be able to pass "type" in level Govno */
        vo = (void *) obj;
	break;
    }

    (*spells_table[sn].spell_fun) ( sn, level, ch, vo );

    if ( spells_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && ch != victim )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		kill_ch( victim, ch );
		break;
	    }
	}
    }

    return;
}

/* pray for me, father, for I have sinned. Govno */
void do_pray( CHAR_DATA *ch, char *argument )
{
	  bool lesser; // Do they use the lesser mem chart?

    last_fun( "do_pray");

        // Mobiles cannot mem spells
        if( IS_NPC( ch ))
           return;

        // Check to see if they are allowed to mem anything
        switch( ch->class )
        {
           default:
             send_to_char( "You are not a spellcaster!\n\r", ch );
             return;
             break;
           case CLASS_PALADIN:
           case CLASS_ANTIPALADIN:
             lesser = TRUE;
             break;
           case CLASS_CLERIC:
           case CLASS_DRUID:
             lesser = FALSE;
             break;
           case CLASS_HUNTER:
	   case CLASS_SHAMAN:
           case CLASS_ELEMENT_AIR:
           case CLASS_ELEMENT_FIRE:
           case CLASS_ELEMENT_WATER:
           case CLASS_ELEMENT_EARTH:
           case CLASS_ILLUSIONIST:
	   case CLASS_SORCERER:
           case CLASS_RANGER:
	     send_to_char( "You memorize your spells, not pray for them.\n\r", ch );
	     return;
	     break;
	}

	memorize( ch, argument, TRUE, lesser );
}

void do_memorize( CHAR_DATA *ch, char *argument )
{
	  bool lesser; // Do they use the lesser mem chart?

    last_fun( "do_memorize");

	  // Mobiles cannot mem spells
        if( IS_NPC( ch ))
           return;

        // Check to see if they are allowed to mem anything
        switch( ch->class )
        {
           default:
             send_to_char( "You are not a spellcaster!\n\r", ch );
             return;
             break;
           case CLASS_CLERIC:
  	     case CLASS_DRUID:
           case CLASS_PALADIN:
           case CLASS_ANTIPALADIN:
	     send_to_char( "You must pray for your spells!\n\r", ch );
             return;
	     break;
           case CLASS_SHAMAN:
           case CLASS_ELEMENT_AIR:
           case CLASS_ELEMENT_FIRE:
           case CLASS_ELEMENT_WATER:
           case CLASS_ELEMENT_EARTH:
           case CLASS_ILLUSIONIST:
           case CLASS_SORCERER:
             lesser = FALSE;
             break;
           case CLASS_HUNTER:
           case CLASS_RANGER:
             lesser = TRUE;
             break;
        }

	memorize( ch, argument, FALSE, lesser );
}

// This is called by do_memorize and do_pray.  This was originally
// attached to each function, but it's silly to have two large blocks
// of the same code
// -- Veygoth
void memorize( CHAR_DATA *ch, char *argument, bool pray, bool lesser )
{

        MEM_DATA *mem = NULL;
        int sn;
        char buf[MAX_STRING_LENGTH];
        char buf2[MAX_STRING_LENGTH];
        int memmed[MAX_SPELL];
        int count;
        int total_mem;
        bool found;
	  bool left;
        int circle[MAX_CIRCLE], circle_index;
        int circfree[MAX_CIRCLE];
        int lvltotal;

        last_fun( "memorize");

        // with an argument they want to start memorizing a new spell.
        if( argument[0] != '\0' )
        {
           // Must be in the proper position
           if( ch->position != POS_RESTING )
           {
             if ( pray )
               send_to_char( "You can only pray for spells while resting.\n\r", ch );
             else
               send_to_char( "You can memorize spells only when resting.\n\r", ch );
             return;
           }

           // Find the spell they want
           sn = spell_lookup( argument );
           if( sn <= 0 )
           {
             send_to_char( "Never heard of that spell...\n\r", ch );
             return;
           }

           // Check to see that they can memorize another spell
           // Immortals have no limits
           if ( !IS_IMMORTAL( ch ) )
           {

             if( ch->pcdata->condition[COND_FULL] <= 0 
                 || ch->pcdata->condition[COND_THIRST] <= 0 )
             {
               send_to_char( "You can't seem to concentrate on anything but your appetite.\n\r", ch );
             }

             if( ch->level < (spells_table[sn].spell_circle[ch->class] * 5 - 4))
             {
               send_to_char( "That spell is beyond you.\n\r", ch );
               return;
             }

             if( !pray && ch->pcdata->spl_lrn[sn] < 1 )
             {
               send_to_char( "You have not yet learned that spell.  Find a place to scribe it.\n\r", ch );
               return;
             }

             lvltotal = 0;
             for( mem = ch->pcdata->memorized; mem; mem = mem->next )
             {
               if( mem->circle == spells_table[sn].spell_circle[ch->class])
                  lvltotal += 1;
             }
             if( lvltotal >= memchart[(ch->level - 1)][(spells_table[sn].spell_circle[ch->class] - 1)])
             {
		   if( pray )
                 send_to_char( "You can pray for no more spells of that level.\n\r", ch );
               else
                 send_to_char( "You can memorize no more spells of that circle.\n\r", ch );
               return;
             }
           } // if( !IS_IMMORTAL( ch ))

           // If we know what they want and they can have it, let's create it.
           if(!(mem = (create_memdata( ch, sn ))))
           {
             bug( "Unable to create memorization (sn %d)", sn );
             return;
           }

           // If they're not already memorizing, they are now.. mebbe
           SET_BIT( ch->act, PLR_MEMORIZING );

           if( pray )
             sprintf( buf, "You start praying for %s which will take about %d seconds.\n\r",
             spells_table[mem->sn].name, (mem->memtime / PULSE_PER_SECOND) );
           else
             sprintf( buf, "You start memorizing %s which will take about %d seconds.\n\r",
             spells_table[mem->sn].name, (mem->memtime / PULSE_PER_SECOND) );
           send_to_char( buf, ch );

           return;           
        }
        else
        {
           found = FALSE;
           // If they didn't give us an argument, that means that they must
           // want to either see their spell list or continue memorizing
           // Either way we show their spell list.
           // make sure they have some mem data first...
           if( ch->pcdata->memorized )
           {
           for( count = 0; count < MAX_SPELL; count ++ )
             memmed[count] = 0;
           // Figure out what spells they have memorized
           for( mem = ch->pcdata->memorized; mem; mem = mem->next )
           {
             if( mem->memmed )
                 memmed[mem->sn] += 1;
           }

           // Show memorized spells
	   if ( pray )
              send_to_char( "You have prayed the following spells:\n\r", ch );
	   else
              send_to_char( "You have memorized the following spells:\n\r", ch );
           for(circle_index = 12; circle_index > 0; circle_index--) {
           for( count = 0; count < MAX_SPELL; count++ )
           {
              if ( spells_table[count].spell_circle[ch->class] != circle_index)
                continue;
              if( memmed[count] > 0 && spells_table[count].name != NULL)
              {
                sprintf( buf, "(%2d%s circle)  %d - %s\n\r",
                   spells_table[count].spell_circle[ch->class],
                   text_number( spells_table[count].spell_circle[ch->class] ),
                   memmed[count],
                   spells_table[count].name );
                send_to_char( buf, ch );
              }
           }
           } //end for(circle)

           // Figure out what spells they are working on
	   if ( pray )
              send_to_char( "You are praying for the following spells:\n\r", ch );
	   else
              send_to_char( "You are memorizing the following spells:\n\r", ch );
           total_mem = 0;
            for( mem = ch->pcdata->memorized_last; mem; mem = mem->prev )
            {
              if( !spells_table[mem->sn].name )
                  break;
              if( mem->memmed )
                  continue;
              else
                  found = TRUE;
              sprintf( buf, "    %d seconds:  (%d%s) %s\n\r",  
                  ((total_mem + mem->memtime) / PULSE_PER_SECOND),
                  spells_table[mem->sn].spell_circle[ch->class],
                  text_number(spells_table[mem->sn].spell_circle[ch->class]),
                  spells_table[mem->sn].name );
              send_to_char( buf, ch );
              total_mem += mem->memtime;
            }
           } // if( ch->pcdata->memorized )

//
           // Tell them what they still have slots open for...
           for( count = 0; count < MAX_CIRCLE; count++ )
                circle[count] = 0;
             for( mem = ch->pcdata->memorized; mem; mem = mem->next )
             {
                  circle[(mem->circle - 1)] += 1;
             }
             left = FALSE;
             for( count = 0; count < MAX_CIRCLE; count++ )
             {
                  circfree[count] = memchart[(ch->level - 1)][count]
                                  - circle[count];
                  if( circfree[count] > 0 )
                      left = TRUE;
             }
             if( !left )
             {
              if( pray )
                 send_to_char( "\n\rYou can pray for no more spells.\n\r", ch );
              else
                 send_to_char( "\n\rYou can memorize no more spells.\n\r", ch );
             }
             else
             {
               if( pray )
                 sprintf( buf, "\n\rYou can pray for" );
               else
                 sprintf( buf, "\n\rYou can memorize" );
               for( count = 0; count < MAX_CIRCLE; count++ )
               {
                 if( circfree[count] > 0 )
                 {
                   sprintf( buf2, " %d-%d%s", circfree[count], (count + 1), text_number( count + 1 ) );
                   strcat( buf, buf2 );
                 }
               }
               strcat( buf, " level spells.\n\r" );
               send_to_char( buf, ch );
             }

           // If they aren't memming and they should be, start 'em up.
           if( found && !IS_SET( ch->act, PLR_MEMORIZING ) 
               && ch->position == POS_RESTING )
           {
             SET_BIT( ch->act, PLR_MEMORIZING );
             if( ch->pcdata->condition[COND_FULL] > 0 
                 && ch->pcdata->condition[COND_THIRST] > 0 )
             {
                 if( pray )
                     send_to_char( "You continue your prayers.\n\r", ch );
                 else
                     send_to_char( "You continue your studies.\n\r", ch );
             }
           }


        }
        return;
}

void do_forget( CHAR_DATA *ch, char *argument )
{
        MEM_DATA *mem;
        MEM_DATA *prev;
        int sn;
        bool found;
        char buf[MAX_STRING_LENGTH];

        last_fun( "do_forget");

        if( IS_NPC( ch ))
            return;

        if( argument[0] == '\0' )
        {
            send_to_char( "Looks like your forgot what you were going to forget.\n\r", ch );
            return;
        }

        if( !ch->pcdata->memorized )
        {
            send_to_char( "You don't have anything to forget.\n\r", ch );
            return;
        }

        if( !str_cmp( argument, "all" ))
        {
            forget_all( ch );
            send_to_char( "You forget everything.\n\r", ch );
            return;
        }

        sn = spell_lookup( argument );
        if( sn <= 0 )
        {
            send_to_char( "Forget what?\n\r", ch );
            return;
        }

        found = FALSE;
        prev = NULL;
        for( mem = ch->pcdata->memorized; mem; mem = mem->next )
        {
            if( mem->sn == sn )
            {
               found = TRUE;
               break;
            }
            prev = mem;
        }

        if( !found )
        {
            send_to_char( "You do not have that spell memorized.\n\r", ch );
            return;
        }

        // This is where we remove the spell from their double-linked
        // list and tack it onto the free list.  Too bad I don't know how
        // yet -- Veygoth
        sprintf( buf, "You forget %s.\n\r", spells_table[sn].name );
        send_to_char( buf, ch );

        // Put the data on the free stack.
        // Reset memorized_last since that doesen't affect much of anything
        if( mem == ch->pcdata->memorized_last )
        {
            if( mem->prev )
                ch->pcdata->memorized_last = mem->prev;
            else
                ch->pcdata->memorized_last = NULL;
        }

        // repoint the previous data to the next data if any
        if( prev )
        {
            prev->next = mem->next;
        }
        else if( ch->pcdata->memorized == mem )
        {
            ch->pcdata->memorized = mem->next;
        }
        else
        {
            bug( "Previous data not found and mem data not at top of list", 0 );
        }

        // repoint the next memorization data to the right place
        if( mem->next )
        {
          if( prev )
             mem->next->prev = prev;
          else
             mem->next->prev = NULL;
        }

        // put it on the free stack
        mem->next = memdata_free;
        memdata_free = mem;

        // reset the previous pointer
        mem->prev = NULL;
        return;
}

// Should hard code level checks to see if it is okay to mem a certain
// spell - Veygoth
MEM_DATA *create_memdata( CHAR_DATA *ch, int sn )
{
	MEM_DATA *mem;

        last_fun( "create_memdata");

        if( IS_NPC( ch ))
          return FALSE;

        if( !memdata_free )
        {
	   mem = alloc_perm( sizeof( *mem ));
           top_memdata++;
        }
        else
        {
           mem = memdata_free;
           memdata_free = memdata_free->next;
        }

        memset( mem, 0, sizeof( MEM_DATA ));

        mem->sn = sn;
        mem->memtime =		calc_memtime( ch, sn );
        mem->full_memtime =     calc_memtime( ch, sn );
        mem->circle = spells_table[sn].spell_circle[ch->class];
        mem->memmed = FALSE;

        // Point the new data to all the rest of the list.
        // The prev pointer is reset when the data is actually
        // used so we don't have to worry about it in the forget
        // command.
        if( ch->pcdata->memorized )
            ch->pcdata->memorized->prev = mem;
        mem->next = ch->pcdata->memorized;
        mem->prev = NULL;
        if( !ch->pcdata->memorized_last )
             ch->pcdata->memorized_last = mem;

        // Put the new mem data at the top of the list.
        ch->pcdata->memorized = mem;

	return mem;
}

int calc_memtime( CHAR_DATA *ch, int sn )
{
        int memtime;
        int attribute;

        last_fun( "calc_memtime");

        if( ch->class == CLASS_CLERIC || ch->class == CLASS_DRUID
              || ch->class == CLASS_PALADIN || ch->class == CLASS_ANTIPALADIN )
            attribute = get_curr_wis( ch );
        else if( ch->class == CLASS_SHAMAN )
            attribute = (get_curr_int( ch ) + get_curr_wis( ch )) / 2;
        else
            attribute = get_curr_int( ch );

        memtime = 220 - attribute - (ch->level * 3)
                + (spells_table[sn].spell_circle[ch->class] * 8);
        if( memtime < 4 )
            memtime = 4;
        return memtime;
}

void memorize_update( void )
{
        MEM_DATA *mem;
        MEM_DATA *chkspl;
        CHAR_DATA *ch;
        bool done;

        last_fun( "memorize_update");

        for( ch = char_list; ch; ch = ch->next )
        {
           done = FALSE;

           if( IS_NPC( ch ) || ch->deleted )
             continue;

           if( !(IS_SET( ch->act, PLR_MEMORIZING )))
             continue;

           if( ch->pcdata->condition[COND_FULL  ] <= 0 )
           {
             send_to_char( "&+rYou are too hungry to concentrate.&n\n\r", ch );
             continue;
           }
           if( ch->pcdata->condition[COND_THIRST] <= 0 )
           {
             send_to_char( "&+rYou are too thirsty to concentrate.&n\n\r", ch );
             continue;
           }

           if( !ch->pcdata->memorized )
           {
             bug( "Memorizing character with no mem_data", 0 );
             REMOVE_BIT( ch->act, PLR_MEMORIZING );
           }

           // Find the oldest unmemmed piece of spell data
           for( mem = ch->pcdata->memorized_last; mem; mem = mem->prev )
           {
              if( !mem->memmed )
              {
                  chkspl = mem;
                  break;
              }
           }


         if( !chkspl )
         {
	     REMOVE_BIT( ch->act, PLR_MEDITATING );
             REMOVE_BIT( ch->act, PLR_MEMORIZING );
             send_to_char( "Your studies are complete...", ch );
             continue;
         }
         else
         {
           if( chkspl->memtime < 1 )
           {
               char buf[MAX_STRING_LENGTH];
               chkspl->memmed = TRUE;
               sprintf( buf, "You have finished memorizing %s.\n\r",
                        spells_table[chkspl->sn].name );
               send_to_char( buf, ch );
           }
           else
           {
               if( IS_SET( ch->act, PLR_MEDITATING ))
                  if( ( chkspl->memtime
                      >= (chkspl->full_memtime / 2)) 
                      && (( chkspl->memtime - PULSE_MEMORIZE)
                       < ( chkspl->full_memtime / 2 )))
               {
                  if( number_percent() < ch->pcdata->skl_lrn[gsn_meditate] )
                  {
                     char buf[MAX_STRING_LENGTH];
                     chkspl->memmed = TRUE;
                     sprintf( buf, "You have finished memorizing %s.\n\r",
                              spells_table[chkspl->sn].name );
                     send_to_char( buf, ch );
                  }
               }
               chkspl->memtime -= PULSE_MEMORIZE;
           } // if( chkspl->memtime < 1 )
         } // if( chkspl )

         done = TRUE;
         for( mem = ch->pcdata->memorized; mem; mem = mem->next )
         {
              if( !mem->memmed )
              {
                  done = FALSE;
                  break;
              }
         }

         if( (done) && (IS_SET( ch->act, PLR_MEMORIZING )))
         {
	    REMOVE_BIT( ch->act, PLR_MEDITATING );
            REMOVE_BIT( ch->act, PLR_MEMORIZING );
            send_to_char( "Your studies are complete.\n\r", ch );
            act( "$n&n is finished memorizing.", ch, NULL, NULL, TO_ROOM );
         }
        }

        return;
}

void do_meditate( CHAR_DATA *ch, char *argument )
{
        if( IS_NPC( ch ))
           return;

        last_fun( "do_meditate");

        if( ch->level < skills_table[gsn_meditate].skill_level[ch->class] )
        {
           send_to_char( "You don't know how to meditate.\n\r", ch );
           return;
        }

        if( IS_SET( ch->act, PLR_MEDITATING ))
        {
           REMOVE_BIT( ch->act, PLR_MEDITATING );
           send_to_char( "You stop meditating.\n\r", ch );
        }

        if( ch->position != POS_RESTING )
        {
           send_to_char( "You must be resting in order to meditate.\n\r", ch );
           return;
        }

        if( ch->fighting )
        {
           send_to_char( "Meditation during battle leads to permenant inner peace.\n\r", ch );
           return;
        }

        SET_BIT( ch->act, PLR_MEDITATING );
        WAIT_STATE( ch, skills_table[gsn_meditate].beats );
        skill_practice( ch, gsn_meditate );
        send_to_char( "You start meditating...\n\r", ch );
        return;

}

// Used for both death and for 'forget all'
void forget_all( CHAR_DATA *ch )
{
       MEM_DATA *mem;

       last_fun( "forget_all");

       if( IS_NPC( ch ))
         return;

       if( !ch->pcdata->memorized )
         return;

       // No clue whether this works, but we yank the whole
       // data chain from the character and place it on the
       // free stack - Veygoth
       for( mem = ch->pcdata->memorized; mem; mem = mem->next )
       {
          mem->prev = NULL;
          if( mem->next == NULL )
              break;
       }
       mem->next = memdata_free;
       memdata_free = ch->pcdata->memorized;
       ch->pcdata->memorized = NULL;
       ch->pcdata->memorized_last = NULL;

       return;
}

void do_scribe( CHAR_DATA *ch, char *argument )
{
       CHAR_DATA *teacher;
       OBJ_DATA *quill;
       char buf[MAX_STRING_LENGTH];
       int sn;

       last_fun( "do_scribe");

       if( IS_NPC( ch ))
           return;

       for( teacher = ch->in_room->people; teacher; teacher = teacher->next_in_room )
       {
           if( !IS_NPC( teacher ))
               continue;
           if( IS_SET( teacher->act, ACT_TEACHER ))
               break;
       }

       if( !teacher )
       {
           send_to_char( "Nobody here can teach you anything.\n\r", ch );
           return;
       }

       for( quill = ch->carrying; quill; quill = quill->next_content )
       {
           if( quill->item_type == TYPE_PEN )
               break;
       }

       if( !quill )
       {
         send_to_char( "You have nothing to write with!\n\r", ch );
         return;
       }

       if( argument[0] == '\0' )
       {
          send_to_char( "Scribe what?\n\r", ch );
          return;
       }

       sn = spell_lookup( argument );

       if( sn <= 0 )
       {
           send_to_char( "No such spell.\n\r", ch );
           return;
       }

       if( (spells_table[sn].spell_circle[teacher->class] * 5) >
            (teacher->level + 4) ) 
       {
           send_to_char( "The teacher does not know that spell.\n\r", ch );
           return;
       }

       if( !spells_table[sn].can_scribe )
       {
           send_to_char( "That spell is not common knowledge - You must quest for it.\n\r", ch );
           return;
       }

       if( (spells_table[sn].spell_circle[ch->class] * 5) > (ch->level + 4) ) 
       {
           send_to_char( "That spell is beyond you.\n\r", ch );
           return;
       }

       if( ch->pcdata->spl_lrn[sn] < 1 )
       {
           // Scribe is used so rarely give them 5 chances to learn it...
           skill_practice( ch, gsn_scribe );
           skill_practice( ch, gsn_scribe );
           skill_practice( ch, gsn_scribe );
           skill_practice( ch, gsn_scribe );
           skill_practice( ch, gsn_scribe );
           sprintf( buf, "You scribe %s.\n\r", spells_table[sn].name );
           send_to_char( buf, ch );
           ch->pcdata->spl_lrn[sn] = BASE_SPELL_ADEPT;
           if( number_percent() < ch->pcdata->skl_lrn[gsn_scribe] )
              ch->pcdata->spl_lrn[sn] += 5;
           if( number_percent() < ch->pcdata->skl_lrn[gsn_scribe] )
              ch->pcdata->spl_lrn[sn] += 4;
           if( number_percent() < ch->pcdata->skl_lrn[gsn_scribe] )
              ch->pcdata->spl_lrn[sn] += 3;
           if( number_percent() < ch->pcdata->skl_lrn[gsn_scribe] )
              ch->pcdata->spl_lrn[sn] += 2;
           if( number_percent() < ch->pcdata->skl_lrn[gsn_scribe] )
              ch->pcdata->spl_lrn[sn] += 1;
           WAIT_STATE( ch, skills_table[gsn_scribe].beats );
       }
       else
       {
           send_to_char( "You already know that spell.\n\r", ch );
       }

       return;
}

// For bards with instruments
void do_play( CHAR_DATA *ch, char *argument )
{
      do_sing( ch, argument );
}

// For bards without instruments
void do_sing( CHAR_DATA *ch, char *argument )
{
    last_fun( "do_sing");
 
    if( ch->class != CLASS_BARD )
    {
        send_to_char( "You don't know how to sing\n\r", ch );
        return;
    }

    if( is_affected( ch, AFF_MINOR_PARA ) ||
        is_affected( ch, AFF_HOLD ))
    {
        send_to_char( "You can't sing when you're paralyzed!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        if( is_affected( ch, AFF_SINGING ))
        {
            REMOVE_AFF_BIT( ch, AFF_SINGING );
            send_to_char( "You stop singing.\n\r", ch );
        }
        else
            send_to_char( "Sing what?\n\r", ch );
	return;
    }

    cast( ch, argument );
}

// When the spell/song event terminates, we need something to happen...
//
// By this point we should have terminated the spell/song event data
// and should only need the info about the character and the sn
// and maybe the argument...
//
// Passing of the correct function parameters should be handled by the
// event system.
void finish_spell( CHAR_DATA *ch, int sn, void *vo, bool song )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    int        chance, dir;
    MEM_DATA  *mem;
    bool found = FALSE;
    bool target_in_range = FALSE;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
//    CHAR_DATA *vch2;

    CAST_DATA *caster;
    CAST_DATA *prev = NULL;
    CAST_DATA *castnext = NULL;

    sprintf( lbuf, "finish_spell: %s by %s", spells_table[sn].name, ch->name );
    last_fun( lbuf );

    // Free up the person from the caster list.
    for( caster = cast_list; caster; caster = castnext )
    {
      castnext = caster->next;
      if( caster->who && caster->who == ch )
      {
        if( cast_list == caster )
          cast_list = caster->next;
        if( prev )
          prev->next = caster->next;
        free_mem( caster, sizeof( CAST_DATA ));
      }
      else
        prev = caster;
    }

    // If they're not casting/singing at the end of the song or spell
    // they certainly can't finish it.
    if( !song )
    {
      if( is_affected( ch, AFF_CASTING ))
        REMOVE_AFF_BIT( ch, AFF_CASTING );
      else
      {
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
        return;
      }
    }
    else
    {
      if( is_affected( ch, AFF_SINGING ))
        REMOVE_AFF_BIT( ch, AFF_SINGING );
      else
      {
        if ( songs_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( songs_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
        return;
      }
    }

    // Bail if they're casting a spell/singing a tune that requires concentration
    if ( !song && ch->position == POS_FIGHTING && !(spells_table[sn].cast_combat) )
    {
	send_to_char( "You can't concentrate enough.\n\r", ch );
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
	return;
    }

    // Bail if they got bashed
    else if( ch->position < POS_FIGHTING )
    {
      if( song )
        send_to_char( "You must be standing to sing loudly enough to be heard!\n\r", ch );
      else
        send_to_char( "You must be standing in order to cast spells!\n\r", ch );
      if ( spells_table[sn].target == TAR_OBJ_CHAR )
          free( (OBJ_CHAR *) vo );
      if ( spells_table[sn].target == TAR_IGNORE )
          free_string( (char *) vo );
      return;
    }

    // We checked for all this stuff before, but the state of things may
    // have changed.
    if( (   is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE ) )
        && ch->class != CLASS_PSIONICIST )
    {
	send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
	return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "You can't...you are in a Cone of Silence!\n\r", ch );
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
        return;
    }

    if( !song && IS_ROOM( ch->in_room, ROOM_NO_MAGIC ))
    {
        send_to_char( "After a brief gathering of energy, your spell fizzles!\n\r", ch );
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
        return;
    }
    if( ch->class != CLASS_BARD )
    {
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+lThe &+yearth&n &+lcomes up &+yand engulfs &+lyour spell.\n\r", ch );
        spell_damage( ch, ch, 1, spl_earthen_starshell, DAM_FIRE );
        WAIT_STATE( ch, 6 );
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+CAir swir&n&+cls a&+Cnd absorbs y&n&+cour spell.&n\n\r", ch );
        WAIT_STATE( ch, 6 );
        if( ch->position > POS_RECLINING && number_percent() + 50 > get_curr_agi( ch ) )
        {
            ch->position = POS_RECLINING;
            WAIT_STATE( ch, 6 );
            send_to_char( "You are knocked over!\n\r", ch);
        }
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+bWater b&+Bursts up a&n&+bnd absor&+Bbs your spell.&n\n\r", ch );
        WAIT_STATE( ch, 6 );
        ch->move -= 20;
        send_to_char( "You feel tired!\n\r", ch);
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+RFire&n&+r engu&+Rlfs y&n&+rour s&+Rpell.&n\n\r", ch );
        spell_damage( ch, ch, 1, spl_fiery_starshell, DAM_FIRE );
        WAIT_STATE( ch, 6 );
        return;
    }
    }

    if( !song && !IS_NPC( ch ) && !IS_IMMORTAL( ch ))
    {
      for( mem = ch->pcdata->memorized; mem; mem = mem->next )
      {
        if( !mem->memmed )
          continue;
        if( mem->sn == sn )
        {
          found = TRUE;
          break;
        }
      }

      if( !found && !IS_NPC(ch) )
      {
         send_to_char( "You do not have that spell memorized!\n\r", ch );
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
         return;
      }
    }

    if ( has_affect(ch, 0, spl_feeblemind, 0) ) {
        send_to_char ("You are just too stupid to cast that spell!\n\r", ch);
        act("$n&n screws up $s face in concentration.", ch, NULL, NULL, TO_ROOM);
        act("$n&n tries really, really hard to complete a spell, but fails.", ch, NULL, NULL, TO_ROOM);
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
        return;
    }

    /*
     * Locate targets.
     */
    victim	= NULL;
      
  if( !song )
  {
    switch ( spells_table[sn].target )
    {
    default:
	bug( "Finish_spell: bad target for sn %d.", sn );
	return;

    case TAR_OBJ_CHAR:
        if (is_affected(ch, AFF_BLIND) ) 
        {
            send_to_char("You cannot see to cast that spell.\n\r", ch);
            free( (OBJ_CHAR *)vo );
            return;
        }
        break;
    case TAR_IGNORE:
	break;

    case TAR_TRAP:
        send_to_char( "You cannot cast a trap!\n\r", ch );
        return;

    case TAR_CHAR_OFFENSIVE:
        victim = (CHAR_DATA *) vo;

        if (is_affected(ch, AFF_BLIND)) {
            //allow casting if in combat and no target specified
            if ( !(ch->fighting && victim == ch->fighting) ) {
                send_to_char("You cannot see to cast that spell.\n\r", ch);
                return;
            }
        }
        if ( !victim ) {
            send_to_char("They aren't here.\n\r", ch);
            return;
        }
        if ( !victim->in_room || victim->in_room != ch->in_room ) {
            send_to_char( "They are not here.\n\r", ch);
            return;
        }
	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;

  	  check_killer( ch, victim );

        /* Check for globes.  This will stop any spells of type TAR_CHAR_OFFENSIVE
         * but area effect spells with type TAR_IGNORE will get through, since we
         * don't know whether they will be offensive or not.  The only thing we can
         * really do is add this same thing in the spell_damage function to prevent
         * those from getting through.  However, we must treat cases of things like
         * an area effect sleep spell as a special case within the spell_whatever
         * function in spells.c.  However, by the nature of the spell, anything
         * that is not either offensive and not direct damage, it should get through
         * just so that these spells have some weaknesses for the strategic to get
         * around.
         *
         * -- Veygoth
         */
/*
 *  Removed per Thendar's agreement... globe does not stop blind/sleep! - Lohrr
        if( is_affected( victim, AFF_MAJOR_GLOBE ) && spells_table[sn].spell_circle[ch->class] <= 6 )
        {
            act( "&+RThe globe around $N&n's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+RYour globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+R$N&+R's globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_ROOM );
            return;
        }
        if( is_affected( victim, AFF_GREATER_SPIRIT_WARD ) && spells_table[sn].spell_circle[ch->class] <= 5 )
        {
            act( "&+WThe aura around $N&n's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+WYour globe absorbs $n&+W's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+W$N&+W's aura absorbs $n&+W's attack!&n", ch, NULL, victim, TO_ROOM );
            return;
        }
        if( is_affected( victim, AFF_MINOR_GLOBE ) && spells_table[sn].spell_circle[ch->class] <= 4 )
        {
            act( "&+RThe globe around $N&n's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+RYour globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+R$N&+R's globe deflects $n&+R's attack!&n", ch, NULL, victim, TO_ROOM );
            return;
        }
        if( is_affected( victim, AFF_SPIRIT_WARD ) && spells_table[sn].spell_circle[ch->class] <= 3 )
        {
            act( "&+WThe aura around $N&n's body bears the brunt of your assault!&n", ch, NULL, victim, TO_CHAR );
            act( "&+WYour globe absorbs $n&+W's attack!&n", ch, NULL, victim, TO_VICT );
            act( "&+W$N&+W's aura absorbs $n&+W's attack!&n", ch, NULL, victim, TO_ROOM );
            return;
        }
*/
	break;
    case TAR_CHAR_WORLD:
        victim = (CHAR_DATA *)vo;

        if (is_affected(ch, AFF_BLIND) && victim != ch) {
            send_to_char("You cannot see to cast that spell.\n\r", ch);
            return;
        }
        break;


    case TAR_CHAR_DEFENSIVE:
        victim = (CHAR_DATA *)vo;

        if (is_affected(ch, AFF_BLIND) && victim != ch) {
            send_to_char("You cannot see to cast that spell.\n\r", ch);
            return;
        }
        if ( ! victim || victim->in_room != ch->in_room )
        {
		send_to_char( "They aren't here.\n\r", ch );
		return;
        }
	break;

    case TAR_CHAR_SELF:
	break;

    case TAR_OBJ_INV:
        obj = (OBJ_DATA *)vo;

        if (is_affected(ch, AFF_BLIND)) {
            send_to_char("You cannot see to cast that spell.\n\r", ch);
            return;
        }
	if ( !obj || obj->carried_by != ch )
	{
	    send_to_char( "You are not carrying that.\n\r", ch );
	    return;
	}
	break;

    case TAR_OBJ_HERE:
        obj = (OBJ_DATA *)vo;

        if (is_affected(ch, AFF_BLIND)) {
            send_to_char("You cannot see to cast that spell.\n\r", ch);
            return;
        }
	if ( !obj || ( obj->carried_by != ch && obj->in_room != ch->in_room) )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}
	break;

    case TAR_OBJ_NECRO:
        break;
    case TAR_CHAR_RANGE:
        victim = (CHAR_DATA *)vo;
        if (is_affected(ch, AFF_BLIND)) {       
            send_to_char("You cannot see to cast that spell.\n\r", ch); 
            return;     
        }       
        if ( !victim || victim->deleted
            || victim->fly_level != ch->fly_level
            || !can_see(ch, victim) ) {
            send_to_char("Your prey has disappeared.\n\r", ch);
            return;
        }
        //check that target is still within the spell range
        if (ch->in_room == victim->in_room) {
            break;
        }
        target_in_range = FALSE;
        for( dir = 0; dir < MAX_DIR; dir++ ) {
           if ( !ch->in_room->exit[dir] 
             || IS_SET( ch->in_room->exit[dir]->exit_info, EX_SECRET )
             || IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED )
             || IS_SET( ch->in_room->exit[dir]->exit_info, EX_BLOCKED )
             || IS_SET( ch->in_room->exit[dir]->exit_info, EX_WALLED ) )
             continue;
           if ( ch->in_room->exit[dir]->to_room == victim->in_room ) {
             target_in_range = TRUE;
             break;
           }
           // for fireball we check two rooms away
           if ( ch->in_room->exit[dir]->to_room &&
                  ch->in_room->exit[dir]->to_room->exit[dir]
                  && ch->in_room->exit[dir]->to_room->exit[dir]->to_room == victim->in_room ) {
               target_in_range = TRUE;
               break;
            }
        }
        if ( !target_in_range ) {
            send_to_char("They are no longer in range!\n\r", ch);
            return;
        }
            

        break;
    }
  } // if( !song )
  else // if they're bards, they should have all songs TAR_IGNORE
  {
    if( songs_table[sn].target != TAR_IGNORE )
    {
       send_to_char( "This song has buggy targeting information.  Please report this.\n\r", ch );
       bug( "Song with buggy targeting information %d", songs_table[sn].target );
       log_string( songs_table[sn].name );
       ch->wait = 0;
       REMOVE_AFF_BIT( ch, AFF_SINGING );
       return;
    }
  }

    // No wait state - we already made them wait.

    if( !song )
      spell_practice( ch, sn );

    if( song || IS_NPC( ch ) )
       chance = 85;
    else if ( ch->level >= (spells_table[sn].spell_circle[ch->class] * 5 + 1) )
       chance = 101;
    else
       chance = ch->pcdata->spl_lrn[sn];

    if ( !IS_IMMORTAL( ch ) && ( number_percent( ) > chance ) )
    {
     if( !song )
     {
	send_to_char( "You lost your concentration.\n\r", ch );
	act( "&+r$n&n&+r stops chanting abruptly.&n", ch, NULL, NULL, TO_ROOM );
     }
     else
     {
	send_to_char( "You hit a wrong note.\n\r", ch );
	act( "&+r$n&n&+r stops singing abruptly.&n", ch, NULL, NULL, TO_ROOM );
     }
    }
    else
    {
      if( song )
      {
        send_to_char( "You complete a verse of the song...\n\r", ch );
        gain_exp( ch, 1 );
        say_song( ch, sn );
      }
      else
      {
        send_to_char( "You complete your spell...\n\r", ch );
        gain_exp( ch, 1 );
    	if ( str_cmp( spells_table[sn].name, "ventriloquate" ) )
	    say_spell( ch, sn );
      }
        if( !IS_NPC( ch ))
        {
          if( !song )
           sprintf( buf, "Spell %d (%s) being cast by %s", sn,
             spells_table[sn].name, ch->name );
          else
           sprintf( buf, "Song %d (%s) being sung by %s", sn,
             songs_table[sn].name, ch->name );
        log_string( buf );
        }

        if( !song )
  	  (*spells_table[sn].spell_fun) ( sn, URANGE( 1, ch->level, LEVEL_HERO ), ch, vo );
        else
  	  (*songs_table[sn].spell_fun) ( sn, URANGE( 1, ch->level, LEVEL_HERO ), ch, vo );
        if( !song && mem && !IS_NPC( ch ) && !IS_IMMORTAL( ch ))
        {
//         MEM_DATA *oldmem;
          mem->memmed = FALSE;
          mem->memtime = mem->full_memtime;
/*
 * Trying to put mem at top of linked list.
 * I screwed up somewhere, commenting it out for now
 * - Lithos
          oldmem = mem;
          //pull mem from its old position in the list
          if ( mem == ch->pcdata->memorized) {
              //at top of list already, do nothing
              }
          else if ( mem == ch->pcdata->memorized_last ) {
              // pull from end, and we know there is at least 2 in list
              mem->prev->next = NULL;
              ch->pcdata->memorized_last = mem->prev;
          }
          else {
              // mem is in the middle
              mem->next->prev = mem->prev;
              mem->prev->next = mem->next;
          }
          mem->prev = NULL;
          mem->next = ch->pcdata->memorized;
          ch->pcdata->memorized->prev = mem;
          ch->pcdata->memorized = mem;
*/
        }
    }

    if ( (spells_table[sn].target == TAR_CHAR_OFFENSIVE 
        || spells_table[sn].target == TAR_CHAR_RANGE)
        && !song
	&& victim->master != ch && victim != ch && IS_AWAKE( victim ) )
    {
        if (ch->in_room == victim->in_room) {
            if ( !victim->fighting && can_see( victim, ch ) )
                kill_ch( victim, ch );
        }
        else {
            // range spell presumably, since different rooms
            start_grudge( victim, ch, TRUE );
            for (vch = victim->in_room->people; vch; vch = vch->next_in_room) {
                if (vch->deleted || vch == victim) continue;
                if (vch->fly_level != ch->fly_level ) continue;
                //protectors will be aggro'd
                if ( IS_SET(vch->act, ACT_PROTECTOR) && (vch->race == victim->race) ) {
                    start_grudge( vch, ch, TRUE );
                }
                // all aggro mobs will hunt down caster
                if ( IS_SET(vch->act, ACT_AGGRESSIVE) )
                    start_grudge( vch, ch, TRUE );
            }
/* mobs come rushing in too fast this way
          //didnt want to do this until all mobs in room were checked
            for(vch = victim->in_room->people; vch; vch = vch2) {
                vch2 = vch->next_in_room;
                if (vch->hunting && vch->hunting->who == ch)
                    hunt_victim(vch);
            }
*/
        }
    }

    if ( !song )
    {
        if ( spells_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( spells_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
    }
    else
    {
        if ( songs_table[sn].target == TAR_OBJ_CHAR )
            free( (OBJ_CHAR *) vo );
        if ( songs_table[sn].target == TAR_IGNORE )
            free_string( (char *) vo );
    }

    return;
}

// Handles all spellcasting, whether it be willing, singing, or casting
// If they got here as a bard, they're using the SING command word,
// if they got here as a psionicist, they're using the WILL command word,
// and if they got here as anything else, they're using CAST.
//
// These are just cheesy details handled by interp.c.... we don't care.
// What we do care about is that we *know* it's safe to base all our
// messages/decisions on the character's class.
//
// This function is also *mob-safe*, meaning that mobs can cast spells
// too.  However, this is not the preferred method (as far as I can tell)
//
// Shaman totems are checked for in this function.
// 
// - Veygoth
void cast( CHAR_DATA *ch, char *argument )
{
    void      *vo;
    OBJ_DATA  *obj;
    OBJ_CHAR  *oc;
    CHAR_DATA *victim;
    MEM_DATA  *mem;
    int        mana;
    char       buf[MAX_STRING_LENGTH];
    bool       found;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        sn;
    int        beats;  // For quick chant
    ROOM_INDEX_DATA *room2, *room3;
    char arg3[MAX_INPUT_LENGTH];
    char       *args;
    int dir;
//    CHAR_DATA *vch;
    OBJ_DATA *totem;
    OBJ_DATA *totem2;

    last_fun( "cast");

    argument = one_argument( argument, arg1 );
    args = one_argument( argument, arg2 );

    // No casting while berserked... Nor singing! Hah!
    if ( has_affect( ch, gsn_berserk, 0, 0 ) )
    {
	send_to_char( "Not while you're in a &+RBl&n&+ro&+Ro&n&+rd&+L Rage&n!\n\r", ch );
	return;
    }

    // No casting while thirsty... Nor singing! Hah!
    if ( has_affect( ch, 0, spl_thirst, 0 ) 
        && ( IS_NPC( ch ) || ch->pcdata->condition[COND_THIRST] < 10 ) )
    {
	send_to_char( "&+BNo&+Ct w&+chi&+ble &+cyo&+Bu'&+cre &+Bso p&+carc&+Bhed&n!\n\r", ch );
	return;
    }

    if (IS_NPC(ch) )
    {
        sprintf(buf,"cast: %s&n is casting %s upon %s.", ch->short_descr,
            arg1, argument);
        wiznet( 0, WIZ_LSPAM, 0, buf);
        log_string( buf );
    }

    if( ch->class == CLASS_BARD )
    {
      if( ( ( sn = song_lookup(arg1) ) < 0 ) || ( (ch->level < (songs_table[sn].spell_circle[ch->class] * 5 - 4) ) && !IS_IMMORTAL(ch) ) )
      {
	send_to_char( "You can't do that.\n\r", ch );
	return;
      }
    }
    else
    {
      if( ( ( sn = spell_lookup(arg1) ) < 0 ) || ( (ch->level < (spells_table[sn].spell_circle[ch->class] * 5 - 4) ) && !IS_IMMORTAL(ch) ) )
      {
	send_to_char( "You can't do that.\n\r", ch );
	return;
      }
    }

    // Totems required for shamans
    if( ch->class == CLASS_SHAMAN )
    {
        // Find totems
        totem = get_eq_char( ch, WEAR_HAND );
        if( totem && totem->item_type != TYPE_TOTEM )
            totem = NULL;
        totem2 = get_eq_char( ch, WEAR_HAND_2 );
        if( totem2 && totem2->item_type != TYPE_TOTEM )
            totem2 = NULL;

        if( !totem && !totem2 )
        {
            send_to_char( "You must be holding a totem in order to cast a spell.\n\r", ch );
            return;
        }
    }

    if ( ch->class != CLASS_BARD && ch->position == POS_FIGHTING && !(spells_table[sn].cast_combat) )
    {
        send_to_char( "You can't concentrate enough.\n\r", ch );
	return;
    }
    else if( ch->position != POS_STANDING && ch->position != POS_FIGHTING )
    {
      if( ch->class == CLASS_BARD )
        send_to_char( "You must be standing to sing loud enough to be heard!\n\r", ch );
      else
        send_to_char( "You must be standing to cast spells!\n\r", ch );
      return;
    }

    found = FALSE;

    if( !IS_NPC(ch) && ch->class != CLASS_BARD && ch->class != CLASS_PSIONICIST && !IS_IMMORTAL( ch ))
    {
      for( mem = ch->pcdata->memorized; mem; mem = mem->next )
      {
        if( !mem->memmed )
          continue;
        if( mem->sn == sn )
        {
          found = TRUE;
          break;
        }
      }

      if( !found && !IS_NPC(ch) )
      {
         send_to_char( "You do not have that spell memorized!\n\r", ch );
         return;
      }
    }

    if( ch->class == CLASS_SHAMAN )
    {
        switch( spells_table[sn].realms )
        {
           default:
              send_to_char( "This spell has an invalid sphere setting.  Please report this as a bug.\n\r", ch );
              return;
              break;
           case SCHOOL_SPIRITUAL:
             if( spells_table[sn].spell_circle[ch->class] <= 5 )
             { 
              if( !totem || !IS_SET(totem->value[0], TOTEM_L_SPIRIT ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_L_SPIRIT ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             else
             {
              if( !totem || !IS_SET(totem->value[0], TOTEM_G_SPIRIT ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_G_SPIRIT ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             break;
           case SCHOOL_ANIMAL:
             if( spells_table[sn].spell_circle[ch->class] <= 5 )
             { 
              if( !totem || !IS_SET(totem->value[0], TOTEM_L_ANIMAL ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_L_ANIMAL ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             else
             {
              if( !totem || !IS_SET(totem->value[0], TOTEM_G_ANIMAL ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_G_ANIMAL ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             break;
           case SCHOOL_ELEMENTAL:
             if( spells_table[sn].spell_circle[ch->class] <= 5 )
             { 
              if( !totem || !IS_SET(totem->value[0], TOTEM_L_ELEMENTAL ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_L_ELEMENTAL ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             else
             {
              if( !totem || !IS_SET(totem->value[0], TOTEM_G_ELEMENTAL ))
              {
                if( !totem2 || !IS_SET(totem2->value[0], TOTEM_G_ELEMENTAL ))
                {
                  send_to_char( "You are not holding the proper totem to cast this spell.\n\r", ch );
                  return;
                }
              }
             }
             break;
        }
    }

    if( (   is_affected( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE ) )
        && ch->class != CLASS_PSIONICIST )
    {
	send_to_char( "Your lips move but no sound comes out.\n\r", ch );
	return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "You can't...you are in a Cone of Silence!\n\r", ch );
        return;
    }

    if( ch->class != CLASS_BARD )
    {
    if ( IS_ROOM( ch->in_room, ROOM_EARTHEN_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+lThe &+yearth&n &+lcomes up &+yand engulfs &+lyour spell.\n\r", ch );
        spell_damage( ch, ch, 1, spl_earthen_starshell, DAM_FIRE );
        WAIT_STATE( ch, 6 );
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_AIRY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+CAir swir&n&+cls a&+Cnd absorbs y&n&+cour spell.&n\n\r", ch );
        WAIT_STATE( ch, 6 );
        if( ch->position > POS_RECLINING && number_percent() + 50 > get_curr_agi( ch ) )
        {
            ch->position = POS_RECLINING;
            WAIT_STATE( ch, 6 );
            send_to_char( "You are knocked over!\n\r", ch);
        }
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_WATERY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+bWater b&+Bursts up a&n&+bnd absor&+Bbs your spell.&n\n\r", ch );
        WAIT_STATE( ch, 6 );
        ch->move -= 20;
        send_to_char( "You feel tired!\n\r", ch);
        return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_FIERY_STARSHELL ) )
    {
        send_to_char( "You start casting...\n\r", ch );
        send_to_char( "&+RFire&n&+r engu&+Rlfs y&n&+rour s&+Rpell.&n\n\r", ch );
        spell_damage( ch, ch, 1, spl_fiery_starshell, DAM_FIRE );
        WAIT_STATE( ch, 6 );
        return;
    }
    }

    if( ch->class != CLASS_PSIONICIST && ch->class != CLASS_BARD && 
        IS_ROOM( ch->in_room, ROOM_NO_MAGIC ) )
    {
        send_to_char( "You start casting...", ch );
        send_to_char( "After a brief gathering of energy, your spell fizzles.\n\r", ch );
        WAIT_STATE( ch, 6 );
        return;
    }
    else if( ch->class == CLASS_PSIONICIST && IS_ROOM( ch->in_room, ROOM_NO_PSIONICS ))
    {
        send_to_char( "Something here prevents you from focusing your will.\n\r", ch );
        WAIT_STATE( ch, 2 );
        return;
    }

    if( ch->class == CLASS_PSIONICIST )
        mana = MANA_COST( ch, sn );
    else if( ch->class == CLASS_BARD )
        mana = songs_table[sn].min_mana;

    /*
     * Locate targets.
     */
    victim	= NULL;
    vo		= NULL;
    
    if (IS_NPC(ch) ) {  
        sprintf(buf,"cast: Attempting to find target for %s&n.", ch->short_descr);
        wiznet( 0, WIZ_LSPAM, 0, buf);
    }
    if( ch->class != CLASS_BARD )
    switch ( spells_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    /* Note: When using this targetting method, it's up finish_spell to
     *         free the memory used by the str_dup! We need to add a case
     *         on all losing concentrations. - Lohrr
     */
    case TAR_IGNORE:
        vo = (void *) str_dup(argument);
	break;

    case TAR_CHAR_WORLD:
        victim = get_char_world( ch, arg2 );
        if ( !victim ) {
            send_to_char( "Cast it on who?\n\r", ch );
            return;
        }
        vo = (void *)victim;
        break;

    case TAR_TRAP:
        send_to_char( "You cannot cast a trap!\n\r", ch );
        return;

    case TAR_CHAR_OFFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    if ( !( victim = ch->fighting ) )
	    {
                if( ch->class == CLASS_PSIONICIST )
                    send_to_char( "Will the spell upon whom?\n\r", ch );
                else
                    send_to_char( "Cast the spell upon whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	if ( is_affected( ch, AFF_CHARM ) && ch->master == victim )
        {
	    send_to_char( "You can't do that to your master!.\n\r",
			 ch );
	    return;
	}

	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;

	check_killer( ch, victim );

	vo = (void *) victim;
        if (IS_NPC(ch) && victim) {
            sprintf(buf,"cast: %s&n is %s&n's target.", victim->short_descr,
                ch->short_descr);
            wiznet( 0, WIZ_LSPAM, 0, buf);
        }
        make_vis( ch );
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
        if (IS_NPC(ch) && victim) {
            sprintf(buf,"cast: %s&n is %s&n's target.", victim->short_descr,
                ch->short_descr);
            wiznet( 0, WIZ_LSPAM, 0, buf);
        }

	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( arg2, ch->name ) && 
             !str_cmp( arg2, "me" ) && !str_cmp( arg2, "self" ) )
	{
	    send_to_char( "You cannot cast this spell on another.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char( "You are not carrying that.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;

    case TAR_OBJ_HERE:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_here( ch, arg2 ) ) )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;

    case TAR_OBJ_NECRO:
        vo = (void *) argument;
        break;

    /* Note: When using this targetting method, it's up finish_spell to
     *         free the memory used by the OBJ_CHAR pointer!!! We need to
     *         add a case on all losing concentrations. - Lohrr
     */
    case TAR_OBJ_CHAR:
        if ( arg2[0] == '\0' ) {
            if (ch->fighting)
                victim = ch->fighting;
            else {
                send_to_char("Cast upon what?\n\r", ch);
                return;
            }
        }
        else if ( !( victim = get_char_room( ch, arg2 ) ) )
            obj = get_obj_here( ch, arg2 );

        if ( victim != NULL )
        {
            oc = malloc( sizeof( OBJ_CHAR ) );
            oc->type = TYPE_CHAR_DATA;
            oc->pointer.ch = victim;
            vo = (void *) oc;
        }
        else if ( obj != NULL )
        {
            oc = malloc( sizeof( OBJ_CHAR ) );
            oc->type = TYPE_OBJ_DATA;
            oc->pointer.obj = obj;
            vo = (void *) oc;
        }
        else
        {
            send_to_char( "You do not see that here.\n\r", ch );
            return;
        }

        break;
    case TAR_CHAR_RANGE:
	if ( arg2[0] == '\0' )
	{
	    if ( !( victim = ch->fighting ) )
	    {
		send_to_char( "Cast the spell on whom?\n\r", ch );
		return;
	    }
	} 
        else {
        args = one_argument( args, arg3 );
        if ( arg3[0] == '\0' ) {
            if (ch->level >= LEVEL_IMMORTAL) {
                sprintf(buf,"Looking for %s in same room.\n\r",arg2);
                send_to_char(buf, ch);
            }
	    victim = get_char_room( ch, arg2 );
            if ( !victim )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
        }
        else {
        dir = find_exit( ch, arg3 );
        if (ch->level >= LEVEL_IMMORTAL) {
           sprintf(buf,"Looking for %s to the %s.\n\r",arg2,arg3);
          send_to_char(buf, ch );
        }
        if ( IS_SET(ch->in_room->exit[dir]->exit_info, EX_WALLED)
             || IS_SET(ch->in_room->exit[dir]->exit_info, EX_BLOCKED)
             || IS_SET(ch->in_room->exit[dir]->exit_info, EX_SECRET)
             || IS_SET(ch->in_room->exit[dir]->exit_info, EX_CLOSED) 
             || !ch->in_room->exit[dir]->to_room
             || ch->in_room->area != ch->in_room->exit[dir]->to_room->area )
            {
            send_to_char("You see nothing in that direction.\n\r", ch);
            return;
            } 
        room2 = find_room(ch, arg3);
        if (!room2) {
            send_to_char("You see nothing in that direction.\n\r", ch);
            return;
        }
        victim = NULL;
        victim = get_char_at(room2, ch, arg2);
        if (!victim) {
            if (room2->exit[dir] && ((room3 = room2->exit[dir]->to_room)) 
                &&  sn == spl_fireball ) {
                victim = get_char_at(room3, ch, arg2);  
            }
        }
        if (!victim ) {
            send_to_char( "They aren't here.\n\r", ch);
            return;
        }
        } //end else
        } //end else
	if ( is_affected( ch, AFF_CHARM ) && ch->master == victim )
        {
	    send_to_char( "You can't do that to your master!.\n\r",
			 ch );
	    return;
	}

	if ( is_safe( ch, victim ) )
	    return;
        // is_safe could wipe out victim, as it calls procs if a boss
        // check and see that victim is still valid
        if ( !victim )
            return;

	check_killer( ch, victim );

	vo = (void *) victim;
        if (IS_NPC(ch) && victim) {
            sprintf(buf,"cast: %s&n is %s&n's target.", victim->short_descr,
                ch->short_descr);
            wiznet( 0, WIZ_LSPAM, 0, buf);
        }
        make_vis( ch );
	break;

    }

    if( ch->class != CLASS_BARD && ch->class != CLASS_PSIONICIST )
    {
      send_to_char( "You begin casting...\n\r", ch );
      if ( str_cmp( spells_table[sn].name, "ventriloquate" ) )
      {
        if ( spells_table[sn].target == TAR_CHAR_OFFENSIVE
               || spells_table[sn].target == TAR_CHAR_RANGE )
            act( "$n&n begins casting an offensive spell...", ch, NULL, NULL, TO_ROOM );
        else
            act( "$n&n begins casting...", ch, NULL, NULL, TO_ROOM );
      }
    }
    else if( ch->class == CLASS_BARD )
    {
      send_to_char( "You begin singing...\n\r", ch );
      act( "$n&n starts singing...", ch, NULL, NULL, TO_ROOM );
    }

    if( ch->class != CLASS_BARD )
        beats = spells_table[sn].beats;
    else
        beats = 0;

    if( ch->class != CLASS_PSIONICIST && ch->class != CLASS_BARD )
    {

      // Characters with int of 110 have normal memtimes.
      // int of 100 worsens casting times by 10%
      // with an int of 55 casting times are doubled.
      // This may seem a bit harsh, but keep in mind any
      // casters are very likely to have an int above 100, as it
      // is their prime requisite.  120 is the max int for Grey Elves
      // to start.  - Veygoth
      if( ch->class == CLASS_CLERIC || ch->class == CLASS_DRUID 
        || ch->class == CLASS_PALADIN || ch->class == CLASS_ANTIPALADIN )
      {
        beats = (beats * 110) / get_curr_wis( ch );
      }
      else if( ch->class == CLASS_SHAMAN )
      {
        beats = (beats * 330) / (get_curr_int( ch ) + (get_curr_wis( ch ) * 2));
      }
      else
      {
        beats = (beats * 110) / get_curr_int( ch );
      }

      if( !IS_NPC(ch) && ch->level >= skills_table[gsn_quick_chant].skill_level[ch->class] )
      {
        if( number_percent() < ch->pcdata->skl_lrn[gsn_quick_chant] )
        {
          // Want this one to go up slowly, so it only checks when successful
          skill_practice( ch, gsn_quick_chant );
          /* Changed this to /2 from * 3 / 5 */
          beats = beats / 2;
        }
      }
      if ( IS_NPC(ch) ) {
          beats = spells_table[sn].beats * ch->perm_int / get_curr_int(ch);
        sprintf(buf,"%s&n is casting %s in %d beats.",ch->short_descr,
            spells_table[sn].name, beats);
        wiznet( 0, WIZ_LSPAM, 0, buf );
      }
/*
 * A check for impossibly long cast times...came about from a player
 * trying to cast when feebleminded.  100 beats is arbitrary.
 * 
 * - Lithos
 */
    if ( beats > 100 ) { 
        send_to_char("Forget it!  In your present state you haven't a dream of success.\n\r", ch);
        return;
    }
      WAIT_STATE( ch, beats );

      // Create an event to handle the spell
//      if( !IS_NPC( ch ) )
      {
       CAST_DATA *caster;
       caster = alloc_mem( sizeof( CAST_DATA ));
       caster->who = ch;
       caster->next = cast_list;
       caster->event = create_event( EVENT_SPELL_CAST, beats, ch, vo, sn );
       cast_list = caster;
       SET_AFF_BIT( ch, AFF_CASTING );
      }
    } // End if( not bard or psionicist )
    else if( ch->class == CLASS_PSIONICIST )
    {
      WAIT_STATE( ch, beats );

      spell_practice( ch, sn );

      if (  !IS_IMMORTAL( ch )  && !IS_NPC(ch)
	  && ch->level < ( spells_table[sn].spell_circle[ch->class] * 5 + 1 )
	  && number_percent( ) > ch->pcdata->spl_lrn[sn] )
      {
  	send_to_char( "You lost your concentration.\n\r", ch );
  	act( "&+r$n&n&+r's face flushes white for a moment.&n", ch, NULL, NULL, TO_ROOM );
	    ch->mana -= mana/2;
      }
      else
      {
  	    ch->mana -= mana;
          sprintf( buf, "Spell %d (%s) being willed by %s", sn,
               spells_table[sn].name, ch->name );
          log_string( buf );
          (*spells_table[sn].spell_fun) ( sn,
				      URANGE( 1, ch->level, LEVEL_HERO ),
				      ch, vo );
          wiznet( 0, WIZ_LSPAM, 0, lbuf );
      }
      if ( ch->position > POS_SLEEPING && ch->mana < 0 )
      {
        WAIT_STATE( ch, 2 * PULSE_PER_SECOND );
        send_to_char( "&+WThat last spe&+wll w&+Las a bit&+l much...&n\n\r", ch );
        ch->position = POS_STANDING;
        ch->fighting = NULL;
        act( "$n&n collapses from exhaustion&n.",
    		ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
      }

      if ( spells_table[sn].target == TAR_CHAR_OFFENSIVE
  	&& victim->master != ch && victim != ch && IS_AWAKE( victim ) )
      {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		kill_ch( victim, ch );
		break;
	    }
	}
      }
    } // End if( psionicist )
    else if( ch->class == CLASS_BARD )
    {
      CAST_DATA *caster;

      WAIT_STATE( ch, 0 );

      // Create an event to handle the spell
      caster = alloc_mem( sizeof( CAST_DATA ));
      caster->who = ch;
      caster->next = cast_list;
      caster->event = create_event( EVENT_SONG, PULSE_SONG, ch, vo, sn );
      caster->event = create_event( EVENT_SONG, PULSE_SONG * 2, ch, vo, sn );
      caster->event = create_event( EVENT_SONG, PULSE_SONG * 3, ch, vo, sn );
      caster->event = create_event( EVENT_SONG, PULSE_SONG * 4, ch, vo, sn );
      caster->event = create_event( EVENT_SONG, PULSE_SONG * 5, ch, vo, sn );
      cast_list = caster;
      SET_AFF_BIT( ch, AFF_SINGING );
    } // End if( bard )
    return;
}

// Function by Veygoth for use with portal spells.  Can be reused for other spells
// if applicable.
bool has_spell_consent( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if( IS_NPC( victim ) || IS_NPC( ch ))
      return TRUE;

    if( is_same_clan( ch, victim ))
      return TRUE;

    if( is_same_group( ch, victim ))
      return TRUE;

    if( IS_IMMORTAL( ch ))
      return TRUE;

    if( victim->pcdata->consent != ch )
    {
        send_to_char( "You do not have their consent.\n\r", ch );
        return FALSE;
    }

    return TRUE;
}

void affect_refresh(CHAR_DATA *ch, int skl, int spl, int song, int duration)
{
    AFFECT_DATA *paf;

    last_fun( "affect_refresh");
    if (!ch) return;
    if (!skl && !spl && !song) return;
    for (paf = ch->affected; paf; paf = paf->next) {
        if ( paf->deleted ) continue;
        if (paf->skill == skl && paf->spell == spl && paf->song == song) {
            if ( paf->duration < duration )
                paf->duration =  duration;
            sprintf(lbuf, "Refreshing %s on %s for %d hours.", spells_table[spl].name, ch->name, duration);
            wiznet( 0, WIZ_LSPAM, 0, lbuf );
        }
    }
    return;
}

void affect_cumulate(CHAR_DATA *ch, int skl, int spl, int song, int duration, int modifier)
{
    AFFECT_DATA *paf;

    last_fun( "affect_cumulate");
    if (!ch) return;
    if (!skl && !spl && !song) return;
    for (paf = ch->affected; paf; paf = paf->next) {
        if ( paf->deleted ) continue;
        if (paf->skill == skl && paf->spell == spl && paf->song == song) {
            if ( paf->duration < duration )
                paf->duration = duration;
            paf->modifier += modifier;
            sprintf(lbuf, "Refreshing %s on %s for %d hours.", spells_table[spl].name, ch->name, duration);
            wiznet( 0, WIZ_LSPAM, 0, lbuf );
        }
    }
    return;
}

