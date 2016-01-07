#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mosaic.h"

inline int max (int a, int b) {
	return (a > b ? a : b);
}


inline int min (int a, int b) {
	return (a < b ? a : b);
}


char mosOutOfBoundaries (MOSAIC *img, int y, int x) {
	return (y < 0 || y >= img->height || x < 0 || x >= img->width);
}


int MOSAICSize (MOSAIC *img) {
	return img->height * img->width;
}


MOSAIC * NewMOSAIC (int new_height, int new_width) {
	MOSAIC *img = (MOSAIC *) malloc (sizeof (MOSAIC));
	if (img == NULL) {
		return NULL;
	}
	// dimensions still 0
	img->height = img->width = 0;
	// NULL pointers, for realloc to work as a malloc
	img->mosaic = NULL;
	img->attr = NULL;
	// it's not a subMOSAIC
	img->isSub = 0;
	
	// alloc the dinamic stuff and fill it: something ResizeMOSAIC already does
	int value = ResizeMOSAIC (img, new_height, new_width);

	// allocation failed
	if (value) {
		free (img);
		return NULL;
	}

	return img;
}


MOSAIC * SubMOSAIC (MOSAIC *parent, int height, int width, int begin_y, int begin_x) {
	MOSAIC *img = (MOSAIC *) malloc (sizeof (MOSAIC));
	if (img == NULL) {
		return NULL;
	}

	img->height = height;
	img->width = width;
	img->isSub = 1;	// hello, i'm a subMOSAIC

	// allocate just the lines
	// mosaic
	img->mosaic = (mos_char **) malloc (height * sizeof (mos_char *));
	if (img->mosaic == NULL) {
		free (img);
		return NULL;
	}
	// attr
	img->attr = (mos_attr **) malloc (height * sizeof (mos_attr *));
	if (img->attr == NULL) {
		free (img->mosaic);
		free (img);
		return NULL;
	}

	// now make mosaic/attr point to the parent MOSAIC at the right closure
	int i;
	for (i = 0; i < height; i++) {
		// using pointer arithmetics, cuz it's cool
		img->mosaic[i] = parent->mosaic[begin_y + i] + begin_x;
		img->attr[i] = parent->attr[begin_y + i] + begin_x;
	}

	return img;
}


int mosSetCh (MOSAIC *img, int y, int x, mos_char c) {
	if (mosOutOfBoundaries (img, y, x)) {
		return 0;
	}
	else {
		img->mosaic[y][x] = c;
		return 1;
	}
}


int mosSetAttr (MOSAIC *img, int y, int x, mos_attr a) {
	if (!mosOutOfBoundaries (img, y, x)) {
		img->attr[y][x] = a;
		return 1;
	}
	else {
		return 0;
	}
}


mos_char mosGetCh (MOSAIC *img, int y, int x) {
	if (!mosOutOfBoundaries (img, y, x)) {
		return img->mosaic[y][x];
	}
	else {
		return 0;
	}
}


mos_attr mosGetAttr (MOSAIC *img, int y, int x) {
	if (!mosOutOfBoundaries (img, y, x)) {
		return img->attr[y][x];
	}
	else {
		return Normal;
	}
}


