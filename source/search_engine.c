/*
	SEARCH_ENGINE.C
	---------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "maths.h"
#include "ctypes.h"
#include "search_engine.h"
#include "file_memory.h"
#include "memory.h"
#include "search_engine_btree_node.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "search_engine_result.h"
#ifdef FILENAME_INDEX
	#include "search_engine_result_id_iterator.h"
#else
	#include "search_engine_result_iterator.h"
#endif
#include "stats_search_engine.h"
#include "ranking_function_bm25.h"
#include "stemmer.h"
#include "stemmer_stem.h"
#include "stemmer_factory.h"
#include "thesaurus.h"
#include "thesaurus_relationship.h"
#include "compress_variable_byte.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_SEARCH_ENGINE::ANT_SEARCH_ENGINE()
	--------------------------------------
*/
ANT_search_engine::ANT_search_engine(ANT_memory *memory, long memory_model)
{
trim_postings_k = global_trim_postings_k = LONG_MAX;
stats = new ANT_stats_search_engine(memory);
stats_for_all_queries = new ANT_stats_search_engine(memory);
this->memory = memory;
this->stemmer = NULL;
results_list = NULL;

if (memory_model)
	index = new ANT_file_memory;
else
	index = new ANT_file;

this->memory_model = memory_model;
}

/*
	ANT_SEARCH_ENGINE::OPEN()
	-------------------------
*/
int ANT_search_engine::open(const char *filename)
{
int32_t four_byte;
int64_t eight_byte;
unsigned char *block;
long long end, term_header, this_header_block_size, sum, current_length, which_stemmer;
long decompressed_integer;
ANT_search_engine_btree_node *current, *end_of_node_list;
ANT_search_engine_btree_leaf collection_details;
ANT_compress_variable_byte variable_byte;
#ifndef IMPACT_HEADER
ANT_compressable_integer *value;
#endif

if (index->open((char *)filename, "rbx") == 0)
	return 0;

index_filename = filename;

/*
	At the end of the file is a "header" that provides various details:
	long long: the location of the b-tree header block
	long: the string length of the longest term
	long: the number of unique terms (new version only)
	long: the length of the longest compressed postings list
	long long: the maximum number of postings in a postings list (the highest DF)
*/
end = index->file_length();
#ifdef IMPACT_HEADER
	index->seek(end - sizeof(eight_byte) - sizeof(four_byte) - sizeof(four_byte) - sizeof(four_byte) - sizeof(eight_byte) - sizeof(eight_byte) - sizeof(eight_byte) - sizeof(four_byte) - sizeof(four_byte));
#else
	index->seek(end - sizeof(eight_byte) - sizeof(four_byte) - sizeof(four_byte) - sizeof(eight_byte) - sizeof(eight_byte) - sizeof(eight_byte) - sizeof(four_byte) - sizeof(four_byte));
#endif

index->read(&eight_byte);
term_header = eight_byte;

index->read(&four_byte);
string_length_of_longest_term = four_byte;

#ifdef IMPACT_HEADER
	index->read(&four_byte);
	unique_terms = four_byte;
#endif

index->read(&four_byte);
postings_buffer_length = four_byte;

index->read(&eight_byte);
highest_df = eight_byte;

// read checksum
index->read(&eight_byte);

// read collection id
index->read(&eight_byte);
if (eight_byte != ANT_file_signature_index)
	exit(printf("ERROR: file type mismatch\n"));

// read version number
index->read(&four_byte);
if (four_byte != ANT_version)
	exit(printf("ERROR: version mismatch\n"));

// read file signature
index->read(&four_byte);
if (four_byte != ANT_file_signature)
	exit(printf("ERROR: file signature mismatch\n"));

/*
	Load the B-tree header
*/
//printf("B-tree header is %lld bytes on disk\n", end - term_header);
index->seek(term_header);
if (this->memory_model) // INDEX_IN_MEMORY, no need to allocate memory
	block = NULL;
else
	block = (unsigned char *)memory->malloc((long)(end - term_header));
block = index->read_return_ptr(block, (long)(end - term_header));

/*
	The first sizeof(int64_t) bytes of the header are the number of nodes in the root
*/
btree_nodes = (long)(ANT_get_long_long(block) + 1);		// +1 because were going to add a sentinal at the start
//printf("There are %ld nodes in the root of the btree\n", btree_nodes - 1);
block += sizeof(int64_t);
btree_root = (ANT_search_engine_btree_node *)memory->malloc((long)(sizeof(ANT_search_engine_btree_node) * (btree_nodes + 1)));	// +1 to null terminate (with the end of last block position)

/*
	Then we have a sequence of '\0' terminated string / offset pairs
	But first add the sentinal to the beginning (this is a one-off expense at startup)
*/
current = btree_root;
current->disk_pos = 0;
current->term = (char *)memory->malloc(2);
current->term[0] = '\0';
end_of_node_list = btree_root + btree_nodes;
for (current++; current < end_of_node_list; current++)
	{
	current->term = (char *)block;
	current->term_length = (long)strlen((char *)block);		// strings of no more than 4GB (to keep the structures small)
	while (*block != '\0')
		block++;
	block++;
	current->disk_pos = ANT_get_long_long(block);
	block += sizeof(current->disk_pos);
	}
current->term = NULL;
current->disk_pos = term_header;

/*
	Compute the size of the largest block and then allocate memory so that it will fit (and use that throughout the execution of this program)
*/
max_header_block_size = 0;
for (current = btree_root + 1; current < end_of_node_list; current++)
	{
	this_header_block_size = (current + 1)->disk_pos - current->disk_pos;
	if (this_header_block_size > max_header_block_size)
		max_header_block_size = this_header_block_size;
//	printf("%s : %lld (size:%lld bytes)\n", current->term, current->disk_pos, this_header_block_size);
	}
memory->realign();
btree_leaf_buffer = (unsigned char *)memory->malloc((long)max_header_block_size);

/*
	Allocate the accumulators array, the docid array, and the term_frequency array
*/
get_postings_details("~length", &collection_details);
documents = collection_details.local_document_frequency;

if (this->memory_model) //INDEX_IN_MEMORY, no need to allocate memory
	{
	postings_buffer = NULL;
	special_compression_buffer = (unsigned char *)memory->malloc(1024);// must be long enough to store the worst case of two posintgs + impact_headers, etc.  1024 is probably way too large.
	}
else
	{
	postings_buffer = (unsigned char *)memory->malloc(postings_buffer_length);
	special_compression_buffer = NULL;
	memory->realign();
	}

#ifdef IMPACT_HEADER
	impact_header.header_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*impact_header.header_buffer) * ANT_impact_header::NUM_OF_QUANTUMS * 3 + ANT_COMPRESSION_FACTORY_END_PADDING);
	memory->realign();
	decompress_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*decompress_buffer) * (documents + ANT_COMPRESSION_FACTORY_END_PADDING));
	memory->realign();
#else
	/*
		Allocate space for decompression.
		NOTES:
			Add 512 because of the tf and 0 at each end of each impact ordered list.
			Further add ANT_COMPRESSION_FACTORY_END_PADDING so that compression schemes that don't know when to stop (such as Simple-9) can overflow without problems.
	*/
	decompress_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*decompress_buffer) * (512 + documents + ANT_COMPRESSION_FACTORY_END_PADDING));
	memory->realign();
#endif

document_lengths = (ANT_compressable_integer *)memory->malloc(documents * sizeof(*document_lengths));

results_list = new (memory) ANT_search_engine_result(memory, documents);

