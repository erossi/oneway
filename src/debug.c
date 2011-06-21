/* This file is part of OneWay
 * Copyright (C) 2011 Enrico Rossi
 *
 * OneWay is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OneWay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file debug.c
  \brief printing to terminal functions.
  */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"

/*! Print a string taken directly from the EEPROM
  avoiding memory allocation.
  \param string the PSTR() string to be printed.
  \param debug the struct debug.
 */
void debug_print_P(PGM_P string, struct debug_t *debug)
{
	if (debug->active) {
		strcpy_P(debug->line, string);
		uart_printstr(0, debug->line);
	}
}

/*! print the debug->line if debug is active.*/
void debug_print(struct debug_t *debug)
{
	if (debug->active)
		uart_printstr(0, debug->line);
}

/*! \brief boot message */
static void hello(struct debug_t *debug)
{
        debug_print_P(PSTR("\nOneWay Rel: "), debug);
        debug_print_P(PSTR(GITREL), debug);
        debug_print_P(PSTR("\n\n"), debug);
        debug_print_P(PSTR("Andrea Marabini <info@marabo.it>\n"), debug);
        debug_print_P(PSTR("Enrico Rossi <e.rossi@tecnobrain.com>\n"), debug);
        debug_print_P(PSTR("URL: http://tecnobrain.com/\n"), debug);
        debug_print_P(PSTR("GNU GPL v3 - use at your own risk!\n\n"), debug);
}

/*! \brief press 'y' or 'n' */
uint8_t debug_wait_for_y(struct debug_t *debug)
{
	uint8_t i;
	char c;

	if (debug->active) {
		for (i = 0; i < SEC_FOR_Y; i++) {
			c = uart_getchar(0, 0);

			/*! "Y" is 89 and "y" is 121 */
			if ((c == 89) || (c == 121)) {
				debug_print_P(PSTR("\n"), debug);
				return(1); /*! Exit the cicle in a bad way */
			} else {
				_delay_ms(1000);
				debug_print_P(PSTR("."), debug);
			}
		}
	}

	return(0);
}

/*! \brief initialize debug struct and uart console */
struct debug_t *debug_init(void)
{
	struct debug_t *debug;

	uart_init(0);
	uart_tx(0, 1);
	uart_rx(0, 1);
	debug = malloc(sizeof(struct debug_t));
	debug->line = malloc(MAX_LINE_LENGHT);
	debug->string = malloc(MAX_STRING_LENGHT);
	debug->active = 1;
	hello(debug);
	/*
	debug_print_P(PSTR("\nActivate debug? (y/N): "), debug);

	if (!debug_wait_for_y(debug)) {
		uart_shutdown(0);
		debug->active = 0;
		free(debug->line);
		free(debug->string);
	}
	*/

	return(debug);
}

/*! \brief free the memory */
void debug_free(struct debug_t *debug)
{
	if (debug->active) {
		free(debug->string);
		free(debug->line);
	}

	free(debug);
}

/*! \brief print the struct htv contents.
 *
 * \note if the strings are not correctly initialized and
 * terminated, this function will memory leak and crash.
 */
void debug_print_htv(struct htv_t *htv, struct debug_t *debug)
{
	strcpy_P(debug->line, PSTR("\nAddr: "));
	debug->string = utoa(htv->address, debug->string, 16);
	strcat(debug->line, debug->string);
	strcat_P(debug->line, PSTR("\n"));
	debug_print(debug);

	strcpy_P(debug->line, PSTR("Pin: "));
	debug->string = utoa(htv->pin, debug->string, 16);
	strcat(debug->line, debug->string);
	strcat_P(debug->line, PSTR("\n"));
	debug_print(debug);

	strcpy_P(debug->line, PSTR("Cmd: "));
	debug->string = utoa(htv->cmd, debug->string, 16);
	strcat (debug->line, debug->string);
	strcat_P(debug->line, PSTR("\n"));
	debug_print(debug);

	strcpy_P(debug->line, PSTR("CRC: "));
	debug->string = utoa(htv->crc, debug->string, 16);
	strcat (debug->line, debug->string);
	strcat_P(debug->line, PSTR("\n"));
	debug_print(debug);
}
