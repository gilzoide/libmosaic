#include "curs_mos.h"


void InitIMGS (IMGS *everyone) {
	everyone->list = NULL;
	everyone->size = 0;
}


CURS_MOS *NewCURS_MOS (int new_height, int new_width) {
	// create new CURS_MOS
	CURS_MOS *new_image;
	if ((new_image = (CURS_MOS*) malloc (sizeof (CURS_MOS))) == NULL)
		return NULL;
	
	NewMOSAIC (&new_image->img, new_height, new_width);

	// create the curses window and panel
	new_image->win = newpad (new_height + 1, new_width + 1);
	dobox (new_image);	// put a border

	new_image->y = new_image->x = 0;
	scrollok (new_image->win, TRUE);
	
	new_image->pan = new_panel (new_image->win);
	DisplayCurrentMOSAIC (new_image);

	return new_image;
}


void dobox (CURS_MOS *img) {
	int i;
	int y = img->img.height;
	int x = img->img.width;
	// bottom
	for (i = 0; i < x; i++) {
		mvwaddch (img->win, y, i, ACS_HLINE);
	}
	// right
	for (i = 0; i < y; i++) {
		mvwaddch (img->win, i, x, ACS_VLINE);
	}
	// bottom-right corner
	mvwaddch (img->win, y, x, ACS_LRCORNER);
}


int ResizeCURS_MOS (CURS_MOS *target, int new_height, int new_width) {
	delwin (target->win);
	target->win = newpad (new_height + 1, new_width + 1);

	int i = ResizeMOSAIC (&target->img, new_height, new_width);
	
	if (i == -1) {
		fprintf (stderr, "Resize failed");
		exit (-1);
	}
	
	dobox (target);

	return i;
}


void LinkCURS_MOS (CURS_MOS *dest, CURS_MOS *src, enum direction dir) {
	if (dest != NULL) {
		CURS_MOS *aux;
		if (dir == before) {
			aux = dest->prev;
			dest->prev = aux->next = src;
			src->next = dest;
			src->prev = aux;			
		}
		else {		// after
			aux = dest->next;
			dest->next = aux->prev = src;
			src->prev = dest;
			src->next = aux;
		}
	}
	else
		fprintf (stderr, "Error: trying to link a CURS_MOS to a NULL pointer!!");
}


int mosAddch (CURS_MOS *image, int y, int x, int c) {
	if (y >= image->img.height || x >= image->img.width)
		return ERR;

	mvwaddch (image->win, y, x, c);
	image->img.mosaic[y][x] = c;
	return 0;
}


void DisplayCurrentMOSAIC (CURS_MOS *current) {
	top_panel (current->pan);
	update_panels ();
	doupdate ();
	prefresh (current->win, current->y, current->x, 0, 0, LINES - 2, COLS - 1);
}


void FreeCURS_MOS (CURS_MOS *image) {
	FreeMOSAIC (&image->img);
	del_panel (image->pan);
	delwin (image->win);
}
