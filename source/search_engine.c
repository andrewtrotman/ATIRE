/*
	SEARCH_ENGINE.C
	---------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits>
#include "search_engine.h"
#include "file.h"
#include "memory.h"
#include "search_engine_btree_node.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "search_engine_stats.h"
#include "top_k_sort.h"
#include "stemmer.h"

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
ANT_search_engine::ANT_search_engine(ANT_memory *memory)
{
unsigned char *block;
long long end, term_header, this_header_block_size, sum, current_length, pointer;
long postings_buffer_length;
ANT_search_engine_btree_node *current, *end_of_node_list;
ANT_search_engine_btree_leaf collection_details;

stats = new ANT_search_engine_stats(memory);
this->memory = memory;
index = new ANT_file(memory);
if (index->open("index.aspt", "rb") == 0)
	exit(printf("Cannot open index file:index.aspt\n"));

/*
	At the end of the file is a "header" that provides various details:
	long long: the location of the b-tree header block
	long: the string length of the longest term
	long: the length of the longest compressed postings list
	long long: the maximum number of postings in a postings list (the highest DF)
*/
end = index->file_length();
index->seek(end - sizeof(term_header) - sizeof(string_length_of_longest_term) - sizeof(postings_buffer_length) - sizeof(highest_df));

index->read(&term_header);
index->read(&string_length_of_longest_term);
index->read(&postings_buffer_length);
index->read(&highest_df);

/*
	Load the b-tree header
*/
printf("Btree header is %lld bytes on disk\n", end - term_header);
index->seek(term_header);
block = (unsigned char *)memory->malloc((long)(end - term_header));
index->read(block, (long)(end - term_header));

/*
	The first sizeof(long long) bytes of the header are the number of nodes in the root
*/
btree_nodes = (long)(get_long_long(block) + 1);		// +1 because were going to add a sentinal at the start
printf("There are %ld nodes in the root of the btree\n", btree_nodes - 1);
block += sizeof(long long);
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
	current->disk_pos = get_long_long(block);
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
documents = collection_details.document_frequency;

postings_buffer = (unsigned char *)memory->malloc(postings_buffer_length);
memory->realign();

decompress_buffer = (ANT_compressable_integer *)memory->malloc(sizeof(*decompress_buffer) * (512 + highest_df));		// 512 because of the tf and 0 at each end of each impact ordered list
memory->realign();

document_lengths = (long *)memory->malloc(documents * sizeof(*document_lengths));

memory->realign();
accumulator = (ANT_search_engine_accumulator *)memory->malloc(sizeof(*accumulator) * documents);
memory->realign();
accumulator_pointers = (ANT_search_engine_accumulator **)memory->malloc(sizeof(*accumulator_pointers) * documents);
for (pointer = 0; pointer < documents; pointer++)
	accumulator_pointers[pointer] = &accumulator[pointer];

/*
	Here we allocate space for the decompressed postings.  Although it might appear as though 
	you only need enough space for the longest postings list, the lists can get longer than that
	due to stemming.  Space for each document is, therefore, needed.
*/
memory->realign();
posting.docid = (ANT_compressable_integer *)memory->malloc((size_t)(sizeof(*posting.docid) * documents));
memory->realign();
posting.tf = (ANT_compressable_integer *)memory->malloc((size_t)(sizeof(*posting.tf) * documents));

/*
	decompress the document length vector
*/
get_postings(&collection_details, postings_buffer);
factory.decompress(posting.docid, postings_buffer, collection_details.document_frequency);

sum = 0;
for (current_length = 0; current_length < documents; current_length++)
	sum += document_lengths[current_length] = posting.docid[current_length];
mean_document_length = (double)sum / (double)documents;

memory->realign();
stem_buffer = (long *)memory->malloc(stem_buffer_length_in_bytes = (sizeof(*stem_buffer) * documents));
}

/*
	ANT_SEARCH_ENGINE::~ANT_SEARCH_ENGINE()
	---------------------------------------
*/
ANT_search_engine::~ANT_search_engine()
{
index->close();
delete index;
delete stats;
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
	ANT_SEARCH_ENGINE::STATS_INITIALISE()
	-------------------------------------
*/
void ANT_search_engine::stats_initialise(void)
{
stats->initialise();
}

/*
	ANT_SEARCH_ENGINE::INIT_ACCUMULATORS()
	--------------------------------------
*/
void ANT_search_engine::init_accumulators(void)
{
long long now;

now = stats->start_timer();
memset(accumulator, 0, (size_t)(sizeof(*accumulator) * documents));
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

leaf_size = 28;		// length of a leaf node (sum of cf, df, etc. sizes)
base = leaf + leaf_size * term_in_leaf + sizeof(long);		// sizeof(long) is for the number of terms in the node
term_details->collection_frequency = get_long(base);
term_details->document_frequency = get_long(base + 4);
term_details->postings_position_on_disk = get_long_long(base + 8);
term_details->impacted_length = get_long(base + 16);
term_details->postings_length = get_long(base + 20);

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
high = nodes = (long)get_long(btree_leaf_buffer);
leaf_size = 28;		// length of a leaf node (sum of cf, df, etc. sizes)

while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp((char *)(btree_leaf_buffer + get_long(btree_leaf_buffer + (leaf_size * (mid + 1)))), term) < 0)
		low = mid + 1;
	else
		high = mid;
	}
