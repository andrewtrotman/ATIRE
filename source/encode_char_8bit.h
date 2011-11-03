#include "encode_char.h"

#ifndef ENCODE_CHAR_8BIT_H_
#define ENCODE_CHAR_8BIT_H_

/*
	Null-encoding, just passses 8-bit chars straight through.
*/
class ANT_encode_char_8bit
{
public:
	static const unsigned int num_symbols = 256;

	static unsigned char encode(unsigned char c)
	{
	return c;
	}
};

#endif
