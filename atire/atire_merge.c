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

#define MERGE_VERBOSE FALSE

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
		if (tf_values != NULL)
			tf_values[docid + offset] = (ANT_weighted_tf)tf;
		#if MERGE_VERBOSE
		printf("<%lld,%lld> ", docid, (long long)tf);
		#endif
		}
	current++;
	}
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
long long maximum_terms = search_engines[0].term_count() + search_engines[1].term_count();

ANT_btree_iterator iterators[2] = {ANT_btree_iterator(&search_engines[0]), ANT_btree_iterator(&search_engines[1])};
ANT_search_engine_btree_leaf leaves[3];
ANT_compression_factory factory;
ANT_compressable_integer *lengths = (ANT_compressable_integer *)malloc(sizeof(*lengths) * (combined_docs));
memset(lengths, 0, sizeof(*lengths) * (combined_docs));

ANT_ranking_function_term_count rf(combined_docs, lengths);

ANT_memory_index *temp_index = new ANT_memory_index(NULL);
ANT_memory_index_hash_node *p;

ANT_file *merged_index = new ANT_file;
merged_index->open(argv[3], "w");
merged_index->write((unsigned char *)file_header, sizeof(file_header));

/*
	Allocate arrays for the tf values, postings lists, etc.
	+2 for +1 on docids for each index
*/
ANT_weighted_tf *tf_values = new ANT_weighted_tf[combined_docs];

postings_list[0] = (unsigned char *)malloc(sizeof(*postings_list[0]) * postings_list_size);
postings_list[1] = (unsigned char *)malloc(sizeof(*postings_list[1]) * postings_list_size);
postings_list[2] = (unsigned char *)malloc(sizeof(*postings_list[2]) * postings_list_size);

long long longest_postings = 0;

raw[0] = (ANT_compressable_integer *)malloc(sizeof(*raw[0]) * raw_list_size);
raw[1] = (ANT_compressable_integer *)malloc(sizeof(*raw[1]) * raw_list_size);
raw[2] = (ANT_compressable_integer *)malloc(sizeof(*raw[2]) * raw_list_size);

terms[0] = iterators[0].first(NULL);
terms[1] = iterators[1].first(NULL);

