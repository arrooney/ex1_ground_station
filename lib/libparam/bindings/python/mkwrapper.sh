#!/bin/sh
ctypesgen.py \
-I../../build/include \
-I../../include \
-I../../src/client \
-I../../src/store \
-I../../src/lock \
../../include/param/param_types.h \
../../include/param/param_string.h \
../../include/param/param_serializer.h \
../../src/client/*.h \
-lparam \
-o pyparam.py
