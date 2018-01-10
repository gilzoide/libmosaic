libmosaic
=========
An open source ASC Art intended library.

__Why ASC Art?__ One doesn't question art, you just feel it.

Optional runtime dependencies: [zlib](http://www.zlib.net/).


Building
--------
Using [CMake](https://cmake.org/):

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make


Install
-------
From build directory:

	$ make install


Change Log
----------
+ 1.1.0 - Clone function, `is_inbounds` instead of `out_of_bounds` (makes more
  sense), `NULL` pointer check on `mos_free`.
+ 1.0.0 - Refactored, image creation, 8 basic colors + bold and underline,
  simple `FILE` I/O.

