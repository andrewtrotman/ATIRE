/*
	UNICODE.C
	---------
*/

#include "unicode.h"
#include "unicode_tables.h"
#include <stdio.h>
#include <stdlib.h>

/*
	UNICODE_CHARTYPE_ASCII_TABLE[]
	------------------------------
	Given an ASCII character (ch < 0x80), return the char type by looking up in this table
*/
ANT_UNICODE_chartype unicode_chartype_ASCII_table[] =
{
CT_ZERO, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_SEPARATOR, CT_SEPARATOR, CT_SEPARATOR, CT_SEPARATOR, CT_SEPARATOR, CT_OTHER, CT_OTHER, 
CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, CT_OTHER, 
CT_SEPARATOR, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, 
CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_NUMBER, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, 
CT_PUNCTUATION, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, 
CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, 
CT_PUNCTUATION, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, 
CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_LETTER, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_PUNCTUATION, CT_OTHER
};

/*
	UNICODE_XML_CLASS()
	-------------------
	Classify the given Unicode character according to the XML spec,
	returning a set of the classes it belongs to.
*/
int unicode_xml_class(unsigned long character)
{
//Use ANT classification tables if possible
if (character <= LAST_ASCII_CHAR)
	{
	if (ANT_isXMLnamestartchar(character))
		return XMLCC_NAME_START | XMLCC_NAME;
	if (ANT_isXMLnamechar(character))
		return XMLCC_NAME;
	return 0;
	}

//Definition from XML-spec is here http://www.w3.org/TR/xml/#NT-NameStartChar

//Is it a name-start character? (name chars are a superset of this)
if (character >= 0xC0 && character <= 0xD6 ||
	character >= 0xD8 && character <= 0xF6 ||
	character >= 0xF8 && character <= 0x2FF ||
	character >= 0x370 && character <= 0x37D ||
	character >= 0x37F && character <= 0x1FFF ||
	character >= 0x200C && character <= 0x200D ||
	character >= 0x2070 && character <= 0x218F ||
	character >= 0x2C00 && character <= 0x2FEF ||
	character >= 0x3001 && character <= 0xD7FF ||
	character >= 0xF900 && character <= 0xFDCF ||
	character >= 0xFDF0 && character <= 0xFFFD ||
	character >= 0x10000 && character <= 0xEFFFF)
	return XMLCC_NAME_START | XMLCC_NAME;

//Just a name char?
if (character == 0xB7 ||
	character >= 0x0300 && character <= 0x036F ||
	character >= 0x203F && character <= 0x2040)
	return XMLCC_NAME;

return 0;
}

/*
	UTF8_TOLOWER()
	--------------
	Convert a Unicode character to lowercase in a new buffer. Pass a pointer to
	the pointer to dest so it can be incremented to advance past the character converted,
	and a pointer to destlen so it can be reduced.

	If destlen is too small, this will silently truncate the string.

	Returns true if the character was modified.

	The result is not null-terminated.
*/
int utf8_tolower(unsigned char **dest, size_t *destlen, unsigned long origchar)
{
unsigned long lowerchar = ANT_UNICODE_tolower(origchar);

int num_dest_bytes = wide_to_utf8(*dest, *destlen, lowerchar);
*destlen -= num_dest_bytes;
*dest += num_dest_bytes;

/* num_dest_bytes is zero if the destination buffer was too small */
return (origchar != lowerchar) && num_dest_bytes;
}

/*
	UTF8_TOLOWER()
	--------------
	Convert a UTF-8 character to lowercase in a new buffer. Pass pointers to
	the pointers to dest and src so they can be incremented to advance past
	the character converted, and to destlen so it can be reduced.

	If destlen is too small, this will silently truncate the destination string.

	Returns true if the character was modified.

	The result is not null-terminated.
*/
int utf8_tolower(unsigned char **dest, size_t *destlen, const unsigned char **src)
{
int success;

if ((**src & 0x80) == 0) //ASCII
	{
	unsigned char origchar = **src;
	unsigned char lowerchar = ANT_tolower(origchar);

	(*src)++;
	if (*destlen)
		{
		**dest = lowerchar;
		(*dest)++;
		(*destlen)--;
		success = 1;
		}
	else
		success = 0;

	/* Did this actually result in the character changing? */
	return success && lowerchar != origchar;
	}
else
	{
	unsigned long num_src_bytes = utf8_bytes(*src);
	unsigned long origchar = utf8_to_wide(*src);

	success = utf8_tolower(dest, destlen, origchar);

	*src += num_src_bytes;

	return success;
	}
}

