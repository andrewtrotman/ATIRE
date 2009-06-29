/*
	ENCODING_FACTORY.C
	------------------
	Created on: Jun 22, 2009
	Author: monfee
*/

#include "encoding_factory.h"
#include "encoding_ascii.h"
#include "encoding_utf8.h"

/*
	ANT_ENCODING_FACTORY::GEN_ENCODING_SCHEME()
	-------------------------------------------
*/
ANT_encoding *ANT_encoding_factory::gen_encoding_scheme(encoding what_encoding)
{
switch (what_encoding)
	{
	case ASCII:
		return new ANT_encoding_ascii();
	case UTF8:		// fall through
	default:
		return new ANT_encoding_utf8();
	}
}
