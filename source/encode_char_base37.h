/*
	ENCODE_CHAR_BASE37.H
	--------------------
*/
#ifndef ENCODE_CHAR_BASE37_H_
#define ENCODE_CHAR_BASE37_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_BASE37
	----------------------------
	Encode a character as a base-37 digit (' ', 0-9, a-z), or CHAR_ENCODE_FAIL if
	there is no mapping for this character
*/
class ANT_encode_char_base37
{
public:
	static const unsigned int num_symbols = 37;

public:
	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;
	if (c >= '0' && c <= '9')
		{
		/*
			Digits sort second
		*/
		return c - '0' + 1;
		}
	else if (c >= 'a' && c <= 'z')
		{
		/*
			Letters last
		*/
		return c - 'a' + 1 + 10;
		}

	return CHAR_ENCODE_FAIL;
	}
};

#endif /* ENCODE_CHAR_BASE37_H_ */
