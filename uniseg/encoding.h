/*
	ENCODING.H
	----------
	Created on: Jun 20, 2009
	Author: monfee
*/
#ifndef __ENCODING_H__
#define __ENCODING_H__

#include "stdio.h"

/*
	class UNISEG_ENCODING
	------------------
*/
class UNISEG_encoding
{
public:
	enum /*language */{ UNKNOWN = 0, NUMBER = 1, SPACE = 2, PUNCT = 4, ASCIICHAR = 8, ALPHA = 16, CHINESE = 32 }; // supported languages

protected:
	long current_lang;
	size_t bytes;

public:
	UNISEG_encoding() : current_lang(ALPHA), bytes(0) {}
	virtual ~UNISEG_encoding() {}

	virtual bool is_valid_char(const unsigned char *c) = 0;
	virtual void UNISEG_tolower(unsigned char *c) = 0;
	virtual void UNISEG_toupper(unsigned char *c) = 0;
	virtual void test_char(const unsigned char *c); // trying to find out the language of the character, and how many bytes it occupies

	size_t howmanybytes() { return bytes; }
	size_t howmanybytes(const unsigned char *c) { test_char(c); return bytes; }
	long lang() { return current_lang; }

	bool is_english() { return current_lang == ALPHA; }
};

inline void UNISEG_encoding::test_char(const unsigned char *c) { is_valid_char(c); }

#endif /* __ENCODING_H__ */
