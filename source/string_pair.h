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
	char *string(void) { return start; }
	char operator[](long pos) { return start[pos]; }
	char *strcpy(char *dest) { *(strncpy(dest, start, string_length) + string_length + 1) = '\0'; return dest; }
	int strcmp(ANT_string_pair *with);
	int strncmp(ANT_string_pair *with, long length);
	int stricmp(ANT_string_pair *with);
	int strnicmp(ANT_string_pair *with, long length);
};


/*
	INT ANT_STRING_PAIR::STRCMP()
	-----------------------------
*/
inline int ANT_string_pair::strcmp(ANT_string_pair *with)
{
int cmp;

if (string_length == with->string_length)
	return ::strncmp(start, with->start, string_length);
else
	{
	cmp = ::strncmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
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
	cmp = ::strncmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

/*
	INT ANT_STRING_PAIR::STRICMP()
	------------------------------
*/
inline int ANT_string_pair::stricmp(ANT_string_pair *with)
{
int cmp;

if (string_length == with->string_length)
	return ::_strnicmp(start, with->start, string_length);
else
	{
	cmp = ::_strnicmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

/*
	INT ANT_STRING_PAIR::STRNICMP()
	-------------------------------
*/
inline int ANT_string_pair::strnicmp(ANT_string_pair *with, long len)
{
int cmp;

if (string_length >= len && with->string_length >= len)
	return ::_strnicmp(start, with->start, len);
else
	{			// at least one string_length must be less than len
	cmp = ::_strnicmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return string_length - with->string_length;
	else
		return cmp;
	}
}

#endif __STRING_PAIR_H__
