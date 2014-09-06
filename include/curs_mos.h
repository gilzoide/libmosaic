/** @file curs_mos.h
 * @ref MOSAIC wrapper for curses
 */

#ifndef CURS_MOS_H
#define CURS_MOS_H

#include <curses.h>
#include <panel.h>
#include "mosaic.h"

/**
 * Curses Mosaic: wraps the MOSAIC; list and curses interface
 * 
 * The MOSAIC structured as a double linked list, for sequencial matters ["video",
 * as we like to call it] and Curses interface.
 */
typedef struct CURS_MOS_t {
	MOSAIC img;	///< the asc art itself
	int y,		///< the Y coordinate of the piece of @ref img showed at the pad
		x;		///< the Y coordinate of the piece of @ref img showed at the pad
	struct CURS_MOS_t *prev,	///< previous CURS_MOS
					*next;	///< next CURS_MOS
	WINDOW *win;	///< window to show the MOSAIC
	PANEL *pan;	///< panel for showing the window
} CURS_MOS;


/// Whole images circular double linked list and it's size
typedef struct {
	CURS_MOS *list;	///< the first image
	int size;	///< the array size
} IMGS;


/// Initializes the IMGS
void InitIMGS (IMGS *everyone);

/** 
 * Create a new @ref CURS_MOS, allocating the necessary memory
 * 
 * @param[in] new_height New MOSAIC's height
 * @param[in] new_width New MOSAIC's width
 * 
 * @return New @ref CURS_MOS, clean and unlinked
 * @return __NULL__ if allocation failed
 */
CURS_MOS *NewCURS_MOS (int new_height, int new_width);
/** aux for the NewCURS_MOS: creates the right and bottom border */
void dobox (CURS_MOS *img);

/**
 * Resize a @ref CURS_MOS, reallocating the necessary memory and resizing it's WINDOW
 * @note This function just resizes the WINDOW, without caring about erasing or printing it on the screen
 * 
 * @param[in] target the target MOSAIC
 * @param[in] new_height MOSAIC's new height
 * @param[in] new_width MOSAIC's new width
 * 
 * @return 0 if successfully resized @ref CURS_MOS
 * @return -1 if allocation failed
 */
int ResizeCURS_MOS (CURS_MOS *target, int new_height, int new_width);

/**
 * Only to say if you want to link the img before or after the other in LinkMOSAIC
 */
enum direction {after = 0, before};
/**
 * Link an image to another, before or after it
 * 
 * @param[in] dest image linked to
 * @param[in] src image to link to dest
 * @param[in] dir link src before or after dest
 * 
 * @return pointer to the linked CURS_MOS, to be stored in the 'current'
 */
void LinkCURS_MOS (CURS_MOS *dest, CURS_MOS *src, enum direction dir);

/**
 * Add a char to the CURS_MOS at position y/x and print it in it's WINDOW.
 * 
 * @note It doesn't refresh the WINDOW, so do it yourself whenever you feel ready
 * 
 * If position is outside image boundaries, returns ERR, as it calls mvwaddch, which calls wmove internally
 * 
 * @return 0 on success
 * @return ERR on error
 */
int mosAddch (CURS_MOS *image, int y, int x, int c);

/**
 * Displays current CURS_MOS in the stdscr
 * 
 * @param[in] current CURS_MOS
 */
void DisplayCurrentMOSAIC (CURS_MOS *current);

/// Destroy a MOSMIG, deallocating the used memory
void FreeCURS_MOS (CURS_MOS *image);

#endif
