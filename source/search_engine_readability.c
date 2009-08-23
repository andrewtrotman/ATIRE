/*
	SEARCH_ENGINE_READABILITY.C
	---------------------------
*/
#include <math.h>
#include <stdlib.h>
#include "search_engine_readability.h"
#include "memory.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"

/*
	ANT_SEARCH_ENGINE_READABILITY::ANT_SEARCH_ENGINE_READABILITY()
	--------------------------------------------------------------
*/
ANT_search_engine_readability::ANT_search_engine_readability(ANT_memory *memory) : ANT_search_engine(memory)
{
long long current_readability;
ANT_search_engine_btree_leaf collection_details;

document_readability = (long *)memory->malloc(documents * sizeof(*document_readability));
memory->realign();

/*
	read and decompress the readability vector
*/
if (get_postings_details("~dalechall", &collection_details) == NULL)
	exit(puts("This index is not annotated with readability data - terminating"));

get_postings(&collection_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, collection_details.document_frequency);

hardest_document = 0;

for (current_readability = 0; current_readability < collection_details.document_frequency; current_readability++)
	{
	document_readability[current_readability] = decompress_buffer[current_readability];
	if (document_readability[current_readability] > hardest_document)
		hardest_document = document_readability[current_readability];
	}
}
