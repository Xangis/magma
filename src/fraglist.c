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

/* All code in this file is original code by Veygoth unless otherwise noted */

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

FRAG_DATA fraglist;

// Code to check if someone just fragged.
// Will also have to add to race, class, and guild frag tables in
// addition to the master frag table.  This does not update any
// lists yet and instead only updates the totals. - Veygoth
void check_frag( CHAR_DATA *ch, CHAR_DATA *victim )
{
     char buf[MAX_STRING_LENGTH];

     last_fun( "check_frag");

     // NPC's don't participate in fragging, can't frag yourself,
     // have to be within 10 levels, no same side frags, no frags
     // from races not participating in racewars, have to be level
     // 20 to frag, and have to be a valid class.

     /* Check to see if kill qualifies for a frag */

     if( IS_NPC( ch ))
        return;
     if( IS_NPC( victim ))
        return;
     if( ch == victim )
        return;
     if( race_table[victim->race].racewar_side == RACEWAR_NONE )
        return;
     if( race_table[victim->race].racewar_side == race_table[ch->race].racewar_side )
         return;
     if( IS_IMMORTAL( ch ))
        return;
     if( IS_IMMORTAL( victim ))
        return;
     if( victim->level < 20 )
        return;
     if( ch->level < 20 )
        return;
     if( (ch->level - victim->level) > 10 || (victim->level - ch->level) > 10)
        return;
     if( ch->class == CLASS_NONE || victim->class == CLASS_NONE )
        return;
     if( victim->race > MAX_PC_RACE )
        return;
 
     /* Give frag to ch */
     ch->pcdata->frags++;

     // Protect against polymorphed character race frags
     if( ch->race < MAX_PC_RACE )
     {      
       fraglist.total_race_class_frags[ch->race][ch->class]++;
       fraglist.total_side_frags[race_table[ch->race].racewar_side]++;
     }

     if( ch->pcdata->clan )
         ch->pcdata->clan->frags++;

     /* Take frag from victim */
     victim->pcdata->frags--;

     // Protect against polymorphed character race frags
     if( victim->race < MAX_PC_RACE )
     {
       fraglist.total_race_class_frags[victim->race][victim->class]--;
       fraglist.total_side_frags[race_table[victim->race].racewar_side]--;
     }

     if( victim->pcdata->clan )
         victim->pcdata->clan->frags--;

     send_to_char( "&+WYou gain a frag!&n\n\r", ch );
     send_to_char( "&+WYou lose a frag!&n\n\r", victim );

     sprintf( buf, "%s has fragged %s in room %d", ch->name, victim->name, ch->in_room->vnum );
     wiznet( ch, WIZ_DEATHS, L_AVA, buf );
     log_string( buf );

     /* Check to see if either person goes up or down on their particular lists */

     if( ch->pcdata->frags > 0 )
     {
       resort_fraglist( ch, fraglist.top_frags );
       resort_fraglist( ch, fraglist.race_top_frags[ch->race] );
       resort_fraglist( ch, fraglist.class_top_frags[ch->class] );
     }
     else if( ch->pcdata->frags < 0 )
     {
       resort_fraglist( ch, fraglist.bottom_frags );
       resort_fraglist( ch, fraglist.race_bottom_frags[ch->race] );
       resort_fraglist( ch, fraglist.class_bottom_frags[ch->class] );
     }

     if( victim->pcdata->frags > 0 )
     {
       resort_fraglist( victim, fraglist.top_frags );
       resort_fraglist( victim, fraglist.race_top_frags[victim->race] );
       resort_fraglist( victim, fraglist.class_top_frags[victim->class] );
     }
     else if( victim->pcdata->frags < 0 )
     {
       resort_fraglist( victim, fraglist.bottom_frags );
       resort_fraglist( victim, fraglist.race_bottom_frags[victim->race] );
       resort_fraglist( victim, fraglist.class_bottom_frags[victim->class] );
     }
     
     save_fraglist();

     return;
}

