/*
	UNICODE.H
	--------------
	Routines for working with UTF-8 text
*/

#ifndef UNICODE_H_
#define UNICODE_H_

#include <string.h>
#include <cassert>
#include "ctypes.h"
#include "unicode_tables.h"

#ifdef _MSC_VER
	#define inline __forceinline
#endif

#define LAST_ASCII_CHAR 0x7F


extern const char * ANT_UNICODE_chartype_string[7];

struct ANT_UNICODE_char_chartype
{
	long character;
	ANT_UNICODE_chartype type;
};

struct ANT_UNICODE_decomposition
{
	long source;
	char const * target;
};

unsigned char *utf8_tolower(unsigned char *here);
inline char *utf8_tolower(char *here) { return (char *) utf8_tolower((unsigned char *) here); }

ANT_UNICODE_chartype utf8_chartype(unsigned long character);

unsigned int ANT_UNICODE_decompose_markstrip_lowercase_toutf8(long character, char * buf, unsigned long buflen);

/*
	UTF8_BYTES()
	------------------------
	How many bytes would the given wide character require if encoded
	in UTF-8?
*/
inline long utf8_bytes(unsigned long c)
{
	if (c <=0x007F)
		return 1;
	if (c <=0x07FF)
		return 2;
	if (c <=0xFFFF)
		return 3;
	return 4;
}

/*
	UTF8_BYTES()
	------------------------
	How many bytes does the UTF8 character take?
*/
inline long utf8_bytes(unsigned char *here)
{
if (*here < 0x80)				// 1-byte (ASCII) character
	return 1;
else if ((*here & 0xE0) == 0xC0)		// 2-byte sequence
	return 2;
else if ((*here & 0xF0) == 0xE0)		// 3-byte sequence
	return 3;
else if ((*here & 0xF8) == 0xF0)		// 4-byte sequence
	return 4;
else
	return 1;		// dunno so make it 1
}

inline long utf8_bytes(char *here) { return utf8_bytes((unsigned char *)here); }

/*
	WIDE_TO_UTF8()
	------------------------
	Encode the given Unicode codepoint into a stream of UTF-8 characters in
	buf, where buf is at least buflen chars long.

	If there is not enough space in buf to encode the given character,
	nothing is written to buf and 0 is returned.

	Returns the number of characters written to buf.
*/
inline int wide_to_utf8(unsigned char * buf, unsigned int buflen, unsigned long c) {
	unsigned int numbytes = utf8_bytes(c);

	if (buflen < numbytes)
		return 0;

	switch (numbytes)
		{
		case 1:
			buf[0] = (unsigned char) (c);
			break;
		case 2:
			buf[0] = (unsigned char) (0xC0 | (c >> 6));
			buf[1] = (unsigned char) (0x80 | (c & 0x3F));
			break;
		case 3:
			buf[0] = (unsigned char) (0xE0 | (c >> 12));
			buf[1] = (unsigned char) (0x80 | ((c & 0xFC0) >> 6));
			buf[2] = (unsigned char) (0x80 | (c & 0x3F));
			break;
		case 4:
			buf[0] = (unsigned char) (0xF0 | (c >> 18));
			buf[1] = (unsigned char) (0x80 | ((c & 0x3F000) >> 12));
			buf[2] = (unsigned char) (0x80 | ((c & 0xFC0) >> 6));
			buf[3] = (unsigned char) (0x80 | (c & 0x3F));
			break;
		default:
			assert(0);
		}

	return numbytes;
}
inline int wide_to_utf8(char * buf, unsigned int buflen, unsigned long c) {
	return wide_to_utf8((unsigned char *) buf, buflen, c);
}

/*
	UTF8_TO_WIDE()
	--------------------------
	Convert a UTF8 sequence into a wide character
*/
inline unsigned long utf8_to_wide(unsigned char *here)
{
if (*here < 0x80)				// 1-byte (ASCII) character
	return *here;
else if ((*here & 0xE0) == 0xC0)	// 2-byte sequence
	return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
else if ((*here & 0xF0) == 0xE0)	// 3-byte sequence
	return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
if ((*here & 0xF8) == 0xF0)	// 4-byte sequence
	return ((*here & 0x03) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 1) & 0x3F);
return 0;
}

/*
	ISUTF8()
	--------------------
	if it is valid uft8 bytes
*/
inline int isutf8(unsigned char *here)
{
int number_of_bytes = utf8_bytes(here);
int i = 1;

for (; i < number_of_bytes; ++i)
	{
	++here;
	char c = (*here) >> 6;
	if (c != 2)
		return 0;
	}
return 1;
}
inline int isutf8(char *here) {
	return isutf8((unsigned char *) here);
}

#endif /* UNICODE_H_ */
