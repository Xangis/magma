/*****************************************************************************
 Name:          save_help_are
 Written by:    Christopher Smith (stoked@iastate.edu)
 Called by:     do_asave(olc_save.c).
 ****************************************************************************/
void save_help_are()
{
    HELP_DATA *pHelp;
    FILE *fp;

    fclose( fpReserve );
    if ( !( fp = fopen( "help.are", "w" ) ) )
    {
        bug( "Open_area: fopen help.are", 0 );
        return;
    }

    fprintf( fp, "#HELPS\n\n" );

    for( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
            fprintf( fp, "%d %s~\n%s~\n\n",
                pHelp->level,
                all_capitalize( pHelp->keyword ),
                fix_string( pHelp->text ) );
    }

    fprintf( fp, "\n0 $~\n\n" );
    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

--------------------------------------------------------------------------
Then in do_asave, just below the "Save the area.lst file", I inserted:
    /* Save the help.are file. */
    /* ----------------------- */
    if ( !str_cmp( arg1, "helps" ) )
    {
        save_help_are();
        return;
    }

So basically you're done, and you just need to type 'asave helps' after
you edit a helpfile.  If you want to autosave, then you could throw in
a call to save_help_are() in the "done" part in do_hedit, something like this:
    if ( !str_cmp(command, "done") )
    {
        edit_done( ch );
        save_help_are();
        return;
    }

