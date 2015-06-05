.. libutil main documentation

.. include:: <isonum.txt>

.. |libutil| replace:: GomSpace Util Library

*********
|libutil|
*********

.. toctree::
   :maxdepth: 3
   

Introduction
============

The |libutil| contains a number of usefull utilities that can be used when developing A3200 applications.

In the following sections desctibes the |libutil| structure and API.

Structure
=========

The |libutil| is structured as follows:

+-----------------+-------------------------------------------------------------------+
| libutil/include | | The src folder contains the |libutil| API header files.         |
+-----------------+-------------------------------------------------------------------+
| libutil/src     | | The src folder contains the |libutil| API source code files.    |
|                 | | See :ref:`libutil_api` below for further details.               |
+-----------------+-------------------------------------------------------------------+
| libutil/doc     | | The doc folder contains the source code for this documentation. |
+-----------------+-------------------------------------------------------------------+

.. _libutil_api:

API
===

The |libutil| includes API functions for 

* Time and clock functions
* Byte order handling
* Print and print formatting functions
* CRC and checksum functions
* Hash, List, Linked List, and Array operation

Time and clock functions
------------------------

clock.h
^^^^^^^
.. doxygenfile:: clock.h


Byte order handling
-------------------

byteorder.h
^^^^^^^^^^^
.. doxygenfile:: byteorder.h

Print and print formatting functions
------------------------------------

color_printf.h
^^^^^^^^^^^^^^
.. doxygenfile:: color_printf.h

base16.h
^^^^^^^^
.. doxygenfile:: base16.h

hexdump.h
^^^^^^^^^
.. doxygenfile:: hexdump.h

error.h
^^^^^^^
.. doxygenfile:: error.h


CRC and checksum functions
--------------------------

crc32.h
^^^^^^^
.. doxygenfile:: crc32.h

.. _libutil_uthash:

Hash, List, Linked List, and Array operation
--------------------------------------------
The uthash module is a set of header files containing some rather clever macros. These
macros include **uthash.h** for hash tables, **utlist.h** for linked lists and **utarray.h** for arrays.

The list macros support the basic linked list operations: adding and deleting elements, sorting them and
iterating over them. It does so for both single linked list double linked lists and circular lists.

The dynamic array macros supports basic operations such as push, pop, and erase on the array elements.
These array elements can be any simple datatype or structure. The array operations are based loosely 
on the C++ STL vector methods. Internally the dynamic array contains a contiguous memory region into 
which the elements are copied. This buffer is grown as needed using realloc to accomodate all the data 
that is pushed into it.

The hash tables provides a good alternative to linked lists for larger tables where scanning through 
the entire list is going to be slow. The overhead added is larger memory usage and the additional 
hash processing time, so for short sets of data linked lists are preferred.

