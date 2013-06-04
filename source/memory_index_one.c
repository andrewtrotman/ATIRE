/*
	MEMORY_INDEX_ONE.C
	------------------
*/
#include <string.h>
#include "hash_table.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "string_pair.h"
#include "memory_index_one.h"
#include "memory_index_one_node.h"
#include "memory_index.h"
#include "memory_index_hash_node.h"
#include "term_divergence.h"

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
return ANT_memory_index::hash(string) % HASH_TABLE_SIZE;
}

/*
	ANT_MEMORY_INDEX_ONE::ANT_MEMORY_INDEX_ONE()
	--------------------------------------------
*/
ANT_memory_index_one::ANT_memory_index_one(ANT_memory *memory, ANT_memory_index *index)
{
hashed_squiggle_length = hash(&squiggle_length);
this->memory = memory;
this->final_index = index;
rewind();

term_details = NULL;
token_as_string = NULL;
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
// the memory->rewind will free the token_as_string, which we then attempt to use
// in the next iteration, so set it to NULL so that it will always be malloc'd afresh
token_as_string = NULL;
memset(hash_table, 0, sizeof(hash_table));
document_length = 0;
nodes_used = 0;
}

/*
	ANT_MEMORY_INDEX_ONE::NEW_HASH_NODE()
	-------------------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::new_hash_node(ANT_string_pair *pair)
{
long hash_value;
ANT_memory_index_hash_node *root = NULL;
ANT_memory_index_one_node *node;

node = new (memory) ANT_memory_index_one_node;
node->left = node->right = NULL;
node->mode = MODE_ABSOLUTE;
node->string.start = (char *)memory->malloc(pair->string_length + 1);
pair->strcpy(node->string.start);
node->string.string_length = pair->string_length;
node->term_frequency = 0;

if (final_index != NULL)
	{
	hash_value = final_index->hash(pair);
	root = final_index->hash_table[hash_value];
	}

if (root == NULL)
	node->final_node = NULL;
else
	node->final_node = final_index->find_node(root, pair);

nodes_used++;

return node;
}

/*
	ANT_MEMORY_INDEX_ONE::FIND_ADD_NODE()
	-------------------------------------
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
	ANT_MEMORY_INDEX_ONE::FIND_NODE()
	---------------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::find_node(ANT_memory_index_one_node *root, ANT_string_pair *string)
{
long cmp;

if (root == NULL)
	return NULL;

while ((cmp = string->strcmp(&(root->string))) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return NULL;
		else
			root = root->left;
	else
		if (root->right == NULL)
			return NULL;
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
ANT_memory_index_one_node *node = add(string, 0, (long)score);

if (node != NULL)
	{
	node->term_frequency = score;
	node->mode = mode;
	}
}

/*
	ANT_MEMORY_INDEX_ONE::ADD_TERM_TO_TABLE()
	-----------------------------------------
*/
void ANT_memory_index_one::add_term_to_table(ANT_memory_indexer_node **table, ANT_memory_index_one_node *node, long *term_id)
{
/*
	Add the term at the current node
*/
if (node->string[0] != '~')
	{
	if (node->final_node == NULL)
		table[*term_id] = node;
	else
		table[*term_id] = node->final_node;
	(*term_id)++;
	}

/*
	Now check the left and the right subtrees for hash collisions
*/
if  (node->left != NULL)
	add_term_to_table(table, node->left, term_id);

if  (node->right != NULL)
	add_term_to_table(table, node->right, term_id);
}

