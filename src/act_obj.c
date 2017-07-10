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



/*
 * Local functions.
 */
#define CD CHAR_DATA
void    get_obj        args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                   OBJ_DATA *container ) );
void    wear_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                   bool fReplace ) );
bool    equip_hand     args( ( CHAR_DATA *ch, OBJ_DATA *obj, int type ));
CD *    find_keeper    args( ( CHAR_DATA *ch, char *argument ) );
#undef    CD

#define EQUIP_HOLD   1
#define EQUIP_WIELD  2
#define EQUIP_SHIELD 3
#define EQUIP_LIGHT  4

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    last_fun( "get_obj");

    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
      send_to_char( "You can't take that.\n\r", ch );
      return;
    }

    if ( !( obj->item_type == TYPE_MONEY ) )
    {
      if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
      {
          act( "$p&n is quite literally the &+Ystraw&n that would break the &n&+ycamel&n's back.", ch, obj, NULL, TO_CHAR );
          return;
      }
    }

    if ( container )
    {
        act( "You get $p&n from $P&n.", ch, obj, container, TO_CHAR );
        act( "$n&n retrieves $p&n from $P&n.", ch, obj, container, TO_ROOM );
        obj_from_obj( obj );
        // Fix for corpse EQ dupe on crash
        if( container->item_type == TYPE_CORPSE_PC )
        {
           save_corpses();
        }
    }
    else
    {
        act( "You get $p&n.", ch, obj, container, TO_CHAR );
        act( "$n&n picks up $p&n.", ch, obj, container, TO_ROOM );
        obj_from_room( obj );
    }


    if ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL  ) && IS_EVIL( ch ))
    {
        act( "&+LYou are &n&+rburned&+L by holy &+Rfire&+L from $p&+L.  Ouch!&n", ch, obj, NULL,
        TO_CHAR );
        act( "$n&+L is &n&+rburned&+L by holy &+Rfire&+L from &n$p&+L!&n", ch, obj, NULL, TO_ROOM );
        spell_damage( ch, ch, 20, spl_burning_hands, DAM_WHITE_MANA );
        obj_to_room( obj, ch->in_room );
        return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL  ) && IS_EVIL( ch ))
    {
        act( "&+LYou are &n&+rburned&+L by holy &+Rfire&+L from $p&+L.  Ouch!&n", ch, obj, NULL,
        TO_CHAR );
        act( "$n&+L is &n&+rburned&+L by holy &+Rfire&+L from &n$p&+L!&n", ch, obj, NULL, TO_ROOM );
        spell_damage( ch, ch, 20, spl_burning_hands, DAM_WHITE_MANA );
        obj_to_room( obj, ch->in_room );
        return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD  ) && IS_GOOD( ch ))
    {
        act( "&+LYou are &n&+rconsumed&+L by &+Rfire&+L and &+Ldespair&n from $p&+L!&n", ch, obj, NULL, TO_CHAR );
        act( "$n&+L is &n&+rengulfed&+L by an abundancy of &+Rflames&+L from &n$p&+L!&n", ch, obj, NULL, TO_ROOM );
        spell_damage( ch, ch, 20, spl_burning_hands, DAM_WHITE_MANA );
        obj_to_room( obj, ch->in_room );
        return;
    }

    if ( obj->item_type == TYPE_MONEY )
    {
        char buf [ MAX_STRING_LENGTH ];
        char buf2[128];
        int  amount;

        amount = obj->value[0] + obj->value[1] + obj->value[2] + obj->value[3];
        ch->money.copper   += obj->value[0];
        ch->money.silver   += obj->value[1];
        ch->money.gold     += obj->value[2];
        ch->money.platinum += obj->value[3];

        if ( amount > 1 )
        {
        sprintf( buf, "You pick up" );
        if ( obj->value[0] > 0 ) {
            sprintf( buf2, " %d &n&+ycopper&n", obj->value[0] );
            strcat( buf, buf2 );
        }
        if ( obj->value[1] > 0 ) {
            sprintf( buf2, " %d &n&+wsilver&n", obj->value[1] );
            strcat( buf, buf2 );
        }
        if ( obj->value[2] > 0 ) {
            sprintf( buf2, " %d &+Ygold&n", obj->value[2]  );
            strcat( buf, buf2 );
        }
        if ( obj->value[3] > 0 ) {
            sprintf( buf2, " %d &+Wplatinum&n" , obj->value[3]);
            strcat( buf, buf2 );
        }
        strcat( buf, " coins.\n\r" );
        send_to_char( buf, ch );
        }

        extract_obj( obj );
    }
    else
    {
      obj_to_char( obj, ch );
      // Prevent item duplication -- Veygoth
      save_char_obj( ch );
    }

    return;
}

void do_get( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *container;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    bool      found;

    last_fun( "do_get");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "Try as you might, your body refuses to move!\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Get what?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
        /* 'get obj' */
        obj = get_obj_list( ch, arg1, ch->in_room->contents );
        if ( !obj )
        {
            act( "I see no $T&n here.", ch, NULL, arg1, TO_CHAR );
            return;
        }

        if ( ch->carry_number + 1 > MAX_CARRY )
        {
            send_to_char( "You have your hands full.\n\r", ch );
            return;
        }


        get_obj( ch, obj, NULL );
    }
    else
    {
        /* 'get all' or 'get all.obj' */
            OBJ_DATA *obj_next;

        found = FALSE;
        for ( obj = ch->in_room->contents; obj; obj = obj_next )
        {
            obj_next = obj->next_content;

        if ( obj->deleted )
            continue;

        if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
            && can_see_obj( ch, obj ) )
        {
            found = TRUE;
            if ( ch->carry_number + 1 > MAX_CARRY )
            {
                send_to_char( "You have your hands full.\n\r", ch );
                return;
            }

            get_obj( ch, obj, NULL );
        }
        }

        if ( !found ) 
        {
        if ( arg1[3] == '\0' )
                    send_to_char( "You see nothing here.\n\r", ch );
        else
                    act( "You see no $T&n here.", ch, NULL, &arg1[4], TO_CHAR );
        }
    }
    }
    else
    {
    /* 'get ... container' */
    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
        send_to_char( "That doesn't seem possible.\n\r", ch );
        return;
    }

    if ( !( container = get_obj_here( ch, arg2 ) ) )
    {
        act( "You don't see any $T&n here.", ch, NULL, arg2, TO_CHAR );
        return;
    }

    switch ( container->item_type )
    {
    default:
        send_to_char( "Unfortunately, that's not a container.\n\r", ch );
        return;

    case TYPE_QUIVER:
    case TYPE_CONTAINER:
    case TYPE_CORPSE_NPC:
        break;

    case TYPE_CORPSE_PC:
    {
          char      *pd;
          char       name [ MAX_INPUT_LENGTH ];

          pd = container->short_descr;
          pd = one_argument( pd, name );
          pd = one_argument( pd, name );
          pd = one_argument( pd, name );

      }
    }

    if ( ch->position == POS_FIGHTING || ch->fighting ) {
        send_to_char("You're too busy to be doing that!\n\r", ch);
        return;
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
        act( "The $d&n is fastened shut.", ch, NULL, container->name, TO_CHAR );
        return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
        /* 'get obj container' */
        obj = get_obj_list( ch, arg1, container->contains );
        if ( !obj )
        {
            act( "You search the $T&n with little success.",
            ch, NULL, arg2, TO_CHAR );
            return;
        }
        if ( ch->carry_number + 1 > MAX_CARRY )
        {
            send_to_char( "You have your hands full.\n\r", ch );
            return;
        }
        get_obj( ch, obj, container );
    }
    else
    {
        /* 'get all container' or 'get all.obj container' */
            OBJ_DATA *obj_next;

        found = FALSE;
        for ( obj = container->contains; obj; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( obj->deleted )
                continue;

            if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
                && can_see_obj( ch, obj ) )
            {
                found = TRUE;
                if ( ch->carry_number + 1 > MAX_CARRY )
                {
                    send_to_char( "You have your hands full.\n\r", ch );
                    return;
                }
                get_obj( ch, obj, container );
                    if( obj->item_type == TYPE_CORPSE_PC )
                    {
                      save_char_obj( ch );
                      save_corpses();
                    }
            }
        }

        if ( !found )
        {
        if ( arg1[3] == '\0' )
                    act( "You see nothing in the $T&n.",
            ch, NULL, arg2, TO_CHAR );
        else
                    act( "You see nothing like that in the $T&n.",
            ch, NULL, arg2, TO_CHAR );
        }
    }
    }

    return;
}

void do_put( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *container;
    OBJ_DATA *obj;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_put");
    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Put what into what?\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if ( !( container = get_obj_here( ch, arg2 ) ) )
    {
        act( "You see no $T&n here.", ch, NULL, arg2, TO_CHAR );
        return;
    }

    /*  Added put <missileweap> <quiver> */
    if( container->item_type != TYPE_CONTAINER &&
        container->item_type != TYPE_QUIVER )
    {
        send_to_char( "That's not a container.\n\r", ch );
        return;
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
        act( "The $d&n is &n&+ystrapped&n shut.", ch, NULL, container->name, TO_CHAR );
        return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
    /* 'put obj container' */
    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( obj == container )
    {
        send_to_char( "You can't fold it into itself.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't seem to let go of it.\n\r", ch );
        return;
    }

    if (  get_obj_weight( obj ) + get_obj_weight( container )
        - container->weight > container->value[0] )
    {
        send_to_char( "It won't fit.\n\r", ch );
        return;
    }

    /* Added put <missileweap> <quiver> */
    if ( container->item_type == TYPE_QUIVER 
        && obj->item_type != TYPE_MISSILEWEAP)
    {
        act( "$p&n doesn't belong in $P&n.", ch, obj, container, TO_CHAR );
        return;
    }

    obj_from_char( obj );
    obj_to_obj( obj, container );
    act( "You put $p&n in $P&n.", ch, obj, container, TO_CHAR );
    act( "$n&n slips $p&n into $P&n.", ch, obj, container, TO_ROOM );
    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_GETPUT ))
        setoff_trap( ch, obj );
    }
    else
    {
    /* 'put all container' or 'put all.obj container' */
        OBJ_DATA *obj_next;
        bool stuff = FALSE;

        for ( obj = ch->carrying; obj; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( obj->deleted )
                continue;

            if ( container->item_type == TYPE_QUIVER 
                && obj->item_type != TYPE_MISSILEWEAP )
                continue;

            if( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
                && can_see_obj( ch, obj )
                && obj->wear_loc == WEAR_NONE
                && obj != container
                && can_drop_obj( ch, obj )
                && get_obj_weight( obj ) + get_obj_weight( container )
                  <= container->value[0] )
            {
                obj_from_char( obj );
                obj_to_obj( obj, container );
                stuff = TRUE;
                act( "You put $p&n in $P&n.", ch, obj, container, TO_CHAR );
//                act( "$n&n puts $p&n in $P&n.", ch, obj, container, TO_ROOM );
                if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_GETPUT ))
                {
                    setoff_trap( ch, obj );

                    if( ch->deleted || ch->position == POS_DEAD )
                        return;
                }
            }
            else if ( get_obj_weight( obj ) + get_obj_weight( container )
                  > container->value[0] && obj != container ) {
                act("$p&n won't fit into $P&n.", ch, obj, container, TO_CHAR);
            }
        }
        if ( stuff )
            act( "$n&n puts some stuff in $P&n.", ch, NULL, container, TO_ROOM );
    }

    return;
}

void do_drop( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj, *cash;
    char      arg [ MAX_INPUT_LENGTH ];
    bool      found;

    last_fun( "do_drop");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You muscles won't respond!\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Drop what?\n\r", ch );
    return;
    }

    if ( is_number( arg ) )
    {
    /* 'drop NNNN coins' */
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
      if ( ch->money.copper < amount )
      {
            send_to_char( "You haven't got that many &n&+ycopper&n coins.\n\r", ch );
        return;
      }
      ch->money.copper -= amount;
          cash = create_money( amount, 0, 0, 0 );
          obj_to_room( cash, ch->in_room );
          cash->fly_level = ch->fly_level;
        }
        else if( !str_prefix( arg, "silver" ))
        {
      if ( ch->money.silver < amount )
      {
            send_to_char( "You haven't got that many &n&+wsilver&n coins.\n\r", ch );
        return;
      }
      ch->money.silver -= amount;
          obj_to_room( create_money( 0, amount, 0, 0 ), ch->in_room );
        }
        else if( !str_prefix( arg, "gold" ))
        {
      if ( ch->money.gold < amount )
      {
        send_to_char( "You haven't got that many &+Ygold&n coins.\n\r", ch );
        return;
      }
      ch->money.gold -= amount;
          obj_to_room( create_money( 0, 0, amount, 0 ), ch->in_room );
        }
        else if( !str_prefix( arg, "platinum" ))
        {
      if ( ch->money.platinum < amount )
      {
        send_to_char( "You haven't got that many &+Wplatinum&n coins.\n\r", ch );
        return;
      }
      ch->money.platinum -= amount;
          obj_to_room( create_money( 0, 0, 0, amount ), ch->in_room );
        }
        else
    {
            send_to_char( "They haven't minted that type of &+Lcoin&n yet.\n\r", ch );
        return;
    }

/* Veygoth - Disabled merging of coin types.  This should eventually be re-enabled
    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
        obj_next = obj->next_content;

        if ( obj->deleted )
            continue;

        switch ( obj->pIndexData->vnum )
        {
        case OBJ_VNUM_MONEY_ONE:
        amount += 1;
        extract_obj( obj );
        break;

        case OBJ_VNUM_MONEY_SOME:
        amount += obj->value[0];
        extract_obj( obj );
        break;
        }
    }
*/
        send_to_char( "Ok.\n\r", ch );
        act( "$n&n drops some &n&+wcoins&n.", ch, NULL, NULL, TO_ROOM );
    return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
      /* 'drop obj' */
      if ( !( obj = get_obj_carry( ch, arg ) ) )
      {
          send_to_char( "You do not have that item.\n\r", ch );
          return;
      }

      if ( !can_drop_obj( ch, obj ) )
      {
            send_to_char( "You can't release your grip on it.\n\r", ch );
            return;
      }

      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );

      // Prevent item duping - Veygoth
      save_char_obj( ch );

      obj->fly_level = ch->fly_level;
      act( "You drop $p&n.", ch, obj, NULL, TO_CHAR );
      act( "$n&n discards $p&n.", ch, obj, NULL, TO_ROOM );
      if( IS_OBJ_STAT( obj, ITEM_TRANSIENT ))
      {
        act( "$p&n crumbles to dust.", ch, obj, NULL, TO_ALL );
        extract_obj( obj );
      }
    }
    else
    {
    /* 'drop all' or 'drop all.obj' */
    OBJ_DATA *obj_next;

    found = FALSE;
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
            obj_next = obj->next_content;

        if ( obj->deleted )
            continue;

        if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
        && can_see_obj( ch, obj )
        && obj->wear_loc == WEAR_NONE
        && can_drop_obj( ch, obj ) )
        {
          found = TRUE;
          obj_from_char( obj );
          obj_to_room( obj, ch->in_room );
          act( "You drop $p&n.", ch, obj, NULL, TO_CHAR );
          act( "$n&n drops $p&n.", ch, obj, NULL, TO_ROOM );
          if( IS_OBJ_STAT( obj, ITEM_TRANSIENT ))
          {
            act( "$p&n crumbles to dust.", ch, obj, NULL, TO_ALL );
            extract_obj( obj );
          }
        }
    }

    if ( !found )
    {
        if ( arg[3] == '\0' )
            send_to_char( "You are not carrying anything.", ch );
        else
                act( "You are not carrying any $T&n.",
            ch, NULL, &arg[4], TO_CHAR );
    }
    }

    return;
}

void do_give( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_give");
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Give what to whom?\n\r", ch );
        return;
    }

    if ( is_number( arg1 ) )
    {
        /* 'give NNNN coins victim' */
        char buf   [ MAX_STRING_LENGTH ];
        char arg3  [ MAX_INPUT_LENGTH ];
        int  amount;

        amount   = atoi( arg1 );
        if ( amount <= 0 )
        {
            send_to_char( "Sorry, you can't do that.\n\r", ch );
            return;
        }

        one_argument( argument, arg3 );

        if ( arg3[0] == '\0' )
        {
            send_to_char( "Give what to whom?\n\r", ch );
            return;
        }

        if ( !( victim = get_char_room( ch, arg3 ) ) )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }

        if( !str_prefix( arg2, "copper" ))
        {
          if ( ch->money.copper < amount )
          {
            send_to_char( "You haven't got that many &+ycopper&n coins.\n\r", ch );
            return;
          }
          ch->money.copper -= amount;
          act( "You give $N&n some &+ycopper&n.",  ch, NULL, victim, TO_CHAR );
          sprintf( buf, "$n&n gives you %d &+ycopper&n.", amount );
          act( buf, ch, NULL, victim, TO_VICT );
          act( "$n&n gives $N&n some &+ycopper&n.",  ch, NULL, victim, TO_NOTVICT );
          prog_bribe_trigger( victim, ch, amount );
          // If this is not for a quest, put the money in the target's pocket.
          // If it is for a quest, don't. That way you can't kill the target to
          // get your money back once the quest is complete.
          if( !check_quest( ch, victim, NULL, amount ))
              victim->money.copper += amount;
          // Prevent money duping
          save_char_obj( ch );
          save_char_obj( victim );
          return;
        }
        else if( !str_prefix( arg2, "silver" ))
        {
          if ( ch->money.silver < amount )
          {
            send_to_char( "You haven't got that many silver coins.\n\r", ch );
            return;
          }
          ch->money.silver -= amount;
          act( "You give $N&n some silver.",  ch, NULL, victim, TO_CHAR );
          sprintf( buf, "$n&n gives you %d silver.", amount );
          act( buf, ch, NULL, victim, TO_VICT );
          act( "$n&n gives $N&n some silver.",  ch, NULL, victim, TO_NOTVICT );
          prog_bribe_trigger( victim, ch, amount * 10);
          if( !check_quest( ch, victim, NULL, (amount * 10) ))
             victim->money.silver += amount;
          // Prevent money duping
          save_char_obj( ch );
          save_char_obj( victim );
          return;
        }
        else if( !str_prefix( arg2, "gold" ))
        {
          if ( ch->money.gold < amount )
          {
            send_to_char( "You haven't got that many &+Ygold&n coins.\n\r", ch );
            return;
          }
          ch->money.gold -= amount;
          act( "You give $N&n some &+Ygold&n.",  ch, NULL, victim, TO_CHAR );
          sprintf( buf, "$n&n gives you %d &+Ygold&n.", amount );
          act( buf, ch, NULL, victim, TO_VICT );
          act( "$n&n gives $N&n some &+Ygold&n.",  ch, NULL, victim, TO_NOTVICT );
          prog_bribe_trigger( victim, ch, amount *100);
          if( !check_quest( ch, victim, NULL, (amount * 100) ))
            victim->money.gold += amount;
          // Prevent money duping
          save_char_obj( ch );
          save_char_obj( victim );
          return;
        }
        else if( !str_prefix( arg2, "platinum" ))
        {
          if ( ch->money.platinum < amount )
          {
            send_to_char( "You haven't got that many &+Wplatinum&n coins.\n\r", ch );
            return;
          }
          ch->money.platinum -= amount;
          act( "You give $N&n some &+Wplatinum&n.",  ch, NULL, victim, TO_CHAR );
          sprintf( buf, "$n&n gives you %d &+Wplatinum&n.", amount );
          act( buf, ch, NULL, victim, TO_VICT );
          act( "$n&n gives $N&n some &+Wplatinum&n.",  ch, NULL, victim, TO_NOTVICT );
          prog_bribe_trigger( victim, ch, amount* 1000 );
          if( !check_quest( ch, victim, NULL, (amount * 1000) ))
            victim->money.platinum += amount;
          // Prevent money duping
          save_char_obj( ch );
          save_char_obj( victim );
          return;
        }
        else
        {
            send_to_char( "You don't have any of _that_ type of coin yet.\n\r", ch );
            return;
        }

    }

    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
        send_to_char( "You must remove it first.\n\r", ch );
        return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
        return;
    }

