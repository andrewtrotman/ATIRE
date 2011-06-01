/*
	QUERY_PARSE_TREE.C
	------------------
*/

#include "query_parse_tree.h"

/*
	ANT_QUERY_PARSE_TREE::TEXT_RENDER()
	-----------------------------------
*/
void ANT_query_parse_tree::text_render(void)
{
internal_text_render();
//printf("\n");
}

/*
	ANT_QUERY_PARSE_TREE::INTERNAL_TEXT_RENDER()
	--------------------------------------------
*/
void ANT_query_parse_tree::internal_text_render(void)
{
if (this == NULL)
	{
	printf("!");
	return;
	}
printf("(");
switch (boolean_operator)
	{
	case LEAF_NODE:
		printf("%*.*s", (int)term.string_length, (int)term.string_length, term.start);
		break;
	case BOOLEAN_AND:
		left->internal_text_render();
		printf(" AND ");
		right->internal_text_render();
		break;
	case BOOLEAN_OR:
		left->internal_text_render();
		printf(" OR ");
		right->internal_text_render();
		break;
	case BOOLEAN_XOR:
		left->internal_text_render();
		printf(" XOR ");
		right->internal_text_render();
		break;
	case BOOLEAN_NOT:
		left->internal_text_render();
		printf(" NOT ");
		right->internal_text_render();
		break;
	}
printf(")");
}
