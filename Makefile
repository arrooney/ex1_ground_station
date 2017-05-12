# Edit compile time settings here           #
#############################################

#############################################
# Stop editting  compile time settings here #

clean: gom_clean cforth ccobj

all: cobj forth gom_build


cforth:
	make -C pforth/build/unix clean

forth:
	make -C pforth/build/unix all

ccobj:
	make -C CObject/main clean

cobj:
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
