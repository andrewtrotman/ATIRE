/*
	MEMORY_INDEX_HASH_NODE.C
	------------------------
*/
#include <stdio.h>
#include "string_pair.h"
#include "memory.h"
#include "memory_index_hash_node.h"
#include "memory_index_stats.h"
#include "postings_piece.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_MEMORY_INDEX_HASH_NODE::ANT_MEMORY_INDEX_HASH_NODE()
	--------------------------------------------------------
*/
ANT_memory_index_hash_node::ANT_memory_index_hash_node(ANT_memory *memory, ANT_string_pair *original_string, ANT_memory_index_stats *stats)
{
postings_initial_length = 8;
postings_growth_factor = 1.5;

this->stats = stats;
left = right = NULL;
this->memory = memory;

string.start = (char *)memory->malloc(original_string->length());
stats->strings++;
stats->bytes_in_string_pool += original_string->length();

original_string->strcpy(string.start);
string.string_length = original_string->length();

docid_list_head = docid_list_tail = new_postings_piece(postings_initial_length);
docid_node_length = postings_initial_length;
docid_node_used = 0;
stats->bytes_allocated_for_docids += postings_initial_length;

tf_list_head = tf_list_tail = new_postings_piece(postings_initial_length);
tf_node_length = postings_initial_length;
tf_node_used = 0;
stats->bytes_allocated_for_tfs += postings_initial_length;

collection_frequency = document_frequency = current_docno = 0;
docids_pos_on_disk = 0;
tfs_pos_on_disk = 0;
end_pos_on_disk = 0;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::~ANT_MEMORY_INDEX_HASH_NODE()
	---------------------------------------------------------
*/
ANT_memory_index_hash_node::~ANT_memory_index_hash_node()
{
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::OPERATOR NEW ()
	-------------------------------------------
*/
void *ANT_memory_index_hash_node::operator new (size_t count, ANT_memory *memory)
{
return memory->malloc(count);
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::COMPRESS_BYTES_NEEDED()
	---------------------------------------------------
*/
inline long ANT_memory_index_hash_node::compress_bytes_needed(long long docno)
{
if (docno & ((long long)127 << 28))
	return 5;
else if (docno & ((long long)127 << 21))
	return 4;
else if (docno & ((long long)127 << 14))
	return 3;
else if (docno & ((long long)127 << 7))
	return 2;
else
	return 1;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::COMPRESS_INTO()
	-------------------------------------------
*/
inline void ANT_memory_index_hash_node::compress_into(unsigned char *dest, long long docno)
{
if (docno & ((long long)127 << 28))
	goto five;
else if (docno & ((long long)127 << 21))
	goto four;
else if (docno & ((long long)127 << 14))
	goto three;
else if (docno & ((long long)127 << 7))
	goto two;
else
	goto one;

five:
	*dest++ = (docno >> 28) & 0x7F;
four:
	*dest++ = (docno >> 21) & 0x7F;
three:
	*dest++ = (docno >> 14) & 0x7F;
two:
	*dest++ = (docno >> 7) & 0x7F;
one:
	*dest++ = (docno & 0x7F) | 0x80;
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::ADD_POSTING()
	-----------------------------------------
*/
void ANT_memory_index_hash_node::add_posting(ANT_string_pair *keyword, long long docno)
{
unsigned char holding_pen[16];	//	we only actually need 10 (64 bits / 7 bit bytes);
long needed, remain;

collection_frequency++;
if (docno == current_docno)
	{
	stats->term_occurences++;
	if (tf_list_tail->data[tf_node_used - 1]++ > 254)
		{
		tf_list_tail->data[tf_node_used - 1] = 254;
#ifdef NEVER
		printf("Doc:%I64d, '%*.*s': TF trimmed at 255 occurences\n", docno, keyword->length(), keyword->length(), keyword->string());
#else
		keyword = keyword;		// this gets around the compiler warning about parameter "keyword" not being used
#endif
		}
	}
else
	{
	document_frequency++;
	needed = compress_bytes_needed(docno - current_docno);
	stats->bytes_to_store_docids += needed;

	if (docid_node_used + needed > docid_node_length)
		{
		/*
			Fill to the end of the block
		*/
		compress_into(holding_pen, docno - current_docno);
		memcpy(docid_list_tail->data + docid_node_used, holding_pen, docid_node_length - docid_node_used);
		remain = needed - (docid_node_length - docid_node_used);
		/*
			Allocate the new block
		*/
		docid_node_length = (long)(postings_growth_factor * docid_node_length);
		stats->bytes_allocated_for_docids += docid_node_length;
		docid_list_tail->next = new_postings_piece(docid_node_length);
		docid_list_tail = docid_list_tail->next;

		/*
			And place the "extra" into the beginning of the new block
		*/
		memcpy(docid_list_tail->data, holding_pen + (needed - remain), remain);
		docid_node_used = remain;
		}
	else
		{
		compress_into(docid_list_tail->data + docid_node_used, docno - current_docno);
		docid_node_used += needed;
		}

	current_docno = docno;

	if (tf_node_used + 1 > tf_node_length)
		{
		tf_node_length = (long)(postings_growth_factor * tf_node_length);
		stats->bytes_allocated_for_tfs += tf_node_length;
		tf_list_tail->next = new_postings_piece(tf_node_length);
		tf_list_tail = tf_list_tail->next;
		tf_node_used = 0;
		}
	stats->bytes_to_store_tfs++;
	tf_list_tail->data[tf_node_used] = 1;
	stats->term_occurences++;
	tf_node_used++;
	}
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::SERIALISE_POSTINGS()
	------------------------------------------------
*/
long ANT_memory_index_hash_node::serialise_postings(unsigned char *doc_into, long *doc_size, unsigned char *tf_into, long *tf_size)
{
ANT_postings_piece *where;
long err, size, doc_bytes, tf_bytes, more;

err = FALSE;
doc_bytes = 0;
size = postings_initial_length;
for (where = docid_list_head; where != NULL; where = where->next)
	{
	more = where->next == NULL ? docid_node_used : size;
	if (doc_bytes + more <= *doc_size)
		{
		if (where->next == NULL)
			memcpy(doc_into + doc_bytes, where->data, docid_node_used);			// final block many not be full
		else
			{
			memcpy(doc_into + doc_bytes, where->data, size);
			size = (long)(postings_growth_factor * size);
			}
		}
	else
		err = TRUE;
	doc_bytes += more;
	}

tf_bytes = 0;
size = postings_initial_length;
for (where = tf_list_head; where != NULL; where = where->next)
	{
	more = where->next == NULL ? tf_node_used : size;
	if (tf_bytes + more <= *tf_size)
		{
		if (where->next == NULL)
			memcpy(tf_into + tf_bytes, where->data, tf_node_used);
		else
			{
			memcpy(tf_into + tf_bytes, where->data, size);
			size = (long)(postings_growth_factor * size);
			}
		}
	else
		err = TRUE;
	tf_bytes += more;
	}

*doc_size = doc_bytes;
*tf_size = tf_bytes;

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
