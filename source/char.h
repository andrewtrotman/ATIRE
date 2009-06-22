/*
 * char.h
 *
 *  Created on: Jun 22, 2009
 *      Author: monfee
 */

#ifndef __CHAR_H__
#define __CHAR_H__

#include "encoding.h"
#include "stdio.h"

class ANT_char {
private:
	ANT_encoding::language lang;
	ANT_encoding* enc;
	char* start;
	size_t bytes;

public:
	ANT_char(ANT_encoding* encoding) : enc(encoding) {}
	~ANT_char() {}


};

#endif /* __CHAR_H__ */
