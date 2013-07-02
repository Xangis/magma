/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  The MOBprograms have been contributed by N'Atas-ha.  Any support for   *
 *  these routines should not be expected from Merc Industries.  However,  *
 *  under no circumstances should the blame for bugs, etc be placed on     *
 *  Merc Industries.  They are not guaranteed to work on all systems due   *
 *  to their frequent use of strxxx functions.  They are also not the most *
 *  efficient way to perform their tasks, but hopefully should be in the   *
 *  easiest possible way to install and begin using. Documentation for     *
 *  such installation can be found in INSTALL.  Enjoy...         N'Atas-Ha *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"



/*
 *  Mudprogram additions
 */
ACT_PROG_DATA *	mob_act_list;
PROG_DATA progdata;

/*
 * Local function prototypes
 */
char *	prog_next_command	args( ( char *clist ) );
bool 	prog_seval		args( ( char *lhs, char *opr, char *rhs ) );
bool 	prog_veval		args( ( int lhs, char *opr, int rhs ) );
bool 	prog_do_ifchck		args( ( char *ifchck, CHAR_DATA *mob,
				       CHAR_DATA *actor, OBJ_DATA *obj,
				       void *vo, CHAR_DATA *rndm ) );
char *	prog_process_if	args( ( char *ifchck, char *com_list,
				       CHAR_DATA *mob, CHAR_DATA *actor,
				       OBJ_DATA *obj, void *vo,
				       CHAR_DATA *rndm ) );
void 	prog_translate		args( ( char ch, char *t, CHAR_DATA *mob,
				       CHAR_DATA *actor, OBJ_DATA *obj,
				       void *vo, CHAR_DATA *rndm ) );
void 	prog_process_cmnd	args( ( char *cmnd, CHAR_DATA * mob,
				       CHAR_DATA *actor, OBJ_DATA *obj,
				       void *vo, CHAR_DATA *rndm ) );
void 	prog_driver		args( ( char *com_list, CHAR_DATA *mob,
				       CHAR_DATA *actor, OBJ_DATA *obj,
				       void *vo ) );

/*
 * Local function code and brief comments.
 */

/*
 * Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 */
char *prog_next_command( char *clist )
{
    char *pointer = clist;

    last_fun( "prog_next_command");

    while ( *pointer != '\n' && *pointer != '\0' )
	pointer++;
    if ( *pointer == '\n' )
	*pointer++ = '\0';
    if ( *pointer == '\r' )
	*pointer++ = '\0';

    return ( pointer );
}


/*
 * These two functions do the basic evaluation of ifcheck operators.
 * It is important to note that the string operations are not what
 * you probably expect.  Equality is exact and division is substring.
 * remember that lhs has been stripped of leading space, but can
 * still have trailing spaces so be careful when editing since:
 * "guard" and "guard " are not equal.
 */
bool prog_seval( char *lhs, char *opr, char *rhs )
{
    last_fun( "prog_seval");

    if ( !lhs || !rhs )
	return 0;

    if ( !str_cmp( opr, "==" ) )
	return (bool) ( !str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "!=" ) )
	return (bool) ( str_cmp( lhs, rhs ) );
    if ( !str_cmp( opr, "/" ) )
	return (bool) ( !str_infix( rhs, lhs ) );
    if ( !str_cmp( opr, "!/" ) )
	return (bool) ( str_infix( rhs, lhs ) );

    bug( "MOBProgs: Improper MOBprog operator\n\r", 0 );
    return 0;
}


bool prog_veval( int lhs, char *opr, int rhs )
{
    last_fun( "prog_veval");

    if ( !lhs || !rhs )
	return 0;

    if ( !str_cmp( opr, "==" ) )
	return ( lhs == rhs );
    if ( !str_cmp( opr, "!=" ) )
	return ( lhs != rhs );
    if ( !str_cmp( opr, ">" ) )
	return ( lhs > rhs );
    if ( !str_cmp( opr, "<" ) )
	return ( lhs < rhs );
    if ( !str_cmp( opr, ">=" ) )
	return ( lhs <= rhs );
    if ( !str_cmp( opr, ">=" ) )
	return ( lhs >= rhs );
    if ( !str_cmp( opr, "&" ) )
	return ( lhs & rhs );
    if ( !str_cmp( opr, "|" ) )
	return ( lhs | rhs );

    bug( "MOBProgs: Improper MOBprog operator\n\r", 0 );
    return 0;
}


