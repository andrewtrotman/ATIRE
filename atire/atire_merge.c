/*
	ATIRE_MERGE.C
	-------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "file.h"
#include "btree.h"
#include "btree_iterator.h"
#include "maths.h"
#include "memory.h"
#include "ranking_function_term_count.h"
#include "btree_head_node.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "memory_index_hash_node.h"
#include "memory_index.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define MULTI_MERGE TRUE

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
	WRITE_VARIABLE()
	----------------
*/
void write_variable(char *variable_name, long long value, ANT_file *index_file, ANT_memory_index *temp_index)
{
ANT_compression_factory factory;
ANT_memory_index_hash_node *p = temp_index->add_term(new ANT_string_pair(variable_name), 0);

long long current_disk_position = index_file->tell();
unsigned long long len;

ANT_compressable_integer raw[2];
unsigned char *postings_list = (unsigned char *)malloc(1024);// should be more than enough

raw[1] = ((unsigned long long)value) & 0xFFFFFFFF;
raw[0] = (((unsigned long long)value) >> 32) & 0xFFFFFFFF;
raw[1] = raw[1] - raw[0]; // diff encoded

len = factory.compress(postings_list, 1024, raw, 2);
index_file->write(postings_list, len);

p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = 2;
p->in_disk.end_pos_on_disk = index_file->tell();
p->collection_frequency = 2;
p->document_frequency = 2;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
#ifdef SPECIAL_COMPRESSION
exit(printf("SPECIAL_COMPRESSION not supported, yet\n"));
#endif

int i;
long long postings_list_size = 500 * 1024;
long long raw_list_size = 500 * 1024;
long long offset, engine, len, upto = 0;

uint64_t current_disk_position;
char file_header[] = "ANT Search Engine Index File\n\0\0";
unsigned char *postings_list;

long long combined_docs = 0, maximum_terms = 0;
long long number_engines = argc - 1;
long long longest_postings = 0;
long long document_filenames_start, document_filenames_finish;
unsigned long longest_doc = 0, buf_size, compress_buf_size;
char *document_decompress_buffer, *document_compress_buffer;

/*
	For now, we only support merging two indexes, although this will probably change.
*/
if (number_engines < 2)
	exit(printf("Usage: %s <index 1> <index 2> ... <index n>\n", argv[0]));

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
	search_engines[engine]->open(argv[engine + 1]);
	
	combined_docs += search_engines[engine]->document_count();
	maximum_terms += search_engines[engine]->term_count();
	longest_doc = ANT_max((unsigned long)search_engines[engine]->get_longest_document_length(), longest_doc);
	
	iterators[engine] = new ANT_btree_iterator(search_engines[engine]);
	leaves[engine] = new ANT_search_engine_btree_leaf;
	}
leaves[number_engines] = new ANT_search_engine_btree_leaf;
raw[number_engines] = (ANT_compressable_integer *)malloc(sizeof(*raw[number_engines]) * raw_list_size);

ANT_compression_factory factory;
ANT_compression_text_factory document_factory;
ANT_compressable_integer *lengths = (ANT_compressable_integer *)malloc(sizeof(*lengths) * combined_docs);
memset(lengths, 0, sizeof(*lengths) * combined_docs);

ANT_ranking_function_term_count rf(combined_docs, lengths);

ANT_memory_index *temp_index = new ANT_memory_index(NULL);
ANT_memory_index_hash_node *p;
ANT_memory_index_hash_node **term_list, **here;

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
long cont = TRUE;

ANT_file *doclist = new ANT_file;
ANT_file *merged_index = new ANT_file;

ANT_weighted_tf *tf_values = new ANT_weighted_tf[combined_docs];
postings_list = (unsigned char *)malloc(sizeof(*postings_list) * postings_list_size);

buf_size = compress_buf_size = longest_doc;
document_decompress_buffer = (char *)malloc((size_t)longest_doc);
document_compress_buffer = (char *)malloc((size_t)longest_doc);

merged_index->open("merged_index.aspt", "w");
merged_index->write((unsigned char *)file_header, sizeof(file_header));

/*
	We should probably actually check that both indexes have the documents in their indexes
	and if they don't then skip over them, or something.
	
	There's also probably a better way we could do this rather than decompress all the
	documents only to recompress them straight away.
*/
for (engine = 0; engine < number_engines; engine++)
	for (i = 0; i < search_engines[engine]->document_count(); i++)
		{
		current_disk_position = merged_index->tell();
		search_engines[engine]->get_document(document_decompress_buffer, &longest_doc, i);
		
		document_compress_buffer = document_factory.compress(document_compress_buffer, &compress_buf_size, document_decompress_buffer, longest_doc);
		
		merged_index->write((unsigned char *)document_compress_buffer, compress_buf_size);
		
		longest_doc = compress_buf_size = buf_size;
		sum += raw[number_engines][upto] = (ANT_compressable_integer)(current_disk_position - (upto == 0 ? 0 : sum));
		upto++;
		}
