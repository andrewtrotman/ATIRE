/*
	READABILITY.H
	-------------
*/

#ifndef __READABILITY_H__
#define __READABILITY_H__

#include "string_pair.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"

/* 
	class ANT_READABILITY
	---------------------
*/
class ANT_readability
{
friend class ANT_readability_factory;

public:
	ANT_readability() {};
	virtual ~ANT_readability() {};
	
	virtual void set_document(unsigned char *document) {};
	virtual void index(ANT_memory_index *index) = 0;

protected:
	ANT_string_pair *measure_name;
	
	virtual long score(void) { return 0; };
	virtual void handle_token(ANT_string_pair *token) {};
	virtual void handle_node(ANT_memory_index_hash_node *node) {};
} ;

#endif __READABILITY_H__
