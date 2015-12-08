# AlbertaSat
---

Contains the gomespace source code for a ground station. The submodules within here include the nanomind repository and AlbertaSat's OCP repository.

## Setting up Your Computer

* sudo apt-get install libzmq3-dev
* sudo apt-get install libelf-dev

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