/*
	UTF8_TOLOWER()
	--------------
	To convert both ASCII and UTF-8 characters to lowercase in-place.

	Returns a pointer to the character after the one processed.
*/
unsigned char *utf8_tolower(unsigned char *here)
{
unsigned long number_of_bytes;
unsigned long origcase, lowercase;

if ((*here & 0x80) == 0) //ASCII
	{
	*here = ANT_tolower(*here);
	return here + 1;
	}
else
	{
	number_of_bytes = utf8_bytes(here);

	origcase = utf8_to_wide(here);
	lowercase = ANT_UNICODE_tolower(origcase);

	if (origcase != lowercase && utf8_bytes(lowercase) == number_of_bytes)
		{
		/* Don't lower`case this character if the lowercase character
		 * doesn't require the same amount of bytes to encode as the
		 * original - that would be incredibly awkward.
		 */
		assert(wide_to_utf8(here, number_of_bytes, lowercase) == number_of_bytes);
		}

	return here + number_of_bytes;
	}
}

/*
	UTF8_ISUPPER()
	--------------
	Check if this character is uppercase by checking if it has a lowercase variant.
*/
int utf8_isupper(unsigned long character)
{
return (unsigned long)ANT_UNICODE_tolower(character) != character;
}

int utf8_isupper(long character)
{
return ANT_UNICODE_tolower(character) != character;
}

/*
	ANT_UNICODE_NORMALIZE_LOWERCASE_TOUTF8()
	----------------------------------------
	Decompose the given Unicode character into its constituent parts (i.e. separate
	characters into their base form + a combining mark character), then throw away
	the combining marks and convert the character to lowercase.

	Writes the result as a UTF-8 string into the buffer buf with length buflen. The
	string is not null-terminated.

	Updates buf to point past the characters written, and reduces buflen by the number
	of bytes written.

	Returns true if successful, 0 otherwise	(e.g. buflen too small).
*/
int ANT_UNICODE_normalize_lowercase_toutf8(unsigned char **buf, size_t *buflen, uint32_t character)
{
size_t num_dest_chars;
const char * decomposition;

if (!*buflen)
	return 0;

if (character <= LAST_ASCII_CHAR)
	{
	**buf = ANT_tolower((unsigned char)character);

	*buf = *buf + 1;
	*buflen = *buflen - 1;

	return 1;
	}

decomposition = ANT_UNICODE_search_decomposition(character);

if (!decomposition)
	{
	/* This character decomposes to itself. */
	num_dest_chars = wide_to_utf8(*buf, *buflen, character);

	*buf = *buf + num_dest_chars;
	*buflen = *buflen - num_dest_chars;

	return num_dest_chars > 0; //Zero if wide_to_utf8 ran out of room
	}
else
	{
	num_dest_chars = strlen(decomposition);

	if (num_dest_chars > *buflen)
		return 0;

	memcpy(*buf, decomposition, num_dest_chars * sizeof(*decomposition));

	*buf = *buf + num_dest_chars;
	*buflen = *buflen - num_dest_chars;

	return 1;
	}
}

/*
	ANT_UNICODE_NORMALIZE_STRING()
	------------------------------


*/
int ANT_UNICODE_normalize_string_tolowercase(unsigned char *buffer, size_t buffer_length_const, size_t *normalized_string_length, unsigned char *source)
{
size_t buffer_length;
unsigned char *buffer_pos;
unsigned char *source_end;

unsigned char *current;
long bytes;

unsigned long character;

current = source;

//normalize the string into this buffer
buffer_pos = buffer;
buffer_length = buffer_length_const;

int result = 0;

source_end = source + strlen((char *)source);
*normalized_string_length = 0;

while (current < source_end)
	{
	character = utf8_to_wide(current);
	bytes = utf8_bytes(character);

	if (bytes < 1)
		break;

	if (unicode_chartype(character) == CT_LETTER)
		ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);
	else
		{
		memcpy(buffer_pos, current, bytes);
		buffer_pos += bytes;
		}

	current += bytes;
	}

*normalized_string_length = buffer_pos - buffer;

result = (*normalized_string_length) <= buffer_length_const;

if (result)
	buffer[*normalized_string_length] = '\0';

return result;
}
/*
	UNICODE_CHARTYPE_SET()
	----------------------
	Classify the given Unicode character, including flags like CT_CHINESE
	for characters which are also Chinese.
*/
unsigned char unicode_chartype_set(unsigned long character)
{
//Use ASCII table if possible
if (character <= LAST_ASCII_CHAR)
	{
	int c = (int)character;

	if (ANT_isalpha(c))
		return CT_LETTER;
	if (ANT_isdigit(c))
		return CT_NUMBER;
	if (ANT_ispunct(c))
		return CT_PUNCTUATION;
	if (ANT_isspace(c))
		return CT_SEPARATOR;

	return CT_OTHER;
	}

return ANT_UNICODE_search_chartype(character);
}

