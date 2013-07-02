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
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "necrospells.h"

/*
 * Necro-pertinent tables/structs moved
 * to necrospells.h
 */

/*
 * Utilities start here.
 */

int fuzzy_level(CHAR_DATA * ch, int corpse_level, int pet_type)
{
    /*
     * This probably needs work.
     */
    int temp = corpse_level;

    if ((corpse_level > undead_table[pet_type].max_level) && \
	(corpse_level <= ch->level))
    {
	  return undead_table[pet_type].max_level;
    }
    else if ((corpse_level >= ch->level) && \
             (ch->level <= undead_table[pet_type].max_level))
    {
       temp = (ch->level + ((dice(1, ((corpse_level + 1) - ch->level))) - 1));
       return UMIN(temp, undead_table[pet_type].max_level);
    } else if (ch->level >= corpse_level)
    {
       return UMIN(ch->level, undead_table[pet_type].max_level);
    }
    return temp;
}

bool is_draco_type(int type)
{
    if (   type != UNDEAD_RED_DRACO \
        && type != UNDEAD_BLUE_DRACO \
	&& type != UNDEAD_BLACK_DRACO \
        && type != UNDEAD_GREEN_DRACO \
	&& type != UNDEAD_WHITE_DRACO  )
	return FALSE;
    else
	return TRUE;
}

int count_undead_pets(CHAR_DATA * ch)
{
    PERSON_DATA *flist;
    CHAR_DATA *pet;
    int count = 0;

    for (flist = ch->followers; flist; flist = flist->next)
      {
	  pet = flist->who;
	  if (pet && IS_NPC(pet))
	    {
		if (!str_cmp("skeleton undead", pet->name))
		    count += undead_table[UNDEAD_SKELETON].slots;
		if (!str_cmp("zombie undead", pet->name))
		    count += undead_table[UNDEAD_ZOMBIE].slots;
		if (!str_cmp("spectre undead", pet->name))
		    count += undead_table[UNDEAD_SPECTRE].slots;
		if (!str_cmp("wraith undead", pet->name))
		    count += undead_table[UNDEAD_WRAITH].slots;
		if (!str_cmp("vampire undead", pet->name))
		    count += undead_table[UNDEAD_VAMPIRE].slots;
		if (!str_cmp("lich undead", pet->name))
		    count += undead_table[UNDEAD_LICH].slots;
		if (!str_suffix("dracolich undead", pet->name))
		    count += undead_table[UNDEAD_RED_DRACO].slots;
		if (!str_suffix("dracolich undead", pet->name))
		    count += undead_table[UNDEAD_BLUE_DRACO].slots;
		if (!str_suffix("dracolich undead", pet->name))
		    count += undead_table[UNDEAD_BLACK_DRACO].slots;
		if (!str_suffix("dracolich undead", pet->name))
		    count += undead_table[UNDEAD_GREEN_DRACO].slots;
		if (!str_suffix("dracolich undead", pet->name))
		    count += undead_table[UNDEAD_WHITE_DRACO].slots;

	    }
      }
    return count;
}

/*
 * Necro spell functions start here.
 */

void spell_heal_undead(int sn, int level, CHAR_DATA * ch, void *vo)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    if ( !(IS_UNDEAD(victim)) )
    {
          send_to_char("But they aren't undead!\n\r", ch);
          return;
    }

    /*
     * The following is based on getting heal_undead
     * at level 25.  Adjust accordingly.
     */

    if (level < 31)
    {                           /* 77  ~ 100 hps */
        heal = (50 + level + dice(2, 10));
    } else if (level < 46)
    {                           /* 144 ~ 230 hps */
        heal = (level * 3 + 50 + dice(1, level));
    } else if (level > 45)
    {                           /* 281 ~ 368 hps */
        heal = (level * 6 + dice(5, 10));
    }

    if ( victim->hit < get_max_hit( victim ) )
        victim->hit = UMIN( (victim->hit + heal) , get_max_hit( victim ));
    update_pos( victim );
    if ( ch != victim )
        send_to_char("Ok.\n\r", ch);
    send_to_char("You feel much better!\n\r", victim);
    return;
}

void spell_protect_undead(int sn, int level, CHAR_DATA * ch, void *vo)
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( !(IS_UNDEAD(victim)) )
    {
          send_to_char("You cannot work with living flesh.\n\r", ch);
          return;
    }


    if ( has_affect( victim, 0, sn, 0 ) )
        return;

    af.skill     = 0;
    af.spell     = sn;
    af.song      = 0;
    af.duration  = 6 + (level / 10);
    af.location  = 0;
    af.modifier  = (level / 4 + number_range( 1, 4 ));
    set_bitvector( &af, AFF_STONESKIN );
    affect_to_char( victim, &af );

    send_to_char( "You feel protected by powers from beyond the grave.\n\r", 
                  victim );
    act( "A dark chill from beyond the grave seems to permeate the air around $n.", 
         victim, NULL, NULL, TO_ROOM );
    return;
}

/*
 *  main undead summoning routine that spells call to create undead pets.
 */
