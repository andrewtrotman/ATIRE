/*
	READABILITY_FACTORY.H
	---------------------
*/
#ifndef __READABILITY_FACTORY_H__
#define __READABILITY_FACTORY_H__

#include "readability.h"

/*
	class ANT_READABILITY_FACTORY
	-----------------------------
*/
class ANT_readability_factory : public ANT_readability
{
public:
	enum {
		NONE       = 1,
		DALE_CHALL = 2
	};
	
	ANT_readability_factory() {};
	virtual ~ANT_readability_factory() {};

	ANT_string_pair *get_next_token();
	void set_document(unsigned char *);
	long score();

	void set_measure(unsigned long);
	void set_parser(ANT_parser *);

private:
	ANT_readability *measure;
};

#endif __READABILITY_FACTORY_H__
