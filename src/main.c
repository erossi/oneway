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

/*! \mainpage HospiTV main page
 *
 * \session proto_sec Protocol definitions:
 *
 * from host to master tx:
 * - a serial string at 9600 bps, none parity, 8 bit, 1 bit stop.
 * The string is in the form:
 * AAAAPPC\n
 * where
 * AAAA is the address in hex (ascii) from 0 to FFFF where FFFF is
 *      considered a broadcast address.
 *
 * PP is the pin number to act to, from 0 to FF (ascii) it represent:
 *    1 - i/o pin 0
 *    2 - i/o pin 1
 *    4 - red led
 *    8 - green led
 *    it's value is the sum of these value, a bitmapped byte.
 *    ex. 0x0a is green led and i/o pin 1,
 *        0x0c is green led and red led.
 *
 * C is the command, for now 2 is on and 3 is off.
 *
 * reply from tx to master:
 * OK the command is received and forwarded to the clients.
 * ko some error occured.
 *
 * from master tx -> slave rx:
 * - a serial string at 4800 bps, none parity, 8 bit, 1 bit stop.
 * The string is in the form:
 * AAAAP:RR\n
 * where
 * AAAA is the same as above, P = PP, while RR is crc of the string.
 *
 */

#include "led.h"
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
	debug = debug_init();
	led_set(BOTH, OFF);

#ifdef MASTER
	master(debug);
#endif

#ifdef SLAVE
	slave(debug);
#endif

	return(0);
}