void load_fraglist( )
{
     FILE *fp;
     char strsave[MAX_INPUT_LENGTH];
     int races;
     int classes;
     int race;
     int class;
     int rank;
     int sides;
     int stat;
     int count, count2;
     char *word;
     char *word2;
     int status;
     
     last_fun( "load_fraglist");

     // Initialize the data to blank
     for( count = 0; count < MAX_FRAG; count++ )
     {
       fraglist.top_frags[count].name = NULL;
       fraglist.top_frags[count].frags = 0;
       fraglist.bottom_frags[count].name = NULL;
       fraglist.bottom_frags[count].frags = 0;
       for( count2 = 0; count2 < MAX_PC_RACE; count2++ )
       {
          fraglist.race_top_frags[count2][count].name = NULL;
          fraglist.race_top_frags[count2][count].frags = 0;
          fraglist.race_bottom_frags[count2][count].name = NULL;
          fraglist.race_bottom_frags[count2][count].frags = 0;
       }
       for( count2 = 0; count2 < MAX_CLASS; count2++ )
       {
          fraglist.class_top_frags[count2][count].name = NULL;
          fraglist.class_top_frags[count2][count].frags = 0;
          fraglist.class_bottom_frags[count2][count].name = NULL;
          fraglist.class_bottom_frags[count2][count].frags = 0;
       }
     }

     sprintf( strsave, "%s%s%s", data_directory, SYSTEM_DIR, FRAG_FILE );
     if( !( fp = fopen( strsave, "r" ) ) )
     {
        bug( "Error opening frag file!  Fraglist left blank.", 0 );
        return;
     }

     log_string( "Loading fraglist" );

      for ( ; !feof (fp) ; )
      {

        word = fread_word( fp, &status );

        if ( !word )
        {
            bug( "fread_word:  Error reading key while loading fraglist.  EOF? (End not found)", 0 );
            fread_to_eol( fp );
            break;
        }

        if( !str_cmp( word, "End" ))
        {
           break;
        }
        if( !str_cmp( word, "Racewar" ) )
        {
           sides = fread_number( fp, &stat );
           for( count = 0; count < sides; count++ )
             fraglist.total_side_frags[count] = fread_number( fp, &stat );
        }
        else if( !str_cmp( word, "Raceclass" ) )
        {
           races = fread_number( fp, &stat );
           classes = fread_number( fp, &stat );
           for( count = 0; count < races; count++ )
             for( count2 = 0; count2 < classes; count2++ )
               fraglist.total_race_class_frags[count][count2] = fread_number( fp, &stat );
        }
        else if( !str_cmp( word, "Frag" ))
        {
           word2 = fread_word( fp, &status );

           if( !str_cmp( word2, "Top" ))
           {
             rank = fread_number( fp, &status );
             fraglist.top_frags[rank].name = fread_string( fp, &status );
             fraglist.top_frags[rank].frags = fread_number( fp, &status );
           }
           else if( !str_cmp( word2, "Bottom" ))
           {
             rank = fread_number( fp, &status );
             fraglist.bottom_frags[rank].name = fread_string( fp, &status );
             fraglist.bottom_frags[rank].frags = fread_number( fp, &status );
           }
           else if( !str_cmp( word2, "RaceTop" ))
           {
             race = fread_number( fp, &status );
             rank = fread_number( fp, &status );
             fraglist.race_top_frags[race][rank].name = fread_string( fp, &status );
             fraglist.race_top_frags[race][rank].frags = fread_number( fp, &status );
           }
           else if( !str_cmp( word2, "RaceBot" ))
           {
             race = fread_number( fp, &status );
             rank = fread_number( fp, &status );
             fraglist.race_bottom_frags[race][rank].name = fread_string( fp, &status );
             fraglist.race_bottom_frags[race][rank].frags = fread_number( fp, &status );
           }
           else if( !str_cmp( word2, "ClassTop" ))
           {
             class = fread_number( fp, &status );
             rank = fread_number( fp, &status );
             fraglist.class_top_frags[class][rank].name = fread_string( fp, &status );
             fraglist.class_top_frags[class][rank].frags = fread_number( fp, &status );
           }
           else if( !str_cmp( word2, "ClassBot" ))
           {
             class = fread_number( fp, &status );
             rank = fread_number( fp, &status );
             fraglist.class_bottom_frags[class][rank].name = fread_string( fp, &status );
             fraglist.class_bottom_frags[class][rank].frags = fread_number( fp, &status );
           }
        }
        else if( !str_cmp( word, "End" ))
        {
           break;
        }
     }

     fclose( fp );
     fpReserve = fopen( NULL_FILE, "r" );
     return;

}