/*
 * This function performs the evaluation of the if checks.  It is
 * here that you can add any ifchecks which you so desire. Hopefully
 * it is clear from what follows how one would go about adding your
 * own. The syntax for an if check is: ifchck ( arg ) [opr val]
 * where the parenthesis are required and the opr and val fields are
 * optional but if one is there then both must be. The spaces are all
 * optional. The evaluation of the opr expressions is farmed out
 * to reduce the redundancy of the mammoth if statement list.
 * If there are errors, then return FALSE otherwise return boolean 1, 0
 *
 * Killed all that big copy-code that performs the same action on each
 * variable - Zen
 */
bool prog_do_ifchck( char *ifchck, CHAR_DATA * mob, CHAR_DATA * actor,
		     OBJ_DATA * obj, void *vo, CHAR_DATA * rndm )
{
    char        buf [ MAX_INPUT_LENGTH ];
    char        arg [ MAX_INPUT_LENGTH ];
    char        opr [ MAX_INPUT_LENGTH ];
    char        val [ MAX_INPUT_LENGTH ];
    CHAR_DATA  *vict	= (CHAR_DATA *) vo;
    OBJ_DATA   *v_obj	= (OBJ_DATA *) vo;
    char       *bufpt	= buf;
    char       *argpt	= arg;
    char       *oprpt	= opr;
    char       *valpt	= val;
    char       *point	= ifchck;
    int         lhsvl;
    int         rhsvl;
    CHAR_DATA  *chkchar	= NULL;
    OBJ_DATA   *chkobj	= NULL;

    last_fun( "prog_do_ifchk");

    if ( *point == '\0' )
    {
	bug( "Mob: %d null ifchck", mob->pIndexData->vnum );
	return FALSE;
    }

    /*
     * skip leading spaces 
     */
    while ( *point == ' ' )
	point++;

    /*
     * get whatever comes before the left paren.. ignore spaces 
     */
    while ( *point != '(' )
	if ( *point == '\0' )
	{
	    bug( "Mob: %d ifchck syntax error", mob->pIndexData->vnum );
	    return FALSE;
	}
	else if ( *point == ' ' )
	    point++;
	else
	    *bufpt++ = *point++;

    *bufpt = '\0';
    point++;

    /*
     * get whatever is in between the parens.. ignore spaces 
     */
    while ( *point != ')' )
	if ( *point == '\0' )
	{
	    bug( "Mob: %d ifchck syntax error", mob->pIndexData->vnum );
	    return FALSE;
	}
	else if ( *point == ' ' )
	    point++;
	else
	    *argpt++ = *point++;

    *argpt = '\0';
    point++;

    /*
     * check to see if there is an operator 
     */
    while ( *point == ' ' )
	point++;
    if ( *point == '\0' )
    {
	*opr = '\0';
	*val = '\0';
    }
    else
	/*
	 * there should be an operator and value, so get them 
	 */
    {
	while ( ( *point != ' ' ) && ( !isalnum( *point ) ) )
	    if ( *point == '\0' )
	    {
		bug( "Mob: %d ifchck operator without value",
		    mob->pIndexData->vnum );
		return FALSE;
	    }
	    else
		*oprpt++ = *point++;

	*oprpt = '\0';

	/*
	 * finished with operator, skip spaces and then get the value 
	 */
	while ( *point == ' ' )
	    point++;
	for ( ; ; )
	{
	    if ( ( *point != ' ' ) && ( *point == '\0' ) )
		break;
	    else
		*valpt++ = *point++;
	}

	*valpt = '\0';
    }
    bufpt = buf;
    argpt = arg;
    oprpt = opr;
    valpt = val;

    /*
     * Ok... now buf contains the ifchck, arg contains the inside of the
     * parentheses, opr contains an operator if one is present, and val
     * has the value if an operator was present.
     * So.. basically use if statements and run over all known ifchecks
     * Once inside, use the argument and expand the lhs. Then if need be
     * send the lhs, opr, rhs off to be evaluated.
     */

    if ( arg[0] == '$' )
    {
	char        buf1 [ MAX_INPUT_LENGTH ];	/* Used for output */

	/*
	 * arg should be "$*" so just get the letter 
	 */
	switch( arg[1] )
	{
	case 'i':	chkchar	= mob;					break;
	case 'n':	chkchar	= actor;				break;
	case 't':	chkchar	= vict;					break;
	case 'r':	chkchar	= rndm;					break;
	case 'o':	chkobj	= obj;					break;
	case 'p':	chkobj	= v_obj;				break;
	default:

	    sprintf( buf1, "Mob: %%d bad argument '%s' to '%s'", arg, buf );
	    bug( buf1, mob->pIndexData->vnum );
	    return FALSE;
	}
	if ( !chkchar && !chkobj )
	    return FALSE;
    }

    if ( !str_cmp( buf, "rand" ) )
    {
	return ( number_percent() <= atoi( arg ) );
    }

    if ( chkchar
	&& !chkchar->deleted )
    {
	if ( !str_cmp( buf, "ispc" ) )
	    return ( IS_NPC( chkchar ) ? FALSE : TRUE );

	if ( !str_cmp( buf, "isnpc" ) )
	    return ( IS_NPC( chkchar ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "ismounted" ) )
	    return ( chkchar->riding   ? TRUE : FALSE );

	if ( !str_cmp( buf, "isgood" ) )
	    return ( IS_GOOD( chkchar ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "isneutral" ) )
	    return ( IS_NEUTRAL( chkchar ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "isevil" ) )
	    return ( IS_EVIL( chkchar ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "isfight" ) )
	    return ( chkchar->fighting ? 1 : 0 );

	if ( !str_cmp( buf, "isimmort" ) )
	    return ( IS_IMMORTAL( chkchar ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "ischarmed" ) )
	    return ( is_affected( chkchar, AFF_CHARM ) ? TRUE : FALSE );

	if ( !str_cmp( buf, "isfollow" ) )
	    return ( chkchar->master
		    && chkchar->master->in_room == chkchar->in_room );

        // Looks like OLC does not support multiple bitvectors - Veygoth
	if ( !str_cmp( buf, "isaffected" ) )
	    return ( IS_SET( chkchar->affected_by[0], atoi( arg ) ) );

	if ( !str_cmp( buf, "hitprcnt" ) )
	{
	    lhsvl = chkchar->hit / get_max_hit( chkchar );
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "inroom" ) )
	{
	    lhsvl = chkchar->in_room->vnum;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "sex" ) )
	{
	    lhsvl = chkchar->sex;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "position" ) )
	{
	    lhsvl = chkchar->position;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "level" ) )
	{
	    lhsvl = get_trust( chkchar );
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "class" ) )
	{
	    lhsvl = chkchar->class;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "goldamt" ) )
	{
	    lhsvl = chkchar->money.gold;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "number" ) )
	{
	    lhsvl = chkchar->pIndexData->vnum;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "name" ) )
	    return prog_seval( chkchar->name, opr, val );
    }

    if ( chkobj
	&& !chkobj->deleted )
    {
	if ( !str_cmp( buf, "objtype" ) )
	{
	    lhsvl = chkobj->item_type;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "objval0" ) )
	{
	    lhsvl = chkobj->value[0];
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "objval1" ) )
	{
	    lhsvl = chkobj->value[1];
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "objval2" ) )
	{
	    lhsvl = chkobj->value[2];
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "objval3" ) )
	{
	    lhsvl = chkobj->value[3];
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "objval4" ) )
	{
	    lhsvl = chkobj->value[4];
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "number" ) )
	{
	    lhsvl = chkobj->pIndexData->vnum;
	    rhsvl = atoi( val );
	    return prog_veval( lhsvl, opr, rhsvl );
	}

	if ( !str_cmp( buf, "name" ) )
	    return prog_seval( chkobj->name, opr, val );
    }

    /*
     * Ok... all the ifchcks are done, so if we didn't find ours then something
     * odd happened.  So report the bug and abort the MOBprogram (return error)
     */
    bug( "Mob: %d unknown ifchck", mob->pIndexData->vnum );
    return FALSE;
}


