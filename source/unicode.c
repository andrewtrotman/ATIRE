/*
	UNICODE.C
	--------------
*/

#include "unicode.h"

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
