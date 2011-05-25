/*
	UNICODE.C
	--------------
*/

#include "unicode.h"
#include "unicode_tables.h"

const char * ANT_UNICODE_chartype_string[] = {"CT_INVALID", "CT_LETTER", "CT_NUMBER", "CT_PUNCTUATION", "CT_SEPARATOR", "CT_OTHER", "CT_MARK"};

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
	ANT_UNICODE_DECOMPOSE_MARKSTRIP_LOWERCASE_TOUTF8()
	---------------------
	Decompose the given Unicode character into its constituent parts (e.g. separate
	characters into their base form + a combining mark character), then throw away
	the combining marks and convert the character to lowercase.

	Writes the result as a UTF-8 string into the buffer buf with length buflen.

	Returns number of characters written to buf if successful, 0 otherwise
	(e.g. buflen too small).
*/
unsigned int ANT_UNICODE_decompose_markstrip_lowercase_toutf8(long character, char * buf, unsigned long buflen)
{
const char * decomposition = ANT_UNICODE_search_decomposition(character);

if (!decomposition)
	{
	/* This character decomposes to itself and so doesn't have an entry in
	 * the decomposition table. Convert it manually. */
	return wide_to_utf8(buf, buflen, ANT_UNICODE_tolower(character));
	}
else
	{
	unsigned int len = strlen(decomposition);

	if (len > buflen)
		return 0;

	memcpy(buf, decomposition, len * sizeof(*decomposition));

	return len;
	}
}

/*
	UTF8_CHARTYPE()
	---------------------
	Classify the given Unicode character.

	Doesn't properly classify most alphas (returns CT_OTHER) for those.
*/
ANT_UNICODE_chartype utf8_chartype(unsigned long character)
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

	ANT_UNICODE_chartype type = ANT_UNICODE_search_chartype(character);

	//Alphas aren't in that table, so check for those
	if (type == CT_OTHER)
		{
		/*
		 * This is ugly as shit. Abusing lowercase/uppercase tables to decide if it
		 * is alpha. Only works for European-ish languages.
		 */
		if (character != ANT_UNICODE_toupper(character) || character != ANT_UNICODE_tolower(character))
			return CT_LETTER;
		}

	return type;
}
