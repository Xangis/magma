make:
	@echo Make what?  You must clarify which system to compile MAGMA.
	@echo
	@echo 'make standard- Default compile'
	@echo 'make std		- Default compile'
	@echo 'make linux	- Default compile'
	@echo 'make windows	- 32-bit Windows machines'
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

