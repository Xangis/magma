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

#if defined( sun )
#include <memory.h>
#endif

#if !defined( macintosh )
extern	int	_filbuf		args( (FILE *) );
#endif

#if defined( sun )
int     system          args( ( const char *string ) );
#endif


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[ MAX_NEST ];

char    lbuf[MAX_STRING_LENGTH];

int stat;

/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( OBJ_DATA  *obj,
			       FILE *fp, int iNest ) );
// Just different enough to need a different function.  Annoying - Veygoth
void	fwrite_corpse	args( ( OBJ_DATA  *obj,
			       FILE *fp, int iNest ) );
int	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
int	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );
int	fread_corpse	args( ( FILE *fp ) );


/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint [ MAX_STRING_LENGTH ];

    strint[0] = LOWER( str[ 0 ] );
    return strint;

}

/*
 * Backups a character and inventory.
 * Courtesy of Zen :)
 */
void backup_char_obj( CHAR_DATA *ch )
{
    FILE *fp;
    char  buf     [ MAX_STRING_LENGTH ];
    char  strsave [ MAX_INPUT_LENGTH  ];

    last_fun( "backup_char_obj");

    if ( IS_NPC( ch ) || ch->level < 1 )
	return;

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s", BACKUP_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );
#else
    sprintf( strsave, "%s%s", BACKUP_DIR, capitalize( ch->name ) );
#endif
    if ( !( fp = fopen( strsave, "w" ) ) )
    {
        sprintf( buf, "Backup_char_obj: fopen %s: ", ch->name );
	bug( buf, 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying )
	    fwrite_obj( ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Delete a character's file.
 * Used for retire & delete commands for now.
 * Courtesy of Zen :)
 */
void delete_char_obj( CHAR_DATA *ch )
{
    char  buf     [ MAX_STRING_LENGTH ];
    char  strsave [ MAX_INPUT_LENGTH  ];

    last_fun( "delete_char_obj");
    if ( IS_NPC( ch ) || ch->level < 1 )
	return;

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
    if ( remove( strsave ) )
    {
        sprintf( buf, "Delete_char_obj: remove %s: ", ch->name );
	bug( buf, 0 );
	perror( strsave );
    }
    return;
}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    FILE *fp;
    char  buf     [ MAX_STRING_LENGTH ];
    char  strsave [ MAX_INPUT_LENGTH  ];

    last_fun( "save_char_obj");

    if ( IS_NPC( ch ) || ch->level < 1 )
	return;

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );

    sprintf( buf, "Saving character: %s", ch->name );
    log_string( buf );

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
    if ( !( fp = fopen( strsave, "w" ) ) )
    {
        sprintf( buf, "Save_char_obj: fopen %s: ", ch->name );
	bug( buf, 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying )
	    fwrite_obj( ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    int          sn;
    int count;
    MEM_DATA    *mem;
    bool         flag;

    last_fun( "fwrite_char");
    if( IS_NPC( ch ))
       return;

    fprintf( fp, "#%s\n", IS_NPC( ch ) ? "MOB" : "PLAYER"		);

    fprintf( fp, "Nm          %s~\n",	ch->name			);
    fprintf( fp, "ShtDsc      %s~\n",	ch->short_descr			);
    fprintf( fp, "LngDsc      %s~\n",	ch->long_descr			);
    fprintf( fp, "Dscr        %s~\n",	fix_string( ch->description )	);
    fprintf( fp, "Prmpt       %s~\n",	ch->pcdata->prompt		);
    fprintf( fp, "Sx          %d\n",	ch->sex				);
    fprintf( fp, "Cla         %d\n",	ch->class			);

    fprintf( fp, "Race        %s~\n",	race_table[ ch->race ].name 	);
    fprintf( fp, "Size        %d\n",	ch->size			); // Govno

    fprintf( fp, "Lvl         %d\n",	ch->level			);
    fprintf( fp, "Trst        %d\n",	ch->trust			);
    fprintf( fp, "Orighome    %d\n",    ch->pcdata->orighome            );
    fprintf( fp, "Currhome    %d\n",    ch->pcdata->currhome            );
    fprintf( fp, "Lastrent    %d\n",    ch->pcdata->lastrent            );
    fprintf( fp, "NewHp       %d\n",    ch->pcdata->new_hitpoints       );
    fprintf( fp, "LostHP      %d\n",    ch->pcdata->lost_hp             );
    fprintf( fp, "Playd       %d\n",
	ch->played + (int) ( current_time - ch->logon )			);
    fprintf( fp, "Created     %d\n",    ch->pcdata->created             );
    fprintf( fp, "Note        %ld\n",   (unlong) ch->last_note		);
    fprintf( fp, "Saved       %ld\n",   (unlong) ch->save_time		); // Govno
    fprintf( fp, "Room        %d\n",
	    (  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	     && ch->was_in_room )
	    ? ch->was_in_room->vnum
	    : ( ch->in_room && ch->in_room->vnum )
              ? ch->in_room->vnum 
              : ( ch->pcdata->lastrent )
                ? ch->pcdata->lastrent : ROOM_VNUM_LIMBO );

    fprintf( fp, "HitMnMv      %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Copper      %d\n",	ch->money.copper	);
    fprintf( fp, "Silver      %d\n",	ch->money.silver	);
    fprintf( fp, "Gold        %d\n",	ch->money.gold		);
    fprintf( fp, "Platinum    %d\n",	ch->money.platinum	);
    fprintf( fp, "BCopper     %d\n",	ch->pcdata->bank.copper	);
    fprintf( fp, "BSilver     %d\n",	ch->pcdata->bank.silver	);
    fprintf( fp, "BGold       %d\n",	ch->pcdata->bank.gold	);
    fprintf( fp, "BPlatinum   %d\n",	ch->pcdata->bank.platinum );
    fprintf( fp, "Experience  %d\n",	ch->exp			);
    fprintf( fp, "Train       %d\n",    ch->pcdata->train       );
    flag = FALSE;
    if( IS_SET( ch->act, PLR_MEMORIZING ))
    {
       flag = TRUE;
       REMOVE_BIT( ch->act, PLR_MEMORIZING );
    }
    fprintf( fp, "Act         %d\n",    ch->act			);
    if( flag )
       SET_BIT( ch->act, PLR_MEMORIZING );

    flag = FALSE; /* Don't save the camping bit .. Govno */
    if ( IS_SET( ch->act, PLR_CAMPING ))
    {
        flag = TRUE;
	REMOVE_BIT( ch->act, PLR_CAMPING );
    }
    if( flag )
        SET_BIT( ch->act, PLR_CAMPING );
  
    fprintf( fp, "AffdBy      %d",	NUM_AFFECT_VECTORS      );
    for( count = 0; count < NUM_AFFECT_VECTORS; count++)
    {
       fprintf( fp, " %10d", ch->affected_by[count] );
    }
    fprintf( fp, "\n" );

    fprintf( fp, "Firstaid    %d\n",    ch->pcdata->firstaid    );

    if ( ch->resistant > 0 )
	fprintf( fp, "Res         %d\n",	ch->resistant		);
    if ( ch->immune > 0)
	fprintf( fp, "Imm         %d\n",	ch->immune			);
    if ( ch->susceptible > 0 )
	fprintf( fp, "Susc        %d\n",	ch->susceptible		);
    /* Bug fix from Alander */
    fprintf( fp, "Pos         %d\n",
	    ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

    fprintf( fp, "SavThr      %d %d %d %d %d\n", ch->saving_throw[0],
             ch->saving_throw[1], ch->saving_throw[2],
             ch->saving_throw[3], ch->saving_throw[4]);
    fprintf( fp, "Align       %d\n",	ch->alignment		);
    fprintf( fp, "Hit         %d\n",	ch->hitroll		);
    fprintf( fp, "Dam         %d\n",	ch->damroll		);
    fprintf( fp, "Armr        %d\n",	ch->armor		);
    fprintf( fp, "Wimp        %d\n",	ch->wimpy		);
    fprintf( fp, "Spkng       %d\n",    ch->pcdata->speaking    );
    fprintf( fp, "Height      %d\n",    ch->pcdata->height      );
    fprintf( fp, "Weight      %d\n",    ch->pcdata->weight      );
    fprintf( fp, "Deaf        %d\n",	ch->deaf		);

    if ( IS_NPC( ch ) )
    {
	fprintf( fp, "Vnum        %d\n",	ch->pIndexData->vnum	);
    }
    else
    {
	fprintf( fp, "Paswd       %s~\n",	ch->pcdata->pwd		);
	fprintf( fp, "Bmfin       %s~\n",	ch->pcdata->bamfin	);
	fprintf( fp, "Bmfout      %s~\n",	ch->pcdata->bamfout	);
	fprintf( fp, "Immskll     %s~\n",	ch->pcdata->immskll	);
	fprintf( fp, "Wiznet      %d\n",	ch->pcdata->wiznet	);
	fprintf( fp, "Ttle        %s~\n",	ch->pcdata->title	);
	fprintf( fp, "AtrPrm      %d %d %d %d %d\n",
		ch->perm_str,
		ch->perm_int,
		ch->perm_wis,
		ch->perm_dex,
		ch->perm_con );
        fprintf( fp, "ExtAtr      %d %d %d %d\n",
                ch->perm_agi,
                ch->perm_cha,
                ch->perm_pow,
                ch->perm_luk );
	fprintf( fp, "AtrMd       %d %d %d %d %d\n",
		ch->mod_str, 
		ch->mod_int, 
		ch->mod_wis,
		ch->mod_dex, 
		ch->mod_con );
        fprintf( fp, "ExtMd       %d %d %d %d\n",
                ch->mod_agi,
                ch->mod_cha,
                ch->mod_pow,
                ch->mod_luk );
	fprintf( fp, "MaxMod      %d %d %d %d %d %d %d %d %d\n",
		ch->pcdata->maxmod_str, 
		ch->pcdata->maxmod_int, 
		ch->pcdata->maxmod_wis,
		ch->pcdata->maxmod_dex, 
		ch->pcdata->maxmod_con,
                ch->pcdata->maxmod_agi,
                ch->pcdata->maxmod_cha,
                ch->pcdata->maxmod_pow,
                ch->pcdata->maxmod_luk );
	fprintf( fp, "RaceMod     %d %d %d %d %d %d %d %d %d\n",
		ch->pcdata->racemod_str, 
		ch->pcdata->racemod_int, 
		ch->pcdata->racemod_wis,
		ch->pcdata->racemod_dex, 
		ch->pcdata->racemod_con,
                ch->pcdata->racemod_agi,
                ch->pcdata->racemod_cha,
                ch->pcdata->racemod_pow,
                ch->pcdata->racemod_luk );

	fprintf( fp, "Cond        %d %d %d\n",
		ch->pcdata->condition[0],
		ch->pcdata->condition[1],
		ch->pcdata->condition[2] );

	fprintf( fp, "Security    %d\n",   ch->pcdata->security		);

	if ( is_clan( ch ) )
	    fprintf( fp, "PClan       %2d %s~\n",
		    ch->pcdata->rank,
		    ch->pcdata->clan->name );

	if ( ch->pcdata->pkills )
	    fprintf( fp, "PKills      %d\n", ch->pcdata->pkills	);
	if ( ch->pcdata->pdeaths )
	    fprintf( fp, "PDeaths     %d\n", ch->pcdata->pdeaths	);
	if ( ch->pcdata->illegal_pk )
	    fprintf( fp, "IllegalPK   %d\n", ch->pcdata->illegal_pk	);

        fprintf( fp, "Frags       %d\n",   ch->pcdata->frags            );
        fprintf( fp, "ModHit      %d\n",   ch->pcdata->mod_hit          );
	fprintf( fp, "MKills      %d\n",   ch->pcdata->mkills		);
	fprintf( fp, "MDeaths     %d\n",   ch->pcdata->mdeaths		);

	fprintf( fp, "Pglen       %d\n",   ch->pcdata->pagelen		);
        fprintf( fp, "AggrLvl     %d\n",   ch->pcdata->aggr_level       );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skills_table[sn].name && ch->pcdata->skl_lrn[sn] > 0 )
	    {
		fprintf( fp, "Skll        %d '%s'\n",
		    ch->pcdata->skl_lrn[sn], skills_table[sn].name );
	    }
	}

	for ( sn = 0; sn < MAX_SPELL; sn++ )
	{
	    if ( spells_table[sn].name && ch->pcdata->spl_lrn[sn] > 0 )
	    {
		fprintf( fp, "Spll        %d '%s'\n",
		    ch->pcdata->spl_lrn[sn], spells_table[sn].name );
	    }
	}

	for ( sn = 0; sn < MAX_SONG; sn++ )
	{
	    if ( songs_table[sn].name && ch->pcdata->sng_lrn[sn] > 0 )
	    {
		fprintf( fp, "Song        %d '%s'\n",
		    ch->pcdata->sng_lrn[sn], songs_table[sn].name );
	    }
	}

	for ( sn = 0; sn < MAX_MONK_SKILL; sn++ )
	{
	    if ( monk_skills[sn].skill_name && ch->pcdata->monk_skill[sn] > 0 )
	    {
		fprintf( fp, "MSkl        %d '%s'\n",
		    ch->pcdata->monk_skill[sn], monk_skills[sn].skill_name );
	    }
	}

	for ( sn = 0; sn < MAX_LANG; sn++ )
	{
	    if ( ch->pcdata->lang_lrn[sn] > 0 )
	    {
		fprintf( fp, "Lang        %d %d\n",
		    sn, ch->pcdata->lang_lrn[sn] );
	    }
	}

        for ( count = 0; count < MAX_LEVEL; count++ )
        {
           fprintf( fp, "Trophy      %d %d\n",
                    ch->pcdata->trophy[count].vnum,
                    ch->pcdata->trophy[count].number );
        }

        for( mem = ch->pcdata->memorized; mem; mem = mem->next )
        {
           fprintf( fp, "Memdata     '%s' %d\n",
               spells_table[mem->sn].name, mem->memmed );
        }

    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
        if ( paf->skill >= MAX_SKILL || paf->spell >= MAX_SPELL || paf->song >= MAX_SONG 
             || paf->skill < 0 || paf->spell < 0 || paf->song < 0 ) {
        sprintf( lbuf, "Fwrite_char: error writing affect (%d,%d,%d) for %s.",
            paf->skill,  paf->spell, paf->song, ch->name);
        bug( lbuf, 0 );
        }
        else {
	fprintf( fp, "Afft       %18s~ %18s~ %18s~ %3d %3d %3d %3d",
		skills_table[ paf->skill ].name,
		spells_table[ paf->spell ].name,
		songs_table[ paf->song ].name,
		paf->duration,
		paf->modifier,
		paf->location,
                NUM_AFFECT_VECTORS );
        for( count = 0; count < NUM_AFFECT_VECTORS; count++)
        {
           fprintf( fp, " %10d", paf->bitvector[count] );
        }
        fprintf( fp, "\n" );
        } //end else
    }

    fprintf( fp, "End\n\n" );
    return;
}

void save_corpses( void )
{
     OBJ_DATA *obj;
     char       strsave [ MAX_INPUT_LENGTH ];
     FILE *fp;

     last_fun( "save_corpses");

     sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, CORPSE_FILE );
     if ( !( fp = fopen( strsave, "w" ) ) )
     {
        bug( "Error opening corpse file for output!", 0 );
        return;
     }

     // This may go haywire if a corpse is inside another object - Veygoth
     for( obj = object_list; obj; obj = obj->next )
     {
        if(  obj->item_type == TYPE_CORPSE_PC 
          && obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC
          && obj->in_room )
        {
          fwrite_corpse( obj, fp, 0 ); 
        }
     }

     fprintf( fp, "#END\n" );
     fclose( fp );

     return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj( OBJ_DATA *obj, FILE *fp, int iNest )
{
    AFFECT_DATA      *paf;
    EXTRA_DESCR_DATA *ed;
    int count;

    last_fun( "fwrite_obj");
    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content )
    {
	fwrite_obj( obj->next_content, fp, iNest );
    }

    /*
     * Castrate storage characters.
     */
    if ( obj->item_type == TYPE_KEY
	|| obj->deleted )
	return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",	iNest			     );
    fprintf( fp, "Name         %s~\n",	obj->name		     );
    fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr	     );
    fprintf( fp, "Description  %s~\n",	obj->description	     );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
    fprintf( fp, "Material     %d\n",     
        ( obj->material <= 0 ? ( obj->pIndexData ?
        (obj->pIndexData->material > 0 ? obj->pIndexData->material : 0 ) : 0 )
        : obj->material )                                            );

    if ( obj->spec_fun )
      fprintf( fp, "Special      %s\n",	spec_obj_string( obj->spec_fun ) );

    fprintf( fp, "ExtraFlags   %d\n",	obj->extra_flags[0]	     );
    fprintf( fp, "ExtraFlags2  %d\n",	obj->extra_flags[1]	     );
    fprintf( fp, "WearFlags    %d\n",	obj->wear_flags		     );
    fprintf( fp, "WearLoc      %d\n",	obj->wear_loc		     );
    fprintf( fp, "AntiFlags    %d\n",   obj->anti_flags              );
    fprintf( fp, "AntiFlags2   %d\n",   obj->anti_flags2             );
    fprintf( fp, "ItemType     %d\n",	obj->item_type		     );
    fprintf( fp, "Weight       %d\n",	obj->weight		     );
    fprintf( fp, "Level        %d\n",	obj->level		     );
    fprintf( fp, "Timer        %d\n",	obj->timer		     );
    fprintf( fp, "Affectby     %d %d %d %d %d\n",
                                              obj->affected_by[0],
                                              obj->affected_by[1],
                                              obj->affected_by[2],
                                              obj->affected_by[3],
                                              obj->affected_by[4]    );

    if( obj->in_room )
      fprintf( fp, "Room         %d\n",   obj->in_room->vnum         );

    fprintf( fp, "Cost         %d\n",	obj->cost		     );
    fprintf( fp, "Values       %d %d %d %d %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	    obj->value[4], obj->value[5], obj->value[6], obj->value[7] );

    switch ( obj->item_type )
    {
    case TYPE_POTION:
    case TYPE_SCROLL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1      '%s'\n", 
		spells_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2      '%s'\n", 
		spells_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		spells_table[obj->value[3]].name );
	}

	if ( obj->value[4] > 0 )
	{
	    fprintf( fp, "Spell 4      '%s'\n", 
		spells_table[obj->value[4]].name );
	}

	break;

    case TYPE_PILL:
    case TYPE_STAFF:
    case TYPE_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		spells_table[obj->value[3]].name );
	}

	break;
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	fprintf( fp, "Affect       %d %d %d %d %d %d\n",
		paf->skill,
		paf->spell,
		paf->duration,
		paf->modifier,
		paf->location,
		NUM_AFFECT_VECTORS );
        for( count = 0; count < NUM_AFFECT_VECTORS; count++)
        {
           fprintf( fp, " %d", paf->bitvector[count] );
        }
        fprintf( fp, "\n" );
    }

    for ( ed = obj->extra_descr; ed; ed = ed->next )
    {
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
		ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains )
	fwrite_obj( obj->contains, fp, iNest + 1 );

    tail_chain();
    return;
}

