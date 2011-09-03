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

/*! \file receive.c
 * \brief the receiver code.
 *
 * \page txrxproto Interface protocol from TX to RX:
 *
 * from master tx -> slave rx:
 * - a serial string at 1200 bps, none parity, 8 bit, 1 bit stop.
 *
 * The module will display any char received on the console, connected
 * to the serial port 1, within the range ascii from 32 to 128.
 *
 * It is possible in this way to monitor constantly the whole network,
 * but only those messages directed to broadcast or to us will be
 * executed.
 *
 * \section secrxcmd Sections:
 * - \ref subrxacmd
 * - \ref subrxpcmd
 *
 * \subsection subrxacmd a - change the address of the receiver.
 *
 * This command must be entered from the console.
 *
 * example:
 *
 * -> a\n
 * <- Change address, remeber:\n
 * <- - the address is in HEX, use digit from 0 to f\n
 * <- - do not use 0000 or ffff as address\n
 * <- Enter the 4 digit address [0001 - fffe]:\n
 *
 * \subsection subrxpcmd TxRx protocol definition.
 * The received string must be in the form:
 *
 * xx[x..x]AAAAPPC:RR
 *
 * where
 * - at least 2 'x' sync char must be received.
 * - AAAA is the address ascii - hex from 0000 to FFFF where:
 *   - 0000 unconfigured device.
 *   - FFFF is broadcast address.
 * - PP is the pin number in Ascii/hex form from 00 to FF where:
 *   - 00 - i/o pin 0
 *   - 01 - i/o pin 1
 *   - FF - All pin
 * - C is the command in ascii/hex where:
 *   - 0 is off.
 *   - 1 is on.
 * - RR is an 8 bit checksum of the whole string.
 *
 * example
 *
 * -> xxx0123011:?? (the value of RR unknown here)\n
 * <- Received: 0123011 OK\n
 * <- Action: Pin1 enable
 *
 * \note any command on the air will be checked and displayed, but
 * only those for us will be executed.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "receive.h"

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
 * \note only char from ascii 32 to 128 are printed back.
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

/*! \brief execute command on a pin.
 *
 * \param pin which pin to enable or disable.
 * \param cmd 1 - enable, 0 - disable
 * \param debug the debug_t struct.
 */
void set_cmd(const uint8_t pin, const uint8_t cmd, struct debug_t *debug)
{
	switch (cmd) {
		case 1:
			IO_PORT |= _BV(pin);
			debug_print_P(PSTR("on"), debug);
			break;
		case 0:
			IO_PORT &= ~_BV(pin);
			debug_print_P(PSTR("off"), debug);
			break;
		default:
			debug_print_P(PSTR("none"), debug);
	}
}

/*! \brief enable the IO and led based on the received command.
 *
 * \note the address check is done by comparing the received
 * address with the ee_address stored in htv_t.
 */
void set_pin(struct htv_t *htv, struct debug_t *debug)
{
	if ((htv->address == 0xffff) || (htv->address == htv->ee_addr)) {
		debug_print_P(PSTR("Action: "), debug);

		switch (htv->pin) {
			case 0:
				debug_print_P(PSTR("Pin0 - "), debug);
				set_cmd(IO_PIN0, htv->cmd, debug);
				break;
			case 1:
				debug_print_P(PSTR("Pin1 - "), debug);
				set_cmd(IO_PIN1, htv->cmd, debug);
				break;
			default:
				debug_print_P(PSTR("Unsupported IO"), debug);
		}

		debug_print_P(PSTR("\n"), debug);
	}
}

/*! \brief receive the AAAAPPC:RR string */
void look_for_cmd(struct htv_t *htv, struct debug_t *debug)
{
	uint8_t i = 0;

	while (i<10) {
		*(htv->x10str + i) = get_char_echo();

		/* ignore 'x' char.
		 * In the beginning there can be more 'x'
		 * before the command string.
		 */
		if (*htv->x10str != 'x')
			i++;
	}

	/* correctly terminate the string */
	*(htv->x10str + 10) = 0;
	/* print what has been received */
	debug_print_P(PSTR("\nReceived: "), debug);
	uart_printstr(0, htv->x10str);
	/* check the command */
	i = htv_check_cmd(htv);

	/* if error */
	if (i) {
		debug_print_P(PSTR(" Error "), debug);
		debug->line = utoa(i, debug->line, 16);
		debug_print(debug);
		debug_print_P(PSTR("\n"), debug);
		/*! \bug in case of error 4 the struct htv
		 * is mostly void, and printing it's content
		 * will be a memory leak crash.
		 */
		/* debug_print_htv(htv, debug); */
	} else {
		debug_print_P(PSTR(" OK\n"), debug);
		/* execute the command */
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
	debug_print_P(PSTR("Receive module.\n"), debug);
	debug_print_address(htv, debug);

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

		/* also read a char from the serial port, unlocked */
		c = uart_getchar(0, 0);

		/* change the running address */
		if (c == 'a') {
			stop_rx();
			uart_flush(0);
			uart_flush(1);
			debug_setup_address(htv, debug);
			start_rx();
		}
	}

	htv_free(htv);
}
