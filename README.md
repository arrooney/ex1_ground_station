# AlbertaSat
---

Contains the gomespace source code for a ground station. The submodules within here include the nanomind repository and AlbertaSat's OCP repository.

## Setting up Your Computer

* sudo apt-get install libzmq3-dev libelf-dev libncurses-dev

## Setting up a Newly Cloned Repository

* git submodule init
* git submodule update
* cd albertasat-gomspace
* git submodule init
* git submodule update
* cd albertasat-on-board-computer
* git fetch
* git checkout origin/nanomind-master-2
* cd ../..

## Building an Running

* ./waf configure && ./waf build
    * Note, the configure command is only need once after you clone or make wscript changes
* ./waf clean
* sudo ./gomshell
    * Needs super user access to do IO over a serial port

## Editing Source

The entire repo is an eclipse project. The source can be edited from eclipse. Note, due to the submodules used, the eclipse indexer finds three CSP libraries. Only refer to the CSP library in this repo. Anything within the albertasat-gomspace directory and beyond is CSP code for a different project.

#Safemode and RAM Image Script
---

In the directory ```./scripts``` is a script for putting the satellite into safemode and uploading a new RAM image. There is a makefile in the ```/scripts``` directory that can be used to configure this script. Open the makefile with a text editor. There are two symbols which can be used for configuration.

```bash
# Enables debug printing while running. 
RAM_DEBUG := true
# The script only puts the nanomind into safemode. It does not upload a new image.
RAM_SAFEMODE_ONLY := true
```

By uncommenting the symbols, the functionality is removed. **However**, the code must be compiled a second time for the changes to take effect.

```bash
cd scripts
make
```

The source file ```./scripts/ram.c``` contains instructions on how to run and use the file. Open it with a text editor to read the usage documentation. To run this script, the data must be piped to the gomshell. This is done like so:

```bash
./scripts/ram.o ../new_image.bin | ./gomshell
```

On some computers, the gomshell cannot be stopped when started like this. To stop it, open a second terminal and run ```ps aux```

```bash
ps aux | grep "gomshell"
username  155003  0.0  0.0  924329  2049 pts/18   S+   09:43   0:00 /bin/bash ./gomshell 
username  195043  0.0  0.0  343229  2049 pts/19   S+   10:43   0:00 grep --color=auto gomshell
```

This is an example of output from ```ps aux```. Yours will contain different numbers, in the same format. The first line of the output is gomshell process that needs to be killed. The second line is the command that ran to produce the output. The second number in the first output line is the process ID.

```bash
kill 155003
```

The gomshell will be killed and your terminal be returned to you.
 

#Piping data to gomshell

For future automation, we need to pipe data to the gomshell. The easiest way is with unix pipes. Take the following example program:

```C
#include <stdio.h>
#include <unistd.h>

int main( int argc, char** argv )
{
	for( ;; ) {
		fputs("k", stdout);
		fflush(stdout);
		usleep(1000*1000);
	}
	
	return 0;
}
```

Running this will print the letter "k" to the console every one second, for example:

```bash
gcc -Wall main.c -o main
./main
kkkkkkkkkk...
```

Using unix pipes, we can send this output to the gomshell instead:

```bash
./main | ./gomshell
```

Now, everyone one second, the gomeshell gets the letter "k". By significantly improving the functionality of this program, we can automate the process of sending data to the satellite. Note, this is a unidirectional shell with data flow like this:

* console -> main stdin
* main stdout -> gomshell
* gomshell stdout -> console 

When the gomshell prints to stdout, the data is sent to the terminal, not to the main program's stdin.