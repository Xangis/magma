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
#include <string.h>
#include <time.h>
#include "merc.h"

char lbuf[MAX_STRING_LENGTH];

/*
struct crime_data
{
    CRIME_DATA *next;
    char *      criminal;
    char *      victim;
    char *      zone;
    int         crime;
    int         time;
};
*/

/*
 * See if an attack justifies an ATT_MURDER crime - Veygoth
 *
 * This requires that the person be in justice and that there
 * be a mob flagged witness present
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *wch;

    last_fun( "check_killer");

    /* Important check!!! */
    if( !ch || !victim || ch->deleted || victim-> deleted )
        return;

    /*
     * Check for justice.  Suicide is ok.  Make sure areas are there.
     */
    if( !ch->in_room || !ch->in_room->area || ch->in_room->area->justice == 0 )
      return;

    /*
     * NPC's are fair game.
     */
    if ( IS_NPC( victim ) )
        return;

    /*
     * NPC's are cool of course
     * Hitting yourself is cool too (bleeding).
     * Hitting immortals are fine.
     */
    if ( IS_NPC( ch )
        || ch == victim
        || victim->level > LEVEL_HERO )
        return;

    /*
     * Vampires are fair game.
     */
    if ( IS_UNDEAD( victim ))
        return;

    // defending yourself once you're already attacked is okay
    if( victim->fighting && victim->fighting == ch )
        return;

    for( wch = ch->in_room->people; wch; wch = wch->next_in_room )
    {
      if( !IS_NPC( ch ) && IS_SET( ch->act, ACT_WITNESS ))
      {
         // crime committed, add a crime data
         create_crime( ch, victim, CRIME_ATT_MURDER );
         save_crimes( );
         return;
      }
      else
      {
         // crime witnessed by player, give them the chance to report it
         return;
      }
    }

    return;
}

/*
 * See if an theft justifies an THEFT crime - Veygoth
 *
 * This requires that the person be in justice and that there
 * be a mob flagged witness present
 */
void check_thief( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *wch;
    /*
     * Check for justice
     */

    last_fun( "check_thief");
    if( ch->in_room->area->justice == 0 )
      return;

    /*
     * NPC's are fair game.
     */
    if ( IS_NPC( victim ) )
        return;

    /*
     * NPC's are cool of course
     * Hitting yourself is cool too (bleeding).
     * Hitting immortals are fine.
     */
    if ( IS_NPC( ch )
        || ch == victim
        || victim->level > LEVEL_HERO )
        return;

    /*
     * Vampires are fair game.
     */
    if ( IS_UNDEAD( victim ))
        return;

    for( wch = ch->in_room->people; wch; wch = wch->next_in_room )
    {
      if( !IS_NPC( ch ) && IS_SET( ch->act, ACT_WITNESS ))
      {
         // crime committed, add a crime data
         create_crime( ch, victim, CRIME_THEFT );
         save_crimes( );
         return;
      }
      else
      {
         // crime witnessed by player, give them the chance to report it
         return;
      }
    }

    return;
}


CRIME_DATA *create_crime( CHAR_DATA *ch, CHAR_DATA *victim, int type )
{
    CRIME_DATA *crime;

    last_fun( "create_crime");
    log_string( "Create crime called." );

    crime = alloc_mem( sizeof( *crime ));

    memset( crime, 0, sizeof( CRIME_DATA ));

    crime->next           = crime_list;
    crime_list            = crime;
    crime->criminal       = str_dup( ch->name );
    crime->victim         = str_dup( victim->name );
    crime->time           = (int)current_time;
    crime->crime          = type;
    crime->zone           = ch->in_room->area->filename;
    top_crime++;

    return crime;
}

void do_statcrime( CHAR_DATA *ch, char *argument )
{
    CRIME_DATA *crime;
    char buf[MAX_STRING_LENGTH];

    last_fun( "do_statcrime");
    for( crime = crime_list; crime; crime = crime->next )
    {
      sprintf( buf, "Offender: %s  Victim: %s  Time: %d  Crime: %d  Zone: %s\n\r",
        crime->criminal, crime->victim, crime->time, crime->crime, crime->zone  );
      send_to_char( buf, ch );
    }

    sprintf( buf, "%d total crimes.\n\r", top_crime );
    send_to_char( buf, ch );

    return;
}

