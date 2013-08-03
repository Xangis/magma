/**************************************************************************
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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if   defined( WIN32 )
char version_str [] = "Magma Mud Windows 32 Bit Version";
/*
 * Provided by Mystro <http://www.cris.com/~Kendugas/mud.shtml>
 */
#elif defined( AmigaTCP )
char version_str [] = "Magma Mud AmiTCP Version";
/*
 * You must rename or delete the sc:sys/types.h, so the 
 * amitcp:netinclude/sys/types.h will be used instead.
 * Also include these assigns in your user-startup (After the SC assigns)
 *    assign lib: Amitcp:netlib add 
 *    assign include: Amitcp:netinclude add
 * If you haven't allready :)
 * Compilled with SasC 6.56 and AmiTCP 4.2
 * http://www.geocities.com/SiliconValley/1411
 * dkaupp@netcom.com (May be defunct soon)
 * or davek@megnet.net
 * 4-16-96
 */
#else
char version_str [] = "Magma Mud *NIX";
#endif

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#if defined( WIN32 )
#include <sys/timeb.h> /*for _ftime(), uses _timeb struct*/
#else
#include <sys/time.h>
#endif
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "merc.h"

// Welcome screens
extern char * help_greeting;
extern char * help_greeting_color;
// Race screens
extern char * help_races_color;
extern char * help_races_nocolor;

/*
 * Malloc debugging stuff.
 */
#if defined( sun )
#undef MALLOC_DEBUG
#endif

#if defined( MALLOC_DEBUG )
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute( atomic ) in signal.h,
 *   I dance around it.
 */
#if defined( apollo )
#define __attribute( x )
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
#include <signal.h>
#endif

#if defined( apollo )
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */
#if	defined( macintosh )
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined( unix ) || defined( AmigaTCP )
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#endif

#if	defined( WIN32 )
// If you get an error here, you probably don't have the platform SDK installed.
// Search http://www.microsoft.com/downloads for "platform SDK" if you need to 
// get the SDK for your version of Windows.
//
// The Windows XP platform SDK is no longer listed there.  You can find that here:
// http://www.microsoft.com/msdownload/platformsdk/sdkupdate/XPSP2FULLInstall.htm
//
// Once the Platform SDK is installed, you will need to add the paths to your
// include directory and library paths.
#include <winsock.h>
#include "telnet.h"
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#endif



/*
 * OS-dependent declarations.
 */
