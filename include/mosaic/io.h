/*
 * Copyright 2017 Gil Barbosa Reis <gilzoide@gmail.com>
 * This file is part of libmosaic.
 * 
 * Libmosaic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Libmosaic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libmosaic.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Any bugs should be reported to <gilzoide@gmail.com>
 */

/** @file io.h
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
 * @ref mos_attr_storage_fmt.
 */
#ifndef __MOSAIC_IO_H_
#define __MOSAIC_IO_H_

#include "image.h"

#include <stdio.h>

/// Attribute storage format
typedef enum {
	MOS_NO_ATTR = '.',      ///< No attributes in this file, so load it all as MOS_DEFAULT_ATTR
	MOS_UNCOMPRESSED = 'U', ///< Binary part not compressed
	MOS_COMPRESSED = 'C',   ///< Binary part compressed with zlib
} mos_attr_storage_fmt;

/**
 * Reads image from the stream pointed to by stream.
 *
 * @note The attr is loaded with @ref MOS_DEFAULT_ATTR attributes even 
 * if we find an unknown attribute storage format
 *
 * @param[out] image The image to store what was read
 * @param[in] stream The stream to be read from
 *
 * @return @ref MOS_OK on success.
 * @return @ref MOS_EMALLOC on resize failure.
 * @return @ref MOS_ENODIMENSIONS if no dimensions are present.
 * @return @ref MOS_EUNKNSTRGFMT if unknown format is found.
 * @return @ref MOS_ECOMPRESSION on compression error.
 * @return @ref MOS_EUNSUPPORTED if compression is not supported.
 */
int fgetMOSAIC(MOSAIC *image, FILE *stream);

/**
 * Writes image in the stream pointed to by stream.
 *
 * @param[in] image The image to be saved
 * @param[in] fmt Compression format to be used
 * @param[out] stream The stream to be written to
 *
 * @return @ref MOS_OK on success.
 * @return @ref MOS_EUNKNSTRGFMT if unknown format is passed.
 * @return @ref MOS_ECOMPRESSION on compression error.
 * @return @ref MOS_EUNSUPPORTED if compression is not supported.
 */
int fputMOSAIC(const MOSAIC *image, mos_attr_storage_fmt fmt, FILE *stream);

/**
 * Saves the image in a file by its name
 * 
 * @param[in] image The image to be saved
 * @param[in] fmt Compression format to be used
 * @param[in] file_name The new file name
 * 
 * @return _errno_ on FILE failure.
 * @return @ref fputMOSAIC result otherwise.
 */
int mos_save(MOSAIC *image, mos_attr_storage_fmt fmt, const char *file_name);

/**
 * Loads the image from a file by its name
 * 
 * @param[out] image The image to be loaded onto
 * @param[in] file_name The file name
 * 
 * @return _errno_ on FILE failure.
 * @return @ref fgetMOSAIC result otherwise.
 */
int mos_load(MOSAIC *image, const char *file_name);

#endif
