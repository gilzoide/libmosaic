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

/** @file moscat.c
 * A 'cat' application for Mosaic images
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mosaic.h"

/* ARGP for parsing the arguments */
#include <argp.h>

const char *argp_program_version = "Moscat 0.2.0";
const char *argp_program_bug_address = "<gilzoide@gmail.com>";
static char doc[] = "A cat program for mosaic image files *.mosi";
static char args_doc[] = "FILE";

// our options
static struct argp_option options[] = {
	{"dimensions",  'd', 0, 0, "Show image dimensions"},
	{"color", 'c', 0, 0,  "Produce colored output" },
	{"stream", 's', 0, 0, "Output mosaic in a stream fashion, perfect for \
piping into other program"},
	{ 0 }
};

/* Used by main to communicate with parse_opt */
struct arguments {
	char *input;
	char dimensions, color, stream;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *argumentos = (struct arguments *) state->input;

	switch (key) {
		case 'c':
			argumentos->color = 1;
			break;
		case 'd':
			argumentos->dimensions = 1;
			break;
		case 's':
			argumentos->stream = 1;
			break;

		case ARGP_KEY_ARG:
			if(state->arg_num >= 1) {
				/* Too many arguments. */
				argp_usage(state);
			}

			argumentos->input = arg;
			break;

		case ARGP_KEY_END:
			if(state->arg_num < 1) {
				/* Not enough arguments. */
				argp_usage(state);
			}
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}


/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };



/* MOSCAT */

void reset_terminal_attr() {
	printf("\e[0m");
}
void set_terminal_attr(mos_attr a) {
	// terminal color codes
	const char const *fg_color_table[] = {"30", "31", "32", "33", "34", "35", "36", "37"};
	const char const *bg_color_table[] = {"40", "41", "42", "43", "44", "45", "46", "47"};

	printf("\e[0m\e[%s;%s%s%sm"
			, fg_color_table[mos_get_fg(a)]
			, bg_color_table[mos_get_bg(a)]
			, mos_get_bold(a) ? ";1" : ""
			, mos_get_underline(a) ? ";4" : "");
}

/**
 * Prints the image at stdout, using `putchar`s
 *
 * @param[in] img The image to be displayed
 * @param[in] color Flag: display colors?
 */
void printMOSAIC(MOSAIC *img, char color) {
	int i, j;
	for(i = 0; i < img->height; i++) {
		for(j = 0; j < img->width; j++) {
			if(color) {
				set_terminal_attr(mos_get_attr(img, i, j));
			}
			putchar(mos_get_char(img, i, j));
		}
		reset_terminal_attr();
		putchar('\n');
	}
}

int main(int argc, char *argv[]) {
	struct arguments arguments;
	arguments.color = 0;
	arguments.dimensions = 0;
	arguments.stream = 0;
	// parse arguments
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	// image to be loaded
	MOSAIC *img = mos_new(0, 0);
	if(!img) {
		exit(1);
	}

	int load_result = mos_load(img, arguments.input);

	// if unknown format, can't write it in colors
	if(load_result == MOS_EUNKNSTRGFMT || load_result == MOS_EMALLOC) {
		if(arguments.color) {
			fprintf(stderr, "Couldn't figure out attribute format."
					" Disabling colors!\n");

			arguments.color = 0;
		}
		load_result = 0;
	}

	if(!load_result) {
		// asked to print it stream style
		if(arguments.stream) {
			fputMOSAIC(img, MOS_UNCOMPRESSED, stdout);
		}
		// or print it nicely
		else {
			if(arguments.dimensions) {
				printf("%dx%d\n", img->height, img->width);
			}	

			// print the image at stdout
			printMOSAIC(img, arguments.color);
		}
	}
	else if(load_result == MOS_ENODIMENSIONS) {
		fprintf(stderr, "There are no dimensions in this file..."
				"It's probably not a mosaic image!\n");
	}
	else {
		fprintf(stderr, "Couldn't load file: %s.\n", strerror(errno));
	}

	mos_free(img);

	return 0;
}
