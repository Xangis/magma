Magma MUD version 3.03
Saturday, December 22, 2007

=== The sys directory

This file gives a brief description of the files in the sys directory.

BUGS.TXT	    - Contains a report of all bugs and player bug comments.  You
		    may wish to look at this file frequently.
CORPSES.DAT     - Contains all of the in-game player corpses that have not
                yet decayed.  This only loads at boot time.
CORPSES.BLANK   - Copy this to CORPSES.DAT if you want to reset (delete)
                all of the corpses in the game.  This is futile to attempt
                while the mud is running, since it will simply be saved
                over.
CRIMES.DAT      - Contains a list of the crimes that have been committed by
                players.
CRIMES.DAT.blank - Copy this to CRIMES.DAT if you want to reset (delete) all
                of the crimes that have been committed by players.
FRAGLIST.DAT    - Contains frag data from player-vs-player combat.
FRAGLIST.DAT.blank - Copy this to FRAGLIST.DAT if you want to reset (delete)
                   all of the frag data in the game.
GREETING.ASC    - Contains the ASCII mud title, like the one on Envy2.2
		    help.are.
IDEAS.TXT	    - Contains a list of all the ideas players contribute using
		    the online IDEA command.
MUDTITLE.ANS    - Contains the ANSI title of the mud.  It's here instead of in
		    help.are so players can choose which title to see.
MUDTITLE.ASC    - Contains the ASCII mud title, like the one on Envy2.2
		    help.are.
NAMES.LST       - Contains a list of inappropriate or illegal names.
NOTES.TXT	    - Contains all the player notes created by the online NOTE
		    command.  You may wish to clean this file periodically using
		    your favorite text editor.
README.TXT      - This file.
SHUTDOWN.TXT    - This file is created by the server when it exits normally
		    by shutdown command or autoshutdown.  The server run script
		    will do its own housekeeping and you will rarely see it.
SOCIALS.TXT	    - Contains all the socials of the server.  You may add more to
		    the file if you wish and then reboot the server.
SYSDATA.TXT     - Contains the system data settings, including MUD configuration
                settings and the in-game time/year.
SYSDATA.TXT.blank - Copy this to SYSDATA.TXT if you want to reset (delete)
                all of the system data and flags to the original state.
TIME.TXT	    - Contains information instructing the server when to
		    autoreboot or autoshutdown.
TYPOS.TXT	    - Contains a list of all the player typo contributions.  You
		    may wish to look at this file frequently to maintain integrity
		    of your area files.
