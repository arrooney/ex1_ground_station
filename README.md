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
Got: k
Got: k
...
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