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
#include <avr/io.h>

#include "htv.h"

struct htv_t *htv_init(struct htv_t *htv)
{
	htv = malloc(sizeof(struct htv_t));
	htv->substr = malloc(MAX_SUBSTR_LENGHT);
	htv->x10str = malloc(MAX_CMD_LENGHT);
	return(htv);
}

void htv_free(struct htv_t *htv)
{
	free(htv->x10str);
	free(htv->substr);
	free(htv);
}

/*! \brief return the crc8 of the string.
 *
 * \param str the string
 */
uint8_t crc8_str(const char *str)
{
	uint8_t i, crc8;

	crc8 = 0;

	for (i=0; i<strlen(str); i++)
		crc8 = _crc_ibutton_update(crc8, *(str + i));

	return(crc8);
}

/*! \brief convert a pre-formatted string to the struct htv.
 *
 * \note the string is stored in the struct itself.
 */
void aaaa_to_htv(struct htv_t *htv)
{
	/* Full address */
	strlcpy(htv->substr, htv->x10str, 5);
	htv->address = strtoul(htv->substr, 0, 16);
}

/*! \sameas aaaa_to_htv */
void s5_to_htv(struct htv_t *htv)
{
	aaaa_to_htv(htv);
	/* pin code */
	strlcpy(htv->substr, htv->x10str + 4, 2);
	htv->pin = strtoul(htv->substr, 0, 16);
}

/*! \sameas aaaa_to_htv */
void s7_to_htv(struct htv_t *htv)
{
	aaaa_to_htv(htv);
	/* pin code */
	strlcpy(htv->substr, htv->x10str + 4, 3);
	htv->pin = strtoul(htv->substr, 0, 16);
	/* cmd code */
	strlcpy(htv->substr, htv->x10str + 6, 2);
	htv->cmd = strtoul(htv->substr, 0, 16);
}

/*! \sameas aaaa_to_htv */
void s8_to_htv(struct htv_t *htv)
{
	s5_to_htv(htv);
	/* crc */
	strlcpy(htv->substr, htv->x10str + 6, 3);
	htv->crc = strtoul(htv->substr, 0, 16);
}

/*! \brief check the validity of the x10str command string.
 *
  A vaild command is in the form of:
  AAAAPPC:RR\n
  where
  AAAA: Client address in HEX (2 byte)
  PP:   pin number in HEX (1 byte)
  C: command: 2 On , 3 Off
  RR crc calculated before ":"

  if cmd is not correct clear the cmd string and return FALSE
  else if cmd is ok, modify cmd and keep only AAAAPPC.

 * \return true: string OK, false: error
 * \todo should return errno code.
 */
uint8_t htv_check_cmd(struct htv_t *htv)
{
	uint8_t err=0;
	uint8_t crc;

	switch (strlen(htv->x10str)) {
		/* simplified str without crc: AAAAP */
		case 5:
			s5_to_htv(htv);
			break;
		/* str without crc: AAAAPPC */
		case 7:
			s7_to_htv(htv);
			break;
		/* simplified str with crc: AAAAP:RR */
		case 8:
			/* check for ":" */
			if (*(htv->x10str + 5) != ':') {
				err |= _BV(2);
			} else {
				s8_to_htv(htv);
				*(htv->x10str + 5) = 0;
				crc = crc8_str(htv->x10str);

				/* crc error */
				if (crc != htv->crc)
					err |= _BV(3);
			}

			break;
		default:
			/* strlen error */
			err |= _BV(1);
	}

	if (err)
		*htv->x10str = 0;

	return (err);
}
