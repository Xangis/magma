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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined( macintosh )
extern  int     _filbuf	        args( (FILE *) );
#endif

#if !defined( ultrix ) && !defined( apollo )
#include <memory.h>
#endif



#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                  field = fread_string( fp, &stat); \
                    fMatch = TRUE;                  \
                    break;                          \
                }



/*
 * Globals.
 */

/* The social table.  New socials contributed by Katrina and Binky */
SOC_INDEX_DATA *	soc_index_hash	[ MAX_WORD_HASH ];

/* Class table.  See class files in the 'classes' directory */
struct  class_type *    class_table     [ MAX_CLASS ];

void add_social( SOC_INDEX_DATA *social );

/*
 * New code for loading classes from file.
 */
bool fread_class( char *filename )
{
    FILE               *fp;
    char               *word;
    struct  class_type *class;
    char                buf [ MAX_STRING_LENGTH ];
    bool                fMatch;
    int                 stat;
    int                 number = -1;

    sprintf( buf, "Loading class: %s", filename );
    log_string( buf );

    sprintf( buf, "%s%s", CLASS_DIR, filename );
    if ( !( fp = fopen( buf, "r" ) ) )
    {
        perror( buf );
        return FALSE;
    }

    class = alloc_mem ( sizeof( CLASS_TYPE ) );

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp, &stat );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
            KEY( "AtrPrm", class->attr_prime, fread_number( fp, &stat ) );
	    break;

	case 'C':
            KEY( "Cla", number, fread_number( fp, &stat ) );

	    if ( number < 0 || number >= MAX_CLASS )
	    {
		sprintf( buf, "Fread_class: bad class  '%s'.", filename );
		bug ( buf, 0 );
		return FALSE;
	    }
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		fclose( fp );
		class_table[number] = class;
		return TRUE;
	    }
	    break;

	case 'H':
            KEY( "Hpmin", class->hp_min, fread_number( fp, &stat ) );
            KEY( "Hpmax", class->hp_max, fread_number( fp, &stat ) );
	    break;

	case 'M':
            KEY( "Mana", class->fMana, fread_number( fp, &stat ) );
	    break;

	case 'N':
            SKEY( "Nm", class->name );
	    break;

	case 'S':
            KEY( "SkllAdpt", class->skill_adept, fread_number( fp, &stat ) );

	    if ( !str_cmp( word, "Skll" ) )
	    {
		int sn;
		int value;
                char *word;

		value = fread_number( fp, &stat );
		sn    = skill_lookup( (word = fread_word( fp, &stat )) );
		if ( sn < 0 )
                {
                    sprintf( buf, "Fread_class - unknown skill: %s", word );
                    bug( buf, 0 );
                }
		else
		    skills_table[sn].skill_level[number] = value;
		fMatch = TRUE;
	    }
	    else if ( !str_cmp( word, "Spll" ) )
	    {
		int sn;
		int value;
                char *word;

		value = fread_number( fp, &stat );
		sn    = spell_lookup( (word = fread_word( fp, &stat )) );
		if ( sn < 0 )
                {
                    sprintf( buf, "Fread_class - unknown spell: %s", word );
                    bug( buf, 0 );
                }
		else
                {
                    if( value <= MAX_CIRCLE && value > 0 )
                    {
		      spells_table[sn].spell_circle[number] = value;
                    }
                    else
                    {
                      bug( "Spell in class file has level greater than MAX_CIRCLE", 0 );
                      spells_table[sn].spell_circle[number] = MAX_CIRCLE+2;
                    }
                }
		fMatch = TRUE;
	    }
	    else if ( !str_cmp( word, "Song" ) )
	    {
		int sn;
		int value;
                char *word;

		value = fread_number( fp, &stat );
		sn    = song_lookup( (word = fread_word( fp, &stat )) );
		if ( sn < 0 )
                {
                    sprintf( buf, "Fread_class - unknown song: %s", word );
                    bug( buf, 0 );
                }
		else
                {
                    if( value < MAX_LEVEL )
                    {
		      songs_table[sn].spell_circle[number] = value;
                    }
                    else
                    {
                      bug( "Song in class file has level greater than MAX_LEVEL", 0 );
                      songs_table[sn].spell_circle[number] = MAX_LEVEL;
                    }
                }
		fMatch = TRUE;
	    }

	    break;

	case 'T':
	    KEY( "Thac0", class->thac0_00, fread_number( fp, &stat ) );
	    KEY( "Thac47", class->thac0_47, fread_number( fp, &stat ) );
	    break;

	case 'W':
	    SKEY( "WhoNm", class->who_name );
	    KEY( "Wpn", class->weapon, fread_number( fp, &stat ) );
	    break;
	}

	if ( !fMatch )
	{
            bugf( "load_class_file: no match: %s", word );
	}
    }

    return FALSE;
}

