/*
	TEST_BOOLEAN_PARSER.C
	---------------------
*/
#include <stdio.h>
#include "../source/query_boolean.h"

static char query[1024];

/*
	MAIN()
	------
*/
int main(void)
{
ANT_query_parse_tree *tree;
ANT_query_boolean parser;

printf("->");
while (fgets(query, sizeof(query), stdin) != NULL)
	{
	tree = parser.parse(query);
	printf("[");
	tree->text_render();
	printf("] (error:%ld)\n->", parser.get_error());
	}

return 0;
}