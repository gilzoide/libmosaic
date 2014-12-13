/** @file moscat.c
 * A 'cat' application for nmos images
 */

#include <stdio.h>
#include <string.h>
#include "mosaic.h"
#include "curs_mos.h"

/* ARGP for parsing the arguments */
#include <argp.h>

const char *argp_program_version = "Moscat 0.1.0";
const char *argp_program_bug_address = "<gilzoide@gmail.com>";
static char doc[] = "A cat program for mosaic image files *.mosi";
static char args_doc[] = "FILE";

// our options
static struct argp_option options[] = {
	{"dimensions",  'd', 0, 0, "Show image dimensions"},
	{"color", 'c', 0, 0,  "Produce colored output" },
	{ 0 }
};

/* Used by main to communicate with parse_opt */
struct arguments {
	char *input;
	char dimensions, color;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	struct arguments *argumentos = (struct arguments*) state->input;

	switch (key)
	{
		case 'c':
			argumentos->color = 1;
			break;
		case 'd':
			argumentos->dimensions = 1;
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 1)
				/* Too many arguments. */
				argp_usage (state);

			argumentos->input = arg;
			break;

		case ARGP_KEY_END:
			if (state->arg_num < 1)
				/* Not enough arguments. */
				argp_usage (state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}


/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };



/* MOSCAT */

/**
 * Prints the image at stdout, using `putchar`s
 *
 * @param[in] img The image to be displayed
 * @param[in] color Flag: display colors?
 */
void printMOSAIC (MOSAIC *img, char color) {
	int i, j;
	for (i = 0; i < img->height; i++) {
		for (j = 0; j < img->width; j++) {
			if (color) {
				Tcolor (_mosGetAttr (img, i, j));
			}
			putchar (_mosGetCh (img, i, j));
		}

		// Reset to default terminal color
		Tcolor (Normal);
		putchar ('\n');
	}
}

int main (int argc, char *argv[]) {
	struct arguments arguments;
	arguments.color = 0;
	arguments.dimensions = 0;
	// parse arguments
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	// image to be loaded
	MOSAIC *img = NewMOSAIC (0, 0);
	if (!img) {
		exit (1);
	}

	int load_result = LoadMOSAIC (img, arguments.input);

	if (!load_result) {
		if (arguments.dimensions) {
			printf ("%dx%d\n", img->height, img->width);
		}

		// print the image at stdout
		printMOSAIC (img, arguments.color);
	}
	else if (load_result == ENODIMENSIONS) {
		fprintf (stderr, "There are no dimensions in this file... \
It's probably not a mosaic image!\n");
	}
	else {
		fprintf (stderr, "Couldn't load file. %s.\n", strerror (errno));
	}

	FreeMOSAIC (img);

	return 0;
}
