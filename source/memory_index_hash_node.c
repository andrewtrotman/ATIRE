/*
	MEMORY_INDEX_HASH_NODE.C
	------------------------
*/


class ANT_memory_index_hash_node
{
private:
	static ANT_memory_index_hash_node *all_nodes;
	static long last_allocated_node;

public:
	ANT_string_pair *string;
	ANT_memory_index_hash_node *left, *right;

public:
	ANT_memory_index_hash_node(ANT_string_pair *string);
	~ANT_memory_index_hash_node() {}
	void *operator new(size_t count);
} ;

ANT_memory_index_hash_node *ANT_memory_index_hash_node::all_nodes = NULL;
long ANT_memory_index_hash_node::last_allocated_node = 0;

/*
	ANT_MEMORY_INDEX_HASH_NODE::ANT_MEMORY_INDEX_HASH_NODE()
	--------------------------------------------------------
*/
ANT_memory_index_hash_node::ANT_memory_index_hash_node(ANT_string_pair string)
{
}

/*
	ANT_MEMORY_INDEX_HAS_NODE::OPERATOR NEW ()
	------------------------------------------
*/
void *ANT_memory_index_hash_node::operator new (size_t count)
{
void *answer;

#pragma omp critical
	{
	if (all_nodes == NULL)
		all_nodes = realloc(all_nodes, sizeof(*all_nodes) * max_nodes);
	answer = (void *)all_nodes[last_allocated_node++];
	}
return answer;
}
