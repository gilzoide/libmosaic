#include "stream_io.h"

#include <string.h>
#include <zlib.h>

/**
 * Compress the MOSAIC for writing it in stream, when using zlib compression
 *
 * It's just an auxiliary function for fputMOSAIC, it's not even in the header
 * @note It expects that you have just read the SEPARATOR and the COMPRESSED
 * marks from the `stream'.
 *
 * @param[in] image The image to be saved
 * @param[out] stream The stream to be written to
 *
 * @return 0 on success
 * @return ERR for compression errors
 */
int compressMOSAIC (MOSAIC *image, attr_storage_fmt fmt, FILE *stream) {
	// accumulator: storage how much space is needed
	size_t compressed_data_size = 0;
	// and the zlib's stream
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (deflateInit (&strm, Z_DEFAULT_COMPRESSION) != Z_OK) {
		return ERR;
	}

#define CHUNK (image->width)
	// write each attr line
	char loop = 1;
	int i, flush = Z_NO_FLUSH;
	mos_attr out[MOSAICSize (image)];
	mos_attr *aux = out;
	for (i = 0; loop; i++) {
		if (i == image->height - 1) {
			loop = 0;
			flush = Z_FINISH;
		}
		strm.avail_in = CHUNK;
		strm.next_in = image->attr[i];


		do {
			strm.avail_out = CHUNK;
			strm.next_out = aux;

			if (deflate (&strm, flush) == Z_STREAM_ERROR) {
				deflateEnd (&strm);
				return ERR;
			}
			int have = CHUNK - strm.avail_out;
			compressed_data_size += have;
			aux += have;
		} while (strm.avail_out == 0);
	}
#undef CHUNK
	deflateEnd (&strm);
	// now write the data into the stream
	// first off, the data_size
	fwrite (&compressed_data_size, sizeof (size_t), 1, stream);
	// and now the compreessed data itself
	fwrite (out, sizeof (char), compressed_data_size, stream);

	return 0;
}


/**
 * Decompress the MOSAIC read from stream, when using zlib compression
 *
 * It's just an auxiliary function for fgetMOSAIC, it's not even in the header
 * @note It expects that you have just read the SEPARATOR and the COMPRESSED
 * marks from the `stream'.
 *
 * @param[in] image The image to be saved
 * @param[out] stream The stream to be written to
 *
 * @return 0 on success
 * @return ERR for decompression errors
 */
int uncompressMOSAIC (MOSAIC *image, FILE *stream) {
	// the auxiliary array, for writing the compressed data 
	mos_attr in[MOSAICSize (image) + 2];
	mos_attr out[MOSAICSize (image)];
	// and the zlib's stream
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (inflateInit (&strm) != Z_OK) {
		return ERR;
	}

	size_t compressed_data_size;
	fread (&compressed_data_size, sizeof (size_t), 1, stream);
	// read it all to `in', and inflate to `out'
	strm.avail_in = fread (in, sizeof (char), compressed_data_size, stream);
	strm.next_in = in;

	strm.avail_out = MOSAICSize (image);
	strm.next_out = out;

	if (inflate (&strm, Z_NO_FLUSH) == Z_STREAM_ERROR) {
		inflateEnd (&strm);
		return ERR;
	}
	inflateEnd (&strm);

	// and copy it back into the mosaic
	mos_attr *aux = out;
	int i;
	for (i = 0; i < image->height; i++) {
		memcpy (image->attr[i], aux, image->width * sizeof (mos_attr));
		aux += image->width;
	}

	return 0;
}


int fgetMOSAIC (MOSAIC *image, FILE *stream) {
	int new_height, new_width;
	int dim_return = fscanf (stream, "%3dx%3d", &new_height, &new_width);
	if (!dim_return || dim_return == EOF) {
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
			if (c == SEPARATOR || c == EOF) {
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
	while (c != SEPARATOR && c != EOF) {
		c = fgetc (stream);
	}
	// if SEPARATOR, and not EOF, read the next char, which hopely 
	// will be the format
	if (c != EOF) {
		c = fgetc (stream);
	}

	// Time for some Attributes! (color/bold)
	switch (c) {
		case UNCOMPRESSED:
			; size_t check = image->width;
			for (i = 0; check == image->width && i < image->width; i++) {
				check = fread (image->attr[i], sizeof (mos_attr), image->width, stream);
			}
			break;

		// uncompress with zlib
		case COMPRESSED:
			return uncompressMOSAIC (image, stream);

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
	
	// Mosaic //
	int i;
	for (i = 0; i < image->height; i++) {
		fprintf (stream, "%.*s\n", image->width, image->mosaic[i]);
	}

	// time for binary stuff
	fputc (SEPARATOR, stream);

	// put the format
	fputc (fmt, stream);

	// Attr //
	switch (fmt) {
		case UNCOMPRESSED:
			for (i = 0; i < image->height; i++) {
				fwrite (image->attr[i], sizeof (mos_attr), image->width, stream);
			}
			break;

		// compress with zlib
		case COMPRESSED:
			return compressMOSAIC (image, fmt, stream);

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

	fputFmtMOSAIC (image, fmt, f);
	
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
