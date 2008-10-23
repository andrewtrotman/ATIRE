/*
	MEMORY_INDEX.C
	--------------
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "memory_index_hash_node.h"
#include "memory_index.h"
#include "memory.h"
#include "string_pair.h"

/*
	ANT_MEMORY_INDEX::ANT_MEMORY_INDEX()
	------------------------------------
*/
ANT_memory_index::ANT_memory_index()
{
memset(hash_table, 0, sizeof(hash_table));
memory = new ANT_memory;
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
delete memory;
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
		return root->left = new (memory) ANT_memory_index_hash_node(memory, string);
	else
		return find_add_node(root->left, string);
else
	if (root->right == NULL)
		return root->right = new (memory) ANT_memory_index_hash_node(memory, string);
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

hash_value = hash(string);
if (hash_table[hash_value] == NULL)
	node = hash_table[hash_value] = new (memory) ANT_memory_index_hash_node(memory, string);
else
	node = find_add_node(hash_table[hash_value], string);
node->add_posting(docno);
}

/*
	ANT_MEMORY_INDEX::SERIALISE_ALL_NODES()
	---------------------------------------
*/
long long ANT_memory_index::serialise_all_nodes(ANT_memory_index_hash_node *root)
{
long long bytes = 0;
long doc_size, tf_size, total;

if (root->right != NULL)
	bytes += serialise_all_nodes(root->right);

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

//
text_render(root, serialised_docids, doc_size, serialised_tfs, tf_size);
//

if (root->left != NULL)
	bytes += serialise_all_nodes(root->left);

return bytes;
}

/*
	ANT_MEMORY_INDEX::SERIALISE()
	-----------------------------
*/
long long ANT_memory_index::serialise()
{
long long bytes = 0;
long hash_val;

for (hash_val = 0; hash_val < HASH_TABLE_SIZE; hash_val++)
	if (hash_table[hash_val] != NULL)
		{
//		unsigned long dehash_val = dehash(hash_val);
//		printf("NODE:%d %c%c%c%c\n", hash_val, dehash_val & 0xFF, (dehash_val >> 8) & 0xFF, (dehash_val >> 16) & 0xFF, (dehash_val >> 24) & 0xFF);
		bytes += serialise_all_nodes(hash_table[hash_val]);
		}
return bytes;
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

