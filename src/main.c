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

/*! \file main.c
  \brief Main.
 */

/*! \mainpage OneWay main page
 *
 * \section secproto Protocol definitions:
 * There are 2 protocol in use:
 *
 * \ref pctxproto
 *
 * \ref txrxproto
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

/*! \brief start here. */
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