raw[number_engines][upto] = (ANT_compressable_integer)(merged_index->tell() - sum);

/*
	Before we write out the "postings" for offsets, we should put the document filenames
	here and get the values for start/finish of those.
*/
document_filenames_start = merged_index->tell();

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
		merged_index->write((unsigned char *)doc_filenames[i], strlen(doc_filenames[i]) + 1);
		}
	}
doclist->close();

document_filenames_finish = merged_index->tell();

/*
	Now we've done the documents, do the filename start/finish. These are "special" variables rather
	than posting lists of length 1, unlike ~documentlongest.  Don't ask me!
*/
write_variable("~documentfilenamesstart", document_filenames_start, merged_index, temp_index);
write_variable("~documentfilenamesfinish", document_filenames_finish, merged_index, temp_index);

p = temp_index->add_term(new ANT_string_pair("~documentoffsets"), 0);

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

current_disk_position = merged_index->tell();

len = factory.compress(postings_list, postings_list_size, raw[number_engines], leaves[number_engines]->impacted_length);
merged_index->write(postings_list, len);

longest_postings = ANT_max(longest_postings, len);

p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = leaves[number_engines]->impacted_length;
p->in_disk.end_pos_on_disk = merged_index->tell();

p->collection_frequency = leaves[number_engines]->local_collection_frequency;
p->document_frequency = leaves[number_engines]->local_document_frequency;

/*
	Now we've done the documents and offsets, do the longest document.
	Note, this is a postings list of length 1, rather than a "variable"
	like documentfilenamesstart/finish, don't ask me why, I'm from Barcelona.
*/
p = temp_index->add_term(new ANT_string_pair("~documentlongest"), 0);
raw[number_engines][0] = longest_doc;
leaves[number_engines]->impacted_length = leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 1;
current_disk_position = merged_index->tell();
len = factory.compress(postings_list, postings_list_size, raw[number_engines], leaves[number_engines]->impacted_length);
merged_index->write(postings_list, len);
longest_postings = ANT_max(longest_postings, len);
p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = leaves[number_engines]->impacted_length;
p->in_disk.end_pos_on_disk = merged_index->tell();
p->collection_frequency = leaves[number_engines]->local_collection_frequency;
p->document_frequency = leaves[number_engines]->local_document_frequency;

/*
	Now for the final ~ variable, length.
	
	Because we are using get_decompressed_postings rather than get_document_lengths
	we don't need to worry about the +1/-1 difference between indexing/searching.
*/
upto = 0;
sum = 0;
leaves[number_engines]->impacted_length = leaves[number_engines]->local_document_frequency = leaves[number_engines]->local_collection_frequency = 0;
for (engine = 0; engine < number_engines; engine++)
	{
	raw[engine] = search_engines[engine]->get_decompressed_postings("~length", leaves[engine]);
	memcpy(raw[number_engines] + upto, raw[engine], sizeof(*raw[engine]) * leaves[engine]->local_document_frequency);
	for (i = 0; i < leaves[engine]->local_document_frequency; i++)
		sum += raw[engine][i];
	
	leaves[number_engines]->impacted_length += leaves[engine]->impacted_length;
	leaves[number_engines]->local_collection_frequency += leaves[engine]->local_collection_frequency;
	leaves[number_engines]->local_document_frequency += leaves[engine]->local_document_frequency;
	
	upto += leaves[engine]->local_document_frequency;
	}

p = temp_index->add_term(new ANT_string_pair("~length"), 0);

len = factory.compress(postings_list, postings_list_size, raw[number_engines], leaves[number_engines]->impacted_length);
current_disk_position = merged_index->tell();
merged_index->write(postings_list, len);
longest_postings = ANT_max(longest_postings, len);

p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = leaves[number_engines]->impacted_length;
p->in_disk.end_pos_on_disk = merged_index->tell();
p->collection_frequency = leaves[number_engines]->local_collection_frequency;
p->document_frequency = leaves[number_engines]->local_document_frequency;

/*
	Now move onto the "normal" terms
*/
for (engine = 0; engine < number_engines; engine++)
	{
	terms[engine] = iterators[engine]->first(NULL);
	cont = cont && terms[engine];
	}

