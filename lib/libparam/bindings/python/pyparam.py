'''Wrapper for param_types.h

Generated with:
/usr/local/bin/ctypesgen.py -I../../build/include -I../../include -I../../src/client -I../../src/store -I../../src/lock ../../include/param/param_types.h ../../include/param/param_string.h ../../include/param/param_serializer.h ../../src/client/rparam_client.h -lparam -o pyparam.py

Do not modify this file.
'''

__docformat__ =  'restructuredtext'

# Begin preamble

import ctypes, os, sys
from ctypes import *

_int_types = (c_int16, c_int32)
if hasattr(ctypes, 'c_int64'):
    # Some builds of ctypes apparently do not have c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (c_int64,)
for t in _int_types:
    if sizeof(t) == sizeof(c_size_t):
        c_ptrdiff_t = t
del t
del _int_types

class c_void(Structure):
    # c_void_p is a buggy return type, converting to int, so
    # POINTER(None) == c_void_p is actually written as
    # POINTER(c_void), so it can be treated as a real pointer.
    _fields_ = [('dummy', c_int)]

def POINTER(obj):
    p = ctypes.POINTER(obj)

    # Convert None to a real NULL pointer to work around bugs
    # in how ctypes handles None on 64-bit platforms
    if not isinstance(p.from_param, classmethod):
        def from_param(cls, x):
            if x is None:
                return cls()
            else:
                return x
        p.from_param = classmethod(from_param)

    return p

class UserString:
    def __init__(self, seq):
        if isinstance(seq, basestring):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq)
    def __str__(self): return str(self.data)
    def __repr__(self): return repr(self.data)
    def __int__(self): return int(self.data)
    def __long__(self): return long(self.data)
    def __float__(self): return float(self.data)
    def __complex__(self): return complex(self.data)
    def __hash__(self): return hash(self.data)

    def __cmp__(self, string):
        if isinstance(string, UserString):
            return cmp(self.data, string.data)
        else:
            return cmp(self.data, string)
    def __contains__(self, char):
        return char in self.data

    def __len__(self): return len(self.data)
    def __getitem__(self, index): return self.__class__(self.data[index])
    def __getslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, basestring):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other))
    def __radd__(self, other):
        if isinstance(other, basestring):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other) + self.data)
    def __mul__(self, n):
        return self.__class__(self.data*n)
    __rmul__ = __mul__
    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self): return self.__class__(self.data.capitalize())
    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))
    def count(self, sub, start=0, end=sys.maxint):
        return self.data.count(sub, start, end)
    def decode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())
    def encode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())
    def endswith(self, suffix, start=0, end=sys.maxint):
        return self.data.endswith(suffix, start, end)
    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))
    def find(self, sub, start=0, end=sys.maxint):
        return self.data.find(sub, start, end)
    def index(self, sub, start=0, end=sys.maxint):
        return self.data.index(sub, start, end)
    def isalpha(self): return self.data.isalpha()
    def isalnum(self): return self.data.isalnum()
    def isdecimal(self): return self.data.isdecimal()
    def isdigit(self): return self.data.isdigit()
    def islower(self): return self.data.islower()
    def isnumeric(self): return self.data.isnumeric()
    def isspace(self): return self.data.isspace()
    def istitle(self): return self.data.istitle()
    def isupper(self): return self.data.isupper()
    def join(self, seq): return self.data.join(seq)
    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))
    def lower(self): return self.__class__(self.data.lower())
    def lstrip(self, chars=None): return self.__class__(self.data.lstrip(chars))
    def partition(self, sep):
        return self.data.partition(sep)
    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))
    def rfind(self, sub, start=0, end=sys.maxint):
        return self.data.rfind(sub, start, end)
    def rindex(self, sub, start=0, end=sys.maxint):
        return self.data.rindex(sub, start, end)
    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))
    def rpartition(self, sep):
        return self.data.rpartition(sep)
    def rstrip(self, chars=None): return self.__class__(self.data.rstrip(chars))
    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)
    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)
    def splitlines(self, keepends=0): return self.data.splitlines(keepends)
    def startswith(self, prefix, start=0, end=sys.maxint):
        return self.data.startswith(prefix, start, end)
    def strip(self, chars=None): return self.__class__(self.data.strip(chars))
    def swapcase(self): return self.__class__(self.data.swapcase())
    def title(self): return self.__class__(self.data.title())
    def translate(self, *args):
        return self.__class__(self.data.translate(*args))
    def upper(self): return self.__class__(self.data.upper())
    def zfill(self, width): return self.__class__(self.data.zfill(width))

