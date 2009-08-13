/*
	READABILITY_FACTORY.H
	---------------------
*/
#ifndef __READABILITY_FACTORY_H__
#define __READABILITY_FACTORY_H__

#include "parser.h"
#include "readability.h"

/*
	class ANT_READABILITY_FACTORY
	-----------------------------
*/
class ANT_readability_factory : public ANT_readability
{
public:
	enum {
		NONE       = 0,
		DALE_CHALL = 1
	};

private:
	long number_of_measures;
	ANT_readability **measure;
	unsigned long measures_to_use;
	ANT_parser *parser;

public:
	ANT_readability_factory();
	virtual ~ANT_readability_factory();
	
	ANT_string_pair *get_next_token(void);
	void handle_node(ANT_memory_index_hash_node *node);
	void index(ANT_memory_index *index);
	
	void set_document(unsigned char *document);
	void set_measure(unsigned long value);
	void set_parser(ANT_parser *parser);
};


#endif __READABILITY_FACTORY_H__
