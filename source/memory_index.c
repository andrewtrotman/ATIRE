/*
	MEMORY_INDEX.C
	--------------
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "memory_index_hash_node.h"
#include "memory_index.h"
#include "memory_index_stats.h"
#include "memory.h"
#include "string_pair.h"
#include "file.h"
#include "btree.h"
#include "btree_head_node.h"
#include "hash_table.h"
#include "fundamental_types.h"
#include "compression_factory.h"

#ifdef QUANTIZED_ORDERING
	#include "maths.h"
#endif

#define DISK_BUFFER_SIZE (10 * 1024 * 1024)

/*
	ANT_MEMORY_INDEX::ANT_MEMORY_INDEX()
	------------------------------------
*/
ANT_memory_index::ANT_memory_index()
{
squiggle_length = new ANT_string_pair("~length");
hashed_squiggle_length = hash(squiggle_length);
memset(hash_table, 0, sizeof(hash_table));
memory = new ANT_memory;
stats = new ANT_memory_index_stats(memory);
serialised_docids_size = 1;
serialised_docids = (unsigned char *)memory->malloc(serialised_docids_size);
serialised_tfs_size = 1;
serialised_tfs = (unsigned char *)memory->malloc(serialised_tfs_size);
largest_docno = 0;
factory = new ANT_compression_factory;
#ifdef QUANTIZED_ORDERING
	document_lengths = NULL;
#endif
}

/*
	ANT_MEMORY_INDEX::~ANT_MEMORY_INDEX()
	-------------------------------------
*/
ANT_memory_index::~ANT_memory_index()
{
delete memory;
delete stats;
delete factory;
delete [] squiggle_length;
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(long which_stats)
{
if (which_stats & STAT_SUMMARY)
	stats->text_render(ANT_memory_index_stats::STAT_SUMMARY);
if (which_stats & STAT_MEMORY)
	stats->text_render(ANT_memory_index_stats::STAT_MEMORY);
if (which_stats & STAT_COMPRESSION)
	factory->text_render();
}

/*
	ANT_MEMORY_INDEX::GET_MEMORY_USAGE()
	------------------------------------
*/
long long ANT_memory_index::get_memory_usage(void)
{
return memory->bytes_used();
}

/*
	ANT_MEMORY_INDEX::HASH()
	------------------------
*/
inline long ANT_memory_index::hash(ANT_string_pair *string)
{
return ANT_hash_24(string);
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
			return root->left = new_memory_index_hash_node(string);
		else
			root = root->left;
	else
		if (root->right == NULL)
			return root->right = new_memory_index_hash_node(string);
		else
			root = root->right;
	}
return root;
}

/*
	ANT_MEMORY_INDEX::ADD_TERM()
	----------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::add_term(ANT_string_pair *string, long long docno)
{
long hash_value;
ANT_memory_index_hash_node *node;

stats->documents = docno;

hash_value = hash(string);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	node = hash_table[hash_value] = new_memory_index_hash_node(string);
	}
else
	node = find_add_node(hash_table[hash_value], string);
node->add_posting(docno);
return node;
}

/*
	ANT_MEMORY_INDEX::SET_DOCUMENT_DETAIL()
	---------------------------------------
*/
void ANT_memory_index::set_document_detail(ANT_string_pair *measure_name, long score)
{
long hash_value;
ANT_memory_index_hash_node *node;

hash_value = hash(measure_name);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	node = hash_table[hash_value] = new_memory_index_hash_node(measure_name);
	}
else
	node = find_add_node(hash_table[hash_value], measure_name);
node->current_docno = 0;
node->add_posting(score);
}

