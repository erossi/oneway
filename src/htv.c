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

/*! \file htv.c
  \brief Main struct of the project and eventually defaults.
  */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <util/crc16.h>
#include "htv.h"

uint8_t crc8_str(char *str)
{
	uint8_t i, crc8;

	crc8 = 0;

	for (i=0; i<strlen(str); i++)
		crc8 = _crc_ibutton_update(crc8, *(str + i));

	return(crc8);
}

void str_to_htv(struct htv_t *htv)
{
	/* Full address */
	strlcpy(htv->substr, htv->x10str, 5);
	htv->address = strtoul(htv->substr, 0, 16);
	/* high and low byte of the address */
	strlcpy(htv->substr, htv->x10str, 3);
	htv->haddr = strtoul(htv->substr, 0, 16);
	strlcpy(htv->substr, htv->x10str + 2, 3);
	htv->laddr = strtoul(htv->substr, 0, 16);
	/* pin code */
	strlcpy(htv->substr, htv->x10str + 4, 3);
	htv->pin = strtoul(htv->substr, 0, 16);
	/* cmd code */
	strlcpy(htv->substr, htv->x10str + 6, 2);
	htv->cmd = strtoul(htv->substr, 0, 16);
	/* crc16 */
	strlcpy(htv->substr, htv->x10str + 8, 3);
	htv->crc16 = strtoul(htv->substr, 0, 16);
}
