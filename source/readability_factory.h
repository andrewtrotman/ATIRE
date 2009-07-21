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
private:
	ANT_readability *measure;

public:
	enum {
		NONE       = 1,
		DALE_CHALL = 2
	};

	ANT_readability_factory();
	virtual ~ANT_readability_factory();

	ANT_string_pair *get_next_token(void);
	void set_document(unsigned char *document);
	void add_node(ANT_memory_index_hash_node *node);
	long score(void);
	void index(ANT_memory_index *index);

	void set_measure(unsigned long value);
	void set_parser(ANT_parser *parser);
};

#endif __READABILITY_FACTORY_H__
