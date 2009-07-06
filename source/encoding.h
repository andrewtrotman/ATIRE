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

	virtual bool is_valid_char(unsigned char *c) = 0;
	virtual void tolower(unsigned char *c) = 0;
	virtual void toupper(unsigned char *c) = 0;

	size_t howmanybytes() { return bytes; }
	language lang() { return current_lang; }

	bool is_english() { return current_lang == ENGLISH; }
};

#endif __ENCODING_H__
