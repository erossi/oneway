/* This file is part of OneWay
 * Copyright (C) 2010, 2011 Enrico Rossi
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

/*! \file led.c
 * \brief led handling functions.
 */

#include <util/delay.h>
#include "led.h"

/*! \brief turn on, off and blink a led or both.
 *
 * \param led RED, GREEN, BOTH, NONE.
 * \param status ON, OFF, BLINK */
void led_set(const uint8_t led, const uint8_t status)
{
	switch (status) {
		case OFF:
			/* led_set(NONE, OFF); */

			if ((led == RED) || (led == BOTH))
				LED_PORT |= _BV(LED_RED);

			if ((led == GREEN) || (led == BOTH))
				LED_PORT |= _BV(LED_GREEN);

			break;
		case ON:
			/* led_set(NONE, OFF); */

			if ((led == RED) || (led == BOTH))
				 LED_PORT &= ~_BV(LED_RED);

			if ((led == GREEN) || (led == BOTH))
				LED_PORT &= ~_BV(LED_GREEN);

			break;
		case BLINK:
			LED_DELAY;

			switch (led) {
				case RED:
					led_set(RED, ON);
					break;
				case GREEN:
					led_set(GREEN, ON);
					break;
				default:
					led_set(BOTH, ON);
			}

			LED_DELAY;
			led_set(NONE, OFF);
			break;
		default:
			LED_PORT |= (_BV(LED_RED) | _BV(LED_GREEN));
	}
}

/*! \brief initialize the port and turn both led on. */
void led_init(void)
{
	LED_DDR |= (_BV(LED_RED) | _BV(LED_GREEN));
	led_set(BOTH, ON);
}
