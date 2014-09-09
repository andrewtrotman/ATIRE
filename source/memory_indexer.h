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
class ANT_stop_word;

/*
	class ANT_MEMORY_INDEXER
	------------------------
*/
class ANT_memory_indexer
{
public:
	enum { MODE_ABSOLUTE, MODE_MONOTONIC };

public:
	static ANT_string_pair squiggle_length;
	static ANT_string_pair *squiggle_puurula_length[];
	static ANT_string_pair *squiggle_puurula_tfidf_powerlaw_length[];
	static ANT_string_pair squiggle_unique_term_count;
	static ANT_string_pair squiggle_puurula_tfidf_length;
public:
	ANT_stop_word *stopwords;

public:
	ANT_memory_indexer() { stopwords = 0; }
	virtual ~ANT_memory_indexer() {}

	virtual ANT_memory_indexer_node *add_term(ANT_string_pair *string, long long docno, long extra_term_frequency = 1) = 0;
	virtual void set_document_length(long long docno, long long length) = 0;
//	virtual void set_puurula_length(long g, double length) = 0;
	virtual void set_document_detail(ANT_string_pair *measure_name, long long length, long mode = MODE_ABSOLUTE) = 0;
	virtual long long get_memory_usage(void) = 0;
	virtual short *get_frequencies(short *frequency, long long tf_cap) = 0;
} ;


#endif /* MEMORY_INDEXER_H_ */
