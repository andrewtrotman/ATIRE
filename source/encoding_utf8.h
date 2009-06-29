/*
 * ENCODING_UTF8.H
 * ---------------
 *
 *  Created on: Jun 29, 2009
 *      Author: monfee
 */

#ifndef __ENCODING_UTF8_H__
#define __ENCODING_UTF8_H__

#include "encoding_ascii.h"

/*
	class ANT_ENCODING_UTF8
	-----------------------
*/
class ANT_encoding_utf8 : public ANT_encoding_ascii
{
public:
	ANT_encoding_utf8() : ANT_encoding_ascii() {}
	virtual ~ANT_encoding_utf8() {}

	int test_utf8char(unsigned char *c);			// return number of bytes occupied by the character to test if the input character is a valid utf8 character? could be anything including symbols and punctuation
	virtual long is_valid_char(unsigned char *c); 	// a valid char will be actual language dependent character for Chinese, a valid char will be an Chinese character rather than symbols or punctuation
	unsigned int to_codepoint(unsigned char *utf8_char);
	bool is_chinese_codepoint(unsigned int cp);
};

/*
	ANT_ENCODING_UTF8::IS_CHINESE_CODEPOINT()
	-----------------------------------------
*/
inline bool ANT_encoding_utf8::is_chinese_codepoint(unsigned int cp)
{
return ((cp >= 0x4e00 && cp <= 0x9fff)
	|| (cp >= 0x3400 && cp <= 0x4dbf)
	|| (cp >=0x20000 && cp <= 0x2a6df)
	|| (cp >=0xf900 && cp <= 0xfaff)
	|| (cp >=0x2f800 && cp <= 0x2fa1f) );
}

#endif __ENCODING_UTF8_H__