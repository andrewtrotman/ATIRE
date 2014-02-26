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
	static const long postings_initial_length;				// these are declared in memory_index_hash_node.c because the beauty of C++
	static const double postings_growth_factor;				// prevents the static const initialisaiton of non-integral types!

public:
	ANT_memory_index_hash_node *left, *right;

	/*
		The point of this union is that the in_disk objects are never needed at the same time as the in_memory objects
		and therefore we can reduce the index-time memory footprint by overlapping them.
	*/
	/*
		clang on the Mac doesn't allow static consts in an anonymous union - so we can't delcare this where it should
		have been declared (which is in the "early" struct).
	*/
	static const size_t postings_held_in_vocab = 2;		// because this is how many are stored in the vocab on disk
	union
		{
		struct { ANT_postings_piece *docid_list_head, *docid_list_tail, *tf_list_head, *tf_list_tail; } in_memory;
		struct { long long docids_pos_on_disk, end_pos_on_disk, impacted_length; } in_disk;

		/*
			Most terms only occur once.  Of the remainder, most occur twice.
			If we store the first two <doc_id,tf> pairs in the ANT_memory_index_hash_node object then
			we get several advantages. First, it requires fewer allocations of memory (but I'm not
			certain it'll take less memory overall). Second, in the case of indexing more documents than
			there is memory it means that we can flush the postings while remaining compatible with
			"SPECIAL_COMPRESSION".  That is, we can put short postings lists into the vocab without
			worrying about the memory for the postings being flushed to disk and having lost the data
			(thus the final index will be smaller (and faster for low frequency terms)).
		*/
		struct
			{
			long long docid[postings_held_in_vocab];
			unsigned short tf[postings_held_in_vocab];
			} early;
		} ;

	size_t docid_node_used, docid_node_length;
	size_t tf_node_used, tf_node_length;

	long long current_docno;
	long long collection_frequency, document_frequency;
	ANT_memory *postings_memory;
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
	friend class ANT_search_engine_memory_index;
	ANT_memory_index_hash_node() {};		// used by ANT_search_engine_memory_index
	inline long compress_bytes_needed(long long val);
	inline void compress_into(unsigned char *dest, long long docno);
	ANT_postings_piece *new_postings_piece(size_t length_in_bytes);
	long insert_docno(long long docno, unsigned short initial_term_frequency = 1);
	inline long append_docno(long long docno, ANT_postings_piece **buffer);
	long copy_from_early_buffers_into_lists(unsigned char *document_buffer, unsigned short *term_frequency_buffer);
	long bytes_needed_for_early_doc_buffer(void);

public:
	ANT_memory_index_hash_node(ANT_memory *string_memory, ANT_memory *postings_memory, ANT_string_pair *string, ANT_stats_memory_index *stats);
	void set(long long value);
	long add_posting(long long docno, long extra_term_frequency = 1);
	long long serialise_postings(unsigned char *doc_into, long long *doc_size, unsigned short *tf_into, long long *tf_size);

	static long decompress(unsigned char **from);
	static int term_compare(const void *a, const void *b);
} ;

/*
	ANT_MEMORY_INDEX_HASH_NODE::NEW_POSTINGS_PIECE()
	------------------------------------------------
*/
inline ANT_postings_piece *ANT_memory_index_hash_node::new_postings_piece(size_t length_in_bytes)
{
ANT_postings_piece *object;

stats->posting_fragments++;

if ((object = new (postings_memory) ANT_postings_piece()) != NULL)
	if (object->initialise(postings_memory, length_in_bytes) != NULL)
		return object;

return NULL;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::ADD_POSTING()
	-----------------------------------------
	returns true on success and false on falure (which happens if we run out of memory)
*/
inline long ANT_memory_index_hash_node::add_posting(long long docno, long extra_term_frequency)
{
unsigned short *into;

if (docno == current_docno)
	{
	/*
		Work out where to put the result
	*/
	if (document_frequency < postings_held_in_vocab + 1)			// +1 because document_frequency is pre-incramented (because the indexer counts from 1)
		into = early.tf + document_frequency - 1;
	else
		into = in_memory.tf_list_tail->data + tf_node_used - 1;

	/*
		Now check for overflow and incrament only up-to the cap (of one unsigned byte)
	*/
	if (*into + extra_term_frequency <= 0xFF)
		*into += (unsigned char)extra_term_frequency;
	else
		*into = 0xFF;

	/*
		update the true term_frequency value (recall that the number above is capped at 254)
	*/
	term_frequency += extra_term_frequency;			// this is the true (uncapped) term frequency
	}
else
	{
	/*
		We're seeing a new doc-id which means we need to store the docid and might need to allocate memory
	*/
	if (!insert_docno(docno - current_docno, extra_term_frequency > 0xFF ? 0xFF : (unsigned short)extra_term_frequency))
		return false;
	current_docno = docno;
	term_frequency = extra_term_frequency;
	}

collection_frequency += extra_term_frequency;

return true;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::DECOMPRESS()
	----------------------------------------
	The postings are stored variable byte encoded and so we decode the variable byte here.
	This method is used for diagnostic purposes.
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
