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
Vehicle/Ship code

The control panel will be handled by the vnum of the room which contains the ship's
control panel.  This may or may not create an object in the control panel room,
this remains to be determined.

We need a method of determining who is able to fly the ship, possibly a bool flag called
locked, which allows only the keyholder to fly the ship.  Can also have a lower level
of lock that allows keyholder's group members to fly the ship, and open which allows
anyone to fly the ship.  Can also have maximum lock that means only the MUD can move the
ship.

Automated ships:
Ship movement patterns should be a string of which way to go.. I.E. "NESSENEENEESEENNNW...ESSSWW"
These can include up and down for flying ships and the "." means sit there and wait for
a bit.

Ships are essentially mobile zones and we need a way of representing them on the map or
in a room.

The disembark command should just scan through all the ship data and see if the room
is flagged as an entry room.  If it is, they can disembark.

How to accurately keep track of the people entering and leaving a ship?  Occupancy limits?
Well, I don't know.

Descriptions have to be entered in manually.

After creating a ship object that will load successfully, we need to give this to builders
and/or include it in the DE distribution and even the creation routines.
*/

void move_vehicle( void )
{
   return;
}

void load_vehicles( FILE *fp )
{
   return;
}

VEHICLE_DATA *new_vehicle( void )
{
    VEHICLE_DATA *vehicle;

    if ( !vehicle_free )
    {
        vehicle      = alloc_perm( sizeof( *vehicle ) );
        top_vehicle++;
    }
    else
    {
        vehicle      = vehicle_free;
        vehicle_free = vehicle_free->next;
    }

    memset( vehicle, 0, sizeof( VEHICLE_DATA ) );

    vehicle->deleted = FALSE;

    vehicle->next           = vehicle_list;
    vehicle_list            = vehicle;

    return vehicle;
}

void do_statvehicle( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vehicle;
    int number = 0;
    char buf[MAX_STRING_LENGTH];

    for( vehicle = vehicle_list; vehicle; vehicle = vehicle->next )
    {
      if( vehicle->deleted )
        continue;
      number++;
      sprintf( buf, "Vehicle type: %d  Hull points: %d  Owner: %s  FlyLevel: %d\n\r",
               vehicle->type, vehicle->hull, vehicle->owner, vehicle->fly_level );
      send_to_char( buf, ch );
      sprintf( buf, "Occupants: %d  Direction: %d  Speed: %d MT/MD/MP: %d %d %d\n\r",
               vehicle->occupants, vehicle->direction, vehicle->speed,
               vehicle->movement_timer, vehicle->movement_delay,
               vehicle->movement_pointer );
      send_to_char( buf, ch );
      if( !vehicle->ship )
      {
        send_to_char( "vehicle has no associated object data!", ch );
        continue;
      }
      sprintf( buf, "Room: %d\n\r",
               vehicle->ship->in_room->vnum );
      send_to_char( buf, ch );
      sprintf( buf, "Vnum: %d  Key: %d  MaxBoard: %d  MaxSpeed: %d  MaxHull: %d  Fire: %d\n\r",
               vehicle->ship->pIndexData->vnum, vehicle->ship->value[0],
               vehicle->ship->value[3], vehicle->ship->value[4],
               vehicle->ship->value[5], vehicle->ship->value[6] );
      send_to_char( buf, ch );
    }
    sprintf( buf, "%d total.\n\r", number );
    send_to_char( buf, ch );
}

void do_disembark( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vehicle;

    last_fun( "do_disembark");
    if( !ch->in_room )
      return;

    for( vehicle = vehicle_list; vehicle; vehicle = vehicle->next )
    {
      if( vehicle->deleted || !vehicle->ship )
        continue;
      if( ch->in_room->vnum == vehicle->entry_point )
      {
        if( vehicle->ship->in_room )
        {
          char_from_room( ch );
          char_to_room( ch, vehicle->ship->in_room );
          last_fun( "do_disembark");
          act( "$n&n disembarks from $p&n.", ch, vehicle->ship, NULL, TO_ROOM );
          act( "You disembark from $p&n.", ch, vehicle->ship, NULL, TO_CHAR );
          return;
        }
      }
    }
}

void do_lookpanel( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    VEHICLE_DATA *vehicle;

    if( !ch->in_room )
      return;

    for( vehicle = vehicle_list; vehicle; vehicle = vehicle->next )
    {
      if( vehicle->deleted || !vehicle->ship )
        continue;
      if( ch->in_room->vnum == vehicle->control_panel )
      {
        sprintf( buf, "Ship Control Panel Readout:\n\r"
                 "Hull: %d/%d  Occupants: %d/%d  Speed: %d/%d Firepower: %d\n\r",
                 vehicle->hull, vehicle->ship->value[5],
                 vehicle->occupants, vehicle->ship->value[3],
                 vehicle->speed, vehicle->ship->value[4],
                 vehicle->ship->value[6] );
        send_to_char( buf, ch );
        return;
      }
    }

    send_to_char( "You do not see that here.\n\r", ch );
    return;
}

