/*
	STRING_PAIR.H
	-------------
*/
#ifndef __STRING_PAIR_H__
#define __STRING_PAIR_H__

#include "str.h"

class ANT_string_pair
{
public:
	char *start;
	long string_length;

public:
	ANT_string_pair() {}
	~ANT_string_pair() {}

	char *str(void) { return strnnew(start, string_length); }
	long length(void) { return string_length; }
	char operator[](long pos) { return start[pos]; }
	char *strcpy(char *dest) { *(strncpy(dest, start, string_length) + string_length + 1) = '\0'; return dest; }
	int strcmp(ANT_string_pair *with);
};


/*
	INT ANT_STRING_PAIR::STRCMP()
	-----------------------------
*/
inline int ANT_string_pair::strcmp(ANT_string_pair *with)
{
int cmp;

if (string_length == with->string_length)
	return strncmp(start, with->start, string_length);
else
	{
	cmp = ::strncmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

#endif __STRING_PAIR_H__