/*
	decompress the document length vector
*/
postings_buffer = get_postings(&collection_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, collection_details.local_document_frequency);

sum = 0;
for (current_length = 0; current_length < documents; current_length++)
	{
	decompressed_integer = decompress_buffer[current_length] - 1;		// because the indexer and the search engine count from different numbers.
	sum += decompressed_integer;
	document_lengths[current_length] = decompressed_integer;
	}
mean_document_length = (double)sum / (double)documents;
collection_length_in_terms = sum;

/*
	Load the positions of the documents from within the index
*/
document_offsets = NULL;
document_longest_raw_length = 0;

if (get_postings_details("~documentoffsets", &collection_details) != NULL)
	{
	memory->realign();
	document_offsets = (long long *)memory->malloc(collection_details.local_document_frequency * sizeof(*document_offsets));
	postings_buffer = get_postings(&collection_details, postings_buffer);
	factory.decompress(decompress_buffer, postings_buffer, collection_details.local_document_frequency);
	document_longest_compressed = sum = 0;
	for (current_length = 0; current_length < collection_details.local_document_frequency; current_length++)
		{
		if (decompress_buffer[current_length] > document_longest_compressed)
			document_longest_compressed = decompress_buffer[current_length];

		sum += decompress_buffer[current_length];
		document_offsets[current_length] = sum;
		}
	document_decompress_buffer = (char *)memory->malloc(document_longest_compressed);

#ifdef IMPACT_HEADER
	document_longest_raw_length = (long)get_variable("~documentlongest");
#else
	if ((value = get_decompressed_postings("~documentlongest", &collection_details)) != NULL)
		#ifdef SPECIAL_COMPRESSION
			document_longest_raw_length = *(value + 1);
		#else
			document_longest_raw_length = *value;
		#endif
#endif
	}

/*
	Allocate the static space for stemming
*/
memory->realign();
stem_buffer = (ANT_weighted_tf *)memory->malloc(stem_buffer_length_in_bytes = (sizeof(*stem_buffer) * documents));

/*
	If we have a stemmed index then build a stemmer for use on the search terms
*/
if ((which_stemmer = get_variable("~stemmer")) != 0)
	{
	stemmer = ANT_stemmer_factory::get_core_stemmer(which_stemmer);
	printf("This index was stemmed using %s, search terms will be stemmed\n", stemmer->name());
	}

is_quantized = get_variable("~quantized");
if ((global_trim_postings_k = get_variable("~trimpoint")) == 0)
	global_trim_postings_k = LONG_MAX;									// trim at 0 means no trimming

stats_for_all_queries->add_disk_bytes_read_on_init(index->get_bytes_read());

#ifdef FILENAME_INDEX
	/*
		Get the location of the filename and the index to the filenames.
	*/
	filename_start = get_variable("~documentfilenamesstart");
	filename_finish = get_variable("~documentfilenamesfinish");
	filename_index_start = get_variable("~documentfilenamesindexstart");
	filename_index_finish = get_variable("~documentfilenamesindexfinish");
#endif

return 1;
}

/*
	ANT_SEARCH_ENGINE::~ANT_SEARCH_ENGINE()
	---------------------------------------
*/
ANT_search_engine::~ANT_search_engine()
{
/* This is allocated in the memory pool, but it won't call the destructor, we
 * have to call the destructor ourselves:
 */
if (results_list)
	results_list->~ANT_search_engine_result();

index->close();
delete index;
delete stats;
delete stats_for_all_queries;
}

/*
	ANT_SEARCH_ENGINE::STATS_TEXT_RENDER()
	--------------------------------------
*/
void ANT_search_engine::stats_text_render(void)
{
stats->text_render();
}

/*
	ANT_SEARCH_ENGINE::STATS_ALL_TEXT_RENDER()
	------------------------------------------
*/
void ANT_search_engine::stats_all_text_render(void)
{
stats_for_all_queries->text_render();
}

/*
	ANT_SEARCH_ENGINE::STATS_INITIALISE()
	-------------------------------------
*/
void ANT_search_engine::stats_initialise(void)
{
stats->initialise();
}

/*
	ANT_SEARCH_ENGINE::STATS_ADD()
	------------------------------
*/
void ANT_search_engine::stats_add(void)
{
stats_for_all_queries->add(stats);
}

/*
	ANT_SEARCH_ENGINE::SET_ACCUMULATOR_WIDTH()
	------------------------------------------
*/
void ANT_search_engine::set_accumulator_width(long long width)
{
#ifdef TWO_D_ACCUMULATORS_POW2_WIDTH
	results_list->set_accumulator_width(ANT_pow2_zero_64(results_list->width_in_bits));				// by default use what ever the constructor used
#elif TWO_D_ACCUMULATORS
	results_list->set_accumulator_width(width);
#else
	// This is a meaningless case as the accumulators would not have width
#endif
}

/*
	ANT_SEARCH_ENGINE::INIT_ACCUMULATORS()
	--------------------------------------
*/
void ANT_search_engine::init_accumulators(long long top_k)
{
long long now;

now = stats->start_timer();

/*
	The line of code below used to add one to documents to prevent a slow-down when every document is in the 
	results list, however this lead to a crash as it would fill the list and then overflow (with a duplicate
	document). So we no longer add one.
*/
results_list->init_accumulators(top_k > documents ? documents : top_k);
stats->add_accumulator_init_time(stats->stop_timer(now));
}

/*
	ANT_SEARCH_ENGINE::GET_BTREE_LEAF_POSITION()
	--------------------------------------------
*/
long long ANT_search_engine::get_btree_leaf_position(char *term, long long *length, long *exact_match, long *btree_root_node)
{
long low, high, mid;

/*
	Binary search to find the block containing the term
*/
low = 0;
high = btree_nodes;
while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp(btree_root[mid].term, term) < 0)
		low = mid + 1;
	else
		high = mid;
	}
if ((low < btree_nodes) && (strcmp(btree_root[low].term, term) == 0))
	{
	/*
		Found, so we're either a short string or we're the name of a header
	*/
	*btree_root_node = low;
	*exact_match = TRUE;
	*length = btree_root[low + 1].disk_pos - btree_root[low].disk_pos;
	return btree_root[low].disk_pos;
	}
else
	{
	/*
		Not Found, so we're one past the header node
	*/
	*btree_root_node = low - 1;
	*exact_match = FALSE;
	*length = btree_root[low].disk_pos - btree_root[low - 1].disk_pos;
	return btree_root[low - 1].disk_pos;
	}
}