/*
 * Load in all the class files.
 */ 
void load_classes( void )
{
    FILE *fpList;
    char *filename;
    char  fname     [ MAX_STRING_LENGTH ];
    char  classlist [ MAX_STRING_LENGTH ];
    int   stat;
    
    sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
    if ( !( fpList = fopen( classlist, "r" ) ) )
    {
        perror( classlist );
        exit( 1 );
    }

    for ( ; ; )
    {
        filename = feof( fpList ) ? "$" : fread_word( fpList, &stat );
	strcpy( fname, filename );
        if ( fname[0] == '$' )
          break;

        if ( !fread_class( fname ) )
        {
          bugf( "Cannot load class file: %s", fname );
        }
    }
    fclose( fpList );

    return;
}

void fread_social( FILE *fp )
{
    char           *word;
    SOC_INDEX_DATA *social;
    char            buf [ MAX_STRING_LENGTH ];
    bool            fMatch;
    int             stat;

    social = new_social( );

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp, &stat );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    SKEY( "CharNoArg", social->char_no_arg );
	    SKEY( "CharFound", social->char_found  );
	    SKEY( "CharAuto",  social->char_auto   );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    SKEY( "Name", social->name );
	    break;

	case 'O':
	    SKEY( "OthersNoArg", social->others_no_arg );
	    SKEY( "OthersFound", social->others_found  );
	    SKEY( "OthersAuto",	 social->others_auto   );
	    break;

	case 'V':
	    SKEY( "VictFound", social->vict_found );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }

    return;
}

void load_socials( void )
{
    FILE *fp;
    int   stat;
    char  strsave [ MAX_INPUT_LENGTH ];

    sprintf( strsave, "%s%s", SYSTEM_DIR, SOCIAL_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
    {
	bugf( "Cannot open SOCIALS.TXT from %s.", strsave );
	exit( 0 );
    }

    for ( ; ; )
    {
	char letter;
	char *word;

	letter = fread_letter( fp );

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_socials: # not found.", 0 );
	    break;
	}

	word = fread_word( fp, &stat );
	if ( !str_cmp( word, "SOCIAL" ) )
	{
	    fread_social( fp );
	    continue;
	}
	else
	if ( !str_cmp( word, "END" ) )
	    break;
	else
	{
	    bug( "Load_socials: bad section.", 0 );
	    continue;
	}
    }
    
    fclose( fp );
    return;
}

// Veygoth - sets a players skills to minimum values
// if they gain a new skill or if they gain a level.
// skills start out at an initial value of 25 and increase
// by a minimum of one point per level.
// Does not lower skills already above the minimum.
//
// This should only be called upon a character levelling
//
void set_skills( CHAR_DATA *ch )
{
    int sn;

    if( IS_NPC( ch ))
      return;

    for( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if( ch->level >= skills_table[sn].skill_level[ch->class] )
        {
          if( ch->pcdata->skl_lrn[sn] <= 24 + ch->level )
              ch->pcdata->skl_lrn[sn] = 24 + ch->level ;
          else if( ch->pcdata->skl_lrn[sn] < MAX_SKILL_ADEPT )
              ch->pcdata->skl_lrn[sn] += 1;
        }
    }

    return;
}

void skill_practice( CHAR_DATA *ch, int sn )
{
    char buf[MAX_STRING_LENGTH];

    if( IS_NPC( ch ))
        return;

    // Have to be below the max and below 95% and make a successful
    // skill check and be able to have the skill.
    if( ch->pcdata->skl_lrn[sn] < ( 26 +
        ((ch->level - skills_table[sn].skill_level[ch->class] ) * 2 ))
        && ch->level >= skills_table[sn].skill_level[ch->class]
        && ch->pcdata->skl_lrn[sn] < MAX_SKILL_ADEPT
        && number_range(1, 1000) <= ( 5 + (get_curr_int( ch ) / 10)) + ch->level )
     {
          ch->pcdata->skl_lrn[sn]++;
          sprintf( buf, "&+cYe feel yer skill in %s improving.&n\n\r",
            skills_table[sn].name );
          send_to_char( buf, ch ); 
    }

     return;
}

