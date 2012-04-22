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
