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
#include <ctype.h>

/*
	class UNISEG_ENCODING_ASCII
	------------------------
*/
class UNISEG_encoding_ascii : public UNISEG_encoding
{
public:
	UNISEG_encoding_ascii() : UNISEG_encoding() {}
	virtual ~UNISEG_encoding_ascii() {}

	virtual bool is_valid_char(const unsigned char *c); // for English, a valid char will be an alphabet character

	virtual void UNISEG_tolower(unsigned char *c);
	virtual void UNISEG_toupper(unsigned char *c);
};

inline bool UNISEG_encoding_ascii::is_valid_char(const unsigned char *c) { return isalpha(*c); }

inline void UNISEG_encoding_ascii::UNISEG_tolower(unsigned char *c)
{
if ((*c) >= 'A' && (*c) <= 'Z')
    (*c) = tolower(*c);
}

inline void UNISEG_encoding_ascii::UNISEG_toupper(unsigned char *c)
{
if ((*c) >= 'a' && (*c) <= 'z')
    (*c) = toupper((*c));
}


#endif /* __ENCODING_ASCII_H__ */
