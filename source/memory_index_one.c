/*
	MEMORY_INDEX_ONE.C
	------------------
*/
#include <string.h>
#include "hash_table.h"
#include "string_pair.h"
#include "memory_index_one.h"
#include "memory_index_one_node.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_MEMORY_INDEX_ONE::HASH()
	----------------------------
*/
inline long ANT_memory_index_one::hash(ANT_string_pair *string)
{
return ANT_hash_24(string) % HASH_TABLE_SIZE;
}

/*
	ANT_MEMORY_INDEX_ONE::ANT_MEMORY_INDEX_ONE()
	--------------------------------------------
*/
ANT_memory_index_one::ANT_memory_index_one(ANT_memory *memory, ANT_memory_index *index)
{
hashed_squiggle_length = hash(squiggle_length);
this->memory = memory;
this->final_index = index;
rewind();
}

/*
	ANT_MEMORY_INDEX_ONE::~ANT_MEMORY_INDEX_ONE()
	---------------------------------------------
*/
ANT_memory_index_one::~ANT_memory_index_one()
{
delete memory;
}

/*
	ANT_MEMORY_INDEX_ONE::REWIND()
	------------------------------
*/
void ANT_memory_index_one::rewind(void)
{
memory->rewind();
memset(hash_table, 0, sizeof(hash_table));
}

/*
	ANT_MEMORY_INDEX_ONE::NEW_HASH_NODE()
	-------------------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::new_hash_node(ANT_string_pair *pair)
{
long hash_value;
ANT_memory_index_hash_node *root;
ANT_memory_index_one_node *node;

node = new (memory) ANT_memory_index_one_node;
node->left = node->right = NULL;
node->mode = MODE_ABSOLUTE;
node->string.start = (char *)memory->malloc(pair->string_length + 1);
pair->strcpy(node->string.start);
node->string.string_length = pair->string_length;
node->term_frequency = 0;

hash_value = final_index->hash(pair);
root = final_index->hash_table[hash_value];
if (root == NULL)
	node->final_node = NULL;
else
	node->final_node = final_index->find_node(root, pair);

return node;
}

/*
	ANT_FOCUS::FIND_ADD_NODE()
	--------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::find_add_node(ANT_memory_index_one_node *root, ANT_string_pair *string)
{
long cmp;

while ((cmp = string->strcmp(&(root->string))) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return root->left = new_hash_node(string);
		else
			root = root->left;
	else
		if (root->right == NULL)
			return root->right = new_hash_node(string);
		else
			root = root->right;
	}

return root;
}

/*
	ANT_MEMORY_INDEX_ONE::ADD()
	---------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::add(ANT_string_pair *string, long long docno, long extra_term_frequency)
{
ANT_memory_index_one_node *answer;
long hash_value = hash(string);

if (hash_table[hash_value] == NULL)
	answer = hash_table[hash_value] = new_hash_node(string);
else
	answer = find_add_node(hash_table[hash_value], string);

answer->term_frequency += extra_term_frequency;
answer->mode = MODE_MONOTONIC;

return answer;
}

/*
	ANT_MEMORY_INDEX_ONE::ADD_TERM()
	--------------------------------
*/
ANT_memory_indexer_node *ANT_memory_index_one::add_term(ANT_string_pair *string, long long docno, long extra_term_frequency)
{
return add(string, docno, extra_term_frequency);
}

/*
	ANT_MEMORY_INDEX_ONE::SET_DOCUMENT_DETAIL()
	-------------------------------------------
*/
void ANT_memory_index_one::set_document_detail(ANT_string_pair *string, long long score, long mode)
{
ANT_memory_index_one_node *node = add(string, 0, score);

if (node != NULL)
	{
	node->term_frequency = score;
	node->mode = mode;
	}
}
