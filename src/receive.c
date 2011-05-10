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
#include <avr/eeprom.h>
#include <util/delay.h>

#include "receive.h"

void start_rx(void)
{
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

	uart_rx(1, 1);
}

void setup_address(struct htv_t *htv, struct debug_t *debug)
{
	debug_print_P(PSTR("You should set the address here!\n"), debug);
}

uint8_t look_for_cmd(struct htv_t *htv, struct debug_t *debug)
{
	uint8_t i = 0;

	while (i<10) {
		*(htv->x10str + i) = uart_getchar(1, 1);
		uart_putchar(0, *(htv->x10str + i));

		/* ignore 'x' char.
		 * In the beginning there can be more 'x'
		 * before the command string.
		 */
		if (*htv->x10str != 'x')
			i++;
	}

	*(htv->x10str + 10) = 0;

	if (htv_check_cmd(htv, 0))
		return(1);
	else
		return(0);

}

/*! The HTV network address */
uint16_t EEMEM EE_address;
/*! The ~EE_address to check the correct value of the address */
uint16_t EEMEM EE_naddress;

void slave(struct debug_t *debug)
{
	struct htv_t *htv;
	char *crc8s;
	long count;

	htv = malloc(sizeof(struct htv_t));
	htv->substr = malloc(5);
	htv->x10str = malloc(MAX_CMD_LENGHT);
	/* Re-use pre-allocated space */
	crc8s = debug->string;

	led_set(GREEN, ON);

	count=0;

	htv->address = eeprom_read_word(&EE_address);

	/* check the if the network address is correct */
	if (htv->address != ~(eeprom_read_word(&EE_naddress))) {
		htv->address = 0;
		setup_address(htv, debug);
	}

	while (1) {
		*htv->x10str = uart_getchar(1, 1);
		uart_putchar(0, *htv->x10str);
		
		/* look for the 1st 'x' */
		if (*htv->x10str == 'x') {
			*htv->x10str = uart_getchar(1, 1);
			uart_putchar(0, *htv->x10str);

			/* look for the 2nd 'x' */
			if ((*htv->x10str == 'x') && look_for_cmd(htv, debug)) {
				led_set(RED, ON);

				if (strstr(htv->x10str, "turn_0"))
					led_set(BOTH, OFF);

				if (strstr(htv->x10str, "turn_1")) {
					led_set(GREEN, ON);
					debug->line = ltoa(count, debug->string, 10);
					strcat(debug->line, "\n");
					debug_print(debug);
					count++;
				}

				led_set(RED, OFF);
			}
		}
	}

	free(crc8s);
	free(htv->x10str);
	free(htv->substr);
	free(htv);
}
