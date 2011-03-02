/* This file is part of hospitv
 * Copyright (C) 2011 Enrico Rossi
 *
 * Chpc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Chpc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file main.c
  \brief Main.
 */

#define AU_PORT PORTA
#define AU_DDR DDRA
#define AU_ENABLE PA5
#define AU_TXRX PA6

#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"

void master(void)
{
	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);
	AU_PORT |= _BV(AU_TXRX);
	_delay_us(400);

	while (1) {
/*
		AU_PORT |= _BV(AU_TXRX);
		_delay_us(400);
*/
		led_set(RED, ON);
		uart_printstr(1, "turn_1");
		_delay_ms(50);
		uart_printstr(1, "turn_1");

/*
		uart_printstr(1, "turn_1");
		_delay_ms(1);

		AU_PORT &= ~_BV(AU_TXRX);
		_delay_us(400);
*/
		_delay_ms(1000);
/*
		AU_PORT |= _BV(AU_TXRX);
		_delay_us(400);
*/
		led_set(BOTH, OFF);
		uart_printstr(1, "turn_0");
		_delay_ms(50);
		uart_printstr(1, "turn_0");
/*
		uart_printstr(1, "turn_0");
		_delay_ms(1);

		AU_PORT &= ~_BV(AU_TXRX);
		_delay_us(400);
*/
		_delay_ms(1000);
		}
}

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

int main(void)
{
	/* Init sequence, turn on both led */
	led_init();
	uart_init(1);
	AU_DDR |= _BV(AU_ENABLE) | _BV(AU_TXRX);
	led_set(BOTH, OFF);

	sei();

#ifdef MASTER
	master();
#endif

#ifdef SLAVE
	slave();
#endif

	cli();
	return(0);
}
