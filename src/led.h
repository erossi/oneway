/* This file is part of HospiTV
 * Copyright (C) 2010, 2011 Enrico Rossi
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

/* Doxygen undocumented */

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <avr/io.h>

/*! the port where led are connected. */
#define LED_PORT PORTB
/*! the data direction register. */
#define LED_DDR DDRB
/*! red led pin */
#define LED_RED PB2
/*! green led pin */
#define LED_GREEN PB3
/*! the delay function used in blink. */
#define LED_DELAY _delay_ms(200)

/*! Leds statuses */
#define OFF 0
#define ON 1
#define BLINK 2

/*! Leds types */
#define NONE 0
#define RED 1
#define GREEN 2
#define BOTH 3

void led_set(const uint8_t led, const uint8_t status);
void led_init(void);

#endif
