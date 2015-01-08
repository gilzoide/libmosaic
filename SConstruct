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
`scons install-pkgconfig` or `scons install-bin`.

Everything can be uninstalled running `scons uninstall`.
""")

if not GetOption ('help'):
    env = Environment (
        CCFLAGS = '-Wall -pipe -O2',
        CPPPATH = '#include',
        CC = 'gcc',
        LIBS = 'z',
    )
    env.Decider ('MD5-timestamp')

    # if user pass debug=0, don't add -g flag for the compiler
    debug = ARGUMENTS.get ('debug', 1)
    if int (debug):
        env.Append (CCFLAGS = ' -g')

    # color needs Curses
    curs_env = env.Clone (
        LIBS = ['panel', 'curses'],
        LIBPATH = ['/usr/lib', '/usr/local/lib'],
    )

    # moscat needs mosaic{,_color}, so link with it as a shared library
    cat_env = env.Clone (LIBS = ['mosaic', 'mosaic_color', 'mosaic_stream_io'],
            LIBPATH = '#build')

    # build mosaic in the 'build' directory, without duplicating
    VariantDir ('build', 'src', duplicate = 0)
    SConscript ('build/SConscript', exports = ['env', 'curs_env', 'cat_env'])

    # headers is defined in the SConscript file globally
    curs_env.Command ("uninstall", None, Delete (FindInstalledFiles ()))
