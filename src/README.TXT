Magma MUD version 3.04

https://github.com/Xangis/magma

=== The src directory

This file gives a brief description of the files in the src directory.

	READMEs
	Makefiles
	source files
	startup scripts
	executable file
	What is new, what is old in the source



=== The READMEs

README		- Is this file.
README.SSM	- Is the README info on Fusion's SSM 2.2.1


=== The Makefiles

* NOTE *
The only makefile that has been kept current is the makefile for STANDARD,
which works on my Linux system.  I don't know anything about any other
systems, so it is up to you to figure out the makefile for a nonstandard
UNIX system.  Sorry if this causes you a problem. - Veygoth.
* END OF NOTE *

Makefile	- Is the top level Makefile to build multiple platforms.
(Other platform makefiles must be created manually)

To compile the executable file 'magma', type 'make' and view the
available platforms.  Then type 'make <system> >&! a.out &' where
<system> is they supported system.

To compile under Redhat Linux 5.2 or 6.1, type make std.

There is also a Visual C++ 6.0 project file included for people who want
to run under windown 9x

We suggest you modify the Makefile to compile with DEBUG = -ggdb if
you have gdb on your system.  This turns on specific debugging for use
with gdb.

We do not guarantee each Makefile will work for your system as each
system administrator will modify to his/her liking.  The Makefile are
only to be used as a close guideline for your system.

We extend a hearty thanks to all who have tested Envy source on these
ports:

cbjones1@unity.ncsu.edu
blitter@planetx.lanz.com
msmith@quix.robins.af.mil
ajohnson@spock2.tacom.army.mil
rks@ipdinc.com
stoked@iastate.edu
hub@hub.eden.com
erwin@pip.dknet.dk
veygoth@stax.net

=== The source files

act_clan.c	- Contains procedures for clans
act_comm.c	- Contains procedures for pc to pc or pc to mud communication
act_info.c	- Contains procedures for information reports for the pc
act_move.c	- Contains procedures for pc/mobile movement around the mud
act_obj.c	- Contains procedures for object manipulation
act_wiz.c	- Contains procedures for immortal commands
bit.c		- Contains tables used within OLC
comm.c		- Contains procedures for game TCP/IP communications
const.c		- Contains tables used within the game
db.c		- Contains procedures for data file loading
event.c         - Contains procedures for the event system
fight.c		- Contains procedures for pc/mob combat
fraglist.c      - Contains procedures used with the fraglist
handler.c	- Contains procedures for data element handling
interp.c	- Contains procedures and tables for game user commands
magic.c		- Contains procedures for magic and spellcasting
map.c           - Contains procedures related to the ANSI world maps
merc.h		- Contains data types/prototypes/macros used within the game
save.c		- Contains procedures for player/mob file read and write
ssm.c		- Contains procedures for string management
update.c	- Contains procedures for game operation
mem.c		- Contains procedures for OLC mem operations
olc.c		- Contains procedures for OLC commands
olc_act.c	- Contains procedures for OLC
olc_save.c	- Contains procedures for OLC area file write
songs.c         - Contains procedures for Bard songs
spells.c        - Contains procedures for spells
string.c	- Contains procedures for OLC's string editor
mob_prog.c	- Contains procedures for MobPrograms
mob_comm.c	- Contains procedures for mob commands (used by MobPrograms)
tables.c	- Contains tables/procs used for class/social loading/saving
track.c		- Contains procedures for hunt/track
traps.c         - Contains procedures for traps
vehicle.c       - Contains procedures used with vehicles and ships
wiznet.c	- Contains procedures for wiznet
gamble.c	- Contains procedures for player gambling.
spec_mob.c	- Contains procedures for mobile special abilities
spec_obj.c	- Contains procedures for object special abilities


=== The startup scripts

start		- Is the start script for the startup script.  System errors
		  are saved in a file called errors.out in the log directory.
startup		- Is the startup script for EnvyMud using CSH
startupSH	- Is the startup script for EnvyMud using SH

On most systems, typing 'startup &' in the src directory is adequate for you.

If you wish a more indepth logging feature, 'start' will give you that.

The startup script will create a number log file starting from 1000.log in
the log directory.  Should your game crash, and you have enabled core dumps
by setting your 'LIMIT coredumpsize unlimited' (see your system manual for
information on the command LIMIT), you will have a core file with a name
matching the appropriate log file in your src directory.

--- Debugging

Get to know the debugger setup in your computer system.  The most well known
are dbx, adb, and gdb.  My favorite is dbx.  All can be accessed by
issuing this command at your computer prompt, 'dbx envy 1000.core'.  The
output is the line of code where the game has crashed.  To gain more
information, you must obtain a stack trace.  (See the system manual for
information on your resident debugger)

=== The executable file

The executable file is created after typing 'make'.  You must first choose
a Makefile best suited for your machine.  See 'port.txt' for more information.

After making, the executable file is named 'envy'.
You may then boot the game by executing the startup script
'startup' or 'startupSH' by typing 'startup &' or 'startupSH &'.

To enter the game, type 'telnet localhost 4000' from your mud account.

=== What is new, what is old

This source is based on the Envy 2.2 release source code.  Some internal
workings have changed and some player visible improvements have been made.
I've been developing the code for the past 3 months with extensive playing.
Unfortunately, there might be bugs i have missed.  If you encounter errors
or bugs, first check your code additions then forward questions to me at
vasc@camoes.rnl.ist.utl.pt, please check with your local Unix/GCC guru first,
since i'm only one person here!  And i'm no genius either ;-)

All in all, Envy will "feel" like any other MERC mud out there.  Envy is
designed to go beyond the MERC releases and make into reality many
improvements that went undone in the MERC releases.

Envy does not support traditional C compilers.

Envy no longer runs on MSDOS machines.  The alternative provided are
the Windows 95/NT ports.  Also, you may load Linux or BSD on your PC
to compile or modify Envy.

See 'new.txt' to see a more accurate list of changes done to the code.

See 'next.txt' to see what might be included in the next release.