/*
 * commented out, screwing up quests - Lithos
 *       || ( IS_NPC( victim ) && ( victim->pIndexData->pShop ) ) )
 *
 * How silly of shopkeepers to refuse blessed items... previously vampires would
 * refuse blessed items... now all undead types refuse blessed items.
 */
    if( IS_OBJ_STAT( obj, ITEM_BLESS ) && IS_UNDEAD( victim ))
    {
        act( "$N&n refuses the $p&n.", ch, obj, victim, TO_CHAR );
        act( "$n&n tries to give $N&n a $p&n but $E refuses.",
            ch, obj, victim, TO_ROOM );
        act( "You refuse to accept the $p&n from $n&n.",
            ch, obj, victim, TO_VICT );
        return;
    }

    if ( victim->carry_number + 1 > MAX_CARRY )
    {
        act( "$N&n has $S hands full.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
        act( "$N&n can't carry that much weight.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
        act( "$N&n can't see it.", ch, NULL, victim, TO_CHAR );
        return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );

    // Prevent item duplication -- Veygoth
    save_char_obj( ch );
    save_char_obj( victim );

    act( "You give $p&n to $N&n.", ch, obj, victim, TO_CHAR    );
    act( "$n&n gives you $p&n.",   ch, obj, victim, TO_VICT    );
    act( "$n&n gives $p&n to $N&n.", ch, obj, victim, TO_NOTVICT );

    prog_give_trigger( victim, ch, obj );

    if ( is_artifact( obj->pIndexData->vnum ) )
    {
        char buf[MAX_STRING_LENGTH];
        sprintf( buf, "%s has given artifact %d to %s", ch->name, obj->pIndexData->vnum, victim->name );
        log_string( buf );
    }

    if( !IS_NPC( victim ))
      return;

    // Destroy the item so they can't kill the mob for the item.
    if( check_quest( ch, victim, obj, 0 ))
      extract_obj( obj );
    return;
}

// Check_quest, checks whether an item given is part of a quest.
// This function should be passed an obj *or* a money amount.
// - Veygoth
bool check_quest( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int money )
{
    QUEST_INDEX_DATA *quest;
    QUEST_DATA *quests, *reset_quest;
    QUEST_ITEM *questitem, *item1, *item2;
    int qcount = 0;
    bool value = FALSE;
    bool qcomplete;
    bool needs_reset = FALSE;
    char buf2[80];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *newobj;
    bool this_obj_done = FALSE;

    last_fun( "check_quest");
    if( !IS_NPC( victim ))
      return value;

    if( !money && !obj )
    {
       log_string( "Check_quest function called with no data" );
       return value;
    }

    if( money && obj )
    {
       log_string( "Check_quest function called with too much data" );
       return value;
    }

    /* Quest Stuff - Veygoth */
    /* modified by Lithos */
    for( quest = quest_first; quest; quest = quest->next )
    {
       if( quest->vnum == victim->pIndexData->vnum )
       {
          sprintf( buf, "%s&n has quest data.",victim->short_descr);
          wiznet( NULL, WIZ_QUESTS, 0, buf );
     
         /* let's just check all the quests first, then test for completion afterward */
         for( quests = quest->quests; quests; quests = quests->next )
         {
              qcount++;
              sprintf(buf,"Give items: ");
              for ( questitem = quests->give; questitem; questitem = questitem->next) {
                sprintf(buf2, "%d ", questitem->value);
                strcat(buf, buf2);
              }
              wiznet( NULL, WIZ_QUESTS, 0, buf);
              this_obj_done = FALSE;
              for( questitem = quests->receive; questitem && !this_obj_done ; questitem = questitem->next)
              {
                 sprintf(buf,"  %s&n receives item type %d, value %d.",victim->short_descr, questitem->type, questitem->value);
                 wiznet(NULL, WIZ_QUESTS, 0, buf);
                 if( questitem->complete )
                 {
                   continue;
                 }

                 if( obj )
                 {
                   if( questitem->type == 0 && questitem->value == obj->pIndexData->vnum )
                   {
                     questitem->complete = TRUE;
                     value = TRUE;
                     sprintf(buf,"%s has given quest item %d to %s&n.",
                       ch->name, questitem->value,victim->short_descr);
                     wiznet(NULL, WIZ_QUESTS, 0, buf);
                     this_obj_done = TRUE;
                   }
/* //this type is money, dont need to process this here
                   else if( questitem->type == 1 && questitem->value == obj->item_type )
                   {
                     questitem->complete = TRUE;
                     value = TRUE;
                   }
//
*/
                    //obj doesnt match this particular questitem
                 }
                 else if( money )
                 {
                   if( questitem->type == 1 && questitem->value <= money )
                   {
                     questitem->complete = TRUE;
                     value = TRUE;
                     sprintf(buf,"  &n%s&n has given %d copper to %s&n.",
                       ch->name, questitem->value, victim->short_descr);
                     wiznet(NULL, WIZ_QUESTS, 0, buf);
                   }
                 }
                 else
                 { // can't imgaine how we would get here
                   log_string( "Check_quest function way buggy!" );
                   sprintf(buf, "check_quest function way buggy!");
                   wiznet( NULL, WIZ_QUESTS, 0, buf);
                   return value;
                 }
              }
          } // end of quests loop

         for( quests = quest->quests; quests; quests = quests->next ) {
             if (needs_reset)
                 break;
             qcomplete = TRUE;
             for( questitem = quests->receive; questitem; questitem = questitem->next) {
                 if( !questitem->complete ) {
                     qcomplete = FALSE;
                     break;
                 }
             }
             if (qcomplete) {
                 sprintf(buf,"%s&n completed a quest for %s&n.",ch->name,victim->short_descr);
                 wiznet( NULL, WIZ_QUESTS, 0, buf);
                 log_string( buf );
                 if( quests->complete )
                     send_to_char( quests->complete, ch );
                 for( item1 = quests->give; item1; item1 = item1->next )
                 {
                     if( item1->complete )
                       continue;
                     switch (item1->type) {
                     default:
                         break;
                     case 0:
                       newobj = create_object( get_obj_index( item1->value), 0 );
                       act( "$n&n hands $p&n to $N&n.", victim, newobj, ch, TO_ROOM );
                       act( "$n&n gives you $p&n.", ch, newobj, victim, TO_VICT );
                       obj_to_char( newobj, ch );
                       item1->complete = TRUE;
                       break;
                     case 1:
                       act( "$N&n gives some &n&+wcoins&n to $n&n.", victim, newobj, ch, TO_NOTVICT );
                       receive_cash( ch, item1->value );
                       item1->complete = TRUE;
                       break;
                     case 2:
                       send_to_char( "Questing for skills is not yet enabled.\n\r", ch );
                       item1->complete = TRUE;
                       break;
                     case 3:
                       send_to_char( "You gain experience!\n\r", ch );
                       gain_exp( ch, item1->value );
                       item1->complete = TRUE;
                       break;
                     case 4:
                       send_to_char( "Questing for spells is not yet enabled.\n\r", ch );
                       break;
                     case 5:
                       send_to_char( "Questing for cast spells is not yet enabled.\n\r", ch );
                       break;
                     }
                     save_char_obj( ch );
                   }
                   if( quests->disappear )
                   {
                     send_to_char( quests->disappear, ch );
                     extract_char( victim, TRUE );
                     last_fun( "check_quest");
                   }
                   needs_reset = TRUE;
                   reset_quest = quests;
                 } //end if(qcomplete)
              } //end of quests loop
         if (needs_reset) {
             for( quests = quest->quests; quests; quests = quests->next ) {
                 for( questitem = quests->receive; questitem; questitem = questitem->next) {
                    for (item2 = reset_quest->receive; item2; item2 = item2->next)
                        if (item2->value == questitem->value && item2->type == questitem->type)
                            questitem->complete = FALSE;
                 }
                 for( questitem = quests->give; questitem; questitem = questitem->next) {
                   for (item2 = reset_quest->give; item2; item2 = item2->next)
                     if (item2->value == questitem->value && item2->type == questitem->type)
                        questitem->complete = FALSE;
                 }
             }
         }
         sprintf(buf, "Done processing %d quests for %s&n.",qcount,victim->short_descr);
         wiznet( NULL, WIZ_QUESTS, 0, buf );
         return value; //we matched the one mob, why cycle through the rest?
       } // end if(vnum) statement
    }
    return value;
}


void do_fill( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    char      arg [ MAX_INPUT_LENGTH ];
    bool      found;

    last_fun( "do_fill");
    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Fill what?\n\r", ch );
    return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain;
    fountain = fountain->next_content )
    {
        if (fountain->fly_level != ch->fly_level) continue;
    if ( fountain->item_type == TYPE_DRINK_CON )
    {
        found = TRUE;
        break;
    }
    }

    if ( !found )
    {
    send_to_char( "There is nothing to fill from here!\n\r", ch );
    return;
    }

    if ( obj->item_type != TYPE_DRINK_CON )
    {
    send_to_char( "You can't fill that.\n\r", ch );
    return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
    {
    send_to_char( "There is already another liquid in it.\n\r", ch );
    return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
    send_to_char( "Your container is full.\n\r", ch );
    return;
    }

    act( "You fill $p&n.", ch, obj, NULL, TO_CHAR );
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    return;
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       amount;
    int       liquid;

    last_fun( "do_drink");
    if ( !check_blind( ch ) )
        return;

    if ( ch->fighting || ch->position == POS_FIGHTING )
    {
        send_to_char( "You can't drink while you're fighting!\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
        {
            if ( obj->deleted )
                continue;

            if ( obj->item_type == TYPE_DRINK_CON )
                break;
        }

        if ( !obj )
        {
            send_to_char( "Drink what?\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !( obj = get_obj_here( ch, arg ) ) )
        {
            send_to_char( "You can't find it.\n\r", ch );
            return;
        }
    }

    // Allow bards to get twice as drunk as other classes - Veygoth
    if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch )
         && ch->pcdata->condition[COND_DRUNK] > 15 && ch->class != CLASS_BARD
         && number_percent() < get_curr_agi( ch ) - ch->pcdata->condition[COND_DRUNK] )
    {
        send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
        return;
    }
    else if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch )
             && ch->pcdata->condition[COND_DRUNK] > 25 && ch->class == CLASS_BARD
         && number_percent() < get_curr_agi( ch ) - ch->pcdata->condition[COND_DRUNK] )
    {
        send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
        return;

    }

    switch ( obj->item_type )
    {
    default:
        send_to_char( "You can't drink from that.\n\r", ch );
        break;

    case TYPE_DRINK_CON:
        // -1 Means a container never goes empty.
        if ( obj->value[1] <= 0 && obj->value[1] != -1)
        {
            send_to_char( "It is already &+Lempty&n.\n\r", ch );
            return;
        }

    /* No drinking if you're full - Govno */
    if ( ( !IS_IMMORTAL( ch ) ) && (
         ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] > 40 ) ||
         ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 50  ) ) )
    {
        send_to_char( "You couldn't possibly drink any more.\n\r", ch );
        return;
    }


        if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
        {
            bug( "Do_drink: bad liquid number %d.", liquid );
            liquid = obj->value[2] = 0;
        }

        act( "You drink $T from $p&n.",
            ch, obj, liq_table[liquid].liq_name, TO_CHAR );
        act( "$n&n drinks $T from $p&n.",
            ch, obj, liq_table[liquid].liq_name, TO_ROOM );

        amount = number_range( 3, 10 );
        if( obj->value[0] != -1 )
          amount = UMIN( amount, obj->value[1] );

        gain_condition( ch, COND_DRUNK, amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );

        if ( !IS_UNDEAD( ch ) )
        {
            gain_condition( ch, COND_FULL,
                         amount * liq_table[liquid].liq_affect[COND_FULL] );
            if ( has_affect( ch, 0, spl_thirst, 0 ) )
            {
                gain_condition( ch, COND_THIRST,
                   ( amount * liq_table[liquid].liq_affect[COND_THIRST ] ) /12 );
                send_to_char( "That doesn't taste as &+bwet&n as it used to.\n\r", ch );
            }
            else
                gain_condition( ch, COND_THIRST,
                    amount * liq_table[liquid].liq_affect[COND_THIRST ] );
        }
        else
            /* If blood */
            if ( liquid == 13 )
            {
                gain_condition( ch, COND_FULL, amount * 2 );
                gain_condition( ch, COND_THIRST, amount );
            }

    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK ] > 10 )
            send_to_char( "You feel &n&+gdrunk&n.\n\r", ch );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL  ] > 20 )
            send_to_char( "You are &n&+yfull&n.\n\r", ch );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] > 20 )
            send_to_char( "You do not feel &n&+cth&+Ci&n&+cr&+Cst&n&+cy&n.\n\r", ch );

        if ( obj->value[3] != 0
            && !CHECK_IMM( ch, RIS_POISON ) )
        {
            /* The stuff was poisoned ! */
            AFFECT_DATA af;

            send_to_char( "You choke and gag.\n\r", ch );
            act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
            af.skill     = 0;
            af.spell     = spl_poison;
            af.song      = 0;
            af.duration  = 3 * amount;
            af.location  = APPLY_STR;
            af.modifier  = -2;
            set_bitvector( &af, AFF_POISON);
            affect_join( ch, &af );
        }

        /* HOLY_WATER and UNHOLY_WATER effects Govno */
        if ( ( IS_GOOD( ch ) && obj->value[2] == 27 ) || // holy Govno
             ( IS_EVIL( ch ) && obj->value[2] == 28 ) )  // unholy Govno
        {
            int heal = dice( 1, 8 );
            if ( ch->hit < get_max_hit( ch ) )
            {
                ch->hit = UMIN( ch->hit + heal, get_max_hit( ch ) );
                update_pos( ch );
                send_to_char( "You feel a little better!\n\r", ch );
            }
        }
        if ( ( IS_EVIL( ch ) && obj->value[2] == 27 ) || // holy Govno
             ( IS_GOOD( ch ) && obj->value[2] == 28 ) )  // unholy Govno
        {
            int harm = dice( 1, 10 );
            ch->hit = UMAX( ch->hit - harm, -10 );
            send_to_char( "You choke and feel as if you'd swallowed boiling oil!\n\r", ch );
            update_pos( ch );
        }
        /* End (UN)HOLY_WATER effects Govno */

        // -1 Means a container never goes empty.
        if( obj->value[1] != -1 )
        {
      obj->value[1] -= amount;
      if ( obj->value[1] <= 0 )
      {
            send_to_char( "The container is now &+Lempty&n.\n\r", ch );
        obj->value[1] = 0;
      }
        }
    break;
    }

    return;
}

