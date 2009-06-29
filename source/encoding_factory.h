/*
	ENCODING_FACTORY.H
	------------------
	Created on: Jun 22, 2009
	Author: monfee
*/
#ifndef __ENCODING_FACTORY_H__
#define __ENCODING_FACTORY_H__

class ANT_encoding;

/*
	class ANT_ENCODING_FACTORY
	--------------------------
*/
class ANT_encoding_factory
{
public:
	enum encoding { ASCII, UTF8 };

public:
	static ANT_encoding *gen_encoding_scheme(encoding what_encoding);
} ;

#endif __ENCODING_FACTORY_H__


