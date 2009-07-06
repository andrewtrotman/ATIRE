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

	virtual bool is_valid_char(unsigned char *c); // for English, a valid char will be an alphabet character

	virtual void tolower(unsigned char *c);
	virtual void toupper(unsigned char *c);
};

inline bool ANT_encoding_ascii::is_valid_char(unsigned char *c) { return ANT_isalpha(*c); }
inline void ANT_encoding_ascii::tolower(unsigned char *c) { if ((*c) >= 'A' && (*c) <= 'Z') *c = ANT_tolower(*c); }
inline void ANT_encoding_ascii::toupper(unsigned char *c) { if ((*c) >= 'a' && (*c) <= 'z') *c = ANT_toupper(*c); }


#endif __ENCODING_ASCII_H__