#if	defined( _AIX )
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if     defined( irix )
int	read		args( ( int fd, char *buf, int nbyte ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
int	close		args( ( int fd ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#endif

#if	defined( __hpux )
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
			       const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if     defined( interactive )
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined( __linux__ )
int	close		args( ( int fd ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if     defined( LINUX2 )
int     accept          args( ( int s, const struct sockaddr *addr,
			    int *addrlen ) );
int     bind            args( ( int s, const struct sockaddr *name,
			    int namelen ) );
#endif
//int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined( macintosh )
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined( isascii )
#define	isascii( c )		( ( c ) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined( MIPS_OS )
extern	int		errno;
#endif

#if	defined( NeXT )
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined( htons )
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined( ntohl )
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined( sequent )
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined( htons )
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined( ntohl )
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			       int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well
 */

#if defined( sun )
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
#if !defined( SYSV )
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#endif
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
#if defined( SYSV )
int     setsockopt      args( ( int s, int level, int optname,
			       const char *optval, int optlen ) );
#else
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined( ultrix )
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined( WIN32 )
void    gettimeofday    args( ( struct timeval *tp, void *tzp ) );
#endif


#if defined( sun )
int     system          args( ( const char *string ) );
#endif




/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    merc_down;		/* Shutdown                     */
bool		    wizlock;		/* Game is wizlocked		*/
int                 numlock = 0;        /* Game is numlocked at <level> */
char		    str_boot_time [ MAX_INPUT_LENGTH ];
time_t		    current_time;	/* Time of this pulse		*/
int		    num_descriptors;


/*
 * OS-dependent local functions.
 */
#if defined( macintosh )
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( u_short port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
void    do_auto_shutdown        args( ( void ) ); // Unix/Linux signal handling auto-shutdown
#endif

#if defined( unix )
void    init_signals            args( ( void ) ); // Unix/Linux signal handling functions
void    sig_handler             args( ( int sig ) );
#endif

/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				       bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( DESCRIPTOR_DATA *d ) );
void    flush_command           args( ( DESCRIPTOR_DATA *d ) );
char   *coloured		args( ( char *buffer, CHAR_DATA *ch) );
void    reroll_hitpoints        args( ( CHAR_DATA *ch ));

int main( int argc, char **argv )
{
    struct  timeval now_time;
    u_short port;

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined( MALLOC_DEBUG )
    malloc_debug( 2 );
#endif

    num_descriptors		= 0;

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined( macintosh )
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( !( fpReserve = fopen( NULL_FILE, "r" ) ) )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 4001;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

    /*
     * Run the game.
     */
#if defined( macintosh )
    boot_db( );
    log_string( "Magma Mud is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined( unix )
    init_signals();
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    control = init_socket( port );
    boot_db( );

    // Make sure mandatory screens have loaded.
    if( help_greeting_color == NULL )
    {
        bug( "Color version of welcome screen not found.  Please ensure that a help entry for '_greeting_color' exists.", 0 );
        help_greeting_color = "Color welcome screen unavailble.\n\r";
    }
    if( help_greeting == NULL )
    {
        bug( "Non-Color version of welcome screen not found.  Please ensure that a help entry for '_greeting_ascii' exists.", 0 );
        help_greeting = "Non-Color welcome screen unavailble.\n\r";
    }
    if( help_races_color == NULL )
    {
        bug( "Color version of welcome screen not found.  Please ensure that a help entry for '_races_color' exists.", 0 );
        help_races_color = "Color race selection screen unavailable.\n\r";
    }
    if( help_races_nocolor == NULL )
    {
        bug( "Non-Color version of welcome screen not found.  Please ensure that a help entry for '_races_nocolor' exists.", 0 );
        help_races_nocolor = "Non-Color race selection screen unavailable.\n\r";
    }

    sprintf( log_buf, "Magma Mud is ready to rock on port %d.", port );
    log_string( log_buf );
    game_loop_unix( control );
#if !defined( WIN32 )
    close( control );
#else
    closesocket( control );
    WSACleanup();
#endif
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );
    exit( 0 );
    return 0;
}



#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
int init_socket( u_short port )
{
    static struct sockaddr_in sa_zero;
           struct sockaddr_in sa;
                  int         x        = 1; 
                  int         fd;

#if !defined( WIN32 )
    system( "touch SHUTDOWN.TXT" );
    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }
#else
    WORD    wVersionRequested = MAKEWORD( 1, 1 );
    WSADATA wsaData;
    int err = WSAStartup( wVersionRequested, &wsaData ); 
    if ( err != 0 )
    {
	perror( "No useable WINSOCK.DLL" );
	exit( 1 );
    }

    if ( ( fd = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "Init_socket: socket" );
	exit( 1 );
    }
#endif

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof( x ) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
#if !defined( WIN32 )
	close( fd );
#else
	closesocket( fd );
#endif
	exit( 1 );
    }

#if defined( SO_DONTLINGER ) && !defined( SYSV )
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof( ld ) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
#if !defined( WIN32 )
	    close( fd );
#else
	    closesocket( fd );
#endif
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
#if !defined( WIN32 )
    sa.sin_family   = AF_INET;
#else
    sa.sin_family   = PF_INET;
#endif
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
    {
	perror( "Init_socket: bind" );
#if !defined( WIN32 )
	close( fd );
#else
	closesocket( fd );
#endif
	exit( 1 );
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
#if !defined( WIN32 )
	close( fd );
#else
	closesocket( fd );
#endif
	exit( 1 );
    }

#if !defined( WIN32 )
    system( "rm SHUTDOWN.TXT" );
#endif
    return fd;
}
#endif

// Init_signals - Unix/Linux signal handlers
#if defined( unix )
void init_signals()
{
        signal( SIGPIPE, SIG_IGN );
	signal( SIGBUS, sig_handler );
	signal( SIGTERM, sig_handler );
	signal( SIGABRT, sig_handler );
	signal( SIGSEGV, sig_handler );
        signal( SIGHUP, sig_handler );
        signal( SIGINT, sig_handler );
        signal( SIGQUIT, sig_handler );
        signal( SIGILL, sig_handler );
        signal( SIGFPE, sig_handler );
        signal( SIGTSTP, sig_handler );
        signal( SIGXCPU, sig_handler );
        signal( SIGXFSZ, sig_handler );
}

// Sig_handler - Unix/Linux signal handlers
void sig_handler( int sig )
{
        char buf[MAX_STRING_LENGTH];

        sprintf( buf, "Last user command : %s.", last_command );
        log_string( buf );
        print_last_coms( );
        print_last_funs( );

	switch( sig )
	{
	  case SIGBUS:
	    bug( "Sig handler SIGBUS.", 0 );
            do_auto_shutdown();
            break;
	  case SIGTERM:
	    bug( "Sig handler SIGTERM.", 0 );
            do_auto_shutdown();
            break;
	  case SIGILL:
	    bug( "Sig handler SIGILL.", 0 );
            do_auto_shutdown();
            break;
	  case SIGSEGV:
	    bug( "Sig handler SIGSEGV.", 0 );
            do_auto_shutdown();
            break;
	  case SIGFPE:
	    bug( "Sig handler SIGFPE.", 0 );
            do_auto_shutdown();
            break;
	  case SIGABRT:
	    bug( "Sig handler SIGABRT.", 0 );
            do_auto_shutdown();
            break;
	  case SIGXCPU:
	    bug( "Sig handler SIGXCPU.", 0 );
            do_auto_shutdown();
            break;
	  case SIGXFSZ:
	    bug( "Sig handler SIGXFSZ.", 0 );
            do_auto_shutdown();
            break;
          case SIGHUP:
            bug( "Signal handler recieved signal SIGHUP.  Ignoring.", 0 );
            return;
            break;
          case SIGTSTP:
            bug( "Signal handler recieved signal SIGTSTP.  Ignoring.", 0 );
            return;
            break;
          case SIGINT:
            bug( "Signal handler recieved signal SIGINT (probably from Ctrl+Break).  Shutting down.", 0 );
            do_auto_shutdown();
            break;
          case SIGQUIT:
            bug( "Signal handler recieved signal SIGQUIT.  Shutting down.", 0 );
            do_auto_shutdown();
            break;
          default:
            bug( "Signal handler recieved UNKNOWN signal.  Shutting down.", 0 );
            do_auto_shutdown();
            break;
	}
}

// Automatic shutdown stuff - quick and easy save of all characters
// prior to everything going down.
void do_auto_shutdown()
{
 	/*This allows for a shutdown without somebody in-game actually calling it. -Ferric*/
	extern bool merc_down;
	
	merc_down = TRUE;
	/* This is to write to the file. */ 
	//fprintf(fp,"Last Command: %s\n", last_command);
        exit(1);
	return;
} 
#endif

// This function is probably mighty out of date because the MUD has
// never been run on a mac or msdos system. - Veygoth
#if defined( macintosh )
void game_loop_mac_msdos( void )
{
    static        DESCRIPTOR_DATA dcon;
           struct timeval         last_time;
           struct timeval         now_time;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.character      = NULL;
    dcon.connected	= CON_GET_COLOR;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.showstr_head   = str_dup( "" );
    dcon.showstr_point  = 0;
    dcon.olc_editing	= NULL;
    dcon.str_editing	= NULL;
    dcon.next		= descriptor_list;
    descriptor_list	= &dcon;
    dcon.infirst	= NULL;
    dcon.inlast		= NULL;
    dcon.histsize	= 0;

    write_to_buffer( d, "Enter your terminal type (0 for ASCII, 1 for ANSI, 9 for Quick): ", 0 );

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( d->character )
	        d->character->timer = 0;
	    if ( !read_from_descriptor( d ) )
	    {
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
		continue;
	    }

	    if ( d->character && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    if ( d->flush_point )
	    {
		flush_command( d );
	    }
	    else
	    {
		read_from_buffer( d );
	    }

	    if ( d->incomm[0] != '\0' || d->flusher[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->flush_point )
		    intcomm = d->flusher;
		else
		    intcomm = d->incomm;

		     if ( d->showstr_point )
			show_string( d, d->incomm );
		else if ( d->str_editing )
			string_add( d->character, d->incomm );
		else
		switch ( d->connected )
		{
		case CON_PLAYING:   interpret( d->character, intcomm );   break;
		default:	    nanny               ( d, intcomm );   break;
		}

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
        event_update( );


	/*
	 * Output.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

	    if ( dcon.character )
	        dcon.character->timer = 0;
	    if ( !read_from_descriptor( &dcon ) )
	    {
		if ( dcon.character )
		    save_char_obj( d->character );
		dcon.outtop	= 0;
		close_socket( &dcon );
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
void game_loop_unix( int control )
{
    static struct timeval null_time;
           struct timeval last_time;

#if !defined( AmigaTCP ) && !defined( WIN32 )
#endif

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;
	fd_set           in_set;
	fd_set           out_set;
	fd_set           exc_set;
	int              maxdesc;
	char            *intcomm;

#if defined( MALLOC_DEBUG )
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
        FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( (unsigned) maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if (   select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0
	    && errno != EINTR )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    if ( d->flush_point )
	    {
		flush_command( d );
	    }
            else
	    {
		read_from_buffer( d );
	    }

	    if ( d->incomm[0] != '\0' || d->flusher[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->flush_point )
		    intcomm = d->flusher;
		else
		    intcomm = d->incomm;

		     if ( d->showstr_point )
			show_string( d, d->incomm );
		else if ( d->str_editing )
			string_add( d->character, d->incomm );
		else
		switch ( d->connected )
		{
		case CON_PLAYING:   interpret( d->character, intcomm );   break;
		default:	    nanny               ( d, intcomm );   break;
		}

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
        event_update( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
		&& FD_ISSET( d->descriptor, &out_set ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
#if !defined( WIN32 )
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ( (int) last_time.tv_usec )
	                - ( (int)  now_time.tv_usec )
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ( (int) last_time.tv_sec )
	                - ( (int)  now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if (   select( 0, NULL, NULL, NULL, &stall_time ) < 0
		    && errno != EINTR )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}
#else
	{
	    int times_up;
	    int nappy_time;
	    struct _timeb start_time;
	    struct _timeb end_time;
	    _ftime( &start_time );
	    times_up = 0;

	    while( times_up == 0 )
	    {
		_ftime( &end_time );
		if ( ( nappy_time =
		      (int) ( 1000 *
			     (double) ( ( end_time.time - start_time.time ) +
				       ( (double) ( end_time.millitm -
						   start_time.millitm ) /
					1000.0 ) ) ) ) >=
		    (double)( 1000 / PULSE_PER_SECOND ) )
		  times_up = 1;
		else
		{
		    Sleep( (int) ( (double) ( 1000 / PULSE_PER_SECOND ) -
				  (double) nappy_time ) );
		    times_up = 1;
		}
	    }
	}
#endif

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
void new_descriptor( int control )
{
    static DESCRIPTOR_DATA  d_zero;
           DESCRIPTOR_DATA *dnew;
    struct sockaddr_in      sock;
    struct hostent         *from;
           BAN_DATA        *pban;
    char                    buf [ MAX_STRING_LENGTH ];
    int                     desc;
    unsigned int            size;
    int                     addr;

    size = sizeof( sock );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined( FNDELAY )
#if defined( __hpux )
#define FNDELAY O_NONBLOCK
#else
#define FNDELAY O_NDELAY
#endif
#endif

#if !defined( AmigaTCP ) && !defined( WIN32 )
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }
#endif

    /*
     * Cons a new descriptor.
     */
    if ( !descriptor_free )
    {
	dnew		= alloc_perm( sizeof( *dnew ) );
        top_desc++;
    }
    else
    {
	dnew		= descriptor_free;
	descriptor_free	= descriptor_free->next;
    }

    *dnew		= d_zero;
    dnew->descriptor	= desc;
    dnew->character     = NULL;
    dnew->connected	= CON_GET_COLOR;
    dnew->showstr_head  = str_dup( "" );
    dnew->showstr_point = 0;
    dnew->olc_editing	= NULL;
    dnew->str_editing	= NULL;
    dnew->outsize	= 2000;
    dnew->outbuf	= alloc_mem( dnew->outsize );
    dnew->infirst	= NULL;
    dnew->inlast	= NULL;
    dnew->histsize	= 0;

    size = sizeof( sock );

    /*
     * Would be nice to use inet_ntoa here but it takes a struct arg,
     * which ain't very compatible between gcc and system libraries.
     */
    addr = ntohl( sock.sin_addr.s_addr );
    sprintf( buf, "%d.%d.%d.%d",
	( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	);
    sprintf( log_buf, "Sock.sinaddr:  %s", buf );
    log_string( log_buf );
    from = gethostbyaddr( (char *) &sock.sin_addr,
			 sizeof(sock.sin_addr), AF_INET );
    dnew->host = str_dup( from ? from->h_name : buf );
    sprintf( log_buf, "New connection: %s (%s)", dnew->host, buf );
    wiznet( NULL, WIZ_LOGINS, L_DIR, log_buf );

	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( !str_suffix( pban->name, dnew->host ) )
	{
	    write_to_descriptor( desc,
		"Your site has been banned from this Mud.\n\r", 0 );
#if !defined( WIN32 )
	    close( desc );
#else
	    closesocket( desc );
#endif
	    free_string( dnew->host );
	    free_mem( dnew->outbuf, dnew->outsize );
	    dnew->next		= descriptor_free;
	    descriptor_free	= dnew;
	    return;
	}
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    write_to_buffer( dnew, "Enter your terminal type (0 for ASCII, 1 for ANSI, 9 for Quick): ", 0 );

    if ( ++num_descriptors > sysdata.max_players )
	sysdata.max_players = num_descriptors;
    if ( sysdata.max_players > sysdata.all_time_max )
    {
	free_string( sysdata.time_of_max );
	sprintf( buf, "%24.24s", ctime( &current_time ) );
	sysdata.time_of_max = str_dup( buf );
	sysdata.all_time_max = sysdata.max_players;
	logff( "Broke all-time maximum player record: %d",
	     sysdata.all_time_max );
	save_sysdata( );
    }

    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by )
    {
	write_to_buffer( dclose->snoop_by,
            "Your prey has fled the realm.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf );
	if ( IS_PLAYING( dclose ) )
	{
            act( "$n has been lost to the &+Rab&n&+ry&+Rs&n&+rs&n.", ch, NULL, NULL, TO_ROOM );
	    sprintf( log_buf, "%s has lost link.", ch->name );
            save_char_obj( ch );
	    wiznet( ch, WIZ_LOGINS, 0, log_buf );
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

#if !defined( WIN32 )
    close( dclose->descriptor );
#else
    closesocket( dclose->descriptor );
#endif
    free_string( dclose->host );

    /* RT socket leak fix */
    free_mem( dclose->outbuf, dclose->outsize );

    dclose->next	= descriptor_free;
    descriptor_free	= dclose;

    --num_descriptors;

#if defined( macintosh )
    exit( 1 );
#endif
    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen( d->inbuf );
    if ( iStart >= sizeof( d->inbuf ) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined( macintosh )
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof( d->inbuf ) - 10 )
	    break;
    }
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    for ( ; ; )
    {
	int nRead;

#if !defined( WIN32 )
	nRead = read( d->descriptor, d->inbuf + iStart,
		     sizeof( d->inbuf ) - 10 - iStart );
#else
	nRead = recv( d->descriptor, d->inbuf + iStart,
		     sizeof( d->inbuf ) - 10 - iStart, 0 );
#endif
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read." );
	    return FALSE;
	}
#if !defined( AmigaTCP ) && !defined( WIN32 )
        else if ( errno == EWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
#if defined( WIN32 )
        else if ( WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i;
    int j;
    int k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii( d->inbuf[i] ) && isprint( d->inbuf[i] ) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Do '!' substitution.
     * Expanded by Zen to allow more complex 'tcsh' like history commands.
     * Big repetitive code, ick.
     */
    if ( d->incomm[0] == '!' )
    {
	HISTORY_DATA  *command;
	char           buf [ MAX_STRING_LENGTH ];

	command = NULL;
	if ( d->incomm[1] == '!' )
	{
	    if ( d->inlast )
		command = d->inlast;
	}
	else
	if ( is_number( &d->incomm[1] ) )
	{
	    int            num;
	    int            line;

	    line = atoi( &d->incomm[1] );
	    if ( line >= 0 )
	    {
		for ( command = d->infirst, num = 0; command;
		     command = command->next, num++ )
		{
		    if ( num == line )
			break;
		}
	    }
	    else
	    {
		for ( command = d->inlast, num = -1; command;
		     command = command->prev, num-- )
		{
		    if ( num == line )
			break;
		}
	    }
	}
	else
	if ( d->incomm[1] == '?' )
	{
	    for ( command = d->inlast; command; command = command->prev )
	    {
		if ( !str_infix( &d->incomm[2], command->comm ) )
		    break;
	    }
	}
	else
	{
	    for ( command = d->inlast; command; command = command->prev )
	    {
		if ( !str_prefix( &d->incomm[1], command->comm ) )
		    break;
	    }
	}

	if ( command )
	{
	    strcpy( d->incomm, command->comm );
	    sprintf( buf, "%s\n\r", d->incomm );
	    write_to_descriptor( d->descriptor, buf, 0 );
	}
    }

    if ( d->incomm[0] != '!' && IS_PLAYING( d ) )
    {
	HISTORY_DATA *command;

	command = alloc_mem( sizeof( HISTORY_DATA ) );
	command->next = NULL;
	command->prev = NULL;
	command->comm = str_dup( d->incomm );

	for ( ; d->histsize >= MAX_HISTORY; d->histsize-- )
	{
	    d->infirst = d->infirst->next;
	    free_string( d->infirst->prev->comm );
	    free_mem( d->infirst->prev, sizeof( HISTORY_DATA ) );
	    d->infirst->prev = NULL;
	}

	if ( d->inlast )
	    d->inlast->next = command;
	command->prev = d->inlast;
	d->inlast = command;
	d->histsize++;

	if ( !d->infirst )
	    d->infirst = command;
    }

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
	   char *ptr;
	   char  buf [ MAX_STRING_LENGTH ];
	   int   shown_lines = 0;
	   int   total_lines = 0;
    extern bool  merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && IS_PLAYING( d ) )
    {
        if ( d->showstr_point )
	{
	    for ( ptr = d->showstr_head; ptr != d->showstr_point; ptr++ )
		if ( *ptr == '\n' )
		    shown_lines++;

	    total_lines = shown_lines;
	    for ( ptr = d->showstr_point; *ptr != '\0'; ptr++ )
		if ( *ptr == '\n' )
		    total_lines++;

	    sprintf( buf,
  "\n\r%d%%) Please type (c)ontinue, (r)efresh, (b)ack, (q)uit, or RETURN\n\r",
		   100 * shown_lines / total_lines );
	    write_to_buffer( d, buf, 0 );
	}
	else	
	  if ( d->str_editing )
	    write_to_buffer( d, "> ", 2 );
	else
	  if ( d->connected > CON_PASSWD_GET_OLD )
	  {
	    CHAR_DATA *ch;

	    ch = d->original ? d->original : d->character;
	    if ( IS_SET( ch->act, PLR_BLANK     ) )
	        write_to_buffer( d, "\n\r", 2 );

	    if ( IS_SET( ch->act, PLR_PROMPT    ) )
	        bust_a_prompt( d );

	    if ( IS_SET( ch->act, PLR_TELNET_GA ) )
	        write_to_buffer( d, go_ahead_str, 0 );
	  }
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 * Altered by Veygoth and Sniktiorg
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
         CHAR_DATA *ch;
   const char      *str;
   const char      *i;
         char      *point;
         char       buf  [ MAX_STRING_LENGTH ];
         char       buf2 [ MAX_STRING_LENGTH ];
         char      *pbuff;
         char       buffer [ 4 * MAX_STRING_LENGTH ];

   /* Will always have a pc ch after this */
   ch = ( d->original ? d->original : d->character );
   if( !ch->pcdata->prompt || ch->pcdata->prompt[0] == '\0' )
   {
      send_to_char( "\n\r\n\r", ch );
      return;
   }

   point = buf;
   str = ch->pcdata->prompt;

   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
         case 'h' :
            // Veygoth - set it so that the hitpoints of a character
            // show up a different color dependent on level of injuries.

            if( ch->hit < (get_max_hit( ch ) / 10))
              sprintf( buf2, "&+R%d&+L", ch->hit );
            else if( ch->hit < (get_max_hit( ch ) / 3))
              sprintf( buf2, "&n&+r%d&+L", ch->hit );
            else if( ch->hit < (get_max_hit( ch ) * 2 / 3))
              sprintf( buf2, "&n&+m%d&+L", ch->hit );
            else
              sprintf( buf2, "%d", ch->hit );
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", get_max_hit( ch )                    );
            i = buf2; break;
         case 'n' :
            sprintf( buf2, "\n\r");
            i = buf2; break;
         case 'l' :
            if (ch->position <= POS_SLEEPING)
              sprintf( buf2, "&n&+r%s&n", capitalize( position_string( ch->position) ) );
            else if ( ch->position <= POS_KNEELING )
              sprintf( buf2, "&n&+y%s&n", capitalize( position_string( ch->position) ) );
            else 
              sprintf( buf2, "&n&+g%s&n", capitalize( position_string( ch->position) ) );
            i = buf2; break;
         case 'm' :
            // Veygoth - set it so that the mana of a character
            // show up a different color dependent on how low it is.

            if( ch->mana < (ch->max_mana / 10))
              sprintf( buf2, "&+R%d&+L", ch->mana );
            if( ch->mana < (ch->max_mana / 3))
              sprintf( buf2, "&n&+r%d&+L", ch->mana );
            else if( ch->mana < ((ch->max_mana * 2) / 3))
              sprintf( buf2, "&n&+m%d&+L", ch->mana );
            else
              sprintf( buf2, "%d", ch->mana );
            i = buf2; break;
         case 'M' :
            sprintf( buf2, "%d", ch->max_mana                          );
            i = buf2; break;
         case 'u' :
            sprintf( buf2, "%d", num_descriptors                       ); 
            i = buf2; break;
         case 'U' :
            sprintf( buf2, "%d", sysdata.max_players                   );
            i = buf2; break;
         case 'v' :
            // Veygoth - set it so that the moves of a character
            // show up a different color dependent on how tired they are.

            if( ch->move < (ch->max_move / 10))
              sprintf( buf2, "&+R%d&+L", ch->move );
            if( ch->move < (ch->max_move / 3))
              sprintf( buf2, "&n&+r%d&+L", ch->move );
            else if( ch->move < ((ch->max_move * 2) / 3))
              sprintf( buf2, "&n&+m%d&+L", ch->move );
            else
              sprintf( buf2, "%d", ch->move ); 
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", ch->max_move                          );
            i = buf2; break;
//
// Altered Tank Status Crapola, compliments of Sniktiorg Hack Hacking
//
          case 'e' :
            // Display (E)nemy's status as Chain Meter in Combat
            // Sniktiorg alteration
            if ( ch->fighting )
            {
              sprintf(buf2,"%s: %s ",
                 capitalize(PERS( ch->fighting, ch)),
                 condition_meter(ch->fighting));
              i = buf2;
            }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
            break;
         case 'E' :
            // Display (E)nemy's status as New Text in Combat
            // Sniktiorg alteration
            if( ch->fighting )
            {
              sprintf(buf2,"%s: %s ",
                 capitalize(PERS( ch->fighting, ch)),
                 condition_string(ch->fighting));
              i = buf2;
            }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
            break;
         case 'b' :
            // Added by Sniktiorg for Old Meters
            // add (B)ad Guy condition Standard status bar here.
            if ( ch->fighting )
            {
              sprintf(buf2,"%s: %s ",
                 capitalize(PERS( ch->fighting, ch)),
                 condition_meter2(ch->fighting));
              i = buf2;
            }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
            break;
         case 'B' :
            // Added by Sniktiorg for Old Text Colors
            // add (B)ad Guy condition Standard Text bar here.
            if ( ch->fighting )
            {
              sprintf(buf2,"%s: %s ",
                 capitalize(PERS( ch->fighting, ch)),
                 condition_string2(ch->fighting));
              i = buf2;
            }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
            break;
          case 'd':
            // Added by Sniktiorg for Old Meters
            // add (D)efensive tank condition Standard status bar here.
            if( ch->fighting && ch->fighting->fighting )
              {
                sprintf(buf2,"%s: %s ",
                   capitalize(PERS( ch->fighting->fighting, ch )),
                   condition_meter2(ch->fighting->fighting));
                i = buf2;
              }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
           break;
          case 'D':
            // Added by Sniktiorg for Old Meters
            // add (D)efensive tank condition Old Text Colors here.
            if( ch->fighting && ch->fighting->fighting )
              {
                sprintf(buf2,"%s: %s ",
                   capitalize(PERS( ch->fighting->fighting, ch )),
                   condition_string2(ch->fighting->fighting));
                i = buf2;
              }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
           break;
         case 't':
            // New Sniktiorg Meter:
            // Add (T)ank condition Chain Status bar here.
            if( ch->fighting && ch->fighting->fighting )
              {
                sprintf(buf2,"%s: %s ",
                   capitalize(PERS( ch->fighting->fighting, ch )),
                   condition_meter(ch->fighting->fighting));
                i = buf2;
              }
            else
            {
              sprintf( buf2, "&n" );
              i = buf2;
            }
           break;
         case 'T':
           // Sniktiorg Alterations
           // add tank condition NEW Text Strings here.
           if( ch->fighting && ch->fighting->fighting )
             {
                sprintf(buf2,"%s: %s ",
                   capitalize(PERS( ch->fighting->fighting, ch )),
                   condition_string(ch->fighting->fighting));
                i = buf2;
             }
           else    
             {
               sprintf( buf2, "&n" );
               i = buf2;
             }
           break;
         case 'P':
           // Add player (not Tank) status strings here.
           // New Text Section
           if( ch->fighting )
             {
               sprintf( buf2, "%s: %s ", capitalize(ch->name), condition_string( ch ) );
               i = buf2;
             }
           else
             {
               sprintf( buf2, "&n" );
               i = buf2;
             }
           break;
         case 'p':
            // Sniktiorg Chain Meter for PC (Not Tank)
            if( ch->fighting )
             {
               sprintf( buf2, "%s: %s ", capitalize(ch->name), condition_meter( ch ));
               i = buf2;
             }
           else
             {
               sprintf( buf2, "&n" );
               i = buf2;
             }
           break;
         case 'Q':
           // Add player (not Tank) status strings here.
           // Old Text Section
           if( ch->fighting )
             {
               sprintf( buf2, "%s: %s ", capitalize(ch->name), condition_string2( ch ) );
               i = buf2;
             }
           else
             {
               sprintf( buf2, "&n" );
               i = buf2;
             }
           break;
         case 'q':
            // Sniktiorg: Old Meter for PC (Not Tank)
            if( ch->fighting )
             {
               sprintf( buf2, "%s: %s ", capitalize(ch->name), condition_meter2( ch ));
               i = buf2;
             }
           else
             {
               sprintf( buf2, "&n" );
               i = buf2;
             }
           break;
//  End Major Alterations
//  by Sniktiorg d'Hack ;)
//
         case 'a' :
            if( ch->level > 10 )
               sprintf( buf2, "%d", ch->alignment                      );
            else
               sprintf( buf2, "%s", IS_GOOD( ch ) ? "good"
		                  : IS_EVIL( ch ) ? "evil" : "neutral" );
            i = buf2; break;
         case 'r' :
            if( ch->in_room )
               sprintf( buf2, "%s",
               ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) ||
                 ( !is_affected( ch, AFF_BLIND ) &&
		   !room_is_dark( ch->in_room ) ) )
	       ? ch->in_room->name : "darkness"                        );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case 'R' :
            if( IS_IMMORTAL( ch ) && ch->in_room )
               sprintf( buf2, "%d", ch->in_room->vnum                  );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case 'z' :
            if( IS_IMMORTAL( ch ) && ch->in_room )
               sprintf( buf2, "%s", ch->in_room->area->name            );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case 'i' :  /* Invisible notification on prompt sent by Kaneda */
	    sprintf( buf2, "%s", is_affected( ch, AFF_INVISIBLE ) ?
		                 "invisible" : "visible" );
	    i = buf2; break;
         case 'I' :
            if( IS_IMMORTAL( ch ) )
               sprintf( buf2, "(wizinv: %s)", IS_SET( ch->act, PLR_WIZINVIS ) ?
		                              "on" : "off" );
            else
               sprintf( buf2, " "                                      );
            i = buf2; break;
         case '%' :
            sprintf( buf2, "%%"                                        );
            i = buf2; break;
      }
      ++str;
      while( ( *point = *i ) != '\0' )
         ++point, ++i;
   }
   *point = '\0';

   pbuff	= buffer;
   colourconv( pbuff, buf, ch );
   write_to_buffer( d, buffer, 0 );
   return;
}

// function by Veygoth; Colors by Sniktiorg
char * condition_string( CHAR_DATA *ch )
{
  int percent;

  if( get_max_hit( ch ) > 0 )
    percent = ( 100 * ch->hit ) / get_max_hit( ch );
  else
    percent = -1;

       if( percent >= 100 ) return "&+CExcellent&n";
  else if( percent >= 90  ) return "&+LBarely &n&+cScratched&n";
  else if( percent >= 79  ) return "&+CBa&n&+ct&+Ct&n&+cer&+Ce&n&+cd&+L and &n&+mBr&+Mu&n&+mi&+Mse&n&+md&n";
  else if( percent >= 69  ) return "&+LLightly &+MWo&n&+mu&+Mn&n&+mde&+Md&n";
  else if( percent >= 58  ) return "&+LModerately &+MWo&+Ru&+Mn&+Rde&+Md&n";
  else if( percent >= 48  ) return "&+LSeverely &n&+rWo&n&+mu&+rn&+mde&n&+rd&n";
  else if( percent >= 37  ) return "&n&+rBl&+Re&n&+re&+Rdi&n&+rn&+Rg&+L Freely&n";
  else if( percent >= 27  ) return "&+LBathed in &n&+rBl&+Ro&n&+ro&+Rd&n";
  else if( percent >= 16  ) return "&+LLeaking&n&+r Guts&n";
  else if( percent >= 6   ) return "&+RWr&n&+ri&+Rt&n&+rhi&+Rn&n&+rg&+L in &n&+rA&+Rgo&n&+rn&+Ry&n";
  else                      return "&+LAt &n&+wDeath&+L's Door&n";
}

// function by Veygoth; Colors by Sniktiorg
char * condition_string2( CHAR_DATA *ch )
{
  int percent;

  if( get_max_hit( ch ) > 0 )
    percent = ( 100 * ch->hit ) / get_max_hit( ch );
  else
    percent = -1;

       if( percent >= 100  ) return "&+GExcellent&n";
  else if( percent >=  90  ) return "&+LBarely &n&+gScratched&n";
  else if( percent >=  79  ) return "&+GBattered&+L and &n&+gBruised&n";
  else if( percent >=  69  ) return "&+LLightly &n&+yWounded&n";
  else if( percent >=  58  ) return "&+LModerately &n&+yWou&+rnded&n";
  else if( percent >=  48  ) return "&+LSeverely &n&+rWo&n&+yu&+rn&+yde&n&+rd&n";
  else if( percent >=  37  ) return "&n&+rBleeding&+L Copiously&n";
  else if( percent >=  27  ) return "&+LCovered in &n&+rBl&+Ro&n&+ro&+Rd&n";
  else if( percent >=  16  ) return "&+LLeaking&n&+R Entrails&n";
  else if( ch->hit >  -3   ) return "&+RWrithing&+L in &n&+rTorment&n";
  else if( ch->hit >  -5   ) return "&+LIncapacitated, and &+RBl&n&+re&+Re&n&+rdi&+Rn&n&+rg&+L to Death&n";
  else if( ch->hit >= -10) return"&+rMortally &+Rw&n&+rou&+Rnd&N&+re&+Rd&n";
   else return "&+WDEAD!&n";
}

// function by Veygoth; Colors by Sniktiorg
char * condition_meter( CHAR_DATA *ch )
{
  int percent;

  if( get_max_hit( ch ) > 0 )
    percent = ( 100 * ch->hit ) / get_max_hit( ch );
  else
    percent = -1;

       if( percent >= 100 ) return "&+L(&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&+L)&n";
  else if( percent >= 90  ) return "&+L(&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&+R=&+L)&n"; 
  else if( percent >= 79  ) return "&+L(&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&+R=&n&+r-&+L)&n";
  else if( percent >= 69  ) return "&+L(&+C-&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&+R=&n&+r-&+R=&+L)&n";
  else if( percent >= 58  ) return "&+L(&n&+c=&+C-&n&+c=&+C-&n&+c=&+C-&+R=&n&+r-&+R=&n&+r-&+L)&n";
  else if( percent >= 48  ) return "&+L(&+C-&n&+c=&+C-&n&+c=&+C-&+R=&n&+r-&+R=&n&+r-&+R=&+L)&n";
  else if( percent >= 37  ) return "&+L(&n&+c=&+C-&n&+c=&+C-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+L)&n";
  else if( percent >= 27  ) return "&+L(&+C-&n&+c=&+C-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&+L)&n";
  else if( percent >= 16  ) return "&+L(&n&+c=&+C-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+L)&n";
  else if( percent >= 6   ) return "&+L(&+C-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&+L)&n";
  else                      return "&+L(&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+R=&n&+r-&+L)&n";
}

// function by Veygoth
char * condition_meter2( CHAR_DATA *ch )
{
  int percent;

  if( get_max_hit( ch ) > 0 )
    percent = ( 100 * ch->hit ) / get_max_hit( ch );
  else
    percent = -1;

       if( percent >= 100 ) return "&+L(&+r+++&+y++&+Y++&n&+g+++&+L)&n";
  else if( percent >= 90  ) return "&+L(&+r+++&+y++&+Y++&n&+g++&+L-&+L)&n"; 
  else if( percent >= 79  ) return "&+L(&+r+++&+y++&+Y++&n&+g+&+L--&+L)&n";
  else if( percent >= 69  ) return "&+L(&+r+++&+y++&+Y++&+L---&+L)&n";
  else if( percent >= 58  ) return "&+L(&+r+++&+y++&+Y+&+L----&+L)&n";
  else if( percent >= 48  ) return "&+L(&+r+++&+y++&+L-----&+L)&n";
  else if( percent >= 37  ) return "&+L(&+r+++&+y+&+L------&+L)&n";
  else if( percent >= 27  ) return "&+L(&+r+++&+L-------&+L)&n";
  else if( percent >= 16  ) return "&+L(&+r++&+L--------&+L)&n";
  else if( percent >= 6   ) return "&+L(&+r+&+L---------&+L)&n";
  else                      return "&+L(&+L----------&+L)&n";
}


/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen( txt );

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]	= '\n';
	d->outbuf[1]	= '\r';
	d->outtop	= 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

        if ( d->outsize >= 32000 && !IS_IMMORTAL( d->original ? d->original : d->character ) )
        {
            /* empty buffer */
            d->outtop = 0;
            bugf( "Buffer overflow. Closing (%s).",
		( d->character ? d->character->name : "???" ) );
	    write_to_descriptor( d->descriptor, "\n\r*** BUFFER OVERFLOW!!! ***\n\r", 0 );
	    close_socket( d );
            return;
        }
        if ( d->outsize >= 32000 && IS_IMMORTAL( d->original ? d->original : d->character ) )
        {
            return;
        }
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.  Modifications sent in by Zavod.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    d->outbuf[d->outtop] = '\0';
    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined( macintosh )
    if ( desc == 0 )
	desc = 1;
#endif

    if ( length <= 0 )
	length = strlen( txt );

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 2048 );
#if !defined( WIN32 )
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
#else
	if ( ( nWrite = send( desc, txt + iStart, nBlock , 0) ) < 0 )
#endif
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}

/*
 * Deal with sockets that haven't logged in yet, and then some more!
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    NOTE_DATA *pnote;
    char      *pwdnew;
    char classname[MAX_STRING_LENGTH];
    char      *p;
    char       buf [ MAX_STRING_LENGTH ];
    int        iClass;
    int        iRace;
    int        notes;
    int        lines;
    bool       fOld;

    last_fun( "nanny");
    while ( isspace( *argument ) )
	argument++;

    /* This is here so we wont get warnings.  ch = NULL anyways - Kahn */
   ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	argument[0] = UPPER( argument[0] );

	fOld = load_char_obj( d, argument );

	if ( !fOld && !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	ch   = d->character;

	if ( IS_SET( ch->act, PLR_DENY ) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ))
	{
	    fOld = TRUE;
	}
	else
	{
	    /* Must be immortal with wizbit in order to get in */
	    if ( wizlock
		&& !IS_HERO( ch )
		&& !IS_SET( ch->act, PLR_WIZBIT ) )
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
		if ( ch->level == 0 && IS_SET( sysdata.act, MUD_NEWLOCK ))
		{
		    write_to_buffer( d, "\n\r\n\rThis is a private port.  If you want to play ", 0 );
		    write_to_buffer( d, "Magma, please contact the administrators for access.\n\r", 0 );
                    close_socket( d ) ;
                    return;
		}
	    if ( ch->level < numlock
		&& !IS_SET( ch->act, PLR_WIZBIT ) )
	    {
		write_to_buffer( d,
			"The game is locked to your level character.\n\rTry contacting the admins for assitance.\n\r\n\r",
				0 );
                    close_socket( d ) ;
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
            if( d->color )
	      write_to_buffer( d, "[0;31mPassword: [0m", 0 );
            else
	      write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	}
	else
	{
	    /* New player */
            sprintf( buf, "Do ye wish to be called %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( argument, ch->pcdata->pwd ) )
	{
            write_to_buffer( d, "Incorrect password.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	if ( check_reconnect( d, ch->name, TRUE ) )
        {
	    return;
        }

        if ( check_playing( d, ch->name ) )
	{
	     sprintf( buf, "A connection has just been overridden for %s.", ch->name );
             log_string( buf );
             return;
	}

	sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
	log_string( log_buf );

	write_to_buffer( d, "\n\r", 2 );

        if( d->color ) 
            SET_BIT( ch->act, PLR_COLOUR );
        else
            REMOVE_BIT( ch->act, PLR_COLOUR );

	lines = ch->pcdata->pagelen;
	ch->pcdata->pagelen = 20;
	if ( IS_HERO( ch ) )
	    do_help( ch, "_imotd" );
	do_help( ch, "_motd" );
	write_to_buffer( d, "\n\r", 2 );
	ch->pcdata->pagelen = lines;

        do_help( ch, "_login_menu" );

	d->connected = CON_MENU;
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		    ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen( argument ) < 5 )
	{
	    write_to_buffer( d,
	       "Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = argument;
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
        write_to_buffer( d, "Please retype your password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( argument, ch->pcdata->pwd ) )
	{
            write_to_buffer( d, "The Passwords don't match.\n\rRetype your password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

        // Eliminated the extra Return - Veygoth
        if( d->color )
        {
          write_to_buffer( d, help_races_color, 0 );
        }
        else
        {
          write_to_buffer( d, help_races_nocolor, 0 );
        }
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	    if ( !str_prefix( argument, race_table[iRace].name )
		&& IS_SET( race_table[ iRace ].race_abilities,
			  RACE_PC_AVAIL ) )
	    {
		ch->race = race_lookup( race_table[iRace].name );
		break;
	    }

	if ( iRace == MAX_RACE )
	{
	    write_to_buffer( d,
			    "That is not a race.\n\rWhat IS your race? ", 0 );
	    return;
	}

	write_to_buffer( d, "\n\r", 0 );
	REMOVE_BIT( ch->act, PLR_PAGER );
	do_help( ch, race_table[ch->race].name );
	write_to_buffer( d, "Are you sure you want this race?  ", 0 );
	d->connected = CON_CONFIRM_NEW_RACE;
	break;

    case CON_CONFIRM_NEW_RACE:
	if ( argument[0] == 'y' 
	     || argument[0] == 'Y' )
	{
	    write_to_buffer( d, "\n\rWhat is your sex (M/F)? ", 0 );
	    d->connected = CON_GET_NEW_SEX;
	}
	else
	{
          if( d->color )
          {
	    write_to_buffer( d, help_races_color, 0 );
          }
          else
          {
	    write_to_buffer( d, help_races_nocolor, 0 );
          }
          d->connected = CON_GET_NEW_RACE;
	}
	break;

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}

        // Eliminated the extra return during character creation - Veygoth
	strcpy( buf, "Select a class [" );
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
            if( class_avail[ch->race][iClass] )
            {
		strcat( buf, " " );
                strcat( buf, class_table[iClass]->name );
            }
	}
	strcat( buf, "]:" );
	write_to_buffer( d, buf, 0 );
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	memset(classname, 0, MAX_STRING_LENGTH);;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
            if( !class_table[iClass] )
            {
               bug( "Class %d does not exist!", iClass );
               continue;
            }
            if( !class_table[iClass]->name )
            {
               bug( "Class %d does not have a name!", iClass );
               continue;
            }
	    if ( !str_prefix( argument, class_table[iClass]->name ) )
	    {
              if( class_avail[ch->race][iClass] )
              {
		ch->class = iClass;
		strcpy(classname, class_table[iClass]->name);
		break;
              }
              else
              {
	        write_to_buffer( d,
	      "That class is not available to your race.\n\rWhat IS your class? ", 0 );
	        return;
              }
            }
	}

	if ( iClass == MAX_CLASS )
	{
	    write_to_buffer( d,
		"That's not a class.\n\rWhat IS your class? ", 0 );
	    return;
	}

	write_to_buffer( d, "\n\r", 0 );

	if ( classname[0] != 0 )
	    do_help( ch, classname );
	else
	    bug( "Nanny CON_GET_NEW_CLASS:  ch->class (%d) not valid",
		ch->class );

	write_to_buffer( d, "Are you sure you want this class?  ", 0 );
	d->connected = CON_CONFIRM_CLASS;
	break;

    case CON_CONFIRM_CLASS:
	switch ( argument[0] )
	{
	  case 'y': case 'Y': break;
	  default:
              // Eliminated the extra return - Veygoth
	      write_to_buffer( d, "\n\rPress Return to continue:\n\r", 0 );
	      strcpy( buf, "Select a class [" );
              for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	      {  
                if( class_avail[ch->race][iClass] )
            	{
		     strcat( buf, " " );
                     strcat( buf, class_table[iClass]->name );
                }	
	      }
	      strcat( buf, "]: " );
	      write_to_buffer( d, buf, 0 );
	      d->connected = CON_GET_NEW_CLASS;
	      return;
	}
       
        ch->perm_str                = dice( 3, 31 ) + 7;
        ch->perm_int                = dice( 3, 31 ) + 7;
        ch->perm_wis                = dice( 3, 31 ) + 7;
        ch->perm_dex                = dice( 3, 31 ) + 7;
        ch->perm_con                = dice( 3, 31 ) + 7;
        ch->perm_agi                = dice( 3, 31 ) + 7;
        ch->perm_cha                = dice( 3, 31 ) + 7;
        ch->perm_pow                = dice( 3, 31 ) + 7;
        ch->perm_luk                = dice( 3, 31 ) + 7;

            switch ( class_table[ch->class]->attr_prime )
            {
            case APPLY_STR:
              ch->perm_str = number_range( 70, 100);
              break;
            case APPLY_INT:
              ch->perm_int = number_range( 70, 100);
              break;
            case APPLY_WIS:
              ch->perm_wis = number_range( 70, 100);
              break;
            case APPLY_DEX:
              ch->perm_dex = number_range( 70, 100);
              break;
            case APPLY_CON:
              ch->perm_con = number_range( 70, 100);
              break;
            case APPLY_AGI:
              ch->perm_agi = number_range( 70, 100);
              break;
            case APPLY_CHA:
              ch->perm_pow = number_range( 70, 100);
              break;
            case APPLY_POW:
              ch->perm_cha = number_range( 70, 100);
              break;
            case APPLY_LUK:
              ch->perm_luk = number_range( 70, 100);
              break;
            }

         sprintf( buf, "\n\r\n\rStr: %-17s    Int: %s\n\rDex: %-17s    Wis: %s\n\rAgi: %-17s    Cha: %s\n\rCon: %s\n\rPow: %s\n\r\n\r",
           stat_string( get_curr_str( ch )),
           stat_string( get_curr_int( ch )),
           stat_string( get_curr_dex( ch )),
           stat_string( get_curr_wis( ch )),
           stat_string( get_curr_agi( ch )),
           stat_string( get_curr_cha( ch )),
           stat_string( get_curr_con( ch )),
           stat_string( get_curr_pow( ch )) );
        strcat( buf, "Do you want to keep these stats? " );

        write_to_buffer( d, buf, 0 );

        d->connected = CON_ROLL_STATS;
        break;

   case CON_ROLL_STATS:
	switch ( argument[0] )
	{
	  case 'y': case 'Y': break;
	  default:
            ch->perm_str                = dice( 3, 31 ) + 7;
            ch->perm_int                = dice( 3, 31 ) + 7;
            ch->perm_wis                = dice( 3, 31 ) + 7;
            ch->perm_dex                = dice( 3, 31 ) + 7;
            ch->perm_con                = dice( 3, 31 ) + 7;
            ch->perm_agi                = dice( 3, 31 ) + 7;
            ch->perm_cha                = dice( 3, 31 ) + 7;
            ch->perm_pow                = dice( 3, 31 ) + 7;
            ch->perm_luk                = dice( 3, 31 ) + 7;

            switch ( class_table[ch->class]->attr_prime )
            {
            case APPLY_STR:
              ch->perm_str = number_range( 70, 100);
              break;
            case APPLY_INT:
              ch->perm_int = number_range( 70, 100);
              break;
            case APPLY_WIS:
              ch->perm_wis = number_range( 70, 100);
              break;
            case APPLY_DEX:
              ch->perm_dex = number_range( 70, 100);
              break;
            case APPLY_CON:
              ch->perm_con = number_range( 70, 100);
              break;
            case APPLY_AGI:
              ch->perm_agi = number_range( 70, 100);
              break;
            case APPLY_CHA:
              ch->perm_pow = number_range( 70, 100);
              break;
            case APPLY_POW:
              ch->perm_cha = number_range( 70, 100);
              break;
            case APPLY_LUK:
              ch->perm_luk = number_range( 70, 100);
              break;
            }
            sprintf( buf, "\n\r\n\rStr: %-17s    Int: %s\n\r"
                                  "Wis: %-17s    Dex: %s\n\r"
                                  "Con: %-17s    Agi: %s\n\r"
                                  "Cha: %-17s    Luk: %s\n\r"
                                  "Pow: %s\n\r\n\r",
		stat_string( get_curr_str( ch )),
		stat_string( get_curr_int( ch )),
		stat_string( get_curr_wis( ch )),
		stat_string( get_curr_dex( ch )),
		stat_string( get_curr_con( ch )),
		stat_string( get_curr_agi( ch )),
		stat_string( get_curr_cha( ch )),
		stat_string( get_curr_luk( ch )),
		stat_string( get_curr_pow( ch )) );
            strcat( buf, "Do you want to keep these stats? " );

   
	      write_to_buffer( d, buf, 0 );
	      d->connected = CON_ROLL_STATS;
	      return;
	}

        write_to_buffer( d, "Which stat do you want to put your first of three bonuses into? ", 0 );
        d->connected = CON_FIRST_BONUS;
	break;

   case CON_FIRST_BONUS:
	if( !str_prefix( argument, "strength" ))
        {
            ch->perm_str += dice( 2, 8 );
            if( ch->perm_str > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "intelligence" ))
        {
            ch->perm_int += dice( 2, 8 );
            if( ch->perm_int > 100 ) ch->perm_int = 100;
        }
	else if( !str_prefix( argument, "wisdom" ))
        {
            ch->perm_wis += dice( 2, 8 );
            if( ch->perm_wis > 100 ) ch->perm_wis = 100;
        }
	else if( !str_prefix( argument, "dexterity" ))
        {
            ch->perm_dex += dice( 2, 8 );
            if( ch->perm_dex > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "constitution" ))
        {
            ch->perm_con += dice( 2, 8 );
            if( ch->perm_con > 100 ) ch->perm_con = 100;
        }
	else if( !str_prefix( argument, "agility" ))
        {
            ch->perm_agi += dice( 2, 8 );
            if( ch->perm_agi > 100 ) ch->perm_agi = 100;
        }
	else if( !str_prefix( argument, "charisma" ))
        {
            ch->perm_cha += dice( 2, 8 );
            if( ch->perm_cha > 100 ) ch->perm_cha = 100;
        }
	else if( !str_prefix( argument, "power" ))
        {
            ch->perm_pow += dice( 2, 8 );
            if( ch->perm_pow > 100 ) ch->perm_pow = 100;
        }
        else
        {
	    write_to_buffer( d, "I didn't quite understand you.  Which stat do you want your first bonus in? ", 0 );
            d->connected = CON_FIRST_BONUS;
            return;
        }

        sprintf( buf, "\n\r\n\rStr: %-17s    Int: %s\n\rWis: %-17s    Dex: %s\n\rCon: %-17s    Agi: %s\n\rCha: %s\n\rPow: %s\n\r\n\r",
		stat_string( get_curr_str( ch )),
		stat_string( get_curr_int( ch )),
		stat_string( get_curr_wis( ch )),
		stat_string( get_curr_dex( ch )),
		stat_string( get_curr_con( ch )),
		stat_string( get_curr_agi( ch )),
		stat_string( get_curr_cha( ch )),
		stat_string( get_curr_pow( ch )) );
   
        write_to_buffer( d, buf, 0 );

        write_to_buffer( d, "Which stat do you want to put your second of three bonuses into? ", 0 );
        d->connected = CON_SECOND_BONUS;
	break;

   case CON_SECOND_BONUS:
	if( !str_prefix( argument, "strength" ))
        {
            ch->perm_str += dice( 2, 8 );
            if( ch->perm_str > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "intelligence" ))
        {
            ch->perm_int += dice( 2, 8 );
            if( ch->perm_int > 100 ) ch->perm_int = 100;
        }
	else if( !str_prefix( argument, "wisdom" ))
        {
            ch->perm_wis += dice( 2, 8 );
            if( ch->perm_wis > 100 ) ch->perm_wis = 100;
        }
	else if( !str_prefix( argument, "dexterity" ))
        {
            ch->perm_dex += dice( 2, 8 );
            if( ch->perm_dex > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "constitution" ))
        {
            ch->perm_con += dice( 2, 8 );
            if( ch->perm_con > 100 ) ch->perm_con = 100;
        }
	else if( !str_prefix( argument, "agility" ))
        {
            ch->perm_agi += dice( 2, 8 );
            if( ch->perm_agi > 100 ) ch->perm_agi = 100;
        }
	else if( !str_prefix( argument, "charisma" ))
        {
            ch->perm_cha += dice( 2, 8 );
            if( ch->perm_cha > 100 ) ch->perm_cha = 100;
        }
	else if( !str_prefix( argument, "power" ))
        {
            ch->perm_pow += dice( 2, 8 );
            if( ch->perm_pow > 100 ) ch->perm_pow = 100;
        }
        else
        {
	    write_to_buffer( d, "I didn't quite understand you.  Which stat do you want your second bonus in? ", 0 );
            d->connected = CON_SECOND_BONUS;
            return;
        }

        sprintf( buf, "\n\r\n\rStr: %-17s    Int: %s\n\rWis: %-17s    Dex: %s\n\rCon: %-17s    Agi: %s\n\rCha: %s\n\rPow: %s\n\r\n\r",
		stat_string( get_curr_str( ch )),
		stat_string( get_curr_int( ch )),
		stat_string( get_curr_wis( ch )),
		stat_string( get_curr_dex( ch )),
		stat_string( get_curr_con( ch )),
		stat_string( get_curr_agi( ch )),
		stat_string( get_curr_cha( ch )),
		stat_string( get_curr_pow( ch )) );
   
        write_to_buffer( d, buf, 0 );

        write_to_buffer( d, "Which stat do you want to put your third of three bonuses into? ", 0 );
        d->connected = CON_THIRD_BONUS;
	break;

   case CON_THIRD_BONUS:
	if( !str_prefix( argument, "strength" ))
        {
            ch->perm_str += dice( 2, 8 );
            if( ch->perm_str > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "intelligence" ))
        {
            ch->perm_int += dice( 2, 8 );
            if( ch->perm_int > 100 ) ch->perm_int = 100;
        }
	else if( !str_prefix( argument, "wisdom" ))
        {
            ch->perm_wis += dice( 2, 8 );
            if( ch->perm_wis > 100 ) ch->perm_wis = 100;
        }
	else if( !str_prefix( argument, "dexterity" ))
        {
            ch->perm_dex += dice( 2, 8 );
            if( ch->perm_dex > 100 ) ch->perm_str = 100;
        }
	else if( !str_prefix( argument, "constitution" ))
        {
            ch->perm_con += dice( 2, 8 );
            if( ch->perm_con > 100 ) ch->perm_con = 100;
        }
	else if( !str_prefix( argument, "agility" ))
        {
            ch->perm_agi += dice( 2, 8 );
            if( ch->perm_agi > 100 ) ch->perm_agi = 100;
        }
	else if( !str_prefix( argument, "charisma" ))
        {
            ch->perm_cha += dice( 2, 8 );
            if( ch->perm_cha > 100 ) ch->perm_cha = 100;
        }
	else if( !str_prefix( argument, "power" ))
        {
            ch->perm_pow += dice( 2, 8 );
            if( ch->perm_pow > 100 ) ch->perm_pow = 100;
        }
        else
        {
	    write_to_buffer( d, "I didn't quite understand you.  Which stat do you want your third bonus in? ", 0 );
            d->connected = CON_THIRD_BONUS;
            return;
        }

        sprintf( buf, "\n\r\n\rStr: %-17s    Int: %s\n\rWis: %-17s    Dex: %s\n\rCon: %-17s    Agi: %s\n\rCha: %s\n\rPow: %s\n\r\n\r",
		stat_string( get_curr_str( ch )),
		stat_string( get_curr_int( ch )),
		stat_string( get_curr_wis( ch )),
		stat_string( get_curr_dex( ch )),
		stat_string( get_curr_con( ch )),
		stat_string( get_curr_agi( ch )),
		stat_string( get_curr_cha( ch )),
		stat_string( get_curr_pow( ch )) );
   
        write_to_buffer( d, buf, 0 );

        add_languages( ch );

        if( ch->race == RACE_HALFELF )
        {
          write_to_buffer( d, "\n\rPlease choose a hometown:  Paranel or Shadowglade\n\r", 0 );
        }
        else if( ch->race == RACE_OGRE )
        {
          write_to_buffer( d, "\n\rPlease choose a hometown:  Kragthor or Bloodrock\n\r", 0 );
        }
        else
        {
          write_to_buffer( d, "\n\rYour race does not get a choice of hometown.\n\rPress <enter> to continue.\n\r", 0 );
        }

	d->connected = CON_CHOOSE_HOMETOWN;
	break;

    case CON_CHOOSE_HOMETOWN:
        if( ch->race == RACE_HALFELF )
        {
  	  if( !str_cmp( argument, "paranel" ))
          {
            ch->pcdata->currhome = repop_point[RACE_HUMAN][ch->class];
          }
          else if( !str_cmp( argument, "shadowglade" ))
          {
            ch->pcdata->currhome = repop_point[RACE_GREYELF][ch->class];
          }
          else
          {
            write_to_buffer( d, "Please choose Paranel or Shadowglade.\n\r", 0 );
            d->connected = CON_CHOOSE_HOMETOWN;
            return;
          }
        }
        else if( ch->race == RACE_OGRE )
        {
  	  if( !str_cmp( argument, "bloodrock" ))
          {
            ch->pcdata->currhome = repop_point[ch->race][ch->class];
          }
          else if( !str_cmp( argument, "kragthor" ))
          {
            switch( ch->class )
            {
              case CLASS_WARRIOR:
                ch->pcdata->currhome = 21144;
                break;
              case CLASS_MERCENARY:
                ch->pcdata->currhome = 21159;
                break;
              case CLASS_SHAMAN:
                ch->pcdata->currhome = 21107;
                break;
              default:
                ch->pcdata->currhome = 21043;
                break;
            }
          }
          else
          {
            write_to_buffer( d, "Please choose Kragthor or Bloodrock.\n\r", 0 );
            d->connected = CON_CHOOSE_HOMETOWN;
            return;
          }
        }

	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	log_string( log_buf );
	wiznet( ch, WIZ_NEWBIE, 0, log_buf );
	write_to_buffer( d, "\n\r", 2 );

        if( d->color ) SET_BIT( ch->act, PLR_COLOUR );
        else REMOVE_BIT( ch->act, PLR_COLOUR );

	lines = ch->pcdata->pagelen;
	ch->pcdata->pagelen = 22;
	if ( IS_HERO( ch ) )
	    do_help( ch, "_imotd" );
	do_help( ch, "_motd" );
	write_to_buffer( d, "\n\r", 2 );
	ch->pcdata->pagelen = lines;

        do_help( ch, "_login_menu" );

	d->connected = CON_MENU;
        break;

    case CON_GET_COLOR:
	switch ( argument[0] )
	{
	case '\0': d->color = TRUE;
		   argument[0] = '1';
		   argument[1] = '\0';
		   break;
	case '1': d->color = TRUE;	break;
	case '0': d->color = FALSE;	break;
        case '9': d->color = TRUE;
           write_to_buffer( d, "State thy name: ", 0 );
           d->connected = CON_GET_NAME;
           return;
           break;
	default:
            write_to_buffer( d, "Invalid terminal type.  0 for ASCII, 1 for ANSI, 9 for Quick.  Enter your terminal type: ", 0 );
	    return;
	}

    /*
     * Send the greeting.
     *
     * We check them for color because we don't have a ch to compare
     * against for the ansi/ascii greetings
     */
      {

    if( d->color )
        {
	if ( help_greeting[0] == '.' )
	    write_to_buffer( d, help_greeting_color+1, 0 );
	else
	    write_to_buffer( d, help_greeting_color  , 0 );
        }
        else
        {
	if ( help_greeting[0] == '.' )
	    write_to_buffer( d, help_greeting+1, 0 );
	else
	    write_to_buffer( d, help_greeting  , 0 );
        }
      }
        d->connected = CON_GET_NAME;
        return;
        break;

    case CON_MENU:
        if( ( argument[0] == '0' ) )
        {
          send_to_char( "Goodbye.\n\r", ch );
          sprintf( log_buf, "%s has left the game.", ch->name );
          wiznet( ch, WIZ_LOGINS, get_trust( ch ), log_buf );
          save_char_obj ( ch );
          extract_char( ch, TRUE );
          last_fun( "nanny");
          if( d )
            close_socket( d );
          return;
        } 
        else if( ( argument[0] == '1' ) )
        {
          reset_stats( ch );
          sprintf( buf, "Character %s entering the realm from the menu.", ch->name );
          log_string( buf );
        }
        else if( ( argument[0] == '2' ) )
        {
          do_password( ch, "" );
          return;
        }
        else if( ( argument[0] == '6' ) )
        {
          do_retire( ch, "" );
          return;
        }
        else
        {
          send_to_char( "Wrong option.\n\r", ch );
          send_to_char( "\n\r", ch );
          do_help( ch, "_login_menu" );
          return;
        }

	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;

        // Check the character's position on the fraglist -- Veygoth.  This ensures proper
        // updates.  No immortals allowed on the fraglist.
        if( !IS_IMMORTAL( ch ))
        {
        if( ch->pcdata->frags > 0 )
        {
          resort_fraglist( ch, fraglist.top_frags );
          resort_fraglist( ch, fraglist.race_top_frags[ch->race] );
          resort_fraglist( ch, fraglist.class_top_frags[ch->class] );
          save_fraglist( );
        }
        else if( ch->pcdata->frags < 0 )
        {
          resort_fraglist( ch, fraglist.bottom_frags );
          resort_fraglist( ch, fraglist.race_bottom_frags[ch->race] );
          resort_fraglist( ch, fraglist.class_bottom_frags[ch->class] );
          save_fraglist( );
        }
        }


	send_to_char(
    "\n\r&nWelcome to &+RMag&+rma&n.  May your visit here be ... Eventful.\n\r",
	    ch );

        // New_hitpoints stuff can be removed after next pwipe -- Veygoth 6/4/00
        if( ch->level == 0 )
            ch->pcdata->new_hitpoints = 1;

        if( !ch->pcdata->new_hitpoints )
            reroll_hitpoints( ch );

	if ( ch->level == 0 )
	{
            SET_BIT( ch->act, PLR_JUST_DIED ); // Use this for new chars too -- Veygoth
            ch->pcdata->created = (int)current_time;
	    ch->level	= 1;
	    ch->exp	= 1;
            ch->money.copper = 0;
            ch->money.silver = 0;
	    ch->money.gold = 0;
            ch->money.platinum = 0;
            // veygoth - con mod to newbie hitpoints for greater variation
            // keep in mind we're *still* using get_max_hit on top of this
            ch->max_hit += (get_curr_con( ch ) / 30);
	    ch->hit	= get_max_hit( ch );
            if( ch->class == CLASS_PSIONICIST )
            {
              // Mana bonuses for newbies -- Veygoth
              ch->max_mana += (get_curr_int( ch ) / 10);
              ch->max_mana += (get_curr_wis( ch ) / 10);
              ch->max_mana += (get_curr_wis( ch ) / 10);
            }
	    ch->mana	= ch->max_mana;
            if( ch->race == RACE_CENTAUR )
              ch->max_move += 80;
	    ch->move	= ch->max_move;
            ch->size    = race_table[ch->race].size;
            ch->alignment = race_table[ch->race].base_align;            
            ch->pcdata->rank = RANK_NORMAL;
            if( ch->class == CLASS_PALADIN )
            {
                ch->alignment = 1000;
            }
            else if( ch->class == CLASS_ANTIPALADIN )
            {
                ch->alignment = -1000;
            }
            else if( ch->class == CLASS_RANGER )
            {
                ch->alignment += 250;
            }
            else if( ch->class == CLASS_ASSASSIN )
            {
                ch->alignment -= 100;
            }
            else if( ch->class == CLASS_CLERIC )
            {
                if( ch->alignment < 0 )
                  ch->alignment -= 250;
                else if( ch->alignment > 0 )
                  ch->alignment += 250;
            }

            if( ch->alignment < -1000 ) ch->alignment = -1000;
            else if( ch->alignment > 1000 ) ch->alignment = 1000;

            if( ch->class == CLASS_PSIONICIST )
            {
               ch->mana = 45 + ( get_curr_pow( ch ) / 10 );
            }

            // Set character's height and weight based on their race
            // using a generic 87.5-112.5% / 80-120% of race standard value
            ch->pcdata->height = number_range( ((race_table[ch->race].height * 7) / 8 ), ((race_table[ch->race].height * 9 ) / 8 ));
            ch->pcdata->weight = number_range( ((race_table[ch->race].weight * 8) / 10 ), ((race_table[ch->race].weight * 12 ) / 10 )); 

            sprintf( buf, "&n" );
            set_title( ch, buf );
            set_skills( ch ); // Sets skills that player has to minimum
                              // levels - Veygoth
            set_spells( ch ); // Sets newbie spells to minimum levels
                              // -- Veygoth
	    free_string( ch->pcdata->prompt );
	    ch->pcdata->prompt = str_dup( 
               "&+L<%h/%Hhp %v/%Vmv>\n\r<&n%T %E&+L>&n " );

            SET_BIT( ch->act, PLR_PAGER );

            newbie_eq_char( ch );

            if( ch->pcdata->currhome && ch->pcdata->orighome )
            {
              char_to_room( ch, get_room_index( ch->pcdata->currhome ));
            }
            else if( ch->pcdata->currhome )
            {
              ch->pcdata->orighome = ch->pcdata->currhome;
              char_to_room( ch, get_room_index( ch->pcdata->currhome ));
            }
            else if( !get_room_index( repop_point[ch->race][ch->class] ))
            {
              send_to_char( "The starting room for your race/class does not exist.  Please bug this.\n\r", ch );
  	      char_to_room( ch, get_room_index( ROOM_VNUM_START ) );
            }
            else
            {
              ch->pcdata->orighome = repop_point[ch->race][ch->class];
              ch->pcdata->currhome = ch->pcdata->orighome;
              char_to_room( ch, get_room_index( repop_point[ch->race][ch->class] ));
            }

            // Give them the universal language
            ch->pcdata->lang_lrn[LANG_UNKNOWN] = 100;

            // Give them their own language
            if( race_table[ch->race].language > 0 && race_table[ch->race].language < MAX_LANG )
            {
              ch->pcdata->lang_lrn[race_table[ch->race].language] = 100;
            }

            save_char_obj( ch );
            send_to_char( "\n\rWelcome to the world of Magma!\n\r"
                          "\n\rNew players should check out HELP INDEX and HELP NEWBIE"
                          "\n\rAnd use the TOGGLE command to change various options.\n\r\n\r", ch );
	} // end if(ch->level == 0)
        else if( ch->pcdata->lastrent != 0 && (ch->pcdata->lastrent != ch->in_room->vnum )
                && get_room_index( ch->pcdata->lastrent ))
        {
            send_to_char( "\n\rReturning you to the last place you rented, camped or quit...\n\r", ch );
            // Veygoth - Added this so that players get all their hitpoints back on a crash.
            ch->hit = get_max_hit( ch );
            char_to_room( ch, get_room_index( ch->pcdata->lastrent ));
        }
        // this is to prevent people popping in zones after a crash - Lithos
        else if ( !ch->pcdata->lastrent && get_room_index( ch->pcdata->currhome ) ) {
            char_to_room( ch, get_room_index( ch->pcdata->currhome ) );
        }
	else if ( ch->in_room )
	{
	    char_to_room( ch, ch->in_room );
	}
	else if ( IS_IMMORTAL( ch ) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}

	if ( !IS_SET( ch->act, PLR_WIZINVIS )
	    && !is_affected( ch, AFF_INVISIBLE ) )
            act( "$n&n has entered the realm.", ch, NULL, NULL, TO_ROOM );

        sprintf( log_buf, "%s has entered the realm.", ch->name );
        wiznet( ch, WIZ_LOGINS, get_trust( ch ), log_buf );

        if( IS_SET( ch->act, PLR_JUST_DIED ))
          create_event( EVENT_REMOVE_JUSTDIED, 20, ch, NULL, 0 );

        if( ch->hit < 1 )
          ch->hit = get_max_hit( ch ) / 3;
        reset_stats( ch );
	do_look( ch, "auto" );
	/* check for new notes */
	notes = 0;

	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
		&& pnote->date_stamp > ch->last_note )
	        notes++;

	if ( notes == 1 )
            send_to_char( "\n\r&+LYou have one new &+Wnote waiting.&n\n\r", ch );
	else
	    if ( notes > 1 )
	    {
                sprintf( buf, "\n\r&+LYou have %d new &+Wnotes&+L waiting.&n\n\r",
			notes );
		send_to_char( buf, ch );
	    }

	break;

    case CON_PASSWD_GET_OLD:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif
        if ( argument[0] == '\0' )
	{
	    write_to_buffer( d, "Password change aborted.\n\r", 0 );
	    write_to_buffer( d, echo_on_str, 0 );
	    d->connected = CON_MENU;
	    return;
	}

	if ( strcmp( argument, ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    write_to_buffer( d, echo_on_str, 0 );
            do_help( ch, "_login_menu" );
	    d->connected = CON_MENU;
	    return;
	}

	write_to_buffer( d, "New password: ", 0 );
	d->connected = CON_PASSWD_GET_NEW;
	break;

    case CON_PASSWD_GET_NEW:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif
        if ( argument[0] == '\0' )
	{
	    write_to_buffer( d, "Password change aborted.\n\r", 0 );
	    write_to_buffer( d, echo_on_str, 0 );
	    d->connected = CON_MENU;
	    return;
	}

	if ( strlen( argument ) < 5 )
	{
	    write_to_buffer( d,
	       "Password must be at least five characters long.\n\rNew password: ",
		0 );
	    return;
	}

	pwdnew = argument;
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rNew password: ",
		    0 );
		return;
	    }
	}

	strcpy( buf, pwdnew );
	free_string( ch->pcdata->pwdnew );
	ch->pcdata->pwdnew = str_dup( buf );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_PASSWD_CONFIRM_NEW;
	break;

    case CON_PASSWD_CONFIRM_NEW:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( argument, ch->pcdata->pwdnew ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_PASSWD_GET_NEW;
	    return;
	}

	strcpy( buf, ch->pcdata->pwdnew );
	free_string( ch->pcdata->pwdnew );
	ch->pcdata->pwdnew = str_dup( "" );
	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( buf );
	write_to_buffer( d, echo_on_str, 0 );
	ch->desc->connected = CON_MENU;
        write_to_buffer( d, "Password successfully changed.\n\r", 0 );
        do_help( ch, "_login_menu" );
	break;


    case CON_RETIRE_GET_PASSWORD:
#if defined( unix ) || defined( AmigaTCP )
	write_to_buffer( d, "\n\r", 2 );
#endif
	write_to_buffer( d, echo_on_str, 0 );

        if ( argument[0] == '\0' )
	{
	    write_to_buffer( d, "Retire aborted.\n\r", 0 );
            do_help( ch, "_login_menu" );
	    d->connected = CON_MENU;
	    return;
	}

	if ( strcmp( argument, ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
            do_help( ch, "_login_menu" );
	    d->connected = CON_MENU;
	    return;
	}

	write_to_buffer( d, "Are you sure (Y/N)? ", 0 );
	d->connected = CON_RETIRE_CONFIRM;
	break;

    case CON_RETIRE_CONFIRM:
	switch ( argument[0] )
	{
	  case 'y': case 'Y': break;
	  default:
	      write_to_buffer( d, "Retire aborted.\n\r", 0 );
              do_help( ch, "_login_menu" );
	      d->connected = CON_MENU;
	      return;
	}

	write_to_buffer( d, "Hope you return soon brave adventurer!\n\r", 0 );

        act( "$n&n has retired from the realm.", ch, NULL, NULL, TO_ROOM );
        sprintf( log_buf, "%s has retired from the realm.", ch->name );
	log_string( log_buf );
	wiznet( ch, WIZ_LOGINS, get_trust( ch ), log_buf );

	/*
	 * After extract_char the ch is no longer valid!
	 *
	 * By saving first i assure i am not removing a non existing file
	 * i know it's stupid and probably useless but... (Zen).
	 */
	save_char_obj( ch );
	delete_char_obj( ch ); /* handy function huh? :) */

	extract_char( ch, TRUE );
        last_fun( "nanny");
	if ( d )
	    close_socket( d );
	break;

    }

    return;
}

void newbie_eq_char( CHAR_DATA *ch )
{
            int vnum;
            int count;
            OBJ_DATA *obj;

            vnum = number_range( OBJ_VNUM_NEWBIE_VEST,
                   ( OBJ_VNUM_NEWBIE_VEST + NUM_NEWBIE_VEST - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_BODY );

            vnum = number_range( OBJ_VNUM_NEWBIE_HELM,
                   ( OBJ_VNUM_NEWBIE_HELM + NUM_NEWBIE_HELM - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_HEAD );

            vnum = number_range( OBJ_VNUM_NEWBIE_SLEEVES,
                   ( OBJ_VNUM_NEWBIE_SLEEVES + NUM_NEWBIE_SLEEVES - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
            equip_char( ch, obj, WEAR_ARMS );

            if( ch->race != RACE_CENTAUR )
            {
            vnum = number_range( OBJ_VNUM_NEWBIE_PANTS,
                   ( OBJ_VNUM_NEWBIE_PANTS + NUM_NEWBIE_PANTS - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
            equip_char( ch, obj, WEAR_LEGS );
            }

            if( ch->race != RACE_CENTAUR )
            {
            vnum = number_range( OBJ_VNUM_NEWBIE_BOOTS,
                   ( OBJ_VNUM_NEWBIE_BOOTS + NUM_NEWBIE_BOOTS - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_FEET );
            }

            vnum = number_range( OBJ_VNUM_NEWBIE_CLOAK,
                   ( OBJ_VNUM_NEWBIE_CLOAK + NUM_NEWBIE_CLOAK - 1) );
	    obj = create_object( get_obj_index( vnum ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_ABOUT );

	    obj = create_object( 
				get_obj_index( class_table[ch->class]->weapon ),
				0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_HAND );

            for( count = 0; count < 5; count++ )
            {
              obj = create_object( get_obj_index( OBJ_VNUM_IRON_RATION ), 0 );
              SET_OBJ_STAT( obj, ITEM_NOSELL );
              obj_to_char( obj, ch );
            }

            obj = create_object( get_obj_index( OBJ_VNUM_WATERSKIN ), 0 );
            SET_OBJ_STAT( obj, ITEM_NOSELL );
            obj_to_char( obj, ch );

            if( ch->class == CLASS_SORCERER )
            {
              obj = create_object( get_obj_index( OBJ_VNUM_QUILL ), 0 );
              SET_OBJ_STAT( obj, ITEM_NOSELL );
              obj_to_char( obj, ch );
            }

            if( ch->class == CLASS_SHAMAN )
            {
              obj = create_object( get_obj_index( OBJ_VNUM_NEWBIE_TOTEM_A ), 0 );
              SET_OBJ_STAT( obj, ITEM_NOSELL );
              obj_to_char( obj, ch );
              obj = create_object( get_obj_index( OBJ_VNUM_NEWBIE_TOTEM_E ), 0 );
              SET_OBJ_STAT( obj, ITEM_NOSELL );
              obj_to_char( obj, ch );
              obj = create_object( get_obj_index( OBJ_VNUM_NEWBIE_TOTEM_S ), 0 );
              SET_OBJ_STAT( obj, ITEM_NOSELL );
              obj_to_char( obj, ch );
            }

          if( !IS_SET( race_table[ch->race].race_abilities, RACE_ULTRAVISION ) )
          {
              obj = create_object( get_obj_index( OBJ_VNUM_TORCH ), 0 );
              obj_to_char( obj, ch );
              obj = create_object( get_obj_index( OBJ_VNUM_TORCH ), 0 );
              obj_to_char( obj, ch );
              obj = create_object( get_obj_index( OBJ_VNUM_TORCH ), 0 );
              obj_to_char( obj, ch );
          }

            return;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_name( name, "all auto imm immortal self someone none . .. ./ ../ /" ) )
	return FALSE;

    /*
     * Obscenities and illegal names in general
     */
    if ( is_name( name, strName ) )
        return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen( name ) <  3 )
	return FALSE;

#if defined( macintosh ) || defined( unix ) || defined( AmigaTCP )
    if ( strlen( name ) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha( *pc ) )
		return FALSE;
	    if ( LOWER( *pc ) != 'i' && LOWER( *pc ) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash [ MAX_KEY_HASH ];
	       MOB_INDEX_DATA *pMobIndex;
	       int             iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    /*
     * Deny the idiots that want to name storage characters stuff
     * like "bobstore"
     */
    if( !str_infix( "stor", name ))
       return FALSE;

    /*
     * Obscenities within name strings are a big nono
     */
    if( !str_infix( "rape", name ))
       return FALSE;
    if( !str_infix( "shit", name ))
       return FALSE;
    if( !str_infix( "piss", name ))
       return FALSE;
    if( !str_infix( "bitch", name ))
       return FALSE;
    if( !str_infix( "fuck", name ))
       return FALSE;

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    last_fun( "check_reconnect" );

    for ( ch = char_list; ch; ch = ch->next )
    {
        if ( ch->deleted )
	    continue;

	if ( !IS_NPC( ch )
	    && ( !fConn || !ch->desc )
	    && !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc     = d;
		ch->timer    = 0;
		d->connected = CON_PLAYING;
		send_to_char( "Reconnecting.\n\r", ch );
                act( "$n&+L has returned from the &+RA&n&+rb&+Rys&n&+rs&+L.&n", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string( log_buf );
	    }
	    return TRUE;
	}
    }

    return FALSE;
}

/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;
    CHAR_DATA       *ch;
    CHAR_DATA       *new_ch;

    last_fun( "check_playing");

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
        ROOM_INDEX_DATA *room;
	if (   /* There's a different desc. */
               dold != d
               /* That desc. has a char. */
	    && dold->character
               /* The desc. is not in the process of getting a name */
	    && dold->connected != CON_GET_NAME
               /* The desc.'s name matches the char we're loading. */
	    && !str_cmp( name, dold->original
			? dold->original->name : dold->character->name )
               /* The desc. isn't deleted. */
	    && !dold->character->deleted )
	{
               /* Tell the new char that we're loading up the old player. */
            write_to_buffer( d, "Navigating the Abyss...\n\r", 0 );
               /* Handle switched chars. */
            ch = ( dold->original ? dold->original : dold->character );
               /* Verify that the old character has a room. */
            if( !ch->in_room )
            {
              log_string( "Character has no ch->in_room in check_playing" );
              ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
            }

            sprintf( log_buf, "Overriding connection for %s.", ch->name );
            wiznet( ch, WIZ_LOGINS, get_trust( ch ), log_buf );
 
               /* Swap characters and destroy dold. */
               /* Preserve d->character. */
            new_ch = d->character;
               /* Set d's characters to dold's. */
            d->original = dold->original;
            d->character = dold->character;
            ch->desc = d;
               /* Set dold's characters. (loading chars can't be switched. ) */
            dold->original = NULL;
            dold->character = new_ch;
            new_ch->desc = dold;
               /* Set the connection status. */
            if( dold->connected == CON_PLAYING )
            {
              d->connected = CON_PLAYING;
            }
            else
            {
              d->connected = CON_MENU;
              do_help( d->character, "_login_menu" );
            }
               /* Get rid of dold. */
            log_string( "Closing old socket" );
            if( d->connected == CON_PLAYING )
            {
                room = ch->in_room;
                char_from_room( ch );
            }
            close_socket( dold );
            if( d->connected == CON_PLAYING )
            {
                char_to_room( ch, room );
                act( "$n&+L has returned from the &+RA&n&+rb&+Rys&n&+rs&+L.&n", ch, NULL, NULL, TO_ROOM );
            }
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    last_fun( "stop_idling");

    if (   !ch
	|| !ch->desc
	||  ch->desc->connected != CON_PLAYING
	|| !ch->was_in_room
	||  ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    last_fun( "stop_idling");
    ch->was_in_room	= NULL;
    act( "$n&+L has returned from the &+RA&n&+rb&+Rys&n&+rs&+L.&n", ch, NULL, NULL, TO_ROOM );
    return;
}

/*
 * Write to all in the room.
 */
void send_to_room( const char *txt, ROOM_INDEX_DATA *room )
{
    DESCRIPTOR_DATA *d;
    
    last_fun( "send_to_room" );

    for ( d = descriptor_list; d; d = d->next )
        if ( d->character != NULL )
	    if ( d->character->in_room == room )
	        act( txt, d->character, NULL, NULL, TO_CHAR );
}

/*
 * Write to all characters.
 */
void send_to_all_char( const char *text )
{
    DESCRIPTOR_DATA *d;

    last_fun( "send_to_all_char" );

    if ( !text )
        return;
    for ( d = descriptor_list; d; d = d->next )
        if ( IS_PLAYING( d ) )
	    send_to_char( text, d->character );

    return;
}

/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( !ch ) return;
    if( !txt || !ch->desc )
        return;

    last_fun( "send_to_char_bw" );

    /*
     * Bypass the paging procedure if the text output is small
     * Saves process time.
     */
    if( strlen( txt ) < 600 || !IS_SET( ch->act, PLR_PAGER ) )
	write_to_buffer( ch->desc, txt, strlen( txt ) );
    else
    {
        free_string( ch->desc->showstr_head );
	ch->desc->showstr_head  = str_dup( txt );
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string( ch->desc, "" );
    }

    return;
}

/*
 * Send to one char, new colour version, by Lope.
 * Enhanced by Zen.
 *
 * Modified by Veygoth to use Basternae/Duris/Tavril's Editor-style color codes
 * Colors modified by Sniktiorg
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const       char    *point;
		char    *point2;
		char     buf     [ MAX_STRING_LENGTH * 8 ];
		int      skip = 0;

    last_fun( "send_to_char" );

    if ( !ch ) return;
    if( !txt || !ch->desc || ch->deleted )
	return;
    buf[0] = '\0';
    point2 = buf;

    for ( point = txt ; *point ; point++ )
    {
	if ( *point == '&' )
	{
	    point++;
            // only colorize shit that needs to be colorized... otherwise
            // don't worry about it
            if( *point != 'N' && *point != 'n' && *point != '+' && *point != '-' )
            {
              point--;
            }
        else
        {
	 if ( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		skip = colour( point, ch, point2 );
		while( skip-- > 0 )
		    ++point2;
		if( *point != 'n' && *point != 'N')
		    point++;
		continue;
	    }
	    if ( *point == '&' )        /* if !IS_SET( ch->act, PLR_COLOUR ) */
	    {
		*point2 = *point;
		*++point2 = '\0';
	    }

	    // was just point++, but since we added another character to the
	    // color function, we needed to take that into account.
	    // so if we don't find an "N", we skip the next two characters
	    // ( "+r" or "+R" )  if we do find the N we just skip it 
	    // should fix the character gobbling for non-color folks
	    // -- Veygoth
	    if( *point != 'n' && *point != 'N')
	       point++;
	    continue;
	}
       }
	*point2 = *point;
	*++point2 = '\0';
    }

    *point2 = '\0';

    /*
     * Bypass the paging procedure if the text output is small
     * Saves process time.
     */
    if( strlen( buf ) < 500 || !IS_SET( ch->act, PLR_PAGER ) )
	write_to_buffer( ch->desc, buf, strlen( buf ) );
    else
    {
	free_string( ch->desc->showstr_head );
	ch->desc->showstr_head  = str_dup( buf );
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string( ch->desc, "" );
    }

    return;
}

// Parses &+ based color codes - Veygoth
int colour(const char *type, CHAR_DATA *ch, char *string )
{
    char        code[ 20 ];
    char        *p = '\0';
    if( IS_NPC( ch ) )
      {
	return( 0 );
      }

    if( *type == 'n' || *type == 'N' )
	sprintf( code, "[0m" );
    else if( *type == '+' )
      {
	switch( *(type + 1) )
	{
	default:
            if( IS_IMMORTAL( ch ) )
                send_to_char( "Problem with color conversion: + found,"
                  " no color.\n\r", ch );
	    break;
	case 'l':
            sprintf( code, FG_B_BLACK );
	    break;
	case 'b':
	    sprintf( code, FG_BLUE );
	    break;
	case 'r':
	    sprintf( code, FG_RED );
	    break;
	case 'y':
	    sprintf( code, FG_YELLOW );
	    break;
	case 'm':
	    sprintf( code, FG_MAGENTA );
	    break;
	case 'c':
	    sprintf( code, FG_CYAN );
	    break;
	case 'w':
	    sprintf( code, FG_WHITE );
	    break;
	case 'g':
	    sprintf( code, FG_GREEN );
	    break;
	case 'L':
	    sprintf( code, FG_B_BLACK );
	    break;
	case 'B':
	    sprintf( code, FG_B_BLUE );
	    break;
	case 'R':
	    sprintf( code, FG_B_RED );
	    break;
	case 'Y':
	    sprintf( code, FG_B_YELLOW );
	    break;
	case 'M':
	    sprintf( code, FG_B_MAGENTA );
	    break;
	case 'C':
	    sprintf( code, FG_B_CYAN );
	    break;
	case 'W':
	    sprintf( code, FG_B_WHITE );
	    break;
	case 'G':
	    sprintf( code, FG_B_GREEN );
	    break;
	}
      }
    else if( *type == '-' )
      {
	switch( *(type + 1) )
	{
	default:
            if( IS_IMMORTAL( ch ) )
                send_to_char( "Problem with color conversion: - found,"
                    " no color", ch );
	    break;
	case 'l':
        case 'L':
	    sprintf( code, BG_BLACK );
	    break;
	case 'g':
        case 'G':
	    sprintf( code, BG_GREEN );
	    break;
	case 'm':
        case 'M':
	    sprintf( code, BG_MAGENTA );
	    break;
	case 'r':
        case 'R':
	    sprintf( code, BG_RED );
	    break;
	case 'w':
        case 'W':
	    sprintf( code, BG_WHITE );
	    break;
	case 'y':
        case 'Y':
	    sprintf( code, BG_YELLOW );
	    break;
	case 'b':
        case 'B':
	    sprintf( code, BG_BLUE );
	    break;
	case 'c':
        case 'C':
	    sprintf( code, BG_CYAN );
	    break;
	}
      }


    p = code;
    while( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }
    return( strlen( code ) );
}

// returns a coloured version of buffer.
char *coloured (char *buffer, CHAR_DATA *ch)
{
    static char buf [ MAX_STRING_LENGTH ];
    colourconv (buf, buffer, ch);
    return buf;
}


// Uses &+ color codes - Veygoth
void colourconv( char *buffer, const char *txt , CHAR_DATA *ch )
{
   const char   *point;
   int           skip = 0;

   if ( !txt )
	return;

   if ( ch->desc && IS_SET( ch->act, PLR_COLOUR ) )
	for( point = txt ; *point ; point++ )
	{
 	  if ( *point == '&' )
	  {
	    point++;
            // only colorize shit that needs to be colorized... otherwise
            // don't worry about it
            if( *point != 'N' && *point != 'n' && *point != '+' && *point != '-' )
            {
              point--;
            }
            else
            {
		skip = colour( point, ch, buffer );
		while( skip-- > 0 )
		   ++buffer;
		if( *point != 'n' && *point !='N' )
		   point++;
		continue;
            }
	   }
	   *buffer = *point;
	   *++buffer = '\0';
	}
   else
	for( point = txt ; *point ; point++ )
	{
	    if( *point == '&' )
	    {
		point++;
                if( *point != 'n' && *point !='N' )
		  point++;
		continue;
	    }
	    *buffer = *point;
	    *++buffer = '\0';
	}

    *buffer = '\0';
    return;
}

/*
 * source: EOD, by John Booth <???>
 * Got it from Erwin S.A. code - Zen
 */
void printf_to_char( CHAR_DATA *ch, char *fmt, ... )
{
    char        buf [ MAX_STRING_LENGTH ];
    va_list     args;

    va_start ( args, fmt );
    vsprintf ( buf, fmt, args );
    va_end ( args );

    send_to_char( buf, ch );
    return;
}

 /* The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom
    for porting this SillyMud code for MERC 2.0 and laying down the groundwork.
    Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which
    the improvements to the pager was modeled from.  - Kahn */
 /* 12/1/94 Fixed bounds and overflow bugs in pager thanks to BoneCrusher
    of EnvyMud Staff - Kahn */

void show_string( struct descriptor_data *d, char *input )
{
    register char *scan;
             char  buffer[ MAX_STRING_LENGTH*12 ];
             char  buf   [ MAX_INPUT_LENGTH    ];
             int   line      = 0;
             int   toggle    = 0;
             int   pagelines = 20;

    one_argument( input, buf );

    switch( UPPER( buf[0] ) )
    {
    case '\0':
    case 'C': /* show next page of text */
	break;

    case 'R': /* refresh current page of text */
	toggle = 1;
	break;

    case 'B': /* scroll back a page of text */
	toggle = 2;
	break;

    default: /*otherwise, stop the text viewing */
	if ( d->showstr_head )
	{
	    free_string( d->showstr_head );
	    d->showstr_head = str_dup( "" );
	}
	d->showstr_point = 0;
	return;

    }

    if ( d->original )
        pagelines = d->original->pcdata->pagelen;
    else
        pagelines = d->character->pcdata->pagelen;

    if ( toggle )
    {
	if ( d->showstr_point == d->showstr_head )
	    return;
	if ( toggle == 1 )
	    line = -1;
	do
	{
	    if ( *d->showstr_point == '\n' )
	      if ( ( line++ ) == ( pagelines * toggle ) )
		break;
	    d->showstr_point--;
	} while( d->showstr_point != d->showstr_head );
    }
    
    line    = 0;
    *buffer = 0;
    scan    = buffer;
    if ( *d->showstr_point )
    {
	do
	{
	    *scan = *d->showstr_point;
	    if ( *scan == '\n' )
	      if ( ( line++ ) == pagelines )
		{
		  scan++;
		  break;
		}
	    scan++;
	    d->showstr_point++;
	    if( *d->showstr_point == 0 )
	      break;
	} while( 1 );
    }

    /* On advice by Scott Mobley and others */
/*
    *scan++ = '\n';
    *scan++ = '\r';
*/
    *scan = 0;

    write_to_buffer( d, buffer, strlen( buffer ) );
    if ( *d->showstr_point == 0 )
    {
      free_string( d->showstr_head );
      d->showstr_head  = str_dup( "" );
      d->showstr_point = 0;
    }

    return;
}

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1,
	 const void *arg2, int type )
{
           OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
	   OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
	   CHAR_DATA       *to;
	   CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
    static char *    const  he_she  [ ] = { "it",  "he",  "she" };
    static char *    const  him_her [ ] = { "it",  "him", "her" };
    static char *    const  his_her [ ] = { "its", "his", "her" };
    const  char            *str;
    const  char            *i;
           char            *point;
           char            *pbuff;
           char             buf     [ MAX_STRING_LENGTH ];
           char             buf1    [ MAX_STRING_LENGTH ];
           char             buffer  [ MAX_STRING_LENGTH*2 ];
           char             fname   [ MAX_INPUT_LENGTH  ];
	   unsigned int	    num;

    /*
     * Discard null and zero-length messages.
     */
    if ( !format || format[0] == '\0' )
	return;

    if ( !ch )
    {
    	bug( "Act: null ch!", 0 );
    }

    if ( ch->deleted )
	return;

    // To prevent crashes
    if( !ch->in_room )
        return;

    to = ch->in_room->people;
    if ( type == TO_VICT || type == TO_ROOM_VICT)
    {
	if ( !vch )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    sprintf( buf1, "Bad act string:  %s", format );
	    bug( buf1, 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to; to = to->next_in_room )
    {
	if ( to->deleted
	    || ( !to->desc && IS_NPC( to ) &&
                !IS_SET( to->pIndexData->progtypes, ACT_PROG ) )
	    || !IS_AWAKE( to ) )
	    continue;

        if ( type == TO_ROOM_VICT && vch->fly_level != to->fly_level)
            continue;

        if ( (type == TO_ROOM || type == TO_NOTVICT) && ch->fly_level != to->fly_level)
            continue;
        if (type == TO_ROOM_VICT && (to == ch || to == vch) )
            continue;
	if ( type == TO_CHAR    && to != ch )
	    continue;
	if ( type == TO_VICT    && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM    && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;
        if ( type == TO_ABOVE && ( to->fly_level <= ch->fly_level ) )
            continue;
        if ( type == TO_BELOW && ( to->fly_level >= ch->fly_level ) )
             continue;

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( !arg2 && isupper( *str ) )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		sprintf( buf1, "Bad act string:  %s", format );
		bug( buf1, 0 );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
		          sprintf( buf1, "Bad act string:  %s", format );
		          bug( buf1, 0 );
			  i = " <@@@> ";				break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );                         break;
		case 'N': i = PERS( vch, to  );	                        break;
		case 'e': i = he_she  [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'E': i = he_she  [URANGE( 0, vch ->sex, 2 )];	break;
		case 'm': i = him_her [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'M': i = him_her [URANGE( 0, vch ->sex, 2 )];	break;
		case 's': i = his_her [URANGE( 0, ch  ->sex, 2 )];	break;
		case 'S': i = his_her [URANGE( 0, vch ->sex, 2 )];	break;

		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? obj1->short_descr
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
			    ? obj2->short_descr
			    : "something";
		    break;

		case 'd':
		    if ( !arg2 || ( (char *) arg2 )[0] == '\0' )
		    {
			i = "door";
		    }
		    else
		    {
			one_argument( (char *) arg2, fname );
			i = fname;
		    }
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

        *point++	= '\n';
        *point++	= '\r';
        *point		= '\0';

	/* needed to capitalize because of ColourUp */
	for ( num = 0; num <= strlen( buf ) ; num++ )
	{
	    if ( buf[num] == '{' )
		num++;
	    else
	    if ( buf[num] == '&' )
	    {
		if ( buf[num+1] == '+' 
		  || buf[num+1] == '-' )
		    num += 2;
		else
		if ( buf[num+1] == 'n'
		  || buf[num+1] == 'N' )
		    num++;
		else
	        {
		    buf[num] = UPPER( buf[num] );
		    break;
	    	}
	    }
	    else
	    {
	    	buf[num] = UPPER( buf[num] );
	    	break;
	    }
	}

	pbuff		= buffer;
	colourconv( pbuff, buf, to );
        capitalize(buffer);
	if( to->desc )
	    write_to_buffer( to->desc, buffer, 0 );
	if ( MOBtrigger )
        {
	   prog_act_trigger( buffer, to, ch, obj1, vch );
        }
    }

    MOBtrigger = TRUE;
    return;
}

/*
 * Macintosh support functions.
 */
#if defined( macintosh )
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

/*
 * Windows 95 and Windows NT support functions
 */
#if defined( WIN32 )
void gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = (long)time( NULL );
    tp->tv_usec = 0;
}
#endif



void flush_command( DESCRIPTOR_DATA *d )
{
    /* flush one command from flusher */
    strcpy( d->flusher, d->flush_point );

    /* and actualize flush_point */
    d->flush_point = strchr( d->flusher, ';' );

    if ( !d->flush_point )
    {
	strcpy( d->incomm, d->flusher );
	return;
    }

    *d->flush_point = '\0';
    d->flush_point++;
    return;
}

// Returns the appropriate number suffix for a string - Veygoth
char *text_number( int number )
{
     // 11 to 13 are a special case...
     if( number > 10 && number < 14 )
       return "th";
     switch( number % 10 )
     {
       case 1:
         return "st";
         break;
       case 2:
         return "nd";
         break;
       case 3:
         return "rd";
         break;
       default:
         return "th";
         break;
    }
}

void do_showdesc( CHAR_DATA *ch, char *argument )
{
   DESCRIPTOR_DATA *desc;
   int count = 0;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *chr;

   for( desc = descriptor_list; desc; desc = desc->next )
   {
      count++;
      sprintf( buf, "Descriptor %d connected state %d character %s",
         count, desc->connected, desc->character ? desc->character->name : "None" );
      if( desc->character )
      {
        for( chr = char_list; chr; chr = chr->next )
        {
           if( desc->character == chr )
           {
             strcat( buf, " found in char_list by pointer" );
             if( desc->character->deleted )
               strcat( buf, " (deleted)" );
           }
           if( !str_cmp( desc->character->name, chr->name ))
           {
             strcat( buf, " found in char_list by name" );
             if( desc->character->deleted )
               strcat( buf, " (deleted)" );
           }
        }
      }
      strcat( buf, "\n\r" );
      send_to_char( buf, ch );
   }

   return;
}

/* Returns TRUE if and only if the first character is a vowel. 
 *   Note: is_vowel ignores any initial colour codes. */
bool is_vowel ( char * arg ) 
{
    char * point = arg;

	/* Kill colour codes. */
    while ( *point == '&' )
    {
    	point++;
	if ( *point == 'n' || *point == 'N' )
	    point++;
	else
	{
	    if ( *point == '+' || *point == '-' )
	    	point += 2;
	/* Begins with an "&" ? Not a vowel.. */
	    else return FALSE;
	}
    }

    if ( *point == 'a' || *point == 'A' ||
	 *point == 'e' || *point == 'E' ||
	 *point == 'i' || *point == 'I' ||
	 *point == 'o' || *point == 'O' ||
	 *point == 'u' || *point == 'U' )
    	return TRUE;
    return FALSE;
}

/* Returns "A &+rDuergar&n" as opposed to "&+rDuergar".
 * Also "An &+bOgre&n" vs. "&+bOgre&n". */
char *race_name ( CHAR_DATA * ch )
{
    static char	 buf [ MAX_STRING_LENGTH ];

    buf[0] = 'A';
    if ( is_vowel( race_table[ ch->race ].name ) )
    {
    	buf[1] = 'n';
	buf[2] = ' ';
	buf[3] = '\0';
    }
    else
    {
	buf[1] = ' ';
	buf[2] = '\0';
    }

    strcat( buf, race_table[ get_race( ch ) ].colorname );

    return buf;
}

// Adds PC's languages. -Lohrr
void add_languages( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
    {
        bug( "Add_languages called with NPC target!", 0 );
        return;
    }

    /* Everybody knows their own language. - Lohrr */
    ch->pcdata->lang_lrn[race_table[ch->race].language] = 100;
    ch->pcdata->speaking = race_table[ch->race].language;

    if( race_table[ch->race].racewar_side == RACEWAR_EVIL )
    {
        if ( race_table[ch->race].language != LANG_ORC )
            ch->pcdata->lang_lrn[ LANG_ORC ] =
                URANGE( 1, ( get_curr_int( ch ) * 3 ) / 2 + 60, 100 );
        if ( race_table[ch->race].language != LANG_OGRE )
            ch->pcdata->lang_lrn[ LANG_OGRE ] =
                URANGE( 1, get_curr_int( ch ) / 2 - 20, 100 );
        if ( race_table[ch->race].language != LANG_TROLL )
            ch->pcdata->lang_lrn[ LANG_TROLL ] =
                URANGE( 1, get_curr_int( ch ) / 3 - 10, 100 );
        if ( race_table[ch->race].language != LANG_SAURIAL )
            ch->pcdata->lang_lrn[ LANG_SAURIAL ] =
                URANGE( 0, get_curr_int( ch ) / 10 - 5, 100 );
        if ( race_table[ch->race].language != LANG_GOBLIN )
            ch->pcdata->lang_lrn[ LANG_GOBLIN ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 5, 100 );
        if ( race_table[ch->race].language != LANG_GITHYANKI )
            ch->pcdata->lang_lrn[ LANG_GITHYANKI ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 10, 100 );
        if ( race_table[ch->race].language != LANG_DROW )
            ch->pcdata->lang_lrn[ LANG_DROW ] =
                URANGE( 1, get_curr_int( ch ) / 5 + 5, 100 );
        if ( race_table[ch->race].language != LANG_KOBOLD )
            ch->pcdata->lang_lrn[ LANG_KOBOLD ] =
                URANGE( 1, get_curr_int( ch ) / 2 - 40, 100 );
        if ( race_table[ch->race].language != LANG_DUERGAR )
            ch->pcdata->lang_lrn[ LANG_DUERGAR ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 5, 100 );
        if ( race_table[ch->race].language != LANG_RAKSHASA )
            ch->pcdata->lang_lrn[ LANG_RAKSHASA ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 10, 100 );
        if ( race_table[ch->race].language != LANG_FLIND )
            ch->pcdata->lang_lrn[ LANG_FLIND ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 15, 100 );
        if ( race_table[ch->race].language != LANG_MINOTAUR )
            ch->pcdata->lang_lrn[ LANG_MINOTAUR ] =
                URANGE( 1, get_curr_int( ch ) / 2 - 40, 100 );
    }
    else if( race_table[ch->race].racewar_side == RACEWAR_GOOD )
    {
        if ( race_table[ch->race].language != LANG_COMMON )
            ch->pcdata->lang_lrn[ LANG_COMMON ] =
                URANGE( 1, ( get_curr_int( ch ) * 3 ) / 2 + 60, 100 );
        if ( race_table[ch->race].language != LANG_ELVEN )
            ch->pcdata->lang_lrn[ LANG_ELVEN ] =
                URANGE( 1, get_curr_int( ch ) / 3 - 10, 100 );
        if ( race_table[ch->race].language != LANG_DWARVEN )
            ch->pcdata->lang_lrn[ LANG_DWARVEN ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 10, 100 );
        if ( race_table[ch->race].language != LANG_CENTAUR )
            ch->pcdata->lang_lrn[ LANG_CENTAUR ] =
                URANGE( 1, get_curr_int( ch ) / 5, 100 );
        if ( race_table[ch->race].language != LANG_AQUA_ELF )
            ch->pcdata->lang_lrn[ LANG_AQUA_ELF ] =
                URANGE( 0, get_curr_int( ch ) / 10 - 5, 100 );
        if ( race_table[ch->race].language != LANG_GNOME )
            ch->pcdata->lang_lrn[ LANG_GNOME ] =
                URANGE( 1, get_curr_int( ch ) / 5 + 5, 100 );
        if ( race_table[ch->race].language != LANG_HALFLING )
            ch->pcdata->lang_lrn[ LANG_HALFLING ] =
                URANGE( 1, get_curr_int( ch ) / 5 + 15, 100 );
        if ( race_table[ch->race].language != LANG_BARBARIAN )
            ch->pcdata->lang_lrn[ LANG_BARBARIAN ] =
                URANGE( 1, get_curr_int( ch ) / 3 + 5, 100 );
        if ( race_table[ch->race].language != LANG_GITHZERAI )
            ch->pcdata->lang_lrn[ LANG_GITHZERAI ] =
                URANGE( 1, get_curr_int( ch ) / 4 - 10, 100 );
    }

    // Added for halfelves -- Veygoth
    if( ch->race == RACE_HALFELF )
      ch->pcdata->lang_lrn[ LANG_ELVEN ] = 90;

    ch->pcdata->lang_lrn[ LANG_DRAGON ]     = 0;
    ch->pcdata->lang_lrn[ LANG_UNKNOWN ]    = 0;
    ch->pcdata->lang_lrn[ LANG_GOD ]        = IS_IMMORTAL( ch ) ? 100 : 0;
    ch->pcdata->lang_lrn[ LANG_MAGICAL ]    = 0;
    ch->pcdata->lang_lrn[ LANG_THRI_KREEN ] = ch->race == RACE_THRIKREEN ? 100: 0;
    ch->pcdata->lang_lrn[ LANG_ANIMAL ]     = 0;
    ch->pcdata->lang_lrn[ LANG_GNOLL ]      = 0;

}

// New_hitpoints stuff can be removed after next pwipe -- Veygoth 6/4/00
void reroll_hitpoints( CHAR_DATA *ch )
{
   int level;
   int experience;

   if( IS_NPC( ch ) || ch->level < 2 || IS_IMMORTAL( ch ) )
   {
     ch->pcdata->new_hitpoints = 1;
     return;
   }

   level = ch->level;
   experience = ch->exp;

   send_to_char( "&+CRerolling your hitpoints!&n\n\r", ch );

   ch->level    = 1;
   ch->max_hit  = 20 + (get_curr_con( ch ) / 30);
   ch->max_mana = 50;
   if( ch->class == CLASS_PSIONICIST )
   {
     // Mana bonuses for newbies -- Veygoth
     ch->max_mana += (get_curr_int( ch ) / 10);
     ch->max_mana += (get_curr_wis( ch ) / 10);
     ch->max_mana += (get_curr_wis( ch ) / 10);
   }
   ch->hit      = get_max_hit( ch );
   ch->mana     = ch->max_mana;

   for( ; ch->level < level; )
   {
     advance_level( ch, FALSE );
   }

   ch->exp = experience;
   ch->pcdata->new_hitpoints = 1;
   save_char_obj( ch );

   send_to_char( "&+CFinished rerolling your hitpoints.&n\n\r", ch );

   send_to_char( "\n\r\n\r&+CYour hitpoints have been rerolled because of a strange code\n\r", ch );
   send_to_char( "\n\r&+Cbug that caused you to gain the wrong amount of hitpoints for\n\r", ch );
   send_to_char( "\n\r&+Cyour class and level.  Everyone is being rerolled.  If this\n\r", ch );
   send_to_char( "\n\r&+Ccreates any balance/imbalance issues, we will address them\n\r", ch );
   send_to_char( "\n\r&+Caccordingly.\n\r", ch );

   return;
}