void spell_raise_undead(CHAR_DATA * ch, OBJ_DATA * obj, int undead_type)
{
    CHAR_DATA *undead;
    AFFECT_DATA af;
    OBJ_DATA *item;
    OBJ_DATA *item_next;
    char buf[MAX_STRING_LENGTH];
    /*
     *  Check that target == corpse. 
     */
    if ((obj->item_type != TYPE_CORPSE_NPC) &&
	(obj->item_type != TYPE_CORPSE_PC))
      {
	  send_to_char("Your arcane magic fails abruptly!\n\r", ch);
	  return;
      }

    /*
     *  Check validity of corpse.
     */
    if (obj->level < undead_table[undead_type].min_level)
      {
	  send_to_char("That corpse is too weak to animate.\n\r", ch);
	  return;
      }
    if (obj->level > (ch->level + 4) && !is_draco_type(undead_type))
      {
	  send_to_char
	      ("You are not powerful enough to raise that corpse!\n\r",
	       ch);
	  return;
      }

    /*
     * Max pet check
     */
    if ((count_undead_pets(ch)) > (ch->level / 2))
      {
	  send_to_char
	      ("You couldn't possibly control any more undead!\n\r", ch);
	  return;
      }


    /*
     * Announce it to the room 
     */
    act
	("You breathe life into the $p with the awesome power of your art.",
	ch, obj, NULL, TO_CHAR);
    act
	("You see the $p take a deep breath, and suddenly come to life again.",
	 ch, obj, NULL, TO_ROOM);
    /*
     *  Create the pet, move it to player and charm it.
     */
    undead =
	create_mobile(get_mob_index(undead_table[undead_type].vnumber));
    char_to_room(undead, ch->in_room);
    add_follower(undead, ch);
    SET_BIT( undead->act, ACT_NOEXP );
    af =
	create_affect(0, spl_charm_person, 0, obj->timer,
		      APPLY_NONE, 0, AFF_CHARM);
    affect_to_char(undead, &af);
    /*
     *  Bookkeeping crap like race, hps, level.
     */
    if (!is_draco_type(undead_type))
      {
	  undead->race = RACE_UNDEAD;
	  undead->level = fuzzy_level(ch, obj->level, undead_type);
	  undead->max_hit = dice((undead->level + 1), 8);	/*
								 * Govno hp
								 * modifier
								 * needed? 
								 */
	  undead->hit = undead->max_hit;
      }

    /*
     *  Give corpse contents to pet.  (will need to be redone for draco possibility)
     */
    for (item = obj->contains; item; item = item_next)
      {
	  item_next = item->next_content;
	  if (item->deleted)
	      continue;
	  obj_from_obj(item);
	  obj_to_char(item, undead);
      }

    /*
     *  Create proper descriptions and such.
     */
    sprintf(buf, "The %s of %s",
	    undead_table[undead_type].name, obj->short_descr + 10);
    free_string(undead->short_descr);
    undead->short_descr = str_dup(buf);
    sprintf(buf, "The %s of %s is standing here.\n\r",
	    undead_table[undead_type].name, obj->short_descr + 10);
    free_string(undead->long_descr);
    undead->long_descr = str_dup(buf);
    /*
     *  Get rid of corpse.
     */
    extract_obj(obj);
}

void spell_animate_dead(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }
    /*
     * Some routine to generate randomly undead based upon corpse/caster level.
     */

    spell_raise_undead(ch, obj, 0);
    return;
}

void spell_raise_skeleton(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_SKELETON);
    return;
}

void spell_raise_zombie(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_ZOMBIE);
    return;
}


void spell_raise_wraith(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_WRAITH);
    return;
}

void spell_raise_spectre(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_SPECTRE);
    return;
}

void spell_raise_vampire(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_VAMPIRE);
    return;
}

void spell_raise_lich(int sn, int level, CHAR_DATA * ch, void *vo)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *targ1;
    OBJ_DATA *obj;
    targ1 = one_argument((char *) vo, arg1);
    one_argument(targ1, arg2);
    if (!(obj = get_obj_here(ch, arg2)))
      {
	  send_to_char("You do not see that here.=\n\r", ch);
	  return;
      }

    spell_raise_undead(ch, obj, UNDEAD_LICH);
    return;
}

void spell_create_dracolich(int sn, int level, CHAR_DATA * ch, void *vo)
{
    return;
}

void spell_cloak_of_fear(int sn, int level, CHAR_DATA * ch, void *vo)
{
    CHAR_DATA	*vch;
    AFFECT_DATA af;

    send_to_char("The very essence of death flows out from you.\n\r", ch );
    act( "$n assumes a terrifying visage of death!", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
                                             :  IS_NPC( vch ) ) )
	    {
		if ( is_affected( vch, AFF_FEAR ) || saves_spell(level,vch, DAM_BLACK_MANA) )
                       continue;
                af.skill     = 0;
                af.spell     = sn;
                af.song      = 0;
                af.duration  = 1 + (level / 5);
                af.location  = 0;
                af.modifier  = 0;
                set_bitvector( &af, AFF_FEAR);
                affect_to_char( vch, &af );

                act( "$N&n is scared!", ch, NULL, vch, TO_CHAR    );
                send_to_char( "You are scared!\n\r", vch );
                act( "$N&n is scared!", ch, NULL, vch, TO_NOTVICT );

                do_flee( vch, "" );
                if( IS_NPC( vch ))
                    start_fearing( vch, ch );
            }

            continue;
        }

    }



    return;
}
