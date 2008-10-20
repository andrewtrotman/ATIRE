/*
	MEMORY_INDEX.C
	--------------
*/
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

ans = tolower((*string)[0]) - 'a';
if ((len = string->length()) > 1)
	ans += (tolower((*string)[1]) - 'a') * 27;
if (len > 2)
	ans += (tolower((*string)[2]) - 'a') * 27 * 27;
if (len > 3)
	ans += (tolower((*string)[3]) - 'a') * 27 * 27 * 27;

return ans;
}

/*
	ANT_MEMORY_INDEX::FIND_ADD_NODE()
	---------------------------------
*/
ANT_memory_index_hash_node *ANT_memory_index::find_add_node(ANT_memory_index_hash_node *root, ANT_string_pair *string)
{
long cmp;

cmp = string->strcmp(&(root->string));
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
		return root->right;
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

