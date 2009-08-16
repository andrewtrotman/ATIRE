/*
 * ENCODING_ASCII.H
 * ----------------
 *
 *  Created on: Jun 29, 2009
 *      Author: monfee
 */

#ifndef __ENCODING_ASCII_H__
#define __ENCODING_ASCII_H__

#include "encoding.h"

/*
	class ANT_ENCODING_ASCII
	------------------------
*/
class ANT_encoding_ascii : public ANT_encoding
{
public:
	ANT_encoding_ascii() : ANT_encoding() {}
	virtual ~ANT_encoding_ascii() {}

	virtual long is_valid_char(unsigned char *c) { return ANT_isalpha(*c); } // for English, a valid char will be an alphabet character
};

#endif __ENCODING_ASCII_H__
