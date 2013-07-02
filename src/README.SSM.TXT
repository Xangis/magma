Envy 2.2 Running Shared String Memory Manager 2.2
Patch by Fusion

SSM 2.2 (UltraEnvy 0.87)

o	Included a more accurate hash function function made by Erwin S.A.
	<erwin@pip.dknet.dk>.  This decreased CPU time usage on server bootup
	quite a lot, and also seems to reduce memory usage a bit.

Zen ( October 16th, 1997 )

--

SSM 2.2 (EnvyMud 2.2)

o	Rewrote the defrag and memory allocation scheme in SSM.

Fusion ( June 27th, 1996 )

SSM 2.0.1 (EnvyMud 2.2)

o	Removed #ifdef ROM for Envy release.  If you wish the ROM
	part, you may contact Fusion or go to phoenix.mercer.peachnet.edu.
o	Removed some unnecessary code with consultation with Fusion.
o	Ported source code into Envy 2.1.25

Kahn ( June 25th, 1996 )

--

SSM 2.0  (EnvyMUD 2.2)

o	Now portable to all OS regardless of 32 or 64 bit addressing.
	Tested personally on Linux and DEC ALPHA (64-bit).
o	Code cleanups. free_string optimized for default path braching.
o	4-8k memory leak fixed.
o	Memory usage smaller now on all platforms due to the structure
	alignment changes.
o	NOTE: Adds code to db.c and also moves some vars that were no longer
	specific to db.c to ssm.c
o	memory wiz command now displays shared string info including
	overflow (mallocs) and tells you when you need increase.

Fusion

--

SSM 1.4 

A while back I posted a shared string manager to replace Merc's
and fix the 'read-only/never freed' problem. Due to the complexity
of the code (maybe?) there was only one person interested, Jason Dinkel
since obviously there was not much need until OLC came along.

With my str_dup, free_string the shared heap can be used as the STANDARD
C str_dup while retaining the ability to share strings. Usage counts allow
this. The strings still hashed at bootup but the hash table is destroyed
so during runtime there is no more hashing. Boot time should be similar
to the stock code. 

The new code module is ssm.c
Three replacement functions are in there:
str_dup(), free_string(), fread_string()
[and fread_string_eol() for ROM] Note: for ROM you need to #define ROM

-- 

I modified the SSM to work with a ROM. Not a lot of changes were to be made:
ROM has a fread_string_to_eol (for socials); i created a new one, modeled
after the new fread_string, having the same functionality as the old
fread_string_eol.
Some code additions were not to be made: the prompt stuff is different in
ROM, so these were not to be added.

Wreck (Jan 31, 1996)