/*
 * Write a corpse and its contents.
 */
void fwrite_corpse( OBJ_DATA *obj, FILE *fp, int iNest )
{
    AFFECT_DATA      *paf;
    EXTRA_DESCR_DATA *ed;
    int count;

    last_fun( "fwrite_corpse");
    if ( obj->deleted )
	return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",	iNest			     );
    fprintf( fp, "Name         %s~\n",	obj->name		     );
    fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr	     );
    fprintf( fp, "Description  %s~\n",	obj->description	     );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );

    if ( obj->spec_fun )
      fprintf( fp, "Special      %s\n",	spec_obj_string( obj->spec_fun ) );

    fprintf( fp, "ExtraFlags   %d\n",	obj->extra_flags[0]	     );
    fprintf( fp, "ExtraFlags2  %d\n",	obj->extra_flags[1]	     );
    fprintf( fp, "WearFlags    %d\n",	obj->wear_flags		     );
    fprintf( fp, "WearLoc      %d\n",	obj->wear_loc		     );
    fprintf( fp, "AntiFlags    %d\n",   obj->anti_flags              );
    fprintf( fp, "AntiFlags2   %d\n",   obj->anti_flags2             );
    fprintf( fp, "ItemType     %d\n",	obj->item_type		     );
    fprintf( fp, "Weight       %d\n",	obj->weight		     );
    fprintf( fp, "Level        %d\n",	obj->level		     );
    fprintf( fp, "Timer        %d\n",	obj->timer		     );
    fprintf( fp, "Affectby     %d %d %d %d %d\n",
                                              obj->affected_by[0],
                                              obj->affected_by[1],
                                              obj->affected_by[2],
                                              obj->affected_by[3],
                                              obj->affected_by[4]    );

    if( obj->in_room )
      fprintf( fp, "Room         %d\n",   obj->in_room->vnum         );

    fprintf( fp, "Cost         %d\n",	obj->cost		     );
    fprintf( fp, "Values       %d %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
							obj->value[4]   );

    switch ( obj->item_type )
    {
    case TYPE_POTION:
    case TYPE_SCROLL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1      '%s'\n", 
		spells_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2      '%s'\n", 
		spells_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		spells_table[obj->value[3]].name );
	}

	if ( obj->value[4] > 0 )
	{
	    fprintf( fp, "Spell 4      '%s'\n", 
		spells_table[obj->value[4]].name );
	}

	break;

    case TYPE_PILL:
    case TYPE_STAFF:
    case TYPE_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		spells_table[obj->value[3]].name );
	}

	break;
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	fprintf( fp, "Affect       %d %d %d %d %d %d\n",
		paf->skill,
		paf->spell,
		paf->duration,
		paf->modifier,
		paf->location,
		NUM_AFFECT_VECTORS );
        for( count = 0; count < NUM_AFFECT_VECTORS; count++)
        {
           fprintf( fp, " %d", paf->bitvector[count] );
        }
        fprintf( fp, "\n" );
    }

    for ( ed = obj->extra_descr; ed; ed = ed->next )
    {
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
		ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains )
    {
        fwrite_obj( obj->contains, fp, 1 );
    }

    tail_chain();
    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
