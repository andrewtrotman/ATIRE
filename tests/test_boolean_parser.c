/*
	TEST_BOOLEAN_PARSER.C
	---------------------
*/
#include <stdio.h>
#include "../source/query.h"
#include "../source/query_boolean.h"

static char query[1024];

/*
	MAIN()
	------
*/
int main(void)
{
ANT_query tree;
ANT_query_boolean parser;

printf("->");
while (fgets(query, sizeof(query), stdin) != NULL)
	{
	parser.parse(&tree, query);
	printf("[");
	tree.boolean_query->text_render();
	printf("] (error:%ld)\n->", parser.get_error());
	}

return 0;
}