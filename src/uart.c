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

#include <avr/io.h>
#include "uart.h"

/*! \file uart.c
  \brief low level interface to the serial port */

/*! \brief enable/disable the trasmit part of a serial port.
  \param port Serial port number (0 or 1)
  \param enable 0 = disable, 1 = enable
 */
void uart_tx(const uint8_t port, const uint8_t enable)
{
	if (port)
		if (enable)
			UCSR1B |= _BV(TXEN1);
		else
			UCSR1B &= ~_BV(TXEN1);
	else
		if (enable)
			UCSR0B |= _BV(TXEN0);
		else
			UCSR0B &= ~_BV(TXEN0);
}

/*! \brief enable/disable the receive part of a serial port.
  \param port Serial port number (0 or 1)
  \param enable 0 = disable, 1 = enable
 */
void uart_rx(const uint8_t port, const uint8_t enable)
{
	if (port)
		if (enable)
			UCSR1B |= _BV(RXEN1);
		else
			UCSR1B &= ~_BV(RXEN1);
	else
		if (enable)
			UCSR0B |= _BV(RXEN0);
		else
			UCSR0B &= ~_BV(RXEN0);
}

/*! \brief initialize the serial port and speed.
 * \param port the port to initialize.
 * \note it does not enable tx or rx.
 */
void uart_init(const uint8_t port)
{
	if (port) {
		/* improve baud rate error by using 2x clk */
#if F_CPU < 2000000UL && defined(U2X1)
		UCSR1A = _BV(U2X1);
		UBRR1L = (F_CPU / (8UL * UART_BAUD_1)) - 1;
#else
		UBRR1L = (F_CPU / (16UL * UART_BAUD_1)) - 1;
#endif

		/* 8n2 */
		UCSR1C = _BV(USBS1) | _BV(UCSZ10) | _BV(UCSZ11);
	} else {
		/* improve baud rate error by using 2x clk */
#if F_CPU < 2000000UL && defined(U2X0)
		UCSR0A = _BV(U2X0);
		UBRR0L = (F_CPU / (8UL * UART_BAUD_0)) - 1;
#else
		UBRR0L = (F_CPU / (16UL * UART_BAUD_0)) - 1;
#endif

		/* 8n2 */
		UCSR0C = _BV(USBS0) | _BV(UCSZ00) | _BV(UCSZ01);
	}
}

/*! turn off the serial port.
 * \param port the serial port to disable.
 */
void uart_shutdown(const uint8_t port)
{
	if (port) {
		UCSR1C = 0;
		UCSR1B = 0;
		UBRR1L = 0;
		UCSR1A = 0;
	} else {
		UCSR0C = 0;
		UCSR0B = 0;
		UBRR0L = 0;
		UCSR0A = 0;
	}
}

/*! \brief get a char from the serial port.
 * \param port the port.
 * \param locked 1 - wait forever until a char is received.
 * 0 - get a char if it is present or exit with 0.
 *
 * \return the char or 0.
 */
char uart_getchar(const uint8_t port, const uint8_t locked)
{
	if (locked) {
		if (port) {
			loop_until_bit_is_set(UCSR1A, RXC1);
			return(UDR1);
		} else {
			loop_until_bit_is_set(UCSR0A, RXC0);
			return(UDR0);
		}
	} else {
		if (port) {
			if (bit_is_set(UCSR1A, RXC1))
				return(UDR1);
			else
				return(0);
		} else {
			if (bit_is_set(UCSR0A, RXC0))
				return(UDR0);
			else
				return(0);
		}
	}
}

/*! \brief send a single char down to the serial port.

  Send character c down the UART Tx, wait until tx holding register
  is empty.
  \param port serial port 0 or 1.
  \param c char to send.
 */
void uart_putchar(const uint8_t port, const char c)
{
	/*
	   if (c == '\n')
	   uart_putchar(port, '\r');
	   */

	if (port) {
		loop_until_bit_is_set(UCSR1A, UDRE1);
		UDR1 = c;
	} else {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = c;
	}
}

/*! Send a C (NUL-terminated) string to the UART Tx.

  \param port serial port 0 or 1.
  \param s NULL terminated string.
  \note It must be a \0 terminated string.
 */
void uart_printstr(const uint8_t port, const char *s)
{
	while (*s)
		uart_putchar(port, *s++);
}

/*! \brief flush the port */
void uart_flush(const uint8_t port)
{
	while (uart_getchar(port, 0));
}
