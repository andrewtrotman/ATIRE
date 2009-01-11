/*
	STR.H
	-----
*/
#ifndef __STR_H__
#define __STR_H__

#include <string.h>
#include <ctype.h>
//#include "ctypes.h"

/*
	STRNEW()
	--------
*/
inline static char *strnew(const char *str)
{
return strcpy(new char[strlen(str) + 1], str);
}

/*
	STRNNEW()
	---------
*/
inline static char *strnnew(const char *str, size_t len)
{
char *new_str;
return (*((new_str = strncpy(new char [len + 1], str, len)) + len) = '\0'), new_str;
}

/*
	STRLWR()
	--------
*/
inline char *strlwr(char *string)
{
unsigned char *ch;

for (ch = (unsigned char *)string; *ch != '\0'; ch++)
	*ch = NCBI_tolower[*ch];

return string;
}


/*
	STRIP_END_PUNC()
	----------------
*/
inline static char *strip_end_punc(char *buffer)
{
char *ch;

for (ch = buffer + strlen(buffer) - 1; ch > buffer; ch--)
	if (isspace(*ch))
		*ch = '\0';
	else
		break;

return buffer;
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
	*ch = NCBI_tolower[*ch];

return string;
}

#endif


#endif __STR_H__
