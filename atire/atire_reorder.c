/*
	ATIRE_MERGE.C
	-------------
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
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
long long postings_list_size = 2;
unsigned char *postings_list = new unsigned char[postings_list_size + ANT_COMPRESSION_FACTORY_END_PADDING];
unsigned char *new_postings_list;
unsigned char *temp;
const double postings_growth_factor = 1.6;

int docid_sort(const void *a, const void *b)
{
	return *((ANT_compressable_integer *)a) - *((ANT_compressable_integer *)b);
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

ANT_string_pair *t = new ANT_string_pair(term); node = new (memory_stats->postings_memory) ANT_memory_index_hash_node(memory_stats->postings_memory, memory_stats->postings_memory, t, memory_stats); delete t;

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
		raw[4] = raw[3] = 0;
		raw[2] = raw[1];
		raw[1] = raw[0];
		raw[0] = 1;
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
		printf("Growing postings list size to: %llu\n", (unsigned long long)(postings_list_size * postings_growth_factor));
		new_postings_list = new unsigned char[(unsigned long long)(postings_list_size * postings_growth_factor) + ANT_COMPRESSION_FACTORY_END_PADDING];
		memcpy(new_postings_list, postings_list, postings_list_size * sizeof(*postings_list));
		
		delete [] postings_list;
		postings_list = new_postings_list;
		
		postings_list_size = (unsigned long long)(postings_list_size * postings_growth_factor);
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

ANT_string_pair *t = new ANT_string_pair(term); node = new (memory_stats->postings_memory) ANT_memory_index_hash_node(memory_stats->postings_memory, memory_stats->postings_memory, t, memory_stats); delete t;

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
		printf("Growing postings list size to: %llu\n", (unsigned long long)(postings_list_size * postings_growth_factor));
		new_postings_list = new unsigned char[(unsigned long long)(postings_list_size * postings_growth_factor) + ANT_COMPRESSION_FACTORY_END_PADDING];
		memcpy(new_postings_list, postings_list, postings_list_size * sizeof(*postings_list));
		postings_ptr = new_postings_list + (postings_ptr - postings_list);
		
		delete [] postings_list;
		postings_list = new_postings_list;
		
		postings_list_size = (unsigned long long)(postings_list_size * postings_growth_factor);
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

ANT_file *index = new ANT_file;
index->open(param_block.index_filename, "w");
index->write((unsigned char *)file_header, sizeof(file_header));

long long combined_docs = 0, maximum_terms = 0;
long long number_engines = argc - first_param;
long long document;
long long longest_postings = 0;
long long document_filenames_start, document_filenames_finish;
unsigned long longest_document = 0, compress_buffer_size;
char *document_compress_buffer;

ANT_memory memory;
ANT_search_engine search_engine(&memory);
search_engine.open(argv[first_param]);

ANT_compression_factory *factory = new ANT_compression_factory;
ANT_stats_time stats;
ANT_stats_memory_index *memory_stats = new ANT_stats_memory_index(&memory, &memory);
ANT_search_engine_btree_leaf *leaf = new ANT_search_engine_btree_leaf;
ANT_btree_iterator iterator(&search_engine);


long long new_order_doc_count = 0;
char *new_order_buffer;
char **new_order = NULL;

new_order_buffer = ANT_disk::read_entire_file(argv[first_param + 1]);
if (new_order_buffer == NULL)
	exit(printf("Unable to read new order list (%s)\n", argv[first_param + 1]));
new_order = ANT_disk::buffer_to_list(new_order_buffer, &new_order_doc_count);

if (new_order_doc_count != search_engine.document_count())
	exit(printf("Reordered document count (%lld) does not match index document count (%lld)\n", new_order_doc_count, search_engine.document_count()));

combined_docs = search_engine.document_count();
printf("Read %lld docs from %s\n", new_order_doc_count, argv[first_param + 1]);

long long *mapping = new long long[new_order_doc_count];
char filename_buffer[1024];
long long found = 0;

ANT_memory_index_hash_node *p;
ANT_memory_index_hash_node **term_list = new ANT_memory_index_hash_node *[search_engine.get_unique_term_count() + 1];

std::map<std::string, int> old_id;
for (long long i = 0; i < search_engine.document_count(); i++)
	{
	old_id[std::string(search_engine.get_document_filename(filename_buffer, i))] = i;
	}

// create a mapping such that mapping[old docid] = new docid
for (long long i = 0; i < new_order_doc_count; i++)
	{
	if (i % 1000 == 0)
		printf("%lld\n", i);
	auto filename = std::string(new_order[i]);
	mapping[i] = old_id[filename];
	}

auto start = search_engine.get_variable("~documentfilenamesstart");
auto end = search_engine.get_variable("~documentfilenamesfinish");

long long *filename_index_offsets = new long long[combined_docs];
long long filename_offset_sum = 0;
long long filename_offset = 0;
char *doc_buf = NULL;
unsigned long buf_len = 0;
doc_buf = (char *)realloc(doc_buf, end - start);
auto doc_filenames = search_engine.get_document_filenames(doc_buf, &buf_len);

document_filenames_start = index->tell();
for (document = 0; document < search_engine.document_count(); document++)
	{
	/* auto new_document = mapping[document]; */
	/* printf("Writing filename: '%s'\n", doc_filenames[new_document]); */
	filename_index_offsets[filename_offset++] = filename_offset_sum;
	filename_offset_sum += strlen(new_order[document]) + 1;
	index->write((unsigned char *)new_order[document], strlen(new_order[document]) + 1);
	}
document_filenames_finish = index->tell();

free(doc_buf);
free(doc_filenames);

