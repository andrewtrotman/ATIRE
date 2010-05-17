/*
	ENCODING_UTF8.C
	---------------
	Created on: Jun 20, 2009
	Author: monfee
*/

#include "encoding_utf8.h"

/*
	UNISEG_ENCODING_UTF8::TEST_UTF8CHAR()
	----------------------------------
*/
size_t UNISEG_encoding_utf8::test_utf8char(const unsigned char *c)
{
size_t num_of_bytes = 1;
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

for (int i = 0; i < num_of_bytes - 1; i++)
	{
	c++;
	code = *c;
	if (code < 0x80 || code > 0xBF)
		{
		num_of_bytes = 1;
		break;
		}
	}
return num_of_bytes;
}

/*
	UNISEG_ENCODING_UTF8::IS_VALID_CHAR()
	----------------------------------
*/
bool UNISEG_encoding_utf8::is_valid_char(const unsigned char *c)
{
if (!(*c & 0x80)) // ASCII characters
	{
	if (isalpha(*c))
		current_lang = ALPHA;
	else if (isspace(*c))
		current_lang = SPACE;
	else if (ispunct(*c))
		current_lang = PUNCT;
	else if (isdigit(*c))
		current_lang = NUMBER;
	else
		current_lang = ASCIICHAR;
	bytes = 1;
	return current_lang == ALPHA;
	}

bytes = test_utf8char(c);
if (bytes > 0)
	if (is_chinese_codepoint(to_codepoint(c, bytes)))
		{
		current_lang = CHINESE;
		return true;
		}
current_lang = UNKNOWN;

return false;
}

/*
	UNISEG_ENCODING_UTF8::TO_CODEPOINT()
	---------------------------------
*/
unsigned long UNISEG_encoding_utf8::to_codepoint(const unsigned char *utf8_char)
{
return to_codepoint(utf8_char, test_utf8char(utf8_char));
}

/*
	UNISEG_ENCODING_UTF8::TO_CODEPOINT()
	---------------------------------
*/
unsigned long UNISEG_encoding_utf8::to_codepoint(const unsigned char *utf8_char, size_t num_of_bytes)
{
// Identifier with _c suffix mean that it is a constant variable
const unsigned int max_bytes_c = 6;  // Maximum bytes of utf8 encoding for purpose of shifting into codepoint
const unsigned int min_c = 128;
const unsigned int max_c = 1;
const unsigned int max_rest_c = 63; // for verification of validity of UTF8 byte
unsigned int low_max = max_c;
unsigned int high_min = min_c;
int code = 0;

//int length = utf8_char.end() - utf8_char.begin();
if (num_of_bytes <= 6 && num_of_bytes > 0)
	{
	unsigned char b1 = utf8_char[0];

	if (num_of_bytes == 1)
		return b1;

	// deciding the first byte valid or not
	unsigned int i;
	for (i = 0; i < (num_of_bytes - 1); i++)
		high_min = (high_min >> 1) + min_c;

	for (i = 0; i < (7 - (num_of_bytes + 1)); i++)
		low_max = (low_max << 1) + max_c;

	if (b1 <= (high_min + low_max) && b1 >= high_min)
		{
		code = b1 - high_min;
		for (i = 1; i < num_of_bytes; i++)
			{
			unsigned char b = utf8_char[i];
			if (b < min_c || b > (min_c + max_rest_c))
				{
				code = 0;
				break;
				}
			code = code << max_bytes_c;
			code += (b - min_c);
			}

		}
	}
return code;
}
