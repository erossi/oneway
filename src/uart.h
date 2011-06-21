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

#ifndef _UART_H_
#define _UART_H_

/* UART baud rate */
#define UART_BAUD_0 9600
#define UART_BAUD_1 1200
#define UART_RXBUF_SIZE 64
#define UART_TXBUF_SIZE 64
#define UART_RXBUF_MASK ( UART_RXBUF_SIZE - 1 )
#define UART_TXBUF_MASK ( UART_TXBUF_SIZE - 1 )
#if ( UART_RXBUF_SIZE & UART_RXBUF_MASK )
#error RX buffer size is not a power of 2
#endif
#if ( UART_TXBUF_SIZE & UART_TXBUF_MASK )
#error TX buffer size is not a power of 2
#endif

/*! used as buffer area for IRQ rx/tx */
struct uartStruct {
	/*! rx buffer area */
        char *rx_buffer;
	/*! tx buffer area */
        char *tx_buffer;
	/*! an IRQ rx string is received. */
        volatile uint8_t rx_flag;
	/*! the IRQ tx has been completed. */
        volatile uint8_t tx_flag;
	/*! rx index */
        volatile uint8_t rxIdx;
	/*! tx index */
        volatile uint8_t txIdx;
};

void uart_tx(const uint8_t port, const uint8_t enable);
void uart_rx(const uint8_t port, const uint8_t enable);
void uart_init(const uint8_t port);
void uart_shutdown(const uint8_t port);
char uart_getchar(const uint8_t port, const uint8_t locked);
void uart_putchar(const uint8_t port, const char c);
void uart_printstr(const uint8_t port, const char *s);
void uart_flush(const uint8_t port);

#endif
