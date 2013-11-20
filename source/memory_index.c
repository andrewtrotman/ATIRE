/*
	MEMORY_INDEX.C
	--------------
*/
#include <new>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits>
#include "ranking_function.h"
#include "ranking_function_factory.h"
#include "memory_index_hash_node.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "memory_index_one_node.h"
#include "stats_memory_index.h"
#include "memory.h"
#include "string_pair.h"
#include "file.h"
#include "btree.h"
#include "btree_head_node.h"
#include "fundamental_types.h"
#include "compression_factory.h"
#include "maths.h"
#include "btree_iterator.h"
#include "stop_word.h"
#include "fence.h"

#define DISK_BUFFER_SIZE (10 * 1024 * 1024)


ANT_string_pair ANT_memory_index::squiggle_document_offsets("~documentoffsets");
ANT_string_pair ANT_memory_index::squiggle_document_longest("~documentlongest");

/*
	ANT_MEMORY_INDEX::ANT_MEMORY_INDEX()
	------------------------------------
*/
ANT_memory_index::ANT_memory_index(char *filename)
{
stop_word_removal_mode = NONE;
hashed_squiggle_length = hash(&squiggle_length);
memset(hash_table, 0, sizeof(hash_table));
titles_memory = serialisation_memory = dictionary_memory = new ANT_memory;		// if you seperate these then remember to update the stats object
postings_memory = new ANT_memory;
stats = new ANT_stats_memory_index(dictionary_memory, postings_memory);
serialised_docids_size = 1;
serialised_docids = (unsigned char *)serialisation_memory->malloc(serialised_docids_size);
serialised_tfs_size = 1;
serialised_tfs = (unsigned short *)serialisation_memory->malloc(serialised_tfs_size);
largest_docno = 0;
factory = new ANT_compression_factory;
document_lengths = NULL;
quantizer = NULL;
documents_in_repository = 0;
compressed_longest_raw_document_size = 0;
index_file = NULL;
open_index_file(filename);
document_filenames = NULL;
document_filenames_used = document_filenames_chunk_size;
static_prune_point = -1;
stop_word_max_proportion = 1.1;			// initialising with anything greater than 1.0 will do
}

/*
	ANT_MEMORY_INDEX::~ANT_MEMORY_INDEX()
	-------------------------------------
*/
ANT_memory_index::~ANT_memory_index()
{
close_index_file();
delete dictionary_memory;
delete postings_memory;
delete stats;
delete factory;
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(long which_stats)
{
if (which_stats & STAT_SUMMARY)
	stats->text_render(ANT_stats_memory_index::STAT_SUMMARY);
if (which_stats & STAT_MEMORY)
	stats->text_render(ANT_stats_memory_index::STAT_MEMORY);
if (which_stats & STAT_COMPRESSION)
	factory->text_render();
}

/*
	ANT_MEMORY_INDEX::FIND_NODE()
	-----------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::find_node(ANT_memory_index_hash_node *root, ANT_string_pair *string)
{
long cmp;
volatile ANT_memory_index_hash_node *finder = root;

if (finder == NULL)
	return NULL;

while ((cmp = string->strcmp((ANT_string_pair *)&(finder->string))) != 0)
	{
	if (cmp > 0)
		if (finder->left == NULL)
			return NULL;
		else
			finder = finder->left;
	else
		if (finder->right == NULL)
			return NULL;
		else
			finder = finder->right;
	}
return (ANT_memory_index_hash_node *)finder;
}

/*
	ANT_MEMORY_INDEX::FIND_ADD_NODE()
	---------------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string)
{
long cmp;

while ((cmp = string->strcmp(&(root->string))) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			{
			ANT_compare_and_swap(&root->left, new_memory_index_hash_node(string), NULL);
			return root->left;
			}
		else
			root = root->left;
	else
		if (root->right == NULL)
			{
			ANT_compare_and_swap(&root->right, new_memory_index_hash_node(string), NULL);
			return root->right;
			}
		else
			root = root->right;
	}
return root;
}

/*
	ANT_MEMORY_INDEX::ADD_TERM()
	----------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::add_term(ANT_string_pair *string, long long docno, long term_frequency)
{
long hash_value;
ANT_memory_index_hash_node *node;

stats->documents = docno;

hash_value = hash(string);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	ANT_compare_and_swap(&hash_table[hash_value], new_memory_index_hash_node(string), NULL);
	node = hash_table[hash_value];
	}
else
	node = find_add_node(hash_table[hash_value], string);

node->add_posting(docno, term_frequency);
return node;
}

/*
	ANT_MEMORY_INDEX::SET_DOCUMENT_DETAIL()
	---------------------------------------
*/
void ANT_memory_index::set_document_detail(ANT_string_pair *measure_name, long long score, long mode)
{
long hash_value;
ANT_memory_index_hash_node *node;

hash_value = hash(measure_name);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	ANT_compare_and_swap(&hash_table[hash_value], new_memory_index_hash_node(measure_name), NULL);
	node = hash_table[hash_value];
	}
else
	node = find_add_node(hash_table[hash_value], measure_name);

if (mode != MODE_MONOTONIC)
	node->current_docno = -1;			// store the value rather than the diff

node->add_posting(score);
}

/*
	ANT_MEMORY_INDEX::SET_VARIABLE()
	--------------------------------
*/
void ANT_memory_index::set_variable(char *measure_name, long long score)
{
ANT_string_pair squiggle(measure_name);

set_variable(&squiggle, score);
}

/*
	ANT_MEMORY_INDEX::SET_VARIABLE()
	--------------------------------
*/
void ANT_memory_index::set_variable(ANT_string_pair *measure_name, long long score)
{
long hash_value;
ANT_memory_index_hash_node *node;

hash_value = hash(measure_name);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	ANT_compare_and_swap(&hash_table[hash_value], new_memory_index_hash_node(measure_name), NULL);
	node = hash_table[hash_value];
	}
