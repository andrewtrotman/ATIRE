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
ANT_memory_index_one_node *node = add(string, 0, (long)score);

if (node != NULL)
	{
	node->term_frequency = score;
	node->mode = mode;
	}
}

/*
	ANT_MEMORY_INDEX_ONE::KL_NODE()
	-------------------------------
*/
double ANT_memory_index_one::kl_node(ANT_memory_index_one_node *node, ANT_search_engine *document_collection)
{
double left, right, center, px, qx;

left = right = center = 0.0;

/*
	Current node
*/
if (node->string[0] != '~')
	{
	/*
		Calculate the Divergence for the current search term.  If the term is not in
		the collection then the divergence is infinate - to get around this we fake
		the existance of every term in the collection (with TF=1)
	*/
	px = (double)node->term_frequency / (double)document_length;

	node->string.strcpy(token_as_string);
	if ((document_collection->process_one_term(token_as_string, term_details)) == NULL)
		qx = 1.0 / (double)document_collection->get_collection_length();
	else
		qx = (double)term_details->collection_frequency / (double)document_collection->get_collection_length();

	node->kl_score = center = px * log (px / qx);

//	node->kl_score =term_details->collection_frequency;
	}

/*
	Children
*/
if  (node->left != NULL)
	left = kl_node(node->left, document_collection);

if  (node->right != NULL)
	right = kl_node(node->right, document_collection);

return left + right + center;
}

/*
	ANT_MEMORY_INDEX_ONE::KL_DIVERGENCE()
	-------------------------------------
	sum(p(x) log (p(x) / q(x))
	where p(x) is the probability in the document and q(x) is the probability in the collection
*/
double ANT_memory_index_one::kl_divergence(ANT_search_engine *collection)
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
		sum += kl_node(hash_table[node], collection);

return sum;
}

/*
	ANT_MEMORY_INDEX_ONE::TOP_TERMS_FROM_TREE()
	-------------------------------------------
*/
void ANT_memory_index_one::top_terms_from_tree(ANT_memory_index_one_node *node)
{
if (node->string[0] != '~')
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
	ANT_MEMORY_INDEX_ONE::TOP_N_DIVERGENT_TERMS()
	---------------------------------------------
*/
ANT_memory_index_one_node **ANT_memory_index_one::top_n_divergent_terms(ANT_search_engine *collection, long terms_wanted, long *terms_found)
{
long node;
double kl;

top_terms = new ANT_memory_index_one_node *[terms_wanted];
heap = new Heap<ANT_memory_index_one_node *, ANT_memory_index_one_node>(*top_terms, terms_wanted);
heap_terms = 0;
heap_size = terms_wanted;

/*
	Compute the term-by-term deviations
*/
kl = kl_divergence(collection);

/*
	Now walk the line
*/
for (node = 0; node < ANT_memory_index_one::HASH_TABLE_SIZE; node++)
	if (hash_table[node] != NULL)
		top_terms_from_tree(hash_table[node]);

delete heap;
*terms_found = heap_terms < terms_wanted ? heap_terms : terms_wanted;

qsort(top_terms, *terms_found, sizeof(*top_terms), ANT_memory_index_one_node_cmp);

return top_terms;
}

