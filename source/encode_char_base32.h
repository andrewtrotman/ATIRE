/*
	ENCODE_CHAR_BASE32.H
	--------------------
*/
#ifndef ENCODE_CHAR_BASE32_H_
#define ENCODE_CHAR_BASE32_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_BASE32
	----------------------------
	Encode ASCII alphanumerics into an 5-bit number, with all punctuation merged to one point,
	and digits doubling up. All letters are distinct codepoints.
*/
class ANT_encode_char_base32
{
public:
	static const unsigned int num_symbols = 32;

	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;

	if (c >= '0' && c <= '9')
		return 1 + ((c - '0') >> 1); // Numbers sort second, but they will have to double-up to fit into 5 encodings

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 6; // Letters sort last

	return CHAR_ENCODE_FAIL;
	}
};

#endif /* ENCODE_CHAR_BASE32_H_ */