class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""
    def __init__(self, string=""):
        self.data = string
    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")
    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + sub + self.data[index+1:]
    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + self.data[index+1:]
    def __setslice__(self, start, end, sub):
        start = max(start, 0); end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start]+sub.data+self.data[end:]
        elif isinstance(sub, basestring):
            self.data = self.data[:start]+sub+self.data[end:]
        else:
            self.data =  self.data[:start]+str(sub)+self.data[end:]
    def __delslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]
    def immutable(self):
        return UserString(self.data)
    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, basestring):
            self.data += other
        else:
            self.data += str(other)
        return self
    def __imul__(self, n):
        self.data *= n
        return self

class String(MutableString, Union):

    _fields_ = [('raw', POINTER(c_char)),
                ('data', c_char_p)]

    def __init__(self, obj=""):
        if isinstance(obj, (str, unicode, UserString)):
            self.data = str(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(POINTER(c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj)

        # Convert from c_char_p
        elif isinstance(obj, c_char_p):
            return obj

        # Convert from POINTER(c_char)
        elif isinstance(obj, POINTER(c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(cast(obj, POINTER(c_char)))

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)
    from_param = classmethod(from_param)

def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)

# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to c_void_p.
def UNCHECKED(type):
    if (hasattr(type, "_type_") and isinstance(type._type_, str)
        and type._type_ != "P"):
        return type
    else:
        return c_void_p

# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self,func,restype,argtypes):
        self.func=func
        self.func.restype=restype
        self.argtypes=argtypes
    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func
    def __call__(self,*args):
        fixed_args=[]
        i=0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i+=1
        return self.func(*fixed_args+list(args[i:]))

# End preamble

_libs = {}
_libdirs = []

# Begin loader

# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

import os.path, re, sys, glob
import platform
import ctypes
import ctypes.util

def _environ_path(name):
    if name in os.environ:
        return os.environ[name].split(":")
    else:
        return []

class LibraryLoader(object):
    def __init__(self):
        self.other_dirs=[]

    def load_library(self,libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            if os.path.exists(path):
                return self.load(path)

        raise ImportError("%s not found." % libname)

    def load(self,path):
        """Given a path to a library, load it."""
        try:
            # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
            # of the default RTLD_LOCAL.  Without this, you end up with
            # libraries not being loadable, resulting in "Symbol not found"
            # errors
            if sys.platform == 'darwin':
                return ctypes.CDLL(path, ctypes.RTLD_GLOBAL)
            else:
                return ctypes.cdll.LoadLibrary(path)
        except OSError,e:
            raise ImportError(e)

    def getpaths(self,libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # FIXME / TODO return '.' and os.path.dirname(__file__)
            for path in self.getplatformpaths(libname):
                yield path

            path = ctypes.util.find_library(libname)
            if path: yield path

    def getplatformpaths(self, libname):
        return []

# Darwin (Mac OS X)

class DarwinLibraryLoader(LibraryLoader):
    name_formats = ["lib%s.dylib", "lib%s.so", "lib%s.bundle", "%s.dylib",
                "%s.so", "%s.bundle", "%s"]

    def getplatformpaths(self,libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [format % libname for format in self.name_formats]

        for dir in self.getdirs(libname):
            for name in names:
                yield os.path.join(dir,name)

    def getdirs(self,libname):
        '''Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        '''

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [os.path.expanduser('~/lib'),
                                          '/usr/local/lib', '/usr/lib']

        dirs = []

        if '/' in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))

        dirs.extend(self.other_dirs)
        dirs.append(".")
        dirs.append(os.path.dirname(__file__))

        if hasattr(sys, 'frozen') and sys.frozen == 'macosx_app':
            dirs.append(os.path.join(
                os.environ['RESOURCEPATH'],
                '..',
                'Frameworks'))

        dirs.extend(dyld_fallback_library_path)

        return dirs

# Posix

class PosixLibraryLoader(LibraryLoader):
    _ld_so_cache = None

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = []
        for name in ("LD_LIBRARY_PATH",
                     "SHLIB_PATH", # HPUX
                     "LIBPATH", # OS/2, AIX
                     "LIBRARY_PATH", # BE/OS
                    ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))
        directories.extend(self.other_dirs)
        directories.append(".")
        directories.append(os.path.dirname(__file__))

        try: directories.extend([dir.strip() for dir in open('/etc/ld.so.conf')])
        except IOError: pass

        unix_lib_dirs_list = ['/lib', '/usr/lib', '/lib64', '/usr/lib64']
        if sys.platform.startswith('linux'):
            # Try and support multiarch work in Ubuntu
            # https://wiki.ubuntu.com/MultiarchSpec
            bitage = platform.architecture()[0]
            if bitage.startswith('32'):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ['/lib/i386-linux-gnu', '/usr/lib/i386-linux-gnu']
            elif bitage.startswith('64'):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ['/lib/x86_64-linux-gnu', '/usr/lib/x86_64-linux-gnu']
            else:
                # guess...
                unix_lib_dirs_list += glob.glob('/lib/*linux-gnu')
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r'lib(.*)\.s[ol]')
        ext_re = re.compile(r'\.s[ol]$')
        for dir in directories:
            try:
                for path in glob.glob("%s/*.s[ol]*" % dir):
                    file = os.path.basename(path)

                    # Index by filename
                    if file not in cache:
                        cache[file] = path

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        if library not in cache:
                            cache[library] = path
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname)
        if result: yield result

        path = ctypes.util.find_library(libname)
        if path: yield os.path.join("/lib",path)