void load_corpses( )
{
    FILE      *fp;
    char       strsave [ MAX_INPUT_LENGTH ];

    last_fun( "load_corpses");
    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, CORPSE_FILE );
    if ( !( fp = fopen( strsave, "r" ) ) )
    {
       bug( "Error opening corpse file!  No corpses loaded!", 0 );
       return;
    }

    log_string( "Loading corpses" );    

    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, CORPSE_FILE );

    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	for ( ; ; )
	{
	    char *word;
	    char  letter;
	    int   status;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_corpses: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp, &status );

	    if( !str_cmp( word, "OBJECT" ) )
	    {
	        if ( !fread_corpse ( fp ) )
		{
		    bug( "Fread_corpse:  section OBJECT corrupt.\n\r", 0 );
		    return;
		}
	    }
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_corpses: bad section.", 0 );
		break;
	    }
	} /* for */

	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    extern char      *daPrompt;
           FILE      *fp;
	   CHAR_DATA *ch;
	   char       strsave [ MAX_INPUT_LENGTH ];
           int        count;
	   bool       found;
           char       buf[MAX_STRING_LENGTH];
           char       sorry_player [] =
	     "********************************************************\n\r"
	     "** One or more of the critical fields in your player  **\n\r"
	     "** file were corrupted since you last played.  Please **\n\r"
	     "** contact an administrator or programmer to          **\n\r"
	     "** investigate the recovery of your characters.       **\n\r"
	     "********************************************************\n\r";
           char       sorry_object [] =
	     "********************************************************\n\r"
	     "** One or more of the critical fields in your player  **\n\r"
	     "** file were corrupted leading to the loss of one or  **\n\r"
	     "** more of your possessions.                          **\n\r"
	     "********************************************************\n\r";

    // Veygoth - added this message for safety's sake.
    sprintf( buf, "Loading character: %s", name );
    log_string( buf );

    ch					= new_character( TRUE );

    ch->followers                       = NULL;
    ch->next_in_group                   = NULL;
    if ( d )
        d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( capitalize ( name ) );
    ch->pcdata->prompt                  = str_dup( daPrompt );
    ch->pcdata->train                   = 0;
    ch->last_note                       = 0;
    ch->save_time			= 0;
    ch->cast_time                       = 0;
    ch->cast_spl                        = 0;
    ch->act				= PLR_BLANK
					| PLR_COMBINE
					| PLR_PROMPT
                                        | PLR_AUTOEXIT
					| PLR_CAST_TICK
					| PLR_PAGER
                                        | PLR_MAP
                                        | PLR_TELL
                                        | PLR_TELNET_GA;
    // Height and weight are a hack till we add them to race structs
    // -- Veygoth
    ch->size                            = SIZE_MEDIUM;
    ch->pcdata->height                  = 60 + dice( 2, 11 );
    ch->pcdata->weight                  = ch->pcdata->height * 2;

    ch->pcdata->consent                 = NULL;
    ch->pcdata->guarding                = NULL;
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->pwdnew			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->immskll			= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->memorized               = NULL;
    for( count = 0; count < MAX_LEVEL; count++ )
    {
      ch->pcdata->trophy->vnum = 0;
      ch->pcdata->trophy->number = 0;
    }
    ch->perm_str		= dice( 3, 31 ) + 7;
    ch->perm_int		= dice( 3, 31 ) + 7; 
    ch->perm_wis		= dice( 3, 31 ) + 7;
    ch->perm_dex		= dice( 3, 31 ) + 7;
    ch->perm_con		= dice( 3, 31 ) + 7;
    ch->perm_agi                = dice( 3, 31 ) + 7;
    ch->perm_cha                = dice( 3, 31 ) + 7;
    ch->perm_pow                = dice( 3, 31 ) + 7;
    ch->perm_luk                = dice( 3, 31 ) + 7;
    ch->pcdata->condition[COND_THIRST]	= 48;
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->pagelen                 = 20;
    ch->pcdata->security		= 0;
    ch->pcdata->rank			= 0;
    ch->pcdata->clan	                = NULL;
    ch->pcdata->wiznet			= 0;

    ch->pcdata->switched                = FALSE;
    ch->pcdata->aggr_level              = -1;
    ch->pcdata->editing                 = EDIT_NONE;
    ch->pcdata->editbuf                 = NULL;
    found = FALSE;
    fclose( fpReserve );

    /* parsed player file directories by Yaz of 4th Realm */
    /* decompress if .gz file exists - Thx Alander */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ), ".gz" );
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
        char       buf     [ MAX_STRING_LENGTH ];

	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	system( buf );
    }
