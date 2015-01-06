Libmosaic
=========

Mosaic, an open source ASC Art intended library.

Run dependencies: curses (or _ncurses_).

Build dependencies: [scons](http://www.scons.org/).

For more information on building libmosaic, hit `scons -h` in your console.

### FAQ
1. __Why ASC Art?__ One doesn't question art, you just feel it.
2. __How can I include libmosaic in my project?__ `#include <mosaic/_header_.h>`, \_header_ being any of the ones in the _include/_ directory.
3. __What about linking?__ `-lmosaic -lmosaic_color -lmosaic_curses`. __TODO__ : make a pkg-config .pc for libmosaic.
