/*
	MEMORY_INDEXER.H
	----------------
*/
#ifndef MEMORY_INDEXER_H_
#define MEMORY_INDEXER_H_

#include "pragma.h"
#include "memory_indexer_node.h"
#include "string_pair.h"

#pragma ANT_PRAGMA_UNUSED_PARAMETER_DISABLE

class ANT_ranking_function_factory;

/*
	class ANT_MEMORY_INDEXER
	------------------------
*/
class ANT_memory_indexer
{
public:
	enum { MODE_ABSOLUTE, MODE_MONOTONIC };

protected:
	ANT_string_pair *squiggle_length;

public:
	ANT_memory_indexer() { squiggle_length = new ANT_string_pair("~length"); }
	virtual ~ANT_memory_indexer() { delete squiggle_length; }

	virtual ANT_memory_indexer_node *add_term(ANT_string_pair *string, long long docno, unsigned char extra_term_frequency = 1) = 0;
	virtual void set_document_length(long long docno, long long length) = 0;
	virtual void set_document_detail(ANT_string_pair *measure_name, long long length, long mode = MODE_ABSOLUTE) = 0;
	virtual long long get_memory_usage(void) = 0;
} ;


#endif /* MEMORY_INDEXER_H_ */
