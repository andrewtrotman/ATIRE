/*
	COMPRESS_FOUR_INTEGER_VARIABLE_BYTE.C
	------------------
	Four Integer Variable Byte, an implementation of "Google's group ints by four and compress" scheme.
	Style note: 'Four' used instead of '4' due to variable naming restrictions.

	See http://static.googleusercontent.com/media/research.google.com/en//people/jeff/WSDM09-keynote.pdf
	Unfortunately slide numbers are not available. See around 75 % of the way through, under "Group Varint Encoding".

	Author: Blake Burgess
	License: BSD
*/

#include <stdio.h>
#include "compress_four_integer_variable_byte.h"
#include "maths.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_COMPRESS_FOUR_INTEGER_VARIABLE_BYTE::COMPRESS()
	--------------------------------
*/
long long ANT_compress_four_integer_variable_byte::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long long pos, bytes_used;
unsigned char *header_byte;
long num_to_pack, header_shift;

bytes_used = 0;
pos = 0;
while (pos < source_integers)
	{
	num_to_pack = (pos + 4 < source_integers) ? 4 : source_integers - pos;
	header_shift = 2 * (4 - num_to_pack); // We need to MSB-align the header byte at the end
	pos += num_to_pack;
	bytes_used++;
	header_byte = destination++;
	*header_byte = 0;
	while (num_to_pack--)
		{
		if (*source <= 0xFF)
			{
			*destination++ = *source & 0xFF;
			bytes_used += 1;
			}
		else if (*source <= 0xFFFF)
			{
			*destination++ = (*source >> 8) & 0xFF;
			*destination++ = *source & 0xFF;
			*header_byte |= 1 << (2 * num_to_pack);
			bytes_used += 2;
			}
		else if (*source <= 0xFFFFFF)
			{
			*destination++ = (*source >> 16) & 0xFF;
			*destination++ = (*source >> 8) & 0xFF;
			*destination++ = *source & 0xFF;
			*header_byte |= 2 << (2 * num_to_pack);
			bytes_used += 3;
			}
		else
			{
			*destination++ = (*source >> 24) & 0xFF;
			*destination++ = (*source >> 16) & 0xFF;
			*destination++ = (*source >> 8) & 0xFF;
			*destination++ = *source;
			*header_byte |= 3 << (2 * num_to_pack);
			bytes_used += 4;
			}
		source++;
		}
	*header_byte <<= header_shift;
	}

return bytes_used;
}

/*
	ANT_COMPRESS_FOUR_INTEGER_VARIABLE_BYTE::DECOMPRESS()
	-----------------------------------------------------
*/
void ANT_compress_four_integer_variable_byte::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
ANT_compressable_integer *end = destination + destination_integers;
while (destination < end)
	{
	switch(*source++)
		{
	case 0:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 1:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 2:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 3:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 4:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 5:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 6:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 7:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 8:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 9:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 10:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 11:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 12:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 13:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 14:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 15:
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 16:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 17:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 18:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 19:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 20:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 21:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 22:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 23:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 24:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 25:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 26:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 27:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 28:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 29:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 30:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 31:
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 32:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 33:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 34:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 35:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 36:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 37:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 38:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 39:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 40:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 41:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 42:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 43:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 44:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 45:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 46:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 47:
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 48:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 49:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 50:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 51:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 52:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 53:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 54:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 55:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 56:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 57:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 58:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 59:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 60:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 61:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 62:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 63:
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 64:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 65:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 66:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 67:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 68:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 69:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 70:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 71:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 72:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 73:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 74:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 75:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 76:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 77:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 78:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 79:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 80:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 81:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 82:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 83:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 84:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 85:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 86:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 87:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 88:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 89:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 90:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 91:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 92:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 93:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 94:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 95:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 96:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 97:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 98:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 99:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 100:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 101:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 102:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 103:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 104:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 105:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 106:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 107:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 108:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 109:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 110:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 111:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 112:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 113:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 114:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 115:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 116:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 117:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 118:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 119:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 120:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 121:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 122:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 123:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 124:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 125:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 126:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 127:
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 128:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 129:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 130:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 131:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 132:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 133:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 134:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 135:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 136:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 137:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 138:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 139:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 140:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 141:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 142:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 143:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 144:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 145:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 146:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 147:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 148:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 149:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 150:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 151:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 152:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 153:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 154:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 155:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 156:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 157:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 158:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 159:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 160:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 161:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 162:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 163:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 164:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 165:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 166:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 167:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 168:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 169:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 170:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 171:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 172:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 173:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 174:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 175:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 176:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 177:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 178:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 179:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 180:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 181:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 182:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 183:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 184:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 185:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 186:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 187:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 188:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 189:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 190:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 191:
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 192:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 193:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 194:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 195:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 196:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 197:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 198:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 199:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 200:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 201:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 202:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 203:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 204:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 205:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 206:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 207:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 208:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 209:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 210:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 211:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 212:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 213:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 214:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 215:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 216:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 217:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 218:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 219:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 220:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 221:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 222:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 223:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 224:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 225:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 226:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 227:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 228:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 229:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 230:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 231:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 232:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 233:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 234:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 235:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 236:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 237:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 238:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 239:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 240:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination++ = *source++;
		break;
	case 241:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 242:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 243:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 244:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 245:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 246:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 247:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 248:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 249:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 250:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 251:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 252:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination++ = *source++;
		break;
	case 253:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 8;
		*destination++ |= *source++;
		break;
	case 254:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
	case 255:
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		*destination = *source++ << 24;
		*destination |= *source++ << 16;
		*destination |= *source++ << 8;
		*destination++ |= *source++;
		break;
		}
	}
}
