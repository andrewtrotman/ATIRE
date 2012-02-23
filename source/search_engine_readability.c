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

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_SEARCH_ENGINE_READABILITY::OPEN()
	-------------------------------------
*/
int ANT_search_engine_readability::open(const char *filename)
{
long long current_readability;
ANT_search_engine_btree_leaf collection_details;

/* Call inherited */
int result = ANT_search_engine::open(filename);

if (result == 0) 
	{
	/* Load failed */
	return 0;
	}

document_readability = (long *)memory->malloc(documents * sizeof(*document_readability));
memory->realign();

/*
	read and decompress the readability vector
*/
if (get_postings_details("~dalechall", &collection_details) == NULL)
	exit(puts("This index is not annotated with readability data - terminating"));

postings_buffer = get_postings(&collection_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, collection_details.local_document_frequency);

hardest_document = 0;

for (current_readability = 0; current_readability < collection_details.local_document_frequency; current_readability++)
	{
	document_readability[current_readability] = decompress_buffer[current_readability];
	if (document_readability[current_readability] > hardest_document)
		hardest_document = document_readability[current_readability];
	}

return 1;
}
