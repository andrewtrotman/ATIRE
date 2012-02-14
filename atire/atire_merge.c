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
unsigned char *postings_list = (unsigned char *)malloc(1024);// should be enough

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
ANT_compressable_integer *raw[3];
int string_compare_result;
long long postings_list_size = 500 * 1024;
long long raw_list_size = 500 * 1024;
long long len, upto;
uint64_t current_disk_position;
char file_header[] = "ANT Search Engine Index File\n\0\0";
unsigned char *postings_list[3] = {NULL};
char *terms[2];
ANT_memory memory[2];
ANT_search_engine search_engines[2] = {ANT_search_engine(&memory[0]), ANT_search_engine(&memory[1])};
search_engines[0].open(argv[1]);
search_engines[1].open(argv[2]);

long long combined_docs = search_engines[0].document_count() + search_engines[1].document_count();
long long maximum_terms = search_engines[0].term_count() + search_engines[1].term_count(); // the maximum possible unique terms

ANT_btree_iterator iterators[2] = {ANT_btree_iterator(&search_engines[0]), ANT_btree_iterator(&search_engines[1])};
ANT_search_engine_btree_leaf leaves[3];
ANT_compression_factory factory;
ANT_compression_text_factory document_factory;
ANT_compressable_integer *lengths = (ANT_compressable_integer *)malloc(sizeof(*lengths) * (combined_docs));
memset(lengths, 0, sizeof(*lengths) * (combined_docs));

ANT_ranking_function_term_count rf(combined_docs, lengths);

ANT_memory_index *temp_index = new ANT_memory_index(NULL);
ANT_memory_index_hash_node *p;
long long sum = 0;

ANT_file *merged_index = new ANT_file;
merged_index->open(argv[3], "w");
merged_index->write((unsigned char *)file_header, sizeof(file_header));

/*
	Allocate arrays for the tf values, postings lists, etc.
*/
ANT_weighted_tf *tf_values = new ANT_weighted_tf[combined_docs];

postings_list[2] = (unsigned char *)malloc(sizeof(*postings_list[2]) * postings_list_size);

long long longest_postings = 0;

raw[2] = (ANT_compressable_integer *)malloc(sizeof(*raw[2]) * raw_list_size);

/*
	Before we do the postings, we do the files and filenames
*/
unsigned long longest_doc = ANT_max(search_engines[0].get_longest_document_length(), search_engines[1].get_longest_document_length());
unsigned long buf_size = longest_doc;
unsigned long compress_buf_size = longest_doc;
char *document_decompress_buffer = (char *)malloc((size_t)longest_doc);
char *document_compress_buffer = (char *)malloc((size_t)longest_doc);

/*
	There has to be a better way to make sure that both indexes contain the
	squiggle value before proceeding than this, however, for now we just 
	print out the message and let the search fail later on.
*/
ANT_search_engine_btree_leaf *l1, *l2;
l1 = search_engines[0].get_postings_details("~documentoffsets", &leaves[0]);
l2 = search_engines[1].get_postings_details("~documentoffsets", &leaves[1]);

if ((l1 && !l2) || (l2 && !l1))
	printf("~documentoffsets not in both indexes, skipping\n");

upto = 0;
for (i = 0; i < search_engines[0].document_count(); i++)
	{
	current_disk_position = merged_index->tell();
	search_engines[0].get_document(document_decompress_buffer, &longest_doc, i);
	
	document_compress_buffer = document_factory.compress(document_compress_buffer, &compress_buf_size, document_decompress_buffer, longest_doc);
	
	merged_index->write((unsigned char *)document_compress_buffer, compress_buf_size);
	
	longest_doc = compress_buf_size = buf_size;
	sum += raw[2][upto] = current_disk_position - (upto == 0 ? 0 : sum);
	upto++;
	}

for (i = 0; i < search_engines[1].document_count(); i++)
	{
	current_disk_position = merged_index->tell();
	search_engines[1].get_document(document_decompress_buffer, &longest_doc, i);
	
	document_compress_buffer = document_factory.compress(document_compress_buffer, &compress_buf_size, document_decompress_buffer, longest_doc);
	
	merged_index->write((unsigned char *)document_compress_buffer, compress_buf_size);
	
	longest_doc = compress_buf_size = buf_size;
	sum += raw[2][upto] = current_disk_position - (upto == 0 ? 0 : sum);
	upto++;
	}
raw[2][upto] = merged_index->tell() - sum;

