/*
	STRING_PAIR.H
	-------------
*/
#ifndef STRING_PAIR_H_
#define STRING_PAIR_H_

#include <stdio.h>
#include "string_pair_constant.h"
#include "pragma.h"
#include "memory.h"
#include "str.h"

/*
	class ANT_STRING_PAIR
	---------------------
*/
class ANT_string_pair : public ANT_string_pair_constant
{
public:
	ANT_string_pair()                                 { }
	ANT_string_pair(char *name)                       { start = name; string_length = ::strlen(name); }
	ANT_string_pair(char *source, long len)           { start = source; string_length = len; }
	ANT_string_pair(ANT_string_pair_constant *source) { start = source->start; string_length = source->string_length; }

	ANT_string_pair &operator=(ANT_string_pair_constant &source) { start = source.start; string_length = source.string_length; return *this; }

#pragma ANT_PRAGMA_NO_DELETE
	void *operator new (size_t count, ANT_memory *memory) { return memory->malloc(count); }
	void *operator new (size_t count)                     { return (char *)malloc(count); }
	unsigned char operator[](long pos)                    { return (unsigned char)start[pos]; }
	void operator delete(void *mem) { free(mem); }

	size_t length(void) { return string_length; }
	char *string(void)  { return start; }

	char *str(void) { return strnnew(start, string_length); }
	char *strcpy(char *dest) { *(::strncpy(dest, start, string_length) + string_length) = '\0'; return dest; }
	char *strncpy(char *dest, size_t length) { *(::strncpy(dest, start, string_length < length ? string_length : length) + (string_length < length ? string_length : length)) = '\0'; return dest; }
	int strcmp(ANT_string_pair *with) { return string_length == with->string_length ? ::memcmp(start, with->start, string_length) : string_length < with->string_length ? -1 : 1; }

	int true_strcmp(const char *string);
	int true_strcmp(ANT_string_pair *with);
	int true_strncmp(ANT_string_pair *with, size_t length);

	ANT_string_pair *strlower(void);

	void text_render(void) { printf("%*.*s", (int)string_length, (int)string_length, start); }
};

/*
	ANT_STRING_PAIR::TRUE_STRCMP()
	------------------------------
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
		return string_length < with->string_length ? -1 : 1;		// cannot be 0 as we already checked for that.
	else
		return cmp;
	}
}

/*
	ANT_STRING_PAIR::TRUE_STRCMP()
	------------------------------
*/
inline int ANT_string_pair::true_strcmp(const char *with)
{
int cmp;
size_t len;

cmp = ::strncmp(start, with, string_length);
if (cmp == 0)
	{
	len = strlen(with);
	return len == string_length ? 0 : len > string_length ? -1 : 1;
	}
else
	return cmp;
}

/*
	ANT_STRING_PAIR::TRUE_STRNCMP()
	-------------------------------
*/
inline int ANT_string_pair::true_strncmp(ANT_string_pair *with, size_t len)
{
int cmp;

if (string_length >= len && with->string_length >= len)
	return ::strncmp(start, with->start, len);
else
	{			// at least one string_length must be less than len
	cmp = ::memcmp(start, with->start, string_length < with->string_length ? string_length : with->string_length);
	if (cmp == 0)
		return with->string_length > string_length  ? -1 : 1;
	else
		return cmp;
	}
}

/*
	ANT_STRING_PAIR::STRLOWER()
	---------------------------
*/
inline ANT_string_pair *ANT_string_pair::strlower(void)
{
size_t current;

for (current = 0; current < string_length; current++)
	start[current] = ANT_tolower(start[current]);

return this;
}


#endif  /* STRING_PAIR_H_ */
