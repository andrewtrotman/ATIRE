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
long long end, term_header;
ANT_search_engine_btree_node *current;

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
btree_nodes = get_long_long(block);
block += sizeof(btree_nodes);
btree_root = (ANT_search_engine_btree_node *)memory->malloc((long)(sizeof(ANT_search_engine_btree_node) * (btree_nodes + 1)));	// +1 to null terminate (with the end of last block position)

/*
	Then we have a sequence of '\0' terminated string / offset pairs
*/
for (current = btree_root; current < btree_root + btree_nodes; current++)
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
