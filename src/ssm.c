/******************************************************************************
 *  SSM v2.2 (shared string manager)                                          *
 *                                                                            *
 *  Copyright(C) 1996 Melvin Smith (Fusion) for EnvyMUD 2.2                   *
 *                                                                            *
 *  Due to alignment differences on 32 bit and 64 bit machines, memory        *
 *  usage is now virtually identical to standard Merc on 32-bit               *
 *  architecture, but slightly larger on 64-bit. Memory usage is still        *
 *  smaller than SSM 2.0 or earlier. The manager now uses short ints for      *
 *  the count and size of chunks, so to increase MAX_STRING you must          *
 *  increase MAX_CHUNKS instead. String have a max reference count of         *
 *  +32766 and max size of CHUNK_SIZE (0xfff0). Fragmentation is also         *
 *  handled more efficiently by marking failed chunks with -1 to temporarily  *
 *  disable them until a defrag_heap() recycles them. This helps when a       *
 *  4 byte chunk is freed low in the heap, so string_dup() doesn't walk       *
 *  the whole heap every time.                                                *
 *                                                                            *
 *  <msmith@falcon.mercer.peachnet.edu>                                       *
 *                                                                            *
 *  ROM2.4 modifications by Tom Adriaenssen (Jan 1996) -- Wreck               *
 *                                                                            *
 *  <tadriaen@zorro.ruca.ua.ac.be>                                            *
 *                                                                            *
 *  Removed ROM2.4 modifications as Envy doesnt need *fread_string_eol - Kahn *
 *                                                                            *
 *  Included a more accurate hashing function by Erwin S.A. -- Zen            *
 *                                                                            *
 *****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined( ultrix )
#include <memory.h>
#endif

#define intType        short int
#define uintType       unsigned intType
#define intTypeSize  ( sizeof( intType ) )
#define addrType       void * 
#define addrTypeSize ( sizeof( addrType ) )
#define addrSizeMask ( sizeof( addrType ) - 1 )

#if !defined( macintosh )
extern  int     _filbuf         args( (FILE *) );
#endif

typedef struct BE BufEntry;

struct BE
{
    BufEntry *next;
    uintType  size;   /* size of the chunk (regardless of NULL CHAR) */ 
    intType   usage;  /* how many pointers to the string */ 
    char      buf[1]; /* chunk starts here */
};

/*
 * This is for the temporary hashing of strings at bootup to speedup
 * comparison/crunching of the string space. The temp_string_hash will
 * be freed after boot_done() is called.
 */
typedef struct TH TempHash;

struct TH
{
    TempHash *next;
    char     *str;
};

TempHash **temp_string_hash; 

/* These are the original Merc vars in db.c */
extern bool  fBootDb;
char         str_empty[1];
char        *string_space;
char        *top_string;
long         nAllocString;
long         sAllocString;
long         nOverFlowString;
long         sOverFlowString;

int          numFree;
bool         Full;

char *		str_dup		args( ( const char * ) );
void		free_string	args( ( char * ) );
char *		fread_string	args( ( FILE *, int * ) );
void		temp_hash_add	args( ( char *, int ) );
char *		temp_hash_find	args( ( const char *, int ) );
static unsigned	get_string_hash	args( ( register const char *, int ) );

/*
 * ssm_buf_head points to start of shared space,
 * ssm_buf_free points to next free block
 */ 
BufEntry *ssm_buf_head, *ssm_buf_free;

/* To allocate more memory increase MAX_CHUNKS in merc.h. */
#define CHUNK_SIZE   0xfff0                  /* DO NOT mess with this! */ 
long    MAX_STRING = MAX_CHUNKS * CHUNK_SIZE;
int     HEADER_SIZE;

/*
 * Not sure what is a good value for MAX_FREE 
 * If a dup fails str_dup will not defrag unless the number
 * of numFree >= MAX_FREE. numFree is NOT the current number of free blocks,
 * it is just a counter so defrag doesnt start dragging the game in the
 * case of a lot of failed dups.
 */
#define MAX_FREE     1000

