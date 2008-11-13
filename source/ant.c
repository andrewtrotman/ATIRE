#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "ctypes.h"
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
ANT_memory memory;
char query[1024], token[1024];
long more, hits;
char *token_start, *token_end;

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
			token_end = query;
			while (*token_end != '\0')
				{
				token_start = token_end;
				while (!ANT_isalpha(*token_start) && *token_start != '\0')
					token_start++;
				if (*token_start == '\0')
					break;
				token_end = token_start;
				while (ANT_isalpha(*token_end))
					token_end++;
				strncpy(token, token_start, token_end - token_start);
				token[token_end - token_start] = '\0';
				
				search_engine.process_one_search_term(token);
				}

			search_engine.generate_results_list(&hits);
			printf("Query '%s' found %d documents\n", query, hits);
			}
		}
	}
puts("Bye");

return 0;
}
