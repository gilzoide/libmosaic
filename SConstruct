# libmosaic, an Asc Art intended library
# Created by Gil Barbosa Reis

env = Environment (
	CCFLAGS = '-Wall -pipe',
	CPPPATH = '#include',
)
env.Decider ('MD5-timestamp')

curs_env = env.Clone (LIBS = ['panel', 'curses'], LIBPATH = ['/usr/lib', '/usr/local/lib'])

# build the editor in the 'build' directory, without duplicating
VariantDir ('build', 'src', duplicate = 0)
SConscript ('build/SConscript', exports = ['env', 'curs_env'])

curs_env.Command ("uninstall", None, Delete (FindInstalledFiles()))
