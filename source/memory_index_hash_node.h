/*
	MEMORY_INDEX_HASH_NODE.H
	------------------------
*/
#ifndef MEMORY_INDEX_HASH_NODE_H_
#define MEMORY_INDEX_HASH_NODE_H_

#include "memory_indexer_node.h"
#include "string_pair.h"
#include "postings_piece.h"
#include "stats_memory_index.h"
#include "memory.h"

class ANT_postings_piece;
class ANT_stats_memory_index;

#ifdef _MSC_VER
	#define inline __forceinline
#endif

/*
	class ANT_MEMORY_INDEX_HASH_NODE
	--------------------------------
*/
class ANT_memory_index_hash_node : public ANT_memory_indexer_node
{
private:
	static const long postings_initial_length;
	static const double postings_growth_factor;

public:
	ANT_memory_index_hash_node *left, *right;

/*
	Thw point of this union is that the in_disk objects are never needed at the same time as the in_memory objects
	and therefore we can reduce the index-time memory footprint by overlapping them.
*/
	union
		{
		struct { ANT_postings_piece *docid_list_head, *docid_list_tail, *tf_list_head, *tf_list_tail; } in_memory;
		struct { long long docids_pos_on_disk, end_pos_on_disk, impacted_length; } in_disk;
		} ;

	long docid_node_used, docid_node_length;
	long tf_node_used, tf_node_length;

	long long current_docno;
	long long collection_frequency, document_frequency;
	ANT_memory *memory;
	ANT_stats_memory_index *stats;


//#ifdef TERM_LOCAL_MAX_IMPACT
	/*
	 *  this value is the max impact for the current term. The impact can be either
	 *  term frequency or quantised rsv value. This requires a extra byte in the
	 *  structure of the dictionary.
	 *
	 */
	unsigned char term_local_max_impact;
//#endif

private:
	inline long compress_bytes_needed(long long val);
	inline void compress_into(unsigned char *dest, long long docno);
	ANT_postings_piece *new_postings_piece(long length_in_bytes);
	void insert_docno(long long docno, unsigned char initial_term_frequency = 1);

public:
	ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *string, ANT_stats_memory_index *stats);
	void set(long long value);
	void add_posting(long long docno, long extra_term_frequency = 1);
	long long serialise_postings(unsigned char *doc_into, long long *doc_size, unsigned char *tf_into, long long *tf_size);

	static long decompress(unsigned char **from);
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
inline void ANT_memory_index_hash_node::add_posting(long long docno, long extra_term_frequency)
{
collection_frequency += extra_term_frequency;
if (docno == current_docno)
	{
	/*
		If we can add without overflowing then do so otherwise cap at 254
	*/
	if (in_memory.tf_list_tail->data[tf_node_used - 1] + extra_term_frequency <= 254)
		in_memory.tf_list_tail->data[tf_node_used - 1] += (unsigned char)extra_term_frequency;
	else
		in_memory.tf_list_tail->data[tf_node_used - 1] = 254;

	term_frequency += extra_term_frequency;
	}
else
	{
	insert_docno(docno - current_docno, extra_term_frequency > 254 ? 254 : (unsigned char)extra_term_frequency);
	current_docno = docno;
	term_frequency = extra_term_frequency;
	}
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::DECOMPRESS()
	----------------------------------------
	The postings are stored variable byte encoded and so we decode the variable byte here.
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
