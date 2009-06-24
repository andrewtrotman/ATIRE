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
#ifdef _MSC_VER
	#include <windows.h>
	wchar_t wide[10 * 1024];	// buffer for storing wide character strings before printing them
#endif

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
		{
#ifdef _MSC_VER
		/*
			Convert into a wide string and print that as Windows printf() doesn't do UTF-8
		*/
		if (MultiByteToWideChar(CP_UTF8, 0, term, -1, wide, sizeof(wide)) == 0)
			printf("FAIL ");
		else
			wprintf(L"%s ", wide);
#else
		printf("%s ", term);
#endif
		printf("%lld %d\n", leaf.collection_frequency, leaf.document_frequency);
		}
	}

return 0;
}