/*
	ANT_MEMORY_INDEX::IMPACT_ORDER()
	--------------------------------
*/
long long ANT_memory_index::impact_order(ANT_compressable_integer *destination, ANT_compressable_integer *docid, unsigned char *term_frequency, long long document_frequency)
{
ANT_compressable_integer sum, bucket_size[0x100], bucket_prev_docid[0x100];
ANT_compressable_integer *pointer[0x100], *current_docid, doc;
unsigned char *current, *end;
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
buckets_used = sum = 0;
for (bucket = 0xFF; bucket >= 0; bucket--)
	{
	pointer[bucket] = destination + sum + 2 * buckets_used;
	sum += bucket_size[bucket];
	if (bucket_size[bucket] != 0)
		{
		*pointer[bucket]++ = bucket;
		buckets_used++;
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
for (bucket = 0; bucket < 0x100; bucket++)
	if (bucket_size[bucket] != 0)
		*pointer[bucket] = 0;

/*
	Return the length of the impact ordered list
*/
return sum + 2 * buckets_used;
}

#ifdef QUANTIZED_ORDERING
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
			serialised_docids = (unsigned char *)memory->malloc(serialised_docids_size);
			}
		if (*tf_size > serialised_tfs_size)
			{
			serialised_tfs_size = *tf_size;
			serialised_tfs = (unsigned char *)memory->malloc(serialised_tfs_size);
			}
		}
	return total;
	}

	/*
		ANT_MEMORY_INDEX::RSV_ALL_NODES()
		---------------------------------
	*/
	double ANT_memory_index::rsv_all_nodes(ANT_memory_index_hash_node *root)
	{
	double right, left, my_rsv;
	long long doc_size, tf_size;
	unsigned char *current, *end;

	/*
		What is the max from the children of this node?
	*/
	left = right = my_rsv = 0;
	if (root->right != NULL)
		right = rsv_all_nodes(root->right);
	if (root->left != NULL)
		left += rsv_all_nodes(root->left);

	/*
		Now we compute the score for the current node
		First get the postings lists (docids and tf scores)
	*/
	get_serialised_postings(root, &doc_size, &tf_size);

	/*
		Now we decompress the docids
	*/
	if (root->string[0] == '~')			// these are "special" strings in the index (e.g. document lengths)
		my_rsv = 0;
	else
		{
		variable_byte.decompress(impacted_postings, serialised_docids, root->document_frequency);
		end = serialised_tfs + root->document_frequency;
		for (current = serialised_tfs; current < end; current++)
			my_rsv = max(my_rsv, (double)*current);
		}
	/*
		now return the max of the three
	*/
	return max(left, right, my_rsv);
	}
#endif

