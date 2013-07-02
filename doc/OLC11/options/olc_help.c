/***************************************************************************
 *  File: olc_help.c                                                       *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This work is a derivative of Talen's post to the Merc Mailing List.    *
 *  It has been modified by Jason Dinkel to work with the new OLC.         *
 *                                                                         *
 ***************************************************************************/
/*
 * Installation Instructions.
 * 
 * ---- olc.h
 * (1) #define ED_HELP 5
 * (2) void	hedit		args( ( CHAR_DATA *ch, char *argument ) );
 * (3) DECLARE_DO_FUN( do_hedit	);
 * (4) extern const struct olc_cmd_type	hedit_table[];
 * (5) HELP_DATA	*new_help		args ( (void) );
 * (6) void		free_help		args ( ( HELP_DATA *pHelp ) );
 * (7) extern HELP_DATA		*	help_last;
 * ---- olc.c
 * (1) run_olc_editor:
    case ED_HELP:
	hedit( d->character, d->incomm );
	break;
 * (2) olc_ed_name:
    case ED_HELP:
	sprintf( buf, "HEdit" );
	break;
 * (3) olc_ed_vnum:
    case ED_HELP:
	sprintf( buf, "%d", 0 );
	break;
 * (4) show_commands:
	case ED_HELP:
	    show_olc_cmds( ch, hedit_table );
	    break;
 * (5) editor_table:
    {   "help",		do_hedit	},
 * ---- mem.c
 * (1) Drop in this code at the end of the file.

HELP_DATA *new_help(void)
{
    HELP_DATA *NewHelp;

    if( help_free == NULL )
    {
        NewHelp = alloc_perm( sizeof(*NewHelp) );
        top_help++;
    }
    else
    {
        NewHelp         = help_free;
        help_free       = help_free->next;
    }

    NewHelp->next       = NULL;
    NewHelp->level      = 0;
    NewHelp->keyword    = &str_empty[0];
    NewHelp->text       = &str_empty[0];

    return NewHelp;
}



void free_help( HELP_DATA *pHelp )
{
    free_string( pHelp->keyword );
    free_string( pHelp->text );

    pHelp->next = help_free;
    help_free   = pHelp;
    return;
}

 * That's all there is to install.
 */

#if defined(macintosh)
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
#include "olc.h"


#define HEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define EDIT_HELP(Ch, Help)	( Help = (HELP_DATA *)Ch->desc->pEdit )

/*
 * Help Editor Prototypes
 */
DECLARE_OLC_FUN( hedit_create 		);
DECLARE_OLC_FUN( hedit_delete 		);
DECLARE_OLC_FUN( hedit_desc 		);
DECLARE_OLC_FUN( hedit_level 		);
DECLARE_OLC_FUN( hedit_keywords 	);
DECLARE_OLC_FUN( hedit_show 		);



const struct olc_cmd_type hedit_table[] =
{
/*  {   command		function	}, */

    {   "commands",	show_commands	},
    {   "create",	hedit_create	},
    {   "delete",	hedit_delete	},
    {   "desc",		hedit_desc	},
    {   "level",	hedit_level	},
    {	"keywords",	hedit_keywords	},
    {	"show",		hedit_show	},
    {   "text",		hedit_desc	},

    {   "?",		show_help	},

    {	"",		0,		}
};



