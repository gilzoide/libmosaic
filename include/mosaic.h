/** @file mosaic.h
 * .mosi format definitions and operations
 */
 
#ifndef IMG_H
#define IMG_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "color.h"

/// Char representation inside MOSAIC
typedef unsigned char mos_char;

/**
 * "Image" in mosaic format
 */
typedef struct {
	int height;	///< img height
	int	width;	///< img width
	mos_char **mosaic;		/**< a height * width sized string: the drawing itself */
	mos_attr **attr;	/**< a height * width sized array with the attributes for each char. */
	unsigned char isSub : 2;
} MOSAIC;

/// Mosi text/binary separator
#define SEPARATOR 31


/**
 * Inline function that measures the total size of an image
 * 
 * @param[in] img MOSAIC to be sized
 * 
 * @return image size: height * width
 */
int MOSAICSize (MOSAIC *img);

/**
 * Checks whether a y/x point is in or out of img's boundaries
 *
 * @param[in] img Target MOSAIC
 * @param[in] y Y coordinate
 * @param[in] x X coordinate
 *
 * @return 1 if out of boundaries
 * @return 0 otherwise
 */
char outOfBoundaries (MOSAIC *img, int y, int x);

/**
 * Add a char to the MOSAIC at position y/x
 * 
 * @note If position is outside image boundaries, returns 0 
 *
 * @param[in] img Target MOSAIC
 * @param[in] y Y coordinate
 * @param[in] x X coordinate
 * @param[in] c Char to be placed
 *
 * @return 1 on success
 * @return 0 on out of boundaries
 */
int mosAddch (MOSAIC *img, int y, int x, mos_char c);
/**
 * Changes a MOSAIC's mos_char's mos_attr, at position y/x
 * 
 * @note If position is outside image boundaries, returns 0 
 *
 * @param[in] img Target MOSAIC
 * @param[in] y Y coordinate
 * @param[in] x X coordinate
 * @param[in] a New char attribute
 *
 * @return 1 on success
 * @return 0 on out of boundaries
 */
int mosSetAttr (MOSAIC *img, int y, int x, mos_attr a);
/**
 * Get the char at position y/x
 *
 * @note If position is outside image boundaries, returns 0 
 *
 * @param[in] img Target MOSAIC
 * @param[in] y Y coordinate
 * @param[in] x X coordinate
 *
 * @return Char in mosaic[y][x]
 * @return 0 on out of boundaries
 */
mos_char mosGetch (MOSAIC *img, int y, int x);
/**
 * Changes a MOSAIC's mos_char's mos_attr, at position y/x
 * 
 * @note If position is outside image boundaries, returns 0 
 *
 * @param[in] img Target MOSAIC
 * @param[in] y Y coordinate
 * @param[in] x X coordinate
 * @param[in] a New char attribute
 *
 * @return The mos_attr
 * @return Normal if out of Boundaries, as it's a default value
 */
mos_attr mosGetAttr (MOSAIC *img, int y, int x);
/** 
 * Create a new @ref MOSAIC, allocating the necessary memory
 * 
 * @param[in] img The MOSAIC to be initialized
 * @param[in] new_height New MOSAIC's height
 * @param[in] new_width New MOSAIC's width
 * 
 * @return A MOSAIC on success
 * @return NULL if allocation failed
 */
MOSAIC * NewMOSAIC (int new_height, int new_width);
/**
 * Gets a MOSAIC inside a MOSAIC: MOSAICception!
 *
 * @note SubMOSAICs shares memory with it's relative MOSAIC, so
 * changes affecting one MOSAIC will affect the other one.
 *
 * @note Freeing a SubMOSAIC before or after it's relative doesn't make a
 * difference, as the actual content will be freed only from the relative MOSAIC
 *
 * @param[in] parent	The outter MOSAIC
 * @param[in] begin_y 	The upper-left Y coordinate, where inner MOSAIC begins
 * @param[in] begin_x 	The upper-left X coordinate, where inner MOSAIC begins
 * @param[in] height	Inner MOSAIC's height
 * @param[in] width		Inner MOSAIC's width
 *
 * @return SubMOSAIC
 * @return NULL if img is NULL, or if coordinates get out of bounds
 */
MOSAIC * SubMOSAIC (MOSAIC *parent, int begin_y, int begin_x, int height, int width);
/**
 * Resize a @ref MOSAIC, reallocating the necessary memory
 * 
 * @param[in] img The target MOSAIC
 * @param[in] new_height MOSAIC's new height
 * @param[in] new_width MOSAIC's new width
 * 
 * @return 0 if successfully resized @ref MOSAIC
 * @return ERR if allocation failed
 */
int ResizeMOSAIC (MOSAIC *img, int new_height, int new_width);
/**
 * Clone a MOSAIC, from _src_ to _dest_
 *
 * @note If _dest_'s width or height are less than _src_'s,
 * the MOSAIC is truncated
 *
 * @param[out] dest Target MOSAIC
 * @param[in] src Source MOSAIC
 *
 * @return 0 if successfully copied from src to dest
 * @return ERR otherwise
 */
int CopyMOSAIC (MOSAIC *dest, MOSAIC *src);
/**
 * Trims a MOSAIC's blank area in each side of target
 *
 * @note You can ask TrimMOSAIC to resize your image. It's expensive to
 * do so, so think carefully about it. If not resizing, it'll just shift
 * the image, as if you copied the rectangle with the mosaic
 * and pasted it at (0,0).
 *
 * @param[in] target Target MOSAIC
 * @param[in] resize Bool: sould we resize the mosaic?
 *
 * @return The return from ResizeMOSAIC if resizing
 * @return ERR if asked for a resize in a blank MOSAIC
 * @return 0 otherwise (it'll never go wrong if not resizing and used wisely)
 */
int TrimMOSAIC (MOSAIC *target, char resize);

/**
 * Saves the image in a file
 * 
 * The file has a header with the mosaic dimensions,
 * the asc art itself and it's attributes.
 * 
 * @note A .mosi file is hybrid.
 * The dimensions and @ref MOSAIC::mosaic are text, so they can be viewed in 
 * any text editor. The @ref MOSAIC::attr part is binary, so expect some weird
 * stuff while oppening it as text.
 * 
 * @param[in] image The image to be saved
 * @param[in] file_name The new file name
 * 
 * @return 0 on success
 * @return _errno_ on failure
 */
int SaveMOSAIC (MOSAIC *image, const char *file_name);
/**
 * Loads the image from a file
 * 
 * It's the same scheme from the @ref SaveMOSAIC function
 * 
 * @param[out] image The image to be loaded onto
 * @param[in] file_name The file name
 * 
 * @return 0 on success
 * @return _errno_ on failure
 * @return 1 if no dimensions present in the file
 */
int LoadMOSAIC (MOSAIC *image, const char *file_name);

/// Destroy an image, deallocating the used memory
void FreeMOSAIC (MOSAIC *img);

#endif
