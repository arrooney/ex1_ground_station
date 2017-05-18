# Edit compile time settings here           #
#############################################

#############################################
# Stop editting  compile time settings here #


clean: gom_clean cslre cforth ccobj

all: cobj forth bslre gom_build

configure: cobj bslre
	./waf configure


cforth:
	make -C pforth/build/unix clean

forth:
	make -C pforth/build/unix all

ccobj:
	make -C CObject/main clean

cobj:
	make -C CObject/main all
cslre:
	make -C slre/ clean

bslre:
	make -C slre/ all


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
