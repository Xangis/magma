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



/*
 * The following special functions are available for objects.
 */
DECLARE_OBJ_FUN(        spec_giggle             );
DECLARE_OBJ_FUN(        spec_soul_moan          );
DECLARE_OBJ_FUN(        spec_hum                );
DECLARE_OBJ_FUN(        spec_haste              );
DECLARE_OBJ_FUN(        spec_stoneskin          );
DECLARE_OBJ_FUN(        spec_sneak              );
DECLARE_OBJ_FUN(        spec_hide               );
DECLARE_OBJ_FUN(        spec_invisibility       );
DECLARE_OBJ_FUN(        spec_wp_lightning       );
DECLARE_OBJ_FUN(        spec_wp_chill           );
DECLARE_OBJ_FUN(        spec_wp_burn            );
DECLARE_OBJ_FUN(        spec_wp_fireball        );
DECLARE_OBJ_FUN(        spec_wp_harm            );
DECLARE_OBJ_FUN(        spec_wp_magic_missile   );
DECLARE_OBJ_FUN(        spec_wp_cutting_breeze  );
DECLARE_OBJ_FUN(        spec_wp_wither          );
DECLARE_OBJ_FUN(        spec_sundial            );
DECLARE_OBJ_FUN(        spec_ski                );
DECLARE_OBJ_FUN(        spec_inferno            );
DECLARE_OBJ_FUN(        spec_autumndecay        );
DECLARE_OBJ_FUN(        spec_celestial          );
DECLARE_OBJ_FUN(        spec_windsabre          );
DECLARE_OBJ_FUN(        spec_hornofplenty       );
DECLARE_OBJ_FUN(        spec_ghoulbane          );
DECLARE_OBJ_FUN(        spec_wp_bonethunder     );
DECLARE_OBJ_FUN(        spec_lightanddark       );  //S**
DECLARE_OBJ_FUN(        spec_trident            );  //S**
DECLARE_OBJ_FUN(        spec_hammer             );



OBJ_FUN *spec_obj_lookup( const char *name )
{
    int cmd;


    for ( cmd = 0; *spec_obj_table[cmd].spec_name; cmd++ )
	if ( !str_cmp( name, spec_obj_table[cmd].spec_name ) )
	    return spec_obj_table[cmd].spec_fun;

    return 0;
}


char *spec_obj_string( OBJ_FUN *fun )
{
    int cmd;

    for ( cmd = 0; *spec_obj_table[cmd].spec_fun; cmd++ )
	if ( fun == spec_obj_table[cmd].spec_fun )
	    return spec_obj_table[cmd].spec_name;

    return 0;
}

/*
 * Object special function commands.
 */
const   struct  spec_obj_type   spec_obj_table  [ ]     =
{
    { "spec_giggle",            spec_giggle             },
    { "spec_soul_moan",         spec_soul_moan          },
    { "spec_hum",               spec_hum                },
    { "spec_haste",             spec_haste              }, 
    { "spec_stoneskin",         spec_stoneskin          },
    { "spec_sneak",             spec_sneak              },
    { "spec_hide",              spec_hide               },
    { "spec_invisibility",      spec_invisibility       },
    { "spec_wp_lightning",      spec_wp_lightning       },
    { "spec_wp_chill",          spec_wp_chill           },
    { "spec_wp_burn",           spec_wp_burn            },
    { "spec_wp_fireball",       spec_wp_fireball        },
    { "spec_wp_harm",           spec_wp_harm            },
    { "spec_wp_magic_missile",  spec_wp_magic_missile   },
    { "spec_wp_cutting_breeze", spec_wp_cutting_breeze  },
    { "spec_wp_wither",         spec_wp_wither          },
    { "spec_sundial",           spec_sundial            },
    { "spec_ski",               spec_ski                },
    { "spec_inferno",           spec_inferno            },
    { "spec_autumndecay",       spec_autumndecay        },
    { "spec_celestial",         spec_celestial          },
    { "spec_windsabre",         spec_windsabre          },
    { "spec_hornofplenty",      spec_hornofplenty       },
    { "spec_ghoulbane",         spec_ghoulbane          },
    { "spec_wp_bonethunder",    spec_wp_bonethunder     },
    { "spec_lightanddark",      spec_lightanddark       },
    { "spec_hammer",            spec_hammer             },
    { "",                       0                       }
};  

/*
 * Special procedures for objects.
 */
