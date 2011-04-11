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

/*! \file main.c
  \brief Main.
 */

#define AU_PORT PORTA
#define AU_DDR DDRA
#define AU_ENABLE PA5
#define AU_TXRX PA6

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"
#include "debug.h"

#ifdef SLAVE
#include "receive.h"
#endif

#ifdef MASTER
#include "transmit.h"
#endif

int main(void)
{
	struct debug_t *debug;

	/* Init sequence, turn on both led */
	led_init();
	uart_init(1);
	debug = debug_init();
	AU_DDR |= _BV(AU_ENABLE) | _BV(AU_TXRX);
	led_set(BOTH, OFF);
	led_set(GREEN, ON);

	sei();

#ifdef MASTER
	master(debug);
#endif

#ifdef SLAVE
	slave(debug);
#endif

	cli();
	return(0);
}
