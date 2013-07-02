CC     = gxx
CPP    = gxx -E
LIBS   = -lm 
CFLAGS =  -O -I. -I c:/djgpp/lib

objdir = objfiles
VPATH  = command edesc inv misc mob obj quest room set shop zone

MA_SRC = de.cpp dispflag.cpp display.cpp \
	 dispmisc.cpp editflag.cpp flagdef.cpp \
	 init.cpp initscr.cpp interp.cpp \
         system.cpp template.cpp \
         vardef.cpp
MA_OBJ = $(objdir)/de.o $(objdir)/dispflag.o $(objdir)/display.o \
	 $(objdir)/dispmisc.o $(objdir)/editflag.o $(objdir)/flagdef.o \
	 $(objdir)/init.o $(objdir)/initscr.o $(objdir)/interp.o \
         $(objdir)/system.o $(objdir)/template.o \
         $(objdir)/vardef.o 

CO_SRC = command/alias.cpp command/check.cpp command/clone.cpp \
	 command/command.cpp command/copydef.cpp command/copydesc.cpp \
	 command/create.cpp command/crtedit.cpp command/delete.cpp \
	 command/editent.cpp command/initcomm.cpp command/list.cpp \
	 command/load.cpp command/look.cpp command/maincomm.cpp \
	 command/misccomm.cpp command/mountfol.cpp command/purge.cpp \
	 command/setcomm.cpp command/setrand.cpp command/stat.cpp \
	 command/var.cpp
CO_OBJ = $(objdir)/alias.o $(objdir)/check.o $(objdir)/clone.o \
	 $(objdir)/command.o $(objdir)/copydef.o $(objdir)/copydesc.o \
	 $(objdir)/create.o $(objdir)/crtedit.o $(objdir)/delete.o \
	 $(objdir)/editent.o $(objdir)/initcomm.o $(objdir)/list.o \
	 $(objdir)/load.o $(objdir)/look.o $(objdir)/maincomm.o \
	 $(objdir)/misccomm.o $(objdir)/mountfol.o $(objdir)/purge.o \
	 $(objdir)/setcomm.o $(objdir)/setrand.o $(objdir)/stat.o \
	 $(objdir)/var.o

ED_SRC = edesc/edesc.cpp
ED_OBJ = $(objdir)/edesc.o

IN_SRC = inv/equip.cpp inv/inv.cpp inv/put.cpp inv/unequip.cpp
IN_OBJ = $(objdir)/equip.o $(objdir)/inv.o $(objdir)/put.o $(objdir)/unequip.o

MI_SRC = misc/config.cpp misc/default.cpp misc/editable.cpp \
	 misc/editchck.cpp misc/editdisp.cpp misc/editichk.cpp \
	 misc/editmchk.cpp misc/editochk.cpp misc/editrchk.cpp \
	 misc/flags.cpp misc/getstrn.cpp misc/history.cpp \
	 misc/keywords.cpp misc/loaded.cpp misc/master.cpp \
	 misc/menu.cpp misc/misc.cpp misc/mudcomm.cpp \
	 misc/strings.cpp misc/strnnode.cpp
MI_OBJ = $(objdir)/config.o $(objdir)/default.o $(objdir)/editable.o \
	 $(objdir)/editchck.o $(objdir)/editdisp.o $(objdir)/editichk.o \
	 $(objdir)/editmchk.o $(objdir)/editochk.o $(objdir)/editrchk.o \
	 $(objdir)/flags.o $(objdir)/getstrn.o $(objdir)/history.o \
	 $(objdir)/keywords.o $(objdir)/loaded.o $(objdir)/master.o \
	 $(objdir)/menu.o $(objdir)/misc.o $(objdir)/mudcomm.o \
	 $(objdir)/strings.o $(objdir)/strnnode.o

MO_SRC = mob/crtmob.cpp mob/crtmobhu.cpp mob/crtmobtu.cpp \
	 mob/delmobhu.cpp mob/delmobt.cpp mob/delmobtu.cpp \
	 mob/dispmob.cpp mob/editmflg.cpp mob/editmms2.cpp \
	 mob/editmms3.cpp mob/editmmsc.cpp mob/editmob.cpp \
	 mob/editmobu.cpp mob/mob.cpp mob/mobhere.cpp \
	 mob/mobtypes.cpp mob/mobu.cpp mob/readmob.cpp \
	 mob/writemob.cpp
MO_OBJ = $(objdir)/crtmob.o $(objdir)/crtmobhu.o $(objdir)/crtmobtu.o \
	 $(objdir)/delmobhu.o $(objdir)/delmobt.o $(objdir)/delmobtu.o \
	 $(objdir)/dispmob.o $(objdir)/editmflg.o $(objdir)/editmms2.o \
	 $(objdir)/editmms3.o $(objdir)/editmmsc.o $(objdir)/editmob.o \
	 $(objdir)/editmobu.o $(objdir)/mob.o $(objdir)/mobhere.o \
	 $(objdir)/mobtypes.o $(objdir)/mobu.o $(objdir)/readmob.o \
	 $(objdir)/writemob.o

OB_SRC = obj/crtobj.cpp obj/crtobjhu.cpp obj/crtobjtu.cpp \
	 obj/delobjh.cpp obj/delobjhu.cpp obj/delobjt.cpp \
	 obj/delobjtu.cpp obj/dispobj.cpp obj/editobj.cpp \
	 obj/editobju.cpp obj/editoexd.cpp obj/editofl2.cpp \
	 obj/editoflg.cpp obj/editoms2.cpp obj/editomsc.cpp \
	 obj/editotrp.cpp obj/object.cpp obj/objectu.cpp \
	 obj/objhere.cpp obj/objtypes.cpp obj/readobj.cpp \
	 obj/writeobj.cpp
