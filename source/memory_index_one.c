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
#include "maths.h"

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
inline long ANT_memory_index_one::hash(ANT_string_pair *string, long *final_hash_value)
{
#ifdef DOUBLE_HASH
	*final_hash_value = -1; // this will cause errors if it ever gets used, which is good
	return ANT_hash_8(string);
#else
	*final_hash_value = ANT_memory_index::hash(string);
	return *final_hash_value % HASH_TABLE_SIZE;
#endif
}

/*
	ANT_MEMORY_INDEX_ONE::ANT_MEMORY_INDEX_ONE()
	--------------------------------------------
*/
ANT_memory_index_one::ANT_memory_index_one(ANT_memory *memory, ANT_memory_index *index)
{
long dummy;
hashed_squiggle_length = hash(&squiggle_length, &dummy);
this->memory = memory;
this->final_index = index;
this->stopwords = new ANT_stop_word(index->stopwords->get_type());		// re-use the same stop words list for each instance of this class
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
delete stopwords;
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
ANT_memory_index_one_node *ANT_memory_index_one::new_hash_node(ANT_string_pair *pair, long final_hash_value)
{
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
#ifdef DOUBLE_HASH
	final_hash_value = final_index->hash(pair);
#endif
	root = final_index->hash_table[final_hash_value];
	}

if (root == NULL)
	node->final_node = NULL;
else
	node->final_node = final_index->find_node(final_hash_value, pair);

nodes_used++;

return node;
}

/*
	ANT_MEMORY_INDEX_ONE::FIND_ADD_NODE()
	-------------------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::find_add_node(long hash_value, long final_hash_value, ANT_string_pair *string, long *depth)
{
ANT_memory_index_one_node *root = hash_table[hash_value];
long cmp;
*depth = 1;

while ((cmp = string->strcmp(&(root->string))) != 0)
	{
	*depth += 1;
	if (cmp > 0)
		if (root->left == NULL)
			return root->left = new_hash_node(string, final_hash_value);
		else
			root = root->left;
	else
		if (root->right == NULL)
			return root->right = new_hash_node(string, final_hash_value);
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
long final_hash_value;
long hash_value = hash(string, &final_hash_value);
long depth = 1;

if (hash_table[hash_value] == NULL)
	answer = hash_table[hash_value] = new_hash_node(string, final_hash_value);
else
	answer = find_add_node(hash_value, final_hash_value, string, &depth);

#if REBALANCE_FACTOR > 0
if (depth >= REBALANCE_FACTOR)
	rebalance_tree(hash_value);
#endif

answer->term_frequency += extra_term_frequency;
answer->mode = MODE_MONOTONIC;

return answer;
}

/*
	ANT_MEMORY_INDEX_ONE::REBALANCE_TREE()
	--------------------------------------
	Uses the DSW algorithm to rebalance the tree at a given hash value
*/
void ANT_memory_index_one::rebalance_tree(long hash_value)
{
dummy_root.right = hash_table[hash_value];

// convert to a singly linked list via right rotations
int size = tree_to_vine(&dummy_root);

int full_size = 1;
while (full_size <= size)
	full_size = full_size + full_size + 1;
full_size /= 2;

// do a series of rotations to get to a balanced tree
vine_to_tree(&dummy_root, size - full_size);
while (full_size > 1)
	vine_to_tree(&dummy_root, full_size /= 2);

// the root of the tree might have changed of course
hash_table[hash_value] = dummy_root.right;
}

/*
	ANT_MEMORY_INDEX_ONE::TREE_TO_VINE()
	------------------------------------
	Converts a tree to a singly linked list, by left rotations, counting the
	number of nodes that are in the tree.
*/
int ANT_memory_index_one::tree_to_vine(ANT_memory_index_one_node *root)
{
ANT_memory_index_one_node *tail = root;
ANT_memory_index_one_node *remainder = root->right;
ANT_memory_index_one_node *tmp;

int nodes = 0;

while (remainder != NULL)
	if (remainder->left == NULL)
		{
		tail = remainder;
		remainder = remainder->right;
		nodes++;
		}
	else
		{
		tmp = remainder->left;
		remainder->left = tmp->right;
		tmp->right = remainder;
		remainder = tmp;
		tail->right = tmp;
		}

return nodes;
}

/*
	ANT_MEMORY_INDEX_ONE::VINE_TO_TREE()
	------------------------------------
	Performs the given number of right rotations on every second node going
	down the right hand side of the tree.
*/
void ANT_memory_index_one::vine_to_tree(ANT_memory_index_one_node *root, int number)
{
ANT_memory_index_one_node *current = root;
ANT_memory_index_one_node *child;

for (int j = 0; j < number; j++)
	{
	child = current->right;
	current->right = child->right;
	current = current->right;
	child->right = current->left;
	current->left = child;
	}
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
long dummy;

left = right = center = 0.0;

/*
	Current node
*/
if (node->string[0] != '~')
	{
	if ((term_details = document_collection->find_node(document_collection->hash_table[hash(&node->string, &dummy)], &node->string)) == NULL)
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

/*
	ANT_MEMORY_INDEX_ONE::TREE_GET_FREQUENCIES()
	--------------------------------------------
*/
void ANT_memory_index_one::tree_get_frequencies(ANT_memory_index_one_node *node, short *frequency, long long tf_cap)
{
long long use;

if (node->string[0] != '~' && !ANT_isupper(node->string[0]))
	{
	use = ANT_min(node->term_frequency, tf_cap);
	frequency[use]++;
	}

if  (node->left != NULL)
	tree_get_frequencies(node->left, frequency, tf_cap);
if  (node->right != NULL)
	tree_get_frequencies(node->right, frequency, tf_cap);
}

/*
	ANT_MEMORY_INDEX_ONE::GET_FREQUENCIES()
	---------------------------------------
*/
short *ANT_memory_index_one::get_frequencies(short *frequency, long long tf_cap)
{
long node;

for (node = 0; node < HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		tree_get_frequencies(hash_table[node], frequency, tf_cap);

return frequency;
}

/*
	ANT_MEMORY_INDEX_ONE::GET_TERM_NODE()
	-------------------------------------
*/
ANT_memory_index_one_node *ANT_memory_index_one::get_term_node(ANT_string_pair *term)
{
long dummy;
size_t hash_value = hash(term, &dummy);

if (hash_table[hash_value] == NULL)
	return 0;

return find_node(hash_table[hash_value], term);
}

