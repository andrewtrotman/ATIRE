#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "search_engine.h"

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
long long buffer_pos, buffer_length;
ANT_memory memory;
char query[1024];
long more;

puts("Ant");
puts("---");
puts("Copyright (c) 2008");
puts("Andrew Trotman, University of Otago");
puts("andrew@cs.otago.ac.nz");
puts("\nuse:\n\t.quit to quit\n");
ANT_search_engine search_engine(&memory);
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
			buffer_pos = search_engine.get_btree_leaf_position(query, &buffer_length);
			printf("%s : pos:%I64d Len:%I64d\n", query, buffer_pos, buffer_length);
			}
		}
	}
	puts("Bye");

return 0;
}