# Windows

class _WindowsLibrary(object):
    def __init__(self, path):
        self.cdll = ctypes.cdll.LoadLibrary(path)
        self.windll = ctypes.windll.LoadLibrary(path)

    def __getattr__(self, name):
        try: return getattr(self.cdll,name)
        except AttributeError:
            try: return getattr(self.windll,name)
            except AttributeError:
                raise

class WindowsLibraryLoader(LibraryLoader):
    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll"]

    def load_library(self, libname):
        try:
            result = LibraryLoader.load_library(self, libname)
        except ImportError:
            result = None
            if os.path.sep not in libname:
                for name in self.name_formats:
                    try:
                        result = getattr(ctypes.cdll, name % libname)
                        if result:
                            break
                    except WindowsError:
                        result = None
            if result is None:
                try:
                    result = getattr(ctypes.cdll, libname)
                except WindowsError:
                    result = None
            if result is None:
                raise ImportError("%s not found." % libname)
        return result

    def load(self, path):
        return _WindowsLibrary(path)

    def getplatformpaths(self, libname):
        if os.path.sep not in libname:
            for name in self.name_formats:
                dll_in_current_dir = os.path.abspath(name % libname)
                if os.path.exists(dll_in_current_dir):
                    yield dll_in_current_dir
                path = ctypes.util.find_library(name % libname)
                if path:
                    yield path

# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin":   DarwinLibraryLoader,
    "cygwin":   WindowsLibraryLoader,
    "win32":    WindowsLibraryLoader
}

