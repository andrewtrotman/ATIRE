/*
	READABILITY_FACTORY.H
	---------------------
*/
#ifndef __READABILITY_FACTORY_H__
#define __READABILITY_FACTORY_H__

#include "parser.h"
#include "readability.h"
#include "readability_factory_measure.h"

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

private:
	static long number_of_measures;
	static ANT_readability_factory_measure measure[];
	unsigned long measures_to_use;
	ANT_parser *parser;

public:	
	ANT_readability_factory() { measures_to_use = 0; }
	virtual ~ANT_readability_factory() {}
	
	ANT_string_pair *get_next_token(void);
	void set_document(unsigned char *document);
	void handle_node(ANT_memory_index_hash_node *node);
	void index(ANT_memory_index *index);
	
	void set_measure(unsigned long value);
	void set_parser(ANT_parser *parser);
};

#endif __READABILITY_FACTORY_H__