#endif

#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ) );

#else
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "\\", capitalize( name ) );
#endif
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	char buf[ MAX_STRING_LENGTH ];
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char *word;
	    char  letter;
	    int   status;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
                log_string( "Comment read in fread_char." );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
                sprintf( buf, "cp %s %s.bugged", strsave, strsave );
                log_string( buf );
		system( buf );
		break;
	    }

	    word = fread_word( fp, &status );
            if ( word == NULL )
	    {
		fread_to_eol( fp );
                log_string( "Empty word read in fread_char." );
		continue;
	    }

	    if ( !str_cmp( word, "PLAYER" ) )
	    {
	        if ( fread_char ( ch, fp ) )
		{
		    sprintf( buf,
			    "Load_char_obj:  %s section PLAYER corrupt.\n\r",
			    name );
		    bug( buf, 0 );
		    write_to_buffer( d, sorry_player, 0 );

		    /* 
		     * In case you are curious,
		     * it is ok to leave ch alone for close_socket
		     * to free.
		     * We want to now kick the bad character out as
		     * what we are missing are MANDATORY fields.  -Kahn
		     */
		    SET_BIT( ch->act, PLR_DENY );
		    return TRUE;
		}
	    }
	    else if ( !str_cmp( word, "OBJECT" ) )
	    {
/* Don't see the purpose of this anymore. - Lohrr*/
//                log_string( "Loading an object..." );
	        if ( !fread_obj  ( ch, fp ) )
		{
		    sprintf( buf,
			    "Load_char_obj:  %s section OBJECT corrupt.\n\r",
			    name );
		    bug( buf, 0 );
		    write_to_buffer( d, sorry_object, 0 );
		    return FALSE;
		}
	    }
	    else if ( !str_cmp( word, "END"    ) ) 
            {
                break;
            }
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
                sprintf( buf, "Bad word is: %s.", word);
                log_string(buf);
		break;
	    }
	} /* for */

	fclose( fp );
    }
    else
	log_string( strsave );

    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}

/*
 * Read in a char.
 */

