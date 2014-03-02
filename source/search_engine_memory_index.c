/*
	SEARCH_ENGINE_MEMORY_INDEX.C
	----------------------------
*/
#include "btree_iterator.h"
#include "search_engine_memory_index.h"
#include "search_engine_btree_leaf.h"
#include "memory_index.h"

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::ANT_SEARCH_ENGINE_MEMORY_INDEX()
	----------------------------------------------------------------
*/
ANT_search_engine_memory_index::ANT_search_engine_memory_index(ANT_memory_index *index, ANT_memory *memory) : ANT_search_engine(memory)
{
this->index = index;
this->memory = memory;
index->quantization_bits = 8;
postings_buffer_location = postings_buffer = NULL;
postings_buffer_length = 0;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::~ANT_SEARCH_ENGINE_MEMORY_INDEX()
	-----------------------------------------------------------------
*/
ANT_search_engine_memory_index::~ANT_search_engine_memory_index()
{
delete memory;
delete index;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::OPEN()
	--------------------------------------
*/
int ANT_search_engine_memory_index::open(const char *filename)
{
documents = index->largest_docno;
memory->realign();
special_compression_buffer = (unsigned char *)memory->malloc(1024);// must be long enough to store the worst case of two posintgs + impact_headers, etc.  1024 is probably way too large.
memory->realign();
#ifdef IMPACT_HEADER
	impact_header.header_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*impact_header.header_buffer) * ANT_impact_header::NUM_OF_QUANTUMS * 3 + ANT_COMPRESSION_FACTORY_END_PADDING);
	decompress_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*decompress_buffer) * (documents + ANT_COMPRESSION_FACTORY_END_PADDING));
#else
	/*
		Allocate space for decompression.
		NOTES:
			Add 512 because of the tf and 0 at each end of each impact ordered list.
			Further add ANT_COMPRESSION_FACTORY_END_PADDING so that compression schemes that don't know when to stop (such as Simple-9) can overflow without problems.
	*/
	decompress_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*decompress_buffer) * (512 + documents + ANT_COMPRESSION_FACTORY_END_PADDING));
#endif

memory->realign();
stem_buffer = (ANT_weighted_tf *)memory->malloc(stem_buffer_length_in_bytes = (sizeof(*stem_buffer) * documents));
memory->realign();

return 1;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::WRITE()
	---------------------------------------
*/
long ANT_search_engine_memory_index::write(unsigned char *data, long long size)
{
char *tmp;
long long used;

if (postings_buffer_location + size > postings_buffer + postings_buffer_length)
	{
	used = postings_buffer_location - postings_buffer;
	postings_buffer_length = postings_buffer_length + size;			// used + size would be the exact size, this creates a bit of spare.
	tmp = (char *)memory->malloc(postings_buffer_length);
	memcpy(tmp, postings_buffer, (size_t)used);
	postings_buffer = tmp;
	postings_buffer_location = postings_buffer + used;
	}

memcpy(postings_buffer_location, data, (size_t)size);
postings_buffer_location += size;

return 1;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::GET_POSTINGS()
	----------------------------------------------
*/
unsigned char *ANT_search_engine_memory_index::get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination)
{
ANT_memory_index_hash_node *index_node, duplicate_node;
ANT_search_engine_btree_leaf leaf;

/*
	Get the node and take a copy of it (because serialise_one_node() will change it)
*/
index_node = (ANT_memory_index_hash_node *)term_details->postings_position_on_disk;
duplicate_node = *index_node;

/*
	Rewind the buffer
*/
postings_buffer_location = postings_buffer;

/*
	Serialise it
*/
index->serialise_one_node(this, &duplicate_node);

#ifdef SPECIAL_COMPRESSION
	/*
		If we're using SPECIAL_COMPRESSION then the index_leaf will contain the postings if Document Frequency is less than or equal to 2
	*/
	if (term_details->local_document_frequency <= 2)
		{
		/*
			Build an ANT_search_engine_btree_leaf and then get that decoded for us.
		*/
		leaf.global_collection_frequency = leaf.local_collection_frequency = duplicate_node.collection_frequency;
		leaf.global_document_frequency = leaf.local_document_frequency = duplicate_node.document_frequency;
		leaf.postings_position_on_disk = duplicate_node.in_disk.docids_pos_on_disk;
		leaf.impacted_length = duplicate_node.in_disk.impacted_length;
		leaf.postings_length = duplicate_node.in_disk.end_pos_on_disk;
		#ifdef TERM_LOCAL_MAX_IMPACT
			leaf.local_max_impact = 128;
		#endif
		postings_buffer = (char *)ANT_search_engine::get_postings(&leaf, (unsigned char *)postings_buffer);
		}
#endif

return (unsigned char *)postings_buffer;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::GET_POSTINGS_DETAILS()
	------------------------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine_memory_index::get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details)
{
ANT_memory_index_hash_node *index_node, duplicate_node;
ANT_string_pair term_as_pair(term);
long hash_of_term = index->hash(&term_as_pair);

if (index->hash_table[hash_of_term] == NULL)
	return NULL;			// term is not in the index

index_node = index->find_node(index->hash_table[hash_of_term], &term_as_pair);

if (index_node == NULL)
	return NULL;

term_details->global_collection_frequency = term_details->local_collection_frequency = index_node->collection_frequency;
term_details->global_document_frequency = term_details->local_document_frequency = index_node->document_frequency;
term_details->postings_position_on_disk = (long long)index_node;		// this is a dreadful hack, it assumes a pointer will fit in a 64-bit pointer and actually points to the index_node not the postings
/*
	We fake the next three because we just don't know.
*/
term_details->impacted_length = 0;
term_details->postings_length = 0;
#ifdef TERM_LOCAL_MAX_IMPACT
	term_details->local_max_impact = 128;
#endif


return term_details;
}

