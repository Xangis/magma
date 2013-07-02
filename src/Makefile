make:
	@echo Make what?  You must clarify which system to compile ENVY.
	@echo
	@echo 'make aix	- AIX'
	@echo 'make amiga	- Amiga with TCP/IP'
	@echo 'make hpux	- HPUX'
	@echo 'make irix	- Irix'
	@echo 'make mip	- MIP'
	@echo 'make next	- NeXT'
	@echo 'make dec-osf	- DEC OSF'
	@echo 'make solaris	- Anything running Solaris'
	@echo 'make standard	- Default compile'
	@echo 'make linux	- For linux 1.2.x'
	@echo 'make linux2	- For linux 2.+'
	@echo 'make sequent	- Sequent machines'
	@echo 'make tektronix	- Tektronix machines'
	@echo 'make windows	- 32-bit Windows machines'
	@echo
	@echo If your system does not appear in the above list, try 'make all'.
	@echo
	@echo For Windows 95/NT platforms please read README.WIN

all:	standard

clean:
	rm -f *.o magma

standard:
	make -f Makefile.std

std:
	make -f Makefile.std

linux:
	make -f Makefile.std

windows:
	make -f Makefile.w32

