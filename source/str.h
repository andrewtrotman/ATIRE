/*
	STR.H
	-----
*/
#ifndef STR_H_
#define STR_H_

#include <string.h>
#include "ctypes.h"

#ifdef __APPLE__
#include <stdlib.h>
#endif

#ifdef _MSC_VER
	inline char *strlower(char *a) { return _strlwr(a); }
#endif

extern "C" int char_star_star_strcmp(const void *one, const void *two);

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
	return (char *)memcpy(new_str, str, (size_t)(old_length < new_length ? old_length : new_length));
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

#endif  /* STR_H_ */