void init_string_space()
{
    BufEntry *walk;
    int       i;

    last_fun( "init_string_space");
    string_space = (char *)malloc( MAX_STRING );
    if( !string_space )
    {
        bug( "[SSM] Cant allocate shared string space.", 0 );
        exit(1);
    }

    top_string   = string_space + MAX_STRING-1;
    ssm_buf_head = (BufEntry *)string_space;
    HEADER_SIZE  = (int)( (char*)&ssm_buf_head->buf[0] - (char*)ssm_buf_head );

    walk = ssm_buf_head;
    for( i = 0; ;i++ )
    {
	walk->usage = 0;
	walk->size  = CHUNK_SIZE - HEADER_SIZE;
	if( i < MAX_CHUNKS - 1 )
	{
	    walk->next = (BufEntry *)( (char*)walk + CHUNK_SIZE );
	    walk       = walk->next;
	    continue;
	}

	walk->next = 0;
	break;
    }

    ssm_buf_free        = ssm_buf_head;
    temp_string_hash    = (TempHash **)calloc( sizeof(TempHash *),
					      MAX_KEY_HASH );
}

int defrag_heap()
{
    /*
     * Walk through the shared heap and merge adjacent free blocks.
     * Free blocks are merged in str_free if free->next is free but
     * if the block preceding free is free, it stays unmerged. I would
     * rather not have the heap as a DOUBLE linked list for 2 reasons...
     *  (1) Extra 4 bytes per struct uses more mem
     *  (2) Speed - don't want to bog down str_ functions with heap management
     * The "orphaned" blocks will eventually be either merged or reused.
     * The str_dup function will call defrag if it cant allocate a buf.
     */

    BufEntry *walk, *last_free, *next;
    int merges   = 0;
    ssm_buf_free = 0;

    last_fun( "defrag_heap");

    for( walk=ssm_buf_head,last_free=0; walk; walk = next )
      {
	next = walk->next;
	if( walk->usage > 0 )
	{
	    /* this block is in use so set last_free to NULL */
	    last_free = 0;
	    continue;
	}
	else if( !last_free )
	{
	    /* OK found a NEW free block, set last_free and move to next */
	    last_free = walk;
	    if( !ssm_buf_free )
	        ssm_buf_free = walk; 

	    continue; 
	}
	else
	{
	    /* previous block free so merge walk into last_free and move on */
	    if( (long)last_free->size + (long)walk->size <= CHUNK_SIZE )
	    {
		merges++;
		last_free->size += walk->size + HEADER_SIZE;
		last_free->next  = walk->next;
		last_free->usage = 0;
	    }
	    else
	        last_free = walk;
	}
    }

    if( merges )
        bug( "[SSM] defrag_heap : made %d block merges.", merges );
    else
        bug( "[SSM] defrag_heap : resulted in 0 merges.", 0 ); 

    /* Start count over again */ 
    numFree = 0;
    return merges;
}


/*
 * Dup a string into shared space. If string exists, the usage count
 * gets incremented and the reference is returned. If the string does
 * not exist in heap, space is allocated and usage is 1.
 * This is a linked list first fit algorithm, so strings can be
 * freed. Upon bootup, there is a seperate hash table constructed in order
 * to do crunching, then the table is destroyed.
 */
