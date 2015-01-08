#include "curs_mos.h"
#include "stream_io.h"

#include <stdlib.h>

void InitIMGS (IMGS *imgs) {
	imgs->list = NULL;
	imgs->size = 0;
}


void CircularIMGS (IMGS *imgs, CURS_MOS *mos) {
	imgs->list = mos;
	mos->next = mos->prev = mos;
}


CURS_MOS * NewCURS_MOS (int new_height, int new_width) {
	CURS_MOS *new_image;
	if ((new_image = (CURS_MOS *) malloc (sizeof (CURS_MOS))) == NULL) {
		return NULL;
	}

	new_image->img = NewMOSAIC (new_height, new_width);

	if (!new_image->img) {
		return NULL;
	}

	// create the curses window and panel
	new_image->win = newpad (new_height, new_width);

	new_image->y = new_image->x = 0;

	new_image->pan = new_panel (new_image->win);
	DisplayCurrentMOSAIC (new_image);

	return new_image;
}


void ResizeCURS_MOS_WINDOW (CURS_MOS *target, int new_height, int new_width) {
	delwin (target->win);
	target->win = newpad (new_height, new_width);
}


int ResizeCURS_MOS (CURS_MOS *target, int new_height, int new_width) {
	int aux = ResizeMOSAIC (target->img, new_height, new_width);
	// resizing was not a problem, so resize the WINDOW
	if (!aux) {
		ResizeCURS_MOS_WINDOW (target, new_height, new_width);
	}
	return aux;
}


int TrimCURS_MOS (CURS_MOS *target, char resize) {
	int aux = TrimMOSAIC (target->img, resize);
	// if TrimMOSAIC returns no errors and needs to resize, do it!
	if (!aux && resize) {
		ResizeCURS_MOS_WINDOW (target, target->img->height, target->img->width);
	}
	return aux;
}


void RefreshCURS_MOS (CURS_MOS *target) {
	wmove (target->win, 0, 0);
	
	// write in the WINDOW
	int i, j;
	for (i = 0; i < target->img->height; i++) {
		for (j = 0; j < target->img->width; j++) {
			wattrset (target->win, CursAttr (target->img->attr[i][j]));
			mvwaddch (target->win, i, j, target->img->mosaic[i][j]);
		}
	}

	wstandend (target->win);

	prefresh (target->win, target->y, target->x, 0, 0, LINES - 2, COLS - 1);
}


void LinkCURS_MOS (CURS_MOS *dest, CURS_MOS *src, enum direction dir) {
	if (dest != NULL) {
		CURS_MOS *aux;
		if (dir == after) {
			aux = dest->next;
			dest->next = aux->prev = src;
			src->prev = dest;
			src->next = aux;
		}
		else {		// before
			aux = dest->prev;
			dest->prev = aux->next = src;
			src->next = dest;
			src->prev = aux;			
		}
	}
	else {
		fprintf (stderr, "Error: trying to link a CURS_MOS to a NULL pointer!!");
	}
}


int curs_mosAddch (CURS_MOS *image, int y, int x, int c) {
	if (!mosAddCh (image->img, y, x, c)) {
		return ERR;
	}

	wattron (image->win, CursAttr (image->img->attr[y][x]));
	mvwaddch (image->win, y, x, c);
	wstandend (image->win);
	return 0;
}


int curs_mosSetAttr (CURS_MOS *image, int y, int x, mos_attr a) {
	if (!mosSetAttr (image->img, y, x, a)) {
		return ERR;
	}

	int bold = extractBold (&a);

	mvwchgat (image->win, y, x, 1, bold ? A_BOLD : A_NORMAL, a, NULL);
	return 0;
}


void DisplayCurrentMOSAIC (CURS_MOS *current) {
	show_panel (current->pan);
	update_panels ();
	doupdate ();
	prefresh (current->win, current->y, current->x, 0, 0, LINES - 2, COLS - 1);
}


int fgetCURS_MOS (CURS_MOS *target, FILE *stream) {
	// get the MOSAIC, please
	int aux = fgetMOSAIC (target->img, stream);

	if (aux == 0 || aux == EUNKNSTRGFMT) {
		// resize only target's WINDOW, as the MOSAIC was resized on fgetMOSAIC
		ResizeCURS_MOS_WINDOW (target, target->img->height, target->img->width);
		// refreshing
		RefreshCURS_MOS (target);
	}

	return aux;
}


int LoadCURS_MOS (CURS_MOS *target, const char *file_name) {
	// load the MOSAIC, please
	int aux = LoadMOSAIC (target->img, file_name);

	if (aux == 0 || aux == EUNKNSTRGFMT) {
		// resize only target's WINDOW, as the MOSAIC was resized on LoadMOSAIC
		ResizeCURS_MOS_WINDOW (target, target->img->height, target->img->width);
		// refreshing
		RefreshCURS_MOS (target);
	}

	return aux;
}


int LoadIMGS (IMGS *imgs, const char *file_name) {
	FILE *f;
	if ((f = fopen (file_name, "w")) == NULL) {
		return errno;
	}
	
	int ret;
	CURS_MOS *new_image;

	// first CURS_MOS
	new_image = NewCURS_MOS (0, 0);
	ret = fgetCURS_MOS (new_image, f);
	if (ret != 0 && ret != EUNKNSTRGFMT) {
		fclose (f);
		return ERR;
	}
	CircularIMGS (imgs, new_image);

	// and the others
	while (1) {
		new_image = NewCURS_MOS (0, 0);
		ret = fgetCURS_MOS (new_image, f);
		if (ret != 0 && ret != EUNKNSTRGFMT) {
			break;
		}
		LinkCURS_MOS (imgs->list->prev, new_image, after);
	}

	fclose (f);

	return 0;
}


void FreeCURS_MOS (CURS_MOS *image) {
	FreeMOSAIC (image->img);
	del_panel (image->pan);
	delwin (image->win);
	free (image);
}


void DestroyIMGS (IMGS *everyone) {
	CURS_MOS *aux, *next;
	unsigned int i;

	for (aux = everyone->list, i = 0; i < everyone->size; i++, aux = next) {
		next = aux->next;
		FreeCURS_MOS (aux);
	}
}
