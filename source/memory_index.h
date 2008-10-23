
/*
	MEMORY_INDEX.H
	--------------
*/

#ifndef __MEMORY_INDEX_H__
#define __MEMORY_INDEX_H__

class ANT_memory_index_hash_node;
class ANT_memory;
class ANT_string_pair;

#define HASH_TABLE_SIZE (27*27*27*27)

class ANT_memory_index
{
private:
	ANT_memory_index_hash_node *hash_table[HASH_TABLE_SIZE];
	ANT_memory *memory;
	unsigned char *serialised_docids, *serialised_tfs;
	long serialised_docids_size, serialised_tfs_size;

private:
	long hash(ANT_string_pair *string);
	unsigned long dehash(long hash_val);
	ANT_memory_index_hash_node *find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string);
	long long serialise_all_nodes(ANT_memory_index_hash_node *root);
	void text_render(ANT_memory_index_hash_node *root, unsigned char *serialised_docids, long doc_size, unsigned char *serialised_tfs, long tf_size);

public:
	ANT_memory_index();
	~ANT_memory_index();

	void add_term(ANT_string_pair *string, long long docno);
	long long serialise();
} ;



#endif __MEMORY_INDEX_H__
