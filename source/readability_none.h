/*
	READABILITY_NONE.H
	------------------
*/
#ifndef __READABILITY_NONE_H__
#define __READABILITY_NONE_H__

#include "readability.h"

/*
	class ANT_READABILITY_NONE
	--------------------------
*/
class ANT_readability_none : public ANT_readability
{
public:
	ANT_readability_none() {};
	virtual ~ANT_readability_none() {};

	ANT_string_pair *get_next_token();
	void set_document(unsigned char *document);
	long score(void) { return 0; };
#pragma warning (suppress : 4100)
	void add_node(ANT_memory_index_hash_node *node) {};
#pragma warning (suppress : 4100)
	void index(ANT_memory_index *index) {};

	void set_parser(ANT_parser *parser);
} ;

#endif __READABILITY_NONE_H__