/*
	UNICODE_CHARTYPE_UTF8()
	-----------------------
	Given a pointer to a UTF-8 string, convert into a wide and return: it, its length (in bytes), and its chartype
	We know, when we get into this routine, that the sequence must be more than one-byte long.  For the version that
	handles one-byte sequences too use: unicode_chartype_utf8
*/
ANT_UNICODE_chartype unicode_chartype_utf8_multibye(unsigned char *current, unsigned long *character, long *bytes)
{
if ((*current & 0xE0) == 0xC0)
	{
	*bytes = 2;
	if (current[1] >> 6 == 2)
		*character = ((*current & 0x1F) << 6) | (*(current + 1) & 0x3F);
	else
		*character = 0;
	}
else if ((*current & 0xF0) == 0xE0)
	{
	*bytes = 3;
	if ((current[1] >> 6 == 2) && (current[2] >> 6 == 2))
		*character = ((*current & 0x0F) << 12) | ((*(current + 1) & 0x3F) << 6) | (*(current + 2) & 0x3F);
	else
		*character = 0;
	}
else if ((*current & 0xF8) == 0xF0)
	{
	*bytes = 4;
	if ((current[1] >> 6 == 2) && (current[2] >> 6 == 2) && (current[3] >> 6 == 2))
		*character = ((*current & 0x07) << 18) | ((*(current + 1) & 0x3F) << 12) | ((*(current + 2) & 0x3F) << 6) | (*(current + 3) & 0x3F);
	else
		*character = 0;
	}
else
	{
	*bytes = 8;
	*character = 0;
	return CT_OTHER;
	}

return (ANT_UNICODE_chartype)(ANT_UNICODE_search_chartype(*character) & ~CT_CHINESE);
}

/*
	UNICODE_CHARTYPE()
	------------------
	Classify the given Unicode character.
*/
ANT_UNICODE_chartype unicode_chartype(unsigned long character)
{
//Use ASCII table if possible
if (character <= LAST_ASCII_CHAR)
	{
	int c = (int)character;

	if (ANT_isalpha(c))
		return CT_LETTER;
	if (ANT_isdigit(c))
		return CT_NUMBER;
	if (ANT_ispunct(c))
		return CT_PUNCTUATION;
	if (ANT_isspace(c))
		return CT_SEPARATOR;

	return CT_OTHER;
	}

//Caller not interested in flags, strip them out so we get a nice pure enum
return (ANT_UNICODE_chartype)(ANT_UNICODE_search_chartype(character) & ~CT_CHINESE);
}

/*
	UTF8_TO_WIDE_SAFE()
	-------------------
	Convert a UTF8 sequence into a wide character.

	If the source buffer ends before the UTF-8 character is completed, zero is returned instead.
*/
unsigned long utf8_to_wide_safe(const unsigned char *here)
{
int numchars = utf8_bytes(here);

if (!isutf8(here))
	return 0;

switch (numchars)
	{
	case 1:
		return *here;
	case 2:
		//here[0] is known to be non-zero (since utf8_bytes(here) returned >1), so it can't be the terminator
		return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
	case 3:
		if (here[1])
			return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
		else
			return 0;
	case 4:
		if (here[1] && here[2])
			return ((*here & 0x07) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 3) & 0x3F);
		else
			return 0;
	default:
		return 0;
	}
}

/*
	UTF8_TO_WIDE_UNSAFE()
	---------------------
	Compared to the _safe version, this version can read up to 3 bytes past the null-terminator of the string at here.
*/
unsigned long utf8_to_wide_unsafe(const unsigned char *here)
{
int numchars = utf8_bytes(here);

if (!isutf8(here))
	return 0;

switch (numchars)
	{
	case 1:
		return *here;
	case 2:
		return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
	case 3:
		return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
	case 4:
		return ((*here & 0x03) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 3) & 0x3F);
	default:
		return 0;
	}
}

/*
	WIDE_TO_UTF8()
	--------------
	Encode the given Unicode codepoint into a stream of UTF-8 characters in
	buf, where buf is at least buflen chars long.

	If there is not enough space in buf to encode the given character,
	nothing is written to buf and 0 is returned.

	Returns the number of characters written to buf.
*/
unsigned long wide_to_utf8(unsigned char *buf, size_t buflen, unsigned long c)
{
unsigned long numbytes = utf8_bytes(c);

if (buflen < numbytes)
	return 0;

switch (numbytes)
	{
	case 1:
		buf[0] = (unsigned char)(c);
		break;
	case 2:
		buf[0] = (unsigned char)(0xC0 | (c >> 6));
		buf[1] = (unsigned char)(0x80 | (c & 0x3F));
		break;
	case 3:
		buf[0] = (unsigned char)(0xE0 | (c >> 12));
		buf[1] = (unsigned char)(0x80 | ((c & 0xFC0) >> 6));
		buf[2] = (unsigned char)(0x80 | (c & 0x3F));
		break;
	case 4:
		buf[0] = (unsigned char)(0xF0 | (c >> 18));
		buf[1] = (unsigned char)(0x80 | ((c & 0x3F000) >> 12));
		buf[2] = (unsigned char)(0x80 | ((c & 0xFC0) >> 6));
		buf[3] = (unsigned char)(0x80 | (c & 0x3F));
		break;
	default:
		assert(0);
	}

return numbytes;
}