// Initialize newbie spells to base values - Veygoth
void set_spells( CHAR_DATA *ch )
{
    int sn;

    if( IS_NPC( ch ))
      return;

    for( sn = 0; sn < MAX_SPELL; sn++ )
    {
        if( spells_table[sn].spell_circle[ch->class] == 1
           && ch->pcdata->spl_lrn[sn] < (BASE_SPELL_ADEPT + 5))
              ch->pcdata->spl_lrn[sn] = (BASE_SPELL_ADEPT + 5);
    }

    return;
}

void spell_practice( CHAR_DATA *ch, int sn )
{
    char buf[MAX_STRING_LENGTH];

    if( IS_NPC( ch ))
        return;

    // Have to be below the max and below 95% and make a successful
    // spell check and be able to have the spell.
    if( ch->pcdata->spl_lrn[sn] < MAX_SPELL_ADEPT
        && number_range(1, 1000) <= ( 50 + get_curr_int( ch )) )
     {
          ch->pcdata->spl_lrn[sn]++;
          sprintf( buf, "&+cYe feel yer knowledge of %s improving.&n\n\r",
            spells_table[sn].name );
          send_to_char( buf, ch ); 
    }

     return;
}

void lang_practice( CHAR_DATA *ch, int lang )
{
    char buf[MAX_STRING_LENGTH];

    if( IS_NPC( ch ))
        return;

    // Have to be below 100 and above 0 and make a successful
    // lang check and be able to learn the lang.
    if( ch->pcdata->lang_lrn[lang] < 100 && ch->pcdata->lang_lrn[lang] > 0
        && number_range(1, 1000) <= ( 50 + get_curr_int( ch )) )
    {
          ch->pcdata->lang_lrn[lang]++;
          sprintf( buf, "&+cYe feel yer knowledge of %s improving.&n\n\r",
            lang_table[lang] );
          if( ch->pcdata->lang_lrn[lang] >= 20 && IS_IMMORTAL( ch ) )
              send_to_char( buf, ch ); 
    }

     return;
}

char * size_string( int size )
{
    switch( size )
    {
      default:
        return "unknown-invalid";
        break;
      case SIZE_ANY:
        return "unknown";
        break;
      case SIZE_TINY:
        return "tiny";
        break;
      case SIZE_SMALL:
        return "small";
        break;
      case SIZE_PETITE:
        return "small-medium";
        break;
      case SIZE_BIG:
        return "medium-large";
        break;
      case SIZE_MEDIUM:
        return "medium";
        break;
      case SIZE_LARGE:
        return "large";
        break;
      case SIZE_HUGE:
        return "huge";
        break;
      case SIZE_GIANT:
        return "giant";
        break;
      case SIZE_TITANIC:
        return "titanic";
        break;
      case SIZE_GARGANTUAN:
        return "gargantuan";
        break;
      case SIZE_NONE:
        return "none";
        break;
     }
}

char * position_string( int position )
{
     switch( position )
     {
       default:
         return "unknown";
         break;
       case POS_STANDING:
         return "standing";
         break;
       case POS_FIGHTING:
         return "standing";
//         return "fighting";
         break;
       case POS_KNEELING:
         return "kneeling";
         break;
       case POS_SITTING:
         return "sitting";
         break;
       case POS_RESTING:
         return "resting";
         break;
       case POS_RECLINING:
         return "reclining";
         break;
       case POS_SLEEPING:
         return "sleeping";
         break;
       case POS_STUNNED:
         return "lying stunned";
         break;
       case POS_UNCONSCIOUS:
         return "lying unconscious";
         break;
       case POS_INCAP:
         return "incapacitated";
         break;
       case POS_MORTAL:
         return "mortally wounded";
         break;
       case POS_DEAD:
         return "dead";
         break;
     }
}

char * align_string( CHAR_DATA *ch )
{
    const static char *numbers = "0123456789";
          static char  buf [ MAX_STRING_LENGTH ];
    int alignment = ch->alignment;

    if ( IS_IMMORTAL( ch ) )
    {
        if ( alignment < 0 )
        {
            buf[0] = '-';
            alignment *= -1;
        }
        else
            buf[0] = '+';
        alignment %= 10000;
        buf[1] = numbers[alignment/1000];
        alignment %= 1000;
        buf[2] = numbers[alignment/100];
        alignment %= 100;
        buf[3] = numbers[alignment/10];
        alignment %= 10;
        buf[4] = numbers[alignment];
        buf[5] = '\0';
        return buf;
    }

         if( alignment >  1000 ) return "buggy - too high";
    else if( alignment == 1000 ) return "pure and holy";
    else if( alignment >   900 ) return "extremely good";
    else if( alignment >   600 ) return "very good";
    else if( alignment >   350 ) return "good";
    else if( alignment >   100 ) return "neutral leaning towards good";
    else if( alignment >  -100 ) return "neutral";
    else if( alignment >  -350 ) return "neutral leaning towards evil";
    else if( alignment >  -600 ) return "evil";
    else if( alignment >  -900 ) return "very evil";
    else if( alignment > -1000 ) return "extremely evil";
    else if( alignment == -1000 ) return "pure evil";
    else return "buggy - too low";
}

