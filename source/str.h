/*
	STR.H
	-----
*/
#ifndef STR_H_
#define STR_H_

#include <string.h>
#include <stddef.h>
#include "ctypes.h"

#ifdef __APPLE__
#include <stdlib.h>
#endif

#ifdef _MSC_VER
	inline char *strlower(char *a) { return _strlwr(a); }
	inline char *strlower(const char *a) { return strlower((char *)a); }
	#define strnicmp _strnicmp
#else
	inline int strnicmp (const char *s1, const char *s2, size_t n) { return ::strncasecmp (s1, s2, n); }
#endif

extern "C" int char_star_star_strcmp(const void *one, const void *two);
extern "C" int char_star_star_star_strcmp(const void *one, const void *two);

inline unsigned char *strstr(const unsigned char *string, const char *key) { return (unsigned char *)::strstr((char *)string, key); }
inline unsigned char *strchr(const unsigned char *string, int key) { return (unsigned char *)::strchr((char *)string, key); }
inline size_t strlen(unsigned char *string) { return ::strlen((char *)string); }

/*
	STRNEW()
	--------
*/
inline char *strnew(const char *str)
{
return strcpy(new char[strlen(str) + 1], str);
}

/*
	STRNNEW()
	---------
*/
inline char *strnnew(const char *str, long long len)
{
char *new_str;

return (*((new_str = strncpy(new char [(size_t)(len + 1)], str, (size_t)len)) + len) = '\0'), new_str;
}

/*
	STRRENEW()
	----------
	realloc using new
*/
inline char *strrenew(const char *str, long long old_length, long long new_length)
{
char *new_str;

new_str = new char [(size_t)new_length];
if (str == NULL)
	return new_str;
else
	{
	memcpy(new_str, str, (size_t)(old_length < new_length ? old_length : new_length));
	delete [] str;
	return new_str;
	}
}

/*
	STRRENEW()
	----------
*/
inline unsigned char *strrenew(const unsigned char *str, long long old_length, long long new_length)
{ 
return (unsigned char *)strrenew((const char *)str, old_length, new_length);
}

/*
	STRIP_END_PUNC()
	----------------
*/
inline char *strip_end_punc(char *buffer)
{
char *ch;

for (ch = buffer + strlen(buffer) - 1; ch > buffer; ch--)
	if (ANT_isspace(*ch))
		*ch = '\0';
	else
		break;

return buffer;
}

/*
	STRIP_SPACE_INPLACE()
	---------------------
*/
inline char *strip_space_inplace(char *source)
{
char *end, *start = source;

while (ANT_isspace(*start))
	start++;

if (start > source)
	memmove(source, start, strlen(start) + 1);      // copy the '\0'

end = source + strlen(source) - 1;
while ((end >= source) && (ANT_isspace(*end)))
	*end-- = '\0';

return source;
}

/*
	STRIP_DUPLICATE_SPACE_INLINE()
	------------------------------
*/
inline char *strip_duplicate_space_inline(char *source)
{
char *start, *into;

start = into = source;
while (*start != '\0')
	{
	*into = *start++;
	if (ANT_isspace(*into))
		*into = ' ';				// replace all white space with ' ' (remove CF/LF)
	if (!(ANT_isspace(*into) && ANT_isspace(*start)))		// skip over multiple spaces
		into++;
	}
*into = '\0';

return source;
}

/*
	STRREV()
	--------
*/
inline char *strrev(char *dest, const char *source)
{
char *into;
const char *from;
size_t len;

len = strlen(source);
for (into = dest + len - 1, from = source; *from != '\0'; from++, into--)
	*into = *from;
dest[len] = '\0';

return dest;
}

/*
	STRREV()
	--------
*/
inline char *strrev(char *what)
{
char *from, *to, tmp;

from = what;
to = what + strlen(what) - 1;
while (from < to)
	{
	tmp = *from;
	*from = *to;
	*to = tmp;
	from++;
	to--;
	}
return what;
}

