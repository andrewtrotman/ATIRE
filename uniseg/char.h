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

class UNISEG_char {
private:
	long lang;
	UNISEG_encoding* enc;
	char *start;
	size_t bytes;

public:
	UNISEG_char(UNISEG_encoding* encoding) : enc(encoding) {}
	~UNISEG_char() {}


};

#endif /* __CHAR_H__ */