loader = loaderclass.get(sys.platform, PosixLibraryLoader)()

def add_library_search_dirs(other_dirs):
    loader.other_dirs = other_dirs

load_library = loader.load_library

del loaderclass

# End loader

add_library_search_dirs([])

# Begin libraries

_libs["param"] = load_library("param")

# 1 libraries
# End libraries

# No modules

enum_param_flags = c_int # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 18

PARAM_F_PERSIST = (1 << 0) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 18

enum_anon_1 = c_int # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_UINT8 = 0 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_UINT16 = (PARAM_UINT8 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_UINT32 = (PARAM_UINT16 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_UINT64 = (PARAM_UINT32 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_INT8 = (PARAM_UINT64 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_INT16 = (PARAM_INT8 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_INT32 = (PARAM_INT16 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_INT64 = (PARAM_INT32 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_X8 = (PARAM_INT64 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_X16 = (PARAM_X8 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_X32 = (PARAM_X16 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_X64 = (PARAM_X32 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_DOUBLE = (PARAM_X64 + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_FLOAT = (PARAM_DOUBLE + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_STRING = (PARAM_FLOAT + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

PARAM_DATA = (PARAM_STRING + 1) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

param_type_t = enum_anon_1 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 42

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 45
for _lib in _libs.values():
    try:
        sum1 = (c_uint16).in_dll(_lib, 'sum1')
        break
    except:
        pass

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 46
for _lib in _libs.values():
    try:
        sum2 = (c_uint16).in_dll(_lib, 'sum2')
        break
    except:
        pass

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 47
try:
    index = (c_int).in_dll(_libs['param'], 'index')
except:
    pass

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 65
class struct_param_table_s(Structure):
    pass

struct_param_table_s.__slots__ = [
    'addr',
    'type',
    'size',
    'count',
    'flags',
    'name',
]
struct_param_table_s._fields_ = [
    ('addr', c_uint16),
    ('type', param_type_t),
    ('size', c_uint8),
    ('count', c_uint8),
    ('flags', c_uint8),
    ('name', c_char * 14),
]

param_table_t = struct_param_table_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 65

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 73
class struct_param_index_s(Structure):
    pass

param_callback_func = CFUNCTYPE(UNCHECKED(None), c_uint16, POINTER(struct_param_index_s)) # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 68

struct_param_index_s.__slots__ = [
    'mem_id',
    'table',
    'count',
    'read',
    'write',
    'physaddr',
    'framaddr',
    'lock',
    'lock_inited',
    'callback',
]
struct_param_index_s._fields_ = [
    ('mem_id', c_uint8),
    ('table', POINTER(param_table_t)),
    ('count', c_uint),
    ('read', CFUNCTYPE(UNCHECKED(POINTER(None)), c_uint16, c_size_t)),
    ('write', CFUNCTYPE(UNCHECKED(None), c_uint16, c_size_t, POINTER(None), c_uint8)),
    ('physaddr', POINTER(None)),
    ('framaddr', c_uint16),
    ('lock', POINTER(None)),
    ('lock_inited', c_int),
    ('callback', param_callback_func),
]

param_index_t = struct_param_index_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 84

param_mem = c_uint8 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 86

enum_rparam_action_e = c_int # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_GET = 0 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_REPLY = 85 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_SET = 255 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_TABLE = 68 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_COPY = 119 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_LOAD = 136 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_SAVE = 153 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

RPARAM_CLEAR = 170 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

rparam_action = enum_rparam_action_e # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 97

enum_rparam_reply_e = c_int # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_SET_OK = 1 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_LOAD_OK = 2 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_SAVE_OK = 3 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_COPY_OK = 4 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_CLEAR_OK = 5 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

RPARAM_ERROR = 255 # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

rparam_reply = enum_rparam_reply_e # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 106

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 117
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    '_from',
    'to',
]
struct_anon_2._fields_ = [
    ('_from', c_uint8),
    ('to', c_uint8),
]

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 121
class struct_anon_3(Structure):
    pass