else
	node = find_add_node(hash_table[hash_value], measure_name);

node->set(score);
}

/*
	ANT_MEMORY_INDEX::ADD_INDEXED_DOCUMENT_NODE()
	---------------------------------------------
*/
void ANT_memory_index::add_indexed_document_node(ANT_memory_index_one_node *node, long long docno)
{
/*
	Add the term at the current node
*/
if (node->string[0] == '~')
	set_document_detail(&node->string, node->term_frequency, node->mode);
else
	{
	if (node->final_node == NULL)
		add_term(&node->string, docno, (long)node->term_frequency);
	else
		node->final_node->add_posting(docno, (long)node->term_frequency);
	}

/*
	Now check the left and the right subtrees for hash collisions
*/
if  (node->left != NULL)
	add_indexed_document_node(node->left, docno);

if  (node->right != NULL)
	add_indexed_document_node(node->right, docno);
}

/*
	ANT_MEMORY_INDEX::ADD_INDEXED_DOCUMENT()
	----------------------------------------
*/
void ANT_memory_index::add_indexed_document(ANT_memory_index_one *index, long long docno)
{
long node;

/*
	March through the hash table of the indexed document looking for non-NULL nodes
	then add those nodes to the collection's hash table
*/
for (node = 0; node < ANT_memory_index_one::HASH_TABLE_SIZE; node++)
	if (index->hash_table[node] != NULL)
		add_indexed_document_node(index->hash_table[node], docno);

largest_docno = docno;
stats->documents = docno;
}

#ifdef IMPACT_HEADER
	/*
		ANT_MEMORY_INDEX::IMPACT_ORDER_WITH_HEADER()
		--------------------------------------------
	*/
	long long ANT_memory_index::impact_order_with_header(ANT_compressable_integer *destination, ANT_compressable_integer *docid, unsigned short *term_frequency, long long document_frequency, unsigned char *max_local)
	{
	ANT_compressable_integer sum, bucket_size[1 << 16], bucket_prev_docid[1 << 16];
	ANT_compressable_integer *pointer[1 << 16], *current_docid, doc, *pruned_point;
	unsigned short *current, *end;
	long bucket, buckets_used;

	/*
		If we only have the term occuring in one document, then we don't need to bother
		doing any processing, so don't
	*/
	if (document_frequency == 1)
		{
		impact_header.the_quantum_count = document_frequency;

		impact_header.impact_value_ptr = impact_header.header_buffer;
		impact_header.doc_count_ptr = impact_header.header_buffer + impact_header.the_quantum_count;
		impact_header.impact_offset_ptr = impact_header.header_buffer + impact_header.the_quantum_count * 2;

		*impact_header.impact_value_ptr = *term_frequency;
		*impact_header.doc_count_ptr = document_frequency;
		*impact_header.impact_offset_ptr = 0;

		*destination = *docid;
		*max_local = (unsigned char)impact_header.header_buffer[0];
		return document_frequency;
		}

	/*
		Set all the buckets to empty;
	*/
	memset(bucket_size, 0, sizeof(bucket_size));

	/*
		Set the previous document ID to zero for each bucket (for difference encoding)
	*/
	memset(bucket_prev_docid, 0, sizeof(bucket_prev_docid));

	/*
		Compute the size of the buckets
	*/
	end = term_frequency + document_frequency;
	for (current = term_frequency; current < end; current++)
		bucket_size[*current]++;

	// find the number of non-zero buckets (the number of quantums)
	impact_header.the_quantum_count = 0;
	for (bucket = 0; bucket < (1 << quantization_bits); bucket++)
		if (bucket_size[bucket] != 0)
			impact_header.the_quantum_count++;

	// setup the pointers for the header
	impact_header.impact_value_ptr = impact_header.header_buffer;
	impact_header.doc_count_ptr = impact_header.header_buffer + impact_header.the_quantum_count;
	impact_header.impact_offset_ptr = impact_header.header_buffer + impact_header.the_quantum_count * 2;

	/*
		Compute the location of the pointers for each bucket
	*/
	pruned_point = NULL;
	buckets_used = sum = 0;
	for (bucket = (1 << quantization_bits) - 1; bucket >= 0; bucket--)
		{
		pointer[bucket] = destination + sum;
		if (bucket_size[bucket] != 0)
			{
			*impact_header.impact_value_ptr++ = bucket;
			*impact_header.doc_count_ptr++ = bucket_size[bucket];
			*impact_header.impact_offset_ptr++ = sum;
			buckets_used++;
			if (sum < static_prune_point && (sum + bucket_size[bucket]) >= static_prune_point)
				pruned_point = pointer[bucket] + (static_prune_point - sum);
			sum += bucket_size[bucket];
			}
		}

	/*
		Now generate the impact ordering
	*/
	current_docid = docid;
	doc = 0;
	for (current = term_frequency; current < end; current++)
		{
		doc += *current_docid;							// because the original list is difference encoded
		*pointer[*current]++ = doc - bucket_prev_docid[*current];		// because this list is also difference encoded
		bucket_prev_docid[*current] = doc;
		current_docid++;
		}

	/*
		The first frequency or impact-value is the max local impact for the term
	*/
	*max_local = (unsigned char)impact_header.header_buffer[0];

	/*
		Should we static print the postings list?
	*/
	if (pruned_point != NULL)
		return pruned_point - destination;

	/*
		Return the length of the impact ordered list
	*/
	return sum;
	}
#endif

