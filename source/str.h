/*
	STR.H
	-----
*/
#ifndef __STR_H__
#define __STR_H__

#include <string.h>
#include "ctypes.h"

#ifdef _MSC_VER
	#define strlwr(a) _strlwr(a)
#endif

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
inline char *strnnew(const char *str, size_t len)
{
char *new_str;
return (*((new_str = strncpy(new char [len + 1], str, len)) + len) = '\0'), new_str;
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
	STRREV()
	--------
*/
inline char *strrev(char *dest, const char *source)
{
char *into;
const char *from;
long len;

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



#ifndef _MSC_VER

/*
	STRLWR()
	--------
*/
inline char *strlwr(char *string)
{
unsigned char *ch;

for (ch = (unsigned char *)string; *ch != '\0'; ch++)
	*ch = ANT_tolower(*ch);

return string;
}

#endif


#endif __STR_H__
