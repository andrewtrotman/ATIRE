/*
	ATIRE_MERGE.C
	-------------
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "btree_head_node.h"
#include "btree_iterator.h"
#include "disk.h"
#include "file.h"
#include "impact_header.h"
#include "maths.h"
#include "memory.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"
#include "merger_param_block.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "stats_memory_index.h"
#include "stats_time.h"
#include "stop_word.h"

/*
	Buffers used for compression of postings lists, global because they are
	shared by write_postings and write_impact_header_postings
*/
long long postings_list_size = 1;
unsigned char *postings_list = new unsigned char[postings_list_size];
unsigned char *new_postings_list;
unsigned char *temp;

ANT_stop_word *stop_words;

/*
	TERM_COMPARE()
	--------------
*/
int term_compare(const void *term, const void *list_term_pointer)
{
	return strcmp((char *)term, *(char **)list_term_pointer);
}

/*
	SHOULD_PRUNE()
	--------------
	Dupulicate ANT_memory_index::should_prune to avoid needing a temporary vocab only index, creating an unecessary hash_node
	and including removing tags and numbers, which are dealt with elsewhere during normal indexing.
*/
inline long should_prune(char *term, ANT_search_engine_btree_leaf *leaf, ANT_merger_param_block *param, long long largest_docno)
{
if (param->stop_word_removal == ANT_memory_index::NONE)
	return false;
else if (*term == '~')
	return false;
else if (param->stop_word_removal & ANT_memory_index::PRUNE_NUMBERS && ANT_isdigit(*term))
	return true;
else if (param->stop_word_removal & ANT_memory_index::PRUNE_CF_SINGLETONS && leaf->local_collection_frequency == 1)
	return true;
else if (param->stop_word_removal & ANT_memory_index::PRUNE_DF_SINGLETONS && leaf->local_document_frequency <= param->stop_word_df_frequencies)
	return true;
else if (param->stop_word_removal & ANT_memory_index::PRUNE_TAGS && ANT_isupper(*term))
	return true;
else if (param->stop_word_removal & ANT_memory_index::PRUNE_DF_FREQUENTS && (double)leaf->local_document_frequency / (double)largest_docno >= param->stop_word_df_threshold)
	return true;
else if (param->stop_word_removal & (ANT_memory_index::PRUNE_NCBI_STOPLIST | ANT_memory_index::PRUNE_PUURULA_STOPLIST) && stop_words->isstop(term))
	return true;
else
	return false;
}

/*
	FIND_END_OF_NODE()
	------------------
	Duplicate of ANT_memory_index::find_end_of_node
*/
ANT_memory_index_hash_node **find_end_of_node(ANT_memory_index_hash_node **start)
{
ANT_memory_index_hash_node **current;

current = start;
if ((*current)->string.length() < B_TREE_PREFIX_SIZE)
	current++;
else
	while (*current != NULL)
		{
		if ((*current)->string.length() < B_TREE_PREFIX_SIZE)
			break;
		if ((*current)->string.true_strncmp(&(*start)->string, B_TREE_PREFIX_SIZE) != 0)
			break;
		current++;
		}
return current;
}

/*
	WRITE_NODE()
	------------
	Duplicate ANT_memory_index::write_node to avoid needing a temporary vocab only index
*/
ANT_memory_index_hash_node **write_node(ANT_file *file, ANT_memory_index_hash_node **start)
{
uint8_t zero = 0;
uint64_t eight_byte;
uint32_t four_byte, string_pos;
uint32_t terms_in_node, current_node_head_length;
ANT_memory_index_hash_node **current, **end;

/*
	Find the end of the node
*/
end = find_end_of_node(start);

/*
	Compute the number of terms in a node
*/
four_byte = terms_in_node = (uint32_t)(end - start);		// the number of terms in the node limited to 4 Billion!
file->write((unsigned char *)&terms_in_node, sizeof(terms_in_node));		// 4 bytes

/*
	CF, DF, Offset_in_postings, DocIDs_Len, Postings_len, String_pos_in_node
*/
current_node_head_length = (*start)->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : (uint32_t)(*start)->string.length();
string_pos = (uint32_t)(end - start) * ANT_btree_iterator::LEAF_SIZE + 4;
for (current = start; current < end; current++)
	{
	four_byte = (uint32_t)(*current)->collection_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)(*current)->document_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	eight_byte = (uint64_t)((*current)->in_disk.docids_pos_on_disk);
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

	four_byte = (uint32_t)((*current)->in_disk.impacted_length);
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)((*current)->in_disk.end_pos_on_disk - (*current)->in_disk.docids_pos_on_disk);
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)string_pos;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	string_pos += (uint32_t)(*current)->string.length() + 1 - current_node_head_length;
	}

/*
	Finally the strings ('\0' terminated)
*/
for (current = start; current < end; current++)
	{
	file->write((unsigned char *)((*current)->string.string() + current_node_head_length), (uint32_t)((*current)->string.length()) - current_node_head_length);
	file->write(&zero, 1);
	}

return end;
}

