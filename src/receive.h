/* This file is part of OneWay
 * Copyright (C) 2010, 2011 Enrico Rossi
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

/*! \file receive.h
  \brief Utility for rx.
  */

#ifndef RX_H
#define RX_H

#include "led.h"
#include "uart.h"
#include "debug.h"
#include "htv.h"

/*! port where the IO pin are connected in the rx module. */
#define IO_PORT PORTA
/*! data direction register */
#define IO_DDR DDRA
/*
#define IO_PIN0 PA0
#define IO_PIN1 PA1
*/
/* chipcard test, test performed on a different circuit with 
 * different IO PIN */
/*! pin 0 */
#define IO_PIN0 PA4
/*! pin 1 */
#define IO_PIN1 PA6

void slave(struct debug_t *debug);

#endif
