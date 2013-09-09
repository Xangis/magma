#!/bin/bash
#
# Takes as an input the name of a Basternae 1 / Duris (DikuEdit) format zone file
# and outputs an .are file with the various zone components assembled.
#
# The files (.wld, .zon, etc.) must reside in the current directory.
cat zone.header room.header $1.wld room.footer mob.header $1.mob mob.footer obj.header $1.obj obj.footer shop.header $1.shp shop.footer reset.header $1.zon quest.header $1.qst quest.footer zone.footer > $1.are

