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

/*! \file receive.c
  \brief Main.
 */

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "receive.h"

void slave(void)
{
	char *buff;
	int i;

	buff = malloc(10);

	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);
	AU_PORT |= _BV(AU_TXRX);
	_delay_us(200);
	AU_PORT &= ~_BV(AU_TXRX);
	_delay_us(40);
	AU_PORT &= ~_BV(AU_ENABLE);
	_delay_us(20);
	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(200);

	while (1) {
		i=0;
		*buff = uart_getchar(1);		
		
		if (*buff == 't') {
			for (i=1; i<6; i++)
				*(buff + i) = uart_getchar(1);

			*(buff + 6) = 0;

			if (strstr(buff, "turn_0"))
				led_set(BOTH, OFF);

			if (strstr(buff, "turn_1"))
				led_set(GREEN, ON);

		}
	}

	free(buff);
}
