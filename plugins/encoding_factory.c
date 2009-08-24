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
	UNISEG_ENCODING_FACTORY::GEN_ENCODING_SCHEME()
	-------------------------------------------
*/
UNISEG_encoding *UNISEG_encoding_factory::gen_encoding_scheme(encoding what_encoding)
{
switch (what_encoding)
	{
	case ASCII:
		return new UNISEG_encoding_ascii();
	case UTF8:		// fall through
	default:
		return new UNISEG_encoding_utf8();
	}
}