int ResizeMOSAIC (MOSAIC *img, int new_height, int new_width) {
	// old dimensions
	const int old_height = img->height;
	const int old_width = img->width;
	// new dimensions
	img->height = new_height;
	img->width = new_width;
	
	int i;
	// when shrinking the MOSAIC, free the lines we're discarding
	for (i = old_height - 1; i >= new_height; i--) {
		free (img->mosaic[i]);
		free (img->attr[i]);
	}
	
	
	// realloc the dinamic stuff
	// Lines
	// mosaic:
	if ((img->mosaic = (mos_char **) realloc (
			img->mosaic, new_height * sizeof (mos_char *))) == NULL) {
		return ERR;
	}
	// when growing, initialize lines with NULL, so realloc mallocs them
	for (i = old_height; i < new_height; i++) {
		img->mosaic[i] = NULL;
	}
	// attributes:
	if ((img->attr = (mos_attr **) realloc (
			img->attr, new_height * sizeof (mos_attr *))) == NULL) {
		return ERR;
	}
	// when growing, initialize lines with NULL, so realloc mallocs them
	for (i = old_height; i < new_height; i++) {
		img->attr[i] = NULL;
	}

	// Columns
	for (i = 0; i < new_height; i++) {
		if ((img->mosaic[i] = (mos_char *) realloc (
				img->mosaic[i], new_width * sizeof (mos_char))) == NULL) {
			return ERR;
		}
		if ((img->attr[i] = (mos_attr *) realloc (
				img->attr[i], new_width * sizeof (mos_attr))) == NULL) {
			return ERR;
		}
	}
	
	// maybe it grew, so complete with blanks
	// new lines, until old width
	for (i = old_height; i < new_height; i++) {
		memset (img->mosaic[i], ' ', old_width);
		memset (img->attr[i], Normal, old_width);
	}
	// new columns, until old height
	if (new_width > old_width) {
		for (i = 0; i < old_height; i++) {
			memset (img->mosaic[i] + old_width, ' ', new_width - old_width);
			memset (img->attr[i] + old_width, Normal, new_width - old_width);
		}
	}

	// the other rectangle: from old to new height/width
	// (for growing on both directions)
	if (new_width > old_width) {
		for (i = old_height; i < new_height; i++) {
			memset (img->mosaic[i] + old_width, ' ', new_width - old_width);
			memset (img->attr[i] + old_width, Normal, new_width - old_width);
		}
	}

	return 0;
}


int CopyMOSAIC (MOSAIC *dest, MOSAIC *src) {
	// no NULL pointers
	if (dest && src) {
		int i, minWidth = min (dest->width, src->width);
		for (i = 0; i < min (dest->height, src->height); i++) {
			memcpy (dest->mosaic[i], src->mosaic[i], minWidth);
			memcpy (dest->attr[i], src->attr[i], minWidth);
			/*for (j = 0; j < min (dest->width, src->width); j++) {*/
				/*dest->mosaic[i][j] = src->mosaic[i][j];*/
				/*dest->attr[i][j] = src->attr[i][j];*/
			/*}*/
		}

		return 0;
	}
	else {
		return ERR;
	}
}


int TrimMOSAIC (MOSAIC *target, char resize) {
	// Rectangle containing the mosaic without blank lines/columns
	int ULy, ULx, BRy, BRx;
	BRy = BRx = 0;
	ULy = target->height - 1;
	ULx = target->width - 1;
	int i, j;
	// check the mosaic
	for (i = 0; i < target->height; i++) {
		for (j = 0; j < target->width; j++) {
			// it's not a blank, so update our rectangle
			if (target->mosaic[i][j] != ' ') {
				ULy = min (ULy, i);
				ULx = min (ULx, j);
				BRy = max (BRy, i);
				BRx = max (BRx, j);
			}
		}
	}

	// asked for a resize, but the entire mosaic is a blank
	// you kidding with me, right?
	if (resize && ULy > BRy) {
		return ERR;
	}

	// move the data from mosaic[src_y][src_x] to mosaic[i][j],
	// but skip if it's already at (0,0)
	if (ULy || ULx) {
		int src_x, src_y;
		for (src_y = ULy, i = 0; src_y <= BRy; src_y++, i++) {
			for (src_x = ULx, j = 0; src_x <= BRx; src_x++, j++) {
				target->mosaic[i][j] = target->mosaic[src_y][src_x];
				target->mosaic[src_y][src_x] = ' ';
				target->attr[i][j] = target->attr[src_y][src_x];
				target->attr[src_y][src_x] = Normal;
			}
		}
	}

	// we already moved the mosaic to (0,0), so if 
	// asked to resize, just do it and we won't lose any data
	if (resize) {
		return ResizeMOSAIC (target, BRy - ULy + 1, BRx - ULx + 1);
	}

	return 0;
}


void FreeMOSAIC (MOSAIC *img) {
	// only free the mosaic/attr if img is an 
	// independent MOSAIC (not a subMOSAIC)
	if (!img->isSub) {
		int i;
		for (i = 0; i < img->height; i++) {
			free (img->attr[i]);
			free (img->mosaic[i]);
		}
	}

	free (img->attr);
	free (img->mosaic);

	free (img);
}
