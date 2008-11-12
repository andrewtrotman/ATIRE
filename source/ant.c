#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	SPECIAL_COMMAND()
	-----------------
*/
long special_command(char *command)
{
if (strcmp(command, ".quit") == 0)
	return FALSE;

return TRUE;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_search_engine_btree_leaf term_details;
long long buffer_pos, buffer_length;
ANT_memory memory;
char query[1024];
long more, hits;
long exact_match;

puts("Ant");
puts("---");
puts("Copyright (c) 2008");
puts("Andrew Trotman, University of Otago");
puts("andrew@cs.otago.ac.nz");
ANT_search_engine search_engine(&memory);
printf("Index contains %d documents\n", search_engine.document_count());
puts("\nuse:\n\t.quit to quit\n");
more = TRUE;
while (more)
	{
	printf("]");
	if (fgets(query, sizeof(query), stdin) == NULL)
		more = FALSE;
	else
		{
		query[strlen(query) - 1] = '\0';
		if (*query == '.')
			more = special_command(query);
		else
			{
			search_engine.init_accumulators();
			buffer_pos = search_engine.get_btree_leaf_position(query, &buffer_length, &exact_match);
//			printf("%s : pos:%I64d Len:%I64d\n", query, buffer_pos, buffer_length);
			search_engine.get_postings_details(query, &term_details);
			search_engine.process_one_search_term(query);
			search_engine.generate_results_list(&hits);
			printf("Query '%s' found %d documents\n", query, hits);
			}
		}
	}
puts("Bye");

return 0;
}