void do_eat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_eat");
    if ( !check_blind( ch ) )
        return;

    if ( ch->fighting || ch->position == POS_FIGHTING )
    {
    send_to_char( "You can't eat while you're fighting!\n\r", ch );
    return;
    }

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Eat what?\n\r", ch );
    return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    if ( !IS_IMMORTAL( ch ) )
    {
    if ( obj->item_type != TYPE_FOOD && obj->item_type != TYPE_PILL )
    {
        send_to_char( "That's not edible.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
    {   
        send_to_char( "You are too full to eat more.\n\r", ch );
        return;
    }
    }

    act( "You consume $p&n.", ch, obj, NULL, TO_CHAR );
    act( "$n&n inhales $p&n.", ch, obj, NULL, TO_ROOM );

    switch ( obj->item_type )
    {

    case TYPE_FOOD:
    if ( !IS_NPC( ch ) )
    {
        int condition;

        condition = ch->pcdata->condition[COND_FULL];
        if ( !IS_UNDEAD( ch ) )
            gain_condition( ch, COND_FULL, obj->value[0] );
        if ( ch->pcdata->condition[COND_FULL] > 40 )
            send_to_char( "You are full.\n\r", ch );
        else if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 0 )
        send_to_char( "You are no longer hungry.\n\r", ch );
    }

    if ( obj->value[3] != 0
        && !CHECK_IMM( ch, RIS_POISON ) )
    {
        /* The stuff was poisoned! */
        AFFECT_DATA af;

        act( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
        send_to_char( "You choke and gag.\n\r", ch );

            af.skill     = 0;
        af.spell     = spl_poison;
            af.song      = 0;
        af.duration  = 2 * obj->value[0];
        af.location  = APPLY_STR;
        af.modifier  = -2;
        set_bitvector( &af, AFF_POISON);
        affect_join( ch, &af );
    }
    break;

    case TYPE_PILL:
    {
        obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
        obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
        obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
        obj_cast_spell( obj->value[4], obj->value[0], ch, ch, NULL );
    }
    break;
    }

    if ( !IS_NPC( ch )
    || ( IS_NPC( ch ) && has_affect( ch, 0, spl_charm_person, 0 ) ) )
        extract_obj( obj );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( !( obj = get_eq_char( ch, iWear ) ) )
    return TRUE;

    if ( !fReplace )
    return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
        act( "Try as you might, you can't remove $p&n.", ch, obj, NULL, TO_CHAR );
    return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n&n stops using $p&n.", ch, obj, NULL, TO_ROOM );
    act( "You remove $p&n.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}

// Checks to see if the object can be worn.  If not, it
// returns the appropriate message - Veygoth
bool wearable_by( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if( IS_IMMORTAL( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ))
         return TRUE;

    if( ch->class == CLASS_MONK )
    {
         if( obj->weight > (1 + (ch->level / 15 )) )
         {
           act( "$p&n is too heavy for you to use.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
    }

    if( IS_SET( obj->anti_flags, USE_ANYONE ) || obj->anti_flags == 0 )
         return TRUE;

    switch( ch->sex )
    {
       case SEX_MALE:
         if( IS_SET( obj->anti_flags2, USE_ANTIMALE ))
         {
           act( "$p&n is too dainty for one of such as yourself.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case SEX_FEMALE:
         if( IS_SET( obj->anti_flags2, USE_ANTIFEMALE ))
         {
           act( "$p&n is too bulky and poorly designed for one as skilled as you.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case SEX_NEUTRAL:
         if( IS_SET( obj->anti_flags2, USE_ANTINEUTER ))
         {
           act( "$p&n was not designed for your form.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       default:
         act( "$p&n cannot be used by you because your sex is buggy.", ch, obj, NULL, TO_CHAR );
         return FALSE;
         break;
    }

    switch( ch->class )
    {
       default:
         break;
       case CLASS_WARRIOR:
         if( !IS_SET( obj->anti_flags, USE_WARRIOR ))
         {
           act( "You won't abide such inferiority as found in $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_RANGER:
       case CLASS_HUNTER:
         if( !IS_SET( obj->anti_flags, USE_RANGER ))
         {
           act( "Your professionalism prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_PALADIN:
         if( !IS_SET( obj->anti_flags, USE_PALADIN ))
         {
           act( "Your convictions prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_ANTIPALADIN:
         if( !IS_SET( obj->anti_flags, USE_ANTI ))
         {
           act( "Your skill and pride prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_CLERIC:
         if( !IS_SET( obj->anti_flags, USE_CLERIC ))
         {
           act( "Your god prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_DRUID:
         if( !IS_SET( obj->anti_flags, USE_DRUID ))
         {
           act( "Your path prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_SHAMAN:
         if( !IS_SET( obj->anti_flags, USE_SHAMAN ))
         {
           act( "You lack the proper know-how to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_SORCERER:
         if( !IS_SET( obj->anti_flags, USE_SORCERER ))
         {
           act( "You neglected use of $p&n in deference to your magics.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_ELEMENT_AIR:
       case CLASS_ELEMENT_EARTH:
       case CLASS_ELEMENT_FIRE:
       case CLASS_ELEMENT_WATER:
         if( !IS_SET( obj->anti_flags, USE_ELEMENTAL ))
         {
           act( "You have better things to waste time on then $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_THIEF:
         if( !IS_SET( obj->anti_flags, USE_THIEF ))
         {
           act( "Using $p&n would only hinder your abilities.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_PSIONICIST:
         if( !IS_SET( obj->anti_flags, USE_PSI ))
         {
           act( "The thought of using $p&n makes your brain hurt.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_ASSASSIN:
         if( !IS_SET( obj->anti_flags, USE_ASSASSIN ))
         {
           act( "You never studied the art behind $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_MERCENARY:
         if( !IS_SET( obj->anti_flags, USE_MERCENARY ))
         {
           act( "Your training never addressed using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case CLASS_MYSTIC:
       case CLASS_MONK:
         if( !IS_SET( obj->anti_flags, USE_MONK ))
         {
           act( "You cannot use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
    }

    switch( ch->race )
    {
       default:
         break;
       case RACE_HUMAN:
         if( IS_SET( obj->anti_flags, USE_NOHUMAN ))
         {
           act( "Your race is ill fit to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_GREYELF:
         if( IS_SET( obj->anti_flags, USE_NOGREYELF ))
         {
           act( "Your race is too weak to handle $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_HALFELF:
         if( IS_SET( obj->anti_flags, USE_NOHALFELF ))
         {
           act( "Your bastardized nature prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_DWARF:
         if( IS_SET( obj->anti_flags, USE_NODWARF ))
         {
           act( "Your body wasn't designed to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_HALFLING:
         if( IS_SET( obj->anti_flags, USE_NOGNOME ))
         {
           act( "Just contemplating $p&n makes you long for food.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_BARBARIAN:
         if( IS_SET( obj->anti_flags, USE_NOBARBARIAN ))
         {
           act( "Your size and mass prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_DUERGAR:
         if( IS_SET( obj->anti_flags, USE_NODUERGAR ))
         {
           act( "Of what use is $p&n to a &n&+rduergar&n?", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_DROW:
         if( IS_SET( obj->anti_flags, USE_NODROW ))
         {
           act( "The design of $p&n is too inferior for use by a &n&+mdrow&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_TROLL:
         if( IS_SET( obj->anti_flags, USE_NOTROLL ))
         {
           act( "You try to use $p&n, but become quickly frustrated.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_OGRE:
         if( IS_SET( obj->anti_flags, USE_NOOGRE ))
         {
           act( "You can't quite figure out how to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_ORC:
         if( IS_SET( obj->anti_flags, USE_NOORC ))
         {
           act( "Obviously, $p&n was not crafted for an &+Lorc&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_CENTAUR:
         if( IS_SET( obj->anti_flags2, USE_NOCENTAUR ))
         {
           act( "Your find $p&n's design unsuitable for a &n&+gcen&+Ltaur&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_GITHYANKI:
         if( IS_SET( obj->anti_flags2, USE_NOGITHYANKI ))
         {
           act( "You find $p&n to be unfitting for a &+Wgith&+Gyanki&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_GITHZERAI:
         if( IS_SET( obj->anti_flags2, USE_NOGITHZERAI ))
         {
           act( "Your nature prevents you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_MINOTAUR:
         if( IS_SET( obj->anti_flags2, USE_NOMINOTAUR ))
         {
           act( "Your distinctive build prevents your use of $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_GOBLIN:
         if( IS_SET( obj->anti_flags2, USE_NOGOBLIN ))
         {
           act( "Your stature and build make it impossible for you to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_RAKSHASA:
         if( IS_SET( obj->anti_flags2, USE_NORAKSHASA ))
         {
           act( "Your feline structure inhibits you from using $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACE_FLIND:
         if( IS_SET( obj->anti_flags2, USE_NOFLIND ))
         {
           act( "Your canine features hinder the use of $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
    }

    switch( race_table[ch->race].racewar_side )
    {
       case RACEWAR_GOOD:
         if( IS_SET( obj->anti_flags, USE_ANTIGOODRACE ))
         {
           act( "Your &n&+cs&+Co&n&+cu&+Cl&n is too &+Wpure&n to use $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       case RACEWAR_EVIL:
         if( IS_SET( obj->anti_flags, USE_ANTIEVILRACE ))
         {
           act( "Your &+Lblack &n&+rhe&+Ra&n&+rr&+Rt&n prevents use of $p&n.", ch, obj, NULL, TO_CHAR );
           return FALSE;
         }
         break;
       default:
         break;
    }

    return TRUE;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    last_fun( "wear_obj");
    if( !wearable_by( ch, obj ))
        return;

    if ( CAN_WEAR( obj, ITEM_TAIL )
     && IS_SET(race_table[ch->race].parts, PART_TAIL ) )
    {
    if ( !remove_obj( ch, WEAR_TAIL, fReplace ) )
        return;
        act( "You wear $p&n on your &+Ltail&n.", ch, obj, NULL, TO_CHAR );
        act( "$n&n wears $p&n on $s &+Ltail&n.", ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_TAIL );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_HORNS )
     && IS_SET(race_table[ch->race].parts, PART_HORNS ) )
    {
    if ( !remove_obj( ch, WEAR_HORNS, fReplace ) )
        return;
        act( "You wear $p&n on your &+Whorns&n.", ch, obj, NULL, TO_CHAR );
        act( "$n&n wears $p&n on $s &+Whorns&n.", ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_HORNS );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_EYES ) )
    {
    if ( !remove_obj( ch, WEAR_EYES, fReplace ) )
        return;
        act( "You place $p&n over your eyes.", ch, obj, NULL, TO_CHAR );
        act( "$n&n places $p&n over $s eyes.", ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_EYES );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) && ch->race != RACE_THRIKREEN )
    {
    if ( get_eq_char( ch, WEAR_FINGER_L )
    &&   get_eq_char( ch, WEAR_FINGER_R )
    &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
    &&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
            return; 

    if ( !get_eq_char( ch, WEAR_FINGER_L ) )
    {
            act( "You slip $p&n onto a finger of your left hand.",  ch, obj, NULL, TO_CHAR );
            act( "$n&n slips $p&n onto a finger of $s left hand.",    ch, obj, NULL, TO_ROOM );
            equip_char( ch, obj, WEAR_FINGER_L );
            return;
    }

    if ( !get_eq_char( ch, WEAR_FINGER_R ) )
    {
            act( "You slide $p&n onto a finger of your right hand.", ch, obj, NULL, TO_CHAR );
            act( "$n&n slides $p&n onto a finger of $s right hand.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_FINGER_R );
        return;
    }

    bug( "Wear_obj: no free finger.", 0 );
        send_to_char( "Anymore rings and you would severly hinder finger movement.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
    if ( get_eq_char( ch, WEAR_NECK_1 )
    &&   get_eq_char( ch, WEAR_NECK_2 )
    &&   !remove_obj( ch, WEAR_NECK_1, fReplace )
    &&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
        return;

    if ( !get_eq_char( ch, WEAR_NECK_1 ) )
    {
            act( "You place $p&n around your neck.", ch, obj, NULL, TO_CHAR );
        act( "$n&n wears $p&n around $s neck.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_NECK_1 );
        return;
    }

    if ( !get_eq_char( ch, WEAR_NECK_2 ) )
    {
        act( "You wear $p&n around your neck.", ch, obj, NULL, TO_CHAR );
            act( "$n&n places $p&n around $s neck.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_NECK_2 );
        return;
    }

    bug( "Wear_obj: no free neck.", 0 );
        send_to_char( "You can't fit anything else around your neck.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) && ch->race != RACE_THRIKREEN )
    {
    if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
        return;
        act( "You slide your body into $p&n.", ch, obj, NULL, TO_CHAR );
    act( "$n&n wears $p&n on $s body.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_BODY );
    return;
    }
    // Snikt - Added check for mino and head gear prevention due to horns
    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
      if ( ch->race != RACE_MINOTAUR )
      {
    if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
        return;
        act( "You don $p&n on your head.", ch, obj, NULL, TO_CHAR );
        act( "$n&n places $p&n on $s head.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_HEAD );
    return;
      }
      else    
      {
        act( "Your &+Lhorns&n prevent you from wearing $p&n.", ch, obj, NULL, TO_CHAR );
        act( "$n&n foolishly attempts to place $p&n on over $s &+Lhorns&n.", ch, obj, NULL, TO_ROOM );
        return;
      }
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) 
     && ch->race != RACE_CENTAUR )
    {
    if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
        return;
        act( "You pull $p&n onto your legs.", ch, obj, NULL, TO_CHAR );
        act( "$n&n slides $s legs into $p&n.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_LEGS );
    return;
    }

    // Snikt - Added Minotaur to the Following ITEM_WEAR_FEET check
    // Check some over the syntax, Vey, if you wouldn't mind.
       if ( CAN_WEAR( obj, ITEM_WEAR_FEET ))
       {
         if ( ch->race != RACE_CENTAUR && ch->race != RACE_MINOTAUR && ch->race != RACE_THRIKREEN)
          {
            if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
            return;
            act( "You wear $p&n on your feet.", ch, obj, NULL, TO_CHAR );
            act( "$n&n wears $p&n on $s feet.",   ch, obj, NULL, TO_ROOM );
            equip_char( ch, obj, WEAR_FEET );
            return;
          }
          else if( ch->race != RACE_THRIKREEN )               
          {
            act( "Your &+Lhooves&n prevent you from wearing $p&n.", ch, obj, NULL, TO_CHAR );
            act( "$n&n foolishly attempts to place $p&n on $s &+Lhooves&n.",   ch, obj, NULL, TO_ROOM );
            return;
          }
          else
          {
            act( "Your underdeveloped feet prevent you from wearing $p&n.", ch, obj, NULL, TO_CHAR );
            act( "$n&n foolishly attempts to place $p&n on $s feet.",   ch, obj, NULL, TO_ROOM );
            return;
          }
       }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
      if( ch->race != RACE_THRIKREEN )
      {
        if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
            return;
        act( "You wear $p&n on your hands.", ch, obj, NULL, TO_CHAR );
        act( "$n&n pulls $p&n onto $s hands.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_HANDS );
        return;
      }
      else
      {
        if ( get_eq_char( ch, WEAR_HANDS )
        &&   get_eq_char( ch, WEAR_LOWER_HANDS )
        &&   !remove_obj( ch, WEAR_HANDS, fReplace )
        &&   !remove_obj( ch, WEAR_LOWER_HANDS, fReplace ) )
          return;

        if ( !get_eq_char( ch, WEAR_HANDS ) )
        {
          act( "You wear $p&n on your hands.", ch, obj, NULL, TO_CHAR );
          act( "$n&n pulls $p&n onto $s hands.", ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_HANDS );
          return;
        }

        if ( !get_eq_char( ch, WEAR_LOWER_HANDS ) )
        {
          act( "You wear $p&n on your hands.", ch, obj, NULL, TO_CHAR );
          act( "$n&n pulls $p&n onto $s hands.", ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_LOWER_HANDS );
          return;
        }
      }
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
      if( ch->race != RACE_THRIKREEN )
      {
        if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
            return;
        act( "You slip your arms into $p&n.", ch, obj, NULL, TO_CHAR );
        act( "$n&n slides $p&n over $s arms.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_ARMS );
        return;
      }
      else
      {
        if ( get_eq_char( ch, WEAR_ARMS )
        &&   get_eq_char( ch, WEAR_LOWER_ARMS )
        &&   !remove_obj( ch, WEAR_ARMS, fReplace )
        &&   !remove_obj( ch, WEAR_LOWER_ARMS, fReplace ) )
          return;

      if ( !get_eq_char( ch, WEAR_ARMS ) )
      {
          act( "You slip your arms into $p&n.", ch, obj, NULL, TO_CHAR );
          act( "$n&n slides $p&n over $s arms.",   ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_ARMS );
          return;
      }

      if ( !get_eq_char( ch, WEAR_LOWER_ARMS ) )
      {
          act( "You slip your arms into $p&n.", ch, obj, NULL, TO_CHAR );
          act( "$n&n slides $p&n over $s arms.",   ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_LOWER_ARMS );
          return;
      }
    }

    bug( "Wear_obj: no free ear.", 0 );
    send_to_char( "You already wear an earring in each ear.\n\r", ch );
    return;
    }


    if ( CAN_WEAR( obj, ITEM_WEAR_ONBACK ) )
    {
    if ( !remove_obj( ch, WEAR_ONBACK, fReplace ) )
        return;
    act( "You strap $p&n to your back.", ch, obj, NULL, TO_CHAR );
    act( "$n&n straps $p&n to $s back.", ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_ONBACK );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_QUIVER ) )
    {
    if ( !remove_obj( ch, WEAR_QUIVER, fReplace ) )
        return;
        act( "You adjust $p&n across your shoulders.", ch, obj, NULL, TO_CHAR );
        act( "$n&n straps $p&n across $s shoulders.", ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_QUIVER );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
    if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
        return;
        act( "You drape $p&n about your body.", ch, obj, NULL, TO_CHAR );
        act( "$n&n wraps $p&n about $s body.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_ABOUT );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
    if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
        return;
        act( "You secure $p&n about your waist.", ch, obj, NULL, TO_CHAR );
        act( "$n&n secures $p&n about $s waist.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_WAIST );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
    if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
        return;
        act( "You place $p&n over your face.", ch, obj, NULL, TO_CHAR );
        act( "$n&n covers $s his face with $p&n.",   ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_FACE );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_BADGE ) )
    {
    if ( !remove_obj( ch, WEAR_BADGE, fReplace ) )
        return;
        act( "You pin $p&n over your right breast.", ch, obj, NULL, TO_CHAR );
        act( "$n&n pins $p&n over $s right breast.", ch, obj, NULL, TO_ROOM );
    equip_char( ch, obj, WEAR_BADGE );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
      if( ch->race != RACE_THRIKREEN )
      {
        if ( get_eq_char( ch, WEAR_WRIST_L )
        &&   get_eq_char( ch, WEAR_WRIST_R )
        &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
        &&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
          return;

        if ( !get_eq_char( ch, WEAR_WRIST_L ) )
        {
          act( "You wear $p&n around your left wrist.",
          ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n around $s left wrist.",
          ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_WRIST_L );
          return;
        }

        if ( !get_eq_char( ch, WEAR_WRIST_R ) )
        {
          act( "You wear $p&n about your right wrist.",
            ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n about $s right wrist.",
            ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_WRIST_R );
          return;
        }

        bug( "Wear_obj: no free wrist.", 0 );
        send_to_char( "You already wear something upon both wrists.\n\r", ch );
        return;
      }
      else
      {
        if ( get_eq_char( ch, WEAR_WRIST_L )
        &&   get_eq_char( ch, WEAR_WRIST_R )
        &&   get_eq_char( ch, WEAR_LWRIST_L )
        &&   get_eq_char( ch, WEAR_LWRIST_R )
        &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
        &&   !remove_obj( ch, WEAR_WRIST_R, fReplace )
        &&   !remove_obj( ch, WEAR_LWRIST_L, fReplace )
        &&   !remove_obj( ch, WEAR_LWRIST_R, fReplace ) )
          return;

        if ( !get_eq_char( ch, WEAR_WRIST_L ) )
        {
          act( "You wear $p&n around your left wrist.",
          ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n around $s left wrist.",
          ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_WRIST_L );
          return;
        }

        if ( !get_eq_char( ch, WEAR_WRIST_R ) )
        {
          act( "You wear $p&n about your right wrist.",
            ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n about $s right wrist.",
            ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_WRIST_R );
          return;
        }

        if ( !get_eq_char( ch, WEAR_LWRIST_L ) )
        {
          act( "You wear $p&n around your lower left wrist.",
          ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n around $s lower left wrist.",
          ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_LWRIST_L );
          return;
        }

        if ( !get_eq_char( ch, WEAR_LWRIST_R ) )
        {
          act( "You wear $p&n about your lower right wrist.",
            ch, obj, NULL, TO_CHAR );
          act( "$n&n wears $p&n about $s lower right wrist.",
            ch, obj, NULL, TO_ROOM );
          equip_char( ch, obj, WEAR_LWRIST_R );
          return;
        }

        bug( "Wear_obj: no free wrist.", 0 );
        send_to_char( "You already wear something upon all four wrists.\n\r", ch );
        return;
      }
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) && ch->race != RACE_THRIKREEN )
    {
    if ( get_eq_char( ch, WEAR_EAR_L )
    &&   get_eq_char( ch, WEAR_EAR_R )
    &&   !remove_obj( ch, WEAR_EAR_L, fReplace )
    &&   !remove_obj( ch, WEAR_EAR_R, fReplace ) )
        return;

    if ( !get_eq_char( ch, WEAR_EAR_L ) )
    {
        act( "You wear $p&n in your left ear.",
        ch, obj, NULL, TO_CHAR );
            act( "$n&n hangs $p&n from $s left ear.",
        ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_EAR_L );
        return;
    }

    if ( !get_eq_char( ch, WEAR_EAR_R ) )
    {
            act( "You hang $p&n from your right ear.",
        ch, obj, NULL, TO_CHAR );
        act( "$n&n wears $p&n in $s right ear.",
        ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_EAR_R );
        return;
    }

    bug( "Wear_obj: no free ear.", 0 );
    send_to_char( "You already wear an earring in each ear.\n\r", ch );
    return;
    }

//  Snikt- Moved these before belt check so as to avoid weapons flagged both
//  wieldable and wear_belt from defaulting to belt position

// Snikt, again.  The following code looks way to repetitive, leading one
// to believe it can be highly optimized.  However, I am not skilled enough
// yet with all the terms [ie. !remove_obj( ch, WEAR_HAND, fReplace ) ) has
// no meaning to me at all.]


    if ( obj->item_type == TYPE_LIGHT )
    {
       equip_hand( ch, obj, EQUIP_LIGHT );
       return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) && obj->item_type == TYPE_WEAPON)
    {
       equip_hand( ch, obj, EQUIP_WIELD );
       return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
       equip_hand( ch, obj, EQUIP_SHIELD );
       return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
       equip_hand( ch, obj, EQUIP_HOLD );
       return;
    }

    // only two belt slots for now - Veygoth
    if ( CAN_WEAR( obj, ITEM_ATTACH_BELT ) )
    {
        OBJ_DATA     *obj2;
        char buf[MAX_STRING_LENGTH];

        if ( !(obj2 = get_eq_char( ch, WEAR_WAIST )))
        {
           send_to_char( "You don't have a &n&+ybelt&n to attach it to.\n\r", ch );
           return;
        }

    if ( get_eq_char( ch, WEAR_BELT_1 )
    &&   get_eq_char( ch, WEAR_BELT_2 )
    &&   !remove_obj( ch, WEAR_BELT_1, fReplace )
    &&   !remove_obj( ch, WEAR_BELT_2, fReplace ) )    
        return;

    if ( !get_eq_char( ch, WEAR_BELT_1 ) )
    {
            sprintf( buf, "You attach $p&n to %s&n.", obj2->short_descr );
            act( buf, ch, obj, NULL, TO_CHAR );
            sprintf( buf, "$n&n attaches $p&n to %s&n.", obj2->short_descr );
            act( buf, ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_BELT_1 );
        return;
    }

    if ( !get_eq_char( ch, WEAR_BELT_2 ) )
    {
            sprintf( buf, "You attach $p&n to %s&n.", obj2->short_descr );
            act( buf, ch, obj, NULL, TO_CHAR );
            sprintf( buf, "$n&n attaches $p&n to %s&n.", obj2->short_descr );
            act( buf, ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_BELT_2 );
        return;
    }

    bug( "Wear_obj: no free belt slot.", 0 );
        send_to_char( "Your &n&+ybelt&n is already full.\n\r", ch );
    return;
    }

    /* This will be fixed when we have something better to check against. */
    if ( CAN_WEAR( obj, ITEM_HORSE_BODY )
     && ch->race == RACE_CENTAUR )
    {
        if ( !remove_obj( ch, WEAR_HORSE_BODY, fReplace ) )
          return;
        act( "You strap $p&n onto your body.", ch, obj, NULL, TO_CHAR );
        act( "$n&n straps $p&n onto $s body.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_HORSE_BODY );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_NOSE ) && ch->race == RACE_MINOTAUR ) {
        if ( !remove_obj( ch, WEAR_NOSE, fReplace ) )
          return;
        act( "You attach $p&n to your nose.", ch, obj, NULL, TO_CHAR );
        act( "$n&n wears $p&n in $s nose.",   ch, obj, NULL, TO_ROOM );
        equip_char( ch, obj, WEAR_NOSE );
        return;
    }

    if ( fReplace )
    send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}

void do_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_wear");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "Your body refuses the call to movement.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && is_affected( ch, AFF_GHOUL ) )
    {
        send_to_char(
           "You try, but your &n&+wghoul&n form resists your attempts.\n\r", ch );
        return;
    }

    if ( ch->fighting || ch->position == POS_FIGHTING )
    {
        send_to_char( "You can't wear stuff while you're fighting!\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Wear, wield, or hold what?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        OBJ_DATA *obj_next;

        for ( obj = ch->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;

        if ( obj->deleted )
        continue;

        if ( obj->wear_loc != WEAR_NONE || !can_see_obj( ch, obj ) )
            continue;

        if ( CAN_WEAR( obj, ITEM_WIELD )
        && !IS_SET( race_table[ ch->race ].race_abilities,
               RACE_WEAPON_WIELD ) )
            continue;

            wear_obj( ch, obj, FALSE );
            if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_WEAR ))
            {
              setoff_trap( ch, obj );
              if( ch->deleted || ch->position == POS_DEAD )
                return;
            }
    }
    return;
    }
    else
    {
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD )
        && !IS_SET( race_table[ ch->race ].race_abilities,
               RACE_WEAPON_WIELD ) )
    {
        send_to_char( "You are not able to wield a weapon.\n\r", ch );
        return;
    }

        wear_obj( ch, obj, TRUE );
        if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_WEAR ))
        {
          setoff_trap( ch, obj );
          if( ch->deleted || ch->position == POS_DEAD )
            return;
        }
    }

    return;
}

// Rewritten by Veygoth 5-30-00
// Now calls equip_hand to resolve the actual equipping
void do_wield( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_wield");

    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "Your body refuses the call to movement.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && is_affected( ch, AFF_GHOUL ) )
    {
      send_to_char(  "You try, but your &n&+wghoul&n form resists your  attempts.\n\r", ch );
      return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
      send_to_char( "Wield what?\n\r", ch );
      return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

   if( !CAN_WEAR( obj, ITEM_WIELD ))
   {
      if( obj->item_type == TYPE_WEAPON )
      {
        send_to_char( "That object is not usable as a weapon.\n\r", ch );
        return;
      }
      else if( obj->item_type == TYPE_RANGED_WEAPON && !CAN_WEAR( obj, ITEM_HOLD ))
      {
        send_to_char( "That object is not usable as a missile weapon.\n\r", ch );
        return;
      }
      else if( obj->item_type == TYPE_RANGED_WEAPON )
      {
        // Ranged weapons flagged wither wield or hold are fine to use -- Veygoth
      }
      else
      {
        send_to_char( "That object is not a weapon.\n\r", ch );
        return;
      }
   }

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_WEAPON_WIELD ) )
    {
        send_to_char( "You are not able to wield a weapon.\n\r", ch );
        return;
    }

    if( !wearable_by( ch, obj ))
        return;

    equip_hand( ch, obj, EQUIP_WIELD );

    return;
}

void do_remove( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    bool      found;

    last_fun( "do_remove");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You no longer have control of your body!\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Remove what?\n\r", ch );
        return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
        /* 'remove obj' */
        if ( !( obj = get_obj_wear( ch, arg ) ) )
        {
            send_to_char( "You do not have that item.\n\r", ch );
            return;
        }

        if ( ch->carry_number + 1 > MAX_CARRY )
        {
          send_to_char( "You have your hands full.\n\r", ch );
          return;
        }

        remove_obj( ch, obj->wear_loc, TRUE );
        if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_UNEQUIP ))
        {
          setoff_trap( ch, obj );
          if( ch->deleted || ch->position == POS_DEAD )
            return;
        }
    }
    else
    {
    /* 'remove all' or 'remove all.obj' */
    OBJ_DATA *obj_next;

    found = FALSE;
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
            obj_next = obj->next_content;

        if ( obj->deleted )
            continue;

        if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
         && obj->wear_loc != WEAR_NONE )
        {
            found = TRUE;
            if ( ch->carry_number + 1 > MAX_CARRY )
            {
              send_to_char( "You have your hands full.\n\r", ch );
              return;
            }
            remove_obj( ch, obj->wear_loc, TRUE );
            if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_UNEQUIP ))
            {
              setoff_trap( ch, obj );
              if( ch->deleted || ch->position == POS_DEAD )
                return;
            }
        }
    }

    if ( !found )
    {
        if ( arg[3] == '\0' )
        send_to_char( "You can't find anything to remove.\n\r", ch );
        else
                act( "You can't find any $T&n to remove.",
            ch, NULL, &arg[4], TO_CHAR );
    }
    }

    return;
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_quaff");
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Quaff what?\n\r", ch );
    return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
    send_to_char( "You do not have that potion.\n\r", ch );
    return;
    }

    if ( obj->item_type != TYPE_POTION )
    {
    send_to_char( "You can quaff only potions.\n\r", ch );
    return;
    }

    act( "You quaff $p&n.", ch, obj, NULL ,TO_CHAR );
    act( "$n&n drains $p&n.", ch, obj, NULL, TO_ROOM );

    WAIT_STATE( ch, 3 );

    if( obj->pIndexData->vnum == OBJ_VNUM_51_POTION )
    {
       extract_obj( obj );
       if( ch->level != 50 || ch->exp < exp_table[ch->level].lvl_exp )
       {
          send_to_char( "Nothing happens.\n\r", ch );
          return;
       }
       else
       {
          advance_level( ch, TRUE );
       }
    }
    if( obj->pIndexData->vnum == OBJ_VNUM_52_POTION )
    {
       extract_obj( obj );
       if( ch->level != 51 || ch->exp < exp_table[ch->level].lvl_exp )
       {
          send_to_char( "Nothing happens.\n\r", ch );
          return;
       }
       else
       {
          advance_level( ch, TRUE );
       }
    }
    if( obj->pIndexData->vnum == OBJ_VNUM_53_POTION )
    {
       extract_obj( obj );
       if( ch->level != 52 || ch->exp < exp_table[ch->level].lvl_exp )
       {
          send_to_char( "Nothing happens.\n\r", ch );
          return;
       }
       else
       {
          advance_level( ch, TRUE );
       }
    }
    if( obj->pIndexData->vnum == OBJ_VNUM_54_POTION )
    {
       extract_obj( obj );
       if( ch->level != 53 || ch->exp < exp_table[ch->level].lvl_exp )
       {
          send_to_char( "Nothing happens.\n\r", ch );
          return;
       }
       else
       {
          advance_level( ch, TRUE );
       }
    }
    if( obj->pIndexData->vnum == OBJ_VNUM_55_POTION )
    {
       extract_obj( obj );
       if( ch->level != 54 || ch->exp < exp_table[ch->level].lvl_exp )
       {
          send_to_char( "Nothing happens.\n\r", ch );
          return;
       }
       else
       {
          advance_level( ch, TRUE );
       }
    }

    {
        /* obj->value[0] is not used for potions */
    obj_cast_spell( obj->value[1], obj->level, ch, ch, NULL );
    obj_cast_spell( obj->value[2], obj->level, ch, ch, NULL );
    obj_cast_spell( obj->value[3], obj->level, ch, ch, NULL );
    obj_cast_spell( obj->value[4], obj->level, ch, ch, NULL );
    }

/* If clause commented out by Lithos.  Take away potion in all cases
    if ( !IS_NPC( ch )
    || ( IS_NPC( ch ) && has_affect( ch, 0, spl_charm_person, 0 ) ) )
*/
        extract_obj( obj );
    return;
}

// - Sniktiorg
void do_smoke( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *herb;
    OBJ_DATA *pipe;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    int       pipebreak;

    last_fun( "do_smoke");
    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
      act( "You whip out a cigarette and puff away.", ch, NULL, NULL, TO_CHAR );
      act( "$n&n lights up a cigarette and inhales deeply.", ch, NULL, NULL, TO_ROOM );
      return;
    }

    if ( arg2[0] == '\0' )
    {
        send_to_char( "What do you want to smoke through?\n\r", ch );
        return;
    }

    if ( !( herb = get_obj_carry( ch, arg1 ) ) )
    {
        send_to_char( "You do not have $p&n.\n\r", ch );
        return;
    }

    if ( herb->item_type != TYPE_HERB )
    {
        send_to_char( "Its unsafe to smoke anything but &n&+gherbs&n.\n\r", ch );
        return;
    }

    if ( !( pipe = get_obj_carry( ch, arg2 ) ) )
    {
        send_to_char( "You do not have $P&n.\n\r", ch );
        return;
    }

    if ( pipe->item_type != TYPE_PIPE )
    {
        send_to_char( "You need a &+Lpipe&n of some sort, $P just won't do.\n\r", ch );
        return;
    }

    if (( weather_info.sky == SKY_RAINING ) && ((ch->in_room->sector_type != SECT_INSIDE)
        || (ch->in_room->sector_type == SECT_SWAMP) || (ch->in_room->sector_type == SECT_FOREST)))
       {
         send_to_char( "There is no way you can smoke in these conditions.\n\r", ch );
         return;
       }

     if ( is_water_room( ch->in_room ))
       {
         send_to_char( "The &n&+cwa&+Ct&n&+ce&+Cr&m makes that an impossiblity.\n\r", ch );
         return;
       }


    act( "You pack $p&n into your $P.", ch, herb, pipe, TO_CHAR );
    act( "You &n&+rlight&n $P&n and inhale deep $p&+W smoke&n.", ch, pipe, herb, TO_CHAR );
    act( "You finish smoking $p&n.", ch, herb, NULL, TO_CHAR );
    act( "$n&n smokes $p&n through $s $P&n.", ch, herb, pipe, TO_ROOM );

    {
        /* herb->value[0] is not used for Herbs */
        obj_cast_spell( herb->value[1], herb->level, ch, ch, NULL );
        obj_cast_spell( herb->value[2], herb->level, ch, ch, NULL );
        obj_cast_spell( herb->value[3], herb->level, ch, ch, NULL );
        obj_cast_spell( herb->value[4], herb->level, ch, ch, NULL );
    }

    // Snikt - pipe->value[1] would be refering to de percent chance burnout of
    // pipe value set when creating item.
    pipebreak = number_range( 1, 100 );

    if ( pipebreak <= pipe->value[1] )
    {
      act( "Your $p&n cracks and becomes useless from too much &n&+rheat&n.", ch, pipe, NULL, TO_CHAR );
      act( "You toss $p&n violently into the distance.", ch, pipe, NULL, TO_CHAR );
      act( "$n&n throws away $p, as cracks have made it useless.", ch, pipe, NULL, TO_ROOM );
      extract_obj( pipe );
    }

    if ( !IS_NPC( ch )
    || ( IS_NPC( ch ) && has_affect( ch, 0, spl_charm_person, 0 ) ) )
        extract_obj( herb );
    return;
}


void do_recite( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *scroll;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_recite");
    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( !( scroll = get_obj_carry( ch, arg1 ) ) )
    {
        send_to_char( "You do not have that &+Wscroll&n.\n\r", ch );
    return;
    }

    if ( scroll->item_type != TYPE_SCROLL )
    {
        send_to_char( "You can recite only &+Wscrolls&n.\n\r", ch );
    return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
    victim = ch;
    if ( ch->fighting )
        victim = ch->fighting;
    }
    else
    {
    if ( !( victim = get_char_room ( ch, arg2 ) )
        && !( obj  = get_obj_here  ( ch, arg2 ) ) )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    }

    if ( is_affected( ch, AFF_MUTE )
    || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
    || IS_ROOM( ch->in_room, ROOM_SILENT ) )
    {
        send_to_char( "Your lips move but no sound comes out.\n\r", ch );
        return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
    || is_affected( ch, AFF_CHARM ) )
    {
    act( "You try to recite $p&n, but you have no free will.",
        ch, scroll, NULL, TO_CHAR );
    act( "$n&n tries to recite $p&n, but has no free will.", 
        ch, scroll, NULL, TO_ROOM );
    return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    act( "You recite $p&n.", ch, scroll, NULL, TO_CHAR );
    act( "$n&n recites $p&n.", ch, scroll, NULL, TO_ROOM );

    skill_practice( ch, gsn_scrolls );

    /* Scrolls skill by Binky for EnvyMud, modified by Thelonius */
    if ( !IS_NPC( ch )
    && !( number_percent( ) < ch->pcdata->skl_lrn[gsn_scrolls] ) )
    {
    switch ( number_bits( 3 ) )
    {
    case 0: 
    case 1:                      
    case 2:
    case 3: 
        act( "You can't understand $p&n at all.",
        ch, scroll, NULL, TO_CHAR );
        act( "$n&n can't understand $p&n at all.",
        ch, scroll, NULL, TO_ROOM );
        return;                    
    case 4:                
    case 5:                      
    case 6:                      
        send_to_char( "You must have said something incorrectly.\n\r",
             ch );
        act( "$n&n must have said something incorrectly.",
        ch, NULL,   NULL, TO_ROOM );
        act( "$p&n blazes brightly, then is gone.",
        ch, scroll, NULL, TO_CHAR );
        act( "$p&n blazes brightly and disappears.",
        ch, scroll, NULL, TO_ROOM );
        extract_obj( scroll );
        return;
    case 7:
        act( 
        "You completely botch the recitation, and $p&n bursts into &+Rflames&n!!", 
        ch, scroll, NULL, TO_CHAR );
            act( "$p&n &+rglows&n and then bursts into &+Rflame&n!", 
        ch, scroll, NULL, TO_ROOM );
        /*
         * damage( ) call after extract_obj in case the damage would
         * have extracted ch.  This is okay because we merely mark
         * obj->deleted; it still retains all values until list_update.
         * Sloppy?  Okay, create another integer variable. ---Thelonius
         */
        extract_obj( scroll );
        damage( ch, ch, scroll->level, gsn_scrolls, WEAR_NONE, DAM_FIRE );
        return;
    }
    }

    if ( scroll->level > ch->level ) 
        act( "$p&n is too high level for you.", ch, scroll, NULL, TO_CHAR );
    else
    {
        /* scroll->value[0] is not used for scrolls */
    obj_cast_spell( scroll->value[1], scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->value[2], scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->value[3], scroll->level, ch, victim, obj );
    obj_cast_spell( scroll->value[4], scroll->level, ch, victim, obj );
    }

    if ( !IS_NPC( ch )
    || ( IS_NPC( ch ) && has_affect( ch, 0, spl_charm_person, 0 ) ) )
        extract_obj( scroll );
    return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *staff;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        sn;

    last_fun( "do_brandish");
    if ( !( staff = get_eq_char( ch, WEAR_HAND ) ) )
    {
        send_to_char( "You hold nothing in your hand.\n\r", ch );
        return;
    }

    if ( staff->item_type != TYPE_STAFF )
    {
        send_to_char( "You can brandish only with a &n&+ystaff&n.\n\r", ch );
        return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
    || is_affected( ch, AFF_CHARM ) )
    {
        act( "You try to brandish $p&n, but you have no free will.",
            ch, staff, NULL, TO_CHAR );
        act( "$n&n tries to brandish $p&n, but has no free will.", 
            ch, staff, NULL, TO_ROOM );
        return;
    }

    if ( ( sn = staff->value[3] ) < 0
    || sn >= MAX_SPELL
    || spells_table[sn].spell_fun == 0 )
    {
        bug( "Do_brandish: bad sn %d.", sn );
        return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {

        act( "You brandish $p&n.",  ch, staff, NULL, TO_CHAR );
        act( "$n&n brandishes $p&n.", ch, staff, NULL, TO_ROOM );

        skill_practice( ch, gsn_staves );

        /* Staves skill by Binky for EnvyMud, modified by Thelonius */
        if ( !IS_NPC( ch ) 
            && ( number_percent( ) > ch->pcdata->skl_lrn[gsn_staves] ) )
        { 
            switch ( number_bits( 3 ) )
            {
            default:
            case 0: 
            case 1:                      
            case 2:                      
            case 3: 
                act( "You are unable to invoke the power of $p&n.",
                    ch, staff, NULL, TO_CHAR );
                act( "$n&n is unable to invoke the power of $p&n.",
                    ch, staff, NULL, TO_ROOM );
                return;                    
            case 4:                
            case 5:                      
            case 6:                      
                act( "You summon the power of $p&n, but it fizzles away.",
                    ch, staff, NULL, TO_CHAR );
                act( "$n&n summons the power of $p&n, but it fizzles away.",
                   ch, staff, NULL, TO_ROOM );
                if ( --staff->value[2] <= 0 )
                {
                    act( "$p&n blazes bright and is gone.",
                    ch, staff, NULL, TO_CHAR );
                    act( "$p&n blazes bright and is gone.",
                    ch, staff, NULL, TO_ROOM );
                    extract_obj( staff );
                }
                return;
            case 7:
                act( "You can't control the power of $p&n, and it shatters!",
                    ch, staff, NULL, TO_CHAR );
                act( "$p&n shatters into tiny pieces!", 
                    ch, staff, NULL, TO_ROOM );
                /*
                 * damage( ) call after extract_obj in case the damage would
                 * have extracted ch.  This is okay because we merely mark
                 * obj->deleted; it still retains all values until list_update.
                 * Sloppy?  Okay, create another integer variable. ---Thelonius
                 */
                extract_obj( staff );
                damage( ch, ch, staff->level, gsn_staves, WEAR_NONE, DAM_ENERGY );
                return;
            }
        }
    }

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;

        if ( vch->deleted )
            continue;

        switch ( spells_table[sn].target )
        {
        default:
            bug( "Do_brandish: bad target for sn %d.", sn );
            return;

        case TAR_IGNORE:
            if ( vch != ch )
                continue;
            break;

        case TAR_CHAR_OFFENSIVE:
            if ( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
                continue;
            break;
        
        case TAR_CHAR_DEFENSIVE:
            if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
                continue;
            break;

        case TAR_CHAR_SELF:
            if ( vch != ch )
                continue;
            break;
        }

        /* staff->value[0] is not used for staves */
        obj_cast_spell( staff->value[3], staff->level, ch, vch, NULL );
    }

    if ( !IS_NPC( ch )
        || ( IS_NPC( ch ) && has_affect( ch, 0, spl_charm_person, 0 ) ) )
        if ( --staff->value[2] <= 0 )
        {
            act( "$p&n blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
            act( "$p&n blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
            extract_obj( staff );
        }

    return;
}

void do_use( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *wand;
    char       arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_use");

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
      send_to_char( "Use what?\n\r", ch );
      return;
    }

    if( !(wand = get_obj_wear( ch, arg ) ) )
    {
        send_to_char( "You can't find it!\n\r", ch );
        return;
    }

    if ( wand->item_type == TYPE_WAND )
    {
      do_zap( ch, argument );
      return;
    }
    else if( wand->item_type == TYPE_STAFF )
    {
      do_brandish( ch, argument );
      return;
    }
    else
    {
      send_to_char( "You can only use staves or wands.\n\r", ch );
      return;
    }

    return;
}

void do_zap( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *wand = NULL;
    OBJ_DATA  *obj = NULL;
    CHAR_DATA *victim = NULL;
    char       arg [ MAX_INPUT_LENGTH ];
    int        sn;
    int        level;

    last_fun( "do_zap");
    argument = one_argument( argument, arg );

    if( arg[0] == '\0' && !ch->fighting )
    {
        send_to_char( "Zap whom or what?\n\r", ch );
        return;
    }

    if( arg[0] != '\0' && !(wand = get_obj_wear( ch, arg ) ) )
    {
        if ( !( wand = get_eq_char( ch, WEAR_HAND ) ) )
        {
            send_to_char( "You hold nothing in your hand.\n\r", ch );
            return;
        }
    }
    else /* Wand was first argument.. arg is now second argument. */
        one_argument( argument, arg );

    if ( wand->item_type != TYPE_WAND )
    {
        send_to_char( "You can zap only with a wand.\n\r", ch );
        return;
    }

    level = wand->level;
    obj = NULL;
    if ( arg[0] == '\0' )
    {
        if ( ch->fighting )
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char( "Zap whom or what?\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !( victim = get_char_room ( ch, arg ) )
            && !( obj  = get_obj_here  ( ch, arg ) ) )
        {
            send_to_char( "You can't find your target.\n\r", ch );
            return;
        }
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
    || is_affected( ch, AFF_CHARM ) )
    {
        act( "You try to zap $p&n, but you have no free will.",
            ch, wand, NULL, TO_CHAR );
        act( "$n&n tries to zap $p&n, but has no free will.", 
            ch, wand, NULL, TO_ROOM );
        return;
    }

    if ( ( sn = wand->value[3] ) < 0
    || sn >= MAX_SPELL
    || spells_table[sn].spell_fun == 0 )
    {
      send_to_char( "That wand is buggy.  Please report it.\n\r", ch );
      bug( "Do_zap: bad sn %d.", sn );
      return;
    }

//    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
      skill_practice( ch, gsn_wands );

    if ( wand->value[2] > 0 )
    {
      if ( victim )
      {
        if ( victim == ch )
        {
          act( "You zap yourself with $p&n.", ch, wand, NULL,   TO_CHAR );
          act( "$n&n zaps $mself with $p&n.",   ch, wand, NULL,   TO_ROOM );
        }
        else
        {
          act( "You zap $N&n with $p&n.",       ch, wand, victim, TO_CHAR );
          act( "$n&n zaps $N&n with $p&n.",       ch, wand, victim, TO_ROOM );
        }
      }
      else
      {
        act( "You zap $P&n with $p&n.",           ch, wand, obj,    TO_CHAR );
        act( "$n&n zaps $P&n with $p&n.",           ch, wand, obj,    TO_ROOM );
      }

      /* Wands skill by Binky for EnvyMud, modified by Thelonius */
      if ( !IS_NPC( ch ) 
          && ( number_percent( ) > ch->pcdata->skl_lrn[gsn_wands] ) )
      { 
        switch ( number_bits( 3 ) )
        {
        case 0: 
        case 1:                      
        case 2:                      
        case 3: 
            act( "You are unable to invoke the power of $p&n.",
                ch, wand, NULL, TO_CHAR );
            act( "$n&n is unable to invoke the power of $p&N.",
                ch, wand, NULL, TO_ROOM );
            break;  
        case 4:                
        case 5:                      
        case 6:                      
            act( "You summon the power of $p&n, but it fizzles away.",
                ch, wand, NULL, TO_CHAR );
            act( "$n&n summons the power of $p&n, but it fizzles away.",
                ch, wand, NULL, TO_ROOM );
            break;  
        case 7:
            act( "You can't control the power of $p&n, and it &+Rexplodes&n!",
                ch, wand, NULL, TO_CHAR );
            act( "$p&n &n&+rexplodes&n into fragments!", ch, wand, NULL, TO_ROOM );
            /*
             * damage( ) call after extract_obj in case the damage would
             * have extracted ch.  This is okay because we merely mark
             * obj->deleted; it still retains all values until list_update.
             * Sloppy?  Okay, create another integer variable. ---Thelonius
             * I hate sloppy... - Lohrr
             */
            extract_obj( wand );
            damage( ch, ch, level, gsn_wands, WEAR_NONE, DAM_ENERGY );
            break;  
        }
        return;
      } /* End bad attempt. */

      /* wand->value[0] is not used for wands */
      obj_cast_spell( wand->value[3], level, ch, victim, obj );
    } /* End check for (usable wand) wand->value[2] > 0 */

    if ( --(wand->value[2]) <= 0 )
    {
        if( !IS_NPC( ch ) && number_percent() > ch->pcdata->skl_lrn[gsn_wands] )
        {
            act( "$p&n &n&+rexplodes&n into fragments.", ch, wand, NULL, TO_CHAR );
            act( "$p&n &+Rexplodes&n into fragments.", ch, wand, NULL, TO_ROOM );
            extract_obj( wand );
            damage( ch, ch, level, gsn_wands, WEAR_NONE, DAM_ENERGY );
        }
        else
        {
            act( "$p&n blazes bright and is gone.",
            ch, wand, NULL, TO_CHAR );
            act( "$p&n blazes bright and is gone.",
            ch, wand, NULL, TO_ROOM );
            extract_obj( wand );
        }
    }

    return;
}

void do_steal( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    bool       sleeping = FALSE;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;
    int        percent;
    int        amount, amount2, amount3, amount4;

    last_fun( "do_steal");
    if ( ch->level < skills_table[gsn_steal].skill_level[ch->class] && !is_affected( ch, AFF_CHARM ))
    {
        send_to_char( "Who are you trying to kid?  You couldn't steal shoes from a &n&+mbl&+Mo&n&+ma&+Mte&n&+md&n corpse.\n\r", ch );
        return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
      send_to_char( "Steal what from whom?\n\r", ch );
      return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    if ( victim == ch )
    {  
      send_to_char( "That's pointless.\n\r", ch );
      return;
    }

    if ( is_safe( ch, victim ) )
        return;

    obj = NULL;

    if ( !IS_IMMORTAL( ch) ) WAIT_STATE( ch, skills_table[gsn_steal].beats );

    // Justice stuff
    check_thief( ch, victim );

    /* Modified chances for stealing by Morpheus */
    if( IS_NPC( ch ))
      percent = ch->level * 2;   
    else
      percent = ch->pcdata->skl_lrn[gsn_steal];

    percent += get_curr_luk( ch ) / 20; /* Luck */

    percent -= victim->level; /* Character level vs victim's */

    if ( ch->race == RACE_HALFLING )
        // Halflings get a racial bonus
        percent += 10;

    if ( is_affected( victim, AFF_CURSE )  )
        percent += 15;

    if ( is_affected( ch, AFF_CURSE ) )
        percent -= 15;

    if ( !IS_AWAKE( victim ) )
        percent += 25; /* Sleeping characters are easier */

    if ( IS_AFFECTED( ch, AFF_SNEAK ) 
       || ( !IS_NPC( ch )  && ch->pcdata 
           && has_affect( ch, gsn_sneak, 0, 0 )
           && number_percent() < ch->pcdata->skl_lrn[gsn_sneak] ) )
        percent += 5; /* Quiet characters steal better */

    if ( !can_see( ch, victim ) )
        percent += 10; /* Unseen characters steal better */

    if ( !str_prefix( arg1, "coins" ) )
        percent = (int) ( percent * 1.2 ); /* Cash is fairly easy to steal */
    else
    {
    number = number_argument( arg1, arg );
    count  = 0;
    for ( obj = victim->carrying; obj; obj = obj->next_content )
    {
        if ( can_see_obj( ch, obj )
        && is_name( arg, obj->name ) )
        {
        if ( ++count == number )
          break;
        }
    }

    if ( !obj )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }

    if ( ch->level < victim->level ) {
        // stealing from higher level is possible, but harder
        percent -= 5 * (victim->level - ch->level );
    }
    else {
        // slight bonus for mobs lower level
        percent += (ch->level - victim->level );
    }
    if ( obj->wear_loc == WEAR_NONE )
        /* Items worn are harder */
        percent = (int) ( percent * .8 );
    else
        percent = (int) ( percent * .4 );
    }

    skill_practice( ch, gsn_steal );

    if( percent > 85 )
      percent = 85;

    if (percent < 2 )
         percent = 2;
    if( percent < number_percent( ) ) {
        /*
         * Failure.
         */
        //strip sneak
        remove_affects( ch, AFF_SNEAK );
        // chance of removing invis
        if ( is_affected( ch, AFF_INVISIBLE ) && number_percent() > percent ) {
            send_to_char( "You really bungled that attempt.\n\r", ch );
            remove_affects( ch, AFF_INVISIBLE );
        }
        else
            send_to_char( "Oops.\n\r", ch );
        act( "$n&n tried to steal from $N&n.",  ch, NULL, victim, TO_NOTVICT );
        if ( IS_AWAKE( victim ) )
            act( "$n&n tried to steal from you!", ch, NULL, victim, TO_VICT);
        else
            sleeping = TRUE;
        // Thief flag for justice - Veygoth 
	// Added soo blind mobs dont hit who ever failed steal from em - Jolle
        if ( IS_NPC( victim ) && !IS_SET( victim->act, ACT_NICE_THIEF ) )
        {
            if ( !sleeping && check_blind( victim ) )
                do_kill( victim, ch->name );
        }
        else
        {
            if (check_blind( victim ) && !sleeping &&  has_affect( victim, gsn_berserk, 0, 0 ) ) {
                send_to_char( "In your &+Rbloodrage&n, you lash out in anger!\n\r", victim );
                do_kill( victim, ch->name );
            }
        }
/*
        if ( !IS_SET( ch->act, PLR_THIEF ) )
        {
            SET_BIT( ch->act, PLR_THIEF );
                    sprintf( buf, "%s became a THIEF by stealing from %s",
                        ch->name, victim->name );
                    wiznet( ch, WIZ_CRIME, get_trust( ch ), buf );
            save_char_obj( ch );
        }
*/
//    }
    if ( sleeping ) {
        if ( number_percent() < get_curr_luk( victim ) )
            do_wake( victim, "" );
    }
    return;
    } //end failure

    if (   !str_prefix( arg1, "coins" ) )
    {
    amount = victim->money.gold * number_range( 1, 20 ) / 100;
    amount2 = victim->money.silver * number_range( 1, 20 ) / 100;
    amount3 = victim->money.copper * number_range( 1, 20 ) / 100;
    amount4 = victim->money.platinum * number_range( 1, 20 ) / 100;

    if ( (amount + amount2 + amount3 + amount4) <= 0 )
    {
        send_to_char( "You couldn't get any &n&+wcoins&n.\n\r", ch );
        return;
    }

    ch->money.gold     += amount;
    ch->money.silver   += amount2;
    ch->money.copper   += amount3;
    ch->money.platinum += amount4;

    victim->money.gold     -= amount;
    victim->money.silver   -= amount2;
    victim->money.copper   -= amount3;
    victim->money.platinum -= amount4;

    sprintf( buf, "Success!  You got %d &+Wplatinum&n, %d &+Ygold&n, %d silver, and %d &+ycopper&n.\n\r",
      amount2, amount3, amount, amount4 );
    send_to_char( buf, ch );
    return;
    }

    if ( !can_drop_obj( ch, obj )
    || IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
    {
      send_to_char( "You can't pry it away.\n\r", ch );
      return;
    }

    if ( ch->carry_number + 1 > MAX_CARRY )
    {
      send_to_char( "You have your hands full.\n\r", ch );
      return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
      send_to_char( "You can't carry that much weight.\n\r", ch );
      return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
      send_to_char( "Very daring, and you got it!\n\r", ch );
      unequip_char( victim, obj );
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "Nice work.\n\r", ch );
    if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_STEAL ))
    {
      setoff_trap( ch, obj );
      if( ch->deleted || ch->position == POS_DEAD )
        return;
    }
    return;
}

/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
    if ( IS_NPC( keeper ) && ( pShop = keeper->pIndexData->pShop )
        && ( argument[0] == '\0' || is_name( argument, keeper->name ) ) )
        break;
    }

    if ( !keeper || !pShop || ( keeper && is_affected( keeper, AFF_CHARM ) ) )
    {
    send_to_char( "You can't do that here.\n\r", ch );
    return NULL;
    }

    /*
     * Undesirables.
     */
    // Veygoth - removed code that would prevent shopkeepers from dealing
    // with pkillers.  However, thieves are not welcome.
/*
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
        do_say( keeper, "We do not allow cutpurses, or their ilk, here!" );
    return NULL;
    }
*/
    /*
     * Shop hours.
     */
    if ( time_info.hour < pShop->open_hour )
    {
    do_say( keeper, "Sorry, come back later." );
    return NULL;
    }
    
    if ( time_info.hour > pShop->close_hour )
    {
    do_say( keeper, "Sorry, come back tomorrow." );
    return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
    do_say( keeper, "I don't trade with folks I can't see." );
    return NULL;
    }

    return keeper;
}

int get_cost( CHAR_DATA *customer, CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int        cost;

    if ( !obj )
      return 0;

    if( !(pShop = keeper->pIndexData->pShop) )
    {
        cost = obj->cost;
    }
    else
    {
      if ( fBuy )
      {
        cost = obj->cost * pShop->profit_buy  / 100;
      }
      else
      {
        int       itype;

        cost = 0;
        for ( itype = 0; itype < MAX_BUY; itype++ )
        {
          if ( obj->item_type == pShop->buy_type[itype] )
          {
          cost = obj->cost * pShop->profit_sell / 100;
          break;
          }
        }
      }
    }

    if ( obj->item_type == TYPE_STAFF || obj->item_type == TYPE_WAND )
      cost = cost * obj->value[2] / obj->value[1];

    if( !fBuy )
      cost = (cost * (80 + get_curr_cha( customer ) / 5 )) / 100;
    else
      cost = (cost * (120 - get_curr_cha( customer ) / 5)) / 100;

    return cost;
}

/*
 * Multiple object buy modifications by Erwin Andreasen
 * Obtained from Canth's snippets page at:
 * http://www.xs4all.nl/~phule/snippets/snippets.html
 */
void do_buy( CHAR_DATA *ch, char *argument )
{
    char arg  [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    char arg3 [ MAX_INPUT_LENGTH ];
    char buf [ MAX_STRING_LENGTH ];
    bool       fPerm = FALSE;

    last_fun( "do_buy");
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2);
    one_argument( argument, arg3 );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Buy what?\n\r", ch );
    return;
    }

    if ( IS_ROOM( ch->in_room, ROOM_PET_SHOP ) )
    {
    CHAR_DATA       *pet;
    ROOM_INDEX_DATA *pRoomIndexNext;
    ROOM_INDEX_DATA *in_room;
    char             buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
        return;

    pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
    if ( !pRoomIndexNext )
    {
        bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
        send_to_char( "Sorry, you can't buy that here.\n\r", ch );
        return;
    }

    in_room     = ch->in_room;
    ch->in_room = pRoomIndexNext;
    pet         = get_char_room( ch, arg );
    ch->in_room = in_room;

    if ( !pet || !IS_SET( pet->act, ACT_PET ) )
    {
        send_to_char( "Sorry, you can't buy that here.\n\r", ch );
        return;
    }
        //  Snikt - Altered Pricing to match the ones that were given in the list
        //  command below... original was set to 500 * pet->level * pet->level
        //  Veygoth tweaked it to 100 * pet level squared

        if ( get_cash( ch ) < (( 100 * pet->level ) * pet->level) )
    {
        send_to_char( "You can't afford it.\n\r", ch );
        return;
    }

        spend_cash( ch, (( 100 * pet->level ) * pet->level) );
    pet      = create_mobile( pet->pIndexData );
    SET_BIT( pet->act,         ACT_PET        );
    SET_BIT( pet->act, ACT_NOEXP );
    SET_AFF_BIT( pet, AFF_CHARM );

    one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        sprintf( buf, "%s %s", pet->name, arg );
        free_string( pet->name );
        pet->name = str_dup( buf );
    }

        sprintf( buf, "%s&+LA neck tag says '&+RI belong to %s&+L'.&n\n\r",
        pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    send_to_char( "Enjoy your pet.\n\r", ch );
        act( "$n&n just purchased $N&n.", ch, NULL, pet, TO_ROOM );
    return;
    }
    else
    {
        OBJ_INDEX_DATA *pObj;
    OBJ_DATA  *obj = NULL;
    CHAR_DATA *keeper;
    int        cost;
    int        item_count = 1; /* buy only one by default */

    if ( is_number( arg ) )
    {
        item_count = atoi( arg );
        strcpy( arg, arg2 );
        strcpy( arg2, arg3 );
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
        return;

        if( keeper->pIndexData->pShop->sell_item[0] != 0 )
          {
            int count = 0;
            for( pObj = get_obj_index(keeper->pIndexData->pShop->sell_item[++count]);
                      count < MAX_SELL;
                      pObj = get_obj_index(keeper->pIndexData->pShop->sell_item[++count] ))
            {
               if( keeper->pIndexData->pShop->sell_item[count] == 0 )
                  break;
               if( !pObj )
                  continue;

               if( is_name( arg, pObj->name ))
               {
                  obj = create_object( pObj, pObj->level );
                  fPerm = TRUE;
                  break;
               }
            }
        }
           
    if( !obj )
           obj = get_obj_carry( keeper, arg );

    if ( !obj || !can_see_obj( ch, obj ) )
    {
            act( "$n&+W tells you 'I don't sell that -- try 'list'.'&n",
        keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
            if( fPerm ) obj->deleted = TRUE;
        return;
    }

    cost = get_cost( ch, keeper, obj, TRUE );

        if( cost < obj->cost )
        {
           bug( "Shopkeeper with vnum %d sells for less than 100 percent of value.\n\r",
              keeper->pIndexData->vnum );
           cost = obj->cost;
        }

    if ( item_count < 1 )
    {
            send_to_char( "Buy how many?  Quantity must be greater than zero.\n\r",
             ch );
        return;
    }

    if ( get_cash( ch ) < ( cost * item_count ) )
    {
        if ( item_count == 1 )
        {
                act( "$n&+W tells you 'You can't afford to buy $p&+W.'&n",
            keeper, obj, ch, TO_VICT );
        ch->reply = keeper;
                if( fPerm ) obj->deleted = TRUE;
        return;
        }
        else
        {
        char buf [ MAX_STRING_LENGTH ];

        if ( ( get_cash( ch ) / cost ) > 0 )
            sprintf( buf,
                            "$n&+W tells you, 'You can only afford %d of those!'&n",
                ( get_cash( ch ) / cost ) );
        else
                    sprintf( buf, "$n&+W tells you, 'You can't even afford one!'&n" );

        act( buf, keeper, obj, ch, TO_VICT );
        ch->reply = keeper;
                if( fPerm ) obj->deleted = TRUE;
        return;
        }
    }
    
    if ( ch->carry_number + item_count > MAX_CARRY )
    {
        send_to_char( "You can't carry that many items.\n\r", ch );
            if( fPerm ) obj->deleted = TRUE;
        return;
    }

    if ( ch->carry_weight + ( item_count * get_obj_weight( obj ) ) >
        can_carry_w( ch ) )
    {
        send_to_char( "You can't carry that much weight.\n\r", ch );
            if( fPerm ) obj->deleted = TRUE;
        return;
    }

    if ( ( item_count > 1 )
        && !IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
    {
            act( "$n&+W tells you, 'Sorry, $p&+W is something I have only one of.'&n",
        keeper, obj, ch, TO_CHAR );
        ch->reply = keeper;
            if( fPerm ) obj->deleted = TRUE;
        return;
    }

        if( cost < 1 ) cost = 1;

    if ( item_count == 1 )
    {
            sprintf( buf, "You buy $p&n for %s.", coin_string( cost * item_count ) );
        act( buf, ch, obj, NULL, TO_CHAR );
        act( "$n&n buys $p&n.", ch, obj, NULL, TO_ROOM );
    }
    else
    {
        sprintf( buf, "$n&n buys %d * $p&n.", item_count );
        act( buf, ch, obj, NULL, TO_ROOM );
        sprintf( buf, "You buy %d * $p&n.", item_count );
        act( buf, ch, obj, NULL, TO_CHAR );
    }

    spend_cash( ch, cost * item_count );

        if( fPerm )
        {
                obj_to_char( obj, ch );
        }
    else if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
        {
        for ( ; item_count > 0; item_count-- )
        {
        obj = create_object( obj->pIndexData, obj->level );
        obj_to_char( obj, ch );
        }
        }
    else
    {
        obj_from_char( obj );
        obj_to_char( obj, ch );
    }

    return;
    }
}

void do_list( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*4 ];
    OBJ_DATA *obj2;

    last_fun( "do_list");
    buf1[0] = '\0';

    if ( IS_ROOM( ch->in_room, ROOM_PET_SHOP ) )
    {
    CHAR_DATA       *pet;
    ROOM_INDEX_DATA *pRoomIndexNext;
    bool             found;
    int        cost;

    pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
    if ( !pRoomIndexNext )
    {
        bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    found = FALSE;
    for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
    {
        if ( IS_SET( pet->act, ACT_PET ) )
        {
            // Snikt, here.  I set this here so as to list a nicer and more comprehensive
            // pet shop  stock/price list.  I *think* I got it right.

            cost = (( 100 * pet->level ) * pet->level );
            if( cost <= 0 )
                cost = 1;
        if ( !found )
        {
            found = TRUE;
                    strcat( buf1, "&+LPets for sale&n&+r:&n\n\r" );
        }

                sprintf( buf, "%s&n for %s.\n\r",
                         pet->short_descr, coin_string( cost ) );
        strcat( buf1, buf );
        }
    }
    if ( !found )
        send_to_char( "Sorry, we're out of pets right now.\n\r", ch );

    send_to_char( buf1, ch );
    return;
    }
    else
    {
    OBJ_DATA  *obj;
        OBJ_INDEX_DATA *pObj;
    CHAR_DATA *keeper;
    char       arg  [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        cost;
    bool       found;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
        return;
      
        if( IS_IMMORTAL( ch ))
        {
            sprintf( buf1, "Shop sell profit: %d percent    Shop buy profit: %d percent\n\r", 
                     keeper->pIndexData->pShop->profit_sell,
                     keeper->pIndexData->pShop->profit_buy );
        }

    found = FALSE;
    for ( obj = keeper->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc != WEAR_NONE )
            continue;

        cost = get_cost( ch, keeper, obj, TRUE );
            if( cost <= 0 )
                cost = 1;

        if ( can_see_obj( ch, obj )
        && (   arg[0] == '\0'
            || !str_cmp( arg, "all" )
            || is_name( arg, obj->name ) ) )
        {
        if ( !found )
        {
            found = TRUE;
                    strcat( buf1, "&+LYou can buy&n&+r:&n\n\r" );
        }

                sprintf( buf, "%s&n for %s.\n\r",
                 obj->short_descr, coin_string( cost ) );
        strcat( buf1, buf );
        }
    }

        if( keeper->pIndexData->pShop->sell_item[0] != 0 )
        {
          bool fListed = FALSE;
          int count = 0;
          for( pObj = get_obj_index(keeper->pIndexData->pShop->sell_item[count]);
                      count < MAX_SELL;
                      pObj = get_obj_index(keeper->pIndexData->pShop->sell_item[count] ))
          {
               count++;
               if( keeper->pIndexData->pShop->sell_item[count] == 0 )
                  break;
               if( !pObj )
                  continue;
               for( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
               {
                  if( obj2->pIndexData == pObj && obj2->wear_loc == WEAR_NONE )
                  {
                     fListed = TRUE;
                     break;
                  }
               }
               if( fListed )
                  continue;
               cost = pObj->cost;
               sprintf( buf, "%s&n for %s.\n\r", 
                        pObj->short_descr, coin_string( cost ));
               strcat( buf1, buf );
          }
        }

    if ( !found )
    {
        if ( arg[0] == '\0' )
        send_to_char( "You can't buy anything here.\n\r", ch );
        else
        send_to_char( "You can't buy that here.\n\r", ch );
        return;
    }

    send_to_char( buf1, ch );
    return;
    }
}

char *coin_string( int cost )
{
      int coins;
      char buf[MAX_STRING_LENGTH];
      static char buf1[MAX_STRING_LENGTH];
      int cop;
      int sil;
      int gol;
      int pla;

      buf1[0] = '\0';

                coins = cost;
                pla = coins / 1000;
                coins -= pla * 1000;
                gol = coins / 100;
                coins -= gol * 100;
                sil = coins / 10;
                coins -= sil * 10;
                cop = coins;

                if( pla >= 0 )
                {
                  sprintf( buf, " %d&+W platinum&n", pla );      
                  strcat( buf1, buf );
                }
                if( gol >= 0 )
                {
                  sprintf( buf, " %d&+Y gold&n", gol );      
                  strcat( buf1, buf );
                }
                if( sil >= 0 )
                {
                  sprintf( buf, " %d&n&+w silver&n", sil );      
                  strcat( buf1, buf );
                }
                if( cop >= 0 )
                {
                  sprintf( buf, " %d&n&+y copper&n", cop );      
                  strcat( buf1, buf );
                }
      if( buf1[0] != 0 )
        return buf1;
      else
        return " nothing";
}

void do_sell( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    int        cost;

    last_fun( "do_sell");
    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Sell what?\n\r", ch );
    return;
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
    return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
        act( "$n&+W tells you 'You don't have that item.'&n",
        keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
    send_to_char( "You can't let go of it.\n\r", ch );
    return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
        act( "$n&+W tells you 'I can't see that item.'&n",
        keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
    }
    
    if ( ( cost = get_cost( ch, keeper, obj, FALSE ) ) <= 0 || IS_OBJ_STAT( obj, ITEM_NOSELL ) )
    {
    act( "$n&n looks uninterested in $p&n.", keeper, obj, ch, TO_VICT );
    return;
    }

    if( cost > obj->cost )
    {
        bug( "Shopkeeper with vnum %d buys for more than 100 percent of value.\n\r",
           keeper->pIndexData->vnum );
        cost = obj->cost;
    }

    if( cost < 1 ) cost = 1;

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
        act( "$n&+W tells you 'I won't buy that!  It's poisoned!'&n",
        keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    sprintf( buf, "You sell $p&n for %s.",
        coin_string( cost ) );
    act( buf, ch, obj, NULL, TO_CHAR );
    act( "$n&n sells $p&n.", ch, obj, NULL, TO_ROOM );
    receive_cash( ch, cost );
    
    if ( obj->item_type == TYPE_TRASH )
    {
    extract_obj( obj );
    }
    else
    {
    obj_from_char( obj );
    obj_to_char( obj, keeper );
    }

    return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    int        cost;

    last_fun( "do_value");
    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Value what?\n\r", ch );
    return;
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
    return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
    act( "$n&n tells you 'You don't have that item'.",
        keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
    send_to_char( "You can't let go of it.\n\r", ch );
    return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
        act( "$n&+W tells you 'You are offering me an imaginary object!?!?'&n",
            keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if ( ( cost = get_cost( ch, keeper, obj, FALSE ) ) <= 0 )
    {
    act( "$n&n looks uninterested in $p&n.", keeper, obj, ch, TO_VICT );
    return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
        act( "$n&+W tells you 'I won't buy that!  It's poisoned!'&n",
        keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    sprintf( buf, "$n&+W tells you 'I'll give you %d &n&+ycopper&+W coins for $p&n.'", cost );    act( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}

/* Undid the goofy Envy black powder + water stuff and made it a simple
 * application of ichor to a blade.
 *
 * - Lithos
 */
void do_poison_weapon( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *pobj;
    char      arg [ MAX_INPUT_LENGTH ];
    AFFECT_DATA af;

    last_fun( "do_poison_weapon");
    /* Don't allow mobs or unskilled pcs to do this */
    if ( IS_NPC( ch )
    || (  !IS_NPC( ch )
        && ch->level
        < skills_table[gsn_poison_weapon].skill_level[ch->class] ) )
    {                                          
    send_to_char( "What do you think you are, a thief?\n\r", ch );
    return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )                                              
    { send_to_char( "What are you trying to poison?\n\r",    ch ); return; }
    if ( ch->fighting )                                       
    { send_to_char( "While you're fighting?  Nice try.\n\r", ch ); return; }
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    { send_to_char( "You do not have that weapon.\n\r",      ch ); return; }
    if ( obj->item_type != TYPE_WEAPON )
    { send_to_char( "That item is not a weapon.\n\r",        ch ); return; }
    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    { send_to_char( "That weapon is already poisoned.\n\r",  ch ); return; }
    if ( obj->value[0] != 2 )
    { send_to_char("You don't have enough poison to cover that!\n\r", ch); return;}


    /* Now we have a valid weapon...check to see if we have the poison. */
    for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
    {
      // here is where we should check to see if they have poison
        if (pobj->item_type == TYPE_DRINK_CON 
            && pobj->value[2] == 27) {
            break;
        }
    }
    if ( !pobj )
    {
        send_to_char( "You do not have any poison.\n\r", ch );
        return;
    }

    if (pobj->value[1] <= 0 && pobj->value[1] != -1) {
        act("Sorry, $p&n seems to be empty.",ch, pobj, NULL, TO_CHAR);
        return;
    }

    WAIT_STATE( ch, skills_table[gsn_poison_weapon].beats );

    skill_practice( ch, gsn_poison_weapon );

    /* Check the skill percentage */
    if ( !IS_NPC( ch )
    && number_percent( ) > ch->pcdata->skl_lrn[gsn_poison_weapon] )
    {
        send_to_char( "You failed and spill some on yourself.  &+ROuch!&n\n\r",
             ch );
        damage( ch, ch, ch->level, gsn_poison_weapon, WEAR_NONE, DAM_POISON );
        act( "$n spills the &+Gpoison&n all over!", ch, NULL, NULL, TO_ROOM );
        pobj->value[1] -= 2;
        return;
    }

    /* Can't have people smearing gunk on artifacts */
    if ( insult_artifact( ch, obj ) )
    {
        pobj->value[1]--;
        return;
    }

    act( "You apply the &+Gpoison&n to $p&n, which glistens wickedly!",
    ch, obj, NULL, TO_CHAR  );
    act( "$n&n pours the &+Gli&n&+gq&+Gu&n&+gid&n over $p&n, which glistens wickedly!",
    ch, obj, NULL, TO_ROOM  );
//    SET_OBJ_STAT( obj, ITEM_POISONED );

    af.skill = gsn_poison_weapon;
    af.spell = 0;
    af.song = 0;
    af.duration = ch->level + dice(4, ch->level/2);
    af.location = APPLY_NONE;
    af.modifier = pobj->value[3];
    af.level = ch->level;
    set_bitvector( &af, AFF_NONE );
    affect_to_obj( obj, &af );
    pobj->value[1]--;
    return;
}

/* 
 * Original Code by Todd Lair.
 * Improvements and Modification by Jason Huang <huangjac@netcom.com>.
 * Permission to use this code is granted provided this header is
 * retained and unaltered.
 *
 * Made a couple of changes here and there - Zen :-)
 */
void imprint_spell( int sn, int level, CHAR_DATA * ch, void *vo )
{
    static const int    sucess_rate[] = { 80, 30, 25, 10 };

    char      buf [ MAX_STRING_LENGTH ];
    OBJ_DATA *obj = ( OBJ_DATA * ) vo;
    int       free_slots;
    int       i;
    int       mana;

    last_fun( "do_imprint_spell");
    if ( spells_table[sn].spell_fun == spell_null )
    {
    send_to_char( "That is not a spell.\n\r", ch );
    return;
    }

    for ( free_slots = i = 1; i < 5; i++ )
    if ( obj->value[i] != -1 )
        free_slots++;

    if ( free_slots > 4 )
    {
    act( "$p&n cannot contain any more spells.", ch, obj, NULL, TO_CHAR );
    return;
    }

    mana = 4 * MANA_COST( ch, sn );

    if ( !IS_NPC( ch ) && ch->mana < mana )
    {
    send_to_char( "You don't have enough mana.\n\r", ch );
    return;
    }

    if ( number_percent( ) > ch->pcdata->spl_lrn[sn]
    && ( ch->level <= (spells_table[sn].spell_circle[ch->class] * 5 + 1) ) )
    {
    send_to_char( "You lost your concentration.\n\r", ch );
        act( "&+r$n&n&+r stops chanting abruptly.&n", ch, NULL, NULL, TO_ROOM );
    ch->mana -= mana / 2;
    return;
    }

    ch->mana -= mana;
    obj->value[free_slots] = sn;

    if ( number_percent( ) > sucess_rate[free_slots - 1] )
    {
    sprintf( buf, "The magic enchantment has failed: the %s vanishes.\n\r",
        item_type_name( obj ) );
    send_to_char( buf, ch );
    extract_obj( obj );
    return;
    }


    free_string( obj->short_descr );
    sprintf( buf, "a %s of ", item_type_name( obj ) );
    for ( i = 1; i <= free_slots; i++ )
    if ( obj->value[ i ] != -1 )
    {
        strcat( buf, spells_table[ obj->value[ i ] ].name );
        ( i != free_slots ) ? strcat( buf, ", " ) : strcat( buf, "" );
    }
    obj->short_descr = str_dup( buf );

    sprintf( buf, "%s %s", obj->name, item_type_name( obj ) );
    free_string( obj->name );
    obj->name = str_dup( buf );

    sprintf( buf, "You have imbued a new spell to the %s.\n\r",
        item_type_name( obj ) );
    send_to_char( buf, ch );

    return;
}

void do_brew( CHAR_DATA * ch, char *argument )
{
    OBJ_DATA *potion;
    char      arg [ MAX_INPUT_LENGTH ];
    int       sn;

    last_fun( "do_brew");
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Which spell do you want to brew into a &+Lpotion&n?\n\r", ch );
    return;
    }

    if ( !( potion = get_eq_char( ch, WEAR_HAND ) ) )
    {
    send_to_char( "You hold nothing in your hand.\n\r", ch );
    return;
    }

    if ( potion->item_type != TYPE_POTION )
    {
    send_to_char( "You are not holding a vial.\n\r", ch );
    return;
    }

    if ( ( sn = spell_lookup( arg ) ) < 0 )
    {
    send_to_char( "You don't know any spells by that name.\n\r", ch );
    return;
    }

    if (   spells_table[sn].target != TAR_CHAR_DEFENSIVE
    && spells_table[sn].target != TAR_CHAR_SELF )
    {
    send_to_char( "You cannot brew that spell.\n\r", ch );
    return;
    }

    act( "$n begins preparing a &+Lpotion&n.", ch, potion, NULL, TO_ROOM );
    WAIT_STATE( ch, skills_table[ gsn_brew ].beats );

    skill_practice( ch, gsn_brew );

    if ( !IS_NPC( ch )
    && ( number_percent( ) > ch->pcdata->skl_lrn[gsn_brew] ||
         number_percent( ) > ( ( get_curr_int( ch ) - 13 ) * 5 +
                   ( get_curr_wis( ch ) - 13 ) * 3 ) ) )
    {
    act( "$p&n explodes violently!", ch, potion, NULL, TO_CHAR );
    act( "$p&n explodes violently!", ch, potion, NULL, TO_ROOM );
    extract_obj( potion );
    damage( ch, ch, get_max_hit( ch ) / 16, gsn_brew, WEAR_NONE, DAM_ENERGY );
    return;
    }

    potion->level    = ch->level / 2;
    potion->value[0]    = ch->level / 4;
    imprint_spell( sn, ch->level, ch, potion );

    return;
}

void do_scribe_scroll( CHAR_DATA * ch, char *argument )
{
    OBJ_DATA *scroll;
    char      arg [ MAX_INPUT_LENGTH ];
    int       sn;

    last_fun( "do_scribe_scroll");
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Scribe what spell?\n\r", ch );
    return;
    }

    if ( !( scroll = get_eq_char( ch, WEAR_HAND ) ) )
    {
    send_to_char( "You hold nothing in your hand.\n\r", ch );
    return;
    }

    if ( scroll->item_type != TYPE_SCROLL )
    {
        send_to_char( "You are not holding a &+Wparchment&n.\n\r", ch );
    return;
    }

    if ( ( sn = spell_lookup( arg ) ) < 0 )
    {
    send_to_char( "You don't know any spells by that name.\n\r", ch );
    return;
    }

    act( "$n begins writing a &+Wscroll&n.", ch, scroll, NULL, TO_ROOM );
    WAIT_STATE( ch, skills_table[gsn_scribe].beats );

    skill_practice( ch, gsn_scribe );

    if ( !IS_NPC( ch )
    && ( number_percent( ) > ch->pcdata->skl_lrn[gsn_brew] ||
         number_percent( ) > ( ( get_curr_int( ch ) - 13 ) * 5 +
                   ( get_curr_wis( ch ) - 13 ) * 3 ) ) )
    {
        act( "$p&n bursts in &n&+rflames&n!", ch, scroll, NULL, TO_CHAR );
        act( "$p&n bursts in &+Rflames&n!", ch, scroll, NULL, TO_ROOM );
    extract_obj( scroll );
    damage( ch, ch, get_max_hit( ch ), gsn_scribe, WEAR_NONE, DAM_FIRE );
    return;
    }

    scroll->level    = ch->level * 2 / 3;
    scroll->value[0]    = ch->level / 3;
    imprint_spell( sn, ch->level, ch, scroll );

    return;
}

int get_cash( CHAR_DATA *ch )
{
    return (ch->money.copper + (ch->money.silver * 10) +
               (ch->money.gold * 100) + (ch->money.platinum * 1000));
}

void spend_cash( CHAR_DATA *ch, int amount )
{
        int value;

    last_fun( "do_spend_cash");
        if( get_cash( ch ) < amount )
        {
            bug( "Spend_cash: spending more money than player has.", 0 );
            ch->money.copper = 0;
            ch->money.silver = 0;
            ch->money.gold = 0;
            ch->money.platinum = 0;
        }

        value = get_cash( ch );

        ch->money.copper = 0;
        ch->money.silver = 0;
        ch->money.gold = 0;
        ch->money.platinum = 0;
        
        // Note that this will automatically convert a player's coins
        // to the most efficient type to carry via this manner - Veygoth
        receive_cash( ch, (value - amount));
}

void receive_cash( CHAR_DATA *ch, int amount )
{
        int number;

    last_fun( "receive_cash");
        number = amount / 1000;
    ch->money.platinum += number;
        amount -= number * 1000;
        number = amount / 100;
        ch->money.gold += number;
        amount -= number * 100;
        number = amount / 10;
        ch->money.silver += number;
        amount -= number * 10;
        ch->money.copper += amount;
        return;
}

// Trying to convince the MUD that it's smart enough to set the prices
// of items - Veygoth
void set_cost( OBJ_INDEX_DATA *obj )
{
        int multiplier;
        int value;

    last_fun( "set_cost");
        // find the value multiplier based on what the object is made of
        // expensive materials yield expensive objects
        switch( obj->material )
        {
           default:
              bug( "Unrecognized material type %d.  Setting to default.", obj->material );
              multiplier = 100;
              break;
           case MAT_UNDEFINED:
              multiplier = 100;
              break;
           case MAT_NONSUBSTANTIAL:
              multiplier = 100;
              break;
           case MAT_FLESH:
              multiplier = 30;
              break;
           case MAT_CLOTH:
              multiplier = 60;
              break;
           case MAT_BARK:
              multiplier = 20;
              break;
           case MAT_SOFTWOOD:
              multiplier = 50;
              break;
           case MAT_HARDWOOD:
              multiplier = 70;
              break;
           case MAT_GLASS:
              multiplier = 130;
              break;
           case MAT_CRYSTAL:
              multiplier = 140;
              break;
           case MAT_CERAMIC:
              multiplier = 65;
              break;
           case MAT_BONE:
              multiplier = 35;
              break;
           case MAT_STONE:
              multiplier = 80;
              break;
           case MAT_HIDE:
              multiplier = 60;
              break;
           case MAT_LEATHER:
              multiplier = 80;
              break;
           case MAT_CUREDLEATHER:
              multiplier = 85;
              break;
           case MAT_IRON:
              multiplier = 100;
              break;
           case MAT_STEEL:
              multiplier = 110;
              break;
           case MAT_BRASS:
              multiplier = 95;
              break;
           case MAT_MITHRIL:
              multiplier = 150;
              break;
           case MAT_ADAMANTIUM:
              multiplier = 200;
              break;
           case MAT_BRONZE:
              multiplier = 90;
              break;
           case MAT_COPPER:
              multiplier = 50;
              break;
           case MAT_SILVER:
              multiplier = 100;
              break;
           case MAT_ELECTRUM:
              multiplier = 150;
              break;
           case MAT_GOLD:
              multiplier = 250;
              break;
           case MAT_PLATINUM:
              multiplier = 500;
              break;
           case MAT_GEM:
              multiplier = 750;
              break;
           case MAT_DIAMOND:
              multiplier = 1000;
              break;
           case MAT_PAPER:
              multiplier = 35;
              break;
           case MAT_PARCHMENT:
              multiplier = 25;
              break;
           case MAT_LEAVES:
              multiplier = 20;
              break;
           case MAT_RUBY:
              multiplier = 800;
              break;
           case MAT_EMERALD:
              multiplier = 900;
              break;
           case MAT_SAPPHIRE:
              multiplier = 950;
              break;
           case MAT_IVORY:
              multiplier = 300;
              break;
           case MAT_DRAGONSCALE:
              multiplier = 220;
              break;
           case MAT_OBSIDIAN:
              multiplier = 130;
              break;
           case MAT_GRANITE:
              multiplier = 88;
              break;
           case MAT_MARBLE:
              multiplier = 83;
              break;
           case MAT_LIMESTONE:
              multiplier = 68;
              break;
           case MAT_LIQUID:
              multiplier = 100;
              break;
           case MAT_BAMBOO:
              multiplier = 30;
              break;
           case MAT_REEDS:
              multiplier = 28;
              break;
           case MAT_HEMP:
              multiplier = 42;
              break;
           case MAT_GLASSTEEL:
              multiplier = 125;
              break;
           case MAT_EGGSHELL:
              multiplier = 15;
              break;
           case MAT_CHITINOUS:
              multiplier = 22;
              break;
           case MAT_REPTILESCALE:
              multiplier = 82;
              break;
           case MAT_GENERICFOOD:
              multiplier = 100;
              break;
           case MAT_RUBBER:
              multiplier = 80;
              break;
           case MAT_WAX:
              multiplier = 45;
              break;
           case MAT_FEATHER:
              multiplier = 38;
              break;
           case MAT_PEARL:
              multiplier = 550;
              break;
           case MAT_SILK:
              multiplier = 205;
              break;
           case MAT_SPIDERSILK:
              multiplier = 235;
              break;
           case MAT_JADE:
              multiplier = 920;
              break;
           case MAT_LEAD:
              multiplier = 60;
        }

       if( obj->craftsmanship != 0 )
       {
           multiplier = (obj->craftsmanship * multiplier) / CRAFT_AVERAGE;
       }

       // Value is in copper pieces for base value
       switch( obj->item_type )
       {
           default:
             value = 10;
             break;
           case TYPE_ARMOR:
             value = obj->value[0] * obj->value[0] * 6 + 10;
             break;
           case TYPE_WEAPON:
             value = (obj->value[1] * obj->value[2] * 2) * 50;
             break;
           case TYPE_STAFF:
           case TYPE_SCROLL:
           case TYPE_WAND:
           case TYPE_POTION:
             value = 10000;
             break;
           case TYPE_FOOD:
             value = 20;
             break;
           case TYPE_LIGHT:
             value = 10;
             break;
           case TYPE_DRINK_CON:
             value = 18;
             break;
           case TYPE_PEN:
             value = 80;
             break;
           case TYPE_CONTAINER:
             value = 50;
             break;
           case TYPE_SCABBARD:
             value = 40;
             break;
           case TYPE_TOTEM:
             value = 90;
             break;
           case TYPE_SPELLBOOK:
             value = (3 * obj->value[2]) + 10;
             break;
           case TYPE_TREASURE:
             value = 100;
             break;
           case TYPE_CLOTHING:
             value = 11;
             break;
           case TYPE_TRASH:
             value = 1;
             break;
           case TYPE_BOAT:
             value = 100;
             break;
           case TYPE_SHIP:
             value = 1000;
             break;
           case TYPE_INSTRUMENT:
             value = 100;
             break;
       }

       // Just to make it _slightly_ less monotonous
       multiplier = number_fuzzy( multiplier );

       obj->cost = (( value * multiplier ) / 100);

       if( obj->condition != 0 )
          obj->cost = ( obj->cost * obj->condition ) / 100;

       if( obj->cost == 0 )
         obj->cost = 1;
}

void do_deposit( CHAR_DATA *ch, char *argument )
{
    int   coinage;
    bool  success = FALSE;
    COIN_DATA coin;

    last_fun( "do_deposit");
    if( IS_NPC( ch ))
    {
        send_to_char( "NPCs do not have bank accounts!\n\r", ch );
        return;
    }

    if ( !IS_ROOM( ch->in_room, ROOM_BANK )) {
        send_to_char( "You're not in a bank!\n\r", ch );
        return;
    }

    if ( argument == NULL || argument[0] == '\0' ) {
        send_to_char( "Deposit what?\n\r", ch );
        return;
    }
    if ( !fill_coin_data(&coin, argument, &(ch->money) ) ) {
        send_to_char( "&+LSyntax:  &+RDeposit &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
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
                ch->pcdata->bank.copper += coin.copper;
                success = TRUE;
                break;
            case 1: //silver
                if (coin.silver < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.silver > ch->money.silver) {
                    send_to_char("You don't have that much &+wsilver&n coin!\n\r", ch);
                    continue;
                }
                if (coin.silver == 0) continue;
                ch->money.silver -= coin.silver;
                ch->pcdata->bank.silver += coin.silver;
                success = TRUE;
                break;
            case 2: //gold
                if (coin.gold < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.gold > ch->money.gold) {
                    send_to_char("You don't have that much &+Ygold&n coin!\n\r", ch);
                    continue;
                }
                if (coin.gold == 0) continue;
                ch->money.gold -= coin.gold;
                ch->pcdata->bank.gold += coin.gold;
                success = TRUE;
                break;
            case 3: //platinum
                if (coin.platinum < 0) {
                    send_to_char("You can't deposit a debt!\n\r", ch);
                    continue;
                    }
                if (coin.platinum > ch->money.platinum) {
                    send_to_char("You don't have that much &+Wplatinum&n coin!\n\r", ch);
                    continue;
                }
                if (coin.platinum == 0) continue;
                ch->money.platinum -= coin.platinum;
                ch->pcdata->bank.platinum += coin.platinum;
                success = TRUE;
                break;
            } //end switch
        } //end for
    if (success) {
        send_to_char( "You make your deposit.\n\r", ch );
        act( "$n&n makes a transaction.", ch, NULL, NULL, TO_ROOM );
    }
    else {
        send_to_char( "&+LSyntax:  &+RDeposit &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
    }
    return;
}

void do_withdraw( CHAR_DATA *ch, char *argument )
{
    int   coinage;
    bool  success = FALSE;
    COIN_DATA coin;

    last_fun( "do_withdraw");
    if( IS_NPC( ch ))
        return;

    if ( !IS_ROOM( ch->in_room, ROOM_BANK )) {
        send_to_char( "You're not in a bank!\n\r", ch );
        return;
    }

    if ( argument == NULL || argument[0] == '\0' ) {
        send_to_char( "Withdraw what?\n\r", ch );
        return;
    }
    if ( !fill_coin_data(&coin, argument, &(ch->pcdata->bank) ) ) {
        send_to_char( "&+LSyntax:  &+RWithdraw &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
        return;
    }
    if ( !coin.copper && !coin.silver && !coin.gold && !coin.platinum ) {
        send_to_char( "The bank has none of that type of &+Lcoin&n yet.\n\r", ch );
        return;
    }
    for (coinage = 0; coinage < 4; coinage++ ) {
        switch (coinage) {
            case 0: //copper
                if (coin.copper < 0) {
                    send_to_char("You can't liquidate a debt!\n\r", ch);
                    continue;
                    }
                if (coin.copper > ch->pcdata->bank.copper) {
                    send_to_char("You haven't saved that much &+ycopper&n coin!\n\r", ch);
                    continue;
                }
                if (coin.copper == 0) continue;
                ch->money.copper += coin.copper;
                ch->pcdata->bank.copper -= coin.copper;
                success = TRUE;
                break;
            case 1: //silver
                if (coin.silver < 0) {
                    send_to_char("You can't liquidate a debt!\n\r", ch);
                    continue;
                    }
                if (coin.silver > ch->pcdata->bank.silver) {
                    send_to_char("You haven't saved that much &+wsilver&n coin!\n\r", ch);
                    continue;
                }
                if (coin.silver == 0) continue;
                ch->money.silver += coin.silver;
                ch->pcdata->bank.silver -= coin.silver;
                success = TRUE;
                break;
            case 2: //gold
                if (coin.gold < 0) {
                    send_to_char("You can't liquidate a debt!\n\r", ch);
                    continue;
                    }
                if (coin.gold > ch->pcdata->bank.gold) {
                    send_to_char("You haven't saved that much &+Ygold&n coin!\n\r", ch);
                    continue;
                }
                if (coin.gold == 0) continue;
                ch->money.gold += coin.gold;
                ch->pcdata->bank.gold -= coin.gold;
                success = TRUE;
                break;
            case 3: //platinum
                if (coin.platinum < 0) {
                    send_to_char("You can't liquidate a debt!\n\r", ch);
                    continue;
                    }
                if (coin.platinum > ch->pcdata->bank.platinum) {
                    send_to_char("You haven't saved that much &+Wplatinum&n coin!\n\r", ch);
                    continue;
                }
                if (coin.platinum == 0) continue;
                ch->money.platinum += coin.platinum;
                ch->pcdata->bank.platinum -= coin.platinum;
                success = TRUE;
                break;
            } //end switch
    } //end for
    if (success) {
        send_to_char( "You make a withdrawal.\n\r", ch );
        act( "$n&n makes a transaction.", ch, NULL, NULL, TO_ROOM );
        }
    else {
        send_to_char( "&+LSyntax:  &+RDeposit &n&+r<&+L# of coins&n&+r> <&+Lcoin type&n&+r>&n\n\r", ch );
    }
    return;
}

// Rewritten by Veygoth 5-30-00
// now calls equip_hand to resolve eq placement
void do_hold( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_hold");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You can't move!\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && is_affected( ch, AFF_GHOUL ) )
    {
      send_to_char(  "You may not wear, wield, or hold anything in &+Wghoul&n form.\n\r",  ch );
      return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' ||
     !( obj = get_obj_carry ( ch, arg ) ) )
    {
      send_to_char( "Hold what now?\n\r", ch );
      return;
    }

    // Light items are automatically holdable.
    if ( !CAN_WEAR( obj, ITEM_HOLD ) && obj->item_type != TYPE_LIGHT )
    {
        send_to_char( "You can't hold that!\n\r" , ch );
        return;
    }

    if( obj->item_type == TYPE_WEAPON || obj->item_type == TYPE_RANGED_WEAPON )
    {
        send_to_char( "You WIELD weapons, they're useless if you hold them.\n\r", ch );
        return;
    }

     if( !wearable_by( ch, obj ))
       return;

    if( equip_hand( ch, obj, EQUIP_HOLD ) )
    {
        act("You hold $p&n.", ch, obj, NULL, TO_CHAR);
        act("$n&n holds $p&n.", ch, obj, NULL, TO_ROOM);
    }

    return;
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int door;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj, *obj2;
    ROOM_INDEX_DATA *location;

    last_fun( "do_drag");
    if( is_affected( ch, AFF_HOLD ) ||
        is_affected( ch, AFF_MINOR_PARA ))
    {
        send_to_char( "You can't move!\n\r", ch );
        return;
    }

    if( argument[0] == '\0' )
    {
      send_to_char( "Drag which what where?\n\r", ch );
      return;
    }

    argument = one_argument( argument, arg1 );

    if( argument[0] == '\0' )
    {
      send_to_char( "You need to specify a direction.\n\r", ch );
      return;
    }

    argument = one_argument( argument, arg2 );

    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEMORIZING ))
    {
        REMOVE_BIT( ch->act, PLR_MEMORIZING );
        send_to_char( "You abandon your studies.\n\r", ch );
    }

    if( !( obj = get_obj_here( ch, arg1 )))
    {
        send_to_char( "You do not see that here.\n\r", ch );
        return;
    }

    if( obj->item_type != TYPE_CORPSE_NPC && obj->item_type != TYPE_CORPSE_PC )
    {
        send_to_char( "You can only drag corpses.\n\r", ch );
        return;
    }


    if ( !strcmp( arg2, "enter" ) ) {
        one_argument( argument, arg2 );
        if ( ( obj2 = get_obj_here( ch, arg2) ) ) {
            switch ( obj2->item_type ) {
            case TYPE_TELEPORT:
            case TYPE_PORTAL:
                if ( obj2->item_type == TYPE_TELEPORT
                    && !check_command( "enter", obj2->value[1] ) ) {
                    send_to_char("Nothing seems to happen.\n\r", ch);
                    return;
                };
                if ( IS_SET( obj2->value[3], PORTAL_RANDOM ) )
                    location = get_random_room();
                else
                    location = get_room_index( obj2->value[0] );
                if ( !location ) {
                    send_to_char( "That portal doesn't seem to go anywhere.\n\r", ch);
                    return;
                }
                act("You drag the $p&n into $P&n.", ch, obj, obj2, TO_CHAR);
                act("$n&n drags the $p&n into $P&n.", ch, obj, obj2, TO_ROOM);
                if ( obj2->value[2] >= 0 )  {
                    obj2->value[2] -= 2;
                    if ( obj2->value[2] <= 0 )  {
                        act("$p&n fades into nothingness.", ch, obj2, NULL, TO_ROOM );
                        obj_from_room( obj2 );
                    }
                }
                obj_from_room( obj );
                char_from_room( ch );
                char_to_room( ch, location );
                obj_to_room( obj, location );
                if ( obj2->item_type == TYPE_PORTAL)
                    act( "$n&n steps out of $P&n dragging the $p&n.", ch, obj, obj2, TO_ROOM );
                else
                    act( "$n&n appears from elsewhere, dragging the $p&n.", ch, obj, NULL, TO_ROOM);
                do_look( ch, "auto" );
                return;
                break;
            case TYPE_VEHICLE:
            case TYPE_SHIP:
                break;
            case TYPE_OTHER:
                break;
            default:
                send_to_char("That cannot be entered.\n\r", ch);
                return;
            }
        }
    }

    door = find_exit( ch, arg2 );

    if( door < 0 )
    {
        send_to_char( "You can't drag anything that way.\n\r", ch );
        return;
    }
    if( ch->move < 5 && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "You are too exhausted to drag that anywhere.\n\r", ch );
      return;
    }

    sprintf( buf, "You drag $p&n %s.", dir_name[door] );
    act( buf, ch, obj, NULL, TO_CHAR );
    sprintf( buf, "$n&n drags $p&n %s.", dir_name[door] );
    act( buf, ch, obj, NULL, TO_ROOM );

    obj_from_room( obj );
    ch->move -= 5;

    move_char( ch, door );
    WAIT_STATE( ch, number_range( 3, 12 ) );

    obj_to_room( obj, ch->in_room );
    act( "$n&n drags $p&n along behind $m.", ch, obj, NULL, TO_ROOM );
}

void do_auction( CHAR_DATA *ch, char *argument )
{
   send_to_char( "There is no such thing as auction.\n\r", ch );
   return;
}

void do_pour( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *otherobj;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];

    last_fun( "do_pour");
    if ( !check_blind( ch ) )
        return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    if ( obj->item_type != TYPE_DRINK_CON )
    {
    send_to_char( "You can't fill that.\n\r", ch );
    return;
    }

    if ( !str_cmp( "out", arg2 ) )
    {
    send_to_char( "You pour it out.\n\r", ch );
    obj->value[1] = 0;
    return;
    }

    if ( !( otherobj = get_obj_here( ch, arg2 ) ) )
    {
    send_to_char( "Pour it where?\n\r", ch );
    return;
    }

    if ( otherobj->value[2] != obj->value[2] && otherobj->value[1] != 0 )
    {
    send_to_char( "It's got another liquid in it.\n\r", ch );
    return;
    }

    act( "You fill $p&n.", ch, otherobj, NULL, TO_CHAR );
    otherobj->value[2] = obj->value[2];
    otherobj->value[1] += obj->value[1];
    obj->value[1] = 0;
    // Over pour in code => just pour it back in the first container.
    if ( otherobj->value[1] > otherobj->value[0] )
    {
    obj->value[1] = otherobj->value[1] - otherobj->value[0];
    otherobj->value[1] = otherobj->value[0];
    }
    return;
}


// recoding of the wear_obj function into a table-based system
// which is fairly complex, but easy to maintain via the table
// in const.c
// bitshifting is used to check each wear location for a piece of equipment.
void new_wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    int count;

    last_fun( "new_wear_obj");
    for( count = 0; count < 30; count++ )
    {
      if( CAN_WEAR( obj, (1 << count )))
      {
        if( wear_table[(1 << count)].part_needed == 0 || 
            IS_SET( race_table[ch->race].parts, wear_table[(1 << count)].part_needed ))
        {
          if( wear_table[(1 << count)].race_not_allowed == -1 ||
            ch->race != wear_table[(1 << count)].race_not_allowed )
          {
            if( (wear_table[(1<<count)].wear_location && !remove_obj( ch, wear_table[(1<<count)].wear_location, fReplace )) &&
                (wear_table[(1<<count)].wear_location2 && !remove_obj( ch, wear_table[(1<<count)].wear_location2, fReplace )) &&
                (wear_table[(1<<count)].wear_location3 && !remove_obj( ch, wear_table[(1<<count)].wear_location3, fReplace )))
              return;
            act( wear_table[(1<<count)].wear_msg, ch, obj, NULL, TO_CHAR );
            act( wear_table[(1<<count)].wear_msg2, ch, obj, NULL, TO_ROOM );
            // need to allow for multiple wear locations
            equip_char( ch, obj, wear_table[(1<<count)].wear_location );
            return;

          }
          else
          {
            send_to_char( "Your race cannot wear that type of equipment.\n\r", ch );
            return;
          }
        }
        else
        {
           send_to_char( "The design of your body prevents you from using that.\n\r", ch );
           return;
        }
      }
    }

}

void do_hide_item( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    last_fun( "do_hide_item");
    if( argument[0] == '\0' )
    {
       send_to_char( "Hide what?\n\r", ch );
       return;
    }

    if( !(obj = get_obj_room( ch, argument )))
    {
       send_to_char( "You do not see that here.\n\r", ch );
       return;
    }

    // dddd
    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
        act("You attempt to hide $p&n.", ch, obj, NULL, TO_CHAR);
        act("$n&n stands in front of $p&n with arms outstretched.",
            ch, obj, NULL, TO_ROOM);
        WAIT_STATE( ch, 12 );
        return;
    }
    else if ( obj->item_type == TYPE_CORPSE_PC || obj->item_type == TYPE_CORPSE_NPC )
    {
        send_to_char( "You can't hide corpses.\n\r", ch );
        return;
    }
    act( "You hide $p&n.", ch, obj, NULL, TO_CHAR );
    act( "$n&n hides $p&n.", ch, obj, NULL, TO_ROOM );
    SET_OBJ_STAT( obj, ITEM_SECRET );

    WAIT_STATE( ch, 24 );

    return;
}

bool insult_artifact( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if (IS_OBJ_STAT( obj, ITEM_ARTIFACT )  && !IS_IMMORTAL( ch ) )
    {
        act( "A &+Bbolt &+Wof &+Cenergy&n from $p zaps you!", ch, obj, NULL, TO_CHAR);
        act("A &+Bbolt &+Wof &+Cenergy&n streaks from $p&n and hits $n&n!", ch, obj, NULL, TO_ROOM);
        if (obj->carried_by == ch) {
            act("$p&n falls to the ground.", ch, obj, NULL, TO_ROOM);
            act("$p&n falls to the ground.", ch, obj, NULL, TO_CHAR);
            obj_from_char( obj );
            obj_to_room( obj, ch->in_room );
            obj->fly_level = ch->fly_level;
            spell_damage(ch, ch, dice(2,6), 0, DAM_OTHER);
        }
        return TRUE;
    }
    return FALSE;
}

void do_invoke( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    last_fun( "do_invoke");
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Invoke what?\n\r", ch );
    return;
    }

    if ( !( obj = get_obj_wear( ch, arg ) ) )
    {
        send_to_char( "You do not have that item equipped.\n\r", ch );
        return;
    }

    invoke( ch, obj );
    return;
}

void invoke( CHAR_DATA *ch, OBJ_DATA *obj )
{
    last_fun( "invoke");
    if ( IS_OBJ_AFF( obj, AFF_STONESKIN ) && !is_affected( ch, AFF_STONESKIN ) )
        spell_stone_skin( spl_stoneskin, UMAX(obj->level, ch->level),
            ch, (void *)ch );
    if ( IS_OBJ_AFF( obj, AFF_FLYING ) && !is_affected( ch, AFF_FLYING ) ) {
        SET_AFF_BIT( ch, AFF_FLYING );
        send_to_char("Your feet rise off the ground.\n\r", ch );
        act( "$n's feet rise off the ground.", ch, NULL, NULL, TO_ROOM );
    }
    return;
}

// equip_hand(), written by Veygoth because stock hand-slot equip code was an ugly kludge,
// and because four-armed thrikreen would be impossible without a function rewrite.
// This function's main access commands are wear, wield, and hold, which have the
// following flow:
// wear:  do_wear, wear_obj, equip_hand
// wield: do_wield, equip_hand
// hold:  do_hold, equip_hand
//
// We assume by this point that the character is physically able to use the item and will be able
// to
// equip it however specified.
bool equip_hand( CHAR_DATA *ch, OBJ_DATA *obj, int type )
{
   OBJ_DATA  *hand1;
   OBJ_DATA  *hand2;
   OBJ_DATA  *hand3;
   OBJ_DATA  *hand4;
   int weight = 0, first_avail = 0, second_avail = 0, last_avail = 0;

   hand1 = get_eq_char( ch, WEAR_HAND );
   hand2 = get_eq_char( ch, WEAR_HAND_2 );
   hand3 = get_eq_char( ch, WEAR_HAND_3 );
   hand4 = get_eq_char( ch, WEAR_HAND_4 );
   if( hand1 && (hand1->item_type == TYPE_WEAPON || hand1->item_type == TYPE_RANGED_WEAPON ))
     weight += get_obj_weight( hand1 );
   if( hand2 && (hand2->item_type == TYPE_WEAPON || hand2->item_type == TYPE_RANGED_WEAPON ))
     weight += get_obj_weight( hand2 );
   if( hand3 && (hand3->item_type == TYPE_WEAPON || hand3->item_type == TYPE_RANGED_WEAPON ))
     weight += get_obj_weight( hand3 );
   if( hand4 && (hand4->item_type == TYPE_WEAPON || hand4->item_type == TYPE_RANGED_WEAPON ))
     weight += get_obj_weight( hand4 );

   if( ch->race != RACE_THRIKREEN )
   {
     if( hand3 ) bug( "non-thrikreen wielding item in hand3", 0 );
     if( hand4 ) bug( "non-thrikreen wielding item in hand4", 0 );
   }

   // Find number of hand slots used and first available hand.
   // Be sure to handle twohanded stuff.
   if( hand4 && IS_OBJ_STAT( hand4, ITEM_TWOHANDED ))
   {
       bug( "Twohanded weapon in fourth hand -- this is not possible.", 0 );
   }
   if( hand3 && IS_OBJ_STAT( hand3, ITEM_TWOHANDED ))
   {
     if( hand4 )
     {
       bug( "Twohanded weapon in third hand with fourth hand holding twohanded weapon -- this is not possible, all twohanded must have a blank hand after it.", 0 );
     }
     hand4 = hand3;
   }
   if( hand2 && IS_OBJ_STAT( hand2, ITEM_TWOHANDED ))
   {
     if( hand3 )
     {
       bug( "Twohanded weapon in second hand with third hand holding twohanded weapon -- this is not possible, all twohanded must have a blank hand after it.", 0 );
     }
     hand2 = hand3;
   }
   if( !IS_SET( race_table[ch->race].race_abilities, RACE_EXTRA_STRONG_WIELD ) )
   {
     if( hand1 && IS_OBJ_STAT( hand1, ITEM_TWOHANDED ))
     {
       if( hand2 )
       {
         bug( "Twohanded weapon in second hand with first hand holding twohanded weapon -- this is not possible, all twohanded must have a blank hand after it.", 0 );
       }
       hand2 = hand1;
     }
   }

   if( IS_OBJ_STAT( obj, ITEM_TWOHANDED )
      && !IS_SET( race_table[ch->race].race_abilities, RACE_EXTRA_STRONG_WIELD ) )
   {
     if( ch->race == RACE_THRIKREEN && !hand4 )
     {
       first_avail = WEAR_HAND_4;
       last_avail = WEAR_HAND_4;
     }
     if( ch->race == RACE_THRIKREEN && !hand3 )
     {
       if( !last_avail) last_avail = WEAR_HAND_3;
       second_avail = first_avail;
       first_avail = WEAR_HAND_3;
     }
     if( !hand2 )
     {
       if( !last_avail) last_avail = WEAR_HAND_2;
       second_avail = first_avail;
       first_avail = WEAR_HAND_2;
     }
     if( !hand1 )
     {
       if( !last_avail) last_avail = WEAR_HAND;
       second_avail = first_avail;
       first_avail = WEAR_HAND;
     }

     if( !first_avail )
     {
        send_to_char( "Your hands are full!\n\r", ch );
        return FALSE;
     }
     else if( !second_avail )
     {
        send_to_char( "You need two hands free to wield that!\n\r", ch );
        return FALSE;
     }
   }
   else if( IS_OBJ_STAT( obj, ITEM_TWOHANDED ))
   {
     if( ch->race == RACE_THRIKREEN && !hand4 )
     {
       first_avail = WEAR_HAND_4;
     }
     if( ch->race == RACE_THRIKREEN && !hand3 )
     {
       if( !last_avail) last_avail = WEAR_HAND_3;
       second_avail = first_avail;
       first_avail = WEAR_HAND_3;
     }
     if( !hand2 )
     {
       if( !last_avail) last_avail = WEAR_HAND_2;
       second_avail = first_avail;
       first_avail = WEAR_HAND_2;
     }
     if( !hand1 )
     {
       if( !last_avail) last_avail = WEAR_HAND;
       second_avail = WEAR_HAND;
       first_avail = WEAR_HAND;
     }

     if( !first_avail )
     {
        send_to_char( "Your hands are full!\n\r", ch );
        return FALSE;
     }
     else if( !second_avail )
     {
        send_to_char( "You need two hands free to wield that!\n\r", ch );
        return FALSE;
     }
   }
   else
   {
     if( ch->race == RACE_THRIKREEN && !hand4 )
     {
       if( !last_avail) last_avail = WEAR_HAND_4;
       first_avail = WEAR_HAND_4;
     }
     if( ch->race == RACE_THRIKREEN && !hand3 )
     {
       if( !last_avail) last_avail = WEAR_HAND_3;
       first_avail = WEAR_HAND_3;
     }
     if( !hand2 )
     {
       if( !last_avail) last_avail = WEAR_HAND_2;
       first_avail = WEAR_HAND_2;
     }
     if( !hand1 )
     {
       if( !last_avail) last_avail = WEAR_HAND;
       first_avail = WEAR_HAND;
     }

     if( !first_avail )
     {
        send_to_char( "Your hands are full!\n\r", ch );
        return FALSE;
     }
   }

   // Successful hand availability, send message and ready the item.
   // Twohanded shields, held items, and lights are equipped primary.
   // This could annoy ogres/thris but twohanded versions of these items
   // are so rare it's not likely to be an issue.
   switch( type )
   {
      case EQUIP_HOLD:
        if( !IS_OBJ_STAT( obj, ITEM_TWOHANDED ))
          equip_char( ch, obj, last_avail );
        else
          equip_char( ch, obj, first_avail );
        break;
      case EQUIP_SHIELD:
        act( "You strap $p&n to your arm.", ch, obj, NULL, TO_CHAR );
        act( "$n&n straps $p&n to $s arm.",   ch, obj, NULL, TO_ROOM );
        if( !IS_OBJ_STAT( obj, ITEM_TWOHANDED ))
          equip_char( ch, obj, last_avail );
        else
          equip_char( ch, obj, first_avail );
        break;
      case EQUIP_LIGHT:
        if ( obj->item_type == TYPE_LIGHT && obj->value[2] != 0 )
        {
          act( "You &n&+rli&+Rght&n $p&n and hold it before you.",  ch, obj, NULL, TO_CHAR );
          act( "$n&n &+Rlig&n&+rhts&n $p&n and holds it before $m.", ch, obj, NULL, TO_ROOM );
        }
        else
        { 
          act( "You hold the &+Lspent&n remains of $p&n.",  ch, obj, NULL, TO_CHAR );
          act( "$n&n holds the spent husk of $p&n.", ch, obj, NULL, TO_ROOM );
        }
        if( !IS_OBJ_STAT( obj, ITEM_TWOHANDED ))
          equip_char( ch, obj, last_avail );
        else
          equip_char( ch, obj, first_avail );
        break;
      case EQUIP_WIELD:
         // Have to check for dual wield skill, and for total weight of weapons.
         if( first_avail != WEAR_HAND )
         {
             // Those without dual wield cannot wield anything in their second hand.  Include thrikreen
             if( !IS_NPC( ch )  &&  ch->level < skills_table[gsn_dual].skill_level[ch->class] )
             {
                 send_to_char( "You lack the skills to wield a weapon in anything but your primary hand.\n\r", ch );
                 return FALSE;
             }
         }
         if( (weight + get_obj_weight( obj )) > str_app[get_curr_str( ch )].wield )
         {
            act( "Your meager strength is overwhelmed by $p&n.", ch, obj, NULL, TO_CHAR );
            return FALSE;
         }
         act( "You wield $p&n.", ch, obj, NULL, TO_CHAR );
         act( "$n&n brandishes $p&n.", ch, obj, NULL, TO_ROOM );
         equip_char( ch, obj, first_avail );
         break;
   }

   // Objects with a trap activated on wear.
   if( obj->trap && obj->trap->charges != 0 && IS_SET( obj->trap->trigger, TRIG_WEAR ))
   {
       setoff_trap( ch, obj );
       if( ch->deleted || ch->position == POS_DEAD )
           return FALSE;
   }

   return TRUE;

}

/* This function returns the number of spaces needed to fill
 *   up total_space length (including string's length).
 * This function currently has a max of 60 spaces returned.
 * The const is in the right place. - Lohrr 
 */
char *fill_spaces( char const * string, int total_space )
{
    //                                 1         2         3         4         5         6
    //                        123456789012345678901234567890123456789012345678901234567890
    static char  spaces [] = "                                                            ";
    int          count = 0;
    char const  *point;

    // Set count to the number of (printing) chars in string.
    // Skip the colour codes.
    for( point = string; point[0] != '\0';point++ )
    {
        count++;
        if( point[0] == '&' )
        {
            point++;
            switch( LOWER( point[0] ) )
            {
                case 'n':
                    count--;
                break;
                case '+':
                case '-':
                    point++;
                    count--;
                break;
                default:
                    point--;
                break;
            }
        }
    }

    // Count now represents the number of spaces to be returned
    count = total_space - count;

    // We return a max of 60 spaces.
    if( count > 60 )
        count = 60;
    // We return a min of 00 spaces.
    if( count < 0 )
        count = 0;

    // return the spaces.
    // Note: spaces[60] is '\0' although it isn't shown in the definition of spaces[].
    return &(spaces[ 60 - count ]);
}