/*
	ANT_MEMORY_INDEX::IMPACT_ORDER()
	--------------------------------
*/
long long ANT_memory_index::impact_order(ANT_compressable_integer *destination, ANT_compressable_integer *docid, unsigned short *term_frequency, long long document_frequency, unsigned char *max_local)
{
ANT_compressable_integer sum, bucket_size[1 << 16], bucket_prev_docid[1 << 16];
ANT_compressable_integer *pointer[1 << 16], *current_docid, doc, *zero_point;
unsigned short *current, *end;
long bucket, buckets_used;

/*
	Set all the buckets to empty;
*/
memset(bucket_size, 0, sizeof(bucket_size));

/*
	Set the previous document ID to zero for each bucket (for difference encoding)
*/
memset(bucket_prev_docid, 0, sizeof(bucket_prev_docid));

/*
	Compute the size of the buckets
*/
end = term_frequency + document_frequency;
for (current = term_frequency; current < end; current++)
	bucket_size[*current]++;

/*
	Compute the location of the pointers for each bucket
*/
zero_point = NULL;
buckets_used = sum = 0;
for (bucket = (1 << quantization_bits) - 1; bucket >= 0; bucket--)
	{
	pointer[bucket] = destination + sum + 2 * buckets_used; // the extra 1 (from 2) counts for the number terminator at the end of each quantum
	if (bucket_size[bucket] != 0)
		{
		*pointer[bucket]++ = bucket;
		buckets_used++;
		if (sum < static_prune_point && (sum + bucket_size[bucket]) >= static_prune_point)
			zero_point = pointer[bucket] + (static_prune_point - sum);
		sum += bucket_size[bucket];
		}
	}

/*
	Now generate the impact ordering
*/
current_docid = docid;
doc = 0;
for (current = term_frequency; current < end; current++)
	{
	doc += *current_docid;							// because the original list is difference encoded
	*pointer[*current]++ = doc - bucket_prev_docid[*current];		// because this list is also difference encoded
	bucket_prev_docid[*current] = doc;
	current_docid++;
	}

/*
	Finally terminate each impact list with a 0
*/
for (bucket = 0; bucket < (1 << quantization_bits); bucket++)
	if (bucket_size[bucket] != 0)
		*pointer[bucket] = 0;

/*
	The first frequency or impact-value is the max local impact for the term
*/
*max_local = (unsigned char)destination[0];

/*
	Should we static print the postings list?
*/
if (zero_point != NULL)
	{
	*zero_point = 0;
	return zero_point - destination + 1;		// +1 to include the zero_point
	}

/*
	Return the length of the impact ordered list
*/
return sum + 2 * buckets_used;
}

/*
	ANT_MEMORY_INDEX::GET_SERIALISED_POSTINGS()
	-------------------------------------------
*/
long long ANT_memory_index::get_serialised_postings(ANT_memory_index_hash_node *root, long long *doc_size, long long *tf_size)
{
long long total;

*doc_size = serialised_docids_size;
*tf_size = serialised_tfs_size;
while ((total = root->serialise_postings(serialised_docids, doc_size, serialised_tfs, tf_size)) == 0)
	{
	if (*doc_size > serialised_docids_size)
		{
		serialised_docids_size = *doc_size;
		serialised_docids = (unsigned char *)serialisation_memory->malloc(serialised_docids_size);
		}
	if (*tf_size > serialised_tfs_size)
		{
		serialised_tfs_size = *tf_size;
		serialised_tfs = (unsigned short *)serialisation_memory->malloc(serialised_tfs_size);
		}
	}
return total;
}

/*
	ANT_MEMORY_INDEX::RSV_ALL_NODES()
	---------------------------------
*/
double ANT_memory_index::rsv_all_nodes(double *minimum, ANT_memory_index_hash_node *root)
{
double right_min, right_max, left_min, left_max, min, max;
long long doc_size, tf_size;

right_max = left_max = max = -std::numeric_limits<double>::max();		// min() returns the minumum positive value
right_min = left_min = min = std::numeric_limits<double>::max();
/*
	What is the max from the children of this node?
*/
if (root->right != NULL)
	right_max = rsv_all_nodes(&right_min, root->right);
if (root->left != NULL)
	left_max = rsv_all_nodes(&left_min, root->left);

/*
	Now we compute the score for the current node
	First get the postings lists (docids and tf scores)
*/
get_serialised_postings(root, &doc_size, &tf_size);

/*
	Now we decompress the docids
*/
if (root->string[0] == '~')
	{
	/*
		Ignore "special" strings such as the document lengths array
	*/
	*minimum = ANT_min(left_min, right_min);
	return ANT_max(left_max, right_max);
	}
else
	{
	variable_byte.decompress(impacted_postings, serialised_docids, root->document_frequency);
	quantizer->get_max_min(&max, &min, root->collection_frequency, root->document_frequency, impacted_postings, serialised_tfs);
	/*
		now return the max of the three
	*/
	*minimum = ANT_min(left_min, right_min, min);
	return ANT_max(left_max, right_max, max);
	}
}

/*
	ANT_MEMORY_INDEX::NODE_TO_POSTINGS()
	------------------------------------
	This function puts the impact ordered (not compressed) postings in decompressed_postings_list
	and returns the length of that list (in integers).
*/
long long ANT_memory_index::node_to_postings(ANT_memory_index_hash_node *root)
{
long long timer, impacted_postings_length;

if (root->string[0] == '~')			// these are "special" strings in the index (e.g. document lengths)
	{
	variable_byte.decompress(impacted_postings, serialised_docids, root->document_frequency);
	impacted_postings_length = root->document_frequency;
	decompressed_postings_list[0] = impacted_postings[0];
	decompressed_postings_list[1] = impacted_postings[1];
	}
else
	{
	variable_byte.decompress(decompressed_postings_list, serialised_docids, root->document_frequency);
	if (quantizer != NULL && index_quantization)
		{
		timer = stats->start_timer();
		quantizer->quantize(maximum_collection_rsv, minimum_collection_rsv, root->collection_frequency, root->document_frequency, decompressed_postings_list, serialised_tfs);
		stats->time_to_quantize += stats->stop_timer(timer);
		}
#ifdef IMPACT_HEADER
	impacted_postings_length = impact_order_with_header(impacted_postings, decompressed_postings_list, serialised_tfs, root->document_frequency, &root->term_local_max_impact);
#else
	impacted_postings_length = impact_order(impacted_postings, decompressed_postings_list, serialised_tfs, root->document_frequency, &root->term_local_max_impact);
#endif
	}

return impacted_postings_length;
}