void save_fraglist( )
{
     FILE *fp;
     char strsave[MAX_INPUT_LENGTH];
     int count, count2;

     last_fun( "save_fraglist");

     sprintf( strsave, "%s%s%s", data_directory, SYSTEM_DIR, FRAG_FILE );
     if( !( fp = fopen( strsave, "w" ) ) )
     {
        bug( "Error opening frag file!  Fraglist not written.", 0 );
        return;
     }

     // Print the current maxes so it doesen't bomb out if we change
     // the number of races/classes - Veygoth

     log_string( "Saving Racewar Sides..." );

     fprintf( fp, "Racewar" );
     for( count = 0; count < MAX_RACEWAR_SIDE; count++ )
     {
        fprintf( fp, " %d", fraglist.total_side_frags[count] );
     } 
     fprintf( fp, "\n" );


     log_string( "Saving RaceClass Totals..." );

     fprintf( fp, "Raceclass" );
     fprintf( fp, " %d", MAX_PC_RACE );
     fprintf( fp, " %d", MAX_CLASS );
     for( count = 0; count < MAX_PC_RACE; count++ )
     {
       for( count2 = 0; count2 < MAX_CLASS; count2++ )
       {
         fprintf( fp, " %d", fraglist.total_race_class_frags[count][count2] );
       }
     } 
     fprintf( fp, "\n" );


     log_string( "Saving Top..." );

     for( count = 0; count < MAX_FRAG; count++ )
     {
        if( fraglist.top_frags[count].name && fraglist.top_frags[count].name[0] != '\0' )
        {
          fprintf( fp, "Frag Top %d %s~ %d\n",
            count,
            fraglist.top_frags[count].name,
            fraglist.top_frags[count].frags );
        }
     }


     log_string( "Saving Bottom..." );

     for( count = 0; count < MAX_FRAG; count++ )
     {
        if( fraglist.bottom_frags[count].name && fraglist.bottom_frags[count].name[0] != '\0' )
        {
          fprintf( fp, "Frag Bottom %d %s~ %d\n",
            count,
            fraglist.bottom_frags[count].name,
            fraglist.bottom_frags[count].frags );
        }
     }


     log_string( "Saving ClassTop..." );

     for( count = 0; count < MAX_CLASS; count++ )
     {
       for( count2 = 0; count2 < MAX_FRAG; count2++ )
       {
         if( fraglist.class_top_frags[count][count2].name && fraglist.class_top_frags[count][count2].name[0] != '\0' )
         {
           fprintf( fp, "Frag ClassTop %d %d %s~ %d\n",
             count,
             count2,
             fraglist.class_top_frags[count][count2].name,
             fraglist.class_top_frags[count][count2].frags );
         }
       }
     }

     log_string( "Saving ClassBottom..." );

     for( count = 0; count < MAX_CLASS; count++ )
     {
       for( count2 = 0; count2 < MAX_FRAG; count2++ )
       {
         if( fraglist.class_bottom_frags[count][count2].name && fraglist.class_bottom_frags[count][count2].name[0] != '\0' )
         {
           fprintf( fp, "Frag ClassBot %d %d %s~ %d\n",
             count,
             count2,
             fraglist.class_bottom_frags[count][count2].name,
             fraglist.class_bottom_frags[count][count2].frags );
         }
       }
     }

     log_string( "Saving RaceTop..." );

     for( count = 0; count < MAX_PC_RACE; count++ )
     {
       for( count2 = 0; count2 < MAX_FRAG; count2++ )
       {
         if( fraglist.race_top_frags[count][count2].name && fraglist.race_top_frags[count][count2].name[0] != '\0' )
         {
           fprintf( fp, "Frag RaceTop %d %d %s~ %d\n",
             count,
             count2,
             fraglist.race_top_frags[count][count2].name,
             fraglist.race_top_frags[count][count2].frags );
         }
       }
     }

     log_string( "Saving RaceBottom..." );

     for( count = 0; count < MAX_PC_RACE; count++ )
     {
       for( count2 = 0; count2 < MAX_FRAG; count2++ )
       {
         if( fraglist.race_bottom_frags[count][count2].name && fraglist.race_bottom_frags[count][count2].name[0] != '\0' )
         {
           fprintf( fp, "Frag RaceBot %d %d %s~ %d\n",
             count,
             count2,
             fraglist.race_bottom_frags[count][count2].name,
             fraglist.race_bottom_frags[count][count2].frags );
         }
       }
     }


     fprintf( fp, "End\n" );
     fclose( fp );
     fpReserve = fopen( NULL_FILE, "r" );
     return;
}

