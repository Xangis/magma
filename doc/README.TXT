Magma MUD version 3.04

https://github.com/Xangis/magma

See /doc/magma_changes.txt for more info on updates since the last version.

To run the MUD on windows, build the code (see VERSION.TXT in the directory above
this one) and run the executable.  For Linux, build the code and run the
"startup.sh" file in the /src directory.  You may need to chmod the startup file to
make it executable.  The MUD will load on port 4001 by default.

For the MUD administrator login, use the username "Superuser" with a password
of "password".

This codebase has been tested to compile with Microsoft Visual C++ 2005 Express
Edition.  Other versions of visual studio may require some changes to the
solution file.

The codebase has also been tested to compile using GCC under Ubuntu Linux 6.06.
Other versions of Linux may or may not compile without errors.

------------------------------------------------------------------
The following information is from the 2000 release of version 3.0:
------------------------------------------------------------------

The documentation for Magma is not incredibly up to date.  If you are unsure
how something works, look at the code and try to figure it out.  If you still
can't make sense of it, file an issue on Github.

One thing you will want to note first off - Though the original UltraEnvy
code supported multiple UNIX platforms, I don't personally support
anything other than RedHat Linux 5.2 or 6.1, or MS Visual C++ under windows 98.
It is up to you to figure out the makefile if you have an unsupported or
nonstandard system.  However, this should be pretty easy for anyone who knows
anything about their operating system.

Magma now loads zones created with Tavril's DikuEdit, which is available
at https://github.com/Xangis/magma. It does not necessarily have code to
handle all of the flags and values that can be set with the editor, but
progress is being made in that direction.

In this version, spells and skills have been split into separate tables.
One important thing to remember when coding:  SKILLS ARE NOT SPELLS.
SPELLS ARE NOT SKILLS.  It is very easy to use the wrong lookup and
reference functions (skill_lookup vs. spell_lookup, skill_affected vs.
spell_affected).  Don't do this, because it will cause you all sorts of
stupid problems.

Skills and spells will now autoadvance with use and training and
practicing have been removed.

The bitvector system has been completely rewritten to allow for easy
expansion by simply incrementing the NUM_AFFECT_BITS in merc.h and then
adding the definitions for the bits and supporting code.  There is nearly
unlimited expansion availiable without completely recoding the
bitvector/affect routines, since I already did.  AFF_WHATEVER bits are now
structs rather than integers and have their own special functions.

Corpses will now save with the game, and players stuff stays with their
corpse.  Corpses have longer decay times.  When you die, you will have to
go back to your corpse to get your belongings.  If the MUD were to crash,
the corpses would reload when the MUD comes back up.

There is now a rent command, and a menu which you will see when you log in
or when you die.  This currently only allows you to enter the game or to
quit, but will be expanded in future releases.  One benefit of this menu
is that newbies that are killed at their start point don't have to die
repeatedly - they can just log out.

The MUD will now load quest data created with Tavril's DikuEdit.  It does
not yet know how to handle this information, but at least it can now load
a complete DikuEdit zone.  It can handle the quest messages but does not
yet process entire quests.

Events are now implemented.  These will allow more accurate timing of
various tasks in the MUD such as autosaving characters.  They are still
young and need some more development.  Playing with events can cause all
sorts of chaos, so they are best left to people more familiar with the
Magma codebase, and MUD code in general.

The magic system has been changed.  Instead of mages using mana, they will
have to memorize spells in order to cast them and once cast they will have
to be rememorized.  This system is quite young and still has a lot of
things that have to be worked out.  Right now players have no method for
learning new spells as they progress in levels.  Also some things that
should disrupt memorization or meditation (like combat) do not yet.

The trophy system has been introduced, which causes players to get less
experience for killing the same mobs repeatedly by keeping track of the
kind and number of mobiles that they have killed and reducing the
experience a little for each time they have killed the mob before.  Mobs
can be 'rolled' off of trophy by filling up your trophy with different
mobs to the point that there is no room for a new kill.  At that point,
the oldest one will be discarded to add the new kill to the trophy and the
player will get full experience for that mob again.

Coins have been converted from the gold system to the copper, silver,
gold, and platinum system.

All areas other than Midgaard have been removed.  Now that the object,
mobile, and room formats and bits have changed completely, these areas
will no longer load properly and I feel that it's not worth supporting
areas that you can find in any other MUD and instead a MUD should come up
with all original areas if it is going to be worth putting up.  However,
in the future all of the old UltraEnvy areas may be converted.  If I get
the time and motivation.

Mobiles now have default special functions assigned to them based on their
class.  Any special functions assigned to them in the #SPECIALS section
will displace the class functions and become the primary function.  If you
want a special function to be secondary and the class function primary you
will have to set them class_none and then manually set the secondary
function first and then set the primary function on the next line in the
#SPECIALS section.