/*
	ANT_MEMORY_INDEX_ONE::GET_TERM_LIST()
	-------------------------------------
*/
ANT_memory_indexer_node **ANT_memory_index_one::get_term_list(void)
{
ANT_memory_indexer_node **node_table;
long node, term_id;

node_table = new ANT_memory_indexer_node *[nodes_used + 1];

for (term_id = node = 0; node < HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		add_term_to_table(node_table, hash_table[node], &term_id);

node_table[term_id] = NULL;

return node_table;
}

/*
	ANT_MEMORY_INDEX_ONE::KL_NODE()
	-------------------------------
*/
double ANT_memory_index_one::kl_node(ANT_term_divergence *divergence, ANT_memory_index_one_node *node, ANT_search_engine *document_collection)
{
long long collection_frequency;
double left, right, center;

left = right = center = 0.0;

/*
	Current node
*/
if (node->string[0] != '~')
	{
	node->string.strcpy(token_as_string);
	if ((document_collection->process_one_term(token_as_string, term_details)) == NULL)
		collection_frequency = 0;
	else
		collection_frequency = term_details->global_collection_frequency;

	node->kl_score = center = divergence->divergence(node->term_frequency, document_length, collection_frequency, document_collection->get_collection_length());
	}

/*
	Children
*/
if  (node->left != NULL)
	left = kl_node(divergence, node->left, document_collection);

if  (node->right != NULL)
	right = kl_node(divergence, node->right, document_collection);

return left + right + center;
}

/*
	ANT_MEMORY_INDEX_ONE::KL_NODE()
	-------------------------------
*/
double ANT_memory_index_one::kl_node(ANT_term_divergence *divergence, ANT_memory_index_one_node *node, ANT_memory_index_one *document_collection)
{
long long collection_frequency;
double left, right, center;
ANT_memory_index_one_node *term_details;

left = right = center = 0.0;

/*
	Current node
*/
if (node->string[0] != '~')
	{
	if ((term_details = document_collection->find_node(document_collection->hash_table[hash(&node->string)], &node->string)) == NULL)
		collection_frequency = 0;
	else
		collection_frequency = term_details->term_frequency;

	node->kl_score = center = divergence->divergence(node->term_frequency, document_length, collection_frequency, document_collection->document_length);
	}

/*
	Children
*/
if  (node->left != NULL)
	left = kl_node(divergence, node->left, document_collection);

if  (node->right != NULL)
	right = kl_node(divergence, node->right, document_collection);

return left + right + center;
}

/*
	ANT_MEMORY_INDEX_ONE::KL_DIVERGENCE()
	-------------------------------------
*/
double ANT_memory_index_one::kl_divergence(ANT_term_divergence *divergence, ANT_search_engine *collection)
{
long node;
double sum = 0;

if (token_as_string == NULL)
	{
	/*
		Initialise first time around
	*/
	token_as_string = (char *)memory->malloc(MAX_TERM_LENGTH);
	term_details = (ANT_search_engine_btree_leaf *)memory->malloc(sizeof(*term_details));
	}

for (node = 0; node < ANT_memory_index_one::HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		sum += kl_node(divergence, hash_table[node], collection);

return sum;
}

/*
	ANT_MEMORY_INDEX_ONE::KL_DIVERGENCE()
	-------------------------------------
*/
double ANT_memory_index_one::kl_divergence(ANT_term_divergence *divergence, ANT_memory_index_one *collection)
{
long node;
double sum = 0;

for (node = 0; node < ANT_memory_index_one::HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		sum += kl_node(divergence, hash_table[node], collection);

return sum;
}

/*
	ANT_MEMORY_INDEX_ONE::TOP_TERMS_FROM_TREE()
	-------------------------------------------
*/
void ANT_memory_index_one::top_terms_from_tree(ANT_memory_index_one_node *node)
{
if (ANT_isalpha(node->string[0]))
	if (heap_terms < heap_size)
		{
		top_terms[heap_terms] = node;
		heap_terms++;
		if (heap_terms == heap_size)
			heap->build_min_heap();
		}
	else if (node->kl_score > top_terms[0]->kl_score)
		{
		top_terms[0] = node;
		heap->min_insert(node);
		heap_terms++;
		}

if  (node->left != NULL)
	top_terms_from_tree(node->left);

if  (node->right != NULL)
	top_terms_from_tree(node->right);
}

/*
	ANT_MEMORY_INDEX_ONE_NODE_CMP()
	-------------------------------
*/
static int ANT_memory_index_one_node_cmp(const void *a, const void *b)
{
ANT_memory_index_one_node *first, *second;

first = *(ANT_memory_index_one_node **)a;
second = *(ANT_memory_index_one_node **)b;

return first->kl_score < second->kl_score ? 1 : first->kl_score == second->kl_score ? 0 : -1;
}

/*
	ANT_MEMORY_INDEX_ONE::TOP_N_TERMS()
	-----------------------------------
*/
ANT_memory_index_one_node **ANT_memory_index_one::top_n_terms(long terms_wanted, long *terms_found)
{
long node;

/*
	Allocate memory for the answer set
*/
top_terms = new ANT_memory_index_one_node *[terms_wanted + 1];

/*
	Set up the heap
*/
heap = new ANT_heap<ANT_memory_index_one_node *, ANT_memory_index_one_node>(*top_terms, terms_wanted);
heap_terms = 0;
heap_size = terms_wanted;

/*
	Walk the vocab
*/
for (node = 0; node < ANT_memory_index_one::HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		top_terms_from_tree(hash_table[node]);

*terms_found = heap_terms < heap_size ? heap_terms : heap_size;

/*
	Sort the heap
*/
qsort(top_terms, *terms_found, sizeof(*top_terms), ANT_memory_index_one_node_cmp);

/*
	NULL terminate the list
*/
top_terms[*terms_found] = NULL;
delete heap;

return top_terms;
}

