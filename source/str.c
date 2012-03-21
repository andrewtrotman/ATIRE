/*
	STR.C
	-----
*/
#include "str.h"

/*
	CHAR_STAR_STAR_STRCMP()
	-----------------------
*/
extern "C" int char_star_star_strcmp(const void *one, const void *two)
{
return strcmp(*(char **)one, *(char **)two);
}

/*
	CHAR_STAR_STAR_STAR_STRCMP()
	----------------------------
*/
extern "C" int char_star_star_star_strcmp(const void *one, const void *two)
{
return strcmp(**(char ***)one, **(char ***)two);
}

/*
	ANT_TURN_BINARY_INTO_ASCII()
	----------------------------
*/
char *ANT_turn_binary_into_ascii(char  *source, long long length)
{
char *current;
char *end = source + length;

for (current = source; current < end; current++)
	{
	if (*current & 0x80)
		*current = ' ';

	if (*current == '\0')
		*current = ' ';
	}

return source;
}