char *str_dup( const char *str )
{
    BufEntry *ptr;
    char     *str_new;
    int       len;
    int       rlen;

    if( !str || !*str )
        return &str_empty[0];

    // See if we've already got the string we're looking for
    // if so, return a pointer to that string.
    if( str > string_space && str < top_string )
    {
	ptr = (BufEntry *)( str - HEADER_SIZE );
	if( ptr->usage <= 0 )
	{
	    bug( "str_dup : invalid str", 0 );
	    bug( str, 0 );
	}

	ptr->usage++;
	return (char *)str;
    }

    // Rlen, for required length - we need an extra char for the '\0'.
    rlen = len = (int)strlen( str ) + 1;

    // Round up to machine dependant address size.
    // Don't remove this, because when the BufEntry struct is overlaid
    // the struct must be aligned correctly.
    if( ( len + HEADER_SIZE ) & addrSizeMask ) 
        len += addrTypeSize - ( ( len + HEADER_SIZE ) & addrSizeMask );

    // Do we have room in the shared string buffer?
    if( ssm_buf_free )
    { 
        RETRY:
	for( ptr = ssm_buf_free; ptr; ptr = ptr->next )
	    if( ptr->usage == 0 && ptr->size >= len )
	        break;

        // If we didn't find a large enough hunk available in the open space,
        // try to call defrag_heap() to compress the space in the hopes we'll
        // have room for our string once we're compressed things.
	if( !ptr )
	{
	    if( numFree >= MAX_FREE )
	    {
		int merges;

		bug( "[SSM] Attempting to optimize shared string heap.", 0 );
		merges = defrag_heap();

		// goto is fine because defrag will return 0 next time
		if( merges )
		    goto RETRY;
	    }	

            // I guess it's just too large for our heap.  It gets to sleep in the yard.
	    str_new = (char *)malloc( rlen );
	    strcpy( str_new, str ); 
	    sOverFlowString += rlen;
	    nOverFlowString++;
	    return str_new;
	}
	// We found a place to put our string.
        // If there is at least header size excess break it up
	else if( ptr->size - len >= HEADER_SIZE ) 
	{
	    BufEntry *temp;

	    // WARNING! - DONT REMOVE THE CASTS BELOW! - Fusion
	    temp = (BufEntry*)( (char *)ptr + HEADER_SIZE + len );
	    temp->size   = ptr->size - ( len + HEADER_SIZE );
	    temp->next   = ptr->next;
	    temp->usage  = 0;
	    ptr->size    = len;
	    ptr->next    = temp;
	    ptr->usage   = 1;
	    ssm_buf_free = temp;
	}
	else
	{
	    ptr->usage = 1;
	    if( ptr != ssm_buf_free )
	        ssm_buf_free->usage--; // buf_free was skipped

	    for( ssm_buf_free = ssm_buf_head; ssm_buf_free;
		ssm_buf_free = ssm_buf_free->next )
	        if( ssm_buf_free->usage == 0 )
		    break;
	}

	str_new = (char *)&ptr->buf[0];
	strcpy( str_new, str ); 
	nAllocString++;
	sAllocString += ptr->size + HEADER_SIZE;
    }
    else  
    {
	// A one time toggle just for bugging purposes
	if( !Full )
	{
	    bug( "[SSM] The shared string heap is full!", 0 );
	    Full = 1;
	}

        // And we let our string sleep in the yard.
	str_new = (char *)malloc( rlen );
	strcpy( str_new, str ); 
	sOverFlowString += rlen;
	nOverFlowString++;
    } 

    return str_new;
}

/*
 * If string is in shared space, decrement usage, if usage then is 0,
 * free the chunk and attempt to merge with next node. Other
 * strings are freed with standard free.
 * Never call free/delete externally on a shared string.
 */
void free_string( char *str )
{
    BufEntry *ptr;

    last_fun( "free_string");
    if( !str || str == &str_empty[0] )
        return;

    if( str > string_space && str < top_string )
    {
	ptr = (BufEntry *)( str - HEADER_SIZE );

	if( --ptr->usage > 0 )
	    return;
	else if( ptr->usage < 0 )
	{
	    bug( "SSM:free_string: multiple free or invalid string.", 0 );
	    bug( (char*)&ptr->buf[0], 0 );
	    return;
	}

	numFree++;
	sAllocString -= ( ptr->size + HEADER_SIZE );
	nAllocString--;

	if( ssm_buf_free > ptr )
	    ssm_buf_free = ptr;

	if( fBootDb )
	{
	    TempHash *ptr;
	    TempHash *walk;
	    int ihash = get_string_hash( str, strlen( str ) );

	    for( ptr = temp_string_hash[ ihash ]; ptr; ptr = ptr->next )
	    {
		if( ptr->str != str )
		    continue;
		else if( ptr == temp_string_hash[ ihash ] )
		    temp_string_hash[ ihash ] = ptr->next;
		else
		    for( walk = temp_string_hash[ ihash ]; walk;
			walk = walk->next )
		    {
			if( walk->next == ptr )
			{
			    walk->next = ptr->next;
			    break;
			}
		    }

		free( ptr );
		break; 
	    }
	}
	return;
    }

    sOverFlowString -= strlen( str ) + 1;
    nOverFlowString--;
    if ( sOverFlowString < 0 || nOverFlowString < 0 )
    {
	bug( "SSM:free_string: string free from outside SS space.", 0 );
	bug( str, 0 );
    }
    else
        free( str );
}

/*
 * Read and allocate space for a string from a file.
 * This replaces db.c fread_string
 * This is modified version of Furey's fread_string from Merc
 */