/*
	ANT_MEMORY_INDEX::SERIALISE_ALL_NODES()
	---------------------------------------
*/
long ANT_memory_index::serialise_all_nodes(ANT_file *file, ANT_memory_index_hash_node *root)
{
long terms = 1;
long long doc_size, tf_size, total, len, impacted_postings_length;

stats->term_occurences += root->collection_frequency;

if (root->right != NULL)
	terms += serialise_all_nodes(file, root->right);

//printf("\t%s (df:%lld cf:%lld)\n", root->string.str(), root->document_frequency, root->collection_frequency);
doc_size = serialised_docids_size;
tf_size = serialised_tfs_size;
while ((total = root->serialise_postings(serialised_docids, &doc_size, serialised_tfs, &tf_size)) == 0)
	{
	if (doc_size > serialised_docids_size)
		{
		serialised_docids_size = doc_size;
		serialised_docids = (unsigned char *)memory->malloc(serialised_docids_size);
		}
	if (tf_size > serialised_tfs_size)
		{
		serialised_tfs_size = tf_size;
		serialised_tfs = (unsigned char *)memory->malloc(serialised_tfs_size);
		}
	}

stats->bytes_to_store_docids += doc_size;
stats->bytes_to_store_tfs += tf_size;

if (root->string[0] == '~')			// these are "special" strings in the index (e.g. document lengths)
	{
	variable_byte.decompress(impacted_postings, serialised_docids, root->document_frequency);
	impacted_postings_length = root->document_frequency;
	}
else
	{
	variable_byte.decompress(decompressed_postings_list, serialised_docids, root->document_frequency);
#ifdef SPECIAL_COMPRESSION
	if (root->document_frequency > 2)
		impacted_postings_length = impact_order(impacted_postings, decompressed_postings_list, serialised_tfs, root->document_frequency);
	else
		impacted_postings_length = 0;
#else
	impacted_postings_length = impact_order(impacted_postings, decompressed_postings_list, serialised_tfs, root->document_frequency);
#endif
	}

#ifdef SPECIAL_COMPRESSION
	if (root->document_frequency <= 2)
		{
		root->docids_pos_on_disk = ((long long)decompressed_postings_list[0]) << 32 | serialised_tfs[0];
		if (root->document_frequency == 2)
			{
			root->impacted_length = decompressed_postings_list[1] + decompressed_postings_list[0];		// because the original list is difference encoded
			root->end_pos_on_disk = serialised_tfs[1] + root->docids_pos_on_disk;		// because root->docids_pos_on_disk is subtracted later
			}
		else
			root->impacted_length = root->end_pos_on_disk = 0;
		}
	else
		{
		len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

		root->docids_pos_on_disk = file->tell();
		file->write(compressed_postings_list, len);

		root->impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
		root->end_pos_on_disk = file->tell();
		}
#else
	len = factory->compress(compressed_postings_list, compressed_postings_list_length, impacted_postings, impacted_postings_length);

	root->docids_pos_on_disk = file->tell();
	file->write(compressed_postings_list, len);

	root->impacted_length = impacted_postings_length;		// length of the impacted list measured in integers (for decompression purposes)
	root->end_pos_on_disk = file->tell();
#endif

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
	Recurse left
*/
if (root->left != NULL)
	terms += generate_term_list(root->left, into, where + terms + 1, length_of_longest_term, highest_df);

return terms + 1;
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
unsigned char zero = 0;
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
string_pos = (uint32_t)(end - start) * (1 * 8 + 5 * 4) + 4;
for (current = start; current < end; current++)
	{
	four_byte = (uint32_t)(*current)->collection_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)(*current)->document_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	eight_byte = (uint64_t)((*current)->docids_pos_on_disk);
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

	four_byte = (uint32_t)((*current)->impacted_length);
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)((*current)->end_pos_on_disk - (*current)->docids_pos_on_disk);
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
	ANT_MEMORY_INDEX::SERIALISE()
	-----------------------------
