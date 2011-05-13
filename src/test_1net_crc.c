#include <stdlib.h>
#include <stdio.h>
#define UInt16 unsigned int
#define UInt8 unsigned short
#define uint16_t unsigned short
#define uint8_t unsigned char

/*! bit by bit algorithm without augmented zero bytes.
  does not use lookup table, suited for polynom orders between 1...16.
 */

uint16_t mycrc(const uint8_t DATA, const uint16_t STARTING_CRC)
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

UInt16 one_net_compute_crc(UInt8 * const DATA, const UInt8 LEN, 
  const UInt16 STARTING_CRC, const UInt8 ORDER)
{
    // bit by bit algorithm without augmented zero bytes.
    // does not use lookup table, suited for polynom orders between 1...16.

    UInt16 polynom;

    UInt16 i, j, c, bit, mask;
    UInt16 crc = STARTING_CRC;
    UInt16 crc_high_bit = (UInt16)1 << (ORDER - 1);
    const UInt8 * p = DATA;

    if(!DATA)
    {
        return 0;
    } // parameter was invalid //

    switch(ORDER)
    {
        case 8:
        {
            polynom = 0x00A6;
            mask = 0x00FF;
            break;
        } // 8th order crc //

        default:
        {
            return 0;
            break;
        } // default //
    } // switch polynomial order //

    for(i = 0; i < LEN; i++)
    {
        c = (UInt16)*p++;

	printf("c hex %x\n",c);

        for(j = 0x80; j; j >>= 1)
        {
            bit = crc & crc_high_bit;
            crc <<= 1;

            if(c & j)
            {
                bit ^= crc_high_bit;
            } // if c & j //

            if(bit)
            {
                crc ^= polynom;
            } // if bit //
        } // for j //
    } // for LEN //

    return crc & mask;

} // one_net_compute_crc //



int main (void)
{

  UInt16 crc ;

  UInt8 p[]= { 
    0Xff,
    0Xdf,
    0Xcf,
    0X0e,
 } ;

  crc=one_net_compute_crc( p, 4,0XFF,8);

  printf("crc hex %x\n",crc);

  crc = mycrc(p[0], 0xff);
  crc = mycrc(p[1], crc);
  crc = mycrc(p[2], crc);
  crc = mycrc(p[3], crc);
  printf("mycrc hex %x\n",crc);

}
