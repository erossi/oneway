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

/*! \file transmit.c
  \brief Main.
 */

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "transmit.h"

/* Enable TX signal */
void start_tx(void)
{
	uart_tx(1, 1);
	AU_PORT |= _BV(AU_TXRX);
	_delay_us(400);
}

/* Disable TX signal */
void stop_tx(void)
{
	uart_tx(1, 0);
	AU_PORT &= ~_BV(AU_TXRX);
	_delay_us(400);
}

void master(struct debug_t *debug)
{
	long count;

	count = 0;

	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);

	while (1) {
		start_tx();
		led_set(RED, ON);
		uart_printstr(1, "turn_1");
		_delay_ms(50);
		uart_printstr(1, "turn_1");
		_delay_ms(1);
		stop_tx();

		_delay_ms(2000);

		start_tx();
		led_set(BOTH, OFF);
		uart_printstr(1, "turn_0");
		_delay_ms(50);
		uart_printstr(1, "turn_0");
		_delay_ms(1);
		stop_tx();

		debug->line = ltoa(count, debug->string, 10);
		strcat(debug->line, "\n");
		debug_print(debug);
		count++;

		_delay_ms(2000);
		}
}