/*
	UTF8_BYTES()
	------------
	How many bytes does the UTF8 character take?
	The spec allows 5/6 byte sequences, but they are deemed invalid,
	so along with rubbish input we return the most that possibly
	could be taken, 8
	
	Our conversion functions will take care of the invalid cases
*/
unsigned long utf8_bytes(const unsigned char *here)
{
if (*here < 0x80)				// 1-byte (ASCII) character
	return 1;
else if ((*here & 0xE0) == 0xC0)		// 2-byte sequence
	return 2;
else if ((*here & 0xF0) == 0xE0)		// 3-byte sequence
	return 3;
else if ((*here & 0xF8) == 0xF0)		// 4-byte sequence
	return 4;
return 8;
}

/*
	UTF8_BYTES()
	------------
	How many bytes would the given wide character require if encoded
	in UTF-8?
*/
unsigned long utf8_bytes(unsigned long c)
{
if (c <= 0x007F)
	return 1;
if (c <= 0x07FF)
	return 2;
if (c <= 0xFFFF)
	return 3;
if (c <= 0x1FFFFF)
	return 4;
return 8;
}

/*
	ISUTF8()
	--------
	if it is valid uft8 bytes
*/
int isutf8(const unsigned char *here)
{
int number_of_bytes = utf8_bytes(here);
int i;

/*
	While the utf-8 spec allows for 5 or 6 byte sequences, they are invalid
	we might also get a sequence we don't know that we're interpreting as 8 ... which is also invalid
*/
if (number_of_bytes > 4)
	return 0;

for (i = 1; i < number_of_bytes; ++i)
	{
	++here;
	char c = (*here) >> 6;
	if (c != 2)
		return 0;
	}
return 1;
}

/*
	ISCHINESE()
	-----------
	Is the given character from the Chinese CodePoint?
*/
int ischinese(unsigned long character)
{
return
	character >= 0x03400 &&
	 ((character <= 0x04dbf)								// CJK Unified Ideographs Extension A
	 || (character >= 0x04e00 && character <= 0x09fff)		// CJK Unified Ideographs
	 || (character >= 0x0f900 && character <= 0x0faff)		// CJK Compatibility Ideographs
	 || (character >= 0x20000 && character <= 0x2a6df)		// CJK Unified Ideographs Extension B
	 || (character >= 0x2f800 && character <= 0x2fa1f));	// CJK Compatibility Ideographs Supplement
}

/*
	ISKOREAN()
	-----------
	Is the given character from the Korean CodePoint?
*/
int iskorean(unsigned long character)
{
return
	character >= 0x01100 &&
	((character <= 0x011ff)									// Hangul Jamo
	 || (character >= 0x0ac00 && character <= 0x0d7a3)		// Hangul Syllables
	 || (character >= 0x03130 && character <= 0x0318f)		// Hangul Compatibility Jamo
	 || (character >= 0x0a960 && character <= 0x0a97f)		// Hangul Jamo Extended-A
	 || (character >= 0x0d7b0 && character <= 0x0d7ff));	// Hangul Jamo Extended-B
}

/*
	ISJAPANESE()
	-----------
	Is the given character from the Japanese CodePoint?
*/
int isjapanese(unsigned long character)
{
return
	character >= 0x03040 &&
	 ((character <= 0x0309f)								// Hiragana
	 || (character >= 0x030a0 && character <= 0x030ff));	// Katakana
}

/*
	IS_CJK_LANGUAGE()
	----------------
	Is the given character from the Chinese, Korean Japanese CodePoint?
*/
int is_cjk_language(unsigned long character)
{
return ischinese(character) || iskorean(character) || isjapanese(character);
}

/*
	UTF8_ISPUNCTUATION()
	-----------
	Is the given character from the punctuation CodePoint?
	http://www.unicode.org/charts/PDF/U2000.pdf
*/
int utf8_ispuntuation(unsigned long character)
{
return character >= 0x02000 && character <= 0x0206f;		// General Punctuation, U+206A - U+206F are deprecated
}
