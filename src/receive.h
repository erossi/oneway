/* This file is part of HospiTV
 * Copyright (C) 2010, 2011 Enrico Rossi
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

/*! \file slave.h
  \brief Utility for rx.
  */

#ifndef RX_H
#define RX_H

#define AU_PORT PORTA
#define AU_DDR DDRA
#define AU_ENABLE PA5
#define AU_TXRX PA6

#include "led.h"
#include "uart.h"

void slave(void);

#endif
