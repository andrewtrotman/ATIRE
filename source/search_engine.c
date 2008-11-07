/*
	SEARCH_ENGINE.C
	---------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search_engine.h"
#include "file.h"
#include "memory.h"
#include "search_engine_btree_node.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_SEARCH_ENGINE::ANT_SEARCH_ENGINE()
	--------------------------------------
*/
ANT_search_engine::ANT_search_engine(ANT_memory *memory)
{
unsigned char *block;
long long end, term_header, max_header_block_size, this_header_block_size;
ANT_search_engine_btree_node *current, *end_of_node_list;

this->memory = memory;
index = new ANT_file(memory);
if (index->open("index.aspt", "rb") == 0)
	exit(printf("Cannot open index file:index.aspt\n"));

/*
	The final sizeof(long long) bytes of the file store the location of the b-tree header block
*/
end = index->file_length();
index->seek(end - sizeof(long long));
index->read(&term_header);

/*
	Load the b-tree header
*/
index->seek(term_header);
block = (unsigned char *)memory->malloc((long)(end - term_header));
index->read(block, (long)(end - term_header));

/*
	The first sizeof(long long) bytes of the header are the number of nodes in the root
*/
btree_nodes = (long)(get_long_long(block) + 1);		// +1 because were going to add a sentinal at the start
block += sizeof(long long);
btree_root = (ANT_search_engine_btree_node *)memory->malloc((long)(sizeof(ANT_search_engine_btree_node) * (btree_nodes + 1)));	// +1 to null terminate (with the end of last block position)

/*
	Then we have a sequence of '\0' terminated string / offset pairs
	But first add the sentinal to the beginning (this is a one-off expense at startup)
*/
current = btree_root;
current->disk_pos = 0;
current->term = (char *)memory->malloc(2);
current->term[0] = '\0';
end_of_node_list = btree_root + btree_nodes;
for (current++; current < end_of_node_list; current++)
	{
	current->term = (char *)block;
	while (*block != '\0')
		block++;
	block++;
	current->disk_pos = get_long_long(block);
	block += sizeof(current->disk_pos);
	}
current->term = NULL;
current->disk_pos = term_header;
/*
	Compute the size of the largest block and then allocate memory so that it will fit (and use that throughout the execution of this program)
*/
max_header_block_size = 0;
for (current = btree_root + 1; current < end_of_node_list; current++)
	{
	this_header_block_size = (current + 1)->disk_pos - current->disk_pos;
	if (this_header_block_size > max_header_block_size)
		max_header_block_size = this_header_block_size;
//	printf("%s : %I64d (size:%I64d bytes)\n", current->term, current->disk_pos, this_header_block_size);
	}
btree_leaf_buffer = (unsigned char *)memory->malloc((long)max_header_block_size);
}

/*
	ANT_SEARCH_ENGINE::~ANT_SEARCH_ENGINE()
	---------------------------------------
*/
ANT_search_engine::~ANT_search_engine()
{
index->close();
delete index;
}

/*
	ANT_SEARCH_ENGINE::GET_BTREE_LEAF_POSITION()
	--------------------------------------------
*/
long long ANT_search_engine::get_btree_leaf_position(char *term, long long *length, long *exact_match)
{
long low, high, mid;

/*
	Binary search to find the block containing the term
*/
low = 0;
high = btree_nodes;
while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp(btree_root[mid].term, term) < 0)
		low = mid + 1;
	else
		high = mid; 
	}
if ((low < btree_nodes) && (strcmp(btree_root[low].term, term) == 0))
	{
	/*
		Found, so we're either a short string or we're the name of a header
	*/
	*exact_match = TRUE;
	*length = btree_root[low + 1].disk_pos - btree_root[low].disk_pos;
	return btree_root[low].disk_pos;
	}
else
	{
	/*
		Not Found, so we're one past the header node
	*/
	*exact_match = FALSE;
	*length = btree_root[low].disk_pos - btree_root[low - 1].disk_pos;
	return btree_root[low - 1].disk_pos;
	}
}

/*
	ANT_SEARCH_ENGINE::GET_POSTINGS_DETAILS()
	-----------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine::get_postings_details(char *term)
{
long long node_position, node_length;
long low, high, mid;
long leaf_size, exact_match;
unsigned char *base;

if ((node_position = get_btree_leaf_position(term, &node_length, &exact_match)) == 0)
	return NULL;		// before the first term in the term list

if (!exact_match && strlen(term) < B_TREE_PREFIX_SIZE)
	return NULL;		// we have a short string (less then the length of the head node) and did not find it as a node

index->seek(node_position);
index->read(btree_leaf_buffer, (long)node_length);
/*
	First 4 bytes are the number of terms in the node
	then there are N loads of:
		CF (4), DF (4), Offset_in_postings (8), DocIDs_Len (4), Postings_len (4), String_pos_in_node (4)
*/
low = 0;
high = (long)get_long(btree_leaf_buffer);
leaf_size = 28;		// length of a leaf node (sum of cf, df, etc. sizes)

term += B_TREE_PREFIX_SIZE;		// it must be at least this long as we checked earlier

while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp((char *)(btree_leaf_buffer + get_long(btree_leaf_buffer + (leaf_size * (mid + 1)))), term) < 0)
		low = mid + 1;
	else
		high = mid;
	}
if ((low < btree_nodes) && (strcmp((char *)(btree_leaf_buffer + get_long(btree_leaf_buffer + (leaf_size * (low + 1)))), term) == 0))
	{
	base = btree_leaf_buffer + leaf_size * low;
	term_details.collection_frequency = get_long(base);
	term_details.document_frequency = get_long(base + 4);
	term_details.postings_position_on_disk = get_long_long(base + 8);
	term_details.docid_length = get_long(base + 16);
	term_details.postings_length = get_long(base + 20);
	return &term_details;
	}
else
	return NULL;
}
