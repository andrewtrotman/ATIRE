/*
	ENCODE_CHAR_BASE40.H
	--------------------
*/
#ifndef ENCODE_CHAR_BASE40_H_
#define ENCODE_CHAR_BASE40_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_BASE40
	----------------------------
	Encode a character as a base-40 digit (' ', punctuation between ' ' and '0' as one char, 0-9,
	punctuation between '9' and 'z' as one char, a-z, punctuation after 'z' as one char).
*/
class ANT_encode_char_base40
{
public:
	static const unsigned int num_symbols = 40;

public:
	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;

	if (c < '0')
		return 1;

	if (c >= '0' && c <= '9')
		return c - '0' + 2;

	if (c < 'a')
		return 2 + 10;

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 2 + 10 + 1;

	if (c > 'z')
		return 2 + 10 + 1 + 26;

	return CHAR_ENCODE_FAIL;
	}
};

#endif /* ENCODE_CHAR_BASE40_H_ */
