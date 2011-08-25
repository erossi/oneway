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

/*! \file transmit.h
  \brief Utility for tx.
  */

#ifndef TX_H
#define TX_H
/*! the header of the packet to tx */
#define TX_HEAD "xxxxxx"
/*! id of the master modules, required if more than 1 master is
 * present.
 */
#define TX_ID "0"

#include "led.h"
#include "uart.h"
#include "debug.h"
#include "htv.h"

void master(struct debug_t *debug);

#endif
