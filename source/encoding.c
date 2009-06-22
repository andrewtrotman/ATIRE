/*
 * encoding.c
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#include "encoding.h"

bool ANT_encoding_utf8::is_valid_char(unsigned char* c)
{
	return ANT_isalpha(*c);
}
