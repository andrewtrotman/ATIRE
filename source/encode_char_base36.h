/*
	ENCODE_CHAR_BASE36.H
	--------------------
*/
#ifndef ENCODE_CHAR_BASE36_H_
#define ENCODE_CHAR_BASE36_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_BASE36
	----------------------------
	Encode a character as a base-36 digit (0-9, a-z), or 255 if there is no
	mapping for this character
*/
class ANT_encode_char_base36
{
public:
	static const unsigned int num_symbols = 36;

public:
	static unsigned char encode(unsigned char c)
	{
	if (c >= '0' && c <= '9')
		{
		/*
			Digits sort first
		*/
		return c - '0';
		}
	else if (c >= 'a' && c <= 'z')
		{
		/*
			Letters last
		*/
		return c - 'a' + 10;
		}
	return CHAR_ENCODE_FAIL;
	}
};

#endif /* ENCODE_CHAR_BASE36_H_ */