if ((low < nodes) && (strcmp((char *)(btree_leaf_buffer + get_long(btree_leaf_buffer + (leaf_size * (low + 1)))), term) == 0))
	return get_leaf(btree_leaf_buffer, low, term_details);
else
	return NULL;
}

/*
	ANT_SEARCH_ENGINE::GET_POSTINGS()
	---------------------------------
*/
unsigned char *ANT_search_engine::get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination)
{
index->seek(term_details->postings_position_on_disk);
index->read(destination, term_details->postings_length);

return destination;
}

/*
	ANT_SEARCH_ENGINE::DECOMPRESS()
	-------------------------------
*/
void ANT_search_engine::decompress(unsigned char *start, ANT_search_engine_btree_leaf *leaf)
{
ANT_compressable_integer *current, term_frequency, *docid, *tf, *end;
long sum;

/*
	Decompress using one of the factory methods
*/
factory.decompress(decompress_buffer, start, leaf->document_frequency);

/*
	Convert from impact order into two arrays, one for docids, the other for tf values;
*/
docid = posting.docid;
tf = posting.tf;
end = decompress_buffer + leaf->impacted_length;
current = decompress_buffer;
while (current < end)
	{
	term_frequency = *current;
	current++;
	sum = -1;
	while (*current != 0)
		{
		sum += *current;			// because we are difference encoded
		*docid++ = sum;
		*tf++ = term_frequency;
		current++;
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_SEARCH_ENGINE::RELEVANCE_RANK()
	-----------------------------------
	BM25 relevance ranking by default (this is a virtual function)
	If you declare FIT_BM25 then you can tune BM25 by doing a grid search.
*/
#ifdef FIT_BM25
	double BM25_k1;
	double BM25_b;
#endif 
void ANT_search_engine::relevance_rank(ANT_search_engine_btree_leaf *term_details, ANT_search_engine_posting *postings)
{
#ifdef FIT_BM25
	double k1 = BM25_k1;
	double b = BM25_b;
#else
	const double k1 = 0.9;
	const double b = 0.4;
#endif
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
long docid, which;
double tf, idf;

docid = -1;

#ifdef NEVER
// This variant has been removed in favour of the one below because Shlomo showed the version below being better.
/*
	          N - n + 0.5
	IDF = log -----------
	            n + 0.5

	It is not clear from the BM25 papers what log-base should be used, so this implementation uses the natural log of x.  
*/
idf = log(((double)documents - (double)term_details->document_frequency + 0.5) / ((double)term_details->document_frequency + 0.5));
#endif

/*
	          N
	IDF = log -
	          n

	This variant of IDF is better than that above on the 70 INEX 2008 Wikipedia topics 
*/
idf = log((double)(documents) / (double)term_details->document_frequency);

/*
	               tf(td) * (k1 + 1)
	rsv = ----------------------------------- * IDF
	                                  len(d)
	      tf(td) + k1 * (1 - b + b * --------)
                                    av_len_d

	In this implementation we ignore k3 and the number of times the term occurs in the query.
*/

for (which = 0; which < term_details->document_frequency; which++)
	{
	docid = postings->docid[which];
	tf = postings->tf[which];

	accumulator[docid].add_rsv(idf * ((tf * k1_plus_1) / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length)))));
	}
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_SEARCH_TERM()
	--------------------------------------------
*/
void ANT_search_engine::process_one_search_term(char *term)
{
ANT_search_engine_btree_leaf term_details;
long long now;

now = stats->start_timer();
if (get_postings_details(term, &term_details) == NULL)
	return;
stats->add_dictionary_lookup_time(stats->stop_timer(now));

now = stats->start_timer();
if (get_postings(&term_details, postings_buffer) == NULL)
	return;
stats->add_posting_read_time(stats->stop_timer(now));

now = stats->start_timer();
decompress(postings_buffer, &term_details);
stats->add_decompress_time(stats->stop_timer(now));

now = stats->start_timer();
relevance_rank(&term_details, &posting);
stats->add_rank_time(stats->stop_timer(now));
}

/*
	ANT_SEARCH_ENGINE::STEM_TO_POSTINGS()
	-------------------------------------
*/
void ANT_search_engine::stem_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_search_engine_posting  *posting, long long collection_frequency, long *stem_buffer)
{
ANT_compressable_integer doc, found;
ANT_compressable_integer *docid, *tf;
long last = -1;

docid = posting->docid;
tf = posting->tf;
found = 0;
for (doc = found = 0; doc < documents; doc++)
	if (stem_buffer[doc] != 0)
		{
		*docid++ = doc - last;
		*tf++ = stem_buffer[doc];
		last = doc;
		found++;
		}

term_details->document_frequency = found;
term_details->collection_frequency = collection_frequency;
}

