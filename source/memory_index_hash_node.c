/*
	MEMORY_INDEX_HASH_NODE.C
	------------------------
*/
#include <stdio.h>
#include "string_pair.h"
#include "memory.h"
#include "memory_index_hash_node.h"
#include "postings_piece.h"

/*
	ANT_MEMORY_INDEX_HASH_NODE::ANT_MEMORY_INDEX_HASH_NODE()
	--------------------------------------------------------
*/
ANT_memory_index_hash_node::ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *original_string)
{
left = right = NULL;
this->memory = memory;
string.start = (char *)memory->malloc(original_string->length());
original_string->strcpy(string.start);
string.string_length = original_string->length();
docid_list_head = docid_list_tail = new (memory) ANT_postings_piece(memory, 16);
tf_list_head = tf_list_tail = new (memory) ANT_postings_piece(memory, 16);
current_docno = 0;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::~ANT_MEMORY_INDEX_HASH_NODE()
	---------------------------------------------------------
*/
ANT_memory_index_hash_node::~ANT_memory_index_hash_node()
{
}

/*
	ANT_MEMORY_INDEX_HAS_NODE::OPERATOR NEW ()
	------------------------------------------
*/
void *ANT_memory_index_hash_node::operator new (size_t count, ANT_memory *memory)
{
return memory->malloc(count);
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::ADD_POSTING()
	-----------------------------------------
*/
void ANT_memory_index_hash_node::add_posting(long long docno)
{
if (docno == current_docno)
	{
	if (tf_list_tail->data[tf_list_tail->used]++ > 254)
		tf_list_tail->data[tf_list_tail->used] = 254;
	}
else
	{
	// diff = docno - current_docno
	// compress diff
	// stuff on the end of docid_list_tail (if it fits)
	// set tf_list_tail to zero (if it fits)
	}
}

