#include "stream_io.h"

#include <string.h>

/**
 * Checks whether fmt is one of the define attr_storage_fmts.
 */
int isAttrSeparator (attr_storage_fmt fmt) {
	return fmt == UNCOMPRESSED || fmt == COMPRESSED || fmt == NO_ATTR;
}


int fgetMOSAIC (MOSAIC *image, FILE *stream) {
	int new_height, new_width;
	if (!fscanf (stream, "%3dx%3d", &new_height, &new_width)) {
		return ENODIMENSIONS;
	}
	
	// try to resize, get out if trouble
	if (ResizeMOSAIC (image, new_height, new_width)) {
		return ERR;
	}

	int c;
	// there's supposed to have a '\n' to discard after %dx%d;
	// but if there ain't one, we read what's after
	if ((c = fgetc (stream)) != '\n') {
		ungetc (c, stream);
	}
	
	int i, j;
	for (i = 0; i < image->height; i++) {
		// read the line until the end or no more width is available
		for (j = 0; j < image->width; j++) {
			c = fgetc (stream);
			if (isAttrSeparator (c) || c == EOF) {
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
			if ((c = fgetc (stream)) != '\n') {
				ungetc (c, stream);
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

	// jump to the SEPARATOR, if it exists
	while (!isAttrSeparator (c) && c != EOF) {
		c = fgetc (stream);
	}

	switch (c) {
		case UNCOMPRESSED:
			// Time for some Attributes! (color/bold)
			; size_t check = image->width;
			for (i = 0; check == image->width && i < image->width; i++) {
				check = fread (image->attr[i], sizeof (mos_attr), image->width, stream);
			}
			break;

		case COMPRESSED:
			break;

		default:
			for (i = 0; i < image->height; i++) {
				memset (image->attr[i], Normal, image->width * sizeof (mos_attr));
			}
			// if separator ain't NO_ATTR, warn that it's an unknown storage
			// format, even though we loaded all the attr with Normal
			if (c != NO_ATTR) {
				return EUNKNSTRGFMT;
			}
			break;
	}

	return 0;
}


int fputFmtMOSAIC (MOSAIC *image, attr_storage_fmt fmt, FILE *stream) {
	fprintf (stream, "%dx%d\n", image->height, image->width);
	
	// Mosaic
	int i;
	for (i = 0; i < image->height; i++) {
		fprintf (stream, "%.*s\n", image->width, image->mosaic[i]);
	}

	// put the separator
	fputc (fmt, stream);

	switch (fmt) {
		case UNCOMPRESSED:
			// Attr
			for (i = 0; i < image->height; i++) {
				fwrite (image->attr[i], sizeof (mos_attr), image->width, stream);
			}
			break;

		case COMPRESSED:
			break;

		// no attributes, don't do anything =P
		case NO_ATTR:
			break;

		// none of the known sorage formats
		default:
			return EUNKNSTRGFMT;
	}

	return 0;
}


int SaveFmtMOSAIC (MOSAIC *image, attr_storage_fmt fmt, const char *file_name) {
	FILE *f;
	if ((f = fopen (file_name, "w")) == NULL) {
		return errno;
	}

	fputMOSAIC (image, f);
	
	fclose (f);

	return 0;
}


int LoadMOSAIC (MOSAIC *image, const char *file_name) {
	FILE *f;
	if ((f = fopen (file_name, "r")) == NULL) {
		return errno;
	}
	
	int aux = fgetMOSAIC (image, f);

	fclose (f);

	return aux;
}