/*
	WRITE_POSTINGS()
	----------------
*/
ANT_memory_index_hash_node *write_postings(char *term, ANT_compressable_integer *raw, ANT_file *index, ANT_stats_memory_index *memory_stats, ANT_search_engine_btree_leaf *leaf, ANT_merger_param_block *param, long long largest_docno, long long *longest_postings, ANT_compression_factory *factory)
{
ANT_memory_index_hash_node *node;
ANT_compressable_integer *current;
uint64_t current_disk_position;
long long len = 0, used = 0;

if (should_prune(term, leaf, param, largest_docno))
	return NULL;

node = new (memory_stats->postings_memory) ANT_memory_index_hash_node(memory_stats->postings_memory, memory_stats->postings_memory, new ANT_string_pair(term), memory_stats);

node->collection_frequency = leaf->local_collection_frequency;
node->document_frequency = leaf->local_document_frequency;

#ifdef SPECIAL_COMPRESSION
if (node->document_frequency <= 2)
	{
	/*
		Squiggle variables are given to us as an array of values but we need to put them out
		impact ordered, so here we impact order them under a 'tf' of 1
	*/
	if (node->string[0] == '~')
		{
		raw[5] = 0;
#ifdef IMPACT_HEADER
		raw[4] = raw[3] = 0;
		raw[2] = raw[1];
		raw[1] = raw[0];
		raw[0] = 1;
#else
		if (node->document_frequency == 2)
			{
			raw[4] = raw[1];
			raw[3] = 1;
			}
		else
			raw[4] = raw[3] = 0;
		raw[2] = 0;
		raw[1] = raw[0];
		raw[0] = 2;
#endif
		}

	node->in_disk.docids_pos_on_disk = ((long long)raw[1]) << 32 | raw[0];
	
	if (node->document_frequency == 2)
		{
		node->in_disk.impacted_length = (raw[2] == 0 ? raw[4] : raw[2]);
		node->in_disk.end_pos_on_disk = (raw[2] == 0 ? raw[3] : raw[0]) + node->in_disk.docids_pos_on_disk;
		}
	else
		node->in_disk.impacted_length = node->in_disk.end_pos_on_disk = 0;
	}
else
#endif
	{
	/*
		Manipulate the impact ordering to deal with the static prune point if necessary
	*/
	if (node->string[0] != '~' && leaf->local_document_frequency > param->static_prune_point)
		{
		current = raw;
		
		while (used < param->static_prune_point)
			{
			current++;
			while (*current != 0 && used < param->static_prune_point)
				{
				used++;
				current++;
				}
			if (used < param->static_prune_point)
				current++;
			}
		/*
			Terminate the impact ordering
		*/
		leaf->impacted_length = current - raw;
		raw[leaf->impacted_length] = 0;
		leaf->impacted_length++;
		}
	
	/*
		Keep trying the compression until it works, doubling the size of the buffer each time.
	*/
	while ((len = factory->compress(postings_list, postings_list_size, raw, leaf->impacted_length)) == 1)
		{
		new_postings_list = new unsigned char[postings_list_size * 2];
		memcpy(new_postings_list, postings_list, postings_list_size * sizeof(*postings_list));
		
		delete [] postings_list;
		postings_list = new_postings_list;
		
		postings_list_size *= 2;
		}
	
	current_disk_position = index->tell();
	index->write(postings_list, len);
	
	node->in_disk.docids_pos_on_disk = current_disk_position;
	node->in_disk.impacted_length = leaf->impacted_length;
	node->in_disk.end_pos_on_disk = index->tell();
	}

*longest_postings = ANT_max(len, *longest_postings);

return node;
}

