#include <stdlib.h>
#include <stdint.h>

/*! bit by bit algorithm without augmented zero bytes.
  does not use lookup table, suited for polynom orders between 1...16.
 */
uint16_t one_net_compute_crc(const uint8_t DATA, const uint16_t STARTING_CRC)
{
	uint16_t j, bit;
	uint16_t crc = STARTING_CRC;
	uint16_t crc_high_bit = (uint16_t)1 << 7;

	for (j = 0x80; j; j >>= 1) {
		bit = crc & crc_high_bit;
		crc <<= 1;

		if (DATA & j)
			bit ^= crc_high_bit;

		/* 8th order crc */
		if (bit)
			crc ^= 0x00a6;
	}

	return(crc & 0xff);
}