*/
long ANT_memory_index::serialise(char *filename)
{
uint8_t zero = 0;
int32_t length_of_longest_term = 0;
uint32_t longest_postings_size;
int64_t highest_df = 0;
uint64_t file_position, terms_in_root, eight_byte;
long terms_in_node, unique_terms = 0, max_terms_in_node = 0, hash_val, where, bytes, btree_root_size;
ANT_file *file;
ANT_memory_index_hash_node **term_list, **here;
ANT_btree_head_node *header, *current_header, *last_header;

file = new ANT_file(memory);
file->open(filename, "w+b");
file->setvbuff(DISK_BUFFER_SIZE);
stats->disk_buffer = DISK_BUFFER_SIZE;

compressed_postings_list_length = 1 + (sizeof(*decompressed_postings_list) * largest_docno);
decompressed_postings_list = (ANT_compressable_integer *)memory->malloc(compressed_postings_list_length - 1);
compressed_postings_list = (unsigned char *)memory->malloc(compressed_postings_list_length);
impacted_postings = (ANT_compressable_integer *)memory->malloc(compressed_postings_list_length + 512);		// 512 because the TF and the 0 at the end of each of 255 lists
stats->bytes_for_decompression_recompression += compressed_postings_list_length * 3 + 512 - 1;

#ifdef QUANTIZED_ORDERING
	/*
		Compute the array of document lengths and other parameters necessary for impact ordering on
		the relevance ranking functions
	*/
	ANT_memory_index_hash_node *node;
	double max_rsv, max_rsv_for_node;
	long long doc_size, tf_size;

	node = find_add_node(hash_table[hashed_squiggle_length], squiggle_length);

	get_serialised_postings(node, &doc_size, &tf_size);
	document_lengths = (ANT_compressable_integer *)memory->malloc((largest_docno  + 1) * sizeof(ANT_compressable_integer));
	node->serialise_postings(serialised_docids, &serialised_docids_size, serialised_tfs, &serialised_tfs_size);
	variable_byte.decompress(document_lengths, serialised_docids, node->document_frequency);

	/*
		Now compute the maximum impact score across the collection
	*/
	max_rsv = 0;
	for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
		if (hash_table[hash_val] != NULL)
			if ((max_rsv_for_node = rsv_all_nodes(hash_table[hash_val])) > max_rsv)
				max_rsv = max_rsv_for_node;

	printf("MAX:%f\n", max_rsv);
#endif
/*
	Write the postings
*/
for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
	if (hash_table[hash_val] != NULL)
		{
		if ((terms_in_node = serialise_all_nodes(file, hash_table[hash_val])) > max_terms_in_node)
			max_terms_in_node = terms_in_node;
		unique_terms += terms_in_node;
		}

/*
	Generate a list of all the unique terms in the collection
*/
bytes = sizeof(*term_list) * (unique_terms + 1);
stats->bytes_used_to_sort_term_list = bytes;
term_list = (ANT_memory_index_hash_node **)memory->malloc(bytes);
where = 0;
for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
	if (hash_table[hash_val] != NULL)
		where += generate_term_list(hash_table[hash_val], term_list, where, &length_of_longest_term, &highest_df);
term_list[unique_terms] = NULL;

/*
	Sort the term list
*/
qsort(term_list, unique_terms, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

/*
	Work out how many nodes there are in the root of the b-tree
*/
btree_root_size = 0;
for (here = term_list; *here != NULL; here = find_end_of_node(here))
	btree_root_size++;

/*
	Write the term list and generate the header list
*/
current_header = header = (ANT_btree_head_node *)memory->malloc(sizeof(ANT_btree_head_node) * btree_root_size);
here = term_list;
while (*here != NULL)
	{
	current_header->disk_pos = file->tell();
	current_header->node = *here;
	current_header++;
	here = write_node(file, here);
	}
last_header = current_header;
terms_in_root = last_header - header;

/*
	Take note of where the header will be located on disk
*/
file_position = file->tell();

/*
	Write the header to disk N then N * (string, offset) pairs
*/
file->write((unsigned char *)&terms_in_root, sizeof(terms_in_root));	// 4 bytes

//printf("Terms in root:%llu\n", (unsigned long long) terms_in_root);

for (current_header = header; current_header < last_header; current_header++)
	{
	file->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	file->write(&zero, sizeof(zero));									// 1 byte
	eight_byte = current_header->disk_pos;
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));		// 8 bytes
	}

/*
	Write the location of the header to file
*/
//printf("Root pos on disk:%llu\n", (unsigned long long) file_position);
file->write((unsigned char *)&file_position, sizeof(file_position));	// 8 bytes

/*
	The string length of the longest term
*/
file->write((unsigned char *)&length_of_longest_term, sizeof(length_of_longest_term));		// 4 bytes

/*
	The maximum length of a compressed posting list
*/
longest_postings_size = (uint32_t)(serialised_docids_size + serialised_tfs_size);
file->write((unsigned char *)&longest_postings_size, sizeof(longest_postings_size));	// 4 byte

/*
	and the maximum number of postings in a postings list (that is, the largest document frequencty (DF))
*/
file->write((unsigned char *)&highest_df, sizeof(highest_df));		// 8 bytes

/*
	Close (and flush) the file
*/
file->close();
delete file;

return 1;
}

/*
	ANT_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------
*/
void ANT_memory_index::text_render(ANT_memory_index_hash_node *root, unsigned char *serialised_docids, long doc_size, unsigned char *serialised_tfs, long tf_size)
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

