#include <avr/io.h>
#include "uart.h"

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

		/*! tx/rx enable */
		UCSR1B = _BV(TXEN1) | _BV(RXEN1);
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

		/*! tx/rx enable */
		UCSR0B = _BV(TXEN0) | _BV(RXEN0);
		/* 8n2 */
		UCSR0C = _BV(USBS0) | _BV(UCSZ00) | _BV(UCSZ01);
	}
}

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

char uart_getchar(const uint8_t port)
{
	if (port) {
		loop_until_bit_is_set(UCSR1A, RXC1);
		return(UDR1);
	} else {
		loop_until_bit_is_set(UCSR0A, RXC0);
		return(UDR0);
	}
}

/*!
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
void uart_putchar(const uint8_t port, const char c)
{
  if (c == '\n')
    uart_putchar(port, '\r');

  if (port) {
	  loop_until_bit_is_set(UCSR1A, UDRE1);
	  UDR1 = c;
  } else {
	  loop_until_bit_is_set(UCSR0A, UDRE0);
	  UDR0 = c;
  }
}

/*!
 * Send a C (NUL-terminated) string down the UART Tx.
 */
void uart_printstr(const uint8_t port, const char *s)
{

  while (*s) {
/*
      if (*s == '\n')
	uart_putchar(port, '\r');
*/

      uart_putchar(port, *s++);
    }
}