OB_OBJ = $(objdir)/crtobj.o $(objdir)/crtobjhu.o $(objdir)/crtobjtu.o \
	 $(objdir)/delobjh.o $(objdir)/delobjhu.o $(objdir)/delobjt.o \
	 $(objdir)/delobjtu.o $(objdir)/dispobj.o $(objdir)/editobj.o \
	 $(objdir)/editobju.o $(objdir)/editoexd.o $(objdir)/editofl2.o \
	 $(objdir)/editoflg.o $(objdir)/editoms2.o $(objdir)/editomsc.o \
	 $(objdir)/editotrp.o $(objdir)/object.o $(objdir)/objectu.o \
	 $(objdir)/objhere.o $(objdir)/objtypes.o $(objdir)/readobj.o \
	 $(objdir)/writeobj.o

QU_SRC = quest/delqstu.cpp quest/editqst.cpp quest/editqsti.cpp \
	 quest/editqstm.cpp quest/editqstq.cpp quest/editqstu.cpp \
	 quest/qsttypes.cpp quest/quest.cpp quest/readqst.cpp \
	 quest/writeqst.cpp
QU_OBJ = $(objdir)/delqstu.o $(objdir)/editqst.o $(objdir)/editqsti.o \
	 $(objdir)/editqstm.o $(objdir)/editqstq.o $(objdir)/editqstu.o \
	 $(objdir)/qsttypes.o $(objdir)/quest.o $(objdir)/readqst.o \
	 $(objdir)/writeqst.o

RO_SRC = room/crtexit.cpp room/crtrexit.cpp room/crtroom.cpp \
	 room/crtroomu.cpp room/delexitu.cpp room/delroomu.cpp \
	 room/disproom.cpp room/editexit.cpp room/editrexd.cpp \
	 room/editrext.cpp room/editrflg.cpp room/editrmsc.cpp \
	 room/editroom.cpp room/editrsec.cpp room/exit.cpp \
	 room/readwld.cpp room/room.cpp room/roomtype.cpp \
	 room/roomu.cpp room/writewld.cpp
RO_OBJ = $(objdir)/crtexit.o $(objdir)/crtrexit.o $(objdir)/crtroom.o \
	 $(objdir)/crtroomu.o $(objdir)/delexitu.o $(objdir)/delroomu.o \
	 $(objdir)/disproom.o $(objdir)/editexit.o $(objdir)/editrexd.o \
	 $(objdir)/editrext.o $(objdir)/editrflg.o $(objdir)/editrmsc.o \
	 $(objdir)/editroom.o $(objdir)/editrsec.o $(objdir)/exit.o \
	 $(objdir)/readwld.o $(objdir)/room.o $(objdir)/roomtype.o \
	 $(objdir)/roomu.o $(objdir)/writewld.o

SE_SRC = set/set.cpp
SE_OBJ = $(objdir)/set.o

SH_SRC = shop/delshpu.cpp shop/editshp.cpp shop/editshpb.cpp \
	 shop/editshpc.cpp shop/editshpm.cpp shop/editshpr.cpp \
	 shop/editshps.cpp shop/editshpt.cpp shop/editshpu.cpp \
	 shop/readshp.cpp shop/shop.cpp shop/writeshp.cpp
SH_OBJ = $(objdir)/delshpu.o $(objdir)/editshp.o $(objdir)/editshpb.o \
	 $(objdir)/editshpc.o $(objdir)/editshpm.o $(objdir)/editshpr.o \
	 $(objdir)/editshps.o $(objdir)/editshpt.o $(objdir)/editshpu.o \
	 $(objdir)/readshp.o $(objdir)/shop.o $(objdir)/writeshp.o

ZO_SRC = zone/dispzone.cpp zone/editzflg.cpp zone/editzmsc.cpp \
	 zone/editzone.cpp zone/readzon.cpp zone/writezon.cpp \
	 zone/zone.cpp zone/zonetype.cpp
ZO_OBJ = $(objdir)/dispzone.o $(objdir)/editzflg.o $(objdir)/editzmsc.o \
	 $(objdir)/editzone.o $(objdir)/readzon.o $(objdir)/writezon.o \
	 $(objdir)/zone.o $(objdir)/zonetype.o

SRC    = $(MA_SRC) $(CO_SRC) $(ED_SRC) $(IN_SRC) $(MI_SRC) $(MO_SRC) \
	 $(OB_SRC) $(QU_SRC) $(RO_SRC) $(SE_SRC) $(SH_SRC) $(ZO_SRC)
OBJ    = $(MA_OBJ) $(CO_OBJ) $(ED_OBJ) $(IN_OBJ) $(MI_OBJ) $(MO_OBJ) \
	 $(OB_OBJ) $(QU_OBJ) $(RO_OBJ) $(SE_OBJ) $(SH_OBJ) $(ZO_OBJ)

OUT    = de

###############################################################################

all: $(OUT)

de: $(OBJ)
	-rm -f de
	$(CC) $(CFLAGS) -o de $(OBJ) $(LIBS)

fmtdep: $(objdir)/fmtdep.o
	$(CC) $(CFLAGS) -o fmtdep $(objdir)/fmtdep.o

clean:
	-rm -f $(OUT) fmtdep $(OBJ) .depend
	-touch .depend

depend: $(SRC) fmtdep
	for i in $(SRC) ; do $(CPP) $(CFLAGS) -MM $$i | fmtdep ; done > .depend~
	mv .depend~ .depend

$(objdir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

include .depend