/*
	ANT_SEARCH_ENGINE::GET_LEAF()
	-----------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine::get_leaf(unsigned char *leaf, long term_in_leaf, ANT_search_engine_btree_leaf *term_details)
{
long leaf_size;
unsigned char *base;

// length of a leaf node (sum of cf, df, etc. sizes)
leaf_size = ANT_btree_iterator::LEAF_SIZE;
base = leaf + leaf_size * term_in_leaf + sizeof(int32_t);		// sizeof(int32_t) is for the number of terms in the node
term_details->local_collection_frequency = ANT_get_long(base);
term_details->global_collection_frequency = term_details->local_collection_frequency;
term_details->local_document_frequency = ANT_get_long(base + 4);
term_details->global_document_frequency = term_details->local_document_frequency;
term_details->postings_position_on_disk = ANT_get_long_long(base + 8);
term_details->impacted_length = ANT_get_long(base + 16);
term_details->postings_length = ANT_get_long(base + 20);
#ifdef TERM_LOCAL_MAX_IMPACT
	term_details->local_max_impact = ANT_get_short(base + 24);
#endif

return term_details;
}

/*
	ANT_SEARCH_ENGINE::GET_POSTINGS_DETAILS()
	-----------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine::get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details)
{
long long node_position, node_length;
long low, high, mid, nodes;
long leaf_size, exact_match, root_node_element;
size_t length_of_term;

if ((node_position = get_btree_leaf_position(term, &node_length, &exact_match, &root_node_element)) == 0)
	return NULL;		// before the first term in the term list

length_of_term = strlen(term);
if (length_of_term < B_TREE_PREFIX_SIZE)
	if (!exact_match)
		return NULL;		// we have a short string (less then the length of the head node) and did not find it as a node
	else
		term += length_of_term;
else
	if (strncmp(btree_root[root_node_element].term, term, B_TREE_PREFIX_SIZE) != 0)
		return NULL;		// there is no node in the list that starts with the head of the string.
	else
		term += B_TREE_PREFIX_SIZE;

index->seek(node_position);
index->read(btree_leaf_buffer, (long)node_length);
/*
	First 4 bytes are the number of terms in the node
	then there are N loads of:
		CF (4), DF (4), Offset_in_postings (8), DocIDs_Len (4), Postings_len (4), String_pos_in_node (4)
*/
low = 0;
high = nodes = (long)ANT_get_long(btree_leaf_buffer);
// length of a leaf node (sum of cf, df, etc. sizes)
leaf_size = ANT_btree_iterator::LEAF_SIZE;

while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp((char *)(btree_leaf_buffer + ANT_get_long(btree_leaf_buffer + (leaf_size * (mid + 1)))), term) < 0)
		low = mid + 1;
	else
		high = mid;
	}
if ((low < nodes) && (strcmp((char *)(btree_leaf_buffer + ANT_get_long(btree_leaf_buffer + (leaf_size * (low + 1)))), term) == 0))
	return get_leaf(btree_leaf_buffer, low, term_details);
else
	return NULL;
}


#ifdef IMPACT_HEADER
/*
	ANT_SEARCH_ENGINE::GET_ONE_QUANTUM()
	-------------------------------------
*/
unsigned char *ANT_search_engine::get_one_quantum(ANT_search_engine_btree_leaf *term_details, ANT_impact_header *the_impact_header, ANT_quantum *the_quantum, unsigned char *destination)
{
#ifdef SPECIAL_COMPRESSION
	if (term_details->local_document_frequency <= 2)
		{
		ANT_compressable_integer *postings;

		// first byte of a quantum is the compression scheme, no compression for special compression
		*destination = 0;
		postings = (ANT_compressable_integer *)(destination + 1);

		// fill in postings
		if (the_impact_header->the_quantum_count == 1)
			{
			*postings++ = term_details->postings_position_on_disk >> 32;
			if (term_details->local_document_frequency == 2)
				*postings = (ANT_compressable_integer)term_details->impacted_length;
			term_details->impacted_length = term_details->local_document_frequency;
			}
		else
			{
			*postings++ = term_details->postings_position_on_disk >> 32; // first docid
			*(unsigned char *)postings = 0; // no compression for second docid list
			postings = (ANT_compressable_integer *)((unsigned char *)postings + 1); // move past the compression scheme byte
			*postings = (ANT_compressable_integer)term_details->impacted_length; // second docid
			term_details->impacted_length = 2;
			}
		}
	else
		{
		index->seek(term_details->postings_position_on_disk + the_impact_header->beginning_of_the_postings + (long long)the_quantum->offset);
		destination = index->read_return_ptr(destination, term_details->postings_length - the_impact_header->beginning_of_the_postings - (long long)the_quantum->offset);
		}
#else
	index->seek(term_details->postings_position_on_disk + the_impact_header->beginning_of_the_postings + the_quantum->offset);
	destination = index->read_return_ptr(destination, term_details->postings_length - the_impact_header->beginning_of_the_postings - the_quantum->offset);
#endif // end of #ifdef SPECIAL_COMPRESSION

if (!destination)
	exit(printf("Error reading from index\n"));

return destination;
}

/*
	ANT_SEARCH_ENGINE::GET_IMPACT_HEADER()
	--------------------------------------
*/
unsigned char *ANT_search_engine::get_impact_header(ANT_search_engine_btree_leaf *term_details, unsigned char *destination)
{
#ifdef SPECIAL_COMPRESSION
	ANT_compressable_integer *into;
	if (term_details->local_document_frequency <= 2)
		{
		quantum_count_type the_quantum_count = 1;
		beginning_of_the_postings_type beginning_of_the_postings = ANT_impact_header::INFO_SIZE + 1;
		ANT_compressable_integer impact_one = term_details->postings_position_on_disk & 0xFFFFFFFF;
		ANT_compressable_integer impact_two = (ANT_compressable_integer)term_details->postings_length;

		if (impact_one != impact_two && term_details->local_document_frequency == 2)
			the_quantum_count = 2;

		beginning_of_the_postings += the_quantum_count * 3 * sizeof(ANT_compressable_integer);

		ANT_impact_header::set_quantum_count(destination, the_quantum_count);
		ANT_impact_header::set_beginning_of_the_postings(destination, beginning_of_the_postings);

		*(destination + ANT_impact_header::INFO_SIZE) = 0; // no compression for the header
		into = (ANT_compressable_integer *)(destination + ANT_impact_header::INFO_SIZE + 1); // the beginning of the header

		// impacts
		*into++ = impact_one;
		if (the_quantum_count == 1)
			{
			// document counts
			*into++ = (ANT_compressable_integer)term_details->local_document_frequency;
			// offsets
			*into++ = 0;
			}
		else
			{
			*into++ = impact_two;
			// document counts
			*into++ = 1;
			*into++ = 1;
			// offsets
			*into++ = 0;
			*into++ = sizeof(ANT_compressable_integer) + 1; // +1 to cover byte for compression scheme
			}
		}
	else
		{
		index->seek(term_details->postings_position_on_disk);
		// FIXME
		destination = index->read_return_ptr(destination, term_details->postings_length);
		}
#else
	index->seek(term_details->postings_position_on_disk);
	// FIXME
	destination = index->read_return_ptr(destination, term_details->postings_length);

#endif // end of #ifdef SPECIAL_COMPRESSION

if (!destination)
	exit(printf("Error reading from index\n"));

return destination;
}
#endif // end of #ifdef IMPACT_HEADER

