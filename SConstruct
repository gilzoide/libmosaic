# libmosaic, an Asc Art intended library
# Created by Gil "gilzoide" Barbosa Reis

Help ("""
Welcome to the libmosaic's build script

The default build output is the build/ directory.
This package provide the shared libraries for mosaic and curses wrapper, 
and a cat application (moscat).

You can `scons install` everything in the /usr/{lib,include,bin}/ directories.
For installing only a part of libmosaic (maybe you want only the shared 
libraries), you can `scons install-lib` or `scons install-include` or 
`scons install-bin`.

Everything can be uninstalled running `scons uninstall`.
""")

if not GetOption ('help'):
	env = Environment (
		CCFLAGS = '-Wall -pipe',
		CPPPATH = '#include',
	)
	env.Decider ('MD5-timestamp')
	
	# headers and install headers
	headers = ['libmosaic.h', 'libmosaic_color.h', 'libmosaic_curses.h']
	instHeaders = ['/usr/include/' + h for h in headers]

	curs_env = env.Clone (LIBS = ['panel', 'curses'], LIBPATH = ['/usr/lib', 
			'/usr/local/lib'])

	# build mosaic in the 'build' directory, without duplicating
	VariantDir ('build', 'src', duplicate = 0)
	SConscript ('build/SConscript', exports = ['env', 'curs_env', 
			'headers', 'instHeaders'])

	# headers is defined in the SConscript file globally
	curs_env.Command ("uninstall", None, Delete (FindInstalledFiles() 
			+ instHeaders))