/*
	WRITE_IMPACT_HEADER_POSTINGS()
	------------------------------
*/
ANT_memory_index_hash_node *write_impact_header_postings(char *term, ANT_compressable_integer *header, ANT_compressable_integer quantum_count, ANT_compressable_integer *raw, ANT_file *index, ANT_stats_memory_index *memory_stats, ANT_search_engine_btree_leaf *leaf, ANT_merger_param_block *param, long long largest_docno, long long *longest_postings, ANT_compression_factory *factory)
{
static long long header_buffer_size = 1 + ANT_impact_header::INFO_SIZE + (ANT_impact_header::NUM_OF_QUANTUMS * 3 * sizeof(*header));

static unsigned char *compressed_impact_header_buffer = new unsigned char[header_buffer_size];

unsigned char *postings_ptr = postings_list;
unsigned char *compressed_header_ptr = compressed_impact_header_buffer + ANT_impact_header::INFO_SIZE;

long long len = 0;
uint64_t current_disk_position;
ANT_memory_index_hash_node *node;

ANT_compressable_integer *impact_value_start = header;
ANT_compressable_integer *document_count_start = header + quantum_count;
ANT_compressable_integer *impact_offset_start = header + (2 * quantum_count);

ANT_compressable_integer *impact_value_pointer = impact_value_start;
ANT_compressable_integer *document_count_pointer = document_count_start;
ANT_compressable_integer *impact_offset_pointer = impact_offset_start;

#ifdef SPECIAL_COMPRESSION
/*
	Because the fiddling we do here will screw up ~ terms, and we take
	care of those in write_postings we ignore them this time
	
	This is quite ugly, turn the impact header format into a impact ordered
	format rather than writing two version of write_potings
*/
if (leaf->local_document_frequency <= 2 && *term != '~')
	{
	ANT_compressable_integer doc_one = raw[*impact_offset_start];
	ANT_compressable_integer doc_two;
	if (quantum_count == 2)
		doc_two = raw[*(impact_offset_start + 1)];
	else
		doc_two = raw[*impact_offset_start + 1];
	ANT_compressable_integer impact_one = *impact_value_start;
	ANT_compressable_integer impact_two = *(impact_value_start + 1);
	
	leaf->impacted_length = 3;
	if (quantum_count == 2)
		{
		raw[5] = 0;
		raw[4] = doc_two;
		raw[3] = impact_two;
		raw[2] = 0;
		leaf->impacted_length += 3;
		}
	else if (leaf->local_document_frequency == 2)
		{
		raw[3] = 0;
		raw[2] = doc_two;
		leaf->impacted_length++;
		}
	else
		raw[2] = 0;
	raw[1] = doc_one;
	raw[0] = impact_one;
	
	return write_postings(term, raw, index, memory_stats, leaf, param, largest_docno, longest_postings, factory);
	}
#endif

if (*term == '~')
	return write_postings(term, raw, index, memory_stats, leaf, param, largest_docno, longest_postings, factory);

if (should_prune(term, leaf, param, largest_docno))
	return NULL;

node = new (memory_stats->postings_memory) ANT_memory_index_hash_node(memory_stats->postings_memory, memory_stats->postings_memory, new ANT_string_pair(term), memory_stats);

node->collection_frequency = leaf->local_collection_frequency;
node->document_frequency = leaf->local_document_frequency;

/*
	Compress each postings
*/
for (ANT_compressable_integer i = 0; i < quantum_count; impact_value_pointer++, document_count_pointer++, impact_offset_pointer++, i++)
	{
	/*
		Keep trying the compression until it works, doubling the size of the buffer each time.
	*/
	while ((len = factory->compress(postings_ptr, postings_list_size - (postings_ptr - postings_list), raw + *impact_offset_pointer, *document_count_pointer)) == 1)
		{
		new_postings_list = new unsigned char[postings_list_size * 2];
		memcpy(new_postings_list, postings_list, postings_list_size * sizeof(*postings_list));
		postings_ptr = new_postings_list + (postings_ptr - postings_list);
		
		delete [] postings_list;
		postings_list = new_postings_list;
		
		postings_list_size *= 2;
		}
	*impact_offset_pointer = (ANT_compressable_integer)(postings_ptr - postings_list); // convert pointer to offset
	postings_ptr += len;
	}

/*
	Compress the header
*/
len = factory->compress(compressed_header_ptr, header_buffer_size, header, quantum_count * 3);

((uint64_t *)compressed_impact_header_buffer)[0] = 0;
((uint64_t *)compressed_impact_header_buffer)[1] = 0;
((uint32_t *)compressed_impact_header_buffer)[4] = quantum_count;
((uint32_t *)compressed_impact_header_buffer)[5] = (uint32_t)(ANT_impact_header::INFO_SIZE + len);

compressed_header_ptr += len;

/*
	Write the header to disk
*/
current_disk_position = index->tell();
index->write(compressed_impact_header_buffer, compressed_header_ptr - compressed_impact_header_buffer);

/*
	Write the postings to disk
*/
index->write(postings_list, postings_ptr - postings_list);

node->in_disk.docids_pos_on_disk = current_disk_position;
node->in_disk.impacted_length = leaf->impacted_length;
node->in_disk.end_pos_on_disk = index->tell();

*longest_postings = ANT_max(*longest_postings, (long long)(postings_ptr - postings_list));

return node;
}

