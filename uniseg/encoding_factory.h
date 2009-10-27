/*
	ENCODING_FACTORY.H
	------------------
	Created on: Jun 22, 2009
	Author: monfee
*/
#ifndef __ENCODING_FACTORY_H__
#define __ENCODING_FACTORY_H__

#include "encoding.h"

/*
	class UNISEG_ENCODING_FACTORY
	--------------------------
*/
class UNISEG_encoding_factory
{
public:
	enum encoding { ASCII, UTF8 };

private:
	UNISEG_encoding *enc_;

public:
	UNISEG_encoding_factory() :enc_(NULL) {}
	virtual ~UNISEG_encoding_factory() { if (enc_) delete enc_; }
	UNISEG_encoding *gen_encoding_scheme(encoding what_encoding);
	UNISEG_encoding *get_encoding();
	static UNISEG_encoding_factory& instance();
} ;

inline UNISEG_encoding_factory& UNISEG_encoding_factory::instance()
{
	static UNISEG_encoding_factory inst;
	return inst;
}

#endif /* __ENCODING_FACTORY_H__ */

