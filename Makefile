# Edit compile time settings here           #
#############################################

#############################################
# Stop editting  compile time settings here #

clean: gom_clean forth_clean cobject_clean

all: cobject_build forth_build gom_build


forth_clean:
	make -C pforth/build/unix clean

forth_build:
	make -C pforth/build/unix all

cobject_clean:
	make -C CObject/main clean

cobject_build:
	make -C CObject/main all


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