/*
	Update stats, -1 because both original lists are diff encoded, so we don't need 1 of the values
*/
leaves[2].impacted_length = leaves[0].impacted_length + leaves[1].impacted_length - 1;
leaves[2].local_document_frequency = leaves[0].local_document_frequency + leaves[1].local_document_frequency - 1;
leaves[2].local_collection_frequency = leaves[0].local_collection_frequency + leaves[1].local_collection_frequency - 1;

/*
	Before we write out the "postings" for offsets, we should
	put the document filenames here and get the values for
	start/finish of those.
*/
long long document_filenames_start = merged_index->tell();

char *doc_buf = NULL;
char **doc_filenames;
long long start, end;
unsigned long buf_len = 0;

/*
	Do the same thing for both indexes, so loop over them :P
*/
for (i = 0; i < 2; i++)
	{
	start = search_engines[i].get_variable("~documentfilenamesstart");
	end = search_engines[i].get_variable("~documentfilenamesfinish");
	
	doc_buf = (char *)realloc(doc_buf, end - start);
	doc_filenames = search_engines[i].get_document_filenames(doc_buf, &buf_len);
	
	/*
		Technically, here we could jsut write out doc_buf here,
		but the underlying representation _might_ change.
	*/
	for (i = 0; i < search_engines[i].document_count(); i++)
		merged_index->write((unsigned char *)doc_filenames[i], strlen(doc_filenames[i]) + 1);
	}

long long document_filenames_finish = merged_index->tell();

p = temp_index->add_term(new ANT_string_pair("~documentoffsets"), 0);

/*
	Serialise the merged postings
*/
current_disk_position = merged_index->tell();

len = factory.compress(postings_list[2], postings_list_size, raw[2], leaves[2].impacted_length);
merged_index->write(postings_list[2], len);

longest_postings = ANT_max(longest_postings, len);

/*
	Now update the hash node disk values we created way back
*/
p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = leaves[2].impacted_length;
p->in_disk.end_pos_on_disk = merged_index->tell();

/*
	Now update the stats for the term
*/
p->collection_frequency = leaves[2].local_collection_frequency;
p->document_frequency = leaves[2].local_document_frequency;

/*
	Now we've done the documents and offsets, do the longest document.
	Note, this is a postings list of length 1, rather than a "variable"
	like documentfilenamesstart/finish, don't ask me why, I'm from Barcelona.
*/
p = temp_index->add_term(new ANT_string_pair("~documentlongest"), 0);
raw[2][0] = longest_doc;
leaves[2].impacted_length = leaves[2].local_document_frequency = leaves[2].local_collection_frequency = 1;
current_disk_position = merged_index->tell();
len = factory.compress(postings_list[2], postings_list_size, raw[2], leaves[2].impacted_length);
merged_index->write(postings_list[2], len);
longest_postings = ANT_max(longest_postings, len);
p->in_disk.docids_pos_on_disk = current_disk_position;
p->in_disk.impacted_length = leaves[2].impacted_length;
p->in_disk.end_pos_on_disk = merged_index->tell();
p->collection_frequency = leaves[2].local_collection_frequency;
p->document_frequency = leaves[2].local_document_frequency;


/*
	Now we've done the documents, do the filename start/finish
	These are "special" variables rather than posting lists of length 1.
	Don't ask me!
*/
write_variable("~documentfilenamesstart", document_filenames_start, merged_index, temp_index);
write_variable("~documentfilenamesfinish", document_filenames_finish, merged_index, temp_index);


/*
	Now move onto the "normal" terms
*/
terms[0] = iterators[0].first(NULL);
terms[1] = iterators[1].first(NULL);

