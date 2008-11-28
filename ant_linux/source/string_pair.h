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
	char *str(void) { return strnnew(start, string_length); }
	long length(void) { return string_length; }
	char *string(void) { return start; }
	char operator[](long pos) { return start[pos]; }
	char *strcpy(char *dest) { *(strncpy(dest, start, string_length) + string_length + 1) = '\0'; return dest; }
	int strcmp(ANT_string_pair *with);
	int true_strcmp(ANT_string_pair *with);
	int strncmp(ANT_string_pair *with, long length);
};

/*
	INT ANT_STRING_PAIR::STRCMP()
	-----------------------------
*/
inline int ANT_string_pair::strcmp(ANT_string_pair *with)
{
return string_length == with->string_length ? memcmp(start, with->start, string_length) : string_length - with->string_length;
}

/*
	INT ANT_STRING_PAIR::TRUE_STRCMP()
	----------------------------------
*/
inline int ANT_string_pair::true_strcmp(ANT_string_pair *with)
{
int cmp;

if (string_length == with->string_length)
	return ::memcmp(start, with->start, string_length);
else
	{
	cmp = ::memcmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

/*
	INT ANT_STRING_PAIR::STRNCMP()
	-------------------------------
*/
inline int ANT_string_pair::strncmp(ANT_string_pair *with, long len)
{
int cmp;

if (string_length >= len && with->string_length >= len)
	return ::strncmp(start, with->start, len);
else
	{			// at least one string_length must be less than len
	cmp = ::memcmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

#endif /* __STRING_PAIR_H__ */
