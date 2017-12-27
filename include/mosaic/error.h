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

/** @file error.h
 * Libmosaic error codes.
 */

#ifndef __MOSAIC_ERROR_H__
#define __MOSAIC_ERROR_H__

/**
 * Numeric error codes returned by Mosaic functions.
 */
typedef enum {
	/// No error at all.
	MOS_OK            = 0,
	/// `malloc` error.
	MOS_EMALLOC       = -1,
	/// Dimension header was not found when reading from file.
	MOS_ENODIMENSIONS = -2,
	/// Unknown attribute storage format when reading from file.
	MOS_EUNKNSTRGFMT  = -3,
	/// Compression error.
	MOS_ECOMPRESSION  = -4,
	/// Unsupported operation.
	MOS_EUNSUPPORTED  = -5,
} mos_error;

#endif

