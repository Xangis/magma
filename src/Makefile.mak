# Generic Makefile.  Read 'port.txt' in the documentation
# directory for the Makefile that is right for you  - Kahn

CC      = gcc
PROF    =
DEBUG   = -g -pg
NOCRYPT =
O_FLAGS =
C_FLAGS = $(O_FLAGS) -Wall $(DEBUG) $(PROF) $(NOCRYPT)
//L_FLAGS = $(O_FLAGS)       $(DEBUG) $(PROF)
L_FLAGS = $(O_FLAGS)       $(DEBUG) $(PROF) -lefence
#
# If want to use the Electric Fence malloc() debugger change L_FLAGS  - Zen

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o ssm.o update.o bit.o gamble.o \
	  mem.o olc.o olc_act.o olc_save.o string.o mob_prog.o mob_comm.o     \
	  tables.o track.o wiznet.o act_clan.o spec_mob.o spec_obj.o magic.o \
          save.o event.o spells.o fraglist.o vehicle.o traps.o map.o songs.o

S_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c comm.c const.c \
          db.c fight.c handler.c interp.c ssm.c update.c bit.c gamble.c \
          mem.c olc.c olc_act.c olc_save.c string.c mob_prog.c mob_comm.c \
          tables.c track.c wiznet.c act_clan.c spec_mob.c spec_obj.c magic.c \
          save.c event.c spells.c fraglist.c vehicle.c traps.c map.c songs.c

magma: $(O_FILES)
	rm -f magma.new
#	purify -cache_dir=/tmp $(CC) $(L_FLAGS) -o magma.new $(O_FILES)
	$(CC) $(L_FLAGS) -o magma.new $(O_FILES)
	mv magma magma.old
	cp magma.new magma

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o magma magma.new magma.old

undo:
	cp magma magma.new
	cp magma.old magma

depend:	makedepend ${S_FILES}

act_comm.o:	merc.h
act_info.o:	merc.h olc.h
act_move.o:	merc.h olc.h
act_obj.o:	merc.h
act_wiz.o:	merc.h olc.h
comm.o:		merc.h telnet.h
const.o:	merc.h
db.o:		merc.h
fight.o:	merc.h
handler.o:	merc.h
interpo:	merc.h
ssm.o:		merc.h
update.o:	merc.h
bit.o:		merc.h
gamble.o:	merc.h
mem.o:		merc.h
olc.o:		merc.h olc.h
olc_act.o:	merc.h olc.h
olc_save.o:	merc.h olc.h
string.o:	merc.h
mob_prog.o:	merc.h
mob_comm.o:	merc.h
tables.o:	merc.h
track.o:	merc.h
wiznet.o:	merc.h
act_clan.o:	merc.h olc.h
spec_mob.o:	merc.h
spec_obj.o:	merc.h
magic.o:	merc.h
save.o:		merc.h
event.o:	merc.h
spells.o:	merc.h
fraglist.o:	merc.h
traps.o:        merc.h
vehicle.o:	merc.h
map.c:          merc.h
songs.c:        merc.h