/*
 * Quite a long and arduous function, this guy handles the control
 * flow part of MOBprograms.  Basicially once the driver sees an
 * 'if' attention shifts to here.  While many syntax errors are
 * caught, some will still get through due to the handling of break
 * and errors in the same fashion.  The desire to break out of the
 * recursion without catastrophe in the event of a mis-parse was
 * believed to be high. Thus, if an error is found, it is bugged and
 * the parser acts as though a break were issued and just bails out
 * at that point. I haven't tested all the possibilites, so I'm speaking
 * in theory, but it is 'guaranteed' to work on syntactically correct
 * MOBprograms, so if the mud crashes here, check the mob carefully!
 */
static char null[1];

char *prog_process_if( char *ifchck, char *com_list, CHAR_DATA * mob,
		       CHAR_DATA * actor, OBJ_DATA * obj, void *vo,
		       CHAR_DATA * rndm )
{
    char buf [ MAX_INPUT_LENGTH ];
    char *morebuf = '\0';
    char *cmnd = '\0';
    bool loopdone = FALSE;
    bool flag = FALSE;
    int legal;

    last_fun( "prog_process_if");

    *null = '\0';

    /*
     * check for trueness of the ifcheck 
     */
    if ( ( legal = prog_do_ifchck( ifchck, mob, actor, obj, vo, rndm ) ) )
    {
	if ( legal == 1 )
	    flag = TRUE;
	else
	    return null;
    }

    while ( !loopdone )		/*
				 * scan over any existing or statements 
				 */
    {
	cmnd = com_list;
	com_list = prog_next_command( com_list );
	while ( *cmnd == ' ' )
	    cmnd++;
	if ( *cmnd == '\0' )
	{
	    bug( "Mob: %d no commands after IF/OR", mob->pIndexData->vnum );
	    return null;
	}
	morebuf = one_argument( cmnd, buf );
	if ( !str_cmp( buf, "or" ) )
	{
	    if ( ( legal = prog_do_ifchck( morebuf, mob, actor, obj, vo,
	        rndm ) ) )
            {
		if ( legal == 1 )
		    flag = TRUE;
		else
		    return null;
            }
	}
	else
	    loopdone = TRUE;
    }

    if ( flag )
	for ( ; ; )		/*
				 * ifcheck was true, do commands but ignore else
				 * to endif
				 */
	{
	    if ( !str_cmp( buf, "if" ) )
	    {
		com_list = prog_process_if( morebuf, com_list, mob, actor, obj,
		    vo, rndm );
		while ( *cmnd == ' ' )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd = com_list;
		com_list = prog_next_command( com_list );
		morebuf = one_argument( cmnd, buf );
		continue;
	    }
	    if ( !str_cmp( buf, "break" ) )
		return null;
	    if ( !str_cmp( buf, "endif" ) )
		return com_list;
	    if ( !str_cmp( buf, "else" ) )
	    {
		while ( str_cmp( buf, "endif" ) )
		{
		    cmnd = com_list;
		    com_list = prog_next_command( com_list );
		    while ( *cmnd == ' ' )
			cmnd++;
		    if ( *cmnd == '\0' )
		    {
			bug( "Mob: %d missing endif after else",
			    mob->pIndexData->vnum );
			return null;
		    }
		    morebuf = one_argument( cmnd, buf );
		}
		return com_list;
	    }
	    prog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
	    cmnd = com_list;
	    com_list = prog_next_command( com_list );
	    while ( *cmnd == ' ' )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		bug( "Mob: %d missing else or endif", mob->pIndexData->vnum );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	}
    else
	/*
	 * false ifcheck, find else and do existing commands or quit at endif
	 */
    {
	while ( ( str_cmp( buf, "else" ) ) && ( str_cmp( buf, "endif" ) ) )
	{
	    cmnd = com_list;
	    com_list = prog_next_command( com_list );
	    while ( *cmnd == ' ' )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		bug( "Mob: %d missing an else or endif",
		    mob->pIndexData->vnum );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	}

	/*
	 * found either an else or an endif.. act accordingly 
	 */
	if ( !str_cmp( buf, "endif" ) )
	    return com_list;
	cmnd = com_list;
	com_list = prog_next_command( com_list );
	while ( *cmnd == ' ' )
	    cmnd++;
	if ( *cmnd == '\0' )
	{
	    bug( "Mob: %d missing endif", mob->pIndexData->vnum );
	    return null;
	}
	morebuf = one_argument( cmnd, buf );

	for ( ; ; )		/*
				 * process the post-else commands until an endif is found.
				 */
	{
	    if ( !str_cmp( buf, "if" ) )
	    {
		com_list = prog_process_if( morebuf, com_list, mob, actor,
					    obj, vo, rndm );
		while ( *cmnd == ' ' )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd = com_list;
		com_list = prog_next_command( com_list );
		morebuf = one_argument( cmnd, buf );
		continue;
	    }
	    if ( !str_cmp( buf, "else" ) )
	    {
		bug( "Mob: %d found else in an else section",
		    mob->pIndexData->vnum );
		return null;
	    }
	    if ( !str_cmp( buf, "break" ) )
		return null;
	    if ( !str_cmp( buf, "endif" ) )
		return com_list;
	    prog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
	    cmnd = com_list;
	    com_list = prog_next_command( com_list );
	    while ( *cmnd == ' ' )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		bug( "Mob:%d missing endif in else section",
		    mob->pIndexData->vnum );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	}
    }

}