void load_crimes( void )
{
   char *word;
   CRIME_DATA *crime;
   FILE      *fp;
   char       strsave [ MAX_INPUT_LENGTH ];
   int status;

    last_fun( "load_crimes");
   log_string( "Loading crimes..." );
log_string( "Buggy!  Not loading crimes!" );
return;

   sprintf( strsave, "%s%s", SYSTEM_DIR, CRIME_FILE );

   if ( ( fp = fopen( strsave, "r" ) ) )
   {
     fclose( fp );
   }
   else
   {
     bug( "Error opening crime file!  No crimes loaded!", 0 );
     return;
   }

   for ( ; !feof (fp) ; )
   {
        word = fread_string( fp, &status );

        if ( !word )
        {
            bug( "load_crimes:  Error reading key.  EOF?", 0 );
            fread_to_eol( fp );
            break;
        }

        if( !str_cmp( word, "Crime" ))
        {
           crime = create_crime( NULL, NULL, 0 );
        }
        else if( !str_cmp( word, "Offender" ))
        {
           crime->criminal = fread_string( fp, &status);
        }
        else if( !str_cmp( word, "Victim" ))
        {
           crime->victim = fread_string( fp, &status);
        }
        else if( !str_cmp( word, "Time" ))
        {
           crime->time = fread_number( fp, &status );
        }
        else if( !str_cmp( word, "Crime" ))
        {
           crime->crime = fread_number( fp, &status );
        }
        else if( !str_cmp( word, "Zone" ))
        {
           crime->zone = fread_string( fp, &status );
        }
        else if( !str_cmp( word, "End" ))
        {
           return;
        }

        bug( "Bad keyword in crime file.", 0 );
        bug( word, 0 );
   }

   return;
}

void save_crimes( void )
{
    CRIME_DATA *crime;
    char       strsave [ MAX_INPUT_LENGTH ];
    FILE *fp;
 
    last_fun( "save_crimes");
    log_string( "Saving crimes..." );

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, CRIME_FILE );
    if ( !( fp = fopen( strsave, "w" ) ) )
    {
      bug( "Error opening crime file for output!", 0 );
      return;
    }

    for( crime = crime_list; crime; crime = crime->next )
    {
      fprintf( fp, "Crime~       %d\n",    crime->crime );
      fprintf( fp, "Offender~    %s~\n",   crime->criminal );
      fprintf( fp, "Victim~      %s~\n",   crime->victim );
      fprintf( fp, "Time~        %d\n",    crime->time );
      fprintf( fp, "Zone~        %s~\n",   crime->zone );
    }

    fprintf( fp, "End~ \n" );

    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
   fclose( fp );

   return;
}

void check_invader( CHAR_DATA *ch )
{

    last_fun( "check_invader");
   if( IS_NPC( ch ) || IS_IMMORTAL( ch ))
     return;

   if( !ch || !ch->in_room || !ch->in_room->area ||
       !ch->in_room->area->justice )
     return;

   switch( race_table[ch->race].racewar_side )
   {
      case RACEWAR_NONE:
           break;
      case RACEWAR_GOOD:
           if( ch->in_room->area->justice == JUSTICE_EVIL
            || ch->in_room->area->justice == JUSTICE_NEUT_CLOSED
            || ch->in_room->area->justice == JUSTICE_LAWFUL_EVIL
            || ch->in_room->area->justice == JUSTICE_CHAOTIC_NEUT ) {
                sprintf(lbuf, "Check_invader: %s invading evil justice", ch->name);
                wiznet( 0, WIZ_LSPAM, 0, lbuf );
                start_invasion( ch );
            }
           return;
           break;
      case RACEWAR_EVIL:
           if( ch->in_room->area->justice == JUSTICE_GOOD
            || ch->in_room->area->justice == JUSTICE_NEUT_CLOSED
            || ch->in_room->area->justice == JUSTICE_CHAOTIC_GOOD
            || ch->in_room->area->justice == JUSTICE_CHAOTIC_NEUT ) {
                sprintf(lbuf, "Check_invader: %s invading good justice", ch->name);
                wiznet( 0, WIZ_LSPAM, 0, lbuf );
                start_invasion( ch );
            }
           return;
           break;
      case RACEWAR_NEUTRAL:
           if( ch->in_room->area->justice == JUSTICE_CLOSED_GOOD
            || ch->in_room->area->justice == JUSTICE_CLOSED_EVIL ) {
                sprintf(lbuf, "Check_invader: %s invading !neutral justice", ch->name);
                wiznet( 0, WIZ_LSPAM, 0, lbuf );
                start_invasion( ch );
            }
           return;
           break;
   }

   return;
}

