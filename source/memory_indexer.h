/*
	MEMORY_INDEXER.H
	----------------
*/
#ifndef MEMORY_INDEXER_H_
#define MEMORY_INDEXER_H_

#include "pragma.h"
#include "memory_indexer_node.h"

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

public:
	ANT_memory_indexer() {}
	virtual ~ANT_memory_indexer() {}

	virtual ANT_memory_indexer_node *add_term(ANT_string_pair *string, long long docno) = 0;

	virtual void set_document_length(long long docno, long long length) {}
	virtual void set_document_detail(ANT_string_pair *measure_name, long long length, long mode = MODE_ABSOLUTE) {}
	virtual void set_compression_scheme(unsigned long scheme) {}
	virtual void set_compression_validation(unsigned long validate) {}
	virtual void text_render(long what) {}
	virtual void add_to_document_repository(char *filename, char *compressed_document, long compressed_length, long length) {}
	virtual long serialise(ANT_ranking_function_factory *factory) { return 0; }
	virtual long long get_memory_usage(void) { return 0; }
	virtual void set_variable(ANT_string_pair *measure_name, long long score) {}
} ;


#endif /* MEMORY_INDEXER_H_ */
