/*
	MEMORY_INDEX_HASH_NODE.H
	------------------------
*/
#ifndef MEMORY_INDEX_HASH_NODE_H_
#define MEMORY_INDEX_HASH_NODE_H_

#include "string_pair.h"
#include "postings_piece.h"
#include "stats_memory_index.h"

class ANT_memory;
class ANT_postings_piece;
class ANT_stats_memory_index;

#ifdef _MSC_VER
	#define inline __forceinline
#endif

/*
	class ANT_MEMORY_INDEX_HASH_NODE
	--------------------------------
*/
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
	ANT_stats_memory_index *stats;
	long long docids_pos_on_disk, end_pos_on_disk, impacted_length;

private:
	inline long compress_bytes_needed(long long val);
	inline void compress_into(unsigned char *dest, long long docno);
	ANT_postings_piece *new_postings_piece(long length_in_bytes);
	void insert_docno(long long docno);

public:
	ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *string, ANT_stats_memory_index *stats);
	~ANT_memory_index_hash_node() {};
	void *operator new(size_t count, ANT_memory *memory);
	void set(long long value);
	void add_posting(long long docno);
	long long serialise_postings(unsigned char *doc_into, long long *doc_size, unsigned char *tf_into, long long *tf_size);

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
	ANT_MEMORY_INDEX_HASH_NODE::ADD_POSTING()
	-----------------------------------------
*/
inline void ANT_memory_index_hash_node::add_posting(long long docno)
{
collection_frequency++;
if (docno == current_docno)
	{
	if (tf_list_tail->data[tf_node_used - 1]++ > 254)
		tf_list_tail->data[tf_node_used - 1] = 254;
	}
else
	{
	insert_docno(docno - current_docno);
	current_docno = docno;
	}
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

#endif  /* MEMORY_INDEX_HASH_NODE_H_ */
