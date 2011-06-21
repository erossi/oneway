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

/*! \file transmit.c
 * \brief Interface and API from PC to TX module.
 *
 * \page pctxproto Interface protocol from host to TX:
 *
 * Serial connection at 9600 bps, none parity, 8 bit, 1 bit stop.
 *
 * \section seccmd Possible command:
 * - \ref subacmd
 * - \ref subccmd
 * - \ref subecmd
 * - \ref sublcmd
 * - \ref subpcmd
 * - \ref subhcmd
 *
 * \subsection subacmd A - change the address of a remote.
 * A:OOOO:NNNN:OOOO:NNNN
 *
 * where:
 * - OOOO is the old address
 * - NNNN is the new address
 *
 * reply to the 'A' command can be:
 * - "OK" the command is received and forwarded to the clients.
 * - "ko" some error occured.
 *
 * example:
 *
 * -> A:0123:1CDF:0123:1CDF\n
 * <- OK
 *
 * will change the device address 0x0123 to 0x1CDF.
 *
 * \subsection subccmd C - change the id of the master.
 * C:N
 *
 * where:
 * - N is the new address [0:f]
 *
 * reply to the 'C' command can be:
 * - "OK" the address has changed.
 * - "ko" some error occured.
 *
 * example:
 *
 * -> C:D\n
 * <- OK
 *
 * will change the master address 0x0 to 0xD.
 *
 * \subsection subecmd E - echo on off.
 * E:X
 *
 * where:
 * - X can be '0' or '1', 0 - disable echo, 1 - enable echo.
 *
 * example:
 *
 * -> E:0\n
 * <- OK
 *
 * echo disabled.
 *
 * \subsection sublcmd L - print the TX id.
 * example (id = 2):
 *
 * -> L\n
 * <- 2
 *
 * \subsection subpcmd P - send a command to a remote.
 * P:AAAA:PP:C\n
 *
 * where
 * - AAAA is the address ascii - hex from 0001 to FFFF where:
 *   - 0000 unconfigured device.
 *   - FFFF is broadcast address.
 * - PP is the pin number in Ascii/hex form from 00 to FF where:
 *   - 00 - i/o pin 0
 *   - 01 - i/o pin 1
 *   - FF - All pin
 * - C is the command in ascii/hex where:
 *   - 0 is off.
 *   - 1 is on.
 *
 * reply to the 'P' command can be:
 * - "OK" the command is received and forwarded to the clients.
 * - "ko" some error occured.
 *
 * example
 *
 * -> P:012F:01:1\n
 * <- OK
 *
 * will send to the device "012F" the command "turn on the pin 1".
 * \note address "0000" is used by unconfigurd devices and
 * should not be used in normal condition.
 *
 * \subsection subhcmd ? - help command.
 * example:
 *
 * -> ?\n
 * <- the brief commands descrption.
 *
 * \todo the A command is not implemented yet.
 * \todo the C command is not implemented yet.
 */

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "transmit.h"

/*! \brief Enable TX signal. */
void start_tx(void)
{
	/*! Enable the serial port */
	uart_tx(1, 1);
	/*! Enable the transmit pin on the rtx only module
	as described in the datasheet with delay timing. */
	AU_PORT |= _BV(AU_TXRX);
	_delay_us(400);

	/*! add another delay for opening the squelch in the receiver. */
	_delay_ms(10);
}

/*! \brief Disable TX signal */
void stop_tx(void)
{
	uart_tx(1, 0);
	AU_PORT &= ~_BV(AU_TXRX);
	_delay_us(400);
}

/*! \brief transmit a string on the air
 *
 * Send the string with the header to the air.
 * \param str the string to be sent.
 * \param port the serial port.
 */
void tx_str(const char *str, const uint8_t port)
{
	led_set(RED, ON);
	start_tx();
	uart_printstr(port, TX_HEAD);
	uart_printstr(port, str);
	_delay_ms(1);
	stop_tx();
	led_set(RED, OFF);
}

/*! \brief wait until a command is entered.
 *
 * Wait for a char from serial port and echo it if
 * echo is '1'. The command is terminated by a '\\r'.
 *
 * \sa host_check_command()
 * \param cmd pre-allocated space for the returned string.
 * \param echo 1: echo the chars while typing.
 * \note A maximum of MAX_CMD_LENGHT number of char can be
 * entered.
 * The last '\\r' is substituted by a '\0' or, if the maximum
 * number of char is reached, the last one is changed.
 */
