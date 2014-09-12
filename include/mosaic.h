/** @file mosaic.h
 * .mosi format definitions and operations
 */
 
#ifndef IMG_H
#define IMG_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "color.h"

/**
 * "MOSAIC" in Nmos mosaic format
 */
typedef struct {
	int height;	///< img height
	int	width;	///< img width
	unsigned char **mosaic;		/**< a height * width sized string: the drawing itself */
	Attr **attr;	/**< a height * width sized array with the attributes for each char. */
} MOSAIC;



/**
 * Inline function that measures the total size of an image
 * 
 * @param[in] img MOSAIC to be sized
 * 
 * @return image size: height * width
 */
inline int MOSAICSize (MOSAIC img);


/** 
 * Create a new @ref MOSAIC, allocating the necessary memory
 * 
 * @param[in] img The MOSAIC to be initialized
 * @param[in] new_height New MOSAIC's height
 * @param[in] new_width New MOSAIC's width
 * 
 * @return 0 on success
 * @return -1 if allocation failed
 */
int NewMOSAIC (MOSAIC *img, int new_height, int new_width);
/**
 * Resize a @ref MOSAIC, reallocating the necessary memory
 * 
 * @param[in] img The target MOSAIC
 * @param[in] new_height MOSAIC's new height
 * @param[in] new_width MOSAIC's new width
 * 
 * @return 0 if successfully resized @ref MOSAIC
 * @return -1 if allocation failed
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
 */
void CopyMOSAIC (MOSAIC *dest, MOSAIC *src);

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
 * @return 0 on success, _errno_ on failure
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
 * @return 0 on success, _errno_ on failure, 1 on no dimensions present in the file
 */
int LoadMOSAIC (MOSAIC *image, const char *file_name);

/// Destroy an image, deallocating the used memory
void FreeMOSAIC (MOSAIC *img);

#endif
