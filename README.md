# ExAlta-1 Gomshell
---

Contains the source code to interface with the ExAlta-1 satellite. The submodules within here include the nanomind repository and AlbertaSat's OCP 

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

## Folder Structure

.-|
  |--> build (compiled binary is put here)
  |--> src (source code for gomshell)
  |--> lib (gomspace libraries the gomshell requires)
  |--> IOController -|
  |                  |--> IOHook (shared library which redfines standard IO functions)
  |--> pforth (Forth kernal used to read scripts. Gomshell front end)
  |--> CObject (Libraries required for data structures like queues)

Everything else may be garbage we haven't deleted yet.

## Building an Running

The gomshell requires the following libraries are installed on your computer:

```bash
sudo apt-get install libzmq3-dev libelf-dev libncurses-dev
```

Several third party libraries need to be compiled from source too. The entire build process is automated
with two bash lines:

```bash
make clean
./waf configure
make all
```

The configure option will prime waf for building the gomshell. It needs to be done one time. Extra configurations will have no effect. However, if the waf script is changed, the configuration will need to be run again.

Additionally, I haven't done a very good job with the .gitignore, so several object files and binaries are in the repo. Clean it just to be safe ```make clean``` before buliding.

Afterwords, if you're only interested in building the gomshell source code, run:

```bash
./waf build
```

To clean the code, run:

```bash
./waf clean
```

To run the code, run:

```bash
sudo ./gomshell
```

This runs a shell script that invokes the gomshell. super user access is required to do IO over the serial port to the antenna or umbillical cord of the satellite.

## Editing Source

The gomshell source code is an eclipse project. All other libraries do not have an a project within an IDE. Set one up, or use a standard text editor like emacs.

## Scripting

The gomshell is a backend of the forth interpreter. To learn how to write forth code, see (this)[http://www.softsynth.com/pforth/pf_tut.php] link. To issue gomshell commands, use the ```GOM``` word:

```forth
GOM ( string location, string length -- , Text within the string is interpreted by the gomshell )
```

For example, the following will invoke the ```eps hk get``` command:

```forth
S" eps hk get" GOM
```

The ```S" ``` begins a string, the closing ```"``` ends the string. This puts the strings memory location and length onto the stack. The command is executed with the ```GOM``` word.

The following words will list all files in the satellite's SD card file system:

```forth
S" ftp ls /sd/" GOM
```

A script is run using the INCLUDE word:

```forth
INCLUDE my_script.fth
```