/*
	ANT_SEARCH_ENGINE::GET_POSTINGS()
	---------------------------------
*/
unsigned char *ANT_search_engine::get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination)
{
#ifdef SPECIAL_COMPRESSION
	ANT_compressable_integer *into;
	if (term_details->local_document_frequency <= 2)
		{
		/*
			We can't return a pointer to a location in the file because there isn't one... so we have to decode the postings and put them somewhere.
		*/
		if (special_compression_buffer != NULL)
			destination = special_compression_buffer;

#ifdef IMPACT_HEADER
		ANT_compressable_integer *postings;
		quantum_count_type the_quantum_count = 1;
		beginning_of_the_postings_type beginning_of_the_postings = ANT_impact_header::INFO_SIZE + 1;
		ANT_compressable_integer impact_one = term_details->postings_position_on_disk & 0xFFFFFFFF;
		ANT_compressable_integer impact_two = (ANT_compressable_integer)term_details->postings_length;

		if (impact_one != impact_two && term_details->local_document_frequency == 2)
			the_quantum_count = 2;

		beginning_of_the_postings += the_quantum_count * 3 * sizeof(ANT_compressable_integer);

		ANT_impact_header::set_quantum_count(destination, the_quantum_count);
		ANT_impact_header::set_beginning_of_the_postings(destination, beginning_of_the_postings);

		*(destination + ANT_impact_header::INFO_SIZE) = 0; // no compression for the header
		into = (ANT_compressable_integer *)(destination + ANT_impact_header::INFO_SIZE + 1); // the beginning of the header

		*(destination + beginning_of_the_postings) = 0; // no compression for the postings
		postings = (ANT_compressable_integer *)(destination + beginning_of_the_postings + 1);

		// impacts
		*into++ = impact_one;
		if (the_quantum_count == 1)
			{
			// document counts
			*into++ = (ANT_compressable_integer)term_details->local_document_frequency;
			// offsets
			*into++ = 0;
			// fill in postings
			*postings++ = term_details->postings_position_on_disk >> 32;
			if (term_details->local_document_frequency == 2)
				*postings = (ANT_compressable_integer)term_details->impacted_length;
			term_details->impacted_length = term_details->local_document_frequency;
			}
		else
			{
			*into++ = impact_two;
			// document counts
			*into++ = 1;
			*into++ = 1;
			// offsets
			*into++ = 0;
			*into++ = sizeof(ANT_compressable_integer) + 1; // +1 to cover byte for compression scheme
			// fill the postings
			*postings++ = term_details->postings_position_on_disk >> 32; // first docid
			*(unsigned char *)postings = 0; // no compression for second docid list
			postings = (ANT_compressable_integer *)((unsigned char *)postings + 1); // move past the compression scheme byte
			*postings = (ANT_compressable_integer)term_details->impacted_length; // second docid
			term_details->impacted_length = 2;
			}
#else
		/*
			We're about to generate the impact-ordering here and so we interlace TF, DOC-ID and 0s
		*/
		*destination = 0;		// no compression
		into = (ANT_compressable_integer *)(destination + 1);
		*into++ = term_details->postings_position_on_disk & 0xFFFFFFFF;
		*into++ = term_details->postings_position_on_disk >> 32;
		/*
			Need to distinguish the cases where we have
			one tf value with one docid, or two tf values with two docids (this if)
			or one tf value with two docids (else)
		*/
		if (term_details->postings_length != *(into - 2))
			{
			*into++ = 0;
			*into++ = (ANT_compressable_integer)term_details->postings_length;
			*into++ = (ANT_compressable_integer)term_details->impacted_length;
			*into++ = 0;
			term_details->impacted_length = term_details->local_document_frequency == 2 ? 6 : 3;
			}
		else
			{
			*into++ = (ANT_compressable_integer)term_details->impacted_length;
			*into++ = 0;
			term_details->impacted_length = 4;
			}
#endif
		}
	else
		{
		index->seek(term_details->postings_position_on_disk);
		destination = index->read_return_ptr(destination, term_details->postings_length);
		}
#else
	index->seek(term_details->postings_position_on_disk);
	destination = index->read_return_ptr(destination, term_details->postings_length);

#endif

if (!destination)
	exit(printf("Error reading from index\n"));

return destination;
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_TERM()
	-------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine::process_one_term(char *term, ANT_search_engine_btree_leaf *term_details)
{
ANT_search_engine_btree_leaf *verify;
long long now, bytes_already_read;

bytes_already_read = index->get_bytes_read();

now = stats->start_timer();

/*
	If we have a stemmed index and the length of the term is greater then 3 then stem the search term
*/
if (stemmer != NULL && ANT_islower(*term) && (*(term + 1) != '\0' && *(term + 2) != '\0'))
	{
	stemmer->stem(term, stemmed_term);
	term = stemmed_term;
	}

/*
	Read the term details out of the vocab
*/
verify = get_postings_details(term, term_details);
stats->add_dictionary_lookup_time(stats->stop_timer(now));
if (verify == NULL)
	{
	/*
		The term was not found so set the collection frequency and document frequency to 0
	*/
	term_details->local_collection_frequency = 0;
	term_details->global_collection_frequency = 0;
	term_details->local_document_frequency = 0;
	term_details->global_document_frequency = 0;
	}

stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);

return verify;
}

/*
	ANT_SEARCH_ENGINE::GET_COLLECTION_FREQUENCY()
	---------------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine::get_collection_frequency(char *base_term, ANT_stem *stem_maker, ANT_search_engine_btree_leaf *stemmed_term_details)
{
char *term;
ANT_search_engine_btree_leaf term_details;
ANT_stemmer_stem stemmer(this, stem_maker);
long long collection_frequency = 0;

/*
	If the index is stemmed then there is no work to do.
*/
if (this->stemmer != NULL || stem_maker == NULL)
	return process_one_term(base_term, stemmed_term_details);

if (!ANT_islower(base_term[0]) || base_term[1] == '\0' || base_term[2] == '\0')
	return process_one_term(base_term, stemmed_term_details);			// don't stem as either not a stemmable word or else too short

/*
	Find the first term that matches the stem
*/
term = stemmer.first(base_term);

while (term != NULL)
	{
	stemmer.get_postings_details(&term_details);
	collection_frequency += term_details.local_collection_frequency;
	term = stemmer.next();
	}

/*
	Produce the result
*/
stemmed_term_details->local_collection_frequency = collection_frequency;
stemmed_term_details->global_collection_frequency = collection_frequency;
stemmed_term_details->local_document_frequency = 0;
stemmed_term_details->global_document_frequency = 0;

return stemmed_term_details;
}

#ifdef IMPACT_HEADER
/*
	ANT_SEARCH_ENGINE::READ_AND_DECOMPRESS_FOR_ONE_QUANTUM()
	--------------------------------------------------------
*/
void *ANT_search_engine::read_and_decompress_for_one_quantum(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_impact_header *the_impact_header, ANT_quantum *the_quantum, ANT_compressable_integer *the_decompressed_buffer)
{
void *verify = NULL;
long long now, bytes_already_read;

if (term_details != NULL && term_details->local_document_frequency > 0)
	{
	bytes_already_read = index->get_bytes_read();

	now = stats->start_timer();
	verify = raw_postings_buffer = get_one_quantum(term_details, the_impact_header, the_quantum, raw_postings_buffer);
	stats->add_posting_read_time(stats->stop_timer(now));

	/*
		Make sure the postings_buffer is not empty and then decompress it
	*/
	if (verify != NULL)
		{
		now = stats->start_timer();
		factory.decompress(the_decompressed_buffer, raw_postings_buffer, the_quantum->doc_count);
		stats->add_decompress_time(stats->stop_timer(now));
		}
	stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);
	}

return verify;
}

