/*
 * encoding.c
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 *
 *
 */

#include "encoding.h"

int ANT_encoding_utf8::test_utf8char(unsigned char* c)
{
	int num_of_bytes = 0;

	unsigned char code = *c;

	if (code >= 0xFC && code <=0xFD)
		num_of_bytes = 6;
	else if (code >= 0xF8 && code <= 0xFB)
		num_of_bytes = 5;
	else if (code >= 0xF0 && code <= 0xF4)
		num_of_bytes = 4;
	else if (code >= 0xE0 && code <= 0xEF)
		num_of_bytes = 3;
	else if (code >= 0xC0 && code <= 0xDF)
		num_of_bytes = 2;

	for (int i = 0; i< num_of_bytes - 1; i++) {
		++c;
		code = *c;
		if (code < 0x80 || code > 0xBF) {
			num_of_bytes = 0;
			break;
		}
	}
	return num_of_bytes;
}

bool ANT_encoding_utf8::is_valid_char(unsigned char* c)
{
	if ((0 <= *c && *c <= 0x7F))
		return ANT_isalpha(*c);

	int bytes = test_utf8char(c);
	if (bytes > 0) {
		unsigned int codepoint = to_codepoint(c);

		if (is_chinese_codepoint(c))
			return true;
	}
	return false;
}

unsigned int to_codepoint(unsigned char* utf8_char)
{
	const unsigned int br_c = 6;  // maximun bytes of utf8 encoding
	const unsigned int min_c = 128;
	const unsigned int max_c = 1;
	const unsigned int max_rest = 63;
	unsigned int low_max = max_c;
	unsigned int high_min = min_c;
	unsigned int code = 0;

	int length = utf8_char.end() - utf8_char.begin();
	if (length <= 6 && length > 0) {
		unsigned char b1 = utf8_char[0];

		if (utf8_char.length() == 1)
			return b1;

		// deciding the first byte valid or not
		unsigned int i;
		for (i = 0; i < (utf8_char.length() - 1); i++) {
			high_min = (high_min >> 1) + min_c;
		}

		for (i = 0; i < (7 - (utf8_char.length() + 1)); i++) {
			low_max = (low_max << 1) + max_c;
		}

		if (b1 <= (high_min + low_max) && b1 >= high_min) {
			code = b1 - high_min;

			for (i = 1; i < utf8_char.length(); i++) {

				unsigned char b = utf8_char[i];
				if (b < min_c || b > (min_c + max_rest)) {
					code = -2;
					break;
				}

				code = code << br_c;
				code += (b - min_c);
			}

		}
	}
	// TODO if code = -1, throw a exception say this is not a valid utf8 char
	return code;
}