int fread_char( CHAR_DATA *ch, FILE *fp )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    char        error [ MAX_STRING_LENGTH ];
    AFFECT_DATA *paf;
    int         sn;
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    int         status1;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;
    int         trophy = 0;
    int         count;
    int         numvectors;
    MEM_DATA    *mem;

    char        def_prompt [] =
         "&+g<%hhp %mm %vmv>\n\r<&n%T %E&+g>&n ";
    char        def_sdesc  [] = "Your short description was corrupted.";
    char        def_ldesc  [] = "Your long description was corrupted.";
    char        def_desc   [] = "Your description was corrupted.";
    char        def_title  [] = "Your title was corrupted.";

    struct key_data key_tab [] = {
      { "ShtDsc", TRUE,  (unlong) &def_sdesc,	{ &ch->short_descr,   NULL } },
      { "LngDsc", TRUE,  (unlong) &def_ldesc,	{ &ch->long_descr,    NULL } },
      { "Dscr",   TRUE,  (unlong) &def_desc,	{ &ch->description,   NULL } },
      { "Prmpt",  TRUE,  (unlong) &def_prompt,	{ &ch->pcdata->prompt,NULL } },
      { "Sx",     FALSE, SEX_MALE,		{ &ch->sex,           NULL } },
      { "Cla",    FALSE, MAND,			{ &ch->class,         NULL } },
      { "Lvl",    FALSE, MAND,			{ &ch->level,         NULL } },
      { "Trst",   FALSE, 0,			{ &ch->trust,         NULL } },
      { "Playd",  FALSE, 0,			{ &ch->played,        NULL } },
      { "Created",FALSE, 0,                     { &ch->pcdata->created, NULL } },
      { "Note",   FALSE, 0,			{ &ch->last_note,     NULL } },
      { "Saved",  FALSE, 0,		        { &ch->save_time,     NULL } },
      { "HitMnMv", FALSE, DEFLT,		{ &ch->hit,
						  &ch->max_hit,
						  &ch->mana,
						  &ch->max_mana,
						  &ch->move,
						  &ch->max_move,      NULL } },
      { "Copper",   FALSE, 0,			{ &ch->money.copper, NULL } },
      { "Silver",   FALSE, 0,			{ &ch->money.silver, NULL } },
      { "Gold",   FALSE, 0,			{ &ch->money.gold, NULL } },
      { "Platinum",   FALSE, 0,			{ &ch->money.platinum, NULL } },
      { "BCopper",   FALSE, 0,			{ &ch->pcdata->bank.copper, NULL } },
      { "BSilver",   FALSE, 0,			{ &ch->pcdata->bank.silver, NULL } },
      { "BGold",   FALSE, 0,			{ &ch->pcdata->bank.gold, NULL } },
      { "BPlatinum",   FALSE, 0,		{ &ch->pcdata->bank.platinum, NULL } },
      { "Experience", FALSE, DEFLT,		{ &ch->exp,           NULL } },
      { "Act",    FALSE, DEFLT,			{ &ch->act,           NULL } },
      { "Firstaid", FALSE, 0,                   { &ch->pcdata->firstaid, NULL } },
      { "Res",    FALSE, 0,			{ &ch->resistant,     NULL } },
      { "Imm",    FALSE, 0,			{ &ch->immune,        NULL } },
      { "Susc",   FALSE, 0,			{ &ch->susceptible,   NULL } },
      { "SavThr", FALSE, MAND,			{ &ch->saving_throw[0],
                                                  &ch->saving_throw[1],
                                                  &ch->saving_throw[2],
                                                  &ch->saving_throw[3],
                                                  &ch->saving_throw[4],
                                                                      NULL } },
      { "Align",  FALSE, 0,			{ &ch->alignment,     NULL } },
      { "Hit",    FALSE, MAND,			{ &ch->hitroll,       NULL } },
      { "Dam",    FALSE, MAND,			{ &ch->damroll,       NULL } },
      { "Armr",   FALSE, MAND,			{ &ch->armor,         NULL } },
      { "Wimp",   FALSE, 10,			{ &ch->wimpy,         NULL } },
      { "Orighome", FALSE, 0,                   { &ch->pcdata->orighome,NULL } },
      { "Currhome", FALSE, 0,                   { &ch->pcdata->currhome,NULL } },
      { "Lastrent", FALSE, 0,                   { &ch->pcdata->lastrent,NULL } },
      { "NewHP", FALSE, 0,                      { &ch->pcdata->new_hitpoints,NULL } },
      { "LostHP", FALSE, 0,                     { &ch->pcdata->lost_hp,NULL } },
      { "Spkng",  FALSE, LANG_COMMON,		{ &ch->pcdata->speaking, NULL } },
      { "Height", FALSE, 66,                    { &ch->pcdata->height, NULL } },
      { "Weight", FALSE, 140,                   { &ch->pcdata->weight, NULL } },
      { "Deaf",   FALSE, 0,			{ &ch->deaf,          NULL } },
      { "Paswd",  TRUE,  MAND,			{ &ch->pcdata->pwd,   NULL } },
      { "Bmfin",  TRUE,  DEFLT,			{ &ch->pcdata->bamfin,
						                  NULL } },
      { "Bmfout", TRUE,  DEFLT,			{ &ch->pcdata->bamfout,
						                  NULL } },
      { "Immskll",TRUE,  DEFLT,			{ &ch->pcdata->immskll,
						                  NULL } },
      { "Wiznet", FALSE,  0,			{ &ch->pcdata->wiznet,
						                  NULL } },
      { "Ttle",   TRUE,  (unlong) &def_title,	{ &ch->pcdata->title, NULL } },
      { "AtrPrm", FALSE, MAND,			{ &ch->perm_str,
						  &ch->perm_int,
						  &ch->perm_wis,
						  &ch->perm_dex,
						  &ch->perm_con,
						                  NULL } },
      { "AtrMd",  FALSE, MAND,			{ &ch->mod_str,
						  &ch->mod_int,
						  &ch->mod_wis,
						  &ch->mod_dex,
						  &ch->mod_con,
						                  NULL } },
      { "ExtAtr", FALSE, MAND,			{ &ch->perm_agi,
						  &ch->perm_cha,
						  &ch->perm_pow,
						  &ch->perm_luk,
						                  NULL } },
      { "ExtMd",  FALSE, MAND,			{ &ch->mod_agi,
						  &ch->mod_cha,
						  &ch->mod_pow,
						  &ch->mod_luk,
						                  NULL } },
      { "MaxMod",  FALSE, 0,			{ &ch->pcdata->maxmod_str,
						  &ch->pcdata->maxmod_int,
						  &ch->pcdata->maxmod_wis,
						  &ch->pcdata->maxmod_dex,
						  &ch->pcdata->maxmod_con,
						  &ch->pcdata->maxmod_agi,
						  &ch->pcdata->maxmod_cha,
						  &ch->pcdata->maxmod_pow,
						  &ch->pcdata->maxmod_luk,
						                  NULL } },
      { "RaceMod",  FALSE, 0,		{ &ch->pcdata->racemod_str,
						  &ch->pcdata->racemod_int,
						  &ch->pcdata->racemod_wis,
						  &ch->pcdata->racemod_dex,
						  &ch->pcdata->racemod_con,
						  &ch->pcdata->racemod_agi,
						  &ch->pcdata->racemod_cha,
						  &ch->pcdata->racemod_pow,
						  &ch->pcdata->racemod_luk,
						                  NULL } },
      { "Cond",   FALSE, DEFLT,			{ &ch->pcdata->condition [0],
						  &ch->pcdata->condition [1],
						  &ch->pcdata->condition [2],
						                  NULL } },
      { "Security", FALSE, DEFLT,             { &ch->pcdata->security,
								  NULL } },
      { "Train",        FALSE, 0,               { &ch->pcdata->train, NULL } },
  
      { "PKills",	FALSE, 0,		{ &ch->pcdata->pkills,
						                  NULL } },
      { "PDeaths",	FALSE, 0,		{ &ch->pcdata->pdeaths,
						                  NULL } },
      { "IllegalPK",	FALSE, 0,		{ &ch->pcdata->illegal_pk,
						                  NULL } },
      { "Frags",        FALSE, 0,               { &ch->pcdata->frags,
                                                                  NULL } },
      { "ModHit",       FALSE, 0,               { &ch->pcdata->mod_hit,
                                                                  NULL } },

      { "MKills",	FALSE, 0,		{ &ch->pcdata->mkills,
						                  NULL } },
      { "MDeaths",	FALSE, 0,		{ &ch->pcdata->mdeaths,
						                  NULL } },
      { "Pglen",  FALSE, 20,			{ &ch->pcdata->pagelen,
						                  NULL } },
      { "AggrLvl",      FALSE, -1,              { &ch->pcdata->aggr_level,
                                                                  NULL } },
      { "\0",     FALSE, 0                                                 } };


    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_char:  Error reading key.  EOF?", 0 );
            fread_to_eol( fp );
            break;
	}

                /* This little diddy searches for the keyword
                   from the last keyword found */

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp (key_tab [i % num_keys].key, word); )
            i++;

        i = i % num_keys;

        if ( !str_cmp (key_tab [i].key, word) )
            last_key = i;
        else
            i = num_keys;

