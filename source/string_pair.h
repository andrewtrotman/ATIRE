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
	long length;

public:
	ANT_string_pair() {}
	~ANT_string_pair() {}

	char *str(void) { return strnnew(start, length); }
	long length(void) { return length; }
	long operator[](long pos) { return start[pos]; }
};


#endif __STRING_PAIR_H__
