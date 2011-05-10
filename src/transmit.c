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

/*! \file transmit.c
  \brief The TX code start here.
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

/*! \brief transmit a string to the air
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

  \sa host_check_command()
  \param cmd pre-allocated space for the returned string.
 */
void host_get_command(char *cmd)
{
	uint8_t i = 0;

	do {
		*(cmd + i) = uart_getchar(0, 1);
		i++;
	} while ((i<MAX_CMD_LENGHT) && (*(cmd + i - 1) != '\r'));

	/*! Substitute '\n' with a \0 to terminate the string or
	 put a \0 at cmd[19] */
	i--;
	*(cmd + i) = 0;
}

void master(struct debug_t *debug)
{
	struct htv_t *htv;
	char *crc8s;

	htv = malloc(sizeof(struct htv_t));
	htv->substr = malloc(5);
	htv->x10str = malloc(MAX_CMD_LENGHT);
	/* Re-use pre-allocated space */
	crc8s = debug->string;

	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);

	led_set(GREEN, ON);

	while (1) {
		host_get_command(htv->x10str);

		if (htv_check_cmd(htv, 1)) {
			/* calculate crc8 of the command */
			htv->crc = crc8_str(htv->x10str);

			/* string leght check and prepend a '0' if
			 * needed.
			 */
			if (htv->crc < 0x10) {
				strcpy_P(crc8s, PSTR("0"));
				strcat(crc8s, utoa(htv->crc, htv->substr, 16));
			} else {
				crc8s = utoa(htv->crc, crc8s, 16);
			}

			htv->x10str = strcat(htv->x10str, crc8s);
			tx_str(htv->x10str, 0);

			/* Debug only
			strcpy_P(debug->line, PSTR("Addr: "));
			debug->string = utoa(htv->address, debug->string, 16);
			strcat(debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);

			strcpy_P(debug->line, PSTR("Addr H: "));
			debug->string = utoa(htv->haddr, debug->string, 16);
			strcat(debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);

			strcpy_P(debug->line, PSTR("Addr L: "));
			debug->string = utoa(htv->laddr, debug->string, 16);
			strcat(debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);

			strcpy_P(debug->line, PSTR("Pin: "));
			debug->string = utoa(htv->pin, debug->string, 16);
			strcat(debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);

			strcpy_P(debug->line, PSTR("Cmd: "));
			debug->string = utoa(htv->cmd, debug->string, 16);
			strcat (debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);

			strcpy_P(debug->line, PSTR("CRC16: "));
			debug->string = utoa(htv->crc16, debug->string, 16);
			strcat (debug->line, debug->string);
			strcat_P(debug->line, PSTR("\n"));
			debug_print(debug);
			*/

			debug_print_P(PSTR("ok\n"), debug);
		} else {
			debug_print_P(PSTR("ko\n"), debug);
		}
	}

	free(crc8s);
	free(htv->x10str);
	free(htv->substr);
	free(htv);
}
