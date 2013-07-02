Magma MUD version 3.04
Saturday, February 23, 2007

Xangis	xangis@yahoo.com

Convzone contains a group of utilities used for assembling a zone built
with DikuEdit from the individual files produced by the editor.


--- Linux/UNIX ---

This directory contains the makezone UNIX/Linux command line utility
used for creating zones from DikuEdit zones.  Just dump the zone files
in this directory and run

./makezone <area name>

And the zone files will be gathered together into an .are file that can be
loaded into the MUD.

There are additional files that let you generate a loadable zone from a
single area file.  These are zonefrommob, zonefromobj, zonefromqst, and
zonefromwld.  DOS version are not provided, but it should be easy enough
to create them if you compare the differences between the DOS and Linux
versions of Makezone.


--- DOS/Windows ---

To build a zone, dump the zone files in this directory and run

makezone.bat <area name>

And the zone files will be gathered together into an .are file that can be
loaded into the MUD.