/*
	STRNREV()
	---------
	if the string exceeds max_length in length then only reverse the first max_len characters
*/
inline char *strnrev(char *dest, const char *source, size_t max_length)
{
char *into;
const char *from;
size_t len;

len = strlen(source);
if (len > max_length)
	len = max_length;
	
for (into = dest + len - 1, from = source; *from != '\0'; from++, into--)
	*into = *from;
dest[len] = '\0';

return dest;
}

/*
	STRCOUNTCHR()
	-------------
*/
inline int strcountchr(const char *token, char ch)
{
int out = 0;

while ((token = strchr(token, ch)) != NULL)
	{
	out++;
	token++;
	}

return out;
}

#ifndef _MSC_VER

	/*
		STRLOWER()
		----------
	*/
	inline char *strlower(char *string)
	{
	unsigned char *ch;

	for (ch = (unsigned char *)string; *ch != '\0'; ch++)
		*ch = ANT_tolower(*ch);

	return string;
	}

#define strupr strupper

	/*
		STRUPPER()
		----------
	*/
	inline char *strupper(char *string)
	{
	unsigned char *ch;

	for (ch = (unsigned char *)string; *ch != '\0'; ch++)
		*ch = ANT_toupper(*ch);

	return string;
	}

#define wcsupr wcstrupper

	inline wchar_t *wcstrupper(wchar_t *s) {
		wchar_t *p = s;

		while (*p) {
			*p = (wchar_t) ANT_toupper(*p);
			p++;
		}

		return s;
	}

#endif

#ifdef __APPLE__

	/*
		STRNLEN()
		---------
	*/
	inline size_t strnlen(char *string, size_t max) 
	{
	size_t length = 0;
	while (length < max && string[length] != '\0')
		length++;
	return length;
	}

#endif

/*
	ANT_ATOL()
	----------
*/
static inline long ANT_atol(char *string)
{
char *ch;
long ans = 0, multiplier;

if (*string == '-')
	{
	multiplier = -1;
	string++;
	}
else if (*string == '+')
	{
	multiplier = 1;
	string++;
	}
else
	multiplier = 1;

for (ch = string; *ch >= '0' && *ch <= '9'; ch++)
	ans = ans * 10 + (*ch - '0');

return multiplier * ans;
}

/*
	ANT_ATOUL()
	-----------
*/
static inline unsigned long ANT_atoul(char *string, size_t length)
{
char *ch;
unsigned long ans = 0;

for (ch = string; ((size_t)(ch - string) < length) && (*ch >= '0' && *ch <= '9'); ch++)
	ans = ans * 10 + (*ch - '0');

return ans;
}

/*
	ANT_ATOI64()
	------------
*/
static inline long long ANT_atoi64(char *string)
{
char *ch;
long long ans = 0, multiplier;

if (*string == '-')
	{
	multiplier = -1;
	string++;
	}
else if (*string == '+')
	{
	multiplier = 1;
	string++;
	}
else
	multiplier = 1;

for (ch = string; *ch >= '0' && *ch <= '9'; ch++)
	ans = ans * 10 + (*ch - '0');

return multiplier * ans;
}


/*
	MEMMEM()
	--------
*/
static inline char *memmem(char *mem, size_t mem_size, char *sub, size_t sub_size)
{    
char *ret = NULL;
char *ptr = mem;

while (ptr != NULL && ret == NULL)
	if ((ptr = (char *)memchr(ptr, *sub, mem_size - (sub_size - 1) - (ptr - mem))) != NULL)
		{
		if (memcmp(ptr, sub, sub_size) == 0)
			ret = ptr;
		++ptr;
		}

return ret;
}

/*
	MEMSTR()
	--------
*/
static inline char *memstr(char *buffer, char *string, size_t buffer_length)
{
return memmem(buffer, buffer_length, string, strlen(string));
}


#endif  /* STR_H_ */