/*
	ANT_SEARCH_ENGINE::READ_AND_DECOMPRESS_FOR_ONE_IMPACT_HEADER()
	--------------------------------------------------------------
*/
void *ANT_search_engine::read_and_decompress_for_one_impact_header(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_impact_header *the_impact_header)
{
void *verify = NULL;
long long now, bytes_already_read;

if (term_details != NULL && term_details->local_document_frequency > 0)
	{
	bytes_already_read = index->get_bytes_read();

	now = stats->start_timer();
	verify = raw_postings_buffer = get_impact_header(term_details, raw_postings_buffer);
	stats->add_posting_read_time(stats->stop_timer(now));

	/*
		Make sure the postings_buffer is not empty and then decompress it
	*/
	if (verify != NULL)
		{
		now = stats->start_timer();

		/*
		 Decompress the header
		 */
		the_impact_header->the_quantum_count = ANT_impact_header::get_quantum_count(raw_postings_buffer);
		the_impact_header->beginning_of_the_postings = ANT_impact_header::get_beginning_of_the_postings(raw_postings_buffer);
		factory.decompress(the_impact_header->header_buffer, raw_postings_buffer + ANT_impact_header::INFO_SIZE, the_impact_header->the_quantum_count * 3);
		the_impact_header->impact_value_start = the_impact_header->header_buffer;
		the_impact_header->doc_count_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count;
		the_impact_header->impact_offset_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count * 2;

		stats->add_decompress_time(stats->stop_timer(now));
		}
	stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);
	}

return verify;
}
#endif // end of #ifdef IMPACT_HEADER


#ifdef IMPACT_HEADER
/*
	ANT_SEARCH_ENGINE::READ_AND_DECOMPRESS_FOR_ONE_TERM()
	-----------------------------------------------------
*/
void *ANT_search_engine::read_and_decompress_for_one_term(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_impact_header *the_impact_header, ANT_compressable_integer *the_decompressed_buffer)
#else
void *ANT_search_engine::read_and_decompress_for_one_term(ANT_search_engine_btree_leaf *term_details, unsigned char *raw_postings_buffer, ANT_compressable_integer *the_decompressed_buffer)
#endif
{
void *verify = NULL;
long long now, bytes_already_read;

if (term_details != NULL && term_details->local_document_frequency > 0)
	{
	trim_postings_k = ANT_min(this->trim_postings_k, this->global_trim_postings_k);
	bytes_already_read = index->get_bytes_read();
	now = stats->start_timer();

#ifndef TOP_K_READ_AND_DECOMPRESSOR
	verify = raw_postings_buffer = get_postings(term_details, raw_postings_buffer);
#else
	/*
		The maximum number of bytes we need to read is the worst case for compression * the cost of the
		tf and 0 bytes (of which there are at most 510).  So we need 5 * df + 510 at worst (for 32 bit integers)
	*/
	long long bytes;

	if (term_details->local_document_frequency > trim_postings_k)
		{
		#ifdef IMPACT_HEADER
			bytes = ANT_impact_header::INFO_SIZE + 5 * ANT_impact_header::NUM_OF_QUANTUMS * 3 + 5 * trim_postings_k;
		#else
			bytes = 510 + 5 * trim_postings_k;
		#endif
		if (term_details->postings_length > bytes)
			term_details->postings_length = bytes;
		}

	verify = raw_postings_buffer = get_postings(term_details, raw_postings_buffer);
#endif // end of TOP_K_READ_AND_DECOMPRESSOR
	stats->add_posting_read_time(stats->stop_timer(now));

	/*
		Make sure the postings_buffer is not empty and then decompress it
	*/
	if (verify != NULL)
		{
		now = stats->start_timer();
#ifndef TOP_K_READ_AND_DECOMPRESSOR
	#ifdef IMPACT_HEADER
		/*
			Decompress the header
		*/
		the_impact_header->the_quantum_count = ANT_impact_header::get_quantum_count(raw_postings_buffer);
		the_impact_header->beginning_of_the_postings = ANT_impact_header::get_beginning_of_the_postings(raw_postings_buffer);
		factory.decompress(the_impact_header->header_buffer, raw_postings_buffer + ANT_impact_header::INFO_SIZE, the_impact_header->the_quantum_count * 3);
		the_impact_header->impact_value_start = the_impact_header->header_buffer;
		the_impact_header->doc_count_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count;
		the_impact_header->impact_offset_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count * 2;

		/*
			Decompress the postings
		*/
		long long sum = 0;
		the_impact_header->doc_count_ptr = the_impact_header->doc_count_start;
		the_impact_header->impact_offset_ptr = the_impact_header->impact_offset_start;
		while (the_impact_header->doc_count_ptr < the_impact_header->impact_offset_start)
			{

			factory.decompress(the_decompressed_buffer + sum, raw_postings_buffer + the_impact_header->beginning_of_the_postings + *the_impact_header->impact_offset_ptr, *the_impact_header->doc_count_ptr);
			sum += *the_impact_header->doc_count_ptr;
			the_impact_header->doc_count_ptr++;
			the_impact_header->impact_offset_ptr++;
			}
		the_impact_header->doc_count_trim_ptr = the_impact_header->doc_count_ptr;
	#else // else of #ifdef IMPACT_HEADER
		factory.decompress(the_decompressed_buffer, raw_postings_buffer, term_details->impacted_length);
	#endif // end of #ifdef IMPACT_HEADER
#else
		/*
			The maximum number of postings we need to decompress in the worst case is the case where each posting has
			a different quantised impact... that is 3*n.  But, there are only 255 possible impacts and so if n > 255
			then it is n + 510 (remember, we can't use 0 as an impact).  Of course we never decompress more postings
			than there are.
		*/
		long long end;

		if (term_details->local_document_frequency <= trim_postings_k)
			end = term_details->impacted_length;
		else
			{
			end = trim_postings_k < 0xFF ? trim_postings_k * 3 : trim_postings_k + 510;
			if (end > term_details->impacted_length)
				end = term_details->impacted_length;
			}

		#ifdef IMPACT_HEADER
			/*
				Decompress the header
			*/
			the_impact_header->the_quantum_count = ANT_impact_header::get_quantum_count(raw_postings_buffer);
			the_impact_header->beginning_of_the_postings = ANT_impact_header::get_beginning_of_the_postings(raw_postings_buffer);
			factory.decompress(the_impact_header->header_buffer, raw_postings_buffer + ANT_impact_header::INFO_SIZE, the_impact_header->the_quantum_count * 3);
			the_impact_header->impact_value_start = the_impact_header->header_buffer;
			the_impact_header->doc_count_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count;
			the_impact_header->impact_offset_start = the_impact_header->header_buffer + the_impact_header->the_quantum_count * 2;

			/*
				Decompress the postings
			*/
			long long sum = 0;
			the_impact_header->doc_count_ptr = the_impact_header->doc_count_start;
			the_impact_header->impact_offset_ptr = the_impact_header->impact_offset_start;
			while (the_impact_header->doc_count_ptr < the_impact_header->impact_offset_start)
				{
				if (sum >= trim_postings_k)
					break;

				/*
					Because we can interrupt a quantum part way through (following non-impact header logic),
					then we want to check, and modify the header so that future processing doesn't go awry
				*/
				if (*the_impact_header->doc_count_ptr > end)
					*the_impact_header->doc_count_ptr = (ANT_compressable_integer)end;

				factory.decompress(the_decompressed_buffer + sum, raw_postings_buffer + the_impact_header->beginning_of_the_postings + *the_impact_header->impact_offset_ptr, *the_impact_header->doc_count_ptr);
				sum += *the_impact_header->doc_count_ptr;
				end -= *the_impact_header->doc_count_ptr;
				the_impact_header->doc_count_ptr++;
				the_impact_header->impact_offset_ptr++;
				}
			the_impact_header->doc_count_trim_ptr = the_impact_header->doc_count_ptr;
		#else // else #ifdef IMPACT_HEADER
			factory.decompress(the_decompressed_buffer, raw_postings_buffer, end);
			the_decompressed_buffer[end] = 0;			// and now 0 terminate the list so that searching terminates
		#endif // end of #ifdef IMPACT_HEADER
#endif // end of #ifdef TOP_K_READ_AND_DECOMPRESSOR
		stats->add_decompress_time(stats->stop_timer(now));
		}
	stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);
	}

