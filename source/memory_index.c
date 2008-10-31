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
#include "btree_head_node.h"

#define DISK_BUFFER_SIZE (10 * 1024 * 1024)
#define B_TREE_PREFIX_SIZE 4

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
}

/*
	ANT_MEMORY_INDEX::HASH()
	------------------------
*/
long ANT_memory_index::hash(ANT_string_pair *string)
{
long ans, len;
const unsigned char base = 'a' - 1;

ans = (tolower((*string)[0]) - base) * 27 * 27 * 27;

if ((len = string->length()) > 1)
	ans += (tolower((*string)[1]) - base) * 27 * 27;
if (len > 2)
	ans += (tolower((*string)[2]) - base) * 27;
if (len > 3)
	ans += (tolower((*string)[3]) - base);

return ans;
}

/*
	ANT_MEMORY_INDEX::DEHASH()
	--------------------------
*/
unsigned long ANT_memory_index::dehash(long hash_val)
{
unsigned long ans;
const unsigned char base = 'a' - 1;

ans  = ((hash_val % 27) + base) << 24;
ans |= ((hash_val / 27) % 27 + base) << 16;
ans |= ((hash_val / (27*27)) % 27 + base) << 8;
ans |= ((hash_val / (27*27*27)) % 27 + base);

return ans;
}

/*
	ANT_MEMORY_INDEX::FIND_ADD_NODE()
	---------------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string)
{
long cmp;

cmp = string->stricmp(&(root->string));
if (cmp == 0)
	return root;
else if (cmp > 0)
	if (root->left == NULL)
		return root->left = new_memory_index_hash_node(string);
	else
		return find_add_node(root->left, string);
else
	if (root->right == NULL)
		return root->right = new_memory_index_hash_node(string);
	else
		return find_add_node(root->right, string);
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
node->add_posting(docno);
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

//printf("\t%s (df:%I64d cf:%I64d)\n", root->string.str(), root->document_frequency, root->collection_frequency);
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
long ANT_memory_index::generate_term_list(ANT_memory_index_hash_node *root, ANT_memory_index_hash_node **into, long where)
{
long terms = 0;

if (root->right != NULL)
	terms = generate_term_list(root->right, into, where);

into[where + terms] = root;

if (root->left != NULL)
	terms += generate_term_list(root->left, into, where + terms + 1);

return terms + 1;
}

/*
	ANT_MEMORY_INDEX::WRITE_NODE()
	------------------------------
*/
ANT_memory_index_hash_node **ANT_memory_index::write_node(ANT_file *file, ANT_memory_index_hash_node **start)
{
unsigned char zero = 0;
unsigned long long eight_byte;
unsigned long four_byte, string_pos;
long terms_in_node, current_node_head_length;
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
		if ((*current)->string.strnicmp(&(*start)->string, B_TREE_PREFIX_SIZE) != 0)
			break;
		current++;
		}
/*
	Number of terms in a node
*/
eight_byte = (unsigned long long)(terms_in_node = current - start);
file->write((unsigned char *)&terms_in_node, sizeof(terms_in_node));	// 8 bytes

/*
	CF, DF, Offset_in_postings, DocIDs_Len, Postings_len, String_pos_in_node
*/
current_node_head_length = (*start)->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : (*start)->string.length();
end = current;
string_pos = (current - start) * (2 * 8 + 4 * 4) + 8;
for (current = start; current < end; current++)
	{
	four_byte = (unsigned long)(*current)->collection_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (unsigned long)(*current)->document_frequency;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	eight_byte = (unsigned long long)((*current)->docids_pos_on_disk);
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

	eight_byte = (unsigned long long)((*current)->tfs_pos_on_disk - (*current)->docids_pos_on_disk);
	file->write((unsigned char *)&eight_byte, sizeof(eight_byte));

	four_byte = (unsigned long)((*current)->end_pos_on_disk - (*current)->docids_pos_on_disk);
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	four_byte = (unsigned long)string_pos;
	file->write((unsigned char *)&four_byte, sizeof(four_byte));

	string_pos += (*current)->string.length() + 1 - current_node_head_length;
	}
/*
	Finally the strings ('\0' terminated)
*/
for (current = start; current < end; current++)
	{
	file->write((unsigned char *)((*current)->string.string() + current_node_head_length), (*current)->string.length() - current_node_head_length);
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
unsigned char zero = 0;
long btree_root_worst_case;
long long file_position, terms_in_root;
long terms_in_node, unique_terms = 0, max_terms_in_node = 0;
long hash_val, where, bytes;
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
		where += generate_term_list(hash_table[hash_val], term_list, where);
term_list[unique_terms] = NULL;

/*
	Sort the term list
*/
qsort(term_list, unique_terms, sizeof(*term_list), ANT_memory_index_hash_node::term_compare);

/*
	Write the term list and generate the header list
*/
btree_root_worst_case = (long)pow((double)27, (double)B_TREE_PREFIX_SIZE);
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
file->write((unsigned char *)&terms_in_root, sizeof(terms_in_root));
for (current_header = header; current_header < last_header; current_header++)
	{
	file->write((unsigned char *)current_header->node->string.string(), current_header->node->string.length() > B_TREE_PREFIX_SIZE ? B_TREE_PREFIX_SIZE : current_header->node->string.length());
	file->write(&zero, sizeof(zero));
	file->write((unsigned char *)&(current_header->disk_pos), sizeof(current_header->disk_pos));
	}

/*
	Write the location of the header to file
*/
file->write((unsigned char *)&file_position, sizeof(file_position));

/*
	Close (and flush) the file
*/
file->close();
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
printf("\t%s (df:%I64d cf:%I64d):", root->string.str(), root->document_frequency, root->collection_frequency);
pos = serialised_docids;
while (pos < serialised_docids + doc_size)
	{
	doc += root->decompress(&pos);
	tf = *serialised_tfs++;
	printf("(%d,%d),", doc, tf);
	}
putchar('\n');
}

