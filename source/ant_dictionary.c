/*
	ANT_DICTIONARY.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "search_engine.h"
#include "btree_iterator.h"
#include "search_engine_btree_leaf.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *term, *first_term, *last_term;
ANT_memory memory;
ANT_search_engine search_engine(&memory);
ANT_btree_iterator iterator(&search_engine);
ANT_search_engine_btree_leaf leaf;

if (argc == 1)
	first_term = last_term = NULL;
else if (argc == 2)
	{
	first_term = argv[1];
	last_term = NULL;
	}
else if (argc == 3)
	{
	first_term = argv[1];
	last_term = argv[2];
	}
else
	exit(printf("Usage:%s [<start word> [<end word>]]\n", argv[0]));

for (term = iterator.first(first_term); term != NULL; term = iterator.next())
	{
	iterator.get_postings_details(&leaf);
	if (last_term != NULL && strcmp(last_term, term) < 0)
		break;
	else
		printf("%s %lld %d\n", term, leaf.collection_frequency, leaf.document_frequency);
	}

return 0;
}
