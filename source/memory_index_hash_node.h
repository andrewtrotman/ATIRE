/*
	MEMORY_INDEX_HASH_NODE.H
	------------------------
*/

#ifndef __MEMORY_INDEX_HASH_NODE_H__
#define __MEMORY_INDEX_HASH_NODE_H__

#include "string_pair.h"
#include "postings_piece.h"
#include "memory_index_stats.h"

class ANT_memory;
class ANT_postings_piece;
class ANT_memory_index_stats;

class ANT_memory_index_hash_node
{
private:
	long postings_initial_length;
	double postings_growth_factor;
public:
	ANT_string_pair string;
	ANT_memory_index_hash_node *left, *right;
	ANT_postings_piece *docid_list_head, *docid_list_tail, *tf_list_head, *tf_list_tail;
	long docid_node_used, docid_node_length;
	long tf_node_used, tf_node_length;

	long long current_docno;
	long long collection_frequency, document_frequency;
	ANT_memory *memory;
	ANT_memory_index_stats *stats;
	long long docids_pos_on_disk, tfs_pos_on_disk, end_pos_on_disk;

private:
	inline long compress_bytes_needed(long long val);
	inline void compress_into(unsigned char *dest, long long docno);
	ANT_postings_piece *new_postings_piece(long length_in_bytes);

public:
	ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *string, ANT_memory_index_stats *stats);
	~ANT_memory_index_hash_node();
	void *operator new(size_t count, ANT_memory *memory);
	void add_posting(ANT_string_pair *keyword, long long docno);
	long serialise_postings(unsigned char *doc_into, long *doc_size, unsigned char *tf_into, long *tf_size);

	long decompress(unsigned char **from);
	static int term_compare(const void *a, const void *b);
} ;


/*
	ANT_MEMORY_INDEX_HASH_NODE::NEW_POSTINGS_PIECE()
	------------------------------------------------
*/
inline ANT_postings_piece *ANT_memory_index_hash_node::new_postings_piece(long length_in_bytes)
{
stats->posting_fragments++;
return new (memory) ANT_postings_piece(memory, length_in_bytes);
}

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