/*
	ANT_SEARCH_ENGINE::PROCESS_ONE_STEMMED_SEARCH_TERM()
	----------------------------------------------------
*/
void ANT_search_engine::process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term)
{
ANT_search_engine_btree_leaf term_details, stemmed_term_details;
long long now, collection_frequency;
ANT_compressable_integer *current_document, *current_tf, *end;
long document;
char *term;

now = stats->start_timer();
memset(stem_buffer, 0, (size_t)stem_buffer_length_in_bytes);
collection_frequency = 0;
stats->add_stemming_time(stats->stop_timer(now));

now = stats->start_timer();
term = stemmer->first(base_term);
stats->add_dictionary_lookup_time(stats->stop_timer(now));

while (term != NULL)
	{
	now = stats->start_timer();
	stemmer->get_postings_details(&term_details);
	stats->add_dictionary_lookup_time(stats->stop_timer(now));

	now = stats->start_timer();
	if (get_postings(&term_details, postings_buffer) == NULL)
		return;
	stats->add_posting_read_time(stats->stop_timer(now));

	now = stats->start_timer();
	decompress(postings_buffer, &term_details);
	stats->add_decompress_time(stats->stop_timer(now));

	now = stats->start_timer();
	collection_frequency += term_details.collection_frequency;
	end = posting.docid + term_details.document_frequency;
	document = -1;
	for (current_document = posting.docid, current_tf = posting.tf; current_document < end; current_document++, current_tf++)
		{
		document += *current_document;
		stem_buffer[document] += *current_tf;
		}
	stats->add_stemming_time(stats->stop_timer(now));

	now = stats->start_timer();
	term = stemmer->next();
	stats->add_dictionary_lookup_time(stats->stop_timer(now));
	}

now = stats->start_timer();
stem_to_postings(&stemmed_term_details, &posting, collection_frequency, stem_buffer);
stats->add_stemming_time(stats->stop_timer(now));

now = stats->start_timer();
relevance_rank(&stemmed_term_details, &posting);
stats->add_rank_time(stats->stop_timer(now));
}

/*
	ANT_SEARCH_ENGINE::SORT_RESULTS_LIST()
	--------------------------------------
*/
ANT_search_engine_accumulator *ANT_search_engine::sort_results_list(long long accurate_rank_point, long long *hits)
{
long long now;
ANT_search_engine_accumulator **current, **back_current, *current_accumulator, *end_accumulator;

/*
	On first observations it appears as though this array does not need to be
	re-initialised because the accumulator_pointers array already has a pointer
	to each accumulator, but they are left in a random order from the previous
	sort - which is good news (right?). Actually, all the zeros are left at the
	end which leads to a pathological case in quick-sort taking tens of seconds
	on the INEX Wikipedia 2009 collection.

	An effective optimisation is to bucket sort into two buckets at the beginning,
	one bucket is the zeros and the other bucket is the non-zeros.  This is essentially
	the first particion of the quick-sort before the call to quick sort.  The advantage
	is that we know in advance what the correct partition value is and that the secone
	partition (of all zeros) is now already sorted.  We also get (for free) the number
	of documents we found.
*/
now = stats->start_timer();

current = accumulator_pointers;
back_current = accumulator_pointers + documents - 1;
end_accumulator = accumulator + documents;
for (current_accumulator = accumulator; current_accumulator < end_accumulator; current_accumulator++)
	if (current_accumulator->is_zero_rsv())
		*back_current-- = current_accumulator;
	else
		*current++ = current_accumulator;

/*
	If the number of found documents is less than the accurate_rank_point then there
	is no point sorting past the number of found documents
*/
*hits = current - accumulator_pointers;
stats->add_count_relevant_documents(stats->stop_timer(now));

/*
	Sort the postings into decreasing order, but only guarantee the first accurate_rank_point postings are accurately ordered (top-k sorting)
*/
now = stats->start_timer();

//qsort(accumulator_pointers, documents, sizeof(*accumulator_pointers), ANT_search_engine_accumulator::compare_pointer);
//top_k_sort(accumulator_pointers, documents, sizeof(*accumulator_pointers), ANT_search_engine_accumulator::compare_pointer);
ANT_search_engine_accumulator::top_k_sort(accumulator_pointers, *hits, accurate_rank_point);

stats->add_sort_time(stats->stop_timer(now));

return accumulator;
}

/*
	ANT_SEARCH_ENGINE::GENERATE_RESULTS_LIST()
	------------------------------------------
*/
char **ANT_search_engine::generate_results_list(char **document_id_list, char **sorted_id_list, long long top_k)
{
long long found;
ANT_search_engine_accumulator **current, **end;

found = 0;
end = accumulator_pointers + documents;
for (current = accumulator_pointers; current < end; current++)
	if (!(*current)->is_zero_rsv())
		{
		if (found < top_k)		// first page
			sorted_id_list[found] = document_id_list[*current - accumulator];
		else
			break;
		found++;
		}

return sorted_id_list;
}
