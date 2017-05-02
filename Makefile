# Edit compile time settings here           #
#############################################

#############################################
# Stop editting  compile time settings here #

clean: gom_clean forth_clean

all: forth_build gom_build



forth_clean:
	make -C pforth/build/unix clean

forth_build:
	make -C pforth/build/unix all



gom_config:
	./waf configure

gom_clean:
	./waf clean

gom_build: gdb shellexec
	./waf build

gom: gom_clean gom_config gom_build



shellexec:
	cp debugshell build

gdb:
	cp IOController/main/.gdbinit build


print-%:
	@echo $* = $($*)
