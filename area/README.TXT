Magma Mud Release 3.04
Monday, March 24th, 2008

https://github.com/Xangis/magma

In version 3.03 of Magma we added support for most, but not all, Envy 2.2
areas.  We did convert Envy values/flags to Magma values, but areas have not
been tested thoroughly, so don't be surprised if you see strange values on 
mobs, objects, or rooms.  Any strangeness should be reported via an issue on
the project page on Github: https://github.com/Xangis/magma

The following areas were changed in order to work with Magma 3.03:

smurf.are:   Vnums were changed because they overlapped with the required limbo.are.
ocean.are:   Vnums were changed because they overlapped with the required limbo.are.
plains.are:  Vnums were changed because they overlapped with the required limbo.are.
ofcol.are:   Vnums pointing to changed areas were updated.
haon.are:    Vnums pointing to changed areas were updated.
shaolin.are: Vnums pointing to changed areas were updated.

In 2013, we fixed the problems that were causing descent.are and gods.are to load.
If you're reading this statement, you have a version new enough to load them.

All other Envy areas included in this distribution should be unchanged in content 
from Envy version 2.2 other than any data formatting changes that were required.

=== The area directory

AREA.LST	   - Contains a list of all areas to be loaded into the server.
		     The areas are loaded in top-down order.
README.TXT     - This file.
limbo.are      - Objects, mobs, and rooms required by the game for proper operation.
                 includes templates for spell-created items and creatures.
screens.are    - Required by the game for proper operation.  Contains welcome, news,
                 message of the day, and race selection screens.
help.are       - The main help file.  Contains help entries for the game.
VNUMS.TXT      - A list of the VNUMs used by each area.
<filename>.are - Contains all of the data for a single area.
/convzone      - This folder contains a script for compiling areas created
                 with DikuEdit into the Magma area format.
/maps          - This folder contains basic C code used for generating overland
                 maps.  This code should be customized for your own use if you
                 wish to generate overland maps.
