/*
	READABILITY.H
	-------------
*/

#ifndef __READABILITY_H__
#define __READABILITY_H__

#include "parser.h"
#include "string_pair.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"

/* 
	class ANT_READABILITY
	---------------------
*/
class ANT_readability
{
public:
	ANT_readability() {};
	virtual ~ANT_readability() {};
	
	virtual ANT_string_pair *get_next_token(void) = 0;
	virtual void set_document(unsigned char *document) = 0;
	virtual long score(void) = 0;
	virtual void add_node(ANT_memory_index_hash_node *node) = 0;
	virtual void index(ANT_memory_index *index) = 0;
	
	void set_parser(ANT_parser *parser) { this->parser = parser; }

protected:
	ANT_parser *parser;
} ;

#endif __READABILITY_H__
