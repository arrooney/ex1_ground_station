#CObject
---

CObject is a small set of macros and functions for using fundamental object oriented concepts (OO) in the C language. 

* Classes
* Single inheritance
* Interfaces
* Virtual methods

It's implementation is designed to be as simple as possible, making application code intuitive to develop and easier to debug. 

CObject is c99 compliant.

#Compiling
---

In the folder liba/Class is the code. It can be compiled into a static library by running ```make all``` in a command line. You need to have gcc and make installed.

#Util
---

Several data structures have been written using CObject in /liba/util. Their documentation is available with github pages at [bandren.github.io/CObject](http://bandren.github.io/CObject). This library can be compiled by running ```make all``` in a command line. To use this in your project, you have link the library in /liba/Class (see above) first.

#Tests
---

In /liba/tests there is a unit test suite for all data structures in liba/util. Compile this into a static library by running ```make all``` in a command line. To run the test suite, you need to compile the code in /main by running ```make all``` then ```make run``` to execute it. All static libaries in /liba/Class, /liba/util, and /liba/tests must be compiled beforehand. 
