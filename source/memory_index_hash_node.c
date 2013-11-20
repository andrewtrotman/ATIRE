/*
	MEMORY_INDEX_HASH_NODE.C
	------------------------
*/
#include <stdio.h>
#include "string_pair.h"
#include "memory.h"
#include "memory_index_hash_node.h"
#include "stats_memory_index.h"
#include "postings_piece.h"
#include "compress_variable_byte.h"
#include "fence.h"

/*
	Static consts for the class, the initial length of a postings list and the growth factor.
	Making these static consts saves 1.6MB on the Wall Street Journal Collection!
*/
const long ANT_memory_index_hash_node::postings_initial_length = 8;			// start with this many bytes
const double ANT_memory_index_hash_node::postings_growth_factor = 1.5;			// and grow at this rate

/*
	ANT_MEMORY_INDEX_HASH_NODE::ANT_MEMORY_INDEX_HASH_NODE()
	--------------------------------------------------------
*/
ANT_memory_index_hash_node::ANT_memory_index_hash_node(ANT_memory *string_memory, ANT_memory *postings_memory, ANT_string_pair *original_string, ANT_stats_memory_index *stats) : ANT_memory_indexer_node()
{
this->stats = stats;
left = right = NULL;
this->postings_memory = postings_memory;

string.start = (char *)string_memory->malloc(original_string->length() + 1);		// +1 because it adds a '\0';
stats->strings++;
stats->bytes_in_string_pool += original_string->length() + 1;

original_string->strcpy(string.start);
string.string_length = original_string->length();

in_memory.docid_list_head = in_memory.docid_list_tail = NULL;
docid_node_length = 0;
docid_node_used = 0;

in_memory.tf_list_head = in_memory.tf_list_tail = NULL;
tf_node_length = 0;
tf_node_used = 0;

collection_frequency = document_frequency = current_docno = term_local_max_impact = 0;

/*
	As we need this code complete before we add it to the structure (because it'll be seen by another thread)
	it is necessary to force the write to finish with a write-barrier (sfence in Intel speak)
*/
ANT_write_fence();
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::COMPRESS_BYTES_NEEDED()
	---------------------------------------------------
*/
inline long ANT_memory_index_hash_node::compress_bytes_needed(long long docno)
{
return ANT_compress_variable_byte::compress_bytes_needed(docno);
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::COMPRESS_INTO()
	-------------------------------------------
*/
inline void ANT_memory_index_hash_node::compress_into(unsigned char *dest, long long docno)
{
ANT_compress_variable_byte::compress_into(dest, docno);
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::SET()
	---------------------------------
	This routine is designed to allow a user to "force" a 64-bit integer into the
	postings lists.  The purpose of this is to make it possible to store 64-bit
	integer variables in the index.  This frees us from file formats.  Its basicly
	used for search engine "variables".
*/
void ANT_memory_index_hash_node::set(long long value)
{
unsigned long top, bottom;

top = (((unsigned long long)value) >> 32) & 0xFFFFFFFF;
bottom = ((unsigned long long)value) & 0xFFFFFFFF;

collection_frequency += 2;
insert_docno(top);
insert_docno(bottom);
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::APPEND_DOCNO()
	------------------------------------------
*/
long ANT_memory_index_hash_node::append_docno(long long docno, ANT_postings_piece **buffer)
{
unsigned char holding_pen[16];
long needed = compress_bytes_needed(docno);
size_t new_docid_node_length, remain;
ANT_postings_piece *underlying = *buffer;

if (docid_node_used + needed > docid_node_length)
	{
	/*
		Allocate the new block
	*/
	new_docid_node_length = (size_t)(postings_growth_factor * docid_node_length);
	if ((underlying->next = new_postings_piece(new_docid_node_length)) == NULL)
		return false;		// out of memory

	/*
		Fill to the end of the block (if there's anything to copy)
	*/
	compress_into(holding_pen, docno);
	if (docid_node_length - docid_node_used != 0)
		memcpy((unsigned char *)underlying->data + docid_node_used, holding_pen, docid_node_length - docid_node_used);
	remain = needed - (docid_node_length - docid_node_used);

	/*
		Move to the buffer we just allocated
	*/
	stats->bytes_allocated_for_docids += docid_node_length = new_docid_node_length;
	*buffer = underlying = underlying->next;

	/*
		And place the "extra" into the beginning of the new block
	*/
	memcpy((unsigned char *)underlying->data, holding_pen + (needed - remain), remain);
	docid_node_used = remain;
	}
else
	{
	compress_into((unsigned char *)underlying->data + docid_node_used, docno);
	docid_node_used += needed;
	}

return true;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::COPY_FROM_EARLY_BUFFERS_INTO_LISTS()
	----------------------------------------------------------------
*/
long ANT_memory_index_hash_node::copy_from_early_buffers_into_lists(unsigned char *document_buffer, unsigned short *term_frequency_buffer)
{
long long which, end;
long bytes_needed;
unsigned char *here = document_buffer;

end = postings_held_in_vocab < document_frequency ? postings_held_in_vocab : document_frequency;
for (which = 0; which < end; which++)
	{
	bytes_needed = compress_bytes_needed(early.docid[which]);
	compress_into(here, early.docid[which]);
	here += bytes_needed;

	term_frequency_buffer[which] = early.tf[which];
	}

return (long)(here - document_buffer);			// the number of bytes used.
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::BYTES_NEEDED_FOR_EARLY_DOC_BUFFER()
	---------------------------------------------------------------
*/
long ANT_memory_index_hash_node::bytes_needed_for_early_doc_buffer(void)
{
size_t posting;
long needed;
unsigned long long end;

needed = 0;
end = postings_held_in_vocab < document_frequency ? postings_held_in_vocab : document_frequency;
for (posting = 0; posting < end; posting++)
	needed += compress_bytes_needed(early.docid[posting]);

return needed;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::INSERT_DOCNO()
	------------------------------------------
	returns true on success and false on failure.  Failure can only happen if we run out of memory
*/
long ANT_memory_index_hash_node::insert_docno(long long docno, unsigned short initial_term_frequency)
{
long posting;
size_t new_tf_node_length;
ANT_postings_piece *document_buffer, *tf_buffer, *document_head;

/*
	First check to see if we can put the result into the "early" structure
*/
if (document_frequency <= postings_held_in_vocab)
	{
	if (document_frequency < postings_held_in_vocab)
		{
		/*
			We can put the scores into the vocab, so do so.
		*/
		early.docid[document_frequency] = docno;
		early.tf[document_frequency] = initial_term_frequency;

		document_frequency++;

		return true;
		}
	else
		{
		/*
			We can't fit the posting into the vocab so we're going to allocate memory for the buffers
		*/

		/*
			allocate buffers for the postings and the term frequencies, check for low memory, then add to the stats
			we need the head because buffer may be moved along
		*/
		document_head = document_buffer = new_postings_piece(postings_initial_length);
		tf_buffer = new_postings_piece(postings_initial_length);

		if (document_buffer == NULL || tf_buffer == NULL)
			return false;

		stats->bytes_allocated_for_docids += docid_node_length = postings_initial_length;
		stats->bytes_allocated_for_tfs += tf_node_length = postings_initial_length;

		/*
			Copy from the early array into the buffer
		*/
		for (posting = 0; posting < postings_held_in_vocab; posting++)
			if (append_docno(early.docid[posting], &document_buffer))
				tf_buffer->data[posting] = early.tf[posting];
			else
				return false;
		tf_node_used = postings_held_in_vocab;

		/*
			Now shove the lists into the internal structures (thus overwriting the "early" objects due to the union)
		*/
		in_memory.docid_list_head = document_head;
		in_memory.docid_list_tail = document_buffer;
		in_memory.tf_list_head = in_memory.tf_list_tail = tf_buffer;

		/*
			Now we fall through and add the posting from the routine's parameters into the end of the list (it should fit exaxtly)
		*/
		}
	}

/*
	Now add to the regular (growing) buffers

	First we'll do the tf because that's easy to wind-back on out of memory.

	2 * because tf_node_used keeps track of items, tf_node_length is bytes, and items are 2 bytes (shorts)
*/
if (2 * (tf_node_used + 1) > tf_node_length)
	{
	new_tf_node_length = (size_t)(tf_node_length * postings_growth_factor);
	/*
		Have to check that the tf's will fit, so tf_node_length has to be divisible by 2
	*/
	if (new_tf_node_length % 2 != 0)
		new_tf_node_length++;
	if ((in_memory.tf_list_tail->next = new_postings_piece(new_tf_node_length)) == NULL)
		return false;		// out of memory
	stats->bytes_allocated_for_tfs += tf_node_length = new_tf_node_length;
	in_memory.tf_list_tail = in_memory.tf_list_tail->next;
	tf_node_used = 0;
	/*
		We'll fill this in later so that we never need to re-wind
	*/
	}

/*
	Finally we do the docid buffer because its a bit more complicated when we run out out memory.
	In this case we need to allocate the new memory before we copy the first part of the compressed
	buffer because otherwise we might incorrectly write it to disk.
*/
if (!append_docno(docno, &in_memory.docid_list_tail))
	return false;

/*
	Update of the docid buffer worked, we know there is memory for the tf-buffer, and so we update
	the tf buffer now.
*/
in_memory.tf_list_tail->data[tf_node_used] = initial_term_frequency;
tf_node_used++;

/*
	At last we can increment the document frequency and return success
*/
document_frequency++;

return true;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::SERIALISE_POSTINGS()
	------------------------------------------------
*/
long long ANT_memory_index_hash_node::serialise_postings(unsigned char *doc_into, long long *doc_size, unsigned short *tf_into, long long *tf_size)
{
ANT_postings_piece *where;
long long err, size, doc_bytes, tf_bytes, more;

err = false;

/*
	if there are very few postings then they are in the "early" buffers, if there
	are many then they are in buffers external to this object.  Check which is the
	case and handle appropriately
*/
if (document_frequency <= postings_held_in_vocab)
	{
	/*
		compute the storage space needed.
	*/
	doc_bytes = bytes_needed_for_early_doc_buffer();
	tf_bytes = 2 * postings_held_in_vocab; // 2 * because shorts are 2 byte

	/*
		if it fits then copy into the buffers (that were passed as parameters) else error
	*/
	if ((*doc_size >= doc_bytes) && (*tf_size >= tf_bytes))
		docid_node_used = copy_from_early_buffers_into_lists(doc_into, tf_into);
	else
		err = true;
	}
else
	{
	/*
		serialise the doc ids by walking the linked list of blocks
	*/
	doc_bytes = 0;
	size = postings_initial_length;
	for (where = in_memory.docid_list_head; where != NULL; where = where->next)
		{
		more = where->next == NULL ? docid_node_used : size;
		if (doc_bytes + more <= *doc_size)
			{
			memcpy(doc_into + doc_bytes, where->data, more);			// final block many not be full
			size = (long)(postings_growth_factor * size);
			}
		else
			err = true;
		doc_bytes += more;
		}

	/*
		serialise the term frequencies
	*/
	tf_bytes = 0;
	size = postings_initial_length;
	for (where = in_memory.tf_list_head; where != NULL; where = where->next)
		{
		more = where->next == NULL ? 2 * tf_node_used : size;
		if (tf_bytes + more <= *tf_size)
			{
			memcpy((unsigned char *)tf_into + tf_bytes, where->data, more);
			size = (long)(postings_growth_factor * size);
			/*
				Because the tfs are 2 bytes and have to fit nice, size can get uneven and
				make it all wobbly
			*/
			if (size % 2 != 0)
				size++;
			}
		else
			err = true;
		tf_bytes += more;
		}
	}

/*
	tell the caller how mich space we needed
*/
*doc_size = doc_bytes;
*tf_size = tf_bytes;

/*
	return the number of bytes used
*/
return err ? 0 : doc_bytes + tf_bytes;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::TERM_COMPARE()
	------------------------------------------
*/
int ANT_memory_index_hash_node::term_compare(const void *a, const void *b)
{
ANT_memory_index_hash_node **one, **two;
one = (ANT_memory_index_hash_node **)a;
two = (ANT_memory_index_hash_node **)b;

return (*one)->string.true_strcmp(&(*two)->string);
}