while (terms[0] && terms[1])
	{
	string_compare_result = strcmp(terms[0], terms[1]);
	
	/*
		Here we ignore all ~ terms, except for length, which is not difference encoded,
		and is like a normal postings list, unliked other ~ terms 
		
		And yes, I have presumably wrought the wrath of Andrew for using goto.
	*/
	if (string_compare_result == 0 && *terms[0] == '~' && strcmp(terms[0], "~length") != 0)
		goto next;
	
	/*
		Get the postings from the respective indexes
	*/
	if (string_compare_result <= 0)
		raw[0] = search_engines[0].get_decompressed_postings(terms[0], &leaves[0]);
	if (string_compare_result >= 0)
		raw[1] = search_engines[1].get_decompressed_postings(terms[1], &leaves[1]);
	
	if (string_compare_result <= 0)
		p = temp_index->add_term(new ANT_string_pair(terms[0]), 0);
	else
		p = temp_index->add_term(new ANT_string_pair(terms[1]), 0);
	
	/*
		Reset the tf values accumulated so far, and fill in from
		the two postings lists we received above.
	*/
	memset(tf_values, 0, sizeof(*tf_values) * combined_docs);
	
	if (string_compare_result <= 0)
		if (*terms[0] != '~')
			process(raw[0], leaves[0].local_document_frequency, tf_values, 0);
	
	if (string_compare_result >= 0)
		if (*terms[1] != '~')
			process(raw[1], leaves[1].local_document_frequency, tf_values, search_engines[0].document_count());
	
	if ((string_compare_result <= 0 && *terms[0] != '~') || (string_compare_result >= 0 && *terms[1] != '~'))
			rf.tf_to_postings(&leaves[2], raw[2], tf_values);
	else if (string_compare_result == 0 && strcmp(terms[0], "~length") == 0)
		{
		/*
			Deal with document lengths, +1 because of the difference in representation between the
			indexer and the search engine, what we get is search engine, what we want is index.
		*/
		upto = 0;
		
		for (i = 0; i < leaves[0].local_document_frequency; i++, upto++)
			raw[2][upto] = raw[0][i] + 1;
		for (i = 0; i < leaves[1].local_document_frequency; i++, upto++)
			raw[2][upto] = raw[1][i] + 1;
		
		leaves[2].impacted_length = leaves[0].impacted_length + leaves[1].impacted_length;
		}
	
	/*
		We ignore the results from tf_to_postings, because those stats are calculated
		using capped term frequencies, and we want the uncapped statistics.
	*/
	leaves[2].local_collection_frequency = leaves[2].local_document_frequency = 0;
	if (string_compare_result <= 0)
		{
		leaves[2].local_collection_frequency += leaves[0].local_collection_frequency;
		leaves[2].local_document_frequency += leaves[0].local_document_frequency;
		}
	if (string_compare_result >= 0)
		{
		leaves[2].local_collection_frequency += leaves[1].local_collection_frequency;
		leaves[2].local_document_frequency += leaves[1].local_document_frequency;
		}
	
	/*
		Serialise the merged postings
	*/
	len = factory.compress(postings_list[2], postings_list_size, raw[2], leaves[2].impacted_length);
	current_disk_position = merged_index->tell();
	merged_index->write(postings_list[2], len);
	longest_postings = ANT_max(longest_postings, len);
	
	/*
		Now update the hash node disk values we created way back
	*/
	p->in_disk.docids_pos_on_disk = current_disk_position;
	p->in_disk.impacted_length = leaves[2].impacted_length;
	p->in_disk.end_pos_on_disk = merged_index->tell();
	
	/*
		Now update the stats for the term
	*/
	p->collection_frequency = leaves[2].local_collection_frequency;
	p->document_frequency = leaves[2].local_document_frequency;
	
next:
	/*
		Move on to the next terms
	*/
	if (string_compare_result <= 0)
		terms[0] = iterators[0].next();
	if (string_compare_result >= 0)
		terms[1] = iterators[1].next();
	}

ANT_memory_index_hash_node **term_list, **here;
size_t bytes = sizeof(*term_list) * (maximum_terms + 1);
term_list = (ANT_memory_index_hash_node **)malloc(bytes);
long where = 0;
int32_t longest_term = 0;
int64_t highest_df = 0;
long btree_root_size = 0;
ANT_btree_head_node *header, *current_header, *last_header;
uint64_t terms_in_root;

for (i = 0; i < ANT_memory_index::HASH_TABLE_SIZE; i++)
	if (temp_index->hash_table[i] != NULL)
		where += temp_index->generate_term_list(temp_index->hash_table[i], term_list, where, &longest_term, &highest_df);
term_list[where] = NULL;

qsort(term_list, where, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

for (here = term_list; *here != NULL; here = temp_index->find_end_of_node(here))
	btree_root_size++;

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

uint8_t zero = 0;
uint32_t four_byte;
uint64_t eight_byte;

for (current_header = header; current_header < last_header; current_header++)
	{
	merged_index->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	merged_index->write(&zero, sizeof(zero));
	eight_byte = current_header->disk_pos;
	merged_index->write((unsigned char *)&eight_byte, sizeof(eight_byte));
	}

merged_index->write((unsigned char *)&current_disk_position, sizeof(current_disk_position));

merged_index->write((unsigned char *)&longest_term, sizeof(longest_term));
four_byte = longest_postings;
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
