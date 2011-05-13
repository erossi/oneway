/* This file is part of HospiTV
 * Copyright (C) 2011 Enrico Rossi
 *
 * HospiTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HospiTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file debug.h
  \brief Functions to activare and display (rs232) debug.
  */

#ifndef DBG_H
#define DBG_H

#include <avr/pgmspace.h>
#include "uart.h"
#include "htv.h"

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

/*! Maximum number of char a line can be */
#define MAX_LINE_LENGHT 80

/*! Number of char a substring can be made of.
Substrings are used to compose the line when, for example,
conversions have to be made from int to string etc. */
#define MAX_STRING_LENGHT 80

#define PRINT_VALUE_X_LINE 16
#define SEC_FOR_Y 5

/*! \struct debug_t
  The main debug structure, it has to be allocated,
  eventually, if debug is not active, you can avoid the
  allocation of the two members char *.
  */
struct debug_t {
	/*! A string of MAX_LINE_LENGHT chars to be printed. */
	char *line;
	/*! A string buffer used to help compose the line. */
	char *string;
	/*! debug status [0, 1] */
	uint8_t active;
};

void debug_print_P(PGM_P string, struct debug_t *debug);
void debug_print(struct debug_t *debug);
uint8_t debug_wait_for_y(struct debug_t *debug);
struct debug_t *debug_init(void);
void debug_free(struct debug_t *debug);
void debug_print_htv(struct htv_t *htv, struct debug_t *debug);

#endif
