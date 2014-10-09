#include "curs_mos.h"


void InitIMGS (IMGS *imgs) {
	imgs->list = NULL;
	imgs->size = 0;
}


void CircularIMGS (IMGS *imgs, CURS_MOS *mos) {
	imgs->list = mos;
	mos->next = mos->prev = mos;
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


void ResizeCURS_MOS_WINDOW (CURS_MOS *target, int new_height, int new_width) {
	delwin (target->win);
	target->win = newpad (new_height + 1, new_width + 1);

	dobox (target);
}


int ResizeCURS_MOS (CURS_MOS *target, int new_height, int new_width) {
	ResizeCURS_MOS_WINDOW (target, new_height, new_width);
	return ResizeMOSAIC (&target->img, new_height, new_width);
}


void RefreshCURS_MOS (CURS_MOS *target) {
	wmove (target->win, 0, 0);
	
	// write in the WINDOW
	int i, j;
	for (i = 0; i < target->img.height; i++) {
		for (j = 0; j < target->img.width; j++) {
			mvwaddch (target->win, i, j, target->img.mosaic[i][j]);
		}
	}

	prefresh (target->win, target->y, target->x, 0, 0, LINES - 2, COLS - 1);
}


int LoadCURS_MOS (CURS_MOS *target, const char *file_name) {
	// load the MOSAIC, please
	int aux = LoadMOSAIC (&target->img, file_name);
	if (aux != 0)
		return aux;

	// resize only target's WINDOW, as the MOSAIC was resized on LoadMOSAIC
	ResizeCURS_MOS_WINDOW (target, target->img.height, target->img.width);
	// refreshing
	RefreshCURS_MOS (target);

	return 0;
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
	else {
		fprintf (stderr, "Error: trying to link a CURS_MOS to a NULL pointer!!");
	}
}


int curs_mosAddch (CURS_MOS *image, int y, int x, int c) {
	if (!mosAddch (&image->img, y, x, c)) {
		return ERR;
	}

	mvwaddch (image->win, y, x, c);
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
