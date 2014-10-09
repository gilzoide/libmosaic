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
/// Puts the first CURS_MOS in imgs, making it a circular list with itself
void CircularIMGS (IMGS *imgs, CURS_MOS *mos);
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
 * Refreshes target's WINDOW, rewriting it from scratch
 *
 * @param[in] target CURS_MOS to have it's WINDOW refreshed
 */
void RefreshCURS_MOS (CURS_MOS *target);
/**
 * Resize a @ref CURS_MOS, resizing the MOSAIC inside and it's WINDOW
 * @sa ResizeCURS_MOS_WINDOW
 * 
 * @param[in] target the target CURS_MOS
 * @param[in] new_height CURS_MOS' new height
 * @param[in] new_width CURS_MOS' new width
 * 
 * @return 0 if successfully resized @ref CURS_MOS
 * @return ERR if allocation failed
 */
int ResizeCURS_MOS (CURS_MOS *target, int new_height, int new_width);
/**
 * Resize a @ref CURS_MOS' WINDOW
 * @sa ResizeCURS_MOS
 *
 * @note This function just resizes the WINDOW, without caring about erasing or
 * showing it on the screen
 *
 * @param[in] target the target CURS_MOS
 * @param[in] new_height CURS_MOS' new height
 * @param[in] new_width CURS_MOS' new width
 */
void ResizeCURS_MOS_WINDOW (CURS_MOS *target, int new_height, int new_width);
/**
 * Loads a @ref CURS_MOS from file.
 *
 * @param[in] target the target CURS_MOS
 * @param[in] file_name the name of the loading file
 *
 * @return 0 if successfully loaded CURS_MOS
 * @return errno on fail
 */
int LoadCURS_MOS (CURS_MOS *target, const char *file_name);
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
int curs_mosAddch (CURS_MOS *image, int y, int x, int c);

/**
 * Displays current CURS_MOS in the stdscr
 * 
 * @param[in] current CURS_MOS
 */
void DisplayCurrentMOSAIC (CURS_MOS *current);

/// Destroy a MOSMIG, deallocating the used memory
void FreeCURS_MOS (CURS_MOS *image);

#endif