void do_ordership( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vehicle;
    char arg[MAX_INPUT_LENGTH];

    if( !ch->in_room )
      return;

    argument = one_argument( argument, arg );

    for( vehicle = vehicle_list; vehicle; vehicle = vehicle->next )
    {
      if( vehicle->deleted || !vehicle->ship )
        continue;
      if( ch->in_room->vnum == vehicle->control_panel )
      {
          if( !str_cmp( arg, "speed" ))
          {
             if( !str_cmp( argument, "stop" ))
             {
                vehicle->speed = 0;
             }
             else if( !str_cmp( argument, "full" ) || !str_cmp( argument, "fast" ))
             {
                vehicle->speed = vehicle->ship->value[4];
             }
             else if( !str_cmp( argument, "medium" ))
             {
                vehicle->speed = vehicle->ship->value[4] * 2 / 3;
             }
             else if( !str_cmp( argument, "slow" ))
             {
                vehicle->speed = vehicle->ship->value[4] * 1 / 3;
             }
             else
             {
                send_to_char( "No such speed, try full, medium, slow, or stop.\n\r", ch );
                return;
             }
             act( "$n&n orders a change of speed.", ch, NULL, NULL, TO_ROOM );
             act( "You order a change of speed.", ch, NULL, NULL, TO_CHAR );
             return;
          }
          else if( !str_cmp( arg, "sail" ))
          {
             DESCRIPTOR_DATA *d;
             int door = -1;
             ROOM_INDEX_DATA *room;
             char buf[MAX_STRING_LENGTH];

  	          if ( !str_prefix( argument, "north" ) ) door = 0; 
 	     else if ( !str_prefix( argument, "east"  ) ) door = 1;
	     else if ( !str_prefix( argument, "south" ) ) door = 2;
	     else if ( !str_prefix( argument, "west"  ) ) door = 3;
	     else if ( !str_prefix( argument, "northwest" ) ) door = 6;
	     else if ( !str_prefix( argument, "southwest" ) ) door = 7;
	     else if ( !str_prefix( argument, "northeast" ) ) door = 8;
	     else if ( !str_prefix( argument, "southeast" ) ) door = 9;
	     else if ( !str_cmp( argument, "nw" ) ) door = 6;
	     else if ( !str_cmp( argument, "sw" ) ) door = 7;
	     else if ( !str_cmp( argument, "ne" ) ) door = 8;
	     else if ( !str_cmp( argument, "se" ) ) door = 9;

             if( door == -1 )
             {
               send_to_char( "That's not a valid direction.\n\r", ch );
               return;
             }

             if( !vehicle->ship->in_room->exit[door] ||
                 !(room = vehicle->ship->in_room->exit[door]->to_room))
             {
               send_to_char( "Your ship cannot go that way.\n\r", ch );
               return;
             }

             if( room->sector_type != SECT_OCEAN && !IS_ROOM( room, ROOM_DOCKABLE ))
             {
               send_to_char( "Your ship can only sail on deep water.\n\r", ch );
               return;
             }

             // Taken from the do_recho code - Veygoth
             sprintf( buf, "%s&n sails %s.", vehicle->ship->short_descr,
                      dir_name[door] );
             for ( d = descriptor_list; d; d = d->next )
             {
               if ( d->connected == CON_PLAYING
                    && d->character->in_room == vehicle->ship->in_room )
               {
                  send_to_char( buf, d->character );
                  send_to_char( "\n\r", d->character );
               }
             }
             act( buf, ch, vehicle->ship, NULL, TO_ROOM );
             act( buf, ch, vehicle->ship, NULL, TO_CHAR );
             sprintf( buf, "%s&n sails in from %s.", vehicle->ship->short_descr,
                      dir_rev[door] );
             for ( d = descriptor_list; d; d = d->next )
             {
               if ( d->connected == CON_PLAYING
                    && d->character->in_room == room )
               {
                  send_to_char( buf, d->character );
                  send_to_char( "\n\r", d->character );
               }
             }
             obj_from_room( vehicle->ship );
             obj_to_room( vehicle->ship, room );
             return;
          }
          else if( !str_cmp( arg, "fire" ))
          {
             send_to_char( "Not implemented yet.\n\r", ch );
             return;
          }
          else if( !str_cmp( arg, "ram" ))
          {
             send_to_char( "Not implemented yet.\n\r", ch );
             return;
          }
          else if( !str_cmp( arg, "board" ))
          {
             send_to_char( "Not implemented yet.\n\r", ch );
             return;
          }
          else
          {
             send_to_char( "What are you trying to order the ship do to?\n\r", ch );
             return;
          }
      }
    }

    send_to_char( "You can't order a ship when you are not at the helm.\n\r", ch );
    return;
}

void do_lookout( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vehicle;

    last_fun( "do_lookout");
    if( !ch->in_room )
      return;

    for( vehicle = vehicle_list; vehicle; vehicle = vehicle->next )
    {
      if( vehicle->deleted || !vehicle->ship )
        continue;
      if( ch->in_room->vnum == vehicle->entry_point )
      {
        if( vehicle->ship->in_room )
        {
          // Copied from farsee code - Veygoth
          ROOM_INDEX_DATA *room;
          room = ch->in_room;
          char_from_room( ch );
          char_to_room( ch, vehicle->ship->in_room );
          do_look( ch, "" );
          char_from_room( ch );
          char_to_room( ch, room );
          last_fun( "do_lookout");
          return;
        }
      }
    }

    send_to_char( "You do not see that here.\n\r", ch );
    return;
}

