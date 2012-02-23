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
#include "file.h"
#include "indexer_param_block.h"
#include "maths.h"
#include "memory.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"
#include "ranking_function_term_count.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	USAGE()
	-------
*/
void usage(char *program_name)
{
puts(ANT_version_string);
printf("Usage: %s [option...] <index 1> <index 2> ... <index n>\n", program_name);
puts("");

puts("See COMPRESSION, OPTIMISATIONS, TERM EXPANSION from index");
exit(0);
}

/*
	PROCESS()
	---------
*/
void process(ANT_compressable_integer *impact_ordering, long long document_frequency, ANT_weighted_tf *tf_values, long long offset)
{
ANT_compressable_integer tf;
long long docid;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + document_frequency;

while (current < end)
	{
	end += 2;
	docid = -1;
	tf = *current++;
	while (*current != 0)
		{
		docid += *current++;
		tf_values[docid + offset] = (ANT_weighted_tf)tf;
		}
	current++;
	}
}

/*
	WRITE_POSTINGS()
	----------------
*/
ANT_memory_index_hash_node *write_postings(char *term, ANT_compressable_integer *raw, unsigned char *postings_list, long long postings_list_size, ANT_file *index, ANT_memory_index *combined_term_index, ANT_search_engine_btree_leaf *leaf, long long *longest_postings)
{
ANT_compression_factory factory;
ANT_memory_index_hash_node *node;
uint64_t current_disk_position;
long long len = 0;

node = combined_term_index->add_term(new ANT_string_pair(term), 0);

node->collection_frequency = leaf->local_collection_frequency;
node->document_frequency = leaf->local_document_frequency;

#ifdef SPECIAL_COMPRESSION
if (node->document_frequency <= 2)
	{
	/*
		Squiggle variables are given to us as an array of values
		but we need to put them out impact ordered, so here we
		impact order them under a 'tf' of 1
	*/
	if (node->string[0] == '~')
		{
		raw[2] = raw[1];
		raw[1] = raw[0];
		raw[0] = 1;
		}
	
	node->in_disk.docids_pos_on_disk = ((long long)raw[1]) << 32 | raw[0];
	
	if (node->document_frequency != 2)
		node->in_disk.impacted_length = node->in_disk.end_pos_on_disk = 0;
	else
		{
		node->in_disk.impacted_length = (raw[2] == 0 ? raw[4] : raw[2]) + raw[1];
		node->in_disk.end_pos_on_disk = (raw[2] == 0 ? raw[3] : raw[0]) + node->in_disk.docids_pos_on_disk;
		}
	}
else
#endif
	{
	len = factory.compress(postings_list, postings_list_size, raw, leaf->impacted_length);
	
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
	WRITE_VARIABLE()
	----------------
*/
ANT_memory_index_hash_node *write_variable(char *term, long long value, unsigned char *postings_list, long long postings_list_size, ANT_file *index, ANT_memory_index *combined_term_index, ANT_search_engine_btree_leaf *leaf, long long *longest_postings)
{
ANT_compressable_integer raw[3]; // make room for it to be impacted in write_postings

raw[0] = ((unsigned long long)value) >> 32;
raw[1] = ((unsigned long long)value) & 0xFFFFFFFF;
raw[1] = raw[1] - raw[0]; // difference encoded

leaf->impacted_length = leaf->local_collection_frequency = leaf->local_document_frequency = 2;

return write_postings(term, raw, postings_list, postings_list_size, index, combined_term_index, leaf, longest_postings);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
int i;
long long postings_list_size = 500 * 1024;
long long raw_list_size = 500 * 1024;
long long offset, engine, upto = 0;
long long trimpoint = LONG_MAX;
long long this_trimpoint;

uint64_t current_disk_position;
char file_header[] = "ANT Search Engine Index File\n\0\0";
unsigned char *postings_list;

ANT_indexer_param_block param_block(argc, argv);

long first_param = param_block.parse();
long long combined_docs = 0, maximum_terms = 0;
long long number_engines = argc - first_param;
long long longest_postings = 0;
long long document_filenames_start, document_filenames_finish;
unsigned long longest_document = 0, buffer_size, compress_buffer_size;
char *document_decompress_buffer, *document_compress_buffer;

if (number_engines < 2)
	usage(argv[0]);

ANT_memory *memory = new ANT_memory[number_engines];
ANT_search_engine **search_engines = (ANT_search_engine **)malloc(sizeof(*search_engines) * number_engines);
ANT_btree_iterator **iterators = (ANT_btree_iterator **)malloc(sizeof(*iterators) * number_engines);
char **terms = (char **)malloc(sizeof(*terms) * number_engines);

/*
	Allocate 1 more for these to allow the merged results to be put on the end
*/
ANT_search_engine_btree_leaf **leaves = (ANT_search_engine_btree_leaf **)malloc(sizeof(*leaves) * (number_engines + 1));
ANT_compressable_integer **raw = (ANT_compressable_integer **)malloc(sizeof(*raw) * (number_engines + 1));

/*
	Initialise the search engines, and related variables
*/
for (engine = 0; engine < number_engines; engine++)
	{
	search_engines[engine] = new ANT_search_engine(&memory[engine]);
	search_engines[engine]->open(argv[engine + first_param]);
	
	combined_docs += search_engines[engine]->document_count();
	maximum_terms += search_engines[engine]->term_count();
	longest_document = ANT_max((unsigned long)search_engines[engine]->get_longest_document_length(), longest_document);
	
	iterators[engine] = new ANT_btree_iterator(search_engines[engine]);
	leaves[engine] = new ANT_search_engine_btree_leaf;
	
	if (search_engines[engine]->quantized())
		exit(printf("Cannot merge quantized indexes (%s is quantized)\n", argv[engine + first_param]));
	
	if ((this_trimpoint = search_engines[engine]->get_variable("~trimpoint")))
		trimpoint = ANT_min(trimpoint, this_trimpoint);
	}
leaves[number_engines] = new ANT_search_engine_btree_leaf;
raw[number_engines] = (ANT_compressable_integer *)malloc(sizeof(*raw[number_engines]) * raw_list_size);

ANT_compression_factory factory;
ANT_compression_text_factory factory_text;
ANT_compressable_integer *lengths = new ANT_compressable_integer[combined_docs];

factory.set_scheme(param_block.compression_scheme);
factory_text.set_scheme(param_block.document_compression_scheme);

ANT_ranking_function_term_count rf(combined_docs, lengths);

ANT_memory_index *combined_term_index = new ANT_memory_index(NULL);
ANT_memory_index_hash_node *p;
ANT_memory_index_hash_node **term_list, **here;

term_list = (ANT_memory_index_hash_node **)malloc(sizeof(*term_list) * (maximum_terms + 1));

ANT_btree_head_node *header, *current_header, *last_header;

uint8_t zero = 0;
uint32_t four_byte;
uint64_t eight_byte;
long btree_root_size = 0, where = 0;
int32_t longest_term = 0;
int64_t highest_df = 0;
uint64_t terms_in_root;

char *doc_buf = NULL;
char **doc_filenames;
long long start, end, sum = 0;
unsigned long buf_len = 0;
long *strcmp_results = (long *)malloc(sizeof(*strcmp_results) * number_engines);
char *next_term_to_process = NULL;
long should_continue = TRUE;
long long do_documents = TRUE;
long long stemmer;

ANT_file *doclist = new ANT_file;
ANT_file *index = new ANT_file;

ANT_weighted_tf *tf_values = new ANT_weighted_tf[combined_docs];
postings_list = (unsigned char *)malloc(sizeof(*postings_list) * postings_list_size);

buffer_size = compress_buffer_size = longest_document;
document_decompress_buffer = (char *)malloc((size_t)longest_document);
document_compress_buffer = (char *)malloc((size_t)longest_document);

index->open("merged_index.aspt", "w");
index->write((unsigned char *)file_header, sizeof(file_header));

stemmer = search_engines[0]->get_variable("~stemmer");
for (engine = 1; engine < number_engines; engine++)
	if (search_engines[engine]->get_variable("~stemmer") != stemmer)
		exit(printf("Cannot merge indexes that have been stemmed differently (%s/%s at least)\n", argv[first_param], argv[first_param + engine]));

for (engine = 0; engine < number_engines; engine++)
	do_documents = do_documents && search_engines[engine]->get_postings_details("~documentoffsets", leaves[engine]);

if (do_documents)
	{
	for (engine = 0; engine < number_engines; engine++)
		for (i = 0; i < search_engines[engine]->document_count(); i++)
			{
			current_disk_position = index->tell();
			search_engines[engine]->get_document(document_decompress_buffer, &longest_document, i);
			
			document_compress_buffer = factory_text.compress(document_compress_buffer, &compress_buffer_size, document_decompress_buffer, longest_document);
			
			index->write((unsigned char *)document_compress_buffer, compress_buffer_size);
			
			longest_document = compress_buffer_size = buffer_size;
			sum += raw[number_engines][upto] = (ANT_compressable_integer)(current_disk_position - (upto == 0 ? 0 : sum));
			upto++;
			}
	raw[number_engines][upto] = (ANT_compressable_integer)(index->tell() - sum);
	
	/*
		Before we write out the "postings" for offsets, we should put the document filenames
		here and get the values for start/finish of those.
	*/
	document_filenames_start = index->tell();
	
	/*
		Copy over the filenames
	*/
	doclist->open("merged_doclist.aspt", "w");
	
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
		for (i = 0; i < search_engines[engine]->document_count(); i++)
			{
			doclist->puts(strip_space_inplace(doc_filenames[i]));
			index->write((unsigned char *)doc_filenames[i], strlen(doc_filenames[i]) + 1);
			}
		}
	doclist->close();
	
	document_filenames_finish = index->tell();
	
	/*
		Now we've done the documents, do the filename start/finish.
	*/
	p = write_variable("~documentfilenamesstart", document_filenames_start, postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);
	p = write_variable("~documentfilenamesfinish", document_filenames_finish, postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);
	
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
	p = write_postings("~documentoffsets", raw[number_engines], postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);
	
	/*
		Now we've done the documents and offsets, do the longest document.
	*/
	raw[number_engines][0] = longest_document;
	leaves[number_engines]->impacted_length = leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 1;
	p = write_postings("~documentlongest", raw[number_engines], postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);
	}

/*
	Now for the final ~ variable, length
	
	Because we are using get_decompressed_postings rather than get_document_lengths we don't need to worry
	about the +1/-1 difference between indexing/searching
*/
sum = 0;
leaves[number_engines]->impacted_length = leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 0;
for (engine = 0; engine < number_engines; engine++)
	{
	raw[engine] = search_engines[engine]->get_decompressed_postings("~length", leaves[engine]);
	memcpy(raw[number_engines] + leaves[number_engines]->local_document_frequency, raw[engine], sizeof(*raw[engine]) * leaves[engine]->local_document_frequency);
	for (i = 0; i < leaves[engine]->local_document_frequency; i++)
		sum += raw[engine][i];
	
	leaves[number_engines]->impacted_length += leaves[engine]->impacted_length;
	leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency;
	leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency;
	}

p = write_postings("~length", raw[number_engines], postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);

if (trimpoint != LONG_MAX)
	p = write_variable("~trimpoint", trimpoint, postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);

/*
	Now move onto the "normal" terms
*/
for (engine = 0; engine < number_engines; engine++)
	should_continue = should_continue && (terms[engine] = iterators[engine]->first(NULL));

while (should_continue)
	{
	next_term_to_process = terms[0];
	for (engine = 1; engine < number_engines; engine++)
		if (strcmp(terms[engine], next_term_to_process) < 0)
			next_term_to_process = terms[engine];
	
	/*
		Save repeated doing strcmp in this while loop
	*/
	for (engine = 0; engine < number_engines; engine++)
		strcmp_results[engine] = strcmp(next_term_to_process, terms[engine]);
	
	/*
		Now next_term_to_process contains the smallest, alphanumerically, string of the lists we're merging together
		
		Ignore all ~ terms, they've already been dealt with
	*/
	if (*next_term_to_process != '~')
		{
		/*
			Reset the tf_values accumulated so far
		*/
		memset(tf_values, 0, sizeof(*tf_values) * combined_docs);
		
		/*
			Get the postings from the respective indexes, and process them
		*/
		offset = 0;
		for (engine = 0; engine < number_engines; engine++)
			{
			if (strcmp_results[engine] == 0)
				{
				raw[engine] = search_engines[engine]->get_decompressed_postings(terms[engine], leaves[engine]);
				process(raw[engine], ANT_min(leaves[engine]->local_document_frequency, trimpoint), tf_values, offset);
				}
			offset += search_engines[engine]->document_count();
			}
		
		/*
			Convert the array of tf values to a postings list
		*/
		rf.tf_to_postings(leaves[number_engines], raw[number_engines], tf_values);
		
		/*
			We ignore the statistic results from tf_to_postings, because they were calculated
			using capped term frequencies, and we want the uncapped statistics
		*/
		leaves[number_engines]->local_collection_frequency = leaves[number_engines]->local_document_frequency = 0;
		for (engine = 0; engine < number_engines; engine++)
			if (strcmp_results[engine] == 0)
				{
				leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency;
				leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency;
				}
		
		/*
			Serialise the merged postings
		*/
		p = write_postings(next_term_to_process, raw[number_engines], postings_list, postings_list_size, index, combined_term_index, leaves[number_engines], &longest_postings);
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
	should_continue = TRUE;
	for (engine = 0; engine < number_engines; engine++)
		should_continue = should_continue && terms[engine];
	}

/*
	Now we've done all the postings, it's time to write the dictionary and header.
*/
for (i = 0; i < ANT_memory_index::HASH_TABLE_SIZE; i++)
	if (combined_term_index->hash_table[i] != NULL)
		where += combined_term_index->generate_term_list(combined_term_index->hash_table[i], term_list, where, &longest_term, &highest_df);
term_list[where] = NULL;

qsort(term_list, where, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

for (here = term_list; *here != NULL; here = combined_term_index->find_end_of_node(here))
	btree_root_size++;

/*
	Write the second level of the btree.
*/
current_header = header = (ANT_btree_head_node *)malloc(sizeof(ANT_btree_head_node) * btree_root_size);
here = term_list;
while (*here != NULL)
	{
	current_header->disk_pos = index->tell();
	current_header->node = *here;
	current_header++;
	here = combined_term_index->write_node(index, here);
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
four_byte = (uint32_t)longest_postings;
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

/*
	Cleanup
*/
for (engine = 0; engine < number_engines; engine++)
	{
	delete iterators[engine];
	delete leaves[engine];
	delete search_engines[engine];
	}
delete leaves[number_engines];
delete raw[number_engines];
delete combined_term_index;
delete doclist;
delete index;

delete [] memory;
delete [] lengths;
delete [] tf_values;

free(terms);
free(term_list);
free(postings_list);
free(document_decompress_buffer);
free(document_compress_buffer);

return 0;
}
