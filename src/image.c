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

#include "mosaic/attr.h"
#include "mosaic/error.h"
#include "mosaic/image.h"

#include <stdlib.h>
#include <string.h>

static inline int max(int a, int b) {
	return (a > b ? a : b);
}

static inline int min(int a, int b) {
	return (a < b ? a : b);
}

int mos_out_of_bounds(const MOSAIC *img, int y, int x) {
	return (y < 0 || y >= img->height || x < 0 || x >= img->width);
}

MOSAIC *mos_new(int height, int width) {
	MOSAIC *img;
	if((img = (MOSAIC *) calloc(1, sizeof(MOSAIC))) != NULL) {
		// alloc the dinamic stuff and fill it: something ResizeMOSAIC already does
		if(mos_resize(img, height, width) != MOS_OK) {
			free(img);
			img = NULL;
		}
	}

	return img;
}


int mos_size(const MOSAIC *img) {
	return img->height * img->width;
}


MOSAIC *mos_submosaic(MOSAIC *parent, int height, int width, int begin_y, int begin_x) {
	MOSAIC *img = malloc(sizeof(MOSAIC));
	if(img == NULL) {
		return NULL;
	}

	img->height = height;
	img->width = width;
	img->is_sub = 1;	// hello, i'm a subMOSAIC

	// allocate just the lines
	// mosaic
	img->mosaic = malloc(height * sizeof(mos_char *));
	if(img->mosaic == NULL) {
		free(img);
		return NULL;
	}
	// attr
	img->attr = malloc(height * sizeof(mos_attr *));
	if(img->attr == NULL) {
		free(img->mosaic);
		free(img);
		return NULL;
	}

	// now make mosaic/attr point to the parent MOSAIC at the right closure
	int i;
	for(i = 0; i < height; i++) {
		// using pointer arithmetics, cuz it's cool
		img->mosaic[i] = parent->mosaic[begin_y + i] + begin_x;
		img->attr[i] = parent->attr[begin_y + i] + begin_x;
	}

	return img;
}


mos_char mos_set_char(MOSAIC *img, int y, int x, mos_char c) {
	return (img->mosaic[y][x] = c);
}


mos_attr mos_set_attr(MOSAIC *img, int y, int x, mos_attr a) {
	return (img->attr[y][x] = a);
}


mos_char mos_get_char(const MOSAIC *img, int y, int x) {
	return img->mosaic[y][x];
}


mos_attr mos_get_attr(const MOSAIC *img, int y, int x) {
	return img->attr[y][x];
}


int mos_resize(MOSAIC *img, int new_height, int new_width) {
	// old dimensions
	const int old_height = img->height;
	const int old_width = img->width;
	// new dimensions
	img->height = new_height;
	img->width = new_width;
	
	int i;
	// when shrinking the MOSAIC, free the lines we're discarding
	for(i = old_height - 1; i >= new_height; i--) {
		free(img->mosaic[i]);
		free(img->attr[i]);
	}
	
	
	// realloc the dinamic stuff
	// Lines
	if((img->mosaic = realloc(img->mosaic, new_height * sizeof(mos_char *))) == NULL
			|| (img->attr = realloc(img->attr, new_height * sizeof(mos_attr *))) == NULL) {
		return MOS_EMALLOC;
	}
	// when growing, initialize lines with NULL, so realloc mallocs them
	for(i = old_height; i < new_height; i++) {
		img->mosaic[i] = NULL;
		img->attr[i] = NULL;
	}

	// Columns
	for(i = 0; i < new_height; i++) {
		if((img->mosaic[i] = realloc(img->mosaic[i], new_width * sizeof(mos_char))) == NULL
				|| (img->attr[i] = realloc(img->attr[i], new_width * sizeof(mos_attr))) == NULL) {
			return MOS_EMALLOC;
		}
	}
	
	// maybe it grew, so complete with blanks
	// new lines, until old width
	for(i = old_height; i < new_height; i++) {
		memset(img->mosaic[i], MOS_DEFAULT_CHAR, old_width);
		memset(img->attr[i], MOS_DEFAULT_ATTR, old_width);
	}
	// new columns, until old height
	if(new_width > old_width) {
		for(i = 0; i < old_height; i++) {
			memset(img->mosaic[i] + old_width, MOS_DEFAULT_CHAR, new_width - old_width);
			memset(img->attr[i] + old_width, MOS_DEFAULT_ATTR, new_width - old_width);
		}
	}

	// the other rectangle: from old to new height/width
	// (forgrowing on both directions)
	if(new_width > old_width) {
		for(i = old_height; i < new_height; i++) {
			memset(img->mosaic[i] + old_width, MOS_DEFAULT_CHAR, new_width - old_width);
			memset(img->attr[i] + old_width, MOS_DEFAULT_ATTR, new_width - old_width);
		}
	}

	return MOS_OK;
}


void mos_copy(MOSAIC *dest, MOSAIC *src) {
	int i, minWidth = min(dest->width, src->width), minHeight = min(dest->height, src->height);
	for(i = 0; i < minHeight; i++) {
		memcpy(dest->mosaic[i], src->mosaic[i], minWidth);
		memcpy(dest->attr[i], src->attr[i], minWidth);
	}
}


int mos_trim(MOSAIC *target, char resize) {
	// Rectangle containing the mosaic without blank lines/columns
	int ULy, ULx, BRy, BRx;
	BRy = BRx = 0;
	ULy = target->height - 1;
	ULx = target->width - 1;
	int i, j;
	// check the mosaic
	for(i = 0; i < target->height; i++) {
		for(j = 0; j < target->width; j++) {
			// it's not a blank, so update our rectangle
			if(target->mosaic[i][j] != MOS_DEFAULT_CHAR) {
				ULy = min(ULy, i);
				ULx = min(ULx, j);
				BRy = max(BRy, i);
				BRx = max(BRx, j);
			}
		}
	}

	// only trim/resize if the entire mosaic is not blank
	if(ULy <= BRy) {
		// move the data from mosaic[src_y][src_x] to mosaic[i][j],
		// but skip if it's already at (0,0)
		if(ULy || ULx) {
			int src_x, src_y;
			for(src_y = ULy, i = 0; src_y <= BRy; src_y++, i++) {
				for(src_x = ULx, j = 0; src_x <= BRx; src_x++, j++) {
					target->mosaic[i][j] = target->mosaic[src_y][src_x];
					target->mosaic[src_y][src_x] = MOS_DEFAULT_CHAR;
					target->attr[i][j] = target->attr[src_y][src_x];
					target->attr[src_y][src_x] = MOS_DEFAULT_ATTR;
				}
			}
		}

		// we already moved the mosaic to (0,0), so if
		// asked to resize, just do it and we won't lose any data
		if(resize) {
			return mos_resize(target, BRy - ULy + 1, BRx - ULx + 1);
		}
	}

	return MOS_OK;
}


void mos_free(MOSAIC *img) {
	// only free the mosaic/attr if img is an 
	// independent MOSAIC (not a subMOSAIC)
	if(!img->is_sub) {
		int i;
		for(i = 0; i < img->height; i++) {
			free(img->attr[i]);
			free(img->mosaic[i]);
		}
	}

	free(img->attr);
	free(img->mosaic);

	free(img);
}
