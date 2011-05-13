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
#ifdef HTV_USE_RTX
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
#endif

	uart_rx(1, 1);
}

char get_char_echo(void)
{
	char c;

	c = uart_getchar(1, 1);

	if ((c > 32) && (c < 128))
		uart_putchar(0, c);
	else
		uart_putchar(0, '*');

	return(c);
}

void setup_address(struct htv_t *htv, struct debug_t *debug)
{
	debug_print_P(PSTR("You should set the address here!\n"), debug);
}

void set_pin(struct htv_t *htv, struct debug_t *debug)
{
	if ((htv->address == 0xffff) || (htv->address == htv->ee_addr)) {
		debug_print_P(PSTR("Enable: "), debug);

		if (htv->pin & _BV(0)) {
			IO_PORT |= _BV(IO_PIN0);
			debug_print_P(PSTR(" Pin0"), debug);
		} else {
			IO_PORT &= ~_BV(IO_PIN0);
		}

		if (htv->pin & _BV(1)) {
			IO_PORT |= _BV(IO_PIN1);
			debug_print_P(PSTR(" Pin1"), debug);
		} else {
			IO_PORT &= ~_BV(IO_PIN1);
		}

		if (htv->pin & _BV(2)) {
			led_set(GREEN, ON);
			debug_print_P(PSTR(" Green"), debug);
		} else {
			led_set(GREEN, OFF);
		}

		if (htv->pin & _BV(3)) {
			led_set(RED, ON);
			debug_print_P(PSTR(" Red"), debug);
		} else {
			led_set(RED, OFF);
		}

		debug_print_P(PSTR("\n"), debug);
	}
}

/*! \brief receive the AAAAP:RR string */
void look_for_cmd(struct htv_t *htv, struct debug_t *debug)
{
	uint8_t i = 0;

	while (i<8) {
		*(htv->x10str + i) = get_char_echo();

		/* ignore 'x' char.
		 * In the beginning there can be more 'x'
		 * before the command string.
		 */
		if (*htv->x10str != 'x')
			i++;
	}

	*(htv->x10str + 8) = 0;

	debug_print_P(PSTR("\nReceived: "), debug);
	uart_printstr(0, htv->x10str);

	i = htv_check_cmd(htv, 0);
	if (i) {
		debug_print_P(PSTR(" Error "), debug);
		debug->line = utoa(i, debug->line, 16);
		debug_print(debug);
		debug_print_P(PSTR("\n"), debug);
		debug_print_htv(htv, debug);
	} else {
		debug_print_P(PSTR(" OK\n"), debug);
		set_pin(htv, debug);
	}
}

/*! The HTV network address */
uint16_t EEMEM EE_address;
/*! The ~EE_address to check the correct value of the address */
uint16_t EEMEM EE_naddress;

void slave(struct debug_t *debug)
{
	struct htv_t *htv;

	htv = malloc(sizeof(struct htv_t));
	htv->substr = malloc(5);
	htv->x10str = malloc(MAX_CMD_LENGHT);

#ifdef HTV_USE_RTX
	AU_DDR |= _BV(AU_ENABLE) | _BV(AU_TXRX);
#endif

	/* Init IO port */
	IO_PORT &= ~(_BV(IO_PIN0) | _BV(IO_PIN1));
	IO_DDR |= _BV(IO_PIN0) | _BV(IO_PIN1);

	uart_init(1);
	start_rx();

	htv->ee_addr = eeprom_read_word(&EE_address);

	/* check the if the network address is correct */
	if (htv->ee_addr != ~(eeprom_read_word(&EE_naddress))) {
		htv->ee_addr = 0;
		setup_address(htv, debug);
	}

	while (1) {
		*htv->x10str = get_char_echo();
		
		/* look for the 1st 'x' */
		if (*htv->x10str == 'x') {
			*htv->x10str = get_char_echo();

			/* look for the mandatory 2nd 'x' */
			if (*htv->x10str == 'x')
			       look_for_cmd(htv, debug);
		}
	}

	free(htv->x10str);
	free(htv->substr);
	free(htv);
}
