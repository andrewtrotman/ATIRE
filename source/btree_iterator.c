/*
	BTREE_ITERATOR.C
	----------------
*/
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "search_engine.h"
#include "search_engine_btree_node.h"
#include "btree_iterator.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef TERM_LOCAL_MAX_IMPACT
	/*
		CF, DF, Offset_in_postings, DocIDs_Len, Postings_len, local_max_impact, String_pos_in_node
		(4 + 4 + 8 + 4 + 4 + 1 + 4)
	 */
	const long ANT_btree_iterator::LEAF_SIZE = 29;
#else
	/*
		CF, DF, Offset_in_postings, DocIDs_Len, Postings_len, String_pos_in_node
		(4 + 4 + 8 + 4 + 4 + 4)
	*/
	const long ANT_btree_iterator::LEAF_SIZE = 28;
#endif

/*
	ANT_BTREE_ITERATOR::ANT_BTREE_ITERATOR()
	----------------------------------------
*/
ANT_btree_iterator::ANT_btree_iterator(ANT_search_engine *search_engine)
{
this->search_engine = search_engine;
btree_leaf_buffer = new unsigned char [(size_t)search_engine->max_header_block_size];
}

/*
	ANT_BTREE_ITERATOR::~ANT_BTREE_ITERATOR()
	-----------------------------------------
*/
ANT_btree_iterator::~ANT_btree_iterator()
{
delete [] btree_leaf_buffer;
}

/*
	ANT_BTREE_ITERATOR::FIRST()
	---------------------------
*/
char *ANT_btree_iterator::first(char *term)
{
long long node_length, node_position;
long exact_match, terms_in_leaf;
size_t length_of_term;
long low, high, mid;
long before_first_term = FALSE;

if (term == NULL)
	node_position = node_length = exact_match = node = 0;
else
	node_position = search_engine->get_btree_leaf_position(term, &node_length, &exact_match, &node);

if (node_position == 0)		// then we are before the first term (or term == NULL)
	{
	before_first_term = TRUE;
	node = 1;
	node_position = search_engine->btree_root[node].disk_pos;
	node_length = search_engine->btree_root[node + 1].disk_pos - node_position;
	}

keyword_head_length = strlen(search_engine->btree_root[node].term);
strcpy(keyword, search_engine->btree_root[node].term);

search_engine->index->seek(node_position);
search_engine->index->read(btree_leaf_buffer, node_length);

if (before_first_term)	// then we are before the first term so use the first term in the node
	{
	leaf = -1;
	return next();
	}
else
	{
	length_of_term = strlen(term);
	if (length_of_term < B_TREE_PREFIX_SIZE)
		if (!exact_match)
			{
			leaf = ANT_get_long(btree_leaf_buffer);		// we have a short string (less then the length of the head node) and did not find it as a node
			return next();
			}
		else
			term += length_of_term;
	else
		if (strncmp(search_engine->btree_root[node].term, term, B_TREE_PREFIX_SIZE) != 0)
			{
			leaf = ANT_get_long(btree_leaf_buffer);		// there is no node in the list that starts with the head of the string.
			return next();
			}
		else
			term += B_TREE_PREFIX_SIZE;

	low = 0;
	high = terms_in_leaf = ANT_get_long(btree_leaf_buffer);

	while (low < high)
		{
		mid = (low + high) / 2;
		if (strcmp((char *)(btree_leaf_buffer + ANT_get_long(btree_leaf_buffer + (LEAF_SIZE * (mid + 1)))), term) < 0)
			low = mid + 1;
		else
			high = mid;
		}
	leaf = low;
	if (leaf >= terms_in_leaf)
		return next();
	else
		strcpy(keyword + keyword_head_length, (char *)(btree_leaf_buffer + ANT_get_long(btree_leaf_buffer + (LEAF_SIZE * (leaf + 1)))));
	}
return keyword;
}

/*
	ANT_BTREE_ITERATOR::NEXT()
	--------------------------
*/
char *ANT_btree_iterator::next(void)
{
long long node_length, node_position;

leaf++;

if (leaf >= ANT_get_long(btree_leaf_buffer))
	{
	node++;
	if (search_engine->btree_root[node].term == NULL)		// then we are at the end of the term list
		return NULL;
	leaf = 0;

	node_position = search_engine->btree_root[node].disk_pos;
	node_length = search_engine->btree_root[node + 1].disk_pos - node_position;

	keyword_head_length = strlen(search_engine->btree_root[node].term);
	strcpy(keyword, search_engine->btree_root[node].term);

	search_engine->index->seek(node_position);
	search_engine->index->read(btree_leaf_buffer, node_length);
	}

strcpy(keyword + keyword_head_length, (char *)(btree_leaf_buffer + ANT_get_long(btree_leaf_buffer + (LEAF_SIZE * (leaf + 1)))));
return keyword;
}

/*
	ANT_BTREE_ITERATOR::GET_POSTINGS_DETAILS()
	------------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_btree_iterator::get_postings_details(ANT_search_engine_btree_leaf *term_details)
{
return search_engine->get_leaf(btree_leaf_buffer, leaf, term_details);
}
