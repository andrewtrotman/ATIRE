/*
 * encoding_factory.h
 *
 *  Created on: Jun 22, 2009
 *      Author: monfee
 */

#ifndef __ENCODING_FACTORY_H__
#define __ENCODING_FACTORY_H__

#include "encoding.h"

class ANT_encoding_factory {
public:
	enum encoding { ASCII, UTF8 };

	static ANT_encoding* gen_encoding_scheme(encoding what_encoding);
};

#endif /* __ENCODING_FACTORY_H__ */