/*
	WRITE_VARIABLE()
	----------------
*/
ANT_memory_index_hash_node *write_variable(char *term, long long value, ANT_stats_memory_index *memory_stats, ANT_file *index, ANT_search_engine_btree_leaf *leaf, ANT_merger_param_block *param, long long largest_docno, long long *longest_postings, ANT_compression_factory *factory)
{
static ANT_compressable_integer raw[6]; // 6 instead of 2 to make room for it to be fiddled with in write_postings

raw[0] = ((unsigned long long)value) >> 32;
raw[1] = ((unsigned long long)value) & 0xFFFFFFFF;

leaf->impacted_length = leaf->local_collection_frequency = leaf->local_document_frequency = 2;

return write_postings(term, raw, index, memory_stats, leaf, param, largest_docno, longest_postings, factory);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long offset, engine, upto = 0;
long long global_trimpoint = 0;
long long this_trimpoint;
long long terms_so_far = 0;
double dummy;

char *intersection_file_buffer;
char *intersection_filename;
char **intersection_term_list = NULL;
int skip_intersection = true;
long long intersection_term_count = 0;

uint64_t current_disk_position;
char file_header[] = "ATIRE Search Engine Index File\n\0\0";

ANT_merger_param_block param_block(argc, argv);
long first_param = param_block.parse();

long long combined_docs = 0, maximum_terms = 0;
long long number_engines = argc - first_param;
long long document;
long long longest_postings = 0;
long long document_filenames_start, document_filenames_finish;
unsigned long longest_document = 0, compress_buffer_size;
char *document_compress_buffer;

ANT_compression_factory *factory = new ANT_compression_factory;
factory->set_scheme(param_block.compression_scheme);

if (param_block.stop_word_removal & ANT_memory_index::PRUNE_PUURULA_STOPLIST)
	stop_words = new ANT_stop_word(ANT_stop_word::PUURULA);
else
	stop_words = new ANT_stop_word(ANT_stop_word::NCBI);

/*
	If performing an intersection, need to load list of terms from disk
*/
if (param_block.skip_intersection == false)
	{
	skip_intersection = false;
	intersection_filename = param_block.intersection_filename;
	intersection_file_buffer = ANT_disk::read_entire_file(intersection_filename);
	if (intersection_file_buffer == NULL)
		exit(printf("Cannot read %s for intersecting\n", intersection_filename));
	intersection_term_list = ANT_disk::buffer_to_list(intersection_file_buffer, &intersection_term_count);
	qsort(intersection_term_list, intersection_term_count, sizeof(*intersection_term_list), char_star_star_strcmp);
	}

ANT_stats_time stats;
ANT_search_engine **search_engines = new ANT_search_engine*[number_engines];
ANT_btree_iterator **iterators = new ANT_btree_iterator*[number_engines];
char **terms = new char*[number_engines];
long long *trimpoints = new long long[number_engines];

/*
	Allocate 1 more for these to allow the merged results to be put on the end
*/
ANT_search_engine_btree_leaf **leaves = new ANT_search_engine_btree_leaf*[number_engines + 1];
ANT_compressable_integer **raw = new ANT_compressable_integer *[number_engines + 1];
ANT_memory *memory = new ANT_memory[number_engines + 1];

/*
	Initialise the search engines, and related variables
*/
for (engine = 0; engine < number_engines; engine++)
	{
	search_engines[engine] = new ANT_search_engine(&memory[engine]);
	search_engines[engine]->open(argv[engine + first_param]);
	
	combined_docs += search_engines[engine]->document_count();
#ifdef IMPACT_HEADER
	maximum_terms += search_engines[engine]->get_unique_term_count(); // Will definitely not be exceeded
	longest_document = ANT_max((unsigned long)search_engines[engine]->get_variable("~documentlongest"), longest_document);
#else
	maximum_terms += 100000; // A not-unreasonable guesstimate ... will expand if necessary
	longest_document = ANT_max((unsigned long)search_engines[engine]->get_longest_document_length(), longest_document);
#endif
	
	iterators[engine] = new ANT_btree_iterator(search_engines[engine]);
	leaves[engine] = new ANT_search_engine_btree_leaf;
	
	if (search_engines[engine]->quantized())
		exit(printf("Cannot merge quantized indexes (%s is quantized)\n", argv[engine + first_param]));
	
	this_trimpoint = search_engines[engine]->get_variable("~trimpoint");
	trimpoints[engine] = this_trimpoint ? this_trimpoint : LONG_MAX;
	global_trimpoint += this_trimpoint;
	}
leaves[number_engines] = new ANT_search_engine_btree_leaf;
/*
	Worst case, all documents and all tf values for impact ordering, all documents for impact header
*/
raw[number_engines] = new ANT_compressable_integer[510 + combined_docs];

#ifdef FILENAME_INDEX
long long *filename_index_offsets = new long long[combined_docs];
long long filename_offset_sum = 0;
long long filename_offset = 0;
#endif

/*
	global_trimpoint could be 0 if none of the given indexes are pruned
*/
param_block.static_prune_point = ANT_min(param_block.static_prune_point, global_trimpoint ? global_trimpoint : LONG_MAX);

ANT_compression_text_factory factory_text;

ANT_stats_memory_index *memory_stats = new ANT_stats_memory_index(&memory[number_engines], &memory[number_engines]);

ANT_memory_index_hash_node *p;
ANT_memory_index_hash_node **term_list, **here;

term_list = new ANT_memory_index_hash_node *[maximum_terms + 1];

ANT_btree_head_node *header, *current_header, *last_header;

#ifdef IMPACT_HEADER
	ANT_compressable_integer *decompress_buffer = new ANT_compressable_integer[combined_docs];

	ANT_compressable_integer **impact_headers = new ANT_compressable_integer *[number_engines + 1];
	ANT_compressable_integer *quantum_counts = new ANT_compressable_integer[number_engines + 1];
	ANT_compressable_integer *postings_begin = new ANT_compressable_integer[number_engines + 1];

	ANT_compressable_integer *quantums_processed = new ANT_compressable_integer[number_engines];
	ANT_compressable_integer *current_impact_header;
	ANT_compressable_integer quantum, number_quantums_used, postings_offset;

	for (engine = 0; engine < number_engines; engine++)
		impact_headers[engine] = new ANT_compressable_integer[ANT_impact_header::NUM_OF_QUANTUMS * 3]; // * 3 -> impact values, doc counts, impact offsets
	impact_headers[number_engines] = new ANT_compressable_integer[ANT_impact_header::NUM_OF_QUANTUMS * 3];

	long number_documents, impact_offset;
#endif

uint8_t zero = 0;
uint32_t four_byte;
uint64_t eight_byte;
long btree_root_size = 0;
int32_t longest_term = 0;
int64_t highest_df = 0;
uint64_t terms_in_root;

char *doc_buf = NULL;
char **doc_filenames;
long long start, end, sum = 0;
unsigned long buf_len = 0;
long *strcmp_results = new long[number_engines];
char *next_term_to_process = NULL;
long should_continue = false;
long long do_documents = param_block.document_compression_scheme != ANT_merger_param_block::NONE;
long long stemmer;

#ifndef FILENAME_INDEX
	ANT_file *doclist = new ANT_file;
#endif
ANT_file *index = new ANT_file;

compress_buffer_size = longest_document;
document_compress_buffer = new char[longest_document];

unsigned int current_tf;
#ifndef IMPACT_HEADER
	ANT_compressable_integer **ends = new ANT_compressable_integer*[number_engines];
#endif
long *should_process = new long[number_engines];
long previous_docid;
long process_this_tf;
ANT_compressable_integer *current = raw[number_engines];

if (param_block.logo)
	puts(ANT_version_string);

index->open(param_block.index_filename, "w");
#ifndef FILENAME_INDEX
	doclist->open(param_block.doclist_filename, "w");
#endif

index->write((unsigned char *)file_header, sizeof(file_header));

/*
	Sort out which stemmer to use, if the merged indexes have all been stemmed the same, use that, otherwise we can't stem
*/
stemmer = search_engines[0]->get_variable("~stemmer");
for (engine = 1; engine < number_engines; engine++)
	if (search_engines[engine]->get_variable("~stemmer") != stemmer)
		exit(printf("Cannot merge indexes that have been stemmed differently (%s/%s)\n", argv[first_param], argv[first_param + engine]));

for (engine = 0; engine < number_engines; engine++)
	do_documents = do_documents && search_engines[engine]->get_postings_details("~documentoffsets", leaves[engine]);

if (do_documents)
	{
	for (engine = 0; engine < number_engines; engine++)
		for (document = 0; document < search_engines[engine]->document_count(); document++)
			{
			current_disk_position = index->tell();
			
			search_engines[engine]->get_compressed_document(document_compress_buffer, &compress_buffer_size, document);
			index->write((unsigned char *)document_compress_buffer, compress_buffer_size);
			
			sum += raw[number_engines][upto] = (ANT_compressable_integer)(current_disk_position - (upto == 0 ? 0 : sum));
			upto++;
			}
	raw[number_engines][upto] = (ANT_compressable_integer)(index->tell() - sum);
#ifdef FILENAME_INDEX
	}
#endif
	
	/*
		Before we write out the "postings" for offsets, we should put the document filenames
		here and get the values for start/finish of those.
	*/
	document_filenames_start = index->tell();
	
	/*
		Copy over the filenames
	*/
	for (engine = 0; engine < number_engines; engine++)
		{
		start = search_engines[engine]->get_variable("~documentfilenamesstart");
		end = search_engines[engine]->get_variable("~documentfilenamesfinish");
		
		doc_buf = (char *)realloc(doc_buf, end - start);
		doc_filenames = search_engines[engine]->get_document_filenames(doc_buf, &buf_len);
		
		/*
			Technically, here we could just write out doc_buf here to the index,
			but the underlying representation _might_ change.
			
			Also write out the the doclist file ... because we need it.
		*/
		for (document = 0; document < search_engines[engine]->document_count(); document++)
			{
#ifdef FILENAME_INDEX
			filename_index_offsets[filename_offset++] = filename_offset_sum;
			filename_offset_sum += strlen(doc_filenames[document]) + 1;
#else
			doclist->puts(strip_space_inplace(doc_filenames[document]));
#endif
			index->write((unsigned char *)doc_filenames[document], strlen(doc_filenames[document]) + 1);
			}
		}
	
	document_filenames_finish = index->tell();
	
	/*
		Now we've done the documents, do the filename start/finish.
	*/
	if ((p = write_variable("~documentfilenamesstart", document_filenames_start, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;
	if ((p = write_variable("~documentfilenamesfinish", document_filenames_finish, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;

#ifdef FILENAME_INDEX

document_filenames_start = index->tell();

index->write((unsigned char *)filename_index_offsets, sizeof(*filename_index_offsets) * combined_docs);
index->write((unsigned char *)&filename_offset_sum, sizeof(filename_offset_sum));

document_filenames_finish = index->tell();

if ((p = write_variable("~documentfilenamesindexstart", document_filenames_start, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;
if ((p = write_variable("~documentfilenamesindexfinish", document_filenames_finish, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;

if (do_documents)
	{
#endif
	
	/*
		Update stats, because this is a difference encoded list, we start with 1, then substract one
		for each of the other engines, this works out to be the right number (number_engines + 1)
		
		The documentoffsets are calculated above.
	*/
	leaves[number_engines]->impacted_length = leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 1;
	for (engine = 0; engine < number_engines; engine++)
		{
		leaves[engine] = search_engines[engine]->get_postings_details("~documentoffsets", leaves[engine]);
		leaves[number_engines]->impacted_length += leaves[engine]->impacted_length - 1;
		leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency - 1;
		leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency - 1;
		}
	if ((p = write_postings("~documentoffsets", raw[number_engines], index, memory_stats, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;
	
	/*
		Now we've done the documents and offsets, do the longest document.
	*/
#ifdef IMPACT_HEADER
	if ((p = write_variable("~documentlongest", longest_document, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;
#else
	raw[number_engines][0] = longest_document;
	leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 1;
	leaves[number_engines]->impacted_length = 3;
	if ((p = write_postings("~documentlongest", raw[number_engines], index, memory_stats, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;
#endif
	
	if (param_block.reporting_frequency != 0)
		{
		printf("Processed documents in ");
		stats.print_elapsed_time();
		}
	}

/*
	Now for the final ~ variable that needs any new computation, length
	
	Because get_document_lengths returns the real lengths we need to add 1 so that they work correctly
*/
offset = 0;
for (engine = 0; engine < number_engines; engine++)
	{
	raw[engine] = search_engines[engine]->get_document_lengths(&dummy);
	for (document = 0; document < search_engines[engine]->document_count(); document++)
		raw[number_engines][document + offset] = raw[engine][document] + 1;
	
	offset += search_engines[engine]->document_count();
	}
leaves[number_engines]->impacted_length = offset;
leaves[number_engines]->local_collection_frequency = offset;
leaves[number_engines]->local_document_frequency = offset;

if ((p = write_postings("~length", raw[number_engines], index, memory_stats, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;

if (param_block.static_prune_point != LONG_MAX)
	if ((p = write_variable("~trimpoint", param_block.static_prune_point, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;

if (stemmer)
	if ((p = write_variable("~stemmer", stemmer, memory_stats, index, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;

/*
	Now move onto the "normal" terms
*/
for (engine = 0; engine < number_engines; engine++)
	{
	terms[engine] = iterators[engine]->first(NULL);
	should_continue = should_continue || terms[engine] != NULL;
	}

while (should_continue)
	{
	/*
		Find the next term that we need to process
	*/
	for (engine = 0; engine < number_engines; engine++)
		if (terms[engine])
			{
			next_term_to_process = terms[engine];
			break;
			}
	for (; engine < number_engines; engine++)
		if (terms[engine] && strcmp(terms[engine], next_term_to_process) < 0)
			next_term_to_process = terms[engine];
	
	/*
		Save repeated doing strcmp in this while loop
	*/
	for (engine = 0; engine < number_engines; engine++)
		strcmp_results[engine] = terms[engine] ? strcmp(next_term_to_process, terms[engine]) : 1;

	/*
		Now next_term_to_process contains the smallest string of the lists we're merging together
		
		Ignore all ~ terms, they've already been dealt with

		Add term if skipping intersection, else add if it is in intersection list
	*/
	if (*next_term_to_process != '~' && (skip_intersection || (bsearch(next_term_to_process, intersection_term_list, intersection_term_count, sizeof(*intersection_term_list), term_compare) != NULL)))
		{
		/*
			Preload the postings lists for each engine
		*/
		for (engine = 0; engine < number_engines; engine++)
			if (strcmp_results[engine] == 0)
				{
				iterators[engine]->get_postings_details(leaves[engine]);
#ifdef IMPACT_HEADER
				raw[engine] = (ANT_compressable_integer *)search_engines[engine]->get_postings(leaves[engine], (unsigned char *)raw[engine]);
#else
				search_engines[engine]->get_postings(leaves[engine], search_engines[engine]->get_postings_buffer());
				factory->decompress(search_engines[engine]->get_decompress_buffer(), search_engines[engine]->get_postings_buffer(), leaves[engine]->impacted_length);
				raw[engine] = search_engines[engine]->get_decompress_buffer();
				ends[engine] = raw[engine] + ANT_min(trimpoints[engine], leaves[engine]->local_document_frequency);
#endif
				}
		
#ifdef IMPACT_HEADER
		/*
			Reread the headers for this new term
		*/
		for (engine = 0; engine < number_engines; engine++)
			if (strcmp_results[engine] == 0)
				{
				quantum_counts[engine] = (ANT_compressable_integer)((uint32_t *)raw[engine])[4];
				postings_begin[engine] = (ANT_compressable_integer)((uint32_t *)raw[engine])[5];
				
				quantums_processed[engine] = 0;
				
				/*
					Decompress the header, * 3 -> impact values, doc counts, impact offsets
				*/
				factory->decompress(impact_headers[engine], (unsigned char *)raw[engine] + ANT_impact_header::INFO_SIZE, quantum_counts[engine] * 3);
				}
		
		current = raw[number_engines];
		current_impact_header = impact_headers[number_engines];
		
		/*
			Reset the header value to all 0
		*/
		memset(current_impact_header, 0, sizeof(*current_impact_header) * ANT_impact_header::NUM_OF_QUANTUMS * 3);
		number_quantums_used = 0;
		postings_offset = 0;
		
		for (current_tf = 255; current_tf > 0; current_tf--)
			{
			process_this_tf = false;
			previous_docid = 0;
			offset = 0;
			
			/*
				Work out if we should process this tf value and each engine
			*/
			for (engine = 0; engine < number_engines; engine++)
				{
				should_process[engine] = false;
				if (strcmp_results[engine] == 0 && quantums_processed[engine] < quantum_counts[engine] && impact_headers[engine][quantums_processed[engine]] == current_tf)
					should_process[engine] = process_this_tf = true;
				}

			if (process_this_tf)
				{
				number_quantums_used += 1;
				*current_impact_header = current_tf;

				/*
					Process them!
				*/
				for (engine = 0; engine < number_engines; engine++)
					{
					if (should_process[engine])
						{
						number_documents = impact_headers[engine][quantums_processed[engine] + quantum_counts[engine]];
						current_impact_header[ANT_impact_header::NUM_OF_QUANTUMS] += number_documents; // doc count
						impact_offset = impact_headers[engine][quantums_processed[engine] + (quantum_counts[engine] * 2)];
						
						factory->decompress(decompress_buffer, (unsigned char *)raw[engine] + postings_begin[engine] + impact_offset, number_documents);
						
						*current = (ANT_compressable_integer)(decompress_buffer[0] + offset - previous_docid);
						previous_docid += *current++;
						for (document = 1; document < number_documents; document++)
							{
							*current = decompress_buffer[document];
							previous_docid += *current++;
							}
						quantums_processed[engine]++;
						}
						
					offset += search_engines[engine]->document_count();
					should_process[engine] = false;
					}
					
				/*
					Set the beginnings of postings
				*/
				current_impact_header[ANT_impact_header::NUM_OF_QUANTUMS * 2] = postings_offset;
				postings_offset = (ANT_compressable_integer)(current - raw[number_engines]);
				current_impact_header++;
				}
			}

		/*
			Move the impact header sections to be contiguous
		*/
		for (quantum = 0; quantum < number_quantums_used; quantum++)
			impact_headers[number_engines][quantum + number_quantums_used] = impact_headers[number_engines][quantum + ANT_impact_header::NUM_OF_QUANTUMS];
		for (quantum = 0; quantum < number_quantums_used; quantum++)
			impact_headers[number_engines][quantum + 2 * number_quantums_used] = impact_headers[number_engines][quantum + 2 * ANT_impact_header::NUM_OF_QUANTUMS];

		/*
			Now impact_headers[number_engines] contains all the data, in 3 * number_quantums_used, and raw[number_engines] contains the postings
		*/
		leaves[number_engines]->local_collection_frequency = leaves[number_engines]->local_document_frequency = 0;
		for (engine = 0; engine < number_engines; engine++)
			if (strcmp_results[engine] == 0)
				{
				leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency;
				leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency;
				}
		
		leaves[number_engines]->impacted_length = current - raw[number_engines];
		
		/*
			Serialise the merged postings
		*/
		if ((p = write_impact_header_postings(next_term_to_process, impact_headers[number_engines], number_quantums_used, raw[number_engines], index, memory_stats, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
			term_list[terms_so_far++] = p;
#else
		current = raw[number_engines];
		for (current_tf = 255; current_tf > 0; current_tf--)
			{
			process_this_tf = false;
			
			for (engine = 0; engine < number_engines; engine++)
				{
				should_process[engine] = false;
				if (strcmp_results[engine] == 0 && ends[engine] > raw[engine] && *raw[engine] == current_tf)
					{
					ends[engine] += 2;
					raw[engine]++;
					should_process[engine] = process_this_tf = true;
					}
				}
			
			if (process_this_tf)
				{
				*current++ = current_tf;
				previous_docid = 0;
				offset = 0;
				
				for (engine = 0; engine < number_engines; engine++)
					{
					if (should_process[engine])
						{
						/*
							The first docid in the impacted order isn't difference encoded
							so make it difference encoded while also accounting for the
							necessary change in docids across engines
						*/
						*current = (ANT_compressable_integer)(*raw[engine] + offset - previous_docid);
						previous_docid += *current;
						current++;
						raw[engine]++;
						
						/*
							The rest of the docids are difference encoded, so we don't
							have to worry about that
						*/
						while (*raw[engine] != 0)
							{
							previous_docid += *raw[engine];
							*current++ = *raw[engine]++;
							}
						/*
							Skip over the 0 at the end of the impact ordered list
						*/
						raw[engine]++;
						
						}
					offset += search_engines[engine]->document_count();
					should_process[engine] = false;
					}
				/*
					Terminate the impact ordering for this tf
				*/
				*current++ = 0;
				}
			}
		
		leaves[number_engines]->local_collection_frequency = leaves[number_engines]->local_document_frequency = 0;
		for (engine = 0; engine < number_engines; engine++)
			if (strcmp_results[engine] == 0)
				{
				leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency;
				leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency;
				}
		
		leaves[number_engines]->impacted_length = current - raw[number_engines];
		
		/*
			Serialise the merged postings
		*/
		if ((p = write_postings(next_term_to_process, raw[number_engines], index, memory_stats, leaves[number_engines], &param_block, combined_docs, &longest_postings, factory)) != NULL)
			term_list[terms_so_far++] = p;
#endif
		
		/*
			Make room for more terms if needed
		*/
		if (terms_so_far == maximum_terms - 1)
			{
			here = new ANT_memory_index_hash_node*[maximum_terms * 2];
			memcpy(here, term_list, terms_so_far * sizeof(*term_list));
			
			delete [] term_list;
			term_list = here;
			
			maximum_terms *= 2;
			}

		if (param_block.reporting_frequency && terms_so_far % param_block.reporting_frequency == 0)
			{
			printf("Processed %lld terms in ", terms_so_far);
			stats.print_elapsed_time();
			}
		}
	
	/*
		Move on to the next terms if necessary
	*/
	for (engine = 0; engine < number_engines; engine++)
		if (strcmp_results[engine] == 0)
			terms[engine] = iterators[engine]->next();
	
	/*
		Decide if we should keep going, or we have processed all terms
	*/
	should_continue = false;
	for (engine = 0; engine < number_engines; engine++)
		should_continue = should_continue || terms[engine];
	}

if (param_block.reporting_frequency != 0)
	{
	printf("Processed %lld terms in ", terms_so_far);
	stats.print_elapsed_time();
	}

/*
	Now we've done all the postings, it's time to write the dictionary and header.
*/
term_list[terms_so_far] = NULL;

qsort(term_list, terms_so_far, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

for (here = term_list; *here != NULL; here = find_end_of_node(here))
	btree_root_size++;

/*
	Write the second level of the btree.
*/
current_header = header = new ANT_btree_head_node[btree_root_size];
here = term_list;
while (*here != NULL)
	{
	current_header->disk_pos = index->tell();
	current_header->node = *here;
	current_header++;
	here = write_node(index, here);
	}
last_header = current_header;
terms_in_root = last_header - header;

current_disk_position = index->tell();

index->write((unsigned char *)&terms_in_root, sizeof(terms_in_root));

/*
	Write the first level of the btree.
*/
for (current_header = header; current_header < last_header; current_header++)
	{
	index->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	index->write(&zero, sizeof(zero));
	eight_byte = current_header->disk_pos;
	index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
	}

/*
	Write the "header" of the index file
*/
index->write((unsigned char *)&current_disk_position, sizeof(current_disk_position));

index->write((unsigned char *)&longest_term, sizeof(longest_term));
#ifdef IMPACT_HEADER
	four_byte = (uint32_t)terms_so_far;
	index->write((unsigned char *)&four_byte, sizeof(four_byte));
#endif

four_byte = (uint32_t)longest_postings;
#ifdef IMPACT_HEADER
	/*
		Include the extra space that's needed for impact header itself
	*/
	four_byte += 3 * sizeof(*impact_headers[number_engines]) * ANT_impact_header::NUM_OF_QUANTUMS + ANT_impact_header::INFO_SIZE;
#endif
index->write((unsigned char *)&four_byte, sizeof(four_byte));

index->write((unsigned char *)&highest_df, sizeof(highest_df));
eight_byte = 0;
index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
eight_byte = ANT_file_signature_index;
index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
four_byte = (uint32_t)ANT_version;
index->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)ANT_file_signature;
index->write((unsigned char *)&four_byte, sizeof(four_byte));

index->close();
#ifndef FILENAME_INDEX
	doclist->close();
#endif

if (param_block.reporting_frequency != 0)
	{
	printf("Finished merge in ");
	stats.print_elapsed_time();
	}

#ifndef FILENAME_INDEX
if (!do_documents)
	{
	printf("Warning: empty doclist generated because not all indexes had filenames or merge was run with -C-.\n");
	printf("Combine the doclists for the indexes merged in the same order given to doclist file: %s.\n", param_block.doclist_filename);
	}
#endif

/*
	Cleanup
*/
for (engine = 0; engine < number_engines; engine++)
	{
	delete iterators[engine];
	delete leaves[engine];
	delete search_engines[engine];
	#ifdef IMPACT_HEADER
		delete [] impact_headers[engine];
	#endif
	}
delete leaves[number_engines];
delete raw[number_engines];
#ifdef IMPACT_HEADER
	delete [] impact_headers[number_engines];
	delete [] impact_headers;
	
	delete [] decompress_buffer;
	delete [] quantum_counts;
	delete [] postings_begin;
	delete [] quantums_processed;
#else
	delete [] ends;
#endif
delete [] document_compress_buffer;
delete [] header;
delete [] iterators;
delete [] leaves;
delete [] memory;
delete [] search_engines;
delete [] should_process;
delete [] strcmp_results;
delete [] term_list;
delete [] terms;
delete [] trimpoints;

delete [] postings_list;

#ifndef FILENAME_INDEX
	delete doclist;
#endif
delete factory;
delete index;
delete memory_stats;

return 0;
}