/*
	ANT_MEMORY_INDEX::SHOULD_PRUNE()
	--------------------------------
*/
inline long ANT_memory_index::should_prune(ANT_memory_index_hash_node *term)
{
if (stop_word_removal_mode == NONE)
	return false;
else if (term->string[0] == '~')
	return false;
else if (stop_word_removal_mode & PRUNE_CF_SINGLETONS && term->collection_frequency == 1)
	return true;
else if (stop_word_removal_mode & PRUNE_DF_SINGLETONS && term->document_frequency <= stop_word_df_frequencies)
	return true;
else if (stop_word_removal_mode & PRUNE_DF_FREQUENTS && (double)term->document_frequency / (double)largest_docno >= stop_word_max_proportion)
	return true;
else if (stop_word_removal_mode & PRUNE_NCBI_STOPLIST && ANT_stop_word::isstop(term->string.string()))
	return true;
else
	return false;
}

/*
	ANT_MEMORY_INDEX::SERIALISE_ALL_NODES()
	---------------------------------------
*/
long ANT_memory_index::serialise_all_nodes(ANT_file *file, ANT_memory_index_hash_node *root)
{
long terms = 0;
long long doc_size, tf_size, len, impacted_postings_length, current_disk_position;
#ifdef IMPACT_HEADER
	unsigned char *compressed_header_ptr, *compressed_postings_ptr;
	ANT_compressable_integer *end;
#endif
#ifdef SPECIAL_COMPRESSION
	ANT_compressable_integer temp;
#endif

if (root->right != NULL)
	terms += serialise_all_nodes(file, root->right);

if (!should_prune(root))
	{
	terms += 1;

	stats->term_occurences += root->collection_frequency;
	get_serialised_postings(root, &doc_size, &tf_size);

	stats->bytes_to_store_docids += doc_size;
	stats->bytes_to_store_tfs += tf_size;

	impacted_postings_length = node_to_postings(root);

	/*
		At this point the impact ordered (not compressed) postings list is in impacted_postings
		and the first 2 elements of decompressed_postings_list are the first 2 numbers (which are
		then shoved in the vocab file to save space (and a seek)).  impacted_postings_length stores
		the length of the impacted_postings array (in integers)
	*/
	#ifdef SPECIAL_COMPRESSION
		if (root->document_frequency <= 2)
			{
			/*
				Store the first postings(4 bytes) in the higher order of the 8 bytes
				and store the first term frequency (4 bytes) in the lower order of the 8 bytes
			*/

#ifndef IMPACT_HEADER
			/*
				If we aren't using IMPACT_HEADER then not doing this will
				lead to SPECIAL_COMPRESSION errors, so we have to do these manipulations
			*/
			if (root->string[0] != '~')
				{
				decompressed_postings_list[0] = impacted_postings[1];
				serialised_tfs[0] = impacted_postings[0];
				if (root->document_frequency == 2)
					{
					decompressed_postings_list[1] = impacted_postings[2] == 0 ? impacted_postings[4] : impacted_postings[2];
					serialised_tfs[1] = impacted_postings[2] == 0 ? impacted_postings[3] : impacted_postings[0];
					}
				}
#else
			/*
				We get the docids difference encoded, but if we have different tfs then they don't need to be
			*/
			if (root->document_frequency == 2 && serialised_tfs[0] != serialised_tfs[1])
				decompressed_postings_list[1] += decompressed_postings_list[0];
#endif

			/*
				We can _still_ get the tfs around the wrong way, still so put them around the right way
			*/
			if (root->document_frequency == 2 && serialised_tfs[1] > serialised_tfs[0])
				{
				temp = serialised_tfs[0];
				serialised_tfs[0] = serialised_tfs[1];
				serialised_tfs[1] = temp;

				temp = decompressed_postings_list[0];
				decompressed_postings_list[0] = decompressed_postings_list[1];
				decompressed_postings_list[1] = temp;
				}

			root->in_disk.docids_pos_on_disk = ((long long)decompressed_postings_list[0]) << 32 | serialised_tfs[0];

			/*
				Use impacted_length and end_pos_on_disk to store the second postings and term frequency
			*/
			if (root->document_frequency == 2)
				{
				root->in_disk.impacted_length = decompressed_postings_list[1];
				root->in_disk.end_pos_on_disk = serialised_tfs[1] + root->in_disk.docids_pos_on_disk; // because root->docids_pos_on_disk is subtracted later
				}
			else
				root->in_disk.impacted_length = root->in_disk.end_pos_on_disk = 0;
			}
		else
			{
			#ifdef IMPACT_HEADER
				if (root->string[0] == '~')
					{
					len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

					current_disk_position = file->tell();
					file->write(compressed_postings_list, len);

					root->in_disk.docids_pos_on_disk = current_disk_position;
					root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
					root->in_disk.end_pos_on_disk = file->tell();
					}
				else
					{
					impact_header.impact_value_start = impact_header.header_buffer;
					impact_header.doc_count_start = impact_header.header_buffer + impact_header.the_quantum_count;
					impact_header.impact_offset_start = impact_header.header_buffer + impact_header.the_quantum_count * 2;

					// compress the impact postings, one quantum at time, and update the corresponding offsets in the header
					end = impact_header.impact_offset_start + impact_header.the_quantum_count;
					impact_header.impact_offset_ptr = impact_header.impact_offset_start;

					compressed_postings_ptr = compressed_postings_list;
					for (impact_header.impact_offset_ptr = impact_header.impact_offset_start, impact_header.doc_count_ptr = impact_header.doc_count_start; impact_header.impact_offset_ptr != end; impact_header.impact_offset_ptr++, impact_header.doc_count_ptr++)
						{
						len = factory->compress(compressed_postings_ptr, (long long)1 + *impact_header.doc_count_ptr * sizeof(ANT_compressable_integer), impacted_postings + *impact_header.impact_offset_ptr, *impact_header.doc_count_ptr);
						// convert the pointer into offset
						*impact_header.impact_offset_ptr = compressed_postings_ptr - compressed_postings_list;
						compressed_postings_ptr += len;
						}

					// compress the impact header
					compressed_header_ptr = compressed_impact_header_buffer + ANT_impact_header::INFO_SIZE;
					len = factory->compress(compressed_header_ptr, compressed_impact_header_size, impact_header.header_buffer, impact_header.the_quantum_count * 3);
					// the offset for the beginning of the postings
					impact_header.beginning_of_the_postings = ANT_impact_header::INFO_SIZE + len;
					impact_header.set_INFO(compressed_impact_header_buffer);
					compressed_header_ptr += len;

					// write the impact header to disk
					current_disk_position = file->tell();
					file->write(compressed_impact_header_buffer, compressed_header_ptr - compressed_impact_header_buffer);

					// write the compressed postings to disk
					file->write(compressed_postings_list, compressed_postings_ptr - compressed_postings_list);

					root->in_disk.docids_pos_on_disk = current_disk_position;
					root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
					root->in_disk.end_pos_on_disk = file->tell();
				}
			#else
				len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

				current_disk_position = file->tell();
				file->write(compressed_postings_list, len);

				root->in_disk.docids_pos_on_disk = current_disk_position;
				root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
				root->in_disk.end_pos_on_disk = file->tell();
			#endif
			}
	#else // else #ifdef SPECIAL_COMPRESSION
		#ifdef IMPACT_HEADER
			if (root->string[0] == '~')
				{
				len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

				current_disk_position = file->tell();
				file->write(compressed_postings_list, len);

				root->in_disk.docids_pos_on_disk = current_disk_position;
				root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
				root->in_disk.end_pos_on_disk = file->tell();
				}
			else
				{
				impact_header.impact_value_start = impact_header.header_buffer;
				impact_header.doc_count_start = impact_header.header_buffer + impact_header.the_quantum_count;
				impact_header.impact_offset_start = impact_header.header_buffer + impact_header.the_quantum_count * 2;

				// compress the impact postings, one quantum at time, and update the corresponding offsets in the header
				end = impact_header.impact_offset_start + impact_header.the_quantum_count;
				impact_header.impact_offset_ptr = impact_header.impact_offset_start;

				compressed_postings_ptr = compressed_postings_list;
				for (impact_header.impact_offset_ptr = impact_header.impact_offset_start, impact_header.doc_count_ptr = impact_header.doc_count_start; impact_header.impact_offset_ptr != end; impact_header.impact_offset_ptr++, impact_header.doc_count_ptr++)
					{
					len = factory->compress(compressed_postings_ptr, (long long)1 + *impact_header.doc_count_ptr * sizeof(ANT_compressable_integer), impacted_postings + *impact_header.impact_offset_ptr, *impact_header.doc_count_ptr);
					// convert the pointer into offset
					*impact_header.impact_offset_ptr = compressed_postings_ptr - compressed_postings_list;
					compressed_postings_ptr += len;
					}

				// compress the impact header
				compressed_header_ptr = compressed_impact_header_buffer + ANT_impact_header::INFO_SIZE;
				len = factory->compress(compressed_header_ptr, compressed_impact_header_size, impact_header.header_buffer, impact_header.the_quantum_count * 3);
				// the offset for the beginning of the postings
				impact_header.beginning_of_the_postings = ANT_impact_header::INFO_SIZE + len;
				impact_header.set_INFO(compressed_impact_header_buffer);
				compressed_header_ptr += len;

				// write the impact header to disk
				current_disk_position = file->tell();
				file->write(compressed_impact_header_buffer, compressed_header_ptr - compressed_impact_header_buffer);

				// write the compressed postings to disk
				file->write(compressed_postings_list, compressed_postings_ptr - compressed_postings_list);

				root->in_disk.docids_pos_on_disk = current_disk_position;
				root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
				root->in_disk.end_pos_on_disk = file->tell();
				}
		#else
			len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

			current_disk_position = file->tell();
			file->write(compressed_postings_list, len);

			root->in_disk.docids_pos_on_disk = current_disk_position;
			root->in_disk.impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
			root->in_disk.end_pos_on_disk = file->tell();
		#endif //end of #ifdef IMPACT_HEADER
	#endif // end of #ifdef SPECIAL_COMPRESSION
	}

if (root->left != NULL)
	terms += serialise_all_nodes(file, root->left);

return terms;
}

