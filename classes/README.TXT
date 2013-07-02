Veygoth's Magma MUD Version 3.0
Monday, September 25, 2000


=== The classes directory ===

This file gies a brief description of the files in the classes directory.

CLASSES.LST	- Contains a list of all classes to be loaded into the server.
                The classes are loaded in top-down order.

*.class		- An individual class file.

unused          - Directory containing unused class template files.

=== Adding, removing, and changing classes ===

Adding, removing, and changing classes in Magma isn't quite just as simple as
changing the CLASSES.LST file to add more classes.  There are a few in-code
things that have to be done:

const.c		- Edit repop_point structure to set race/class repop points.
		- Edit class_avail structure to set race/class availability.
merc.h		- Edit numbers assigned to each class, i.e. CLASS_WARRIOR
		- Edit MAX_CLASS to reflect the number of classes available.
		  Numbering starts at 0.

Classes do not have to load in any particular order, but every class number
must be filled and there must be no duplicates.  This will cause all manner
of problems and crashes.

