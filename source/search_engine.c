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
btree_nodes = get_long_long(block) + 1;		// +1 because were going to add a sentinal at the start
block += sizeof(btree_nodes);
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
long long ANT_search_engine::get_btree_leaf_position(char *term, long long *length)
{
long long low, high, mid;

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
	*length = btree_root[low + 1].disk_pos - btree_root[low].disk_pos;
	return btree_root[low].disk_pos;
	}
else
	{
	/*
		Not Found, so we're one past the header node
	*/
	*length = btree_root[low].disk_pos - btree_root[low - 1].disk_pos;
	return btree_root[low - 1].disk_pos;
	}
}

/*
	ANT_SEARCH_ENGINE::GET_POSTINGS_POSITION()
	------------------------------------------
*/
void *ANT_search_engine::get_postings_position(char *term)
{
long long node_position, node_length;

if ((node_position = get_btree_leaf_position(term, &node_length)) == 0)
	return NULL;

index->seek(node_position);
index->read(btree_leaf_buffer, (long)node_length);
/*
	Next we have to do the inplace binary search.
*/
return NULL;
}