while (cont)
	{
	next_term_to_process = terms[0];
	for (engine = 1; engine < number_engines; engine++)
		if (strcmp(terms[engine], next_term_to_process) < 0)
			next_term_to_process = terms[engine];
	
	/*
		Now next_term_to_process contains the smallest, alphanumerically,
		string of the lists we're merging together.
		
		Ignore all ~ terms, they've already been dealt with. If the first
		term starts with a ~ then all must do.
	*/
	if (*next_term_to_process == '~')
		break;
	
	/*
		Add the term to our temporary index, and reset the tf_values accumulated so far.
	*/
	p = temp_index->add_term(new ANT_string_pair(next_term_to_process), 0);
	memset(tf_values, 0, sizeof(*tf_values) * combined_docs);
	
	/*
		Save repeated doing strcmp in this while loop
	*/
	for (engine = 0; engine < number_engines; engine++)
		strcmp_results[engine] = strcmp(terms[engine], next_term_to_process);
	
	/*
		Get the postings from the respective indexes, and process them.
	*/
	offset = 0;
	for (engine = 0; engine < number_engines; engine++)
		{
		if (strcmp_results[engine] == 0)
			{
			raw[engine] = search_engines[engine]->get_decompressed_postings(terms[engine], leaves[engine]);
			process(raw[engine], leaves[engine]->local_document_frequency, tf_values, offset);
			}
		offset += search_engines[engine]->document_count();
		}
	
	rf.tf_to_postings(leaves[number_engines], raw[number_engines], tf_values);
	
	/*
		We ignore the results from tf_to_postings, because those stats are calculated
		using capped term frequencies, and we want the uncapped statistics.
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
	len = factory.compress(postings_list, postings_list_size, raw[number_engines], leaves[number_engines]->impacted_length);
	current_disk_position = merged_index->tell();
	merged_index->write(postings_list, len);
	longest_postings = ANT_max(longest_postings, len);
	
	/*
		Now update the hash node disk values we created way back
	*/
	p->in_disk.docids_pos_on_disk = current_disk_position;
	p->in_disk.impacted_length = leaves[number_engines]->impacted_length;
	p->in_disk.end_pos_on_disk = merged_index->tell();
	
	/*
		Now update the stats for the term
	*/
	p->collection_frequency = leaves[number_engines]->local_collection_frequency;
	p->document_frequency = leaves[number_engines]->local_document_frequency;
	
	/*
		Move on to the next terms if necessary
	*/
	for (engine = 0; engine < number_engines; engine++)
		if (strcmp_results[engine] == 0)
			terms[engine] = iterators[engine]->next();
	
	/*
		Decide if we should keep going, this should never fail in practice, this while loop should exit
		above when we encounter ~ terms, but for completeness sake
	*/
	cont = TRUE;
	for (engine = 0; engine < number_engines; engine++)
		cont = cont && terms[engine];
	}

/*
	Now we've done all the postings, it's time to write the dictionary and header.
*/
term_list = (ANT_memory_index_hash_node **)malloc(sizeof(*term_list) * (maximum_terms + 1));

for (i = 0; i < ANT_memory_index::HASH_TABLE_SIZE; i++)
	if (temp_index->hash_table[i] != NULL)
		where += temp_index->generate_term_list(temp_index->hash_table[i], term_list, where, &longest_term, &highest_df);
term_list[where] = NULL;

qsort(term_list, where, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

for (here = term_list; *here != NULL; here = temp_index->find_end_of_node(here))
	btree_root_size++;

/*
	Write the second level of the btree.
*/
current_header = header = (ANT_btree_head_node *)malloc(sizeof(ANT_btree_head_node) * btree_root_size);
here = term_list;
while (*here != NULL)
	{
	current_header->disk_pos = merged_index->tell();
	current_header->node = *here;
	current_header++;
	here = temp_index->write_node(merged_index, here);
	}
last_header = current_header;
terms_in_root = last_header - header;

current_disk_position = merged_index->tell();

merged_index->write((unsigned char *)&terms_in_root, sizeof(terms_in_root));

/*
	Write the first level of the btree.
*/
for (current_header = header; current_header < last_header; current_header++)
	{
	merged_index->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	merged_index->write(&zero, sizeof(zero));
	eight_byte = current_header->disk_pos;
	merged_index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
	}

/*
	Write the "header" of the index file
*/
merged_index->write((unsigned char *)&current_disk_position, sizeof(current_disk_position));

merged_index->write((unsigned char *)&longest_term, sizeof(longest_term));
four_byte = (uint32_t)longest_postings;
merged_index->write((unsigned char *)&four_byte, sizeof(four_byte));

merged_index->write((unsigned char *)&highest_df, sizeof(highest_df));
eight_byte = 0;
merged_index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
eight_byte = ANT_file_signature_index;
merged_index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
four_byte = (uint32_t)ANT_version;
merged_index->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)ANT_file_signature;
merged_index->write((unsigned char *)&four_byte, sizeof(four_byte));

merged_index->close();

return 0;
}
