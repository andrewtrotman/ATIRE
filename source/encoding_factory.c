/*
 * encoding_factory.c
 *
 *  Created on: Jun 22, 2009
 *      Author: monfee
 */

#include "encoding_factory.h"

ANT_encoding* ANT_encoding_factory::gen_encoding_scheme(encoding what_encoding)
{
	switch (what_encoding) {
	case ASCII:
		return new ANT_encoding_ascii();
	case UTF8:
	default:
		return new ANT_encoding_utf8();
	}
	return NULL;
}
