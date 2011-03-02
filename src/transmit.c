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
#include <avr/io.h>
#include <util/delay.h>
#include "transmit.h"

void master(void)
{
	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);

	while (1) {
		/* Enable TX signal */
		AU_PORT |= _BV(AU_TXRX);
		_delay_us(400);

		led_set(RED, ON);
		uart_printstr(1, "turn_1");
		_delay_ms(50);
		uart_printstr(1, "turn_1");

		/* Disable TX signal */
		_delay_ms(1);
		AU_PORT &= ~_BV(AU_TXRX);
		_delay_us(400);

		_delay_ms(1000);

		/* Enable TX signal */
		AU_PORT |= _BV(AU_TXRX);
		_delay_us(400);

		led_set(BOTH, OFF);
		uart_printstr(1, "turn_0");
		_delay_ms(50);
		uart_printstr(1, "turn_0");

		/* Disable TX signal */
		_delay_ms(1);
		AU_PORT &= ~_BV(AU_TXRX);
		_delay_us(400);

		_delay_ms(1000);
		}
}