//        log_string( word );

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
	    {
		sprintf( error, "Key (%s) already specified.", key_tab[i].key);
                bug ( error, 0 );
	    }

                        /* Entry is a string */

            else
	      if ( key_tab [i].string )
	      {
                    p = fread_string( fp, &status );
                  if ( p && !status )
		  {
		      free_string ( *(char **)key_tab [i].ptrs [0] );
		      *(char **)key_tab [i].ptrs [0] = p;
		  }
	      }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number ( fp, &status );
                    if ( !status )
                        *(int *)key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if( !str_cmp( word, "Pos" ) )
        {
            ch->position = fread_number( fp, &stat );
            if( ch->position <= POS_FIGHTING )
              ch->position = POS_STANDING;
        }

        else if ( *word == '*' || !str_cmp( word, "Nm" ) )
            fread_to_eol( fp );

        else if ( !str_cmp( word, "End" ) )
            break;

        else if( !str_cmp ( word, "Act2" ))
            fread_to_eol( fp );

        else if ( !str_cmp( word, "Room" ) )
	  {
	      ch->in_room = get_room_index( fread_number( fp, &status ) );
	      if ( !ch->in_room )
                  ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
	  }

	else if ( !str_cmp( word, "Race" ) )
	  {
	      i  = race_lookup( fread_string( fp, &status ) );

	      if ( status )
	      {
		  bug( "Fread_char: Error reading Race (%d).", i );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( i < 0 )
		  bug( "Fread_char: Unknown Race.", 0 );
	      else
		  ch->race = i;
	  }

	else if ( !str_cmp( word, "Size" ) )
	  {
	      i = ( fread_number( fp, &stat ) );
	      if ( ( i < 0 ) || ( i > MAX_SIZE ) )
	      {
	          bug( "Fread_number: Error reading Size.", 0 );
		  ch->size = SIZE_MEDIUM;
		  // continue;
	      }
	      else
	      {
	          ch->size = i;
	      }
	  }

        else if ( !str_cmp( word, "PClan" ) )
        {
	      char *clan_name;

	      ch->pcdata->rank      = fread_number( fp, &status );
	      clan_name             = fread_string( fp, &status1 );

	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading PClan.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( !clan_name
		  || !( ch->pcdata->clan = get_clan( clan_name ) ) )
	      {
		  bug( "Fread_char: Unknown PClan.", 0 );
                  if( clan_name )
                      send_to_char( "\n\r&+CYour clan has been disbanded!\n\r", ch );
                  log_string( clan_name );
		  ch->pcdata->rank      = 0;
                  ch->pcdata->clan = NULL;
		  continue;
	      }
        }

        else if ( !str_cmp( word, "AffdBy" ) )
	  {
              int numvectors = fread_number( fp, &status );

              // If there are more vectors in game than in file, the
              // remaining ones will be set to zero.  If there are more
              // vectors in file than in game, the fread_to_eol will
              // ensure that they are ignored - Veygoth
              for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
              {
                 if( count < numvectors )
                   ch->affected_by[count] = fread_number( fp, &status );
                 else
                   ch->affected_by[count] = 0;
              }
              fread_to_eol( fp );
	  }

        else if ( !str_cmp( word, "Skll" ) )
	  {
              i  = fread_number( fp, &status );
	      sn = skill_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading skill.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 || sn > MAX_SKILL )
	      {
		  sprintf( error, "Fread_char: unknown skill (%d).", sn );
                  bug( error, 0 );
	      }
	      else
                  ch->pcdata->skl_lrn[sn] = i;
	  }

        else if ( !str_cmp( word, "Spll" ) )
	  {
              i  = fread_number( fp, &status );
	      sn = spell_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading spell.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 )
                  bug( "Fread_char: unknown spell.", 0 );
	      else
                  ch->pcdata->spl_lrn[sn] = i;
	  }

        else if ( !str_cmp( word, "Song" ) )
	  {
              i  = fread_number( fp, &status );
	      sn = song_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading song.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 )
                  bug( "Fread_char: unknown song.", 0 );
	      else
                  ch->pcdata->sng_lrn[sn] = i;
	  }

        else if ( !str_cmp( word, "MSkl" ) )
	  {
              i  = fread_number( fp, &status );
	      sn = monk_skill_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading monk skill.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 || sn > MAX_MONK_SKILL )
	      {
		  sprintf( error, "Fread_char: unknown monk skill (%d).", sn );
                  bug( error, 0 );
	      }
	      else
                  ch->pcdata->monk_skill[sn] = i;
	  }

        else if ( !str_cmp( word, "Lang" ) )
	  {
              i  = fread_number( fp, &status );

              if( i >= MAX_LANG || i < 0 )
              {
                 bug( "Invalid language number %d found in pfile.  Skipping.", i );
                 fread_number( fp, &status );
                 continue;
              }
	      
              ch->pcdata->lang_lrn[i] = fread_number( fp, &status );

              if( ch->pcdata->lang_lrn[i] > 100 )
              {
                 bug( "Language value %d greater than 100", ch->pcdata->lang_lrn[i] );
                 ch->pcdata->lang_lrn[i] = 100;
              }
	  }

        else if ( !str_cmp( word, "Trophy"  ) )
          {
            if( trophy < MAX_LEVEL )
            {
              ch->pcdata->trophy[trophy].vnum = fread_number( fp, &stat );
              ch->pcdata->trophy[trophy].number = fread_number( fp, &stat );
              trophy++;
            }
            else
            {
              bug( "Fread_char: trophy filled up.", 0 );
              fread_number( fp, &stat );
              fread_number( fp, &stat );
            }
          }

        else if ( !str_cmp( word, "Memdata" ) )
	  {
	      sn = spell_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading spell.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 )
              {
                  bug( "Fread_char: unknown spell.", 0 );
              }
              else
              {
                  mem = create_memdata( ch, sn );

                  mem->memmed = fread_number( fp, &stat );
              }
	  }

	else if ( !str_cmp ( word, "Afft" ) )
	  {

	      int   status;
	      char  buf1 [ MAX_STRING_LENGTH ];

	      paf                 = new_affect();

	      temp_fread_string( fp, buf1 );
              if ( strlen(buf1) < 1 ) {
                  bug(" End of file encountered in fread_char.", 0);
                  fread_to_eol( fp );
                  send_to_char("An error was encountered loading your pfile.  This may result in a loss of equipment or affects.\n\r", ch);
                  return 1;
              }
	      paf->skill           = skill_affect_lookup( buf1 );
              temp_fread_string( fp, buf1 );
              if ( strlen(buf1) < 1 ) {
                  bug(" End of file encountered in fread_char.", 0);
                  fread_to_eol( fp );
                  send_to_char("An error was encountered loading your pfile.  This may result in a loss of equipment or affects.\n\r", ch);
                  return 1;
              }
	      paf->spell           = spell_affect_lookup( buf1 );
	      temp_fread_string( fp, buf1 );
              if ( strlen(buf1) < 1 ) {
                  bug(" End of file encountered in fread_char.", 0);
                  fread_to_eol( fp );
                  send_to_char("An error was encountered loading your pfile.  This may result in a loss of equipment or affects.\n\r", ch);
                  return 1;
              }
	      paf->song            =  song_affect_lookup( buf1 );

	      if ( paf->skill < 0 && paf->spell < 0 )
	      {
		  paf->next	  = affect_free;
		  affect_free	  = paf;

		  sprintf( buf, "Fread_char: Error reading Afft %s.", buf1 );
		  bug( buf, 0 );

		  fread_to_eol( fp );
		  continue;
	      }

	      paf->duration       = fread_number( fp, &status );
	      paf->modifier       = fread_number( fp, &status );
	      paf->location       = fread_number( fp, &status );
              numvectors          = fread_number( fp, &status );
              // If there are more vectors in game than in file, the
              // remaining ones will be set to zero.  If there are more
              // vectors in file than in game, the fread_to_eol will
              // ensure that they are ignored - Veygoth
              for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
              {
                 if( count < numvectors )
                   paf->bitvector[count] = fread_number( fp, &status );
                 else
                   paf->bitvector[count] = 0;
              }
              fread_to_eol( fp );

	      paf->deleted        = FALSE;
	      paf->next           = ch->affected;
	      ch->affected        = paf;
	  }

        else
	{
	    sprintf( buf, "Fread_char: Unknown key '%s' in pfile.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
	
    }

                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Manditory field '%s' missing from pfile.",
                          key_tab [i].key );
            bug( buf, 0 );
            error_count++;
            continue;
	}

               /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
	{
	    free_string( *(char **)key_tab [i].ptrs [0] );
            *(char **)key_tab [i].ptrs [0] =
	      str_dup( (char *)key_tab [i].deflt );
	}
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
	        *(int *)key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

                /* Fixups */

    if ( ch->pcdata->title )
    {
        sprintf( buf, " %s", ch->pcdata->title );
        free_string( ch->pcdata->title );
        ch->pcdata->title = str_dup( buf );
    }

    if( ch->pcdata->created == 0 )
        ch->pcdata->created = (int)current_time - ch->played;
    if ( IS_SET( ch->act, PLR_CAMPING ) )
        REMOVE_BIT( ch->act, PLR_CAMPING );
    return error_count;

}

void recover( FILE *fp, long fpos )
{
    char        buf[ MAX_STRING_LENGTH ];

    last_fun( "recover");
    fseek( fp, fpos, 0 );

    while ( !feof (fp) )
    {
        fpos = ftell( fp );

        if ( !fgets( buf, MAX_STRING_LENGTH, fp ) )
            return;

        if ( !strncmp( buf, "#OBJECT", 7 ) ||
             !strncmp( buf, "#END", 4 ) )
	{
            fseek( fp, fpos, 0 );
            return;
	}
    }
}

