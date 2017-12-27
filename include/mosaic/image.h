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

/** @file image.h
 * Mosaic image format and operations.
 */

#ifndef __MOSAIC_IMAGE_H__
#define __MOSAIC_IMAGE_H__

#include <stdint.h>

/**
 * Char representation inside a MOSAIC.
 */
typedef char mos_char;

/**
 * "Image" in MOSAIC format.
 */
typedef struct {
	int height;	///< img height
	int	width;	///< img width
	mos_char **mosaic;		///< a height * width sized string: the drawing itself
	mos_attr **attr;	///< a height * width sized array with the attributes for each char
	unsigned char is_sub : 1;	///< boolean: is it a subMOSAIC?
} MOSAIC;

/// Default attribute for Mosaics: white on black
#define MOS_DEFAULT_ATTR 0b00000111
/// Default char for Mosaics: blank
#define MOS_DEFAULT_CHAR ' '

/** 
 * Create a new @ref MOSAIC, allocating the necessary memory.
 * 
 * @param[in] height New MOSAIC's height
 * @param[in] width  New MOSAIC's width
 * 
 * @return A MOSAIC on success
 * @return NULL if allocation failed
 */
MOSAIC *mos_new(int height, int width);

/**
 * Destroy an image, deallocating the memory used
 */
void mos_free(MOSAIC *img);

/**
 * Measures the total size of an image
 * 
 * @param[in] img MOSAIC to be sized
 * 
 * @return image size: height * width
 */
int mos_size(const MOSAIC *img);

/**
 * Checks whether a y/x point is in or out of img's boundaries
 *
 * @param[in] img Target MOSAIC
 * @param[in] y   Y coordinate
 * @param[in] x   X coordinate
 *
 * @return 1 if out of boundaries
 * @return 0 otherwise
 */
int mos_out_of_bounds(const MOSAIC *img, int y, int x);

/**
 * Add a char to the MOSAIC at position y/x
 *
 * @param[in] img Target MOSAIC
 * @param[in] y   Y coordinate
 * @param[in] x   X coordinate
 * @param[in] c   Char to be placed
 *
 * @return The passed char
 */
mos_char mos_set_char(MOSAIC *img, int y, int x, mos_char c);

/**
 * Changes a MOSAIC's mos_char's attribute, at position y/x
 *
 * @param[in] img Target MOSAIC
 * @param[in] y   Y coordinate
 * @param[in] x   X coordinate
 * @param[in] a   New char attribute
 *
 * @return The passed attribute
 */
mos_attr mos_set_attr(MOSAIC *img, int y, int x, mos_attr a);

/**
 * Get the char at position y/x
 *
 * @param[in] img Target MOSAIC
 * @param[in] y   Y coordinate
 * @param[in] x   X coordinate
 *
 * @return Char in mosaic[y][x]
 */
mos_char mos_get_char(const MOSAIC *img, int y, int x);

/**
 * Changes a MOSAIC's mos_char's attribute, at position y/x
 *
 * @param[in] img Target MOSAIC
 * @param[in] y   Y coordinate
 * @param[in] x   X coordinate
 *
 * @return Attribute in attr[y][x]
 */
mos_attr mos_get_attr(const MOSAIC *img, int y, int x);

/**
 * Gets a MOSAIC inside a MOSAIC: MOSAICception!
 *
 * @note SubMOSAICs share memory with their parent MOSAIC, so
 * changes affecting one MOSAIC will affect the other one.
 *
 * @note Freeing a SubMOSAIC before or after it's relative doesn't make a
 * difference, as the actual content will be freed only from the relative MOSAIC
 *
 * @param[in] parent  The outter MOSAIC
 * @param[in] height  Inner MOSAIC's height
 * @param[in] width   Inner MOSAIC's width
 * @param[in] begin_y The upper-left Y coordinate, where inner MOSAIC begins
 * @param[in] begin_x The upper-left X coordinate, where inner MOSAIC begins
 *
 * @return SubMOSAIC
 * @return NULL on allocation errors
 */
MOSAIC *mos_submosaic(MOSAIC *parent, int height, int width, int begin_y, int begin_x);

/**
 * Resize a @ref MOSAIC, reallocating the necessary memory
 * 
 * @param[in] img The target MOSAIC
 * @param[in] new_height MOSAIC's new height
 * @param[in] new_width MOSAIC's new width
 * 
 * @return @ref MOS_OK if successfully resized @ref MOSAIC
 * @return @ref MOS_EMALLOC on `malloc` errors
 */
int mos_resize(MOSAIC *img, int new_height, int new_width);

/**
 * Clone a MOSAIC, from _src_ to _dest_
 *
 * @note If _dest_'s width or height are less than _src_'s,
 * the MOSAIC is truncated
 *
 * @param[out] dest Target MOSAIC
 * @param[in] src   Source MOSAIC
 */
void mos_copy(MOSAIC *dest, MOSAIC *src);

/**
 * Trim a MOSAIC's blank area in each side of target
 *
 * @note Resizing the image may be expensive, so think carefully about it. If
 * not resizing, it'll just shift the image, as if you copied the rectangle
 * with the mosaic and pasted it at (0,0).
 *
 * @param[in] target Target MOSAIC
 * @param[in] resize Bool: sould we resize the mosaic?
 *
 * @return The result from @ref mos_resize if resizing.
 * @return @ref MOS_OK otherwise.
 */
int mos_trim(MOSAIC *target, char resize);

#endif
