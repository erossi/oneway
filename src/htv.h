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

/*! \file htv.h
  \brief Main struct of the project and eventually defaults.
  */

#ifndef HTV_H
#define HTV_H

#include "1net_crc.h"

#define MAX_CMD_LENGHT 20
#define MAX_SUBSTR_LENGHT 10

/*#define HTV_USE_RTX */
#define AU_PORT PORTA
#define AU_DDR DDRA
#define AU_ENABLE PA5
#define AU_TXRX PA6

/*! structure of the data packet */
struct htv_t {
	/*! full 16 bit address */
	uint16_t address;
	/*! high byte of the address, used only in 1net crc */
	uint8_t haddr;
	/*! low byte of the address, used only in 1net crc */
	uint8_t laddr;
	/*! pin code */
	uint8_t pin;
	/*! command */
	uint8_t cmd;
	/*! crc */
	uint8_t crc;
	/*! x10 like string from the host */
	char *x10str;
	/*! string space used during conversion */
	char *substr;
	/*! eeprom stored rx address */
	uint16_t ee_addr;
};

struct htv_t *htv_init(struct htv_t *htv);
void htv_free(struct htv_t *htv);
uint8_t crc8_str(const char *str);
void str_to_htv(struct htv_t *htv);
uint8_t htv_check_cmd(struct htv_t *htv, const uint8_t crctype);

#endif