while (terms[0] && terms[1])
	{
	string_compare_result = strcmp(terms[0], terms[1]);
	
	#if MERGE_VERBOSE
	if (string_compare_result <= 0)
		{
		printf("%s occurs in first!", terms[0]);
		if (string_compare_result != 0) putchar('\n');
		}
	if (string_compare_result >= 0)
		{
		if (string_compare_result == 0) putchar(' ');
		printf("%s occurs in second!\n", terms[1]);
		}
	#endif

	iterators[0].get_postings_details(&leaves[0]);
	iterators[1].get_postings_details(&leaves[1]);
	
	/*
		Get the postings for the first index
	*/
	if (string_compare_result <= 0)
		{
		if (*terms[0] != '~')
			{
			if (leaves[0].local_document_frequency > 2)
				if (leaves[0].postings_length > postings_list_size)
					{
					fprintf(stderr, "Enlarge pls before extract 1\n");
					postings_list_size = 2 * leaves[0].postings_length;
					postings_list[0] = (unsigned char *)realloc(postings_list[0], (size_t)postings_list_size);
					postings_list[2] = (unsigned char *)realloc(postings_list[2], (size_t)postings_list_size);
					}
			postings_list[0] = search_engines[0].get_postings(&leaves[0], postings_list[0]);
			if (leaves[0].impacted_length > raw_list_size)
				{
				fprintf(stderr, "Enlarge raw before extract 1\n");
				raw_list_size = 2 * leaves[0].impacted_length;
				raw[0] = (ANT_compressable_integer *)realloc(raw[0], (size_t)raw_list_size);
				raw[2] = (ANT_compressable_integer *)realloc(raw[2], (size_t)raw_list_size);
				}
			factory.decompress(raw[0], postings_list[0], leaves[0].impacted_length);
			}
		else
			{
			postings_list[0] = search_engines[0].get_postings(&leaves[0], postings_list[0]);
			factory.decompress(raw[0], postings_list[0], leaves[0].impacted_length);
			}
		}
	
	/*
		Get the postings for the second index
	*/
	if (string_compare_result >= 0)
		{
		if (*terms[1] != '~')
			{
			if (leaves[1].local_document_frequency > 2)
				if (leaves[1].postings_length > postings_list_size)
					{
					fprintf(stderr, "Enlarge pls before extract 2\n");
					postings_list_size = 2 * leaves[1].postings_length;
					postings_list[1] = (unsigned char *)realloc(postings_list[1], (size_t)postings_list_size);
					postings_list[2] = (unsigned char *)realloc(postings_list[2], (size_t)postings_list_size);
					}
			postings_list[1] = search_engines[1].get_postings(&leaves[1], postings_list[1]);
			if (leaves[1].impacted_length > raw_list_size)
				{
				fprintf(stderr, "Enlarge raw before extract 2\n");
				raw_list_size = 2 * leaves[1].impacted_length;
				raw[1] = (ANT_compressable_integer *)realloc(raw[1], (size_t)raw_list_size);
				raw[2] = (ANT_compressable_integer *)realloc(raw[2], (size_t)raw_list_size);
				}
			factory.decompress(raw[1], postings_list[1], leaves[1].impacted_length);
			}
		else
			{
			postings_list[1] = search_engines[1].get_postings(&leaves[1], postings_list[1]);
			factory.decompress(raw[1], postings_list[1], leaves[1].impacted_length);
			}
		}
	
	if (string_compare_result <= 0)
		{
		p = temp_index->add_term(new ANT_string_pair(terms[0]), 0);
		#if MERGE_VERBOSE
		printf("Added '%s' to dictionary (%p)\n", terms[0], p);
		#endif
		}
	if (string_compare_result > 0)
		{
		p = temp_index->add_term(new ANT_string_pair(terms[1]), 0);
		#if MERGE_VERBOSE
		printf("Added '%s' to dictionary (%p)\n", terms[1], p);
		#endif
		}
	
	/*
		Reset the tf values accumulated so far, and fill in from
		the two postings lists we received above.
		
		The process function also prints the postings for manual
		inspection and comparison while coding the merge.
	*/
	memset(tf_values, 0, sizeof(*tf_values) * combined_docs);
	double dummy;
	
	#if MERGE_VERBOSE
	printf("Pre-merge:\n");
	printf("Index 1: ");
	#endif
	
	if (string_compare_result <= 0)
		{
		if (*terms[0] != '~')
			process(raw[0], leaves[0].local_document_frequency, tf_values, 0);
		else 
			raw[0] = search_engines[0].get_document_lengths(&dummy);
		}
	
	#if MERGE_VERBOSE
	printf("\nIndex 2: ");
	#endif
	
	if (string_compare_result >= 0)
		{
		if (*terms[1] != '~')
			process(raw[1], leaves[1].local_document_frequency, tf_values, search_engines[0].document_count());
		else
			raw[1] = search_engines[1].get_document_lengths(&dummy);
		}
	
	if ((string_compare_result <= 0 && *terms[0] != '~') || (string_compare_result >= 0 && *terms[1] != '~'))
			rf.tf_to_postings(&leaves[2], raw[2], tf_values);
	else
		{
		/*
			Deal with document lengths, +1 because of the difference in
			representation between the indexer and the search engine,
			what we get is search engine, what we want is index.
		*/
		int i;
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
	
	#if MERGE_VERBOSE
	/*
		Print the merged postings for confirmation.
	*/
	if (*terms[0] != '~')
		{
		printf("\n");
		printf("Post-merge: ");
		process(raw[2], leaves[2].local_document_frequency, NULL, 0);
		}
	printf("\n");
	#endif
	
	/*
		Serialise the merged postings
	*/
	{
//		long long i;
//		for (i = 0; i < leaves[2].impacted_length; i++)
//			printf("%lld: %ld\n", i, (long)raw[2][i]);
	}
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
	
	#if MERGE_VERBOSE
	printf("%s: cf(%llu) df(%llu)\n", p->string.str(), p->collection_frequency, p->document_frequency);
	printf("%s: %llu to %llu ~= %llu (%llu)\n", p->string.str(), current_disk_position, p->in_disk.end_pos_on_disk, len, leaves[2].impacted_length);
	
	printf("\n");
	#endif
	
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
long i, where = 0;
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
