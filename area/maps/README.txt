The file makemap.c is custom utility used to generate the ANSI worldmaps from a text file
looking somewhat like:

****----x
xxuuuuxxx
xxx---xxx
xxx-xx***
xx***nnnn
xx****nnn

Where each different character represents a different terrain type.  The utility is not
very versatile and was only designed to work with the specific maps that were used with
Basternae.  In the future this utility may be modified for use with a wider range of
maps and user-defined keys.  It is included just so that one may see how the ANSI
surface maps were generated.

It generates a magma-format area file that can be loaded into the MUD.