/*
 * Take a social data from the free list and clean it out.
 */
SOC_INDEX_DATA *new_social( void )
{
    SOC_INDEX_DATA *soc;

    soc         = alloc_perm( sizeof( SOC_INDEX_DATA ) );
    top_social++;

    clear_social( soc );

    return soc;
}

/*
 * Add a social to the social index table                       - Thoric
 * Hashed and insert sorted.
 */
void add_social( SOC_INDEX_DATA *social )
{
    SOC_INDEX_DATA *tmp;
    SOC_INDEX_DATA *prev;
    int             hash;
    int             x;

    if ( !social )
    {
        bug( "Add_social: NULL social", 0 );
        return;
    }

    if ( !social->name )
    {
        bug( "Add_social: NULL social->name", 0 );
        return;
    }

    if ( !social->char_no_arg )
    {
        bug( "Add_social: NULL social->char_no_arg", 0 );
        return;
    }

    /* make sure the name is all lowercase */
    for ( x = 0; social->name[x] != '\0'; x++ )
        social->name[x] = LOWER( social->name[x] );

    if ( social->name[0] < 'a' || social->name[0] > 'z' )
        hash = 0;
    else
        hash = ( social->name[0] - 'a' ) + 1;

    if ( !( prev = tmp = soc_index_hash[hash] ) )
    {
        social->next = soc_index_hash[hash];
        soc_index_hash[hash] = social;
        return;
    }

    for ( ; tmp; tmp = tmp->next )
    {
        if ( !( x = strcmp( social->name, tmp->name ) ) )
        {
            bug( "Add_social: trying to add duplicate name to bucket %d", hash);
            free_social( social );
            return;
        }
        else
        if ( x < 0 )
        {
            if ( tmp == soc_index_hash[hash] )
            {
                social->next = soc_index_hash[hash];
                soc_index_hash[hash] = social;
                return;
            }
            prev->next = social;
            social->next = tmp;
            return;
        }
        prev = tmp;
    }

    /* add to end */
    prev->next = social;
    social->next = NULL;
    return;
}

/*
 * Free a social structure
 */
void free_social( SOC_INDEX_DATA *social )
{
    free_string( social->name          );
    free_string( social->char_no_arg   );
    free_string( social->others_no_arg );
    free_string( social->char_found    );
    free_string( social->others_found  );
    free_string( social->vict_found    );
    free_string( social->char_auto     );
    free_string( social->others_auto   );

    free_mem( social, sizeof( SOC_INDEX_DATA ) );
    return;
}

/*
 * Clear a new social.
 */
void clear_social( SOC_INDEX_DATA *soc )
{
    static SOC_INDEX_DATA soc_zero;

    *soc                        = soc_zero;
    soc->name                   = NULL;
    soc->char_no_arg            = NULL;
    soc->others_no_arg          = NULL;
    soc->char_found             = NULL;
    soc->others_found           = NULL;
    soc->vict_found             = NULL;
    soc->char_auto              = NULL;
    soc->others_auto            = NULL;
    return;
}

char * get_tradition_name( int tradition )
{
   if( tradition == TRADITION_SCORPION )
     return "Striking Scorpion Tradition";

   return "none";
}

char * stance_name( int stance )
{
   if( stance == STANCE_NONE )
     return "none";
   else if( stance == STANCE_BEAR )
     return "bear";
   else if( stance == STANCE_CAT )
     return "cat";
   else if( stance == STANCE_COBRA )
     return "cobra";
   else if( stance == STANCE_CRANE )
     return "crane";
   else if( stance == STANCE_DRAGON )
     return "dragon";
   else if( stance == STANCE_DRAGONFLY )
     return "dragonfly";
   else if( stance == STANCE_HAWK )
     return "hawk";
   else if( stance == STANCE_LEOPARD )
     return "leopard";
   else if( stance == STANCE_MANTIS )
     return "mantis";
   else if( stance == STANCE_MONKEY )
     return "monkey";
   else if( stance == STANCE_SNAKE )
     return "snake";
   else if( stance == STANCE_TIGER )
     return "tiger";

   return "none";
}
