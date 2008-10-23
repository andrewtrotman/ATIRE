/*
	MEMORY_INDEX_HASH_NODE.H
	------------------------
*/

#ifndef __MEMORY_INDEX_HASH_NODE_H__
#define __MEMORY_INDEX_HASH_NODE_H__

#include "string_pair.h"

class ANT_memory;
class ANT_postings_piece;

class ANT_memory_index_hash_node
{
public:
	ANT_string_pair string;
	ANT_memory_index_hash_node *left, *right;
	ANT_postings_piece *docid_list_head, *docid_list_tail, *tf_list_head, *tf_list_tail;
	long long current_docno;
	long long collection_frequency, document_frequency;
	ANT_memory *memory;

private:
	long compress_bytes_needed(long long val);
	void compress_into(unsigned char *dest, long long docno);

public:
	ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *string);
	~ANT_memory_index_hash_node();
	void *operator new(size_t count, ANT_memory *memory);
	void add_posting(long long docno);
	long serialise_postings(unsigned char *doc_into, long *doc_size, unsigned char *tf_into, long *tf_size);

	long decompress(unsigned char **from);
} ;

/*
	ANT_MEMORY_INDEX_HASH_NODE::DECOMPRESS()
	----------------------------------------
*/
inline long ANT_memory_index_hash_node::decompress(unsigned char **from)
{
long ans = 0;

while ((**from & 0x80) == 0)
	{
	ans = (ans << 7) + **from;
	(*from)++;
	}

ans = (ans << 7) + (**from & 0x7F);
(*from)++;

return ans;
}

#endif __MEMORY_INDEX_HASH_NODE_H__