void start_invasion( CHAR_DATA *ch )
{
    DESCRIPTOR_DATA *d;
    int count;
    CHAR_DATA *mob;

    last_fun( "start_invasion");
    // if there are no protector mobs, who cares if someone walks in.
    if( !ch->in_room->area->defender ||
        !ch->in_room->area->defender_num ) {
       sprintf(lbuf, "Start_invasion: no defender mobs");
        wiznet( 0, WIZ_LSPAM, 0, lbuf );
      return;
    }

    // any town can only dispatch 5 batches of guards.
    if( ch->in_room->area->dispatched >= (ch->in_room->area->defender_num * 5 ) ) {
//    if( ch->in_room->area->dispatched >= (ch->in_room->area->defender_num *
//        ch->in_room->area->squads )) {
       sprintf(lbuf, "Start_invasion: dispatched all defender mobs, disregarding");
        wiznet( 0, WIZ_LSPAM, 0, lbuf );
//      return;
    }

    // warn of invasion
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( d->connected == CON_PLAYING
           && d->character->in_room->area == ch->in_room->area )
      {
         send_to_char( "&+RYou hear the guards sound the invasion alarm!\n\r", d->character );
      }
    }

    // create and dispatch defenders
    for( count = 0; count < ch->in_room->area->defender_num; count++ )
    {
       mob = create_mobile( get_mob_index( ch->in_room->area->defender ) );
       char_to_room( mob, get_room_index( ch->in_room->area->barracks ));
       if( !IS_SET( mob->act, ACT_MEMORY ))
         SET_BIT( mob->act, ACT_MEMORY );
       if( !IS_SET( mob->act, ACT_HUNTER ))
         SET_BIT( mob->act, ACT_HUNTER );
       if( IS_SET( mob->act, ACT_SENTINEL ))
         REMOVE_BIT( mob->act, ACT_SENTINEL );
       mob->spec_fun = spec_mob_lookup( "spec_justice_guard" );
       start_grudge( mob, ch, FALSE );
       ch->in_room->area->dispatched++;
    }
    ch->in_room->area->squads++;

    return;
}

// dispatches a guard to hunt down the player that has committed a crime and
// has the guard capture them.
void dispatch_guard( CHAR_DATA *ch )
{
    CHAR_DATA *mob;

    last_fun( "dispatch_guard");
    mob = create_mobile( get_mob_index( ch->in_room->area->defender ));
    char_to_room( mob, get_room_index( ch->in_room->area->barracks ));
    if( !IS_SET( mob->act, ACT_MEMORY ))
      SET_BIT( mob->act, ACT_MEMORY );
    if( !IS_SET( mob->act, ACT_HUNTER ))
    if( IS_SET( mob->act, ACT_SENTINEL ))
      REMOVE_BIT( mob->act, ACT_SENTINEL );
    SET_AFF_BIT( mob, AFF_JUSTICE_TRACKER );
    mob->spec_fun = spec_mob_lookup( "spec_justice_guard" );
    start_grudge( mob, ch, FALSE );

    return;  
}

char *get_invader_string( int justice ) {

    switch (justice) {
        
        case  JUSTICE_GOOD :
            return "Evil races despised."; break;
        case  JUSTICE_EVIL :
            return "Good races killed on sight."; break;
        case  JUSTICE_NEUTRAL:
            return "All races welcome."; break;
        case  JUSTICE_NEUT_CLOSED:
            return "Evil and good races hunted down."; break;      
        case  JUSTICE_LAWFUL_EVIL:
            return "Good races killed on sight."; break;
        case  JUSTICE_CHAOTIC_GOOD :
            return "Evil races killed on sight."; break;
        case  JUSTICE_CHAOTIC_NEUT:
            return "Evil and good races hunted down."; break;
        case  JUSTICE_CLOSED_GOOD :
            return "Evil and neutral races not allowed."; break;
        case  JUSTICE_CLOSED_EVIL :
            return "Good and neutral races prohibited."; break;
    } //end switch
    return NULL;
}

