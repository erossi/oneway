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
#include <util/crc16.h>
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

void tx_str(const char *str)
{
	led_set(RED, ON);
	start_tx();
	uart_printstr(1, "xxx");
	uart_printstr(1, str);
	_delay_ms(1);
	stop_tx();
	led_set(RED, OFF);
}

uint8_t crc8_str(char *str)
{
	uint8_t i, crc8;

	crc8 = 0;

	for (i=0; i<strlen(str); i++)
		crc8 = _crc_ibutton_update(crc8, *(str + i));

	return(crc8);
}

/*! \brief Check if the command received by the host is correct.

  A vaild command is in the form of:
  AAAAPPC:RR\n
  where
  AAAA: Client address in HEX (2 byte)
  PP:   pin number in HEX (1 byte)
  C: command: 2 On , 3 Off
  RR crc calculated before ":"

  if cmd is not correct clear the cmd string and return FALSE
  else if cmd is ok, modify cmd and keep only AAAAPPC.
 */
uint8_t host_check_command(char *cmd)
{
	uint8_t err=0;

	if (strlen(cmd) != 10)
		err |= _BV(1);

	if (*(cmd + 7) != ':')
		err |= _BV(2);

	/*! \todo Do the crc checksum on the string. */

	if (err)
		*cmd = 0;
	else
		*(cmd + 7) = 0;

	return (!err);
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
	char *cmd;
	char *crc8s;
	uint8_t crc8;

	cmd = malloc(MAX_CMD_LENGHT);
	/* Re-use pre-allocated space */
	crc8s = debug->string;

	AU_PORT |= _BV(AU_ENABLE);
	_delay_us(20);

	led_set(GREEN, ON);

	while (1) {
		host_get_command(cmd);

		if (host_check_command(cmd)) {
			crc8 = crc8_str(cmd);
			crc8s = utoa(crc8, crc8s, 16);
			cmd = strcat(cmd, crc8s);
			tx_str(cmd);
			debug_print_P(PSTR("ok\n"), debug);
		} else {
			debug_print_P(PSTR("ko\n"), debug);
		}
	}

	free(crc8s);
	free(cmd);
}