bool spec_giggle( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_giggle");
        if( hit )
          return FALSE;

    if ( !keeper || !keeper->in_room )
	return FALSE;

    if ( number_percent( ) < 5 )
    {
	act( "$p&n carried by $n&n starts giggling to itself!",
	    keeper, obj, NULL, TO_ROOM );
	act( "$p&n carried by you starts giggling to itself!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    return FALSE;
}

bool spec_hum( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_hum");
    if( hit )
        return FALSE;

    if ( !keeper || !keeper->in_room )
	return FALSE;

    if ( number_percent( ) < 9 )
    {
	act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    return FALSE;
}


bool spec_soul_moan( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_soul_moan");
    if( hit )
        return FALSE;

    if ( !keeper || !keeper->in_room )
	return FALSE;

    if ( number_percent( ) < 3 )
    {
	act( "The soul in $p&n carried by $n&n moans in agony.",
	    keeper, obj, NULL, TO_ROOM );
	act( "The soul in $p&n carried by you moans to be set free!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    if ( number_percent( ) < 2 )
    {
	act( "The soul in $p&n carried by $n&n tries to free itself!",
	    keeper, obj, NULL, TO_ROOM );
	act( "The soul in $p&n carried by you starts writhing!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    return FALSE;
}

// Haste proc disabled until haste spell coded - Veygoth
bool spec_haste( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_haste");

    if( hit )
        return FALSE;
    if( !is_affected( keeper, AFF_HASTE ))
     {
      spell_haste( spl_haste, 30, keeper, keeper );
      return TRUE;
     }

    return FALSE;
}

bool spec_sneak( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_sneak");
	return FALSE;
}

bool spec_hide( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_hide");
	return FALSE;
}

bool spec_invisibility( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_invisibility");
	return FALSE;
}

bool spec_stoneskin( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_stoneskin");
	return FALSE;
}

bool spec_wp_chill( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_chill");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_chill_touch( spl_chill_touch, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_burn( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_burn");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_burning_hands( spl_burning_hands, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_fireball( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_fireball");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_fireball( spl_fireball, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_lightning( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_lightning");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_lightning_bolt( spl_lightning_bolt, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_harm( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_harm");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_harm( spl_harm, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_destroy_undead( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
   CHAR_DATA  *victim = NULL;

    last_fun( "spec_wp_destroy_undead");

        if( !keeper || !keeper->fighting || !hit )
     	  return FALSE;

        if ( IS_UNDEAD( victim ))
        {

          if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
          {
            spell_destroy_undead( spl_destroy_undead, 1, keeper, keeper->fighting );
            return TRUE;
          }
        }
        return FALSE;
}

bool spec_wp_magic_missile( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_magic_missile");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_magic_missile( spl_magic_missile, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_cutting_breeze( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_cutting_breeze");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_cutting_breeze( spl_cutting_breeze, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_wp_wither( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_wither");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          spell_wither( spl_wither, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_sundial( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_sundial");
        if( hit )
          return FALSE;
        else
          return FALSE;
}

bool spec_ski( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_ski");
    /* check to make sure the object has an owner and he's in the room */

    if ( !keeper || !keeper->in_room )
	return FALSE;

    /* chance of humming, just like Monica Lewinski */

    if ( number_percent( ) < 20 )
    {
	act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    /* chance of weapon screaming */

    if ( number_percent( ) < 20 )
    {
	act( "&+LThe soul in&n $p&n &+Lcarried b&ny $n&n &+Lmoans in&n &+Ragony&n&+L.&n",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LThe soul in&n $p&n &+Lcarried by you&n &+Rmoans&n &+Lto be set free!&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    if ( number_percent( ) < 20 )
    {
	act( "&+LThe soul in&n $p&n &+Lcarried by&n $n&n &+Ltries to free itself!&n",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LThe soul in&n $p&n &+Lcarried by you starts&n &+Bwrithing!&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    /* check to see if the weapon is in combat */

    if( !keeper || !keeper->fighting || !hit )
        return FALSE;          

    /* harm proc - note that there has to be a spec_wp_harm above before this
       will work */

    if( number_percent() < 20 && ( keeper->fighting->hit >= -9 ) )
    {
        spell_harm( spl_harm, keeper->level, keeper, keeper->fighting );
        return TRUE;
    }
         
    return FALSE;
}

bool spec_autumndecay( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_autumn_decay");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 )
          && !has_affect( keeper->fighting, 0, spl_wither, 0 ) )
        {
          act( "&+y$n's&n $p&n &+ydives into &n$N&n&+y, and a &+Lblack m&N&+wis&+Lt&n&+y flows into the &+rwound!&N",
	      keeper, obj, keeper->fighting, TO_NOTVICT );
          act( "&+yYour&n $p&n &+ydives into &n$N&n&+y, and a &+Lblack m&N&+wis&+Lt&n&+y flows into the &+rwound!&N",
	      keeper, obj, keeper->fighting, TO_CHAR );
          act( "&+y$n's&n $p&n &+ydives into you, and a &+Lblack m&N&+wis&+Lt&n&+y flows into the &+rwound!&N",
	      keeper, obj, keeper->fighting, TO_VICT );
          spell_wither( spl_wither, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_celestial( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
          act( "&+b$n's&n $p&n &+bs&+Bp&+ca&+Wr&n&+Ck&n&+cl&N&+Be&n&+bs with a &+csoft &+Bblue &n&+wg&+Wl&n&+wi&+Wtt&N&+we&+Wr&n&+b...&N",
	      keeper, obj, keeper->fighting, TO_NOTVICT );
          act( "&+bYour&n $p&n &+bs&+Bp&N&+ca&+Cr&+Wk&+Cl&+Be&n&+bs at &n$N&n&+b, &+Bglowing&N&+b with a&+c soft &+Bblue &N&+wg&+Wl&n&+wi&+Wtt&n&+we&+Wr&n&+b!&N ",
	      keeper, obj, keeper->fighting, TO_CHAR );
          act( "&+b$n's&n $p&n &+bs&+Bp&N&+ca&+Cr&+Wk&+Cl&+Be&n&+bs at you, &+Bglowing&N&+b with a&+c soft &+Bblue &N&+wg&+Wl&n&+wi&+Wtt&n&+we&+Wr&n&+b!&N ",
	      keeper, obj, keeper->fighting, TO_VICT );
          spell_magic_missile( spl_magic_missile, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_windsabre( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

        if( number_percent() < 10 + keeper->level / 10 && ( keeper->fighting->hit >= -9 ) )
        {
          act( "&+c$n's&n $p&n &+csummons forth a &+Cgust&n&+c of &+Wstrong winds!&N",
	      keeper, obj, keeper->fighting, TO_NOTVICT );
          act( "&+cYour&n $p&n &+csummons forth a &+Cgust&n&+c of &+Wstrong winds&N&+c, cutting into &n$N&n&+c!&N ",
	      keeper, obj, keeper->fighting, TO_CHAR );
          act( "&+c$n's&n $p&n &+csummons forth a &+Cgust&n&+c of &+Wstrong winds&N&+c, cutting into you!&N ",
	      keeper, obj, keeper->fighting, TO_VICT );
          spell_chill_touch( spl_windsaber, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}


bool spec_hornofplenty( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{

    /* check to make sure the object has an owner and he's in the room */

    if ( !keeper || !keeper->in_room )
	return FALSE;

    /* chance of humming, just like Monica Lewinski */

    if ( number_percent( ) < 2 )
    {
	act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    /* we need to make sure it doesn't check for hunger on an NPC */

    if( !keeper || IS_NPC(keeper))
        return FALSE;          

    /* check for the hunger level of the holder */

    if( keeper->pcdata->condition[COND_FULL] < 1 || 
        keeper->pcdata->condition[COND_THIRST] < 1)
    {
        CHAR_DATA  *victim;

        act( "$n's $p&n &+mglows&n&+y softly.&n",
            keeper, obj, NULL, TO_ROOM );
        act( "&+yYour&n $p&n &+mglows&n&+y softly.&n",
            keeper, obj, NULL, TO_CHAR );

        for( victim = keeper->in_room->people; victim; victim = victim->next_in_room )
        {
            if( victim->deleted || IS_NPC(victim))
                continue;

            if( victim->pcdata->condition[COND_FULL] < 30 )
            {
                 victim->pcdata->condition[COND_FULL] = 30;
                 send_to_char( "You feel full.\n\r", victim );
            }

            if( victim->pcdata->condition[COND_THIRST] < 30 )
            {     
                 victim->pcdata->condition[COND_THIRST] = 30; 
                 send_to_char( "You do not feel thirsty.\n\r", victim );
            }

        }
        return TRUE;
    }
         
    return FALSE;
}

bool spec_ghoulbane( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{

        CHAR_DATA  *victim;

        if( !keeper || !keeper->fighting || !hit )
     	  return FALSE;

        victim = keeper->fighting;

        if ( IS_UNDEAD( victim ))
        {

          if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
          {
            act( "&+w$n's&n $p&n &+Wglows&n &+wwith a powerful light!&N",
              keeper, obj, keeper->fighting, TO_NOTVICT );
            act( "&+wYour&n $p&n &+Wglows&n &+was it bites into its victim!&n", keeper, 
              obj, keeper->fighting, TO_CHAR );
            act( "&+w$n's&n $p&n &+Wglows&n &+win an angry light as it burns you horribly!&N", keeper,
              obj, keeper->fighting, TO_VICT );
            spell_destroy_undead( spl_destroy_undead, 1, keeper, keeper->fighting );
            return TRUE;
          }
       }
       return FALSE;
}

bool spec_wp_bonethunder( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_wp_bonethunder");
    if( !keeper || !keeper->fighting || !hit )
      return FALSE;

        if( number_percent() < 10 && ( keeper->fighting->hit >= -9 ) )
        {
            act( "&+w$n's&n $p&n &+wemits a horrible &+WCRACKING&n &+wnoise and&n $N&n &+wshreaks in agony!&N",
              keeper, obj, keeper->fighting, TO_NOTVICT );
            act( "&+wYour&n $p&n &+wmakes a horrible&n &+WCRACKING&n &+wnoise and&n $N&n &+wshreaks in pain!", keeper, 
              obj, keeper->fighting, TO_CHAR );
            act( "&+w$n's&n $p&n &+wmakes a horrible&n &+WCRACKING&n &+wnoise and you feel your bones breaking!&N", keeper,
              obj, keeper->fighting, TO_VICT );

          spell_bonethunder( spl_bonethunder, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_inferno( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )
{
    last_fun( "spec_inferno");
    /* check to make sure the object has an owner and he's in the room */

    if ( !keeper || !keeper->in_room )
	return FALSE;

    /* chance of humming, just like Monica Lewinski */
    if ( number_percent( ) < 10 )
    {
	act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
	    keeper, obj, NULL, TO_ROOM );
	act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    /* check to see if the weapon is in combat */
    if( !keeper->fighting || !hit )
        return FALSE;          


    if( keeper->fighting && keeper->in_room && number_percent() < 10
        && keeper->fighting->hit >= -9 )
    {
        CHAR_DATA *victim;
        CHAR_DATA *next_in_room;
        int        dam;

	act( "&+r$n&+r's $p&+r glows brightly and emits a storm of fire!&n",
	    keeper, obj, NULL, TO_ROOM );
	act( "Your $p&+r glows brightly and emits a storm of fire!&n",
	    keeper, obj, NULL, TO_CHAR );
        dam = dice( 10, 10 ) + 150 ;
        for( victim = next_in_room = keeper->in_room->people; victim; victim = next_in_room )
        {
            next_in_room = next_in_room->next_in_room;

            if( victim->deleted || is_same_group( victim, keeper ) 
                || victim == keeper || victim->hit < -9 )
                continue;
            if (victim->fly_level != keeper->fly_level)
                continue;
            if ( saves_spell( obj->level, victim, DAM_FIRE ) )
                spell_damage( keeper, victim, dam/2, spl_inferno, DAM_FIRE );
            else
                spell_damage( keeper, victim, dam, spl_inferno, DAM_FIRE );
        }
    }

    return FALSE;
}

//S**
bool spec_lightanddark( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )  
{
    last_fun( "spec_lightanddark");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

		//Hum
		if ( number_percent( ) < 10 )
		{
		act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
			keeper, obj, NULL, TO_ROOM );
		act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
			keeper, obj, NULL, TO_CHAR );
		return TRUE;
		}

		//Weapon proc
        if( keeper->fighting && number_percent() < 15 )
        {
          act( "&n$n&+L's sword fulgurates fiercely as a &+Csearing &+clight &+Lcollects at the blade's end. ",
	      keeper, obj, keeper->fighting, TO_ROOM );
          spell_lightanddark( spl_lightanddark, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

//S**
bool spec_trident( OBJ_DATA *obj, CHAR_DATA *keeper, bool hit )  
{
    last_fun( "spec_trident");
        if( !keeper || !keeper->fighting || !hit )
  	  return FALSE;

		//Hum
		if ( number_percent( ) < 10 )
		{
		act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
			keeper, obj, NULL, TO_ROOM );
		act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
			keeper, obj, NULL, TO_CHAR );
		return TRUE;
		}

		//Weapon proc
        if( keeper->fighting && number_percent() < 15 )
        {
          act( "&n$n&+c's &n&+Ctr&n&+cident &n&+bglows &+Bblue&n&+c as &n&+Csur&n&+crea&n&+Cl &n&+Bwat&n&+bers &n&+Bbeg&n&+bin &n&+cto &n&+Cg&n&+cath&n&+Cer &n&+bat i&n&+Bt&n&+bs &n&+Cti&n&+cp.&n",
	      keeper, obj, keeper->fighting, TO_ROOM );
          spell_lightanddark( spl_lightanddark, keeper->level, keeper, keeper->fighting );
          return TRUE;
        }

        return FALSE;
}

bool spec_hammer(  OBJ_DATA *obj, CHAR_DATA *keeper, bool hit ) {
  int dam;
  CHAR_DATA  *vict;
  CHAR_DATA *tch, *next;
  int dir;
  bool retval = FALSE;

   last_fun( "spec_hammer");
   if( !keeper )
        return FALSE;

    //Hum
    if ( ( !keeper->fighting && number_percent( ) < 10 ) ||
         ( keeper->fighting && number_percent() < 2 ) )
    {
        act( "&+LA faint hum can be heard from&n $p&n &+Lcarried by&n $n&n.",
           keeper, obj, NULL, TO_ROOM );
        act( "&+LA faint hum can be heard from&n $p&n &+Lyou are carrying.&n",
           keeper, obj, NULL, TO_CHAR );
        if ( !is_affected( keeper, AFF_STONESKIN) && number_percent() < 30 )
           spell_stone_skin(spl_stoneskin, 60, keeper, (void *)keeper );
         retval = TRUE;
    }

    if ( !keeper->fighting )
        return retval;

    // it fires off 1 in 12 times
    if( number_percent() > 8 )
        return FALSE;

    // if not wielded as primary, do nothing
    if ( get_eq_char(keeper, WEAR_HAND ) != obj )
        return FALSE;

    // grumbar's hammer needs to be grounded to work
    if ( keeper->fly_level )
        return FALSE;

    // need to have some earthly elements around
//    if ( keeper->in_room->sector == SECT_PLANE_AIR ||
//         keeper->in_room->sector == SECT_PLANE_WATER ||
//         keeper->in_room->sector == SECT_PLANE_FIRE )
//        return FALSE;

    vict = keeper->fighting;

    switch ( number_range( 1, 3 ) ) {
    case 1:
        //throw a wall
        dir = number_range(0,MAX_DIR-1);
        if ( keeper->in_room->exit[dir] )
            spell_wall_of_stone( spl_wall_stone, 50, keeper, (void *)dir_name[dir]);
        retval = TRUE;
        break;
  
    case 2:
        // earthen rain
        if ( !IS_OUTSIDE( keeper ) ) {
            retval = FALSE;
            break;
        }
        act("Your $p crushes $N in a rain of &+yearth&N and &+Lstone&N!", keeper,
           obj, vict, TO_CHAR);
        act("$n's $p crushes you under a rain of &+yearth&N and &+Lstone&N!", keeper,
           obj, vict, TO_VICT);
        act("$n's $p crushes $N under a rain of &+yearth&N and &+Lstone&N!.", keeper,
           obj, vict, TO_NOTVICT);
        // same damage as earthen rain at level 50
        dam = dice(150,3) + 100;
        spell_damage( keeper, vict, dam, spl_earthen_rain, DAM_CRUSHING );
        retval = TRUE;
        break;
    case 3:
      // cause an earthquake
      act("Your $p causes the ground to rise up!",
        keeper, obj, vict, TO_CHAR);
      act("$n's $p blasts the ground with a &+yshockwave&N!",
        keeper, obj, 0, TO_ROOM); 
      for (tch = keeper->in_room->people;tch; tch = next) {
          next = tch->next_in_room;
          if ((tch == keeper) || IS_IMMORTAL(tch))
            continue;
          if ( tch->deleted )
            continue;
          if ( is_same_group( keeper, tch ) )
            continue;
          if ( is_affected(tch, AFF_FLYING) )
            continue;
          if ( IS_NPC( keeper ) && IS_NPC( tch ) && !is_same_group( vict, tch ) )
            continue;
          if ( saves_spell( 60, tch, DAM_EARTH ) )
          {
              act("You wobble precipitously, but keep your feet.", 
                keeper, NULL, tch, TO_VICT);
            act("$n&n stumbles slightly but keeps $s balance.", tch, NULL, NULL, TO_ROOM);
          } else {
             act("You are knocked to the ground!",
                keeper, NULL, tch, TO_VICT);
             act("$n&n is knocked to the ground!", tch, NULL, NULL, TO_ROOM);
             tch->position = POS_KNEELING;
             WAIT_STATE(tch, PULSE_VIOLENCE);
          }
        } // end for
        retval = TRUE;
        break;
    default:
		break;
    } //end switch
  return retval;
}