/*
	ANT_MEMORY_INDEX::GENERATE_TERM_LIST()
	--------------------------------------
*/
long ANT_memory_index::generate_term_list(ANT_memory_index_hash_node *root, ANT_memory_index_hash_node **into, long where, int32_t *length_of_longest_term, int64_t *highest_df)
{
long term_length, terms = 0;

/*
	Recurse right
*/
if (root->right != NULL)
	terms = generate_term_list(root->right, into, where, length_of_longest_term, highest_df);

/*
	Only include this term if its not stopped somehow
*/
if (!should_prune(root))
	{
	/*
		Deal with the current node
	*/
	into[where + terms] = root;

	/*
		Compute the string length of the longest string
	*/
	if ((term_length = (long)root->string.length()) > *length_of_longest_term)
		*length_of_longest_term = term_length;

	/*
		Compute the highest DF value
	*/
	if (root->document_frequency > *highest_df)
		*highest_df = root->document_frequency;

	/*
		And yes, we were a term
	*/
	terms++;
	}

/*
	Recurse left
*/
if (root->left != NULL)
	terms += generate_term_list(root->left, into, where + terms, length_of_longest_term, highest_df);

return terms;
}

/*
	ANT_MEMORY_INDEX::FIND_END_OF_NODE()
	------------------------------------
*/
ANT_memory_index_hash_node **ANT_memory_index::find_end_of_node(ANT_memory_index_hash_node **start)
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
	ANT_MEMORY_INDEX::WRITE_NODE()
	------------------------------
