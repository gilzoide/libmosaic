/** @file stream_io.h
 * Mosaic's input/output definitions, for handling saving and loading from files
 *
 * The .mosi file has a header with the mosaic dimensions,
 * the asc art itself and it's attributes.
 *
 * @note A .mosi file is hybrid.
 * The dimensions and @ref MOSAIC::mosaic are text, so they can be viewed in 
 * any text editor. The @ref MOSAIC::attr part is binary, so expect some weird
 * stuff while oppening it as text.
 *
 * @note Note also that there are more than one way of keeping the binary part,
 * from uncompressed, to compressed and no binary at all. There's a separator
 * after the text part of the file, which is SEPARATOR and one of
 * @ref attr_storage_fmt.
 *
 * @note A .mosv file is a file with multiple mosaics in sequence.
 */
#ifndef STREAM_IO_H
#define STREAM_IO_H

#include <errno.h>

#include "mosaic.h"

/// Mosi text/binary separator char
#define SEPARATOR '\t'

/// Mosi stream error code: "no dimensions specified"
#define ENODIMENSIONS -2
/// Mosi stream error code: "unknown attribute storage format"
#define EUNKNSTRGFMT -3

/// Attribute storage format
typedef enum {
	UNCOMPRESSED = 'U',	///< Binary part not compressed
	COMPRESSED = 'C',	///< Binary part compressed with zlib
	NO_ATTR = '.'	///< No attributes in this file, so load it all as Normal
} attr_storage_fmt;

/// The default attribute storage type
#define DEFAULT_STORAGE_FMT COMPRESSED

/**
 * Checks if fmt is an available storage format
 *
 * @param[in] fmt Format to be tested
 *
 * @return 1 if valid
 * @return 0 otherwise
 */
char isValidFormat (attr_storage_fmt fmt);

/**
 * Reads image from the stream pointed to by stream.
 *
 * @note The attr is loaded with Normal attributes even 
 * if we find an unknown attribute storage format
 *
 * @param[out] image The image to store what was read
 * @param[in] stream The stream to be read from
 *
 * @return 0 on success
 * @return ERR on failure
 * @return ENODIMENSIONS if no dimensions are present
 * @return EUNKNSTRGFMT if unknown format is found
 */
int fgetMOSAIC (MOSAIC *image, FILE *stream);
/**
 * Writes image in the stream pointed to by stream.
 *
 * @param[in] image The image to be saved
 * @param[in] fmt Compression format to be used
 * @param[out] stream The stream to be written to
 *
 * @return 0 on success
 * @return EUNKNSTRGFMT if unknown format is passed
 * @return ERR for other errors (might be error in the compression)
 */
int fputFmtMOSAIC (MOSAIC *image, attr_storage_fmt fmt, FILE *stream);
/// Just a nice way to call fputMOSAIC with the default storage format
#define fputMOSAIC(img, stream) \
		fputFmtMOSAIC ((img), DEFAULT_STORAGE_FMT, (stream))
/**
 * Saves the image in a file by its name
 * 
 * @param[in] image The image to be saved
 * @param[in] fmt Compression format to be used
 * @param[in] file_name The new file name
 * 
 * @return 0 on success
 * @return _errno_ on failure
 */
int SaveFmtMOSAIC (MOSAIC *image, attr_storage_fmt fmt, const char *file_name);
/// Just a nice way to call Save with the default storage format
#define SaveMOSAIC(img, file_name) \
		SaveFmtMOSAIC ((img), DEFAULT_STORAGE_FMT, (file_name))
/**
 * Loads the image from a file by its name
 * 
 * @param[out] image The image to be loaded onto
 * @param[in] file_name The file name
 * 
 * @return 0 on success
 * @return _errno_ on failure
 * @return ENODIMENSIONS if no dimensions present in the file
 * @return EUNKNSTRGFMT if unknown format is found in the file
 */
int LoadMOSAIC (MOSAIC *image, const char *file_name);

#endif
