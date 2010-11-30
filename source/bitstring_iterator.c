
/*
	BITSTRING_ITERATOR.C
	--------------------
*/
#include "bitstring_iterator.h"
#include "bitstring.h"

/*
	ANT_BITSTRING_ITERATOR::ANT_BITSTRING_ITERATOR()
	------------------------------------------------
*/
ANT_bitstring_iterator::ANT_bitstring_iterator(ANT_bitstring *string)
{
bitstring = string;
byte_pos = 0;
bit_pos = 0;
}

/*
	ANT_BITSTRING_ITERATOR::~ANT_BITSTRING_ITERATOR()
	-------------------------------------------------
*/
ANT_bitstring_iterator::~ANT_bitstring_iterator()
{
}

/*
	ANT_BITSTRING_ITERATOR::NEXT()
	------------------------------
*/
long ANT_bitstring_iterator::next(void)
{
long ans;

while (byte_pos < bitstring->bytes_long)
	{
	while (bit_pos < 8)
		{
		if (bitstring->bits[byte_pos] & 1 << bit_pos)
			{
			ans = byte_pos * 8 + bit_pos;
			bit_pos++;
			return ans;
			}
		bit_pos++;
		}
	bit_pos = 0;
	byte_pos++;
	}

return -1;
}