char *fread_string( FILE *fp, int *status )
{
    char  buf [ MAX_STRING_LENGTH*8 ];
    char *ptr = buf;
    char  c;
    int count;

    count = 0;

    *status = 0;

    do { c = getc( fp ); }
    while( isspace( c ) );

    if( ( *ptr++ = c ) == '~' )
        return &str_empty[0];

    for ( ;; )
    {
        count++;
        if( count > (MAX_STRING_LENGTH*8 - 2))
            log_string( "Error in fread_string: the MUD is about to crash because a string is too large." );
	switch ( *ptr = getc( fp ) )
	{
	    default:
	        ptr++;
		break;

	    case EOF:
		bug( "Fread_string: EOF", 0 );
		*status = 1;
		return NULL;
		break;

	    case '\n':
		ptr++;
		*ptr++ = '\r';
		break;

	    case '\r':
		break;

	    case '~':
		*ptr = '\0';
		if( fBootDb )
		{ 
		    int len = ptr - buf;
	
		    ptr = temp_hash_find( buf, len ); 
		    if( ptr )
			return str_dup( ptr );

		    ptr = str_dup( buf );
		    temp_hash_add( ptr, len );
		    return ptr;
		}

                if( strlen( buf ) > MAX_STRING_LENGTH*7 )
                {
                    log_string( "Warning: a string is nearing the maximum length allowed by the MUD.  The fread_string function may bomb out soon." );
                    log_string( "This is probably the news file.  Chop out the old news entries and see if this message disappears." );
                }

		return str_dup( buf );
	}
    }
}


/*
 * Read string into user supplied buffer.
 * Modified version of Furey's fread_string
 */
void temp_fread_string( FILE *fp, char *buf )
{
    char *ptr = buf;
    char  c;

    last_fun( "temp_fread_string");
    do { c = getc( fp ); }
    while ( isspace( c ) );

    if ( ( *ptr++ = c ) == '~' )
    {   
	*buf = '\0';
	return;
    }

    for ( ;; )
    {
	switch ( *ptr = getc( fp ) )
	{
	    default:
	        ptr++;
		break;

/*	    case EOF:
		bug( "Temp_fread_string: EOF", 0 );
		exit( 1 );
		break;
*/
            case EOF:
                bug( "Temp_fread_string: EOF", 0 );
                *buf = '\0';
                return;
	    case '\n':
		ptr++;
		*ptr++ = '\r';
		break;

	    case '\r':
		break;

	    case '~':
		*ptr = '\0';
		return;
	}
    }
}


/* Hashing function from erwin@pip.dknet.dk */
// "Included is a more accurate hash function for the temp_hash{find,add}
// functions.  It is based off a hash function that supposedly comes from
// perl."
// "The hash function calculates the hash based on the first 32 characters; I
// found that setting that value to 8 characters causes a 50% increase in the
// number of strcmps in temp_hash_find; setting it beyond 32 did not decrease
// it."
// "The hash function takes the string plus its length as a parameter:
// fread_string can figure out the length of the string so there is no reason
// to recalculate it."
// -- Erwin
static unsigned get_string_hash( register const char *key, int len )
{
    register int      i		= UMIN( len, 32 );
    register unsigned hash	= 0;

    while( i-- )
	hash = hash * 33U + *key++;

    return hash % MAX_KEY_HASH;
}

/* Lookup the string in the boot-time hash table. */
char *temp_hash_find( const char *str, int len )
{
    TempHash *ptr;
    int       ihash;

    if( !fBootDb || !*str )
	return 0;

    ihash = get_string_hash( str, len );

    for( ptr = temp_string_hash[ ihash ]; ptr; ptr = ptr->next )
    {
	if( *ptr->str != *str )
	    continue;
	else if( strcmp( ptr->str, str ) )
	    continue;
        else return ptr->str;
    }

    return 0;
}



/*
 * Add a reference in the temporary hash table.
 * String is still in the linked list structure but
 * reference is kept here for quick lookup at boot time;
 */
void temp_hash_add( char *str, int len )
{
    TempHash *add;
    int       ihash;

    last_fun( "temp_hash_add");

    if( !fBootDb || !*str || ( str <= string_space && str >= top_string ))
	return;

    ihash = get_string_hash( str, len );

    add = (TempHash *)malloc( sizeof( TempHash ) );
    add->next = temp_string_hash[ ihash ];
    temp_string_hash[ ihash ] = add;
    add->str = str;
}


/* Free the temp boot string hash table */
void boot_done( void )
{
    TempHash *ptr, *next;
    int       ihash;

    last_fun( "boot_done");
    for( ihash = 0; ihash < MAX_KEY_HASH; ihash++ )
    {
	for( ptr = temp_string_hash[ ihash ]; ptr; ptr = next )
	{
	    next = ptr->next;
	    free( ptr );
	}
    }

    free( temp_string_hash );
    temp_string_hash = 0; /* Bug check in case someone accesses later */
}
