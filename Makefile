# Edit compile time settings here           #
#############################################

#############################################
# Stop editting  compile time settings here #


clean: gom_clean cforth ccobj

all: cobj forth gom_build
	(cd pforth/fth; ./../../.pforth_init)

configure: cobj 
	./waf configure


cforth:
	make -C pforth/build/unix clean

forth:
	make -C pforth/build/unix obj_only

ccobj:
	make -C CObject/main clean

cobj:
	make -C CObject/main all


gom_config:
	./waf configure

gom_clean:
	./waf clean

gom_build: shellexec
	./waf build

gom: gom_clean gom_config gom_build

shellexec:
	cp debugshell build

print-%:
	@echo $* = $($*)
