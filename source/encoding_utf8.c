/*
	ENCODING_UTF8.C
	---------------
	Created on: Jun 20, 2009
	Author: monfee
*/

#include "encoding_utf8.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ENCODING_UTF8::TEST_UTF8CHAR()
	----------------------------------
	return number of bytes occupied by the character to test if the input character is a valid 
	utf8 character? Could be anything including symbols and punctuation
*/
size_t ANT_encoding_utf8::test_utf8char(unsigned char *c)
{
size_t number_of_bytes = 0;
unsigned char code = *c;
size_t byte;

if (code >= 0xC0 && code <= 0xDF)
	number_of_bytes = 2;
else if (code >= 0xE0 && code <= 0xEF)
	number_of_bytes = 3;
else if (code >= 0xF0 && code <= 0xF4)
	number_of_bytes = 4;
else if (code >= 0xF8 && code <= 0xFB)
	number_of_bytes = 5;
else if (code >= 0xFC && code <=0xFD)
	number_of_bytes = 6;

for (byte = 0; byte < number_of_bytes - 1; byte++)
	{
	c++;
	code = *c;
	if (code < 0x80 || code > 0xBF)
		{
		number_of_bytes = 0;
		break;
		}
	}
return number_of_bytes;
}

/*
	ANT_ENCODING_UTF8::IS_VALID_CHAR()
	----------------------------------
*/
long ANT_encoding_utf8::is_valid_char(unsigned char *character)
{
if ((*character & 0x80) == 0) // ASCII characters
	{
	if (ANT_isalpha(*character))
		current_lang = ENGLISH;
	else
		current_lang = ASCIICHAR;
	bytes = 1;

	return current_lang == ENGLISH;
	}

bytes = test_utf8char(character);
if (bytes > 0)
	if (is_chinese_codepoint(to_codepoint(character, bytes)))
		{
		current_lang = CHINESE;
		return TRUE;
		}
current_lang = UNKNOWN;

return FALSE;
}

/*
	ANT_ENCODING_UTF8::TO_CODEPOINT()
	---------------------------------
*/
unsigned long ANT_encoding_utf8::to_codepoint(unsigned char *utf8_char)
{
return to_codepoint(utf8_char, test_utf8char(utf8_char));
}

/*
	ANT_ENCODING_UTF8::TO_CODEPOINT()
	---------------------------------
*/
unsigned long ANT_encoding_utf8::to_codepoint(unsigned char *utf8_char, size_t num_of_bytes)
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
