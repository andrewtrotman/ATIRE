/*
	ENCODE_CHAR_PRINTABLE_ASCII.H
	-----------------------------
*/
#ifndef ENCODE_CHAR_PRINTABLE_ASCII_H_
#define ENCODE_CHAR_PRINTABLE_ASCII_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_PRINTABLE_ASCII
	-------------------------------------
	Encode all printable ASCII characters as distinct codepoints (except uppercase letters,
 	provide lowercased input!), plus encode all Unicode characters as one single
	codepoint larger than ASCII.
*/
class ANT_encode_char_printable_ascii
{
public:
	static const unsigned int num_symbols = 70;

public:
	static unsigned char encode(unsigned char c)
	{
	if (c < ' ')
		return CHAR_ENCODE_FAIL;

	/* 
		Unicode folds on top of the delete character
	*/
	if (c > 0x7F)
		c = 0x7F;

	/*
		Eliminate the hole where uppercase characters used to be
	*/
	if (c > 'Z')
		c -= ('Z' - 'A' + 1);

	/*
		Eliminate control characters (everything below space)
	*/
	c -= ' ';

	return c;
	}
};

#endif /* ENCODE_CHAR_PRINTABLE_ASCII_H_ */
