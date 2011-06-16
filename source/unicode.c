/*
	UNICODE.C
	--------------
*/

#include "unicode.h"
#include "unicode_tables.h"

/*
	UNICODE_XML_CLASS()
	---------------------
	Classify the given Unicode character according to the XML spec,
	returning a set of the classes it belongs to.
*/
int unicode_xml_class(unsigned long character)
{
//Use ANT classification tables if possible
if (character<=LAST_ASCII_CHAR)
	{
	if (ANT_isXMLnamestartchar(character))
		return XMLCC_NAME_START | XMLCC_NAME;
	if (ANT_isXMLnamechar(character))
		return XMLCC_NAME;
	return 0;
	}

//Definition from XML-spec is here http://www.w3.org/TR/xml/#NT-NameStartChar

//Is it a name-start character? (name chars are a superset of this)
if (character>=0xC0 && character<=0xD6 ||
	character>=0xD8 && character<=0xF6 ||
	character>=0xF8 && character<=0x2FF ||
	character>=0x370 && character<=0x37D ||
	character>=0x37F && character<=0x1FFF ||
	character>=0x200C && character<=0x200D ||
	character>=0x2070 && character<=0x218F ||
	character>=0x2C00 && character<=0x2FEF ||
	character>=0x3001 && character<=0xD7FF ||
	character>=0xF900 && character<=0xFDCF ||
	character>=0xFDF0 && character<=0xFFFD ||
	character>=0x10000 && character<=0xEFFFF)
	return XMLCC_NAME_START | XMLCC_NAME;

//Just a name char?
if (character==0xB7 ||
	character>=0x0300 && character<=0x036F ||
	character>=0x203F && character<=0x2040)
	return XMLCC_NAME;

return 0;
}

/*
	UTF8_TOLOWER()
	---------------------
	Convert a Unicode character to lowercase in a new buffer. Pass a pointer to
	the pointer to dest so it can be incremented to advance past the character converted,
	and a pointer to destlen so it can be reduced.

	If destlen is too small, this will silently truncate the string.

	Returns true if the character was modified.

	The result is not null-terminated.
*/
int utf8_tolower(unsigned char ** dest, size_t * destlen, unsigned long origchar)
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
	---------------------
	Convert a UTF-8 character to lowercase in a new buffer. Pass pointers to
	the pointers to dest and src so they can be incremented to advance past
	the character converted, and to destlen so it can be reduced.

	If destlen is too small, this will silently truncate the destination string.

	Returns true if the character was modified.

	The result is not null-terminated.
*/
int utf8_tolower(unsigned char ** dest, size_t * destlen, unsigned char **src)
{
int success;

if ((**src & 0x80) == 0) //ASCII
	{
	unsigned char origchar = **src;
	unsigned char lowerchar = ANT_tolower(origchar);

	*src++;
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
	---------------------
	To convert both ASCII and UTF-8 characters to lowercase in-place.

	Returns a pointer to the character after the one processed.
*/
unsigned char *utf8_tolower(unsigned char *here)
{
long number_of_bytes;
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
	return ANT_UNICODE_tolower(character) != character;
}

/*
	ANT_UNICODE_NORMALIZE_LOWERCASE_TOUTF8()
	---------------------
	Decompose the given Unicode character into its constituent parts (i.e. separate
	characters into their base form + a combining mark character), then throw away
	the combining marks and convert the character to lowercase.

	Writes the result as a UTF-8 string into the buffer buf with length buflen. The
	string is not null-terminated.

	Updates buf to point past the characters written, and reduces buflen by the number
	of bytes written.

	Returns true if successful, 0 otherwise	(e.g. buflen too small).
*/
int ANT_UNICODE_normalize_lowercase_toutf8(unsigned char ** buf, size_t * buflen, unsigned long character)
{
size_t num_dest_chars;
const char * decomposition;

if (!*buflen)
	return 0;

if (character <= LAST_ASCII_CHAR)
	{
	**buf = ANT_tolower((unsigned char) character);
	
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

	return num_dest_chars; //Zero if wide_to_utf8 ran out of room
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
	UNICODE_CHARTYPE_SET()
	---------------------
	Classify the given Unicode character, including flags like CT_CHINESE
	for characters which are also Chinese.
*/
unsigned char unicode_chartype_set(unsigned long character)
{
	//Use ASCII table if possible
	if (character <= LAST_ASCII_CHAR)
		{
		int c = (int) character;

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
	UNICODE_CHARTYPE()
	---------------------
	Classify the given Unicode character.
*/
ANT_UNICODE_chartype unicode_chartype(unsigned long character)
{
	//Use ASCII table if possible
	if (character <= LAST_ASCII_CHAR)
		{
		int c = (int) character;

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
	return (ANT_UNICODE_chartype) (ANT_UNICODE_search_chartype(character) & ~CT_CHINESE);
}
