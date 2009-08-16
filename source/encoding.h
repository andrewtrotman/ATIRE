/*
	ENCODING.H
	----------
	Created on: Jun 20, 2009
	Author: monfee
*/
#ifndef __ENCODING_H__
#define __ENCODING_H__

#include "stdio.h"
#include "ctypes.h"

/*
	class ANT_ENCODING
	------------------
*/
class ANT_encoding
{
public:
	enum language { UNKNOWN, ASCIICHAR, ENGLISH, CHINESE }; // supported languages

protected:
	language current_lang;
	size_t bytes;

public:
	ANT_encoding() : current_lang(ENGLISH), bytes(0) {}
	virtual ~ANT_encoding() {}

	virtual long is_valid_char(unsigned char *c) = 0;
	virtual void test_char(unsigned char *c) { is_valid_char(c); } // trying to find out the language of the character, and how many bytes it occupies

	size_t howmanybytes() { return bytes; }
	language lang() { return current_lang; }

	long is_english() { return current_lang == ENGLISH; }
};

#endif __ENCODING_H__
