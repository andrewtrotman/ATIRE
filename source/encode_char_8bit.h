/*
	ENCODE_CHAR_8BIT.H
	------------------
*/
#ifndef ENCODE_CHAR_8BIT_H_
#define ENCODE_CHAR_8BIT_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_8BIT
	--------------------------
	Null-encoding, just passses 8-bit chars straight through.
*/
class ANT_encode_char_8bit
{
public:
	static const unsigned int num_symbols = 256;

public:
	static unsigned char encode(unsigned char c) { return c; }
};

#endif /* ENCODE_CHAR_8BIT_H_ */