int fread_obj( CHAR_DATA *ch, FILE *fp )
{
    EXTRA_DESCR_DATA *ed;
    OBJ_DATA          obj;
    OBJ_DATA         *new_obj;
    AFFECT_DATA      *paf;
    char              buf[ MAX_STRING_LENGTH ];
    char             *spell_name = NULL;
    char             *p          = NULL;
    char             *word;
    char             *tmp_ptr;
    bool              fNest;
    bool              fVnum;
    long              fpos;
    int               iNest;
    int               iValue;
    int               status;
    int               sn;
    int               vnum;
    int               num_keys;
    int               last_key   = 0;
    int               i, j, tmpi;
    int               count;
    int               numvectors;

    char              corobj [] = "This object was corrupted.";

    struct key_data key_tab [] =
      {
	{ "Name",        TRUE,  MAND,             { &obj.name,        NULL } },
	{ "Material",    FALSE, 0,                { &obj.material,    NULL } },
	{ "ShortDescr",  TRUE,  (unlong) &corobj, { &obj.short_descr, NULL } },
	{ "Description", TRUE,  (unlong) &corobj, { &obj.description, NULL } },
	{ "ExtraFlags",  FALSE, MAND,             { &obj.extra_flags[0], NULL } },
	{ "ExtraFlags2", FALSE, MAND,             { &obj.extra_flags[1], NULL } },
	{ "WearFlags",   FALSE, MAND,             { &obj.wear_flags, NULL } },
	{ "WearLoc",     FALSE, MAND,             { &obj.wear_loc,    NULL } },
        { "AntiFlags",   FALSE, 0,                { &obj.anti_flags,  NULL } },
        { "AntiFlags2",  FALSE, 0,                { &obj.anti_flags,  NULL } },
	{ "ItemType",    FALSE, MAND,             { &obj.item_type,   NULL } },
	{ "Weight",      FALSE, 10,               { &obj.weight,      NULL } },
	{ "Level",       FALSE, 1        ,        { &obj.level,       NULL } },
	{ "Timer",       FALSE, 0,                { &obj.timer,       NULL } },
	{ "Affectby",    FALSE, 0,             { &obj.affected_by [0],
						 &obj.affected_by [1],
						 &obj.affected_by [2],
						 &obj.affected_by [3],
						 &obj.affected_by [4], NULL } },
	{ "Cost",        FALSE, 300,              { &obj.cost,        NULL } },
	{ "Values",      FALSE, MAND,                { &obj.value [0],
						    &obj.value [1],
						    &obj.value [2],
						    &obj.value [3],
						    &obj.value [4],
						    &obj.value [5],
						    &obj.value [6],
						    &obj.value [7],
                                                      NULL } },
	{ "\0",          FALSE, 0                                          } };

    memset( &obj, 0, sizeof( OBJ_DATA ) );

    obj.name        = str_dup( "" );
    obj.short_descr = str_dup( "" );
    obj.description = str_dup( "" );
    obj.deleted     = FALSE;

    fNest           = FALSE;
    fVnum           = TRUE;
    iNest           = 0;

    new_obj = new_object ();

    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( fpos = ftell( fp ) ; !feof( fp ) ; )
    {

        word = fread_word( fp, &status );

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp( key_tab [i % num_keys].key, word ); )
            i++;

        i = i % num_keys;

        if ( !str_cmp( key_tab [i].key, word ) )
            last_key = i + 1;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug( "Key already specified.", 0 );

                        /* Entry is a string */

            else if ( key_tab [i].string )
	    {
                if ( ( p = fread_string( fp, &status ) ) && !status )
		{
                   free_string ( * (char **) key_tab [i].ptrs [0] );
                   * (char **) key_tab [i].ptrs [0] = p;
		}
	    }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number( fp, &status );
                    if ( !status )
                        * (int *) key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( *word == '*' )
            fread_to_eol( fp );

        else if ( !str_cmp( word, "End" ) )
	{
            if ( !fNest || !fVnum )
	    {
                bug( "Fread_obj: incomplete object.", 0 );

		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );

		return FALSE;
	    }
            break;
	}

        else if ( !str_cmp( word, "Nest" ) )
	{

            iNest = fread_number( fp, &status );

            if ( status )       /* Losing track of nest level is bad */
                iNest = 0;      /* This makes objs go to inventory */

            else if ( iNest < 0 || iNest >= MAX_NEST )
                bug( "Fread_obj: bad nest %d.", iNest );

            else
	    {
                rgObjNest[iNest] = new_obj;
                fNest = TRUE;
	    }
	}

        else if( !str_cmp( word, "Special" ))
        {
            obj.spec_fun = spec_obj_lookup( fread_word( fp, &status ));
        }

        else if ( !str_cmp( word, "Spell" ) )
	{

            iValue = fread_number( fp, &status );

            if ( !status )
                spell_name = fread_word( fp, &status );

            if ( status )       /* Recover is to skip spell */
	    {
                fread_to_eol( fp );
                continue;
	    }

            sn = spell_lookup( spell_name );

            if ( iValue < 0 || iValue > 4 )
                bug( "Fread_obj: bad iValue %d.", iValue );

            else if ( sn < 0 )
                bug( "Fread_obj: unknown skill.", 0 );

            else
                obj.value [iValue] = sn;
	}

        else if ( !str_cmp( word, "Vnum" ) )
	{

            vnum = fread_number( fp, &status );

            if ( status )               /* Can't live without vnum */
	    {
                log_string( "Object: Error reading vnum" );
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            if ( !( obj.pIndexData = get_obj_index( vnum ) ) ) {
                bug( "Fread_obj: bad vnum %d.", vnum );
                send_to_char( "Bad vnum loading objects.\n\r", ch );
                recover    ( fp, fpos        );
                free_string( obj.name        );
                free_string( obj.short_descr );
                free_string( obj.description );
                extract_obj( new_obj         );
                return FALSE;
            }
            else
                fVnum = TRUE;
	}

                /* The following keys require extra processing */

        if ( !str_cmp( word, "Affect" ) )
	{
            paf = new_affect ();

	    paf->skill      = fread_number( fp, &status );
	    paf->spell      = fread_number( fp, &status );
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
            numvectors      = fread_number( fp, &status );
            // If there are more vectors in game than in file, the
            // remaining ones will be set to zero.  If there are more
            // vectors in file than in game, the fread_to_eol will
            // ensure that they are ignored - Veygoth
            for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
            {
               if( count < numvectors )
                 paf->bitvector[count] = fread_number( fp, &status );
               else
                 paf->bitvector[count] = 0;
            }
            fread_to_eol( fp );

            paf->next = obj.affected;
            obj.affected = paf;
	}

        else if ( !str_cmp( word, "ExtraDescr" ) )
	{
	    tmp_ptr = fread_string( fp, &status );

            if ( !status )
                p = fread_string( fp, &status );

            if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            ed = new_extra_descr ();

            ed->keyword     = tmp_ptr;
            ed->description = p;
            ed->next        = obj.extra_descr;
            obj.extra_descr = ed;
	}
    }
                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Mandatory obj field '%s' missing from pfile(%s).",
		    key_tab [i].key, ch->name );
            bug( buf, 0 );

	    recover    ( fp, fpos        );
	    free_string( obj.name        );
	    free_string( obj.short_descr );
	    free_string( obj.description );
	    extract_obj( new_obj         );

	    return FALSE;
	}

                /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
            * (char **) key_tab [i].ptrs [0] =
                        str_dup ( (char *) key_tab [i].deflt );
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
                * (int *) key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

    memcpy( new_obj, &obj, sizeof( OBJ_DATA ) );

    new_obj->next = object_list;
    object_list   = new_obj;

    new_obj->pIndexData->count++;

    if ( fVnum )
    {
         new_obj->anti_flags = obj.pIndexData->anti_flags;
         new_obj->anti_flags2 = obj.pIndexData->anti_flags2;
    }
    else
         new_obj->anti_flags = 1;

    if ( iNest == 0 || !rgObjNest[iNest] )
    {
          obj_to_char( new_obj, ch );
    }
    else
    {
        obj_to_obj( new_obj, rgObjNest[iNest-1] );
    }
    return TRUE;
}

