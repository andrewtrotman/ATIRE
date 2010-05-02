/*
	MEMORY_INDEX_ONE.H
	------------------
*/
#ifndef MEMORY_INDEX_ONE_H_
#define MEMORY_INDEX_ONE_H_

#include "memory_indexer.h"

class ANT_memory;
class ANT_memory_index_one_node;
class ANT_string_pair;

/*
	class ANT_MEMORY_INDEX_ONE
	--------------------------
*/
class ANT_memory_index_one : public ANT_memory_indexer
{
private:
	/*
		This is a careful ballence, it should be both large enough to avoid collisions of the terms in a
		single document, but small enough that itterating over the whole hash table will be fast, Otherwise
		the sequential cost post indexing will be too large.
	*/
	static const long HASH_TABLE_SIZE = 0x10000;
	ANT_memory_index_one_node *hash_table[HASH_TABLE_SIZE];
	ANT_memory *memory;

private:
	ANT_memory_index_one_node *new_hash_node(ANT_string_pair *string);
	ANT_memory_index_one_node *find_add_node(ANT_memory_index_one_node *root, ANT_string_pair *string);

public:
	ANT_memory_index_one(ANT_memory *memory);
	virtual ~ANT_memory_index_one() {}

	virtual ANT_memory_indexer_node *add_term(ANT_string_pair *string, long long docno);

	void rewind(void);
} ;

#endif /* MEMORY_INDEX_ONE_H_ */

