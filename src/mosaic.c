#include "mosaic.h"

inline int max (int a, int b) {
	return (a > b ? a : b);
}


inline int min (int a, int b) {
	return (a < b ? a : b);
}


char outOfBoundaries (MOSAIC *img, int y, int x) {
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
	img->mosaic = img->attr = NULL;
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


MOSAIC * SubMOSAIC (MOSAIC *parent, int begin_y, int begin_x, int height, int width) {
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


int mosAddch (MOSAIC *img, int y, int x, mos_char c) {
	if (outOfBoundaries (img, y, x)) {
		return 0;
	}
	else {
		img->mosaic[y][x] = c;
		return 1;
	}
}


int mosSetAttr (MOSAIC *img, int y, int x, mos_attr a) {
	if (!outOfBoundaries (img, y, x)) {
		img->attr[y][x] = a;
		return 1;
	}
	else {
		return 0;
	}
}


mos_char mosGetch (MOSAIC *img, int y, int x) {
	if (!outOfBoundaries (img, y, x)) {
		return img->mosaic[y][x];
	}
	else {
		return 0;
	}
}


mos_attr mosGetAttr (MOSAIC *img, int y, int x) {
	if (!outOfBoundaries (img, y, x)) {
		return img->attr[y][x];
	}
	else {
		return Normal;
	}

}


int ResizeMOSAIC (MOSAIC *img, int new_height, int new_width) {
	// old dimensions
	int old_height = img->height;
	int old_width = img->width;
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
	if ((img->mosaic = (mos_char**) realloc (
			img->mosaic, new_height * sizeof (mos_char*))) == NULL) {
		return -1;
	}
	// when growing, initialize lines with NULL, so realloc mallocs them
	for (i = old_height; i < new_height; i++) {
		img->mosaic[i] = NULL;
	}
	// attributes:
	if ((img->attr = (mos_attr**) realloc (
			img->attr, new_height * sizeof (mos_attr*))) == NULL) {
		return -1;
	}
	// when growing, initialize lines with NULL, so realloc mallocs them
	for (i = old_height; i < new_height; i++) {
		img->attr[i] = NULL;
	}

	// Columns
	for (i = 0; i < new_height; i++) {
		if ((img->mosaic[i] = (mos_char*) realloc (
				img->mosaic[i], new_width * sizeof (mos_char))) == NULL) {
			return -1;
		}
		if ((img->attr[i] = (mos_attr*) realloc (
				img->attr[i], new_width * sizeof (mos_attr))) == NULL) {
			return -1;
		}
	}
	
	// maybe it grew, so complete with blanks
	int j;
	if (old_height > 0 && old_width > 0) {
		// new lines, until old width
		for (i = old_height; i < new_height; i++) {
			for (j = 0; j < old_width; j++) {
				img->mosaic[i][j] = ' ';
				img->attr[i][j] = Normal;
			}
		}
		// new columns, until old height
		for (i = old_width; i < new_width; i++) {
			for (j = 0; j < old_height; j++) {
				img->mosaic[j][i] = ' ';
				img->attr[j][i] = Normal;
			}
		}
	}

	// the other rectangle: from old to new height/width
	// (for growing on both directions)
	for (i = old_height; i < new_height; i++) {
		for (j = old_width; j < new_width; j++) {
			img->mosaic[i][j] = ' ';
			img->attr[i][j] = Normal;
		}
	}

	return 0;
}


int CopyMOSAIC (MOSAIC *dest, MOSAIC *src) {
	// no NULL pointers
	if (dest && src) {
		int i, j;
		for (i = 0; i < min (dest->height, src->height); i++) {
			for (j = 0; j < min (dest->width, src->width); j++) {
				dest->mosaic[i][j] = src->mosaic[i][j];
				dest->attr[i][j] = src->attr[i][j];
			}
		}

		return 0;
	}
	else {
		return -1;
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
			if (mosGetch (target, i, j) != ' ') {
				ULy = min (ULy, i);
				ULx = min (ULx, j);
				BRy = max (BRy, i);
				BRx = max (BRx, j);
			}
		}
	}
	// move the data from mosaic[src_y][src_x] to mosaic[i][j]
	int src_x, src_y;
	for (src_y = ULy, i = 0; src_y <= BRy; src_y++, i++) {
		for (src_x = ULx, j = 0; src_x <= BRx; src_x++, j++) {
			target->mosaic[i][j] = target->mosaic[src_y][src_x];
			target->mosaic[src_y][src_x] = ' ';
		}
	}

	// we already moved the mosaic to (0,0), so if 
	// asked to resize, just do it and we won't lose any data
	if (resize) {
		return ResizeMOSAIC (target, BRy - ULy + 1, BRx - ULx + 1);
	}

	return 0;
}


int SaveMOSAIC (MOSAIC *image, const char *file_name) {
	FILE *f;
	if ((f = fopen (file_name, "w")) == NULL) {
		return errno;
	}

	fprintf (f, "%dx%d\n", image->height, image->width);
	
	// Mosaic
	int i;
	for (i = 0; i < image->height; i++) {
		fprintf (f, "%.*s\n", image->width, image->mosaic[i]);
	}
	fputc (SEPARATOR, f);
	// Attr
	for (i = 0; i < image->height; i++) {
		fwrite (image->attr[i], sizeof (mos_attr), image->width, f);
	}
	
	fclose (f);

	return 0;
}


int LoadMOSAIC (MOSAIC *image, const char *file_name) {
	FILE *f;
	if ((f = fopen (file_name, "r")) == NULL) {
		return errno;
	}
	
	int new_height, new_width;
	if (!fscanf (f, "%3dx%3d", &new_height, &new_width)) {
		fclose (f);
		return 1;
	}
	
	ResizeMOSAIC (image, new_height, new_width);

	int c;
	// there's supposed to have a '\n' to discard after %dx%d;
	// but if there ain't one, we read what's after
	if ((c = fgetc (f)) != '\n')
		ungetc (c, f);
	
	int i, j;
	for (i = 0; i < image->height; i++) {
		// read the line until the end or no more width is available
		for (j = 0; j < image->width; j++) {
			c = fgetc (f);
			if (c == EOF || c == SEPARATOR) {
				// used so won't need a flag or more comparisons
				// to break both the fors
				goto FILL_WITH_BLANK;
			}
			// if it reached newline before width...
			else if (c == '\n') {
				break;
			}
			image->mosaic[i][j] = c;
		}
		// ...complete with whitespaces
		for ( ;  j < image->width; j++) {
			image->mosaic[i][j] = ' ';
		}
		
		// we read the whole line, but the tailing '\n', we need to discard it
		if (c != '\n') {
			// may happen it's not a newline yet, so let's reread it =P
			if ((c = fgetc (f)) != '\n') {
				ungetc (c, f);
			}
		}
	}
	
FILL_WITH_BLANK:
	// well, maybe we reached EOF or SEPARATOR, so everything else is a blank...
	for ( ; i < image->height; i++) {
		for (j = 0;  j < image->width; j++) {
			image->mosaic[i][j] = ' ';
		}
	}

	if (c != EOF) {
		// jump to the SEPARATOR, if it exists
		while (c != SEPARATOR && c != EOF) {
			c = fgetc (f);
		}
		// Time for some Attributes! (color/bold)
		size_t check = image->width;
		for (i = 0; check == image->width && i < image->width; i++) {
			check = fread (image->attr[i], sizeof (mos_attr), image->width, f);
		}
	}

	fclose (f);

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