int fread_corpse( FILE *fp )
{
    EXTRA_DESCR_DATA *ed;
    OBJ_DATA         obj;
    OBJ_DATA         *new_obj;
    AFFECT_DATA      *paf;
    char              buf[ MAX_STRING_LENGTH ];
    char             *spell_name = NULL;
    char             *p          = NULL;
    char             *word;
    char             *tmp_ptr;
    bool              fNest;
    bool              fVnum;
    long              fpos;
    int               iNest;
    int               iValue;
    int               status;
    int               sn;
    int               vnum;
    int               num_keys;
    int               last_key   = 0;
    int               i, j, tmpi;
    int               count;
    int               numvectors;
    int               room = ROOM_VNUM_LIMBO;

    char              corobj [] = "This object was corrupted.";

    struct key_data key_tab [] =
      {
	{ "Name",        TRUE,  MAND,             { &obj.name,        NULL } },
	{ "ShortDescr",  TRUE,  (unlong) &corobj, { &obj.short_descr, NULL } },
	{ "Description", TRUE,  (unlong) &corobj, { &obj.description, NULL } },
	{ "ExtraFlags",  FALSE, MAND,             { &obj.extra_flags[0], NULL } },
	{ "ExtraFlags2",  FALSE, MAND,            { &obj.extra_flags[1], NULL } },
	{ "WearFlags",   FALSE, MAND,             { &obj.wear_flags,  NULL } },
	{ "WearLoc",     FALSE, MAND,             { &obj.wear_loc,    NULL } },
        { "AntiFlags",   FALSE, 0,                { &obj.anti_flags,  NULL } },
        { "AntiFlags2",  FALSE, 0,                { &obj.anti_flags,  NULL } },
	{ "ItemType",    FALSE, MAND,             { &obj.item_type,   NULL } },
	{ "Weight",      FALSE, 10,               { &obj.weight,      NULL } },
	{ "Level",       FALSE, 1        ,        { &obj.level,       NULL } },
	{ "Timer",       FALSE, 0,                { &obj.timer,       NULL } },
	{ "Affectby",    FALSE, 0,             { &obj.affected_by[0],
						 &obj.affected_by[1],
						 &obj.affected_by[2],
						 &obj.affected_by[3],
						 &obj.affected_by[4], NULL } },
	{ "Cost",        FALSE, 300,              { &obj.cost,        NULL } },
	{ "Values",      FALSE, MAND,             { &obj.value [0],
						    &obj.value [1],
						    &obj.value [2],
						    &obj.value [3],
						    &obj.value [4],
                                                    NULL } },
	{ "\0",          FALSE, 0                                          } };

    memset( &obj, 0, sizeof( OBJ_DATA ) );

    obj.name        = str_dup( "" );
    obj.short_descr = str_dup( "" );
    obj.description = str_dup( "" );
    obj.deleted     = FALSE;

    fNest           = FALSE;
    fVnum           = TRUE;
    iNest           = 0;

    new_obj = new_object ();

    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( fpos = ftell( fp ) ; !feof( fp ) ; )
    {

        word = fread_word( fp, &status );

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp( key_tab [i % num_keys].key, word ); )
            i++;

        i = i % num_keys;

        if ( !str_cmp( key_tab [i].key, word ) )
            last_key = i + 1;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug( "Key already specified.", 0 );

                        /* Entry is a string */

            else if ( key_tab [i].string )
	    {
                if ( ( p = fread_string( fp, &status ) ) && !status )
		{
                   free_string ( * (char **) key_tab [i].ptrs [0] );
                   * (char **) key_tab [i].ptrs [0] = p;
		}
	    }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number( fp, &status );
                    if ( !status )
                        * (int *) key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( *word == '*' )
            fread_to_eol( fp );

        else if ( !str_cmp( word, "Room" ) )
            room = fread_number( fp, &stat );

        else if ( !str_cmp( word, "End" ) )
	{
            if ( !fNest || !fVnum )
	    {
                bug( "Fread_obj: incomplete object.", 0 );

		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );

		return FALSE;
	    }
            break;
	}

        else if ( !str_cmp( word, "Nest" ) )
	{

            iNest = fread_number( fp, &status );

            if ( status )       /* Losing track of nest level is bad */
                iNest = 0;      /* This makes objs go to inventory */

            else if ( iNest < 0 || iNest >= MAX_NEST )
                bug( "Fread_obj: bad nest %d.", iNest );

            else
	    {
                rgObjNest[iNest] = new_obj;
                fNest = TRUE;
	    }
	}

        else if( !str_cmp( word, "Special" ))
        {
            obj.spec_fun = spec_obj_lookup( fread_word( fp, &status ));
        }

        else if ( !str_cmp( word, "Spell" ) )
	{

            iValue = fread_number( fp, &status );

            if ( !status )
                spell_name = fread_word( fp, &status );

            if ( status )       /* Recover is to skip spell */
	    {
                fread_to_eol( fp );
                continue;
	    }

            sn = spell_lookup( spell_name );

            if ( iValue < 0 || iValue > 4 )
                bug( "Fread_obj: bad iValue %d.", iValue );

            else if ( sn < 0 )
                bug( "Fread_obj: unknown skill.", 0 );

            else
                obj.value [iValue] = sn;
	}

        else if ( !str_cmp( word, "Vnum" ) )
	{

            vnum = fread_number( fp, &status );

            if ( status )               /* Can't live without vnum */
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            if ( !( obj.pIndexData = get_obj_index( vnum ) ) )
                bug( "Fread_obj: bad vnum %d.", vnum );
            else
                fVnum = TRUE;
	}

                /* The following keys require extra processing */

        if ( !str_cmp( word, "Affect" ) )
	{
            paf = new_affect ();

	    paf->skill      = fread_number( fp, &status );
	    paf->spell      = fread_number( fp, &status );
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
            numvectors      = fread_number( fp, &status );
            // If there are more vectors in game than in file, the
            // remaining ones will be set to zero.  If there are more
            // vectors in file than in game, the fread_to_eol will
            // ensure that they are ignored - Veygoth
            for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
            {
               if( count < numvectors )
                 paf->bitvector[count] = fread_number( fp, &status );
               else
                 paf->bitvector[count] = 0;
            }
            fread_to_eol( fp );

            paf->next = obj.affected;
            obj.affected = paf;
	}

        else if ( !str_cmp( word, "ExtraDescr" ) )
	{
	    tmp_ptr = fread_string( fp, &status );

            if ( !status )
                p = fread_string( fp, &status );

            if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            ed = new_extra_descr ();

            ed->keyword     = tmp_ptr;
            ed->description = p;
            ed->next        = obj.extra_descr;
            obj.extra_descr = ed;
	}
    }
                /* Require all mandatory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Mandatory obj field '%s' missing from corpse (%s).",
		    key_tab [i].key, obj.name );
            bug( buf, 0 );

	    recover    ( fp, fpos        );
	    free_string( obj.name        );
	    free_string( obj.short_descr );
	    free_string( obj.description );
	    extract_obj( new_obj         );

	    return FALSE;
	}

                /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
            * (char **) key_tab [i].ptrs [0] =
                        str_dup ( (char *) key_tab [i].deflt );
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
                * (int *) key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

    memcpy( new_obj, &obj, sizeof( OBJ_DATA ) );

    new_obj->next = object_list;
    object_list   = new_obj;

    new_obj->pIndexData->count++;

    if ( iNest == 0 || !rgObjNest[iNest] )
    {
          ROOM_INDEX_DATA *location;
          location = get_room_index( room );
          if ( location == NULL )
            location = get_room_index( ROOM_VNUM_LIMBO );
          sprintf( buf, "Sending object %s to room %d",
            obj.name, room );
          obj_to_room( new_obj, location );
    }
    else
    {
        obj_to_obj( new_obj, rgObjNest[iNest-1] );
    }
    return TRUE;
}

