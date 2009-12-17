/*
	ENCODING_FACTORY.C
	------------------
	Created on: Jun 22, 2009
	Author: monfee
*/

#include "encoding_factory.h"
#include "encoding_ascii.h"
#include "encoding_utf8.h"
#include "uniseg_settings.h"

//UNISEG_encoding *UNISEG_encoding_factory::enc = NULL;

UNISEG_encoding *UNISEG_encoding_factory::get_encoding()
{
    if (!enc_)
    	enc_ = gen_encoding_scheme((encoding)(UNISEG_settings::instance().encoding_scheme));
    return enc_;
    //return gen_encoding_scheme((encoding)(UNISEG_settings::instance().encoding_scheme));
}

/*
	UNISEG_ENCODING_FACTORY::GEN_ENCODING_SCHEME()
	-------------------------------------------
*/
UNISEG_encoding *UNISEG_encoding_factory::gen_encoding_scheme(encoding what_encoding)
{
UNISEG_encoding *a_enc;
switch (what_encoding)
	{
	case ASCII:
		a_enc = new UNISEG_encoding_ascii();
	case UTF8:		// fall through
	default:
		a_enc = new UNISEG_encoding_utf8();
	}
return a_enc;
}
