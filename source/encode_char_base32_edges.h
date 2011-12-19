/*
	ENCODE_CHAR_BASE32_EDGES.H
	--------------------------
*/
#ifndef ENCODE_CHAR_BASE32_EDGES_H_
#define ENCODE_CHAR_BASE32_EDGES_H_

#include "encode_char.h"

/*
	class ANT_ENCODE_CHAR_BASE32_EDGES
	----------------------------------
	Encode ASCII alphanumerics into an 5-bit number, with all punctuation merged to one point,
	and digits doubling up. All letters are distinct codepoints.
*/
class ANT_encode_char_base32_edges
{
public:
	static const unsigned int num_symbols = 32;

public:
	static unsigned char encode(unsigned char c)
	{
	if (c >= '0' && c <= '9')
		return 1 + ((c - '0') >> 1); // Numbers sort second, but they will have to double-up to fit into 5 encodings

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 5; // Letters sort last

	if (c <= 0x7F)
		return 0; // Punctuation sorts first

	// We'll sort Unicode along with Z
	return (unsigned char) (num_symbols - 1);
	}
};

#endif /* ENCODE_CHAR_BASE32_EDGES_H_ */
