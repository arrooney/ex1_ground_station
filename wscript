#!/usr/bin/env python
# encoding: utf-8

import os
import time

APPNAME = 'csp-term'
VERSION = '2.0'

top	= '.'
out	= 'build'

# Scan modules in local lib dir
modules = ['lib/libcsp', 'lib/libparam', 'lib/libutil', 'lib/libgosh', 'lib/libftp', 'lib/liblog']




def options(ctx):
	ctx.load('gcc')
	if os.path.exists('../tools/eclipse.py'):
		ctx.load('eclipse', tooldir='../tools/')
	ctx.recurse(modules)
	ctx.load('eclipse')
	gr = ctx.add_option_group("CSP-Term options")
	gr.add_option('--with-adcs', action='store_true',help='Enable ADCS client module')
	
def configure(ctx):
	ctx.load('gcc')
	
	try:
		ctx.find_program("sphinx-build", var="SPHINX_BUILD")
	except ctx.errors.ConfigurationError:
		pass
	
	ctx.env.append_unique('INCLUDES_CSPTERM',['include', 'client', 'Prototypes/libex', \
                                                  'albertasat-gomspace/albertasat-on-board-computer/liba/Subsystem/include', \
                                                  'CObject/liba/Class', 'CObject/liba/util', 'pforth/csrc'])
	ctx.env.append_unique('FILES_CSPTERM', 'src/*.c')
	ctx.env.append_unique('LIBS_CSPTERM', ['pforth', 'm', 'cutil', 'cclass', 'rt', 'pthread', 'elf', 'ncurses'])
	ctx.env.append_unique('DEFINES_CSPTERM', ['AUTOMATION', 'BDEBUG', 'OUTPUT_LOG', 'OUTPUT_LOG_NAME="' + os.getcwd() + '/logs/output_log.txt"', \
                                                  'PFORTH_DIC_PATH="pforth/build/unix/pforth.dic"'])
	ctx.env.append_unique('LIBPATH_CSPTERM', [os.getcwd() + '/Prototypes/libex/debug', \
						  os.getcwd() + '/CObject/liba/Class/debug', \
						  os.getcwd() + '/CObject/liba/util/debug', \
                                                  os.getcwd() + '/pforth/build/unix'])
        ctx.env.append_unique('LINKFLAGS_CSPTERM', ['-O0'])
	ctx.env.append_unique('CFLAGS_CSPTERM', ['-O0'])
	
	# Check WAF can find the required libraries.


	# Options for CSP
	ctx.options.with_os = 'posix'
	ctx.options.enable_rdp = True
	ctx.options.enable_qos = True
	ctx.options.enable_crc32 = True
	ctx.options.enable_hmac = True
	ctx.options.enable_xtea = True
	ctx.options.enable_promisc = True
	ctx.options.enable_if_kiss = True
	ctx.options.enable_if_can = True
	ctx.options.enable_if_zmqhub = True
	ctx.options.disable_stlib = True
	ctx.options.with_rtable = 'cidr'
	ctx.options.with_driver_can = 'socketcan'
	ctx.options.with_driver_usart = 'linux'

	# Options for libftp
	ctx.options.enable_ftp_server = True
	ctx.options.enable_ftp_client = True
	ctx.options.enable_fat = False
	ctx.options.enable_uffs = False
	ctx.options.enable_newlib = True
	
	# Options for liblog
	ctx.options.enable_log_node = True

	# Options for libutil
	ctx.options.clock = 'linux'
	ctx.options.enable_driver_debug = True
	ctx.options.with_log = 'cdh'
	ctx.options.enable_vmem = True
	ctx.options.enable_lzo = True

	# Options for libadcs
	if ctx.options.with_adcs == True:
		ctx.define('WITH_ADCS', '1')
		ctx.options.enable_adcs_client = True
	
	# Options for libparam
	ctx.options.enable_param_local_client = True
	ctx.options.enable_param_csp_server = True
	ctx.options.enable_param_csp_client = True
	ctx.options.enable_param_cmd = True
	ctx.options.param_lock = 'none'

	ctx.define('CSPTERM_VERSION', VERSION)

	# Recurse and write config
	ctx.write_config_header('include/conf_cspterm.h', top=True, remove=True)
	ctx.recurse(modules, mandatory=False)	

def build(ctx):
	ctx(export_includes=ctx.env.INCLUDES_CSPTERM, name='include')
	ctx.recurse(modules, mandatory=False)
	ctx.program(
		source=ctx.path.ant_glob(ctx.env.FILES_CSPTERM), 
		stdlibpath = ['-LCObject/liba/Class/debug/', '-LCObject/liba/util/debug', '-Lpforth/build/unix'],
		defines = ctx.env.DEFINES_NANOMIND,
		target='csp-term', 
		use=['CSPTERM', 'csp', 'param', 'util', 'gosh', 'ftp', 'log', 'cclass', 'cutil', 'pforth'],
		lib=ctx.env.LIBS_CSPTERM + ctx.env.LIBS
		)

def doc(ctx):
	ctx(
		cwd    = ctx.path.abspath(),
		rule   = 'doxygen doc/breathe.doxygen 2>&1 > /dev/null || echo doxygen error',
		source = './doc/breathe.doxygen',
		target = './doxygen/xml/index.xml',
		)
	ctx(
		rule   = "${SPHINX_BUILD} -q -c ./doc -b html -D release="+VERSION+" -d build/doc/doctrees . build/doc/html",
		cwd    = ctx.path.abspath(),
		source = ctx.path.ant_glob('**/*.rst') + ['doxygen/xml/index.xml'],
		target = './doc/html/doc/index.html',
		)
	ctx(
		rule   = "${SPHINX_BUILD} -q -c ./doc -b latex -D release="+VERSION+" -d build/doc/doctrees . build/doc/latex",
		cwd    = ctx.path.abspath(),
		source = ctx.path.ant_glob('**/*.rst') + ['doxygen/xml/index.xml'],
		target = './doc/latex/csp-term.tex',
		)
	ctx(
		cwd    = ctx.path.abspath(),
		rule   = 'make -C build/doc/latex all-pdf 2>&1 > /dev/null || echo make error',
		source = './doc/latex/csp-term.tex',
		target = './doc/latex/csp-term.pdf',
		)

from waflib.Build import BuildContext
class Doc(BuildContext):
   cmd = 'doc'
   fun = 'doc'
   
def dist(ctx):
    if not ctx.path.find_node('build/doc/latex/csp-term.pdf'):
        ctx.fatal('You forgot to run \'waf doc\' first, we need to include the documentation in the output')

    git_rev = os.popen('git describe --always --dirty=-dirty 2> /dev/null || echo unknown').read().strip()
    os.system('cp build/doc/latex/csp-term.pdf doc/csp-term-'+VERSION+'.pdf')
    os.system('cp -r build/doc/html doc/html')
    ctx.files = ctx.path.ant_glob(['waf', 'wscript', 'eclipse.py', 'client/*', 'src/*', 'lib/**', 'doc/**', 'CHANGELOG'])