void do_fraglist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int count, count2;
    int race, class;
    bool racelist = FALSE;
    bool classlist = FALSE;

    last_fun( "do_fraglist");

    if( argument[0] == '\0' )
    {
      sprintf( buf, "&+BTotal Good Side Frags: &+W%5d     &+BTotal Evil Side Frags: &+W%5d&n\n\r\n\r",
             fraglist.total_side_frags[RACEWAR_GOOD],
             fraglist.total_side_frags[RACEWAR_EVIL] );

      sprintf( buf2, "    Top Frags                     Bottom Frags\n\r" );
      strcat( buf, buf2 );
      for( count = 0; count < MAX_FRAG; count++ )
      {
        sprintf( buf2, "    &+y%-18s&n:&+Y %5d&n     &+y%-18s&n:&+Y %5d&n\n\r",
            fraglist.top_frags[count].name,
            fraglist.top_frags[count].frags,
            fraglist.bottom_frags[count].name,
            fraglist.bottom_frags[count].frags );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r" );
    }
    else if( !str_prefix( argument, "raceclass" ) || !str_prefix( argument, "classrace" ))
    {
      // Funky header formatting.  If any class has a name fewer than 4 characters the mud will crash -- Veygoth
      sprintf( buf, "            " );
      for( count = 0; count < MAX_CLASS; count++ )
      {
        sprintf( buf2, "  %c", class_table[count]->name[0] );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r            " );
      for( count = 0; count < MAX_CLASS; count++ )
      {
        sprintf( buf2, "  %c", class_table[count]->name[1] );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r            " );
      for( count = 0; count < MAX_CLASS; count++ )
      {
        sprintf( buf2, "  %c", class_table[count]->name[2] );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r            " );
      for( count = 0; count < MAX_CLASS; count++ )
      {
        sprintf( buf2, "  %c", class_table[count]->name[3] );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r\n\r" );

      // Actually display the info
      for( count = 0; count < MAX_PC_RACE; count++ )
      {
        sprintf( buf2, "%12s", race_table[count].name );
        strcat( buf, buf2 );
        for( count2 = 0; count2 < MAX_CLASS; count2++ )
        {
          sprintf( buf2, " %2d", fraglist.total_race_class_frags[count][count2] );
          strcat( buf, buf2 );
        }
        strcat( buf, "\n\r" );
      }
      strcat( buf, "\n\r" );
    }
    else if( !str_prefix( argument, "human" ))
    {
      race = RACE_HUMAN;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "barbarian" ))
    {
      race = RACE_BARBARIAN;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "grey elf" ))
    {
      race = RACE_GREYELF;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "rakshasa" ))
    {
      race = RACE_RAKSHASA;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "half-elf" ))
    {
      race = RACE_HALFELF;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "flind" ))
    {
      race = RACE_FLIND;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "drow" ))
    {
      race = RACE_DROW;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "dwarf" ))
    {
      race = RACE_DWARF;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "duergar" ))
    {
      race = RACE_DUERGAR;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "halfling" ))
    {
      race = RACE_HALFLING;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "troll" ))
    {
      race = RACE_TROLL;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "ogre" ))
    {
      race = RACE_OGRE;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "orc" ))
    {
      race = RACE_ORC;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "gnome" ))
    {
      race = RACE_GNOME;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "centaur" ))
    {
      race = RACE_CENTAUR;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "githyanki" ))
    {
      race = RACE_GITHYANKI;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "goblin" ))
    {
      race = RACE_GOBLIN;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "minotaur" ))
    {
      race = RACE_MINOTAUR;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "githzerai" ))
    {
      race = RACE_GITHZERAI;
      racelist = TRUE;
    }
    else if( !str_prefix( argument, "warrior" ))
    {
      class = CLASS_WARRIOR;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "sorcerer" ))
    {
      class = CLASS_SORCERER;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "psionicist" ))
    {
      class = CLASS_PSIONICIST;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "cleric" ))
    {
      class = CLASS_CLERIC;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "thief" ))
    {
      class = CLASS_THIEF;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "assassin" ))
    {
      class = CLASS_ASSASSIN;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "mercenary" ))
    {
      class = CLASS_MERCENARY;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "paladin" ))
    {
      class = CLASS_PALADIN;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "anti-paladin" ))
    {
      class = CLASS_ANTIPALADIN;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "ranger" ))
    {
      class = CLASS_RANGER;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "earth elementalist" ))
    {
      class = CLASS_ELEMENT_EARTH;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "air elementalist" ))
    {
      class = CLASS_ELEMENT_AIR;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "fire elementalist" ))
    {
      class = CLASS_ELEMENT_FIRE;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "water elementalist" ))
    {
      class = CLASS_ELEMENT_WATER;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "shaman" ))
    {
      class = CLASS_SHAMAN;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "druid" ))
    {
      class = CLASS_DRUID;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "bard" ))
    {
      class = CLASS_BARD;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "hunter" ))
    {
      class = CLASS_HUNTER;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "monk" ))
    {
      class = CLASS_MONK;
      classlist = TRUE;
    }
    else if( !str_prefix( argument, "mystic" ))
    {
      class = CLASS_MYSTIC;
      classlist = TRUE;
    }
    else
    {
      send_to_char( "That's not a valid extra fraglist.  Try again.\n\r", ch );
      return;
    }

    if( classlist )
    {
      sprintf( buf, "&+BTop frags for class %s&n\n\r\n\r", class_table[class]->who_name );

      sprintf( buf2, "    Top Frags                     Bottom Frags\n\r" );
      strcat( buf, buf2 );
      for( count = 0; count < MAX_FRAG; count++ )
      {
        sprintf( buf2, "    &+y%-18s&n:&+Y %5d&n     &+y%-18s&n:&+Y %5d&n\n\r",
            fraglist.class_top_frags[class][count].name,
            fraglist.class_top_frags[class][count].frags,
            fraglist.class_bottom_frags[class][count].name,
            fraglist.class_bottom_frags[class][count].frags );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r" );
    }
    else if( racelist )
    {
      sprintf( buf, "&+BTop frags for race %s&n\n\r\n\r", race_table[race].colorname );

      sprintf( buf2, "    Top Frags                     Bottom Frags\n\r" );
      strcat( buf, buf2 );
      for( count = 0; count < MAX_FRAG; count++ )
      {
        sprintf( buf2, "    &+y%-18s&n:&+Y %5d&n     &+y%-18s&n:&+Y %5d&n\n\r",
            fraglist.race_top_frags[race][count].name,
            fraglist.race_top_frags[race][count].frags,
            fraglist.race_bottom_frags[race][count].name,
            fraglist.race_bottom_frags[race][count].frags );
        strcat( buf, buf2 );
      }
      strcat( buf, "\n\r" );
    }

    send_to_char( buf, ch );
}

