===============
Getting Started
===============

How to compile csp-term
=======================

Prerequisites: Recent linux with python (example build with ubuntu 14.10)

Install requirements::

    $ sudo apt get install build-essential libzmq3-dev

Configure source::

    $ waf configure

Build source::

    $ waf build

Command line arguments
======================

::

    -a Address
    -c CAN device
    -d USART device
    -b USART buad
    -z ZMQHUB server

Example 1: Starting csp-term with address 10 and connecting to a ZMQ proxy on localhost::

    $ ./build/csp-term -a 10 -z localhost
    
Example 2: Starting csp-term with address 10 using usart to /dev/ttyUSB0 at baudrate 500000::

    $ ./build/csp-term -a 10 -d /dev/ttyUSB0 -b 500000
    