void host_get_command(char *cmd, const uint8_t echo)
{
	uint8_t i = 0;

	do {
		*(cmd + i) = uart_getchar(0, 1);

		/* echo the char */
		if (echo)
			uart_putchar(0, *(cmd + i));

		i++;
	} while ((i<MAX_CMD_LENGHT) && ((*(cmd + i - 1) != '\r') || (*(cmd + i - 1) != '\n')));

	/* Substitute '\n' with a \0 to terminate the string or
	 put a \0 at cmd[19] */
	i--;
	*(cmd + i) = 0;
}

/*! \brief pin related command
 * in the form:
 * P:AAAA:PP:C
 */
void p_cmd(struct htv_t *htv, struct debug_t *debug)
{
	char *crc8s;

	/* Re-use pre-allocated space */
	crc8s = debug->string;
	/* transform to AAAAaa:PP:C */
	memmove(htv->x10str, htv->x10str + 2, 4);
	/* to AAAAPP:PP:C */
	memmove(htv->x10str + 4, htv->x10str + 7, 2);
	/* to AAAAPPC */
	memmove(htv->x10str + 6, htv->x10str + 10, 1);
	*(htv->x10str + 7) = 0;

	/* check the command */
	if (!htv_check_cmd(htv)) {
		/* calculate crc8 */
		htv->crc = crc8_str(htv->x10str);

		/* if crc is a single digit, prepend a '0' */
		if (htv->crc < 0x10) {
			strcpy_P(crc8s, PSTR("0"));
			strcat(crc8s, utoa(htv->crc, htv->substr, 16));
		} else {
			crc8s = utoa(htv->crc, crc8s, 16);
		}

		/* Convert AAAAPPC to AAAAPPC:RR */
		*(htv->x10str + 7) = ':';
		*(htv->x10str + 8) = 0;
		htv->x10str = strcat(htv->x10str, crc8s);
		tx_str(htv->x10str, 1);
		debug_print_P(PSTR("OK\n"), debug);
	} else {
		debug_print_P(PSTR("ko\n"), debug);
	}
}

/*! \brief main TX loop */
void master(struct debug_t *debug)
{
	struct htv_t *htv;
	uint8_t echo = 1;

	htv = NULL;
	htv = htv_init(htv);

#ifdef HTV_USE_RTX
	AU_DDR |= _BV(AU_ENABLE) | _BV(AU_TXRX);
	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);
#endif

	uart_init(1);
	led_set(GREEN, ON);
	debug_print_P(PSTR("Master module.\n"), debug);

	while (1) {
		host_get_command(htv->x10str, echo);

		switch (*(htv->x10str)) {
			case 'A':
				debug_print_P(PSTR("ko\n"), debug);
				break;
			case 'C':
				debug_print_P(PSTR("ko\n"), debug);
				break;
			case 'E':
				switch (*(htv->x10str + 2)) {
					case '0':
						echo = 0;
						debug_print_P(PSTR("OK\n"), debug);
						break;
					case '1':
						echo = 1;
						debug_print_P(PSTR("OK\n"), debug);
						break;
					default:
						debug_print_P(PSTR("ko\n"), debug);
				}
				break;
			case 'L':
				debug_print_P(PSTR(TX_ID), debug);
				debug_print_P(PSTR("\n"), debug);
				break;
			case 'P':
				p_cmd(htv, debug);
				break;
			case '?':
				debug_print_P(PSTR("Help:\n"), debug);
				debug_print_P(PSTR("A:OOOO:NNNN:OOOO:NNNN change the remote device's address from OOOO to NNNN.\n"), debug);
				debug_print_P(PSTR("P:AAAA:PP:C send a command.\n"), debug);
				debug_print_P(PSTR("L print the TX id.\n"), debug);
				debug_print_P(PSTR("E:x where x 1 or 0, enable or disable echo.\n"), debug);
				debug_print_P(PSTR("? this help.\n"), debug);
				break;
			default:
				debug_print_P(PSTR("ko\n"), debug);
		}
	}

	htv_free(htv);
}
