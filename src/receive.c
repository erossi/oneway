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
 * \bried the receiver code.
 */

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "receive.h"

/*! The HTV network address */
uint16_t EEMEM EE_address;
/*! The ~EE_address to check the correct value of the address */
uint16_t EEMEM EE_naddress;

/*! \brief enable the RX module and related serial port.
 * \note if RTX moduled is used, then a more complicated
 * init sequence must be used.
 */
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

/*! \brief shutdown the receiver.
 *
 * \todo implement the pin sequence for rtx module.
 */
void stop_rx(void)
{
	uart_rx(1, 0);
}

/*! \brief get a char from the RX and echo it on the console.
 * \return the received char.
 */
char get_char_echo(void)
{
	char c;

	c = uart_getchar(1, 1);

	/* print it if it is readable */
	if ((c > 32) && (c < 128))
		uart_putchar(0, c);
	/*
	   else
	   uart_putchar(0, '*');
	   */

	return(c);
}

/*! \brief print the RX address in use.
 */
void print_address(struct htv_t *htv, struct debug_t *debug)
{
	debug_print_P(PSTR("\nAddress set to: 0x"), debug);
	debug->line = utoa(htv->ee_addr, debug->string, 16);
	debug_print(debug);
	debug_print_P(PSTR("\n"), debug);
}

/*! \brief input and store the address of the unit in EEPROM.
 */
void setup_address(struct htv_t *htv, struct debug_t *debug)
{
	uint8_t i;
	char c = 0;

	while ((c != 'y') && (c != 'Y')) {
		print_address(htv, debug);
		debug_print_P(PSTR("\nChange address, remeber:\n"), debug);
		debug_print_P(PSTR(" - the address is in HEX, use digit from 0 to f\n"), debug);
		debug_print_P(PSTR(" - do not use 0000 or ffff as address\n"), debug);
		debug_print_P(PSTR("\nEnter the 4 digit address [0001 - fffe]: "), debug);

		for (i=0; i<4; i++) {
			*(htv->substr + i) = uart_getchar(0, 1);
			uart_putchar(0, *(htv->substr + i));
		}

		*(htv->substr + 4) = 0;
		htv->ee_addr = strtoul(htv->substr, 0, 16);
		print_address(htv, debug);
		debug_print_P(PSTR("confirm? (y/n) "), debug);
		c = uart_getchar(0, 1);
		uart_putchar(0, c);
	}

	eeprom_write_word(&EE_address, htv->ee_addr);
	eeprom_write_word(&EE_naddress, ~(htv->ee_addr));
	debug_print_P(PSTR("\nAddress changed and saved.\n"), debug);
	debug_print_P(PSTR("Reset the receiver to check if everything is OK\n"), debug);
}

/*! \brief enable the IO and led based on the received command.
 *
 * \note the address check is done by comparing the received
 * address with the ee_address stored in htv_t.
 */
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

	i = htv_check_cmd(htv);
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

/*! \brief the main RX program */
void slave(struct debug_t *debug)
{
	struct htv_t *htv;
	char c;

	htv = NULL;
	htv = htv_init(htv);

#ifdef HTV_USE_RTX
	AU_DDR |= _BV(AU_ENABLE) | _BV(AU_TXRX);
#endif

	/* Init IO port */
	IO_PORT &= ~(_BV(IO_PIN0) | _BV(IO_PIN1));
	IO_DDR |= _BV(IO_PIN0) | _BV(IO_PIN1);
	uart_init(1);
	htv->ee_addr = eeprom_read_word(&EE_address);

	/* check the if the network address is correct */
	if (htv->ee_addr != ~(eeprom_read_word(&EE_naddress))) {
		htv->ee_addr = 0;
		setup_address(htv, debug);
	} else {
		print_address(htv, debug);
	}

	start_rx();

	while (1) {
		c = get_char_echo();
		
		/* look for the 1st 'x' */
		if (c == 'x') {
			c = get_char_echo();

			/* look for the mandatory 2nd 'x' */
			if (c == 'x')
			       look_for_cmd(htv, debug);
		}

		c = uart_getchar(0, 0);

		/* change the running address */
		if (c == 'a') {
			stop_rx();
			uart_flush(0);
			uart_flush(1);
			setup_address(htv, debug);
			start_rx();
		}
	}

	htv_free(htv);
}