/*
 * This routine handles the variables for command expansion.
 * If you want to add any go right ahead, it should be fairly
 * clear how it is done and they are quite easy to do, so you
 * can be as creative as you want. The only catch is to check
 * that your variables exist before you use them. At the moment,
 * using $t when the secondary target refers to an object 
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>
 * probably makes the mud crash (vice versa as well) The cure
 * would be to change act() so that vo becomes vict & v_obj.
 * but this would require a lot of small changes all over the code.
 */
void prog_translate( char ch, char *t, CHAR_DATA * mob, CHAR_DATA * actor,
		     OBJ_DATA * obj, void *vo, CHAR_DATA * rndm )
{
    static char *he_she [ ] =
    { "it", "he", "she" };
    static char *him_her [ ] =
    { "it", "him", "her" };
    static char *his_her [ ] =
    { "its", "his", "her" };
    CHAR_DATA *vict = (CHAR_DATA *) vo;
    OBJ_DATA *v_obj = (OBJ_DATA *) vo;

    last_fun( "prog_translate");

    /* Just a little safety check - Zen */
    if ( !mob || mob->deleted )
	return;

    *t = '\0';
    switch ( ch )
    {
	case 'i':
	    one_argument( mob->name, t );
	    break;

	case 'I':
	    strcpy( t, mob->short_descr );
	    break;

	case 'n':
	    if ( !actor || actor->deleted )
		break;
	    if ( can_see( mob, actor ) )
		one_argument( actor->name, t );
	    if ( !IS_NPC( actor ) )
		*t = UPPER(*t);
	    break;

	case 'N':
	    if ( !actor || actor->deleted )
		break;
	    if ( can_see( mob, actor ) )
	    {
		if ( IS_NPC( actor ) )
		    strcpy( t, actor->short_descr );
		else
		{
		    strcpy( t, actor->name );
		    strcat( t, " " );
		    strcat( t, actor->pcdata->title );
		}
	    }
	    else
		strcpy( t, "someone" );
	    break;

	case 't':
	    if ( !vict || vict->deleted )
		break;
	    if ( can_see( mob, vict ) )
		one_argument( vict->name, t );
	    if ( !IS_NPC( vict ) )
		*t = UPPER( *t );
	    break;

	case 'T':
	    if ( !vict || vict->deleted )
		break;
	    if ( can_see( mob, vict ) )
	    {
		if ( IS_NPC( vict ) )
		    strcpy( t, vict->short_descr );
		else
		{
		    strcpy( t, vict->name );
		    strcat( t, " " );
		    strcat( t, vict->pcdata->title );
		}
	    }
	    else
		strcpy( t, "someone" );
	    break;

	case 'r':
	    if ( !rndm || rndm->deleted )
		break;
	    if ( can_see( mob, rndm ) )
		one_argument( rndm->name, t );	    
	    if ( !IS_NPC( rndm ) )
		*t = UPPER( *t );
	    break;

	case 'R':
	    if ( !rndm || rndm->deleted )
		break;
	    if ( can_see( mob, rndm ) )
	    {
		if ( IS_NPC( rndm ) )
		    strcpy( t, rndm->short_descr );
		else
		{
		    strcpy( t, rndm->name );
		    strcat( t, " " );
		    strcat( t, rndm->pcdata->title );
		}
	    }
	    else
		strcpy( t, "someone" );
	    break;

	case 'e':
	    if ( actor && !actor->deleted )
		can_see( mob, actor ) ? strcpy( t, he_she[actor->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 'm':
	    if ( actor && !actor->deleted )
		can_see( mob, actor ) ? strcpy( t, him_her[actor->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 's':
	    if ( actor && !actor->deleted )
		can_see( mob, actor ) ? strcpy( t, his_her[actor->sex] )
		    : strcpy( t, "someone's" );
	    break;

	case 'E':
	    if ( vict && !vict->deleted )
		can_see( mob, vict ) ? strcpy( t, he_she[vict->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 'M':
	    if ( vict && !vict->deleted )
		can_see( mob, vict ) ? strcpy( t, him_her[vict->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 'S':
	    if ( vict && !vict->deleted )
		can_see( mob, vict ) ? strcpy( t, his_her[vict->sex] )
		    : strcpy( t, "someone's" );
	    break;

	case 'j':
	    strcpy( t, he_she[mob->sex] );
	    break;

	case 'k':
	    strcpy( t, him_her[mob->sex] );
	    break;

	case 'l':
	    strcpy( t, his_her[mob->sex] );
	    break;

	case 'J':
	    if ( rndm && !rndm->deleted )
		can_see( mob, rndm ) ? strcpy( t, he_she[rndm->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 'K':
	    if ( rndm && !rndm->deleted )
		can_see( mob, rndm ) ? strcpy( t, him_her[rndm->sex] )
		    : strcpy( t, "someone" );
	    break;

	case 'L':
	    if ( rndm && !rndm->deleted )
		can_see( mob, rndm ) ? strcpy( t, his_her[rndm->sex] )
		    : strcpy( t, "someone's" );
	    break;

	case 'o':
	    if ( obj && !obj->deleted )
		can_see_obj( mob, obj ) ? one_argument( obj->name, t )
		    : strcpy( t, "something" );
	    break;

	case 'O':
	    if ( obj && !obj->deleted )
		can_see_obj( mob, obj ) ? strcpy( t, obj->short_descr )
		    : strcpy( t, "something" );
	    break;

	case 'p':
	    if ( v_obj && !v_obj->deleted )
		can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t )
		    : strcpy( t, "something" );
	    break;

	case 'P':
	    if ( v_obj && !v_obj->deleted )
		can_see_obj( mob, v_obj ) ? strcpy( t, v_obj->short_descr )
		    : strcpy( t, "something" );
	    break;

	case 'a':
	    if ( obj && !obj->deleted )
		switch (*(obj->name))
		{
		    case 'a':
		    case 'e':
		    case 'i':
		    case 'o':
		    case 'u':
			strcpy( t, "an" );
			break;
		    default:
			strcpy( t, "a" );
		}
	    break;

	case 'A':
	    if ( v_obj && !v_obj->deleted )
		switch (*(v_obj->name))
		{
		    case 'a':
		    case 'e':
		    case 'i':
		    case 'o':
		    case 'u':
			strcpy( t, "an" );
			break;
		    default:
			strcpy( t, "a" );
		}
	    break;

	case '$':
	    strcpy( t, "$" );
	    break;

	default:
	    bug( "Mob: %d bad $var", mob->pIndexData->vnum );
	    break;
    }

    return;
}


/*
 * This procedure simply copies the cmnd to a buffer while expanding
 * any variables by calling the translate procedure.  The observant
 * code scrutinizer will notice that this is taken from act()
 */
void prog_process_cmnd( char *cmnd, CHAR_DATA * mob, CHAR_DATA * actor,
			OBJ_DATA * obj, void *vo, CHAR_DATA * rndm )
{
    char buf [ MAX_INPUT_LENGTH ];
    char tmp [ MAX_INPUT_LENGTH ];
    char *str;
    char *i;
    char *point;

    last_fun( "prog_process_cmnd");

    point = buf;
    str = cmnd;

    while ( *str != '\0' )
    {
	if ( *str != '$' )
	{
	    *point++ = *str++;
	    continue;
	}
	str++;
	prog_translate( *str, tmp, mob, actor, obj, vo, rndm );
	i = tmp;
	++str;
	while ( ( *point = *i ) != '\0' )
	    ++point, ++i;
    }
    *point = '\0';
    interpret( mob, buf );

    return;
}


/*
 * The main focus of the MOBprograms.  This routine is called 
 * whenever a trigger is successful.  It is responsible for parsing
 * the command list and figuring out what to do. However, like all
 * complex procedures, everything is farmed out to the other guys.
 */
void prog_driver( char *com_list, CHAR_DATA * mob, CHAR_DATA * actor,
		  OBJ_DATA * obj, void *vo )
{
    char tmpcmndlst [ MAX_STRING_LENGTH ];
    char buf [ MAX_INPUT_LENGTH ];
    char *morebuf;
    char *command_list;
    char *cmnd;
    CHAR_DATA *rndm = NULL;
    CHAR_DATA *vch = NULL;
    int count = 0;

    last_fun( "prog_driver");

    if ( is_affected( mob, AFF_CHARM ) )
	return;

    /*
     * get a random visable mortal player who is in the room with the mob 
     */
    for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
	if ( !IS_NPC( vch )
	    && !vch->deleted
	    && vch->level < LEVEL_IMMORTAL
	    && can_see( mob, vch ) )
	{
	    if ( number_range( 0, count ) == 0 )
		rndm = vch;
	    count++;
	}

    strcpy( tmpcmndlst, com_list );
    command_list = tmpcmndlst;
    cmnd = command_list;
    command_list = prog_next_command( command_list );
    while ( *cmnd != '\0' )
    {
	morebuf = one_argument( cmnd, buf );
	if ( !str_cmp( buf, "if" ) )
	    command_list = prog_process_if( morebuf, command_list, mob,
					    actor, obj, vo, rndm );
	else
	    prog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
	cmnd = command_list;
	command_list = prog_next_command( command_list );
    }

    return;
}


/*
 * Global function code and brief comments.
 */

/*
 * The next two routines are the basic trigger types. Either trigger
 * on a certain percent, or trigger on a keyword or word phrase.
 * To see how this works, look at the various trigger routines..
 */
void prog_wordlist_check( char *arg, CHAR_DATA * mob, CHAR_DATA * actor,
			  OBJ_DATA * obj, void *vo, int type )
{
    MPROG_DATA   *mprg;
    char         *list;
    char         *start;
    char         *dupl;
    char         *end;
    char          temp1 [ MAX_STRING_LENGTH ];
    char          temp2 [ MAX_INPUT_LENGTH ];
    char          word  [ MAX_INPUT_LENGTH ];
    unsigned int  i;

    last_fun( "prog_wordlist_check");

    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	if ( mprg->type & type )
	{
	    strcpy( temp1, mprg->arglist );
	    list = temp1;
	    for ( i = 0; i < strlen( list ); i++ )
		list[i] = LOWER( list[i] );

	    /*
	     * BUGFIX: Copy AT MOST sizeof( temp2) characters.  Walker 
	     */
	    strncpy( temp2, arg, sizeof( temp2 ) );
	    /*
	     * Add a NULL char at end of temp2.  Walker 
	     */
	    temp2[ MAX_INPUT_LENGTH - 1 ] = '\0';

	    dupl = temp2;
	    for ( i = 0; i < strlen( dupl ); i++ )
		dupl[i] = LOWER( dupl[i] );
	    if ( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
	    {
		list += 2;
		while ( ( start = strstr( dupl, list ) ) )
		    if ( ( start == dupl || *(start - 1) == ' ' )
			&& ( *(end = start + strlen(list)) == ' '
			    || *end == '\n'
			    || *end == '\r'
			    || *end == 0 ) )
		    {
			prog_driver( mprg->comlist, mob, actor, obj, vo );
			break;
		    }
		    else
			dupl = start + 1;
	    }
	    else
	    {
		list = one_argument( list, word );
		for ( ; word[0] != '\0'; list = one_argument( list, word ) )
		    while ( ( start = strstr( dupl, word ) ) )
			if ( ( start == dupl || *(start - 1) == ' ' )
			    && ( *(end = start + strlen( word )) == ' '
				|| *end == '\n'
				|| *end == '\r'
				|| *end == 0 ) )
			{
			    prog_driver( mprg->comlist, mob, actor, obj, vo );
			    break;
			}
			else
			    dupl = start + 1;
	    }
	}

    return;
}


void prog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj,
			 void *vo, int type )
{
    MPROG_DATA *mprg;

    last_fun( "prog_percent_check");

    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	if ( (mprg->type & type )
	    && ( number_percent() < atoi( mprg->arglist ) ) )
	{
	    prog_driver( mprg->comlist, mob, actor, obj, vo );
	    if ( type != GREET_PROG && type != ALL_GREET_PROG )
		break;
	}

    return;
}


void mob_act_add( CHAR_DATA *mob )
{
    ACT_PROG_DATA * runner;
  
    last_fun( "mob_act_add");

    for ( runner = mob_act_list; runner; runner = runner->next )
	if ( runner->vo == mob )
	    return;

    runner		= alloc_mem( sizeof( ACT_PROG_DATA ) );
    runner->vo		= mob;
    runner->next	= mob_act_list;
    mob_act_list	= runner;
}


/*
 * The triggers.. These are really basic, and since most appear only
 * once in the code (hmm. i think they all do) it would be more efficient
 * to substitute the code in and make the mprog_xxx_check routines global.
 * However, they are all here in one nice place at the moment to make it
 * easier to see what they look like. If you do substitute them back in,
 * make sure you remember to modify the variable names to the ones in the
 * trigger calls.
 */
void prog_act_trigger( char *buf, CHAR_DATA * mob, CHAR_DATA * ch,
		       OBJ_DATA * obj, void *vo )
{
    MPROG_ACT_LIST *tmp_act;

    last_fun( "prog_act_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, ACT_PROG ) )
    {
        /* 
         * Don't let a mob trigger itself, nor one instance of a mob
         * trigger another instance.
         */
	if ( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
	    return;
	tmp_act = alloc_mem( sizeof( MPROG_ACT_LIST ) );
	if ( mob->mpactnum > 0 )
	    tmp_act->next = mob->mpact->next;
	else
	    tmp_act->next = NULL;

	mob->mpact = tmp_act;
	mob->mpact->buf = str_dup( buf );
	mob->mpact->ch = ch;
	mob->mpact->obj = obj;
	mob->mpact->vo = vo;
	mob->mpactnum++;
	mob_act_add( mob );
    }

    return;
}

void prog_bribe_trigger( CHAR_DATA * mob, CHAR_DATA * ch, int amount )
{
    char buf [ MAX_STRING_LENGTH ];
    MPROG_DATA *mprg;
    OBJ_DATA *obj;

    last_fun( "prog_bribe_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, BRIBE_PROG ) )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
	sprintf( buf, obj->short_descr, amount );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	obj->value[0] = amount;
	obj_to_char( obj, mob );
	spend_cash( mob, amount );

	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL;
	     mprg = mprg->next )

	    if ( IS_SET( mprg->type, BRIBE_PROG )
		&& ( amount >= atoi( mprg->arglist ) ) )
	    {
		prog_driver( mprg->comlist, mob, ch, obj, NULL );
		break;
	    }
    }

    return;
}

void prog_death_trigger( CHAR_DATA * mob )
{
    last_fun( "prog_death_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, DEATH_PROG ) )
    {
	prog_percent_check( mob, NULL, NULL, NULL, DEATH_PROG );
    }

    death_cry( mob );
    return;
}

void prog_entry_trigger( CHAR_DATA * mob )
{
    last_fun( "prog_entry_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, ENTRY_PROG ) )
	prog_percent_check( mob, NULL, NULL, NULL, ENTRY_PROG );

    return;
}

void prog_fight_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
    last_fun( "prog_fight_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, FIGHT_PROG ) )
	prog_percent_check( mob, ch, NULL, NULL, FIGHT_PROG );

    return;
}

void prog_give_trigger( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj )
{
    char buf [ MAX_INPUT_LENGTH ];
    MPROG_DATA *mprg;

    last_fun( "prog_give_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, GIVE_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );
	    if ( IS_SET( mprg->type, GIVE_PROG )
		&& ( !str_cmp( obj->name, mprg->arglist )
		    || !str_cmp( "all", buf ) ) )
	    {
		prog_driver( mprg->comlist, mob, ch, obj, NULL );
		break;
	    }
	}

    return;
}

void prog_greet_trigger( CHAR_DATA * ch )
{
    CHAR_DATA *vmob;

    last_fun( "prog_greet_trigger");

    if ( ch->deleted
	|| !ch->in_room )
	return;

    for ( vmob = ch->in_room->people; vmob; vmob = vmob->next_in_room )
    {
	if ( vmob->deleted )
	    continue;

	if ( !IS_NPC( vmob )
	    || is_affected( vmob, AFF_CHARM )
	    || !IS_AWAKE( vmob )
	    || vmob->fighting )
	    continue;

	if ( ch != vmob
	    && can_see( vmob, ch )
	    && IS_SET( vmob->pIndexData->progtypes, GREET_PROG ) )
	    prog_percent_check( vmob, ch, NULL, NULL, GREET_PROG );
	else if ( IS_SET( vmob->pIndexData->progtypes, ALL_GREET_PROG ) )
	    prog_percent_check( vmob, ch, NULL, NULL, ALL_GREET_PROG );

    }
    return;

}

void prog_hitprcnt_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
    MPROG_DATA *mprg;

    last_fun( "prog_hitprcnt_trigger");

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, HITPRCNT_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
	    if ( IS_SET( mprg->type, HITPRCNT_PROG )
	      && ( ( 100 * mob->hit / get_max_hit( mob ) ) < atoi( mprg->arglist ) ) )
	    {
		prog_driver( mprg->comlist, mob, ch, NULL, NULL );
		break;
	    }

    return;
}

void prog_random_trigger( CHAR_DATA * mob )
{
    last_fun( "prog_random_trigger");

    if ( IS_SET( mob->pIndexData->progtypes, RAND_PROG ) )
	prog_percent_check( mob, NULL, NULL, NULL, RAND_PROG );

    return;
}

void prog_speech_trigger( char *txt, CHAR_DATA * mob )
{
    CHAR_DATA *vmob;

    last_fun( "prog_speech_trigger" );

    if ( mob->deleted
	|| !mob->in_room )
	return;

    for ( vmob = mob->in_room->people; vmob; vmob = vmob->next_in_room )
    {
	if ( vmob->deleted )
	    continue;

	if ( !IS_NPC( vmob )
	    || is_affected( vmob, AFF_CHARM )
	    || vmob->fighting )
	    continue;

	if ( mob != vmob
	    && IS_SET( vmob->pIndexData->progtypes, SPEECH_PROG ) )
	    prog_wordlist_check( txt, vmob, mob, NULL, NULL, SPEECH_PROG );

    }

    return;
}
