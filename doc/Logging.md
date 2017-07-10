# Logging Information

The Magma MUD codebase has extensive logging, and it may be overwhelming at 
times, and may use far too much disk space.

However, the logs do contain some very detailed information that can help 
in troubleshooting bugs and problems.

## Location

Log files are in the /log directory and are numbered sequentially, starting
with 1000.log. Each log is a full run of the program from start to finish.

If run with the startup.sh/startup.bat, the game will automatically restart 
after a crash, so be sure to check the date and time of any log files to be
sure you're looking at the right one. Log lines will include a timestamp, so
if you know the time of an incident, you should be able to find out more about
what happened.

## Other Log Files

While not specifically log files, these other files will contain game 
information, typically submitted by players and should be reviewed from 
time to time.

- sys/TYPOS.TXT - Contains player-submitted typos using the "typo" command.
- sys/BUGS.TXT - Contains player-submitted bugs using the "bug" command in
  addition to code-generated bug notifications.
- sys/IDEAS.TXT - Contains player-submitted ideas using the "idea" command.
- sys/CRASHBUGS.TXT - Contains player-submitted crashbugs using the "crashbug"
  command.

Each of these files will prefix each log line with the room number where the
command was issued, which can be particularly helpful in the case of the 
typo file.

## Log Levels

Unlabeled log messages should be considered informational.

Messages labeled WARNING should be considered non-fatal, but things that
could cause annoyances. Common warnings include settings specified in area 
files that are abnormal, but won't break the game.

Messages labeled ERROR may or may not be fatal, but will cause problems.
This includes anything from a teleporter trying to send a player to a room
that does not exist (and failing), to severe problems that will crash the 
game.

## Startup Log Sequence

When the MUD boots up it does things in this order:

- Initializes variables
- Loads socials
- Loads classes
- Loads area files
- Links room exits together across zones
- Resets areas (loads monsters and equipment into rooms)
- Loads notes
- Loads bans
- Loads the illegal name list
- Loads clans/guilds
- Loads "down time" settings
- Loads corpses
- Loads the fraglist
- Creates recurring event timers (update handlers, etc.)
- Announces that the MUD is running.

At any point during this sequence something could go wrong due to a missing or 
corrupted file.

It is typical to see some warnings and errors during this sequence, especially 
during zone loading. Not all zones are perfect (including the ones included in 
the download). 

Here's what some of the more common messages mean:

#### WARNING: Object vnum 12345: object type weapon with no damage type specified

This means that an object that is tagged as a weapon doesn't have a damage type.
It's a pretty minor problem, but could cause odd combat messages, such as "an ogre
slashes you with its warhammer".

#### ERROR: Get_room_index: bad vnum 123

This means that an exit points to a room that does not exist. This can either be 
because a zone has a connection to another zone that has not been loaded, or 
because exits between rooms within the zone are broken. In any case, the 
connection will be ignored, and it's up to you to decide whether that's OK or a
problem.

#### WARNING: Equip_char: dead reaper already equipped at 16

This means that a zone has tried to place more than one item in the same 
equipment slot on a creature. The second item will not be equipped. This 
typically happens when a mob is supposed to be wielding two weapons, but they 
are told to equip both weapons in the same hand slot.

This is something that could probably be mostly fixed by having "fallback"
equipment slots -- as in, if the first hand slot is full, try to fall back to 
equipping the item in the second hand slot. That should be considered a 
"nice to have" item on the todo list.

## Terminations

If the MUD was shut down either on-purpose or due to a crash, there should 
be some information at the end of the log file showing what the last commands 
and functions executed were.

The "Last user command" will be the last command entered by a player.

The "Last commands, oldest to newest" will include both user commands and 
commands generated due to the event system.

The "Last functions, oldest to newest" will list the last 25 functions called.
It can be considered a sort of primitive stack trace, since C code doesn't
do that natively.

The last line of the log file will typically show what caused the MUD to shut
down, whether it be a SIGSEGV (segmentation fault), or a SIGINT (interrupt, 
typically Ctrl+Break). This will make it easy to spot abnormal terminations.