*/
ANT_memory_index_hash_node **ANT_memory_index::write_node(ANT_file *file, ANT_memory_index_hash_node **start)
{
uint8_t zero = 0;
uint64_t eight_byte;
uint32_t four_byte, string_pos;
uint32_t terms_in_node, current_node_head_length;
#ifdef TERM_LOCAL_MAX_IMPACT
	uint8_t one_byte;
#endif
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

#ifdef TERM_LOCAL_MAX_IMPACT
	one_byte = (unsigned char)((*current)->term_local_max_impact);
	file->write((unsigned char *)&one_byte, sizeof(one_byte));
#endif

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
	ANT_MEMORY_INDEX::OPEN_INDEX_FILE()
	-----------------------------------
*/
void ANT_memory_index::open_index_file(char *filename)
{
char file_header[] = "ATIRE Search Engine Index File\n\0\0";

if (index_file == NULL && filename != NULL)
	{
	index_file = new ANT_file;

	if (index_file->open(filename, "w+bx") == 0)
		{
		fprintf(stderr, "Couldn't open index file for writing \"%s\"\n", filename);
		exit(-1);
		}

	index_file->setvbuff(DISK_BUFFER_SIZE);
	stats->disk_buffer = DISK_BUFFER_SIZE;
	index_file->write((unsigned char *)file_header, sizeof(file_header));
	}
}

/*
	ANT_MEMORY_INDEX::CLOSE_INDEX_FILE()
	------------------------------------
*/
void ANT_memory_index::close_index_file(void)
{
if (index_file != NULL)
	{
	index_file->close();
	delete index_file;
	index_file = NULL;
	}
}

/*
	ANT_MEMORY_INDEX::SERIALISE()
	-----------------------------
*/
long ANT_memory_index::serialise(ANT_ranking_function_factory *factory)
{
uint8_t zero = 0;
int32_t length_of_longest_term = 0;
uint32_t longest_postings_size, four_byte;
int64_t highest_df = 0;
uint64_t file_position, terms_in_root, eight_byte;
long terms_in_node, unique_terms = 0, max_terms_in_node = 0, hash_val, where, bytes, btree_root_size;
ANT_memory_index_hash_node **term_list, **here;
ANT_btree_head_node *header, *current_header, *last_header;
ANT_memory_index_hash_node *node;
double max_rsv_for_node, min_rsv_for_node;
long long doc_size, tf_size;
long long timer;
long long pos;

/*
	Can't serialise if we're an in-memory index
*/
if (index_file == NULL)
	return 0;

#ifdef IMPACT_HEADER
	impact_header.postings_chain = 0;
	impact_header.chain_length = 0;
	impact_header.the_quantum_count = 0;
	// extra 1 byte is for the compression scheme
	impact_value_size = 1 + sizeof(*impact_header.impact_value_start) * ANT_impact_header::NUM_OF_QUANTUMS;
	doc_count_size = 1 + sizeof(*impact_header.doc_count_start) * ANT_impact_header::NUM_OF_QUANTUMS;
	impact_offset_size = 1 + sizeof(*impact_header.impact_offset_start) * ANT_impact_header::NUM_OF_QUANTUMS;
	impact_header.header_size =  impact_value_size + doc_count_size + impact_offset_size;
	impact_header.header_buffer = (ANT_compressable_integer *)serialisation_memory->malloc(impact_header.header_size);
	compressed_impact_header_size = (long long)1 + ANT_impact_header::INFO_SIZE + impact_header.header_size;
	compressed_impact_header_buffer = (unsigned char *)serialisation_memory->malloc(compressed_impact_header_size);

	// the first compressed byte is a indication of what compression scheme is used in each quantum
	compressed_postings_list_length = 1 * ANT_impact_header::NUM_OF_QUANTUMS + (sizeof(*decompressed_postings_list) * largest_docno);
	decompressed_postings_list = (ANT_compressable_integer *)serialisation_memory->malloc(compressed_postings_list_length - ANT_impact_header::NUM_OF_QUANTUMS);
	compressed_postings_list = (unsigned char *)serialisation_memory->malloc(compressed_postings_list_length);
	// 1 * NUM_OF_QUANTUMS because the TF in each of 255 lists
	impacted_postings = (ANT_compressable_integer *)serialisation_memory->malloc(compressed_postings_list_length + (1 * ANT_impact_header::NUM_OF_QUANTUMS * sizeof(*decompressed_postings_list)));
	stats->bytes_for_decompression_recompression += impact_header.header_size * 2 + compressed_postings_list_length * 3 + (1 * ANT_impact_header::NUM_OF_QUANTUMS * sizeof(*decompressed_postings_list)) - ANT_impact_header::NUM_OF_QUANTUMS;
#else
	compressed_postings_list_length = 1 + (sizeof(*decompressed_postings_list) * largest_docno);
	decompressed_postings_list = (ANT_compressable_integer *)serialisation_memory->malloc(compressed_postings_list_length - 1);
	compressed_postings_list = (unsigned char *)serialisation_memory->malloc(compressed_postings_list_length);
	impacted_postings = (ANT_compressable_integer *)serialisation_memory->malloc(compressed_postings_list_length + (512 * sizeof(*decompressed_postings_list)));		// 512 because the TF and the 0 at the end of each of 255 lists
	stats->bytes_for_decompression_recompression += compressed_postings_list_length * 3 + 512 - 1;
#endif

/*
	If we have the documents stored on disk then we need to store the position of the end of the final document.
	But, only add the position if we are using an index with the documents in it.
*/
if (find_node(hash_table[hash(&squiggle_document_offsets)], &squiggle_document_offsets) != NULL)
	{
	set_document_detail(&squiggle_document_offsets, index_file->tell(), MODE_MONOTONIC);			// store the position of the end of the last document
#ifdef IMPACT_HEADER
	set_variable(&squiggle_document_longest, compressed_longest_raw_document_size);
#else
	set_document_detail(&squiggle_document_longest, compressed_longest_raw_document_size);		// store the length of the longest document once it is decompressed
#endif
	}

/*
	If we are storing filenames on disk then store the location of the filename buffer and serialise it.
*/
if (document_filenames != NULL)
	{
	pos = index_file->tell();
	set_variable("~documentfilenamesstart", pos);
	serialise_filenames(document_filenames);
	pos = index_file->tell();
	set_variable("~documentfilenamesfinish", pos);
	}

/*
	if the index is static pruned
*/
if (static_prune_point < largest_docno)
	set_variable("~trimpoint", static_prune_point);

/*
	Compute the array of document lengths and other parameters necessary for impact ordering on
	the relevance ranking functions
*/

node = find_add_node(hash_table[hashed_squiggle_length], &squiggle_length);

get_serialised_postings(node, &doc_size, &tf_size);
document_lengths = (ANT_compressable_integer *)serialisation_memory->malloc(stats->bytes_to_quantize += ((largest_docno  + 1) * sizeof(ANT_compressable_integer)));
variable_byte.decompress(document_lengths, serialised_docids, node->document_frequency);

/*
	If we want to quantize the ranking scores for impact ordering in the index then
	we need to compute the min and max scores the function will produce and then
	use those later - this takes time so time it.
*/
timer = stats->start_timer();

/*
	Create the quantizer
*/
if ((quantizer = factory->get_indexing_ranker(largest_docno, document_lengths, &index_quantization, &quantization_bits)) != NULL)
	{
	/*
		Store (in the index) the fact that we're a quantized index
	*/
	if (index_quantization)
		set_variable("~quantized", 1);

	/*
		Now compute the maximum impact score across the collection
	*/
	maximum_collection_rsv = -std::numeric_limits<double>::max();			// min() returns the minimum positive value
	minimum_collection_rsv = std::numeric_limits<double>::max();
	for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
		if (hash_table[hash_val] != NULL)
			{
			max_rsv_for_node = rsv_all_nodes(&min_rsv_for_node, hash_table[hash_val]);
			if (max_rsv_for_node > maximum_collection_rsv)
				maximum_collection_rsv = max_rsv_for_node;
			if (min_rsv_for_node < minimum_collection_rsv)
				minimum_collection_rsv = min_rsv_for_node;
			}

	set_variable("~quantmax", *(long long *)&maximum_collection_rsv);
	set_variable("~quantmin", *(long long *)&minimum_collection_rsv);
	}

if (quantizer == NULL || !index_quantization)
	quantization_bits = 8; // this is by default for tf-indexes

/*
	Store how long quantizaton took.
*/
stats->time_to_quantize += stats->stop_timer(timer);

/*
	Write the postings
*/
for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
	if (hash_table[hash_val] != NULL)
		{
		if ((terms_in_node = serialise_all_nodes(index_file, hash_table[hash_val])) > max_terms_in_node)
			max_terms_in_node = terms_in_node;
		unique_terms += terms_in_node;
		}

/*
	Generate a list of all the unique terms in the collection
*/
bytes = sizeof(*term_list) * (unique_terms + 1);
stats->bytes_used_to_sort_term_list = bytes;
term_list = (ANT_memory_index_hash_node **)serialisation_memory->malloc(bytes);
where = 0;
for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
	if (hash_table[hash_val] != NULL)
		where += generate_term_list(hash_table[hash_val], term_list, where, &length_of_longest_term, &highest_df);
term_list[unique_terms] = NULL;

//printf("unique_terms: %ld\n", unique_terms);

/*
	Sort the term list
*/
qsort(term_list, unique_terms, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

/*
	Work out how many nodes there are in the root of the b-tree
	(Counts the number of nodes in the first-level of the dictionary)
*/
btree_root_size = 0;
for (here = term_list; *here != NULL; here = find_end_of_node(here))
	btree_root_size++;

/*
	Write the term list and generate the header list
	(Generate the first-level of the dictionary, and write the dictionary's
	  secondary-level to disk)
*/
current_header = header = (ANT_btree_head_node *)serialisation_memory->malloc(sizeof(ANT_btree_head_node) * btree_root_size);
here = term_list;
while (*here != NULL)
	{
	current_header->disk_pos = index_file->tell();
	current_header->node = *here;
	current_header++;
	here = write_node(index_file, here);
	}
last_header = current_header;
terms_in_root = last_header - header;

//printf("btree_root_size++: %ld, terms_in_root: %ld\n", btree_root_size, terms_in_root);

/*
	Take note of where the header will be located on disk
	(Remember where the first-level of the dictionary gets started)
*/
file_position = index_file->tell();

/*
	Write the header to disk N then N * (string, offset) pairs
	(Write the number of the terms in the dictionary's first-level, and
	 write all terms of the first-level to disk)
*/
index_file->write((unsigned char *)&terms_in_root, sizeof(terms_in_root));	// 4 bytes

//printf("Terms in root:%llu\n", (unsigned long long) terms_in_root);

for (current_header = header; current_header < last_header; current_header++)
	{
	index_file->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	index_file->write(&zero, sizeof(zero));									// 1 byte
	eight_byte = current_header->disk_pos;
	index_file->write((unsigned char *)&eight_byte, sizeof(eight_byte));		// 8 bytes
	}


/*
	Write the location of the header to file
*/
//printf("Root pos on disk:%llu\n", (unsigned long long) file_position);
index_file->write((unsigned char *)&file_position, sizeof(file_position));	// 8 bytes

/*
	The string length of the longest term
*/
index_file->write((unsigned char *)&length_of_longest_term, sizeof(length_of_longest_term));		// 4 bytes

#ifdef IMPACT_HEADER
	/*
		The number of unique terms
	*/
	four_byte = unique_terms;
	index_file->write((unsigned char *)&four_byte, sizeof(four_byte));
#endif

/*
	The maximum length of a compressed posting list
*/
#ifdef IMPACT_HEADER
	longest_postings_size = (uint32_t)(impact_header.header_size + serialised_docids_size + serialised_tfs_size);
#else
	longest_postings_size = (uint32_t)(serialised_docids_size + serialised_tfs_size);
#endif
index_file->write((unsigned char *)&longest_postings_size, sizeof(longest_postings_size));	// 4 byte

/*
	and the maximum number of postings in a postings list (that is, the largest document frequencty (DF))
*/
index_file->write((unsigned char *)&highest_df, sizeof(highest_df));		// 8 bytes

//
// 64 bit integer reserved for future use (maybe checksum)
//
eight_byte = 0;
index_file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

//
// 64 bit integer reserved for future use (collection name)
//
eight_byte = ANT_file_signature_index;
index_file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

//
// 32 bit integer for version number (ANT_version)
//
four_byte = (uint32_t)ANT_version;
index_file->write((unsigned char *)&four_byte, sizeof(four_byte));

//
// 32 bit integer for ANT_ID_THESAURUS_SIGNATURE_MAJOR (tools/wordnet_to_ant_thesaurus.c)
//
four_byte = (uint32_t)ANT_file_signature;
index_file->write((unsigned char *)&four_byte, sizeof(four_byte));


/*
	We're done!
*/
return 1;
}

/*
	ANT_MEMORY_INDEX::ADD_TO_FILENAME_REPOSITORY()
	----------------------------------------------
	The filenames are stored in a big buffer.  The start of that buffer points to the
	start of the previous buffer, and so on until we get a NULL pointer.
*/
void ANT_memory_index::add_to_filename_repository(char *filename)
{
long long remaining, length;
char *new_buffer;

length = strlen(filename) + 1;		// +1 to include the '\0'
if ((document_filenames_used + length) > document_filenames_chunk_size)
	{
	if ((remaining = document_filenames_chunk_size - document_filenames_used) != 0)
		memcpy(document_filenames + document_filenames_used, filename, (size_t)remaining);
	new_buffer = (char *)titles_memory->malloc(document_filenames_chunk_size);
	*(char **)new_buffer = document_filenames;
	document_filenames_used = sizeof(char *);

	filename += remaining;
	length -= remaining;
	document_filenames = new_buffer;
	}
if (length != 0)
	{
	memcpy(document_filenames + document_filenames_used, filename, (size_t)length);
	document_filenames_used += length;
	}
}

/*
	ANT_MEMORY_INDEX::SERIALISE_FILENAMES()
	---------------------------------------
*/
void ANT_memory_index::serialise_filenames(char *source, long depth)
{
if (source == NULL)
	return;

serialise_filenames(*(char **)source, depth + 1);

if (depth == 0)
	index_file->write((unsigned char *)(source + sizeof(char *)), document_filenames_used - sizeof(char *));
else
	index_file->write((unsigned char *)(source + sizeof(char *)), document_filenames_chunk_size - sizeof(char *));
}

/*
	ANT_MEMORY_INDEX::ADD_TO_DOCUMENT_REPOSITORY()
	----------------------------------------------
*/
void ANT_memory_index::add_to_document_repository(char *filename, char *compressed_document, long compressed_length, long raw_length)
{
long long start, timer;

if (index_file == NULL)
	return;			// don't write documents if we're an in-memory indexer

timer = stats->start_timer();

if (filename != NULL)
	add_to_filename_repository(filename);

if (compressed_document != NULL)
	{
	if (raw_length > compressed_longest_raw_document_size)
		compressed_longest_raw_document_size = raw_length;

	start = index_file->tell();
	index_file->write((unsigned char *)compressed_document, compressed_length);
	stats->bytes_to_store_documents_on_disk += compressed_length;
	set_document_detail(&squiggle_document_offsets, start, MODE_MONOTONIC);		// use the search engine itself to store the offsets in the index

	documents_in_repository++;
	}
stats->time_to_store_documents_on_disk += stats->stop_timer(timer);
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(ANT_memory_index_hash_node *root, unsigned char *serialised_docids, long doc_size, unsigned short *serialised_tfs, long tf_size)
{
unsigned char *pos;
long tf, doc = 0;

tf = tf_size;		// this does nothing but was added to remove a compiler warning about an unused parameter
printf("\t%s (df:%lld cf:%lld):", root->string.str(), root->document_frequency, root->collection_frequency);
pos = serialised_docids;
while (pos < serialised_docids + doc_size)
	{
	doc += root->decompress(&pos);
	tf = *serialised_tfs++;
	printf("(%ld,%ld),", doc, tf);
	}
putchar('\n');
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(ANT_compressable_integer *docid, unsigned char *term_frequency, long long document_frequency)
{
long long doc;
ANT_compressable_integer *current_docid;
unsigned char *current, *end;

printf("PRE-IMPACT:");
end = term_frequency + document_frequency;
current_docid = docid;
doc = 0;
for (current = term_frequency; current < end; current++)
	{
	doc += *current_docid;							// because the original list is difference encoded
	printf("<%lld,%lld>", (long long)*current, (long long)doc);
	current_docid++;
	}
printf("\n");
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(ANT_compressable_integer *impact_ordering, size_t document_frequency)
{
long docid;
ANT_compressable_integer *current, *end, tf;

printf("POSTIMPACT:");
current = impact_ordering;
end = impact_ordering + document_frequency;

while (current < end)
	{
	end += 2;
	tf = *current++;
	docid = 0;
	while (*current != 0)
		{
		docid += *current++;
		printf("<%lld,%lld>", (long long)tf, (long long)docid);
		}
	current++;
	}
printf("\n");
}
