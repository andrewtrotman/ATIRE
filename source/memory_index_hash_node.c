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
collection_frequency = document_frequency = current_docno = 0;
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
void ANT_memory_index_hash_node::add_posting(long long docno)
{
long needed;

collection_frequency++;
if (docno == current_docno)
	{
	if (tf_list_tail->data[tf_list_tail->used]++ > 254)
		tf_list_tail->data[tf_list_tail->used] = 254;
	}
else
	{
	document_frequency++;
	needed = compress_bytes_needed(docno - current_docno);
	if (docid_list_tail->used + needed > docid_list_tail->length)
		{
		docid_list_tail->next = new (memory) ANT_postings_piece(memory, 2 * docid_list_tail->length);
		docid_list_tail = docid_list_tail->next;
		}
	compress_into(docid_list_tail->data + docid_list_tail->used, docno - current_docno);
	docid_list_tail->used += needed;
	current_docno = docno;

	if (tf_list_tail->used + 1 > tf_list_tail->length)
		{
		tf_list_tail->next = new (memory) ANT_postings_piece(memory, 2 * tf_list_tail->length);
		tf_list_tail = tf_list_tail->next;
		}
	tf_list_tail->data[tf_list_tail->used] = 1;
	}
}

/*
	ANT_MEMORY_INDEX_HASH_NODE::SERIALISE_POSTINGS()
	------------------------------------------------
*/
long ANT_memory_index_hash_node::serialise_postings(char *doc_into, long *doc_size, char *tf_into, long *tf_size)
{
ANT_postings_piece *where;
long doc_bytes, tf_bytes;

doc_bytes = 0;
for (where = docid_list_head; where != NULL; where = where->next)
	{
	memcpy(doc_into + doc_bytes, where->data, where->used);
	doc_bytes += where->used;
	}

tf_bytes = 0;
for (where = tf_list_head; where != NULL; where = where->next)
	{
	memcpy(tf_into + tf_bytes, where->data, where->used);
	tf_bytes += where->used;
	}

*doc_size = doc_bytes;
*tf_size = tf_bytes;

return doc_bytes + tf_bytes;
}



