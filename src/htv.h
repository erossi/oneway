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

#define MAX_CMD_LENGHT 20

/*! structure of the data packet */
struct htv_t {
	/*! full 16 bit address */
	uint16_t address;
	/*! high byte of the address */
	uint8_t haddr;
	/*! low byte of the address */
	uint8_t laddr;
	/*! pin code */
	uint8_t pin;
	/*! command */
	uint8_t cmd;
	/*! 1net crc16 from the host */
	uint16_t crc16;
	/*! crc8 to the client */
	uint8_t crc8;
	/*! x10 like string from the host */
	char *x10str;
	/*! string space used during conversion */
	char *substr;
};

uint8_t crc8_str(char *str);
void str_to_htv(struct htv_t *htv);

#endif
