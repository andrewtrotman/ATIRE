/*
	STR.H
	-----
*/
#ifndef __STR_H__
#define __STR_H__

#include <string.h>

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


#endif __STR_H__
