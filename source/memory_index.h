
/*
	MEMORY_INDEX.H
	--------------
*/

#ifndef __MEMORY_INDEX_H__
#define __MEMORY_INDEX_H__

class ANT_memory_index_hash_node;
class ANT_memory;
class ANT_string_pair;

class ANT_memory_index
{
private:
	ANT_memory_index_hash_node *hash_table[27*27*27*27];
	ANT_memory *memory;

private:
	long hash(ANT_string_pair *string);
	ANT_memory_index_hash_node *find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string);

public:
	ANT_memory_index();
	~ANT_memory_index();

	void add_term(ANT_string_pair *string, long long docno);
} ;



#endif __MEMORY_INDEX_H__