// There are top lists, race lists, and class lists.  By passing the actual
// list
void resort_fraglist( CHAR_DATA *ch, FRAGGER_DATA list[] )
{
     FRAGGER_DATA person;
     FRAGGER_DATA temp;
     int count;
     bool found = FALSE;

    last_fun( "resort_fraglist");

     if( IS_NPC( ch ))
     {
       bug( "Mobile in resort_fraglist, check frag code!", 0 );
       return;
     }

     for( count = 0; count < MAX_FRAG; count++ )
     {
       if( !str_cmp( list[count].name, ch->name ))
       {
         // remove them from the list if they're in it already
         free_string( list[count].name );
         list[count].frags = 0;
         found = TRUE;
       }
     }

     // Shift a blank entry to the bottom of the list
     if( found )
     {
       for( count = 0; count < MAX_FRAG; count++ )
       {
         if( list[count].name == NULL && count < (MAX_FRAG - 1) )
         {
            list[count].name = str_dup( list[count+1].name );
            list[count].frags = list[count+1].frags;
            free_string( list[count+1].name );
            list[count+1].frags = 0;
         }
       }
     }

     person.name = str_dup( ch->name );
     person.frags = ch->pcdata->frags;

     // This works fine provided a blank entry isn't found where the player
     // used to be.  If a blank entry is found it'll plug in the data and continue
     // without
     for( count = 0; count < MAX_FRAG; count++ )
     {
        // Use absolute value so we can handle the negative frags properly too
        if( (abs(list[count].frags) > abs( person.frags )) )
          continue;

        temp.name = str_dup( list[count].name );
        temp.frags = list[count].frags;

        free_string( list[count].name );
        list[count].name = str_dup( person.name );
        list[count].frags = person.frags;

        person.name = str_dup( temp.name );
        person.frags = temp.frags;
        free_string( temp.name );

     }
 
     // See if the ch or victim qualify to replace an entry in either list.
     // damn hard code to write.
}