void hedit( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    if ( ch->pcdata->security == 0 )
	send_to_char( "HEdit: Insufficient security to modify area.\n\r", ch );

    if( command[0] == '\0' )
    {
	hedit_show( ch, argument );
	return;
    }

    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }

    if ( ch->pcdata->security == 0 )
    {
	interpret( ch, arg );
	return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; @hedit_table[cmd].name; cmd++ )
    {
	if ( !str_prefix( command, hedit_table[cmd].name ) )
	{
	    (*hedit_table[cmd].olc_fun) ( ch, argument );
	    return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}



/* Entry point for editing help_data. */
void do_hedit( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    HELP_DATA   *iHelp;

    if( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  hedit <keyword>\n\r", ch );
	return;
    }
    else
    {
	for( iHelp = help_first; iHelp; iHelp = iHelp->next )
	{
	    /*
	     * This help better not exist already!
	     */
	    if( is_name( arg, iHelp->keyword ) )
	    {
		ch->desc->pEdit = (void *)iHelp;
		ch->desc->editor = ED_HELP;
		break;
	    }
	}

	if( !iHelp )
	{
	    iHelp		= new_help();
	    iHelp->keyword	= str_dup( arg );

	    if( !help_first )
		help_first	= iHelp;
	    if( help_last )
		help_last->next	= iHelp;

	    help_last	= iHelp;
	    iHelp->next	= NULL;
	    ch->desc->pEdit     = (void *)iHelp;
	    ch->desc->editor = ED_HELP;
	}
    }
    hedit_show( ch, "" );
    return;
}



HEDIT( hedit_show )
{
    HELP_DATA *pHelp;
    char buf[MAX_STRING_LENGTH];
    
    if ( !EDIT_HELP( ch, pHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    sprintf( buf,
            "Seen at level: [%d]\n\r"
            "Keywords:      [%s]\n\r"
            "Text:\n\r%s\n\r",
            pHelp->level, pHelp->keyword, pHelp->text );
    send_to_char( buf, ch );

    return FALSE;
}



HEDIT( hedit_create )
{
    HELP_DATA *iHelp;
    HELP_DATA *NewHelp;
    char buf[MAX_STRING_LENGTH];

    if ( !EDIT_HELP( ch, iHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    if( argument[0] == '\0' )
    {
	send_to_char( "Syntax: create <keywords>\n\r", ch );
	return FALSE;
    }

    /*
     * This help better not exist already!
     */
    for( iHelp = help_first; iHelp; iHelp = iHelp->next )
    {
	if( is_name( argument, iHelp->keyword ) )
	{
	    send_to_char( "That help file already exists.\n\r", ch );
	    return FALSE;
	}
    }

    NewHelp		= new_help();
    NewHelp->keyword	= str_dup( argument );

    if( !help_first )	/* If it is we have a leak */
	help_first	= NewHelp;
    if( help_last )
	help_last->next	= NewHelp;

    help_last	= NewHelp;
    NewHelp->next	= NULL;
    ch->desc->pEdit	= (void *)NewHelp;
    ch->desc->editor = ED_HELP;

    sprintf( buf, "Created help with the keyword(s): %s\n\r",
	NewHelp->keyword );
    send_to_char( buf, ch );

    return TRUE;
}



HEDIT( hedit_delete )
{
    HELP_DATA *pHelp;
    HELP_DATA *PrevHelp = help_first;

    if ( !EDIT_HELP( ch, pHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    if( argument[0] == '\0' )
    {
	send_to_char( "Syntax: delete <keyword>\n\r", ch );
	return FALSE;
    }

    /*
     * This help better exist
     */
    for( pHelp = help_first; pHelp; PrevHelp = pHelp, pHelp = pHelp->next )
    {
	if( is_name( argument, pHelp->keyword ) )
	    break;
    }

    if( !pHelp )
    {
	send_to_char( "That help file does not exist.\n\r", ch );
	return FALSE;
    }

    if( pHelp == (HELP_DATA *)ch->desc->pEdit )
    {
	edit_done( ch );
    }

    if( !PrevHelp )          /* At first help file   */
    {
	help_first  = pHelp->next;
	free_help( pHelp );
    }
    else if( !pHelp->next )  /* At the last help file*/
    {
	help_last           = PrevHelp;
	PrevHelp->next      = NULL;
	free_help( pHelp );
    }
    else                            /* Somewhere else...    */
    {
	PrevHelp->next      = pHelp->next;
	free_help( pHelp );
    }

    send_to_char( "Help file deleted.\n\r", ch );
    return TRUE;
}



HEDIT( hedit_desc )
{
    HELP_DATA *pHelp;

    if ( !EDIT_HELP( ch, pHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    if ( argument[0] != '\0' )
    {
	send_to_char( "Syntax:  desc\n\r", ch );
	return FALSE;
    }
    
    string_append( ch, &pHelp->text );
    return TRUE;
}



HEDIT( hedit_level )
{
    HELP_DATA *pHelp;
    int value;

    if ( !EDIT_HELP( ch, pHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    value = atoi( argument );

    if ( argument[0] == '\0' || value < -1 )
    {
	send_to_char( "Syntax:  level [level >= -1]\n\r", ch );
	return FALSE;
    }

    pHelp->level = value;
    send_to_char( "Help level set.\n\r", ch );

    return TRUE;
}



HEDIT( hedit_keywords )
{
    HELP_DATA *pHelp;
    int i;
    int length;

    if ( !EDIT_HELP( ch, pHelp ) )
    {
	send_to_char( "Null help file.\n\r", ch );
	return FALSE;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  keywords <keywords>\n\r", ch );
	return FALSE;
    }

    length = strlen(argument);
    for (i = 0; i < length; i++)
	argument[i] = toupper(argument[i]);

    pHelp->keyword = str_dup( argument );
    send_to_char( "Help keywords set.\n\r", ch );
    return TRUE;
}
