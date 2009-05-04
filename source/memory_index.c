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

#define DISK_BUFFER_SIZE (10 * 1024 * 1024)

/*
	ANT_MEMORY_INDEX::ANT_MEMORY_INDEX()
	------------------------------------
*/
ANT_memory_index::ANT_memory_index()
{
memset(hash_table, 0, sizeof(hash_table));
memory = new ANT_memory;
stats = new ANT_memory_index_stats(memory);
serialised_docids_size = 1;
serialised_docids = (unsigned char *)memory->malloc(serialised_docids_size);
serialised_tfs_size = 1;
serialised_tfs = (unsigned char *)memory->malloc(serialised_tfs_size);
factory = new ANT_compression_factory;
}

/*
	ANT_MEMORY_INDEX::~ANT_MEMORY_INDEX()
	-------------------------------------
*/
ANT_memory_index::~ANT_memory_index()
{
stats->text_render();
delete memory;
delete stats;
delete factory;
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
void ANT_memory_index::add_term(ANT_string_pair *string, long long docno)
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
node->add_posting(string, docno);
}

/*
	ANT_MEMORY_INDEX::SET_DOCUMENT_LENGTH()
	---------------------------------------
*/
void ANT_memory_index::set_document_length(long long docno, long length)
{
ANT_string_pair string("~length", 7);
long hash_value;
ANT_memory_index_hash_node *node;
long long doc;

doc = docno;				// remove compiler warning

hash_value = hash(&string);
if (hash_table[hash_value] == NULL)
	{
	stats->hash_nodes++;
	node = hash_table[hash_value] = new_memory_index_hash_node(&string);
	}
else
	node = find_add_node(hash_table[hash_value], &string);
node->current_docno = 0;
node->add_posting(&string, length);
}

/*
	ANT_MEMORY_INDEX::SERIALISE_ALL_NODES()
	---------------------------------------
*/
long ANT_memory_index::serialise_all_nodes(ANT_file *file, ANT_memory_index_hash_node *root)
{
long terms = 1;
long doc_size, tf_size, total;

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

#ifdef NEVER

/*
	At this point we know the number of documents in the collection so we can allocate the
	source and destination buffers once and be sure it all fits.

	FIX THIS CODE 
*/
source_buffer = new ANT_compressable_integer[root->document_frequency];
destination_buffer = new unsigned char [destination_length = (sizeof(ANT_compressable_integer) * root->document_frequency)];
factory->compress(destination_buffer, destination_length, source_buffer, root->document_frequency);

delete [] source_buffer;
delete [] destination_buffer[];

#endif

/*
	text_render(root, serialised_docids, doc_size, serialised_tfs, tf_size);
*/
root->docids_pos_on_disk = file->tell();
file->write(serialised_docids, doc_size);
root->tfs_pos_on_disk = file->tell();
file->write(serialised_tfs, tf_size);
root->end_pos_on_disk = file->tell();

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

current = start;
/*
	Find the end of the node
*/
if ((*current)->string.length() < B_TREE_PREFIX_SIZE)
	current++;
else
	while (*current != NULL)
		{
		if ((*current)->string.length() < B_TREE_PREFIX_SIZE)
			break;
		if ((*current)->string.strncmp(&(*start)->string, B_TREE_PREFIX_SIZE) != 0)
			break;
		current++;
		}
/*
	Number of terms in a node
*/
four_byte = terms_in_node = (uint32_t)(current - start);		// the number of terms in the node limited to 4 Billion!
file->write((unsigned char *)&terms_in_node, sizeof(terms_in_node));		// 4 bytes

/*
	CF, DF, Offset_in_postings, DocIDs_Len, Postings_len, String_pos_in_node
*/
current_node_head_length = (*start)->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : (uint32_t)(*start)->string.length();
end = current;
string_pos = (uint32_t)(current - start) * (1 * 8 + 5 * 4) + 4;
for (current = start; current < end; current++)
	{
	four_byte = (uint32_t)(*current)->collection_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (uint32_t)(*current)->document_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	eight_byte = (uint64_t)((*current)->docids_pos_on_disk);
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

	four_byte = (uint32_t)((*current)->tfs_pos_on_disk - (*current)->docids_pos_on_disk);
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
long btree_root_worst_case;
uint64_t file_position;
uint64_t terms_in_root, eight_byte;
long terms_in_node, unique_terms = 0, max_terms_in_node = 0;
long hash_val, where, bytes;
int32_t length_of_longest_term = 0;
uint32_t longest_postings_size;
int64_t highest_df = 0;
ANT_file *file;
ANT_memory_index_hash_node **term_list, **here;
ANT_btree_head_node *header, *current_header, *last_header;

file = new ANT_file(memory);
file->open(filename, "w+b");
file->setvbuff(DISK_BUFFER_SIZE);
stats->disk_buffer = DISK_BUFFER_SIZE;

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
	Write the term list and generate the header list
*/
btree_root_worst_case = (long)pow((double)ANT_ALPHABET_SIZE, (double)B_TREE_PREFIX_SIZE) + 1; // +1 for "special" terms such as document lengths
current_header = header = (ANT_btree_head_node *)memory->malloc(sizeof(ANT_btree_head_node) * btree_root_worst_case);
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

printf("Terms in root:%llu\n", (unsigned long long) terms_in_root);

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
printf("Root pos on disk:%llu\n", (unsigned long long) file_position);
file->write((unsigned char *)&file_position, sizeof(file_position));	// 8 bytes
/*
	The string length of the longest term
*/
file->write((unsigned char *)&length_of_longest_term, sizeof(length_of_longest_term));		// 4 bytes
/*
	The maximum length of a compressed posting list
*/
longest_postings_size = serialised_docids_size + serialised_tfs_size;
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

