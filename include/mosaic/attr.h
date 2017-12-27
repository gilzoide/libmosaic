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

/** @file attr.h
 * Mosaic char attributes.
 */

#ifndef __MOSAIC_ATTR_H__
#define __MOSAIC_ATTR_H__

#include <stdint.h>

/**
 * Attributes of Mosaic chars.
 *
 * Components (in least to most significative bits):
 * + Foreground color (3 bits)
 * + Background color (3 bits)
 * + Bold modifier (1 bit)
 * + Underline modifier (1 bit)
 */
typedef uint8_t mos_attr;

/**
 * Supported colors.
 */
typedef enum {
	MOS_BLACK     = 0b000,
	MOS_RED       = 0b001,
	MOS_GREEN     = 0b010,
	MOS_YELLOW    = 0b011,
	MOS_BLUE      = 0b100,
	MOS_MAGENTA   = 0b101,
	MOS_CYAN      = 0b110,
	MOS_WHITE     = 0b111,
} mos_color;

/**
 * Attribute mask in `mos_attr`.
 */
typedef enum {
	MOS_FG        = 0b00000111,
	MOS_BG        = 0b00111000,
	MOS_BOLD      = 0b01000000,
	MOS_UNDERLINE = 0b10000000,
} mos_attr_mask;

/**
 * Offset of attribute masks, to be used internally.
 */
typedef enum {
	MOS_FG_OFFSET = 0,
	MOS_BG_OFFSET = 3,
	MOS_BOLD_OFFSET = 6,
	MOS_UNDERLINE_OFFSET = 7,
} mos_attr_mask_offset;

/**
 * Make a `mos_attr` from it's parts: foreground, background, bold, underline.
 */
mos_attr mos_mkattr(mos_color fg, mos_color bg, int bold, int underline);

/**
 * Get the foreground color of a `mos_attr`.
 */
mos_attr mos_get_fg(mos_attr a);
/**
 * Get the background color of a `mos_attr`.
 */
mos_attr mos_get_bg(mos_attr a);
/**
 * Get bold flag of a `mos_attr`.
 */
mos_attr mos_get_bold(mos_attr a);
/**
 * Get underline flag of a `mos_attr`.
 */
mos_attr mos_get_underline(mos_attr a);

#endif