struct_anon_3.__slots__ = [
    'id',
]
struct_anon_3._fields_ = [
    ('id', c_uint8),
]

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 113
class union_anon_4(Union):
    pass

union_anon_4.__slots__ = [
    'addr',
    'packed',
    'filename',
    'copy',
    'clear',
]
union_anon_4._fields_ = [
    ('addr', c_uint16 * 0),
    ('packed', c_uint8 * 0),
    ('filename', c_char * 0),
    ('copy', struct_anon_2),
    ('clear', struct_anon_3),
]

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 125
class struct_rparam_frame_s(Structure):
    pass

struct_rparam_frame_s.__slots__ = [
    'action',
    'mem',
    'length',
    'checksum',
    'unnamed_1',
]
struct_rparam_frame_s._anonymous_ = [
    'unnamed_1',
]
struct_rparam_frame_s._fields_ = [
    ('action', rparam_action),
    ('mem', c_uint8),
    ('length', c_uint16),
    ('checksum', c_uint16),
    ('unnamed_1', union_anon_4),
]

rparam_query = struct_rparam_frame_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 125

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 20
if hasattr(_libs['param'], 'param_find_name'):
    param_find_name = _libs['param'].param_find_name
    param_find_name.argtypes = [POINTER(param_table_t), c_size_t, String]
    param_find_name.restype = POINTER(param_table_t)

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 29
if hasattr(_libs['param'], 'param_find_addr'):
    param_find_addr = _libs['param'].param_find_addr
    param_find_addr.argtypes = [POINTER(param_table_t), c_size_t, c_uint16]
    param_find_addr.restype = POINTER(param_table_t)

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 36
if hasattr(_libs['param'], 'param_list'):
    param_list = _libs['param'].param_list
    param_list.argtypes = [POINTER(param_index_t), c_int]
    param_list.restype = None

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 43
if hasattr(_libs['param'], 'param_list_simple'):
    param_list_simple = _libs['param'].param_list_simple
    param_list_simple.argtypes = [POINTER(param_index_t), c_int]
    param_list_simple.restype = None

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 52
if hasattr(_libs['param'], 'param_from_string'):
    param_from_string = _libs['param'].param_from_string
    param_from_string.argtypes = [POINTER(param_table_t), String, POINTER(None)]
    param_from_string.restype = c_int

# /home/johan/git/pygnd/lib/libparam/include/param/param_string.h: 64
if hasattr(_libs['param'], 'param_to_string'):
    param_to_string = _libs['param'].param_to_string
    param_to_string.argtypes = [POINTER(param_table_t), POINTER(c_uint8), c_int, POINTER(None), c_int, c_int]
    param_to_string.restype = c_int

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 19
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'param_betoh'):
        continue
    param_betoh = _lib.param_betoh
    param_betoh.argtypes = [param_type_t, POINTER(None)]
    param_betoh.restype = c_int
    break

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 27
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'param_htobe'):
        continue
    param_htobe = _lib.param_htobe
    param_htobe.argtypes = [param_type_t, POINTER(None)]
    param_htobe.restype = c_int
    break

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 41
if hasattr(_libs['param'], 'param_serialize'):
    param_serialize = _libs['param'].param_serialize
    param_serialize.argtypes = [POINTER(param_index_t), POINTER(c_uint16), c_uint, POINTER(c_uint8), c_int, c_int, c_int, c_int]
    param_serialize.restype = c_int

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 55
if hasattr(_libs['param'], 'param_serialize_item'):
    param_serialize_item = _libs['param'].param_serialize_item
    param_serialize_item.argtypes = [POINTER(param_table_t), c_uint16, POINTER(c_uint8), POINTER(c_uint16), c_uint, POINTER(None), c_int, c_int]
    param_serialize_item.restype = c_int

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 67
if hasattr(_libs['param'], 'param_deserialize'):
    param_deserialize = _libs['param'].param_deserialize
    param_deserialize.argtypes = [POINTER(param_index_t), POINTER(c_uint8), c_int, c_int, c_int, c_int]
    param_deserialize.restype = c_int