if ((p = write_variable("~documentfilenamesstart", document_filenames_start, memory_stats, index, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;
if ((p = write_variable("~documentfilenamesfinish", document_filenames_finish, memory_stats, index, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;

document_filenames_start = index->tell();
index->write((unsigned char *)filename_index_offsets, sizeof(*filename_index_offsets) * combined_docs);
index->write((unsigned char *)&filename_offset_sum, sizeof(filename_offset_sum));
document_filenames_finish = index->tell();

if ((p = write_variable("~documentfilenamesindexstart", document_filenames_start, memory_stats, index, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;
if ((p = write_variable("~documentfilenamesindexfinish", document_filenames_finish, memory_stats, index, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;

offset = 0;
ANT_compressable_integer *raw = search_engine.get_document_lengths(&dummy);
ANT_compressable_integer *new_lengths = new ANT_compressable_integer[search_engine.document_count()];
for (document = 0; document < search_engine.document_count(); document++)
	new_lengths[mapping[document]] = raw[document] + 1;

leaf->impacted_length = search_engine.document_count();
leaf->local_collection_frequency = search_engine.document_count();
leaf->local_document_frequency = search_engine.document_count();

if ((p = write_postings("~length", new_lengths, index, memory_stats, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
	term_list[terms_so_far++] = p;

auto stemmer = search_engine.get_variable("~stemmer");
if (stemmer)
	if ((p = write_variable("~stemmer", stemmer, memory_stats, index, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;

ANT_compressable_integer *decompress_buffer = new ANT_compressable_integer[combined_docs];

ANT_compressable_integer *impact_headers = new ANT_compressable_integer[ANT_impact_header::NUM_OF_QUANTUMS * 3];
ANT_compressable_integer quantum_counts;
ANT_compressable_integer postings_begin;

ANT_compressable_integer quantums_processed;
ANT_compressable_integer *current_impact_header;
ANT_compressable_integer quantum, number_quantums_used, postings_offset;

ANT_compressable_integer *workspace = new ANT_compressable_integer[combined_docs];
ANT_compressable_integer previous_docid = 0;
ANT_compressable_integer *current;
ANT_compressable_integer done = 0;

char *term = iterator.first(NULL);
long long processed = 0;

while (term != NULL)
	{
	if (*term == '~')
		break;

	if (param_block.reporting_frequency != 0 && processed % param_block.reporting_frequency == 0)
		printf("%s\n", term);

	iterator.get_postings_details(leaf);
	raw = (ANT_compressable_integer *)search_engine.get_postings(leaf, (unsigned char *)raw);

	quantum_counts = (ANT_compressable_integer)((uint32_t *)raw)[4];
	postings_begin = (ANT_compressable_integer)((uint32_t *)raw)[5];

	quantums_processed = 0;

	/*
	   Decompress the header, * 3 -> impact values, doc counts, impact offsets
	*/
	factory->decompress(impact_headers, (unsigned char *)raw + ANT_impact_header::INFO_SIZE, quantum_counts * 3);

	current = workspace;
	offset = 0;
	for (quantums_processed = 0; quantums_processed < quantum_counts; quantums_processed++) 
	{
		previous_docid = 0;

		auto impact_offset = impact_headers[quantums_processed + (quantum_counts * 2)];
		auto number_documents = impact_headers[quantums_processed + quantum_counts];
		factory->decompress(decompress_buffer, (unsigned char *)raw + postings_begin + impact_offset, number_documents);

		*current = (ANT_compressable_integer)decompress_buffer[0];
		previous_docid = *current++;
		for (document = 1; document < number_documents; document++)
			{
			*current = decompress_buffer[document] + previous_docid;
			previous_docid = *current++;
			}

		for (auto jj = 0; jj < number_documents; jj++)
			workspace[offset + jj] = mapping[workspace[offset + jj] - 1] + 1; // -1 to get to 0-based, +1 to get to 1-based

		qsort(workspace + offset, number_documents, sizeof(*workspace), docid_sort);

		// diff encode
		for (auto jj = (offset + number_documents); jj > offset; jj--)
			workspace[jj] -= workspace[jj - 1];

		impact_headers[quantums_processed + (2 * quantum_counts)] = offset;
		offset += number_documents;
	}

	if ((p = write_impact_header_postings(term, impact_headers, quantum_counts, workspace, index, memory_stats, leaf, &param_block, combined_docs, &longest_postings, factory)) != NULL)
		term_list[terms_so_far++] = p;

	term = iterator.next();
	processed++;
	}

/*
	Now we've done all the postings, it's time to write the dictionary and header.
*/
term_list[terms_so_far] = NULL;

qsort(term_list, terms_so_far, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

uint8_t zero = 0;
uint32_t four_byte;
uint64_t eight_byte;
long btree_root_size = 0;
int32_t longest_term = 0;
int64_t highest_df = 0;
uint64_t terms_in_root;

ANT_memory_index_hash_node **here = new ANT_memory_index_hash_node*[search_engine.get_unique_term_count() * 2];
for (here = term_list; *here != NULL; here = find_end_of_node(here))
	btree_root_size++;

/*
	Write the second level of the btree.
*/
ANT_btree_head_node *header, *current_header, *last_header;
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
four_byte = (uint32_t)terms_so_far;
index->write((unsigned char *)&four_byte, sizeof(four_byte));

four_byte = (uint32_t)longest_postings;
/*
	Include the extra space that's needed for impact header itself
*/
four_byte += 3 * sizeof(*impact_headers) * ANT_impact_header::NUM_OF_QUANTUMS + ANT_impact_header::INFO_SIZE;
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

return 0;
}