return verify;
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_TERM_DETAIL()
	--------------------------------------------
*/
void ANT_search_engine::process_one_term_detail(ANT_search_engine_btree_leaf *term_details, ANT_ranking_function *ranking_function, double prescalar, double postscalar, double query_frequency, ANT_bitstring *bitstring)
{
void *verify;
long long now;

#ifdef IMPACT_HEADER
verify = read_and_decompress_for_one_term(term_details, postings_buffer, &impact_header, decompress_buffer);
#else
verify = read_and_decompress_for_one_term(term_details, postings_buffer, decompress_buffer);
#endif

//
// make sure the postings_buffer is not empty and then decompress it
//
if (verify != NULL)
	{
	now = stats->start_timer();
	if (bitstring == NULL)
		{ // it bitstring != NULL then we're boolean ranking hybrid
		#ifdef IMPACT_HEADER
			ranking_function->relevance_rank_top_k(results_list, term_details, &impact_header, decompress_buffer, trim_postings_k, prescalar, postscalar, query_frequency);
			//ranking_function->relevance_rank_quantum(results_list, term_details, &impact_header, decompress_buffer, trim_postings_k);
		#else
			ranking_function->relevance_rank_top_k(results_list, term_details, decompress_buffer, trim_postings_k, prescalar, postscalar, query_frequency);
		#endif
		}
	else
		{
		#ifdef IMPACT_HEADER
			ranking_function->relevance_rank_boolean(bitstring, results_list, term_details, &impact_header, decompress_buffer, trim_postings_k, prescalar, postscalar, query_frequency);
		#else
			ranking_function->relevance_rank_boolean(bitstring, results_list, term_details, decompress_buffer, trim_postings_k, prescalar, postscalar, query_frequency);
		#endif
		}
	stats->add_rank_time(stats->stop_timer(now));
	}
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_SEARCH_TERM()
	--------------------------------------------
*/
void ANT_search_engine::process_one_search_term(char *term, ANT_ranking_function *ranking_function, double prescalar, double postscalar, double query_frequency, ANT_bitstring *bitstring)
{
ANT_search_engine_btree_leaf term_details;

process_one_term_detail(process_one_term(term, &term_details), ranking_function, prescalar, postscalar, query_frequency, bitstring);
}

/*
	ANT_SEARCH_ENGINE::PLACE_INTO_INTERNAL_BUFFERS()
	------------------------------------------------
	returns collection_frequency or 0 on error
*/
#ifdef USE_FLOATED_TF
	long long ANT_search_engine::place_into_internal_buffers(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf term_frequency_weight)
#else
	long long ANT_search_engine::place_into_internal_buffers(ANT_search_engine_btree_leaf *term_details)
#endif
{
void *verify;
long long now;
ANT_compressable_integer *current_document, *end;
long document;
ANT_compressable_integer term_frequency;

/*
	load the postings from disk
*/
now = stats->start_timer();
verify = postings_buffer = get_postings(term_details, postings_buffer);
stats->add_posting_read_time(stats->stop_timer(now));
if (verify == NULL)			// something has gone wrong
	return 0;

/*
	Decompress the postings
*/
now = stats->start_timer();
#ifdef IMPACT_HEADER
// decompress the header
impact_header.the_quantum_count = ANT_impact_header::get_quantum_count(postings_buffer);
impact_header.beginning_of_the_postings = ANT_impact_header::get_beginning_of_the_postings(postings_buffer);
factory.decompress(impact_header.header_buffer, postings_buffer+ANT_impact_header::INFO_SIZE, impact_header.the_quantum_count * 3);
impact_header.impact_value_start = impact_header.header_buffer;
impact_header.doc_count_start = impact_header.header_buffer + impact_header.the_quantum_count;
impact_header.impact_offset_start = impact_header.header_buffer + impact_header.the_quantum_count * 2;

// decompress the postings
long long sum = 0;
//end = impact_header.doc_count_start + impact_header.the_quantum_count;
impact_header.doc_count_ptr = impact_header.doc_count_start;
impact_header.impact_offset_ptr = impact_header.impact_offset_start;
// impact_offset_start is the end of doc_count
while (impact_header.doc_count_ptr < impact_header.impact_offset_start) {
	factory.decompress(decompress_buffer + sum,
					   postings_buffer+ impact_header.beginning_of_the_postings + *impact_header.impact_offset_ptr,
					   *impact_header.doc_count_ptr);
	sum += *impact_header.doc_count_ptr;
	impact_header.doc_count_ptr++;
	impact_header.impact_offset_ptr++;
}
#else
factory.decompress(decompress_buffer, postings_buffer, term_details->impacted_length);
#endif
stats->add_decompress_time(stats->stop_timer(now));

/*
	Process the postings list
*/
now = stats->start_timer();
#ifdef IMPACT_HEADER
impact_header.impact_value_ptr = impact_header.impact_value_start;
impact_header.doc_count_ptr = impact_header.doc_count_start;
current_document = decompress_buffer;
// impact_offset_start is the end of the doc_count
while(impact_header.doc_count_ptr < impact_header.impact_offset_start) {
	term_frequency = *impact_header.impact_value_ptr;
	document = -1;
	end = current_document + *impact_header.doc_count_ptr;
	while (current_document < end) {
		document += *current_document++;
		/*
			Only weight TFs if we're using floats, this makes no sense for integer tfs.
		*/
		#ifdef USE_FLOATED_TF
			stem_buffer[document] += term_frequency * term_frequency_weight;
		#else
			stem_buffer[document] += term_frequency;
		#endif
	}
	current_document = end;
	impact_header.impact_value_ptr++;
	impact_header.doc_count_ptr++;
}
#else
current_document = decompress_buffer;
end = decompress_buffer + term_details->impacted_length;
while (current_document < end)
	{
	term_frequency = *current_document++;
	document = -1;
	while (*current_document != 0)
		{
		document += *current_document++;
		/*
			Only weight TFs if we're using floats, this makes no sense for integer tfs.
		*/
		#ifdef USE_FLOATED_TF
			stem_buffer[document] += term_frequency * term_frequency_weight;
		#else
			stem_buffer[document] += term_frequency;
		#endif
		}
	current_document++;
	}
#endif
stats->add_stemming_time(stats->stop_timer(now));

return term_details->local_collection_frequency;
}

/*
	ANT_SEARCH_ENGINE::STEM_INTO_INTERNAL_BUFFERS()
	-----------------------------------------------
	returns 0 on error else cf
*/
long long ANT_search_engine::stem_into_internal_buffers(ANT_stemmer *stemmer, char *base_term)
{
ANT_search_engine_btree_leaf term_details;
long long now, collection_frequency;
char *term;
#ifdef USE_FLOATED_TF
	ANT_weighted_tf tf_weight;
	long weight_terms;
#endif

/*
	Initialise cf, which we return later.
*/
collection_frequency = 0;

/*
	Find the first term that matches the stem
*/
now = stats->start_timer();
term = stemmer->first(base_term);
stats->add_dictionary_lookup_time(stats->stop_timer(now));

while (term != NULL)
	{
	/*
		get the location of the postings on disk
	*/
	now = stats->start_timer();
	stemmer->get_postings_details(&term_details);
	stats->add_dictionary_lookup_time(stats->stop_timer(now));

	/*
		Should the term be weighted?
	*/
#ifdef USE_FLOATED_TF
	weight_terms = stemmer->weight_terms(&tf_weight, term);
#endif

	/*
		Load the postings from disk then process into the internal buffers
	*/
#ifdef USE_FLOATED_TF
	collection_frequency += place_into_internal_buffers(&term_details, weight_terms ? tf_weight : 1);
#else
	collection_frequency += place_into_internal_buffers(&term_details);
#endif

	/*
		Now move on to the next term that matches the stem
	*/
	now = stats->start_timer();
	term = stemmer->next();
	stats->add_dictionary_lookup_time(stats->stop_timer(now));
	}

return collection_frequency;
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_STEMMED_SEARCH_TERM()
	----------------------------------------------------
*/
void ANT_search_engine::process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function, double prescalar, double postscalar, double query_frequency, ANT_bitstring *bitstring)
{
long long bytes_already_read;
ANT_search_engine_btree_leaf stemmed_term_details;
long long now, collection_frequency;

/*
	The way we stem is to load the terms that match the stem one at a time and to
	accumulate the term frequences into the stem_buffer accumulator list.  This then
	gets converted into a postings list and processed (ranked) as if a single search
	term within the search engine.
*/
bytes_already_read = index->get_bytes_read();

/*
	Initialise the term_frequency accumulators to all zero.
*/
now = stats->start_timer();
memset(stem_buffer, 0, (size_t)stem_buffer_length_in_bytes);
stats->add_stemming_time(stats->stop_timer(now));

collection_frequency = stem_into_internal_buffers(stemmer, base_term);

/*
	Finally, if we had no problem loading the search terms then
	do the relevance ranking as if it was a single search term
*/
if (collection_frequency != 0)
	{
	now = stats->start_timer();
	stemmed_term_details.local_collection_frequency = collection_frequency;
	ranking_function->relevance_rank_tf(bitstring, results_list, &stemmed_term_details, stem_buffer, ANT_min(trim_postings_k, global_trim_postings_k), prescalar, postscalar, query_frequency);
	stats->add_rank_time(stats->stop_timer(now));
	}

/*
	Update the stats
*/
stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_THESAURUS_SEARCH_TERM()
	------------------------------------------------------
*/
void ANT_search_engine::process_one_thesaurus_search_term(ANT_thesaurus *expander, ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function, double prescalar, double postscalar, double query_frequency, ANT_bitstring *bitstring)
{
ANT_thesaurus_relationship *expansion;
void *verify;
long long bytes_already_read, number_of_terms_in_expansion;
ANT_search_engine_btree_leaf term_details, stemmed_term_details;
long long now, collection_frequency;
char *term;

/*
	Thesaurus expansion in this code is treated as synonym conflation which is to
	combine all the postings lists for all the synonyms into one postings lists and
	to take the term frequencies and document frequencies form that
*/
verify = NULL;
bytes_already_read = index->get_bytes_read();

/*
	Initialise the term_frequency accumulators to all zero.
	Then do the term expansion
*/
now = stats->start_timer();
memset(stem_buffer, 0, (size_t)stem_buffer_length_in_bytes);
collection_frequency = 0;
expansion = expander->get_synset(base_term, &number_of_terms_in_expansion);
stats->add_thesaurus_time(stats->stop_timer(now));

/*
	Make sure there is an expansion
*/
if (number_of_terms_in_expansion == 0)
	{
	/*
		No term expansion so we fall-back to the regular search process because
		its more efficient than decoding and re-encoding
	*/
	if (stemmer == NULL)
		process_one_search_term(base_term, ranking_function, prescalar, postscalar, query_frequency, bitstring);
	else
		process_one_stemmed_search_term(stemmer, base_term, ranking_function, prescalar, postscalar, query_frequency, bitstring);
	return;
	}

/*
	The first term is the keyword itself because a term is not a synonym of itself
*/
term = base_term;

while (term != NULL)
	{
	if (stemmer != NULL)
		{
		/*
			Stem this term into the internal tf-buffer
		*/
		collection_frequency += stem_into_internal_buffers(stemmer, term);
		}
	else
		{
		/*
			get the location of the postings on disk
		*/
		now = stats->start_timer();
		verify = process_one_term(term, &term_details);		// if the index is stemmed then this will stem the expanded term
		stats->add_dictionary_lookup_time(stats->stop_timer(now));
		/*
			process into the internal tf-buffer
		*/
		if (verify != NULL)		// else the term expanded to something not in the vocab!
			collection_frequency += ANT_search_engine::place_into_internal_buffers(&term_details);
		}

	/*
		Now move on to the next term
	*/
	term = expansion->term;
	expansion++;
	}

/*
	Finally, if we had no problem loading the search terms then
	do the relevance ranking as if it was a single search term
*/
if (collection_frequency != 0)
	{
	now = stats->start_timer();
	stemmed_term_details.local_collection_frequency = collection_frequency;
	ranking_function->relevance_rank_tf(bitstring, results_list, &stemmed_term_details, stem_buffer, ANT_min(trim_postings_k, global_trim_postings_k), prescalar, postscalar, query_frequency);
	stats->add_rank_time(stats->stop_timer(now));
	}

stats->add_disk_bytes_read_on_search(index->get_bytes_read() - bytes_already_read);
}

/*
	ANT_SEARCH_ENGINE::SORT_RESULTS_LIST()
	--------------------------------------
*/
ANT_search_engine_accumulator **ANT_search_engine::sort_results_list(long long accurate_rank_point, long long *hits)
{
long long now;

now = stats->start_timer();
*hits = results_list->init_pointers();
stats->add_count_relevant_documents(stats->stop_timer(now));

/*
	Sort the postings into decreasing order, but only guarantee the first accurate_rank_point postings are accurately ordered (top-k sorting)
*/
now = stats->start_timer();

//qsort(accumulator_pointers, documents, sizeof(*accumulator_pointers), ANT_search_engine_accumulator::compare_pointer);
ANT_search_engine_accumulator::top_k_sort(results_list->accumulator_pointers, *hits, accurate_rank_point);
stats->add_sort_time(stats->stop_timer(now));

return results_list->accumulator_pointers;
}

/*
	ANT_SEARCH_ENGINE::BOOLEAN_RESULTS_LIST()
	-----------------------------------------
*/
long long ANT_search_engine::boolean_results_list(long terms_in_query)
{
ANT_search_engine_accumulator **current, **end;
long long now, hits = 0;

now = stats->start_timer();

end = results_list->accumulator_pointers + results_list->results_list_length;

for (current = results_list->accumulator_pointers; current < end; current++)
	if ((*current)->get_rsv() < terms_in_query)
		(*current)->clear_rsv();
	else
		hits++;

results_list->results_list_length = hits;

stats->add_rank_time(stats->stop_timer(now));

return hits;
}

#ifdef FILENAME_INDEX
	/*
		ANT_SEARCH_ENGINE::GET_DOCUMENT_FILENAME()
		------------------------------------------
	*/
	char *ANT_search_engine::get_document_filename(char *filename, long long internal_document_id)
	{
	long long offset[2];

	/*
		Read the location of the filename
	*/
	index->seek(filename_index_start + internal_document_id * sizeof (long long));
	index->read((unsigned char *)&offset, sizeof(offset));

	/*
		Fix the byte order (for portability to ARM)
	*/
	offset[0] = ANT_get_long_long((unsigned char *)offset);
	offset[1] = ANT_get_long_long((unsigned char *)(offset + 1));

	/*
		Get the filename
	*/
	index->seek(filename_start + offset[0]);
	index->read((unsigned char *)filename, offset[1] - offset[0]);

	return filename;
	}
#else
	/*
		ANT_SEARCH_ENGINE::GENERATE_RESULTS_LIST()
		------------------------------------------
	*/
	char **ANT_search_engine::generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k)
	{
	ANT_search_engine_accumulator **current, **end;
	char **into = sorted_id_list;

	end = results_list->accumulator_pointers + (results_list->results_list_length < top_k ? results_list->results_list_length : top_k);
	for (current = results_list->accumulator_pointers; current < end; current++)
		{
		#ifdef NEVER
			/*
				Is this a hang over from way-back when we used to walk through the entire array?  Leaving it in means we can
				no longer find relevant documents with a zero rsv (which happens when a query contains one term and that term
				is in every document).
			*/
			if (results_list->is_zero_rsv(*current - results_list->accumulator))
				break;
			else
				*into++ = document_id_list[*current - results_list->accumulator];
		#else
			*into++ = document_id_list[*current - results_list->accumulator];
		#endif
		}

	return sorted_id_list;
	}
#endif

/*
	ANT_SEARCH_ENGINE::GET_VARIABLE()
	---------------------------------
*/
long long ANT_search_engine::get_variable(char *name)
{
ANT_search_engine_btree_leaf term_details;
unsigned long long answer;
ANT_compressable_integer *bits;

if ((bits = get_decompressed_postings(name, &term_details)) == NULL)
	answer = 0;
else
	{
#ifdef SPECIAL_COMPRESSION
	/*
		The sequence we'll get back is a postings list so it will be [2,<high32>,0,1,<low32>,0]
		from which we want to extract <high32> and <low32> the high and low 32 bits of the integer
		so that we can then construct a 64 bit integer from it.

		Unless we use impact headers in which case it's the same as below.
	*/
#ifdef IMPACT_HEADER
		answer = (((unsigned long long)bits[0]) << 32 | (unsigned long long)bits[1]);
#else
		answer = (((unsigned long long)bits[1]) << 32) | (unsigned long long)bits[4];
#endif
#else
	/*
		In the case where we don't have special compression we have shoved the value into a
		postings list and then compressed the postings list.  The consequence is that the result
		of decompression is *not* a valid postings list, but rather it *is* the number stored in
		2*32 bit integers.
	*/
	answer = ((unsigned long long)bits[0] << 32) | (unsigned long long) bits[1];
#endif
	}

return (long long)answer;
}

/*
	ANT_SEARCH_ENGINE::GET_DECOMPRESSED_POSTINGS()
	----------------------------------------------
*/
ANT_compressable_integer *ANT_search_engine::get_decompressed_postings(char *term, ANT_search_engine_btree_leaf *term_details)
{
void *verify;
long long now;
#ifdef IMPACT_HEADER
uint32_t beginning_of_postings = 0;
#endif

/*
	Find the term in the term vocab
*/
now = stats->start_timer();
verify = get_postings_details(term, term_details);
stats->add_dictionary_lookup_time(stats->stop_timer(now));
if (verify == NULL)
	return NULL;
/*
	Load the postings from disk
*/
now = stats->start_timer();
verify = postings_buffer = get_postings(term_details, postings_buffer);
stats->add_posting_read_time(stats->stop_timer(now));
if (verify == NULL)
	return NULL;

/*
	And now decompress
*/
now = stats->start_timer();
#ifdef IMPACT_HEADER
#ifdef SPECIAL_COMPRESSION
	/*
		Special compression assumes that we want an impact header back in get_postings, so builds it, but we don't want it so skip it
	*/
	beginning_of_postings = ((uint32_t *)postings_buffer)[5];
#else
	/*
		If we aren't using special compression, then the impact header doesn't exist for ~ terms
	*/
	if (*term != '~')
		beginning_of_postings = ((uint32_t *)postings_buffer)[5];
#endif

	factory.decompress(decompress_buffer, postings_buffer + beginning_of_postings, term_details->impacted_length);
#else
	factory.decompress(decompress_buffer, postings_buffer, term_details->impacted_length);
#endif
stats->add_decompress_time(stats->stop_timer(now));

return decompress_buffer;
}

/*
	ANT_SEARCH_ENGINE::GET_COMPRESSED_DOCUMENT()
	--------------------------------------------
	Note that this also contains the char saying which compression scheme is used as
	well as the compressed document, so caveat emptor
*/
char *ANT_search_engine::get_compressed_document(char *destination, unsigned long *destination_length, long long id)
{
long long start, end;

if (document_offsets == NULL || id >= document_count())
	{
	*destination = '\0';
	*destination_length = 0;
	return NULL;
	}

start = document_offsets[id];
end = document_offsets[id + 1];

index->seek(start);
index->read((unsigned char *)destination, end - start);

*destination_length = (unsigned long)(end - start);

return destination;
}

/*
	ANT_SEARCH_ENGINE::GET_DOCUMENT()
	---------------------------------
*/
char *ANT_search_engine::get_document(char *destination, unsigned long *destination_length, long long id)
{
long long start, end;

if (document_offsets == NULL || id >= document_count())
	{
	*destination = '\0';
	*destination_length = 0;
	return NULL;
	}

start = document_offsets[id];
end = document_offsets[id + 1];

index->seek(start);
index->read((unsigned char *)document_decompress_buffer, end - start);

document_factory.decompress(destination, destination_length, document_decompress_buffer, (unsigned long)(end - start));

return destination;
}

/*
	ANT_SEARCH_ENGINE::GET_DOCUMENTS()
	----------------------------------
*/
long long ANT_search_engine::get_documents(char **destination, unsigned long **destination_length, long long from, long long to)
{
long long start, end, times, get, id;
#ifdef FILENAME_INDEX
	ANT_search_engine_result_id_iterator current;
#else
	ANT_search_engine_result_iterator current;
#endif

if (document_offsets == NULL)
	return 0;

times = 0;
get = to - from;
#ifdef FILENAME_INDEX
for (id = current.first(this->results_list, from); id >= 0 && times < get; id = current.next())
#else
for (id = current.first(this, from); id >= 0 && times < get; id = current.next())
#endif
	{
	/*
		This isn't really the best way to do this but it will suffice in the mean time.  The best way is to
		read in increasing docid order as this reduces the number of disk seeks.
	*/
	times++;
	start = document_offsets[id];
	end = document_offsets[id + 1];

	index->seek(start);
	index->read((unsigned char *)document_decompress_buffer, end - start);

	document_factory.decompress(*destination, *destination_length, document_decompress_buffer, (unsigned long)(end - start));
	destination++;
	destination_length++;
	}

return times;
}

/*
	ANT_SEARCH_ENGINE::GET_DOCUMENT_FILENAMES()
	-------------------------------------------
*/
char **ANT_search_engine::get_document_filenames(char *buffer, unsigned long *buf_length)
{
long long start = get_variable("~documentfilenamesstart");
long long end = get_variable("~documentfilenamesfinish");
long long current_doc;
char *upto = buffer;
char **document_filenames = (char **)malloc((size_t)(sizeof(char *) * (end - start)));

*buf_length = (unsigned long)(end - start);

index->seek(start);
index->read((unsigned char *)buffer, (long long)(end - start));

for (current_doc = 0; current_doc < documents; current_doc++)
	{
	document_filenames[current_doc] = upto;
	upto += strlen(upto) + 1;
	}

return document_filenames;
}