# /home/johan/git/pygnd/lib/libparam/include/param/param_serializer.h: 80
if hasattr(_libs['param'], 'param_deserialize_item'):
    param_deserialize_item = _libs['param'].param_deserialize_item
    param_deserialize_item.argtypes = [POINTER(param_table_t), c_uint16, POINTER(param_index_t), POINTER(None), c_int, c_int, c_int]
    param_deserialize_item.restype = c_int

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 14
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'cmd_rparam_setup'):
        continue
    cmd_rparam_setup = _lib.cmd_rparam_setup
    cmd_rparam_setup.argtypes = []
    cmd_rparam_setup.restype = None
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 15
if hasattr(_libs['param'], 'rparam_get_full_table'):
    rparam_get_full_table = _libs['param'].rparam_get_full_table
    rparam_get_full_table.argtypes = [POINTER(param_index_t), c_int, c_int, c_int, c_int]
    rparam_get_full_table.restype = c_int

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 16
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_single'):
        continue
    rparam_get_single = _lib.rparam_get_single
    rparam_get_single.argtypes = [POINTER(None), c_uint16, param_type_t, c_int, c_int, c_int, c_int, c_int]
    rparam_get_single.restype = c_int
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 35
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_uint8'):
        continue
    rparam_get_uint8 = _lib.rparam_get_uint8
    rparam_get_uint8.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_uint8.restype = c_uint8
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 36
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_uint16'):
        continue
    rparam_get_uint16 = _lib.rparam_get_uint16
    rparam_get_uint16.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_uint16.restype = c_uint16
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 37
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_uint32'):
        continue
    rparam_get_uint32 = _lib.rparam_get_uint32
    rparam_get_uint32.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_uint32.restype = c_uint32
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 38
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_uint64'):
        continue
    rparam_get_uint64 = _lib.rparam_get_uint64
    rparam_get_uint64.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_uint64.restype = c_uint64
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 39
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_int8'):
        continue
    rparam_get_int8 = _lib.rparam_get_int8
    rparam_get_int8.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_int8.restype = c_int8
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 40
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_int16'):
        continue
    rparam_get_int16 = _lib.rparam_get_int16
    rparam_get_int16.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_int16.restype = c_int16
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 41
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_int32'):
        continue
    rparam_get_int32 = _lib.rparam_get_int32
    rparam_get_int32.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_int32.restype = c_int32
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 42
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_int64'):
        continue
    rparam_get_int64 = _lib.rparam_get_int64
    rparam_get_int64.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_int64.restype = c_int64
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 43
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_float'):
        continue
    rparam_get_float = _lib.rparam_get_float
    rparam_get_float.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_float.restype = c_float
    break

# /home/johan/git/pygnd/lib/libparam/src/client/rparam_client.h: 44
for _lib in _libs.itervalues():
    if not hasattr(_lib, 'rparam_get_double'):
        continue
    rparam_get_double = _lib.rparam_get_double
    rparam_get_double.argtypes = [POINTER(None), c_uint16, c_int, c_int, c_int, c_int]
    rparam_get_double.restype = c_double
    break

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 14
try:
    MAX_PARAM_NAME_LEN = 14
except:
    pass

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 15
try:
    MAX_PARAM_MEMS = 10
except:
    pass

# /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 16
try:
    RPARAM_QUERY_MAX_LEN = 180
except:
    pass

param_table_s = struct_param_table_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 65

param_index_s = struct_param_index_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 73

rparam_frame_s = struct_rparam_frame_s # /home/johan/git/pygnd/lib/libparam/include/param/param_types.h: 125

# No inserted files

